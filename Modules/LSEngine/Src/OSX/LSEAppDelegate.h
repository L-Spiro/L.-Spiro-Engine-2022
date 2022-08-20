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
 * Description: The app delegate for Macintosh OS X apps.
 */


#ifndef __LSE_APPDELEGATE_H__
#define __LSE_APPDELEGATE_H__

#include "../LSEEngine.h"

#ifdef LSE_OSX

#import <Cocoa/Cocoa.h>

@interface LSAppDelegate : NSObject <NSApplicationDelegate> {
}


// == Functions.
/**
 * Initializes the engine and game.  Must be overridden.  Use this to create your own CGame instance,
 *	after calling [super application:application didFinishLaunchingWithOptions:launchOptions].
 *
 * \param application Unused parameter.
 * \param launchOptions Unused parameter.
 * \return Returns YES if the engine was successfully created.
 */
- (void) applicationDidFinishLaunching:(NSNotification *)aNotification;

/**
 * Shuts down the game engine.  Must be overridden to include the deletion of the custom game class
 *	created in the overidden application:didFinishLaunchingWithOptions: method.  Delete the game
 *	object BEFORE calling [super applicationWillTerminate:application].
 *
 * \param application Unused parameter.
 */
- (void) applicationWillTerminate:(NSNotification *)aNotification;

@end

#endif	// #ifdef LSE_OSX

#endif	// #ifndef __LSE_APPDELEGATE_H__
