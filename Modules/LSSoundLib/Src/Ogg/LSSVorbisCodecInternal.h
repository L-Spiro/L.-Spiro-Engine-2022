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

 function: libvorbis codec headers
 last mod: $Id: codec_internal.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISCODECINTERNAL_H__
#define __LSS_VORBISCODECINTERNAL_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"
#include "LSSVorbisBackends.h"
#include "LSSVorbisBitrate.h"
#include "LSSVorbisCodeBook.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisConstants.h"
#include "LSSVorbisEnvelope.h"
#include "LSSVorbisHighLevel.h"
#include "LSSVorbisPsy.h"
#include "LSSVorbisSmallFt.h"

namespace lss {

	/**
	 * Class CVorbisCodecInternal
	 * \brief Vorbis codec internals.
	 *
	 * Description: Vorbis codec internals.
	 */
	class CVorbisCodecInternal {
	public :
		// == Types.
		typedef struct vorbis_block_internal {
			float **								pcmdelay;					// This is a pointer into local storage.
			float									ampmax;
			int32_t									blocktype;

			COgg::oggpack_buffer *					packetblob[PACKETBLOBS];	// Initialized, must be freed;
																				//	blob [PACKETBLOBS/2] points to
																				//	the oggpack_buffer in the
																				//	main vorbis_block.
		} * LPvorbis_block_internal, * const LPCvorbis_block_internal;

		/**
		 * Private state.
		 */
		typedef struct private_state {
			// Local lookup storage.
			CVorbisEnvelope::envelope_lookup *		ve;				// Envelope lookup.
			int32_t									window[2];
			CVorbisStructs::vorbis_look_transform **transform[2];	// block, type.
			CVorbisSmallFt::drft_lookup					fft_look[2];

			int32_t									modebits;
			CVorbisStructs::vorbis_look_floor **	flr;
			CVorbisStructs::vorbis_look_residue **	residue;
			CVorbisPsy::vorbis_look_psy *			psy;
			CVorbisPsy::vorbis_look_psy_global *	psy_g_look;

			// Local storage, only used on the encoding side.  This way the
			//	application does not need to worry about freeing some packets'
			//	memory and not others'; packet storage is always tracked.
			//	Cleared next call to a _dsp_ function.
			uint8_t *								header;
			uint8_t *								header1;
			uint8_t *								header2;

			CVorbisBitrate::bitrate_manager_state	bms;

			int64_t									sample_count;
		} private_state;

		/**
		 * Setup information.
		 */
		typedef struct codec_setup_info {
			// Vorbis supports only short and int32_t blocks, but allows the
			//	encoder to choose the sizes.
			int32_t									blocksizes[2];

			// Modes are the primary means of supporting on-the-fly different
			//	blocksizes, different channel mappings (LR or M/A),
			//	different residue backends, etc.  Each mode consists of a
			//	blocksize flag and a mapping (along with the mapping setup.

			int32_t									modes;
			int32_t									maps;
			int32_t									floors;
			int32_t									residues;
			int32_t									books;
			int32_t									psys;			// Encode only.

			CVorbisStructs::vorbis_info_mode *		mode_param[64];
			int32_t									map_type[64];
			CVorbisStructs::vorbis_info_mapping *	map_param[64];
			int32_t									floor_type[64];
			CVorbisStructs::vorbis_info_floor *		floor_param[64];
			int32_t									residue_type[64];
			CVorbisStructs::vorbis_info_residue *	residue_param[64];
			CVorbisCodeBook::static_codebook *		book_param[256];
			CVorbisCodeBook::codebook *				fullbooks;

			CVorbisPsy::vorbis_info_psy *			psy_param[4];	// Encode only.
			CVorbisPsy::vorbis_info_psy_global		psy_g_param;

			CVorbisBitrate::bitrate_manager_info	bi;
			CVorbisHighLevel::highlevel_encode_setup
													hi;
			int32_t									halfrate_flag;	// Painless downsample for decode.
		} codec_setup_info;

