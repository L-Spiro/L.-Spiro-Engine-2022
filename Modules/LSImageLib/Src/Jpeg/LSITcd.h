/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
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

#ifndef __LSI_TCD_H__
#define __LSI_TCD_H__

#include "../LSIImageLib.h"
#include "LSIJ2k.h"
#include "LSIJpegImage.h"
#include "LSITgt.h"

namespace lsi {

	/**
	 * Class CTcd
	 * \brief Implementation of a tile coder/decoder (TCD).
	 *
	 * Description: Implementation of a tile coder/decoder (TCD).
	 */
	class CTcd {
	public :
		// == Types.
		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_seg {
			uint8_t **							data;
			int32_t								dataindex;
			int32_t								numpasses;
			int32_t								len;
			int32_t								maxpasses;
			int32_t								numnewpasses;
			int32_t								newlen;
		} opj_tcd_seg_t;

		/**
		 * FIXME: documentation.
		 */
		typedef struct opj_tcd_pass {
			int32_t								rate;
			double							distortiondec;
			int32_t								term, len;
		} opj_tcd_pass_t;

		/**
		 * FIXME: documentation.
		 */
		typedef struct opj_tcd_layer {
			int32_t								numpasses;			/* Number of passes in the layer. */
			int32_t								len;				/* Length of information. */
			double							disto;				/* Adds for index (Cfr. Marcela). */
			uint8_t *							data;				/* Data. */
		} opj_tcd_layer_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_cblk_enc {
			uint8_t *							data;				/* Data. */
			opj_tcd_layer_t *					layers;				/* Layer information. */
			opj_tcd_pass_t *					passes;				/* Information about the passes. */
			int32_t								x0, y0, x1, y1;		/* Dimension of the code-blocks: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								numbps;
			int32_t								numlenbits;
			int32_t								numpasses;			/* Number of pass already done for the code-blocks. */
			int32_t								numpassesinlayers;	/* Number of passes in the layer. */
			int32_t								totalpasses;		/* Total number of passes. */
		} opj_tcd_cblk_enc_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_cblk_dec {
			uint8_t *							data;				/* Data. */
			opj_tcd_seg_t *						segs;				/* Segments informations. */
			int32_t								x0, y0, x1, y1;		/* Dimension of the code-blocks: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								numbps;
			int32_t								numlenbits;
			int32_t								len;				/* Length. */
			int32_t								numnewpasses;		/* Number of pass added to the code-blocks. */
			int32_t								numsegs;			/* Number of segments. */
		} opj_tcd_cblk_dec_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_precinct {
			int32_t								x0, y0, x1, y1;		/* Dimension of the precinct: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								cw, ch;				/* Number of precinct in width and heigth. */
			union {													/* Code-blocks informations. */
				opj_tcd_cblk_enc_t *			enc;
				opj_tcd_cblk_dec_t *			dec;
			}									cblks;
			CTgt::opj_tgt_tree_t *				incltree;			/* Inclusion tree. */
			CTgt::opj_tgt_tree_t *				imsbtree;			/* IMSB tree. */
		} opj_tcd_precinct_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_band {
			int32_t								x0, y0, x1, y1;		/* Dimension of the subband: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								bandno;
			opj_tcd_precinct_t *				precincts;			/* Precinct information. */
			int32_t								numbps;
			float								stepsize;
		} opj_tcd_band_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_resolution {
			int32_t								x0, y0, x1, y1;		/* Dimension of the resolution level: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								pw, ph;
			int32_t								numbands;			/* Number sub-band for the resolution level. */
			opj_tcd_band_t						bands[3];			/* Subband information. */
		} opj_tcd_resolution_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_tilecomp {
			int32_t								x0, y0, x1, y1;		/* Dimension of component: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								numresolutions;		/* Number of resolutions level. */
			opj_tcd_resolution_t *				resolutions;		/* Resolutions information. */
			int32_t *							data;				/* Data of the component. */
			int32_t								numpix;				/* Adds fixed_quality. */
		} opj_tcd_tilecomp_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_tile {
			int32_t								x0, y0, x1, y1;		/* Dimension of the tile: left upper corner (x0, y0) right low corner (x1, y1). */
			int32_t								numcomps;			/* Number of components in tile. */
			opj_tcd_tilecomp_t *				comps;				/* Components information. */
			int32_t								numpix;				/* Adds fixed_quality. */
			double							distotile;			/* Adds fixed_quality. */
			double							distolayer[100];	/* Adds fixed_quality. */
			/** packet number. */
			int32_t								packno;
		} opj_tcd_tile_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_tcd_image {
			int32_t								tw, th;				/* Number of tiles in width and heigth. */
			opj_tcd_tile_t *					tiles;				/* Tiles information. */
		} opj_tcd_image_t;

		/**
		 * Tile coder/decoder
		 */
		typedef struct opj_tcd_t {
			/** Position of the tilepart flag in Progression order*/
			int32_t								tp_pos;
			/** Tile part number*/
			int32_t								tp_num;
			/** Current tile part number*/
			int32_t								cur_tp_num;
			/** Total number of tileparts of the current tile*/
			int32_t								cur_totnum_tp;
			/** Current Packet iterator number. */
			int32_t								cur_pino;
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr	cinfo;

			/** Info on each image tile. */
			opj_tcd_image_t *					tcd_image;
			/** Image. */
			CJpegImage::opj_image_t *			image;
			/** Coding parameters. */
			CJ2k::opj_cp_t *					cp;
			/** Pointer to the current encoded/decoded tile. */
			opj_tcd_tile_t *					tcd_tile;
			/** Coding/decoding parameters common to all tiles. */
			CJ2k::opj_tcp_t *					tcp;
			/** Current encoded/decoded tile. */
			int32_t								tcd_tileno;
			/** Time taken to encode a tile. */
			double							encoding_time;
		} * LPopj_tcd_t, * const LPCopj_tcd_t;


