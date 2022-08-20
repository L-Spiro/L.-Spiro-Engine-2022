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


#ifndef __LSG_RENDERTARGETINTERFACE_H__
#define __LSG_RENDERTARGETINTERFACE_H__

#include "../LSGGraphicsLib.h"
#include "Atomic/LSSTDAtomic.h"

namespace lsg {

	/**
	 * Class CRenderTargetInterface
	 * \brief The interface for all render targets.
	 *
	 * Description: The interface for all render targets.
	 */
	class CRenderTargetInterface {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CRenderTargetInterface();
		virtual LSE_CALLCTOR					~CRenderTargetInterface();


		// == Functions.
		/**
		 * Gets the width of the surface.
		 *
		 * \return Returns the width of the surface.
		 */
		LSE_INLINE uint32_t LSE_CALL			GetWidth() const;

		/**
		 * Gets the height of the surface.
		 *
		 * \return Returns the height of the surface.
		 */
		LSE_INLINE uint32_t LSE_CALL			GetHeight() const;

		/**
		 * Gets the unique ID of this render target, which can never be 0.
		 *
		 * \return Returns the unique ID of this render target.
		 */
		LSE_INLINE uint32_t LSE_CALL			GetId() const;

		/**
		 * Activates the object as a color render target.
		 *
		 * \param _ui32Mipmap The mipmap to set.
		 * \param _ui32Slice The slice in a 3D texture to set.
		 * \param _ui32Slot The color slot into which to activate the render target.
		 * \return Returns true if it is ready to be used as a render target.
		 */
		virtual LSBOOL LSE_CALL					SetAsColorRenderTarget( uint32_t _ui32Mipmap, uint32_t _ui32Slice, uint32_t _ui32Slot ) const;

		/**
		 * Activate as a depth/stencil render target.
		 *
		 * \return Returns true if it is ready to be used as a render target.
		 */
		virtual LSBOOL LSE_CALL					SetAsDepthStencilRenderTarget() const;


	protected :
		// == Members.
		/** ID of this render target. */
		uint32_t								m_ui32Id;

		/** Width of the surface. */
		uint32_t								m_ui32Width;

		/** Height of the surface. */
		uint32_t								m_ui32Height;

		/** ID counter. */
		static CAtomic::ATOM					m_aIdCounter;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the width of the surface.
	 *
	 * \return Returns the width of the surface.
	 */
	LSE_INLINE uint32_t LSE_CALL CRenderTargetInterface::GetWidth() const { return m_ui32Width; }

	/**
	 * Gets the height of the surface.
	 *
	 * \return Returns the height of the surface.
	 */
	LSE_INLINE uint32_t LSE_CALL CRenderTargetInterface::GetHeight() const { return m_ui32Height; }

	/**
	 * Gets the unique ID of this render target, which can never be 0.
	 *
	 * \return Returns the unique ID of this render target.
	 */
	LSE_INLINE uint32_t LSE_CALL CRenderTargetInterface::GetId() const {
		return m_ui32Id;
	}

}	// namespace lsg

#endif	// __LSG_RENDERTARGETINTERFACE_H__
