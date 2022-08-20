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
 * Description: DESCRIPTION.
 */


#ifndef __LST_TRIANGLETEST_H__
#define __LST_TRIANGLETEST_H__

#include "../LSTTest.h"

/**
 * Class CTriangleTest
 * \brief BRIEF
 *
 * Description: DESCRIPTION.
 */
class CTriangleTest : public CState {
public :
	// == Various constructors.
	LSE_CALLCTOR					CTriangleTest();


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
	 * Called once every frame the state has focus to allow the state to render itself.
	 * If the state is in the background, this is not called.  Return false to exit the game.
	 *
	 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
	 * \return Return false if the game should close.
	 */
	virtual LSBOOL LSE_CALL			Draw( CGame * _pgGame );

protected :
	// == Members.
	CShader							m_sShader;
	CVertexBuffer					m_vbVertBuffer;
	CIndexBuffer					m_ibIndexBuffer;
	uint32_t						m_ui32Frames;
	uint64_t						m_ui64LastTime;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#endif	// __LST_TRIANGLETEST_H__
