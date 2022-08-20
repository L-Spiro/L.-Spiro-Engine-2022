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
* Description: The Direct3D 9 implementation of a framebuffer.
*/

#include "LSGDirectX9FramebufferBase.h"

#ifdef LSG_DX9

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9FramebufferBase::CDirectX9FramebufferBase() {
	}
	LSE_CALLCTOR CDirectX9FramebufferBase::~CDirectX9FramebufferBase() {
	}

	// == Functions.
	/**
	 * Activates the framebuffer.  Applies all render targets.
	 *
	 * \return Returns true if the framebuffer was activated.
	 */
	LSBOOL LSE_CALL CDirectX9FramebufferBase::Activate() {
		for ( uint32_t I = LSE_ELEMENTS( m_aColors ); I--; ) {
			if ( !m_aColors[I].prtiRenderTarget ) {
				CDirectX9::m_rsCurRenderState.prtRenderTargets[I] = NULL;
			}
			else {
				m_aColors[I].prtiRenderTarget->SetAsColorRenderTarget( m_aColors[I].ui32Mipmap, m_aColors[I].ui32ArraySlice, I );
			}
		}
		if ( !m_aDepthStencil.prtiRenderTarget ) {
			CDirectX9::m_rsCurRenderState.pdsDepthStencil = NULL;
		}
		else {
			m_aDepthStencil.prtiRenderTarget->SetAsDepthStencilRenderTarget();
		}
		return true;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