		// == Functions.
		/**
		 * Dump the content of a tcd structure.
		 */
		//static void tcd_dump( FILE * fd, opj_tcd_t * _ptTcd, opj_tcd_image_t * img );

		/**
		 * Creates a new TCD handle.
		 *
		 * \param _cpCInfo Codec context info.
		 * \return Returns a new TCD handle if successful returns NULL otherwise.
		 */
		static opj_tcd_t * LSE_CALL				tcd_create( CJpegCommonStructs::opj_common_ptr _cpCInfo );

		/**
		 * Destroy a previously created TCD handle.
		 *
		 * \param _ptTcd TCD handle to destroy.
		 */
		static void LSE_CALL					tcd_destroy( opj_tcd_t * _ptTcd );

		/**
		 * Initialize the tile coder (allocate the memory).
		 *
		 * \param _ptTcd TCD handle.
		 * \param _piImage Raw _piImage.
		 * \param _pcCp Coding parameters.
		 * \param _i32CurtileNo Number that identifies the tile that will be encoded.
		 */
		static void LSE_CALL					tcd_malloc_encode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32CurtileNo );

		/**
		 * Free the memory allocated for encoding.
		 *
		 * \param _ptTcd TCD handle.
		 */
		static void LSE_CALL					tcd_free_encode( opj_tcd_t * _ptTcd );

		/**
		 * Initialize the tile coder (reuses the memory allocated by tcd_malloc_encode).
		 *
		 * \param _ptTcd TCD handle.
		 * \param _piImage Raw image.
		 * \param _pcCp Coding parameters.
		 * \param _i32CurTileNo Number that identifies the tile that will be encoded.
		 */
		static void LSE_CALL					tcd_init_encode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32CurTileNo );

		/**
		 * Initialize the tile decoder.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _piImage Raw image.
		 * \param _pcCp Coding parameters.
		 */
		static void LSE_CALL					tcd_malloc_decode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _piImage Raw image.
		 * \param _pcCp Coding parameters.
		 * \param _i32TileNo Undocumented OpenJpeg Library parameter.
		 * \param _pciInfoC Undocumented OpenJpeg Library parameter.
		 */
		static void LSE_CALL					tcd_malloc_decode_tile( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _i32LayNo Undocumented OpenJpeg Library parameter.
		 * \param _i32Final Undocumented OpenJpeg Library parameter.
		 */
		static void LSE_CALL					tcd_makelayer_fixed( opj_tcd_t * _ptTcd, int32_t _i32LayNo, int32_t _i32Final );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 */
		static void LSE_CALL					tcd_rateallocate_fixed( opj_tcd_t * _ptTcd );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _i32LayNo Undocumented OpenJpeg Library parameter.
		 * \param _dThresh Undocumented OpenJpeg Library parameter.
		 * \param _i32Final Undocumented OpenJpeg Library parameter.
		 */
		static void LSE_CALL					tcd_makelayer( opj_tcd_t * _ptTcd, int32_t _i32LayNo, double _dThresh, int32_t _i32Final );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _pui8Dest Undocumented OpenJpeg Library parameter.
		 * \param _i32Len Undocumented OpenJpeg Library parameter.
		 * \param _pciInfoC Undocumented OpenJpeg Library parameter.
		 */
		static LSBOOL LSE_CALL					tcd_rateallocate( opj_tcd_t * _ptTcd, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC );

		/**
		 * Encode a tile from the raw image into a buffer.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _i32TileNo Number that identifies one of the tiles to be encoded.
		 * \param _pui8Dest Destination buffer.
		 * \param _i32Len Length of destination buffer.
		 * \param _pciInfoC Codestream information structure.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL					tcd_encode_tile( opj_tcd_t * _ptTcd, int32_t _i32TileNo, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC );
		
		/**
		 * Decode a tile from a buffer into a raw image.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _pui8Src Source buffer.
		 * \param _i32Len Length of source buffer.
		 * \param _i32TileNo Number that identifies one of the tiles to be decoded.
		 * \param _pciInfoC Codestream information structure.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static LSBOOL LSE_CALL					tcd_decode_tile( opj_tcd_t * _ptTcd, uint8_t * _pui8Src, int32_t _i32Len, int32_t _i32TileNo, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC );

		/**
		 * Free the memory allocated for decoding.
		 *
		 * \param _ptTcd TCD handle.
		 */
		static void LSE_CALL					tcd_free_decode( opj_tcd_t * _ptTcd );

		/**
		 * Undocumented OpenJpeg Library function.
		 *
		 * \param _ptTcd TCD handle.
		 * \param _i32TileNo Undocumented OpenJpeg Library parameter.
		 */
		static void LSE_CALL					tcd_free_decode_tile( opj_tcd_t * _ptTcd, int32_t _i32TileNo );
	};

}	// namespace lsi

#endif	// __LSI_TCD_H__
