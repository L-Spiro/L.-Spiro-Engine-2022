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


#ifndef __LSG_OPENGLESTEXTURECONSTANTS_H__
#define __LSG_OPENGLESTEXTURECONSTANTS_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGLES

#include "../OpenGLES/LSGOpenGlEs.h"

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

	class COpenGlEsTextureConstants {
	public :
		// == Functions.
		/**
		 * Given an image format, return the OpenGL internal format for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \param _bsRgb If true, an sRGB format is selected, if possible.
		 * \return Returns the OpenGL internal format to use with the given image-library format.
		 */
		static LSE_INLINE GLint LSE_CALL		GetOpenGlInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb );

		/**
		 * Given an image format, return the OpenGL format for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \param _bsRgb If true, an sRGB format is selected, if possible.
		 * \return Returns the OpenGL format to use with the given image-library format.
		 */
		static LSE_INLINE GLenum LSE_CALL		GetOpenGlFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb );

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
	LSE_INLINE GLint LSE_CALL COpenGlEsTextureConstants::GetOpenGlInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb ) {
		if ( _bsRgb ) {
			static const GLint iFormats[] = {
				GL_ALPHA,				// LSI_PF_A8
				0,						// LSI_PF_R3G3B2
				0,						// LSI_PF_R5G6B5
				0,						// LSI_PF_R4G4B4A4
				0,						// LSI_PF_R5G5B5A1
				GL_SRGB_EXT,			// LSI_PF_R8G8B8
				GL_SRGB_ALPHA_EXT,		// LSI_PF_R8G8B8A8
				0,						// LSI_PF_R16G16B16A16
				0,						// LSI_PF_R16G16B16A16F
				0,						// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		}
		else {
			static const GLint iFormats[] = {
				GL_ALPHA,				// LSI_PF_A8
				GL_RGB,					// LSI_PF_R3G3B2
				GL_RGB,					// LSI_PF_R5G6B5
				GL_RGBA,				// LSI_PF_R4G4B4A4
				GL_RGBA,				// LSI_PF_R5G5B5A1
				GL_RGB,					// LSI_PF_R8G8B8
				GL_RGBA,				// LSI_PF_R8G8B8A8
				GL_RGBA,				// LSI_PF_R16G16B16A16
				GL_RGBA,				// LSI_PF_R16G16B16A16F
				GL_RGBA,				// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		}
		return GL_NONE;
	}

	/**
	 * Given an image format, return the OpenGL format for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \param _bsRgb If true, an sRGB format is selected, if possible.
	 * \return Returns the OpenGL format to use with the given image-library format.
	 */
	LSE_INLINE GLenum LSE_CALL COpenGlEsTextureConstants::GetOpenGlFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb ) {
		if ( _bsRgb ) {
			static const GLint iFormats[] = {
				GL_ALPHA,				// LSI_PF_A8
				0,						// LSI_PF_R3G3B2
				0,						// LSI_PF_R5G6B5
				0,						// LSI_PF_R4G4B4A4
				0,						// LSI_PF_R5G5B5A1
				GL_SRGB_EXT,			// LSI_PF_R8G8B8
				GL_SRGB_ALPHA_EXT,		// LSI_PF_R8G8B8A8
				0,						// LSI_PF_R16G16B16A16
				0,						// LSI_PF_R16G16B16A16F
				0,						// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		}
		else {
			static const GLint iFormats[] = {
				GL_ALPHA,				// LSI_PF_A8
				GL_RGB,					// LSI_PF_R3G3B2
				GL_RGB,					// LSI_PF_R5G6B5
				GL_RGBA,				// LSI_PF_R4G4B4A4
				GL_RGBA,				// LSI_PF_R5G5B5A1
				GL_RGB,					// LSI_PF_R8G8B8
				GL_RGBA,				// LSI_PF_R8G8B8A8
				GL_RGBA,				// LSI_PF_R16G16B16A16
				GL_RGBA,				// LSI_PF_R16G16B16A16F
				GL_RGBA,				// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iFormats[_pfFormat]; }
		}
		return GL_NONE;
	}

	/**
	 * Given an image format, return the OpenGL type for standard textures.
	 *
	 * \param _pfFormat Format to convert.
	 * \return Returns the OpenGL type to use with the given image-library format.
	 */
	LSE_INLINE GLenum LSE_CALL COpenGlEsTextureConstants::GetOpenGlTypeStandard( LSI_PIXEL_FORMAT _pfFormat ) {
		static const GLint iTypes[] = {
			GL_UNSIGNED_BYTE,						// LSI_PF_A8
			GL_NONE,								// LSI_PF_R3G3B2
			GL_UNSIGNED_SHORT_5_6_5,				// LSI_PF_R5G6B5
			GL_UNSIGNED_SHORT_4_4_4_4,				// LSI_PF_R4G4B4A4
			GL_UNSIGNED_SHORT_5_5_5_1,				// LSI_PF_R5G5B5A1
			GL_UNSIGNED_BYTE,						// LSI_PF_R8G8B8
			GL_UNSIGNED_BYTE,						// LSI_PF_R8G8B8A8
			GL_UNSIGNED_SHORT,						// LSI_PF_R16G16B16A16
			GL_HALF_FLOAT_OES,						// LSI_PF_R16G16B16A16F
			GL_FLOAT,								// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return iTypes[_pfFormat]; }
		return GL_NONE;
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES

#endif	// __LSG_OPENGLESTEXTURECONSTANTS_H__
