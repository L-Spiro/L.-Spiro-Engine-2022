/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An OpenGL ES framebuffer.  Manages the lifetime of an OpenGL ES framebuffer ID and keeps the resolution
 *	and set of attachments.  The resolution of the framebuffer should never be changed (instead, a new framebuffer
 *	with the new size should be made).
 */

#include "LSGOpenGlEsFramebuffer.h"

#ifdef LSG_OGLES

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsFramebuffer::COpenGlEsFramebuffer() :
		m_uiId( 0 ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ) {

		CStd::MemSet( m_faColorAttachments, 0, sizeof( m_faColorAttachments ) );
		CStd::MemSet( &m_faDepth, 0, sizeof( m_faDepth ) );
		CStd::MemSet( &m_faStencil, 0, sizeof( m_faStencil ) );
	}
	LSE_CALLCTOR COpenGlEsFramebuffer::~COpenGlEsFramebuffer() {
		Reset();
	}

	// == Functions.
	/**
	 * Create it with the given width and height.  It can be created any number of times, but
	 *	each time a new OpenGL ES framebuffer object will be generated (and the previous one deleted).
	 *
	 * \param _ui32Width Width of the framebuffer.
	 * \param _ui32Height Height of the framebuffer.
	 * \return Returns true if the framebuffer was generated.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::CreateFramebuffer( uint32_t _ui32Width, uint32_t _ui32Height ) {
		Reset();

		::glGenFramebuffers( 1, &m_uiId );
		if ( !m_uiId ) { return false; }
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		return true;
	}

	/**
	 * Resets the framebuffer to scratch.
	 */
	void LSE_CALL COpenGlEsFramebuffer::Reset() {
		if ( m_uiId ) {
			CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
			// CPU-side unbind.
			if ( COpenGlEs::m_rsCurRenderState.uiFramebuffer == m_uiId ) {
				COpenGlEs::m_rsCurRenderState.uiFramebuffer = 0;
			}

			::glDeleteFramebuffers( 1, &m_uiId );
			m_uiId = 0;
		}
		CStd::MemSet( m_faColorAttachments, 0, sizeof( m_faColorAttachments ) );
		CStd::MemSet( &m_faDepth, 0, sizeof( m_faDepth ) );
		CStd::MemSet( &m_faStencil, 0, sizeof( m_faStencil ) );
	}

	/**
	 * Binds the framebuffer.  Must be bound before anything can be attached.
	 *
	 * \return Returns true if the framebuffer was bound.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::Bind() const {
		if ( !m_uiId ) { return false; }

		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( IsBound() ) { return true; }
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGlEs::BindFramebuffer( m_uiId );
		if ( glWarnErrorAlways( "Invalid framebuffer ID" ) ) {
			// Wrong OpenGL ES context?
			return false;
		}
		// CPU-side bind.
		COpenGlEs::m_rsCurRenderState.uiFramebuffer = m_uiId;
		return true;
	}

	/**
	 * Attaches a renderbuffer to the given color attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiRenderbuffer Renderbuffer to attach to the given color slot.
	 * \param _ui32Slot The slot to which to attach the renderbuffer.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::RenderbufferColorAttach( GLuint _uiRenderbuffer, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faColorAttachments[_ui32Slot].eTexTarget != GL_RENDERBUFFER ||
			m_faColorAttachments[_ui32Slot].uiTexture != _uiRenderbuffer ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferRenderbuffer( GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + _ui32Slot,
				GL_RENDERBUFFER,
				_uiRenderbuffer );
			if ( glWarnErrorAlways( "RenderbufferColorAttach failed" ) ) { return false; }
			m_faColorAttachments[_ui32Slot].eTexTarget = GL_RENDERBUFFER;
			m_faColorAttachments[_ui32Slot].uiTexture = _uiRenderbuffer;
			m_faColorAttachments[_ui32Slot].iLevel = 0;
		}
		return true;
	}

	/**
	 * Attaches a renderbuffer to the given depth-stencil attachment.  Framebuffer must be bound.
	 *
	 * \param _uiRenderbuffer Renderbuffer to attach to the depth-stencil.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::RenderbufferDepthAttach( GLuint _uiRenderbuffer ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faDepth.eTexTarget != GL_RENDERBUFFER ||
			m_faDepth.uiTexture != _uiRenderbuffer ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferRenderbuffer( GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER,
				_uiRenderbuffer );
			if ( glWarnErrorAlways( "RenderbufferDepthAttach failed" ) ) { return false; }
			m_faDepth.eTexTarget = GL_RENDERBUFFER;
			m_faDepth.uiTexture = _uiRenderbuffer;
			m_faDepth.iLevel = 0;
		}
		return true;
	}
	
	/**
	 * Attaches a renderbuffer to the given stencil attachment.  Framebuffer must be bound.
	 *
	 * \param _uiRenderbuffer Renderbuffer to attach to the stencil.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::RenderbufferStencilAttach( GLuint _uiRenderbuffer ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faStencil.eTexTarget != GL_RENDERBUFFER ||
			m_faStencil.uiTexture != _uiRenderbuffer ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferRenderbuffer( GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_OES,
				GL_RENDERBUFFER,
				_uiRenderbuffer );
			if ( glWarnErrorAlways( "RenderbufferStencilAttach failed" ) ) { return false; }
			m_faStencil.eTexTarget = GL_RENDERBUFFER;
			m_faStencil.uiTexture = _uiRenderbuffer;
			m_faStencil.iLevel = 0;
		}
		return true;
	}

	/**
	 * Attaches a texture to the given color attachment by index.  Framebuffer must be bound.
	 *
	 * \param _eTexTarget The type of texture being attached.
	 * \param _uiTexture Texture to attach to the given color slot.
	 * \param _iLevel Mipmap level to attach.
	 * \param _ui32Slot The slot to which to attach the texture.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::Texture2DColorAttach( GLuint _uiTexture, GLint _iLevel, uint32_t _ui32Slot, GLenum _eTexTarget ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faColorAttachments[_ui32Slot].eTexTarget != _eTexTarget ||
			m_faColorAttachments[_ui32Slot].uiTexture != _uiTexture ||
			m_faColorAttachments[_ui32Slot].iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferTexture2D( GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + _ui32Slot,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture2DColorAttach failed" ) ) { return false; }
			m_faColorAttachments[_ui32Slot].eTexTarget = _eTexTarget;
			m_faColorAttachments[_ui32Slot].uiTexture = _uiTexture;
			m_faColorAttachments[_ui32Slot].iLevel = _iLevel;
		}
		return true;
	}

	/**
	 * Attaches a texture to the given depth attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiTexture Texture to attach to the depth.
	 * \param _iLevel Mipmap level to attach.
	 * \param _eTexTarget The type of texture being attached.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::Texture2DDepthAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faDepth.eTexTarget != _eTexTarget ||
			m_faDepth.uiTexture != _uiTexture ||
			m_faDepth.iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferTexture2D( GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture2DDepthAttach failed" ) ) { return false; }
			m_faDepth.eTexTarget = _eTexTarget;
			m_faDepth.uiTexture = _uiTexture;
			m_faDepth.iLevel = _iLevel;
		}
		return true;
	}
	
	/**
	 * Attaches a texture to the given stencil attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiTexture Texture to attach to the stencil.
	 * \param _iLevel Mipmap level to attach.
	 * \param _eTexTarget The type of texture being attached.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlEsFramebuffer::Texture2DStencilAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faStencil.eTexTarget != _eTexTarget ||
			m_faStencil.uiTexture != _uiTexture ||
			m_faStencil.iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			::glFramebufferTexture2D( GL_FRAMEBUFFER,
				GL_STENCIL_ATTACHMENT,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture2DStencilAttach failed" ) ) { return false; }
			m_faStencil.eTexTarget = _eTexTarget;
			m_faStencil.uiTexture = _uiTexture;
			m_faStencil.iLevel = _iLevel;
		}
		return true;
	}

	/**
	 * Signals that a texture has been deleted.
	 *
	 * \param _uiId The OpenGL ES texture ID.
	 */
	void LSE_CALL COpenGlEsFramebuffer::TextureDeleted( GLuint _uiId ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( m_faStencil.eTexTarget != GL_RENDERBUFFER && m_faStencil.uiTexture == _uiId ) {
			m_faStencil.uiTexture = m_faStencil.iLevel = m_faStencil.eTexTarget = 0;
		}
		if ( m_faDepth.eTexTarget != GL_RENDERBUFFER && m_faDepth.uiTexture == _uiId ) {
			m_faDepth.uiTexture = m_faDepth.iLevel = m_faDepth.eTexTarget = 0;
		}
		for ( uint32_t I = CGfx::GetMetrics().ui32MaxRenderTargets; I--; ) {
			if ( m_faColorAttachments[I].eTexTarget != GL_RENDERBUFFER && m_faColorAttachments[I].uiTexture == _uiId ) {
				m_faColorAttachments[I].uiTexture = m_faColorAttachments[I].iLevel = m_faColorAttachments[I].eTexTarget = 0;
			}
		}
	}

	/**
	 * Signals that a renderbuffer has been deleted.
	 *
	 * \param _uiId The OpenGL ES renderbuffer ID.
	 */
	void LSE_CALL COpenGlEsFramebuffer::RenderbufferDeleted( GLuint _uiId ) {
		CCriticalSection::CLocker lLock( COpenGlEs::m_csFramebufferCrit );
		if ( m_faStencil.eTexTarget == GL_RENDERBUFFER && m_faStencil.uiTexture == _uiId ) {
			m_faStencil.uiTexture = m_faStencil.iLevel = m_faStencil.eTexTarget = 0;
		}
		if ( m_faDepth.eTexTarget == GL_RENDERBUFFER && m_faDepth.uiTexture == _uiId ) {
			m_faDepth.uiTexture = m_faDepth.iLevel = m_faDepth.eTexTarget = 0;
		}
		for ( uint32_t I = CGfx::GetMetrics().ui32MaxRenderTargets; I--; ) {
			if ( m_faColorAttachments[I].eTexTarget == GL_RENDERBUFFER && m_faColorAttachments[I].uiTexture == _uiId ) {
				m_faColorAttachments[I].uiTexture = m_faColorAttachments[I].iLevel = m_faColorAttachments[I].eTexTarget = 0;
			}
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
