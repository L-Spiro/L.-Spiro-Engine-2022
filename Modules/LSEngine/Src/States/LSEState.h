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
 * Description: The base class for all states.  A state is any modular portion of the game, such as the main
 *	menu, options menu, credits screen, gameplay area, bonus stages, stage-select screen, etc.
 * When created, Init() is called.  When destroyed, Destroy() is called.  Each frame the state is in focus,
 *	Tick() is called followed by Draw().
 * It keeps its own local time which it may use as it pleases.  The local time counts starting from the time
 *	the state was created.  Since it is local time, the state can reset it, tick it twice as fast, or whatever
 *	it pleases.
 * DO NOT LOAD RESOURCES VIA THE CONSTRUCTOR.
 *	#1: Its local memory manager will not be initialized yet.
 *	#2: Resources frequently rely on data provided by the game class, which you will not have until Init() is
 *	called.
 */


#ifndef __LSE_STATE_H__
#define __LSE_STATE_H__

#include "../LSEEngine.h"
#include "Allocators/LSAStdAllocator.h"
#include "SoundCore/LSSSoundManagerEx.h"
#include "Time/LSSTDTime.h"

namespace lse {

	// We need to know what a game class is.  This creates a small circular dependancy but
	//	is really no problem since there is no default implementation here.
	class CGame;

	/**
	 * Predefined game states.
	 */
	enum LSE_GAMESTATES {
		LSE_GS_INVALID				= -1,
		LSE_GS_DEFAULT				= -2,
	};

	/**
	 * Class CState
	 * \brief A state in the game
	 *
	 * Description: The base class for all states.  A state is any modular portion of the game, such as the main
	 *	menu, options menu, credits screen, gameplay area, bonus stages, stage-select screen, etc.
	 * When created, Init() is called.  When destroyed, Destroy() is called.  Each frame the state is in focus,
	 *	Tick() is called followed by Draw().
	 * It keeps its own local time which it may use as it pleases.  The local time counts starting from the time
	 *	the state was created.  Since it is local time, the state can reset it, tick it twice as fast, or whatever
	 *	it pleases.
	 * DO NOT LOAD RESOURCES VIA THE CONSTRUCTOR.
	 *	#1: Its local memory manager will not be initialized yet.
	 *	#2: Resources frequently rely on data provided by the game class, which you will not have until Init() is
	 *	called.
	 */
	class CState {
		// Also, the state manager needs to have access to our private parts.
		friend class				CStateMachine;

	public :
		// == Various constructors.
		LSE_CALL					CState();
		virtual LSE_CALL			~CState();


		// == Functions.
		/**
		 * Called when the state is just created.  Use this to initialize resources.
		 *
		 * \param _pgGame Can be accessed for whatever game-specific information needs to be
		 *	obtained.
		 * \param _ui32State The current state identifier (the same class could
		 *	then be used for multiple states if they are similar enough).
		 * \param _ui32PrevState The identifier of the previous state.
		 * \param _uptrUser User-defined data passed to the state.  How this is used is entirely
		 *	defined by the state.  Examples of use:
		 * #1: Which map to load in a game state.
		 * #2: Which tutorial page to show in a tutorial state.
		 * #3: Used as a pointer to data containing a lot of state-specific information such as
		 *	map file to load, previous state of the player, etc.
		 */
		virtual void LSE_CALL		Init( CGame * _pgGame, int32_t _ui32State, int32_t _ui32PrevState, uintptr_t _uptrUser );

		/**
		 * Called when it is time for the state to die.  Free resources here.
		 *
		 * \param _pgGame Pointer to the game class which allows access to game-specific resources.
		 * \param _ui32NextState The identifier of the next state.
		 */
		virtual void LSE_CALL		Destroy( CGame * _pgGame, int32_t _ui32NextState );

		/**
		 * Called once every frame the state has focus (or if it is in the background but is
		 *	non-interruptable).  Do the state-specific processing here.  This can be anything,
		 *	but usually means ticking the menu manager, distributing input, running physics,
		 *	and all other game logic.
		 *
		 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
		 * \return Return false if the game should close.
		 */
		virtual LSBOOL LSE_CALL		Tick( CGame * _pgGame );

