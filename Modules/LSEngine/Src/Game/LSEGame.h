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


#ifndef __LSE_GAME_H__
#define __LSE_GAME_H__

#include "../LSEEngine.h"
#include "../Input/LSEInputQueue.h"
#include "../Input/LSEKeyboard.h"
#include "../Input/LSEMouse.h"
#include "../Input/LSETouch.h"
#include "../States/LSEStateFactory.h"
#include "../States/LSEStateMachine.h"
#include "Gfx/LSGGfx.h"
#include "Rand/LSMRand.h"
#include "SoundCore/LSSSoundManagerEx.h"
#include "Time/LSSTDTime.h"

namespace lse {

	/**
	 * Class CGame
	 * \brief A generic game class as the basis for any game.
	 *
	 * Description: The heart of the whole engine.  A game class.  This wraps together all the major features
	 *	in the engine and runs the main game loop.  It uses a state machine to manage states in a game.
	 * Inherit from this class to build your own game around this engine.
	 * The game is paused automatically when any state but the main state has focus.  This does not stop the
	 *	global timer from counting, but local timers will stop unless the state is flagged as non-interruptable.
	 *	All virtual timers always stop when the game is flagged as being paused.
	 */
	class CGame {
		friend class								CStateMachine;
	public :
		// == Various constructors.
		LSE_CALL									CGame();
		virtual LSE_CALL							~CGame();
		
		
		// == Types.
		/**
		 * The type of function used to present the back buffer when we need.
		 * Presenting the back buffer to the screen.
		 */
		typedef void (LSE_CALL *					PfPresentFunc)( void * _pvParm );


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// GETS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Gets the global time value.  For a state's local time, use the state's m_tLocalTime
		 *	member.
		 *
		 * \return Returns a constant reference to the logical timer.
		 */
		const CTime & LSE_CALL						Time() const;

		/**
		 * Gets the global rendering time value.
		 *
		 * \return Returns a constant reference to the rendering timer.
		 */
		const CTime & LSE_CALL						RenderTime() const;

		/**
		 * Are we paused?
		 */
		virtual LSBOOL LSE_CALL						IsPaused() const;

		/**
		 * Gets the width of the screen.
		 *
		 * \return Returns the width of the screen as an integer.
		 */
		int32_t LSE_CALL							GetWidth() const;

		/**
		 * Gets the height of the screen.
		 *
		 * \return Returns the height of the screen as an integer.
		 */
		int32_t LSE_CALL							GetHeight() const;

		/**
		 * Gets the width of the screen.
		 *
		 * \return Returns the width of the screen as a real.
		 */
		LSREAL LSE_CALL								GetWidthF() const;

		/**
		 * Gets the height of the screen.
		 *
		 * \return Returns the height of the screen as a real.
		 */
		LSREAL LSE_CALL								GetHeightF() const;

		/**
		 * Gets the half-width of the screen.
		 *
		 * \return Returns the half-width of the screen as a real.
		 */
		LSREAL LSE_CALL								GetHalfWidthF() const;

		/**
		 * Gets the half-height of the screen.
		 *
		 * \return Returns the half-height of the screen as a real.
		 */
		LSREAL LSE_CALL								GetHalfHeightF() const;

		/**
		 * Are we in full-screen mode?
		 *
		 * \return Returns true if in full-screen mode.
		 */
		LSBOOL LSE_CALL								IsFullScreen() const;

		/**
		 * Gets the current state ID.
		 */
		int32_t LSE_CALL							CurStateId() const;
		
		/**
		 * Gets a pointer to the current state.
		 *
		 * /return Returns a pointer to the current state.
		 */
		CState * LSE_CALL							CurState();
		
		/**
		 * Gets a pointer to the current state.
		 *
		 * /return Returns a pointer to the current state.
		 */
		const CState * LSE_CALL						CurState() const;

		/**
		 * Gets the sound manager.
		 */
		CSoundManagerEx & LSE_CALL					SoundMan();



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// SETS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Sets the function we use to present the back buffer.  The game class does not present the back buffer under
		 *	normal conditions, but the users may decide to force-present it under certain circumstances.
		 *
		 * \param _pfFunc The function pointer used to present the back buffer.
		 */
		void LSE_CALL								SetPresentFunc( PfPresentFunc _pfFunc, void * _pvUser );

