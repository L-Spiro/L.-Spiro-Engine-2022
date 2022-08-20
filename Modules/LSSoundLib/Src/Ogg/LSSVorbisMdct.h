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

 function: modified discrete cosine transform prototypes
 last mod: $Id: mdct.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISMDCT_H__
#define __LSS_VORBISMDCT_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisConstants.h"

namespace lss {

	/**
	 * Class CVorbisMdct
	 * \brief Modified discrete cosine transform prototypes.
	 *
	 * Description: Modified discrete cosine transform prototypes.
	 */
	class CVorbisMdct {
	public :
		// == Types.
		/**
		 * Vorbis structure.
		 */
		typedef struct mdct_lookup {
			int32_t						n;
			int32_t						log2n;

			DATA_TYPE *					trig;
			int32_t *					bitrev;

			DATA_TYPE					scale;
		} * LPmdct_lookup, * const LPCmdct_lookup;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookUp Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_init( mdct_lookup * _plLookUp, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookUp Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_clear( mdct_lookup * _plLookUp );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plInit Undocumented parameter from the Vorbis library.
		 * \param _pdtIn Undocumented parameter from the Vorbis library.
		 * \param _pdtOut Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_forward( mdct_lookup * _plInit, DATA_TYPE * _pdtIn, DATA_TYPE * _pdtOut );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plInit Undocumented parameter from the Vorbis library.
		 * \param _pdtIn Undocumented parameter from the Vorbis library.
		 * \param _pdtOut Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_backward( mdct_lookup * _plInit, DATA_TYPE * _pdtIn, DATA_TYPE * _pdtOut );


	protected :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plInit Undocumented parameter from the Vorbis library.
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 * \param _i32Points Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterflies( mdct_lookup * _plInit,
			DATA_TYPE * _pdtX,
			int32_t _i32Points );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plInit Undocumented parameter from the Vorbis library.
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_bitreverse( mdct_lookup * _plInit,
			DATA_TYPE * _pdtX );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pdtT Undocumented parameter from the Vorbis library.
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 * \param _i32Points Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterfly_first( DATA_TYPE * _pdtT,
			DATA_TYPE * _pdtX,
			int32_t _i32Points );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pdtT Undocumented parameter from the Vorbis library.
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 * \param _i32Points Undocumented parameter from the Vorbis library.
		 * \param _i32TrigInt Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterfly_generic( DATA_TYPE * _pdtT,
			DATA_TYPE *_pdtX,
			int32_t _i32Points,
			int32_t _i32TrigInt );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterfly_32( DATA_TYPE * _pdtX );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterfly_16( DATA_TYPE * _pdtX );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pdtX Undocumented parameter from the Vorbis library.
		 */
		static void					mdct_butterfly_8( DATA_TYPE * _pdtX );
	};

}	// namespace lss

#endif	// __LSS_VORBISMDCT_H__

