//
//  LSMetalViewController.mm
//  LSEngine iOS
//
//  Created by USER on 3/31/14.
//  Copyright (c) 2014 L. Spiro Soft. All rights reserved.
//

#import "LSEMetalViewController.h"
#include "LSEMetalView.h"

#if defined( LSE_IPHONE ) && defined( LSE_METAL )

@interface LSMetalViewController ()

@end

@implementation LSMetalViewController

// == Properties.
@synthesize metalObject = m_pmoMetalObject;
@synthesize game = m_pgGame;

// == Functions.
/**
 * Initialize.
 *
 * \return Returns nil if creation failed or a self pointer otherwise.
 */
- (id) init {
	if ( self = [super init] ) {
		m_pgGame = nil;
		
		
	}
	return self;
}

/**
 * Called when the view loads.
 */
- (void) viewDidLoad {
	// THREAD 0.
    [super viewDidLoad];
	
	// Create the view.
	LSMetalView * pvView = [[LSMetalView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	[self setView:pvView];
	pvView.backgroundColor = [UIColor blackColor];
	pvView.hidden = NO;
	pvView.alpha = 1.0f;
	pvView.contentScaleFactor = 2.0f;
	
	m_pmoMetalObject = [[CMetalObject alloc] initWithView:pvView];
	CMetal::SetDevice( (__bridge LSVOID *)m_pmoMetalObject );

	
#ifdef LSE_MULTITHREADLOOP
	m_aRefreshFramebuffer = 1;
#endif	// #ifdef LSE_MULTITHREADLOOP

}


/**
 * Releases memory.
 */
- (void) didReceiveMemoryWarning {
	// THREAD 0.
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/**
 * Called when the view is rotated.
 *
 * \param fromInterfaceOrientation The previous orientation.
 */
- (void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation {
	// THREAD 0.
#ifdef LSE_MULTITHREADLOOP
	// Let the game thread handle it.
	CAtomic::InterlockedOr( m_aRefreshFramebuffer, 1 );
#else
	[self updateViewFramebuffer];
#endif	// #ifdef LSE_MULTITHREADLOOP
}

/**
 * Sets the game pointer.  Must be set before the game can begin.
 *
 * \param game The game pointer.
 */
- (void) setGame:(lse::CGame *)game {
	// THREAD 0.
	assert( game );
	if ( m_pgGame ) {
		NSLog( @"The game pointer can only be set once." );
		return;
	}
	m_pgGame = game;
	LSMetalView * view = reinterpret_cast<LSMetalView *>(self.view);
	[view setGame:game];
	
	CGraphicsLib::InitGraphicsLibApi( NULL );
	
	// Create the view framebuffer and tell the game the size of it.
	[self updateViewFramebuffer];
#ifdef LSE_MULTITHREADLOOP
	m_aRefreshFramebuffer = 0;	// No need to create the buffers now.
#endif	// #ifdef LSE_MULTITHREADLOOP
	
	lse::CEngine::LSE_ENGINE_SECONDARY_INIT esiInit = {
		m_pgGame,
		static_cast<LSUINT32>(m_pgGame->GetWidth()), static_cast<LSUINT32>(m_pgGame->GetHeight()),
		static_cast<LSUINT32>(m_pgGame->GetWidth()), static_cast<LSUINT32>(m_pgGame->GetHeight()),
		"",
		true
	};
	lse::CEngine::SecondaryInit( esiInit );
	
#ifdef LSE_MULTITHREADLOOP
	// Create the game thread.
	m_ptGameThread = [[NSThread alloc] initWithTarget:self selector:@selector( gameThread: ) object:nil];
	m_bThreadRunning = false;
	if ( m_ptGameThread ) {
		m_bThreadRunning = true;
		[m_ptGameThread start];
	}
#endif	// #ifdef LSE_MULTITHREADLOOP
	
	// Begin the loop.
	CADisplayLink * pdlLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(handleDisplayLink:)];
	[pdlLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

/**
 * Handles the display link.
 *
 * \param displayLink The display link that triggered.
 */
- (void) handleDisplayLink:(CADisplayLink *)displayLink {
	// THREAD 0.
#ifdef LSE_MULTITHREADLOOP
	// Just trigger the game-loop thread.
	m_eVSyncEvent.Trigger();
#else
	// Single-threaded loop: Tick and present on the display link.
	if ( m_pgGame ) {
		m_pgGame->Tick();
	}
	CGfx::Present();
#endif	// #ifdef LSE_MULTITHREADLOOP
}

/**
 * Closes the game and waits for the game thread (if applicable) to end.
 */
- (void) closeGame {
	// THREAD 0.
	if ( m_pgGame ) {
		m_pgGame->Close();
		
#ifdef LSE_MULTITHREADLOOP
		while ( m_bThreadRunning ) {
			[self handleDisplayLink:nil];
		}
#else
		m_pgGame->SetNextState( lse::LSE_GS_INVALID, 0 );
		m_pgGame->Tick();
#endif	// #ifdef LSE_MULTITHREADLOOP
	}
	m_pgGame = NULL;
}

#ifdef LSE_MULTITHREADLOOP

/**
 * The game thread.
 *
 * \param argument The thread argument.
 */
- (void) gameThread:(id)argument {
	// THREAD 1.

	while ( !m_pgGame || !m_pgGame->IsClosing() ) {
		// Wait for the signal.
		m_eVSyncEvent.WaitEvent();
		if ( m_pgGame && m_pgGame->IsClosing() ) { break; }
		

		if ( CAtomic::InterlockedCompareExchange( m_aRefreshFramebuffer, 1, 0 ) == 1 ) {
			[self updateViewFramebuffer];
		}

		// V-sync signalled.  Tick the game.
		if ( m_pgGame ) {
			m_pgGame->Tick();
		}
		
		CGfx::Present();
	}
	if ( m_pgGame ) {
		m_pgGame->SetNextState( lse::LSE_GS_INVALID, 0 );
		m_pgGame->Tick();
	}
	m_bThreadRunning = false;
	return;
}

#endif	// #ifdef LSE_MULTITHREADLOOP

/**
 * Updates the framebuffer of the view if invalid.
 */
- (void) updateViewFramebuffer {
	LSMetalView * pvView = reinterpret_cast<LSMetalView *>(self.view);
	CGRect rRect = pvView.bounds;
	CGFloat fScale = pvView.contentScaleFactor;
	
	if ( m_pgGame ) {
		m_pgGame->SetScreenDimensions( rRect.size.width * fScale,
			rRect.size.height * fScale );
	}
}

@end

#endif	// #if defined( LSE_IPHONE ) && defined( LSE_METAL )
