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

#include "LSITcd.h"
#include "LSIDwt.h"
#include "LSIEvent.h"
#include "LSIJpegInt.h"
#include "LSIMct.h"
#include "LSIT1.h"
#include "LSIT2.h"
#include "Time/LSSTDTime.h"


namespace lsi {

	// == Functions.
	/**
	 * Creates a new TCD handle.
	 *
	 * \param _cpCInfo Codec context info.
	 * \return Returns a new TCD handle if successful returns NULL otherwise.
	 */
	CTcd::opj_tcd_t * LSE_CALL CTcd::tcd_create( CJpegCommonStructs::opj_common_ptr _cpCInfo ) {
		// Create the ptTcd structure.
		opj_tcd_t * ptTcd = static_cast<opj_tcd_t *>(CMemLib::MAlloc( sizeof( opj_tcd_t ) ));
		if ( !ptTcd ) { return NULL; }
		ptTcd->cinfo = _cpCInfo;
		ptTcd->tcd_image = static_cast<opj_tcd_image_t *>(CMemLib::MAlloc( sizeof( opj_tcd_image_t ) ));
		if ( !ptTcd->tcd_image ) {
			CMemLib::Free( ptTcd );
			return NULL;
		}

		return ptTcd;
	}

	/**
	 * Destroy a previously created TCD handle.
	 *
	 * \param _ptTcd TCD handle to destroy.
	 */
	void LSE_CALL CTcd::tcd_destroy( opj_tcd_t * _ptTcd ) {
		if ( _ptTcd ) {
			CMemLib::Free( _ptTcd->tcd_image );
			CMemLib::Free( _ptTcd );
		}
	}

