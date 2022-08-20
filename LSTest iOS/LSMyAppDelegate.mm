//
//  LSMyAppDelegate.m
//  LSTest iOS
//
//  Created by USER on 4/3/14.
//  Copyright (c) 2014 L. Spiro Soft. All rights reserved.
//

#import "LSMyAppDelegate.h"
#include "../LSTest/Src/States/LSTStateFactory.h"

@implementation LSMyAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [super application:application didFinishLaunchingWithOptions:launchOptions];

	m_pgGame = LSENEW CGame();
	//[super.viewController setGame:m_pgGame];
	[self.viewController setGame:m_pgGame];
	m_pgGame->SetStateFactory( LSENEW ::CStateFactory() );
	m_pgGame->SetNextState( LST_S_LOADMESHTEST, 0 );

    return YES;
}

- (void)applicationWillTerminate:(UIApplication *)application {
	if ( m_pgGame ) {
		[self.viewController closeGame];

		LSEDELETE m_pgGame;
		m_pgGame = NULL;
	}
	[super applicationWillTerminate:application];
}

@end
