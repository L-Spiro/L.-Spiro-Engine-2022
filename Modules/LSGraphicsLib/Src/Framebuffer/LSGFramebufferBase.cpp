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

#include "LSGFramebufferBase.h"
#include "../Gfx/LSGGfx.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CFramebufferBase::CFramebufferBase() :
		m_ui32Width( 0 ),
		m_ui32Height( 0 ) {
		CStd::MemSet( m_aColors, 0, sizeof( m_aColors ) );
		CStd::MemSet( &m_aDepthStencil, 0, sizeof( m_aDepthStencil ) );
	}
	LSE_CALLCTOR CFramebufferBase::~CFramebufferBase() {
	}

	// == Functions.
	/**
	 * Attaches a renderbuffer to the given color attachment by index.
	 *
	 * \param _prtiRenderTarget Renderbuffer to attach to the given color slot.
	 * \param _ui32Slot The slot to which to attach the renderbuffer.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::RenderbufferColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		m_aColors[_ui32Slot].prtiRenderTarget = _prtiRenderTarget;
		m_aColors[_ui32Slot].ui32Mipmap = 0;
		m_aColors[_ui32Slot].ui32ArraySlice = 0;
		return true;
	}

	/**
	 * Attaches a renderbuffer to the given depth-stencil attachment.
	 *
	 * \param _prtiRenderTarget Renderbuffer to attach to the depth-stencil.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::RenderbufferDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget ) {
		m_aDepthStencil.prtiRenderTarget = _prtiRenderTarget;
		m_aDepthStencil.ui32Mipmap = 0;
		m_aDepthStencil.ui32ArraySlice = 0;
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
	LSBOOL LSE_CALL CFramebufferBase::Texture1DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		m_aColors[_ui32Slot].prtiRenderTarget = _prtiRenderTarget;
		m_aColors[_ui32Slot].ui32Mipmap = _ui32Level;
		m_aColors[_ui32Slot].ui32ArraySlice = 0;
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
	LSBOOL LSE_CALL CFramebufferBase::Texture2DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		m_aColors[_ui32Slot].prtiRenderTarget = _prtiRenderTarget;
		m_aColors[_ui32Slot].ui32Mipmap = _ui32Level;
		m_aColors[_ui32Slot].ui32ArraySlice = 0;
		return true;
	}

	/**
	 * Attaches a texture to the given color attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the given color slot.
	 * \param _ui32Level Mipmap level to attach.
	 * \param _ui32Layer 2D slice of the 3D texture to attach.
	 * \param _ui32Slot The slot to which to attach the texture.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::Texture3DColorAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Layer, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		m_aColors[_ui32Slot].prtiRenderTarget = _prtiRenderTarget;
		m_aColors[_ui32Slot].ui32Mipmap = _ui32Level;
		m_aColors[_ui32Slot].ui32ArraySlice = _ui32Layer;
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
	 * \param _ui32Level Mipmap level to attach.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::Texture1DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level ) {
		m_aDepthStencil.prtiRenderTarget = _prtiRenderTarget;
		m_aDepthStencil.ui32Mipmap = _ui32Level;
		m_aDepthStencil.ui32ArraySlice = 0;
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
	 * \param _ui32Level Mipmap level to attach.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::Texture2DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level ) {
		m_aDepthStencil.prtiRenderTarget = _prtiRenderTarget;
		m_aDepthStencil.ui32Mipmap = _ui32Level;
		m_aDepthStencil.ui32ArraySlice = 0;
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.
	 *
	 * \param _prtiRenderTarget Texture to attach to the depth-stencil.
	 * \param _ui32Level Mipmap level to attach.
	 * \param _ui32Layer 2D slice of the 3D texture to attach.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL CFramebufferBase::Texture3DDepthStencilAttach( CRenderTargetInterface * _prtiRenderTarget, uint32_t _ui32Level, uint32_t _ui32Layer ) {
		m_aDepthStencil.prtiRenderTarget = _prtiRenderTarget;
		m_aDepthStencil.ui32Mipmap = _ui32Level;
		m_aDepthStencil.ui32ArraySlice = _ui32Layer;
		return true;
	}

}	// namespace lsg
