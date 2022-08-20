/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
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


#ifndef __LSI_JP2_H__
#define __LSI_JP2_H__

#include "../LSIImageLib.h"
#include "LSICio.h"
#include "LSIJ2k.h"
#include "LSIJpegCommonStructs.h"
#include "LSIJpegImage.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define JPIP_JPIP	0x6A706970

#define JP2_JP		0x6A502020			/**< JPEG 2000 signature box. */
#define JP2_FTYP	0x66747970			/**< File type box. */
#define JP2_JP2H	0x6A703268			/**< JP2 header box. */
#define JP2_IHDR	0x69686472			/**< Image header box. */
#define JP2_COLR	0x636F6C72			/**< Color specification box. */
#define JP2_JP2C	0x6A703263			/**< Contiguous codestream box. */
#define JP2_URL		0x75726C20			/**< URL box. */
#define JP2_DTBL	0x6474626C			/**< Data Reference box. */
#define JP2_BPCC	0x62706363			/**< Bits per component box. */
#define JP2_JP2		0x6A703220			/**< File type fields. */
#define JP2_PCLR	0x70636C72			/**< Palette box. */
#define JP2_CMAP	0x636D6170			/**< Component Mapping box. */
#define JP2_CDEF	0x63646566			/**< Channel Definition box. */


namespace lsi {

	/**
	 * Class CJp2
	 * \brief The JPEG-2000 file format Reader/Writer (JP2).
	 *
	 * Description: The JPEG-2000 file format Reader/Writer (JP2).
	 */
	class CJp2 {
	public :
		// == Types.
		/** 
		 * Channel description: channel index, type, assocation.
		 */
		typedef struct opj_jp2_cdef_info_t {
			uint16_t					cn, typ, asoc;
		} * LPopj_jp2_cdef_info_t, * const LPCopj_jp2_cdef_info_t;

		/** 
		 * Channel descriptions and number of descriptions.
		 */
		typedef struct opj_jp2_cdef {
			opj_jp2_cdef_info_t *		info;
			uint16_t					n;
		} opj_jp2_cdef_t;

		/** 
		 * Component mappings: channel index, mapping type, palette index.
		 */
		typedef struct opj_jp2_cmap_comp {
			uint16_t					cmp;
			uint8_t						mtyp, pcol;
		} opj_jp2_cmap_comp_t;

		/** 
		 * Palette data: table entries, palette columns.
		 */
		typedef struct opj_jp2_pclr {
			uint32_t *					entries;
			uint8_t *					channel_sign;
			uint8_t *					channel_size;
			opj_jp2_cmap_comp_t *		cmap;
			uint16_t					nr_entries, nr_channels;
		} opj_jp2_pclr_t;

		/** 
		 * Collector for ICC profile, palette, component mapping, channel description.
		 */
		typedef struct opj_jp2_color {
			uint8_t *					icc_profile_buf;
			int32_t						icc_profile_len;

			opj_jp2_cdef_t *			jp2_cdef;
			opj_jp2_pclr_t *			jp2_pclr;
			uint8_t						jp2_has_colr;
		} opj_jp2_color_t;

		/** 
		 * JP2 component.
		 */
		typedef struct opj_jp2_comps {
			int32_t						depth;
			int32_t						sgnd;
			int32_t						bpcc;
		} opj_jp2_comps_t;

		/**
		 * JPEG-2000 file format reader/writer.
		 */
		typedef struct opj_jp2 {
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr
										cinfo;
			/** Handle to the J2K codec. */
			CJ2k::opj_j2k_t *			j2k;
			uint32_t					w;
			uint32_t					h;
			uint32_t					numcomps;
			uint32_t					bpc;
			uint32_t					C;
			uint32_t					UnkC;
			uint32_t					IPR;
			uint32_t					meth;
			uint32_t					approx;
			uint32_t					enumcs;
			uint32_t					precedence;
			uint32_t					brand;
			uint32_t					minversion;
			uint32_t					numcl;
			uint32_t *					cl;
			opj_jp2_comps_t *			comps;
			uint32_t					j2k_codestream_offset;
			uint32_t					j2k_codestream_length;
		} opj_jp2_t;

		/**
		 * JP2 Box.
		 */
		typedef struct opj_jp2_box {
			int32_t						length;
			int32_t						type;
			int32_t						init_pos;
		} opj_jp2_box_t;

		

