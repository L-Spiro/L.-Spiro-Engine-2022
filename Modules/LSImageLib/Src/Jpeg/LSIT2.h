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


#ifndef __LSI_T2_H__
#define __LSI_T2_H__

#include "../LSIImageLib.h"
#include "LSIJ2k.h"
#include "LSIJpegCommonStructs.h"
#include "LSIJpegImage.h"
#include "LSIPi.h"
#include "LSITcd.h"

namespace lsi {

	/**
	 * Class CT2
	 * \brief Implementation of a tier-2 coding (packetization of code-block data) (T2).
	 *
	 * Description: Implementation of a tier-2 coding (packetization of code-block data) (T2).
	 */
	class CT2 {
	public :
		// == Types.
		/**
		 * Tier-2 coding
		 */
		typedef struct opj_t2 {
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr		cinfo;

			/** Encoding: pointer to the source image. Decoding: pointer to the destination image. */
			CJpegImage::opj_image_t *				image;

			/** Pointer to the image coding parameters. */
			CJ2k::opj_cp_t *						cp;
		} opj_t2_t;


		// == Functions.
		/**
		 * Encode the packets of a tile to a destination buffer.
		 *
		 * \param _ptT2 T2 handle.
		 * \param _i32TileNo Number of the tile encoded.
		 * \param _pttTile The tile for which to write the packets.
		 * \param _i32MaxLayers Maximum number of layers.
		 * \param _pui8Dest The destination buffer.
		 * \param _i32Len The length of the destination buffer.
		 * \param _pciInfoC Codestream information structure.
		 * \param _i32TpNum Tile part number of the current tile.
		 * \param _i32TpPos The position of the tile part flag in the progression order.
		 * \param _i32PiNo Undocumented OpenJpeg Library parameter.
		 * \param _tmT2Mode If 0 we are in the threshold calculation. If 1 we are in the final pass.
		 * \param _i32CurTotalTp The total number of tile parts in the current tile.
		 * \return Undocumented OpenJpeg Library return.
		 */
		static int32_t LSE_CALL						t2_encode_packets( opj_t2_t * _ptT2, int32_t _i32TileNo, CTcd::opj_tcd_tile_t * _pttTile, int32_t _i32MaxLayers, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC, int32_t _i32TpNum, int32_t _i32TpPos, int32_t _i32PiNo, CJ2k::J2K_T2_MODE _tmT2Mode, int32_t _i32CurTotalTp );

		/**
		 * Decode the packets of a tile from a source buffer.
		 *
		 * \param _ptT2 T2 handle
		 * \param _pui8Src the source buffer
		 * \param _ui32Len length of the source buffer
		 * \param _i32TileNo number that identifies the tile for which to decode the packets
		 * \param _pttTile tile for which to decode the packets
		 * \param _pciInfoC Codestream information structure.
		 * \return Undocumented OpenJpeg Library return.
		 */
		static int32_t LSE_CALL						t2_decode_packets( opj_t2_t * _ptT2, uint8_t * _pui8Src, int32_t _ui32Len, int32_t _i32TileNo, CTcd::opj_tcd_tile_t * _pttTile, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC );

		/**
		 * Creates a T2 handle.
		 *
		 * \param _cpInfo Codec context info.
		 * \param _piImage Source or destination image.
		 * \param _pcCp Image coding parameters.
		 * \return Returns a new T2 handle if successful, NULL otherwise.
		 */
		static opj_t2_t * LSE_CALL					t2_create( CJpegCommonStructs::opj_common_ptr _cpInfo, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp );

		/**
		 * Destroy a T2 handle.
		 *
		 * \param _ptT2 T2 handle to destroy.
		 */
		static void LSE_CALL						t2_destroy( opj_t2_t * _ptT2 );


	protected :
		// == Functions.
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
		 * \param _i32N Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						t2_putcommacode( CBio::opj_bio_t * _pbBio, int32_t _i32N );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL						t2_getcommacode( CBio::opj_bio_t * _pbBio );
		
		/**
		 * Variable length code for signalling delta Zil (truncation point).
		 *
		 * \param _pbBio Bit Input/Output component.
		 * \param _i32N delta Zil.
		 */
		static void LSE_CALL						t2_putnumpasses( CBio::opj_bio_t * _pbBio, int32_t _i32N );

		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL						t2_getnumpasses( CBio::opj_bio_t * _pbBio );

		/**
		 * Encode a packet of a tile to a destination buffer.
		 *
		 * \param _pttTile Tile for which to write the packets.
		 * \param _ptTcp Tile coding parameters.
		 * \param _ppiPi Packet identity.
		 * \param _pui8Dest Destination buffer.
		 * \param _ui32Len Length of the destination buffer.
		 * \param _pciInfoC Codestream information structure.
		 * \param _i32TileNo Number of the tile encoded.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL						t2_encode_packet( CTcd::opj_tcd_tile_t * _pttTile, CJ2k::opj_tcp_t * _ptTcp, CPi::opj_pi_iterator_t * _ppiPi, uint8_t * _pui8Dest, int32_t _ui32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC, int32_t _i32TileNo );
		
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _ptcdCodeBlock Undocumented parameter from the OpenJpeg Library.
		 * \param _i32Index Undocumented parameter from the OpenJpeg Library.
		 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
		 * \param _i32First Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL						t2_init_seg( CTcd::opj_tcd_cblk_dec_t * _ptcdCodeBlock, int32_t _i32Index, int32_t _i32CodeBlockStY, int32_t _i32First );

		/**
		 * Decode a packet of a tile from a source buffer.
		 *
		 * \param _ptT2 T2 handle.
		 * \param _pui8Src Source buffer.
		 * \param _ui32Len Length of the source buffer.
		 * \param _pttTile Tile for which to write the packets.
		 * \param _ptTcp Tile coding parameters.
		 * \param _ppiPi Packet identity.
		 * \param _ppiPackInfo Packet information.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL						t2_decode_packet( opj_t2_t * _ptT2, uint8_t * _pui8Src, int32_t _ui32Len, CTcd::opj_tcd_tile_t * _pttTile,
			CJ2k::opj_tcp_t * _ptTcp, CPi::opj_pi_iterator_t * _ppiPi, CJpegCommonStructs::opj_packet_info_t * _ppiPackInfo );
	};

}	// namespace lsi

#endif	// __LSI_T2_H__
