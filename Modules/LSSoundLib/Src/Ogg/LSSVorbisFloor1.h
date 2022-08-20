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

 function: floor backend 1 implementation
 last mod: $Id: floor1.c 17555 2010-10-21 18:14:51Z tterribe $

 ********************************************************************/


#ifndef __LSS_VORBISFLOOR1_H__
#define __LSS_VORBISFLOOR1_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"
#include "LSSVorbisBackends.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisStructs.h"

namespace lss {

	/**
	 * Class CVorbisFloor1
	 * \brief Floor backend 1 implementation.
	 *
	 * Description: Floor backend 1 implementation.
	 */
	class CVorbisFloor1 {
	public :
		// == Members.
		/**
		 * Undocumented Vorbis library data.
		 */
		static const CVorbisBackends::vorbis_func_floor	floor1_exportbundle;

	protected :
		// == Types.
		/**
		 * Undocumented Vorbis library structure.
		 */
		typedef struct lsfit_acc {
			int32_t										x0;
			int32_t										x1;

			int32_t										xa;
			int32_t										ya;
			int32_t										x2a;
			int32_t										y2a;
			int32_t										xya;
			int32_t										an;

			int32_t										xb;
			int32_t										yb;
			int32_t										x2b;
			int32_t										y2b;
			int32_t										xyb;
			int32_t										bn;
		} * LPlsfit_acc, * const LPClsfit_acc;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 */
		static void									floor1_free_info( CVorbisStructs::vorbis_info_floor * _pvifI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvlfI Undocumented parameter from the Vorbis library.
		 */
		static void									floor1_free_look( CVorbisStructs::vorbis_look_floor * _pvlfI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvifI Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static void									floor1_pack( CVorbisStructs::vorbis_info_floor * _pvifI, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_info_floor *		floor1_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _pvifIn Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_look_floor *		floor1_look( CVorbisCodec::vorbis_dsp_state * _pvdsVd, CVorbisStructs::vorbis_info_floor * _pvifIn );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlfIn Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static void *									floor1_inverse1( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfIn );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlfIn Undocumented parameter from the Vorbis library.
		 * \param _pvMemo Undocumented parameter from the Vorbis library.
		 * \param _pfOut Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t									floor1_inverse2( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfIn, void * _pvMemo, float * _pfOut );

	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t									ilog2( uint32_t _ui32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvContext Unused.
		 * \param _pvA Undocumented parameter from the Vorbis library.
		 * \param n Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t LSE_CCALL						icomp( void * _pvContext, const void * _pvA, const void * _pvB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t									ilog( uint32_t _ui32V );

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
		static int32_t									render_point( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t _i32X );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32X0 Undocumented parameter from the Vorbis library.
		 * \param _i32X1 Undocumented parameter from the Vorbis library.
		 * \param _i32Y0 Undocumented parameter from the Vorbis library.
		 * \param _i32Y1 Undocumented parameter from the Vorbis library.
		 * \param _pfD Undocumented parameter from the Vorbis library.
		 */
		static void									render_line( int32_t _i32N, int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, float * _pfD );
	};

}	// namespace lss

#endif	// __LSS_VORBISFLOOR1_H__