		/**
		 * Called once every frame the state has focus to allow the state to render itself.
		 * If the state is in the background, this is not called.  Return false to exit the game.
		 *
		 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
		 * \return Return false if the game should close.
		 */
		virtual LSBOOL LSE_CALL		Draw( CGame * _pgGame );

		/**
		 * If a state is not on top of the state stack, it is not ticked normally.  If the state flags
		 *	itself as non-interruptable, however, it will be ticked every frame regardless of whether
		 *	it is on top or not.
		 *
		 * \return Return true to make this state interruptable.  The default behavior is true.  States
		 *	that need to run online or continuously in the background for any reason should return false.
		 */
		virtual LSBOOL LSE_CALL		Interruptable() const;

		/**
		 * Gets the size of the heap to be allocated for this state, or 0 for none.
		 *
		 * \param _pgGame The game pointer.
		 * \param _ui32State The ID of the current state.
		 * \param _uptrUser User data passed to the state upon initialization.  Same value as what is passed in
		 *	Init().
		 * \return Return a non-zero value to indicate the size of the heap to be allocated for this class.
		 */
		virtual LSA_SIZE LSE_CALL	GetHeapSize( CGame * _pgGame, int32_t _ui32State, uintptr_t _uptrUser ) const;
		
		/**
		 * Gets our time manager.
		 *
		 * \return Returns the time manager for this state.
		 */
		const CTime & LSE_CALL		GetTimeMan() const;

		/**
		 * Handler for sound events.
		 *
		 * \param _pgGame The game class.
		 * \param _pssSource The sound source that caused the event.
		 * \param _ui32EventType The type of event.  Can be CSoundManager::LSS_ET_SOUND_JUST_STARTED or
		 *	CSoundManager::LSS_ET_SOUND_JUST_ENDED.
		 * \param _psmManager The sound manager.
		 */
		virtual void LSE_CALL		EventSound( CGame * _pgGame,
			CSoundSource * _pssSource,
			uint32_t _ui32EventType, CSoundManagerEx * _psmManager );
			
		/**
		 * Handler for touch events.
		 * \param _vPos The 2D position of the touch in view space.
		 */
		virtual void LSE_CALL		EventTouch( const CVector2 &_vPos );

		/**
		 * Returns true if custom shaders should be automatically unregistered when this state is destroyed.
		 *	By default this returns true because it generally improves performance and it also removes some
		 *	fake memory-leak warnings.
		 *
		 * \return Returns true by default.
		 */
		virtual LSBOOL LSE_CALL		UnRegisterAllCustomShaders() const;

	protected :
		// == Members.
		/**
		 * Local time which keeps track of time since the start of this state.
		 * The state should never update the time; it will automatically be updated by the engine,
		 *	so manually updating it would cause it to tick twice per frame instead of once.
		 * However, it is perfectly acceptible for a state to reset its own time counter at any time.
		 *	This helps a great deal when breaking a state into multiple parts, each of timed length
		 *	(for example, a count-down before a match; after the count-down, the timer can be reset,
		 *	so that it correctly reports the actual length of the match from that point forward).
		 */
		CTime						m_tLocalTime;
		
		/**
		 * Local time that keeps track of the render time, which may not be the same as the logical
		 *	time stored in m_tLocalTime.  This should be used in Draw().
		 */
		CTime						m_tRenderTime;

		/**
		 * Its own heap.  It is encouraged, but not required, that this be used for allocations related
		 *	to the state.
		 * This is created and destroyed by the state machine.  It is created before Init() is called and
		 *	destroyed after Destroy() is called.
		 * This design helps reduce (or eliminate if used strictly) fragmentation and helps the system track
		 *	memory leaks.
		 */
		CStdAllocator *				m_psaAllocator;

		/**
		 * This state ID.
		 */
		uint32_t					m_ui32StateId;
	};

}	// namespace lse

#endif	// __LSE_STATE_H__
