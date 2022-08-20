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

 function: basic shared codebook operations
 last mod: $Id: codebook.h 17030 2010-03-25 06:52:55Z xiphmont $

 ********************************************************************/


#ifndef __LSS_VORBISCODEBOOK_H__
#define __LSS_VORBISCODEBOOK_H__

#include "../LSSSoundLib.h"
#include "LSSOgg.h"

namespace lss {

	/**
	 * Class CVorbisCodeBook
	 * \brief Basic shared codebook operations.
	 *
	 * Description: Basic shared codebook operations.
	 */
	class CVorbisCodeBook {
	public :
		// == Types.
		/**
		 * This structure encapsulates huffman and VQ style encoding books; it
		 *	doesn't do anything specific to either.
		 *
		 * Valuelist/quantlist are nonNULL (and q_* significant) only if
		 *	there's entry->value mapping to be done.
		 *
		 * If encode-side mapping must be done (and thus the entry needs to be
		 *	hunted), the auxiliary encode pointer will point to a decision
		 *	tree.  This is true of both VQ and huffman, but is mostly useful
		 *	with VQ.
		 */
		typedef struct static_codebook {
			int32_t								dim;				// Codebook dimensions (elements per vector).
			int32_t								entries;			// Codebook entries.
			int32_t *							lengthlist;			// Codeword lengths in bits.

			// Mapping.
			int32_t								maptype;			// 0 = none.
																	// 1 = implicitly populated values from map column.
																	// 2 = listed arbitrary values.

			// The below does a linear, single monotonic sequence mapping.
			int32_t								q_min;				// Packed 32 bit float; quant value 0 maps to minval.
			int32_t								q_delta;			// Packed 32 bit float; val 1 - val 0 == delta.
			int32_t								q_quant;			// Bits: 0 < quant <= 16.
			int32_t								q_sequencep;		// Bitflag.

			int32_t *							quantlist;			// Map == 1: (int32_t)(entries^(1/dim)) element column map.
																	// Map == 2: list of dim*entries quantized entry vals.
			int32_t								allocedp;
		} * LPstatic_codebook, * const LPCstatic_codebook;

		typedef struct codebook {
			int32_t								dim;				// Codebook dimensions (elements per vector).
			int32_t								entries;			// Codebook entries.
			int32_t								used_entries;		// Populated codebook entries.
			const static_codebook *				c;

			// For encode, the below are entry-ordered, fully populated.
			// For decode, the below are ordered by bitreversed codeword and only
			//	used entries are populated.
			float *							valuelist;			// List of dim*entries actual entry values.
			uint32_t *							codelist;			// List of bitstream codewords for each entry.

			int32_t *							dec_index;			// Only used if sparseness collapsed.
			int8_t *							dec_codelengths;
			uint32_t *							dec_firsttable;
			int32_t								dec_firsttablen;
			int32_t								dec_maxlength;

			// The current encoder uses only centered, integer-only lattice books.
			int32_t								quantvals;
			int32_t								minval;
			int32_t								delta;
		} * LPcodebook, * const LPCcodebook;


		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pscbB Undocumented parameter from the Vorbis library.
		 */
		static void							vorbis_staticbook_destroy( static_codebook * _pscbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbDest Undocumented parameter from the Vorbis library.
		 * \param _pscbSource Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_init_encode( codebook * _pcbDest, const static_codebook * _pscbSource );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbDest Undocumented parameter from the Vorbis library.
		 * \param _pscbSource Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_init_decode( codebook * _pcbDest, const static_codebook * _pscbSource );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbB Undocumented parameter from the Vorbis library.
		 */
		static void							vorbis_book_clear( codebook * _pcbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pscbSource Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _pi32Map Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float *						_book_unquantize( const static_codebook * _pscbSource, int32_t _i32N, int32_t * _pi32Map );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pscbB Undocumented parameter from the Vorbis library.
		 * \param _pfVals Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static float *						_book_logdist( const static_codebook * _pscbB, float * _pfVals );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Val Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static float							_float32_unpack( int32_t _i32Val );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32Val Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_float32_pack( float _i32Val );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _i32Step Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static int32_t							_best( codebook * _pcbBook, float * _pfA, int32_t _i32Step );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32V Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_ilog( uint32_t _i32V );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pscbB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							_book_maptype1_quantvals( const static_codebook * _pscbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param step Undocumented parameter from the Vorbis library.
		 * \param _i32AddMul Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static int32_t							vorbis_book_besterror( codebook * _pcbBook, float * _pfA, int32_t step, int32_t _i32AddMul );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _i32Entry Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_codeword( codebook * _pcbBook, int32_t _i32Entry );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _i32Entry Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_codelen( codebook * _pcbBook, int32_t _i32Entry );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pscbC Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_staticbook_pack( const static_codebook * _pscbC, COgg::oggpack_buffer * _ppbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static static_codebook *				vorbis_staticbook_unpack( COgg::oggpack_buffer * _ppbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _fA Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_encode( codebook * _pcbBook, int32_t _fA, COgg::oggpack_buffer * _ppbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_decode( codebook * _pcbBook, COgg::oggpack_buffer * _ppbB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_decodevs_add( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_decodev_set( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _pfA Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_decodev_add( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _ppfA Undocumented parameter from the Vorbis library.
		 * \param _i32Off Undocumented parameter from the Vorbis library.
		 * \param _i32Ch Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							vorbis_book_decodevv_add( codebook * _pcbBook, float ** _ppfA, int32_t _i32Off, int32_t _i32Ch, COgg::oggpack_buffer * _ppbB, int32_t _i32N );


	protected :
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pi32L Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \param _i32SparseCount Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static uint32_t *						_make_words( int32_t * _pi32L, int32_t _i32N, int32_t _i32SparseCount );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _ui32X Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static uint32_t							bitreverse( uint32_t _ui32X );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pvContext Unused.
		 * \param _pvA Undocumented parameter from the Vorbis library.
		 * \param _pvB Undocumented parameter from the Vor_pvBis li_pvBrary.
		 * \return Undocumented return from the Vor_pvBis li_pvBrary.
		 */
		static int32_t LSE_CCALL				sort32a( void * _pvContext, const void * _pvA, const void * _pvB );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pcbBook Undocumented parameter from the Vorbis library.
		 * \param _ppbB Undocumented parameter from the Vor_pvBis li_pvBrary.
		 * \return Undocumented return from the Vor_pvBis li_pvBrary.
		 */
		static int32_t							decode_packed_entry_number( codebook * _pcbBook, COgg::oggpack_buffer * _ppbB );
	};

}	// namespace lss

#endif	// __LSS_VORBISCODEBOOK_H__

