/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A framebuffer describes what render targets are set at a time.
 */

#include "LSGFramebuffer.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CFramebuffer::CFramebuffer() {
	}
	LSE_CALLCTOR CFramebuffer::~CFramebuffer() {
		if ( CGfx::m_rsCurRenderState.pfFramebuffer == this ) {
			CGfx::m_rsCurRenderState.pfFramebuffer = NULL;
		}
	}

}	// namespace lsg
