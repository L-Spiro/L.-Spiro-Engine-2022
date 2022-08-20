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
 * Description: The heart of the whole engine.  A game class.  This wraps together all the major features
 *	in the engine and runs the main game loop.  It uses a state machine to manage states in a game.
 * Inherit from this class to build your own game around this engine.
 * The game is paused automatically when any state but the main state has focus.  This does not stop the
 *	global timer from counting, but local timers will stop unless the state is flagged as non-interruptable.
 *	All virtual timers always stop when the game is flagged as being paused.
 */

#include "LSEGame.h"
//#include "PostProcessing/LSGPostProcessor.h"


namespace lse {

	// == Various constructors.
	LSE_CALL CGame::CGame() :
		m_psfStateFactory( NULL ),
		m_pfPresent( NULL ),
		m_lpvPresentUser( NULL ),
		m_i32Width( 0 ),
		m_i32Height( 0 ),
		m_bActive( false ),
		m_bFullScreen( false ),
		m_bCaptureMouse( true ),
		m_piqInputQueue( NULL ),
		m_bTicked( false ),
		m_bPaused( false ),
		m_bClosing( false ),
		m_bCostantTimeTicked( false )
		/*m_ptmSrcTouchMan( NULL ),
		m_pswWorld( NULL )*/
		{

		m_sNextState.bSetNextState = false;
		m_sNextState.ui32Id = static_cast<uint32_t>(LSE_GS_INVALID);
		m_sNextState.uiptrUser = 0;

		SetStateFactory( LSENEW CStateFactory() );

		//m_tRenderTime = m_tTime;
		m_rRand.Seed( static_cast<uint32_t>(m_tTime.GetRealTime()) );

#ifdef LSE_IPHONE
		m_aSoundManAllocator.SetAllocator( CMemLib::MGetAllocator( 4 * 1024 * 1024, true ) );
#else
		m_aSoundManAllocator.SetAllocator( CMemLib::MGetAllocator( 32 * 1024 * 1024, true ) );
#endif	// #ifdef LSE_IPHONE
		m_smeSoundMan = LSENEW CSoundManagerEx();
		m_smeSoundMan->SetAllocator( &m_aSoundManAllocator );
		m_smeSoundMan->SetDevice( NULL );
	}

	LSE_CALL CGame::~CGame() {
		SetStateFactory( NULL );

		LSEDELETE m_smeSoundMan;
		CMemLib::DestroyAllocator( m_aSoundManAllocator.GetAllocator() );
		m_aSoundManAllocator.SetAllocator( NULL );
	}

	// == Functions.
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// SETS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Sets the function we use to present the back buffer.  The game class does not present the back buffer under
	 *	normal conditions, but the users may decide to force-present it under certain circumstances.
	 *
	 * \param _pfFunc The function pointer used to present the back buffer.
	 */
	void LSE_CALL CGame::SetPresentFunc( CGame::PfPresentFunc _pfFunc, void * _pvUser ) {
		m_pfPresent = _pfFunc;
		m_lpvPresentUser = _pvUser;
	}

	/**
	 * Sets the next state.  State is set on the next frame, after the time and input have been updated.
	 *
	 * \param _ui32Id ID of the state.  Can be one of the LSE_GS_* enumerated values or a user-defined per-game value.  To
	 *	generate per-game states (based off this ID), a custom state factory must be provided to this class.
	 * \param _uptrUser User-defined data to be sent to the state in its Init() function.  This has no meaning to the engine.
	 * \param _bForce If the given state is already active, this value determines if it will be forcefully reloaded or not.
	 */
	void LSE_CALL CGame::SetNextState( uint32_t _ui32Id, uintptr_t _uptrUser, LSBOOL _bForce ) {
		//CCriticalSection::CLocker lLockMe( m_csCrit );
		m_sNextState.ui32Id = _ui32Id;
		m_sNextState.uiptrUser = _uptrUser;
		m_sNextState.bForce = _bForce;
		m_sNextState.bSetNextState = true;
	}

