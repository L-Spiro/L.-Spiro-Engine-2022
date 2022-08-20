/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: The game class for the sprite editor.
 */


#ifndef __LSSE_SPRITEGAME_H__
#define __LSSE_SPRITEGAME_H__

#include "../LSSESpriteEditor.h"
#include "Game/LSEGame.h"

/**
 * Class CSpriteGame
 * \brief The game class for the sprite editor.
 *
 * Description: The game class for the sprite editor.
 */
class CSpriteGame : public CGame {
public :
	// == Various constructors.
	LSE_CALLCTOR							CSpriteGame();
	LSE_CALLCTOR							~CSpriteGame();


	// == Functions.


protected :
	// == Members.
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

#endif	// __LSSE_SPRITEGAME_H__
