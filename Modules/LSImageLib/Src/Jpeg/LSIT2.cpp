/*
 * Copyright (pui8C) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (pui8C) 2002-2007, Professor Benoit Macq
 * Copyright (pui8C) 2001-2003, David Janssens
 * Copyright (pui8C) 2002-2003, Yannick Verschueren
 * Copyright (pui8C) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (pui8C) 2005, Herve Drolon, FreeImage Team
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

#include "LSIT2.h"
#include "LSIEvent.h"
#include "LSIJpegInt.h"


namespace lsi {

	// == Functions.
	/**
	 * Encode the packets of a tile to a destination buffer.
	 *
	 * \param _ptT2 T2 handle.
	 * \param _i32TileNo Number of the tile encoded.
	 * \param _pttTile The tile for which to write the packets.
	 * \param _i32MaxLayers Maximum number of layers.
	 * \param _pui8Dest The destination buffer.
	 * \param _i32Len The length of the destination buffer.
	 * \param _pciInfoC Codestream information structure.
	 * \param _i32TpNum Tile part number of the current tile.
	 * \param _i32TpPos The position of the tile part flag in the progression order.
	 * \param _i32PiNo Undocumented OpenJpeg Library parameter.
	 * \param _tmT2Mode If 0 we are in the threshold calculation. If 1 we are in the final ptpPass.
	 * \param _i32CurTotalTp The total number of tile parts in the current tile.
	 * \return Undocumented OpenJpeg Library return.
	 */
	int32_t LSE_CALL CT2::t2_encode_packets( opj_t2_t * _ptT2, int32_t _i32TileNo, CTcd::opj_tcd_tile_t * _pttTile, int32_t _i32MaxLayers, uint8_t * _pui8Dest, int32_t _i32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC, int32_t _i32TpNum, int32_t _i32TpPos, int32_t _i32PiNo, CJ2k::J2K_T2_MODE _tmT2Mode, int32_t _i32CurTotalTp ) {
		uint8_t * pui8C = _pui8Dest;
		int32_t i32E = 0;
		int32_t i32CompNo;
		CPi::opj_pi_iterator_t * ppiPi = NULL;
		int32_t i32Poc;
		CJpegImage::opj_image_t * piImage = _ptT2->image;
		CJ2k::opj_cp_t * pcCp = _ptT2->cp;
		CJ2k::opj_tcp_t * ptTcp = &pcCp->tcps[_i32TileNo];
		int32_t i32PocNo = pcCp->cinema == CINEMA4K_24 ? 2: 1;
		int32_t i32MaxComp = pcCp->max_comp_size > 0 ? piImage->numcomps : 1;
		
		ppiPi = CPi::pi_initialise_encode( piImage, pcCp, _i32TileNo, _tmT2Mode );
		if ( !ppiPi ) { return -999; }
		
		if ( _tmT2Mode == CJ2k::THRESH_CALC ) { // Calculating threshold.
			for ( i32CompNo = 0; i32CompNo < i32MaxComp; i32CompNo++ ) {
				for ( i32Poc = 0; i32Poc < i32PocNo ; i32Poc++ ) {
					int32_t i32CompLen = 0;
					int32_t i32TpNum = i32CompNo;
					if ( CPi::pi_create_encode( ppiPi, pcCp, _i32TileNo, i32Poc, i32TpNum, _i32TpPos, _tmT2Mode, _i32CurTotalTp ) ) {
						CEvent::opj_event_msg( _ptT2->cinfo, EVT_ERROR, "Error initializing packet iterator.\r\n" );
						CPi::pi_destroy( ppiPi, pcCp, _i32TileNo );
						return -999;
					}
					while ( CPi::pi_next( &ppiPi[i32Poc] ) ) {
						if ( ppiPi[i32Poc].layno < _i32MaxLayers ) {
							i32E = t2_encode_packet( _pttTile, &pcCp->tcps[_i32TileNo], &ppiPi[i32Poc], pui8C, static_cast<int32_t>(_pui8Dest + _i32Len - pui8C), _pciInfoC, _i32TileNo );
							i32CompLen = i32CompLen + i32E;
							if ( i32E == -999 ) {
								break;
							}
							else {
								pui8C += i32E;
							}
						}
					}
					if ( i32E == -999 ) { break; }
					if ( pcCp->max_comp_size ) {
						if ( i32CompLen > pcCp->max_comp_size ) {
							i32E = -999;
							break;
						}
					}
				}
				if ( i32E == -999 ) { break; }
			}
		}
		else {	// _tmT2Mode == FINAL_PASS.
			CPi::pi_create_encode( ppiPi, pcCp, _i32TileNo, _i32PiNo, _i32TpNum, _i32TpPos, _tmT2Mode, _i32CurTotalTp );
			while ( CPi::pi_next( &ppiPi[_i32PiNo] ) ) {
				if ( ppiPi[_i32PiNo].layno < _i32MaxLayers ) {
					i32E = t2_encode_packet( _pttTile, &pcCp->tcps[_i32TileNo], &ppiPi[_i32PiNo], pui8C, static_cast<int32_t>(_pui8Dest + _i32Len - pui8C), _pciInfoC, _i32TileNo );
					if ( i32E == -999 ) {
						break;
					} 
					else {
						pui8C += i32E;
					}
					if ( _pciInfoC ) {
						if ( _pciInfoC->index_write ) {
							CJpegCommonStructs::opj_tile_info_t * ptiInfoTl = &_pciInfoC->tile[_i32TileNo];
							CJpegCommonStructs::opj_packet_info_t * ppiInfoPk = &ptiInfoTl->packet[_pciInfoC->packno];
							if ( !_pciInfoC->packno ) {
								ppiInfoPk->start_pos = ptiInfoTl->end_header + 1;
							}
							else {
								ppiInfoPk->start_pos = ((pcCp->tp_on | ptTcp->POC) && ppiInfoPk->start_pos) ? ppiInfoPk->start_pos : ptiInfoTl->packet[_pciInfoC->packno-1].end_pos + 1;
							}
							ppiInfoPk->end_pos = ppiInfoPk->start_pos + i32E - 1;
							ppiInfoPk->end_ph_pos += ppiInfoPk->start_pos - 1;	// End of packet header which now only represents the distance to start of packet is incremented by value of start of packet.
						}
						
						_pciInfoC->packno++;
					}
					_pttTile->packno++;
				}
			}
		}
		
		CPi::pi_destroy( ppiPi, pcCp, _i32TileNo );
		
		if ( i32E == -999 ) {
			return i32E;
		}

		return static_cast<int32_t>(pui8C - _pui8Dest);
	}

	/**
	 * Decode the packets of a tile from a source buffer.
	 *
	 * \param _ptT2 T2 handle
	 * \param _pui8Src the source buffer
	 * \param _ui32Len length of the source buffer
	 * \param _i32TileNo number that identifies the tile for which to decode the packets
	 * \param _pttTile tile for which to decode the packets
	 * \param _pciInfoC Codestream information structure.
	 * \return Undocumented OpenJpeg Library return.
	 */
	int32_t LSE_CALL CT2::t2_decode_packets( opj_t2_t * _ptT2, uint8_t * _pui8Src, int32_t _ui32Len, int32_t _i32TileNo, CTcd::opj_tcd_tile_t * _pttTile, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC ) {
		uint8_t * pui8C = _pui8Src;
		CPi::opj_pi_iterator_t * ppiPi;
		int32_t i32PiNo, i32E = 0;
		int32_t i32N = 0, i32CurTp = 0;
		int32_t i32TpStartPacketNo;

		CJpegImage::opj_image_t * piImage = _ptT2->image;
		CJ2k::opj_cp_t * pcCp = _ptT2->cp;
		
		// Create a packet iterator.
		ppiPi = CPi::pi_create_decode( piImage, pcCp, _i32TileNo );
		if ( !ppiPi ) {
			return -999;
		}

		i32TpStartPacketNo = 0;
		
		for ( i32PiNo = 0; i32PiNo <= pcCp->tcps[_i32TileNo].numpocs; i32PiNo++ ) {
			while ( CPi::pi_next( &ppiPi[i32PiNo] ) ) {
				if ( pcCp->layer == 0 || pcCp->layer >= (ppiPi[i32PiNo].layno + 1) ) {
					CJpegCommonStructs::opj_packet_info_t * ppiPackInfo;
					if ( _pciInfoC ) {
						ppiPackInfo = &_pciInfoC->tile[_i32TileNo].packet[_pciInfoC->packno];
					}
					else {
						ppiPackInfo = NULL;
					}
					i32E = t2_decode_packet( _ptT2, pui8C, static_cast<int32_t>(_pui8Src + _ui32Len - pui8C), _pttTile, &pcCp->tcps[_i32TileNo], &ppiPi[i32PiNo], ppiPackInfo );
				}
				else {
					i32E = 0;
				}
				if ( i32E == -999 ) { return -999; }
				// Progression in resolution.
				piImage->comps[ppiPi[i32PiNo].compno].resno_decoded =	
					(i32E > 0) ? 
						CJpegInt::int_max( ppiPi[i32PiNo].resno, piImage->comps[ppiPi[i32PiNo].compno].resno_decoded ) :
						piImage->comps[ppiPi[i32PiNo].compno].resno_decoded;
				i32N++;

				if ( _pciInfoC ) {
					CJpegCommonStructs::opj_tile_info_t * ptiInfoTl = &_pciInfoC->tile[_i32TileNo];
					CJpegCommonStructs::opj_packet_info_t * ppiInfoPk = &ptiInfoTl->packet[_pciInfoC->packno];
					if ( !_pciInfoC->packno ) {
						ppiInfoPk->start_pos = ptiInfoTl->end_header + 1;
					}
					else if ( ptiInfoTl->packet[_pciInfoC->packno-1].end_pos >= ( int32_t )_pciInfoC->tile[_i32TileNo].tp[i32CurTp].tp_end_pos ) { // New tile part.
						ptiInfoTl->tp[i32CurTp].tp_numpacks = _pciInfoC->packno - i32TpStartPacketNo; // Number of packets in previous tile part.
						i32TpStartPacketNo = _pciInfoC->packno;
						i32CurTp++;
						ppiInfoPk->start_pos = _pciInfoC->tile[_i32TileNo].tp[i32CurTp].tp_end_header+1;
					}
					else {
						ppiInfoPk->start_pos = (pcCp->tp_on && ppiInfoPk->start_pos) ? ppiInfoPk->start_pos : ptiInfoTl->packet[_pciInfoC->packno-1].end_pos + 1;
					}
					ppiInfoPk->end_pos = ppiInfoPk->start_pos + i32E - 1;
					ppiInfoPk->end_ph_pos += ppiInfoPk->start_pos - 1;	// End of packet header which now only represents the distance to start of packet is incremented by value of start of packet.
					_pciInfoC->packno++;
				}
				
				if ( i32E == -999 ) { break; }
				else { pui8C += i32E; }			
			}
		}
		if ( _pciInfoC ) {
			_pciInfoC->tile[_i32TileNo].tp[i32CurTp].tp_numpacks = _pciInfoC->packno - i32TpStartPacketNo; // Number of packets in last _pttTile-part
		}

		// Don't forget to release ppiPi.
		CPi::pi_destroy( ppiPi, pcCp, _i32TileNo );
		
		if ( i32E == -999 ) { return i32E; }
		
		return static_cast<int32_t>(pui8C - _pui8Src);
	}

	/**
	 * Creates a T2 handle.
	 *
	 * \param _cpInfo Codec context info.
	 * \param _piImage Source or destination image.
	 * \param _pcCp Image coding parameters.
	 * \return Returns a new T2 handle if successful, NULL otherwise.
	 */
	CT2::opj_t2_t * LSE_CALL CT2::t2_create( CJpegCommonStructs::opj_common_ptr _cpInfo, CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp ) {
		// Create the tcd structure.
		opj_t2_t * ptT2 = static_cast<opj_t2_t *>(CMemLib::MAlloc( sizeof( opj_t2_t ) ));
		if ( !ptT2 ) { return NULL; }
		ptT2->cinfo = _cpInfo;
		ptT2->image = _piImage;
		ptT2->cp = _pcCp;

		return ptT2;
	}

	/**
	 * Destroy a T2 handle.
	 *
	 * \param _ptT2 T2 handle to destroy.
	 */
	void LSE_CALL CT2::t2_destroy( opj_t2_t * _ptT2 ) {
		CMemLib::Free( _ptT2 );
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
	 * \param _i32N Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT2::t2_putcommacode( CBio::opj_bio_t * _pbBio, int32_t _i32N ) {
		while ( --_i32N >= 0 ) {
			CBio::bio_write( _pbBio, 1, 1 );
		}
		CBio::bio_write( _pbBio, 0, 1 );
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CT2::t2_getcommacode( CBio::opj_bio_t * _pbBio ) {
		int32_t i32N;
		for ( i32N = 0; CBio::bio_read( _pbBio, 1 ); i32N++ ) {}
		return i32N;
	}

	/**
	 * Variable length code for signalling delta Zil (truncation point).
	 *
	 * \param _pbBio Bit Input/Output component.
	 * \param _i32N delta Zil.
	 */
	void LSE_CALL CT2::t2_putnumpasses( CBio::opj_bio_t * _pbBio, int32_t _i32N ) {
		if ( _i32N == 1 ) {
			CBio::bio_write( _pbBio, 0, 1 );
		}
		else if ( _i32N == 2 ) {
			CBio::bio_write( _pbBio, 2, 2 );
		}
		else if ( _i32N <= 5 ) {
			CBio::bio_write( _pbBio, 0xC | (_i32N - 3), 4 );
		}
		else if ( _i32N <= 36 ) {
			CBio::bio_write( _pbBio, 0x1E0 | (_i32N - 6), 9 );
		}
		else if ( _i32N <= 164 ) {
			CBio::bio_write( _pbBio, 0xFF80 | (_i32N - 37), 16 );
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pbBio Undocumented parameter from the OpenJpeg Library.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CT2::t2_getnumpasses( CBio::opj_bio_t * _pbBio ) {
		int32_t i32N;
		if ( !CBio::bio_read( _pbBio, 1 ) ) { return 1; }
		if ( !CBio::bio_read( _pbBio, 1 ) ) { return 2; }
		if ( (i32N = CBio::bio_read( _pbBio, 2 )) != 3 ) { return (3 + i32N); }
		if ( (i32N = CBio::bio_read( _pbBio, 5 )) != 31 ) { return (6 + i32N); }
		return (37 + CBio::bio_read( _pbBio, 7 ));
	}

	/**
	 * Encode a packet of a tile to a destination buffer.
	 *
	 * \param _pttTile Tile for which to write the packets.
	 * \param _ptTcp Tile coding parameters.
	 * \param _ppiPi Packet identity.
	 * \param _pui8Dest Destination buffer.
	 * \param _ui32Len Length of the destination buffer.
	 * \param _pciInfoC Codestream information structure.
	 * \param _i32TileNo Number of the tile encoded.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CT2::t2_encode_packet( CTcd::opj_tcd_tile_t * _pttTile, CJ2k::opj_tcp_t * _ptTcp, CPi::opj_pi_iterator_t * _ppiPi, uint8_t * _pui8Dest, int32_t _ui32Len, CJpegCommonStructs::opj_codestream_info_t * _pciInfoC, int32_t _i32TileNo ) {
		int32_t i32BandNo, i32CBlockNo;
		uint8_t * pui8C = _pui8Dest;

		int32_t i32CompNo = _ppiPi->compno;		// Component value.
		int32_t i32ResNo = _ppiPi->resno;		// Resolution level value.
		int32_t i32PrecNo = _ppiPi->precno;		// Precinct value.
		int32_t i32LayNo = _ppiPi->layno;		// Quality layer value.

		CTcd::opj_tcd_tilecomp_t * pttTileC = &_pttTile->comps[i32CompNo];
		CTcd::opj_tcd_resolution_t * ptrRes = &pttTileC->resolutions[i32ResNo];
		
		CBio::opj_bio_t * pbBio = NULL;	// BIO component.
		
		if ( _ptTcp->csty & J2K_CP_CSTY_SOP ) {
			pui8C[0] = 255;
			pui8C[1] = 145;
			pui8C[2] = 0;
			pui8C[3] = 4;
			pui8C[4] = static_cast<uint8_t>((_pttTile->packno % 65536) / 256);
			pui8C[5] = static_cast<uint8_t>((_pttTile->packno % 65536) % 256);
			pui8C += 6;
		}
		
		if ( !i32LayNo ) {
			for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
				CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
				CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
				CTgt::tgt_reset( ptpPrc->incltree );
				CTgt::tgt_reset( ptpPrc->imsbtree );
				for ( i32CBlockNo = 0; i32CBlockNo < ptpPrc->cw * ptpPrc->ch; i32CBlockNo++ ) {
					CTcd::opj_tcd_cblk_enc_t * ptceCBlock = &ptpPrc->cblks.enc[i32CBlockNo];
					ptceCBlock->numpasses = 0;
					CTgt::tgt_setvalue( ptpPrc->imsbtree, i32CBlockNo, ptbBand->numbps - ptceCBlock->numbps );
				}
			}
		}
		
		pbBio = CBio::bio_create();
		CBio::bio_init_enc( pbBio, pui8C, _ui32Len );
		CBio::bio_write( pbBio, 1, 1 );		// Empty header bit.
		
		// Writing Packet header.
		for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
			CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
			CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
			for ( i32CBlockNo = 0; i32CBlockNo < ptpPrc->cw * ptpPrc->ch; i32CBlockNo++ ) {
				CTcd::opj_tcd_cblk_enc_t * ptceCBlock = &ptpPrc->cblks.enc[i32CBlockNo];
				CTcd::opj_tcd_layer_t * ptlLayer = &ptceCBlock->layers[i32LayNo];
				if ( !ptceCBlock->numpasses && ptlLayer->numpasses ) {
					CTgt::tgt_setvalue( ptpPrc->incltree, i32CBlockNo, i32LayNo );
				}
			}
			for ( i32CBlockNo = 0; i32CBlockNo < ptpPrc->cw * ptpPrc->ch; i32CBlockNo++ ) {
				CTcd::opj_tcd_cblk_enc_t * ptceCBlock = &ptpPrc->cblks.enc[i32CBlockNo];
				CTcd::opj_tcd_layer_t * ptlLayer = &ptceCBlock->layers[i32LayNo];
				int32_t i32Increment = 0;
				int32_t i32NumP = 0;
				int32_t i32Len = 0, i32PassNo;
				// ptceCBlock inclusion bits.
				if ( !ptceCBlock->numpasses ) {
					CTgt::tgt_encode( pbBio, ptpPrc->incltree, i32CBlockNo, i32LayNo + 1 );
				}
				else {
					CBio::bio_write( pbBio, ptlLayer->numpasses != 0, 1 );
				}
				// If ptceCBlock is not included, go to the next cblock.
				if ( !ptlLayer->numpasses ) {
					continue;
				}
				// If this is the first instance of cblock, zero bit-planes information.
				if ( !ptceCBlock->numpasses ) {
					ptceCBlock->numlenbits = 3;
					CTgt::tgt_encode( pbBio, ptpPrc->imsbtree, i32CBlockNo, 999 );
				}
				// Number of coding passes included.
				t2_putnumpasses( pbBio, ptlLayer->numpasses );
				
				// Computation of the increase of the length indicator and insertion in the header.
				for ( i32PassNo = ptceCBlock->numpasses; i32PassNo < ptceCBlock->numpasses + ptlLayer->numpasses; i32PassNo++ ) {
					CTcd::opj_tcd_pass_t * ptpPass = &ptceCBlock->passes[i32PassNo];
					i32NumP++;
					i32Len += ptpPass->len;
					if ( ptpPass->term || i32PassNo == (ptceCBlock->numpasses + ptlLayer->numpasses) - 1 ) {
						i32Increment = CJpegInt::int_max( i32Increment, CJpegInt::int_floorlog2( i32Len ) + 1 - (ptceCBlock->numlenbits + CJpegInt::int_floorlog2( i32NumP )) );
						i32Len = 0;
						i32NumP = 0;
					}
				}
				t2_putcommacode( pbBio, i32Increment );

				// Computation of the new length indicator.
				ptceCBlock->numlenbits += i32Increment;

				// Insertion of the codeword segment length.
				for ( i32PassNo = ptceCBlock->numpasses; i32PassNo < ptceCBlock->numpasses + ptlLayer->numpasses; i32PassNo++ ) {
					CTcd::opj_tcd_pass_t * ptpPass = &ptceCBlock->passes[i32PassNo];
					i32NumP++;
					i32Len += ptpPass->len;
					if ( ptpPass->term || i32PassNo == (ptceCBlock->numpasses + ptlLayer->numpasses) - 1 ) {
						CBio::bio_write( pbBio, i32Len, ptceCBlock->numlenbits + CJpegInt::int_floorlog2( i32NumP ) );
						i32Len = 0;
						i32NumP = 0;
					}
				}
			}
		}

		if ( CBio::bio_flush( pbBio ) ) {
			CBio::bio_destroy( pbBio );
			return -999;
		}

		pui8C += CBio::bio_numbytes( pbBio );
		CBio::bio_destroy( pbBio );
		
		if ( _ptTcp->csty & J2K_CP_CSTY_EPH ) {
			pui8C[0] = 255;
			pui8C[1] = 146;
			pui8C += 2;
		}

		// End of packet header position.  Currently only represents the distance to start of packet.
		//	Will be updated later by incrementing with packet start value.
		if ( _pciInfoC && _pciInfoC->index_write ) {
			CJpegCommonStructs::opj_packet_info_t * ppiInfoPk = &_pciInfoC->tile[_i32TileNo].packet[_pciInfoC->packno];
			ppiInfoPk->end_ph_pos = static_cast<int32_t>(pui8C - _pui8Dest);
		}
		
		// Writing the packet body.
		for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
			CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
			CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
			for ( i32CBlockNo = 0; i32CBlockNo < ptpPrc->cw * ptpPrc->ch; i32CBlockNo++ ) {
				CTcd::opj_tcd_cblk_enc_t * ptceCBlock = &ptpPrc->cblks.enc[i32CBlockNo];
				CTcd::opj_tcd_layer_t * ptlLayer = &ptceCBlock->layers[i32LayNo];
				if ( !ptlLayer->numpasses ) { continue; }
				if ( pui8C + ptlLayer->len > _pui8Dest + _ui32Len ) { return -999; }

				CStd::MemCpy( pui8C, ptlLayer->data, static_cast<uint32_t>(ptlLayer->len) );
				ptceCBlock->numpasses += ptlLayer->numpasses;
				pui8C += ptlLayer->len;
				if ( _pciInfoC && _pciInfoC->index_write ) {
					CJpegCommonStructs::opj_packet_info_t * ppiInfoPk = &_pciInfoC->tile[_i32TileNo].packet[_pciInfoC->packno];
					ppiInfoPk->disto += ptlLayer->disto;
					if ( _pciInfoC->D_max < ppiInfoPk->disto ) {
						_pciInfoC->D_max = ppiInfoPk->disto;
					}
				}
			}
		}
		
		return static_cast<int32_t>(pui8C - _pui8Dest);
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _ptcdCodeBlock Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Index Undocumented parameter from the OpenJpeg Library.
	 * \param _i32CodeBlockStY Undocumented parameter from the OpenJpeg Library.
	 * \param _i32First Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CT2::t2_init_seg( CTcd::opj_tcd_cblk_dec_t * _ptcdCodeBlock, int32_t _i32Index, int32_t _i32CodeBlockStY, int32_t _i32First ) {
		CTcd::opj_tcd_seg_t * ptsSeg;
		_ptcdCodeBlock->segs = static_cast<CTcd::opj_tcd_seg_t *>(CMemLib::MReAlloc( _ptcdCodeBlock->segs, (static_cast<uint32_t>(_i32Index) + 1) * sizeof( CTcd::opj_tcd_seg_t ) ));
		ptsSeg = &_ptcdCodeBlock->segs[_i32Index];
		ptsSeg->data = NULL;
		ptsSeg->dataindex = 0;
		ptsSeg->numpasses = 0;
		ptsSeg->len = 0;
		if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_TERMALL ) {
			ptsSeg->maxpasses = 1;
		}
		else if ( _i32CodeBlockStY & J2K_CCP_CBLKSTY_LAZY ) {
			if ( _i32First ) {
				ptsSeg->maxpasses = 10;
			}
			else {
				ptsSeg->maxpasses = (((ptsSeg - 1)->maxpasses == 1) || ((ptsSeg - 1)->maxpasses == 10)) ? 2 : 1;
			}
		}
		else {
			ptsSeg->maxpasses = 109;
		}
	}

	/**
	 * Decode a packet of a tile from a source buffer.
	 *
	 * \param _ptT2 T2 handle.
	 * \param _pui8Src Source buffer.
	 * \param _ui32Len Length of the source buffer.
	 * \param _pttTile Tile for which to write the packets.
	 * \param _ptTcp Tile coding parameters.
	 * \param _ppiPi Packet identity.
	 * \param _ppiPackInfo Packet information.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CT2::t2_decode_packet( opj_t2_t * _ptT2, uint8_t * _pui8Src, int32_t _ui32Len, CTcd::opj_tcd_tile_t * _pttTile,
		CJ2k::opj_tcp_t * _ptTcp, CPi::opj_pi_iterator_t * _ppiPi, CJpegCommonStructs::opj_packet_info_t * _ppiPackInfo ) {
		int32_t i32BandNo, i32CodeBlockNo;
		uint8_t * pui8C = _pui8Src;

		CJ2k::opj_cp_t * pcCp = _ptT2->cp;

		int32_t i32CompNo = _ppiPi->compno;		// Component value.
		int32_t i32ResNo = _ppiPi->resno;		// Resolution level value.
		int32_t i32PrecNo = _ppiPi->precno;		// Precinct value.
		int32_t i32LayNo = _ppiPi->layno;		// Quality layer value.

		CTcd::opj_tcd_resolution_t * ptrRes = &_pttTile->comps[i32CompNo].resolutions[i32ResNo];

		uint8_t * pui8Hd = NULL;
		int32_t i32Present;
		
		CBio::opj_bio_t * pbBio = NULL;			// BIO component.
		
		if ( i32LayNo == 0 ) {
			for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
				CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
				CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
				
				if ( (ptbBand->x1 - ptbBand->x0 == 0) || (ptbBand->y1 - ptbBand->y0 == 0) ) { continue; }
				
				CTgt::tgt_reset( ptpPrc->incltree );
				CTgt::tgt_reset( ptpPrc->imsbtree );
				for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
					CTcd::opj_tcd_cblk_dec_t * ptcdCodeBlock = &ptpPrc->cblks.dec[i32CodeBlockNo];
					ptcdCodeBlock->numsegs = 0;
				}
			}
		}
		
		// SOP markers.		
		if ( _ptTcp->csty & J2K_CP_CSTY_SOP ) {
			if ( (*pui8C) != 0xFF || (*(pui8C + 1) != 0x91) ) {
				CEvent::opj_event_msg( _ptT2->cinfo, EVT_WARNING, "Expected SOP marker.\r\n" );
			}
			else {
				pui8C += 6;
			}
		}
		
		// When the marker PPT/PPM is used the packet header is stored in PPT/PPM marker.
		// This part deals with this characteristic.
		//	step 1: Read packet header in the saved structure.
		//	step 2: Return to codestream for decoding.
		pbBio = CBio::bio_create();
		
		if ( pcCp->ppm == 1 ) {					// PPM.
			pui8Hd = pcCp->ppm_data;
			CBio::bio_init_dec( pbBio, pui8Hd, pcCp->ppm_len );
		}
		else if ( _ptTcp->ppt == 1 ) {			// PPT.
			pui8Hd = _ptTcp->ppt_data;
			CBio::bio_init_dec( pbBio, pui8Hd, _ptTcp->ppt_len );
		}
		else {									// Normal Case.
			pui8Hd = pui8C;
			CBio::bio_init_dec( pbBio, pui8Hd, static_cast<int32_t>(_pui8Src + _ui32Len - pui8Hd) );
		}
		
		i32Present = CBio::bio_read( pbBio, 1 );
		
		if ( !i32Present ) {
			CBio::bio_inalign( pbBio );
			pui8Hd += CBio::bio_numbytes( pbBio );
			CBio::bio_destroy( pbBio );
			
			// EPH markers.
			
			if ( _ptTcp->csty & J2K_CP_CSTY_EPH ) {
				if ( (*pui8Hd) != 0xFF || ((*(pui8Hd + 1)) != 0x92) ) {
					CStd::DebugPrintA( "CT2::t2_decode_packet(): Expected EPH marker\r.\n" );
				}
				else {
					pui8Hd += 2;
				}
			}

			// End of packet header position. Currently only represents the distance to start of packet.
			//	Will be updated later by incrementing with packet start value.
			if ( _ppiPackInfo ) {
				_ppiPackInfo->end_ph_pos = static_cast<int32_t>(pui8C - _pui8Src);
			}
			
			if ( pcCp->ppm == 1 ) {		// PPM case.
				pcCp->ppm_len += static_cast<int32_t>(pcCp->ppm_data - pui8Hd);
				pcCp->ppm_data = pui8Hd;
				return static_cast<int32_t>(pui8C - _pui8Src );
			}
			if ( _ptTcp->ppt == 1 ) {	// PPT case.
				_ptTcp->ppt_len += static_cast<int32_t>(_ptTcp->ppt_data - pui8Hd);
				_ptTcp->ppt_data = pui8Hd;
				return static_cast<int32_t>(pui8C - _pui8Src );
			}
			
			return static_cast<int32_t>(pui8Hd - _pui8Src);
		}
		
		for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
			CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
			CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
			
			if ( (ptbBand->x1 - ptbBand->x0 == 0) || (ptbBand->y1 - ptbBand->y0 == 0) ) { continue; }
			
			for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
				int32_t i32Included, i32Increment, i32N, i32SegNo;
				CTcd::opj_tcd_cblk_dec_t * ptcdCodeBlock = &ptpPrc->cblks.dec[i32CodeBlockNo];
				// If ptcdCodeBlock not yet included before, inclusion tagtree.
				if ( !ptcdCodeBlock->numsegs ) {
					i32Included = CTgt::tgt_decode( pbBio, ptpPrc->incltree, i32CodeBlockNo, i32LayNo + 1 );
					// Else one bit.
				}
				else {
					i32Included = CBio::bio_read( pbBio, 1 );
				}
				// If ptcdCodeBlock not included.
				if ( !i32Included ) {
					ptcdCodeBlock->numnewpasses = 0;
					continue;
				}
				// If ptcdCodeBlock not yet included, zero-bitplane tagtree.
				if ( !ptcdCodeBlock->numsegs ) {
					int32_t I, i32NumImsbs;
					for ( I = 0; !CTgt::tgt_decode( pbBio, ptpPrc->imsbtree, i32CodeBlockNo, I ); I++ ) {}
					i32NumImsbs = I - 1;
					ptcdCodeBlock->numbps = ptbBand->numbps - i32NumImsbs;
					ptcdCodeBlock->numlenbits = 3;
				}
				// Number of coding passes.
				ptcdCodeBlock->numnewpasses = t2_getnumpasses( pbBio );
				i32Increment = t2_getcommacode( pbBio );
				// Length indicator increment.
				ptcdCodeBlock->numlenbits += i32Increment;
				i32SegNo = 0;
				if ( !ptcdCodeBlock->numsegs ) {
					t2_init_seg( ptcdCodeBlock, i32SegNo, _ptTcp->tccps[i32CompNo].cblksty, 1 );
				}
				else {
					i32SegNo = ptcdCodeBlock->numsegs - 1;
					if ( ptcdCodeBlock->segs[i32SegNo].numpasses == ptcdCodeBlock->segs[i32SegNo].maxpasses ) {
						++i32SegNo;
						t2_init_seg( ptcdCodeBlock, i32SegNo, _ptTcp->tccps[i32CompNo].cblksty, 0 );
					}
				}
				i32N = ptcdCodeBlock->numnewpasses;
				
				do {
					ptcdCodeBlock->segs[i32SegNo].numnewpasses = CJpegInt::int_min( ptcdCodeBlock->segs[i32SegNo].maxpasses - ptcdCodeBlock->segs[i32SegNo].numpasses, i32N );
					ptcdCodeBlock->segs[i32SegNo].newlen = CBio::bio_read( pbBio, ptcdCodeBlock->numlenbits + CJpegInt::int_floorlog2( ptcdCodeBlock->segs[i32SegNo].numnewpasses ) );
					i32N -= ptcdCodeBlock->segs[i32SegNo].numnewpasses;
					if ( i32N > 0 ) {
						++i32SegNo;
						t2_init_seg( ptcdCodeBlock, i32SegNo, _ptTcp->tccps[i32CompNo].cblksty, 0 );
					}
				} while ( i32N > 0 );
			}
		}
		
		if ( CBio::bio_inalign( pbBio ) ) {
			CBio::bio_destroy( pbBio );
			return -999;
		}
		
		pui8Hd += CBio::bio_numbytes( pbBio );
		CBio::bio_destroy( pbBio );
		
		// EPH markers.
		if ( _ptTcp->csty & J2K_CP_CSTY_EPH ) {
			if ( (*pui8Hd) != 0xFF || ((*(pui8Hd + 1)) != 0x92) ) {
				CEvent::opj_event_msg( _ptT2->cinfo, EVT_ERROR, "Expected EPH marker.\r\n" );
				return -999;
			}
			else {
				pui8Hd += 2;
			}
		}

		// End of packet header position. Currently only represents the distance to start of packet.
		//	Will be updated later by incrementing with packet start value.
		if ( _ppiPackInfo ) {
			_ppiPackInfo->end_ph_pos = static_cast<int32_t>(pui8Hd - _pui8Src);
		}
		
		if ( pcCp->ppm == 1 ) {
			pcCp->ppm_len += static_cast<int32_t>(pcCp->ppm_data - pui8Hd);
			pcCp->ppm_data = pui8Hd;
		}
		else if ( _ptTcp->ppt == 1 ) {
			_ptTcp->ppt_len += static_cast<int32_t>(_ptTcp->ppt_data - pui8Hd);
			_ptTcp->ppt_data = pui8Hd;
		}
		else {
			pui8C = pui8Hd;
		}
		
		for ( i32BandNo = 0; i32BandNo < ptrRes->numbands; i32BandNo++ ) {
			CTcd::opj_tcd_band_t * ptbBand = &ptrRes->bands[i32BandNo];
			CTcd::opj_tcd_precinct_t * ptpPrc = &ptbBand->precincts[i32PrecNo];
			
			if ( (ptbBand->x1 - ptbBand->x0 == 0) || (ptbBand->y1 - ptbBand->y0 == 0) ) { continue; }
			
			for ( i32CodeBlockNo = 0; i32CodeBlockNo < ptpPrc->cw * ptpPrc->ch; i32CodeBlockNo++ ) {
				CTcd::opj_tcd_cblk_dec_t * ptcdCodeBlock = &ptpPrc->cblks.dec[i32CodeBlockNo];
				CTcd::opj_tcd_seg_t * ptsSeg = NULL;
				if ( !ptcdCodeBlock->numnewpasses ) { continue; }
				if ( !ptcdCodeBlock->numsegs ) {
					ptsSeg = &ptcdCodeBlock->segs[0];
					ptcdCodeBlock->numsegs++;
					ptcdCodeBlock->len = 0;
				}
				else {
					ptsSeg = &ptcdCodeBlock->segs[ptcdCodeBlock->numsegs-1];
					if ( ptsSeg->numpasses == ptsSeg->maxpasses ) {
						ptsSeg++;
						ptcdCodeBlock->numsegs++;
					}
				}
				
				do {
					if ( pui8C + ptsSeg->newlen > _pui8Src + _ui32Len ) {
						return -999;
					}
					
					ptcdCodeBlock->data = static_cast<uint8_t *>(CMemLib::MReAlloc( ptcdCodeBlock->data, (static_cast<uint32_t>(ptcdCodeBlock->len) + static_cast<uint32_t>(ptsSeg->newlen)) * sizeof( uint8_t * ) ));
					CStd::MemCpy( ptcdCodeBlock->data + ptcdCodeBlock->len, pui8C, static_cast<uint32_t>(ptsSeg->newlen) );
					if ( ptsSeg->numpasses == 0 ) {
						ptsSeg->data = &ptcdCodeBlock->data;
						ptsSeg->dataindex = ptcdCodeBlock->len;
					}
					pui8C += ptsSeg->newlen;
					ptcdCodeBlock->len += ptsSeg->newlen;
					ptsSeg->len += ptsSeg->newlen;
					ptsSeg->numpasses += ptsSeg->numnewpasses;
					ptcdCodeBlock->numnewpasses -= ptsSeg->numnewpasses;
					if ( ptcdCodeBlock->numnewpasses > 0 ) {
						ptsSeg++;
						ptcdCodeBlock->numsegs++;
					}
				} while ( ptcdCodeBlock->numnewpasses > 0 );
			}
		}
		
		return static_cast<int32_t>(pui8C - _pui8Src);
	}

}	// namespace lsi
