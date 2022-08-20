/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A state machine just manages the current state(s).  It keeps a stack of states such that the
 *	bottom state is the primary state while the top state is the active state.  Only the active state is
 *	processed normally, although states may define for themselves whether or not they should not be processed
 *	when not on top (important for online games).  99% of the time only one state is in the stack.
 * For a good example of what multiple states would be used, consider any game on Wii.  At any time during
 *	gameplay it must be possible to show the disk-error screen or home menu.  These screens are each separate
 *	states that would be stacked over the main state when called.  Being above the main state, they would be
 *	processed/drawn instead of the main state (note that they would not both be on top at the same time, and
 *	so would not both be processed).  The main state still needs to exist, but unless it is flagged as
 *	uninterruptable it will not be ticked/drawn.
 * An uninterruptable state is ticked every frame of the game as long as it is alive.  It is only drawn if it
 *	is on the top of the stack.
 */

#include "LSEStateMachine.h"
#include "../Game/LSEGame.h"
//#include "../Models/LSEDrawableModelInstance.h"
//#include "../Models/LSEModelInstance.h"
//#include "Models/LSDDrawableSharedModel.h"
//#include "Shaders/LSDModelShaderManager.h"
//#include "Sprites/LSGSprite.h"


namespace lse {

	// == Functions.
	/**
	 * Sets a new state.  Removes all states from the stack and puts the new one on bottom.
	 *
	 * \param _psState The new state to make active.
	 * \param _pgGame The game class.
	 * \param _ui32State The state ID.
	 * \param _ui32PrevState The ID of the previous state.
	 * \param _uptrUser The user data to pass to the state.
	 * \param _ui32AllocCutOff The allocation ID cut-off for printing unreleased allocations.
	 * \return Returns true if the state was set.
	 */
	LSBOOL LSE_CALL CStateMachine::SetState( CState * _psState, CGame * _pgGame, int32_t _ui32State, int32_t _ui32PrevState, uintptr_t _uptrUser,
		uint32_t _ui32AllocCutOff ) {
		if ( !PopAllStates( _pgGame, _ui32State ) ) { return false; }

#ifdef _DEBUG
		/*uint32_t ui32SpriteTextures = CCoreSprite::RemoveUnreferencedTextures();
		if ( ui32SpriteTextures ) {
			char szBuffer[32];
			CStd::DebugPrintA( CStd::UI32toA( ui32SpriteTextures, szBuffer, 10 ) );
			CStd::DebugPrintA( " shared sprite textures unreleased.\r\n" );
		}

		ui32SpriteTextures = CCoreParticle3::RemoveUnreferencedTextures();
		if ( ui32SpriteTextures ) {
			char szBuffer[32];
			CStd::DebugPrintA( CStd::UI32toA( ui32SpriteTextures, szBuffer, 10 ) );
			CStd::DebugPrintA( " shared particle textures unreleased.\r\n" );
		}*/
#else
		//CCoreSprite::RemoveUnreferencedTextures();
		//CCoreParticle3::RemoveUnreferencedTextures();
#endif	// #ifdef _DEBUG


		/*CModelInstance::m_mmModelMan.Release();
		CDrawableModelInstance::m_mmDrawableModelMan.Release();
		CModelInstance::m_amAnimMan.Release();
		CDrawableSharedModel::m_tmTexMan.Release();
		CSprite::m_tmTexMan.Release();
		CModelShaderManager::DeleteUnrefedShaders();*/

		CModelLib::SoftReset();
		CGraphicsLib::FreeTmpResources();
		CGfx::ApplyDefaultStates();


		_pgGame->SoundMan().SoftReset();

		if ( _pgGame->IsClosing() ) {
			CGraphicsLib::DestroyGraphicsLibWindow();
		}

		//_pgGame->m_saSoundManAllocatorAllocator->PrintAllocations();
		
		// Release empty heaps.
		CMemLib::ReleaseEmptyPools();


		if ( m_ui32AllocNumber != 0 ) {
			// Print allocations beyond the point of the last state change.
			LSA_SIZE sSize = CMemLib::PrintAllocations( m_ui32AllocNumber, _ui32AllocCutOff );
			if ( sSize ) {
				char szBuffer[32];
				CStd::DebugPrintA( "Leaked ~" );
				CStd::DebugPrintA( CStd::UI64toA( sSize, szBuffer, 10 ) );
				CStd::DebugPrintA( " bytes.\r\n" );
			}
		}
		// And reset our counter.
		m_ui32AllocNumber = _ui32AllocCutOff;


		if ( !_psState ) { return true; }	// NULL states end here.
		if ( !m_vCurStates.Push( _psState ) ) { return false; }
		_psState->m_ui32StateId = _ui32State;
		LSA_SIZE sHeap = _psState->GetHeapSize( _pgGame, _ui32State, _uptrUser );
		if ( sHeap ) {
			_psState->m_psaAllocator = CMemLib::MGetAllocator( sHeap, true );
			assert( _psState->m_psaAllocator );
		}
		_psState->Init( _pgGame, _ui32State, _ui32PrevState, _uptrUser );

		// It may have made a lot of temporary allocations while loading things,
		//	so once again release empty pools.
		CMemLib::ReleaseEmptyPools();
		return true;
	}

