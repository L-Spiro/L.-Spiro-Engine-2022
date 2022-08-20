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

#include "LSGOpenGlEsFramebufferBase.h"

#ifdef LSG_OGLES

#include "../OpenGLESTexture/LSGOpenGlEsTexture2d.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsFramebufferBase::COpenGlEsFramebufferBase() {
	}

	// == Functions.
	/**
	 * Activates the framebuffer.  Applies all render targets.
	 *
	 * \return Returns true if the framebuffer was activated.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::Activate() {
		return m_ogfbFrameBuffer.Bind();
	}

	/**
	 * Attaches a renderbuffer to the given color attachment by index.
	 *
	 * \param _prtiRenderTarget Renderbuffer to attach to the given color slot.
	 * \param _ui32Slot The slot to which to attach the renderbuffer.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::RenderbufferColorAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Slot ) {
		if ( !CreateIfNeeded( _prtiRenderTarget ) ) { return false; }
		if ( !m_ogfbFrameBuffer.Id() ) { return false; }
		Parent::RenderbufferColorAttach( _prtiRenderTarget, _ui32Slot );
		COpenGlEs::CTempFramebufferBinder tfbBind( m_ogfbFrameBuffer.Id() );
		assert( 0 );
		//m_ogfbFrameBuffer.RenderbufferColorAttach( _prtiRenderTarget->
		return true;
	}

	/**
	 * Attaches a renderbuffer to the given depth-stencil attachment.
	 *
	 * \param _prtiRenderTarget Renderbuffer to attach to the depth-stencil.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::RenderbufferDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget ) {
		if ( !CreateIfNeeded( _prtiRenderTarget ) ) { return false; }
		if ( !m_ogfbFrameBuffer.Id() ) { return false; }
		Parent::RenderbufferDepthStencilAttach( _prtiRenderTarget );
		COpenGlEs::CTempFramebufferBinder tfbBind( m_ogfbFrameBuffer.Id() );
		assert( 0 );
		return true;
	}

	/**
	 * Attaches a texture to the given color attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the given color slot.
	 * \param _ui32Level Mipmap level to attach.
	 * \param _ui32Slot The slot to which to attach the texture.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::Texture2DColorAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Level, LSUINT32 _ui32Slot ) {
		if ( !CreateIfNeeded( _prtiRenderTarget ) ) { return false; }
		if ( !m_ogfbFrameBuffer.Id() ) { return false; }
		Parent::Texture2DColorAttach( _prtiRenderTarget, _ui32Level, _ui32Slot );
		COpenGlEs::CTempFramebufferBinder tfbBind( m_ogfbFrameBuffer.Id() );
		return m_ogfbFrameBuffer.Texture2DColorAttach( static_cast<COpenGlEsTexture2d *>(_prtiRenderTarget)->OpenGlEsId(), _ui32Level, _ui32Slot,
			static_cast<COpenGlEsTexture2d *>(_prtiRenderTarget)->OpenGlEsTextureType() );
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
	 * \param _ui32Level Mipmap level to attach.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::Texture2DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, LSUINT32 _ui32Level ) {
		if ( !CreateIfNeeded( _prtiRenderTarget ) ) { return false; }
		if ( !m_ogfbFrameBuffer.Id() ) { return false; }
		Parent::Texture2DDepthStencilAttach( _prtiRenderTarget, _ui32Level );
		COpenGlEs::CTempFramebufferBinder tfbBind( m_ogfbFrameBuffer.Id() );
		assert( 0 );
		return true;
	}

	/**
	 * Creates the FBO if it does not exist.
	 *
	 * \param _prtiRenderTarget Texture from which to get the width and height to create the framebuffer.
	 * \return Returns false if the FBO could not be created.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebufferBase::CreateIfNeeded( CRenderTargetInterface * _prtiRenderTarget ) {
		if ( m_ogfbFrameBuffer.Id() &&
			m_ogfbFrameBuffer.Width() == _prtiRenderTarget->GetWidth() &&
			m_ogfbFrameBuffer.Height() == _prtiRenderTarget->GetHeight() ) { return true; }
		return m_ogfbFrameBuffer.CreateFramebuffer( _prtiRenderTarget->GetWidth(), _prtiRenderTarget->GetHeight() );
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
