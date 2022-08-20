/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *

 ********************************************************************

 function: libvorbis codec headers
 last mod: $Id: codec.h 17021 2010-03-24 09:29:41Z xiphmont $

 ********************************************************************/

#include "LSSVorbisCodec.h"
#include "LSSOggWindow.h"
#include "LSSVorbisCodecInternal.h"
#include "LSSVorbisLpc.h"
#include "LSSVorbisMisc.h"
#include "LSSVorbisRegistry.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_info_init( vorbis_info * _pviVi ) {
		CStd::MemSet( _pviVi, 0, sizeof( (*_pviVi) ) );
		_pviVi->codec_setup = CMemLib::CMAlloc( sizeof( CVorbisCodecInternal::codec_setup_info ) );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_info_clear( vorbis_info * _pviVi ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		int32_t I;

		if ( pcsiCi ) {

			for ( I = 0; I < pcsiCi->modes; I++ ) {
				if ( pcsiCi->mode_param[I] ) { CMemLib::Free( pcsiCi->mode_param[I] ); }
			}

			for ( I = 0; I < pcsiCi->maps; I++ ) {	// Unpack does the range checking.
				if ( pcsiCi->map_param[I] ) {		// This may be cleaning up an aborted
													//	unpack, in which case the below i32Type
													//	cannot be trusted.
					CVorbisRegistry::_mapping_P[pcsiCi->map_type[I]]->free_info( pcsiCi->map_param[I] );
				}
			}

			for ( I = 0; I < pcsiCi->floors; I++ ) {	// Unpack does the range checking.
				if ( pcsiCi->floor_param[I] ) {			// This may be cleaning up an aborted
														//	unpack, in which case the below i32Type
														//	cannot be trusted.
					CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->free_info( pcsiCi->floor_param[I] );
				}
			}

			for ( I = 0; I < pcsiCi->residues; I++ ) {		// Unpack does the range checking.
				if ( pcsiCi->residue_param[I] ) {			// This may be cleaning up an aborted
															//	unpack, in which case the below i32Type
															//	cannot be trusted.
					CVorbisRegistry::_residue_P[pcsiCi->residue_type[I]]->free_info( pcsiCi->residue_param[I] );
				}
			}

			for ( I = 0; I < pcsiCi->books; I++ ) {
				if ( pcsiCi->book_param[I] ) {
					// Knows if the book was not allocated.
					CVorbisCodeBook::vorbis_staticbook_destroy( pcsiCi->book_param[I] );
				}
				if ( pcsiCi->fullbooks ) {
					CVorbisCodeBook::vorbis_book_clear( pcsiCi->fullbooks + I );
				}
			}
			if ( pcsiCi->fullbooks ) {
					CMemLib::Free( pcsiCi->fullbooks );
			}

			for ( I = 0; I < pcsiCi->psys; I++ ) {
				CVorbisPsy::_vi_psy_free( pcsiCi->psy_param[I] );
			}

			CMemLib::Free( pcsiCi );
		}

		CStd::MemSet( _pviVi, 0, sizeof( (*_pviVi) ) );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _i32Zo Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_info_blocksize( vorbis_info * _pviVi, int32_t _i32Zo ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		return pcsiCi ? pcsiCi->blocksizes[_i32Zo] : -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_comment_init( vorbis_comment * _pvcVc ) {
		CStd::MemSet( _pvcVc, 0, sizeof( (*_pvcVc) ) );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _pcComment Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_comment_add( vorbis_comment * _pvcVc, const char * _pcComment ) {
		_pvcVc->user_comments = static_cast<char **>(CMemLib::MReAlloc( _pvcVc->user_comments, 
			(_pvcVc->comments + 2) * sizeof( (*_pvcVc->user_comments) ) ));
		_pvcVc->comment_lengths = static_cast<int32_t *>(CMemLib::MReAlloc( _pvcVc->comment_lengths, 
			(_pvcVc->comments + 2) * sizeof( (*_pvcVc->comment_lengths) ) ) );
		_pvcVc->comment_lengths[_pvcVc->comments] = CStd::StrLen( _pcComment );
		_pvcVc->user_comments[_pvcVc->comments] = static_cast<char *>(CMemLib::MAlloc( _pvcVc->comment_lengths[_pvcVc->comments] + 1 ));
		CStd::StrCpy( _pvcVc->user_comments[_pvcVc->comments], _pcComment );
		_pvcVc->comments++;
		_pvcVc->user_comments[_pvcVc->comments] = NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _pcTag Undocumented parameter from the Vorbis library.
	 * \param _pcContents Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_comment_add_tag( vorbis_comment * _pvcVc, 
		const char * _pcTag, const char * _pcContents ) {
		char * comment = static_cast<char *>(::alloca( CStd::StrLen( _pcTag )+CStd::StrLen( _pcContents )+2 ) );
		CStd::StrCpy( comment, _pcTag );
		CStd::StrCat( comment, "=" );
		CStd::StrCat( comment, _pcContents );
		vorbis_comment_add( _pvcVc, comment );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _pcTag Undocumented parameter from the Vorbis library.
	 * \param _i32Count Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	char * CVorbisCodec::vorbis_comment_query( vorbis_comment * _pvcVc, const char * _pcTag, int32_t _i32Count ) {
		int32_t I;
		int32_t i32Found = 0;
		int32_t i32TagLen = CStd::StrLen( _pcTag ) + 1;
		char * pcFullTag = static_cast<char *>(::alloca( i32TagLen + 1 ));

		CStd::StrCpy( pcFullTag, _pcTag );
		CStd::StrCat( pcFullTag, "=" );

		for ( I = 0; I < _pvcVc->comments; I++ ) {
			if ( !tagcompare( _pvcVc->user_comments[I], pcFullTag, i32TagLen ) ) {
				if ( _i32Count == i32Found ) {
					// We return a pointer to the data, not a copy.
					return _pvcVc->user_comments[I] + i32TagLen;
				}
				else {
					i32Found++;
				}
			}
		}
		return NULL;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _pcTag Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_comment_query_count( vorbis_comment * _pvcVc, const char * _pcTag ) {
		int32_t I, i32Count = 0;
		int32_t i32TagLen = CStd::StrLen( _pcTag ) + 1;
		char * pcFullTag = static_cast<char *>(::alloca( i32TagLen + 1 ));
		CStd::StrCpy( pcFullTag, _pcTag );
		CStd::StrCat( pcFullTag, "=" );

		for ( I = 0; I < _pvcVc->comments; I++ ) {
			if ( !tagcompare( _pvcVc->user_comments[I], pcFullTag, i32TagLen ) ) {
				i32Count++;
			}
		}

		return i32Count;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_comment_clear( vorbis_comment * _pvcVc ) {
		if ( _pvcVc ) {
			int32_t I;
			if ( _pvcVc->user_comments ) {
				for ( I = 0; I < _pvcVc->comments; I++ ) {
					if ( _pvcVc->user_comments[I] ) { CMemLib::Free( _pvcVc->user_comments[I] ); }
				}
				CMemLib::Free( _pvcVc->user_comments );
			}
			if ( _pvcVc->comment_lengths ) { CMemLib::Free( _pvcVc->comment_lengths ); }
			if ( _pvcVc->vendor ) { CMemLib::Free( _pvcVc->vendor ); }
			CStd::MemSet( _pvcVc, 0, sizeof( (*_pvcVc) ) );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_block_init( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb ) {
		int32_t I;
		CStd::MemSet( _pvbVb, 0, sizeof( (*_pvbVb) ) );
		_pvbVb->vd = _pvdsV;
		_pvbVb->localalloc = 0;
		_pvbVb->localstore = NULL;
		if ( _pvdsV->analysisp ) {
			CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = 
				static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal = 
				CMemLib::CMAlloc( sizeof( CVorbisCodecInternal::vorbis_block_internal ) ));
			pvbiVbi->ampmax = -9999.0f;

			for ( I = 0; I < PACKETBLOBS; I++ ) {
				if ( I == PACKETBLOBS / 2 ) {
					pvbiVbi->packetblob[I] = &_pvbVb->opb;
				}
				else {
					pvbiVbi->packetblob[I] = static_cast<COgg::oggpack_buffer *>(CMemLib::CMAlloc( sizeof( COgg::oggpack_buffer ) ));
				}
				COgg::oggpack_writeinit( pvbiVbi->packetblob[I] );
			}
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_block_clear( vorbis_block * _pvbVb ) {
		int32_t I;
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal);

		CVorbisMisc::_vorbis_block_ripcord( _pvbVb );
		if ( _pvbVb->localstore ) { CMemLib::Free( _pvbVb->localstore ); }

		if ( pvbiVbi ) {
			for ( I = 0; I < PACKETBLOBS; I++ ) {
				COgg::oggpack_writeclear( pvbiVbi->packetblob[I] );
				if ( I != PACKETBLOBS / 2 ) { CMemLib::Free( pvbiVbi->packetblob[I] ); }
			}
			CMemLib::Free( pvbiVbi );
		}
		CStd::MemSet( _pvbVb, 0, sizeof( (*_pvbVb) ) );
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::vorbis_dsp_clear( vorbis_dsp_state * _pvdsV ) {
		int32_t I;
		if ( _pvdsV ) {
			vorbis_info * pviVi = _pvdsV->vi;
			CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi ? pviVi->codec_setup : NULL);
			CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);

			if ( ppsBackend ) {

				if ( ppsBackend->ve ) {
					CVorbisEnvelope::_ve_envelope_clear( ppsBackend->ve );
					CMemLib::Free( ppsBackend->ve );
				}

				if ( ppsBackend->transform[0] ) {
					CVorbisMdct::mdct_clear( static_cast<CVorbisMdct::mdct_lookup *>(ppsBackend->transform[0][0]) );
					CMemLib::Free( ppsBackend->transform[0][0] );
					CMemLib::Free( ppsBackend->transform[0] );
				}
				if ( ppsBackend->transform[1] ) {
					CVorbisMdct::mdct_clear( static_cast<CVorbisMdct::mdct_lookup *>(ppsBackend->transform[1][0]) );
					CMemLib::Free( ppsBackend->transform[1][0] );
					CMemLib::Free( ppsBackend->transform[1] );
				}

				if ( ppsBackend->flr ) {
					if ( pcsiCi ) {
						for ( I = 0; I < pcsiCi->floors; I++ ) {
							CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->free_look( ppsBackend->flr[I] );
						}
					}
					CMemLib::Free( ppsBackend->flr );
				}
				if ( ppsBackend->residue ) {
					if ( pcsiCi ) {
						for ( I = 0; I < pcsiCi->residues; I++ ) {
							CVorbisRegistry::_residue_P[pcsiCi->residue_type[I]]->free_look( ppsBackend->residue[I] );
						}
					}
					CMemLib::Free( ppsBackend->residue );
				}
				if ( ppsBackend->psy ) {
					if ( pcsiCi ) {
						for ( I = 0; I < pcsiCi->psys; I++ ) {
							CVorbisPsy::_vp_psy_clear( ppsBackend->psy + I );
						}
					}
					CMemLib::Free( ppsBackend->psy );
				}

				if ( ppsBackend->psy_g_look ) { CVorbisCodecInternal::_vp_global_free( ppsBackend->psy_g_look ); }
				CVorbisBitrate::vorbis_bitrate_clear( &ppsBackend->bms );

				CVorbisSmallFt::drft_clear( &ppsBackend->fft_look[0] );
				CVorbisSmallFt::drft_clear( &ppsBackend->fft_look[1] );

			}

			if ( _pvdsV->pcm ) {
				if ( pviVi ) {
					for ( I = 0; I < pviVi->channels; I++ ) {
						if ( _pvdsV->pcm[I] ) { CMemLib::Free( _pvdsV->pcm[I] ); }
					}
				}
				CMemLib::Free( _pvdsV->pcm );
				if ( _pvdsV->pcmret ) { CMemLib::Free( _pvdsV->pcmret ); }
			}

			if ( ppsBackend ) {
				// Free header, header1, header2.
				if ( ppsBackend->header ) { CMemLib::Free( ppsBackend->header ); }
				if ( ppsBackend->header1 ) { CMemLib::Free( ppsBackend->header1 ); }
				if ( ppsBackend->header2 ) { CMemLib::Free( ppsBackend->header2 ); }
				CMemLib::Free( ppsBackend );
			}

			CStd::MemSet( _pvdsV, 0, sizeof( (*_pvdsV) ) );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i64GranulePos Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	double CVorbisCodec::vorbis_granule_time( vorbis_dsp_state * _pvdsV, 
		int64_t _i64GranulePos ) {
		if ( _i64GranulePos == -1 ) { return -1; }

		/* We're not guaranteed a 64 bit unsigned i32Type everywhere, so we
			 have to put the unsigned granpo in a signed i32Type. */
		if ( _i64GranulePos >= 0 ) {
			return static_cast<double>(_i64GranulePos ) / _pvdsV->vi->rate;
		}
		else {
			int64_t i64GranuleOff = 0xFFFFFFFF;
			i64GranuleOff <<= 31;
			i64GranuleOff |= 0x7FFFFFFFF;
			return (static_cast<double>(_i64GranulePos) + 2 + i64GranuleOff + i64GranuleOff) / _pvdsV->vi->rate;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \return Undocumented return from the Vorbis library.
	 */
	const char * CVorbisCodec::vorbis_version_string() {
		return GENERAL_VENDOR_STRING;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_analysis_init( vorbis_dsp_state * _pvdsV, vorbis_info * _pviVi ) {
		CVorbisCodecInternal::private_state * ppsBackend = NULL;

		if ( _vds_shared_init( _pvdsV, _pviVi, 1 ) ) { return 1; }
		ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);
		ppsBackend->psy_g_look = CVorbisCodecInternal::_vp_global_look( _pviVi );

		// Initialize the envelope state storage.
		ppsBackend->ve = static_cast<CVorbisEnvelope::envelope_lookup *>(CMemLib::CMAlloc( sizeof( (*ppsBackend->ve) ) ));
		CVorbisEnvelope::_ve_envelope_init( ppsBackend->ve, _pviVi );

		CVorbisBitrate::vorbis_bitrate_init( _pviVi, &ppsBackend->bms );

		// Compressed audio packets start after the headers with sequence number 3.
		_pvdsV->sequence = 3;
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_commentheader_out( vorbis_comment * _pvcVc, COgg::ogg_packet * _popOp ) {
		COgg::oggpack_buffer _pobOpb;

		COgg::oggpack_writeinit( &_pobOpb );
		if ( _vorbis_pack_comment( &_pobOpb, _pvcVc ) ) { return OV_EIMPL; }

		_popOp->packet = static_cast<uint8_t *>(CMemLib::MAlloc( COgg::oggpack_bytes( &_pobOpb ) ));
		CStd::MemCpy( _popOp->packet, _pobOpb.buffer, COgg::oggpack_bytes( &_pobOpb ) );

		_popOp->bytes = COgg::oggpack_bytes( &_pobOpb );
		_popOp->b_o_s = 0;
		_popOp->e_o_s = 0;
		_popOp->granulepos = 0;
		_popOp->packetno = 1;

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \param _popComm Undocumented parameter from the Vorbis library.
	 * \param _popCode Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_analysis_headerout( vorbis_dsp_state * _pvdsV, 
		vorbis_comment * _pvcVc, 
		COgg::ogg_packet * _popOp, 
		COgg::ogg_packet * _popComm, 
		COgg::ogg_packet * _popCode ) {
		int32_t i32Ret = OV_EIMPL;
		vorbis_info * pviVi = _pvdsV->vi;
		COgg::oggpack_buffer obBuffer;
		CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);

		if ( !ppsBackend ) {
			i32Ret = OV_EFAULT;
			goto ErrOut;
		}

		// First header packet.

		COgg::oggpack_writeinit( &obBuffer );
		if ( _vorbis_pack_info( &obBuffer, pviVi ) ) { goto ErrOut; }

		// Build the packet.
		if ( ppsBackend->header ) { CMemLib::Free( ppsBackend->header ); }
		ppsBackend->header = static_cast<uint8_t *>(CMemLib::MAlloc( COgg::oggpack_bytes( &obBuffer ) ));
		CStd::MemCpy( ppsBackend->header, obBuffer.buffer, COgg::oggpack_bytes( &obBuffer ) );
		_popOp->packet = ppsBackend->header;
		_popOp->bytes = COgg::oggpack_bytes( &obBuffer );
		_popOp->b_o_s = 1;
		_popOp->e_o_s = 0;
		_popOp->granulepos = 0;
		_popOp->packetno = 0;

		// Second header packet (comments).

		COgg::oggpack_reset( &obBuffer );
		if ( _vorbis_pack_comment( &obBuffer, _pvcVc ) ) { goto ErrOut; }

		if ( ppsBackend->header1 ) { CMemLib::Free( ppsBackend->header1 ); }
		ppsBackend->header1 = static_cast<uint8_t *>(CMemLib::MAlloc( COgg::oggpack_bytes( &obBuffer ) ));
		CStd::MemCpy( ppsBackend->header1, obBuffer.buffer, COgg::oggpack_bytes( &obBuffer ) );
		_popComm->packet = ppsBackend->header1;
		_popComm->bytes = COgg::oggpack_bytes( &obBuffer );
		_popComm->b_o_s = 0;
		_popComm->e_o_s = 0;
		_popComm->granulepos = 0;
		_popComm->packetno = 1;

		// Third header packet (modes/codebooks).

		COgg::oggpack_reset( &obBuffer );
		if ( _vorbis_pack_books( &obBuffer, pviVi ) ) { goto ErrOut; }

		if ( ppsBackend->header2 ) { CMemLib::Free( ppsBackend->header2 ); }
		ppsBackend->header2 = static_cast<uint8_t *>(CMemLib::MAlloc( COgg::oggpack_bytes( &obBuffer ) ));
		CStd::MemCpy( ppsBackend->header2, obBuffer.buffer, COgg::oggpack_bytes( &obBuffer ) );
		_popCode->packet = ppsBackend->header2;
		_popCode->bytes = COgg::oggpack_bytes( &obBuffer );
		_popCode->b_o_s = 0;
		_popCode->e_o_s = 0;
		_popCode->granulepos = 0;
		_popCode->packetno = 2;

		COgg::oggpack_writeclear( &obBuffer );
		return 0;
	ErrOut :
		CStd::MemSet( _popOp, 0, sizeof( (*_popOp) ) );
		CStd::MemSet( _popComm, 0, sizeof( (*_popComm) ) );
		CStd::MemSet( _popCode, 0, sizeof( (*_popCode) ) );

		if ( ppsBackend ) {
			COgg::oggpack_writeclear( &obBuffer );
			if ( ppsBackend->header ) { CMemLib::Free( ppsBackend->header ); }
			if ( ppsBackend->header1 ) { CMemLib::Free( ppsBackend->header1 ); }
			if ( ppsBackend->header2 ) { CMemLib::Free( ppsBackend->header2 ); }
			ppsBackend->header = NULL;
			ppsBackend->header1 = NULL;
			ppsBackend->header2 = NULL;
		}
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i32Vals Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float ** CVorbisCodec::vorbis_analysis_buffer( vorbis_dsp_state * _pvdsV, int32_t _i32Vals ) {
		int32_t I;
		vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);

		// Free header, header1, and header2.
		if ( ppsBackend->header ) { CMemLib::Free( ppsBackend->header ); ppsBackend->header = NULL; }
		if ( ppsBackend->header1 ) { CMemLib::Free( ppsBackend->header1 ); ppsBackend->header1 = NULL; }
		if ( ppsBackend->header2 ) { CMemLib::Free( ppsBackend->header2 ); ppsBackend->header2 = NULL; }

		// Do we have enough storage space for the requested buffer?  If not, expand the PCM (and envelope) storage.
		if ( _pvdsV->pcm_current + _i32Vals >= _pvdsV->pcm_storage ) {
			_pvdsV->pcm_storage = _pvdsV->pcm_current + _i32Vals * 2;

			for ( I = 0; I < pviVi->channels; I++ ) {
				_pvdsV->pcm[I] = static_cast<float *>(CMemLib::MReAlloc( _pvdsV->pcm[I], _pvdsV->pcm_storage*sizeof( *_pvdsV->pcm[I] ) ));
			}
		}

		for ( I = 0; I < pviVi->channels; I++ ) {
			_pvdsV->pcmret[I] = _pvdsV->pcm[I]+_pvdsV->pcm_current;
		}

		return _pvdsV->pcmret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i32Vals Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_analysis_wrote( vorbis_dsp_state * _pvdsV, int32_t _i32Vals ) {
		vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);

		if ( _i32Vals <= 0 ) {
			int32_t i32Order = 32;
			int32_t I;
			float * fLpc = static_cast<float *>(::alloca( i32Order * sizeof( (*fLpc) ) ));

			// If it wasn't done earlier (very short sample).
			if ( !_pvdsV->preextrapolate ) {
				_preextrapolate_helper( _pvdsV );
			}

			// We're encoding the end of the stream.  Just make sure we have (at least) a few full blocks of zeroes at the end.
			// Actually, we don't want zeroes; that could drop a large amplitude off a cliff, creating spread spectrum noise that will
			//	suck to encode.  Extrapolate for the sake of cleanliness.

			vorbis_analysis_buffer( _pvdsV, pcsiCi->blocksizes[1] * 3 );
			_pvdsV->eofflag = _pvdsV->pcm_current;
			_pvdsV->pcm_current += pcsiCi->blocksizes[1] * 3;

			for ( I = 0; I < pviVi->channels; I++ ) {
				if ( _pvdsV->eofflag>i32Order*2 ) {
					// Extrapolate with LPC to fill in.
					int32_t i32N;

					// Make a predictor filter.
					i32N = _pvdsV->eofflag;
					if ( i32N>pcsiCi->blocksizes[1] ) { i32N = pcsiCi->blocksizes[1]; }
					CVorbisLpc::vorbis_lpc_from_data( _pvdsV->pcm[I]+_pvdsV->eofflag-i32N, fLpc, i32N, i32Order );

					// Run the predictor filter.
					CVorbisLpc::vorbis_lpc_predict( fLpc, _pvdsV->pcm[I]+_pvdsV->eofflag-i32Order, i32Order, 
														 _pvdsV->pcm[I]+_pvdsV->eofflag, _pvdsV->pcm_current-_pvdsV->eofflag );
				}
				else {
					// Not enough data to extrapolate (unlikely to happen due to guarding the overlap, but bulletproof in case that
					//	assumtion goes away).  Zeroes will do.
					CStd::MemSet( _pvdsV->pcm[I] + _pvdsV->eofflag, 0, (_pvdsV->pcm_current - _pvdsV->eofflag) * sizeof( (*_pvdsV->pcm[I]) ));
				}
			}
		}
		else {

			if ( _pvdsV->pcm_current + _i32Vals > _pvdsV->pcm_storage ) { return OV_EINVAL; }

			_pvdsV->pcm_current += _i32Vals;

			// We may want to reverse extrapolate the beginning of a stream too. In case we're beginning on a cliff!
			// Clumsy, but simple.  It only runs once, so simple is good.
			if ( !_pvdsV->preextrapolate && _pvdsV->pcm_current-_pvdsV->centerW>pcsiCi->blocksizes[1] ) {
				_preextrapolate_helper( _pvdsV );
			}

		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_analysis_blockout( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb ) {
		int32_t I;
		CVorbisCodec::vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);
		CVorbisPsy::vorbis_look_psy_global * pvlpgG = ppsBackend->psy_g_look;
		int32_t i32BeginW = _pvdsV->centerW - pcsiCi->blocksizes[_pvdsV->W] / 2, i32CenterNext;
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal);

		// Check to see if we're started.
		if ( !_pvdsV->preextrapolate ) { return 0; }

		// Check to see if we're done.
		if ( _pvdsV->eofflag == -1 ) { return 0; }

		// By our invariant, we have lW, W and centerW set.  Search for the next boundary so we can determine nW (the next window size)
		//	which lets us compute the shape of the current block's window.

		// We do an envelope search even on a single blocksize; we may still be throwing more bits at impulses, and envelope search handles
		//	marking impulses too.
		{
			int32_t i32Bp = CVorbisEnvelope::_ve_envelope_search( _pvdsV );
			if ( i32Bp == -1 ) {
				if ( _pvdsV->eofflag == 0 ) { return 0; }
				_pvdsV->nW = 0;
			}
			else {

				if ( pcsiCi->blocksizes[0] == pcsiCi->blocksizes[1] ) {
					_pvdsV->nW = 0;
				}
				else {
					_pvdsV->nW = i32Bp;
				}
			}
		}

		i32CenterNext = _pvdsV->centerW + pcsiCi->blocksizes[_pvdsV->W] / 4 + pcsiCi->blocksizes[_pvdsV->nW] / 4;

		{
			int32_t i32BlockBound = i32CenterNext + pcsiCi->blocksizes[_pvdsV->nW] / 2;
			if ( _pvdsV->pcm_current < i32BlockBound ) { return 0; }

		}

		// Fill in the block.  Note that for a short window, lW and nW are short regardless of actual settings in the stream.

		CVorbisMisc::_vorbis_block_ripcord( _pvbVb );
		_pvbVb->lW = _pvdsV->lW;
		_pvbVb->W = _pvdsV->W;
		_pvbVb->nW = _pvdsV->nW;

		if ( _pvdsV->W ) {
			if ( !_pvdsV->lW || !_pvdsV->nW ) {
				pvbiVbi->blocktype = BLOCKTYPE_TRANSITION;
			}
			else {
				pvbiVbi->blocktype = BLOCKTYPE_LONG;
			}
		}
		else {
			if ( CVorbisEnvelope::_ve_envelope_mark( _pvdsV ) ) {
				pvbiVbi->blocktype = BLOCKTYPE_IMPULSE;

			}
			else {
				pvbiVbi->blocktype = BLOCKTYPE_PADDING;

			}
		}

		_pvbVb->vd = _pvdsV;
		_pvbVb->sequence = _pvdsV->sequence++;
		_pvbVb->granulepos = _pvdsV->granulepos;
		_pvbVb->pcmend = pcsiCi->blocksizes[_pvdsV->W];

		// Copy the vectors; this uses the local storage in _pvbVb.

		// This tracks 'strongest peak' for later psychoacoustics.
		// Moved to the global psy state; clean this mess up.
		if ( pvbiVbi->ampmax>pvlpgG->ampmax ) { pvlpgG->ampmax = pvbiVbi->ampmax; }
		pvlpgG->ampmax = CVorbisPsy::_vp_ampmax_decay( pvlpgG->ampmax, _pvdsV );
		pvbiVbi->ampmax = pvlpgG->ampmax;

		_pvbVb->pcm = static_cast<float **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*_pvbVb->pcm) ) * pviVi->channels ));
		pvbiVbi->pcmdelay = static_cast<float **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*pvbiVbi->pcmdelay) ) * pviVi->channels ));
		for ( I = 0; I < pviVi->channels; I++ ) {
			pvbiVbi->pcmdelay[I] = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, (_pvbVb->pcmend + i32BeginW) * sizeof( (*pvbiVbi->pcmdelay[I]) ) ));
			CStd::MemCpy( pvbiVbi->pcmdelay[I], _pvdsV->pcm[I], (_pvbVb->pcmend + i32BeginW) * sizeof( (*pvbiVbi->pcmdelay[I]) ));
			_pvbVb->pcm[I] = pvbiVbi->pcmdelay[I] + i32BeginW;


		}

		if ( _pvdsV->eofflag ) {
			if ( _pvdsV->centerW >= _pvdsV->eofflag ) {
				_pvdsV->eofflag = -1;
				_pvbVb->eofflag = 1;
				return 1;
			}
		}

		// Advance storage vectors and clean up.
		{
			int32_t i32NewCenterNext = pcsiCi->blocksizes[1] / 2;
			int32_t i32MovementW = i32CenterNext-i32NewCenterNext;

			if ( i32MovementW > 0 ) {

				CVorbisEnvelope::_ve_envelope_shift( ppsBackend->ve, i32MovementW );
				_pvdsV->pcm_current -= i32MovementW;

				for ( I = 0; I < pviVi->channels; I++ ) {
					CStd::MemMove( _pvdsV->pcm[I], _pvdsV->pcm[I] + i32MovementW, _pvdsV->pcm_current * sizeof( (*_pvdsV->pcm[I]) ) );
				}


				_pvdsV->lW = _pvdsV->W;
				_pvdsV->W = _pvdsV->nW;
				_pvdsV->centerW = i32NewCenterNext;

				if ( _pvdsV->eofflag ) {
					_pvdsV->eofflag -= i32MovementW;
					if ( _pvdsV->eofflag <= 0 ) { _pvdsV->eofflag = -1; }
					// Do not add padding to end of stream!
					if ( _pvdsV->centerW >= _pvdsV->eofflag ) {
						_pvdsV->granulepos += i32MovementW - (_pvdsV->centerW - _pvdsV->eofflag);
					}
					else {
						_pvdsV->granulepos += i32MovementW;
					}
				}
				else {
					_pvdsV->granulepos += i32MovementW;
				}
			}
		}

		return 1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_analysis( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp ) {
		int32_t i32Ret, I;
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal);

		_pvbVb->glue_bits = 0;
		_pvbVb->time_bits = 0;
		_pvbVb->floor_bits = 0;
		_pvbVb->res_bits = 0;

		// First things first.  Make sure encode is ready.
		for ( I = 0; I < PACKETBLOBS; I++ ) {
			COgg::oggpack_reset( pvbiVbi->packetblob[I] );
		}

		// We only have one mapping i32Type (0), and we let the mapping code
		//	itself figure out what soft i32Mode to use.  This allows easier
		//	bitrate management.

		i32Ret = CVorbisRegistry::_mapping_P[0]->forward( _pvbVb );
		if ( i32Ret ) {
			return i32Ret;
		}

		if ( _popOp ) {
			if ( CVorbisBitrate::vorbis_bitrate_managed( _pvbVb ) ) {
				// The app is using a bitmanaged i32Mode, but not using the bitrate management interface.
				return OV_EINVAL;
			}

			_popOp->packet = COgg::oggpack_get_buffer( &_pvbVb->opb );
			_popOp->bytes = COgg::oggpack_bytes( &_pvbVb->opb );
			_popOp->b_o_s = 0;
			_popOp->e_o_s = _pvbVb->eofflag;
			_popOp->granulepos = _pvbVb->granulepos;
			_popOp->packetno = _pvbVb->sequence;
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_bitrate_addblock( vorbis_block * _pvbVb ) {
		CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(_pvbVb->internal );
		vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd->backend_state );
		CVorbisBitrate::bitrate_manager_state * pbmsBm = &ppsBackend->bms;
		vorbis_info * pviVi = pvdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisBitrate::bitrate_manager_info * bi = &pcsiCi->bi;

		int32_t	i32Choice = static_cast<int32_t>(rint( pbmsBm->avgfloat ));
		int32_t i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
		int32_t i32MinTargetBits = (_pvbVb->W?pbmsBm->min_bitsper*pbmsBm->short_per_long:pbmsBm->min_bitsper);
		int32_t i32MaxTargetBits = (_pvbVb->W?pbmsBm->max_bitsper*pbmsBm->short_per_long:pbmsBm->max_bitsper);
		int32_t	i32Samples = pcsiCi->blocksizes[_pvbVb->W] >> 1;
		int32_t i32DesiredFill = static_cast<int32_t>(bi->reservoir_bits * bi->reservoir_bias );
		if ( !pbmsBm->managed ) {
			if ( pbmsBm->vb ) { return -1; }
			pbmsBm->vb = _pvbVb;
			return 0;
		}

		pbmsBm->vb = _pvbVb;

		if ( pbmsBm->avg_bitsper>0 ) {
			double dSlew = 0.0;
			int32_t i32AvgTargetBits = (_pvbVb->W ? pbmsBm->avg_bitsper * pbmsBm->short_per_long : pbmsBm->avg_bitsper);
			double dSlewLimit = 15.0 / bi->slew_damp;


			if ( pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) > i32DesiredFill ) {
				while ( i32Choice > 0 && i32ThisBits > i32AvgTargetBits &&
					pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) > i32DesiredFill ) {
					i32Choice--;
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
			else if ( pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) < i32DesiredFill ) {
				while ( i32Choice + 1 < PACKETBLOBS && i32ThisBits < i32AvgTargetBits &&
					pbmsBm->avg_reservoir + (i32ThisBits - i32AvgTargetBits) < i32DesiredFill ) {
					i32Choice++;
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}

			dSlew = rint( i32Choice-pbmsBm->avgfloat ) / i32Samples * pviVi->rate;
			if ( dSlew < -dSlewLimit ) { dSlew = -dSlewLimit; }
			if ( dSlew > dSlewLimit ) { dSlew = dSlewLimit; }
			i32Choice = static_cast<int32_t>(rint( pbmsBm->avgfloat += dSlew / pviVi->rate * i32Samples ));
			i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
		}


		if ( pbmsBm->min_bitsper > 0 ) {
			if ( i32ThisBits < i32MinTargetBits ) {
				while ( pbmsBm->minmax_reservoir - (i32MinTargetBits - i32ThisBits) < 0 ) {
					i32Choice++;
					if ( i32Choice >= PACKETBLOBS ) { break; }
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
		}

		if ( pbmsBm->max_bitsper > 0 ) {
			// Do we need to force the bitrate down?
			if ( i32ThisBits > i32MaxTargetBits ) {
				while ( pbmsBm->minmax_reservoir + (i32ThisBits - i32MaxTargetBits) > bi->reservoir_bits ) {
					i32Choice--;
					if ( i32Choice < 0 ) { break; }
					i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
				}
			}
		}

		// Choice of packetblobs now made based on floater, and min/max requirements. Now boundary check extreme choices.

		if ( i32Choice < 0 ) {
			// Choosing a smaller packetblob is insufficient to trim bitrate.  Frame will need to be truncated.
			int32_t i32MaxSize = (i32MaxTargetBits + (bi->reservoir_bits - pbmsBm->minmax_reservoir)) / 8;
			pbmsBm->choice = i32Choice = 0;

			if ( COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) > i32MaxSize ) {
				COgg::oggpack_writetrunc( pvbiVbi->packetblob[i32Choice], i32MaxSize * 8 );
				i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;
			}
		}
		else {
			int32_t i32MinSize = (i32MinTargetBits - pbmsBm->minmax_reservoir + 7) / 8;
			if ( i32Choice >= PACKETBLOBS ) {
				i32Choice = PACKETBLOBS - 1;
			}

			pbmsBm->choice = i32Choice;

			// Prop up bitrate according to demand.  Pad this frame out with zeroes.
			i32MinSize -= COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] );
			while ( i32MinSize-- > 0 ) { COgg::oggpack_write( pvbiVbi->packetblob[i32Choice], 0, 8 ); }
			i32ThisBits = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] ) * 8;

		}

		// Now we have the final packet and the final packet size.  Update statistics.
		// Min and max reservoir.
		if ( pbmsBm->min_bitsper > 0 || pbmsBm->max_bitsper > 0 ) {
			if ( i32MaxTargetBits>0 && i32ThisBits>i32MaxTargetBits ) {
				pbmsBm->minmax_reservoir += (i32ThisBits - i32MaxTargetBits);
			}
			else if ( i32MinTargetBits > 0 && i32ThisBits < i32MinTargetBits ) {
				pbmsBm->minmax_reservoir += (i32ThisBits - i32MinTargetBits);
			}
			else {
				if ( pbmsBm->minmax_reservoir > i32DesiredFill ) {
					if ( i32MaxTargetBits > 0 ) {
						pbmsBm->minmax_reservoir += (i32ThisBits - i32MaxTargetBits);
						if ( pbmsBm->minmax_reservoir < i32DesiredFill ) { pbmsBm->minmax_reservoir = i32DesiredFill; }
					}
					else {
						pbmsBm->minmax_reservoir = i32DesiredFill;
					}
				}
				else {
					if ( i32MinTargetBits > 0 ) {
						pbmsBm->minmax_reservoir += (i32ThisBits - i32MinTargetBits);
						if ( pbmsBm->minmax_reservoir > i32DesiredFill ) { pbmsBm->minmax_reservoir = i32DesiredFill; }
					}
					else {
						pbmsBm->minmax_reservoir = i32DesiredFill;
					}
				}
			}
		}

		// Average reservoir.
		if ( pbmsBm->avg_bitsper > 0 ) {
			int32_t i32AvgTargetBits = (_pvbVb->W ? pbmsBm->avg_bitsper * pbmsBm->short_per_long : pbmsBm->avg_bitsper);
			pbmsBm->avg_reservoir += i32ThisBits - i32AvgTargetBits;
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_bitrate_flushpacket( vorbis_dsp_state * _pvdsVd, 
		COgg::ogg_packet * _popOp ) {
		CVorbisCodecInternal::private_state * ppsBackend = static_cast<CVorbisCodecInternal::private_state *>(_pvdsVd->backend_state );
		CVorbisBitrate::bitrate_manager_state * pbmsBm = &ppsBackend->bms;
		vorbis_block * pvbVb = pbmsBm->vb;
		int32_t i32Choice = PACKETBLOBS / 2;
		if ( !pvbVb ) { return 0; }

		if ( _popOp ) {
			CVorbisCodecInternal::vorbis_block_internal * pvbiVbi = static_cast<CVorbisCodecInternal::vorbis_block_internal *>(pvbVb->internal);

			if ( CVorbisBitrate::vorbis_bitrate_managed( pvbVb ) ) {
				i32Choice = pbmsBm->choice;
			}

			_popOp->packet = COgg::oggpack_get_buffer( pvbiVbi->packetblob[i32Choice] );
			_popOp->bytes = COgg::oggpack_bytes( pvbiVbi->packetblob[i32Choice] );
			_popOp->b_o_s = 0;
			_popOp->e_o_s = pvbVb->eofflag;
			_popOp->granulepos = pvbVb->granulepos;
			_popOp->packetno = pvbVb->sequence;
		}

		pbmsBm->vb = 0;
		return 1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_idheader( COgg::ogg_packet * _popOp ) {
		COgg::oggpack_buffer obBuffer;
		char szBuffer[6];

		if ( _popOp ) {
			COgg::oggpack_readinit( &obBuffer, _popOp->packet, _popOp->bytes );

			if ( !_popOp->b_o_s )
				return 0; // Not the initial packet.

			if ( COgg::oggpack_read( &obBuffer, 8 ) != 1 ) {
				return 0; // Not an ID header.
			}

			CStd::MemSet( szBuffer, 0, 6 );
			_v_readstring( &obBuffer, szBuffer, 6 );
			if ( !CStd::MemCmpF( szBuffer, "vorbis", 6 ) )
				return 0;	// Not vorbis.

			return 1;
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_headerin( vorbis_info * _pviVi, vorbis_comment * _pvcVc, 
		COgg::ogg_packet * _popOp ) {

		COgg::oggpack_buffer obBuffer;

		if ( _popOp ) {
			COgg::oggpack_readinit( &obBuffer, _popOp->packet, _popOp->bytes );

			// Which of the three types of header is this?
			// Also verify header-ness, Vorbis.
			{
				char szBuffer[6];
				int32_t i32PackType = COgg::oggpack_read( &obBuffer, 8 );
				CStd::MemSet( szBuffer, 0, 6 );
				_v_readstring( &obBuffer, szBuffer, 6 );
				if ( !CStd::MemCmpF( szBuffer, "vorbis", 6 ) ) {
					// Not a vorbis header.
					return OV_ENOTVORBIS;
				}
				switch( i32PackType ) {
					case 0x01 : {
						if ( !_popOp->b_o_s ) {
							// Not the initial packet.
							return OV_EBADHEADER;
						}
						if ( _pviVi->rate != 0 ) {
							// Previously initialized info header.
							return OV_EBADHEADER;
						}

						return _vorbis_unpack_info( _pviVi, &obBuffer );
					}
					case 0x03 : {
						if ( _pviVi->rate == 0 ) {
							return OV_EBADHEADER;
						}

						return _vorbis_unpack_comment( _pvcVc, &obBuffer );
					}
					case 0x05 : {
						if ( _pviVi->rate == 0 || _pvcVc->vendor == NULL ) {
							return OV_EBADHEADER;
						}

						return _vorbis_unpack_books( _pviVi, &obBuffer );
					}
					default : {
						// Not a valid vorbis header type.
						return OV_EBADHEADER;
					}
				}
			}
		}
		return OV_EBADHEADER;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_init( vorbis_dsp_state * _pvdsV, vorbis_info * _pviVi ) {
		if ( _vds_shared_init( _pvdsV, _pviVi, 0 ) ) {
			vorbis_dsp_clear( _pvdsV );
			return 1;
		}
		vorbis_synthesis_restart( _pvdsV );
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_restart( vorbis_dsp_state * _pvdsV ) {
		vorbis_info * pviInfo = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi;
		int32_t i32Hs;

		if ( !_pvdsV->backend_state ) { return -1; }
		if ( !pviInfo ) { return -1; }
		pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviInfo->codec_setup);
		if ( !pcsiCi ) { return -1; }
		i32Hs = pcsiCi->halfrate_flag;

		_pvdsV->centerW = pcsiCi->blocksizes[1] >> (i32Hs + 1);
		_pvdsV->pcm_current = _pvdsV->centerW >> i32Hs;

		_pvdsV->pcm_returned = -1;
		_pvdsV->granulepos = -1;
		_pvdsV->sequence = -1;
		_pvdsV->eofflag = 0;
		static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state)->sample_count = -1;

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp ) {
		vorbis_dsp_state * pvdsVd = _pvbVb ? _pvbVb->vd : 0;
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd ? pvdsVd->backend_state : 0);
		vorbis_info * pviVi = pvdsVd ? pvdsVd->vi : 0;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi ? pviVi->codec_setup : 0);
		COgg::oggpack_buffer * pobBuffer = _pvbVb ? &_pvbVb->opb : 0;
		int32_t i32Type, i32Mode, I;

		if ( !pvdsVd || !ppsB || !pviVi || !pcsiCi || !pobBuffer ) {
			return OV_EBADPACKET;
		}

		// First thing's first.  Make sure decode is ready.
		CVorbisMisc::_vorbis_block_ripcord( _pvbVb );
		COgg::oggpack_readinit( pobBuffer, _popOp->packet, _popOp->bytes );

		// Check the packet type.
		if ( COgg::oggpack_read( pobBuffer, 1 ) != 0 ) {
			// Oops.  This is not an audio data packet.
			return OV_ENOTAUDIO;
		}

		// Read our mode and pre/post windowsize.
		i32Mode = COgg::oggpack_read( pobBuffer, ppsB->modebits );
		if ( i32Mode == -1 ) {
			return OV_EBADPACKET;
		}

		_pvbVb->mode = i32Mode;
		if ( !pcsiCi->mode_param[i32Mode] ) {
			return OV_EBADPACKET;
		}

		_pvbVb->W = pcsiCi->mode_param[i32Mode]->blockflag;
		if ( _pvbVb->W ) {

			// This doesn't get mapped through mode selection as it's used only for window selection.
			_pvbVb->lW = COgg::oggpack_read( pobBuffer, 1 );
			_pvbVb->nW = COgg::oggpack_read( pobBuffer, 1 );
			if ( _pvbVb->nW == -1 ) {
				return OV_EBADPACKET;
			}
		}
		else {
			_pvbVb->lW = 0;
			_pvbVb->nW = 0;
		}

		// More setup.
		_pvbVb->granulepos = _popOp->granulepos;
		_pvbVb->sequence = _popOp->packetno;
		_pvbVb->eofflag = _popOp->e_o_s;

		// Alloc pcm passback storage.
		_pvbVb->pcmend = pcsiCi->blocksizes[_pvbVb->W];
		_pvbVb->pcm = static_cast<float **>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, sizeof( (*_pvbVb->pcm) ) * pviVi->channels ));
		for ( I = 0; I < pviVi->channels; I++ ) {
			_pvbVb->pcm[I] = static_cast<float *>(CVorbisMisc::_vorbis_block_alloc( _pvbVb, _pvbVb->pcmend * sizeof( (*_pvbVb->pcm[I]) ) ));
		}

		// Unpack_header enforces range checking.
		i32Type = pcsiCi->map_type[pcsiCi->mode_param[i32Mode]->mapping];

		return CVorbisRegistry::_mapping_P[i32Type]->inverse( _pvbVb, pcsiCi->map_param[pcsiCi->mode_param[i32Mode]->mapping] );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_trackonly( vorbis_block * _pvbVb, COgg::ogg_packet * _popOp ) {
		vorbis_dsp_state * pvdsVd = _pvbVb->vd;
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(pvdsVd->backend_state);
		vorbis_info * pviVi = pvdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		COgg::oggpack_buffer * pobOpb = &_pvbVb->opb;
		int32_t i32Mode;

		// First thing's first.  Make sure decode is ready.
		CVorbisMisc::_vorbis_block_ripcord( _pvbVb );
		COgg::oggpack_readinit( pobOpb, _popOp->packet, _popOp->bytes );

		// Check the packet type.
		if ( COgg::oggpack_read( pobOpb, 1 ) != 0 ) {
			// Oops.  This is not an audio data packet.
			return OV_ENOTAUDIO;
		}

		// Read our mode and pre/post windowsize.
		i32Mode = COgg::oggpack_read( pobOpb, ppsB->modebits );
		if ( i32Mode == -1 ) { return OV_EBADPACKET; }

		_pvbVb->mode = i32Mode;
		if ( !pcsiCi->mode_param[i32Mode] ) {
			return OV_EBADPACKET;
		}
		
		_pvbVb->W = pcsiCi->mode_param[i32Mode]->blockflag;
		if ( _pvbVb->W ) {
			_pvbVb->lW = COgg::oggpack_read( pobOpb, 1 );
			_pvbVb->nW = COgg::oggpack_read( pobOpb, 1 );
			if ( _pvbVb->nW == -1 )	{ return OV_EBADPACKET; }
		}
		else {
			_pvbVb->lW = 0;
			_pvbVb->nW = 0;
		}

		// More setup.
		_pvbVb->granulepos = _popOp->granulepos;
		_pvbVb->sequence = _popOp->packetno;
		_pvbVb->eofflag = _popOp->e_o_s;

		// No PCM.
		_pvbVb->pcmend = 0;
		_pvbVb->pcm = NULL;

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pvbVb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_blockin( vorbis_dsp_state * _pvdsV, vorbis_block * _pvbVb ) {
		vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);
		int32_t i32Hs = pcsiCi->halfrate_flag;
		int32_t I, J;

		if ( !_pvbVb ) { return OV_EINVAL; }
		if ( _pvdsV->pcm_current > _pvdsV->pcm_returned && _pvdsV->pcm_returned != -1 ) { return OV_EINVAL; }

		_pvdsV->lW = _pvdsV->W;
		_pvdsV->W = _pvbVb->W;
		_pvdsV->nW = -1;

		if ( (_pvdsV->sequence == -1) ||
			(_pvdsV->sequence + 1 != _pvbVb->sequence) ) {
			_pvdsV->granulepos = -1;
			ppsB->sample_count = -1;
		}

		_pvdsV->sequence = _pvbVb->sequence;

		if ( _pvbVb->pcm ) {
			int32_t i32N = pcsiCi->blocksizes[_pvdsV->W] >> (i32Hs + 1);
			int32_t i32N0 = pcsiCi->blocksizes[0] >> (i32Hs + 1);
			int32_t i32N1 = pcsiCi->blocksizes[1] >> (i32Hs + 1);

			int32_t i32ThisCenter;
			int32_t i32PrevCenter;

			_pvdsV->glue_bits += _pvbVb->glue_bits;
			_pvdsV->time_bits += _pvbVb->time_bits;
			_pvdsV->floor_bits += _pvbVb->floor_bits;
			_pvdsV->res_bits += _pvbVb->res_bits;

			if ( _pvdsV->centerW ) {
				i32ThisCenter = i32N1;
				i32PrevCenter = 0;
			}
			else {
				i32ThisCenter = 0;
				i32PrevCenter = i32N1;
			}

			for ( J = 0; J < pviVi->channels; J++ ) {
				if ( _pvdsV->lW ) {
					if ( _pvdsV->W ) {
						float * pfW = COggWindow::_vorbis_window_get( ppsB->window[1] - i32Hs );
						float * pfPcm = _pvdsV->pcm[J] + i32PrevCenter;
						float * pfP = _pvbVb->pcm[J];
						for ( I = 0; I < i32N1; I++ ) {
							pfPcm[I] = pfPcm[I] * pfW[i32N1-I-1] + pfP[I] * pfW[I];
						}
					}
					else {
						float * pfW = COggWindow::_vorbis_window_get( ppsB->window[0] - i32Hs );
						float * pfPcm = _pvdsV->pcm[J] + i32PrevCenter + i32N1 / 2 - i32N0 / 2;
						float * pfP = _pvbVb->pcm[J];
						for ( I = 0; I < i32N0; I++ ) {
							pfPcm[I] = pfPcm[I] * pfW[i32N0-I-1] + pfP[I] * pfW[I];
						}
					}
				}
				else {
					if ( _pvdsV->W ) {
						float * pfW = COggWindow::_vorbis_window_get( ppsB->window[0] - i32Hs );
						float * pfPcm = _pvdsV->pcm[J] + i32PrevCenter;
						float * pfP = _pvbVb->pcm[J] + i32N1 / 2 - i32N0 / 2;
						for ( I = 0; I < i32N0; I++ ) {
							pfPcm[I] = pfPcm[I] * pfW[i32N0-I-1] + pfP[I] * pfW[I];
						}
						for ( ; I < i32N1 / 2 + i32N0 / 2; I++ ) { pfPcm[I] = pfP[I]; }
					}
					else {
						float * pfW = COggWindow::_vorbis_window_get( ppsB->window[0] - i32Hs );
						float * pfPcm = _pvdsV->pcm[J] + i32PrevCenter;
						float * pfP = _pvbVb->pcm[J];
						for ( I = 0; I < i32N0; I++ ) {
							pfPcm[I] = pfPcm[I] * pfW[i32N0-I-1] + pfP[I] * pfW[I];
						}
					}
				}

				{
					float * pfPcm = _pvdsV->pcm[J] + i32ThisCenter;
					float * pfP = _pvbVb->pcm[J] + i32N;
					for ( I = 0; I < i32N; I++ ) {
						pfPcm[I] = pfP[I];
					}
				}
			}

			if ( _pvdsV->centerW ) {
				_pvdsV->centerW = 0;
			}
			else {
				_pvdsV->centerW = i32N1;
			}

			if ( _pvdsV->pcm_returned == -1 ) {
				_pvdsV->pcm_returned = i32ThisCenter;
				_pvdsV->pcm_current = i32ThisCenter;
			}
			else {
				_pvdsV->pcm_returned = i32PrevCenter;
				_pvdsV->pcm_current = i32PrevCenter +
					((pcsiCi->blocksizes[_pvdsV->lW] / 4 + pcsiCi->blocksizes[_pvdsV->W] / 4) >> i32Hs);
			}

		}

		if ( ppsB->sample_count == -1 ) {
			ppsB->sample_count = 0;
		}
		else {
			ppsB->sample_count += pcsiCi->blocksizes[_pvdsV->lW] / 4 + pcsiCi->blocksizes[_pvdsV->W] / 4;
		}

		if ( _pvdsV->granulepos == -1 ) {
			if ( _pvbVb->granulepos != -1 ) {
				_pvdsV->granulepos = _pvbVb->granulepos;

				// Is this a short page?
				if ( ppsB->sample_count>_pvdsV->granulepos ) {
					// Corner case; if this is both the first and last audio page, 
					//	then spec says the end is cut, not beginning.
					int32_t i32Extra = static_cast<int32_t>(ppsB->sample_count - _pvbVb->granulepos);

					// We use int64_t for granule positions because a
					//	uint64_t isn't universally available.  Unfortunately, 
					//	that means granposes can be negative and result in
					//	i32Extra being negative.
					if ( i32Extra < 0 ) {
						i32Extra = 0;
					}

					if ( _pvbVb->eofflag ) {
						// Trim the end.
						// No preceding granulepos; assume we started at zero (we'd have to in a short single-page stream).
						// granulepos could be -1 due to a seek, but that would result
						//	in a long count, not short count.

						// Guard against corrupt/malicious frames that set EOP and
						//	a backdated granpos; don't rewind more samples than we
						//	actually have.
						if ( i32Extra > (_pvdsV->pcm_current - _pvdsV->pcm_returned) << i32Hs ) {
							i32Extra = (_pvdsV->pcm_current - _pvdsV->pcm_returned) << i32Hs;
						}

						_pvdsV->pcm_current -= i32Extra >> i32Hs;
					}
					else {
						// Trim the beginning.
						_pvdsV->pcm_returned += i32Extra >> i32Hs;
						if ( _pvdsV->pcm_returned > _pvdsV->pcm_current ) {
							_pvdsV->pcm_returned = _pvdsV->pcm_current;
						}
					}

				}

			}
		}
		else {
			_pvdsV->granulepos += pcsiCi->blocksizes[_pvdsV->lW] / 4 + pcsiCi->blocksizes[_pvdsV->W] / 4;
			if ( _pvbVb->granulepos != -1 && _pvdsV->granulepos != _pvbVb->granulepos ) {

				if ( _pvdsV->granulepos > _pvbVb->granulepos ) {
					int32_t i32Extra = static_cast<int32_t>(_pvdsV->granulepos - _pvbVb->granulepos);

					if ( i32Extra ) {
						if ( _pvbVb->eofflag ) {
							if ( i32Extra > (_pvdsV->pcm_current - _pvdsV->pcm_returned) << i32Hs ) {
								i32Extra = (_pvdsV->pcm_current - _pvdsV->pcm_returned) << i32Hs;
							}

							if ( i32Extra < 0 ) {
								i32Extra = 0;
							}

							_pvdsV->pcm_current -= i32Extra>>i32Hs;
						}
					}
				}
				_pvdsV->granulepos = _pvbVb->granulepos;
			}
		}

		// Update, cleanup.
		if ( _pvbVb->eofflag ) { _pvdsV->eofflag = 1; }
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pppfPcm Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_pcmout( vorbis_dsp_state * _pvdsV, float *** _pppfPcm ) {
		vorbis_info * pviVi = _pvdsV->vi;

		if ( _pvdsV->pcm_returned > -1 && _pvdsV->pcm_returned < _pvdsV->pcm_current ) {
			if ( _pppfPcm ) {
				int32_t I;
				for ( I = 0; I < pviVi->channels; I++ ) {
					_pvdsV->pcmret[I] = _pvdsV->pcm[I] + _pvdsV->pcm_returned;
				}
				(*_pppfPcm) = _pvdsV->pcmret;
			}
			return _pvdsV->pcm_current - _pvdsV->pcm_returned;
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pppfPcm Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_lapout( vorbis_dsp_state * _pvdsV, float *** _pppfPcm ) {
		vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		int32_t i32Hs = pcsiCi->halfrate_flag;

		int32_t i32N = pcsiCi->blocksizes[_pvdsV->W] >> (i32Hs + 1);
		int32_t i32N0 = pcsiCi->blocksizes[0] >> (i32Hs + 1);
		int32_t i32N1 = pcsiCi->blocksizes[1] >> (i32Hs + 1);
		int32_t I, J;

		if ( _pvdsV->pcm_returned < 0 ) { return 0; }

		if ( _pvdsV->centerW == i32N1 ) {
			for ( J = 0; J < pviVi->channels; J++ ) {
				float * pfP = _pvdsV->pcm[J];
				for ( I = 0; I < i32N1; I++ ) {
					float fTemp = pfP[I];
					pfP[I] = pfP[I+i32N1];
					pfP[I+i32N1] = fTemp;
				}
			}

			_pvdsV->pcm_current -= i32N1;
			_pvdsV->pcm_returned -= i32N1;
			_pvdsV->centerW = 0;
		}

		// Solidify buffer into contiguous space.
		if ( ( _pvdsV->lW ^ _pvdsV->W ) == 1 ) {
			// long/short or short/long.
			for ( J = 0; J < pviVi->channels; J++ ) {
				float * pfS = _pvdsV->pcm[J];
				float * pfD = _pvdsV->pcm[J] + (i32N1 - i32N0) / 2;
				for ( I = (i32N1 + i32N0) / 2 - 1; I >= 0; --I ) {
					pfD[I] = pfS[I];
				}
			}
			_pvdsV->pcm_returned += (i32N1 - i32N0) / 2;
			_pvdsV->pcm_current += (i32N1 - i32N0) / 2;
		}
		else {
			if ( _pvdsV->lW == 0 ) {
				// short/short.
				for ( J = 0; J < pviVi->channels; J++ ) {
					float * pfS = _pvdsV->pcm[J];
					float * pfD = _pvdsV->pcm[J] + i32N1 - i32N0;
					for ( I = i32N0 - 1; I >= 0; --I ) {
						pfD[I] = pfS[I];
					}
				}
				_pvdsV->pcm_returned += i32N1 - i32N0;
				_pvdsV->pcm_current += i32N1 - i32N0;
			}
		}

		if ( _pppfPcm ) {
			for ( I = 0; I < pviVi->channels; I++ ) {
				_pvdsV->pcmret[I] = _pvdsV->pcm[I] + _pvdsV->pcm_returned;
			}
			(*_pppfPcm) = _pvdsV->pcmret;
		}

		return i32N1 + i32N - _pvdsV->pcm_returned;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i32Samples Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_read( vorbis_dsp_state * _pvdsV, int32_t _i32Samples ) {
		if ( _i32Samples && _pvdsV->pcm_returned + _i32Samples > _pvdsV->pcm_current ) { return OV_EINVAL; }
		_pvdsV->pcm_returned += _i32Samples;
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _popOp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_packet_blocksize( vorbis_info * _pviVi, COgg::ogg_packet * _popOp ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		COgg::oggpack_buffer obBuffer;
		int32_t i32Mode;

		COgg::oggpack_readinit( &obBuffer, _popOp->packet, _popOp->bytes );

		// Check the packet type.
		if ( COgg::oggpack_read( &obBuffer, 1 ) != 0 ) {
			// Oops.  This is not an audio data packet.
			return OV_ENOTAUDIO;
		}

		{
			int32_t i32ModeBits = 0;
			int32_t i32V = pcsiCi->modes;
			while ( i32V>1 ) {
				i32ModeBits++;
				i32V >>= 1;
			}

			// Read our i32Mode and pre/post windowsize.
			i32Mode = COgg::oggpack_read( &obBuffer, i32ModeBits );
		}
		if ( i32Mode == -1 ) { return OV_EBADPACKET; }
		return pcsiCi->blocksizes[pcsiCi->mode_param[i32Mode]->blockflag];
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i32Flag Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_halfrate( vorbis_info * _pvdsV, int32_t _i32Flag ) {
		// Set/clear half-sample-rate mode.
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvdsV->codec_setup);

		// Right now, our MDCT can't handle < 64 sample windows.
		if ( pcsiCi->blocksizes[0] <= 64 && _i32Flag ) { return -1; }
		pcsiCi->halfrate_flag = (_i32Flag ? 1 : 0);
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::vorbis_synthesis_halfrate_p( vorbis_info * _pvdsV ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pvdsV->codec_setup);
		return pcsiCi->halfrate_flag;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _i32Window Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float * CVorbisCodec::vorbis_window( vorbis_dsp_state *_pvdsV, int32_t _i32Window ) {
		vorbis_info * pviInfo = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiSetupInfo = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviInfo->codec_setup);
		int32_t i32Hs = pcsiSetupInfo->halfrate_flag;
		CVorbisCodecInternal::private_state * ppsB = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state);

		if ( ppsB->window[_i32Window] - 1 < 0 ) { return NULL; }
		return COggWindow::_vorbis_window_get( ppsB->window[_i32Window] - i32Hs );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pcS1 Undocumented parameter from the Vorbis library.
	 * \param _pcS2 Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::tagcompare( const char * _pcS1, const char * _pcS2, int32_t _i32N ) {
		int32_t i32C = 0;
		while ( i32C < _i32N ) {
			if ( CStd::ToUpper( _pcS1[i32C] ) != CStd::ToUpper( _pcS2[i32C] ) ) {
				return 1;
			}
			i32C++;
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _i32Encp Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vds_shared_init( CVorbisCodec::vorbis_dsp_state * _pvdsV, CVorbisCodec::vorbis_info * _pviVi, int32_t _i32Encp ) {
		int32_t I;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		CVorbisCodecInternal::private_state * ppsB = NULL;
		int32_t i32Hs;

		if ( pcsiCi == NULL ) { return 1; }
		i32Hs = pcsiCi->halfrate_flag;

		CStd::MemSet( _pvdsV, 0, sizeof( (*_pvdsV) ) );
		ppsB = static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state = CMemLib::CMAlloc( sizeof( (*ppsB) ) ) );

		_pvdsV->vi = _pviVi;
		ppsB->modebits = ilog2( pcsiCi->modes );

		ppsB->transform[0] = static_cast<CVorbisStructs::vorbis_look_transform **>(CMemLib::CMAlloc( VI_TRANSFORMB * sizeof( (*ppsB->transform[0]) ) ));
		ppsB->transform[1] = static_cast<CVorbisStructs::vorbis_look_transform **>(CMemLib::CMAlloc( VI_TRANSFORMB * sizeof( (*ppsB->transform[1]) ) ));

		// MDCT is tranform 0.
		ppsB->transform[0][0] = CMemLib::CMAlloc( sizeof( CVorbisMdct::mdct_lookup ) );
		ppsB->transform[1][0] = CMemLib::CMAlloc( sizeof( CVorbisMdct::mdct_lookup ) );
		CVorbisMdct::mdct_init( static_cast<CVorbisMdct::mdct_lookup *>(ppsB->transform[0][0]), pcsiCi->blocksizes[0] >> i32Hs );
		CVorbisMdct::mdct_init( static_cast<CVorbisMdct::mdct_lookup *>(ppsB->transform[1][0]), pcsiCi->blocksizes[1] >> i32Hs );

		// Vorbis I uses only window type 0.
		ppsB->window[0] = ilog2( pcsiCi->blocksizes[0] ) - 6;
		ppsB->window[1] = ilog2( pcsiCi->blocksizes[1] ) - 6;

		if ( _i32Encp ) { // Encode/decode differ here.

			// Analysis always needs an FFT.
			CVorbisSmallFt::drft_init( &ppsB->fft_look[0], pcsiCi->blocksizes[0] );
			CVorbisSmallFt::drft_init( &ppsB->fft_look[1], pcsiCi->blocksizes[1] );

			// Finish the codebooks.
			if ( !pcsiCi->fullbooks ) {
				pcsiCi->fullbooks = static_cast<CVorbisCodeBook::codebook *>(CMemLib::CMAlloc( pcsiCi->books * sizeof( (*pcsiCi->fullbooks) ) ));
				for ( I = 0; I < pcsiCi->books; I++ ) {
					CVorbisCodeBook::vorbis_book_init_encode( pcsiCi->fullbooks + I, pcsiCi->book_param[I] );
				}
			}

			ppsB->psy = static_cast<CVorbisPsy::vorbis_look_psy *>(CMemLib::CMAlloc( pcsiCi->psys * sizeof( (*ppsB->psy) ) ));
			for ( I = 0; I < pcsiCi->psys; I++ ) {
				CVorbisPsy::_vp_psy_init( ppsB->psy + I,
					pcsiCi->psy_param[I], 
					&pcsiCi->psy_g_param, 
					pcsiCi->blocksizes[pcsiCi->psy_param[I]->blockflag] / 2, 
					_pviVi->rate );
			}

			_pvdsV->analysisp = 1;
		}
		else {
			// Finish the codebooks.
			if ( !pcsiCi->fullbooks ) {
				pcsiCi->fullbooks = static_cast<CVorbisCodeBook::codebook *>(CMemLib::CMAlloc( pcsiCi->books * sizeof( (*pcsiCi->fullbooks) ) ));
				for ( I = 0; I < pcsiCi->books; I++ ) {
					if ( pcsiCi->book_param[I] == NULL ) { goto AbortBooks; }
					if ( CVorbisCodeBook::vorbis_book_init_decode( pcsiCi->fullbooks + I, pcsiCi->book_param[I] ) ) { goto AbortBooks; }
					// Decode codebooks are now standalone after init.
					CVorbisCodeBook::vorbis_staticbook_destroy( pcsiCi->book_param[I] );
					pcsiCi->book_param[I] = NULL;
				}
			}
		}

		// Initialize the storage vectors.  blocksizes[1] is small for encode, 
		//	but the correct size for decode.
		_pvdsV->pcm_storage = pcsiCi->blocksizes[1];
		_pvdsV->pcm = static_cast<float **>(CMemLib::MAlloc( _pviVi->channels * sizeof( (*_pvdsV->pcm) ) ));
		_pvdsV->pcmret = static_cast<float **>(CMemLib::MAlloc( _pviVi->channels * sizeof( (*_pvdsV->pcmret) ) ));
		{
			for ( I = 0; I < _pviVi->channels; I++ ) {
				_pvdsV->pcm[I] = static_cast<float *>(CMemLib::CMAlloc( _pvdsV->pcm_storage * sizeof( (*_pvdsV->pcm[I]) ) ));
			}
		}

		_pvdsV->lW = 0;
		_pvdsV->W = 0;

		// All vector indexes.
		_pvdsV->centerW = pcsiCi->blocksizes[1] / 2;

		_pvdsV->pcm_current = _pvdsV->centerW;

		// Initialize all the backend lookups.
		ppsB->flr = static_cast<CVorbisStructs::vorbis_look_floor **>(CMemLib::CMAlloc( pcsiCi->floors * sizeof( (*ppsB->flr) ) ));
		ppsB->residue = static_cast<CVorbisStructs::vorbis_look_residue **>(CMemLib::CMAlloc( pcsiCi->residues * sizeof( (*ppsB->residue) ) ));

		for ( I = 0; I < pcsiCi->floors; I++ ) {
			ppsB->flr[I] = CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->look( _pvdsV, pcsiCi->floor_param[I] );
		}

		for ( I = 0; I < pcsiCi->residues; I++ ) {
			ppsB->residue[I] = CVorbisRegistry::_residue_P[pcsiCi->residue_type[I]]->look( _pvdsV, pcsiCi->residue_param[I] );
		}

		return 0;
	AbortBooks :
		for ( I = 0; I < pcsiCi->books; I++ ) {
			if ( pcsiCi->book_param[I] != NULL ) {
				CVorbisCodeBook::vorbis_staticbook_destroy( pcsiCi->book_param[I] );
				pcsiCi->book_param[I] = NULL;
			}
		}
		CVorbisCodec::vorbis_dsp_clear( _pvdsV );
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ui32V Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::ilog2( uint32_t _ui32V ) {
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
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_pack_comment( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_comment * _pvcVc ) {
		int32_t bytes = CStd::StrLen( ENCODE_VENDOR_STRING );

		// Preamble.
		COgg::oggpack_write( _pobOpb, 0x03, 8 );
		_v_writestring( _pobOpb, "vorbis", 6 );

		// Vendor.
		COgg::oggpack_write( _pobOpb, bytes, 32 );
		_v_writestring( _pobOpb, ENCODE_VENDOR_STRING, bytes );

		// Comments.

		COgg::oggpack_write( _pobOpb, _pvcVc->comments, 32 );
		if ( _pvcVc->comments ) {
			int32_t I;
			for ( I = 0; I < _pvcVc->comments; I++ ) {
				if ( _pvcVc->user_comments[I] ) {
					COgg::oggpack_write( _pobOpb, _pvcVc->comment_lengths[I], 32 );
					_v_writestring( _pobOpb, _pvcVc->user_comments[I], _pvcVc->comment_lengths[I] );
				}
				else {
					COgg::oggpack_write( _pobOpb, 0, 32 );
				}
			}
		}
		COgg::oggpack_write( _pobOpb, 1, 1 );

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pobOut Undocumented parameter from the Vorbis library.
	 * \param _pcString Undocumented parameter from the Vorbis library.
	 * \param _i32Length Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::_v_writestring( COgg::oggpack_buffer * _pobOut, const char * _pcString, int32_t _i32Length ) {
		while ( _i32Length-- ) {
			COgg::oggpack_write( _pobOut, *_pcString++, 8 );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_pack_info( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_info * _pviVi ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		if ( !pcsiCi ) { return OV_EFAULT; }

		// Preamble.
		COgg::oggpack_write( _pobOpb, 0x01, 8 );
		_v_writestring( _pobOpb, "vorbis", 6 );

		// Basic information about the stream.
		COgg::oggpack_write( _pobOpb, 0x00, 32 );
		COgg::oggpack_write( _pobOpb, _pviVi->channels, 8 );
		COgg::oggpack_write( _pobOpb, _pviVi->rate, 32 );

		COgg::oggpack_write( _pobOpb, _pviVi->bitrate_upper, 32 );
		COgg::oggpack_write( _pobOpb, _pviVi->bitrate_nominal, 32 );
		COgg::oggpack_write( _pobOpb, _pviVi->bitrate_lower, 32 );

		COgg::oggpack_write( _pobOpb, ilog2( pcsiCi->blocksizes[0] ), 4 );
		COgg::oggpack_write( _pobOpb, ilog2( pcsiCi->blocksizes[1] ), 4 );
		COgg::oggpack_write( _pobOpb, 1, 1 );

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_pack_books( COgg::oggpack_buffer * _pobOpb, CVorbisCodec::vorbis_info * _pviVi ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		int32_t I;
		if ( !pcsiCi ) { return OV_EFAULT; }

		COgg::oggpack_write( _pobOpb, 0x05, 8 );
		_v_writestring( _pobOpb, "vorbis", 6 );

		// Books.
		COgg::oggpack_write( _pobOpb, pcsiCi->books - 1, 8 );
		for ( I = 0; I < pcsiCi->books; I++ ) {
			if ( CVorbisCodeBook::vorbis_staticbook_pack( pcsiCi->book_param[I], _pobOpb ) ) { goto ErrOut; }
		}

		// Times; hook placeholders.
		COgg::oggpack_write( _pobOpb, 0, 6 );
		COgg::oggpack_write( _pobOpb, 0, 16 );

		// Floors .
		COgg::oggpack_write( _pobOpb, pcsiCi->floors - 1, 6 );
		for ( I = 0; I < pcsiCi->floors; I++ ) {
			COgg::oggpack_write( _pobOpb, pcsiCi->floor_type[I], 16 );
			if ( CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->pack ) {
				CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->pack( pcsiCi->floor_param[I], _pobOpb );
			}
			else {
				goto ErrOut;
			}
		}

		// Residues.
		COgg::oggpack_write( _pobOpb, pcsiCi->residues-1, 6 );
		for ( I = 0; I < pcsiCi->residues; I++ ) {
			COgg::oggpack_write( _pobOpb, pcsiCi->residue_type[I], 16 );
			CVorbisRegistry::_residue_P[pcsiCi->residue_type[I]]->pack( pcsiCi->residue_param[I], _pobOpb );
		}

		// Maps.
		COgg::oggpack_write( _pobOpb, pcsiCi->maps - 1, 6 );
		for ( I = 0; I < pcsiCi->maps; I++ ) {
			COgg::oggpack_write( _pobOpb, pcsiCi->map_type[I], 16 );
			CVorbisRegistry::_mapping_P[pcsiCi->map_type[I]]->pack( _pviVi, pcsiCi->map_param[I], _pobOpb );
		}

		// Modes.
		COgg::oggpack_write( _pobOpb, pcsiCi->modes - 1, 6 );
		for ( I = 0; I < pcsiCi->modes; I++ ) {
			COgg::oggpack_write( _pobOpb, pcsiCi->mode_param[I]->blockflag, 1 );
			COgg::oggpack_write( _pobOpb, pcsiCi->mode_param[I]->windowtype, 16 );
			COgg::oggpack_write( _pobOpb, pcsiCi->mode_param[I]->transformtype, 16 );
			COgg::oggpack_write( _pobOpb, pcsiCi->mode_param[I]->mapping, 8 );
		}
		COgg::oggpack_write( _pobOpb, 1, 1 );

		return 0;
	ErrOut :
		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::_preextrapolate_helper( CVorbisCodec::vorbis_dsp_state * _pvdsV ) {
		int32_t I;
		int32_t i32Order = 16;
		float * pfLpc = static_cast<float *>(::alloca( i32Order * sizeof( (*pfLpc) ) ));
		float * pfWork = static_cast<float *>(::alloca( _pvdsV->pcm_current * sizeof( (*pfWork) ) ));
		int32_t J;
		_pvdsV->preextrapolate = 1;

		if ( _pvdsV->pcm_current - _pvdsV->centerW > i32Order * 2 ) {
			for ( I = 0; I < _pvdsV->vi->channels; I++ ) {
				// Need to run the extrapolation in reverse!
				for ( J = 0; J < _pvdsV->pcm_current; J++ ) {
					pfWork[J] = _pvdsV->pcm[I][_pvdsV->pcm_current-J-1];
				}

				// Prime as above.
				CVorbisLpc::vorbis_lpc_from_data( pfWork, pfLpc, _pvdsV->pcm_current - _pvdsV->centerW, i32Order );


				// Run the predictor filter.
				CVorbisLpc::vorbis_lpc_predict( pfLpc, pfWork + _pvdsV->pcm_current - _pvdsV->centerW - i32Order, 
					i32Order, 
					pfWork + _pvdsV->pcm_current - _pvdsV->centerW, 
					_pvdsV->centerW );

				for ( J = 0; J < _pvdsV->pcm_current; J++ ) {
					_pvdsV->pcm[I][_pvdsV->pcm_current-J-1] = pfWork[J];
				}
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _popbBuffer Undocumented parameter from the Vorbis library.
	 * \param _pcBuf Undocumented parameter from the Vorbis library.
	 * \param _i32Bytes Undocumented parameter from the Vorbis library.
	 */
	void CVorbisCodec::_v_readstring( COgg::oggpack_buffer * _popbBuffer, char * _pcBuf, int32_t _i32Bytes ) {
		while ( _i32Bytes-- ) {
			(*_pcBuf) = static_cast<char>(COgg::oggpack_read( _popbBuffer, 8 ));
			++_pcBuf;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_unpack_info( vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		if ( !pcsiCi ) { return OV_EFAULT; }

		_pviVi->version = COgg::oggpack_read( _pobOpb, 32 );
		if ( _pviVi->version != 0 ) { return OV_EVERSION; }

		_pviVi->channels = COgg::oggpack_read( _pobOpb, 8 );
		_pviVi->rate = COgg::oggpack_read( _pobOpb, 32 );

		_pviVi->bitrate_upper = COgg::oggpack_read( _pobOpb, 32 );
		_pviVi->bitrate_nominal = COgg::oggpack_read( _pobOpb, 32 );
		_pviVi->bitrate_lower = COgg::oggpack_read( _pobOpb, 32 );

		pcsiCi->blocksizes[0] = 1 << COgg::oggpack_read( _pobOpb, 4 );
		pcsiCi->blocksizes[1] = 1 << COgg::oggpack_read( _pobOpb, 4 );

		if ( _pviVi->rate < 1 ) { goto ErrOut; }
		if ( _pviVi->channels < 1 ) { goto ErrOut; }
		if ( pcsiCi->blocksizes[0] < 64 ) { goto ErrOut; }
		if ( pcsiCi->blocksizes[1] < pcsiCi->blocksizes[0] ) { goto ErrOut; }
		if ( pcsiCi->blocksizes[1] > 8192 ) { goto ErrOut; }

		if ( COgg::oggpack_read( _pobOpb, 1 ) != 1 ) { goto ErrOut; }

		return 0;
	ErrOut:
		vorbis_info_clear( _pviVi );
		return OV_EBADHEADER;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_unpack_comment( vorbis_comment * _pvcVc, COgg::oggpack_buffer * _pobOpb ) {
		int32_t I;
		int32_t i32VendorLen = COgg::oggpack_read( _pobOpb, 32 );
		if ( i32VendorLen < 0 ) { goto ErrOut; }
		if ( i32VendorLen > _pobOpb->storage - 8 ) { goto ErrOut; }
		_pvcVc->vendor = static_cast<char *>(CMemLib::CMAlloc( (i32VendorLen + 1) * 1 ));
		_v_readstring( _pobOpb, _pvcVc->vendor, i32VendorLen );
		I = COgg::oggpack_read( _pobOpb, 32 );
		if ( I < 0 ) { goto ErrOut; }
		if ( I > ((_pobOpb->storage-COgg::oggpack_bytes( _pobOpb )) >> 2) ) { goto ErrOut; }
		_pvcVc->comments = I;
		_pvcVc->user_comments = static_cast<char **>(CMemLib::CMAlloc( (_pvcVc->comments + 1) * sizeof( (*_pvcVc->user_comments) ) ));
		_pvcVc->comment_lengths = static_cast<int32_t *>(CMemLib::CMAlloc( (_pvcVc->comments + 1) * sizeof( (*_pvcVc->comment_lengths) ) ));

		for ( I = 0; I < _pvcVc->comments; I++ ) {
			int32_t i32Len = COgg::oggpack_read( _pobOpb, 32 );
			if ( i32Len < 0 ) { goto ErrOut; }
			if ( i32Len > _pobOpb->storage - COgg::oggpack_bytes( _pobOpb ) ) { goto ErrOut; }
			_pvcVc->comment_lengths[I] = i32Len;
			_pvcVc->user_comments[I] = static_cast<char *>(CMemLib::CMAlloc( (i32Len + 1) * 1 ));
			_v_readstring( _pobOpb, _pvcVc->user_comments[I], i32Len );
		}
		if ( COgg::oggpack_read( _pobOpb, 1 ) != 1 ) { goto ErrOut; }

		return 0;
	 ErrOut:
		vorbis_comment_clear( _pvcVc );
		return OV_EBADHEADER;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pobOpb Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisCodec::_vorbis_unpack_books( vorbis_info * _pviVi, COgg::oggpack_buffer * _pobOpb ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_pviVi->codec_setup);
		int32_t I;
		if ( !pcsiCi ) { return OV_EFAULT; }

		// Codebooks.
		pcsiCi->books = COgg::oggpack_read( _pobOpb, 8 ) + 1;
		if ( pcsiCi->books <= 0 ) { goto ErrOut; }
		for ( I = 0; I < pcsiCi->books; I++ ) {
			pcsiCi->book_param[I] = CVorbisCodeBook::vorbis_staticbook_unpack( _pobOpb );
			if ( !pcsiCi->book_param[I] ) { goto ErrOut; }
		}

		{
			int32_t i32Times = COgg::oggpack_read( _pobOpb, 6 ) + 1;
			if ( i32Times <= 0 ) { goto ErrOut; }
			for ( I = 0; I < i32Times; I++ ) {
				int32_t i32Test = COgg::oggpack_read( _pobOpb, 16 );
				if ( i32Test < 0 || i32Test >= VI_TIMEB ) { goto ErrOut; }
			}
		}

		// Floor backend settings.
		pcsiCi->floors = COgg::oggpack_read( _pobOpb, 6 ) + 1;
		if ( pcsiCi->floors <= 0 ) { goto ErrOut; }
		for ( I = 0; I < pcsiCi->floors; I++ ) {
			pcsiCi->floor_type[I] = COgg::oggpack_read( _pobOpb, 16 );
			if ( pcsiCi->floor_type[I] < 0 || pcsiCi->floor_type[I] >= VI_FLOORB ) { goto ErrOut; }
			pcsiCi->floor_param[I] = CVorbisRegistry::_floor_P[pcsiCi->floor_type[I]]->unpack( _pviVi, _pobOpb );
			if ( !pcsiCi->floor_param[I] ) { goto ErrOut; }
		}

		// Residue backend settings.
		pcsiCi->residues = COgg::oggpack_read( _pobOpb, 6 ) + 1;
		if ( pcsiCi->residues <= 0 ) { goto ErrOut; }
		for ( I = 0; I < pcsiCi->residues; I++ ) {
			pcsiCi->residue_type[I] = COgg::oggpack_read( _pobOpb, 16 );
			if ( pcsiCi->residue_type[I] < 0 || pcsiCi->residue_type[I] >= VI_RESB ) { goto ErrOut; }
			pcsiCi->residue_param[I] = CVorbisRegistry::_residue_P[pcsiCi->residue_type[I]]->unpack( _pviVi, _pobOpb );
			if ( !pcsiCi->residue_param[I] ) { goto ErrOut; }
		}

		// Map backend settings.
		pcsiCi->maps = COgg::oggpack_read( _pobOpb, 6 ) + 1;
		if ( pcsiCi->maps <= 0 ) { goto ErrOut; }
		for ( I = 0; I < pcsiCi->maps; I++ ) {
			pcsiCi->map_type[I] = COgg::oggpack_read( _pobOpb, 16 );
			if ( pcsiCi->map_type[I] < 0 || pcsiCi->map_type[I] >= VI_MAPB ) { goto ErrOut; }
			pcsiCi->map_param[I] = CVorbisRegistry::_mapping_P[pcsiCi->map_type[I]]->unpack( _pviVi, _pobOpb );
			if ( !pcsiCi->map_param[I] ) { goto ErrOut; }
		}

		// Mode settings.
		pcsiCi->modes = COgg::oggpack_read( _pobOpb, 6 ) + 1;
		if ( pcsiCi->modes <= 0 ) { goto ErrOut; }
		for ( I = 0; I < pcsiCi->modes; I++ ) {
			pcsiCi->mode_param[I] = static_cast<CVorbisStructs::vorbis_info_mode *>(CMemLib::CMAlloc( sizeof( (*pcsiCi->mode_param[I]) ) ));
			pcsiCi->mode_param[I]->blockflag = COgg::oggpack_read( _pobOpb, 1 );
			pcsiCi->mode_param[I]->windowtype = COgg::oggpack_read( _pobOpb, 16 );
			pcsiCi->mode_param[I]->transformtype = COgg::oggpack_read( _pobOpb, 16 );
			pcsiCi->mode_param[I]->mapping = COgg::oggpack_read( _pobOpb, 8 );

			if ( pcsiCi->mode_param[I]->windowtype >= VI_WINDOWB ) { goto ErrOut; }
			if ( pcsiCi->mode_param[I]->transformtype >= VI_WINDOWB ) { goto ErrOut; }
			if ( pcsiCi->mode_param[I]->mapping >= pcsiCi->maps ) { goto ErrOut; }
			if ( pcsiCi->mode_param[I]->mapping<0 ) { goto ErrOut; }
		}

		if ( COgg::oggpack_read( _pobOpb, 1 ) != 1 ) { goto ErrOut; }

		return 0;
	ErrOut:
		vorbis_info_clear( _pviVi );
		return OV_EBADHEADER;
	}

}	// namespace lss
