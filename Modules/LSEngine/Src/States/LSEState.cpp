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

#include "LSEState.h"


namespace lse {

	// == Various constructors.
	LSE_CALL CState::CState() :
		m_psaAllocator( NULL ),
		m_ui32StateId( static_cast<uint32_t>(LSE_GS_INVALID) ) {
	}
	LSE_CALL CState::~CState() {
	}

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
	void LSE_CALL CState::Init( CGame * /*_pgGame*/, int32_t /*_ui32State*/, int32_t /*_ui32PrevState*/, uintptr_t /*_uptrUser*/ ) {
		// Default implementation does nothing.
	}

	/**
	 * Called when it is time for the state to die.  Free resources here.
	 *
	 * \param _pgGame Pointer to the game class which allows access to game-specific resources.
	 * \param _ui32NextState The identifier of the next state.
	 */
	void LSE_CALL CState::Destroy( CGame * /*_pgGame*/, int32_t /*_ui32NextState*/ ) {
		// Default implementation does nothing.
	}

	/**
	 * Called once every frame the state has focus (or if it is in the background but is
	 *	non-interruptable).  Do the state-specific processing here.  This can be anything,
	 *	but usually means ticking the menu manager, distributing input, running physics,
	 *	and all other game logic.
	 *
	 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
	 * \return Return false if the game should close.
	 */
	LSBOOL LSE_CALL CState::Tick( CGame * /*_pgGame*/ ) {
		return true;
	}

	/**
	 * Called once every frame the state has focus to allow the state to render itself.
	 * If the state is in the background, this is not called.  Return false to exit the game.
	 *
	 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
	 * \return Return false if the game should close.
	 */
	LSBOOL LSE_CALL CState::Draw( CGame * /*_pgGame*/ ) {
		return true;
	}

	/**
	 * If a state is not on top of the state stack, it is not ticked normally.  If the state flags
	 *	itself as non-interruptable, however, it will be ticked every frame regardless of whether
	 *	it is on top or not.
	 *
	 * \return Return true to make this state interruptable.  The default behavior is true.  States
	 *	that need to run online or continuously in the background for any reason should return false.
	 */
	LSBOOL LSE_CALL CState::Interruptable() const {
		return true;
	}

	/**
	 * Gets the size of the heap to be allocated for this state, or 0 for none.
	 *
	 * \param _pgGame The game pointer.
	 * \param _ui32State The ID of the current state.
	 * \param _uptrUser User data passed to the state upon initialization.  Same value as what is passed in
	 *	Init().
	 * \return Return a non-zero value to indicate the size of the heap to be allocated for this class.
	 */
	LSA_SIZE LSE_CALL CState::GetHeapSize( CGame * /*_pgGame*/, int32_t /*_ui32State*/, uintptr_t /*_uptrUser*/ ) const {
		return 0;
	}
	
	/**
	 * Gets our time manager.
	 *
	 * \return Returns the time manager for this state.
	 */
	const CTime & LSE_CALL CState::GetTimeMan() const {
		return m_tLocalTime;
	}

	/**
	 * Handler for sound events.
	 *
	 * \param _pgGame The game class.
	 * \param _pssSource The sound source that caused the event.
	 * \param _ui32EventType The type of event.  Can be CSoundManager::LSS_ET_SOUND_JUST_STARTED or
	 *	CSoundManager::LSS_ET_SOUND_JUST_ENDED.
	 * \param _psmManager The sound manager.
	 */
	void LSE_CALL CState::EventSound( CGame * /*_pgGame*/,
		CSoundSource * /*_pssSource*/,
		uint32_t /*_ui32EventType*/, CSoundManagerEx * /*_psmManager*/ ) {
	}
	
	/**
	 * Handler for touch events.
	 * \param _vPos The 2D position of the touch in view space.
	 */
	void LSE_CALL CState::EventTouch( const CVector2 &/*_vPos*/ ) {
	}

	/**
	 * Returns true if custom shaders should be automatically unregistered when this state is destroyed.
	 *	By default this returns true because it generally improves performance and it also removes some
	 *	fake memory-leak warnings.
	 *
	 * \return Returns true by default.
	 */
	LSBOOL LSE_CALL CState::UnRegisterAllCustomShaders() const {
		return true;
	}

}	// namespace lse
