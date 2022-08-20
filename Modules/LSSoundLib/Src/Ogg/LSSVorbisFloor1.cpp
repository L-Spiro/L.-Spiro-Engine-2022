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

#include "LSSVorbisFloor1.h"
#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisMisc.h"
#include "Search/LSSTDSearch.h"


namespace lss {

	// == Members.
	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_floor CVorbisFloor1::floor1_exportbundle = {
		&floor1_pack,					// pack
		&floor1_unpack,					// unpack
		&floor1_look,					// pvlfLook
		&floor1_free_info,				// free_info
		&floor1_free_look,				// free_look
		&floor1_inverse1,				// inverse1
		&floor1_inverse2				// inverse2
	};

	// == Functions.

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFloor1::floor1_free_info( CVorbisStructs::vorbis_info_floor * _pvifI ) {
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor1 *>(_pvifI);
		if ( pvifInfo ) {
			CStd::MemSet( pvifInfo, 0, sizeof( (*pvifInfo) ) );
			CMemLib::Free( pvifInfo );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvlfI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFloor1::floor1_free_look( CVorbisStructs::vorbis_look_floor * _pvlfI ) {
		CVorbisCodecInternal::vorbis_look_floor1 * pvlfLook = static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(_pvlfI);
		if ( pvlfLook ) {
			CStd::MemSet( pvlfLook, 0, sizeof( (*pvlfLook) ) );
			CMemLib::Free( pvlfLook );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	void CVorbisFloor1::floor1_pack( CVorbisStructs::vorbis_info_floor * _pvifI, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor1 *>(_pvifI);
		int32_t J, K;
		int32_t i32Count = 0;
		int32_t i32RangeBits;
		int32_t i32MaxPosit = pvifInfo->postlist[1];
		int32_t i32MaxClass = -1;

		// Save out partitions.
		COgg::oggpack_write( _pobOpb, pvifInfo->partitions, 5 );				// Only 0 to 31 legal.
		for ( J = 0; J < pvifInfo->partitions; J++ ) {
			COgg::oggpack_write( _pobOpb, pvifInfo->partitionclass[J], 4 );		// Only 0 to 15 legal.
			if ( i32MaxClass<pvifInfo->partitionclass[J] ) { i32MaxClass = pvifInfo->partitionclass[J]; }
		}

		// save out partition classes.
		for ( J = 0; J < i32MaxClass+1; J++ ) {
			COgg::oggpack_write( _pobOpb, pvifInfo->class_dim[J] - 1, 3 );		// 1 to 8.
			COgg::oggpack_write( _pobOpb, pvifInfo->class_subs[J], 2 );			// 0 to 3.
			if ( pvifInfo->class_subs[J] ) { COgg::oggpack_write( _pobOpb, pvifInfo->class_book[J], 8 ); }
			for ( K = 0; K < (1 << pvifInfo->class_subs[J]); K++ ) {
				COgg::oggpack_write( _pobOpb, pvifInfo->class_subbook[J][K] + 1, 8 );
			}
		}

		// Save out the post list.
		COgg::oggpack_write( _pobOpb, pvifInfo->mult - 1, 2 );					// Only 1, 2, 3, and 4 are legal now.
		COgg::oggpack_write( _pobOpb, ilog2( i32MaxPosit ), 4 );
		i32RangeBits = ilog2( i32MaxPosit );

		for ( J = 0, K = 0; J < pvifInfo->partitions; J++ ) {
			i32Count += pvifInfo->class_dim[pvifInfo->partitionclass[J]];
			for ( ; K < i32Count; K++ ) {
				COgg::oggpack_write( _pobOpb, pvifInfo->postlist[K+2], i32RangeBits );
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_info_floor * CVorbisFloor1::floor1_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		int32_t J, K, i32Count = 0, i32MaxClass = -1, i32RangeBits;

		CVorbisBackends::vorbis_info_floor1 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor1 *>(CMemLib::CMAlloc( sizeof( (*pvifInfo) ) ));
		// Read partitions.
		pvifInfo->partitions = COgg::oggpack_read( _pobOpb, 5 );				// Only 0 to 31 legal.
		for ( J = 0; J < pvifInfo->partitions; J++ ) {
			pvifInfo->partitionclass[J] = COgg::oggpack_read( _pobOpb, 4 );		// only 0 to 15 legal.
			if ( pvifInfo->partitionclass[J] < 0 ) { goto ErrOut; }
			if ( i32MaxClass < pvifInfo->partitionclass[J] ) { i32MaxClass = pvifInfo->partitionclass[J]; }
		}

		// read partition classes.
		for ( J = 0; J < i32MaxClass+1; J++ ) {
			pvifInfo->class_dim[J] = COgg::oggpack_read( _pobOpb, 3 ) + 1;		// 1 to 8.
			pvifInfo->class_subs[J] = COgg::oggpack_read( _pobOpb, 2 );			// 0, 1, 2, 3 bits.
			if ( pvifInfo->class_subs[J] < 0 ) {
				goto ErrOut;
			}
			if ( pvifInfo->class_subs[J] ) { pvifInfo->class_book[J] = COgg::oggpack_read( _pobOpb, 8 ); }
			if ( pvifInfo->class_book[J] < 0 || pvifInfo->class_book[J] >= pcsiCi->books ) {
				goto ErrOut;
			}
			for ( K = 0; K < ( 1 << pvifInfo->class_subs[J] ); K++ ) {
				pvifInfo->class_subbook[J][K] = COgg::oggpack_read( _pobOpb, 8 ) - 1;
				if ( pvifInfo->class_subbook[J][K] < -1 || pvifInfo->class_subbook[J][K] >= pcsiCi->books ) {
					goto ErrOut;
				}
			}
		}

		// Read the post list.
		pvifInfo->mult = COgg::oggpack_read( _pobOpb, 2 ) + 1;					// Only 1, 2, 3, and 4 are legal now.
		i32RangeBits = COgg::oggpack_read( _pobOpb, 4 );
		if ( i32RangeBits < 0 ) { goto ErrOut; }

		for ( J = 0, K = 0; J < pvifInfo->partitions; J++ ) {
			i32Count += pvifInfo->class_dim[pvifInfo->partitionclass[J]];
			for ( ; K < i32Count; K++ ) {
				int32_t _i32T = pvifInfo->postlist[K+2] = COgg::oggpack_read( _pobOpb, i32RangeBits );
				if ( _i32T < 0 || _i32T >= (1 << i32RangeBits) ) {
					goto ErrOut;
				}
			}
		}
		pvifInfo->postlist[0] = 0;
		pvifInfo->postlist[1] = 1 << i32RangeBits;

		// Don't allow repeated values in post list as they'd result in zero-length segments.
		{
			int32_t * pi32SortPointer[VIF_POSIT+2];
			for ( J = 0; J < i32Count + 2; J++ ) { pi32SortPointer[J] = pvifInfo->postlist + J; }
			CSearch::QSort( pi32SortPointer, i32Count + 2, sizeof( (*pi32SortPointer) ), icomp, NULL );

			for ( J = 1; J < i32Count + 2; J++ ) {
				if ( *pi32SortPointer[J-1] == (*pi32SortPointer[J]) ) { goto ErrOut; }
			}
		}

		return pvifInfo;

	ErrOut :
		floor1_free_info( pvifInfo );
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _pvifIn Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_look_floor * CVorbisFloor1::floor1_look( CVorbisCodec::vorbis_dsp_state * /*_pvdsVd*/, CVorbisStructs::vorbis_info_floor * _pvifIn ) {
		int32_t * pi32SortPointer[VIF_POSIT+2];
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor1 *>(_pvifIn);
		CVorbisCodecInternal::vorbis_look_floor1 * pvlfLook = static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(CMemLib::CMAlloc( sizeof( (*pvlfLook) ) ));
		int32_t I, J, i32N = 0;

		pvlfLook->vi = pvifInfo;
		pvlfLook->n = pvifInfo->postlist[1];

		// We drop each position value in-between already decoded values, 
		//	and use linear interpolation to predict each new value past the
		//	edges.  The positions are read in the order of the position
		//	list.  We precompute the bounding positions _pvifIn the lookup.  Of
		//	course, the neighbors can change ( if a position is declined ), but
		//	this is an initial mapping.
		for ( I = 0; I < pvifInfo->partitions; I++ ) {
			i32N += pvifInfo->class_dim[pvifInfo->partitionclass[I]];
		}
		i32N += 2;
		pvlfLook->posts = i32N;

		// Also store a sorted position index.
		for ( I = 0; I < i32N; I++ ) {
			pi32SortPointer[I] = pvifInfo->postlist + I;
		}
		CSearch::QSort( pi32SortPointer, i32N, sizeof( (*pi32SortPointer) ), icomp, NULL );

		// Points from sort order back to range number.
		for ( I = 0; I < i32N; I++ ) {
			pvlfLook->forward_index[I] = static_cast<int32_t>(pi32SortPointer[I] - pvifInfo->postlist);	// TODO: Verify that int32_t is large enough for this.
		}
		// Points from range order to sorted position.
		for ( I = 0; I < i32N; I++ ) {
			pvlfLook->reverse_index[pvlfLook->forward_index[I]] = I;
		}
		// We actually need the post values too.
		for ( I = 0; I < i32N; I++ ) {
			pvlfLook->sorted_index[I] = pvifInfo->postlist[pvlfLook->forward_index[I]];
		}

		// Quantize values to multiplier spec.
		switch( pvifInfo->mult ) {
			case 1 : { // 1024 -> 256.
				pvlfLook->quant_q = 256;
				break;
			}
			case 2 : { // 1024 -> 128.
				pvlfLook->quant_q = 128;
				break;
			}
			case 3 : { // 1024 -> 86.
				pvlfLook->quant_q = 86;
				break;
			}
			case 4 : { // 1024 -> 64.
				pvlfLook->quant_q = 64;
				break;
			}
		}

		// Discover our neighbors for decode where we don't use fit flags ( that would push the neighbors outward ).
		for ( I = 0; I < i32N-2; I++ ) {
			int32_t i32Lo = 0;
			int32_t i32Hi = 1;
			int32_t i32Lx = 0;
			int32_t i32Hx = pvlfLook->n;
			int32_t i32CurrentX = pvifInfo->postlist[I+2];
			for ( J = 0; J < I+2; J++ ) {
				int32_t i32X = pvifInfo->postlist[J];
				if ( i32X > i32Lx && i32X < i32CurrentX ) {
					i32Lo = J;
					i32Lx = i32X;
				}
				if ( i32X<i32Hx && i32X>i32CurrentX ) {
					i32Hi = J;
					i32Hx = i32X;
				}
			}
			pvlfLook->loneighbor[I] = i32Lo;
			pvlfLook->hineighbor[I] = i32Hi;
		}

		return pvlfLook;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlfIn Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	void * CVorbisFloor1::floor1_inverse1( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfIn ) {
		CVorbisCodecInternal::vorbis_look_floor1 * pvlfLook = static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(_pvlfIn);
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = pvlfLook->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvbVb->vd->vi->codec_setup);

		int32_t I, J, K;
		CVorbisCodeBook::codebook * pcbBooks = pcsiCi->fullbooks;

		// Unpack wrapped/i32Predicted values from stream.
		if ( COgg::oggpack_read( &_pvbVb->opb, 1 ) == 1 ) {
			int32_t * pi32FitValue = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, (pvlfLook->posts) * sizeof( (*pi32FitValue) ) ));

			pi32FitValue[0] = COgg::oggpack_read( &_pvbVb->opb, ilog( pvlfLook->quant_q - 1 ) );
			pi32FitValue[1] = COgg::oggpack_read( &_pvbVb->opb, ilog( pvlfLook->quant_q - 1 ) );

			// Partition by partition.
			for ( I = 0, J = 2; I < pvifInfo->partitions; I++ ) {
				int32_t i32Class = pvifInfo->partitionclass[I];
				int32_t i32CDim = pvifInfo->class_dim[i32Class];
				int32_t i32CSubBits = pvifInfo->class_subs[i32Class];
				int32_t i32CSub = 1 << i32CSubBits;
				int32_t i32CVal = 0;

				// Decode the partition's first stage cascade value.
				if ( i32CSubBits ) {
					i32CVal = CVorbisCodeBook::vorbis_book_decode( pcbBooks+pvifInfo->class_book[i32Class], &_pvbVb->opb );

					if ( i32CVal == -1 ) { goto Eop; }
				}

				for ( K = 0; K < i32CDim; K++ ) {
					int32_t i32Book = pvifInfo->class_subbook[i32Class][i32CVal&(i32CSub-1)];
					i32CVal >>= i32CSubBits;
					if ( i32Book >= 0 ) {
						if ( (pi32FitValue[J+K] = CVorbisCodeBook::vorbis_book_decode( pcbBooks+i32Book, &_pvbVb->opb )) == -1 ) {
							goto Eop;
						}
					}
					else {
						pi32FitValue[J+K] = 0;
					}
				}
				J += i32CDim;
			}

			// Unwrap positive values and reconsitute via linear interpolation.
			for ( I = 2; I < pvlfLook->posts; I++ ) {
				int32_t i32Predicted = render_point( pvifInfo->postlist[pvlfLook->loneighbor[I-2]], 
					pvifInfo->postlist[pvlfLook->hineighbor[I-2]], 
					pi32FitValue[pvlfLook->loneighbor[I-2]], 
					pi32FitValue[pvlfLook->hineighbor[I-2]], 
					pvifInfo->postlist[I] );
				int32_t i32HiRoom = pvlfLook->quant_q - i32Predicted;
				int32_t i32LoRoom = i32Predicted;
				int32_t i32Room = (i32HiRoom < i32LoRoom ? i32HiRoom : i32LoRoom) << 1;
				int32_t i32Val = pi32FitValue[I];

				if ( i32Val ) {
					if ( i32Val >= i32Room ) {
						if ( i32HiRoom > i32LoRoom ) {
							i32Val = i32Val - i32LoRoom;
						}
						else {
							i32Val = -1 - (i32Val - i32HiRoom);
						}
					}
					else {
						if ( i32Val & 1 ) {
							i32Val = -((i32Val + 1) >> 1);
						}
						else {
							i32Val >>= 1;
						}
					}

					pi32FitValue[I] = i32Val + i32Predicted & 0x7FFF;
					pi32FitValue[pvlfLook->loneighbor[I-2]] &= 0x7FFF;
					pi32FitValue[pvlfLook->hineighbor[I-2]] &= 0x7FFF;

				}
				else {
					pi32FitValue[I] = i32Predicted | 0x8000;
				}

			}

			return pi32FitValue;
		}
	Eop :
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlfIn Undocumented parameter from the Vorbis library.
	 * \param _pvMemo Undocumented parameter from the Vorbis library.
	 * \param _pfOut Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFloor1::floor1_inverse2( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfIn, void * _pvMemo, float * _pfOut ) {
		CVorbisCodecInternal::vorbis_look_floor1 * pvlfLook = static_cast<CVorbisCodecInternal::vorbis_look_floor1 *>(_pvlfIn);
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = pvlfLook->vi;

		CVorbisCodecInternal::codec_setup_info *pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvbVb->vd->vi->codec_setup);
		int32_t i32N = pcsiCi->blocksizes[_pvbVb->W] / 2;
		int32_t J;

		if ( _pvMemo ) {
			// Render the lines.
			int32_t * pi32FitValue = static_cast<int32_t *>(_pvMemo);
			int32_t i32Hx = 0;
			int32_t i32Lx = 0;
			int32_t i32Ly = pi32FitValue[0]*pvifInfo->mult;
			// Guard lookup against out-of-range values.
			i32Ly = (i32Ly < 0 ? 0 : i32Ly > 255 ? 255 : i32Ly);

			for ( J = 1; J < pvlfLook->posts; J++ ) {
				int32_t i32Current = pvlfLook->forward_index[J];
				int32_t i32Hy = pi32FitValue[i32Current] & 0x7FFF;
				if ( i32Hy == pi32FitValue[i32Current] ) {

					i32Hx = pvifInfo->postlist[i32Current];
					i32Hy *= pvifInfo->mult;
					// Guard lookup against out-of-range values.
					i32Hy = (i32Hy < 0 ? 0 : i32Hy > 255 ? 255 : i32Hy);

					render_line( i32N, i32Lx, i32Hx, i32Ly, i32Hy, _pfOut );

					i32Lx = i32Hx;
					i32Ly = i32Hy;
				}
			}
			for ( J = i32Hx; J < i32N; J++ ) {
				_pfOut[J] *= CVorbisConstants::FLOOR1_fromdB_LOOKUP[i32Ly];	// Be certain.
			}
			return 1;
		}
		CStd::MemSet( _pfOut, 0, sizeof( (*_pfOut) ) * i32N );
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFloor1::ilog2( uint32_t _ui32V ) {
		int32_t i32Ret = 0;
		if ( _ui32V ) { --_ui32V; }
		while ( _ui32V ) {
			i32Ret++;
			_ui32V >>= 1;
		}
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvContext Unused.
	 * \param _pvA Undocumented parameter from the Vorbis library.
	 * \param i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t LSE_CCALL CVorbisFloor1::icomp( void * /*_pvContext*/, const void * _pvA, const void * _pvB ) {
		return ((*(*static_cast<int32_t * const*>(_pvA))) - (*(*static_cast<int32_t * const*>(_pvB))));
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFloor1::ilog( uint32_t _ui32V ) {
		int32_t i32Ret = 0;
		while ( _ui32V ) {
			i32Ret++;
			_ui32V >>= 1;
		}
		return i32Ret;
	}

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
	int32_t CVorbisFloor1::render_point( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t _i32X ) {
		_i32Y0 &= 0x7FFF;
		_i32Y1 &= 0x7FFF;

		{
			int32_t i32Dy = _i32Y1 - _i32Y0;
			int32_t i32Adx = _i32X1 - _i32X0;
			int32_t i32Ady = ::abs( i32Dy );
			int32_t i32Err = i32Ady * (_i32X - _i32X0);

			int32_t i32Off = i32Err / i32Adx;
			if ( i32Dy < 0 ) { return _i32Y0 - i32Off; }
			return _i32Y0 + i32Off;
		}
	}

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
	void CVorbisFloor1::render_line( int32_t _i32N, int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, float * _pfD ) {
		int32_t i32Dy = _i32Y1 - _i32Y0;
		int32_t i32Adx = _i32X1 - _i32X0;
		int32_t i32Ady = ::abs( i32Dy );
		int32_t i32Base = i32Dy / i32Adx;
		int32_t i32Sy = (i32Dy < 0 ? i32Base - 1 : i32Base + 1);
		int32_t i32X = _i32X0;
		int32_t i32Y = _i32Y0;
		int32_t i32Err = 0;

		i32Ady -= ::abs( i32Base * i32Adx );

		if ( _i32N > _i32X1 ) { _i32N = _i32X1; }

		if ( i32X < _i32N ) {
			_pfD[i32X] *= CVorbisConstants::FLOOR1_fromdB_LOOKUP[i32Y];
		}

		while ( ++i32X < _i32N ) {
			i32Err = i32Err + i32Ady;
			if ( i32Err >= i32Adx ) {
				i32Err -= i32Adx;
				i32Y += i32Sy;
			}
			else {
				i32Y += i32Base;
			}
			_pfD[i32X] *= CVorbisConstants::FLOOR1_fromdB_LOOKUP[i32Y];
		}
	}

}	// namespace lss
