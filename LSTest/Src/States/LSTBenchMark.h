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
 * Description: Benchmarking the engine.
 */


#ifndef __LST_BENCHMARK_H__
#define __LST_BENCHMARK_H__

#include "../LSTTest.h"
#include "VertexBuffer/LSGVertexBuffer.h"

/**
 * Class CBenchMark
 * \brief Benchmarking the engine.
 *
 * Description: Benchmarking the engine.
 */
class CBenchMark : public CState {
public :
	// == Various constructors.
	LSE_CALLCTOR					CBenchMark();


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
	virtual void LSE_CALL			Init( CGame * _pgGame, int32_t _ui32State, int32_t _ui32PrevState, uintptr_t _uptrUser );

	/**
	 * Sort function.
	 *
	 * \param _pvContext Unused.
	 * \param _pui32Left Left operand.
	 * \param _pui32Right Left operand.
	 * \return Returns the lexicographical comparison between the two objects.
	 */
	static int32_t LSE_CCALL		SortFunc( void * _pvContext,
			const uint32_t * _pui32Left, const uint32_t * _pui32Right );
};

#endif	// __LST_BENCHMARK_H__

