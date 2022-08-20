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


#ifndef __LSE_STATEMACHINE_H__
#define __LSE_STATEMACHINE_H__

#include "../LSEEngine.h"
//#include "../Mesh/LSECore3dModel.h"
//#include "../Sound/LSECoreSoundManager.h"
#include "LSEState.h"
#include "Vector/LSTLVectorPoD.h"

namespace lse {
	// We need to know about the game class because states need to know about the game class.
	// But it is just for passing down, so the circular dependency is rather minor.
	class CGame;

	/**
	 * Class CStateMachine
	 *
	 * Description: Manage the current state(s).
	 */
	class CStateMachine {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALL					CStateMachine();


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
		LSBOOL LSE_CALL						SetState( CState * _psState, CGame * _pgGame, int32_t _ui32State, int32_t _ui32PrevState, uintptr_t _uptrUser,
			uint32_t _ui32AllocCutOff );

		/**
		 * Gets the current main state or NULL for none.
		 *
		 * \return Returns the current main state or NULL for none.
		 */
		LSE_INLINE CState * LSE_CALL		CurState();
		
		/**
		 * Gets the current main state or NULL for none.
		 *
		 * \return Returns the current main state or NULL for none.
		 */
		LSE_INLINE const CState * LSE_CALL	CurState() const;

		/**
		 * Gets the current top-most state or NULL for none.
		 *
		 * \return Returns the current top-most state or NULL for none.
		 */
		LSE_INLINE CState * LSE_CALL		GetCurTopState();

		/**
		 * Gets the current state ID.
		 *
		 * \return Returns the current state ID.
		 */
		LSE_INLINE uint32_t LSE_CALL		CurStateId() const;

		/**
		  * Gets the top state ID.
		 *
		 * \return Returns the top state ID.
		 */
		LSE_INLINE uint32_t LSE_CALL		GetTopStateId() const;

		/**
		 * Gets the total states.
		 *
		 * \return Returns the total states.
		 */
		LSE_INLINE uint32_t LSE_CALL		GetTotalStates() const;

		/**
		 * Tick the current top-most state and any other states tagged as non-interruptable.
		 * If any states return false, the function returns false and the game closes.
		 *
		 * \param _pgGame The game object to pass to the current states.
		 * \return Returns true if the game is to continue running.
		 */
		LSBOOL LSE_CALL						Tick( CGame * _pgGame );

		/**
		 * Draws the current top-most state and advances the local time on the top-most state and
		 *	all states tagged as non-interruptable.
		 * If any states return false, the function returns false and the game closes.
		 *
		 * \param _pgGame The game object to pass to the current states.
		 * \return Returns true if the game is to continue running.
		 */
		LSBOOL LSE_CALL						Draw( CGame * _pgGame );

		/**
		 * Send a sound-related event to all states.
		 *
		 * \param _pgGame The game class.
		 * \param _pssSource The sound source that caused the event.
		 * \param _ui32EventType The type of event.
		 * \param _psmManager The sound manager.
		 */
		void LSE_CALL						IssueSoundEvent( CGame * _pgGame,
			CSoundSource * _pssSource,
			uint32_t _ui32EventType, CSoundManagerEx * _psmManager );


	protected :
		// == Members.
		/**
		 * The allocation number at the time of switching states.  Used for debug output related to tracking
		 *	memory allocations.
		 */
		uint32_t							m_ui32AllocNumber;

		/**
		 * Stack of states.
		 */
		CVectorPoD<CState *, uint16_t, 2>	m_vCurStates;


		/**
		 * Pop all states off the stack.
		 *
		 * \param _pgGame Game object to pass to the states before destroying them.
		 * \param _ui32NextState Next state ID to pass to the states before destroying them.
		 * \return Returns true if all memory was freed properly since the last state change.
		 */
		LSBOOL LSE_CALL						PopAllStates( CGame * _pgGame, int32_t _ui32NextState );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALL CStateMachine::CStateMachine() :
		m_ui32AllocNumber( 0 ) {
	}

	// == Functions.
	/**
	 * Gets the current main state or NULL for none.
	 *
	 * \return Returns the current main state or NULL for none.
	 */
	LSE_INLINE CState * CStateMachine::CurState() {
		if ( !m_vCurStates.Length() ) { return NULL; }
		return m_vCurStates[0];
	}
	
	/**
	 * Gets the current main state or NULL for none.
	 *
	 * \return Returns the current main state or NULL for none.
	 */
	LSE_INLINE const CState * LSE_CALL CStateMachine::CurState() const {
		if ( !m_vCurStates.Length() ) { return NULL; }
		return m_vCurStates[0];
	}

	/**
	 * Gets the current top-most state or NULL for none.
	 *
	 * \return Returns the current top-most state or NULL for none.
	 */
	LSE_INLINE CState * CStateMachine::GetCurTopState() {
		if ( !m_vCurStates.Length() ) { return NULL; }
		return m_vCurStates[m_vCurStates.Length()-1];
	}

	/**
	 * Gets the current state ID.
	 *
	 * \return Returns the current state ID.
	 */
	LSE_INLINE uint32_t LSE_CALL CStateMachine::CurStateId() const {
		if ( !m_vCurStates.Length() ) { return static_cast<uint32_t>(LSE_GS_INVALID); }
		return m_vCurStates[0]->m_ui32StateId;
	}

	/**
	  * Gets the top state ID.
	 *
	 * \return Returns the top state ID.
	 */
	LSE_INLINE uint32_t LSE_CALL CStateMachine::GetTopStateId() const {
		if ( !m_vCurStates.Length() ) { return static_cast<uint32_t>(LSE_GS_INVALID); }
		return m_vCurStates[m_vCurStates.Length()-1]->m_ui32StateId;
	}

	/**
	 * Gets the total states.
	 *
	 * \return Returns the total states.
	 */
	LSE_INLINE uint32_t LSE_CALL CStateMachine::GetTotalStates() const {
		return m_vCurStates.Length();
	}

}	// namespace lse

#endif	// __LSE_STATEMACHINE_H__