	/**
	 * Initialize the tile coder (allocate the memory).
	 *
	 * \param _ptTcd TCD handle.
	 * \param _piImage Raw _piImage.
	 * \param _pcCp Coding parameters.
	 * \param _i32CurtileNo Number that identifies the tile that will be encoded.
	 */
	void LSE_CALL CTcd::tcd_malloc_encode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32CurtileNo ) {
		int32_t i32TileNo, i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CblkNo;

		_ptTcd->image = _piImage;
		_ptTcd->cp = _pcCp;
		_ptTcd->tcd_image->tw = _pcCp->tw;
		_ptTcd->tcd_image->th = _pcCp->th;
		_ptTcd->tcd_image->tiles = static_cast<opj_tcd_tile_t *>(CMemLib::MAlloc( sizeof( opj_tcd_tile_t ) ));
		
		for ( i32TileNo = 0; i32TileNo < 1; i32TileNo++ ) {
			CJ2k::opj_tcp_t * ptTcp = &_pcCp->tcps[_i32CurtileNo];
			int32_t J;

			// cfr p59 ISO/IEC FDIS15444-1: 2000 (18 august 2000).
			int32_t i32P = _i32CurtileNo % _pcCp->tw;
			int32_t i32Q = _i32CurtileNo / _pcCp->tw;

			// opj_tcd_tile_t * pttTile = &_ptTcd->tcd_image->tiles[i32TileNo];
			opj_tcd_tile_t * pttTile = _ptTcd->tcd_image->tiles;

			// 4 borders of the tile rescale on the image if necessary.
			pttTile->x0 = CJpegInt::int_max( _pcCp->tx0 + i32P * _pcCp->tdx, _piImage->x0 );
			pttTile->y0 = CJpegInt::int_max( _pcCp->ty0 + i32Q * _pcCp->tdy, _piImage->y0 );
			pttTile->x1 = CJpegInt::int_min( _pcCp->tx0 + (i32P + 1) * _pcCp->tdx, _piImage->x1 );
			pttTile->y1 = CJpegInt::int_min( _pcCp->ty0 + (i32Q + 1) * _pcCp->tdy, _piImage->y1 );
			pttTile->numcomps = _piImage->numcomps;
			// pttTile->PPT = piImage->PPT;

			// Modification of the RATE.
			for ( J = 0; J < ptTcp->numlayers; J++ ) {
				ptTcp->rates[J] = ptTcp->rates[J] ? 
					_pcCp->tp_on ?
						(static_cast<float>(pttTile->numcomps * (pttTile->x1 - pttTile->x0) * (pttTile->y1 - pttTile->y0) * _piImage->comps[0].prec) /
						(ptTcp->rates[J] * 8.0f * _piImage->comps[0].dx * _piImage->comps[0].dy)) - (((_ptTcd->cur_totnum_tp - 1) * 14) / ptTcp->numlayers) :
						(static_cast<float>(pttTile->numcomps * (pttTile->x1 - pttTile->x0) * (pttTile->y1 - pttTile->y0) * _piImage->comps[0].prec)) /
						(ptTcp->rates[J] * 8 * _piImage->comps[0].dx * _piImage->comps[0].dy) : 0;

				if ( ptTcp->rates[J] ) {
					if ( J && ptTcp->rates[J] < ptTcp->rates[J - 1] + 10 ) {
						ptTcp->rates[J] = ptTcp->rates[J - 1] + 20;
					}
					else {
						if ( !J && ptTcp->rates[J] < 30 ) {
							ptTcp->rates[J] = 30;
						}
					}
					
					if ( J == ( ptTcp->numlayers-1 ) ) {
						ptTcp->rates[J] = ptTcp->rates[J]- 2;
					}
				}
			}
			// End modification of the RATE.
			
			pttTile->comps = static_cast<opj_tcd_tilecomp_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_piImage->numcomps) * sizeof( opj_tcd_tilecomp_t ) ));
			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				CJ2k::opj_tccp_t * ptTccp = &ptTcp->tccps[i32CompNo];

				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];

				// Border of each tile component (global).
				pttTileC->x0 = CJpegInt::int_ceildiv( pttTile->x0, _piImage->comps[i32CompNo].dx );
				pttTileC->y0 = CJpegInt::int_ceildiv( pttTile->y0, _piImage->comps[i32CompNo].dy );
				pttTileC->x1 = CJpegInt::int_ceildiv( pttTile->x1, _piImage->comps[i32CompNo].dx );
				pttTileC->y1 = CJpegInt::int_ceildiv( pttTile->y1, _piImage->comps[i32CompNo].dy );
				
				pttTileC->data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>((pttTileC->x1 - pttTileC->x0) * (pttTileC->y1 - pttTileC->y0)) * sizeof( int32_t ) ));
				pttTileC->numresolutions = ptTccp->numresolutions;

				pttTileC->resolutions = static_cast<opj_tcd_resolution_t *>(CMemLib::MAlloc( static_cast<uint32_t>(pttTileC->numresolutions) * sizeof( opj_tcd_resolution_t ) ));
				
				for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
					int32_t i32PdX, i32PdY;
					int32_t i32LevelNo = pttTileC->numresolutions - 1 - i32ResNo;
					int32_t i32TlprcXStart, i32TlprcYStart, i32BrprcXEnd, i32BrprcYEnd;
					int32_t i32TlcbgXStart, i32TlcbgYStart, i32BrcbgXEnd, i32BrcbgYEnd;
					int32_t i32CbgWidthExpn, i32CbgHeightExpn;
					int32_t i32CblkWidthExpn, i32CblkHeightExpn;

					opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];
					
					// Border for each resolution level (global).
					ptrRes->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
					ptrRes->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
					ptrRes->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
					ptrRes->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );
					
					ptrRes->numbands = i32ResNo == 0 ? 1 : 3;
					// p. 35, table A-23, ISO/IEC FDIS154444-1: 2000 (18 august 2000).
					if ( ptTccp->csty & J2K_CCP_CSTY_PRT ) {
						i32PdX = ptTccp->prcw[i32ResNo];
						i32PdY = ptTccp->prch[i32ResNo];
					}
					else {
						i32PdX = 15;
						i32PdY = 15;
					}
					// p. 64, B.6, ISO/IEC FDIS15444-1: 2000 (18 august 2000).
					i32TlprcXStart = CJpegInt::int_floordivpow2( ptrRes->x0, i32PdX ) << i32PdX;
					i32TlprcYStart = CJpegInt::int_floordivpow2( ptrRes->y0, i32PdY ) << i32PdY;
					
					i32BrprcXEnd = CJpegInt::int_ceildivpow2( ptrRes->x1, i32PdX ) << i32PdX;
					i32BrprcYEnd = CJpegInt::int_ceildivpow2( ptrRes->y1, i32PdY ) << i32PdY;
					
					ptrRes->pw = (i32BrprcXEnd - i32TlprcXStart) >> i32PdX;
					ptrRes->ph = (i32BrprcYEnd - i32TlprcYStart) >> i32PdY;
					
					if ( i32ResNo == 0 ) {
						i32TlcbgXStart = i32TlprcXStart;
						i32TlcbgYStart = i32TlprcYStart;
						i32BrcbgXEnd = i32BrprcXEnd;
						i32BrcbgYEnd = i32BrprcYEnd;
						i32CbgWidthExpn = i32PdX;
						i32CbgHeightExpn = i32PdY;
					}
					else {
						i32TlcbgXStart = CJpegInt::int_ceildivpow2( i32TlprcXStart, 1 );
						i32TlcbgYStart = CJpegInt::int_ceildivpow2( i32TlprcYStart, 1 );
						i32BrcbgXEnd = CJpegInt::int_ceildivpow2( i32BrprcXEnd, 1 );
						i32BrcbgYEnd = CJpegInt::int_ceildivpow2( i32BrprcYEnd, 1 );
						i32CbgWidthExpn = i32PdX - 1;
						i32CbgHeightExpn = i32PdY - 1;
					}
					
					i32CblkWidthExpn = CJpegInt::int_min( ptTccp->cblkw, i32CbgWidthExpn );
					i32CblkHeightExpn = CJpegInt::int_min( ptTccp->cblkh, i32CbgHeightExpn );
					
					for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
						int32_t i32X0b, i32Y0b, I;
						int32_t i32Gain, i32NumBps;
						CJ2k::opj_stepsize_t * psStepSize = NULL;

						opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];

						ptbBand->bandno = i32ResNo == 0 ? 0 : i32BandNo + 1;
						i32X0b = (ptbBand->bandno == 1) || (ptbBand->bandno == 3) ? 1 : 0;
						i32Y0b = (ptbBand->bandno == 2) || (ptbBand->bandno == 3) ? 1 : 0;
						
						if ( ptbBand->bandno == 0 ) {
							// Band border (global).
							ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
							ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
							ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
							ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );
						}
						else {
							// Band border (global).
							ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
							ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
							ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
							ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
						}
						
						psStepSize = &ptTccp->stepsizes[i32ResNo == 0 ? 0 : 3 * (i32ResNo - 1) + i32BandNo + 1];
						i32Gain = ptTccp->qmfbid == 0 ? CDwt::dwt_getgain_real( ptbBand->bandno ) : CDwt::dwt_getgain( ptbBand->bandno );					
						i32NumBps = _piImage->comps[i32CompNo].prec + i32Gain;
						
						ptbBand->stepsize = static_cast<float>((1.0 + psStepSize->mant / 2048.0 ) * std::pow( 2.0, static_cast<double>(i32NumBps - psStepSize->expn) ) );
						ptbBand->numbps = psStepSize->expn + ptTccp->numgbits - 1;	// WHY -1?
						
						ptbBand->precincts = static_cast<opj_tcd_precinct_t *>(CMemLib::MAlloc( static_cast<uint32_t>(3 * ptrRes->pw * ptrRes->ph) * sizeof( opj_tcd_precinct_t ) ));
						
						for ( I = 0; I < ptrRes->pw * ptrRes->ph * 3; I++ ) {
							ptbBand->precincts[I].imsbtree = NULL;
							ptbBand->precincts[I].incltree = NULL;
						}
						
						for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
							int32_t i32TlcBlkXStart, i32TlcBlkYStart, i32BrcBlkXEnd, i32BrcBlkYEnd;

							int32_t i32CbgXStart = i32TlcbgXStart + (i32PrecNo % ptrRes->pw) * (1 << i32CbgWidthExpn);
							int32_t i32CbgYStart = i32TlcbgYStart + (i32PrecNo / ptrRes->pw) * (1 << i32CbgHeightExpn);
							int32_t i32CbgXEnd = i32CbgXStart + (1 << i32CbgWidthExpn);
							int32_t i32CbgYEnd = i32CbgYStart + (1 << i32CbgHeightExpn);

							opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];

							// Precinct size (global).
							ptpPrc->x0 = CJpegInt::int_max( i32CbgXStart, ptbBand->x0 );
							ptpPrc->y0 = CJpegInt::int_max( i32CbgYStart, ptbBand->y0 );
							ptpPrc->x1 = CJpegInt::int_min( i32CbgXEnd, ptbBand->x1 );
							ptpPrc->y1 = CJpegInt::int_min( i32CbgYEnd, ptbBand->y1 );

							i32TlcBlkXStart = CJpegInt::int_floordivpow2( ptpPrc->x0, i32CblkWidthExpn ) << i32CblkWidthExpn;
							i32TlcBlkYStart = CJpegInt::int_floordivpow2( ptpPrc->y0, i32CblkHeightExpn ) << i32CblkHeightExpn;
							i32BrcBlkXEnd = CJpegInt::int_ceildivpow2( ptpPrc->x1, i32CblkWidthExpn ) << i32CblkWidthExpn;
							i32BrcBlkYEnd = CJpegInt::int_ceildivpow2( ptpPrc->y1, i32CblkHeightExpn ) << i32CblkHeightExpn;
							ptpPrc->cw = ( i32BrcBlkXEnd - i32TlcBlkXStart ) >> i32CblkWidthExpn;
							ptpPrc->ch = ( i32BrcBlkYEnd - i32TlcBlkYStart ) >> i32CblkHeightExpn;

							ptpPrc->cblks.enc = static_cast<opj_tcd_cblk_enc_t *>(CMemLib::CMAlloc( static_cast<uint32_t>((ptpPrc->cw * ptpPrc->ch)) * sizeof( opj_tcd_cblk_enc_t ) ));
							ptpPrc->incltree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );
							ptpPrc->imsbtree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );
							
							for ( i32CblkNo = 0; i32CblkNo < ptpPrc->cw * ptpPrc->ch; i32CblkNo++ ) {
								int32_t i32CodeBlockXStart = i32TlcBlkXStart + (i32CblkNo % ptpPrc->cw) * (1 << i32CblkWidthExpn);
								int32_t i32CodeBlockYStart = i32TlcBlkYStart + (i32CblkNo / ptpPrc->cw) * (1 << i32CblkHeightExpn);
								int32_t i32CodeBlockXEnd = i32CodeBlockXStart + (1 << i32CblkWidthExpn);
								int32_t i32CodeBlockYEnd = i32CodeBlockYStart + (1 << i32CblkHeightExpn);
								
								opj_tcd_cblk_enc_t * ptceCodeBlock = &ptpPrc->cblks.enc[i32CblkNo];

								// Code-block size (global).
								ptceCodeBlock->x0 = CJpegInt::int_max( i32CodeBlockXStart, ptpPrc->x0 );
								ptceCodeBlock->y0 = CJpegInt::int_max( i32CodeBlockYStart, ptpPrc->y0 );
								ptceCodeBlock->x1 = CJpegInt::int_min( i32CodeBlockXEnd, ptpPrc->x1 );
								ptceCodeBlock->y1 = CJpegInt::int_min( i32CodeBlockYEnd, ptpPrc->y1 );
								ptceCodeBlock->data = static_cast<uint8_t *>(CMemLib::CMAlloc( (8192 + 2) * sizeof( uint8_t ) ));
								// FIXME: mqc_init_enc and mqc_byteout underrun the buffer if we don't do this. Why?
								ptceCodeBlock->data += 2;
								ptceCodeBlock->layers = static_cast<opj_tcd_layer_t *>(CMemLib::CMAlloc( 100 * sizeof( opj_tcd_layer_t ) ));
								ptceCodeBlock->passes = static_cast<opj_tcd_pass_t *>(CMemLib::CMAlloc( 100 * sizeof( opj_tcd_pass_t ) ));
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Free the memory allocated for encoding.
	 *
	 * \param _ptTcd TCD handle.
	 */
	void LSE_CALL CTcd::tcd_free_encode( opj_tcd_t * _ptTcd ) {
		int32_t i32TileNo, i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;

		for ( i32TileNo = 0; i32TileNo < 1; i32TileNo++ ) {
			opj_tcd_tile_t * pttTile = _ptTcd->tcd_image->tiles;

			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];

				for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
					opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];

					for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
						opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];

						for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
							opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];

							if ( ptpPrc->incltree != NULL ) {
								CTgt::tgt_destroy( ptpPrc->incltree );
								ptpPrc->incltree = NULL;
							}
							if ( ptpPrc->imsbtree != NULL ) {
								CTgt::tgt_destroy( ptpPrc->imsbtree );	
								ptpPrc->imsbtree = NULL;
							}
							for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
								CMemLib::Free( ptpPrc->cblks.enc[i32CodeBlockNo].data - 2 );
								CMemLib::Free( ptpPrc->cblks.enc[i32CodeBlockNo].layers );
								CMemLib::Free( ptpPrc->cblks.enc[i32CodeBlockNo].passes );
							}
							CMemLib::Free( ptpPrc->cblks.enc );
						}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
						CMemLib::Free( ptbBand->precincts );
						ptbBand->precincts = NULL;
					}	// for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
				}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
				CMemLib::Free( pttTileC->resolutions );
				pttTileC->resolutions = NULL;
			}	// for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
			CMemLib::Free( pttTile->comps );
			pttTile->comps = NULL;
		}	// for ( i32TileNo = 0; i32TileNo < 1; i32TileNo++ ) {
		CMemLib::Free( _ptTcd->tcd_image->tiles );
		_ptTcd->tcd_image->tiles = NULL;
	}

	/**
	 * Initialize the tile coder (reuses the memory allocated by tcd_malloc_encode).
	 *
	 * \param _ptTcd TCD handle.
	 * \param _piImage Raw piImage.
	 * \param _pcCp Coding parameters.
	 * \param _i32CurTileNo Number that identifies the tile that will be encoded.
	 */
	void LSE_CALL CTcd::tcd_init_encode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32CurTileNo ) {
		int32_t i32TileNo, i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;

		for ( i32TileNo = 0; i32TileNo < 1; i32TileNo++ ) {
			CJ2k::opj_tcp_t * ptTcp = &_pcCp->tcps[_i32CurTileNo];
			int32_t J;
			// cfr p59 ISO/IEC FDIS15444-1: 2000 (18 august 2000).
			int32_t i32P = _i32CurTileNo % _pcCp->tw;
			int32_t i32Q = _i32CurTileNo / _pcCp->tw;

			opj_tcd_tile_t * pttTile = _ptTcd->tcd_image->tiles;
			
			// 4 borders of the tile rescale on the image if necessary.
			pttTile->x0 = CJpegInt::int_max( _pcCp->tx0 + i32P * _pcCp->tdx, _piImage->x0 );
			pttTile->y0 = CJpegInt::int_max( _pcCp->ty0 + i32Q * _pcCp->tdy, _piImage->y0 );
			pttTile->x1 = CJpegInt::int_min( _pcCp->tx0 + (i32P + 1) * _pcCp->tdx, _piImage->x1 );
			pttTile->y1 = CJpegInt::int_min( _pcCp->ty0 + (i32Q + 1) * _pcCp->tdy, _piImage->y1 );
			
			pttTile->numcomps = _piImage->numcomps;
			// pttTile->PPT = _piImage->PPT;

			// Modification of the RATE.
			for ( J = 0; J < ptTcp->numlayers; J++ ) {
				ptTcp->rates[J] = ptTcp->rates[J] ? 
					_pcCp->tp_on ? 
						((static_cast<float>(pttTile->numcomps * (pttTile->x1 - pttTile->x0) * (pttTile->y1 - pttTile->y0) * _piImage->comps[0].prec)) /
						(ptTcp->rates[J] * 8 * _piImage->comps[0].dx * _piImage->comps[0].dy)) - (((_ptTcd->cur_totnum_tp - 1) * 14) / ptTcp->numlayers) :
						(static_cast<float>(pttTile->numcomps * (pttTile->x1 - pttTile->x0) * (pttTile->y1 - pttTile->y0) * _piImage->comps[0].prec)) /
						(ptTcp->rates[J] * 8 * _piImage->comps[0].dx * _piImage->comps[0].dy) : 0;

				if ( ptTcp->rates[J] ) {
					if ( J && ptTcp->rates[J] < ptTcp->rates[J - 1] + 10 ) {
						ptTcp->rates[J] = ptTcp->rates[J - 1] + 20;
					}
					else {
						if ( !J && ptTcp->rates[J] < 30 ) {
							ptTcp->rates[J] = 30;
						}
					}
				}
			}
			// End modification of the RATE.

			// pttTile->comps = static_cast<opj_tcd_tilecomp_t *>(CMemLib::MReAlloc( pttTile->comps, _piImage->numcomps*sizeof( opj_tcd_tilecomp_t ) ));
			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				CJ2k::opj_tccp_t * ptTccp = &ptTcp->tccps[i32CompNo];
				
				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];

				// Border of each tile component (global).
				pttTileC->x0 = CJpegInt::int_ceildiv( pttTile->x0, _piImage->comps[i32CompNo].dx );
				pttTileC->y0 = CJpegInt::int_ceildiv( pttTile->y0, _piImage->comps[i32CompNo].dy );
				pttTileC->x1 = CJpegInt::int_ceildiv( pttTile->x1, _piImage->comps[i32CompNo].dx );
				pttTileC->y1 = CJpegInt::int_ceildiv( pttTile->y1, _piImage->comps[i32CompNo].dy );
				
				pttTileC->data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>((pttTileC->x1 - pttTileC->x0) * (pttTileC->y1 - pttTileC->y0)) * sizeof( int32_t ) ));
				pttTileC->numresolutions = ptTccp->numresolutions;
				// pttTileC->resolutions = static_cast<opj_tcd_resolution_t *>(CMemLib::MReAlloc( pttTileC->resolutions, pttTileC->numresolutions*sizeof( opj_tcd_resolution_t ) ));
				for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
					int32_t i32PdX, i32PdY;

					int32_t i32LevelNo = pttTileC->numresolutions - 1 - i32ResNo;
					int32_t i32TlprcXStart, i32TlprcYStart, i32BrprcXEnd, i32BrprcYEnd;
					int32_t i32TlcbgXStart, i32TlcbgYStart, i32BrcbgXEnd, i32BrcbgYEnd;
					int32_t i32CbgWidthExpn, i32CbgHeightExpn;
					int32_t i32CblkWidthExpn, i32CblkHeightExpn;
					
					opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];

					// Border for each resolution level (global).
					ptrRes->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
					ptrRes->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
					ptrRes->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
					ptrRes->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );	
					ptrRes->numbands = i32ResNo == 0 ? 1 : 3;

					// p. 35, table A-23, ISO/IEC FDIS154444-1: 2000 (18 august 2000).
					if ( ptTccp->csty & J2K_CCP_CSTY_PRT ) {
						i32PdX = ptTccp->prcw[i32ResNo];
						i32PdY = ptTccp->prch[i32ResNo];
					}
					else {
						i32PdX = 15;
						i32PdY = 15;
					}
					// p. 64, B.6, ISO/IEC FDIS15444-1: 2000 (18 august 2000).
					i32TlprcXStart = CJpegInt::int_floordivpow2( ptrRes->x0, i32PdX ) << i32PdX;
					i32TlprcYStart = CJpegInt::int_floordivpow2( ptrRes->y0, i32PdY ) << i32PdY;
					i32BrprcXEnd = CJpegInt::int_ceildivpow2( ptrRes->x1, i32PdX ) << i32PdX;
					i32BrprcYEnd = CJpegInt::int_ceildivpow2( ptrRes->y1, i32PdY ) << i32PdY;
					
					ptrRes->pw = (i32BrprcXEnd - i32TlprcXStart) >> i32PdX;
					ptrRes->ph = (i32BrprcYEnd - i32TlprcYStart) >> i32PdY;
					
					if ( i32ResNo == 0 ) {
						i32TlcbgXStart = i32TlprcXStart;
						i32TlcbgYStart = i32TlprcYStart;
						i32BrcbgXEnd = i32BrprcXEnd;
						i32BrcbgYEnd = i32BrprcYEnd;
						i32CbgWidthExpn = i32PdX;
						i32CbgHeightExpn = i32PdY;
					}
					else {
						i32TlcbgXStart = CJpegInt::int_ceildivpow2( i32TlprcXStart, 1 );
						i32TlcbgYStart = CJpegInt::int_ceildivpow2( i32TlprcYStart, 1 );
						i32BrcbgXEnd = CJpegInt::int_ceildivpow2( i32BrprcXEnd, 1 );
						i32BrcbgYEnd = CJpegInt::int_ceildivpow2( i32BrprcYEnd, 1 );
						i32CbgWidthExpn = i32PdX - 1;
						i32CbgHeightExpn = i32PdY - 1;
					}
					
					i32CblkWidthExpn = CJpegInt::int_min( ptTccp->cblkw, i32CbgWidthExpn );
					i32CblkHeightExpn = CJpegInt::int_min( ptTccp->cblkh, i32CbgHeightExpn );
					
					for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
						int32_t i32X0b, i32Y0b;
						int32_t i32Gain, i32NumBps;
						CJ2k::opj_stepsize_t * psStepSize = NULL;

						opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];


						ptbBand->bandno = i32ResNo == 0 ? 0 : i32BandNo + 1;
						i32X0b = (ptbBand->bandno == 1) || (ptbBand->bandno == 3) ? 1 : 0;
						i32Y0b = (ptbBand->bandno == 2) || (ptbBand->bandno == 3) ? 1 : 0;
						
						if ( ptbBand->bandno == 0 ) {
							// Band border.
							ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
							ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
							ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
							ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );
						}
						else {
							ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
							ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
							ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
							ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
						}
						
						psStepSize = &ptTccp->stepsizes[i32ResNo == 0 ? 0 : 3 * (i32ResNo - 1) + i32BandNo + 1];
						i32Gain = ptTccp->qmfbid == 0 ? CDwt::dwt_getgain_real( ptbBand->bandno ) : CDwt::dwt_getgain( ptbBand->bandno );
						i32NumBps = _piImage->comps[i32CompNo].prec + i32Gain;
						ptbBand->stepsize = static_cast<float>(((1.0 + psStepSize->mant / 2048.0) * std::pow( 2.0, static_cast<double>(i32NumBps - psStepSize->expn) ) ));
						ptbBand->numbps = psStepSize->expn + ptTccp->numgbits - 1;	// Why -1?
						
						for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
							int32_t i32TlcBlkXStart, i32TlcBlkYStart, i32BrcBlkXEnd, i32BrcBlkYEnd;

							int32_t i32CbgXStart = i32TlcbgXStart + (i32PrecNo % ptrRes->pw ) * (1 << i32CbgWidthExpn);
							int32_t i32CbgYStart = i32TlcbgYStart + (i32PrecNo / ptrRes->pw ) * (1 << i32CbgHeightExpn);
							int32_t i32CbgXEnd = i32CbgXStart + (1 << i32CbgWidthExpn);
							int32_t i32CbgYEnd = i32CbgYStart + (1 << i32CbgHeightExpn);
							
							opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];

							// Precinct size (global).
							ptpPrc->x0 = CJpegInt::int_max( i32CbgXStart, ptbBand->x0 );
							ptpPrc->y0 = CJpegInt::int_max( i32CbgYStart, ptbBand->y0 );
							ptpPrc->x1 = CJpegInt::int_min( i32CbgXEnd, ptbBand->x1 );
							ptpPrc->y1 = CJpegInt::int_min( i32CbgYEnd, ptbBand->y1 );

							i32TlcBlkXStart = CJpegInt::int_floordivpow2( ptpPrc->x0, i32CblkWidthExpn ) << i32CblkWidthExpn;
							i32TlcBlkYStart = CJpegInt::int_floordivpow2( ptpPrc->y0, i32CblkHeightExpn ) << i32CblkHeightExpn;
							i32BrcBlkXEnd = CJpegInt::int_ceildivpow2( ptpPrc->x1, i32CblkWidthExpn ) << i32CblkWidthExpn;
							i32BrcBlkYEnd = CJpegInt::int_ceildivpow2( ptpPrc->y1, i32CblkHeightExpn ) << i32CblkHeightExpn;
							ptpPrc->cw = (i32BrcBlkXEnd - i32TlcBlkXStart) >> i32CblkWidthExpn;
							ptpPrc->ch = (i32BrcBlkYEnd - i32TlcBlkYStart) >> i32CblkHeightExpn;

							CMemLib::Free( ptpPrc->cblks.enc );
							ptpPrc->cblks.enc = static_cast<opj_tcd_cblk_enc_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ptpPrc->cw * ptpPrc->ch) * sizeof( opj_tcd_cblk_enc_t ) ));

							if ( ptpPrc->incltree != NULL ) {
								CTgt::tgt_destroy( ptpPrc->incltree );
							}
							if ( ptpPrc->imsbtree != NULL ) {
								CTgt::tgt_destroy( ptpPrc->imsbtree );
							}
							
							ptpPrc->incltree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );
							ptpPrc->imsbtree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );

							for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
								int32_t i32CodeBlockXStart = i32TlcBlkXStart + (i32CodeBlockNo % ptpPrc->cw) * (1 << i32CblkWidthExpn);
								int32_t i32CodeBlockYStart = i32TlcBlkYStart + (i32CodeBlockNo / ptpPrc->cw) * (1 << i32CblkHeightExpn);
								int32_t i32CodeBlockXEnd = i32CodeBlockXStart + (1 << i32CblkWidthExpn);
								int32_t i32CodeBlockYEnd = i32CodeBlockYStart + (1 << i32CblkHeightExpn);

								opj_tcd_cblk_enc_t * ptceCodeBlock = &ptpPrc->cblks.enc[i32CodeBlockNo];

								// Code-block size (global).
								ptceCodeBlock->x0 = CJpegInt::int_max( i32CodeBlockXStart, ptpPrc->x0 );
								ptceCodeBlock->y0 = CJpegInt::int_max( i32CodeBlockYStart, ptpPrc->y0 );
								ptceCodeBlock->x1 = CJpegInt::int_min( i32CodeBlockXEnd, ptpPrc->x1 );
								ptceCodeBlock->y1 = CJpegInt::int_min( i32CodeBlockYEnd, ptpPrc->y1 );
								ptceCodeBlock->data = static_cast<uint8_t *>(CMemLib::CMAlloc( (8192 + 2) * sizeof( uint8_t ) ));
								// FIXME: mqc_init_enc and mqc_byteout underrun the buffer if we don't do this. Why?
								ptceCodeBlock->data  += 2;
								ptceCodeBlock->layers = static_cast<opj_tcd_layer_t *>(CMemLib::CMAlloc( 100 * sizeof( opj_tcd_layer_t ) ));
								ptceCodeBlock->passes = static_cast<opj_tcd_pass_t *>(CMemLib::CMAlloc( 100 * sizeof( opj_tcd_pass_t ) ));
							}
						}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
					}	// for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
				}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
			}	// for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
		}	// for ( i32TileNo = 0; i32TileNo < 1; i32TileNo++ ) {
	}

	/**
	 * Initialize the tile decoder.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _piImage Raw piImage.
	 * \param _pcCp Coding parameters.
	 */
	void LSE_CALL CTcd::tcd_malloc_decode( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp ) {
		int32_t I, J, i32TileNo, i32P, i32Q;
		int32_t i32X0 = 0, i32Y0 = 0, i32X1 = 0, i32Y1 = 0, i32Width, i32Height;

		_ptTcd->image = _piImage;
		_ptTcd->tcd_image->tw = _pcCp->tw;
		_ptTcd->tcd_image->th = _pcCp->th;
		_ptTcd->tcd_image->tiles = static_cast<opj_tcd_tile_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_pcCp->tw * _pcCp->th) * sizeof( opj_tcd_tile_t ) ));

		// Allocate place to store the decoded data = _i32Final piImage.
		// Place limited by the tile really present in the codestream.

		for ( J = 0; J < _pcCp->tileno_size; J++ ) {
			opj_tcd_tile_t * pttTile;
			
			i32TileNo = _pcCp->tileno[J];
			pttTile = &(_ptTcd->tcd_image->tiles[_pcCp->tileno[i32TileNo]]);
			pttTile->numcomps = _piImage->numcomps;
			pttTile->comps = static_cast<opj_tcd_tilecomp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_piImage->numcomps) * sizeof( opj_tcd_tilecomp_t ) ));
		}

		for ( I = 0; I < _piImage->numcomps; I++ ) {
			for ( J = 0; J < _pcCp->tileno_size; J++ ) {
				opj_tcd_tile_t * pttTile;
				opj_tcd_tilecomp_t * pttTileC;
				
				// cfr p59 ISO/IEC FDIS15444-1: 2000 (18 august 2000).
				i32TileNo = _pcCp->tileno[J];
				
				pttTile = &_ptTcd->tcd_image->tiles[_pcCp->tileno[i32TileNo]];
				pttTileC = &pttTile->comps[I];
				
				i32P = i32TileNo % _pcCp->tw;
				i32Q = i32TileNo / _pcCp->tw;
				
				// 4 borders of the tile rescale on the _piImage if necessary.
				pttTile->x0 = CJpegInt::int_max( _pcCp->tx0 + i32P * _pcCp->tdx, _piImage->x0 );
				pttTile->y0 = CJpegInt::int_max( _pcCp->ty0 + i32Q * _pcCp->tdy, _piImage->y0 );
				pttTile->x1 = CJpegInt::int_min( _pcCp->tx0 + (i32P + 1) * _pcCp->tdx, _piImage->x1 );
				pttTile->y1 = CJpegInt::int_min( _pcCp->ty0 + (i32Q + 1) * _pcCp->tdy, _piImage->y1 );

				pttTileC->x0 = CJpegInt::int_ceildiv( pttTile->x0, _piImage->comps[I].dx );
				pttTileC->y0 = CJpegInt::int_ceildiv( pttTile->y0, _piImage->comps[I].dy );
				pttTileC->x1 = CJpegInt::int_ceildiv( pttTile->x1, _piImage->comps[I].dx );
				pttTileC->y1 = CJpegInt::int_ceildiv( pttTile->y1, _piImage->comps[I].dy );

				i32X0 = J == 0 ? pttTileC->x0 : CJpegInt::int_min( i32X0, pttTileC->x0 );
				i32Y0 = J == 0 ? pttTileC->y0 : CJpegInt::int_min( i32Y0, pttTileC->x0 );
				i32X1 = J == 0 ? pttTileC->x1 : CJpegInt::int_max( i32X1, pttTileC->x1 );
				i32Y1 = J == 0 ? pttTileC->y1 : CJpegInt::int_max( i32Y1, pttTileC->y1 );
			}

			i32Width = CJpegInt::int_ceildivpow2( i32X1 - i32X0, _piImage->comps[I].factor );
			i32Height = CJpegInt::int_ceildivpow2( i32Y1 - i32Y0, _piImage->comps[I].factor );

			_piImage->comps[I].w = i32Width;
			_piImage->comps[I].h = i32Height;
			_piImage->comps[I].x0 = i32X0;
			_piImage->comps[I].y0 = i32Y0;
		}
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _piImage Raw piImage.
	 * \param _pcCp Coding parameters.
	 * \param _i32TileNo Undocumented OpenJpeg Library parameter.
	 * \param _pciInfoC Undocumented OpenJpeg Library parameter.
	 */
	void LSE_CALL CTcd::tcd_malloc_decode_tile( opj_tcd_t * _ptTcd, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, CJpegCommonStructs::opj_codestream_info_t * /*_pciInfoC*/ ) {
		int32_t i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;
		CJ2k::opj_tcp_t * ptTcp;
		opj_tcd_tile_t * pttTile;

		_ptTcd->cp = _pcCp;
		
		ptTcp = &(_pcCp->tcps[_pcCp->tileno[_i32TileNo]]);
		pttTile = &(_ptTcd->tcd_image->tiles[_pcCp->tileno[_i32TileNo]]);
		
		_i32TileNo = _pcCp->tileno[_i32TileNo];
		
		for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
			CJ2k::opj_tccp_t * ptTccp = &ptTcp->tccps[i32CompNo];
			opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
			
			// Border of each tile component (global).
			pttTileC->x0 = CJpegInt::int_ceildiv( pttTile->x0, _piImage->comps[i32CompNo].dx );
			pttTileC->y0 = CJpegInt::int_ceildiv( pttTile->y0, _piImage->comps[i32CompNo].dy );
			pttTileC->x1 = CJpegInt::int_ceildiv( pttTile->x1, _piImage->comps[i32CompNo].dx );
			pttTileC->y1 = CJpegInt::int_ceildiv( pttTile->y1, _piImage->comps[i32CompNo].dy );

			pttTileC->numresolutions = ptTccp->numresolutions;
			pttTileC->resolutions = static_cast<opj_tcd_resolution_t *>(CMemLib::MAlloc( static_cast<uint32_t>(pttTileC->numresolutions) * sizeof( opj_tcd_resolution_t ) ));
			
			for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
				int32_t i32PdX, i32PdY;
				int32_t i32LevelNo = pttTileC->numresolutions - 1 - i32ResNo;
				int32_t i32TlPrcStartX, i32TlPrcStartY, i32BrPrcEndX, i32BrPrcEndY;
				int32_t i32TlcbgXStart, i32TlcbgYStart, i32BrcbgXEnd, i32BrcbgYEnd;
				int32_t i32CbgWidthExpn, i32CbgHeightExpn;
				int32_t i32CblkWidthExpn, i32CblkHeightExpn;
				
				opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];
				
				// Border for each resolution level (global).
				ptrRes->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
				ptrRes->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
				ptrRes->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
				ptrRes->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );
				ptrRes->numbands = i32ResNo == 0 ? 1 : 3;
				
				// p. 35, table A-23, ISO/IEC FDIS154444-1: 2000 (18 august 2000).
				if ( ptTccp->csty & J2K_CCP_CSTY_PRT ) {
					i32PdX = ptTccp->prcw[i32ResNo];
					i32PdY = ptTccp->prch[i32ResNo];
				}
				else {
					i32PdX = 15;
					i32PdY = 15;
				}			
				
				// p. 64, B.6, ISO/IEC FDIS15444-1: 2000 (18 august 2000).
				i32TlPrcStartX = CJpegInt::int_floordivpow2( ptrRes->x0, i32PdX ) << i32PdX;
				i32TlPrcStartY = CJpegInt::int_floordivpow2( ptrRes->y0, i32PdY ) << i32PdY;
				i32BrPrcEndX = CJpegInt::int_ceildivpow2( ptrRes->x1, i32PdX ) << i32PdX;
				i32BrPrcEndY = CJpegInt::int_ceildivpow2( ptrRes->y1, i32PdY ) << i32PdY;
				
				ptrRes->pw = (ptrRes->x0 == ptrRes->x1) ? 0 : ((i32BrPrcEndX - i32TlPrcStartX) >> i32PdX);
				ptrRes->ph = (ptrRes->y0 == ptrRes->y1) ? 0 : ((i32BrPrcEndY - i32TlPrcStartY) >> i32PdY);
				
				if ( i32ResNo == 0 ) {
					i32TlcbgXStart = i32TlPrcStartX;
					i32TlcbgYStart = i32TlPrcStartY;
					i32BrcbgXEnd = i32BrPrcEndX;
					i32BrcbgYEnd = i32BrPrcEndY;
					i32CbgWidthExpn = i32PdX;
					i32CbgHeightExpn = i32PdY;
				}
				else {
					i32TlcbgXStart = CJpegInt::int_ceildivpow2( i32TlPrcStartX, 1 );
					i32TlcbgYStart = CJpegInt::int_ceildivpow2( i32TlPrcStartY, 1 );
					i32BrcbgXEnd = CJpegInt::int_ceildivpow2( i32BrPrcEndX, 1 );
					i32BrcbgYEnd = CJpegInt::int_ceildivpow2( i32BrPrcEndY, 1 );
					i32CbgWidthExpn = i32PdX - 1;
					i32CbgHeightExpn = i32PdY - 1;
				}
				
				i32CblkWidthExpn = CJpegInt::int_min( ptTccp->cblkw, i32CbgWidthExpn );
				i32CblkHeightExpn = CJpegInt::int_min( ptTccp->cblkh, i32CbgHeightExpn );
				
				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
					int32_t i32X0b, i32Y0b;
					int32_t i32Gain, i32NumBps;
					CJ2k::opj_stepsize_t * psStepSize = NULL;
					
					opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
					ptbBand->bandno = i32ResNo == 0 ? 0 : i32BandNo + 1;
					i32X0b = (ptbBand->bandno == 1) || (ptbBand->bandno == 3) ? 1 : 0;
					i32Y0b = (ptbBand->bandno == 2) || (ptbBand->bandno == 3) ? 1 : 0;
					
					if ( ptbBand->bandno == 0 ) {
						// Band border (global).
						ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0, i32LevelNo );
						ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0, i32LevelNo );
						ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1, i32LevelNo );
						ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1, i32LevelNo );
					}
					else {
						// Band border (global).
						ptbBand->x0 = CJpegInt::int_ceildivpow2( pttTileC->x0 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
						ptbBand->y0 = CJpegInt::int_ceildivpow2( pttTileC->y0 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
						ptbBand->x1 = CJpegInt::int_ceildivpow2( pttTileC->x1 - (1 << i32LevelNo) * i32X0b, i32LevelNo + 1 );
						ptbBand->y1 = CJpegInt::int_ceildivpow2( pttTileC->y1 - (1 << i32LevelNo) * i32Y0b, i32LevelNo + 1 );
					}
					
					psStepSize = &ptTccp->stepsizes[i32ResNo==0?0:3*(i32ResNo-1)+i32BandNo+1];
					i32Gain = ptTccp->qmfbid == 0 ? CDwt::dwt_getgain_real( ptbBand->bandno ) : CDwt::dwt_getgain( ptbBand->bandno );
					i32NumBps = _piImage->comps[i32CompNo].prec + i32Gain;
					ptbBand->stepsize = static_cast<float>((((1.0 + psStepSize->mant / 2048.0) * std::pow( 2.0, static_cast<double>(i32NumBps - psStepSize->expn) )) * 0.5));
					ptbBand->numbps = psStepSize->expn + ptTccp->numgbits - 1;	// WHY -1?
					
					ptbBand->precincts = static_cast<opj_tcd_precinct_t *>(CMemLib::MAlloc( static_cast<uint32_t>(ptrRes->pw * ptrRes->ph) * sizeof( opj_tcd_precinct_t ) ));
					
					for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
						int32_t i32TlCodeBlockStartX, i32TlCodeBlockStartY, i32BrCodeBlockEndX, i32BrCodeBlockEndY;
						int32_t i32CbgStartX = i32TlcbgXStart + (i32PrecNo % ptrRes->pw) * (1 << i32CbgWidthExpn);
						int32_t i32CbgStartY = i32TlcbgYStart + (i32PrecNo / ptrRes->pw) * (1 << i32CbgHeightExpn);
						int32_t i32CbgEndX = i32CbgStartX + (1 << i32CbgWidthExpn);
						int32_t i32CbgEndY = i32CbgStartY + (1 << i32CbgHeightExpn);
						
						opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
						// Precinct size (global).
						ptpPrc->x0 = CJpegInt::int_max( i32CbgStartX, ptbBand->x0 );
						ptpPrc->y0 = CJpegInt::int_max( i32CbgStartY, ptbBand->y0 );
						ptpPrc->x1 = CJpegInt::int_min( i32CbgEndX, ptbBand->x1 );
						ptpPrc->y1 = CJpegInt::int_min( i32CbgEndY, ptbBand->y1 );
						
						i32TlCodeBlockStartX = CJpegInt::int_floordivpow2( ptpPrc->x0, i32CblkWidthExpn ) << i32CblkWidthExpn;
						i32TlCodeBlockStartY = CJpegInt::int_floordivpow2( ptpPrc->y0, i32CblkHeightExpn ) << i32CblkHeightExpn;
						i32BrCodeBlockEndX = CJpegInt::int_ceildivpow2( ptpPrc->x1, i32CblkWidthExpn ) << i32CblkWidthExpn;
						i32BrCodeBlockEndY = CJpegInt::int_ceildivpow2( ptpPrc->y1, i32CblkHeightExpn ) << i32CblkHeightExpn;
						ptpPrc->cw = (i32BrCodeBlockEndX - i32TlCodeBlockStartX) >> i32CblkWidthExpn;
						ptpPrc->ch = (i32BrCodeBlockEndY - i32TlCodeBlockStartY) >> i32CblkHeightExpn;

						ptpPrc->cblks.dec = static_cast<opj_tcd_cblk_dec_t *>(CMemLib::MAlloc( static_cast<uint32_t>(ptpPrc->cw * ptpPrc->ch) * sizeof( opj_tcd_cblk_dec_t ) ));

						ptpPrc->incltree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );
						ptpPrc->imsbtree = CTgt::tgt_create( ptpPrc->cw, ptpPrc->ch );
						
						for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
							int32_t i32CblkXStart = i32TlCodeBlockStartX + (i32CodeBlockNo % ptpPrc->cw) * (1 << i32CblkWidthExpn);
							int32_t i32CblkYStart = i32TlCodeBlockStartY + (i32CodeBlockNo / ptpPrc->cw) * (1 << i32CblkHeightExpn);
							int32_t i32CblkXEnd = i32CblkXStart + (1 << i32CblkWidthExpn);
							int32_t i32CblkYEnd = i32CblkYStart + (1 << i32CblkHeightExpn);

							opj_tcd_cblk_dec_t * ptcdCodeBlock = &ptpPrc->cblks.dec[i32CodeBlockNo];
							ptcdCodeBlock->data = NULL;
							ptcdCodeBlock->segs = NULL;
							// Code-block size (global).
							ptcdCodeBlock->x0 = CJpegInt::int_max( i32CblkXStart, ptpPrc->x0 );
							ptcdCodeBlock->y0 = CJpegInt::int_max( i32CblkYStart, ptpPrc->y0 );
							ptcdCodeBlock->x1 = CJpegInt::int_min( i32CblkXEnd, ptpPrc->x1 );
							ptcdCodeBlock->y1 = CJpegInt::int_min( i32CblkYEnd, ptpPrc->y1 );
							ptcdCodeBlock->numsegs = 0;
						}
					}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
				}	// for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
			}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
		}	// for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _i32LayNo Undocumented OpenJpeg Library parameter.
	 * \param _i32Final Undocumented OpenJpeg Library parameter.
	 */
	void LSE_CALL CTcd::tcd_makelayer_fixed( opj_tcd_t * _ptTcd, int32_t _i32LayNo, int32_t _i32Final ) {
		int32_t i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo;
		int32_t i32Value;
		int32_t i32Matrix[10][10][3];
		int32_t I, J, K;

		CJ2k::opj_cp_t * pcCp = _ptTcd->cp;
		opj_tcd_tile_t * pttTcdTile = _ptTcd->tcd_tile;
		CJ2k::opj_tcp_t * ptTcdTcp = _ptTcd->tcp;

		
		for ( i32CompNo = 0; i32CompNo < pttTcdTile->numcomps; i32CompNo++ ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTcdTile->comps[i32CompNo];
			for ( I = 0; I < ptTcdTcp->numlayers; I++ ) {
				for ( J = 0; J < pttTileC->numresolutions; J++ ) {
					for ( K = 0; K < 3; K++ ) {
						i32Matrix[I][J][K] =
							static_cast<int32_t>(pcCp->matrice[I*pttTileC->numresolutions*3+J*3+K] *
							static_cast<float>(_ptTcd->image->comps[i32CompNo].prec / 16.0f));
					}
				}
			}
	        
			for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
				opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];
				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
					opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
					for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
						opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
						for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
							opj_tcd_cblk_enc_t * ptcdCodeBlock = &ptpPrc->cblks.enc[i32CodeBlockNo];
							opj_tcd_layer_t * ptlLayer = &ptcdCodeBlock->layers[_i32LayNo];
							int32_t i32N;
							int32_t i32Imsb = _ptTcd->image->comps[i32CompNo].prec - ptcdCodeBlock->numbps;	// Number of bit-plan equal to zero.
							// Correction of the matrix of coefficient to include the IMSB information.
							if ( _i32LayNo == 0 ) {
								i32Value = i32Matrix[_i32LayNo][i32ResNo][i32BandNo];
								if ( i32Imsb >= i32Value ) {
									i32Value = 0;
								}
								else {
									i32Value -= i32Imsb;
								}
							}
							else {
								i32Value =	i32Matrix[_i32LayNo][i32ResNo][i32BandNo] -	i32Matrix[_i32LayNo-1][i32ResNo][i32BandNo];
								if ( i32Imsb >= i32Matrix[_i32LayNo-1][i32ResNo][i32BandNo] ) {
									i32Value -= (i32Imsb - i32Matrix[_i32LayNo-1][i32ResNo][i32BandNo]);
									if ( i32Value < 0 ) {
										i32Value = 0;
									}
								}
							}
							
							if ( _i32LayNo == 0 ) {
								ptcdCodeBlock->numpassesinlayers = 0;
							}
							
							i32N = ptcdCodeBlock->numpassesinlayers;
							if ( ptcdCodeBlock->numpassesinlayers == 0 ) {
								if ( i32Value != 0 ) {
									i32N = 3 * i32Value - 2 + ptcdCodeBlock->numpassesinlayers;
								}
								else {
									i32N = ptcdCodeBlock->numpassesinlayers;
								}
							}
							else {
								i32N = 3 * i32Value + ptcdCodeBlock->numpassesinlayers;
							}
							
							ptlLayer->numpasses = i32N - ptcdCodeBlock->numpassesinlayers;
							
							if ( !ptlLayer->numpasses ) { continue; }
							
							if ( ptcdCodeBlock->numpassesinlayers == 0 ) {
								ptlLayer->len = ptcdCodeBlock->passes[i32N-1].rate;
								ptlLayer->data = ptcdCodeBlock->data;
							}
							else {
								ptlLayer->len = ptcdCodeBlock->passes[i32N-1].rate - ptcdCodeBlock->passes[ptcdCodeBlock->numpassesinlayers-1].rate;
								ptlLayer->data = ptcdCodeBlock->data + ptcdCodeBlock->passes[ptcdCodeBlock->numpassesinlayers-1].rate;
							}
							if ( _i32Final ) {
								ptcdCodeBlock->numpassesinlayers = i32N;
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 */
	void LSE_CALL CTcd::tcd_rateallocate_fixed( opj_tcd_t * _ptTcd ) {
		for ( int32_t i32LayNo = 0; i32LayNo < _ptTcd->tcp->numlayers; i32LayNo++ ) {
			tcd_makelayer_fixed( _ptTcd, i32LayNo, 1 );
		}
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _i32LayNo Undocumented OpenJpeg Library parameter.
	 * \param _dThresh Undocumented OpenJpeg Library parameter.
	 * \param _i32Final Undocumented OpenJpeg Library parameter.
	 */
	void LSE_CALL CTcd::tcd_makelayer( opj_tcd_t * _ptTcd, int32_t _i32LayNo, double _dThresh, int32_t _i32Final ) {
		int32_t i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo, i32PassNo;
	
		opj_tcd_tile_t * pttTcdTile = _ptTcd->tcd_tile;

		pttTcdTile->distolayer[_i32LayNo] = 0;	// Fixed_quality.
		
		for ( i32CompNo = 0; i32CompNo < pttTcdTile->numcomps; i32CompNo++ ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTcdTile->comps[i32CompNo];
			for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
				opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];
				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
					opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
					for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
						opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
						for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
							opj_tcd_cblk_enc_t * ptcdCodeBlock = &ptpPrc->cblks.enc[i32CodeBlockNo];
							opj_tcd_layer_t * ptlLayer = &ptcdCodeBlock->layers[_i32LayNo];
							
							int32_t i32N;
							if ( _i32LayNo == 0 ) {
								ptcdCodeBlock->numpassesinlayers = 0;
							}
							i32N = ptcdCodeBlock->numpassesinlayers;
							for ( i32PassNo = ptcdCodeBlock->numpassesinlayers; i32PassNo < ptcdCodeBlock->totalpasses; i32PassNo++ ) {
								int32_t i32Dr;
								double dDd;
								opj_tcd_pass_t * ptpPass = &ptcdCodeBlock->passes[i32PassNo];
								if ( i32N == 0 ) {
									i32Dr = ptpPass->rate;
									dDd = ptpPass->distortiondec;
								}
								else {
									i32Dr = ptpPass->rate - ptcdCodeBlock->passes[i32N - 1].rate;
									dDd = ptpPass->distortiondec - ptcdCodeBlock->passes[i32N - 1].distortiondec;
								}
								if ( !i32Dr ) {
									if ( dDd != 0 ) {
										i32N = i32PassNo + 1;
									}
									continue;
								}
								if ( dDd / i32Dr >= _dThresh ) {
									i32N = i32PassNo + 1;
								}
							}
							ptlLayer->numpasses = i32N - ptcdCodeBlock->numpassesinlayers;
							
							if ( !ptlLayer->numpasses ) {
								ptlLayer->disto = 0;
								continue;
							}
							if ( ptcdCodeBlock->numpassesinlayers == 0 ) {
								ptlLayer->len = ptcdCodeBlock->passes[i32N-1].rate;
								ptlLayer->data = ptcdCodeBlock->data;
								ptlLayer->disto = ptcdCodeBlock->passes[i32N-1].distortiondec;
							}
							else {
								ptlLayer->len = ptcdCodeBlock->passes[i32N-1].rate - ptcdCodeBlock->passes[ptcdCodeBlock->numpassesinlayers-1].rate;
								ptlLayer->data = ptcdCodeBlock->data + ptcdCodeBlock->passes[ptcdCodeBlock->numpassesinlayers-1].rate;
								ptlLayer->disto = ptcdCodeBlock->passes[i32N-1].distortiondec - ptcdCodeBlock->passes[ptcdCodeBlock->numpassesinlayers-1].distortiondec;
							}
							
							pttTcdTile->distolayer[_i32LayNo] += ptlLayer->disto;	// Fixed_quality.
							
							if ( _i32Final ) {
								ptcdCodeBlock->numpassesinlayers = i32N;
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _pui8Dest Undocumented OpenJpeg Library parameter.
	 * \param _i32Len Undocumented OpenJpeg Library parameter.
	 * \param _pciInfoC Undocumented OpenJpeg Library parameter.
	 */
	LSBOOL LSE_CALL CTcd::tcd_rateallocate( opj_tcd_t * _ptTcd, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC ) {
		int32_t i32CompNo, i32ResNo, i32BandNo, i32PrecNo, i32CodeBlockNo, i32PassNo, i32LayNo;
		double dMin, dMax;
		double dCumDisto[100];		// Fixed_quality.
		const double dK = 1.0;		// 1.1; Fixed_quality.
		double dMaxSe = 0.0;

		CJ2k::opj_cp_t * pcCp = _ptTcd->cp;
		opj_tcd_tile_t * pttTcdTile = _ptTcd->tcd_tile;
		CJ2k::opj_tcp_t * ptTcdTcp = _ptTcd->tcp;

		dMin = LSM_DBL_MAX;
		dMax = 0.0;
		
		pttTcdTile->numpix = 0;			// Fixed_quality.
		
		for ( i32CompNo = 0; i32CompNo < pttTcdTile->numcomps; i32CompNo++ ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTcdTile->comps[i32CompNo];
			pttTileC->numpix = 0;

			for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
				opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];

				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
					opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];

					for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
						opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];

						for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
							opj_tcd_cblk_enc_t * ptcdCodeBlock = &ptpPrc->cblks.enc[i32CodeBlockNo];

							for ( i32PassNo = 0; i32PassNo < ptcdCodeBlock->totalpasses; i32PassNo++ ) {
								opj_tcd_pass_t * ptpPass = &ptcdCodeBlock->passes[i32PassNo];
								int32_t i32Dr;
								double dDd, dRdSlope;
								if ( i32PassNo == 0 ) {
									i32Dr = ptpPass->rate;
									dDd = ptpPass->distortiondec;
								}
								else {
									i32Dr = ptpPass->rate - ptcdCodeBlock->passes[i32PassNo-1].rate;
									dDd = ptpPass->distortiondec - ptcdCodeBlock->passes[i32PassNo-1].distortiondec;
								}
								if ( i32Dr == 0 ) { continue; }
								dRdSlope = dDd / i32Dr;
								if ( dRdSlope < dMin ) { dMin = dRdSlope; }
								if ( dRdSlope > dMax ) { dMax = dRdSlope; }
							}	// for ( i32PassNo = 0; i32PassNo < ptcdCodeBlock->totalpasses; i32PassNo++ ) {
							
							// Fixed_quality.
							pttTcdTile->numpix += ((ptcdCodeBlock->x1 - ptcdCodeBlock->x0) * (ptcdCodeBlock->y1 - ptcdCodeBlock->y0));
							pttTileC->numpix += ((ptcdCodeBlock->x1 - ptcdCodeBlock->x0) * (ptcdCodeBlock->y1 - ptcdCodeBlock->y0));
						}	// for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
					}	// for ( i32PrecNo = 0; i32PrecNo < ptrRes->pw * ptrRes->ph; i32PrecNo++ ) {
				}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
			}	// for ( i32ResNo = 0; i32ResNo < pttTileC->numresolutions; i32ResNo++ ) {
			
			dMaxSe += ((static_cast<double>(1 << _ptTcd->image->comps[i32CompNo].prec ) - 1.0) *
				(static_cast<double>(1 << _ptTcd->image->comps[i32CompNo].prec) - 1.0)) *
				static_cast<double>(pttTileC->numpix);
		}	// for ( i32CompNo = 0; i32CompNo < pttTcdTile->numcomps; i32CompNo++ ) {
		
		// Index file.
		if ( _pciInfoC ) {
			CJpegCommonStructs::opj_tile_info_t * ptiTileInfo = &_pciInfoC->tile[_ptTcd->tcd_tileno];
			ptiTileInfo->numpix = pttTcdTile->numpix;
			ptiTileInfo->distotile = pttTcdTile->distotile;
			ptiTileInfo->thresh = static_cast<double *>(CMemLib::MAlloc( static_cast<uint32_t>(ptTcdTcp->numlayers) * sizeof( double ) ));
		}
		
		for ( i32LayNo = 0; i32LayNo < ptTcdTcp->numlayers; i32LayNo++ ) {
			double dLo = dMin;
			double dHi = dMax;
			int32_t i32Success = 0;
			int32_t i32MaxLen = ptTcdTcp->rates[i32LayNo] ? CJpegInt::int_min( static_cast<int32_t>(::ceil( ptTcdTcp->rates[i32LayNo] )), _i32Len ) : _i32Len;
			double dGoodThresh = 0.0;
			double dStableThresh = 0.0;
			int32_t I;
			double dDisToTarget;		// Fixed_quality.
			
			// Fixed_quality.
			dDisToTarget = pttTcdTile->distotile - ((dK * dMaxSe) / std::pow( 10.0, ptTcdTcp->distoratio[i32LayNo] / 10.0 ));
	        
			// Don't try to find an optimal threshold but rather take everything not included yet, if
			//	-r xx, yy, zz, 0	(disto_alloc == 1 and rates == 0)
			//	-q xx, yy, zz, 0	(fixed_quality == 1 and distoratio == 0)
			//	==> possible to have some lossy layers and the last layer for sure lossless.
			if ( ((pcCp->disto_alloc == 1) && (ptTcdTcp->rates[i32LayNo] > 0)) || ((pcCp->fixed_quality == 1) && (ptTcdTcp->distoratio[i32LayNo] > 0)) ) {
				CT2::opj_t2_t * ptT2 = CT2::t2_create( _ptTcd->cinfo, _ptTcd->image, pcCp );
				double dThresh = 0.0;

				for ( I = 0; I < 128; I++ ) {
					int32_t i32L = 0;
					double dDisToAchieved = 0.0;	// Fixed_quality.
					dThresh = (dLo + dHi) / 2;
					
					tcd_makelayer( _ptTcd, i32LayNo, dThresh, 0 );
					
					if ( pcCp->fixed_quality ) {	// Fixed_quality.
						if ( pcCp->cinema ) {
							i32L = CT2::t2_encode_packets( ptT2, _ptTcd->tcd_tileno, pttTcdTile, i32LayNo + 1, _pui8Dest, i32MaxLen, _pciInfoC, _ptTcd->cur_tp_num, _ptTcd->tp_pos, _ptTcd->cur_pino, CJ2k::THRESH_CALC, _ptTcd->cur_totnum_tp );
							if ( i32L == -999 ) {
								dLo = dThresh;
								continue;
							}
							else {
           						dDisToAchieved = i32LayNo == 0 ? 
								pttTcdTile->distolayer[0] : dCumDisto[i32LayNo-1] + pttTcdTile->distolayer[i32LayNo];
								if ( dDisToAchieved < dDisToTarget ) {
									dHi = dThresh; 
									dStableThresh = dThresh;
									continue;
								}
								else {
									dLo = dThresh;
								}
							}
						}
						else {
							dDisToAchieved = (i32LayNo == 0) ? 
								pttTcdTile->distolayer[0] : (dCumDisto[i32LayNo-1] + pttTcdTile->distolayer[i32LayNo]);
							if ( dDisToAchieved < dDisToTarget ) {
								dHi = dThresh;
								dStableThresh = dThresh;
								continue;
							}
							dLo = dThresh;
						}
					}
					else {
						i32L = CT2::t2_encode_packets( ptT2, _ptTcd->tcd_tileno, pttTcdTile, i32LayNo + 1, _pui8Dest, i32MaxLen, _pciInfoC, _ptTcd->cur_tp_num, _ptTcd->tp_pos, _ptTcd->cur_pino, CJ2k::THRESH_CALC, _ptTcd->cur_totnum_tp );
						if ( i32L == -999 ) {
							dLo = dThresh;
							continue;
						}
						dHi = dThresh;
						dStableThresh = dThresh;
					}
				}
				i32Success = 1;
				dGoodThresh = dStableThresh == 0 ? dThresh : dStableThresh;
				CT2::t2_destroy( ptT2 );
			}
			else {
				i32Success = 1;
				dGoodThresh = dMin;
			}
			
			if ( !i32Success ) { return false; }
			
			if ( _pciInfoC ) {		// Threshold for Marcela Index.
				_pciInfoC->tile[_ptTcd->tcd_tileno].thresh[i32LayNo] = dGoodThresh;
			}
			tcd_makelayer( _ptTcd, i32LayNo, dGoodThresh, 1 );
	        
			// Fixed_quality.
			dCumDisto[i32LayNo] = (i32LayNo == 0) ? pttTcdTile->distolayer[0] : (dCumDisto[i32LayNo-1] + pttTcdTile->distolayer[i32LayNo]);
		}

		return true;
	}

	/**
	 * Encode a tile from the raw image into a buffer.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _i32TileNo Number that identifies one of the tiles to be encoded.
	 * \param _pui8Dest Destination buffer.
	 * \param _i32Len Length of destination buffer.
	 * \param _pciInfoC Codestream information structure.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CTcd::tcd_encode_tile( opj_tcd_t * _ptTcd, int32_t _i32TileNo, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC ) {
		int32_t i32CompNo;
		int32_t i32L, I, i32NumPacks = 0;
		opj_tcd_tile_t * pttTile = NULL;
		CJ2k::opj_tcp_t * ptTcdTcp = NULL;
		CJ2k::opj_cp_t * pcCp = NULL;

		CJ2k::opj_tcp_t * ptTcp = &_ptTcd->cp->tcps[0];
		CJ2k::opj_tccp_t * ptTccp = &ptTcp->tccps[0];
		CJpegImage::opj_image_t * piImage = _ptTcd->image;
		
		CT1::opj_t1_t * ptT1 = NULL;		// T1 component.
		CT2::opj_t2_t * ptT2 = NULL;		// T2 component.

		_ptTcd->tcd_tileno = _i32TileNo;
		_ptTcd->tcd_tile = _ptTcd->tcd_image->tiles;
		_ptTcd->tcp = &_ptTcd->cp->tcps[_i32TileNo];

		pttTile = _ptTcd->tcd_tile;
		ptTcdTcp = _ptTcd->tcp;
		pcCp = _ptTcd->cp;

		CTime tTimer;
		
		if ( _ptTcd->cur_tp_num == 0 ) {
			tTimer.Update( false );
			_ptTcd->encoding_time = tTimer.GetCurMicros() * (1.0 / 1000000.0);	// Time needed to decode a tile.
			if ( _pciInfoC ) {
				opj_tcd_tilecomp_t * pttTileIdx = &pttTile->comps[0];			// Based on component 0.
				for ( I = 0; I < pttTileIdx->numresolutions; I++ ) {
					opj_tcd_resolution_t * ptrResIdx = &pttTileIdx->resolutions[I];
					
					_pciInfoC->tile[_i32TileNo].pw[I] = ptrResIdx->pw;
					_pciInfoC->tile[_i32TileNo].ph[I] = ptrResIdx->ph;
					
					i32NumPacks += ptrResIdx->pw * ptrResIdx->ph;
					
					_pciInfoC->tile[_i32TileNo].pdx[I] = ptTccp->prcw[I];
					_pciInfoC->tile[_i32TileNo].pdy[I] = ptTccp->prch[I];
				}
				_pciInfoC->tile[_i32TileNo].packet = static_cast<CJpegCommonStructs::opj_packet_info_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_pciInfoC->numcomps * _pciInfoC->numlayers * i32NumPacks) * sizeof( CJpegCommonStructs::opj_packet_info_t ) ));
			}
			
			//---------------TILE-------------------
			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				int32_t i32X, i32Y;
				
				int32_t i32Adjust = piImage->comps[i32CompNo].sgnd ? 0 : 1 << ( piImage->comps[i32CompNo].prec - 1 );
				int32_t i32OffsetX = CJpegInt::int_ceildiv( piImage->x0, piImage->comps[i32CompNo].dx );
				int32_t i32OffsetY = CJpegInt::int_ceildiv( piImage->y0, piImage->comps[i32CompNo].dy );
				
				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
				int32_t i32Tw = pttTileC->x1 - pttTileC->x0;
				int32_t i32W = CJpegInt::int_ceildiv( piImage->x1 - piImage->x0, piImage->comps[i32CompNo].dx );
				
				// Extract tile data.
				if ( ptTcdTcp->tccps[i32CompNo].qmfbid == 1 ) {
					for ( i32Y = pttTileC->y0; i32Y < pttTileC->y1; i32Y++ ) {
						// Start of the source tile scanline.
						int32_t * pi32Data = &piImage->comps[i32CompNo].data[(pttTileC->x0-i32OffsetX)+(i32Y-i32OffsetY)*i32W];
						// Start of the destination tile scanline.
						int32_t * pi32TileData = &pttTileC->data[(i32Y-pttTileC->y0)*i32Tw];
						for ( i32X = pttTileC->x0; i32X < pttTileC->x1; i32X++ ) {
							(*pi32TileData++) = (*pi32Data++) - i32Adjust;
						}
					}
				}
				else if ( ptTcdTcp->tccps[i32CompNo].qmfbid == 0 ) {
					for ( i32Y = pttTileC->y0; i32Y < pttTileC->y1; i32Y++ ) {
						// Start of the source tile scanline.
						int32_t * pi32Data = &piImage->comps[i32CompNo].data[(pttTileC->x0-i32OffsetX)+(i32Y-i32OffsetY)*i32W];
						// Start of the destination tile scanline.
						int32_t * pi32TileData = &pttTileC->data[(i32Y-pttTileC->y0)*i32Tw];
						for ( i32X = pttTileC->x0; i32X < pttTileC->x1; i32X++ ) {
							(*pi32TileData++) = ((*pi32Data++) - i32Adjust) << 11;
						}
						
					}
				}
			}
			
			//----------------MCT-------------------
			if ( ptTcdTcp->mct ) {
				int32_t i32Samples = ( pttTile->comps[0].x1 - pttTile->comps[0].x0 ) * ( pttTile->comps[0].y1 - pttTile->comps[0].y0 );
				if ( ptTcdTcp->tccps[0].qmfbid == 0 ) {
					CMct::mct_encode_real( pttTile->comps[0].data, pttTile->comps[1].data, pttTile->comps[2].data, i32Samples );
				}
				else {
					CMct::mct_encode( pttTile->comps[0].data, pttTile->comps[1].data, pttTile->comps[2].data, i32Samples );
				}
			}
			
			//----------------DWT---------------------
			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
				if ( ptTcdTcp->tccps[i32CompNo].qmfbid == 1 ) {
					CDwt::dwt_encode( pttTileC );
				}
				else if ( ptTcdTcp->tccps[i32CompNo].qmfbid == 0 ) {
					CDwt::dwt_encode_real( pttTileC );
				}
			}
			
			//------------------TIER1-----------------
			ptT1 = CT1::t1_create( _ptTcd->cinfo );
			CT1::t1_encode_cblks( ptT1, pttTile, ptTcdTcp );
			CT1::t1_destroy( ptT1 );
			
			//-----------RATE-ALLOCATE------------------
			// INDEX.
			if ( _pciInfoC ) {
				_pciInfoC->index_write = 0;
			}
			if ( pcCp->disto_alloc || pcCp->fixed_quality ) {	// Fixed_quality.
				// Normal Rate/distortion allocation.
				tcd_rateallocate( _ptTcd, _pui8Dest, _i32Len, _pciInfoC );
			}
			else {
				// Fixed layer allocation.
				tcd_rateallocate_fixed( _ptTcd );
			}
		}
		//--------------TIER2------------------

		// INDEX.
		if ( _pciInfoC ) {
			_pciInfoC->index_write = 1;
		}

		ptT2 = CT2::t2_create( _ptTcd->cinfo, piImage, pcCp );
		i32L = CT2::t2_encode_packets( ptT2, _i32TileNo, pttTile, ptTcdTcp->numlayers, _pui8Dest, _i32Len, _pciInfoC, _ptTcd->tp_num, _ptTcd->tp_pos, _ptTcd->cur_pino, CJ2k::FINAL_PASS, _ptTcd->cur_totnum_tp );
		CT2::t2_destroy( ptT2 );
		
		//---------------CLEAN-------------------
		if ( _ptTcd->cur_tp_num == _ptTcd->cur_totnum_tp - 1 ) {
			tTimer.Update( false );
			_ptTcd->encoding_time = tTimer.GetCurMicros() * (1.0 / 1000000.0) - _ptTcd->encoding_time;
			CEvent::opj_event_msg( _ptTcd->cinfo, EVT_INFO, "- Tile encoded in %f seconds.\r\n", _ptTcd->encoding_time );

			// Cleaning memory.
			for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
				opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
				CMemLib::Free( pttTileC->data );
			}
		}

		return i32L;
	}

	/**
	 * Decode a tile from a buffer into a raw piImage.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _pui8Src Source buffer.
	 * \param _i32Len Length of source buffer.
	 * \param _i32TileNo Number that identifies one of the tiles to be decoded.
	 * \param _pciInfoC Codestream information structure.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	LSBOOL LSE_CALL CTcd::tcd_decode_tile( opj_tcd_t * _ptTcd, uint8_t * _pui8Src, int32_t _i32Len, int32_t _i32TileNo, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC ) {
		int32_t i32L;
		int32_t i32CompNo;
		int32_t i32Eof = 0;
		double dTileTime, dT1Time, dDwtTime;
		opj_tcd_tile_t * pttTile = NULL;

		CT1::opj_t1_t * ptT1 = NULL;		// T1 component.
		CT2::opj_t2_t * ptT2 = NULL;		// T2 component.
		
		_ptTcd->tcd_tileno = _i32TileNo;
		_ptTcd->tcd_tile = &(_ptTcd->tcd_image->tiles[_i32TileNo]);
		_ptTcd->tcp = &(_ptTcd->cp->tcps[_i32TileNo]);
		pttTile = _ptTcd->tcd_tile;
		
		CTime tTimer;
		tTimer.Update( false );
		dTileTime = tTimer.GetCurMicros() * (1.0 / 1000000.0);	// Time needed to decode a tile.
		CEvent::opj_event_msg( _ptTcd->cinfo, EVT_INFO, "Tile %d of %d.\r\n", _i32TileNo + 1, _ptTcd->cp->tw * _ptTcd->cp->th );

		if ( _pciInfoC ) {
			int32_t i32ResNo, i32NumPrec = 0;
			for ( i32CompNo = 0; i32CompNo < _pciInfoC->numcomps; i32CompNo++ ) {
				CJ2k::opj_tcp_t * ptTcp = &_ptTcd->cp->tcps[0];
				CJ2k::opj_tccp_t * ptTccp = &ptTcp->tccps[i32CompNo];
				opj_tcd_tilecomp_t * pttTileIdx = &pttTile->comps[i32CompNo];	
				for ( i32ResNo = 0; i32ResNo < pttTileIdx->numresolutions; i32ResNo++ ) {
					opj_tcd_resolution_t * ptrResIdx = &pttTileIdx->resolutions[i32ResNo];
					_pciInfoC->tile[_i32TileNo].pw[i32ResNo] = ptrResIdx->pw;
					_pciInfoC->tile[_i32TileNo].ph[i32ResNo] = ptrResIdx->ph;
					i32NumPrec += ptrResIdx->pw * ptrResIdx->ph;
					if ( ptTccp->csty & J2K_CP_CSTY_PRT ) {
						_pciInfoC->tile[_i32TileNo].pdx[i32ResNo] = ptTccp->prcw[i32ResNo];
						_pciInfoC->tile[_i32TileNo].pdy[i32ResNo] = ptTccp->prch[i32ResNo];
					}
					else {
						_pciInfoC->tile[_i32TileNo].pdx[i32ResNo] = 15;
						_pciInfoC->tile[_i32TileNo].pdx[i32ResNo] = 15;
					}
				}
			}
			_pciInfoC->tile[_i32TileNo].packet = static_cast<CJpegCommonStructs::opj_packet_info_t *>(CMemLib::MAlloc( static_cast<uint32_t>(_pciInfoC->numlayers * i32NumPrec) * sizeof( CJpegCommonStructs::opj_packet_info_t ) ));
			_pciInfoC->packno = 0;
		}
		
		//--------------TIER2------------------
		ptT2 = CT2::t2_create( _ptTcd->cinfo, _ptTcd->image, _ptTcd->cp );
		i32L = CT2::t2_decode_packets( ptT2, _pui8Src, _i32Len, _i32TileNo, pttTile, _pciInfoC );
		CT2::t2_destroy( ptT2 );

		if ( i32L == -999 ) {
			i32Eof = 1;
			CEvent::opj_event_msg( _ptTcd->cinfo, EVT_ERROR, "CTcd::tcd_decode_tile: Incomplete bistream.\r\n" );
		}
		
		//------------------TIER1-----------------
		tTimer.Update( false );
		dT1Time = tTimer.GetCurMicros() * (1.0 / 1000000.0);	// Time needed to decode a tile.
		ptT1 = CT1::t1_create( _ptTcd->cinfo );
		for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; ++i32CompNo ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
			// The +3 is headroom required by the vectorized DWT.
			pttTileC->data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(((pttTileC->x1 - pttTileC->x0) * (pttTileC->y1 - pttTileC->y0)) + 3) * sizeof( int32_t ) ));
			CT1::t1_decode_cblks( ptT1, pttTileC, &_ptTcd->tcp->tccps[i32CompNo] );
		}
		CT1::t1_destroy( ptT1 );
		tTimer.Update( false );
		dT1Time = tTimer.GetCurMicros() * (1.0 / 1000000.0) - dT1Time;
		CEvent::opj_event_msg( _ptTcd->cinfo, EVT_INFO, "- Tier-1 took %f seconds.\r\n", dT1Time );
		
		//----------------DWT---------------------
		tTimer.Update( false );
		dDwtTime = tTimer.GetCurMicros() * (1.0 / 1000000.0);	// Time needed to decode a tile.
		for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
			int32_t i32NumResToDecode;

			if ( _ptTcd->cp->reduce != 0 ) {
				_ptTcd->image->comps[i32CompNo].resno_decoded = pttTile->comps[i32CompNo].numresolutions - _ptTcd->cp->reduce - 1;
				if ( _ptTcd->image->comps[i32CompNo].resno_decoded < 0 ) {				
					CEvent::opj_event_msg( _ptTcd->cinfo, EVT_ERROR, "Error decoding tile. The number of resolutions to remove [%d+1] is higher than the number "
						" of resolutions in the original codestream [%d].\r\nModify the cp_reduce parameter.\r\n", _ptTcd->cp->reduce, pttTile->comps[i32CompNo].numresolutions );
					return false;
				}
			}

			i32NumResToDecode = _ptTcd->image->comps[i32CompNo].resno_decoded + 1;
			if ( i32NumResToDecode > 0 ) {
				if ( _ptTcd->tcp->tccps[i32CompNo].qmfbid == 1 ) {
					CDwt::dwt_decode( pttTileC, i32NumResToDecode );
				}
				else {
					CDwt::dwt_decode_real( pttTileC, i32NumResToDecode );
				}
			}
		}
		tTimer.Update( false );
		dDwtTime = (tTimer.GetCurMicros() * (1.0 / 1000000.0)) - dDwtTime;
		CEvent::opj_event_msg( _ptTcd->cinfo, EVT_INFO, "- DWT took %f seconds.\r\n", dDwtTime );

		//----------------MCT-------------------

		if ( _ptTcd->tcp->mct ) {
			int32_t i32N = (pttTile->comps[0].x1 - pttTile->comps[0].x0) * (pttTile->comps[0].y1 - pttTile->comps[0].y0);
			if ( _ptTcd->tcp->tccps[0].qmfbid == 1 ) {
				CMct::mct_decode( 
					pttTile->comps[0].data, 
					pttTile->comps[1].data, 
					pttTile->comps[2].data, 
					i32N );
			}
			else {
				CMct::mct_decode_real( 
					reinterpret_cast<float *>(pttTile->comps[0].data), 
					reinterpret_cast<float *>(pttTile->comps[1].data),
					reinterpret_cast<float *>(pttTile->comps[2].data),
					i32N );
			}
		}

		//---------------TILE-------------------

		for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; ++i32CompNo ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
			CJpegImage::opj_image_comp_t * picImageC = &_ptTcd->image->comps[i32CompNo];
			opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[picImageC->resno_decoded];
			int32_t i32Adjust = picImageC->sgnd ? 0 : 1 << ( picImageC->prec - 1 );
			int32_t i32Min = picImageC->sgnd ? -(1 << (picImageC->prec - 1)) : 0;
			int32_t i32Max = picImageC->sgnd ? (1 << (picImageC->prec - 1)) - 1 : (1 << picImageC->prec) - 1;

			int32_t i32Tw = pttTileC->x1 - pttTileC->x0;
			int32_t i32W = picImageC->w;

			int32_t i32OffsetX = CJpegInt::int_ceildivpow2( picImageC->x0, picImageC->factor );
			int32_t i32OffsetY = CJpegInt::int_ceildivpow2( picImageC->y0, picImageC->factor );

			int32_t I, J;
			if ( !picImageC->data ) {
				picImageC->data = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(picImageC->w * picImageC->h) * sizeof( int32_t ) ));
			}
			if ( _ptTcd->tcp->tccps[i32CompNo].qmfbid == 1 ) {
				for ( J = ptrRes->y0; J < ptrRes->y1; ++J ) {
					for ( I = ptrRes->x0; I < ptrRes->x1; ++I ) {
						int32_t i32V = pttTileC->data[I-ptrRes->x0+(J-ptrRes->y0)*i32Tw];
						i32V += i32Adjust;
						picImageC->data[(I-i32OffsetX)+(J-i32OffsetY)*i32W] = CJpegInt::int_clamp( i32V, i32Min, i32Max );
					}
				}
			}
			else {
				for ( J = ptrRes->y0; J < ptrRes->y1; ++J ) {
					for ( I = ptrRes->x0; I < ptrRes->x1; ++I ) {
						float fTemp = reinterpret_cast<float *>(pttTileC->data)[I-ptrRes->x0+(J-ptrRes->y0)*i32Tw];
						int32_t i32V = static_cast<int32_t>(CStd::RoundInt( fTemp ));
						i32V += i32Adjust;
						picImageC->data[(I-i32OffsetX)+(J-i32OffsetY)*i32W] = CJpegInt::int_clamp( i32V, i32Min, i32Max );
					}
				}
			}
			CMemLib::Free( pttTileC->data );
		}

		tTimer.Update( false );
		dTileTime = tTimer.GetCurMicros() * (1.0 / 1000000.0) - dTileTime;	// Time needed to decode a tile.
		CEvent::opj_event_msg( _ptTcd->cinfo, EVT_INFO, "- Tile decoded in %f seconds.\r\n", dTileTime );

		if ( i32Eof ) {
			return false;
		}
		
		return true;
	}

	/**
	 * Free the memory allocated for decoding.
	 *
	 * \param _ptTcd TCD handle.
	 */
	void LSE_CALL CTcd::tcd_free_decode( opj_tcd_t * _ptTcd ) {
		opj_tcd_image_t * ptiTcdImage = _ptTcd->tcd_image;	
		CMemLib::Free( ptiTcdImage->tiles );
	}

	/**
	 * Undocumented OpenJpeg Library function.
	 *
	 * \param _ptTcd TCD handle.
	 * \param _i32TileNo Undocumented OpenJpeg Library parameter.
	 */
	void LSE_CALL CTcd::tcd_free_decode_tile( opj_tcd_t * _ptTcd, int32_t _i32TileNo ) {
		int32_t i32CompNo, i32Tcp, i32BandNo, i32PrecNo;

		opj_tcd_image_t * ptiTcdImage = _ptTcd->tcd_image;

		opj_tcd_tile_t * pttTile = &ptiTcdImage->tiles[_i32TileNo];
		for ( i32CompNo = 0; i32CompNo < pttTile->numcomps; i32CompNo++ ) {
			opj_tcd_tilecomp_t * pttTileC = &pttTile->comps[i32CompNo];
			for ( i32Tcp = 0; i32Tcp < pttTileC->numresolutions; i32Tcp++ ) {
				opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32Tcp];
				for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
					opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
					for ( i32PrecNo = 0; i32PrecNo < ptrRes->ph * ptrRes->pw; i32PrecNo++ ) {
						opj_tcd_precinct_t * ptpPrec = &ptbBand->precincts[i32PrecNo];
						if ( ptpPrec->imsbtree != NULL ) { CTgt::tgt_destroy( ptpPrec->imsbtree ); }
						if ( ptpPrec->incltree != NULL ) { CTgt::tgt_destroy( ptpPrec->incltree ); }
					}
					CMemLib::Free( ptbBand->precincts );
				}
			}
			CMemLib::Free( pttTileC->resolutions );
		}
		CMemLib::Free( pttTile->comps );
	}

}	// namespace lsi
