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
 * Description: Framebuffer interface for OpenGL, not to be confused with the COpenGlFramebuffer class.  The
 *	COpenGlFramebuffer class directly wraps an OpenGL framebuffer object, whereas this class inherits from
 *	CFramebufferBase and is meant to provide the OpenGL functionality for generic cross-platform framebuffers.
 */


#ifndef __LSG_OPENGLFRAMEBUFFERBASE_H__
#define __LSG_OPENGLFRAMEBUFFERBASE_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGL

#include "../Framebuffer/LSGFramebufferBase.h"
#include "LSGOpenGlFramebuffer.h"

namespace lsg {

	/**
	 * Class COpenGlFramebufferBase
	 * \brief Framebuffer interface for OpenGL, not to be confused with the COpenGlFramebuffer class.
	 *
	 * Description: Framebuffer interface for OpenGL, not to be confused with the COpenGlFramebuffer class.  The
	 *	COpenGlFramebuffer class directly wraps an OpenGL framebuffer object, whereas this class inherits from
	 *	CFramebufferBase and is meant to provide the OpenGL functionality for generic cross-platform framebuffers.
	 */
	class COpenGlFramebufferBase : public CFramebufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlFramebufferBase();


		// == Functions.
		/**
		 * Activates the framebuffer.  Applies all render targets.
		 *
		 * \return Returns true if the framebuffer was activated.
		 */
		LSBOOL LSE_CALL							Activate();

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
		// == Members.
		/** The framebuffer object we manage. */
		COpenGlFramebuffer						m_ogfbFrameBuffer;


		// == Functions.
		/**
		 * Creates the FBO if it does not exist.
		 *
		 * \param _prtiRenderTarget Texture from which to get the width and height to create the framebuffer.
		 * \return Returns false if the FBO could not be created.
		 */
		LSBOOL LSE_CALL							CreateIfNeeded( CRenderTargetInterface * _prtiRenderTarget );


	private :
		typedef CFramebufferBase				Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// #ifdef LSG_OGL

#endif	// __LSG_OPENGLFRAMEBUFFERBASE_H__
