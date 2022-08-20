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

 function: floor backend 0 implementation
 last mod: $Id: floor0.c 17558 2010-10-22 00:24:41Z tterribe $

 ********************************************************************/


#ifndef __LSS_VORBISFLOOR0_H__
#define __LSS_VORBISFLOOR0_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"
#include "LSSVorbisBackends.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisStructs.h"

namespace lss {

	/**
	 * Class CVorbisFloor0
	 * \brief Floor backend 0 implementation.
	 *
	 * Description: Floor backend 0 implementation.
	 */
	class CVorbisFloor0 {
	public :
		// == Members.
		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_floor	floor0_exportbundle;

	protected :
		// == Types.
		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_look_floor0 {
			int32_t										ln;
			int32_t										m;
			int32_t **									linearmap;
			int32_t										n[2];

			CVorbisBackends::vorbis_info_floor0 *		vi;

			int32_t										bits;
			int32_t										frames;
		} * LPvorbis_look_floor0, * const LPCvorbis_look_floor0;

		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 */
		static void									floor0_free_info( CVorbisStructs::vorbis_info_floor * _pvifI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 */
		static void									floor0_free_look( CVorbisStructs::vorbis_look_floor * _pvifI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_info_floor *		floor0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvifInfoX Undocumented parameter from the Vorbis library.
		 * \param _pvlfLook Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static void									floor0_map_lazy_init( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_info_floor * _pvifInfoX, vorbis_look_floor0 * _pvlfLook );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_look_floor *		floor0_look( CVorbisCodec::vorbis_dsp_state * _pvdsVd, CVorbisStructs::vorbis_info_floor * _pvifI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static void *									floor0_inverse1( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvifI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlfI Undocumented parameter from the Vorbis library.
		 * \param _pvMemo Undocumented parameter from the Vorbis library.
		 * \param _pfOut Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t									floor0_inverse2( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfI, void * _pvMemo, float * _pfOut );
	};

}	// namespace lss

#endif	// __LSS_VORBISFLOOR0_H__

