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
 * Description: OpenGL texture constants.
 */


#ifndef __LSG_OPENGLTEXTURECONSTANTS_H__
#define __LSG_OPENGLTEXTURECONSTANTS_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGL

#include "../OpenGL/LSGOpenGl.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Texture usage flags. */
	enum LSG_TEXTURE_USAGE_TYPES {
		LSG_TUT_SETONLY							= (1 << 0),										/**< Texture data is never read or written to. */
		LSG_TUT_STANDARD						= (1 << 1),										/**< Texture data is not read or written to often by the CPU. */
		LSG_TUT_DYNAMIC							= (1 << 2),										/**< Texture data is updated frequently. */
		LSG_TUT_RENDERTARGET					= (1 << 3),										/**< The resource will be a render target. */

		LSG_TUT_FORCE_DWORD						= 0x7FFFFFFF
	};

	class COpenGlTextureConstants {
	public :
		// == Functions.
		/**
		 * Given an image format, return the OpenGL internal format for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \param _bsRgb If true, an sRGB format is selected, if possible.
		 * \return Returns the OpenGL internal format to use with the given image-library format.
		 */
		static LSE_INLINE GLint LSE_CALL		GetOpenGlInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb = false );

		/**
		 * Given an image format, return the OpenGL format for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \return Returns the OpenGL format to use with the given image-library format.
		 */
		static LSE_INLINE GLenum LSE_CALL		GetOpenGlFormatStandard( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Given an image format, return the OpenGL type for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \return Returns the OpenGL type to use with the given image-library format.
		 */
		static LSE_INLINE GLenum LSE_CALL		GetOpenGlTypeStandard( LSI_PIXEL_FORMAT _pfFormat );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Given an image format, return the OpenGL internal format for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \param _bsRgb If true, an sRGB format is selected, if possible.
	 * \return Returns the OpenGL internal format to use with the given image-library format.
	 */
	LSE_INLINE GLint LSE_CALL COpenGlTextureConstants::GetOpenGlInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb ) {
		if ( _bsRgb ) {
			if ( _pfFormat >= LSI_PF_DXT1 && _pfFormat <= LSI_PF_BC7_SRGB ) {
				static const GLint iInternalFormatssRgb[] = {
					GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,			// LSI_PF_DXT1
					GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
					GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,			// LSI_PF_DXT3
					GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
					GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,			// LSI_PF_DXT5
					0,												// LSI_PF_BC4/LSI_PF_BC4U
					0,												// LSI_PF_BC4S
					0,												// LSI_PF_BC5/LSI_PF_BC5U
					0,												// LSI_PF_BC5S
					0,												// LSI_PF_BC6/LSI_PF_BC6U
					0,												// LSI_PF_BC6S
					0,												// LSI_PF_BC7/LSI_PF_BC7U
					GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,			// LSI_PF_BC7_SRGB
				};

				return iInternalFormatssRgb[_pfFormat-LSI_PF_DXT1];
			}
			static const GLint iFormatssRgb[] = {
				GL_ALPHA,				// LSI_PF_A8
				0,						// LSI_PF_R3G3B2
				0,						// LSI_PF_R5G6B5
				0,						// LSI_PF_R4G4B4A4
				0,						// LSI_PF_R5G5B5A1
				GL_SRGB8,				// LSI_PF_R8G8B8
				GL_SRGB8_ALPHA8,		// LSI_PF_R8G8B8A8
				0,						// LSI_PF_R16G16B16A16
				0,						// LSI_PF_R16G16B16A16F
				0,						// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormatssRgb[_pfFormat]; }
		}
		else {
			if ( _pfFormat >= LSI_PF_DXT1 && _pfFormat <= LSI_PF_BC7_SRGB ) {
				static const GLint iInternalFormats[] = {
					GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,				// LSI_PF_DXT1
					GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
					GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,				// LSI_PF_DXT3
					GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
					GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,				// LSI_PF_DXT5
					GL_COMPRESSED_RED_RGTC1,						// LSI_PF_BC4/LSI_PF_BC4U
					GL_COMPRESSED_SIGNED_RED_RGTC1,					// LSI_PF_BC4S
					GL_COMPRESSED_RG_RGTC2,							// LSI_PF_BC5/LSI_PF_BC5U
					GL_COMPRESSED_SIGNED_RG_RGTC2,					// LSI_PF_BC5S
					GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,			// LSI_PF_BC6/LSI_PF_BC6U
					GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,			// LSI_PF_BC6S
					GL_COMPRESSED_RGBA_BPTC_UNORM,					// LSI_PF_BC7/LSI_PF_BC7U
					0,												// LSI_PF_BC7_SRGB
				};
				return iInternalFormats[_pfFormat-LSI_PF_DXT1];
			}
			static const GLint iFormats[] = {
				GL_ALPHA,				// LSI_PF_A8
				GL_RGB,					// LSI_PF_R3G3B2
				GL_RGB,					// LSI_PF_R5G6B5
				GL_RGBA,				// LSI_PF_R4G4B4A4
				GL_RGBA,				// LSI_PF_R5G5B5A1
				GL_RGB,					// LSI_PF_R8G8B8
				GL_RGBA,				// LSI_PF_R8G8B8A8
				GL_RGBA16,				// LSI_PF_R16G16B16A16
				GL_RGBA16F,				// LSI_PF_R16G16B16A16F
				GL_RGBA32F,				// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		}
		return GL_NONE;
	}

	/**
	 * Given an image format, return the OpenGL format for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \return Returns the OpenGL format to use with the given image-library format.
	 */
	LSE_INLINE GLenum LSE_CALL COpenGlTextureConstants::GetOpenGlFormatStandard( LSI_PIXEL_FORMAT _pfFormat ) {
		static const GLint iFormats[] = {
			GL_ALPHA,				// LSI_PF_A8
			GL_RGB,					// LSI_PF_R3G3B2
			GL_RGB,					// LSI_PF_R5G6B5
			GL_RGBA,				// LSI_PF_R4G4B4A4
			GL_RGBA,				// LSI_PF_R5G5B5A1
			GL_BGR,					// LSI_PF_R8G8B8
			GL_RGBA,				// LSI_PF_R8G8B8A8
			GL_RGBA,				// LSI_PF_R16G16B16A16
			GL_RGBA,				// LSI_PF_R16G16B16A16F
			GL_RGBA,				// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		return GL_NONE;
	}

	/**
	 * Given an image format, return the OpenGL type for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \return Returns the OpenGL type to use with the given image-library format.
	 */
	LSE_INLINE GLenum LSE_CALL COpenGlTextureConstants::GetOpenGlTypeStandard( LSI_PIXEL_FORMAT _pfFormat ) {
		static const GLint iTypes[] = {
			GL_UNSIGNED_BYTE,						// LSI_PF_A8
			GL_UNSIGNED_BYTE_3_3_2,					// LSI_PF_R3G3B2
			GL_UNSIGNED_SHORT_5_6_5,				// LSI_PF_R5G6B5
			GL_UNSIGNED_SHORT_4_4_4_4,				// LSI_PF_R4G4B4A4
			GL_UNSIGNED_SHORT_5_5_5_1,				// LSI_PF_R5G5B5A1
			GL_UNSIGNED_BYTE,						// LSI_PF_R8G8B8
			GL_UNSIGNED_INT_8_8_8_8,				// LSI_PF_R8G8B8A8
			GL_UNSIGNED_SHORT,						// LSI_PF_R16G16B16A16
			GL_HALF_FLOAT,							// LSI_PF_R16G16B16A16F
			GL_FLOAT,								// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iTypes[_pfFormat]; }
		return GL_NONE;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGL

#endif	// __LSG_OPENGLTEXTURECONSTANTS_H__
