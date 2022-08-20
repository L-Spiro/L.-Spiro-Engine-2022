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
 fLast mod: $Id: codebook.h 17030 2010-03-25 06:52:55Z xiphmont $

 ********************************************************************/

#include "LSSVorbisCodeBook.h"
#include "LSSVorbisConstants.h"
#include "Search/LSSTDSearch.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pscbB Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodeBook::vorbis_staticbook_destroy( static_codebook * _pscbB ) {
		if ( _pscbB->allocedp ) {
			if ( _pscbB->quantlist ) { CMemLib::Free( _pscbB->quantlist ); }
			if ( _pscbB->lengthlist ) { CMemLib::Free( _pscbB->lengthlist ); }
			CStd::MemSet( _pscbB, 0, sizeof( (*_pscbB) ) );
			CMemLib::Free( _pscbB );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbDest Undocumented parameter from the Vorbis library.
	 * \param _pscbSource Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_init_encode( codebook * _pcbDest, const static_codebook * _pscbSource ) {
		CStd::MemSet( _pcbDest, 0, sizeof( (*_pcbDest) ) );
		_pcbDest->c = _pscbSource;
		_pcbDest->entries = _pscbSource->entries;
		_pcbDest->used_entries = _pscbSource->entries;
		_pcbDest->dim = _pscbSource->dim;
		_pcbDest->codelist = _make_words( _pscbSource->lengthlist, _pscbSource->entries, 0 );
		//_pcbDest->valuelist = _book_unquantize( _pscbSource, _pscbSource->entries, NULL );
		_pcbDest->quantvals = _book_maptype1_quantvals( _pscbSource );
		_pcbDest->minval = static_cast<int32_t>(rint( _float32_unpack( _pscbSource->q_min ) ));
		_pcbDest->delta = static_cast<int32_t>(rint( _float32_unpack( _pscbSource->q_delta ) ));

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbDest Undocumented parameter from the Vorbis library.
	 * \param _pscbSource Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_init_decode( codebook * _pcbDest, const static_codebook * _pscbSource ) {
		int32_t I, J, i32N = 0, i32TabN;
		int32_t * pi32SortIndex;
		CStd::MemSet( _pcbDest, 0, sizeof( (*_pcbDest) ) );

		// Count actual used entries.
		for ( I = 0; I < _pscbSource->entries; I++ ) {
			if ( _pscbSource->lengthlist[I] > 0 ) {
				i32N++;
			}
		}

		_pcbDest->entries = _pscbSource->entries;
		_pcbDest->used_entries = i32N;
		_pcbDest->dim = _pscbSource->dim;

		if ( i32N > 0 ) {

			// Two different remappings go on here.
			//
			// First, we collapse the likely sparse codebook down only to
			//	actually represented values/words.  This collapsing needs to be
			//	indexed as map-valueless books are used to encode original entry
			//	positions as integers.
			//
			// Second, we reorder all vectors, including the entry i32Index above, 
			//	by sorted bitreversed codeword to allow treeless decode.

			// Perform sort.
			uint32_t * pi32Codes = _make_words( _pscbSource->lengthlist, _pscbSource->entries, _pcbDest->used_entries );
			uint32_t ** ppi32CodeP = static_cast<uint32_t **>(::alloca( sizeof( (*ppi32CodeP) ) * i32N ));

			if ( pi32Codes == NULL ) { goto ErrOut; }

			for ( I = 0; I < i32N; I++ ) {
				pi32Codes[I] = bitreverse( pi32Codes[I] );
				ppi32CodeP[I] = pi32Codes+I;
			}

			CSearch::QSort( ppi32CodeP, i32N, sizeof( (*ppi32CodeP) ), sort32a, NULL );

			pi32SortIndex = static_cast<int32_t *>(::alloca( i32N * sizeof( (*pi32SortIndex) ) ));
			_pcbDest->codelist = static_cast<uint32_t *>(CMemLib::MAlloc( i32N * sizeof( (*_pcbDest->codelist) ) ));
			// The i32Index is a reverse i32Index.
			for ( I = 0; I < i32N; I++ ) {
				uintptr_t position = ppi32CodeP[I] - pi32Codes;
				pi32SortIndex[position] = I;
			}

			for ( I = 0; I < i32N; I++ ) {
				_pcbDest->codelist[pi32SortIndex[I]] = pi32Codes[I];
			}
			CMemLib::Free( pi32Codes );


			_pcbDest->valuelist = _book_unquantize( _pscbSource, i32N, pi32SortIndex );
			_pcbDest->dec_index = static_cast<int32_t *>(CMemLib::CMAlloc( i32N * sizeof( (*_pcbDest->dec_index) ) ));

			for ( i32N = 0, I = 0; I < _pscbSource->entries; I++ ) {
				if ( _pscbSource->lengthlist[I] > 0 ) {
					_pcbDest->dec_index[pi32SortIndex[i32N++]] = I;
				}
			}

			_pcbDest->dec_codelengths = static_cast<int8_t *>(CMemLib::MAlloc( i32N * sizeof( (*_pcbDest->dec_codelengths) ) ));
			for ( i32N = 0, I = 0; I < _pscbSource->entries; I++ ) {
				if ( _pscbSource->lengthlist[I] > 0 ) {
					_pcbDest->dec_codelengths[pi32SortIndex[i32N++]] = static_cast<int8_t>(_pscbSource->lengthlist[I]);
				}
			}

			_pcbDest->dec_firsttablen = _ilog( _pcbDest->used_entries ) - 4; // This is magic.
			if ( _pcbDest->dec_firsttablen < 5 ) { _pcbDest->dec_firsttablen = 5; }
			if ( _pcbDest->dec_firsttablen > 8 ) { _pcbDest->dec_firsttablen = 8; }

			i32TabN = 1 << _pcbDest->dec_firsttablen;
			_pcbDest->dec_firsttable = static_cast<uint32_t *>(CMemLib::CMAlloc( i32TabN * sizeof( (*_pcbDest->dec_firsttable) ) ));
			_pcbDest->dec_maxlength = 0;

			for ( I = 0; I < i32N; I++ ) {
				if ( _pcbDest->dec_maxlength < _pcbDest->dec_codelengths[I] ) {
					_pcbDest->dec_maxlength = _pcbDest->dec_codelengths[I];
				}
				if ( _pcbDest->dec_codelengths[I] <= _pcbDest->dec_firsttablen ) {
					uint32_t i32Orig = bitreverse( _pcbDest->codelist[I] );
					for ( J = 0; J < (1 << (_pcbDest->dec_firsttablen - _pcbDest->dec_codelengths[I])); J++ ) {
						_pcbDest->dec_firsttable[i32Orig|(J << _pcbDest->dec_codelengths[I])] = I + 1;
					}
				}
			}

			// Now fill in "unused" entries in the firsttable with hi/lo search
			//	hints for the non-direct-hits.
			{
				uint32_t i32Mask = static_cast<uint32_t>(0xFFFFFFFE) << (31 - _pcbDest->dec_firsttablen);
				int32_t i32Lo = 0, i32Hi = 0;

				for ( I = 0; I < i32TabN; I++ ) {
					uint32_t i32Word = I << (32 - _pcbDest->dec_firsttablen);
					if ( _pcbDest->dec_firsttable[bitreverse( i32Word )] == 0 ) {
						while ( (i32Lo + 1) < i32N && _pcbDest->codelist[i32Lo+1] <= i32Word ) { i32Lo++; }
						while ( i32Hi < i32N && i32Word >= (_pcbDest->codelist[i32Hi] & i32Mask) ) { i32Hi++; }

						// We only actually have 15 bits per hint to play with here.
						//	In order to overflow gracefully ( nothing breaks, efficiency
						//	just drops ), encode as the difference from the extremes.
						{
							uint32_t i32LoVal = i32Lo;
							uint32_t i32HiVal = i32N-i32Hi;

							if ( i32LoVal > 0x7FFF ) { i32LoVal = 0x7FFF; }
							if ( i32HiVal > 0x7FFF ) { i32HiVal = 0x7FFF; }
							_pcbDest->dec_firsttable[bitreverse( i32Word )] = 
								0x80000000UL | (i32LoVal << 15) | i32HiVal;
						}
					}
				}
			}
		}

		return 0;
	ErrOut :
		vorbis_book_clear( _pcbDest );
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbB Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodeBook::vorbis_book_clear( codebook * _pcbB ) {
		// Static book is not cleared; we're likely called on the lookup and
		//	the static codebook belongs to the info struct.
		if ( _pcbB->valuelist ) { CMemLib::Free( _pcbB->valuelist ); }
		if ( _pcbB->codelist ) { CMemLib::Free( _pcbB->codelist ); }

		if ( _pcbB->dec_index ) { CMemLib::Free( _pcbB->dec_index ); }
		if ( _pcbB->dec_codelengths ) { CMemLib::Free( _pcbB->dec_codelengths ); }
		if ( _pcbB->dec_firsttable ) { CMemLib::Free( _pcbB->dec_firsttable ); }

		CStd::MemSet( _pcbB, 0, sizeof( *_pcbB ) );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pscbSource Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pi32Map Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float * CVorbisCodeBook::_book_unquantize( const static_codebook * _pscbSource, int32_t _i32N, int32_t * _pi32Map ) {
		int32_t J, K, i32Count = 0;
		if ( _pscbSource->maptype == 1 || _pscbSource->maptype == 2 ) {
			int32_t i32QuantValues;
			float fMindel = _float32_unpack( _pscbSource->q_min );
			float fDelta = _float32_unpack( _pscbSource->q_delta );
			float * pfR = static_cast<float *>(CMemLib::CMAlloc( (_i32N * _pscbSource->dim) * sizeof( (*pfR) ) ));

			// Maptype 1 and 2 both use a quantized value vector, but
			//	different sizes.
			switch ( _pscbSource->maptype ) {
				case 1 : {
					// Most of the time, entries%dimensions  == 0, but we need to be
					//	well defined.  We define that the possible vales at each
					//	scalar is values  == entries/dim.  If entries%dim != 0, we'll
					//	have "too few" values ( values*dim<entries ), which means that
					//	we'll have "left over" entries; left over entries use zeroed
					//	values ( and are wasted ).  So don't generate codebooks like
					//	that.
					i32QuantValues = _book_maptype1_quantvals( _pscbSource );
					for ( J = 0; J < _pscbSource->entries; J++ ) {
						if ( ( _pi32Map && _pscbSource->lengthlist[J] ) || !_pi32Map ) {
							float fLast = 0.0f;
							int32_t i32IndexDiv = 1;
							for ( K = 0; K < _pscbSource->dim; K++ ) {
								int32_t i32Index = (J / i32IndexDiv) % i32QuantValues;
								float fVal = static_cast<float>(_pscbSource->quantlist[i32Index]);
								fVal = ::fabsf( fVal ) * fDelta + fMindel + fLast;
								if ( _pscbSource->q_sequencep ) { fLast = fVal; }
								if ( _pi32Map ) {
									pfR[_pi32Map[i32Count]*_pscbSource->dim+K] = fVal;
								}
								else {
									pfR[i32Count*_pscbSource->dim+K] = fVal;
								}
								i32IndexDiv *= i32QuantValues;
							}
							i32Count++;
						}

					}
					break;
				}
				case 2 : {
					for ( J = 0; J < _pscbSource->entries; J++ ) {
						if ( ( _pi32Map && _pscbSource->lengthlist[J] ) || !_pi32Map ) {
							float fLast = 0.0f;

							for ( K = 0; K < _pscbSource->dim; K++ ) {
								float fVal = static_cast<float>(_pscbSource->quantlist[J*_pscbSource->dim+K]);
								fVal = ::fabsf( fVal ) * fDelta + fMindel + fLast;
								if ( _pscbSource->q_sequencep ) { fLast = fVal; }
								if ( _pi32Map ) {
									pfR[_pi32Map[i32Count]*_pscbSource->dim+K] = fVal;
								}
								else {
									pfR[i32Count*_pscbSource->dim+K] = fVal;
								}
							}
							i32Count++;
						}
					}
					break;
				}
			}

			return pfR;
		}
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pscbB Undocumented parameter from the Vorbis library.
	 * \param _pfVals Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	/*float * CVorbisCodeBook::_book_logdist( const static_codebook * _pscbB, float * _pfVals ) {
	}*/

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Val Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float CVorbisCodeBook::_float32_unpack( int32_t _i32Val ) {
		double dMant = _i32Val & 0x1FFFFF;
		int32_t i32Sign = _i32Val & 0x80000000;
		int32_t i32Exp  = (_i32Val & 0x7FE00000L) >> VQ_FMAN;
		if ( i32Sign ) { dMant = -dMant; }
		return static_cast<float>((ldexp( dMant, i32Exp - (VQ_FMAN - 1) - VQ_FEXP_BIAS )));
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Val Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::_float32_pack( float _i32Val ) {
		int32_t i32Sign = 0;
		int32_t i32Exp;
		int32_t i32Mant;
		if ( _i32Val < 0 ) {
			i32Sign = 0x80000000;
			_i32Val = -_i32Val;
		}
		i32Exp = static_cast<int32_t>(::floor( ::log( _i32Val ) / ::log( 2.0f ) + 0.001f )); // +Epsilon.
		i32Mant = static_cast<int32_t>(rint( ldexp( _i32Val, (VQ_FMAN - 1) - i32Exp ) ));
		i32Exp = (i32Exp + VQ_FEXP_BIAS) << VQ_FMAN;

		return i32Sign | i32Exp | i32Mant;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _i32Step Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	/*int32_t CVorbisCodeBook::_best( codebook * _pcbBook, float * _pfA, int32_t _i32Step ) {
	}*/

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::_ilog( uint32_t _i32V ) {
		int32_t i32Ret = 0;
		while ( _i32V ) {
			i32Ret++;
			_i32V >>= 1;
		}
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pscbB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::_book_maptype1_quantvals( const static_codebook * _pscbB ) {
		int32_t i32Vals = static_cast<int32_t>(std::floor( std::pow( static_cast<float>(_pscbB->entries), 1.0f / _pscbB->dim ) ));

		// The above should be reliable, but we'll not assume that FP is
		//	ever reliable when bitstream sync is at stake; verify via integer
		//	means that i32Vals really is the greatest value of dim for which
		//	i32Vals^_pscbB->bim <= _pscbB->entries.
		// Treat the above as an initial guess.
		while ( true ) {
			int32_t i32Acc = 1;
			int32_t i32Acc1 = 1;
			int32_t I;
			for ( I = 0; I < _pscbB->dim; I++ ) {
				i32Acc *= i32Vals;
				i32Acc1 *= i32Vals + 1;
			}
			if ( i32Acc <= _pscbB->entries && i32Acc1 > _pscbB->entries ) {
				return i32Vals;
			}
			else {
				if ( i32Acc > _pscbB->entries ) {
					i32Vals--;
				}
				else {
					i32Vals++;
				}
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param i32Step Undocumented parameter from the Vorbis library.
	 * \param _i32AddMul Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	/*int32_t CVorbisCodeBook::vorbis_book_besterror( codebook * _pcbBook, float * _pfA, int32_t i32Step, int32_t _i32AddMul ) {
	}*/

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _i32Entry Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_codeword( codebook * _pcbBook, int32_t _i32Entry ) {
		if ( _pcbBook->c ) {	// Only use with encode; decode optimizations are
								//	allowed to break this.
			return _pcbBook->codelist[_i32Entry];
		}
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _i32Entry Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_codelen( codebook * _pcbBook, int32_t _i32Entry ) {
		if ( _pcbBook->c ) {	// only use with encode; decode optimizations are
								//	allowed to break this.
			return _pcbBook->c->lengthlist[_i32Entry];
		}
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pscbC Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_staticbook_pack( const static_codebook * _pscbC, COgg::oggpack_buffer * _ppbB ) {
		int32_t I, J;
		int32_t i32Ordered = 0;

		// First the basic parameters.
		COgg::oggpack_write( _ppbB, 0x564342, 24 );
		COgg::oggpack_write( _ppbB, _pscbC->dim, 16 );
		COgg::oggpack_write( _ppbB, _pscbC->entries, 24 );

		// Pack the codewords.  There are two packings; length ordered and
		//	length random.  Decide between the two now.

		for ( I = 1; I < _pscbC->entries; I++ ) {
			if ( _pscbC->lengthlist[I-1] == 0 || _pscbC->lengthlist[I] < _pscbC->lengthlist[I-1] ) { break; }
		}
		if ( I == _pscbC->entries ) { i32Ordered = 1; }

		if ( i32Ordered ) {
			// Length ordered.  We only need to say how many codewords of
			//	each length.  The actual codewords are generated
			//	deterministically.

			int32_t i32Count = 0;
			COgg::oggpack_write( _ppbB, 1, 1 );	// Ordered.
			COgg::oggpack_write( _ppbB, _pscbC->lengthlist[0]-1, 5 ); // 1 to 32.

			for ( I = 1; I < _pscbC->entries; I++ ) {
				int32_t i32This = _pscbC->lengthlist[I];
				int32_t i32Last = _pscbC->lengthlist[I-1];
				if ( i32This > i32Last ) {
					for ( J = i32Last; J < i32This; J++ ) {
						COgg::oggpack_write( _ppbB, I - i32Count, _ilog( _pscbC->entries - i32Count ) );
						i32Count = I;
					}
				}
			}
			COgg::oggpack_write( _ppbB, I-i32Count, _ilog( _pscbC->entries-i32Count ) );

		}
		else {
			// Length random.  Again, we don't code the codeword itself, just
			//	the length.  This time, though, we have to encode each length.
			COgg::oggpack_write( _ppbB, 0, 1 );		// Unordered.

			// Algortihmic mapping has use for "unused entries", which we tag
			//	here.  The algorithmic mapping happens as usual, but the unused
			//	entry has no codeword.
			for ( I = 0; I < _pscbC->entries; I++ ) {
				if ( _pscbC->lengthlist[I] == 0 ) { break; }
			}

			if ( I == _pscbC->entries ) {
				COgg::oggpack_write( _ppbB, 0, 1 );		// No unused entries.
				for ( I = 0; I < _pscbC->entries; I++ ) {
					COgg::oggpack_write( _ppbB, _pscbC->lengthlist[I] - 1, 5 );
				}
			}
			else {
				COgg::oggpack_write( _ppbB, 1, 1 );		// We have unused entries; thus we tag.
				for ( I = 0; I < _pscbC->entries; I++ ) {
					if ( _pscbC->lengthlist[I] == 0 ) {
						COgg::oggpack_write( _ppbB, 0, 1 );
					}
					else {
						COgg::oggpack_write( _ppbB, 1, 1 );
						COgg::oggpack_write( _ppbB, _pscbC->lengthlist[I] - 1, 5 );
					}
				}
			}
		}

		// Is the entry number the desired return value, or do we have a
		//	mapping? If we have a mapping, what type?
		COgg::oggpack_write( _ppbB, _pscbC->maptype, 4 );
		switch ( _pscbC->maptype ) {
			case 0 : {
				// No mapping.
				break;
			}
			case 1 : {}
			case 2 : {
				// Implicitly populated value mapping.
				// Explicitly populated value mapping.
				if ( !_pscbC->quantlist ) {
					// No quantlist?  Error.
					return -1;
				}

				// Values that define the dequantization.
				COgg::oggpack_write( _ppbB, _pscbC->q_min, 32 );
				COgg::oggpack_write( _ppbB, _pscbC->q_delta, 32 );
				COgg::oggpack_write( _ppbB, _pscbC->q_quant - 1, 4 );
				COgg::oggpack_write( _ppbB, _pscbC->q_sequencep, 1 );

				{
					int32_t i32QuantValues;
					switch ( _pscbC->maptype ) {
						case 1 : {
							// A single column of ( _pscbC->entries/_pscbC->dim ) quantized values for
							//	building a full value list algorithmically ( square lattice ).
							i32QuantValues = _book_maptype1_quantvals( _pscbC );
							break;
						}
						case 2 : {
							// Every value (_pscbC->entries * _pscbC->dim total) specified explicitly.
							i32QuantValues = _pscbC->entries * _pscbC->dim;
							break;
						}
						default : {				// NOT_REACHABLE.
							i32QuantValues = -1;
						}
					}

					// Quantized values.
					for ( I = 0; I < i32QuantValues; I++ ) {
						COgg::oggpack_write( _ppbB, static_cast<uint32_t>(labs( _pscbC->quantlist[I] )), _pscbC->q_quant );
					}

				}
				break;
			}
			default : {
				// Error case; we don't have any other map types now.
				return -1;
			}
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisCodeBook::static_codebook * CVorbisCodeBook::vorbis_staticbook_unpack( COgg::oggpack_buffer * _ppbB ) {
		int32_t I, J;
		static_codebook * pscbS = static_cast<static_codebook *>(CMemLib::CMAlloc( sizeof( (*pscbS) ) ));
		pscbS->allocedp = 1;

		// Make sure alignment is correct.
		if ( COgg::oggpack_read( _ppbB, 24 ) != 0x564342 ) { goto EofOut; }

		// First the basic parameters.
		pscbS->dim = COgg::oggpack_read( _ppbB, 16 );
		pscbS->entries = COgg::oggpack_read( _ppbB, 24 );
		if ( pscbS->entries == -1 ) { goto EofOut; }

		if ( _ilog( pscbS->dim ) + _ilog( pscbS->entries ) > 24 ) { goto EofOut; }

		// Codeword ordering.  Length ordered or unordered?
		switch ( static_cast<int32_t>(COgg::oggpack_read( _ppbB, 1 )) ) {
			case 0 : {
				int32_t i32Unused;
				// Allocated but i32Unused entries?
				i32Unused = COgg::oggpack_read( _ppbB, 1 );
				if ( (pscbS->entries * (i32Unused ? 1 : 5) + 7) >> 3 > _ppbB->storage - COgg::oggpack_bytes( _ppbB ) ) {
					goto EofOut;
				}
				// Unordered.
				pscbS->lengthlist = static_cast<int32_t *>(CMemLib::MAlloc( sizeof( *pscbS->lengthlist ) * pscbS->entries ));

				// Allocated but i32Unused entries?
				if ( i32Unused ) {
					// Yes, i32Unused entries.
					for ( I = 0; I < pscbS->entries; I++ ) {
						if ( COgg::oggpack_read( _ppbB, 1 ) ) {
							int32_t i32Num = COgg::oggpack_read( _ppbB, 5 );
							if ( i32Num == -1 ) { goto EofOut; }
							pscbS->lengthlist[I] = i32Num + 1;
						}
						else {
							pscbS->lengthlist[I] = 0;
						}
					}
				}
				else {
					// All entries used; no tagging.
					for ( I = 0; I < pscbS->entries; I++ ) {
						int32_t i32Num = COgg::oggpack_read( _ppbB, 5 );
						if ( i32Num == -1 ) { goto EofOut; }
						pscbS->lengthlist[I] = i32Num + 1;
					}
				}

				break;
			}
			case 1 : {
				// Ordered.
				{
					int32_t i32Length = COgg::oggpack_read( _ppbB, 5 )+1;
					if ( i32Length == 0 ) { goto EofOut; }
					pscbS->lengthlist = static_cast<int32_t *>(CMemLib::MAlloc( sizeof( (*pscbS->lengthlist) ) * pscbS->entries ));

					for ( I = 0; I < pscbS->entries; ) {
						int32_t i32Num = COgg::oggpack_read( _ppbB, _ilog( pscbS->entries - I ) );
						if ( i32Num == -1 ) { goto EofOut; }
						if ( i32Length > 32 || i32Num>pscbS->entries - I ||
							(i32Num > 0 && (i32Num - 1) >> (i32Length - 1) > 1) ) {
							goto ErrOut;
						}
						if ( i32Length > 32 ) { goto ErrOut; }
						for ( J = 0; J < i32Num; J++, I++ ) {
							pscbS->lengthlist[I] = i32Length;
						}
						i32Length++;
					}
				}
				break;
			}
			default : {
				// EOF.
				goto EofOut;
			}
		}

		// Do we have a mapping to unpack?
		switch ( (pscbS->maptype = COgg::oggpack_read( _ppbB, 4 )) ) {
			case 0 : {
				// No mapping.
				break;
			}
			case 1 : {}
			case 2 : {
				// Implicitly populated value mapping.
				// Explicitly populated value mapping.

				pscbS->q_min = COgg::oggpack_read( _ppbB, 32 );
				pscbS->q_delta = COgg::oggpack_read( _ppbB, 32 );
				pscbS->q_quant = COgg::oggpack_read( _ppbB, 4 ) + 1;
				pscbS->q_sequencep = COgg::oggpack_read( _ppbB, 1 );
				if ( pscbS->q_sequencep == -1 ) { goto EofOut; }

				{
					int32_t i32QuantValues = 0;
					switch ( pscbS->maptype ) {
						case 1 : {
							i32QuantValues = (pscbS->dim == 0 ? 0 : _book_maptype1_quantvals( pscbS ));
							break;
						}
						case 2 : {
							i32QuantValues = pscbS->entries * pscbS->dim;
							break;
						}
					}

					// Quantized values.
					// TODO: Verify the change made to the >> here.
					if ( ((i32QuantValues * pscbS->q_quant + 7) >> 3) > _ppbB->storage - COgg::oggpack_bytes( _ppbB ) ) {
						goto EofOut;
					}
					pscbS->quantlist = static_cast<int32_t *>(CMemLib::MAlloc( sizeof( (*pscbS->quantlist) ) * i32QuantValues ));
					for ( I = 0; I < i32QuantValues; I++ ) {
						pscbS->quantlist[I] = COgg::oggpack_read( _ppbB, pscbS->q_quant );
					}

					if ( i32QuantValues && pscbS->quantlist[i32QuantValues-1] == -1 ) { goto EofOut; }
				}
				break;
			}
			default : {
				goto ErrOut;
			}
		}

		// All set.
		return pscbS;

	ErrOut :
	EofOut :
		vorbis_staticbook_destroy( pscbS );
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _fA Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_encode( codebook * _pcbBook, int32_t _fA, COgg::oggpack_buffer * _ppbB ) {
		if ( _fA < 0 || _fA >= _pcbBook->c->entries ) { return 0; }
		COgg::oggpack_write( _ppbB, _pcbBook->codelist[_fA], _pcbBook->c->lengthlist[_fA] );
		return _pcbBook->c->lengthlist[_fA];
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_decode( codebook * _pcbBook, COgg::oggpack_buffer * _ppbB ) {
		if ( _pcbBook->used_entries>0 ) {
			int32_t packed_entry = decode_packed_entry_number( _pcbBook, _ppbB );
			if ( packed_entry >= 0 ) {
				return _pcbBook->dec_index[packed_entry];
			}
		}

		// If there's no dec_index, the codebook unpacking isn't collapsed.
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_decodevs_add( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N ) {
		if ( _pcbBook->used_entries > 0 ) {
			int32_t i32Step = _i32N / _pcbBook->dim;
			int32_t * pi32Entry = static_cast<int32_t *>(::alloca( sizeof( (*pi32Entry) ) * i32Step ));
			float ** ppifT = static_cast<float **>(::alloca( sizeof( *ppifT ) * i32Step ));
			int32_t I, J, i32O;

			for ( I = 0; I < i32Step; I++ ) {
				pi32Entry[I] = decode_packed_entry_number( _pcbBook, _ppbB );
				if ( pi32Entry[I] == -1 ) { return -1; }
				ppifT[I] = _pcbBook->valuelist+pi32Entry[I]*_pcbBook->dim;
			}
			for ( I = 0, i32O = 0; I < _pcbBook->dim; I++, i32O += i32Step ) {
				for ( J = 0; J < i32Step; J++ ) {
					_pfA[i32O+J] += ppifT[J][I];
				}
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_decodev_set( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N ) {
		if ( _pcbBook->used_entries>0 ) {
			int32_t I, J, i32Entry;
			float * pfT;

			for ( I = 0; I < _i32N; ) {
				i32Entry = decode_packed_entry_number( _pcbBook, _ppbB );
				if ( i32Entry == -1 ) { return -1; }
				pfT = _pcbBook->valuelist + i32Entry * _pcbBook->dim;
				for ( J = 0; J < _pcbBook->dim; ) {
					_pfA[I++] = pfT[J++];
				}
			}
		}
		else {
			int32_t I, J;

			for ( I = 0; I < _i32N; ) {
				for ( J = 0; J < _pcbBook->dim; ) {
					_pfA[I++] = 0.0f;
				}
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodeBook::vorbis_book_decodev_add( codebook * _pcbBook, float * _pfA, COgg::oggpack_buffer * _ppbB, int32_t _i32N ) {
		if ( _pcbBook->used_entries > 0 ) {
			int32_t I, J, i32Entry;
			float * pfT;

			if ( _pcbBook->dim > 8 ) {
				for ( I = 0; I < _i32N; ) {
					i32Entry = decode_packed_entry_number( _pcbBook, _ppbB );
					if ( i32Entry == -1 ) { return -1; }
					pfT = _pcbBook->valuelist + i32Entry * _pcbBook->dim;
					for ( J = 0; J < _pcbBook->dim; ) {
						_pfA[I++] += pfT[J++];
					}
				}
			}
			else {
				for ( I = 0; I < _i32N; ) {
					i32Entry = decode_packed_entry_number( _pcbBook, _ppbB );
					if ( i32Entry == -1 ) { return -1; }
					pfT = _pcbBook->valuelist + i32Entry * _pcbBook->dim;
					J = 0;
					switch ( static_cast<int32_t>(_pcbBook->dim) ) {
						case 8 : {
							_pfA[I++] += pfT[J++];
						}
						case 7 : {
							_pfA[I++] += pfT[J++];
						}
						case 6 : {
							_pfA[I++] += pfT[J++];
						}
						case 5 : {
							_pfA[I++] += pfT[J++];
						}
						case 4 : {
							_pfA[I++] += pfT[J++];
						}
						case 3 : {
							_pfA[I++] += pfT[J++];
						}
						case 2 : {
							_pfA[I++] += pfT[J++];
						}
						case 1 : {
							_pfA[I++] += pfT[J++];
						}
						case 0 : {
							break;
						}
					}
				}
			}
		}
		return 0;
	}

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
	int32_t CVorbisCodeBook::vorbis_book_decodevv_add( codebook * _pcbBook, float ** _ppfA, int32_t _i32Off, int32_t _i32Ch, COgg::oggpack_buffer * _ppbB, int32_t _i32N ) {
		int32_t I, J, i32Entry;
		int32_t i32Chptr = 0;
		if ( _pcbBook->used_entries > 0 ) {
			for ( I = _i32Off / _i32Ch; I < (_i32Off + _i32N) / _i32Ch; ) {
				i32Entry = decode_packed_entry_number( _pcbBook, _ppbB );
				if ( i32Entry == -1 ) { return -1; }
				{
					const float * pfT = _pcbBook->valuelist + i32Entry * _pcbBook->dim;
					for ( J = 0; J < _pcbBook->dim; J++ ) {
						_ppfA[i32Chptr++][I] += pfT[J];
						if ( i32Chptr == _i32Ch ) {
							i32Chptr = 0;
							I++;
						}
					}
				}
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pi32L Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32SparseCount Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	uint32_t * CVorbisCodeBook::_make_words( int32_t * _pi32L, int32_t _i32N, int32_t _i32SparseCount ) {
		int32_t I, J, i32Count = 0;
		uint32_t i32Marker[33];
		uint32_t * pi32R = static_cast<uint32_t *>(CMemLib::MAlloc( (_i32SparseCount ? _i32SparseCount : _i32N) * sizeof( (*pi32R) ) ));
		CStd::MemSet( i32Marker, 0, sizeof( i32Marker ) );

		for ( I = 0; I < _i32N; I++ ) {
			int32_t i32Length = _pi32L[I];
			if ( i32Length>0 ) {
				uint32_t i32Entry = i32Marker[i32Length];

				// When we claim a node for an i32Entry, we also claim the nodes
				//	below it ( pruning off the imagined tree that may have dangled
				//	from it ) as well as blocking the use of any nodes directly
				//	above for leaves.

				// Update ourself.
				if ( i32Length < 32 && (i32Entry >> i32Length) ) {
					// Error condition; the lengths must specify an overpopulated tree.
					CMemLib::Free( pi32R );
					return NULL;
				}
				pi32R[i32Count++] = i32Entry;

				// Look to see if the next shorter marker points to the node
				//	above. if so, update it and repeat.
				{
					for ( J = i32Length; J > 0; J-- ) {

						if ( i32Marker[J] & 1 ) {
							// Have to jump branches.
							if ( J == 1 ) {
								i32Marker[1]++;
							}
							else {
								i32Marker[J] = i32Marker[J-1] << 1;
							}
							break;	// Invariant says next upper marker would already
									//	have been moved if it was on the same path.
						}
						i32Marker[J]++;
					}
				}

				// Prune the tree; the implicit invariant says all the longer
				//	markers were dangling from our just-taken node.	Dangle them
				//	from our *new* node.
				for ( J = i32Length+1; J < 33; J++ )
					if ( (i32Marker[J] >> 1)  == i32Entry ) {
						i32Entry = i32Marker[J];
						i32Marker[J] = i32Marker[J-1] << 1;
					}
					else { break; }
			}
			else {
				if ( _i32SparseCount == 0 ) { i32Count++; }
			}
		}

		// Sanity check the huffman tree; an underpopulated tree must be
		//	rejected. The only exception is the one-node pseudo-nil tree, 
		//	which appears to be underpopulated because the tree doesn't
		//	really exist; there's only one possible "codeword" or zero bits, 
		//	but the above tree-gen code doesn't mark that.
		if ( _i32SparseCount != 1 ) {
			for ( I = 1; I < 33; I++ ) {
				if ( i32Marker[I] & (0xFFFFFFFFUL >> (32 - I)) ) {
				
					CMemLib::Free( pi32R );
					return NULL;
				}
			}
		}

		// Bitreverse the words because our bitwise packer/unpacker is LSb
		//	endian.
		for ( I = 0, i32Count = 0; I < _i32N; I++ ) {
			uint32_t i32Temp = 0;
			for ( J = 0; J < _pi32L[I]; J++ ) {
				i32Temp <<= 1;
				i32Temp |= ( pi32R[i32Count] >> J )&1;
			}

			if ( _i32SparseCount ) {
				if ( _pi32L[I] ) {
					pi32R[i32Count++] = i32Temp;
				}
			}
			else {
				pi32R[i32Count++] = i32Temp;
			}
		}

		return pi32R;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32X Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	uint32_t CVorbisCodeBook::bitreverse( uint32_t _ui32X ) {
		_ui32X = ((_ui32X >> 16) & 0x0000FFFFUL) | ((_ui32X << 16) & 0xFFFF0000UL);
		_ui32X = ((_ui32X >> 8) & 0x00FF00FFUL) | ((_ui32X << 8) & 0xFF00FF00UL);
		_ui32X = ((_ui32X >> 4) & 0x0F0F0F0FUL) | ((_ui32X << 4) & 0xF0F0F0F0UL);
		_ui32X = ((_ui32X >> 2) & 0x33333333UL ) | ((_ui32X << 2) & 0xCCCCCCCCUL);
		return ((_ui32X >> 1) & 0x55555555UL) | ((_ui32X << 1) & 0xAAAAAAAAUL);
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvContext Unused.
	 * \param _pvA Undocumented parameter from the Vorbis library.
	 * \param _pvB Undocumented parameter from the Vor_pvBis li_pvBrary.
	 * \return Undocumented return from the Vor_pvBis li_pvBrary.
	 */
	int32_t LSE_CCALL CVorbisCodeBook::sort32a( void * /*_pvContext*/, const void * _pvA, const void * _pvB ) {
		return ((*(*reinterpret_cast<const uint32_t * const *>(_pvA ))) > (*(*reinterpret_cast<const uint32_t * const *>(_pvB )))) -
			((*(*reinterpret_cast<const uint32_t * const *>(_pvA))) < (*(*reinterpret_cast<const uint32_t * const *>(_pvB))));
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcbBook Undocumented parameter from the Vorbis library.
	 * \param _ppbB Undocumented parameter from the Vor_pvBis li_pvBrary.
	 * \return Undocumented return from the Vor_pvBis li_pvBrary.
	 */
	int32_t CVorbisCodeBook::decode_packed_entry_number( codebook * _pcbBook, COgg::oggpack_buffer * _ppbB ) {
		int32_t	i32Read = _pcbBook->dec_maxlength;
		int32_t i32Lo, i32Hi;
		int32_t i32Lok = COgg::oggpack_look( _ppbB, _pcbBook->dec_firsttablen );

		if ( i32Lok >= 0 ) {
			int32_t i32Entry = _pcbBook->dec_firsttable[i32Lok];
			if ( i32Entry&0x80000000UL ) {
				i32Lo = ( i32Entry >> 15 )&0x7fff;
				i32Hi = _pcbBook->used_entries-( i32Entry&0x7fff );
			}
			else {
				COgg::oggpack_adv( _ppbB, _pcbBook->dec_codelengths[i32Entry-1] );
				return i32Entry-1;
			}
		}
		else {
			i32Lo = 0;
			i32Hi = _pcbBook->used_entries;
		}

		i32Lok = COgg::oggpack_look( _ppbB, i32Read );

		while ( i32Lok < 0 && i32Read > 1 ) {
			i32Lok = COgg::oggpack_look( _ppbB, --i32Read );
		}
		if ( i32Lok < 0 ) { return -1; }

		// Bisect search for the codeword in the i32Ordered list.
		{
			uint32_t ui32TestWord = bitreverse( static_cast<uint32_t>(i32Lok) );

			while ( i32Hi - i32Lo > 1 ) {
				int32_t i32P = (i32Hi - i32Lo) >> 1;
				int32_t i32Test = _pcbBook->codelist[i32Lo+i32P] > ui32TestWord;
				i32Lo += i32P & (i32Test - 1);
				i32Hi -= i32P & (-i32Test);
			}

			if ( _pcbBook->dec_codelengths[i32Lo] <= i32Read ) {
				COgg::oggpack_adv( _ppbB, _pcbBook->dec_codelengths[i32Lo] );
				return i32Lo;
			}
		}

		COgg::oggpack_adv( _ppbB, i32Read );

		return -1;
	}

}	// namespace lss

#pragma warning( pop )