	/**
	 * Tick the current top-most state and any other states tagged as non-interruptable.
	 * If any states return false, the function returns false and the game closes.
	 *
	 * \param _pgGame The game object to pass to the current states.
	 * \return Returns true if the game is to continue running.
	 */
	LSBOOL LSE_CALL CStateMachine::Tick( CGame * _pgGame ) {
		CState * psTop = GetCurTopState();
		if ( !psTop ) { return true; }
		psTop->m_tLocalTime.UpdateBy( _pgGame->Time().MicrosToTicks( LSE_CONSTANTTIMESTEP ), static_cast<LSBOOL>(!_pgGame->IsPaused()) );
		
		LSBOOL bRet = psTop->Tick( _pgGame );
		for ( uint32_t I = m_vCurStates.Length(); I--; ) {
			if ( !m_vCurStates[I]->Interruptable() ) {
				m_vCurStates[I]->m_tLocalTime.UpdateBy( _pgGame->Time().MicrosToTicks( LSE_CONSTANTTIMESTEP ), static_cast<LSBOOL>(false) );
				if ( !m_vCurStates[I]->Tick( _pgGame ) ) {
					bRet = false;
				}
			}
		}
		return bRet;
	}

	/**
	 * Draws the current top-most state and advances the local time on the top-most state and
	 *	all states tagged as non-interruptable.
	 * If any states return false, the function returns false and the game closes.
	 *
	 * \param _pgGame The game object to pass to the current states.
	 * \return Returns true if the game is to continue running.
	 */
	LSBOOL LSE_CALL CStateMachine::Draw( CGame * _pgGame ) {
		CState * psTop = GetCurTopState();
		if ( !psTop ) { return true; }
		LSBOOL bRet = psTop->Draw( _pgGame );
		
		psTop->m_tRenderTime.UpdateBy( _pgGame->RenderTime().GetTicksSinceLastFrame(), static_cast<LSBOOL>(!_pgGame->IsPaused()) );
		for ( uint32_t I = m_vCurStates.Length(); I--; ) {
			if ( !m_vCurStates[I]->Interruptable() ) {
				if ( !m_vCurStates[I]->Draw( _pgGame ) ) {
					bRet = false;
				}
				psTop->m_tRenderTime.UpdateBy( _pgGame->RenderTime().GetTicksSinceLastFrame(), static_cast<LSBOOL>(false) );
			}
		}
		return bRet;
	}

	/**
	 * Send a sound-related event to all states.
	 *
	 * \param _pgGame The game class.
	 * \param _pssSource The sound source that caused the event.
	 * \param _ui32EventType The type of event.
	 * \param _psmManager The sound manager.
	 */
	void LSE_CALL CStateMachine::IssueSoundEvent( CGame * _pgGame,
		CSoundSource * _pssSource,
		uint32_t _ui32EventType, CSoundManagerEx * _psmManager ) {
		for ( uint32_t I = m_vCurStates.Length(); I--; ) {
			m_vCurStates[I]->EventSound( _pgGame, _pssSource, _ui32EventType, _psmManager );
		}
	}

	/**
	 * Pop all states off the stack.
	 *
	 * \param _pgGame Game object to pass to the states before destroying them.
	 * \param _ui32NextState Next state ID to pass to the states before destroying them.
	 * \return Returns true if all memory was freed properly since the last state change.
	 */
	LSBOOL LSE_CALL CStateMachine::PopAllStates( CGame * _pgGame, int32_t _ui32NextState ) {
		LSBOOL bRet = true;
		for ( uint32_t I = m_vCurStates.Length(); I--; ) {
			m_vCurStates[I]->Destroy( _pgGame, _ui32NextState );
			/*if ( m_vCurStates[I]->UnRegisterAllCustomShaders() ) {
				CModelShaderManager::UnRegisterAllCustomShaders();
			}*/
			CStdAllocator * paAllocator = m_vCurStates[I]->m_psaAllocator;

			// Have to call the destructor before releasing the allocator.
			m_vCurStates[I]->~CState();

#ifdef LSA_DEBUG
			// In debug mode we verify that all of the RAM was deallocated correctly.
			CStd::DebugPrintA( "*==\r\n" );
			if ( !CMemLib::DestroyAllocator( paAllocator ) ) {
				bRet = false;
			}
#else	// #ifdef LSA_DEBUG
			CMemLib::DestroyAllocator( paAllocator );
#endif	// #ifdef LSA_DEBUG
			uint8_t * pui8MemToFree = reinterpret_cast<uint8_t *>(m_vCurStates[I]);
			// Free as bytes so that the destructor is not called again.
			LSEDELETE pui8MemToFree;
			m_vCurStates[I] = NULL;
		}
		m_vCurStates.Reset();
		return bRet;
	}

}	// namespace lse
