/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2010             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: channel mapping 0 implementation
 last mod: $Id: mapping0.c 17022 2010-03-25 03:45:42Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISMAPPING0_H__
#define __LSS_VORBISMAPPING0_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisBackends.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisStructs.h"

namespace lss {

	/**
	 * Class CVorbisMapping0
	 * \brief Channel mapping 0 implementation.
	 *
	 * Description: Channel mapping 0 implementation.
	 */
	class CVorbisMapping0 {
	public :
		// == Members.
		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_mapping					mapping0_exportbundle;


	protected :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pvimVm Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 */
		static void														mapping0_pack( CVorbisCodec::vorbis_info * _pviVi, CVorbisStructs::vorbis_info_mapping * _pvimVm, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_info_mapping *						mapping0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvmiI Undocumented parameter from the Vorbis library.
		 */
		static void														mapping0_free_info( CVorbisStructs::vorbis_info_mapping * _pvmiI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t														mapping0_forward( CVorbisCodec::vorbis_block * _pvbVb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvimL Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t														mapping0_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_info_mapping * _pvimL );


	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t														ilog( uint32_t _ui32V );
	};

}	// namespace lss

#endif	// __LSS_VORBISMAPPING0_H__

