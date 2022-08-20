/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS ( C ) COPYRIGHT 1994-2010             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: residue backend 0, 1 and 2 implementation
 last mod: $Id: res0.i32C 17556 2010-10-21 18:25:19Z tterribe $

 ********************************************************************/

#include "LSSVorbisRes0.h"
#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisMisc.h"


namespace lss {

	// == Members.
	/**
	 * Undocumented Vorbis library function.
	 */
	const CVorbisBackends::vorbis_func_residue CVorbisRes0::residue0_exportbundle = {
		NULL, 
		res0_unpack, 
		res0_look, 
		res0_free_info, 
		res0_free_look, 
		NULL, 
		NULL, 
		res0_inverse
	};

	/**
	 * Undocumented Vorbis library function.
	 */
	const CVorbisBackends::vorbis_func_residue CVorbisRes0::residue1_exportbundle = {
		res0_pack, 
		res0_unpack, 
		res0_look, 
		res0_free_info, 
		res0_free_look, 
		res1_class, 
		res1_forward, 
		res1_inverse
	};

	/**
	 * Undocumented Vorbis library function.
	 */
	const CVorbisBackends::vorbis_func_residue CVorbisRes0::residue2_exportbundle = {
		res0_pack, 
		res0_unpack, 
		res0_look, 
		res0_free_info, 
		res0_free_look, 
		res2_class, 
		res2_forward, 
		res2_inverse
	};

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvirI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisRes0::res0_free_info( CVorbisStructs::vorbis_info_residue * _pvirI ) {
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = static_cast<CVorbisBackends::vorbis_info_residue0 *>(_pvirI);
		if ( pvirInfo ) {
			CStd::MemSet( pvirInfo, 0, sizeof( (*pvirInfo) ) );
			CMemLib::Free( pvirInfo );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvlrI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisRes0::res0_free_look( CVorbisStructs::vorbis_look_residue * _pvlrI ) {
		int32_t J;
		if ( _pvlrI ) {
			vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrI);

			for ( J = 0; J < pvlrLook->parts; J++ ) {
				if ( pvlrLook->partbooks[J] ) { CMemLib::Free( pvlrLook->partbooks[J] ); }
			}
			CMemLib::Free( pvlrLook->partbooks );
			for ( J = 0; J < pvlrLook->partvals; J++ ) {
				CMemLib::Free( pvlrLook->decodemap[J] );
			}
			CMemLib::Free( pvlrLook->decodemap );

			CStd::MemSet( pvlrLook, 0, sizeof( (*pvlrLook) ) );
			CMemLib::Free( pvlrLook );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvirVr Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 */
	void CVorbisRes0::res0_pack( CVorbisStructs::vorbis_info_residue * _pvirVr, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = static_cast<CVorbisBackends::vorbis_info_residue0 *>(_pvirVr);
		int32_t J, i32Acc = 0;
		COgg::oggpack_write( _pobOpb, pvirInfo->begin, 24 );
		COgg::oggpack_write( _pobOpb, pvirInfo->end, 24 );

		COgg::oggpack_write( _pobOpb, pvirInfo->grouping-1, 24 );		// Residue vectors to group and code with a partitioned book.
		COgg::oggpack_write( _pobOpb, pvirInfo->partitions-1, 6 );		// Possible partition choices.
		COgg::oggpack_write( _pobOpb, pvirInfo->groupbook, 8 );			// Group huffman book.

		// Secondstages is a bitmask; as encoding progresses pass by pass, a bitmask of one indicates this partition class has i32Bits to write this pass.
		for ( J = 0; J < pvirInfo->partitions; J++ ) {
			if ( ilog( pvirInfo->secondstages[J] ) > 3 ) {
				// Yes, this is a minor hack due to not thinking ahead.
				COgg::oggpack_write( _pobOpb, pvirInfo->secondstages[J], 3 );
				COgg::oggpack_write( _pobOpb, 1, 1 );
				COgg::oggpack_write( _pobOpb, pvirInfo->secondstages[J] >> 3, 5 );
			}
			else {
				COgg::oggpack_write( _pobOpb, pvirInfo->secondstages[J], 4 );
			}
			i32Acc += icount( pvirInfo->secondstages[J] );
		}
		for ( J = 0; J < i32Acc; J++ ) {
			COgg::oggpack_write( _pobOpb, pvirInfo->booklist[J], 8 );
		}

	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_info_residue * CVorbisRes0::res0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		int32_t J, i32Acc = 0;
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = static_cast<CVorbisBackends::vorbis_info_residue0 *>(CMemLib::CMAlloc( sizeof( (*pvirInfo) ) ));
		CVorbisCodecInternal::codec_setup_info * pvsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);

		pvirInfo->begin = COgg::oggpack_read( _pobOpb, 24 );
		pvirInfo->end = COgg::oggpack_read( _pobOpb, 24 );
		pvirInfo->grouping = COgg::oggpack_read( _pobOpb, 24 ) + 1;
		pvirInfo->partitions = COgg::oggpack_read( _pobOpb, 6 ) + 1;
		pvirInfo->groupbook = COgg::oggpack_read( _pobOpb, 8 );

		// Check for premature EOP.
		if ( pvirInfo->groupbook < 0 ) { goto ErrOut; }

		for ( J = 0; J < pvirInfo->partitions; J++ ) {
			int32_t i32Cascade = COgg::oggpack_read( _pobOpb, 3 );
			int32_t i32CFlag = COgg::oggpack_read( _pobOpb, 1 );
			if ( i32CFlag < 0 ) { goto ErrOut; }
			if ( i32CFlag ) {
				int32_t i32C = COgg::oggpack_read( _pobOpb, 5 );
				if ( i32C < 0 ) { goto ErrOut; }
				i32Cascade |= (i32C << 3);
			}
			pvirInfo->secondstages[J] = i32Cascade;

			i32Acc += icount( i32Cascade );
		}
		for ( J = 0; J < i32Acc; J++ ) {
			int32_t i32Book = COgg::oggpack_read( _pobOpb, 8 );
			if ( i32Book < 0 ) { goto ErrOut; }
			pvirInfo->booklist[J] = i32Book;
		}

		if ( pvirInfo->groupbook >= pvsiCi->books ) { goto ErrOut; }
		for ( J = 0; J < i32Acc; J++ ) {
			if ( pvirInfo->booklist[J] >= pvsiCi->books ) { goto ErrOut; }
			if ( pvsiCi->book_param[pvirInfo->booklist[J]]->maptype == 0 ) { goto ErrOut; }
		}

		// Verify the phrasebook is not specifying an impossible or inconsistent partitioning scheme.
		// Modify the phrasebook ranging check from r16327; an early beta
		//	encoder had a bug where it i32Used an oversized phrasebook by
		//	accident.  These files should continue to be playable, but don't
		//	allow an exploit.
		{
			int32_t i32Entries = pvsiCi->book_param[pvirInfo->groupbook]->entries;
			int32_t i32Dim = pvsiCi->book_param[pvirInfo->groupbook]->dim;
			int32_t i32PartVals = 1;
			if ( i32Dim < 1 ) { goto ErrOut; }
			while ( i32Dim > 0 ) {
				i32PartVals *= pvirInfo->partitions;
				if ( i32PartVals > i32Entries ) { goto ErrOut; }
				i32Dim--;
			}
			pvirInfo->partvals = i32PartVals;
		}

		return pvirInfo;
	ErrOut :
		res0_free_info( pvirInfo );
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _pvirVr Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_look_residue * CVorbisRes0::res0_look( CVorbisCodec::vorbis_dsp_state * _pvdsVd, 
		CVorbisStructs::vorbis_info_residue * _pvirVr ) {
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = static_cast<CVorbisBackends::vorbis_info_residue0 *>(_pvirVr);
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(CMemLib::CMAlloc( sizeof( (*pvlrLook) ) ));
		CVorbisCodecInternal::codec_setup_info * pvsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvdsVd->vi->codec_setup);

		int32_t J, K, i32Acc = 0;
		int32_t i32Dim;
		int32_t i32MaxStage = 0;
		pvlrLook->info = pvirInfo;

		pvlrLook->parts = pvirInfo->partitions;
		pvlrLook->fullbooks = pvsiCi->fullbooks;
		pvlrLook->phrasebook = pvsiCi->fullbooks+pvirInfo->groupbook;
		i32Dim = pvlrLook->phrasebook->dim;

		pvlrLook->partbooks = static_cast<CVorbisCodeBook::codebook ***>(CMemLib::CMAlloc( pvlrLook->parts * sizeof( (*pvlrLook->partbooks) ) ));

		for ( J = 0; J < pvlrLook->parts; J++ ) {
			int32_t i32Stages = ilog( pvirInfo->secondstages[J] );
			if ( i32Stages ) {
				if ( i32Stages>i32MaxStage ) { i32MaxStage = i32Stages; }
				pvlrLook->partbooks[J] = static_cast<CVorbisCodeBook::codebook **>(CMemLib::CMAlloc( i32Stages * sizeof( (*pvlrLook->partbooks[J]) ) ));
				for ( K = 0; K < i32Stages; K++ )
					if ( pvirInfo->secondstages[J] & (1 << K) ) {
						pvlrLook->partbooks[J][K] = pvsiCi->fullbooks + pvirInfo->booklist[i32Acc++];

					}
			}
		}

		pvlrLook->partvals = 1;
		for ( J = 0; J < i32Dim; J++ ) {
			pvlrLook->partvals *= pvlrLook->parts;
		}

		pvlrLook->stages = i32MaxStage;
		pvlrLook->decodemap = static_cast<int32_t **>(CMemLib::MAlloc( pvlrLook->partvals * sizeof( (*pvlrLook->decodemap) ) ));
		for ( J = 0; J < pvlrLook->partvals; J++ ) {
			int32_t i32Val = J;
			int32_t i32Mult = pvlrLook->partvals/pvlrLook->parts;
			pvlrLook->decodemap[J] = static_cast<int32_t *>(CMemLib::MAlloc( i32Dim * sizeof( (*pvlrLook->decodemap[J]) ) ));
			for ( K = 0; K < i32Dim; K++ ) {
				int32_t i32Deco = i32Val / i32Mult;
				i32Val -= i32Deco * i32Mult;
				i32Mult /= pvlrLook->parts;
				pvlrLook->decodemap[J][K] = i32Deco;
			}
		}

		return pvlrLook;
	}

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
	int32_t CVorbisRes0::res0_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, float ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch ) {
		int32_t I, i32Used = 0;
		for ( I = 0; I < _i32Ch; I++ ) {
			if ( _pi32NonZero[I] ) {
				_ppi32In[i32Used++] = _ppi32In[I];
			}
		}
		if ( i32Used ) {
			return  _01inverse( _pvbVb, _pvlrVl, _ppi32In, i32Used, CVorbisCodeBook::vorbis_book_decodevs_add );
		}
		else {
			return 0;
		}
	}

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
	int32_t ** CVorbisRes0::res1_class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch ) {
		int32_t I, i32Used = 0;
		for ( I = 0; I < _i32Ch; I++ ) {
			if ( _pi32NonZero[I] ) {
				_ppi32In[i32Used++] = _ppi32In[I];
			}
		}
		if ( i32Used ) {
			return _01class( _pvbVb, _pvlrVl, _ppi32In, i32Used );
		}
		else {
			return 0;
		}
	}

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
	int32_t CVorbisRes0::res1_forward( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch, int32_t ** _ppi32PartWord, int32_t _i32SubMap ) {
		int32_t I, i32Used = 0;
		for ( I = 0; I < _i32Ch; I++ ) {
			if ( _pi32NonZero[I] ) {
				_ppi32In[i32Used++] = _ppi32In[I];
			}
		}

		if ( i32Used ) {
			return _01forward( _pobOpb, _pvbVb, _pvlrVl, _ppi32In, i32Used, _ppi32PartWord, _encodepart, _i32SubMap );
		}
		else {
			return 0;
		}
	}

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
	int32_t CVorbisRes0::res1_inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		float ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch ) {
		int32_t I, i32Used = 0;
		for ( I = 0; I < _i32Ch; I++ ) {
			if ( _pi32NonZero[I] ) {
				_ppi32In[i32Used++] = _ppi32In[I];
			}
		}
		if ( i32Used ) {
			return _01inverse( _pvbVb, _pvlrVl, _ppi32In, i32Used, CVorbisCodeBook::vorbis_book_decodev_add );
		}
		else {
			return 0;
		}
	}

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
	int32_t ** CVorbisRes0::res2_class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch ) {
		int32_t I, i32Used = 0;
		for ( I = 0; I < _i32Ch; I++ ) {
			if ( _pi32NonZero[I] ) { i32Used++; }
		}
		if ( i32Used ) {
			return _2class( _pvbVb, _pvlrVl, _ppi32In, _i32Ch );
		}
		else {
			return 0;
		}

	}

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
	int32_t CVorbisRes0::res2_forward( COgg::oggpack_buffer * _pbOpb, 
		CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch, int32_t ** i32PartWord, int32_t i32SubMap ) {
		int32_t I, J, K, i32N = _pvbVb->pcmend / 2, i32Used = 0;

		// Don't duplicate the code; use a working vector hack for now and reshape ourselves into a single channel res1.
		// Ugly; reallocs for each coupling pass.
		int32_t * pi32Work = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, _i32Ch * i32N * sizeof( (*pi32Work) ) ));
		for ( I = 0; I < _i32Ch; I++ ) {
			int32_t * pi32Pcm = _ppi32In[I];
			if ( _pi32NonZero[I] ) { i32Used++; }
			for ( J = 0, K = I; J < i32N; J++, K += _i32Ch ) {
				pi32Work[K] = pi32Pcm[J];
			}
		}

		if ( i32Used ) {
			return _01forward( _pbOpb, _pvbVb, _pvlrVl, &pi32Work, 1, i32PartWord, _encodepart, i32SubMap );
		}
		else {
			return 0;
		}
	}

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
	int32_t CVorbisRes0::res2_inverse( CVorbisCodec::vorbis_block *_pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		float **_ppi32In, int32_t * _pi32NonZero, int32_t _i32Ch ) {
		int32_t I, K, L, i32S;
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrVl);
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = pvlrLook->info;

		// Move all this setup out later.
		int32_t i32SamplesPerPartition = pvirInfo->grouping;
		int32_t i32PartitionsPerWord = pvlrLook->phrasebook->dim;
		int32_t i32Max = (_pvbVb->pcmend * _i32Ch) >> 1;
		int32_t i32End = (pvirInfo->end < i32Max ? pvirInfo->end : i32Max);
		int32_t i32N = i32End-pvirInfo->begin;

		if ( i32N > 0 ) {
			int32_t i32PartVals = i32N / i32SamplesPerPartition;
			int32_t i32PartWords = (i32PartVals + i32PartitionsPerWord - 1) / i32PartitionsPerWord;
			int32_t ** ppi32PartWord = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32PartWords * sizeof( (*ppi32PartWord) ) ));

			for ( I = 0; I < _i32Ch; I++ ) {
				if ( _pi32NonZero[I] ) { break; }
			}
			if ( I == _i32Ch ) { return 0; }

			for ( i32S = 0; i32S < pvlrLook->stages; i32S++ ) {
				for ( I = 0, L = 0; I < i32PartVals; L++ ) {

					if ( i32S == 0 ) {
						// Fetch the partition word.
						int32_t i32Temp = CVorbisCodeBook::vorbis_book_decode( pvlrLook->phrasebook, &_pvbVb->opb );
						if ( i32Temp == -1 || i32Temp >= pvirInfo->partvals ) { goto EopBreak; }
						ppi32PartWord[L] = pvlrLook->decodemap[i32Temp];
						if ( ppi32PartWord[L] == NULL ) { goto ErrOut; }
					}

					// Now we decode residual values for the partitions.
					for ( K = 0; K < i32PartitionsPerWord && I < i32PartVals; K++, I++ ) {
						if ( pvirInfo->secondstages[ppi32PartWord[L][K]] & (1 << i32S) ) {
							CVorbisCodeBook::codebook * pcbStageBook = pvlrLook->partbooks[ppi32PartWord[L][K]][i32S];

							if ( pcbStageBook ) {
								if ( CVorbisCodeBook::vorbis_book_decodevv_add( pcbStageBook, _ppi32In, 
									I * i32SamplesPerPartition + pvirInfo->begin, _i32Ch, 
									&_pvbVb->opb, i32SamplesPerPartition ) == -1 ) {
									goto EopBreak;
								}
							}
						}
					}
				}
			}
		}
	ErrOut :
	EopBreak :
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisRes0::ilog( uint32_t _ui32V ) {
		int32_t i32Ret = 0;
		while ( _ui32V ) {
			i32Ret++;
			_ui32V  >>= 1;
		}
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisRes0::icount( uint32_t _ui32V ) {
		int32_t i32Ret = 0;
		while ( _ui32V ) {
			i32Ret += _ui32V&1;
			_ui32V  >>= 1;
		}
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlrVl Undocumented parameter from the Vorbis library.
	 * \param _ppi32In Undocumented parameter from the Vorbis library.
	 * \param _i32Ch Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t ** CVorbisRes0::_01class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t _i32Ch ) {
		int32_t I, J, K;
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrVl);
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = static_cast<CVorbisBackends::vorbis_info_residue0 *>(pvlrLook->info);

		// Move all this setup out later.
		int32_t i32SamplesPerPartition = pvirInfo->grouping;
		int32_t i32PossiblePartitions = pvirInfo->partitions;
		int32_t i32N = pvirInfo->end-pvirInfo->begin;

		int32_t i32PartVals = i32N/i32SamplesPerPartition;
		int32_t ** _ppi32PartWord = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, _i32Ch*sizeof( *_ppi32PartWord ) ) );
		float fScale = 100.0f / i32SamplesPerPartition;

		// We find the partition type for each partition of each channel.  We'll go back and do the interleaved encoding in a bit.	For now, clarity.

		for ( I = 0; I < _i32Ch; I++ ) {
			_ppi32PartWord[I] = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32N/i32SamplesPerPartition * sizeof( (*_ppi32PartWord[I]) ) ));
			CStd::MemSet( _ppi32PartWord[I], 0, i32N / i32SamplesPerPartition * sizeof( (*_ppi32PartWord[I]) ) );
		}

		for ( I = 0; I < i32PartVals; I++ ) {
			int32_t i32Offset = I * i32SamplesPerPartition + pvirInfo->begin;
			for ( J = 0; J < _i32Ch; J++ ) {
				int32_t i32Max = 0;
				int32_t i32Ent = 0;
				for ( K = 0; K < i32SamplesPerPartition; K++ ) {
					if ( ::abs( _ppi32In[J][i32Offset+K] ) > i32Max ) { i32Max = ::abs( _ppi32In[J][i32Offset+K] ); }
					i32Ent += ::abs( _ppi32In[J][i32Offset+K] );
				}
				i32Ent = static_cast<int32_t>(i32Ent * fScale );	// TODO: Verify i32This is the same as i32Ent *= fScale.

				for ( K = 0; K < i32PossiblePartitions-1; K++ ) {
					if ( i32Max <= pvirInfo->classmetric1[K] &&
						( pvirInfo->classmetric2[K] < 0 || i32Ent<pvirInfo->classmetric2[K] ) ) {
						break;
					}
				}

				_ppi32PartWord[J][I] = K;
			}
		}


		pvlrLook->frames++;

		return _ppi32PartWord;
	}

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
	int32_t CVorbisRes0::_encodepart( COgg::oggpack_buffer * _pobOpb, int32_t * _pi32Vec, int32_t _i32N, 
		CVorbisCodeBook::codebook * _pcbBook, int32_t * /*_pi32Acc*/ ) {
		int32_t I, i32Bits = 0;
		int32_t i32Dim = _pcbBook->dim;
		int32_t i32Step = _i32N / i32Dim;

		for ( I = 0; I < i32Step; I++ ) {
			int32_t i32Entry = local_book_besterror( _pcbBook, _pi32Vec + I * i32Dim );

			i32Bits += CVorbisCodeBook::vorbis_book_encode( _pcbBook, i32Entry, _pobOpb );

		}

		return i32Bits;
	}

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
	int32_t CVorbisRes0::_01forward( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_block * /*_pvbVb*/, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		int32_t ** _ppi32In, int32_t _i32Ch, 
		int32_t ** _ppi32PartWord, 
		int32_t (* _pfEncode)( COgg::oggpack_buffer *, int32_t *, int32_t, CVorbisCodeBook::codebook *, int32_t * ), 
		int32_t /*_i32SubMap*/ ) {
		int32_t I, J, K, i32S;
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrVl);
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = pvlrLook->info;


		// Move all this setup out later.
		int32_t i32SamplesPerPartition = pvirInfo->grouping;
		int32_t i32PossiblePartitions = pvirInfo->partitions;
		int32_t i32PartitionsPerWord = pvlrLook->phrasebook->dim;
		int32_t i32N = pvirInfo->end-pvirInfo->begin;

		int32_t i32PartVals = i32N/i32SamplesPerPartition;
		int32_t i32ResBits[128];
		int32_t i32ResVals[128];



		CStd::MemSet( i32ResBits, 0, sizeof( i32ResBits ) );
		CStd::MemSet( i32ResVals, 0, sizeof( i32ResVals ) );

		// We code the partition words for each channel, then the residual words for a partition per channel until we've written all the
		//	residual words for that partition word.  Then write the next partition channel words.

		for ( i32S = 0; i32S < pvlrLook->stages; i32S++ ) {

			for ( I = 0; I < i32PartVals; ) {

				// First we encode a partition codeword for each channel.
				if ( i32S == 0 ) {
					for ( J = 0; J < _i32Ch; J++ ) {
						int32_t i32Val = _ppi32PartWord[J][I];
						for ( K = 1; K < i32PartitionsPerWord; K++ ) {
							i32Val *= i32PossiblePartitions;
							if ( I + K < i32PartVals ) {
								i32Val += _ppi32PartWord[J][I+K];
							}
						}

						// Training hack.
						if ( i32Val<pvlrLook->phrasebook->entries ) {
							pvlrLook->phrasebits += CVorbisCodeBook::vorbis_book_encode( pvlrLook->phrasebook, i32Val, _pobOpb );
						}

					}
				}

				// Now we encode interleaved residual values for the partitions.
				for ( K = 0; K < i32PartitionsPerWord && I<i32PartVals; K++, I++ ) {
					int32_t i32Offset = I * i32SamplesPerPartition + pvirInfo->begin;

					for ( J = 0; J < _i32Ch; J++ ) {
						if ( i32S == 0 ) { i32ResVals[_ppi32PartWord[J][I]] += i32SamplesPerPartition; }
						if ( pvirInfo->secondstages[_ppi32PartWord[J][I]] & (1 << i32S) ) {
							CVorbisCodeBook::codebook * pcbStateBook = pvlrLook->partbooks[_ppi32PartWord[J][I]][i32S];
							if ( pcbStateBook ) {
								int32_t i32Ret;
								int32_t * i32Accumulator = NULL;


								i32Ret = _pfEncode( _pobOpb, _ppi32In[J] + i32Offset, i32SamplesPerPartition, pcbStateBook, i32Accumulator );

								pvlrLook->postbits += i32Ret;
								i32ResBits[_ppi32PartWord[J][I]] += i32Ret;
							}
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
	 * \param _pi32A Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisRes0::local_book_besterror( CVorbisCodeBook::codebook * _pcbBook, int32_t * _pi32A ) {
		int32_t i32Dim = _pcbBook->dim;
		int32_t I, J, i32O;
		int32_t i32MinVal = _pcbBook->minval;
		int32_t i32Del = _pcbBook->delta;
		int32_t i32Qv = _pcbBook->quantvals;
		int32_t i32Ze = (i32Qv >> 1);
		int32_t i32Index = 0;
		// Assumes integer/centered encoder codebook maptype 1 no more than dim 8.
		int32_t i32P[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

		if ( i32Del != 1 ) {
			for ( I = 0, i32O = i32Dim; I < i32Dim; I++ ) {
				int32_t i32V = (_pi32A[--i32O] - i32MinVal + (i32Del >> 1)) / i32Del;
				int32_t i32M = (i32V < i32Ze ? ((i32Ze - i32V) << 1) - 1 : ((i32V - i32Ze) << 1));
				i32Index = i32Index * i32Qv + (i32M < 0 ? 0 : (i32M >= i32Qv ? i32Qv - 1 : i32M));
				i32P[i32O] = i32V * i32Del + i32MinVal;
			}
		}
		else {
			for ( I = 0, i32O = i32Dim; I < i32Dim; I++ ) {
				int32_t i32V = _pi32A[--i32O] - i32MinVal;
				int32_t i32M = (i32V < i32Ze ? ((i32Ze - i32V) << 1) - 1 : ((i32V - i32Ze) << 1));
				i32Index = i32Index * i32Qv + (i32M < 0 ? 0 : (i32M >= i32Qv ? i32Qv - 1 : i32M));
				i32P[i32O] = i32V * i32Del + i32MinVal;
			}
		}

		if ( _pcbBook->c->lengthlist[i32Index] <= 0 ) {
			const CVorbisCodeBook::static_codebook * pcbC = _pcbBook->c;
			int32_t i32Best = -1;
			// Assumes integer/centered encoder codebook maptype 1 no more than dim 8.
			int32_t i32E[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
			int32_t i32MaxVal = _pcbBook->minval + _pcbBook->delta * (_pcbBook->quantvals - 1);
			for ( I = 0; I < _pcbBook->entries; I++ ) {
				if ( pcbC->lengthlist[I] > 0 ) {
					int32_t i32This = 0;
					for ( J = 0; J < i32Dim; J++ ) {
						int32_t i32Val = (i32E[J] - _pi32A[J]);
						i32This += i32Val * i32Val;
					}
					if ( i32Best == -1 || i32This < i32Best ) {
						CStd::MemCpy( i32P, i32E, sizeof( i32P ) );
						i32Best = i32This;
						i32Index = I;
					}
				}
				// Assumes the value patterning created by the tools in vq.
				J = 0;
				while ( i32E[J] >= i32MaxVal ) {
					i32E[J++] = 0;
				}
				if ( i32E[J] >= 0 ) {
					i32E[J] += _pcbBook->delta;
				}
				i32E[J] = -i32E[J];
			}
		}

		if ( i32Index > -1 ) {
			for ( I = 0; I < i32Dim; I++ ) {
				*_pi32A++ -= i32P[I];
			}
		}

		return i32Index;
	}

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
	int32_t CVorbisRes0::_01inverse( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, 
		float ** _ppi32In, int32_t _i32Ch, 
		int32_t (* _pfEncode)( CVorbisCodeBook::codebook *, float *, COgg::oggpack_buffer *, int32_t ) ) {

		int32_t I, J, K, L, i32S;
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrVl);
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = pvlrLook->info;

		// Move all this setup out later.
		int32_t i32SamplesPerPartition = pvirInfo->grouping;
		int32_t i32PartitionsPerWord = pvlrLook->phrasebook->dim;
		int32_t i32Max = _pvbVb->pcmend >> 1;
		int32_t i32End = (pvirInfo->end < i32Max ? pvirInfo->end : i32Max);
		int32_t i32N = i32End - pvirInfo->begin;

		if ( i32N > 0 ) {
			int32_t i32PartVals = i32N / i32SamplesPerPartition;
			int32_t i32PartWords = (i32PartVals + i32PartitionsPerWord - 1) / i32PartitionsPerWord;
			int32_t *** i32PartWord = static_cast<int32_t ***>(::alloca( _i32Ch * sizeof( (*i32PartWord) ) ) );

			for ( J = 0; J < _i32Ch; J++ ) {
				i32PartWord[J] = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32PartWords * sizeof( (*i32PartWord[J]) ) ));
			}

			for ( i32S = 0; i32S < pvlrLook->stages; i32S++ ) {

				// Each loop decodes on partition codeword containing
				//	i32PartitionsPerWord partitions.
				for ( I = 0, L = 0; I < i32PartVals;L++ ) {
					if ( i32S == 0 ) {
						// Fetch the partition word for each channel.
						for ( J = 0; J < _i32Ch; J++ ) {
							int32_t i32Temp = CVorbisCodeBook::vorbis_book_decode( pvlrLook->phrasebook, &_pvbVb->opb );

							if ( i32Temp == -1 || i32Temp >= pvirInfo->partvals ) { goto EopBreak; }
							i32PartWord[J][L] = pvlrLook->decodemap[i32Temp];
							if ( i32PartWord[J][L] == NULL ) { goto ErrOut; }
						}
					}

					// Now we decode residual values for the partitions.
					for ( K = 0; K < i32PartitionsPerWord && I < i32PartVals; K++, I++ )
						for ( J = 0; J < _i32Ch; J++ ) {
							int32_t i32Offset = pvirInfo->begin + I * i32SamplesPerPartition;
							if ( pvirInfo->secondstages[i32PartWord[J][L][K]] & (1 << i32S) ) {
								CVorbisCodeBook::codebook * pcbStageBook = pvlrLook->partbooks[i32PartWord[J][L][K]][i32S];
								if ( pcbStageBook ) {
									if ( _pfEncode( pcbStageBook, _ppi32In[J] + i32Offset, &_pvbVb->opb, 
										i32SamplesPerPartition ) == -1 ) { goto EopBreak; }
								}
							}
						}
				}
			}
		}
	ErrOut:
	EopBreak:
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlrVl Undocumented parameter from the Vorbis library.
	 * \param _ppi32In Undocumented parameter from the Vorbis library.
	 * \param _i32Ch Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t ** CVorbisRes0::_2class( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_residue * _pvlrVl, int32_t ** _ppi32In, 
		int32_t _i32Ch ) {
		int32_t I, J, K, L;
		vorbis_look_residue0 * pvlrLook = static_cast<vorbis_look_residue0 *>(_pvlrVl);
		CVorbisBackends::vorbis_info_residue0 * pvirInfo = pvlrLook->info;

		// Move all this setup out later.
		int32_t i32SamplesPerPartition = pvirInfo->grouping;
		int32_t i32PossiblePartitions = pvirInfo->partitions;
		int32_t i32N = pvirInfo->end-pvirInfo->begin;

		int32_t i32PartVals = i32N/i32SamplesPerPartition;
		int32_t ** ppi32PartWord = static_cast<int32_t **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*ppi32PartWord) ) ));

		ppi32PartWord[0] = static_cast<int32_t *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, i32PartVals * sizeof( (*ppi32PartWord[0]) ) ));
		CStd::MemSet( ppi32PartWord[0], 0, i32PartVals * sizeof( (*ppi32PartWord[0]) ) );

		for ( I = 0, L = pvirInfo->begin / _i32Ch; I < i32PartVals; I++ ) {
			int32_t i32MagMax = 0;
			int32_t i32AngMax = 0;
			for ( J = 0; J < i32SamplesPerPartition; J += _i32Ch ) {
				if ( ::abs( _ppi32In[0][L] ) > i32MagMax ) { i32MagMax = ::abs( _ppi32In[0][L] ); }
				for ( K = 1; K < _i32Ch; K++ ) {
					if ( ::abs( _ppi32In[K][L] )>i32AngMax ) { i32AngMax = ::abs( _ppi32In[K][L] ); }
				}
				L++;
			}

			for ( J = 0; J < i32PossiblePartitions - 1; J++ ) {
				if ( i32MagMax <= pvirInfo->classmetric1[J] &&
					i32AngMax <= pvirInfo->classmetric2[J] ) {
					break;
				}
			}

			ppi32PartWord[0][I] = J;

		}


		pvlrLook->frames++;

		return ppi32PartWord;
	}

}	// namespace lss
