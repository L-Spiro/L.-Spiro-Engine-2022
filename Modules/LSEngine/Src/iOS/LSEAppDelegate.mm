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
#import <UIKit/UIKit.h>

// == Properties.
@implementation LSAppDelegate
@synthesize window = _window;
@synthesize viewController = _viewController;

// == Functions.
/**
 * Initializes the engine and game.  Must be overridden.  Use this to create your own CGame instance,
 *	after calling [super application:application didFinishLaunchingWithOptions:launchOptions].
 *
 * \param application Unused parameter.
 * \param launchOptions Unused parameter.
 * \return Returns YES if the engine was successfully created.
 */
- (BOOL) application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
	if ( application == nil && application != nil ) { return NO; }		// Just to remove the warning.
	if ( launchOptions == nil && launchOptions != nil ) { return NO; }	// Just to remove the warning.
	lse::CEngine::LSE_ENGINE_INIT eiInit = {
		8 * 1024 * 1024,	// 8-megabyte default heap.
		true				// Growable.
	};
	lse::CEngine::InitEngine( eiInit );
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.viewController = [[LSViewController alloc] init];

	self.window.backgroundColor = [UIColor greenColor];	// Green screen errors.
	[UIApplication sharedApplication].idleTimerDisabled = YES;
	[UIApplication sharedApplication].statusBarHidden = YES;
	

	self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    return YES;
}

/**
 * Shuts down the game engine.  Must be overridden to include the deletion of the custom game class
 *	created in the overidden application:didFinishLaunchingWithOptions: method.  Delete the game
 *	object BEFORE calling [super applicationWillTerminate:application].
 *
 * \param application Unused parameter.
 */
- (void) applicationWillTerminate:(UIApplication *)application {
	if ( application == nil && application != nil ) { return; }	// Just to remove the warning.
	lse::CEngine::DestroyEngine();
	NSLog( @"applicationWillTerminate" );
}

/**
 * Called when the game goes into the background.  Sets the game to inactive.
 *
 * \param application Unused parameter.
 */
- (void) applicationDidEnterBackground:(UIApplication *)application {
	if ( application == nil && application != nil ) { return; }	// Just to remove the warning.
	if ( self.viewController.game != nil ) {
		self.viewController.game->SetActive( false );
	}
}

/**
 * Called when the game goes into the foreground.  Sets the game to active.
 *
 * \param application Unused parameter.
 */
- (void) applicationDidBecomeActive:(UIApplication *)application {
	if ( application == nil && application != nil ) { return; }	// Just to remove the warning.
	if ( self.viewController.game != nil ) {
		self.viewController.game->SetActive( true );
	}
}

@end

#endif	// #ifdef LSE_IPHONE
