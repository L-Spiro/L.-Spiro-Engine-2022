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
 * Description: The main view.
 */


#ifndef __LSE_METALVIEW_H__
#define __LSE_METALVIEW_H__

#include "../LSEEngine.h"

#if defined( LSE_IPHONE ) && defined( LSE_METAL )

#include "../Game/LSEGame.h"
#include "../Input/LSEInputQueue.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface LSMetalView : UIView <UIAccelerometerDelegate> {
	// == Members.
	/** The game pointer. */
	lse::CGame *								m_pgGame;

#ifdef LSG_OGLES
	/** Copy of the Metal context. */
	EAGLContext *								m_pcContext;

	/** Metal handle to the framebuffer. */
	GLuint										m_uiFramebuffer;
	
	/** Metal handle to the renderbuffer. */
	GLuint										m_uiRenderbuffer;
	
	/** Metal handle to the depth/stencil buffer. */
	GLuint										m_uiDepthStencilBuffer;
	
	/** Width of the back buffer. */
	GLint										m_iWidth;
	
	/** Height of the back buffer. */
	GLint										m_iHeight;
#endif	// #ifdef LSG_OGLES
	
	/** Slave timer for stamping inputs. */
	CTime										m_tInputTime;
	
	/** The input queue on the main thread. */
	lse::CInputQueue *							m_piqInputQueue;

}


// == Properties.
@property (readonly, nonatomic) lse::CGame *	game;
#ifdef LSG_OGLES
@property (nonatomic) EAGLContext *				context;
@property (readonly, nonatomic) GLuint			framebufferId;
@property (readonly, nonatomic) GLuint			renderbufferId;
@property (readonly, nonatomic) GLint			bufferWidth;
@property (readonly, nonatomic) GLint			bufferHeight;
#endif	// #ifdef LSG_OGLES


// == Functions.
/**
 * Sets the game pointer.  Must be set before the game can begin.
 *
 * \param game The game pointer.
 */
- (void) setGame:(lse::CGame *)game;

#ifdef LSG_OGLES
/**
 * Creates the framebuffers.
 *
 * \return Returns an error code indicate success or type of failure.
 */
- (LSSTD_ERRORS) createFramebuffer;
#endif	// #ifdef LSG_OGLES

@end

#endif	// #if defined( LSE_IPHONE ) && defined( LSE_METAL )
#endif	// __LSE_METALVIEW_H__
