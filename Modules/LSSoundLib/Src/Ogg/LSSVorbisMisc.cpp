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

 function: miscellaneous prototypes
 last mod: $Id: misc.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisMisc.h"
#include "LSSVorbisCodec.h"
#include "LSSVorbisConstants.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _i32Bytes Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	void * CVorbisMisc::_vorbis_block_alloc( CVorbisCodec::vorbis_block * _pvbVb, int32_t _i32Bytes ) {
		_i32Bytes = (_i32Bytes+( WORD_ALIGN-1 ) ) & ~( WORD_ALIGN-1 );
		if ( _i32Bytes+_pvbVb->localtop>_pvbVb->localalloc ) {
			// Can't just CMemLib::MReAlloc().  There are outstanding pointers.
			if ( _pvbVb->localstore ) {
				CVorbisCodec::alloc_chain * pacLink = static_cast<CVorbisCodec::alloc_chain *>(CMemLib::MAlloc( sizeof( (*pacLink) ) ));
				_pvbVb->totaluse += _pvbVb->localtop;
				pacLink->next = _pvbVb->reap;
				pacLink->ptr = _pvbVb->localstore;
				_pvbVb->reap = pacLink;
			}
			// Highly conservative.
			_pvbVb->localalloc = _i32Bytes;
			_pvbVb->localstore = CMemLib::MAlloc( _pvbVb->localalloc );
			_pvbVb->localtop = 0;
		}
		{
			void * pvRet = static_cast<void *>(static_cast<int8_t *>(_pvbVb->localstore) + _pvbVb->localtop);
			_pvbVb->localtop += _i32Bytes;
			return pvRet;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMisc::_vorbis_block_ripcord( CVorbisCodec::vorbis_block * _pvbVb ) {
		// Reap the chain.
		CVorbisCodec::alloc_chain * pacReap = _pvbVb->reap;
		while ( pacReap ) {
			CVorbisCodec::alloc_chain * pacNext = pacReap->next;
			CMemLib::Free( pacReap->ptr );
			CStd::MemSet( pacReap, 0, sizeof( (*pacReap) ) );
			CMemLib::Free( pacReap );
			pacReap = pacNext;
		}
		// Consolidate storage.
		if ( _pvbVb->totaluse ) {
			_pvbVb->localstore = CMemLib::MReAlloc( _pvbVb->localstore, _pvbVb->totaluse+_pvbVb->localalloc );
			_pvbVb->localalloc += _pvbVb->totaluse;
			_pvbVb->totaluse = 0;
		}

		// Pull the ripcord.
		_pvbVb->localtop = 0;
		_pvbVb->reap = NULL;
	}

}	// namespace lss
