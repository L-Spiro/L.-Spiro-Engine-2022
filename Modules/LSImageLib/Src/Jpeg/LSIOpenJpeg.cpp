/*
 * Copyright (c) 2005, HervEDrolon, FreeImage Team
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

#include "LSIOpenJpeg.h"
#include "LSIJ2k.h"
#include "LSIJp2.h"


namespace lsi {

	// == Functions.
	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _cpInfo Undocumented parameter from the OpenJpeg Library.
	 * \param _pemEventManager Undocumented parameter from the OpenJpeg Library.
	 * \param _pvContext Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	CJpegCommonStructs::opj_event_mgr_t * LSE_CALL COpenJpeg::opj_set_event_mgr( CJpegCommonStructs::opj_common_ptr _cpInfo, CJpegCommonStructs::opj_event_mgr_t * _pemEventManager, void * _pvContext ) {
		if ( _cpInfo ) {
			CJpegCommonStructs::opj_event_mgr_t * _pemPrev = _cpInfo->event_mgr;
			_cpInfo->event_mgr = _pemEventManager;
			_cpInfo->client_data = _pvContext;
			return _pemPrev;
		}

		return NULL;
	}

	/**
	 * Creates a J2K/JPT/JP2 decompression structure.
	 *
	 * \param _cfFormat Decoder to select.
	 * \return Returns a handle to a decompressor if successful, returns NULL otherwise.
	 */
	CJpegCommonStructs::opj_dinfo_t * LSE_CALL COpenJpeg::opj_create_decompress( OPJ_CODEC_FORMAT _cfFormat ) {
		CJpegCommonStructs::opj_dinfo_t * pdInfo = static_cast<CJpegCommonStructs::opj_dinfo_t *>(CMemLib::CMAlloc( sizeof( CJpegCommonStructs::opj_dinfo_t ) ));
		if ( !pdInfo ) return NULL;
		pdInfo->is_decompressor = true;
		switch( _cfFormat ) {
			case CODEC_J2K : {}
			case CODEC_JPT : {
				// Get a J2K decoder handle.
				pdInfo->j2k_handle = static_cast<void *>(CJ2k::j2k_create_decompress( reinterpret_cast<CJpegCommonStructs::opj_common_ptr>(pdInfo) ));
				if ( !pdInfo->j2k_handle ) {
					CMemLib::Free( pdInfo );
					return NULL;
				}
				break;
			}
			case CODEC_JP2 : {
				// Get a JP2 decoder handle.
				pdInfo->jp2_handle = static_cast<void *>(CJp2::jp2_create_decompress( reinterpret_cast<CJpegCommonStructs::opj_common_ptr>(pdInfo) ));
				if ( !pdInfo->jp2_handle ) {
					CMemLib::Free( pdInfo );
					return NULL;
				}
				break;
			}
			case CODEC_UNKNOWN : {}
			default : {
				CMemLib::Free( pdInfo );
				return NULL;
			}
		}

		pdInfo->codec_format = _cfFormat;

		return pdInfo;
	}

	/**
	 * Destroy a decompressor handle
	 *
	 * \param _pdInfo decompressor handle to destroy
	 */
	void LSE_CALL COpenJpeg::opj_destroy_decompress( CJpegCommonStructs::opj_dinfo_t * _pdInfo ) {
		if ( _pdInfo ) {
			// Destroy the codec.
			switch( _pdInfo->codec_format ) {
				case CODEC_J2K : {}
				case CODEC_JPT : {
					CJ2k::j2k_destroy_decompress( static_cast<CJ2k::opj_j2k_t *>(_pdInfo->j2k_handle) );
					break;
				}
				case CODEC_JP2 : {
					CJp2::jp2_destroy_decompress( static_cast<CJp2::opj_jp2_t *>(_pdInfo->jp2_handle) );
					break;
				}
				case CODEC_UNKNOWN : {}
				default : {
					break;
				}
			}
			// Destroy the decompressor.
			CMemLib::Free( _pdInfo );
		}
	}

	/**
	 * Sets decoding parameters to default values.
	 *
	 * \param _pdParameters Decompression parameters.
	 */
	void LSE_CALL COpenJpeg::opj_set_default_decoder_parameters( CJpegCommonStructs::opj_dparameters_t * _pdParameters ) {
		if ( _pdParameters ) {
			CStd::MemSet( _pdParameters, 0, sizeof( CJpegCommonStructs::opj_dparameters_t ) );
			// Default decoding parameters.
			_pdParameters->cp_layer = 0;
			_pdParameters->cp_reduce = 0;
			_pdParameters->cp_limit_decoding = NO_LIMITATION;

			_pdParameters->decod_format = -1;
			_pdParameters->cod_format = -1;
#ifdef USE_JPWL
			_pdParameters->jpwl_correct = false;
			_pdParameters->jpwl_exp_comps = JPWL_EXPECTED_COMPONENTS;
			_pdParameters->jpwl_max_tiles = JPWL_MAXIMUM_TILES;
#endif	//#ifdef USE_JPWL
		}
	}

	/**
	 * Setup the decoder decoding parameters using user parameters.
	 * Decoding parameters are returned in j2k->cp.
	 *
	 * \param _pdInfo decompressor handle.
	 * \param _pdParameters decompression parameters.
	 */
	void LSE_CALL COpenJpeg::opj_setup_decoder( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CJpegCommonStructs::opj_dparameters_t * _pdParameters ) {
		if ( _pdInfo && _pdParameters ) {
			switch( _pdInfo->codec_format ) {
				case CODEC_J2K : {}
				case CODEC_JPT : {
					CJ2k::j2k_setup_decoder( static_cast<CJ2k::opj_j2k_t *>(_pdInfo->j2k_handle), _pdParameters );
					break;
				}
				case CODEC_JP2 : {
					CJp2::jp2_setup_decoder( static_cast<CJp2::opj_jp2_t *>(_pdInfo->jp2_handle), _pdParameters );
					break;
				}
				case CODEC_UNKNOWN : {}
				default : { break; }
			}
		}
	}

	/**
	 * Decode an image from a JPEG-2000 codestream.
	 *
	 * \param _pdInfo decompressor handle.
	 * \param _pcCio Input buffer stream.
	 * \return Returns a decoded image if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL COpenJpeg::opj_decode( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CCio::opj_cio_t * _pcCio ) {
		return opj_decode_with_info( _pdInfo, _pcCio, NULL );
	}

	/**
	 * Decode an image from a JPEG-2000 codestream and extract the codestream information.
	 *
	 * \param _pdInfo decompressor handle.
	 * \param _pcCio Input buffer stream.
	 * \param _pciInfo Codestream information structure if needed afterwards, NULL otherwise.
	 * \return Returns a decoded image if successful, returns NULL otherwise.
	 */
	CJpegImage::opj_image_t * LSE_CALL COpenJpeg::opj_decode_with_info( CJpegCommonStructs::opj_dinfo_t * _pdInfo, CCio::opj_cio_t * _pcCio, CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		if ( _pdInfo && _pcCio ) {
			switch( _pdInfo->codec_format ) {
				case CODEC_J2K : {
					return CJ2k::j2k_decode( static_cast<CJ2k::opj_j2k_t *>(_pdInfo->j2k_handle), _pcCio, _pciInfo );
				}
				case CODEC_JPT : {
					return CJ2k::j2k_decode_jpt_stream( static_cast<CJ2k::opj_j2k_t *>(_pdInfo->j2k_handle), _pcCio, _pciInfo );
				}
				case CODEC_JP2 : {
					return CJp2::jp2_decode( static_cast<CJp2::opj_jp2_t *>(_pdInfo->jp2_handle), _pcCio, _pciInfo );
				}
				case CODEC_UNKNOWN : {}
				default : { break; }
			}
		}
		return NULL;
	}

	/**
	 * Creates a J2K/JP2 compression structure.
	 *
	 * \param _cfFormat Coder to select.
	 * \return Returns a handle to a compressor if successful, returns NULL otherwise.
	 */
	CJpegCommonStructs::opj_cinfo_t * LSE_CALL COpenJpeg::opj_create_compress( OPJ_CODEC_FORMAT _cfFormat ) {
		CJpegCommonStructs::opj_cinfo_t * pcInfo = static_cast<CJpegCommonStructs::opj_cinfo_t *>(CMemLib::CMAlloc( sizeof( CJpegCommonStructs::opj_cinfo_t ) ));
		if ( !pcInfo ) { return NULL; }
		pcInfo->is_decompressor = false;
		switch( _cfFormat ) {
			case CODEC_J2K : {
				// Get a J2K coder handle.
				pcInfo->j2k_handle = static_cast<void *>(CJ2k::j2k_create_compress( reinterpret_cast<CJpegCommonStructs::opj_common_ptr>(pcInfo) ));
				if ( !pcInfo->j2k_handle ) {
					CMemLib::Free( pcInfo );
					return NULL;
				}
				break;
			}
			case CODEC_JP2 : {
				// Get a JP2 coder handle.
				pcInfo->jp2_handle = static_cast<void *>(CJp2::jp2_create_compress( reinterpret_cast<CJpegCommonStructs::opj_common_ptr>(pcInfo) ));
				if ( !pcInfo->jp2_handle ) {
					CMemLib::Free( pcInfo );
					return NULL;
				}
				break;
			}
			case CODEC_JPT : {}
			case CODEC_UNKNOWN : {}
			default : {
				CMemLib::Free( pcInfo );
				return NULL;
			}
		}

		pcInfo->codec_format = _cfFormat;

		return pcInfo;
	}

	/**
	 * Destroy a compressor handle.
	 *
	 * \param _cpInfo compressor handle to destroy.
	 */
	void LSE_CALL COpenJpeg::opj_destroy_compress( CJpegCommonStructs::opj_cinfo_t * _cpInfo ) {
		if ( _cpInfo ) {
			// Destroy the codec.
			switch( _cpInfo->codec_format ) {
				case CODEC_J2K : {
					CJ2k::j2k_destroy_compress( static_cast<CJ2k::opj_j2k_t *>(_cpInfo->j2k_handle) );
					break;
				}
				case CODEC_JP2 : {
					CJp2::jp2_destroy_compress( static_cast<CJp2::opj_jp2_t *>(_cpInfo->jp2_handle) );
					break;
				}
				case CODEC_JPT : {}
				case CODEC_UNKNOWN : {}
				default : { break; }
			}
			// Destroy the decompressor.
			CMemLib::Free( _cpInfo );
		}
	}

	/**
	 * Sets encoding parameters to default values, that means: 
	 * <ul>
	 * <li>Lossless
	 * <li>1 tile
	 * <li>Size of precinct : 2^15 x 2^15 (means 1 precinct)
	 * <li>Size of code-block : 64 x 64
	 * <li>Number of resolutions: 6
	 * <li>No SOP marker in the codestream
	 * <li>No EPH marker in the codestream
	 * <li>No sub-sampling in x or y direction
	 * <li>No mode switch activated
	 * <li>Progression order: LRCP
	 * <li>No index file
	 * <li>No ROI upshifted
	 * <li>No offset of the origin of the image
	 * <li>No offset of the origin of the tiles
	 * <li>Reversible DWT 5-3
	 * </ul>
	 *
	 * \param _pdParameters Compression parameters.
	 */
	void LSE_CALL COpenJpeg::opj_set_default_encoder_parameters( CJpegCommonStructs::opj_cparameters_t * _pdParameters ) {
		if ( _pdParameters ) {
			CStd::MemSet( _pdParameters, 0, sizeof( CJpegCommonStructs::opj_cparameters_t ) );
			// default coding _pdParameters.
			_pdParameters->cp_cinema = OFF; 
			_pdParameters->max_comp_size = 0;
			_pdParameters->numresolution = 6;
			_pdParameters->cp_rsiz = STD_RSIZ;
			_pdParameters->cblockw_init = 64;
			_pdParameters->cblockh_init = 64;
			_pdParameters->prog_order = LRCP;
			_pdParameters->roi_compno = -1;			// no ROI.
			_pdParameters->subsampling_dx = 1;
			_pdParameters->subsampling_dy = 1;
			_pdParameters->tp_on = 0;
			_pdParameters->decod_format = -1;
			_pdParameters->cod_format = -1;
			_pdParameters->tcp_rates[0] = 0;   
			_pdParameters->tcp_numlayers = 0;
			_pdParameters->cp_disto_alloc = 0;
			_pdParameters->cp_fixed_alloc = 0;
			_pdParameters->cp_fixed_quality = 0;

#ifdef USE_JPWL
			_pdParameters->jpwl_epc_on = false;
			_pdParameters->jpwl_hprot_MH = -1;					// -1 means unassigned.
			for ( int32_t I = 0; I < JPWL_MAX_NO_TILESPECS; I++ ) {
				_pdParameters->jpwl_hprot_TPH_tileno[I] = -1;	// Unassigned.
				_pdParameters->jpwl_hprot_TPH[I] = 0;			// Absent.
			}
			for ( int32_t I = 0; I < JPWL_MAX_NO_PACKSPECS; I++ ) {
				_pdParameters->jpwl_pprot_tileno[I] = -1;		// Unassigned.
				_pdParameters->jpwl_pprot_packno[I] = -1;		// Unassigned.
				_pdParameters->jpwl_pprot[I] = 0;				// Absent.
			}
			_pdParameters->jpwl_sens_size = 0;					// 0 means no ESD.
			_pdParameters->jpwl_sens_addr = 0;					// 0 means auto.
			_pdParameters->jpwl_sens_range = 0;					// 0 means packet.
			_pdParameters->jpwl_sens_MH = -1;					// -1 means unassigned.

			for ( int32_t I = 0; I < JPWL_MAX_NO_TILESPECS; I++ ) {
				_pdParameters->jpwl_sens_TPH_tileno[I] = -1;	// Unassigned.
				_pdParameters->jpwl_sens_TPH[I] = -1;			// Absent.
			}
#endif	// #ifdef USE_JPWL
		}
	}

	/**
	 * Setup the encoder parameters using the current image and using user parameters.
	 *
	 * \param _cpInfo Compressor handle.
	 * \param _pdParameters Compression parameters.
	 * \param _piImage Input filled image.
	 */
	void LSE_CALL COpenJpeg::opj_setup_encoder( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CJpegCommonStructs::opj_cparameters_t * _pdParameters, CJpegImage::opj_image_t * _piImage ) {
		if ( _cpInfo && _pdParameters && _piImage ) {
			switch( _cpInfo->codec_format ) {
				case CODEC_J2K : {
					CJ2k::j2k_setup_encoder( static_cast<CJ2k::opj_j2k_t *>(_cpInfo->j2k_handle), _pdParameters, _piImage );
					break;
				}
				case CODEC_JP2 : {
					CJp2::jp2_setup_encoder( static_cast<CJp2::opj_jp2_t *>(_cpInfo->jp2_handle), _pdParameters, _piImage );
					break;
				}
				case CODEC_JPT : {}
				case CODEC_UNKNOWN : {}
				default : { break; }
			}
		}
	}

	/**
	 * Encode an image into a JPEG-2000 codestream.
	 *
	 * \param _cpInfo compressor handle.
	 * \param _pcCio Output buffer stream.
	 * \param _piImage Image to encode.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL COpenJpeg::opj_encode( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage ) {
		return opj_encode_with_info( _cpInfo, _pcCio, _piImage, NULL );
	}

	/**
	 * Encode an image into a JPEG-2000 codestream and extract the codestream information.
	 *
	 * \param _cpInfo compressor handle.
	 * \param _pcCio Output buffer stream.
	 * \param _piImage Image to encode.
	 * \param _pciInfo Codestream information structure if needed afterwards, NULL otherwise.
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CALL COpenJpeg::opj_encode_with_info( CJpegCommonStructs::opj_cinfo_t * _cpInfo, CCio::opj_cio_t * _pcCio, CJpegImage::opj_image_t * _piImage, CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		if ( _cpInfo && _pcCio && _piImage ) {
			switch( _cpInfo->codec_format ) {
				case CODEC_J2K : {
					return CJ2k::j2k_encode( static_cast<CJ2k::opj_j2k_t *>(_cpInfo->j2k_handle), _pcCio, _piImage, _pciInfo );
				}
				case CODEC_JP2 : {
					return CJp2::jp2_encode( static_cast<CJp2::opj_jp2_t *>(_cpInfo->jp2_handle), _pcCio, _piImage, _pciInfo );
				}
				case CODEC_JPT : {}
				case CODEC_UNKNOWN : {}
				default : { break; }
			}
		}
		return false;
	}

	/**
	 * Destroy Codestream information after compression or decompression.
	 *
	 * \param _pciInfo Codestream information structure.
	 */
	void LSE_CALL COpenJpeg::opj_destroy_cstr_info( CJpegCommonStructs::opj_codestream_info_t * _pciInfo ) {
		if ( _pciInfo ) {
			for ( int32_t i32TileNo = 0; i32TileNo < _pciInfo->tw * _pciInfo->th; i32TileNo++ ) {
				CJpegCommonStructs::opj_tile_info_t * ptiInfo = &_pciInfo->tile[i32TileNo];
				CMemLib::Free( ptiInfo->thresh );
				CMemLib::Free( ptiInfo->packet );
				CMemLib::Free( ptiInfo->tp );
			}
			CMemLib::Free( _pciInfo->tile );
			CMemLib::Free( _pciInfo->marker );
			CMemLib::Free( _pciInfo->numdecompos );
		}
	}

}	// namespace lsi
