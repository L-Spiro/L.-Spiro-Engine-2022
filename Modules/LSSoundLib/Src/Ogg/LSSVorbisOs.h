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

 function: #ifdef jail to whip a few platforms into the UNIX ideal.
 last mod: $Id: os.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISOS_H__
#define __LSS_VORBISOS_H__

#include "../LSSSoundLib.h"

namespace lss {

	/**
	 * Class CVorbisOs
	 * \brief Functions/types that change per compiler/operating system.
	 *
	 * Description: Functions/types that change per compiler/operating system.
	 */
	class CVorbisOs {
		// All is public.  This class has no secrets.
	public :
		// == Types.
		/**
		 * Undocumented Vorbis library type.
		 */
		typedef int16_t							vorbis_fpu_control;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _dVal Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_ftoi( double _dVal );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvfcFpu Undocumented parameter from the Vorbis library.
		 */
		static void							vorbis_fpu_setround( vorbis_fpu_control * _pvfcFpu );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _vfcFpu Undocumented parameter from the Vorbis library.
		 */
		static void							vorbis_fpu_restore( vorbis_fpu_control _vfcFpu );
	};

}	// namespace lss

#endif	// __LSS_VORBISOS_H__

