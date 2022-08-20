 /*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain ( UCL ), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
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


#ifndef __LSI_OPENJPEG_H__
#define __LSI_OPENJPEG_H__

#include "../LSIImageLib.h"
#include "LSICio.h"
#include "LSIJpegCommonStructs.h"
#include "LSIJpegImage.h"



namespace lsi {

	/**
	 * Class COpenJpeg
	 * \brief OpenJpeg declarations.
	 *
	 * Description: OpenJpeg declarations.
	 */
	class COpenJpeg {
	public :
		// == Functions.
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _cpInfo Undocumented parameter from the OpenJpeg Library.
		 * \param _pemEventManager Undocumented parameter from the OpenJpeg Library.
		 * \param _pvContext Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static CJpegCommonStructs::opj_event_mgr_t * LSE_CALL		opj_set_event_mgr( CJpegCommonStructs::opj_common_ptr _cpInfo, CJpegCommonStructs::opj_event_mgr_t * _pemEventManager, void * _pvContext );

		/**
		 * Creates a J2K/JPT/JP2 decompression structure.
		 *
		 * \param _cfFormat Decoder to select.
		 * \return Returns a handle to a decompressor if successful, returns NULL otherwise.
		 */
		static CJpegCommonStructs::opj_dinfo_t * LSE_CALL			opj_create_decompress( OPJ_CODEC_FORMAT _cfFormat );

		/**
		 * Destroy a decompressor handle.
		 *
		 * \param _pdInfo decompressor handle to destroy.
		 */
		static void LSE_CALL										opj_destroy_decompress( CJpegCommonStructs::opj_dinfo_t * _pdInfo );

		/**
		 * Sets decoding parameters to default values.
		 *
		 * \param _pdParameters Decompression parameters.
		 */
		static void LSE_CALL										opj_set_default_decoder_parameters( CJpegCommonStructs::opj_dparameters_t * _pdParameters );

		/**
		 * Setup the decoder decoding parameters using user parameters.
		 * Decoding parameters are returned in j2k->cp.
		 *
		 * \param _pdInfo decompressor handle.
		 * \param _pdParameters decompression parameters.
		 */
		static void LSE_CALL										opj_setup_decoder( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CJpegCommonStructs::opj_dparameters_t * _pdParameters );

		/**
		 * Decode an image from a JPEG-2000 codestream.
		 *
		 * \param _pdInfo decompressor handle.
		 * \param _pcCio Input buffer stream.
		 * \return Returns a decoded image if successful, returns NULL otherwise.
		 */
		static CJpegImage::opj_image_t * LSE_CALL						opj_decode( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CCio::opj_cio_t * _pcCio );

		/**
		 * Decode an image from a JPEG-2000 codestream and extract the codestream information.
		 *
		 * \param _pdInfo decompressor handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pciInfo Codestream information structure if needed afterwards, NULL otherwise.
		 * \return Returns a decoded image if successful, returns NULL otherwise.
		 */
		static CJpegImage::opj_image_t * LSE_CALL						opj_decode_with_info( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciInfo );

		/**
		 * Creates a J2K/JP2 compression structure.
		 *
		 * \param _cfFormat Coder to select.
		 * \return Returns a handle to a compressor if successful, returns NULL otherwise.
		 */
		static CJpegCommonStructs::opj_cinfo_t * LSE_CALL			opj_create_compress( OPJ_CODEC_FORMAT _cfFormat );

		/**
		 * Destroy a compressor handle.
		 *
		 * \param _cpInfo compressor handle to destroy.
		 */
		static void LSE_CALL										opj_destroy_compress( CJpegCommonStructs::opj_cinfo_t * _cpInfo );

		/**
		 * Sets encoding parameters to default values, that means: 
		 * <ul>
		 * <li>Lossless
		 * <li>1 tile
		 * <li>Size of precinct : 2^15 x 2^15 (means 1 precinct)
		 * <li>Size of code-block : 64 x 64
		 * <li>Number of resolutions: 6
		 * <li>No SOP marker in the codestream
		 * <li>No EPH marker in the codestream
		 * <li>No sub-sampling in x or y direction
		 * <li>No mode switch activated
		 * <li>Progression order: LRCP
		 * <li>No index file
		 * <li>No ROI upshifted
		 * <li>No offset of the origin of the image
		 * <li>No offset of the origin of the tiles
		 * <li>Reversible DWT 5-3
		 * </ul>
		 *
		 * \param _pdParameters Compression parameters.
		 */
		static void LSE_CALL										opj_set_default_encoder_parameters( CJpegCommonStructs::opj_cparameters_t * _pdParameters );

		/**
		 * Setup the encoder parameters using the current image and using user parameters.
		 *
		 * \param _cpInfo Compressor handle.
		 * \param _pdParameters Compression parameters.
		 * \param _piImage Input filled image.
		 */
		static void LSE_CALL										opj_setup_encoder( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CJpegCommonStructs::opj_cparameters_t * _pdParameters, CJpegImage::opj_image_t * _piImage );

		/**
		 * Encode an image into a JPEG-2000 codestream.
		 *
		 * \param _cpInfo compressor handle.
		 * \param _pcCio Output buffer stream.
		 * \param _piImage Image to encode.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL										opj_encode( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage );

		/**
		 * Encode an image into a JPEG-2000 codestream and extract the codestream information.
		 *
		 * \param _cpInfo compressor handle.
		 * \param _pcCio Output buffer stream.
		 * \param _piImage Image to encode.
		 * \param _pciInfo Codestream information structure if needed afterwards, NULL otherwise.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL										opj_encode_with_info( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo );

		/**
		 * Destroy Codestream information after compression or decompression.
		 *
		 * \param _pciInfo Codestream information structure.
		 */
		static void LSE_CALL										opj_destroy_cstr_info( CJpegCommonStructs::opj_codestream_info_t * _pciInfo );
	};

}	// namespace lsi

#endif	// __LSI_OPENJPEG_H__
