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
 * Description: An OpenGL framebuffer.  Manages the lifetime of an OpenGL framebuffer ID and keeps the resolution
 *	and set of attachments.  The resolution of the framebuffer should never be changed (instead, a new framebuffer
 *	with the new size should be made).
 */

#include "LSGOpenGlFramebuffer.h"

#ifdef LSG_OGL

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlFramebuffer::COpenGlFramebuffer() :
		m_uiId( 0 ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ) {

		CStd::MemSet( m_faColorAttachments, 0, sizeof( m_faColorAttachments ) );
		CStd::MemSet( &m_faDepthStencil, 0, sizeof( m_faDepthStencil ) );
	}
	LSE_CALLCTOR COpenGlFramebuffer::~COpenGlFramebuffer() {
		Reset();
	}

	// == Functions.
	/**
	 * Create it with the given width and height.  It can be created any number of times, but
	 *	each time a new OpenGL framebuffer object will be generated (and the previous one deleted).
	 *
	 * \param _ui32Width Width of the framebuffer.
	 * \param _ui32Height Height of the framebuffer.
	 * \return Returns true if the framebuffer was generated.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::CreateFramebuffer( uint32_t _ui32Width, uint32_t _ui32Height ) {
		Reset();

		if ( !COpenGl::glGenFramebuffers ) { return false; }
		COpenGl::glGenFramebuffers( 1, &m_uiId );
		if ( !m_uiId ) { return false; }
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		return true;
	}

	/**
	 * Resets the framebuffer to scratch.
	 */
	void LSE_CALL COpenGlFramebuffer::Reset() {
		if ( m_uiId && COpenGl::glDeleteFramebuffers ) {
			CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
			// CPU-side unbind.
			if ( COpenGl::m_rsCurRenderState.uiFramebuffer == m_uiId ) {
				COpenGl::m_rsCurRenderState.uiFramebuffer = 0;
			}

			COpenGl::glDeleteFramebuffers( 1, &m_uiId );
			m_uiId = 0;
		}
		CStd::MemSet( m_faColorAttachments, 0, sizeof( m_faColorAttachments ) );
		CStd::MemSet( &m_faDepthStencil, 0, sizeof( m_faDepthStencil ) );
	}

	/**
	 * Binds the framebuffer.  Must be bound before anything can be attached.
	 *
	 * \return Returns true if the framebuffer was bound.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::Bind() const {
		if ( !m_uiId ) { return false; }

		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( IsBound() ) { return true; }
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGl::BindFramebuffer( m_uiId );
		if ( glWarnErrorAlways( "Invalid framebuffer ID" ) ) {
			// Wrong OpenGL context?
			return false;
		}
		return true;
	}

	/**
	 * Attaches a renderbuffer to the given color attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiRenderbuffer Renderbuffer to attach to the given color slot.
	 * \param _ui32Slot The slot to which to attach the renderbuffer.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::RenderbufferColorAttach( GLuint _uiRenderbuffer, uint32_t _ui32Slot ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faColorAttachments[_ui32Slot].eTexTarget != GL_RENDERBUFFER ||
			m_faColorAttachments[_ui32Slot].uiTexture != _uiRenderbuffer ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferRenderbuffer( GL_FRAMEBUFFER,
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
	LSBOOL LSE_CALL COpenGlFramebuffer::RenderbufferDepthStencilAttach( GLuint _uiRenderbuffer ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faDepthStencil.eTexTarget != GL_RENDERBUFFER ||
			m_faDepthStencil.uiTexture != _uiRenderbuffer ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferRenderbuffer( GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				GL_RENDERBUFFER,
				_uiRenderbuffer );
			if ( glWarnErrorAlways( "RenderbufferDepthStencilAttach failed" ) ) { return false; }
			m_faDepthStencil.eTexTarget = GL_RENDERBUFFER;
			m_faDepthStencil.uiTexture = _uiRenderbuffer;
			m_faDepthStencil.iLevel = 0;
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
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture1DColorAttach( GLuint _uiTexture, GLint _iLevel, uint32_t _ui32Slot, GLenum _eTexTarget ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faColorAttachments[_ui32Slot].eTexTarget != _eTexTarget ||
			m_faColorAttachments[_ui32Slot].uiTexture != _uiTexture ||
			m_faColorAttachments[_ui32Slot].iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferTexture1D( GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0 + _ui32Slot,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture1DColorAttach failed" ) ) { return false; }
			m_faColorAttachments[_ui32Slot].eTexTarget = _eTexTarget;
			m_faColorAttachments[_ui32Slot].uiTexture = _uiTexture;
			m_faColorAttachments[_ui32Slot].iLevel = _iLevel;
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
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture2DColorAttach( GLuint _uiTexture, GLint _iLevel, uint32_t _ui32Slot, GLenum _eTexTarget ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faColorAttachments[_ui32Slot].eTexTarget != _eTexTarget ||
			m_faColorAttachments[_ui32Slot].uiTexture != _uiTexture ||
			m_faColorAttachments[_ui32Slot].iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferTexture2D( GL_FRAMEBUFFER,
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
	 * Attaches a texture to the given color attachment by index.  Framebuffer must be bound.
	 *
	 * \param _eTexTarget The type of texture being attached.
	 * \param _uiTexture Texture to attach to the given color slot.
	 * \param _iLevel Mipmap level to attach.
	 * \param _iLayer 2D slice of the 3D texture to attach.
	 * \param _ui32Slot The slot to which to attach the texture.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture3DColorAttach( GLuint _uiTexture, GLint _iLevel, GLint _iLayer, uint32_t _ui32Slot, GLenum _eTexTarget ) {
		assert( _ui32Slot < CGfx::GetMetrics().ui32MaxRenderTargets );
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

	
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGl::glFramebufferTexture3D( GL_FRAMEBUFFER,
			GL_COLOR_ATTACHMENT0 + _ui32Slot,
			_eTexTarget,
			_uiTexture,
			_iLevel,
			_iLayer );
		if ( glWarnErrorAlways( "Texture3DColorAttach failed" ) ) { return false; }
		m_faColorAttachments[_ui32Slot].eTexTarget = _eTexTarget;
		m_faColorAttachments[_ui32Slot].uiTexture = _uiTexture;
		m_faColorAttachments[_ui32Slot].iLevel = _iLevel;
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiTexture Texture to attach to the depth-stencil.
	 * \param _iLevel Mipmap level to attach.
	 * \param _eTexTarget The type of texture being attached.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture1DDepthStencilAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faDepthStencil.eTexTarget != _eTexTarget ||
			m_faDepthStencil.uiTexture != _uiTexture ||
			m_faDepthStencil.iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferTexture1D( GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture1DDepthStencilAttach failed" ) ) { return false; }
			m_faDepthStencil.eTexTarget = _eTexTarget;
			m_faDepthStencil.uiTexture = _uiTexture;
			m_faDepthStencil.iLevel = _iLevel;
		}
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiTexture Texture to attach to the depth-stencil.
	 * \param _iLevel Mipmap level to attach.
	 * \param _eTexTarget The type of texture being attached.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture2DDepthStencilAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

		if ( m_faDepthStencil.eTexTarget != _eTexTarget ||
			m_faDepthStencil.uiTexture != _uiTexture ||
			m_faDepthStencil.iLevel != _iLevel ) {
			// Not the same attachment.
			glWarnErrorAlways( "Uncaught" );	// Clear error flag.
			COpenGl::glFramebufferTexture2D( GL_FRAMEBUFFER,
				GL_DEPTH_STENCIL_ATTACHMENT,
				_eTexTarget,
				_uiTexture,
				_iLevel );
			if ( glWarnErrorAlways( "Texture2DDepthStencilAttach failed" ) ) { return false; }
			m_faDepthStencil.eTexTarget = _eTexTarget;
			m_faDepthStencil.uiTexture = _uiTexture;
			m_faDepthStencil.iLevel = _iLevel;
		}
		return true;
	}

	/**
	 * Attaches a texture to the given depth-stencil attachment by index.  Framebuffer must be bound.
	 *
	 * \param _uiTexture Texture to attach to the depth-stencil.
	 * \param _iLevel Mipmap level to attach.
	 * \param _iLayer 2D slice of the 3D texture to attach.
	 * \param _eTexTarget The type of texture being attached.
	 * \return Returns true if the binding was successful.
	 */
	LSBOOL LSE_CALL COpenGlFramebuffer::Texture3DDepthStencilAttach( GLuint _uiTexture, GLint _iLevel, GLint _iLayer, GLenum _eTexTarget ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( !IsBound() ) { return false; }

	
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGl::glFramebufferTexture3D( GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			_eTexTarget,
			_uiTexture,
			_iLevel,
			_iLayer );
		if ( glWarnErrorAlways( "Texture3DDepthStencilAttach failed" ) ) { return false; }
		m_faDepthStencil.eTexTarget = _eTexTarget;
		m_faDepthStencil.uiTexture = _uiTexture;
		m_faDepthStencil.iLevel = _iLevel;
		return true;
	}

	/**
	 * Signals that a texture has been deleted.
	 *
	 * \param _uiId The OpenGL texture ID.
	 */
	void LSE_CALL COpenGlFramebuffer::TextureDeleted( GLuint _uiId ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( m_faDepthStencil.eTexTarget != GL_RENDERBUFFER && m_faDepthStencil.uiTexture == _uiId ) {
			m_faDepthStencil.uiTexture = m_faDepthStencil.iLevel = m_faDepthStencil.eTexTarget = 0;
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
	 * \param _uiId The OpenGL renderbuffer ID.
	 */
	void LSE_CALL COpenGlFramebuffer::RenderbufferDeleted( GLuint _uiId ) {
		CCriticalSection::CLocker lLock( COpenGl::m_csFramebufferCrit );
		if ( m_faDepthStencil.eTexTarget == GL_RENDERBUFFER && m_faDepthStencil.uiTexture == _uiId ) {
			m_faDepthStencil.uiTexture = m_faDepthStencil.iLevel = m_faDepthStencil.eTexTarget = 0;
		}
		for ( uint32_t I = CGfx::GetMetrics().ui32MaxRenderTargets; I--; ) {
			if ( m_faColorAttachments[I].eTexTarget == GL_RENDERBUFFER && m_faColorAttachments[I].uiTexture == _uiId ) {
				m_faColorAttachments[I].uiTexture = m_faColorAttachments[I].iLevel = m_faColorAttachments[I].eTexTarget = 0;
			}
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGL
