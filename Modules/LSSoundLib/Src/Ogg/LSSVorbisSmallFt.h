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

 function: fft transform
 last mod: $Id: smallft.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISSMALLFT_H__
#define __LSS_VORBISSMALLFT_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class CVorbisSmallFt
	 * \brief Fast Fourier transform
	 *
	 * Description: Fast Fourier transform.
	 */
	class CVorbisSmallFt {
	public :
		// == Types.
		/**
		 * A look-up structure.
		 */
		typedef struct drft_lookup {
			int32_t						n;
			float *					trigcache;
			int32_t *					splitcache;
		} * LPdrft_lookup, * const LPCdrft_lookup;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookup Undocumented parameter from the Vorbis library.
		 * \param _pfData Undocumented parameter from the Vorbis library.
		 */
		static void					drft_forward( drft_lookup * _plLookup, float * _pfData );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookup Undocumented parameter from the Vorbis library.
		 * \param _pfData Undocumented parameter from the Vorbis library.
		 */
		static void					drft_backward( drft_lookup * _plLookup, float * _pfData );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookup Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 */
		static void					drft_init( drft_lookup * _plLookup, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _plLookup Undocumented parameter from the Vorbis library.
		 */
		static void					drft_clear( drft_lookup * _plLookup );


	protected :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pfC Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa Undocumented parameter from the Vorbis library.
		 * \param _pi32Fac Undocumented parameter from the Vorbis library.
		 */
		static void					drftf1( int32_t _i32N, float * _pfC, float * _pfCh, float * _pfWa, int32_t * _pi32Fac );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pfC Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa Undocumented parameter from the Vorbis library.
		 * \param _pi32Fac Undocumented parameter from the Vorbis library.
		 */
		static void					drftb1( int32_t _i32N, float * _pfC, float * _pfCh, float * _pfWa, int32_t * _pi32Fac );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pfWSave Undocumented parameter from the Vorbis library.
		 * \param _pi32Fac Undocumented parameter from the Vorbis library.
		 */
		static void					fdrffti( int32_t _i32N, float * _pfWSave, int32_t * _pi32Fac );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa1 Undocumented parameter from the Vorbis library.
		 * \param _pfWa2 Undocumented parameter from the Vorbis library.
		 * \param _pfWa3 Undocumented parameter from the Vorbis library.
		 */
		static void					dradf4( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1,
			float * _pfWa2, float * _pfWa3 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa1 Undocumented parameter from the Vorbis library.
		 */
		static void					dradf2( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32Ip Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param idl1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfC1 Undocumented parameter from the Vorbis library.
		 * \param _pfC2 Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfCh2 Undocumented parameter from the Vorbis library.
		 * \param _pfWa Undocumented parameter from the Vorbis library.
		 */
		static void					dradfg( int32_t _i32Ido, int32_t _i32Ip, int32_t _i32L1, int32_t idl1, float * _pfCc, float * _pfC1,
			float * _pfC2, float * _pfCh, float * _pfCh2, float * _pfWa );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa1 Undocumented parameter from the Vorbis library.
		 * \param _pfWa2 Undocumented parameter from the Vorbis library.
		 * \param _pfWa3 Undocumented parameter from the Vorbis library.
		 */
		static void					dradb4( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1,
			float * _pfWa2, float * _pfWa3 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa1 Undocumented parameter from the Vorbis library.
		 */
		static void					dradb2( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfWa1 Undocumented parameter from the Vorbis library.
		 * \param _pfWa2 Undocumented parameter from the Vorbis library.
		 */
		static void					dradb3( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1,
			float * _pfWa2 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Ido Undocumented parameter from the Vorbis library.
		 * \param _i32Ip Undocumented parameter from the Vorbis library.
		 * \param _i32L1 Undocumented parameter from the Vorbis library.
		 * \param idl1 Undocumented parameter from the Vorbis library.
		 * \param _pfCc Undocumented parameter from the Vorbis library.
		 * \param _pfC1 Undocumented parameter from the Vorbis library.
		 * \param _pfC2 Undocumented parameter from the Vorbis library.
		 * \param _pfCh Undocumented parameter from the Vorbis library.
		 * \param _pfCh2 Undocumented parameter from the Vorbis library.
		 * \param _pfWa Undocumented parameter from the Vorbis library.
		 */
		static void					dradbg( int32_t _i32Ido, int32_t _i32Ip, int32_t _i32L1, int32_t idl1, float * _pfCc, float * _pfC1, 
			float * _pfC2, float * _pfCh, float * _pfCh2, float * _pfWa );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pfWa Undocumented parameter from the Vorbis library.
		 * \param _pi32Fac Undocumented parameter from the Vorbis library.
		 */
		static void					drfti1( int32_t _i32N, float * _pfWa, int32_t * _pi32Fac );
	};

}	// namespace lss

#endif	// __LSS_VORBISSMALLFT_H__

