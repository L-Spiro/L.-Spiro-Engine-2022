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

 function: libvorbis codec headers
 last mod: $Id: codec_internal.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisMisc.h"
#include <cstdlib>


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisPsy::vorbis_look_psy_global * CVorbisCodecInternal::_vp_global_look( CVorbisCodec::vorbis_info * _pviVi ) {
		codec_setup_info * pcsiCi = static_cast<codec_setup_info *>(_pviVi->codec_setup );
		CVorbisPsy::vorbis_info_psy_global * pvipgGi = &pcsiCi->psy_g_param;
		CVorbisPsy::vorbis_look_psy_global * _pvlfLook = static_cast<CVorbisPsy::vorbis_look_psy_global *>(CMemLib::CMAlloc( sizeof( (*_pvlfLook) ) ) );

		_pvlfLook->channels = _pviVi->channels;

		_pvlfLook->ampmax = -9999.0f;
		_pvlfLook->gi = pvipgGi;
		return _pvlfLook;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvlpgLook Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodecInternal::_vp_global_free( CVorbisPsy::vorbis_look_psy_global * _pvlpgLook ) {
		if ( _pvlpgLook ) {
			CStd::MemSet( _pvlpgLook, 0, sizeof( (*_pvlpgLook) ) );
			CMemLib::Free( _pvlpgLook );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pvlfLook Undocumented parameter from the Vorbis library.
	 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
	 * \param _pfLogMask Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t * CVorbisCodecInternal::floor1_fit( CVorbisCodec::vorbis_block * _pvbVb, vorbis_look_floor1 * _pvlfLook, 
		const float * _pfLogMdct, 
		const float * _pfLogMask ) {

		int32_t I, J;
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor1 *>(_pvlfLook->vi );
		int32_t i32N = _pvlfLook->n;
		int32_t i32Posts = _pvlfLook->posts;
		int32_t i32NonZero = 0;
		CVorbisStructs::lsfit_acc laFits[VIF_POSIT+1];
		int32_t i32FitValueA[VIF_POSIT+2]; // Index by range list position.
		int32_t i32FitValueB[VIF_POSIT+2]; // Index by range list position.

		int32_t i32LoNeighbor[VIF_POSIT+2]; // Sorted index of range list position ( +2 ).
		int32_t i32HiNeighbor[VIF_POSIT+2];
		int32_t * pi32Output = NULL;
		int32_t i32Memo[VIF_POSIT+2];

		for ( I = 0; I < i32Posts; I++ ) { i32FitValueA[I] = -200; }	// Mark all unused.
		for ( I = 0; I < i32Posts; I++ ) { i32FitValueB[I] = -200; }	// Mark all unused.
		for ( I = 0; I < i32Posts; I++ ) { i32LoNeighbor[I] = 0; }		// 0 for the implicit 0 _pi32Post.
		for ( I = 0; I < i32Posts; I++ ) { i32HiNeighbor[I] = 1; }		// 1 for the implicit _pi32Post at i32N.
		for ( I = 0; I < i32Posts; I++ ) { i32Memo[I] = -1; }			// No neighbor yet.

		// Quantize the relevant floor points and collect them into line fit
		//	structures ( one per minimal division ) at the same time.
		if ( i32Posts == 0 ) {
			i32NonZero += accumulate_fit( _pfLogMask, _pfLogMdct, 0, i32N, laFits, i32N, pvifInfo );
		}
		else {
			for ( I = 0; I < i32Posts - 1; I++ ) {
				i32NonZero += accumulate_fit( _pfLogMask, _pfLogMdct, _pvlfLook->sorted_index[I], 
					_pvlfLook->sorted_index[I+1], laFits + I, 
					i32N, pvifInfo );
			}
		}

		if ( i32NonZero ) {
			// Start by fitting the implicit i32Base case.
			int32_t i32Y0 = -200;
			int32_t i32Y1 = -200;
			fit_line( laFits, i32Posts - 1, &i32Y0, &i32Y1, pvifInfo );

			i32FitValueA[0] = i32Y0;
			i32FitValueB[0] = i32Y0;
			i32FitValueB[1] = i32Y1;
			i32FitValueA[1] = i32Y1;

			// Non degenerate case.
			// Start progressive splitting.  This is a greedy, non-optimal
			//	algorithm, but simple and close enough to the best
			//	answer.
			for ( I = 2; I < i32Posts; I++ ) {
				int32_t i32SortPos = _pvlfLook->reverse_index[I];
				int32_t i32Ln = i32LoNeighbor[i32SortPos];
				int32_t i32Hn = i32HiNeighbor[i32SortPos];

				// Eliminate repeat searches of a particular range with a i32Memo.
				if ( i32Memo[i32Ln] != i32Hn ) {
					// Haven't performed this error search yet.
					int32_t i32LSortPos = _pvlfLook->reverse_index[i32Ln];
					int32_t i32HSortPos = _pvlfLook->reverse_index[i32Hn];
					i32Memo[i32Ln] = i32Hn;

					{
						// A note: we want to bound/minimize *local*, not global, error.
						int32_t i32Lx = pvifInfo->postlist[i32Ln];
						int32_t i32Hx = pvifInfo->postlist[i32Hn];
						int32_t i32Ly = post_Y( i32FitValueA, i32FitValueB, i32Ln );
						int32_t i32Hy = post_Y( i32FitValueA, i32FitValueB, i32Hn );

						if ( i32Ly == -1 || i32Hy == -1 ) {
							::exit( 1 );	// TODO: Get this the hell out.  WTF.  The sound
											//	library cannot shut down the game.
						}

						if ( inspect_error( i32Lx, i32Hx, i32Ly, i32Hy, _pfLogMask, _pfLogMdct, pvifInfo ) ) {
							// Outside error bounds/begin search area.  Split it.
							int32_t i32Ly0 = -200;
							int32_t i32Ly1 = -200;
							int32_t i32Hy0 = -200;
							int32_t i32Hy1 = -200;
							int32_t i32Ret0 = fit_line( laFits+i32LSortPos, i32SortPos - i32LSortPos, &i32Ly0, &i32Ly1, pvifInfo );
							int32_t i32Ret1 = fit_line( laFits+i32SortPos, i32HSortPos - i32SortPos, &i32Hy0, &i32Hy1, pvifInfo );

							if ( i32Ret0 ) {
								i32Ly0 = i32Ly;
								i32Ly1 = i32Hy0;
							}
							if ( i32Ret1 ) {
								i32Hy0 = i32Ly1;
								i32Hy1 = i32Hy;
							}

							if ( i32Ret0 && i32Ret1 ) {
								i32FitValueA[I] = -200;
								i32FitValueB[I] = -200;
							}
							else {
								// Store new edge values.
								i32FitValueB[i32Ln] = i32Ly0;
								if ( i32Ln == 0 ) { i32FitValueA[i32Ln] = i32Ly0; }
								i32FitValueA[I] = i32Ly1;
								i32FitValueB[I] = i32Hy0;
								i32FitValueA[i32Hn] = i32Hy1;
								if ( i32Hn == 1 ) { i32FitValueB[i32Hn] = i32Hy1; }

								if ( i32Ly1 >= 0 || i32Hy0 >= 0 ) {
									// Store new neighbor values.
									for ( J = i32SortPos - 1; J >= 0; J-- ) {
										if ( i32HiNeighbor[J] == i32Hn ) {
											i32HiNeighbor[J] = I;
										}
										else {
											break;
										}
									}
									for ( J = i32SortPos + 1; J < i32Posts; J++ ) {
										if ( i32LoNeighbor[J] == i32Ln ) {
											i32LoNeighbor[J] = I;
										}
										else {
											break;
										}
									}
								}
							}
						}
						else {
							i32FitValueA[I] = -200;
							i32FitValueB[I] = -200;
						}
					}
				}
			}

			pi32Output = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*pi32Output) ) * i32Posts ));

			pi32Output[0] = post_Y( i32FitValueA, i32FitValueB, 0 );
			pi32Output[1] = post_Y( i32FitValueA, i32FitValueB, 1 );

			// Fill in i32Posts marked as not using a fit; we will zero
			//	back i32Out to "unused" when encoding them so int32_t as curve
			//	interpolation doesn't force them into use.
			for ( I = 2; I < i32Posts; I++ ) {
				int32_t i32Ln = _pvlfLook->loneighbor[I-2];
				int32_t i32Hn = _pvlfLook->hineighbor[I-2];
				int32_t i32X0 = pvifInfo->postlist[i32Ln];
				int32_t i32X1 = pvifInfo->postlist[i32Hn];
				int32_t i32NewY0 = pi32Output[i32Ln];
				int32_t i32NewY1 = pi32Output[i32Hn];

				int32_t i32Predicted = render_point( i32X0, i32X1, i32NewY0, i32NewY1, pvifInfo->postlist[I] );
				int32_t i32Yx = post_Y( i32FitValueA, i32FitValueB, I );

				if ( i32Yx >= 0 && i32Predicted != i32Yx ) {
					pi32Output[I] = i32Yx;
				}
				else {
					pi32Output[I] = i32Predicted | 0x8000;
				}
			}
		}

		return pi32Output;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pvlfLook Undocumented parameter from the Vorbis library.
	 * \param _pi32A Undocumented parameter from the Vorbis library.
	 * \param _pi32B Undocumented parameter from the Vorbis library.
	 * \param _i32Del Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t * CVorbisCodecInternal::floor1_interpolate_fit( CVorbisCodec::vorbis_block * _pvbVb, vorbis_look_floor1 * _pvlfLook, 
		int32_t * _pi32A, int32_t * _pi32B, 
		int32_t _i32Del ) {

		int32_t I;
		int32_t i32Posts = _pvlfLook->posts;
		int32_t * pi32Output = NULL;

		if ( _pi32A && _pi32B ) {
			pi32Output = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*pi32Output) ) * i32Posts ));

			// Overly simpleminded.  Look again post 1.2.
			for ( I = 0; I < i32Posts; I++ ) {
				pi32Output[I] = ((65536 - _i32Del) * (_pi32A[I] & 0x7FFF) + _i32Del * (_pi32B[I] & 0x7FFF) + 32768) >> 16;
				if ( _pi32A[I] & 0x8000 && _pi32B[I] & 0x8000 ) { pi32Output[I] |= 0x8000; }
			}
		}

		return pi32Output;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _poOpb Undocumented parameter from the Vorbis library.
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlfLook Undocumented parameter from the Vorbis library.
	 * \param _pi32Post Undocumented parameter from the Vorbis library.
	 * \param _pi32LogMaskI Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::floor1_encode( COgg::oggpack_buffer * _poOpb, CVorbisCodec::vorbis_block * _pvbVb, 
		vorbis_look_floor1 * _pvlfLook, 
		int32_t * _pi32Post, int32_t * _pi32LogMaskI ) {

		int32_t I, J;
		CVorbisBackends::vorbis_info_floor1 * pvifInfo = _pvlfLook->vi;
		int32_t i32Posts = _pvlfLook->posts;
		codec_setup_info * pcsiCi = static_cast<codec_setup_info *>(_pvbVb->vd->vi->codec_setup);
		int32_t i32Out[VIF_POSIT+2];
		CVorbisCodeBook::static_codebook ** ppscSBooks = pcsiCi->book_param;
		CVorbisCodeBook::codebook * pcbBooks = pcsiCi->fullbooks;

		// Quantize values to multiplier spec.
		if ( _pi32Post ) {
			for ( I = 0; I < i32Posts; I++ ) {
				int32_t i32Val = _pi32Post[I] & 0x7FFF;
				switch( pvifInfo->mult ) {
					case 1 : {// 1024 -> 256.
						i32Val >>= 2;
						break;
					}
					case 2 : {// 1024 -> 128.
						i32Val >>= 3;
						break;
					}
					case 3 : {// 1024 -> 86.
						i32Val /= 12;
						break;
					}
					case 4 : {// 1024 -> 64.
						i32Val >>= 4;
						break;
					}
				}
				_pi32Post[I] = i32Val | (_pi32Post[I] & 0x8000);
			}

			i32Out[0] = _pi32Post[0];
			i32Out[1] = _pi32Post[1];

			// Find prediction values for each _pi32Post and subtract them.
			for ( I = 2; I < i32Posts; I++ ) {
				int32_t i32Ln = _pvlfLook->loneighbor[I-2];
				int32_t i32Hn = _pvlfLook->hineighbor[I-2];
				int32_t i32X0 = pvifInfo->postlist[i32Ln];
				int32_t i32X1 = pvifInfo->postlist[i32Hn];
				int32_t i32Y0 = _pi32Post[i32Ln];
				int32_t i32Y1 = _pi32Post[i32Hn];

				int32_t i32Predicted = render_point( i32X0, i32X1, i32Y0, i32Y1, pvifInfo->postlist[I] );

				if ( (_pi32Post[I] & 0x8000 ) || (i32Predicted == _pi32Post[I]) ) {
					_pi32Post[I] = i32Predicted | 0x8000;	// In case there was roundoff jitter
															//	in interpolation.
					i32Out[I] = 0;
				}
				else {
					int32_t i32HeadRoom = ( _pvlfLook->quant_q-i32Predicted<i32Predicted?
						_pvlfLook->quant_q-i32Predicted:i32Predicted );

					int32_t i32Val = _pi32Post[I]-i32Predicted;

					// At this point the "deviation" value is in the range +/- max
					//	range, but the real, unique range can always be mapped to
					//	only [0-maxrange ).  So we want to wrap the deviation into
					//	this limited range, but do it in the way that least screws
					//	an essentially gaussian probability distribution.

					if ( i32Val < 0 )
						if ( i32Val < -i32HeadRoom ) {
							i32Val = i32HeadRoom - i32Val - 1;
						}
						else {
							i32Val = -1 - (i32Val << 1);
						}
					else {
						if ( i32Val >= i32HeadRoom ) {
							i32Val = i32Val + i32HeadRoom;
						}
						else {
							i32Val <<= 1;
						}
					}
					i32Out[I] = i32Val;
					_pi32Post[i32Ln] &= 0x7FFF;
					_pi32Post[i32Hn] &= 0x7FFF;
				}
			}

			// We have everything we need. pack it i32Out.
			// Mark nontrivial floor.
			COgg::oggpack_write( _poOpb, 1, 1 );

			// Beginning/end post.
			_pvlfLook->frames++;
			_pvlfLook->postbits += ilog( _pvlfLook->quant_q - 1 ) * 2;
			COgg::oggpack_write( _poOpb, i32Out[0], ilog( _pvlfLook->quant_q - 1 ) );
			COgg::oggpack_write( _poOpb, i32Out[1], ilog( _pvlfLook->quant_q - 1 ) );


			// Partition by partition.
			for ( I = 0, J = 2; I < pvifInfo->partitions; I++ ) {
				int32_t i32Class = pvifInfo->partitionclass[I];
				int32_t i32CDim = pvifInfo->class_dim[i32Class];
				int32_t i32CSubBits = pvifInfo->class_subs[i32Class];
				int32_t i32CSub = 1 << i32CSubBits;
				int32_t i32BookAs[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
				int32_t i32CVal = 0;
				int32_t i32CShift = 0;
				int32_t K, L;

				// Generate the partition's first stage cascade value.
				if ( i32CSubBits ) {
					int32_t i32MaxVal[8];
					for ( K = 0; K < i32CSub; K++ ) {
						int32_t i32BookNum = pvifInfo->class_subbook[i32Class][K];
						if ( i32BookNum<0 ) {
							i32MaxVal[K] = 1;
						}
						else {
							i32MaxVal[K] = ppscSBooks[pvifInfo->class_subbook[i32Class][K]]->entries;
						}
					}
					for ( K = 0; K < i32CDim; K++ ) {
						for ( L = 0; L < i32CSub; L++ ) {
							int32_t i32Val = i32Out[J+K];
							if ( i32Val<i32MaxVal[L] ) {
								i32BookAs[K] = L;
								break;
							}
						}
						i32CVal |= i32BookAs[K] << i32CShift;
						i32CShift += i32CSubBits;
					}
					// Write it.
					_pvlfLook->phrasebits += CVorbisCodeBook::vorbis_book_encode( pcbBooks + pvifInfo->class_book[i32Class], i32CVal, _poOpb );

				}

				// Write post values.
				for ( K = 0; K < i32CDim; K++ ) {
					int32_t i32Book = pvifInfo->class_subbook[i32Class][i32BookAs[K]];
					if ( i32Book >= 0 ) {
						// Hack to allow training with "bad" pcbBooks.
						if ( i32Out[J+K] < (pcbBooks + i32Book)->entries ) {
							_pvlfLook->postbits += CVorbisCodeBook::vorbis_book_encode( pcbBooks + i32Book, i32Out[J+K], _poOpb );
						}
						/*else
							fprintf( stderr, "+!" );*/

					}
				}
				J += i32CDim;
			}

			{
				// Generate i32Quantized floor equivalent to what we'd unpack in decode render the lines.
				int32_t i32Hx = 0;
				int32_t i32Lx = 0;
				int32_t i32Ly = _pi32Post[0] * pvifInfo->mult;
				int32_t i32N = pcsiCi->blocksizes[_pvbVb->W] / 2;

				for ( J = 1; J < _pvlfLook->posts; J++ ) {
					int32_t i32Current = _pvlfLook->forward_index[J];
					int32_t i32Hy = _pi32Post[i32Current] & 0x7FFF;
					if ( i32Hy == _pi32Post[i32Current] ) {

						i32Hy *= pvifInfo->mult;
						i32Hx = pvifInfo->postlist[i32Current];

						render_line0( i32N, i32Lx, i32Hx, i32Ly, i32Hy, _pi32LogMaskI );

						i32Lx = i32Hx;
						i32Ly = i32Hy;
					}
				}
				for ( J = i32Hx; J < _pvbVb->pcmend / 2; J++ ) { _pi32LogMaskI[J] = i32Ly; }	// Be certain.
				return 1;
			}
		}
		else {
			COgg::oggpack_write( _poOpb, 0, 1 );
			CStd::MemSet( _pi32LogMaskI, 0, _pvbVb->pcmend / 2 * sizeof( (*_pi32LogMaskI) ) );
			return 0;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfFlr Undocumented parameter from the Vorbis library.
	 * \param _pfMdct Undocumented parameter from the Vorbis library.
	 * \param _i32X0 Undocumented parameter from the Vorbis library.
	 * \param _i32X1 Undocumented parameter from the Vorbis library.
	 * \param _plaA Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pvfiInfo Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::accumulate_fit( const float * _pfFlr, const float * _pfMdct, 
		int32_t _i32X0, int32_t _i32X1, CVorbisStructs::lsfit_acc * _plaA, 
		int32_t _i32N, CVorbisBackends::vorbis_info_floor1 * _pvfiInfo ) {
		int32_t I;

		int32_t i32Xa = 0, i32Ya = 0, i32X2a = 0, i32Y2a = 0, i32Xya = 0, i32Na = 0, i32Xb = 0, i32Yb = 0, i32X2b = 0, i32Y2b = 0, i32Xyb = 0, i32Nb = 0;

		CStd::MemSet( _plaA, 0, sizeof( (*_plaA) ) );
		_plaA->x0 = _i32X0;
		_plaA->x1 = _i32X1;
		if ( _i32X1 >= _i32N ) { _i32X1 = _i32N-1; }

		for ( I = _i32X0; I <= _i32X1; I++ ) {
			int32_t i32Quantized = vorbis_dBquant( _pfFlr + I );
			if ( i32Quantized ) {
				if ( _pfMdct[I]+_pvfiInfo->twofitatten >= _pfFlr[I] ) {
					i32Xa += I;
					i32Ya += i32Quantized;
					i32X2a += I * I;
					i32Y2a += i32Quantized * i32Quantized;
					i32Xya += I * i32Quantized;
					i32Na++;
				}
				else {
					i32Xb += I;
					i32Yb += i32Quantized;
					i32X2b += I * I;
					i32Y2b += i32Quantized * i32Quantized;
					i32Xyb += I * i32Quantized;
					i32Nb++;
				}
			}
		}

		_plaA->xa = i32Xa;
		_plaA->ya = i32Ya;
		_plaA->x2a = i32X2a;
		_plaA->y2a = i32Y2a;
		_plaA->xya = i32Xya;
		_plaA->an = i32Na;

		_plaA->xb = i32Xb;
		_plaA->yb = i32Yb;
		_plaA->x2b = i32X2b;
		_plaA->y2b = i32Y2b;
		_plaA->xyb = i32Xyb;
		_plaA->bn = i32Nb;

		return i32Na;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plaA Undocumented parameter from the Vorbis library.
	 * \param _i32Fits Undocumented parameter from the Vorbis library.
	 * \param _pi32Y0 Undocumented parameter from the Vorbis library.
	 * \param _pi32Y1 Undocumented parameter from the Vorbis library.
	 * \param _pvifInfo Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::fit_line( CVorbisStructs::lsfit_acc * _plaA, int32_t _i32Fits, int32_t * _pi32Y0, int32_t * _pi32Y1, 
		CVorbisBackends::vorbis_info_floor1 * _pvifInfo ) {
		double dXb = 0, dYb = 0, dX2b = 0, dY2b = 0, dXyb = 0, dBn = 0;
		int32_t I;
		int32_t _i32X0 = _plaA[0].x0;
		int32_t _i32X1 = _plaA[_i32Fits-1].x1;

		for ( I = 0; I < _i32Fits; I++ ) {
			double dWeight = (_plaA[I].bn + _plaA[I].an) * _pvifInfo->twofitweight / (_plaA[I].an + 1) + 1.0;

			dXb += _plaA[I].xb + _plaA[I].xa * dWeight;
			dYb += _plaA[I].yb + _plaA[I].ya * dWeight;
			dX2b += _plaA[I].x2b + _plaA[I].x2a * dWeight;
			dY2b += _plaA[I].y2b + _plaA[I].y2a * dWeight;
			dXyb += _plaA[I].xyb + _plaA[I].xya * dWeight;
			dBn += _plaA[I].bn + _plaA[I].an * dWeight;
		}

		if ( (*_pi32Y0) >= 0 ) {
			dXb += _i32X0;
			dYb += (*_pi32Y0);
			dX2b += _i32X0 * _i32X0;
			dY2b += (*_pi32Y0) * (*_pi32Y0);
			dXyb += (*_pi32Y0) * _i32X0;
			dBn++;
		}

		if ( (*_pi32Y1) >= 0 ) {
			dXb += _i32X1;
			dYb += (*_pi32Y1);
			dX2b += _i32X1 * _i32X1;
			dY2b += (*_pi32Y1) * (*_pi32Y1);
			dXyb += (*_pi32Y1) * _i32X1;
			dBn++;
		}

		{
			double dDemon = (dBn * dX2b - dXb * dXb);

			if ( dDemon > 0.0 ) {
				double dA = (dYb * dX2b - dXyb * dXb) / dDemon;
				double dB = (dBn * dXyb - dXb * dYb) / dDemon;
				(*_pi32Y0) = static_cast<int32_t>(rint( dA + dB * _i32X0 ));
				(*_pi32Y1) = static_cast<int32_t>(rint( dA + dB * _i32X1 ));

				// Limit to our range!
				if ( (*_pi32Y0) > 1023 ) { (*_pi32Y0) = 1023; }
				if ( (*_pi32Y1) > 1023 ) { (*_pi32Y1) = 1023; }
				if ( (*_pi32Y0) < 0 ) { (*_pi32Y0) = 0; }
				if ( (*_pi32Y1) < 0 ) { (*_pi32Y1) = 0; }

				return 0;
			}
			else {
				(*_pi32Y0) = 0;
				(*_pi32Y1) = 0;
				return 1;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pi32A Undocumented parameter from the Vorbis library.
	 * \param _pi32B Undocumented parameter from the Vorbis library.
	 * \param _i32Pos Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::post_Y( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Pos ) {
		if ( _pi32A[_i32Pos] < 0 ) { return _pi32B[_i32Pos]; }
		if ( _pi32B[_i32Pos] < 0 ) { return _pi32A[_i32Pos]; }

		return (_pi32A[_i32Pos] + _pi32B[_i32Pos]) >> 1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32X0 Undocumented parameter from the Vorbis library.
	 * \param _i32X1 Undocumented parameter from the Vorbis library.
	 * \param _i32Y0 Undocumented parameter from the Vorbis library.
	 * \param _i32Y1 Undocumented parameter from the Vorbis library.
	 * \param _pfMask Undocumented parameter from the Vorbis library.
	 * \param _pfMdct Undocumented parameter from the Vorbis library.
	 * \param _pvifInfo Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::inspect_error( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, const float * _pfMask, 
		const float * _pfMdct, 
		CVorbisBackends::vorbis_info_floor1 * _pvifInfo ) {
		int32_t i32Dy = _i32Y1 - _i32Y0;
		int32_t i32Adx = _i32X1 - _i32X0;
		int32_t i32Ady = ::abs( i32Dy );
		int32_t i32Base = i32Dy / i32Adx;
		int32_t i32Sy = (i32Dy < 0 ? i32Base - 1 : i32Base + 1);
		int32_t i32X = _i32X0;
		int32_t i32Y = _i32Y0;
		int32_t i32Err = 0;
		int32_t i32Val = vorbis_dBquant( _pfMask + i32X );
		int32_t i32Mse = 0;
		int32_t i32N = 0;

		i32Ady -= ::abs( i32Base * i32Adx );

		i32Mse = (i32Y - i32Val);
		i32Mse *= i32Mse;
		i32N++;
		if ( _pfMdct[i32X] + _pvifInfo->twofitatten >= _pfMask[i32X] ) {
			if ( i32Y + _pvifInfo->maxover < i32Val ) { return 1; }
			if ( i32Y - _pvifInfo->maxunder > i32Val ) { return 1; }
		}

		while ( ++i32X < _i32X1 ) {
			i32Err = i32Err + i32Ady;
			if ( i32Err >= i32Adx ) {
				i32Err -= i32Adx;
				i32Y += i32Sy;
			}
			else {
				i32Y += i32Base;
			}

			i32Val = vorbis_dBquant( _pfMask + i32X );
			i32Mse += ((i32Y - i32Val) * (i32Y - i32Val));
			i32N++;
			if ( _pfMdct[i32X] + _pvifInfo->twofitatten >= _pfMask[i32X] ) {
				if ( i32Val ) {
					if ( i32Y + _pvifInfo->maxover < i32Val ) { return 1; }
					if ( i32Y - _pvifInfo->maxunder > i32Val ) { return 1; }
				}
			}
		}

		if ( _pvifInfo->maxover * _pvifInfo->maxover / i32N > _pvifInfo->maxerr ) { return 0; }
		if ( _pvifInfo->maxunder * _pvifInfo->maxunder / i32N > _pvifInfo->maxerr ) { return 0; }
		if ( i32Mse / i32N > _pvifInfo->maxerr ) { return 1; }
		return 0;
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
	int32_t CVorbisCodecInternal::render_point( int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t _i32X ) {
		_i32Y0 &= 0x7FFF; // Mask off flag.
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
	 * \param _pfX Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::vorbis_dBquant( const float * _pfX ) {
		int32_t i32Value = static_cast<int32_t>((*_pfX) * 7.3142857f + 1023.5f);
		if ( i32Value > 1023 ) { return 1023; }
		if ( i32Value < 0 ) { return 0; }
		return i32Value;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodecInternal::ilog( uint32_t _ui32V ) {
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
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32X0 Undocumented parameter from the Vorbis library.
	 * \param _i32X1 Undocumented parameter from the Vorbis library.
	 * \param _i32Y0 Undocumented parameter from the Vorbis library.
	 * \param _i32Y1 Undocumented parameter from the Vorbis library.
	 * \param _pi32D Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodecInternal::render_line0( int32_t _i32N, int32_t _i32X0, int32_t _i32X1, int32_t _i32Y0, int32_t _i32Y1, int32_t * _pi32D ) {
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
			_pi32D[i32X] = i32Y;
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
			_pi32D[i32X] = i32Y;
		}
	}

}	// namespace lss
