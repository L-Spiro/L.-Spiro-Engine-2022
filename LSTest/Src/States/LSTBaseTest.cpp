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

#include "LSTBaseTest.h"


// == Various constructors.
LSE_CALLCTOR CBaseTest::CBaseTest() {
}

// == Functions.
/**
 * Called once every frame the state has focus to allow the state to render itself.
 * If the state is in the background, this is not called.  Return false to exit the game.
 *
 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
 * \return Return false if the game should close.
 */
LSBOOL LSE_CALL CBaseTest::Draw( CGame * _pgGame ) {
	CGfx::SetClearColor( CMathLib::FMod( _pgGame->Time().GetCurMills() / 1000.0f, LSM_ONE ), 1.0f, 0.0f, 1.0f );
	//CGfx::Clear();
	return true;
}