		/**
		 * Sets the next state.  State is set on the next frame, after the time and input have been updated.
		 *
		 * \param _ui32Id ID of the state.  Can be one of the LSE_GS_* enumerated values or a user-defined per-game value.  To
		 *	generate per-game states (based off this ID), a custom state factory must be provided to this class.
		 * \param _uptrUser User-defined data to be sent to the state in its Init() function.  This has no meaning to the engine.
		 * \param _bForce If the given state is already active, this value determines if it will be forcefully reloaded or not.
		 */
		virtual void LSE_CALL						SetNextState( uint32_t _ui32Id, uintptr_t _uptrUser, LSBOOL _bForce = false );

		/**
		 * Sets the state factory.
		 */
		virtual void LSE_CALL						SetStateFactory( CStateFactory * _psfFactory );



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// INPUT
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Sets the input-queue pointer.
		 *
		 * \param _piqInputQueue The input-queue pointer.
		 */
		void LSE_CALL								SetInputQueue( CInputQueue * _piqInputQueue );

		/**
		 * Sets the initial mouse positions.  Should only be done by the system.
		 *
		 * \param _ui32X X position of the mouse in screen coordinates.
		 * \param _ui32Y Y position of the mouse in screen coordinates.
		 */
		virtual void LSE_CALL						SetInitialMousePosition( int32_t _ui32X, int32_t _ui32Y );

		/**
		 * Gets the high-definition mouse for read and write.
		 *
		 * \return The high-definition mouse used for controlling the game environment
		 */
		CMouse & LSE_CALL							Mouse();

		/**
		 * Gets the high-definition mouse for read only.
		 *
		 * \return The high-definition mouse used for controlling the game environment.
		 */
		const CMouse & LSE_CALL						Mouse() const;

		/**
		 * Gets our keyboard.
		 *
		 * \return Returns the keyboard.
		 */
		const CKeyboard & LSE_CALL					Keyboard() const;

		/**
		 * Are we capturing the mouse?
		 *
		 * \return Returns true if the mouse is intended to stay inside the game window.
		 */
		LSE_INLINE LSBOOL LSE_CALL					MouseCapture() const;
		
		/**
		 * The touch object.
		 *
		 *　\return Returns a constant reference to the touch input object.
		 */
		 LSE_INLINE const CTouch LSE_CALL			Touch() const;



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// OTHER
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Tick a single frame.  Not to be called by users.
		 */
		virtual LSBOOL LSE_CALL						Tick();

		/**
		 * Update the time.  Called at the start of every frame.  Should never be called by states except in rare circumstances.
		 */
		virtual void LSE_CALL						UpdateTime();
		
		/**
		 * Sets the width and height of the screen.
		 *
		 * \param _i32Width Width of the screen in pixels.
		 * \param _i32Height Height of the screen in pixels.
		 */
		virtual void LSE_CALL						SetScreenDimensions( int32_t _i32Width, int32_t _i32Height );

		/**
		 * Sets the active flag.  This should be done only by the system.
		 *
		 * \param _bVal Active value.
		 */
		virtual void LSE_CALL						SetActive( LSBOOL _bVal );

		/**
		 * Gets the active flag.
		 */
		virtual LSBOOL LSE_CALL						Active() const;

		/**
		 * Sets the fullscreen flag.  This should be done only by the system.
		 *
		 * \param _bVal Full-screen value.
		 */
		virtual void LSE_CALL						SetFullScreen( LSBOOL _bVal );

		/**
		 * Present the back buffer to the screen.
		 */
		virtual void LSE_CALL						Present();

		/**
		 * Stop the game.
		 */
		virtual void LSE_CALL						Close();

		/**
		 * Are we closing?
		 */
		LSBOOL LSE_CALL								IsClosing() const;


	protected :
		// == Members.
		/** Our critical section for thread safety. */
		mutable CCriticalSection					m_csCrit;

		/** Global time.  This is for the logic. */
		CTime										m_tTime;

