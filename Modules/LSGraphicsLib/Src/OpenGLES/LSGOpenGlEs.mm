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
 * Description: Interface with the OpenGL ES * API.
 */

#include "LSGOpenGlEs.h"

#ifdef LSG_OGLES

#ifdef LSE_IPHONE
#import <QuartzCore/QuartzCore.h>
#endif	// #ifdef LSE_IPHONE

namespace lsg {

	// == Functions.
	/**
	 * Present the backbuffer to the screen.
	 */
	LSVOID LSE_FCALL COpenGlEs::Present() {
		if ( m_pvContext0 && m_bActive ) {
			glWarnError( "Uncaught" );
			static const GLenum eDiscard[] = {
				GL_DEPTH_ATTACHMENT,
				GL_STENCIL_ATTACHMENT,
			};
			::glDiscardFramebufferEXT( GL_FRAMEBUFFER, LSE_ELEMENTS( eDiscard ), eDiscard );
			
			BindRenderbuffer( m_uiBackRenderbuffer );
			[(__bridge EAGLContext *)m_pvContext0 presentRenderbuffer:GL_RENDERBUFFER];
			glWarnError( "Failed to present" );
		}
	}
	
	/**
	 * Make this OpenGL context current.
	 */
	LSVOID LSE_CALL COpenGlEs::MakeCurrent() {
		[EAGLContext setCurrentContext:(__bridge EAGLContext *)(m_pvContext0)];
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