		// == Functions.
		/**
		 * Write the JP2 Header box (used in MJ2).
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Output buffer stream.
		 */
		static void LSE_CALL			jp2_write_jp2h( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Read the JP2 Header box (used in MJ2).
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjcColor Undocumented paramater from the OpenJpeg Library.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_jp2h( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, opj_jp2_color_t * _pjcColor );

		/**
		 * Creates a JP2 decompression structure.
		 *
		 * \param _cpInfo Codec context info.
		 * \return Returns a handle to a JP2 decompressor if successful, returns NULL otherwise.
		 */
		static opj_jp2_t * LSE_CALL		jp2_create_decompress( CJpegCommonStructs::opj_common_ptr _cpInfo );

		/**
		 * Destroy a JP2 decompressor handle.
		 *
		 * \param _pjJp2 JP2 decompressor handle to destroy.
		 */
		static void LSE_CALL			jp2_destroy_decompress( opj_jp2_t * _pjJp2 );

		/**
		 * Setup the decoder decoding parameters using user parameters.
		 * Decoding parameters are returned in _pjJp2->j2k->cp.
		 *
		 * \param _pjJp2 JP2 decompressor handle.
		 * \param _pdParameters decompression _pdParameters.
		 */
		static void LSE_CALL			jp2_setup_decoder( opj_jp2_t * _pjJp2, CJpegCommonStructs::opj_dparameters_t * _pdParameters );

		/**
		 * Decode an image from a JPEG-2000 file stream.
		 *
		 * \param _pjJp2 JP2 decompressor handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pciInfo Codestream information structure if required, NULL otherwise.
		 * \return Returns a decoded image if successful, returns NULL otherwise.
		 */
		static CJpegImage::opj_image_t * LSE_CALL
										jp2_decode( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciInfo );

		/**
		 * Creates a JP2 compression structure.
		 *
		 * \param _cpInfo Codec context info.
		 * \return Returns a handle to a JP2 compressor if successful, returns NULL otherwise.
		 */
		static opj_jp2_t * LSE_CALL		jp2_create_compress( CJpegCommonStructs::opj_common_ptr _cpInfo );

		/**
		 * Destroy a JP2 compressor handle.
		 *
		 * \param _pjJp2 JP2 compressor handle to destroy.
		 */
		static void LSE_CALL			jp2_destroy_compress( opj_jp2_t * _pjJp2 );

		/**
		 * Setup the encoder _pdParameters using the current image and using user _pdParameters.
		 * Coding _pdParameters are returned in _pjJp2->j2k->cp.
		 *
		 * \param _pjJp2 JP2 compressor handle.
		 * \param _pdParameters compression parameters.
		 * \param _piImage input filled image.
		 */
		static void LSE_CALL			jp2_setup_encoder( opj_jp2_t * _pjJp2, CJpegCommonStructs::opj_cparameters_t * _pdParameters, CJpegImage::opj_image_t * _piImage );

		/**
		 * Encode an image into a JPEG-2000 file stream.
		 *
		 * \param _pjJp2 JP2 compressor handle.
		 * \param _pcCio Output buffer stream.
		 * \param _piImage Image to encode.
		 * \param _pciInfo Codestream information structure if required, NULL otherwise.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_encode( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo );


	protected :
		// == Functions.
		/**
		 * Read box headers.
		 *
		 * \param _cpInfo Codec context info.
		 * \param _pcCio Input stream.
		 * \param box.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_boxhdr( CJpegCommonStructs::opj_common_ptr _cpInfo, CCio::opj_cio_t * _pcCio, opj_jp2_box_t * _pjbBox );

		/**
		 * Read the IHDR box - Image Header box.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_ihdr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Output buffer stream.
		 */
		static void LSE_CALL			jp2_write_ihdr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Output buffer stream.
		 */
		static void LSE_CALL			jp2_write_bpcc( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param Input Output buffer stream.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_bpcc( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Output buffer stream.
		 */
		static void LSE_CALL			jp2_write_colr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Write the FTYP box - File type box.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Output buffer stream.
		 */
		static void LSE_CALL			jp2_write_ftyp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Read the FTYP box - File type box.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_ftyp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 Undocumented parameter from the OpenJpeg Library.
		 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
		 * \param _piImage Undocumented parameter from the OpenJpeg Library.
		 * \param _pciInfo Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL			jp2_write_jp2c( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 Undocumented parameter from the OpenJpeg Library.
		 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
		 * \param _pui32CodeStreamLen Undocumented parameter from the OpenJpeg Library.
		 * \param _pui32CodeStreamOffset Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static LSBOOL LSE_CALL			jp2_read_jp2c( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, uint32_t * _pui32CodeStreamLen, uint32_t * _pui32CodeStreamOffset );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL			jp2_write_jp( CCio::opj_cio_t * _pcCio );

		/**
		 * Read the JP box - JPEG 2000 signature.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_jp( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio );

		/**
		 * Decode the structure of a JP2 file.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_struct( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
			opj_jp2_color_t * _pjcColor );

		/**
		 * Apply collected palette data.
		 *
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \param _piImage Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL			jp2_apply_pclr( opj_jp2_color_t * _pjcColor, CJpegImage::opj_image_t * _piImage );

		/**
		 * Collect palette data.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_pclr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
			opj_jp2_box_t * _pjbBox, opj_jp2_color_t * _pjcColor );

		/**
		 * Collect component mapping data.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_cmap( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
			opj_jp2_box_t * _pjbBox, opj_jp2_color_t * _pjcColor );

		/**
		 * Collect color specification data.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_colr( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
			opj_jp2_box_t * _pjbBox, opj_jp2_color_t * _pjcColor );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjJp2 JP2 handle.
		 * \param _pcCio Input buffer stream.
		 * \param _pjbBox Undocumented parameter from the OpenJpeg Library.
		 * \param _pjcColor Collector for profile, cdef and pclr data.
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CALL			jp2_read_cdef( opj_jp2_t * _pjJp2, CCio::opj_cio_t * _pcCio, 
			opj_jp2_box_t * _pjbBox, opj_jp2_color_t * _pjcColor );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL			free_color_data( opj_jp2_color_t * _pjcColor );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _piImage Undocumented parameter from the OpenJpeg Library.
		 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL			jp2_apply_cdef( CJpegImage::opj_image_t * _piImage, opj_jp2_color_t * _pjcColor );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pjcColor Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL			jp2_free_pclr( opj_jp2_color_t * _pjcColor );

	};

}	// namespace lsi

#endif	// __LSI_JP2_H__