		/** Global time.  This is for the rendering.  The logic timer usually runs at a fixed interval,
		 *	and is not synced with the rendering timer.  The rendering timer should be used to determine
		 *	how much time since the last frame, which is not the same as the amount of time since the
		 *	last logical update. */
		CTime										m_tRenderTime;

		/** Present function. */
		PfPresentFunc								m_pfPresent;
		
		/** Present function user value. */
		void *									m_lpvPresentUser;

		/** Width and height of the screen. */
		int32_t										m_i32Width,
													m_i32Height;

		/** Are we active?  Use fewer CPU resources if we are not. */
		uint8_t										m_bActive				: 1;
		
		/** Are we in full-screen mode? */
		uint8_t										m_bFullScreen			: 1;

		/** Should input be sent to the hi-definition mouse or the
		 *	low-definition mouse?  Hi-definition is used for 3D areas
		 *	typically while the low-definition mouse is used for
		 *	menu screens. */
		uint8_t										m_bCaptureMouse			: 1;
		
		/** Have we ticked yet? */
		uint8_t										m_bTicked				: 1;

		/** Are we paused? */
		uint8_t										m_bPaused				: 1;

		/** Are we closing? */
		uint8_t										m_bClosing				: 1;

		/** If stepping by a constant amount of time, we need to keep track of the first
		 *	cycle in which we ticked so that we do not call Draw() before that. */
		uint8_t										m_bCostantTimeTicked	: 1;

		/** Input buffer. */
		const LSE_INPUTQUEUEITEMBASE *				m_piqibInputBuffer[LSE_IQ_MAX_QUEUE];

		/** Pointer to the input queue. */
		CInputQueue *								m_piqInputQueue;

		/** The keyboard. */
		CKeyboard									m_kKeyboard;

		/** The mouse. */
		CMouse										m_mMouse;
		
		/** The touch input. */
		CTouch										m_tTouch;

		/** Random numbers. */
		CRand										m_rRand;

		/** State machine. */
		CStateMachine								m_smStateMachine;

		/** State factory. */
		CStateFactory *								m_psfStateFactory;

		/** Data for the next state. */
		struct {
			/** If true, the state will be changed on the next tick. */
			LSBOOL									bSetNextState;

			/** The new state ID. */
			uint32_t								ui32Id;

			/** Data to pass to the state. */
			uintptr_t								uiptrUser;

			/** If true, the state will be changed even if the current state ID matches the new state ID. */
			LSBOOL									bForce;
		}											m_sNextState;

		/** The sound manager. */
		CSoundManagerEx *							m_smeSoundMan;

		/** Sound manager allocator. */
		CAllocator									m_aSoundManAllocator;


		// == Functions.
		/**
		 * Tick the game logic.
		 *
		 * \return Returns false if the game needs to close.
		 */
		LSBOOL LSE_CALL								TickLogic();

		/**
		 * Sets the next state.
		 *
		 * \param _ui32Id ID of the state to set.
		 * \param _uptrUser User-defined data to pass to the new state.
		 * \param _bForce If the new state and the current state are the same, setting this to false causes the state change to
		 *	be abandoned, and the current state remains active.
		 * \return Returns false if a memory error occurred while trying to create the new state.
		 */
		LSBOOL LSE_CALL								SetState( uint32_t _ui32Id, uintptr_t _uptrUser, LSBOOL _bForce = false );

		/**
		 * Handles the transfer of input from the main thread's input queue, mapping keys/buttons and
		 *	dispatching them to the keyboard, mouse, touch, joystick(s), etc.
		 *
		 * \param _ui64Time The time until which to eat inputs from the input queue.
		 * \return Returns the number of inputs eaten.
		 */
		uint32_t LSE_CALL							GrabInput( uint64_t _ui64Time );

		/**
		 * Callback to handle sound events synchronously.
		 *
		 * \param _uiptrUserPtr A pointer to this class.
		 * \param _pssSource The sound source that raised the event.
		 * \param _ui32EventType The type of event raised.
		 * \param _psmManager A pointer to the sound manager that owns the sound.
		 */
		static void LSE_CALL						SoundEventHandler( uintptr_t _uiptrUserPtr, CSoundSource * _pssSource,
			uint32_t _ui32EventType, CSoundManager * _psmManager );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the global time value.  For a state's local time, use the state's m_tLocalTime
	 *	member.
	 *
	 * \return Returns a constant reference to the logical timer.
	 */
	LSE_INLINE const CTime & LSE_CALL CGame::Time() const {
		return m_tTime;
	}

