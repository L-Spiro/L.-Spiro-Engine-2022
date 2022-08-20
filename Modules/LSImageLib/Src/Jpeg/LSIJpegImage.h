/*
 * Copyright (c) 2005, HervÅEDrolon, FreeImage Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __LSI_JPEGIMAGE_H__
#define __LSI_JPEGIMAGE_H__

#include "../LSIImageLib.h"

namespace lsi {

	// == Enumerations.
	/**
	 * Supported image color spaces.
	 */
	typedef enum COLOR_SPACE {
		CLRSPC_UNKNOWN = -1, 		/**< Not supported by the library. */
		CLRSPC_UNSPECIFIED = 0, 	/**< Not specified in the codestream. */ 
		CLRSPC_SRGB = 1, 			/**< sRGB. */
		CLRSPC_GRAY = 2, 			/**< Grayscale. */
		CLRSPC_SYCC = 3				/**< YUV. */
	} OPJ_COLOR_SPACE;


	/**
	 * Class CJpegImage
	 * \brief Implementation of operations on images.
	 *
	 * Description: Implementation of operations on images.
	 */
	class CJpegImage {
	public :
		// == Types.
		/**
		 * Defines a single image component
		 */
		typedef struct opj_image_comp {
			/** XRsiz: horizontal separation of a sample of ith component with respect to the reference grid. */
			int32_t													dx;
			/** YRsiz: vertical separation of a sample of ith component with respect to the reference grid. */
			int32_t													dy;
			/** Data width. */
			int32_t													w;
			/** Data height. */
			int32_t													h;
			/** X component offset compared to the whole image. */
			int32_t													x0;
			/** Y component offset compared to the whole image. */
			int32_t													y0;
			/** Precision. */
			int32_t													prec;
			/** Image depth in bits. */
			int32_t													bpp;
			/** Signed (1)/unsigned (0). */
			int32_t													sgnd;
			/** Number of decoded resolution. */
			int32_t													resno_decoded;
			/** Number of division by 2 of the out image compared to the original size of image. */
			int32_t													factor;
			/** Image component data. */
			int32_t *												data;
		} opj_image_comp_t;

		/** 
		 * Defines image data and characteristics
		 */
		typedef struct opj_image {
			/** XOsiz: horizontal offset from the origin of the reference grid to the left side of the image area. */
			int32_t													x0;
			/** YOsiz: vertical offset from the origin of the reference grid to the top side of the image area. */
			int32_t													y0;
			/** Xsiz: width of the reference grid. */
			int32_t													x1;
			/** Ysiz: height of the reference grid. */
			int32_t													y1;
			/** Number of components in the image. */
			int32_t													numcomps;
			/** Color space: sRGB, Greyscale or YUV. */
			OPJ_COLOR_SPACE											color_space;
			/** Image components. */
			opj_image_comp_t *										comps;
			/** "Restricted" ICC profile. */
			uint8_t *												icc_profile_buf;
			/** Size of ICC profile. */
			int32_t													icc_profile_len;
		} opj_image_t;

		/**
		 * Component parameters structure used by the opj_image_create function
		 */
		typedef struct opj_image_comptparm {
			/** XRsiz: horizontal separation of a sample of ith component with respect to the reference grid. */
			int32_t													dx;
			/** YRsiz: vertical separation of a sample of ith component with respect to the reference grid. */
			int32_t													dy;
			/** Data width. */
			int32_t													w;
			/** Data height. */
			int32_t													h;
			/** X component offset compared to the whole image. */
			int32_t													x0;
			/** Y component offset compared to the whole image. */
			int32_t													y0;
			/** Precision. */
			int32_t													prec;
			/** Image depth in bits. */
			int32_t													bpp;
			/** Signed (1)/unsigned (0). */
			int32_t													sgnd;
		} opj_image_cmptparm_t;


		// == Functions.
		/**
		 * Create an empty image.
		 * \todo this function should be removed.
		 * \return returns an empty image if successful, returns NULL otherwise.
		 */
		static opj_image_t *										opj_image_create0();

		/**
		 * Create an image.
		 *
		 * \param _i32NumComponents Number of components.
		 * \param _picComponentParms Components parameters.
		 * \param _csColorSpace Image color space.
		 * \return Returns a new image structure if successful, returns NULL otherwise.
		 */
		static opj_image_t * LSE_CALL								opj_image_create( int32_t _i32NumComponents, opj_image_cmptparm_t * _picComponentParms, OPJ_COLOR_SPACE _csColorSpace );

		/**
		 * Deallocate any resources associated with an image.
		 *
		 * \param _piImage Image to be destroyed.
		 */
		static void LSE_CALL										opj_image_destroy( opj_image_t * _piImage );
	};

}	// namespace lsi

#endif	// __LSI_JPEGIMAGE_H__

