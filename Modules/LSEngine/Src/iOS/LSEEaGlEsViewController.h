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
 * Description: The view controller for the main view.
 */


#ifndef __LSE_EAGLESVIEWCONTROLLER_H__
#define __LSE_EAGLESVIEWCONTROLLER_H__

#include "../LSEEngine.h"

#if defined( LSE_IPHONE ) && defined( LSG_OGLES )

#include "../Game/LSEGame.h"
#include "Atomic/LSSTDAtomic.h"
#include "Events/LSSTDEvent.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Put the game thread on a second thread? */
#define LSE_MULTITHREADLOOP

@interface LSEaGlEsViewController : UIViewController {
	// == Members.
	/** The game pointer. */
	lse::CGame *								m_pgGame;
	
#ifdef LSE_MULTITHREADLOOP
	/** The game thread. */
	NSThread *									m_ptGameThread;
	
	/** The v-sync event. */
	CEvent										m_eVSyncEvent;
	
	/** Do we need to reallocate the main framebuffer/renderbuffer?  Used on the game thread. */
	CAtomic::ATOM								m_aRefreshFramebuffer;
	
	/** Is the game thread running? */
	LSBOOL										m_bThreadRunning;
#endif	// #ifdef LSE_MULTITHREADLOOP
}


// == Properties.
@property (strong, nonatomic) EAGLContext *		contextThread0;
@property (readonly, nonatomic) lse::CGame *	game;



// == Functions.
/**
 * Sets the game pointer.  Must be set before the game can begin.
 *
 * \param game The game pointer.
 */
- (void) setGame:(lse::CGame *)game;

/**
 * Handles the display link.
 *
 * \param displayLink The display link that triggered.
 */
- (void) handleDisplayLink:(CADisplayLink *)displayLink;

/**
 * Makes the first thread context current.
 */
- (void) makeCurrentThread0;

/**
 * Closes the game and waits for the game thread (if applicable) to end.
 */
- (void) closeGame;


#ifdef LSE_MULTITHREADLOOP
/**
 * The game thread.
 *
 * \param argument The thread argument.
 */
- (void) gameThread:(id)argument;
#endif	// #ifdef LSE_MULTITHREADLOOP
 
@end

#endif	// #if defined( LSE_IPHONE ) && defined( LSG_OGLES )

#endif	// __LSE_EAGLESVIEWCONTROLLER_H__
