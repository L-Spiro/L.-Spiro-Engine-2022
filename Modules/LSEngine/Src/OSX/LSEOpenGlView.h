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


#ifndef __LSE_EAGLESVIEW_H__
#define __LSE_EAGLESVIEW_H__

#include "../LSEEngine.h"

#ifdef LSE_IPHONE

#include "../Game/LSEGame.h"
#include "../Input/LSEInputQueue.h"
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface LSOpenGlView {
	// == Members.
	/** The game pointer. */
	lse::CGame *								m_pgGame;
	
	/** Copy of the OpenGL ES context. */
	EAGLContext *								m_pcContext;

	/** OpenGL ES handle to the framebuffer. */
	GLuint										m_uiFramebuffer;
	
	/** OpenGL ES handle to the renderbuffer. */
	GLuint										m_uiRenderbuffer;
	
	/** OpenGL ES handle to the depth/stencil buffer. */
	GLuint										m_uiDepthStencilBuffer;
	
	/** Width of the back buffer. */
	GLint										m_iWidth;
	
	/** Height of the back buffer. */
	GLint										m_iHeight;
	
	/** Slave timer for stamping inputs. */
	CTime										m_tInputTime;
	
	/** The input queue on the main thread. */
	lse::CInputQueue *							m_piqInputQueue;

}


// == Properties.
@property (readonly, nonatomic) lse::CGame *	game;
@property (nonatomic) EAGLContext *				context;
@property (readonly, nonatomic) GLuint			framebufferId;
@property (readonly, nonatomic) GLuint			renderbufferId;
@property (readonly, nonatomic) GLint			bufferWidth;
@property (readonly, nonatomic) GLint			bufferHeight;


// == Functions.
/**
 * Sets the game pointer.  Must be set before the game can begin.
 *
 * \param game The game pointer.
 */
- (void) setGame:(lse::CGame *)game;

/**
 * Creates the framebuffers.
 *
 * \return Returns an error code indicate success or type of failure.
 */
- (LSSTD_ERRORS) createFramebuffer;

@end

#endif	// #ifdef LSE_IPHONE
#endif	// __LSE_EAGLESVIEW_H__
