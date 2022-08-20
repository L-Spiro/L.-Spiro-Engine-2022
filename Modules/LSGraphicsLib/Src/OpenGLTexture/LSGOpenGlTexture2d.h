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
 * Description: A 2D OpenGL texture.
 */


#ifndef __LSG_OPENGLTEXTURE2D_H__
#define __LSG_OPENGLTEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGL

#include "../OpenGL/LSGOpenGl.h"
#include "../Texture/LSGTexture2dBase.h"
#include "LSGOpenGlTextureConstants.h"

namespace lsg {

	/**
	 * Class COpenGlTexture2d
	 * \brief A 2D OpenGL texture.
	 *
	 * Description: A 2D OpenGL texture.
	 */
	class COpenGlTexture2d : public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlTexture2d();
		LSE_CALLCTOR							~COpenGlTexture2d();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets the OpenGL texture ID.
		 *
		 * \return Returns the OpenGL texture ID.
		 */
		LSE_INLINE GLuint LSE_CALL				OpenGlId() const;

		/**
		 * Gets the OpenGL texture type.
		 *
		 * \return Returns the OpenGL texture type.
		 */
		LSE_INLINE GLenum LSE_CALL				OpenGlTextureType() const;


	protected :
		// == Members.
		/** ID of the texture as recognized by OpenGL. */
		GLuint									m_uiTexture;

		/** Total textures in the array. */
		GLsizei									m_sArrayDepth;

		/** Texture type. */
		GLenum									m_eTextureType;

		/** Internal format (KTX 1 files only). */
		GLenum									m_eInternalFormatKtx1;

		/** Base internal format (KTX 1 files only). */
		GLenum									m_eBaseInternalFormatKtx1;

		/** Format (KTX 1 files only). */
		GLenum									m_eFormatKtx1;

		/** Type (KTX 1 files only). */
		GLenum									m_eTypeKtx1;


		// == Functions.
		/**
		 * Creates a OpenGL texture and fills it with our texel data.  Mipmaps are generated if necessary.
		 *
		 * \param _piImages Array of images to use to create the texture.
		 * \param _ui32Total Number of images to which _piImages points.
		 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
		 */
		LSBOOL LSE_CALL							CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total );

		/**
		 * Activates the texture in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
		 * \return Returns true if the texture was activated in the given slot.
		 */
		LSE_INLINE LSBOOL LSE_CALL				ActivateApi( uint32_t _ui32Slot );

		/**
		 * Gets this object's LSG_SHADER_RESOURCE ID.
		 *
		 * \return Returns this object's LSG_SHADER_RESOURCE ID.
		 */
		LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL
												ShaderResourceApi();

		/**
		 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *	It acts on the currently bound texture.
		 *
		 * \param _piImages The source image(s).
		 * \param _ui32Total Total images to which _piImages points.
		 * \return Returns true if there were no failures.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( const CImage * _piImages, uint32_t _ui32Total );

		/**
		 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *	It acts on the currently bound texture.
		 *
		 * \param _iMipLevel The surface level to which to copy our texel data.
		 * \param _tbTexels The texel data to copy.
		 * \param _pfFormat Format of the data to copy.
		 * \param _ui32Width Width of the image data.
		 * \param _ui32Height Height of the image data.
		 * \param _bsRgb Specifies if the image is sRGB or not.
		 * \return Returns true if there were no failures.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( GLint _iMipLevel,
			const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height, LSBOOL _bsRgb );

		/**
		 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *	It acts on the currently bound texture.
		 *
		 * \param _iMipLevel The surface level to which to copy our texel data.
		 * \param _iLayer The layer to which to copy our texel data.
		 * \param _sTotalLayers The total number of layers in the image.
		 * \param _tbTexels The texel data to copy.
		 * \param _pfFormat Format of the data to copy.
		 * \param _ui32Width Width of the image data.
		 * \param _ui32Height Height of the image data.
		 * \return Returns true if there were no failures.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( GLint _iMipLevel, GLint _iLayer, GLsizei _sTotalLayers,
			const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height );


	private :
		// == Types.
		typedef CTexture2dBase					Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the OpenGL texture ID.
	 *
	 * \return Returns the OpenGL texture ID.
	 */
	LSE_INLINE GLuint LSE_CALL COpenGlTexture2d::OpenGlId() const {
		return m_uiTexture;
	}

	/**
	 * Gets the OpenGL texture type.
	 *
	 * \return Returns the OpenGL texture type.
	 */
	LSE_INLINE GLenum LSE_CALL COpenGlTexture2d::OpenGlTextureType() const {
		return m_eTextureType;
	}

	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSE_INLINE LSBOOL LSE_CALL COpenGlTexture2d::ActivateApi( uint32_t _ui32Slot ) {
#ifdef LSA_TOOLS
		if ( m_uiTexture ) {
			assert( ::glIsTexture( m_uiTexture ) );
		}
#endif	// #ifdef LSA_TOOLS
		COpenGl::m_rsCurRenderState.uiMipLevels[_ui32Slot] = m_ui16MipMaps;
		return true;
	}

	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL COpenGlTexture2d::ShaderResourceApi() {
		if ( !m_uiTexture ) { return NULL; }
		return &m_uiTexture;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGL

#endif	// __LSG_OPENGLTEXTURE2D_H__
