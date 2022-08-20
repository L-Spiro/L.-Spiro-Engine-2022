/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "LSIJ2k.h"
#include "LSICio.h"
#include "LSIDwt.h"
#include "LSIEvent.h"
#include "LSIJp2.h"
#include "LSIJpegInt.h"
#include "LSIJpt.h"
#include "LSIPi.h"
#include "LSITcd.h"


namespace lsi {

	// == Members.
	CJ2k::opj_dec_mstabent_t CJ2k::j2k_dec_mstab[] = {
		{ J2K_MS_SOC, J2K_STATE_MHSOC, j2k_read_soc },
		{ J2K_MS_SOT, J2K_STATE_MH | J2K_STATE_TPHSOT, j2k_read_sot },
		{ J2K_MS_SOD, J2K_STATE_TPH, j2k_read_sod },
		{ J2K_MS_EOC, J2K_STATE_TPHSOT, j2k_read_eoc },
		{ J2K_MS_SIZ, J2K_STATE_MHSIZ, j2k_read_siz },
		{ J2K_MS_COD, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_cod },
		{ J2K_MS_COC, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_coc },
		{ J2K_MS_RGN, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_rgn },
		{ J2K_MS_QCD, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_qcd },
		{ J2K_MS_QCC, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_qcc },
		{ J2K_MS_POC, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_poc },
		{ J2K_MS_TLM, J2K_STATE_MH, j2k_read_tlm },
		{ J2K_MS_PLM, J2K_STATE_MH, j2k_read_plm },
		{ J2K_MS_PLT, J2K_STATE_TPH, j2k_read_plt },
		{ J2K_MS_PPM, J2K_STATE_MH, j2k_read_ppm },
		{ J2K_MS_PPT, J2K_STATE_TPH, j2k_read_ppt },
		{ J2K_MS_SOP, 0, 0 },
		{ J2K_MS_CRG, J2K_STATE_MH, j2k_read_crg },
		{ J2K_MS_COM, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_com },

#ifdef USE_JPWL
		{ J2K_MS_EPC, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_epc },
		{ J2K_MS_EPB, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_epb },
		{ J2K_MS_ESD, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_esd },
		{ J2K_MS_RED, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_red },
#endif	// #ifdef USE_JPWL
#ifdef USE_JPSEC
		{ J2K_MS_SEC, J2K_STATE_MH, j2k_read_sec },
		{ J2K_MS_INSEC, 0, j2k_read_insec },
#endif	// #ifdef USE_JPSEC

		{ 0, J2K_STATE_MH | J2K_STATE_TPH, j2k_read_unk}
	};


	// == Functions.
	/**
	 * Creates a _pjJ2k decompression structure.
	 *
	 * \param _cpInfoC Codec context info.
	 * \return Returns a handle to a _pjJ2k decompressor if successful, returns NULL otherwise.
	 */
	CJ2k::opj_j2k_t * LSE_CALL CJ2k::j2k_create_decompress( CJpegCommonStructs::opj_common_ptr _cpInfoC ) {
		opj_j2k_t * pjJ2k = static_cast<opj_j2k_t *>(CMemLib::CMAlloc( sizeof( opj_j2k_t ) ));
		if ( !pjJ2k ) { return NULL; }

		pjJ2k->default_tcp = static_cast<opj_tcp_t *>(CMemLib::CMAlloc( sizeof( opj_tcp_t ) ));
		if ( !pjJ2k->default_tcp ) {
			CMemLib::Free( pjJ2k );
			return NULL;
		}

		pjJ2k->cinfo = _cpInfoC;
		pjJ2k->tile_data = NULL;

		return pjJ2k;
	}

	/**
	 * Destroy a _pjJ2k decompressor handle.
	 *
	 * \param _pjJ2k _pjJ2k decompressor handle to destroy.
	 */
	void LSE_CALL CJ2k::j2k_destroy_decompress( opj_j2k_t * _pjJ2k ) {
		int32_t I = 0;

		if ( _pjJ2k->tile_len != NULL ) {
			CMemLib::Free( _pjJ2k->tile_len );
		}
		if ( _pjJ2k->tile_data != NULL ) {
			CMemLib::Free( _pjJ2k->tile_data );
		}
		if ( _pjJ2k->default_tcp != NULL ) {
			opj_tcp_t * ptDefaultTcp = _pjJ2k->default_tcp;
			if ( ptDefaultTcp->ppt_data_first != NULL ) {
				CMemLib::Free( ptDefaultTcp->ppt_data_first );
			}
			if ( _pjJ2k->default_tcp->tccps != NULL ) {
				CMemLib::Free( _pjJ2k->default_tcp->tccps );
			}
			CMemLib::Free( _pjJ2k->default_tcp );
		}
		if ( _pjJ2k->cp != NULL ) {
			opj_cp_t * pcCp = _pjJ2k->cp;
			if ( pcCp->tcps != NULL ) {
				for ( I = 0; I < pcCp->tw * pcCp->th; I++ ) {
					if ( pcCp->tcps[I].ppt_data_first != NULL ) {
						CMemLib::Free( pcCp->tcps[I].ppt_data_first );
					}
					if ( pcCp->tcps[I].tccps != NULL ) {
						CMemLib::Free( pcCp->tcps[I].tccps );
					}
				}
				CMemLib::Free( pcCp->tcps );
			}
			if ( pcCp->ppm_data_first != NULL ) {
				CMemLib::Free( pcCp->ppm_data_first );
			}
			if ( pcCp->tileno != NULL ) {
				CMemLib::Free( pcCp->tileno );  
			}
			if ( pcCp->comment != NULL ) {
				CMemLib::Free( pcCp->comment );
			}

			CMemLib::Free( pcCp );
		}
		CMemLib::Free( _pjJ2k );
	}

	/**
	 * Setup the decoder decoding parameters using user parameters.
	 * Decoding parameters are returned in _pjJ2k->cp.
	 *
	 * \param _pjJ2k _pjJ2k decompressor handle.
	 * \param _pdParameters decompression _pdParameters.
	 */
	void LSE_CALL CJ2k::j2k_setup_decoder( opj_j2k_t * _pjJ2k, CJpegCommonStructs::opj_dparameters_t * _pdParameters ) {
		if ( _pjJ2k && _pdParameters ) {
			// Create and initialize the coding parameters structure.
			opj_cp_t * pcCp = static_cast<opj_cp_t *>(CMemLib::CMAlloc( sizeof( opj_cp_t ) ));
			pcCp->reduce = _pdParameters->cp_reduce;	
			pcCp->layer = _pdParameters->cp_layer;
			pcCp->limit_decoding = _pdParameters->cp_limit_decoding;

#ifdef USE_JPWL
			pcCp->correct = _pdParameters->jpwl_correct;
			pcCp->exp_comps = _pdParameters->jpwl_exp_comps;
			pcCp->max_tiles = _pdParameters->jpwl_max_tiles;
#endif	// #ifdef USE_JPWL


			// Keep a link to pcCp so that we can destroy it later in j2k_destroy_decompress.
			_pjJ2k->cp = pcCp;
		}
	}

