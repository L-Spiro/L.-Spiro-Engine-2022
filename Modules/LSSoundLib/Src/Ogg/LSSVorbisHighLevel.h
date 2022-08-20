/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: highlevel encoder setup struct separated out for vorbisenc clarity
 last mod: $Id: highlevel.h 17195 2010-05-05 21:49:51Z giles $

 ********************************************************************/


#ifndef __LSS_VORBISHIGHLEVEL_H__
#define __LSS_VORBISHIGHLEVEL_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class CVorbisHighLevel
	 * \brief High-level Vorbis structures.
	 *
	 * Description: High-level Vorbis structures.
	 */
	class CVorbisHighLevel {
	public :
		// == Types.
		/**
		 * By-block type.
		 */
		typedef struct highlevel_byblocktype {
			double							tone_mask_setting;
			double							tone_peaklimit_setting;
			double							noise_bias_setting;
			double							noise_compand_setting;
		} * LPhighlevel_byblocktype, * const LPChighlevel_byblocktype;

		/**
		 * Encode setup.
		 */
		typedef struct highlevel_encode_setup {
			int32_t								set_in_stone;
			const void *						setup;
			double							base_setting;

			double							impulse_noisetune;

			// Bitrate management below all settable.
			float								req;
			int32_t								managed;
			int32_t								bitrate_min;
			int32_t								bitrate_av;
			double							bitrate_av_damp;
			int32_t								bitrate_max;
			int32_t								bitrate_reservoir;
			double							bitrate_reservoir_bias;

			int32_t								impulse_block_p;
			int32_t								noise_normalize_p;
			int32_t								coupling_p;

			double							stereo_point_setting;
			double							lowpass_kHz;
			int32_t								lowpass_altered;

			double							ath_floating_dB;
			double							ath_absolute_dB;

			double							amplitude_track_dBpersec;
			double							trigger_setting;

			highlevel_byblocktype				block[4]; // Padding, impulse, transition, long.
		} * LPhighlevel_encode_setup, * const LPChighlevel_encode_setup;
	};

}	// namespace lss

#endif	// __LSS_VORBISHIGHLEVEL_H__

