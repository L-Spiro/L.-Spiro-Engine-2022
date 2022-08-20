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

#include "LSIJpegImage.h"


namespace lsi {

	// == Functions.
	/**
	 * Create an empty image.
	 * \todo this function should be removed.
	 * \return returns an empty image if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * CJpegImage::opj_image_create0() {
		opj_image_t * piImage = static_cast<opj_image_t *>(CMemLib::CMAlloc( sizeof( opj_image_t ) ));
		return piImage;
	}

	/**
	 * Create an image.
	 *
	 * \param _i32NumComponents Number of components.
	 * \param _picComponentParms Components parameters.
	 * \param _csColorSpace Image color space.
	 * \return Returns a new image structure if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL CJpegImage::opj_image_create( int32_t _i32NumComponents, opj_image_cmptparm_t * _picComponentParms, OPJ_COLOR_SPACE _csColorSpace ) {
		int32_t i32CompNo;
		opj_image_t * piImage = NULL;

		piImage = static_cast<opj_image_t *>(CMemLib::CMAlloc( sizeof( opj_image_t ) ));
		if ( piImage ) {
			piImage->color_space = _csColorSpace;
			piImage->numcomps = _i32NumComponents;
			// Allocate memory for the per-component information.
			piImage->comps = static_cast<opj_image_comp_t *>(CMemLib::MAlloc( static_cast<uint32_t>(piImage->numcomps) * sizeof( opj_image_comp_t ) ));
			if ( !piImage->comps ) {
				CStd::DebugPrintA( "Unable to allocate memory for piImage.\r\n" );
				opj_image_destroy( piImage );
				return NULL;
			}
			// Create the individual piImage components.
			for ( i32CompNo = 0; i32CompNo < _i32NumComponents; i32CompNo++ ) {
				opj_image_comp_t * picComp = &piImage->comps[i32CompNo];
				picComp->dx = _picComponentParms[i32CompNo].dx;
				picComp->dy = _picComponentParms[i32CompNo].dy;
				picComp->w = _picComponentParms[i32CompNo].w;
				picComp->h = _picComponentParms[i32CompNo].h;
				picComp->x0 = _picComponentParms[i32CompNo].x0;
				picComp->y0 = _picComponentParms[i32CompNo].y0;
				picComp->prec = _picComponentParms[i32CompNo].prec;
				picComp->bpp = _picComponentParms[i32CompNo].bpp;
				picComp->sgnd = _picComponentParms[i32CompNo].sgnd;
				picComp->data = static_cast<int32_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(picComp->w * picComp->h) * sizeof( int32_t ) ));
				if ( !picComp->data ) {
					CStd::DebugPrintA( "Unable to allocate memory for piImage.\r\n" );
					opj_image_destroy( piImage );
					return NULL;
				}
			}
		}

		return piImage;
	}

	/**
	 * Deallocate any resources associated with an image.
	 *
	 * \param _piImage Image to be destroyed.
	 */
	void LSE_CALL CJpegImage::opj_image_destroy( opj_image_t * _piImage ) {
		int32_t I;
		if ( _piImage ) {
			if ( _piImage->comps ) {
				// Image components.
				for ( I = 0; I < _piImage->numcomps; I++ ) {
					opj_image_comp_t * picImageComp = &_piImage->comps[I];
					if ( picImageComp->data ) {
						CMemLib::Free( picImageComp->data );
					}
				}
				CMemLib::Free( _piImage->comps );
			}
			CMemLib::Free( _piImage );
		}
	}

}	// namespace lsi
