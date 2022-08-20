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

  function: LPC low level routines
  last mod: $Id: lpc.h 16037 2009-05-26 21:10:58Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISLPC_H__
#define __LSS_VORBISLPC_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class CVorbisLpc
	 * \brief LPC low level routines.
	 *
	 * Description: LPC low level routines.
	 */
	class CVorbisLpc {
	public :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfData Undocumented parameter from the Vorbis library.
		 * \param _pfLpcI Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32M Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float							vorbis_lpc_from_data( float * _pfData, float * _pfLpcI, int32_t _i32N, int32_t _i32M );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfCoeff Undocumented parameter from the Vorbis library.
		 * \param _pfPrime Undocumented parameter from the Vorbis library.
		 * \param _i32M Undocumented parameter from the Vorbis library.
		 * \param _pfData Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 */
		static void							vorbis_lpc_predict( float * _pfCoeff, float * _pfPrime, int32_t _i32M,
			float * _pfData, int32_t _i32N );

		// == Members.
	};

}	// namespace lss

#endif	// __LSS_VORBISLPC_H__

