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

 function: residue backend 0, 1 and 2 implementation
 last mod: $Id: res0.c 17556 2010-10-21 18:25:19Z tterribe $

 ********************************************************************/


#ifndef __LSS_VORBISRES0_H__
#define __LSS_VORBISRES0_H__

#include "../LSSSoundLib.h"
#include "LSSVorbisBackends.h"
#include "LSSVorbisCodeBook.h"

namespace lss {

	/**
	 * Class CVorbisRes0
	 * \brief Residue backend 0, 1, and 2 implementation.
	 *
	 * Description: Residue backend 0, 1, and 2 implementation.
	 */
	class CVorbisRes0 {
	public :
		// == Members.
		/**
		 * Undocumented Vorbis library function.
		 */
		static const CVorbisBackends::vorbis_func_residue	residue0_exportbundle;

		/**
		 * Undocumented Vorbis library function.
		 */
		static const CVorbisBackends::vorbis_func_residue	residue1_exportbundle;

		/**
		 * Undocumented Vorbis library function.
		 */
		static const CVorbisBackends::vorbis_func_residue	residue2_exportbundle;

	protected :
		// == Types.
		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_look_residue0 {
			CVorbisBackends::vorbis_info_residue0 *			info;

			int32_t											parts;
			int32_t											stages;
			CVorbisCodeBook::codebook *						fullbooks;
			CVorbisCodeBook::codebook *						phrasebook;
			CVorbisCodeBook::codebook ***					partbooks;

			int32_t											partvals;
			int32_t **										decodemap;

			int32_t											postbits;
			int32_t											phrasebits;
			int32_t											frames;
		} * LPvorbis_look_residue0, * LPCvorbis_look_residue0;
		

		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvirI Undocumented parameter from the Vorbis library.
		 */
		static void										res0_free_info( CVorbisStructs::vorbis_info_residue * _pvirI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvlrI Undocumented parameter from the Vorbis library.
		 */
		static void										res0_free_look( CVorbisStructs::vorbis_look_residue * _pvlrI );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvirVr Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 */
		static void										res0_pack( CVorbisStructs::vorbis_info_residue * _pvirVr, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pviVi Undocumented parameter from the Vorbis library.
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_info_residue *		res0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _pvirVr Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static CVorbisStructs::vorbis_look_residue *		res0_look( CVorbisCodec::vorbis_dsp_state * _pvdsVd,
			CVorbisStructs::vorbis_info_residue * _pvirVr );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										res0_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, float ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t **									res1_class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \param _ppi32PartWord Undocumented parameter from the Vorbis library.
		 * \param _i32SubMap Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										res1_forward( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch, int32_t ** _ppi32PartWord, int32_t _i32SubMap );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										res1_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			float ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t **									res2_class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pbOpb Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \param i32PartWord Undocumented parameter from the Vorbis library.
		 * \param i32SubMap Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										res2_forward( COgg::oggpack_buffer * _pbOpb,
			CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch, int32_t ** i32PartWord, int32_t i32SubMap );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										res2_inverse( CVorbisCodec::vorbis_block *_pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			float **_ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch );


	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										ilog( uint32_t _ui32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										icount( uint32_t _ui32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t **									_01class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t _i32Ch );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pi32Vec Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pi32Acc Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										_encodepart( COgg::oggpack_buffer * _pobOpb, int32_t * _pi32Vec, int32_t _i32N,
			CVorbisCodeBook::codebook * _pcbBook, int32_t * _pi32Acc );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pobOpb Undocumented parameter from the Vorbis library.
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \param _ppi32PartWord Undocumented parameter from the Vorbis library.
		 * \param _pfEncode Undocumented parameter from the Vorbis library.
		 * \param _i32SubMap Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										_01forward( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			int32_t ** _ppi32In, int32_t _i32Ch,
			int32_t ** _ppi32PartWord,
			int32_t (* _pfEncode)( COgg::oggpack_buffer *, int32_t *, int32_t, CVorbisCodeBook::codebook *, int32_t * ),
			int32_t _i32SubMap );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pi32A Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										local_book_besterror( CVorbisCodeBook::codebook * _pcbBook, int32_t * _pi32A );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \param _pfEncode Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t										_01inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl,
			float ** _ppi32In, int32_t _i32Ch,
			int32_t (* _pfEncode)( CVorbisCodeBook::codebook *, float *, COgg::oggpack_buffer *, int32_t ) );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvbVb Undocumented parameter from the Vorbis library.
		 * \param _pvlrVl Undocumented parameter from the Vorbis library.
		 * \param _ppi32In Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t **									_2class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, int32_t ** _ppi32In,
			int32_t _i32Ch );
	};

}	// namespace lss

#endif	// __LSS_VORBISRES0_H__

