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

 function: PCM data envelope analysis and manipulation
 last mod: $Id: envelope.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISENVELOPE_H__
#define __LSS_VORBISENVELOPE_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisConstants.h"
#include "LSSVorbisMdct.h"
#include "LSSVorbisPsy.h"

namespace lss {

	/**
	 * Class CVorbisEnvelope
	 * \brief Vorbis PCM data envelope analysis and manipulation.
	 *
	 * Description: Vorbis PCM data envelope analysis and manipulation.
	 */
	class CVorbisEnvelope {
	public :
		// == Types.
		/**
		 * Envelope filter state.
		 */
		typedef struct envelope_filter_state {
			float								ampbuf[VE_AMP];
			int32_t								ampptr;

			float								nearDC[VE_NEARDC];
			float								nearDC_acc;
			float								nearDC_partialacc;
			int32_t								nearptr;
		} * LPenvelope_filter_state, * const LPCenvelope_filter_state;

		/**
		 * Envelope band.
		 */
		typedef struct envelope_band {
			int32_t								begin;
			int32_t								end;
			float *							window;
			float								total;
		} * LPenvelope_band, * const LPCenvelope_band;

		/**
		 * Envelope look-up.
		 */
		typedef struct envelope_lookup {
			int32_t								ch;
			int32_t								winlength;
			int32_t								searchstep;
			float								minenergy;

			CVorbisMdct::mdct_lookup			mdct;
			float *							mdct_win;

			envelope_band						band[VE_BANDS];
			envelope_filter_state *				filter;
			int32_t								stretch;

			int32_t *							mark;

			int32_t								storage;
			int32_t								current;
			int32_t								curmark;
			int32_t								cursor;
		} * LPenvelope_lookup, * const LPCenvelope_lookup;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pelE Undocumented parameter from the Vorbis library.
		 * \param _piVi Undocumented parameter from the Vorbis library.
		 */
		static void							_ve_envelope_init( envelope_lookup * _pelE, CVorbisCodec::vorbis_info * _piVi );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pelE Undocumented parameter from the Vorbis library.
		 */
		static void							_ve_envelope_clear( envelope_lookup * _pelE );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_ve_envelope_search( CVorbisCodec::vorbis_dsp_state * _pvdsV );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pelE Undocumented parameter from the Vorbis library.
		 * \param _i32Shift Undocumented parameter from the Vorbis library.
		 */
		static void							_ve_envelope_shift( envelope_lookup * _pelE, int32_t _i32Shift );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsV Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_ve_envelope_mark( CVorbisCodec::vorbis_dsp_state * _pvdsV );


	protected :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pelVe Undocumented parameter from the Vorbis library.
		 * \param _pvisgGi Undocumented parameter from the Vorbis library.
		 * \param _pfData Undocumented parameter from the Vorbis library.
		 * \param _pebBands Undocumented parameter from the Vorbis library.
		 * \param _pefsFilters Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_ve_amp( envelope_lookup * _pelVe,
			CVorbisPsy::vorbis_info_psy_global * _pvisgGi,
			float * _pfData,
			envelope_band * _pebBands,
			envelope_filter_state * _pefsFilters );
	};

}	// namespace lss

#endif	// __LSS_VORBISENVELOPE_H__

