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
 * Description: The base class for framebuffers.
 */


#ifndef __LSG_FRAMEBUFFERBASE_H__
#define __LSG_FRAMEBUFFERBASE_H__

#include "../LSGGraphicsLib.h"
#include "../Gfx/LSGGfx.h"
#include "../RenderTarget/LSGRenderTargetInterface.h"

namespace lsg {

	/**
	 * Class CFramebufferBase
	 * \brief The base class for framebuffers.
	 *
	 * Description: The base class for framebuffers.
	 */
	class CFramebufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CFramebufferBase();
		LSE_CALLCTOR							~CFramebufferBase();


		// == Functions.
		/**
		 * Gets the width of the framebuffer.
		 *
		 * \return Returns the width of the framebuffer.
		 */
		LSE_INLINE uint32_t LSE_CALL			Width() const;

		/**
		 * Gets the height of the framebuffer.
		 *
		 * \return Returns the height of the framebuffer.
		 */
		LSE_INLINE uint32_t LSE_CALL			Height() const;

		/**
		 * Attaches a renderbuffer to the given color attachment by index.
		 *
		 * \param _prtiRenderTarget Renderbuffer to attach to the given color slot.
		 * \param _ui32Slot The slot to which to attach the renderbuffer.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							RenderbufferColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Slot );

		/**
		 * Attaches a renderbuffer to the given depth-stencil attachment.
		 *
		 * \param _prtiRenderTarget Renderbuffer to attach to the depth-stencil.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							RenderbufferDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget );

		/**
		 * Attaches a texture to the given color attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the given color slot.
		 * \param _ui32Level Mipmap level to attach.
		 * \param _ui32Slot The slot to which to attach the texture.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture1DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Slot );

		/**
		 * Attaches a texture to the given color attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the given color slot.
		 * \param _ui32Level Mipmap level to attach.
		 * \param _ui32Slot The slot to which to attach the texture.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Slot );

		/**
		 * Attaches a texture to the given color attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the given color slot.
		 * \param _ui32Level Mipmap level to attach.
		 * \param _ui32Layer 2D slice of the 3D texture to attach.
		 * \param _ui32Slot The slot to which to attach the texture.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture3DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Layer, uint32_t _ui32Slot );

		/**
		 * Attaches a texture to the given depth-stencil attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
		 * \param _ui32Level Mipmap level to attach.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture1DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level );

		/**
		 * Attaches a texture to the given depth-stencil attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
		 * \param _ui32Level Mipmap level to attach.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level );

		/**
		 * Attaches a texture to the given depth-stencil attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
		 * \param _ui32Level Mipmap level to attach.
		 * \param _ui32Layer 2D slice of the 3D texture to attach.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture3DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Layer );


	protected :
		// == Types.
		/** A single attachment. */
		typedef struct LSG_ATTACHMENT {
			/** The render target itself. */
			CRenderTargetInterface *			prtiRenderTarget;

			/** Which mipmap to apply. */
			uint32_t							ui32Mipmap;

			/** Which array slice to apply. */
			uint32_t							ui32ArraySlice;
		} * LPLSG_ATTACHMENT, * const LPCLSG_ATTACHMENT;


		// == Members.
		/** Width of the framebuffer.  Set when the framebuffer is applied. */
		uint32_t								m_ui32Width;

		/** Height of the framebuffer.  Set when the framebuffer is applied. */
		uint32_t								m_ui32Height;

		/** The color render targets. */
		LSG_ATTACHMENT							m_aColors[LSG_MAX_RENDER_TARGET_COUNT];

		/** The depth/stencil target. */
		LSG_ATTACHMENT							m_aDepthStencil;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the width of the framebuffer.
	 *
	 * \return Returns the width of the framebuffer.
	 */
	LSE_INLINE uint32_t LSE_CALL CFramebufferBase::Width() const {
		return m_ui32Width;
	}

	/**
	 * Gets the height of the framebuffer.
	 *
	 * \return Returns the height of the framebuffer.
	 */
	LSE_INLINE uint32_t LSE_CALL CFramebufferBase::Height() const {
		return m_ui32Height;
	}

}	// namespace lsg

#endif	// __LSG_FRAMEBUFFERBASE_H__
