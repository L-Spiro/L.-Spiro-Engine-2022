//
//  LSMyAppDelegate.h
//  LSTest iOS
//
//  Created by USER on 4/3/14.
//  Copyright (c) 2014 L. Spiro Soft. All rights reserved.
//

#import <iOS/LSEAppDelegate.h>
#include <Game/LSEGame.h>
using namespace lse;

@interface LSMyAppDelegate : LSAppDelegate {
	CGame *					m_pgGame;
}

@end