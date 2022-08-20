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
 * Description: The main include file in this demonstration.  Just includes the engine, since that is all that
 *	is needed.
 */

#include "LSTTest.h"
#include "States/LSTStateFactory.h"


INT APIENTRY wWinMain( HINSTANCE /*_hInstance*/, HINSTANCE /*_hPrevInstance*/, LPWSTR /*_lpwsCmdLine*/, INT /*_iCmdShow*/ ) {
	// Initialize the memory manager.
	lse::CEngine::LSE_ENGINE_INIT eiInit = {
		64 * 1024 * 1024,				// Initial size of the heap.
		true							// If true, the heap is growable.
	};
	lse::CEngine::InitEngine( eiInit );
	CFileLib::SetResPath( "../../LSTest/Data/" );

	INT iRet;

	{	// Scope the game class so that it is destroyed before lse::CEngine::DestroyEngine() is called.
		// We do not need to make a custom game class for this primitive demo.  We would want to make a custom game class
		//	that inherits from lse::CGame if we wanted to handle any game data not handled by lse::CGame.  Any real game
		//	will have a custom game class.
		lse::CGame gGame;

		gGame.SetStateFactory( LSENEW ::CStateFactory() );

		// Before running the "game" we need to tell it where to begin. 
		// In this demo we just set the state to lse::LSE_GS_DEFAULT.
		gGame.SetNextState( LST_S_LOADMESHTEST, 0, true );
		
		// After creating a game class, we can finish initializing the engine.
		lse::CEngine::LSE_ENGINE_SECONDARY_INIT esiSecondInit = {
			&gGame,							// Needed for the window to send input to the game class.
			480 * 2,	320 * 2,			// Windowed resolution.
			//2000,	1125,
			0,		0,						// Full-screen resolution.  0 = desktop resolution.
			"L. Spiro Engine",				// Window title.
			false							// false = windowed mode, true = full-screen.
		};
		lse::CEngine::SecondaryInit( esiSecondInit );


		// From here, simply run the game.  It will handle the ticking of the game object and passing window messages
		//	to it so it can handle input, etc.
		iRet = lse::CEngine::Run();

	}

	// The game class and window have been destroyed by this point.  Shut down the rest of the engine.
	lse::CEngine::DestroyEngine();
	return iRet;
}
