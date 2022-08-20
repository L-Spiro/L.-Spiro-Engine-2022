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

 function: miscellaneous prototypes
 last mod: $Id: misc.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISMISC_H__
#define __LSS_VORBISMISC_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisCodec.h"

namespace lss {

	/**
	 * Class CVorbisMisc
	 * \brief Miscellaneous prototypes.
	 *
	 * Description: Miscellaneous prototypes.
	 */
	class CVorbisMisc {
	public :

		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _i32Bytes Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static void *						_vorbis_block_alloc( CVorbisCodec::vorbis_block * _pvbVb, int32_t _i32Bytes );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 */
		static void						_vorbis_block_ripcord( CVorbisCodec::vorbis_block * _pvbVb );
	};

}	// namespace lss

#endif	// __LSS_VORBISMISC_H__

