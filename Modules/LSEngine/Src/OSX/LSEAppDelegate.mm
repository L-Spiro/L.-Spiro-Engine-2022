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
 * Description: The app delegate for iOS apps.
 */

#include "LSEAppDelegate.h"

#ifdef LSE_IPHONE

#include "../Game/LSEGame.h"

// == Properties.
@implementation LSAppDelegate

// == Functions.
/**
 * Initializes the engine and game.  Must be overridden.  Use this to create your own CGame instance,
 *	after calling [super application:application didFinishLaunchingWithOptions:launchOptions].
 *
 * \param application Unused parameter.
 * \param launchOptions Unused parameter.
 * \return Returns YES if the engine was successfully created.
 */
- (void) applicationDidFinishLaunching:(NSNotification *)aNotification {
	lse::CEngine::LSE_ENGINE_INIT eiInit = {
		32 * 1024 * 1024,	// 32-megabyte default heap.
		true				// Growable.
	};
	lse::CEngine::InitEngine( eiInit );

    return YES;
}

/**
 * Shuts down the game engine.  Must be overridden to include the deletion of the custom game class
 *	created in the overidden application:didFinishLaunchingWithOptions: method.  Delete the game
 *	object BEFORE calling [super applicationWillTerminate:application].
 *
 * \param application Unused parameter.
 */
- (void) applicationWillTerminate:(NSNotification *)aNotification {
	if ( application == nil && application != nil ) { return; }	// Just to remove the warning.
	lse::CEngine::DestroyEngine();
	NSLog( @"applicationWillTerminate" );
}

@end

#endif	// #ifdef LSE_IPHONE
