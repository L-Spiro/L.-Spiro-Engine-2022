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


#ifndef __LSG_OPENGLESFRAMEBUFFER_H__
#define __LSG_OPENGLESFRAMEBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGLES

#include "../OpenGLES/LSGOpenGlEs.h"

namespace lsg {

	/**
	 * Class COpenGlEsFramebuffer
	 * \brief An OpenGL ES framebuffer.
	 *
	 * Description: An OpenGL ES framebuffer.  Manages the lifetime of an OpenGL ES framebuffer ID and keeps the resolution
	 *	and set of attachments.  The resolution of the framebuffer should never be changed (instead, a new framebuffer
	 *	with the new size should be made).
	 */
	class COpenGlEsFramebuffer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlEsFramebuffer();
		LSE_CALLCTOR							~COpenGlEsFramebuffer();


		// == Functions.
		/**
		 * Create it with the given width and height.  It can be created any number of times, but
		 *	each time a new OpenGL ES framebuffer object will be generated (and the previous one deleted).
		 *
		 * \param _ui32Width Width of the framebuffer.
		 * \param _ui32Height Height of the framebuffer.
		 * \return Returns true if the framebuffer was generated.
		 */
		LSBOOL LSE_CALL							CreateFramebuffer( uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Resets the framebuffer to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets a constant reference to the OpenGL ES ID for this framebuffer.
		 *
		 * \return Returns a constant reference to the OpenGL ES ID for this framebuffer.
		 */
		LSE_INLINE const GLuint & LSE_CALL		Id() const;

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
		 * Binds the framebuffer.  Must be bound before anything can be attached.
		 *
		 * \return Returns true if the framebuffer was bound.
		 */
		LSBOOL LSE_CALL							Bind() const;

		/**
		 * Attaches a renderbuffer to the given color attachment by index.  Framebuffer must be bound.
		 *
		 * \param _uiRenderbuffer Renderbuffer to attach to the given color slot.
		 * \param _ui32Slot The slot to which to attach the renderbuffer.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							RenderbufferColorAttach( GLuint _uiRenderbuffer, uint32_t _ui32Slot );

		/**
		 * Attaches a renderbuffer to the given depth-stencil attachment.  Framebuffer must be bound.
		 *
		 * \param _uiRenderbuffer Renderbuffer to attach to the depth-stencil.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							RenderbufferDepthAttach( GLuint _uiRenderbuffer );
		
		/**
		 * Attaches a renderbuffer to the given stencil attachment.  Framebuffer must be bound.
		 *
		 * \param _uiRenderbuffer Renderbuffer to attach to the stencil.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							RenderbufferStencilAttach( GLuint _uiRenderbuffer );

		/**
		 * Attaches a texture to the given color attachment by index.  Framebuffer must be bound.
		 *
		 * \param _uiTexture Texture to attach to the given color slot.
		 * \param _iLevel Mipmap level to attach.
		 * \param _ui32Slot The slot to which to attach the texture.
		 * \param _eTexTarget The type of texture being attached.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DColorAttach( GLuint _uiTexture, GLint _iLevel, uint32_t _ui32Slot, GLenum _eTexTarget = GL_TEXTURE_2D );

		/**
		 * Attaches a texture to the given depth attachment by index.  Framebuffer must be bound.
		 *
		 * \param _uiTexture Texture to attach to the depth.
		 * \param _iLevel Mipmap level to attach.
		 * \param _eTexTarget The type of texture being attached.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DDepthAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget = GL_TEXTURE_2D );
		
		/**
		 * Attaches a texture to the given stencil attachment by index.  Framebuffer must be bound.
		 *
		 * \param _uiTexture Texture to attach to the stencil.
		 * \param _iLevel Mipmap level to attach.
		 * \param _eTexTarget The type of texture being attached.
		 * \return Returns true if the binding was successful.
		 */
		LSBOOL LSE_CALL							Texture2DStencilAttach( GLuint _uiTexture, GLint _iLevel, GLenum _eTexTarget = GL_TEXTURE_2D );

		/**
		 * Signals that a texture has been deleted.
		 *
		 * \param _uiId The OpenGL ES texture ID.
		 */
		void LSE_CALL							TextureDeleted( GLuint _uiId );

		/**
		 * Signals that a renderbuffer has been deleted.
		 *
		 * \param _uiId The OpenGL ES renderbuffer ID.
		 */
		void LSE_CALL							RenderbufferDeleted( GLuint _uiId );


	protected :
		// == Types.
		/** Attachment data for each possible slot. */
		typedef struct LSG_FRAMEBUFFER_ATTACHMENT {
			GLenum								eTexTarget;					/**< The attachment type (GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_2D, GL_RENDERBUFFER, etc.) */
			GLuint								uiTexture;					/**< OpenGL ES texture ID. */
			GLint								iLevel;						/**< Level of the texture.  0 if renderbuffer. */
		} * LPLSG_FRAMEBUFFER_ATTACHMENT, * const LPCLSG_FRAMEBUFFER_ATTACHMENT;


		// == Members.
		/** Active color attachments. */
		LSG_FRAMEBUFFER_ATTACHMENT				m_faColorAttachments[LSG_MAX_RENDER_TARGET_COUNT];

		/** Active depth attachment. */
		LSG_FRAMEBUFFER_ATTACHMENT				m_faDepth;
		
		/** Active stencil attachment. */
		LSG_FRAMEBUFFER_ATTACHMENT				m_faStencil;

		/** Width of the framebuffer.  Set when the framebuffer is applied. */
		uint32_t								m_ui32Width;

		/** Height of the framebuffer.  Set when the framebuffer is applied. */
		uint32_t								m_ui32Height;

		/** Framebuffer ID. */
		GLuint									m_uiId;


		// == Functions.
		/**
		 * Checks that the framebuffer is bound.  Not thread-safe, so only call after locking COpenGl::m_csFramebufferCrit.
		 *
		 * \return Returns true if bound.
		 */
		LSE_INLINE LSBOOL LSE_CALL				IsBound() const;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a constant reference to the OpenGL ES ID for this framebuffer.
	 *
	 * \return Returns a constant reference to the OpenGL ES ID for this framebuffer.
	 */
	LSE_INLINE const GLuint & LSE_CALL COpenGlEsFramebuffer::Id() const {
		return m_uiId;
	}

	/**
	 * Gets the width of the framebuffer.
	 *
	 * \return Returns the width of the framebuffer.
	 */
	LSE_INLINE uint32_t LSE_CALL COpenGlEsFramebuffer::Width() const {
		return m_ui32Width;
	}

	/**
	 * Gets the height of the framebuffer.
	 *
	 * \return Returns the height of the framebuffer.
	 */
	LSE_INLINE uint32_t LSE_CALL COpenGlEsFramebuffer::Height() const {
		return m_ui32Height;
	}

	/**
	 * Checks that the framebuffer is bound.  Not thread-safe, so only call after locking COpenGl::m_csFramebufferCrit.
	 *
	 * \return Returns true if bound.
	 */
	LSE_INLINE LSBOOL LSE_CALL COpenGlEsFramebuffer::IsBound() const {
		return m_uiId && COpenGlEs::m_rsCurRenderState.uiFramebuffer == m_uiId;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES

#endif	// __LSG_OPENGLESFRAMEBUFFER_H__
