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
 * Description: Framebuffer interface for OpenGL ES, not to be confused with the COpenGlEsFramebuffer class.  The
 *	COpenGlEsFramebuffer class directly wraps an OpenGL ES framebuffer object, whereas this class inherits from
 *	CFramebufferBase and is meant to provide the OpenGL ES functionality for generic cross-platform framebuffers.
 */


#ifndef __LSG_OPENGLESFRAMEBUFFERBASE_H__
#define __LSG_OPENGLESFRAMEBUFFERBASE_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGLES

#include "../Framebuffer/LSGFramebufferBase.h"
#include "LSGOpenGlEsFramebuffer.h"

namespace lsg {

	/**
	 * Class COpenGlEsFramebufferBase
	 * \brief Framebuffer interface for OpenGL ES, not to be confused with the COpenGlEsFramebuffer class.
	 *
	 * Description: Framebuffer interface for OpenGL ES, not to be confused with the COpenGlEsFramebuffer class.  The
	 *	COpenGlEsFramebuffer class directly wraps an OpenGL ES framebuffer object, whereas this class inherits from
	 *	CFramebufferBase and is meant to provide the OpenGL ES functionality for generic cross-platform framebuffers.
	 */
	class COpenGlEsFramebufferBase : public CFramebufferBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlEsFramebufferBase();


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
		LSBOOL LSE_CALL							RenderbufferColorAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Slot );

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
		LSBOOL LSE_CALL							Texture2DColorAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Level, LSUINT32 _ui32Slot );

		/**
		 * Attaches a texture to the given depth-stencil attachment by index.
		 *
		 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
		 * \param _ui32Level Mipmap level to attach.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Level );


	protected :
		// == Members.
		/** The framebuffer object we manage. */
		COpenGlEsFramebuffer					m_ogfbFrameBuffer;


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

#endif	// #ifdef LSG_OGLES

#endif	// __LSG_OPENGLESFRAMEBUFFERBASE_H__
