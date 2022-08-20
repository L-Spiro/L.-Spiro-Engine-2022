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


#ifndef __LSI_PI_H__
#define __LSI_PI_H__

#include "../LSIImageLib.h"
#include "LSIJ2k.h"
#include "LSIJpegCommonStructs.h"
#include "LSIJpegImage.h"

namespace lsi {

	/**
	 * Class CPi
	 * \brief Implementation of a packet iterator (PI).
	 *
	 * Description: Implementation of a packet iterator (PI).
	 */
	class CPi {
	public :
		// == Types.
		/**
		 * FIXME: documentation
		 */
		typedef struct opj_pi_resolution {
		  int32_t								pdx, pdy;
		  int32_t								pw, ph;
		} opj_pi_resolution_t;

		/**
		 * FIXME: documentation
		 */
		typedef struct opj_pi_comp {
		  int32_t								dx, dy;
		  /** Number of resolution levels. */
		  int32_t								numresolutions;
		  opj_pi_resolution_t *					resolutions;
		} opj_pi_comp_t;

		/** 
		 * Packet iterator.
		 */
		typedef struct opj_pi_iterator {
			/** Enabling Tile part generation. */
			char								tp_on;
			/** Precise if the packet has been already used (useful for progression order change). */
			int16_t *							include;
			/** Layer step used to localize the packet in the include vector. */
			int32_t								step_l;
			/** Resolution step used to localize the packet in the include vector. */
			int32_t								step_r;
			/** Component step used to localize the packet in the include vector. */
			int32_t								step_c;
			/** Precinct step used to localize the packet in the include vector. */
			int32_t								step_p;
			/** Component that identify the packet. */
			int32_t								compno;
			/** Resolution that identify the packet. */
			int32_t								resno;
			/** Precinct that identify the packet. */
			int32_t								precno;
			/** Layer that identify the packet. */
			int32_t								layno;
			/** 0 if the first packet. */
			int32_t								first;
			/** Progression order change information. */
			CJpegCommonStructs::opj_poc_t		poc;
			/** Number of components in the image. */
			int32_t								numcomps;
			/** Components. */
			opj_pi_comp_t *						comps;
			int32_t								tx0, ty0, tx1, ty1;
			int32_t								x, y, dx, dy;
		} opj_pi_iterator_t;


		// == Functions.
		/**
		 * Creates a packet iterator for the encoder.
		 *
		 * \param _pImage Raw image for which the packets will be listed.
		 * \param _pcCp Coding parameters.
		 * \param _i32TileNo Number that identifies the tile for which to list the packets.
		 * \param _t2Mode If 0 we are in the threshold calculation.  If 1 we are in the final pass.
		 * \return Returns a packet iterator that points to the first packet of the tile.
		 * \see pi_destroy
		 */
		static opj_pi_iterator_t * LSE_CALL		pi_initialise_encode( CJpegImage::opj_image_t * _pImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, CJ2k::J2K_T2_MODE _t2Mode );

		/**
		 * Modify the packet iterator for enabling tile part generation.
		 *
		 * \param _ppiPi Handle to the packet iterator generated in pi_initialise_encode.
		 * \param _pcCp Coding parameters.
		 * \param _i32TileNo Number that identifies the tile for which to list the packets.
		 * \param _i32PiNo Iterator index for _ppiPi.
		 * \param _i32TpNum Tile part number of the current tile.
		 * \param _i32TpPos The position of the tile part flag in the progression order.
		 * \param _t2Mode If 0 we are in the threshold calculation.  If 1 we are in the final pass.
		 * \param _i32CurTotTp The total number of tile parts in the current tile.
		 * \return Returns true if an error is detected.
		 */
		static LSBOOL LSE_CALL					pi_create_encode( opj_pi_iterator_t * _ppiPi, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, int32_t _i32PiNo, int32_t _i32TpNum, int32_t _i32TpPos, CJ2k::J2K_T2_MODE _t2Mode, int32_t _i32CurTotTp );

		/**
		 * Creates a packet iterator for the decoder.
		 *
		 * \param _piImage Raw image for which the packets will be listed.
		 * \param _pcCp Coding parameters.
		 * \param _i32TileNo Number that identifies the tile for which to list the packets.
		 * \return Returns a packet iterator that points to the first packet of the tile.
		 * \see pi_destroy
		 */
		static opj_pi_iterator_t * LSE_CALL		pi_create_decode( CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo );

		/**
		 * Destroy a packet iterator.
		 *
		 * \param _ppiPi Previously created packet iterator.
		 * \param _pcCp Coding parameters.
		 * \param _i32TileNo Number that identifies the tile for which the packets were listed.
		 * \see pi_create
		 */
		static void LSE_CALL					pi_destroy( opj_pi_iterator_t * _ppiPi, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo );

		/**
		 * Modify the packet iterator to point to the next packet.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet or else returns true.
		 */
		static LSBOOL LSE_CALL					pi_next( opj_pi_iterator_t * _ppiPi );


	protected :
		// == Functions.
		/**
		 * Gets next packet in layer-resolution-component-precinct order.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
		 */
		static LSBOOL LSE_CALL					pi_next_lrcp( opj_pi_iterator_t * _ppiPi );

		/**
		 * Gets next packet in resolution-layer-component-precinct order.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
		 */
		static LSBOOL LSE_CALL					pi_next_rlcp( opj_pi_iterator_t * _ppiPi );

		/**
		 * Gets next packet in resolution-precinct-component-layer order.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
		 */
		static LSBOOL LSE_CALL					pi_next_rpcl( opj_pi_iterator_t * _ppiPi );

		/**
		 * Gets next packet in precinct-component-resolution-layer order.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
		 */
		static LSBOOL LSE_CALL					pi_next_pcrl( opj_pi_iterator_t * _ppiPi );

		/**
		 * Gets next packet in component-precinct-resolution-layer order.
		 *
		 * \param _ppiPi Packet iterator to modify.
		 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
		 */
		static LSBOOL LSE_CALL					pi_next_cprl( opj_pi_iterator_t * _ppiPi );

	};

}	// namespace lsi

#endif	// __LSI_PI_H__

