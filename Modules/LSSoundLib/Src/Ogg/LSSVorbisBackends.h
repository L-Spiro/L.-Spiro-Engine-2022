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

 function: libvorbis backend and mapping structures; needed for
           static mode headers
 last mod: $Id: backends.h 16962 2010-03-11 07:30:34Z xiphmont $

 ********************************************************************/

#ifndef __LSS_VORBISBACKENDS_H__
#define __LSS_VORBISBACKENDS_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisConstants.h"
#include "LSSVorbisStructs.h"

namespace lss {

	/**
	 * Class CVorbisBackends
	 * \brief A backend for libvorbis.
	 *
	 * Description: A backend for libvorbis.
	 */
	class CVorbisBackends {
	public :
		// == Types.
		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_func_floor {
			void											(* pack)( CVorbisStructs::vorbis_info_floor *, COgg::oggpack_buffer * );
			CVorbisStructs::vorbis_info_floor *				(* unpack)( CVorbisCodec::vorbis_info *, COgg::oggpack_buffer * );
			CVorbisStructs::vorbis_look_floor *				(* look)( CVorbisCodec::vorbis_dsp_state *, CVorbisStructs::vorbis_info_floor * );
			void											(* free_info)( CVorbisStructs::vorbis_info_floor * );
			void											(* free_look)( CVorbisStructs::vorbis_look_floor * );
			void *										(* inverse1)( CVorbisCodec::vorbis_block *, CVorbisStructs::vorbis_look_floor * );
			int32_t											(* inverse2)( CVorbisCodec::vorbis_block *, CVorbisStructs::vorbis_look_floor *, 
				void *buffer, float * );
		} * LPvorbis_func_floor, * const LPCvorbis_func_floor;

		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_info_floor0 {
			int32_t											order;
			int32_t											rate;
			int32_t											barkmap;

			int32_t											ampbits;
			int32_t											ampdB;

			int32_t											numbooks;						// <= 16.
			int32_t											books[16];

			float											lessthan;						// Encode-only config setting hacks for libvorbis.
			float											greaterthan;					// Encode-only config setting hacks for libvorbis.

		} * LPvorbis_info_floor0, * const LPCvorbis_info_floor0;

		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_info_floor1 {
			int32_t											partitions;						// 0 to 31.
			int32_t											partitionclass[VIF_PARTS];		// 0 to 15.

			int32_t											class_dim[VIF_CLASS];			// 1 to 8.
			int32_t											class_subs[VIF_CLASS];			// 0, 1, 2, 3 (bits: 1 << n possible).
			int32_t											class_book[VIF_CLASS];			// subs ^ dim entries.
			int32_t											class_subbook[VIF_CLASS][8];	// [VIF_CLASS][subs].


			int32_t											mult;							// 1 2 3 or 4.
			int32_t											postlist[VIF_POSIT+2];			// First two implicit.


			// Encode side analysis parameters.
			float											maxover;
			float											maxunder;
			float											maxerr;

			float											twofitweight;
			float											twofitatten;

			int32_t											n;

		} * LPvorbis_info_floor1, * const LPCvorbis_info_floor1;

		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_func_residue {
			void											(* pack)( CVorbisStructs::vorbis_info_residue *, COgg::oggpack_buffer * );
			CVorbisStructs::vorbis_info_residue *			(* unpack)( CVorbisCodec::vorbis_info *, COgg::oggpack_buffer * );
			CVorbisStructs::vorbis_look_residue *			(* look)( CVorbisCodec::vorbis_dsp_state *, 
				CVorbisStructs::vorbis_info_residue * );
			void											(* free_info)( CVorbisStructs::vorbis_info_residue * );
			void											(* free_look)( CVorbisStructs::vorbis_look_residue * );
			int32_t **										(* class0)( CVorbisCodec::vorbis_block *, CVorbisStructs::vorbis_look_residue *, 
				int32_t **, int32_t *, int32_t );
			int32_t											(* forward)( COgg::oggpack_buffer *, CVorbisCodec::vorbis_block *, 
				CVorbisStructs::vorbis_look_residue *, 
				int32_t **, int32_t *, int32_t, int32_t **, int32_t );
			int32_t											(* inverse)( CVorbisCodec::vorbis_block *, CVorbisStructs::vorbis_look_residue *, 
				float **, int32_t *, int32_t );
		} * LPvorbis_func_residue, * const LPCvorbis_func_residue;

		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_info_residue0 {
			// Block-partitioned VQ coded straight residue.
			int32_t											begin;
			int32_t											end;

			// First stage (lossless partitioning).
			int32_t											grouping;						// Froup n vectors per partition.
			int32_t											partitions;						// Possible codebooks for a partition.
			int32_t											partvals;						// Partitions ^ groupbook dim.
			int32_t											groupbook;						// Huffbook for partitioning.
			int32_t											secondstages[64];				// Expanded out to pointers in lookup.
			int32_t											booklist[512];					// List of second stage books.

			int32_t											classmetric1[64];
			int32_t											classmetric2[64];
		} * LPvorbis_info_residue0, * const LPCvorbis_info_residue0;

		/**
		 * Undocumented Vorbis structure.
		 */
		typedef struct vorbis_func_mapping {
			void											(* pack)( CVorbisCodec::vorbis_info *, CVorbisStructs::vorbis_info_mapping *, 
				COgg::oggpack_buffer * );
			CVorbisStructs::vorbis_info_mapping *			(* unpack)( CVorbisCodec::vorbis_info *, COgg::oggpack_buffer * );
			void											(* free_info)( CVorbisStructs::vorbis_info_mapping * );
			int32_t											(* forward)( CVorbisCodec::vorbis_block * vb );
			int32_t											(* inverse)( CVorbisCodec::vorbis_block * vb, CVorbisStructs::vorbis_info_mapping * );
		} * LPvorbis_func_mapping, * const LPCvorbis_func_mapping;


		// == Members.
		
	};

}	// namespace lss

#endif	// __LSS_VORBISBACKENDS_H__