	/**
	 * Decode an piImage from a JPEG-2000 codestream.
	 *
	 * \param _pjJ2k _pjJ2k decompressor handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
	 * \return Returns a decoded piImage if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL CJ2k::j2k_decode( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciCStrInfo ) {
		CJpegImage::opj_image_t * piImage = NULL;

		CJpegCommonStructs::opj_common_ptr cpInfo = _pjJ2k->cinfo;	

		_pjJ2k->cio = _pcCio;
		_pjJ2k->cstr_info = _pciCStrInfo;
		if ( _pciCStrInfo ) {
			CStd::MemSet( _pciCStrInfo, 0, sizeof( CJpegCommonStructs::opj_codestream_info_t ) );
		}

		// Create an empty piImage.
		piImage = CJpegImage::opj_image_create0();
		_pjJ2k->image = piImage;

		_pjJ2k->state = J2K_STATE_MHSOC;

		for ( ; ; ) {
			opj_dec_mstabent_t * pdmE;
			uint32_t ui32Id = CCio::cio_read( _pcCio, 2 );

			if ( ui32Id >> 8 != 0xFF ) {
				CJpegImage::opj_image_destroy( piImage );
				CEvent::opj_event_msg( cpInfo, EVT_ERROR, "%.8x: expected a marker instead of %x.\r\n", CCio::cio_tell( _pcCio ) - 2, ui32Id );
				return 0;
			}
			pdmE = j2k_dec_mstab_lookup( static_cast<int32_t>(ui32Id) );
			// Check if the marker is known
			if ( !( _pjJ2k->state & pdmE->states ) ) {
				CJpegImage::opj_image_destroy( piImage );
				CEvent::opj_event_msg( cpInfo, EVT_ERROR, "%.8x: unexpected marker %x.\r\n", CCio::cio_tell( _pcCio ) - 2, ui32Id );
				return 0;
			}
			// Check if the decoding is limited to the main header
			if ( pdmE->id == J2K_MS_SOT && _pjJ2k->cp->limit_decoding == LIMIT_TO_MAIN_HEADER ) {
				CEvent::opj_event_msg( cpInfo, EVT_INFO, "Main Header decoded.\r\n" );
				return piImage;
			}		

			if ( pdmE->handler ) {
				(*pdmE->handler)( _pjJ2k );
			}
			if ( _pjJ2k->state & J2K_STATE_ERR ) { return NULL;	}

			if ( _pjJ2k->state == J2K_STATE_MT ) { break; }
			if ( _pjJ2k->state == J2K_STATE_NEOC ) { break; }
		}
		if ( _pjJ2k->state == J2K_STATE_NEOC ) {
			j2k_read_eoc( _pjJ2k );
		}

		if ( _pjJ2k->state != J2K_STATE_MT ) {
			CEvent::opj_event_msg( cpInfo, EVT_WARNING, "Incomplete bitstream.\r\n" );
		}

		return piImage;
	}

	/**
	 * Decode an image form a JPT-stream (JPEG 2000, JPIP).
	 *
	 * \param _pjJ2k J2K decompressor handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
	 * \return Returns a decoded image if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL CJ2k::j2k_decode_jpt_stream( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * /*_pciCStrInfo*/ ) {
		CJpegImage::opj_image_t * piImage = NULL;
		CJpt::opj_jpt_msg_header_t jmhHeader;
		int32_t i32Position;

		CJpegCommonStructs::opj_common_ptr cpInfo = _pjJ2k->cinfo;
		
		_pjJ2k->cio = _pcCio;

		// Create an empty image.
		piImage = CJpegImage::opj_image_create0();
		_pjJ2k->image = piImage;

		_pjJ2k->state = J2K_STATE_MHSOC;
		
		// Initialize the header.
		CJpt::jpt_init_msg_header( &jmhHeader );
		// Read the first header of the message.
		CJpt::jpt_read_msg_header( cpInfo, _pcCio, &jmhHeader );
		
		i32Position = CCio::cio_tell( _pcCio );
		if ( jmhHeader.Class_Id != 6 ) {	// 6 : Main jmhHeader data-bin message.
			CJpegImage::opj_image_destroy( piImage );
			CEvent::opj_event_msg( cpInfo, EVT_ERROR, "[JPT-stream]: Expecting Main jmhHeader first [class_Id %d]!\r\n", jmhHeader.Class_Id );
			return 0;
		}
		
		for ( ; ; ) {
			opj_dec_mstabent_t * pdmE = NULL;
			uint32_t ui32Id;
			
			if ( !CCio::cio_numbytesleft( _pcCio ) ) {
				j2k_read_eoc( _pjJ2k );
				return piImage;
			}
			// Data-bin read.  Need to read a new header.
			if ( static_cast<uint32_t>(CCio::cio_tell( _pcCio ) - i32Position) == jmhHeader.Msg_length ) {
				CJpt::jpt_read_msg_header( cpInfo, _pcCio, &jmhHeader );
				i32Position = CCio::cio_tell( _pcCio );
				if ( jmhHeader.Class_Id != 4 ) {	// 4: Tile data-bin message.
					CJpegImage::opj_image_destroy( piImage );
					CEvent::opj_event_msg( cpInfo, EVT_ERROR, "[JPT-stream]: Expecting tile info!\r\n" );
					return 0;
				}
			}
			
			ui32Id = CCio::cio_read( _pcCio, 2 );
			if ( ui32Id >> 8 != 0xFF ) {
				CJpegImage::opj_image_destroy( piImage );
				CEvent::opj_event_msg( cpInfo, EVT_ERROR, "%.8x: Expected a marker instead of %x.\r\n", CCio::cio_tell( _pcCio ) - 2, ui32Id );
				return 0;
			}
			pdmE = j2k_dec_mstab_lookup( static_cast<int32_t>(ui32Id) );
			if ( !(_pjJ2k->state & pdmE->states) ) {
				CJpegImage::opj_image_destroy( piImage );
				CEvent::opj_event_msg( cpInfo, EVT_ERROR, "%.8x: Unexpected marker %x.\r\n", CCio::cio_tell( _pcCio ) - 2, ui32Id );
				return 0;
			}
			if ( pdmE->handler ) { (*pdmE->handler)( _pjJ2k ); }
			if ( _pjJ2k->state == J2K_STATE_MT ) { break; }
			if ( _pjJ2k->state == J2K_STATE_NEOC ) { break; }
		}
		if ( _pjJ2k->state == J2K_STATE_NEOC ) {
			j2k_read_eoc( _pjJ2k );
		}
		
		if ( _pjJ2k->state != J2K_STATE_MT ) {
			CEvent::opj_event_msg( cpInfo, EVT_WARNING, "Incomplete bitstream.\r\n" );
		}

		return piImage;
	}

	/**
	 * Creates a J2K compression structure.
	 *
	 * \param _cpInfo Codec context info.
	 * \return Returns a handle to a J2K compressor if successful, returns NULL otherwise.
	 */
	CJ2k::opj_j2k_t * LSE_CALL CJ2k::j2k_create_compress( CJpegCommonStructs::opj_common_ptr _cpInfo ) {
		opj_j2k_t * pjJ2k = static_cast<opj_j2k_t *>(CMemLib::CMAlloc( sizeof( opj_j2k_t ) ));
		if ( pjJ2k ) { pjJ2k->cinfo = _cpInfo; }
		return pjJ2k;
	}

	/**
	 * Destroy a J2K compressor handle.
	 *
	 * \param _pjJ2k J2K compressor handle to destroy.
	 */
	void LSE_CALL CJ2k::j2k_destroy_compress( opj_j2k_t * _pjJ2k ) {
		if ( !_pjJ2k ) { return; }
		if ( _pjJ2k->cp != NULL ) {
			opj_cp_t * pcCp = _pjJ2k->cp;

			if ( pcCp->comment ) {
				CMemLib::Free( pcCp->comment );
			}
			if ( pcCp->matrice ) {
				CMemLib::Free( pcCp->matrice );
			}
			for ( int32_t i32TileNo = 0; i32TileNo < pcCp->tw * pcCp->th; i32TileNo++ ) {
				CMemLib::Free( pcCp->tcps[i32TileNo].tccps );
			}
			CMemLib::Free( pcCp->tcps );
			CMemLib::Free( pcCp );
		}

		CMemLib::Free( _pjJ2k );
	}

	/**
	 * Setup the encoder parameters using the current image and using user parameters.
	 * Coding parameters are returned in _pjJ2k->cp.
	 *
	 * \param _pjJ2k J2K compressor handle.
	 * \param _pcpParameters compression parameters.
	 * \param _piImage input filled image.
	 */
	void LSE_CALL CJ2k::j2k_setup_encoder( opj_j2k_t * _pjJ2k, CJpegCommonStructs::opj_cparameters_t * _pcpParameters, CJpegImage::opj_image_t * _piImage ) {
		int32_t I, J, i32TileNo, i32NumPocsTile;
		opj_cp_t * pcCp = NULL;

		if ( !_pjJ2k || !_pcpParameters || !_piImage ) { return; }

		// Create and initialize the coding _pcpParameters structure.
		pcCp = static_cast<opj_cp_t *>(CMemLib::CMAlloc( sizeof( opj_cp_t ) ));

		// Keep a link to pcCp so that we can destroy it later in j2k_destroy_compress.
		_pjJ2k->cp = pcCp;

		// Set default values for pcCp.
		pcCp->tw = 1;
		pcCp->th = 1;

		// Copy user encoding parameters.
		pcCp->cinema = _pcpParameters->cp_cinema;
		pcCp->max_comp_size = _pcpParameters->max_comp_size;
		pcCp->rsiz = _pcpParameters->cp_rsiz;
		pcCp->disto_alloc = _pcpParameters->cp_disto_alloc;
		pcCp->fixed_alloc = _pcpParameters->cp_fixed_alloc;
		pcCp->fixed_quality = _pcpParameters->cp_fixed_quality;

		// Mod of fixed quality.
		if ( _pcpParameters->cp_matrice ) {
			uintptr_t i32ArraySize = static_cast<uint32_t>(_pcpParameters->tcp_numlayers * _pcpParameters->numresolution) * 3 * sizeof( int32_t );
			pcCp->matrice = static_cast<int32_t *>(CMemLib::MAlloc( i32ArraySize ));
			CStd::MemCpy( pcCp->matrice, _pcpParameters->cp_matrice, i32ArraySize );
		}

		// Tiles.
		pcCp->tdx = _pcpParameters->cp_tdx;
		pcCp->tdy = _pcpParameters->cp_tdy;

		// Tile offset.
		pcCp->tx0 = _pcpParameters->cp_tx0;
		pcCp->ty0 = _pcpParameters->cp_ty0;

		// Comment string.
		if ( _pcpParameters->cp_comment ) {
			pcCp->comment = static_cast<char *>(CMemLib::MAlloc( strlen( _pcpParameters->cp_comment ) + 1 ));
			if ( pcCp->comment ) {
				CStd::StrCpy( pcCp->comment, _pcpParameters->cp_comment );
			}
		}

		// Calculate other encoding _pcpParameters.
		if ( _pcpParameters->tile_size_on ) {
			pcCp->tw = CJpegInt::int_ceildiv( _piImage->x1 - pcCp->tx0, pcCp->tdx );
			pcCp->th = CJpegInt::int_ceildiv( _piImage->y1 - pcCp->ty0, pcCp->tdy );
		}
		else {
			pcCp->tdx = _piImage->x1 - pcCp->tx0;
			pcCp->tdy = _piImage->y1 - pcCp->ty0;
		}

		if ( _pcpParameters->tp_on ) {
			pcCp->tp_flag = _pcpParameters->tp_flag;
			pcCp->tp_on = 1;
		}
		
		pcCp->img_size = 0;
		for ( I = 0; I < _piImage->numcomps; I++ ) {
			pcCp->img_size += (_piImage->comps[I].w *_piImage->comps[I].h * _piImage->comps[I].prec);
		}


		// Initialize the mutiple tiles.
		pcCp->tcps = static_cast<opj_tcp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( opj_tcp_t ) ));

		for ( i32TileNo = 0; i32TileNo < pcCp->tw * pcCp->th; i32TileNo++ ) {
			opj_tcp_t * ptTcp = &pcCp->tcps[i32TileNo];
			ptTcp->numlayers = _pcpParameters->tcp_numlayers;
			for ( J = 0; J < ptTcp->numlayers; J++ ) {
				if ( pcCp->cinema ) {
					if ( pcCp->fixed_quality ) {
						ptTcp->distoratio[J] = _pcpParameters->tcp_distoratio[J];
					}
					ptTcp->rates[J] = _pcpParameters->tcp_rates[J];
				}
				else {
					if ( pcCp->fixed_quality ) {	// Add fixed_quality.
						ptTcp->distoratio[J] = _pcpParameters->tcp_distoratio[J];
					}
					else {
						ptTcp->rates[J] = _pcpParameters->tcp_rates[J];
					}
				}
			}
			ptTcp->csty = _pcpParameters->csty;
			ptTcp->prg = _pcpParameters->prog_order;
			ptTcp->mct = _pcpParameters->tcp_mct; 

			i32NumPocsTile = 0;
			ptTcp->POC = 0;
			if ( _pcpParameters->numpocs ) {
				// Initialisation of POC.
				ptTcp->POC = 1;
				for ( I = 0; I < _pcpParameters->numpocs; I++ ) {
					if ( (i32TileNo == _pcpParameters->POC[I].tile - 1) || (_pcpParameters->POC[I].tile == -1) ) {
						CJpegCommonStructs::opj_poc_t * ppTcpPoc = &ptTcp->pocs[i32NumPocsTile];
						ppTcpPoc->resno0 = _pcpParameters->POC[i32NumPocsTile].resno0;
						ppTcpPoc->compno0 = _pcpParameters->POC[i32NumPocsTile].compno0;
						ppTcpPoc->layno1 = _pcpParameters->POC[i32NumPocsTile].layno1;
						ppTcpPoc->resno1 = _pcpParameters->POC[i32NumPocsTile].resno1;
						ppTcpPoc->compno1 = _pcpParameters->POC[i32NumPocsTile].compno1;
						ppTcpPoc->prg1 = _pcpParameters->POC[i32NumPocsTile].prg1;
						ppTcpPoc->tile = _pcpParameters->POC[i32NumPocsTile].tile;
						i32NumPocsTile++;
					}
				}
				ptTcp->numpocs = i32NumPocsTile - 1;
			}
			else { 
				ptTcp->numpocs = 0;
			}

			ptTcp->tccps = static_cast<opj_tccp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_piImage->numcomps) * sizeof( opj_tccp_t ) ));

			for ( I = 0; I < _piImage->numcomps; I++ ) {
				opj_tccp_t * ptTccp = &ptTcp->tccps[I];
				ptTccp->csty = _pcpParameters->csty & 0x01;
				ptTccp->numresolutions = _pcpParameters->numresolution;
				ptTccp->cblkw = CJpegInt::int_floorlog2( _pcpParameters->cblockw_init );
				ptTccp->cblkh = CJpegInt::int_floorlog2( _pcpParameters->cblockh_init );
				ptTccp->cblksty = _pcpParameters->mode;
				ptTccp->qmfbid = _pcpParameters->irreversible ? 0 : 1;
				ptTccp->qntsty = _pcpParameters->irreversible ? J2K_CCP_QNTSTY_SEQNT : J2K_CCP_QNTSTY_NOQNT;
				ptTccp->numgbits = 2;
				if ( I == _pcpParameters->roi_compno ) {
					ptTccp->roishift = _pcpParameters->roi_shift;
				}
				else {
					ptTccp->roishift = 0;
				}

				if ( _pcpParameters->cp_cinema ) {
					// Precinct size for lowest frequency subband = 128.
					ptTccp->prcw[0] = 7;
					ptTccp->prch[0] = 7;
					// Precinct size at all other resolutions = 256.
					for ( J = 1; J < ptTccp->numresolutions; J++ ) {
						ptTccp->prcw[J] = 8;
						ptTccp->prch[J] = 8;
					}
				}
				else {
					if ( _pcpParameters->csty & J2K_CCP_CSTY_PRT ) {
						int32_t i32P = 0;
						for ( J = ptTccp->numresolutions - 1; J >= 0; J-- ) {
							if ( i32P < _pcpParameters->res_spec ) {
								
								if ( _pcpParameters->prcw_init[i32P] < 1 ) {
									ptTccp->prcw[J] = 1;
								}
								else {
									ptTccp->prcw[J] = CJpegInt::int_floorlog2( _pcpParameters->prcw_init[i32P] );
								}
								
								if ( _pcpParameters->prch_init[i32P] < 1 ) {
									ptTccp->prch[J] = 1;
								}
								else {
									ptTccp->prch[J] = CJpegInt::int_floorlog2( _pcpParameters->prch_init[i32P] );
								}

							}
							else {
								int32_t i32ResSpec = _pcpParameters->res_spec;
								int32_t i32SizePrcW = _pcpParameters->prcw_init[i32ResSpec-1] >> (i32P - (i32ResSpec - 1));
								int32_t i32SizePrcH = _pcpParameters->prch_init[i32ResSpec-1] >> (i32P - (i32ResSpec - 1));
								
								if ( i32SizePrcW < 1 ) {
									ptTccp->prcw[J] = 1;
								}
								else {
									ptTccp->prcw[J] = CJpegInt::int_floorlog2( i32SizePrcW );
								}
								
								if ( i32SizePrcH < 1 ) {
									ptTccp->prch[J] = 1;
								}
								else {
									ptTccp->prch[J] = CJpegInt::int_floorlog2( i32SizePrcH );
								}
							}
							i32P++;
						}	// for ( J = ptTccp->numresolutions - 1; J >= 0; J-- ) {
					}
					else {
						for ( J = 0; J < ptTccp->numresolutions; J++ ) {
							ptTccp->prcw[J] = 15;
							ptTccp->prch[J] = 15;
						}
					}
				}

				CDwt::dwt_calc_explicit_stepsizes( ptTccp, _piImage->comps[I].prec );
			}
		}
	}

	/**
	 * Converts an enum type progression order to string type.
	 *
	 * \param _poOrder Undocumented parameter.
	 * \return Undocumented return.
	 */
	const char * LSE_CALL CJ2k::j2k_convert_progression_order( OPJ_PROG_ORDER _poOrder ) {
		typedef struct LSI_PROG_ORDER {
			OPJ_PROG_ORDER	poProg;
			char			szName[8];
		} * LPLSI_PROG_ORDER, * const LPCLSI_PROG_ORDER;

		static const LSI_PROG_ORDER poProgOrderList[] = {
			{ CPRL,								"CPRL" },
			{ LRCP,								"LRCP" },
			{ PCRL,								"PCRL" },
			{ RLCP,								"RLCP" },
			{ RPCL,								"RPCL" },
			{ static_cast<OPJ_PROG_ORDER>(-1),	"" }
		};
		const LSI_PROG_ORDER * ppoPo = poProgOrderList;
		for ( ; ppoPo->poProg != -1; ppoPo++ ) {
			if ( ppoPo->poProg == _poOrder ) { break; }
		}
		return ppoPo->szName;
	}

	/**
	 * Encode an image into a JPEG-2000 codestream.
	 *
	 * \param _pjJ2k J2K compressor handle.
	 * \param _pcCio Output buffer stream.
	 * \param _piImage Image to encode.
	 * \param _pciCStrInfo Codestream information structure if required, NULL otherwise.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL CJ2k::j2k_encode( opj_j2k_t * _pjJ2k, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciCStrInfo ) {
		int32_t i32TileNo, i32CompNo;
		opj_cp_t * pcCp = NULL;

		CTcd::opj_tcd_t * ptTcd = NULL;	// TCD component.

		_pjJ2k->cio = _pcCio;	
		_pjJ2k->image = _piImage;

		pcCp = _pjJ2k->cp;

		/* INDEX >> */
		_pjJ2k->cstr_info = _pciCStrInfo;
		if ( _pciCStrInfo ) {
			_pciCStrInfo->tile = static_cast<CJpegCommonStructs::opj_tile_info_t *>(CMemLib::MAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( CJpegCommonStructs::opj_tile_info_t ) ));
			_pciCStrInfo->image_w = _piImage->x1 - _piImage->x0;
			_pciCStrInfo->image_h = _piImage->y1 - _piImage->y0;
			_pciCStrInfo->prog = (&pcCp->tcps[0])->prg;
			_pciCStrInfo->tw = pcCp->tw;
			_pciCStrInfo->th = pcCp->th;
			_pciCStrInfo->tile_x = pcCp->tdx;
			_pciCStrInfo->tile_y = pcCp->tdy;
			_pciCStrInfo->tile_Ox = pcCp->tx0;
			_pciCStrInfo->tile_Oy = pcCp->ty0;
			_pciCStrInfo->numcomps = _piImage->numcomps;
			_pciCStrInfo->numlayers = ( &pcCp->tcps[0] )->numlayers;
			_pciCStrInfo->numdecompos = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_piImage->numcomps) * sizeof( int32_t ) ));
			for ( i32CompNo = 0; i32CompNo < _piImage->numcomps; i32CompNo++ ) {
				_pciCStrInfo->numdecompos[i32CompNo] = (&pcCp->tcps[0])->tccps->numresolutions - 1;
			}
			_pciCStrInfo->D_max = 0.0;
			_pciCStrInfo->main_head_start = CCio::cio_tell( _pcCio ); // Position of SOC.
			_pciCStrInfo->maxmarknum = 100;
			_pciCStrInfo->marker = static_cast<CJpegCommonStructs::opj_marker_info_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_pciCStrInfo->maxmarknum) * sizeof( CJpegCommonStructs::opj_marker_info_t ) ));
			_pciCStrInfo->marknum = 0;
		}
		/* << INDEX */

		j2k_write_soc( _pjJ2k );
		j2k_write_siz( _pjJ2k );
		j2k_write_cod( _pjJ2k );
		j2k_write_qcd( _pjJ2k );

		if ( pcCp->cinema ) {
			for ( i32CompNo = 1; i32CompNo < _piImage->numcomps; i32CompNo++ ) {
				j2k_write_coc( _pjJ2k, i32CompNo );
				j2k_write_qcc( _pjJ2k, i32CompNo );
			}
		}

		for ( i32CompNo = 0; i32CompNo < _piImage->numcomps; i32CompNo++ ) {
			opj_tcp_t *tcp = &pcCp->tcps[0];
			if ( tcp->tccps[i32CompNo].roishift )
				j2k_write_rgn( _pjJ2k, i32CompNo, 0 );
		}
		if ( pcCp->comment != NULL ) {
			j2k_write_com( _pjJ2k );
		}

		_pjJ2k->totnum_tp = j2k_calculate_tp( pcCp, _piImage, _pjJ2k );
		// TLM Marker.
		if ( pcCp->cinema ) {
			j2k_write_tlm( _pjJ2k );
			if ( pcCp->cinema == CINEMA4K_24 ) {
				j2k_write_poc( _pjJ2k );
			}
		}

		// Uncomment only for testing JPSEC marker writing.
		// j2k_write_sec( _pjJ2k );

		if ( _pciCStrInfo ) {
			_pciCStrInfo->main_head_end = CCio::cio_tell( _pcCio ) - 1;
		}
		/**** Main Header ENDS here. ***/

		// Create the tile encoder.
		ptTcd = CTcd::tcd_create( _pjJ2k->cinfo );

		// Encode each tile.
		for ( i32TileNo = 0; i32TileNo < pcCp->tw * pcCp->th; i32TileNo++ ) {
			int32_t i32PiNo;
			int32_t i32TilePartNo = 0;
			int32_t i32AccPackNum = 0;


			opj_tcp_t * ptTcp = &pcCp->tcps[i32TileNo];
			CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_INFO, "Tile number %d/%d.\r\n", i32TileNo + 1, pcCp->tw * pcCp->th );

			_pjJ2k->curtileno = i32TileNo;
			_pjJ2k->cur_tp_num = 0;
			ptTcd->cur_totnum_tp = _pjJ2k->cur_totnum_tp[_pjJ2k->curtileno];
			// Initialisation before tile encoding.
			if ( i32TileNo == 0 ) {
				CTcd::tcd_malloc_encode( ptTcd, _piImage, pcCp, _pjJ2k->curtileno );
			}
			else {
				CTcd::tcd_init_encode( ptTcd, _piImage, pcCp, _pjJ2k->curtileno );
			}

			if ( _pciCStrInfo ) {
				_pciCStrInfo->tile[_pjJ2k->curtileno].start_pos = CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction;
			}

			for ( i32PiNo = 0; i32PiNo <= ptTcp->numpocs; i32PiNo++ ) {
				int32_t i32TotNumTp;
				ptTcd->cur_pino = i32PiNo;

				// Get number of tile parts.
				i32TotNumTp = j2k_get_num_tp( pcCp, i32PiNo, i32TileNo );
				ptTcd->tp_pos = pcCp->tp_pos;

				for ( i32TilePartNo = 0; i32TilePartNo < i32TotNumTp ; i32TilePartNo++ ) {
					_pjJ2k->tp_num = i32TilePartNo;
					if ( _pciCStrInfo ) {
						_pciCStrInfo->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_start_pos =
							CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction;
					}
					j2k_write_sot( _pjJ2k );

					if ( _pjJ2k->cur_tp_num == 0 && pcCp->cinema == 0 ) {
						for ( i32CompNo = 1; i32CompNo < _piImage->numcomps; i32CompNo++ ) {
							j2k_write_coc( _pjJ2k, i32CompNo );
							j2k_write_qcc( _pjJ2k, i32CompNo );
						}
						if ( pcCp->tcps[i32TileNo].numpocs ) {
							j2k_write_poc( _pjJ2k );
						}
					}

					if ( _pciCStrInfo ) {
						_pciCStrInfo->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_end_header =
							CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction + 1;
					}

					j2k_write_sod( _pjJ2k, ptTcd );

					if ( _pciCStrInfo ) {
						_pciCStrInfo->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_end_pos =
							CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction - 1;
						_pciCStrInfo->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_start_pack =
							i32AccPackNum;
						_pciCStrInfo->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_numpacks =
							_pciCStrInfo->packno - i32AccPackNum;
						i32AccPackNum = _pciCStrInfo->packno;
					}

					_pjJ2k->cur_tp_num++;
				}			
			}
			if ( _pciCStrInfo ) {
				_pciCStrInfo->tile[_pjJ2k->curtileno].end_pos = CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction - 1;
			}

		}

		// Destroy the tile encoder.
		CTcd::tcd_free_encode( ptTcd );
		CTcd::tcd_destroy( ptTcd );

		CMemLib::Free( _pjJ2k->cur_totnum_tp );

		j2k_write_eoc( _pjJ2k );

		if ( _pciCStrInfo ) {
			_pciCStrInfo->codestream_size = CCio::cio_tell( _pcCio ) + _pjJ2k->pos_correction;
			// The following adjustment is done to adjust the codestream size
			//	if SOD is not at 0 in the buffer.  Useful in case of JP2, where
			//	the first bunch of bytes is not in the codestream.
			_pciCStrInfo->codestream_size -= _pciCStrInfo->main_head_start;
		}


		return true;
	}

	/**
	 * Read the lookup table containing all the marker, cStatus and action.
	 *
	 * \param _i32Id Marker value.
	 * \return Undocumented return.
	 */
	CJ2k::opj_dec_mstabent_t * LSE_CALL CJ2k::j2k_dec_mstab_lookup( int32_t _i32Id ) {
		opj_dec_mstabent_t * pdmE;
		for ( pdmE = j2k_dec_mstab; pdmE->id != 0; pdmE++ ) {
			if ( pdmE->id == _i32Id ) {
				break;
			}
		}
		return pdmE;
	}

	/**
	 * Write the SOC marker (Start Of Codestream).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_soc( opj_j2k_t * _pjJ2k ) {
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_SOC), 2 );

#ifdef USE_JPWL

		// Update markers struct.
		j2k_add_marker( _pjJ2k->cstr_info, J2K_MS_SOC, cio_tell( pcCio ) - 2, 2 );

#endif	// #ifdef USE_JPWL
	}

	/**
	 * Read the SOC marker (Start of Codestream).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_soc( opj_j2k_t * _pjJ2k ) {
		_pjJ2k->state = J2K_STATE_MHSIZ;
		// Index.
		if ( _pjJ2k->cstr_info ) {
			_pjJ2k->cstr_info->main_head_start = CCio::cio_tell( _pjJ2k->cio ) - 2;
			_pjJ2k->cstr_info->codestream_size = CCio::cio_numbytesleft( _pjJ2k->cio ) + 2 - _pjJ2k->cstr_info->main_head_start;
		}
	}

	/**
	 * Write the SIZ marker (image and tile size).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_siz( opj_j2k_t * _pjJ2k ) {
		int32_t I;
		int32_t i32LenP, i32Len;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;
		opj_cp_t * pcCp = _pjJ2k->cp;
		
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_SIZ), 2 );		// SIZ.
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );
		CCio::cio_write( pcCio, static_cast<uint32_t>(pcCp->rsiz), 2 );		// Rsiz (capabilities).
		CCio::cio_write( pcCio, piImage->x1, 4 );		// Xsiz.
		CCio::cio_write( pcCio, piImage->y1, 4 );		// Ysiz.
		CCio::cio_write( pcCio, piImage->x0, 4 );		// X0siz.
		CCio::cio_write( pcCio, piImage->y0, 4 );		// Y0siz.
		CCio::cio_write( pcCio, pcCp->tdx, 4 );			// XTsiz.
		CCio::cio_write( pcCio, pcCp->tdy, 4 );			// YTsiz.
		CCio::cio_write( pcCio, pcCp->tx0, 4 );			// XT0siz.
		CCio::cio_write( pcCio, pcCp->ty0, 4 );			// YT0siz.
		CCio::cio_write( pcCio, piImage->numcomps, 2 );	// Csiz.
		for ( I = 0; I < piImage->numcomps; I++ ) {
			CCio::cio_write( pcCio, piImage->comps[I].prec - 1 + (piImage->comps[I].sgnd << 7), 1 );	// Ssiz_i.
			CCio::cio_write( pcCio, piImage->comps[I].dx, 1 );	// XRsiz_i.
			CCio::cio_write( pcCio, piImage->comps[I].dy, 1 );	// YRsiz_i.
		}
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );			// Lsiz.
		CCio::cio_seek( pcCio, i32LenP + i32Len );
	}

	/**
	 * Read the SIZ marker (image and tile size).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_siz( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, I;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;
		opj_cp_t * pcCp = _pjJ2k->cp;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// Lsiz.
		CCio::cio_read( pcCio, 2 );					// Rsiz (capabilities).
		piImage->x1 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));	// Xsiz.
		piImage->y1 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));	// Ysiz.
		piImage->x0 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));	// X0siz.
		piImage->y0 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));	// Y0siz.
		pcCp->tdx = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));		// XTsiz.
		pcCp->tdy = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));		// YTsiz.
		pcCp->tx0 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));		// XT0siz.
		pcCp->ty0 = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));		// YT0siz.
		
		if ( (piImage->x0 < 0) || (piImage->x1 < 0) || (piImage->y0 < 0) || (piImage->y1 < 0) ) {
			CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, 
				"%s: Invalid piImage size (x0:%d, x1:%d, y0:%d, y1:%d).\r\n", 
				piImage->x0, piImage->x1, piImage->y0, piImage->y1 );
			return;
		}
		
		piImage->numcomps = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));	// Csiz.

#ifdef USE_JPWL
		if ( _pjJ2k->cp->correct ) {
			// If JPWL is on, we check whether TX errors have damaged
			//	too much the SIZ parameters.
			if ( !(piImage->x1 * piImage->y1) ) {
				CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, 
					"JPWL: bad piImage size ( %d x %d )\r\n", 
					piImage->x1, piImage->y1 );
				if ( !JPWL_ASSUME || JPWL_ASSUME ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
					return;
				}
			}
			if ( piImage->numcomps != ( ( i32Len - 38 ) / 3 ) ) {
				CEvent::opj_event_msg( _pjJ2k->cinfo, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR, 
					"JPWL: Csiz is %d = > space in SIZ only for %d comps.!!!\r\n", 
					piImage->numcomps, ( ( i32Len - 38 ) / 3 ) );
				if ( !JPWL_ASSUME ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
					return;
				}
				// we try to correct.
				CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- trying to adjust this\r\n" );
				if ( piImage->numcomps < ( ( i32Len - 38 ) / 3 ) ) {
					i32Len = 38 + 3 * piImage->numcomps;
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting Lsiz to %d = > HYPOTHESIS!!!\r\n", 
						i32Len );				
				}
				else {
					piImage->numcomps = ( ( i32Len - 38 ) / 3 );
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting Csiz to %d = > HYPOTHESIS!!!\r\n", 
						piImage->numcomps );				
				}
			}

			// update components number in the jpwl_exp_comps filed.
			pcCp->exp_comps = piImage->numcomps;
		}
#endif	// #ifdef USE_JPWL

		piImage->comps = static_cast<CJpegImage::opj_image_comp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(piImage->numcomps) * sizeof( CJpegImage::opj_image_comp_t ) ));
		for ( I = 0; I < piImage->numcomps; I++ ) {
			int32_t i32Temp, i32W, i32H;
			i32Temp = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));		// Ssiz_i.
			piImage->comps[I].prec = (i32Temp & 0x7f) + 1;
			piImage->comps[I].sgnd = i32Temp >> 7;
			piImage->comps[I].dx = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));	// XRsiz_i.
			piImage->comps[I].dy = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));	// YRsiz_i.
			
#ifdef USE_JPWL
			if ( _pjJ2k->cp->correct ) {
				// If JPWL is on, we check whether TX errors have damaged
				//	too much the SIZ parameters, again.
				if ( !(piImage->comps[I].dx * piImage->comps[I].dy) ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR, 
						"JPWL: bad XRsiz_%d/YRsiz_%d (%d x %d).\r\n", 
						I, I, piImage->comps[I].dx, piImage->comps[I].dy );
					if ( !JPWL_ASSUME ) {
						CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
						return;
					}
					// we try to correct.
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- trying to adjust them\r\n" );
					if ( !piImage->comps[I].dx ) {
						piImage->comps[I].dx = 1;
						CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting XRsiz_%d to %d = > HYPOTHESIS!!!\r\n", 
							I, piImage->comps[I].dx );
					}
					if ( !piImage->comps[I].dy ) {
						piImage->comps[I].dy = 1;
						CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting YRsiz_%d to %d = > HYPOTHESIS!!!\r\n", 
							I, piImage->comps[I].dy );
					}
				}
				
			}
#endif	// #ifdef USE_JPWL

			// TODO: unused ?.
			i32W = CJpegInt::int_ceildiv( piImage->x1 - piImage->x0, piImage->comps[I].dx );
			i32H = CJpegInt::int_ceildiv( piImage->y1 - piImage->y0, piImage->comps[I].dy );

			piImage->comps[I].resno_decoded = 0;	// number of resolution decoded.
			piImage->comps[I].factor = pcCp->reduce; // reducing factor per component.
		}
		
		pcCp->tw = CJpegInt::int_ceildiv( piImage->x1 - pcCp->tx0, pcCp->tdx );
		pcCp->th = CJpegInt::int_ceildiv( piImage->y1 - pcCp->ty0, pcCp->tdy );

#ifdef USE_JPWL
		if ( _pjJ2k->cp->correct ) {
			// If JPWL is on, we check whether TX errors have damaged
			//	too much the SIZ parameters.
			if ( ( pcCp->tw < 1 ) || ( pcCp->th < 1 ) || ( pcCp->tw > pcCp->max_tiles ) || ( pcCp->th > pcCp->max_tiles ) ) {
				CEvent::opj_event_msg( _pjJ2k->cinfo, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR, 
					"JPWL: bad number of tiles ( %d x %d )\r\n", 
					pcCp->tw, pcCp->th );
				if ( !JPWL_ASSUME ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
					return;
				}
				// we try to correct.
				CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- trying to adjust them\r\n" );
				if ( pcCp->tw < 1 ) {
					pcCp->tw = 1;
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting %d tiles in x = > HYPOTHESIS!!!\r\n", 
						pcCp->tw );
				}
				if ( pcCp->tw > pcCp->max_tiles ) {
					pcCp->tw = 1;
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- too large x, increase expectance of %d\r\n"
						"- setting %d tiles in x = > HYPOTHESIS!!!\r\n", 
						pcCp->max_tiles, pcCp->tw );
				}
				if ( pcCp->th < 1 ) {
					pcCp->th = 1;
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- setting %d tiles in y = > HYPOTHESIS!!!\r\n", 
						pcCp->th );
				}
				if ( pcCp->th > pcCp->max_tiles ) {
					pcCp->th = 1;
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- too large y, increase expectance of %d to continue\r\n", 
						"- setting %d tiles in y = > HYPOTHESIS!!!\r\n", 
						pcCp->max_tiles, pcCp->th );
				}
			}
		}
#endif	// #ifdef USE_JPWL

		pcCp->tcps = static_cast<opj_tcp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( opj_tcp_t ) ));
		pcCp->tileno = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( int32_t ) ));
		pcCp->tileno_size = 0;
		
#ifdef USE_JPWL
		if ( _pjJ2k->cp->correct ) {
			if ( !pcCp->tcps ) {
				CEvent::opj_event_msg( _pjJ2k->cinfo, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR, 
					"JPWL: could not alloc tcps field of pcCp\r\n" );
				if ( !JPWL_ASSUME || JPWL_ASSUME ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
					return;
				}
			}
		}
#endif	// #ifdef USE_JPWL

		for ( I = 0; I < pcCp->tw * pcCp->th; I++ ) {
			pcCp->tcps[I].POC = 0;
			pcCp->tcps[I].numpocs = 0;
			pcCp->tcps[I].first = 1;
		}
		
		// Initialization for PPM marker.
		pcCp->ppm = 0;
		pcCp->ppm_data = NULL;
		pcCp->ppm_data_first = NULL;
		pcCp->ppm_previous = 0;
		pcCp->ppm_store = 0;

		_pjJ2k->default_tcp->tccps = static_cast<opj_tccp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(piImage->numcomps) * sizeof( opj_tccp_t ) ));
		for ( I = 0; I < pcCp->tw * pcCp->th; I++ ) {
			pcCp->tcps[I].tccps = static_cast<opj_tccp_t *>(CMemLib::MAlloc( static_cast<uint32_t>(piImage->numcomps) * sizeof( opj_tccp_t ) ));
		}	
		_pjJ2k->tile_data = static_cast<uint8_t **>(CMemLib::CMAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( uint8_t * ) ));
		_pjJ2k->tile_len = static_cast<int32_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( int32_t ) ));
		_pjJ2k->state = J2K_STATE_MH;

		// Index.
		if ( _pjJ2k->cstr_info ) {
			CJpegCommonStructs::opj_codestream_info_t * pciCStrInfo = _pjJ2k->cstr_info;
			pciCStrInfo->image_w = piImage->x1 - piImage->x0;
			pciCStrInfo->image_h = piImage->y1 - piImage->y0;
			pciCStrInfo->numcomps = piImage->numcomps;
			pciCStrInfo->tw = pcCp->tw;
			pciCStrInfo->th = pcCp->th;
			pciCStrInfo->tile_x = pcCp->tdx;	
			pciCStrInfo->tile_y = pcCp->tdy;	
			pciCStrInfo->tile_Ox = pcCp->tx0;	
			pciCStrInfo->tile_Oy = pcCp->ty0;			
			pciCStrInfo->tile = static_cast<CJpegCommonStructs::opj_tile_info_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(pcCp->tw * pcCp->th) * sizeof( CJpegCommonStructs::opj_tile_info_t ) ));
		}
	}

	/**
	 * Write the COM marker (comment).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_com( opj_j2k_t * _pjJ2k ) {
		uint32_t I;
		int32_t i32LenP, i32Len;

		if ( _pjJ2k->cp->comment ) {
			CCio::opj_cio_t * pcCio = _pjJ2k->cio;
			char * pcComment = _pjJ2k->cp->comment;

			CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_COM), 2 );
			i32LenP = CCio::cio_tell( pcCio );
			CCio::cio_skip( pcCio, 2 );
			CCio::cio_write( pcCio, static_cast<uint32_t>(1), 2 );		// General use (IS 8859-15:1999 (Latin) values).
			for ( I = 0; I < CStd::StrLen( pcComment ); I++ ) {
				CCio::cio_write( pcCio, static_cast<uint32_t>(pcComment[I]), 1 );
			}
			i32Len = CCio::cio_tell( pcCio ) - i32LenP;
			CCio::cio_seek( pcCio, i32LenP );
			CCio::cio_write( pcCio, i32Len, 2 );
			CCio::cio_seek( pcCio, i32LenP + i32Len );
		}
	}

	/**
	 * Read the COM marker (comment).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_com( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));
		CCio::cio_skip( pcCio, i32Len - 2 );
	}

	/**
	 * Write the value concerning the specified component in the marker COD and COC.
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information written.
	 */
	void LSE_CALL CJ2k::j2k_write_cox( opj_j2k_t * _pjJ2k, int32_t _i32CompNo ) {
		int32_t I;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = &pcCp->tcps[_pjJ2k->curtileno];
		opj_tccp_t * ptCcp = &ptTcp->tccps[_i32CompNo];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		CCio::cio_write( pcCio, ptCcp->numresolutions - 1, 1 );	// SPcox (D).
		CCio::cio_write( pcCio, ptCcp->cblkw - 2, 1 );			// SPcox (E).
		CCio::cio_write( pcCio, ptCcp->cblkh - 2, 1 );			// SPcox (F).
		CCio::cio_write( pcCio, ptCcp->cblksty, 1 );			// SPcox (G).
		CCio::cio_write( pcCio, ptCcp->qmfbid, 1 );				// SPcox (H).
		
		if ( ptCcp->csty & J2K_CCP_CSTY_PRT ) {
			for ( I = 0; I < ptCcp->numresolutions; I++ ) {
				CCio::cio_write( pcCio, ptCcp->prcw[I] + ( ptCcp->prch[I] << 4 ), 1 );	// SPcox (I_i).
			}
		}
	}

	/**
	 * Read the value concerning the specified component in the marker COD and COC.
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information read.
	 */
	void LSE_CALL CJ2k::j2k_read_cox( opj_j2k_t * _pjJ2k, int32_t _i32CompNo ) {
		int32_t I;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		opj_tccp_t * ptCcp = &ptTcp->tccps[_i32CompNo];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		ptCcp->numresolutions = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ) + 1);	// SPcox (D).

		// If user wants to remove more resolutions than the codestream contains, error.
		if ( pcCp->reduce >= ptCcp->numresolutions ) {
			CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "Error decoding component %d.\nThe number of resolutions to remove is higher than the number "
				"of resolutions of this component\nModify the cp_reduce parameter.\r\n\r\n", _i32CompNo );
			_pjJ2k->state |= J2K_STATE_ERR;
		}

		ptCcp->cblkw = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ) + 2);			// SPcox (E).
		ptCcp->cblkh = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ) + 2);			// SPcox (F).
		ptCcp->cblksty = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));				// SPcox (G).
		ptCcp->qmfbid = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));				// SPcox (H).
		if ( ptCcp->csty & J2K_CP_CSTY_PRT ) {
			for ( I = 0; I < ptCcp->numresolutions; I++ ) {
				int32_t i32Temp = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));	// SPcox (I_i).
				ptCcp->prcw[I] = i32Temp & 0xf;
				ptCcp->prch[I] = i32Temp >> 4;
			}
		}

		if ( _pjJ2k->cstr_info && _i32CompNo == 0 ) {
			for ( I = 0; I < ptCcp->numresolutions; I++ ) {
				if ( ptCcp->csty & J2K_CP_CSTY_PRT ) {
					_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].pdx[I] = ptCcp->prcw[I];
					_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].pdy[I] = ptCcp->prch[I];
				}
				else {
					_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].pdx[I] = 15;
					_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].pdx[I] = 15;
				}
			}
		}
	}

	/**
	 * Write the COD marker (coding style default).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_cod( opj_j2k_t * _pjJ2k ) {
		opj_cp_t * pcCp = NULL;
		opj_tcp_t * ptTcp = NULL;
		int32_t i32LenP, i32Len;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_COD), 2 );				// COD.
		
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );
		
		pcCp = _pjJ2k->cp;
		ptTcp = &pcCp->tcps[_pjJ2k->curtileno];

		CCio::cio_write( pcCio, ptTcp->csty, 1 );				// Scod.
		CCio::cio_write( pcCio, static_cast<uint32_t>(ptTcp->prg), 1 );				// SGcod (A).
		CCio::cio_write( pcCio, ptTcp->numlayers, 2 );			// SGcod (B).
		CCio::cio_write( pcCio, ptTcp->mct, 1 );				// SGcod (C).
		
		j2k_write_cox( _pjJ2k, 0 );
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );					// Lcod.
		CCio::cio_seek( pcCio, i32LenP + i32Len );
	}

	/**
	 * Read the COD marker (coding style default).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_cod( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, I, i32Pos;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));						// Lcod.
		ptTcp->csty = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));					// Scod.
		ptTcp->prg = static_cast<OPJ_PROG_ORDER>(CCio::cio_read( pcCio, 1 ));			// SGcod ( A).
		ptTcp->numlayers = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));			// SGcod ( B).
		ptTcp->mct = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));					// SGcod (C).
		
		i32Pos = CCio::cio_tell( pcCio );
		for ( I = 0; I < piImage->numcomps; I++ ) {
			ptTcp->tccps[I].csty = ptTcp->csty & J2K_CP_CSTY_PRT;
			CCio::cio_seek( pcCio, i32Pos );
			j2k_read_cox( _pjJ2k, I );
		}

		// Index.
		if ( _pjJ2k->cstr_info ) {
			CJpegCommonStructs::opj_codestream_info_t * pciCStrInfo = _pjJ2k->cstr_info;
			pciCStrInfo->prog = ptTcp->prg;
			pciCStrInfo->numlayers = ptTcp->numlayers;
			pciCStrInfo->numdecompos = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(piImage->numcomps) * sizeof( int32_t ) ));
			for ( I = 0; I < piImage->numcomps; I++ ) {
				pciCStrInfo->numdecompos[I] = ptTcp->tccps[I].numresolutions - 1;
			}
		}
	}

	/**
	 * Write the COC marker (coding style component).
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information written.
	 */
	void LSE_CALL CJ2k::j2k_write_coc( opj_j2k_t * _pjJ2k, int32_t _i32CompNo ) {
		int32_t i32LenP, i32Len;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = &pcCp->tcps[_pjJ2k->curtileno];
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_COC), 2 );				// COC.
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );
		CCio::cio_write( pcCio, _i32CompNo, piImage->numcomps <= 256 ? 1 : 2 );		// Ccoc.
		CCio::cio_write( pcCio, ptTcp->tccps[_i32CompNo].csty, 1 );					// Scoc.
		j2k_write_cox( _pjJ2k, _i32CompNo );
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );										// Lcoc.
		CCio::cio_seek( pcCio, i32LenP + i32Len );
	}

	/**
	 * Read the COC marker (coding style component).
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_coc( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32CompNo;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));										// Lcoc.
		i32CompNo = static_cast<int32_t>(CCio::cio_read( pcCio, piImage->numcomps <= 256 ? 1 : 2 ));	// Ccoc.
		ptTcp->tccps[i32CompNo].csty = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));				// Scoc.
		j2k_read_cox( _pjJ2k, i32CompNo );
	}

	/**
	 * Write the value concerning the specified component in the marker QCD and QCC.
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information written.
	 */
	void LSE_CALL CJ2k::j2k_write_qcx( opj_j2k_t * _pjJ2k, int32_t _i32CompNo ) {
		int32_t i32BandNo, i32NumBands;
		int32_t i32Exp, i32Mant;
		
		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = &pcCp->tcps[_pjJ2k->curtileno];
		opj_tccp_t * ptTccp = &ptTcp->tccps[_i32CompNo];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		CCio::cio_write( pcCio, ptTccp->qntsty + (ptTccp->numgbits << 5), 1 );		// Sqcx.
		i32NumBands = ptTccp->qntsty == J2K_CCP_QNTSTY_SIQNT ? 1 : ptTccp->numresolutions * 3 - 2;
		
		for ( i32BandNo = 0; i32BandNo < i32NumBands; i32BandNo++ ) {
			i32Exp = ptTccp->stepsizes[i32BandNo].expn;
			i32Mant = ptTccp->stepsizes[i32BandNo].mant;
			
			if ( ptTccp->qntsty == J2K_CCP_QNTSTY_NOQNT) {
				CCio::cio_write( pcCio, i32Exp << 3, 1 );							// SPqcx_i.
			}
			else {
				CCio::cio_write( pcCio, (i32Exp << 11) + i32Mant, 2 );				// SPqcx_i.
			}
		}
	}

	/**
	 * Read the value concerning the specified component in the marker QCD and QCC.
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concern by the information read.
	 * \param _i32Len Length of the information in the QCX part of the marker QCD/QCC.
	 */
	void LSE_CALL CJ2k::j2k_read_qcx(  opj_j2k_t * _pjJ2k, int32_t _i32CompNo, int32_t _i32Len ) {
		int32_t i32Temp;
		int32_t i32BandNo, i32NumBands;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		opj_tccp_t * ptTccp = &ptTcp->tccps[_i32CompNo];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32Temp = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));										// Sqcx.
		ptTccp->qntsty = i32Temp & 0x1F;
		ptTccp->numgbits = i32Temp >> 5;
		i32NumBands = ( ptTccp->qntsty == J2K_CCP_QNTSTY_SIQNT ) ? 
			1 : ((ptTccp->qntsty == J2K_CCP_QNTSTY_NOQNT) ? _i32Len - 1 : (_i32Len - 1) / 2);