		/**
		 * Undocumented Vorbis library structure.
		 */
		typedef struct vorbis_look_floor1 {
			int32_t									sorted_index[VIF_POSIT+2];
			int32_t									forward_index[VIF_POSIT+2];
			int32_t									reverse_index[VIF_POSIT+2];

			int32_t									hineighbor[VIF_POSIT];
			int32_t									loneighbor[VIF_POSIT];
			int32_t									posts;

			int32_t									n;
			int32_t									quant_q;
			CVorbisBackends::vorbis_info_floor1 *	vi;

			int32_t									phrasebits;
			int32_t									postbits;
			int32_t									frames;
		} * LPvorbis_look_floor1, * const LPCvorbis_look_floor1;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisPsy::vorbis_look_psy_global *	_vp_global_look( CVorbisCodec::vorbis_info * _pviVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvlpgLook Undocumented parameter from the Vorbis library.
		 */
		static void								_vp_global_free( CVorbisPsy::vorbis_look_psy_global * _pvlpgLook );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pvlfLook Undocumented parameter from the Vorbis library.
		 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
		 * \param _pfLogMask Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t *							floor1_fit( CVorbisCodec::vorbis_block * _pvbVb, vorbis_look_floor1 * _pvlfLook,
			const float * _pfLogMdct,
			const float * _pfLogMask );
		
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pvlfLook Undocumented parameter from the Vorbis library.
		 * \param _pi32A Undocumented parameter from the Vorbis library.
		 * \param _pi32B Undocumented parameter from the Vorbis library.
		 * \param _i32Del Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t *							floor1_interpolate_fit( CVorbisCodec::vorbis_block * _pvbVb, vorbis_look_floor1 * _pvlfLook,
			int32_t * _pi32A, int32_t * _pi32B,
			int32_t _i32Del );
		
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _poOpb Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlfLook Undocumented parameter from the Vorbis library.
		 * \param _pi32Post Undocumented parameter from the Vorbis library.
		 * \param _pi32LogMaskI Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								floor1_encode( COgg::oggpack_buffer * _poOpb, CVorbisCodec::vorbis_block * _pvbVb,
			vorbis_look_floor1 * _pvlfLook,
			int32_t * _pi32Post, int32_t * _pi32LogMaskI );


	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfFlr Undocumented parameter from the Vorbis library.
		 * \param _pfMdct Undocumented parameter from the Vorbis library.
		 * \param _i32X0 Undocumented parameter from the Vorbis library.
		 * \param _i32X1 Undocumented parameter from the Vorbis library.
		 * \param _plaA Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pvfiInfo Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								accumulate_fit( const float * _pfFlr, const float * _pfMdct,
			int32_t _i32X0, int32_t _i32X1, CVorbisStructs::lsfit_acc * _plaA,
			int32_t _i32N, CVorbisBackends::vorbis_info_floor1 * _pvfiInfo );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plaA Undocumented parameter from the Vorbis library.
		 * \param _i32Fits Undocumented parameter from the Vorbis library.
		 * \param _pi32Y0 Undocumented parameter from the Vorbis library.
		 * \param _pi32Y1 Undocumented parameter from the Vorbis library.
		 * \param _pvifInfo Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								fit_line( CVorbisStructs::lsfit_acc * _plaA, int32_t _i32Fits, int32_t * _pi32Y0, int32_t * _pi32Y1,
			CVorbisBackends::vorbis_info_floor1 * _pvifInfo );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pi32A Undocumented parameter from the Vorbis library.
		 * \param _pi32B Undocumented parameter from the Vorbis library.
		 * \param _i32Pos Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								post_Y( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Pos );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32X0 Undocumented parameter from the Vorbis library.
		 * \param _i32X1 Undocumented parameter from the Vorbis library.
		 * \param _i32Y0 Undocumented parameter from the Vorbis library.
		 * \param _i32Y1 Undocumented parameter from the Vorbis library.
		 * \param _pfMask Undocumented parameter from the Vorbis library.
		 * \param _pfMdct Undocumented parameter from the Vorbis library.
		 * \param _pvifInfo Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								inspect_error( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, const float * _pfMask,
			const float * _pfMdct,
			CVorbisBackends::vorbis_info_floor1 * _pvifInfo );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32X0 Undocumented parameter from the Vorbis library.
		 * \param _i32X1 Undocumented parameter from the Vorbis library.
		 * \param _i32Y0 Undocumented parameter from the Vorbis library.
		 * \param _i32Y1 Undocumented parameter from the Vorbis library.
		 * \param _i32X Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								render_point( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t _i32X );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfX Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								vorbis_dBquant( const float * _pfX );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t								ilog( uint32_t _ui32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32X0 Undocumented parameter from the Vorbis library.
		 * \param _i32X1 Undocumented parameter from the Vorbis library.
		 * \param _i32Y0 Undocumented parameter from the Vorbis library.
		 * \param _i32Y1 Undocumented parameter from the Vorbis library.
		 * \param _pi32D Undocumented parameter from the Vorbis library.
		 */
		static void								render_line0( int32_t _i32N, int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t * _pi32D );

	};

}	// namespace lss

#endif	// __LSS_VORBISCODECINTERNAL_H__

