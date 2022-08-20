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
 * Description: A 2D OpenGL ES texture.
 */


#ifndef __LSG_OPENGLESTEXTURE2D_H__
#define __LSG_OPENGLESTEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGLES

#include "../OpenGLES/LSGOpenGlEs.h"
#include "../Texture/LSGTexture2dBase.h"
#include "LSGOpenGlEsTextureConstants.h"

namespace lsg {

	/**
	 * Class COpenGlEsTexture2d
	 * \brief A 2D OpenGL ES texture.
	 *
	 * Description: A 2D OpenGL ES texture.
	 */
	class COpenGlEsTexture2d : public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlEsTexture2d();
		LSE_CALLCTOR							~COpenGlEsTexture2d();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();
		
		/**
		 * Gets the OpenGL ES texture ID.
		 *
		 * \return Returns the OpenGL ES texture ID.
		 */
		LSE_INLINE GLuint LSE_CALL				OpenGlEsId() const;

		/**
		 * Gets the OpenGL ES texture type.
		 *
		 * \return Returns the OpenGL ES texture type.
		 */
		LSE_INLINE GLenum LSE_CALL				OpenGlEsTextureType() const;


	protected :
		// == Members.
		/** ID of the texture as recognized by OpenGLES. */
		GLuint									m_uiTexture;


		// == Functions.
		/**
		 * Creates a OpenGLES texture and fills it with our texel data.  Mipmaps are generated if necessary.
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
		 * Gets a texel from a buffer given the row width, X, Y, format, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _pfFormat Format of the data.
		 * \param _pfReqFormat Format of the output data.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \return Returns a texel as an uint64_t value.
		 */
		static LSE_INLINE uint64_t LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
			LSI_PIXEL_FORMAT _pfReqFormat,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y );
			
		/**
		 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \param _f16Return Holds the returned values.
		 */
		static LSE_INLINE void LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y,
			CFloat16 * _pf16Return );

		/**
		 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \param _pfReturn Holds the returned values.
		 */
		static LSE_INLINE void LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y,
			float * _pfReturn );

	private :
		// == Types.
		typedef CTexture2dBase					Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the OpenGL ES texture ID.
	 *
	 * \return Returns the OpenGL ES texture ID.
	 */
	GLuint LSE_CALL COpenGlEsTexture2d::OpenGlEsId() const {
		return m_uiTexture;
	}

	/**
	 * Gets the OpenGL ES texture type.
	 *
	 * \return Returns the OpenGL ES texture type.
	 */
	GLenum LSE_CALL COpenGlEsTexture2d::OpenGlEsTextureType() const {
		return GL_TEXTURE_2D;
	}

	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSE_INLINE LSBOOL LSE_CALL COpenGlEsTexture2d::ActivateApi( uint32_t _ui32Slot ) {
		COpenGlEs::m_rsCurRenderState.uiMipLevels[_ui32Slot] = m_ui16MipMaps;
		return true;
	}
	
	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL COpenGlEsTexture2d::ShaderResourceApi() {
		if ( !m_uiTexture ) { return NULL; }
		return &m_uiTexture;
	}
	
	/**
	 * Gets a texel from a buffer given the row width, X, Y, format, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _pfFormat Format of the data.
	 * \param _pfReqFormat Format of the output data.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \return Returns a texel as an uint64_t value.
	 */
	LSE_INLINE uint64_t LSE_CALL COpenGlEsTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
		LSI_PIXEL_FORMAT _pfReqFormat,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y ) {
		return CImageLib::IntegerTexelToIntegerTexel( _pfFormat, _pfReqFormat, (*reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize])) );
	}

	/**
	 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \param _f16Return Holds the returned values.
	 */
	LSE_INLINE void LSE_CALL COpenGlEsTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y,
		CFloat16 * _pf16Return ) {
		// Can be copied via a single 64-bit bitwise copy.
		uint64_t * pui64Ret = reinterpret_cast<uint64_t *>(_pf16Return);
		(*pui64Ret) = (*reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]));
	}

	/**
	 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \param _pfReturn Holds the returned values.
	 */
	LSE_INLINE void LSE_CALL COpenGlEsTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y,
		float * _pfReturn ) {
		// Can be copied via 2 64-bit bitwise copies.
		uint64_t * pui64Ret = reinterpret_cast<uint64_t *>(_pfReturn);
		const uint64_t * pui64Src = reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]);
		(*pui64Ret++) = (*pui64Src++);
		(*pui64Ret) = (*pui64Src);
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES

#endif	// __LSG_OPENGLESTEXTURE2D_H__
