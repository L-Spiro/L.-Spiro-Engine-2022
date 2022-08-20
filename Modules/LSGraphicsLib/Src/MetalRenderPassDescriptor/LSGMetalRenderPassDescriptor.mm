/**
 * Copyright L. Spiro 2017
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A Metal render-pass descriptor.
 */

#include "LSGMetalRenderPassDescriptor.h"

#ifdef LSG_METAL

#include <Metal/MTLRenderPass.h>

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalRenderPassDescriptor::CMetalRenderPassDescriptor() {
		MTLRenderPassDescriptor * prpdDesc = [MTLRenderPassDescriptor renderPassDescriptor];
		m_pvObj = CFBridgingRetain( prpdDesc );
	}
	LSE_CALLCTOR CMetalRenderPassDescriptor::~CMetalRenderPassDescriptor() {
		if ( m_pvObj ) {
			CFBridgingRelease( m_pvObj );
			m_pvObj = NULL;
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_METAL