#ifdef USE_JPWL
		if ( _pjJ2k->cp->correct ) {
			// If JPWL is on, we check whether there are too many subbands.
			if ( ( i32NumBands < 0 ) || ( i32NumBands >= J2K_MAXBANDS ) ) {
				CEvent::opj_event_msg( _pjJ2k->cinfo, JPWL_ASSUME ? EVT_WARNING : EVT_ERROR, 
					"JPWL: bad number of subbands in Sqcx ( %d )\r\n", 
					i32NumBands );
				if ( !JPWL_ASSUME ) {
					CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
					return;
				}
				// we try to correct.
				i32NumBands = 1;
				CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "- trying to adjust them\r\n"
					"- setting number of bands to %d = > HYPOTHESIS!!!\r\n", 
					i32NumBands );
			}

		}
#endif	// #ifdef USE_JPWL

		for ( i32BandNo = 0; i32BandNo < i32NumBands; i32BandNo++ ) {
			int32_t i32Exp, i32Mant;
			if ( ptTccp->qntsty == J2K_CCP_QNTSTY_NOQNT ) {
				i32Exp = CCio::cio_read( pcCio, 1 ) >> 3;	// SPqcx_i.
				i32Mant = 0;
			}
			else {
				i32Temp = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// SPqcx_i.
				i32Exp = i32Temp >> 11;
				i32Mant = i32Temp & 0x7ff;
			}
			ptTccp->stepsizes[i32BandNo].expn = i32Exp;
			ptTccp->stepsizes[i32BandNo].mant = i32Mant;
		}

		// Add Antonin: if scalar_derived, compute other step sizes.
		if ( ptTccp->qntsty == J2K_CCP_QNTSTY_SIQNT ) {
			for ( i32BandNo = 1; i32BandNo < J2K_MAXBANDS; i32BandNo++ ) {
				ptTccp->stepsizes[i32BandNo].expn = 
					((ptTccp->stepsizes[0].expn) - ((i32BandNo - 1) / 3) > 0) ? 
						(ptTccp->stepsizes[0].expn) - ((i32BandNo - 1) / 3) : 0;
				ptTccp->stepsizes[i32BandNo].mant = ptTccp->stepsizes[0].mant;
			}
		}
	}

	/**
	 * Write the QCD marker (quantization default).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_qcd(  opj_j2k_t * _pjJ2k ) {
		int32_t i32LenP, i32Len;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_QCD), 2 );	// QCD.
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );
		j2k_write_qcx( _pjJ2k, 0 );
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );			// Lqcd.
		CCio::cio_seek( pcCio, i32LenP + i32Len );
	}

	/**
	 * Read the QCD marker (quantization default).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_qcd(  opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i, i32Pos;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		CJpegImage::opj_image_t * piImage = _pjJ2k->image;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// Lqcd.
		i32Pos = static_cast<int32_t>(CCio::cio_tell( pcCio ));
		for ( i = 0; i < piImage->numcomps; i++ ) {
			CCio::cio_seek( pcCio, i32Pos );
			j2k_read_qcx( _pjJ2k, i, i32Len - 2 );
		}
	}

	/**
	 * Write the QCC marker (quantization component).
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information written.
	 */
	void LSE_CALL CJ2k::j2k_write_qcc(  opj_j2k_t * _pjJ2k, int32_t _i32CompNo ) {
		int32_t i32LenP, i32Len;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_QCC), 2 );											// QCC.
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );
		CCio::cio_write( pcCio, _i32CompNo, _pjJ2k->image->numcomps <= 256 ? 1 : 2 );		// Cqcc.
		j2k_write_qcx( _pjJ2k, _i32CompNo );
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );												// Lqcc.
		CCio::cio_seek( pcCio, i32LenP + i32Len );
	}

	/**
	 * Read the QCC marker (quantization component).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_qcc( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32CompNo;
		int32_t i32NumComp = _pjJ2k->image->numcomps;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));								// Lqcc.
		i32CompNo = static_cast<int32_t>(CCio::cio_read( pcCio, i32NumComp <= 256 ? 1 : 2 ));	// Cqcc.


		j2k_read_qcx( _pjJ2k, i32CompNo, i32Len - 2 - (i32NumComp <= 256 ? 1 : 2) );
	}

	/**
	 * Write the POC marker (progression order change).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_poc( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32NumPChgs, I;

		int32_t i32NumComps = _pjJ2k->image->numcomps;
		
		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = &pcCp->tcps[_pjJ2k->curtileno];
		opj_tccp_t * ptTccp = &ptTcp->tccps[0];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32NumPChgs = 1 + ptTcp->numpocs;
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_POC), 2 );									// POC.
		i32Len = 2 + (5 + 2 * (i32NumComps <= 256 ? 1 : 2)) * i32NumPChgs;
		CCio::cio_write( pcCio, i32Len, 2 );										// Lpoc.
		for ( I = 0; I < i32NumPChgs; I++ ) {
			CJpegCommonStructs::opj_poc_t * poc = &ptTcp->pocs[I];
			CCio::cio_write( pcCio, poc->resno0, 1 );								// RSpoc_i.
			CCio::cio_write( pcCio, poc->compno0, (i32NumComps <= 256 ? 1 : 2) );	// CSpoc_i.
			CCio::cio_write( pcCio, poc->layno1, 2 );								// LYEpoc_i.
			poc->layno1 = CJpegInt::int_min( poc->layno1, ptTcp->numlayers );
			CCio::cio_write( pcCio, poc->resno1, 1 );								// REpoc_i.
			poc->resno1 = CJpegInt::int_min( poc->resno1, ptTccp->numresolutions );
			CCio::cio_write( pcCio, poc->compno1, (i32NumComps <= 256 ? 1 : 2) );	// CEpoc_i.
			poc->compno1 = CJpegInt::int_min( poc->compno1, i32NumComps );
			CCio::cio_write( pcCio, static_cast<uint32_t>(poc->prg), 1 );									// Ppoc_i.
		}
	}

	/**
	 * Read the POC marker (progression order change).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_poc( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32NumPChgs, I, i32OldPoc;

		int32_t i32NumComps = _pjJ2k->image->numcomps;
		
		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32OldPoc = ptTcp->POC ? ptTcp->numpocs + 1 : 0;
		ptTcp->POC = 1;
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));														// Lpoc.
		i32NumPChgs = (i32Len - 2) / (5 + 2 * (i32NumComps <= 256 ? 1 : 2));
		
		for ( I = i32OldPoc; I < i32NumPChgs + i32OldPoc; I++ ) {
			CJpegCommonStructs::opj_poc_t * poc;
			poc = &ptTcp->pocs[I];
			poc->resno0 = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));												// RSpoc_i.
			poc->compno0 = static_cast<int32_t>(CCio::cio_read( pcCio, i32NumComps <= 256 ? 1 : 2 ));						// CSpoc_i.
			poc->layno1 = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));												// LYEpoc_i.
			poc->resno1 = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));												// REpoc_i.
			poc->compno1 = CJpegInt::int_min( 
				static_cast<int32_t>(CCio::cio_read( pcCio, i32NumComps <= 256 ? 1 : 2 )), i32NumComps );	// CEpoc_i.
			poc->prg = static_cast<OPJ_PROG_ORDER>(CCio::cio_read( pcCio, 1 ));								// Ppoc_i.
		}
		
		ptTcp->numpocs = i32NumPChgs + i32OldPoc - 1;
	}

	/**
	 * Read the CRG marker (component registration).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_crg( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, I, i32XCrgI, i32YCrgI;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		int32_t i32NumComps = _pjJ2k->image->numcomps;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));			// Lcrg.
		for ( I = 0; I < i32NumComps; I++ ) {
			i32XCrgI = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// Xcrg_i.
			i32YCrgI = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// Ycrg_i.
		}
	}

	/**
	 * Read the TLM marker (tile-part lengths).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_tlm( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32Ztlm, i32Stlm, i32St, i32Sp, i32TileTlm, I;
		int64_t i32TtlmI, i32PtlmI;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));		// Ltlm.
		i32Ztlm = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));		// i32Ztlm.
		i32Stlm = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));		// i32Stlm.
		i32St = ((i32Stlm >> 4) & 0x01) + ((i32Stlm >> 4) & 0x02);
		i32Sp = (i32Stlm >> 6) & 0x01;
		i32TileTlm = (i32Len - 4) / ((i32Sp + 1) * 2 + i32St);
		for ( I = 0; I < i32TileTlm; I++ ) {
			i32TtlmI = static_cast<int32_t>(CCio::cio_read( pcCio, static_cast<uint32_t>(i32St) ));			// i32TtlmI.
			i32PtlmI = static_cast<int32_t>(CCio::cio_read( pcCio, i32Sp ? 4 : 2 ));	// i32PtlmI.
		}
	}

	/**
	 * Read the PLM marker (packet length, main header).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_plm( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, I, i32Zplm, i32Nplm, i32Add, i32PacketLen = 0;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));						// Lplm.
		i32Zplm = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));						// i32Zplm.
		i32Len -= 3;
		while ( i32Len > 0 ) {
			i32Nplm = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));					// i32Nplm.
			i32Len -= 4;
			for ( I = i32Nplm; I > 0; I-- ) {
				i32Add = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));
				i32Len--;
				i32PacketLen = (i32PacketLen << 7) + i32Add;		// Iplm_ij.
				if ( (i32Add & 0x80) == 0 ) {
					// New packet.
					i32PacketLen = 0;
				}
				if ( i32Len <= 0 ) { break; }
			}
		}
	}

	/**
	 * Read the PLT marker (packet length, tile-part header).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_plt( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, I, i32Zplt, i32PacketLen = 0, i32Add;
	
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));				// Lplt.
		i32Zplt = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));				// i32Zplt.
		for ( I = i32Len - 3; I > 0; I-- ) {
			i32Add = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));
			i32PacketLen = (i32PacketLen << 7) + i32Add;	// Iplt_i.
			if ( ( i32Add & 0x80 ) == 0 ) {
				// New packet.
				i32PacketLen = 0;
			}
		}
	}

	/**
	 * Read the PPM marker (packet packet headers, main header).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_ppm( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32Zppm, I, J;
		int32_t i32Nppm;

		opj_cp_t * pcCp = _pjJ2k->cp;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		
		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));
		pcCp->ppm = 1;
		
		i32Zppm = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));			// i32Zppm.
		i32Len -= 3;
		while ( i32Len > 0 ) {
			if ( pcCp->ppm_previous == 0 ) {
				i32Nppm = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));	// i32Nppm.
				i32Len -= 4;
			}
			else {
				i32Nppm = pcCp->ppm_previous;
			}
			J = pcCp->ppm_store;
			if ( i32Zppm == 0 ) {	// First PPM marker.
				pcCp->ppm_data = static_cast<uint8_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Nppm) * sizeof( uint8_t ) ));
				pcCp->ppm_data_first = pcCp->ppm_data;
				pcCp->ppm_len = i32Nppm;
			}
			else {			// NON-first PPM marker.

				pcCp->ppm_data = static_cast<uint8_t *>(CMemLib::MReAlloc( pcCp->ppm_data, static_cast<uint32_t>(i32Nppm + pcCp->ppm_store) * sizeof( uint8_t ) ));

#ifdef USE_JPWL
				// This memory allocation check could be done even in non-JPWL cases.
				if ( pcCp->correct ) {
					if ( !pcCp->ppm_data ) {
						CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, 
							"JPWL: failed memory allocation during PPM marker parsing ( pos. %x )\r\n", 
							cio_tell( pcCio ) );
						if ( !JPWL_ASSUME || JPWL_ASSUME ) {
							CMemLib::Free( pcCp->ppm_data );
							CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_ERROR, "JPWL: giving up\r\n" );
							return;
						}
					}
				}
#endif	// #ifdef USE_JPWL

				pcCp->ppm_data_first = pcCp->ppm_data;
				pcCp->ppm_len = i32Nppm + pcCp->ppm_store;
			}
			for ( I = i32Nppm; I > 0; I-- ) {	// Read packet header.
				pcCp->ppm_data[J] = static_cast<uint8_t>(CCio::cio_read( pcCio, 1 ));
				J++;
				i32Len--;
				if ( i32Len == 0 ) {
					break;			// Case of non-finished packet header in present marker but finished in next one.
				}
			}
			pcCp->ppm_previous = I - 1;
			pcCp->ppm_store = J;
		}
	}

	/**
	 * Read the PPT marker (packet packet headers, tile-part header).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_ppt( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32Zppt, I, J = 0;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = pcCp->tcps + _pjJ2k->curtileno;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));
		i32Zppt = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));
		ptTcp->ppt = 1;
		if ( i32Zppt == 0 ) {		// First PPT marker.
			ptTcp->ppt_data = static_cast<uint8_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Len - 3) * sizeof( uint8_t ) ));
			ptTcp->ppt_data_first = ptTcp->ppt_data;
			ptTcp->ppt_store = 0;
			ptTcp->ppt_len = i32Len - 3;
		}
		else {						// NON-first PPT marker.
			ptTcp->ppt_data = static_cast<uint8_t *>(CMemLib::MReAlloc( ptTcp->ppt_data, static_cast<uint32_t>(i32Len - 3 + ptTcp->ppt_store) * sizeof( uint8_t ) ));
			ptTcp->ppt_data_first = ptTcp->ppt_data;
			ptTcp->ppt_len = i32Len - 3 + ptTcp->ppt_store;
		}
		J = ptTcp->ppt_store;
		for ( I = i32Len - 3; I > 0; I-- ) {
			ptTcp->ppt_data[J] = static_cast<uint8_t>(CCio::cio_read( pcCio, 1 ));
			J++;
		}
		ptTcp->ppt_store = J;
	}

	/**
	 * Write the TLM marker (Mainheader).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_tlm( opj_j2k_t * _pjJ2k ) {
		int32_t i32LenP;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		_pjJ2k->tlm_start = CCio::cio_tell( pcCio );
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_TLM), 2 );			// TLM.
		i32LenP = 4 + (5 * _pjJ2k->totnum_tp);
		CCio::cio_write( pcCio, i32LenP, 2 );				// Ltlm.
		CCio::cio_write( pcCio, static_cast<uint32_t>(0), 1 );						// i32Ztlm = 0.
		CCio::cio_write( pcCio, static_cast<uint32_t>(80UL), 1 );					// i32Stlm i32St = 1 (8bits-255 tiles max), i32Sp = 1 (Ptlm = 32bits).
		CCio::cio_skip( pcCio, 5 * _pjJ2k->totnum_tp );
	}

	/**
	 * Write the SOT marker (start of tile-part).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_sot( opj_j2k_t * _pjJ2k ) {
		int32_t i32LenP, i32Len;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		_pjJ2k->sot_start = CCio::cio_tell( pcCio );
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_SOT), 2 );									// SOT.
		i32LenP = CCio::cio_tell( pcCio );
		CCio::cio_skip( pcCio, 2 );													// Lsot (further).
		CCio::cio_write( pcCio, _pjJ2k->curtileno, 2 );								// Isot.
		CCio::cio_skip( pcCio, 4 );													// Psot (further in j2k_write_sod).
		CCio::cio_write( pcCio, _pjJ2k->cur_tp_num , 1 );							// TPsot.
		CCio::cio_write( pcCio, _pjJ2k->cur_totnum_tp[_pjJ2k->curtileno], 1 );		// TNsot.
		i32Len = CCio::cio_tell( pcCio ) - i32LenP;
		CCio::cio_seek( pcCio, i32LenP );
		CCio::cio_write( pcCio, i32Len, 2 );										// Lsot.
		CCio::cio_seek( pcCio, i32LenP + i32Len );

#ifdef USE_JPWL
		// Update markers struct.
		j2k_add_marker( _pjJ2k->cstr_info, J2K_MS_SOT, _pjJ2k->sot_start, i32Len + 2 );
#endif	// #ifdef USE_JPWL
	}

	/**
	 * Read the SOT marker (start of tile-part).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_sot( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32TileNo, i32TotLen, i32PartNo, i32NumParts, I;
		opj_tcp_t * ptTcp = NULL;
		char cStatus = 0;

		opj_cp_t * pcCp = _pjJ2k->cp;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));
		i32TileNo = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));

		
		if ( pcCp->tileno_size == 0 ) {
			pcCp->tileno[pcCp->tileno_size] = i32TileNo;
			pcCp->tileno_size++;
		}
		else {
			I = 0;
			while ( I < pcCp->tileno_size && cStatus == 0 ) {
				cStatus = pcCp->tileno[I] == i32TileNo ? 1 : 0;
				I++;
			}
			if ( cStatus == 0 ) {
				pcCp->tileno[pcCp->tileno_size] = i32TileNo;
				pcCp->tileno_size++;
			}
		}
		
		i32TotLen = static_cast<int32_t>(CCio::cio_read( pcCio, 4 ));

		if ( !i32TotLen ) {
			i32TotLen = CCio::cio_numbytesleft( pcCio ) + 8;
		}
		
		i32PartNo = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));
		i32NumParts = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));
		
		_pjJ2k->curtileno = i32TileNo;
		_pjJ2k->cur_tp_num = i32PartNo;
		_pjJ2k->eot = CCio::cio_getbp( pcCio ) - 12 + i32TotLen;
		_pjJ2k->state = J2K_STATE_TPH;
		ptTcp = &pcCp->tcps[_pjJ2k->curtileno];

		// Index.
		if ( _pjJ2k->cstr_info ) {
			if ( ptTcp->first ) {
				if ( i32TileNo == 0 ) {
					_pjJ2k->cstr_info->main_head_end = CCio::cio_tell( pcCio ) - 13;
				}
				_pjJ2k->cstr_info->tile[i32TileNo].tileno = i32TileNo;
				_pjJ2k->cstr_info->tile[i32TileNo].start_pos = CCio::cio_tell( pcCio ) - 12;
				_pjJ2k->cstr_info->tile[i32TileNo].end_pos = _pjJ2k->cstr_info->tile[i32TileNo].start_pos + i32TotLen - 1;				
				_pjJ2k->cstr_info->tile[i32TileNo].num_tps = i32NumParts;
				if ( i32NumParts ) {
					_pjJ2k->cstr_info->tile[i32TileNo].tp = static_cast<CJpegCommonStructs::opj_tp_info_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32NumParts) * sizeof( CJpegCommonStructs::opj_tp_info_t ) ));
				}
				else {
					_pjJ2k->cstr_info->tile[i32TileNo].tp = static_cast<CJpegCommonStructs::opj_tp_info_t *>(CMemLib::MAlloc( 10 * sizeof( CJpegCommonStructs::opj_tp_info_t ) )); // Fixme (10)
				}
			}
			else {
				_pjJ2k->cstr_info->tile[i32TileNo].end_pos += i32TotLen;
			}		
			_pjJ2k->cstr_info->tile[i32TileNo].tp[i32PartNo].tp_start_pos = CCio::cio_tell( pcCio ) - 12;
			_pjJ2k->cstr_info->tile[i32TileNo].tp[i32PartNo].tp_end_pos = 
				_pjJ2k->cstr_info->tile[i32TileNo].tp[i32PartNo].tp_start_pos + i32TotLen - 1;
		}
		
		if ( ptTcp->first == 1 ) {		
			// Initialization PPT.
			opj_tccp_t * ptTemp = ptTcp->tccps;
			CStd::MemCpy( ptTcp, _pjJ2k->default_tcp, sizeof( opj_tcp_t ) );
			ptTcp->ppt = 0;
			ptTcp->ppt_data = NULL;
			ptTcp->ppt_data_first = NULL;
			ptTcp->tccps = ptTemp;

			for ( I = 0; I < _pjJ2k->image->numcomps; I++ ) {
				ptTcp->tccps[I] = _pjJ2k->default_tcp->tccps[I];
			}
			pcCp->tcps[_pjJ2k->curtileno].first = 0;
		}
	}

	/**
	 * Write the SOD marker (start of data).
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32TileCoder Pointer to a TCD handle.
	 */
	void LSE_CALL CJ2k::j2k_write_sod( opj_j2k_t * _pjJ2k, void * _i32TileCoder ) {
		int32_t i32L, i32LayNo;
		int32_t i32TotLen;
		opj_tcp_t * ptTcp = NULL;
		CJpegCommonStructs::opj_codestream_info_t * pciCStrInfo = NULL;
		
		CTcd::opj_tcd_t * ptTcd = static_cast<CTcd::opj_tcd_t *>(_i32TileCoder);
		opj_cp_t * pcCp = _pjJ2k->cp;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;

		ptTcd->tp_num = _pjJ2k->tp_num;
		ptTcd->cur_tp_num = _pjJ2k->cur_tp_num;
		
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_SOD), 2 );
		if ( _pjJ2k->curtileno == 0 ) {
			_pjJ2k->sod_start = CCio::cio_tell( pcCio ) + _pjJ2k->pos_correction;
		}

		pciCStrInfo = _pjJ2k->cstr_info;
		if ( pciCStrInfo ) {
			if ( !_pjJ2k->cur_tp_num ) {
				pciCStrInfo->tile[_pjJ2k->curtileno].end_header = CCio::cio_tell( pcCio ) + _pjJ2k->pos_correction - 1;
				_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].tileno = _pjJ2k->curtileno;
			}
			else {
				if ( pciCStrInfo->tile[_pjJ2k->curtileno].packet[pciCStrInfo->packno-1].end_pos < CCio::cio_tell( pcCio ) ) {
					pciCStrInfo->tile[_pjJ2k->curtileno].packet[pciCStrInfo->packno].start_pos = CCio::cio_tell( pcCio );
				}
			}
