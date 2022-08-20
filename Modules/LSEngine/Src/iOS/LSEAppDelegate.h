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


#ifndef __LSE_APPDELEGATE_H__
#define __LSE_APPDELEGATE_H__

#include "../LSEEngine.h"

#ifdef LSE_IPHONE

#include "LSEEaGlEsViewController.h"
#include "LSEMetalViewController.h"
#include <UIKit/UIKit.h>

#ifdef LSE_METAL
#define LSViewController								LSMetalViewController
#else
#define LSViewController								LSEaGlEsViewController
#endif	// #ifdef LSG_METAL

@interface LSAppDelegate : UIResponder <UIApplicationDelegate> {
	UIWindow *											window;
	LSViewController *									viewController;
}


// == Properties.
@property (strong, nonatomic) UIWindow *				window;
@property (strong, nonatomic) LSViewController *		viewController;


// == Functions.
/**
 * Initializes the engine and game.  Must be overridden.  Use this to create your own CGame instance,
 *	after calling [super application:application didFinishLaunchingWithOptions:launchOptions].
 *
 * \param application Unused parameter.
 * \param launchOptions Unused parameter.
 * \return Returns YES if the engine was successfully created.
 */
- (BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

/**
 * Shuts down the game engine.  Must be overridden to include the deletion of the custom game class
 *	created in the overidden application:didFinishLaunchingWithOptions: method.  Delete the game
 *	object BEFORE calling [super applicationWillTerminate:application].
 *
 * \param application Unused parameter.
 */
- (void) applicationWillTerminate:(UIApplication *)application;

/**
 * Called when the game goes into the background.  Sets the game to inactive.
 *
 * \param application Unused parameter.
 */
- (void) applicationDidEnterBackground:(UIApplication *)application;

/**
 * Called when the game goes into the foreground.  Sets the game to active.
 *
 * \param application Unused parameter.
 */
- (void) applicationDidBecomeActive:(UIApplication *)application;

@end

#endif	// #ifdef LSE_IPHONE

#endif	// #ifndef __LSE_APPDELEGATE_H__
