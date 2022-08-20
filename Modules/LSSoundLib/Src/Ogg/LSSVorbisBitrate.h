/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: bitrate tracking and management
 last mod: $Id: bitrate.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISBITRATE_H__
#define __LSS_VORBISBITRATE_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"
#include "LSSVorbisCodec.h"

namespace lss {

	/**
	 * Class CVorbisBitrate
	 * \brief Bitrate tracking and management.
	 *
	 * Description: Bitrate tracking and management.
	 */
	class CVorbisBitrate {
	public :
		// == Types.
		/**
		 * Encode side bitrate tracking.
		 */
		typedef struct bitrate_manager_state {
			int32_t							managed;

			int32_t							avg_reservoir;
			int32_t							minmax_reservoir;
			int32_t							avg_bitsper;
			int32_t							min_bitsper;
			int32_t							max_bitsper;

			int32_t							short_per_long;
			double						avgfloat;

			CVorbisCodec::vorbis_block *	vb;
			int32_t							choice;
		} * LPbitrate_manager_state, * const LPCbitrate_manager_state;

		/**
		 * Undocumented structure from the Vorbis library.
		 */
		typedef struct bitrate_manager_info {
			int32_t							avg_rate;
			int32_t							min_rate;
			int32_t							max_rate;
			int32_t							reservoir_bits;
			double						reservoir_bias;

			double						slew_damp;

		} * LPbitrate_manager_info, * const LPCbitrate_manager_info;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pbmsBs Undocumented parameter from the Vorbis library.
		 */
		static void						vorbis_bitrate_init( CVorbisCodec::vorbis_info * _pviVi, bitrate_manager_state * _pbmsBs );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pbmsBs Undocumented parameter from the Vorbis library.
		 */
		static void						vorbis_bitrate_clear( bitrate_manager_state * _pbmsBs );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						vorbis_bitrate_managed( CVorbisCodec::vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						vorbis_bitrate_addblock( CVorbisCodec::vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _popOp Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t						vorbis_bitrate_flushpacket( CVorbisCodec::vorbis_dsp_state * _pvdsVd, COgg::ogg_packet * _popOp );

	};

}	// namespace lss

#endif	// __LSS_VORBISBITRATE_H__

