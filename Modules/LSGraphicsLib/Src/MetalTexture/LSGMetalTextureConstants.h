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
 * Description: A Metal texture constants.
 */


#ifndef __LSG_METALTEXTURECONSTANTS_H__
#define __LSG_METALTEXTURECONSTANTS_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Metal/LSGMetal.h"

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

	class CMetalTextureConstants {
	public :
		// == Functions.
		/**
		 * Given an image format, return the Metal internal format for standard textures.
		 *
		 * \param _pfFormat Format to convert.
		 * \param _bsRgb If true, an sRGB format is selected, if possible.
		 * \return Returns the Metal internal format to use with the given image-library format.
		 */
		static LSINT32 LSE_CALL					GetMetalInternalFormatStandard( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bsRgb );

	};

}	// namespace lsg

#endif	// #ifdef LSG_METAL

#endif	// __LSG_METALTEXTURECONSTANTS_H__
