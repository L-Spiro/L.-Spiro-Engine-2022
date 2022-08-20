/********************************************************************
 *                                                                 *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                 *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2009             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                 *
 ********************************************************************

 function: random psychoacoustics (not including preecho)
 last mod: $Id: psy.h 16946 2010-03-03 16:12:40Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISPSY_H__
#define __LSS_VORBISPSY_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisConstants.h"
#include "LSSVorbisMasking.h"
#include "LSSVorbisStructs.h"

namespace lss {

	/**
	 * Class CVorbisPsy
	 * \brief Random psychoacoustics (not including pre-echo).
	 *
	 * Description: Random psychoacoustics (not including pre-echo).
	 */
	class CVorbisPsy {
	public :
		// == Types.
		/**
		 * Psychoacoustic info.
		 */
		typedef struct vorbis_info_psy {
			int32_t						blockflag;

			float						ath_adjatt;
			float						ath_maxatt;

			float						tone_masteratt[P_NOISECURVES];
			float						tone_centerboost;
			float						tone_decay;
			float						tone_abs_limit;
			float						toneatt[P_BANDS];

			int32_t						noisemaskp;
			float						noisemaxsupp;
			float						noisewindowlo;
			float						noisewindowhi;
			int32_t						noisewindowlomin;
			int32_t						noisewindowhimin;
			int32_t						noisewindowfixed;
			float						noiseoff[P_NOISECURVES][P_BANDS];
			float						noisecompand[NOISE_COMPAND_LEVELS];

			float						max_curve_dB;

			int32_t						normal_p;
			int32_t						normal_start;
			int32_t						normal_partition;
			double					normal_thresh;
		} * LPvorbis_info_psy, * const LPCvorbis_info_psy;

		/**
		 * Psychoacoustic global info.
		 */
		typedef struct vorbis_info_psy_global {
			int32_t						eighth_octave_lines;

			// For block int32_t/short tuning; encode only.
			float						preecho_thresh[VE_BANDS];
			float						postecho_thresh[VE_BANDS];
			float						stretch_penalty;
			float						preecho_minenergy;

			float						ampmax_att_per_sec;

			// Channel coupling config.
			int32_t						coupling_pkHz[PACKETBLOBS];
			int32_t						coupling_pointlimit[2][PACKETBLOBS];
			int32_t						coupling_prepointamp[PACKETBLOBS];
			int32_t						coupling_postpointamp[PACKETBLOBS];
			int32_t						sliding_lowpass[2][PACKETBLOBS];

		} * LPvorbis_info_psy_global, * const LPCvorbis_info_psy_global;

		/**
		 * Psychoacoustic global looking info.
		 */
		typedef struct vorbis_look_psy_global {
			float						ampmax;
			int32_t						channels;

			vorbis_info_psy_global *	gi;
			int32_t						coupling_pointlimit[2][P_NOISECURVES];
		} * LPvorbis_look_psy_global, * const LPCvorbis_look_psy_global;

		/**
		 * Psychoacoustic looking info.
		 */
		typedef struct vorbis_look_psy {
			int32_t						n;
			vorbis_info_psy *			vi;

			float ***					tonecurves;
			float **					noiseoffset;

			float *					ath;
			int32_t *					octave;					// In n.ocshift format.
			int32_t *					bark;

			int32_t						firstoc;
			int32_t						shiftoc;
			int32_t						eighth_octave_lines;	// Power of two, please.
			int32_t						total_octave_lines;
			int32_t						rate;					// Cache it.

			float						m_val;					// Masking compensation value.

		} * LPvorbis_look_psy, * LPCvorbis_look_psy;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 * \param _pipVi Undocumented parameter from the Vorbis library.
		 * \param _pipgGi Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32Rate Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_psy_init( vorbis_look_psy * _plpP, vorbis_info_psy * _pipVi, 
			vorbis_info_psy_global * _pipgGi, int32_t _i32N, int32_t _i32Rate );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_psy_clear( vorbis_look_psy * _plpP );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvSource Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static void * 				_vi_psy_dup( void * _pvSource );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pipI Undocumented parameter from the Vorbis library.
		 */
		static void					_vi_psy_free( vorbis_info_psy * _pipI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pipI Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static vorbis_info_psy * 		_vi_psy_copy( vorbis_info_psy * _pipI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
		 * \param _pfLogMask Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_noisemask( vorbis_look_psy * _plpP, 
			float * _pfLogMdct, 
			float * _pfLogMask );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 * \param _pfLogFft Undocumented parameter from the Vorbis library.
		 * \param _pfLogMask Undocumented parameter from the Vorbis library.
		 * \param _fGlobalSpecMax Undocumented parameter from the Vorbis library.
		 * \param _fLocalSpecMax Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_tonemask( vorbis_look_psy * _plpP, 
			float * _pfLogFft, 
			float * _pfLogMask, 
			float _fGlobalSpecMax, 
			float _fLocalSpecMax );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 * \param _pfNoise Undocumented parameter from the Vorbis library.
		 * \param _pfTone Undocumented parameter from the Vorbis library.
		 * \param _i32OffsetSelect Undocumented parameter from the Vorbis library.
		 * \param _pfLogMask Undocumented parameter from the Vorbis library.
		 * \param _pfMdct Undocumented parameter from the Vorbis library.
		 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_offset_and_mix( vorbis_look_psy * _plpP, 
			float * _pfNoise, 
			float * _pfTone, 
			int32_t _i32OffsetSelect, 
			float * _pfLogMask, 
			float * _pfMdct, 
			float * _pfLogMdct );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _fAmp Undocumented parameter from the Vorbis library.
		 * \param _pdsVd Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float					_vp_ampmax_decay( float _fAmp, CVorbisCodec::vorbis_dsp_state * _pdsVd );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32BlobNo Undocumented parameter from the Vorbis library.
		 * \param _pipgG Undocumented parameter from the Vorbis library.
		 * \param _plpP Undocumented parameter from the Vorbis library.
		 * \param _pipVi Undocumented parameter from the Vorbis library.
		 * \param _pfMdct Undocumented parameter from the Vorbis library.
		 * \param _ppi32Work Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32SlidingLowPass Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 */
		static void					_vp_couple_quantize_normalize( int32_t _i32BlobNo, 
			vorbis_info_psy_global * _pipgG, 
			vorbis_look_psy * _plpP, 
			CVorbisStructs::vorbis_info_mapping0 * _pipVi, 
			float ** _pfMdct, 
			int32_t ** _ppi32Work, 
			int32_t * _pi32NonZero, 
			int32_t _i32SlidingLowPass, 
			int32_t _i32Ch );


		protected :
			// == Functions.
			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _fCurveAttDb Undocumented parameter from the Vorbis library.
			 * \param _fBinHz Undocumented parameter from the Vorbis library.
			 * \param _i32N Undocumented parameter from the Vorbis library.
			 * \param _fCenterBoost Undocumented parameter from the Vorbis library.
			 * \param _fCenterDecayRate Undocumented parameter from the Vorbis library.
			 * \return Undocumented return from the Vorbis library.
			 */
			static float ***			setup_tone_curves( float _fCurveAttDb[P_BANDS], float _fBinHz, int32_t _i32N,
				float _fCenterBoost, float _fCenterDecayRate );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pfC Undocumented parameter from the Vorbis library.
			 * \param _pfC2 Undocumented parameter from the Vorbis library.
			 */
			static void				min_curve( float * _pfC, float * _pfC2 );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pfC Undocumented parameter from the Vorbis library.
			 * \param _pfC2 Undocumented parameter from the Vorbis library.
			 */
			static void				max_curve( float * _pfC, float * _pfC2 );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pfC Undocumented parameter from the Vorbis library.
			 * \param _fAtt Undocumented parameter from the Vorbis library.
			 */
			static void				attenuate_curve( float * _pfC, float _fAtt );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _i32N Undocumented parameter from the Vorbis library.
			 * \param _pi32B Undocumented parameter from the Vorbis library.
			 * \param _pfF Undocumented parameter from the Vorbis library.
			 * \param _pfNoise Undocumented parameter from the Vorbis library.
			 * \param _fOffset Undocumented parameter from the Vorbis library.
			 * \param _i32Fixed Undocumented parameter from the Vorbis library.
			 */
			static void				bark_noise_hybridmp( int32_t _i32N, const int32_t * _pi32B, 
				const float * _pfF, 
				float * _pfNoise, 
				const float _fOffset, 
				const int32_t _i32Fixed );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _plpP Undocumented parameter from the Vorbis library.
			 * \param _pppfCurves Undocumented parameter from the Vorbis library.
			 * \param _pfF Undocumented parameter from the Vorbis library.
			 * \param _pfFlr Undocumented parameter from the Vorbis library.
			 * \param _pfSeed Undocumented parameter from the Vorbis library.
			 * \param _fSpecMax Undocumented parameter from the Vorbis library.
			 */
			static void				seed_loop( vorbis_look_psy * _plpP,
				const float *** _pppfCurves,
				const float * _pfF,
				const float * _pfFlr,
				float * _pfSeed,
				float _fSpecMax );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _plpP Undocumented parameter from the Vorbis library.
			 * \param _pfSeed Undocumented parameter from the Vorbis library.
			 * \param _pfFlr Undocumented parameter from the Vorbis library.
			 */
			static void				max_seeds( vorbis_look_psy * _plpP,
				float * _pfSeed,
				float * _pfFlr );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pfSeed Undocumented parameter from the Vorbis library.
			 * \param _pppfCurves Undocumented parameter from the Vorbis library.
			 * \param _fAmp Undocumented parameter from the Vorbis library.
			 * \param _i32Oc Undocumented parameter from the Vorbis library.
			 * \param _i32N Undocumented parameter from the Vorbis library.
			 * \param _i32LinesPer Undocumented parameter from the Vorbis library.
			 * \param _fDbOffset Undocumented parameter from the Vorbis library.
			 */
			static void				seed_curve( float * _pfSeed,
				const float ** _pppfCurves,
				float _fAmp,
				int32_t _i32Oc, int32_t _i32N,
				int32_t _i32LinesPer,
				float _fDbOffset );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pfSeeds Undocumented parameter from the Vorbis library.
			 * \param _i32LinesPer Undocumented parameter from the Vorbis library.
			 * \param _i32N Undocumented parameter from the Vorbis library.
			 */
			static void				seed_chase( float * _pfSeeds, int32_t _i32LinesPer, int32_t _i32N );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _i32Limit Undocumented parameter from the Vorbis library.
			 * \param _fPrePoint Undocumented parameter from the Vorbis library.
			 * \param _fPostPoint Undocumented parameter from the Vorbis library.
			 * \param _pfMdct Undocumented parameter from the Vorbis library.
			 * \param _pfFloor Undocumented parameter from the Vorbis library.
			 * \param _pi32Flag Undocumented parameter from the Vorbis library.
			 * \param _i32I Undocumented parameter from the Vorbis library.
			 * \param _i32Jn Undocumented parameter from the Vorbis library.
			 */
			static void				flag_lossless( int32_t _i32Limit, float _fPrePoint, float _fPostPoint, float * _pfMdct,
				float * _pfFloor, int32_t * _pi32Flag, int32_t _i32I, int32_t _i32Jn );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _plpP Undocumented parameter from the Vorbis library.
			 * \param _i32Limit Undocumented parameter from the Vorbis library.
			 * \param _pfR Undocumented parameter from the Vorbis library.
			 * \param _pfQ Undocumented parameter from the Vorbis library.
			 * \param _pfF Undocumented parameter from the Vorbis library.
			 * \param _pi32Flags Undocumented parameter from the Vorbis library.
			 * \param _pfAcc Undocumented parameter from the Vorbis library.
			 * \param _i32I Undocumented parameter from the Vorbis library.
			 * \param _i32N Undocumented parameter from the Vorbis library.
			 * \param _pi32Out Undocumented parameter from the Vorbis library.
			 * \return Undocumented return from the Vorbis library.
			 */
			static float				noise_normalize( vorbis_look_psy * _plpP, int32_t _i32Limit, float * _pfR, float * _pfQ, float * _pfF, int32_t * _pi32Flags, float _pfAcc, int32_t _i32I, int32_t _i32N, int32_t * _pi32Out );

			/**
			 * Undocumented function from the Vorbis library.
			 *
			 * \param _pvContext Unused.
			 * \param _pvA Undocumented parameter from the Vorbis library.
			 * \param _pvB Undocumented parameter from the Vorbis library.
			 * \return Undocumented return from the Vorbis library.
			 */
			static int32_t LSE_CCALL	apsort( void * _pvContext, const void * _pvA, const void * _pvB );
	};

}	// namespace lss

#endif	// __LSS_VORBISPSY_H__