	/**
	 * Sets the state factory.
	 */
	void LSE_CALL CGame::SetStateFactory( CStateFactory * _psfFactory ) {
		//CCriticalSection::CLocker lLockMe( m_csCrit );
		if ( m_psfStateFactory ) {
			LSEDELETE m_psfStateFactory;
		}
		m_psfStateFactory = _psfFactory;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// INPUT
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Sets the input-queue pointer.
	 *
	 * \param _piqInputQueue The input-queue pointer.
	 */
	void LSE_CALL CGame::SetInputQueue( CInputQueue * _piqInputQueue ) {
		m_piqInputQueue = _piqInputQueue;
	}

	/**
	 * Sets the initial mouse positions.  Should only be done by the system.
	 *
	 * \param _ui32X X position of the mouse in screen coordinates.
	 * \param _ui32Y Y position of the mouse in screen coordinates.
	 */
	void LSE_CALL CGame::SetInitialMousePosition( int32_t _ui32X, int32_t _ui32Y ) {
		//CCriticalSection::CLocker lLockMe( m_csCrit );
		m_mMouse.SetStartPos( static_cast<LSREAL>(_ui32X), static_cast<LSREAL>(_ui32Y) );
	}

	/**
	 * Gets the high-definition mouse for read and write.
	 *
	 * \return The high-definition mouse used for controlling the game environment
	 */
	CMouse & LSE_CALL CGame::Mouse() {
		return m_mMouse;
	}

	/**
	 * Gets the high-definition mouse for read only.
	 *
	 * \return The high-definition mouse used for controlling the game environment.
	 */
	const CMouse & LSE_CALL CGame::Mouse() const {
		return m_mMouse;
	}
	
	/**
	 * Gets our keyboard.
	 *
	 * \return Returns the keyboard.
	 */
	const CKeyboard & LSE_CALL CGame::Keyboard() const {
		return m_kKeyboard;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// OTHER
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Tick a single frame.  Not to be called by users.
	 */
	LSBOOL LSE_CALL CGame::Tick() {
		// Figure out how many logical ticks need to be made and make them.
		m_tRenderTime.Update( !IsPaused() );

		uint32_t ui32RenderFrac;
		uint64_t ui64Ticks = m_tTime.MicrosToTicks( LSE_CONSTANTTIMESTEP );
		uint32_t ui32Steps = CTime::GetConstantStepUpdateTimesFromTicks( m_tTime.GetCurrentRealTime(), m_tRenderTime.GetCurrentRealTime(),
			ui64Ticks, ui32RenderFrac );
		
		for ( uint32_t I = ui32Steps;
			I--; ) {
			if ( !TickLogic() ) { return false; }
		}
		// Determine the fraction between frames where this logical update lands.  This is used for rendering.
		//LSREAL fRenderFraction = ui32RenderFrac * static_cast<LSREAL>(1.0 / 1000.0);


		if ( !IsClosing() ) {
			// Render the scene.
			if ( !CGfx::BeginRender() ) { return false; }
			CGfx::Clear( LSG_B_COLOR_BUFFER | LSG_B_STENCIL_BUFFER | LSG_B_DEPTH_BUFFER );
			if ( m_bCostantTimeTicked ) {
				if ( !m_smStateMachine.Draw( this ) ) {
					CGfx::EndRender();
					return false;
				}
			}

			CGfx::EndRender();
		}

		// Update the sound manager.
		SoundMan().Tick();

		return true;
	}

	/**
	 * Update the time.  Called at the start of every frame.  Should never be called by states except in rare circumstances.
	 */
	void LSE_CALL CGame::UpdateTime() {
		m_tTime.Update( !IsPaused() );
		m_tRenderTime.Update( !IsPaused() );
	}

	/**
	 * Sets the width and height of the screen.
	 *
	 * \param _i32Width Width of the screen in pixels.
	 * \param _i32Height Height of the screen in pixels.
	 */
	void LSE_CALL CGame::SetScreenDimensions( int32_t _i32Width, int32_t _i32Height ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		m_i32Width = _i32Width;
		m_i32Height = _i32Height;
		CGfx::SetScreenSize( m_i32Width, m_i32Height );
		CGfx::SetViewport( LSM_ZERO, LSM_ZERO, static_cast<LSREAL>(m_i32Width), static_cast<LSREAL>(m_i32Height) );
		Mouse().Reset();
		/*for ( uint32_t I = m_pcCamList.Length(); I--; ) {
			m_pcCamList[I]->AdjustToResolution( m_i32Width, m_i32Height );
		}
		GetMenuMan().SetScreenDimensions( _i32Width, _i32Height );

		GetGameWorld().SetScreenDimensions( _i32Width, _i32Height );*/
	}

	/**
	 * Sets the active flag.  This should be done only by the system.
	 *
	 * \param _bVal Active value.
	 */
	void LSE_CALL CGame::SetActive( LSBOOL _bVal ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		// If there is any change in the active state, reset mouses.
		if ( m_bActive != static_cast<LSBOOL>(_bVal != false) ) {
			m_mMouse.ResetClicks();
			
			/*m_mmMenuMan.ReleaseHold();

			if ( m_ptmSrcTouchMan ) {
				m_ptmSrcTouchMan->Reset();
			}
			m_tmTouchMan.Reset();*/
		}

		m_bActive = _bVal != false;
#ifdef LSG_OGLES
		CGfx::SetActive( m_bActive );
#endif	// #ifdef LSG_OGLES
	}

	/**
	 * Gets the active flag.
	 */
	LSBOOL LSE_CALL CGame::Active() const {
		return m_bActive;
	}

	/**
	 * Sets the fullscreen flag.  This should be done only by the system.
	 *
	 * \param _bVal Full-screen value.
	 */
	void LSE_CALL CGame::SetFullScreen( LSBOOL _bVal ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		m_bFullScreen = _bVal != false;
	}

	/**
	 * Present the back buffer to the screen.
	 */
	void LSE_CALL CGame::Present() {
		if ( m_pfPresent ) {
			m_pfPresent( m_lpvPresentUser );
		}
	}

	/**
	 * Stop the game.
	 */
	void LSE_CALL CGame::Close() {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		m_bClosing = true;
	}

	/**
	 * Are we closing?
	 */
	LSBOOL LSE_CALL CGame::IsClosing() const {
		return m_bClosing;
	}

	/**
	 * Tick the game logic.
	 *
	 * \return Returns false if the game needs to close.
	 */
	LSBOOL LSE_CALL CGame::TickLogic() {
		uint64_t ui64Ticks = m_tTime.MicrosToTicks( LSE_CONSTANTTIMESTEP );

		// Begin the sound manager on the first tick.
		if ( !m_bTicked ) {
			m_bTicked = true;
			m_smeSoundMan->Run();
		}
		/*if ( m_pkbKeyboardBuffer ) {
			m_pkbKeyboardBuffer->UpdateKeyboard( &m_kKeyboard );
		}*/


		// Issue sound-related events.
		m_smeSoundMan->SynchronousUpdate( reinterpret_cast<uintptr_t>(this), SoundEventHandler );


		// If there should be a new state, set it.
		if ( m_sNextState.bSetNextState ) {
			CCriticalSection::CLocker lLockMe( m_csCrit );
#ifdef _DEBUG
			CStd::DebugPrintA( "Changing state.\r\n" );
#endif	// #ifdef _DEBUG
			m_sNextState.bSetNextState = false;
			if ( !SetState( m_sNextState.ui32Id, m_sNextState.uiptrUser, m_sNextState.bForce ) ) { return false; }
		}


		if ( !IsClosing() ) {
			// Tick the current state.
			// When not in constant-time mode, we advance the timer after the frame, but when stepping by
			//	a constant time we have to advance before, because the advancement tells us how many ticks to make.
			m_tTime.UpdateFullyBy( ui64Ticks, !IsPaused() );

			// After updating the time, grab input data from the input queue.
			uint32_t ui32Inputs = GrabInput( m_tTime.GetCurMicros() );

			if ( !m_smStateMachine.Tick( this ) ) {
				m_piqInputQueue->RemoveInputs( ui32Inputs );
				return false;
			}
			if ( m_piqInputQueue ) {
				m_piqInputQueue->RemoveInputs( ui32Inputs );
			}
			m_bCostantTimeTicked = true;
		}

		return true;
	}

	/**
	 * Sets the next state.
	 *
	 * \param _ui32Id ID of the state to set.
	 * \param _uptrUser User-defined data to pass to the new state.
	 * \param _bForce If the new state and the current state are the same, setting this to false causes the state change to
	 *	be abandoned, and the current state remains active.
	 * \return Returns false if a memory error occurred while trying to create the new state.
	 */
	LSBOOL LSE_CALL CGame::SetState( uint32_t _ui32Id, uintptr_t _uptrUser, LSBOOL _bForce ) {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		// If we are not forcing the change, only change if the new state is not the same as the
		//	current state.
		uint32_t ui32CurState = m_smStateMachine.CurStateId();
		if ( !_bForce && ui32CurState == _ui32Id ) { return true; }

		assert( m_psfStateFactory );

		uint32_t ui32AllocCutOff = CMemLib::GetAllocationCounter();
		CState * psNewState = m_psfStateFactory->GetState( _ui32Id );
		return m_smStateMachine.SetState( psNewState, this, _ui32Id, ui32CurState, _uptrUser,
			ui32AllocCutOff );
	}

	/**
	 * Handles the transfer of input from the main thread's input queue, mapping keys/buttons and
	 *	dispatching them to the keyboard, mouse, touch, joystick(s), etc.
	 *
	 * \param _ui64Time The time until which to eat inputs from the input queue.
	 * \return Returns the number of inputs eaten.
	 */
	uint32_t LSE_CALL CGame::GrabInput( uint64_t _ui64Time ) {
		// Regardless of input, swap keyboard data.
		CStd::MemCpy( &m_kKeyboard.m_kiLastKeys, &m_kKeyboard.m_kiKeys, sizeof( m_kKeyboard.m_kiKeys ) );
		m_mMouse.ApplyChanges( _ui64Time );
		m_tTouch.Tick();
		if ( !m_piqInputQueue ) { return 0; }

		uint32_t ui32Total = m_piqInputQueue->EatInputs( _ui64Time, m_piqibInputBuffer );

		/*if ( ui32Total ) {
			CStd::DebugPrintA( "CGame::GrabInput(): Total: " );
			CStd::DebugPrintA( CString::CreateFromUI64( ui32Total ).CStr() );
			CStd::DebugPrintA( "\r\n" );
		}*/
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			switch ( m_piqibInputBuffer[I]->itType ) {
				case LSE_IT_KEYBOARDBTNDWN : {
					const LSE_INPUTQUEUEKEYDOWN * piqkdEvent = static_cast<const LSE_INPUTQUEUEKEYDOWN *>(m_piqibInputBuffer[I]);
					if ( static_cast<uint32_t>(piqkdEvent->kcKey) < LSE_ELEMENTS( m_kKeyboard.m_kiKeys ) ) {
						m_kKeyboard.m_kiKeys[piqkdEvent->kcKey].bIsDown = true;
						m_kKeyboard.m_kiKeys[piqkdEvent->kcKey].ui64TimePressed = piqkdEvent->ui64Time;
						m_kKeyboard.m_kiKeys[piqkdEvent->kcKey].ui64TimeReleased = LSE_MAXU64;
					}
					break;
				}
				case LSE_IT_KEYBOARDBTNUP : {
					const LSE_INPUTQUEUEKEYUP * piqkuEvent = static_cast<const LSE_INPUTQUEUEKEYUP *>(m_piqibInputBuffer[I]);
					if ( static_cast<uint32_t>(piqkuEvent->kcKey) < LSE_ELEMENTS( m_kKeyboard.m_kiKeys ) ) {
						m_kKeyboard.m_kiKeys[piqkuEvent->kcKey].bIsDown = false;
						m_kKeyboard.m_kiKeys[piqkuEvent->kcKey].ui64TimeReleased = piqkuEvent->ui64Time;
					}
					break;
				}
				case LSE_IT_MOUSEBTNDWN : {
					const LSE_INPUTQUEUEMOUSEDOWN * piqmdEvent = static_cast<const LSE_INPUTQUEUEMOUSEDOWN *>(m_piqibInputBuffer[I]);
					if ( static_cast<uint32_t>(piqmdEvent->mbcButton) < LSE_MOUSE_BUTTONS ) {
						m_mMouse.Click( piqmdEvent->mbcButton, piqmdEvent->ui64Time );
					}
					break;
				}
				case LSE_IT_MOUSEBTNUP : {
					const LSE_INPUTQUEUEMOUSEUP * piqmuEvent = static_cast<const LSE_INPUTQUEUEMOUSEUP *>(m_piqibInputBuffer[I]);
					if ( static_cast<uint32_t>(piqmuEvent->mbcButton) < LSE_MOUSE_BUTTONS ) {
						m_mMouse.UnClick( piqmuEvent->mbcButton, piqmuEvent->ui64Time );
					}
					break;
				}
				case LSE_IT_MOUSEMOVE : {
					const LSE_INPUTQUEUEMOUSEMOVE * piqmmEvent = static_cast<const LSE_INPUTQUEUEMOUSEMOVE *>(m_piqibInputBuffer[I]);
					if ( piqmmEvent->bAbsolute ) {
						m_mMouse.SetPos( piqmmEvent->fX, piqmmEvent->fY, piqmmEvent->ui64Time );
					}
					else {
						m_mMouse.AddPos( piqmmEvent->fX, piqmmEvent->fY, piqmmEvent->ui64Time );
					}
					break;
				}
				/*case LSE_IT_MOUSEWHEEL : {
					const LSE_INPUTQUEUEMOUSEWHEEL * piqmwEvent = static_cast<const LSE_INPUTQUEUEMOUSEWHEEL *>(m_piqibInputBuffer[I]);
					break;
				}*/
				case LSE_IT_TOUCHBEGIN : {
					const LSE_INPUTQUEUETOUCHBEGIN * piqtbEvent = static_cast<const LSE_INPUTQUEUETOUCHBEGIN *>(m_piqibInputBuffer[I]);
					m_tTouch.TouchDown( CVector2( piqtbEvent->fX, piqtbEvent->fY ), piqtbEvent->ui64Time );
					break;
				}
				case LSE_IT_TOUCHMOVE : {
					const LSE_INPUTQUEUETOUCHMOVE * piqtmEvent = static_cast<const LSE_INPUTQUEUETOUCHMOVE *>(m_piqibInputBuffer[I]);
					m_tTouch.TouchMove( CVector2( piqtmEvent->fX, piqtmEvent->fY ), CVector2( piqtmEvent->fEndX, piqtmEvent->fEndY ),
						piqtmEvent->ui64Time );
					break;
				}
				case LSE_IT_TOUCHEND : {
					const LSE_INPUTQUEUETOUCHEND * piqteEvent = static_cast<const LSE_INPUTQUEUETOUCHEND *>(m_piqibInputBuffer[I]);
					m_tTouch.TouchEnd( CVector2( piqteEvent->fX, piqteEvent->fY ), CVector2( piqteEvent->fEndX, piqteEvent->fEndY ),
						piqteEvent->ui64Time );
					break;
				}
				case LSE_IT_TOUCHCANCEL : {
					const LSE_INPUTQUEUETOUCHCANCELLED * piqteEvent = static_cast<const LSE_INPUTQUEUETOUCHCANCELLED *>(m_piqibInputBuffer[I]);
					m_tTouch.TouchEnd( CVector2( piqteEvent->fX, piqteEvent->fY ), CVector2( piqteEvent->fEndX, piqteEvent->fEndY ),
						piqteEvent->ui64Time );
					break;
				}
				default : {
					CStd::DebugPrintA( "CGame::GrabInput(): Unknown input type.\r\n" );
				}
			}
		}

		return ui32Total;
	}

	/**
	 * Callback to handle sound events synchronously.
	 *
	 * \param _uiptrUserPtr A pointer to this class.
	 * \param _pssSource The sound source that raised the event.
	 * \param _ui32EventType The type of event raised.
	 * \param _psmManager A pointer to the sound manager that owns the sound.
	 */
	void LSE_CALL CGame::SoundEventHandler( uintptr_t _uiptrUserPtr, CSoundSource * _pssSource,
		uint32_t _ui32EventType, CSoundManager * _psmManager ) {
		CGame * pgThis = reinterpret_cast<CGame *>(_uiptrUserPtr);

		pgThis->m_smStateMachine.IssueSoundEvent( pgThis, _pssSource, _ui32EventType, reinterpret_cast<CSoundManagerEx *>(_psmManager) );
	}

}	// namespace lse