#ifdef USE_JPWL
			// Update markers struct.
			j2k_add_marker( _pjJ2k->cstr_info, J2K_MS_SOD, _pjJ2k->sod_start, 2 );
#endif	// #ifdef USE_JPWL
		}
		
		ptTcp = &pcCp->tcps[_pjJ2k->curtileno];
		for ( i32LayNo = 0; i32LayNo < ptTcp->numlayers; i32LayNo++ ) {
			if ( ptTcp->rates[i32LayNo] > (_pjJ2k->sod_start / (pcCp->th * pcCp->tw)) ) {
				ptTcp->rates[i32LayNo] -= (_pjJ2k->sod_start / (pcCp->th * pcCp->tw) );
			}
			else if ( ptTcp->rates[i32LayNo] ) {
				ptTcp->rates[i32LayNo] = 1;
			}
		}
		if ( _pjJ2k->cur_tp_num == 0 ) {
			ptTcd->tcd_image->tiles->packno = 0;
			if ( pciCStrInfo ) {
				pciCStrInfo->packno = 0;
			}
		}
		
		i32L = CTcd::tcd_encode_tile( ptTcd, _pjJ2k->curtileno, CCio::cio_getbp( pcCio ), CCio::cio_numbytesleft( pcCio ) - 2, pciCStrInfo );
		
		// Writing Psot in SOT marker.
		i32TotLen = CCio::cio_tell( pcCio ) + i32L - _pjJ2k->sot_start;
		CCio::cio_seek( pcCio, _pjJ2k->sot_start + 6 );
		CCio::cio_write( pcCio, i32TotLen, 4 );
		CCio::cio_seek( pcCio, _pjJ2k->sot_start + i32TotLen );
		// Writing Ttlm and Ptlm in TLM marker.
		if ( pcCp->cinema ) {
			CCio::cio_seek( pcCio, _pjJ2k->tlm_start + 6 + (5 * _pjJ2k->cur_tp_num) );
			CCio::cio_write( pcCio, _pjJ2k->curtileno, 1 );
			CCio::cio_write( pcCio, i32TotLen, 4 );
		}
		CCio::cio_seek( pcCio, _pjJ2k->sot_start + i32TotLen );
	}

	/**
	 * Read the SOD marker (start of data).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_sod( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32Truncate = 0, I;
		uint8_t * pui8Data = NULL, * pui8DataPtr = NULL;

		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		int32_t curtileno = _pjJ2k->curtileno;

		// Index.
		if ( _pjJ2k->cstr_info ) {
			_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].tp[_pjJ2k->cur_tp_num].tp_end_header = 
				CCio::cio_tell( pcCio ) + _pjJ2k->pos_correction - 1;
			if ( _pjJ2k->cur_tp_num == 0 ) {
				_pjJ2k->cstr_info->tile[_pjJ2k->curtileno].end_header = CCio::cio_tell( pcCio ) + _pjJ2k->pos_correction - 1;
			}
			_pjJ2k->cstr_info->packno = 0;
		}
		
		i32Len = CJpegInt::int_min( static_cast<int32_t>(_pjJ2k->eot - CCio::cio_getbp( pcCio )), CCio::cio_numbytesleft( pcCio ) + 1 );

		if ( i32Len == CCio::cio_numbytesleft( pcCio ) + 1 ) {
			i32Truncate = 1;		// Case of a truncate codestream.
		}	

		pui8Data = _pjJ2k->tile_data[curtileno];
		pui8Data = static_cast<uint8_t *>(CMemLib::MReAlloc( pui8Data, static_cast<uint32_t>(_pjJ2k->tile_len[curtileno] + i32Len) * sizeof( uint8_t ) ));

		pui8DataPtr = pui8Data + _pjJ2k->tile_len[curtileno];
		for ( I = 0; I < i32Len; I++ ) {
			pui8DataPtr[I] = static_cast<uint8_t>(CCio::cio_read( pcCio, 1 ));
		}

		_pjJ2k->tile_len[curtileno] += i32Len;
		_pjJ2k->tile_data[curtileno] = pui8Data;
		
		if ( !i32Truncate ) {
			_pjJ2k->state = J2K_STATE_TPHSOT;
		}
		else {
			_pjJ2k->state = J2K_STATE_NEOC;
		}
		_pjJ2k->cur_tp_num++;
	}

	/**
	 * Write the RGN marker (region-of-interest).
	 *
	 * \param _pjJ2k J2K handle.
	 * \param _i32CompNo Number of the component concerned by the information written.
	 * \param _i32TileNo Number of the tile concerned by the information written.
	 */
	void LSE_CALL CJ2k::j2k_write_rgn( opj_j2k_t * _pjJ2k, int32_t _i32CompNo, int32_t _i32TileNo ) {
		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = &pcCp->tcps[_i32TileNo];
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		int32_t i32NumComps = _pjJ2k->image->numcomps;
		
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_RGN), 2 );							// RGN.
		CCio::cio_write( pcCio, static_cast<uint32_t>(i32NumComps <= 256 ? 5 : 6), 2 );		// Lrgn.
		CCio::cio_write( pcCio, _i32CompNo, i32NumComps <= 256 ? 1 : 2 );						// Crgn.
		CCio::cio_write( pcCio, static_cast<uint32_t>(0), 1 );								// Srgn.
		CCio::cio_write( pcCio, ptTcp->tccps[_i32CompNo].roishift, 1 );							// SPrgn.
	}

	/**
	 * Read the RGN marker (region-of-interest).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_rgn( opj_j2k_t * _pjJ2k ) {
		int32_t i32Len, i32CompNo, i32Roisty;

		opj_cp_t * pcCp = _pjJ2k->cp;
		opj_tcp_t * ptTcp = _pjJ2k->state == J2K_STATE_TPH ? &pcCp->tcps[_pjJ2k->curtileno] : _pjJ2k->default_tcp;
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		int32_t i32NumComps = _pjJ2k->image->numcomps;

		i32Len = static_cast<int32_t>(CCio::cio_read( pcCio, 2 ));								// Lrgn.
		i32CompNo = static_cast<int32_t>(CCio::cio_read( pcCio, i32NumComps <= 256 ? 1 : 2 ));	// Crgn.
		i32Roisty = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));								// Srgn.


		ptTcp->tccps[i32CompNo].roishift = static_cast<int32_t>(CCio::cio_read( pcCio, 1 ));		// SPrgn.
	}

	/**
	 * Write the EOC marker (end of codestream).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_write_eoc( opj_j2k_t * _pjJ2k ) {
		CCio::opj_cio_t * pcCio = _pjJ2k->cio;
		CCio::cio_write( pcCio, static_cast<uint32_t>(J2K_MS_EOC), 2 );
	}

	/**
	 * Read the EOC marker (end of codestream).
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_eoc( opj_j2k_t * _pjJ2k ) {
		int32_t I, i32TileNo;
		LSBOOL bSuccess;

		// If packets should be decoded.
		if ( _pjJ2k->cp->limit_decoding != DECODE_ALL_BUT_PACKETS ) {
			CTcd::opj_tcd_t * ptTcd = CTcd::tcd_create( _pjJ2k->cinfo );
			CTcd::tcd_malloc_decode( ptTcd, _pjJ2k->image, _pjJ2k->cp );
			for ( I = 0; I < _pjJ2k->cp->tileno_size; I++ ) {
				CTcd::tcd_malloc_decode_tile( ptTcd, _pjJ2k->image, _pjJ2k->cp, I, _pjJ2k->cstr_info );
				i32TileNo = _pjJ2k->cp->tileno[I];
				bSuccess = CTcd::tcd_decode_tile( ptTcd, _pjJ2k->tile_data[i32TileNo], _pjJ2k->tile_len[i32TileNo], i32TileNo, _pjJ2k->cstr_info );
				CMemLib::Free( _pjJ2k->tile_data[i32TileNo] );
				_pjJ2k->tile_data[i32TileNo] = NULL;
				CTcd::tcd_free_decode_tile( ptTcd, I );
				if ( bSuccess == false ) {
					_pjJ2k->state |= J2K_STATE_ERR;
					break;
				}
			}
			CTcd::tcd_free_decode( ptTcd );
			CTcd::tcd_destroy( ptTcd );
		}
		// If packets should not be decoded.
		else {
			for ( I = 0; I < _pjJ2k->cp->tileno_size; I++ ) {
				i32TileNo = _pjJ2k->cp->tileno[I];
				CMemLib::Free( _pjJ2k->tile_data[i32TileNo] );
				_pjJ2k->tile_data[i32TileNo] = NULL;
			}
		}	
		if ( _pjJ2k->state & J2K_STATE_ERR ) {
			_pjJ2k->state = J2K_STATE_MT + J2K_STATE_ERR;
		}
		else {
			_pjJ2k->state = J2K_STATE_MT;
		}
	}

	/**
	 * Read an unknown marker.
	 *
	 * \param _pjJ2k J2K handle.
	 */
	void LSE_CALL CJ2k::j2k_read_unk( opj_j2k_t * _pjJ2k ) {
		CEvent::opj_event_msg( _pjJ2k->cinfo, EVT_WARNING, "Unknown marker.\r\n" );
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pcCp Undocumented function from the OpenJpeg Library.
	 * \param _i32PiNo Undocumented function from the OpenJpeg Library.
	 * \param _i32TileNo Undocumented function from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CJ2k::j2k_get_num_tp( opj_cp_t * _pcCp, int32_t _i32PiNo, int32_t _i32TileNo ) {
		const char * pcProg;
		int32_t i32TpNum = 1, i32TpEnd = 0;
		opj_tcp_t * ptTcp = &_pcCp->tcps[_i32TileNo];
		pcProg = j2k_convert_progression_order( ptTcp->prg );
		
		if ( _pcCp->tp_on == 1 ) {
			for ( int32_t I = 0; I < 4; I++ ) {
				if ( i32TpEnd != 1 ) {
					if ( _pcCp->tp_flag == pcProg[I] ) {
						i32TpEnd = 1;_pcCp->tp_pos=I;
					}
					switch( pcProg[I] ) {
						case 'C' : {
							i32TpNum =  i32TpNum * ptTcp->pocs[_i32PiNo].compE;
							break;
						}
						case 'R' : {
							i32TpNum =  i32TpNum * ptTcp->pocs[_i32PiNo].resE;
							break;
						}
						case 'P' : {
							i32TpNum =  i32TpNum * ptTcp->pocs[_i32PiNo].prcE;
							break;
						}
						case 'L' : {
							i32TpNum =  i32TpNum * ptTcp->pocs[_i32PiNo].layE;
							break;
						}
					}
				}
			}
		}
		else {
			i32TpNum = 1;
		}
		return i32TpNum;
	}

	/**
	 * Memory allocation for TLM marker.
	 *
	 * \param _pcCp Undocumented function from the OpenJpeg Library.
	 * \param _piImage Undocumented function from the OpenJpeg Library.
	 * \param _pjJ2k Undocumented function from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CJ2k::j2k_calculate_tp( opj_cp_t * _pcCp, CJpegImage::opj_image_t * _piImage, opj_j2k_t * _pjJ2k ) {
		int32_t i32PiNo, i32TileNo, i32TotalTp = 0;
		_pjJ2k->cur_totnum_tp = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_pcCp->tw * _pcCp->th) * sizeof( int32_t ) ));
		for ( i32TileNo = 0; i32TileNo < _pcCp->tw * _pcCp->th; i32TileNo++ ) {
			int32_t i32CurTotalTp = 0;
			opj_tcp_t * ptTcp = &_pcCp->tcps[i32TileNo];
			for ( i32PiNo = 0; i32PiNo <= ptTcp->numpocs; i32PiNo++ ) {
				int32_t i32TpNum = 0;
				CPi::opj_pi_iterator_t * ppiPi = CPi::pi_initialise_encode( _piImage, _pcCp, i32TileNo, FINAL_PASS );
				if ( !ppiPi ) { return -1;}
				i32TpNum = j2k_get_num_tp( _pcCp, i32PiNo, i32TileNo );
				i32TotalTp = i32TotalTp + i32TpNum;
				i32CurTotalTp = i32CurTotalTp + i32TpNum;
				CPi::pi_destroy( ppiPi, _pcCp, i32TileNo );
			}
			_pjJ2k->cur_totnum_tp[i32TileNo] = i32CurTotalTp;

			if ( _pjJ2k->cstr_info ) {
				_pjJ2k->cstr_info->tile[i32TileNo].num_tps = i32CurTotalTp;
				_pjJ2k->cstr_info->tile[i32TileNo].tp = static_cast<CJpegCommonStructs::opj_tp_info_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32CurTotalTp) * sizeof( CJpegCommonStructs::opj_tp_info_t ) ));
			}
		}
		return i32TotalTp;
	}

}	// namespace lsi
