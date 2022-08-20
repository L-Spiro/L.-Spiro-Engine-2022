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

 function: ::floor backend 0 implementation
 last mod: $Id: floor0.c 17558 2010-10-22 00:24:41Z tterribe $

 ********************************************************************/

#include "LSSVorbisFloor0.h"
#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisCodeBook.h"
#include "LSSVorbisLsp.h"
#include "LSSVorbisMisc.h"


namespace lss {

	// == Members.
	/**
	 * Undocumented Vorbis library data.
	 */
	const CVorbisBackends::vorbis_func_floor CVorbisFloor0::floor0_exportbundle = {
		NULL,							// pack
		&floor0_unpack,					// unpack
		&floor0_look,					// pvlfLook
		&floor0_free_info,				// free_info
		&floor0_free_look,				// free_look
		&floor0_inverse1,				// inverse1
		&floor0_inverse2				// inverse2
	};

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFloor0::floor0_free_info( CVorbisStructs::vorbis_info_floor * _pvifI ) {
		CVorbisBackends::vorbis_info_floor0 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor0 *>(_pvifI);
		if ( pvifInfo ) {
			CStd::MemSet( pvifInfo, 0, sizeof( (*pvifInfo) ) );
			CMemLib::Free( pvifInfo );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFloor0::floor0_free_look( CVorbisStructs::vorbis_look_floor * _pvifI ) {
		vorbis_look_floor0 * pvlfLook = static_cast<vorbis_look_floor0 *>(_pvifI);
		if ( pvlfLook ) {

			if ( pvlfLook->linearmap ) {

				if ( pvlfLook->linearmap[0] ) { CMemLib::Free( pvlfLook->linearmap[0] ); }
				if ( pvlfLook->linearmap[1] ) { CMemLib::Free( pvlfLook->linearmap[1] ); }

				CMemLib::Free( pvlfLook->linearmap );
			}
			CStd::MemSet( pvlfLook, 0, sizeof( (*pvlfLook) ) );
			CMemLib::Free( pvlfLook );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_info_floor * CVorbisFloor0::floor0_unpack( CVorbisCodec::vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisCodecInternal::codec_setup_info * pvsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		int32_t J;

		CVorbisBackends::vorbis_info_floor0 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor0 *>(CMemLib::MAlloc( sizeof( *pvifInfo ) ));
		pvifInfo->order=COgg::oggpack_read( _pobOpb, 8 );
		pvifInfo->rate=COgg::oggpack_read( _pobOpb, 16 );
		pvifInfo->barkmap=COgg::oggpack_read( _pobOpb, 16 );
		pvifInfo->ampbits=COgg::oggpack_read( _pobOpb, 6 );
		pvifInfo->ampdB=COgg::oggpack_read( _pobOpb, 8 );
		pvifInfo->numbooks=COgg::oggpack_read( _pobOpb, 4 )+1;

		if ( pvifInfo->order < 1 ) { goto ErrOut; }
		if ( pvifInfo->rate < 1 ) { goto ErrOut; }
		if ( pvifInfo->barkmap < 1 ) { goto ErrOut; }
		if ( pvifInfo->numbooks < 1 ) { goto ErrOut; }

		for( J = 0; J < pvifInfo->numbooks; J++ ) {
			pvifInfo->books[J]=COgg::oggpack_read( _pobOpb, 8 );
			if ( pvifInfo->books[J] < 0 || pvifInfo->books[J] >= pvsiCi->books ) { goto ErrOut; }
			if ( pvsiCi->book_param[pvifInfo->books[J]]->maptype == 0 ) { goto ErrOut; }
			if ( pvsiCi->book_param[pvifInfo->books[J]]->dim < 1 ) { goto ErrOut; }
		}
		return pvifInfo;

	ErrOut :
		floor0_free_info( pvifInfo );
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvifInfoX Undocumented parameter from the Vorbis library.
	 * \param _pvlfLook Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	void CVorbisFloor0::floor0_map_lazy_init( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_info_floor * _pvifInfoX, vorbis_look_floor0 * _pvlfLook ) {
		if ( !_pvlfLook->linearmap[_pvbVb->W] ) {
			CVorbisCodec::vorbis_dsp_state * pvdsVd =_pvbVb->vd;
			CVorbisCodec::vorbis_info * pviVi = pvdsVd->vi;
			CVorbisCodecInternal::codec_setup_info * pvsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
			CVorbisBackends::vorbis_info_floor0 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor0 *>(_pvifInfoX);
			int32_t i32W = _pvbVb->W;
			int32_t i32N = pvsiCi->blocksizes[i32W] / 2, J;

			// We choose a scaling constant so that:
			//	floor( bark( rate/2-1 )*C )=mapped-1.
			//	floor( bark( rate/2 )*C )=mapped.
			float fScale = static_cast<float>(_pvlfLook->ln / toBARK( pvifInfo->rate / 2.0f ));

			// The mapping from a linear fScale to a smaller bark fScale is
			//	straightforward.  We do not make sure that the linear mapping
			//	does not skip bark-fScale bins; the decoder simply skips them and
			//	the encoder may do what it wishes in filling them.  They're
			//	necessary in some mapping combinations to keep the fScale spacing
			//	accurate.
			_pvlfLook->linearmap[i32W] = static_cast<int32_t *>(CMemLib::MAlloc( (i32N + 1) * sizeof( (*(*_pvlfLook->linearmap)) ) ));
			for( J = 0; J < i32N; J++ ) {
				int32_t i32Val = static_cast<int32_t>(::floor( toBARK( (pvifInfo->rate / 2.0f) / i32N * J ) * fScale ));		// Bark numbers represent band edges.
				if ( i32Val >= _pvlfLook->ln ) { i32Val = _pvlfLook->ln - 1; }		// Guard against the approximation.
				_pvlfLook->linearmap[i32W][J] = i32Val;
			}
			_pvlfLook->linearmap[i32W][J] = -1;
			_pvlfLook->n[i32W] = i32N;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	CVorbisStructs::vorbis_look_floor * CVorbisFloor0::floor0_look( CVorbisCodec::vorbis_dsp_state * /*_pvdsVd*/, CVorbisStructs::vorbis_info_floor * _pvifI ) {
		CVorbisBackends::vorbis_info_floor0 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor0 *>(_pvifI);
		vorbis_look_floor0 * pvlfLook = static_cast<vorbis_look_floor0 *>(CMemLib::CMAlloc( sizeof( (*pvlfLook) ) ));
		pvlfLook->m = pvifInfo->order;
		pvlfLook->ln = pvifInfo->barkmap;
		pvlfLook->vi = pvifInfo;

		pvlfLook->linearmap = static_cast<int32_t **>(CMemLib::CMAlloc( 2 * sizeof( (*pvlfLook->linearmap) ) ));

		return pvlfLook;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvifI Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	void * CVorbisFloor0::floor0_inverse1( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvifI ) {
		vorbis_look_floor0 * pvlfLook = static_cast<vorbis_look_floor0 *>(_pvifI);
		CVorbisBackends::vorbis_info_floor0 * pvifInfo = static_cast<CVorbisBackends::vorbis_info_floor0 *>(pvlfLook->vi);
		int32_t J, K;

		int32_t i32AmpRaw=COgg::oggpack_read( &_pvbVb->opb, pvifInfo->ampbits );
		if ( i32AmpRaw>0 ) {	// Also handles the -1 out of data case.
			int32_t i32MaxVal=( 1<<pvifInfo->ampbits )-1;
			float fAmp = static_cast<float>(i32AmpRaw) / i32MaxVal * pvifInfo->ampdB;	// TODO: Verify the cast change.
			int32_t i32BookNum = COgg::oggpack_read( &_pvbVb->opb, CVorbisCodeBook::_ilog( pvifInfo->numbooks ) );

			if ( i32BookNum!=-1 && i32BookNum<pvifInfo->numbooks ) {	// Be paranoid.
				CVorbisCodecInternal::codec_setup_info * pvsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvbVb->vd->vi->codec_setup);
				CVorbisCodeBook::codebook * pcbBook = pvsiCi->fullbooks + pvifInfo->books[i32BookNum];
				float fLast = 0.0f;

				// The additional pcbBook->dim is a guard against any possible stack
				//	smash; pcbBook->dim is provably more than we can overflow the
				//	vector.
				float * pfLsp = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*pfLsp) ) * (pvlfLook->m + pcbBook->dim + 1) ));

				for( J = 0; J < pvlfLook->m; J += pcbBook->dim ) {
					if ( CVorbisCodeBook::vorbis_book_decodev_set( pcbBook, pfLsp + J, &_pvbVb->opb, pcbBook->dim ) == -1 ) { goto Eop; }
				}
				for( J = 0; J < pvlfLook->m; ) {
					for( K = 0; K < pcbBook->dim; K++, J++ ) { pfLsp[J] += fLast; }
					fLast = pfLsp[J-1];
				}

				pfLsp[pvlfLook->m] = fAmp;
				return pfLsp;
			}
		}
	Eop :
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _pvlfI Undocumented parameter from the Vorbis library.
	 * \param _pvMemo Undocumented parameter from the Vorbis library.
	 * \param _pfOut Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFloor0::floor0_inverse2( CVorbisCodec::vorbis_block * _pvbVb, CVorbisStructs::vorbis_look_floor * _pvlfI, void * _pvMemo, float * _pfOut ) {
		vorbis_look_floor0 * pvlfLook = static_cast<vorbis_look_floor0 *>(_pvlfI);
		CVorbisBackends::vorbis_info_floor0 * pvifInfo = pvlfLook->vi;

		floor0_map_lazy_init( _pvbVb, pvifInfo, pvlfLook );

		if ( _pvMemo ) {
			float * pfLsp = static_cast<float *>(_pvMemo);
			float fAmp = pfLsp[pvlfLook->m];

			// Take the coefficients back to a spectral envelope curve.
			CVorbisLsp::vorbis_lsp_to_curve( _pfOut, 
				pvlfLook->linearmap[_pvbVb->W], 
				pvlfLook->n[_pvbVb->W], 
				pvlfLook->ln, 
				pfLsp, pvlfLook->m, fAmp, static_cast<float>(pvifInfo->ampdB) );
			return 1;
		}
		CStd::MemSet( _pfOut, 0, sizeof( (*_pfOut) ) * pvlfLook->n[_pvbVb->W] );
		return 0;
	}

}	// namespace lss
