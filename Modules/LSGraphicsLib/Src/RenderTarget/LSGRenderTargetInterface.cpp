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
 * Description: The interface for all render targets.
 */

#include "LSGRenderTargetInterface.h"


namespace lsg {

	// == Members.
	/** ID counter. */
	CAtomic::ATOM CRenderTargetInterface::m_aIdCounter = 0;

	// == Various constructors.
	LSE_CALLCTOR CRenderTargetInterface::CRenderTargetInterface() :
		m_ui32Id( CAtomic::InterlockedIncrement( m_aIdCounter ) ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ) {
	}
	LSE_CALLCTOR CRenderTargetInterface::~CRenderTargetInterface() {
	}

	// == Functions.
	/**
	 * Activates the object as a color render target.
	 *
	 * \param _ui32Mipmap The mipmap to set.
	 * \param _ui32Slice The slice in a 3D texture to set.
	 * \param _ui32Slot The color slot into which to activate the render target.
	 * \return Returns true if it is ready to be used as a render target.
	 */
	LSBOOL LSE_CALL CRenderTargetInterface::SetAsColorRenderTarget( uint32_t /*_ui32Mipmap*/, uint32_t /*_ui32Slice*/, uint32_t /*_ui32Slot*/ ) const {
		return false;
	}

	/**
	 * Activate as a depth/stencil render target.
	 *
	 * \return Returns true if it is ready to be used as a render target.
	 */
	LSBOOL LSE_CALL CRenderTargetInterface::SetAsDepthStencilRenderTarget() const {
		return false;
	}

}	// namespace lsg