	/**
	 * Gets the global rendering time value.
	 *
	 * \return Returns a constant reference to the rendering timer.
	 */
	LSE_INLINE const CTime & LSE_CALL CGame::RenderTime() const {
		return m_tRenderTime;
	}

	/**
	 * Are we paused?
	 */
	LSE_INLINE LSBOOL LSE_CALL CGame::IsPaused() const {
		return m_bPaused || m_smStateMachine.GetTotalStates() > 1;
	}

	/**
	 * Gets the width of the screen.
	 *
	 * \return Returns the width of the screen as an integer.
	 */
	LSE_INLINE int32_t LSE_CALL CGame::GetWidth() const {
		return static_cast<uint32_t>(m_i32Width);
	}

	/**
	 * Gets the height of the screen.
	 *
	 * \return Returns the height of the screen as an integer.
	 */
	LSE_INLINE int32_t LSE_CALL CGame::GetHeight() const {
		return static_cast<uint32_t>(m_i32Height);
	}

	/**
	 * Gets the width of the screen.
	 *
	 * \return Returns the width of the screen as a real.
	 */
	LSE_INLINE LSREAL LSE_CALL CGame::GetWidthF() const {
		return static_cast<LSREAL>(m_i32Width);
	}

	/**
	 * Gets the height of the screen.
	 *
	 * \return Returns the height of the screen as a real.
	 */
	LSE_INLINE LSREAL LSE_CALL CGame::GetHeightF() const {
		return static_cast<LSREAL>(m_i32Height);
	}

	/**
	 * Gets the half-width of the screen.
	 *
	 * \return Returns the half-width of the screen as a real.
	 */
	LSE_INLINE LSREAL LSE_CALL CGame::GetHalfWidthF() const {
		return static_cast<LSREAL>(m_i32Width) * LSM_HALF;
	}

	/**
	 * Gets the half-height of the screen.
	 *
	 * \return Returns the half-height of the screen as a real.
	 */
	LSE_INLINE LSREAL LSE_CALL CGame::GetHalfHeightF() const {
		return static_cast<LSREAL>(m_i32Height) * LSM_HALF;
	}

	/**
	 * Are we in full-screen mode?
	 *
	 * \return Returns true if in full-screen mode.
	 */
	LSE_INLINE LSBOOL LSE_CALL CGame::IsFullScreen() const {
		return m_bFullScreen;
	}

	/**
	 * Gets the current state ID.
	 */
	LSE_INLINE int32_t LSE_CALL CGame::CurStateId() const {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		return m_smStateMachine.CurStateId();
	}
	
	/**
	 * Gets a pointer to the current state.
	 *
	 * /return Returns a pointer to the current state.
	 */
	LSE_INLINE CState * LSE_CALL CGame::CurState() {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		return m_smStateMachine.CurState();
	}
	
	/**
	 * Gets a pointer to the current state.
	 *
	 * /return Returns a pointer to the current state.
	 */
	LSE_INLINE const CState * LSE_CALL CGame::CurState() const {
		CCriticalSection::CLocker lLockMe( m_csCrit );
		return m_smStateMachine.CurState();
	}

	/**
	 * Gets the sound manager.
	 */
	LSE_INLINE CSoundManagerEx & LSE_CALL CGame::SoundMan() {
		return (*m_smeSoundMan);
	}

	/**
	 * Are we capturing the mouse?
	 *
	 * \return Returns true if the mouse is intended to stay inside the game window.
	 */
	LSE_INLINE LSBOOL LSE_CALL CGame::MouseCapture() const {
		return m_bCaptureMouse;
	}
	
	/**
	 * The touch object.
	 *
	 *　\return Returns a constant reference to the touch input object.
	 */
	 LSE_INLINE const CTouch LSE_CALL CGame::Touch() const {
		return m_tTouch;
	 }

}	// namespace lse

#endif	// #ifndef __LSE_GAME_H__
