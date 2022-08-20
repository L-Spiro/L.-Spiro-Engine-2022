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

#include "LSIPi.h"
#include "LSIJpegInt.h"


namespace lsi {

	// == Functions.
	/**
	 * Creates a packet iterator for the encoder.
	 *
	 * \param _pImage Raw image for which the packets will be listed.
	 * \param _pcCp Coding parameters.
	 * \param _i32TileNo Number that identifies the tile for which to list the packets.
	 * \param _t2Mode If 0 we are in the threshold calculation.  If 1 we are in the final pass.
	 * \return Returns a packet iterator that points to the first packet of the tile.
	 * \see pi_destroy
	 */
	CPi::opj_pi_iterator_t * LSE_CALL CPi::pi_initialise_encode( CJpegImage::opj_image_t * _pImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, CJ2k::J2K_T2_MODE _t2Mode ) {
		int32_t i32P, i32Q, i32PiNo;
		int32_t i32CompNo, i32ResNo;
		int32_t i32MaxRes = 0;
		int32_t i32MaxPrec = 0;
		opj_pi_iterator_t * ppiPi = NULL;
		CJ2k::opj_tcp_t * ptTcp = NULL;
		CJ2k::opj_tccp_t * ptTccp = NULL;
		
		ptTcp = &_pcCp->tcps[_i32TileNo];

		ppiPi = static_cast<opj_pi_iterator_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ptTcp->numpocs + 1) * sizeof( opj_pi_iterator_t ) ));
		if ( !ppiPi ) { return NULL; }
		ppiPi->tp_on = _pcCp->tp_on;

		for ( i32PiNo = 0; i32PiNo < ptTcp->numpocs + 1; i32PiNo++ ) {
			i32P = _i32TileNo % _pcCp->tw;
			i32Q = _i32TileNo / _pcCp->tw;

			ppiPi[i32PiNo].tx0 = CJpegInt::int_max( _pcCp->tx0 + i32P * _pcCp->tdx, _pImage->x0 );
			ppiPi[i32PiNo].ty0 = CJpegInt::int_max( _pcCp->ty0 + i32Q * _pcCp->tdy, _pImage->y0 );
			ppiPi[i32PiNo].tx1 = CJpegInt::int_min( _pcCp->tx0 + (i32P + 1) * _pcCp->tdx, _pImage->x1 );
			ppiPi[i32PiNo].ty1 = CJpegInt::int_min( _pcCp->ty0 + (i32Q + 1) * _pcCp->tdy, _pImage->y1 );
			ppiPi[i32PiNo].numcomps = _pImage->numcomps;
			
			ppiPi[i32PiNo].comps = static_cast<opj_pi_comp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_pImage->numcomps) * sizeof( opj_pi_comp_t ) ));
			if ( !ppiPi[i32PiNo].comps ) {
				pi_destroy( ppiPi, _pcCp, _i32TileNo );
				return NULL;
			}
			
			for ( i32CompNo = 0; i32CompNo < ppiPi[i32PiNo].numcomps; i32CompNo++ ) {
				int32_t i32TcX0, i32TcY0, i32TcX1, i32TcY1;
				opj_pi_comp_t * ppiComp = &ppiPi[i32PiNo].comps[i32CompNo];
				ptTccp = &ptTcp->tccps[i32CompNo];
				ppiComp->dx = _pImage->comps[i32CompNo].dx;
				ppiComp->dy = _pImage->comps[i32CompNo].dy;
				ppiComp->numresolutions = ptTccp->numresolutions;

				ppiComp->resolutions = static_cast<opj_pi_resolution_t *>(CMemLib::MAlloc( static_cast<uint32_t>(ppiComp->numresolutions) * sizeof( opj_pi_resolution_t ) ));
				if ( !ppiComp->resolutions ) {
					pi_destroy( ppiPi, _pcCp, _i32TileNo );
					return NULL;
				}

				i32TcX0 = CJpegInt::int_ceildiv( ppiPi[i32PiNo].tx0, ppiComp->dx );
				i32TcY0 = CJpegInt::int_ceildiv( ppiPi[i32PiNo].ty0, ppiComp->dy );
				i32TcX1 = CJpegInt::int_ceildiv( ppiPi[i32PiNo].tx1, ppiComp->dx );
				i32TcY1 = CJpegInt::int_ceildiv( ppiPi[i32PiNo].ty1, ppiComp->dy );
				if ( ppiComp->numresolutions > i32MaxRes ) {
					i32MaxRes = ppiComp->numresolutions;
				}

				for ( i32ResNo = 0; i32ResNo < ppiComp->numresolutions; i32ResNo++ ) {
					int32_t i32LevelNo;
					int32_t i32Rx0, i32Ry0, i32Rx1, i32Ry1;
					int32_t i32Px0, i32Py0, i32Px1, i32Py1;
					opj_pi_resolution_t * pprRes = &ppiComp->resolutions[i32ResNo];
					if ( ptTccp->csty & J2K_CCP_CSTY_PRT ) {
						pprRes->pdx = ptTccp->prcw[i32ResNo];
						pprRes->pdy = ptTccp->prch[i32ResNo];
					}
					else {
						pprRes->pdx = 15;
						pprRes->pdy = 15;
					}
					i32LevelNo = ppiComp->numresolutions - 1 - i32ResNo;
					i32Rx0 = CJpegInt::int_ceildivpow2( i32TcX0, i32LevelNo );
					i32Ry0 = CJpegInt::int_ceildivpow2( i32TcY0, i32LevelNo );
					i32Rx1 = CJpegInt::int_ceildivpow2( i32TcX1, i32LevelNo );
					i32Ry1 = CJpegInt::int_ceildivpow2( i32TcY1, i32LevelNo );
					i32Px0 = CJpegInt::int_floordivpow2( i32Rx0, pprRes->pdx ) << pprRes->pdx;
					i32Py0 = CJpegInt::int_floordivpow2( i32Ry0, pprRes->pdy ) << pprRes->pdy;
					i32Px1 = CJpegInt::int_ceildivpow2( i32Rx1, pprRes->pdx ) << pprRes->pdx;
					i32Py1 = CJpegInt::int_ceildivpow2( i32Ry1, pprRes->pdy ) << pprRes->pdy;
					pprRes->pw = (i32Rx0 == i32Rx1) ? 0 : ((i32Px1 - i32Px0) >> pprRes->pdx);
					pprRes->ph = (i32Ry0 == i32Ry1) ? 0 : ((i32Py1 - i32Py0) >> pprRes->pdy);

					if ( pprRes->pw * pprRes->ph > i32MaxPrec ) {
						i32MaxPrec = pprRes->pw * pprRes->ph;
					}
				}
			}
			
			ptTccp = &ptTcp->tccps[0];
			ppiPi[i32PiNo].step_p = 1;
			ppiPi[i32PiNo].step_c = i32MaxPrec * ppiPi[i32PiNo].step_p;
			ppiPi[i32PiNo].step_r = _pImage->numcomps * ppiPi[i32PiNo].step_c;
			ppiPi[i32PiNo].step_l = i32MaxRes * ppiPi[i32PiNo].step_r;
			
			for ( i32CompNo = 0; i32CompNo < ppiPi->numcomps; i32CompNo++ ) {
				opj_pi_comp_t * ppiComp = &ppiPi->comps[i32CompNo];
				for ( i32ResNo = 0; i32ResNo < ppiComp->numresolutions; i32ResNo++ ) {
					int32_t i32Dx, i32Dy;
					opj_pi_resolution_t * pprRes = &ppiComp->resolutions[i32ResNo];
					i32Dx = ppiComp->dx * (1 << (pprRes->pdx + ppiComp->numresolutions - 1 - i32ResNo));
					i32Dy = ppiComp->dy * (1 << (pprRes->pdy + ppiComp->numresolutions - 1 - i32ResNo));
					ppiPi[i32PiNo].dx = !ppiPi->dx ? i32Dx : CJpegInt::int_min( ppiPi->dx, i32Dx );
					ppiPi[i32PiNo].dy = !ppiPi->dy ? i32Dy : CJpegInt::int_min( ppiPi->dy, i32Dy );
				}
			}

			if ( i32PiNo == 0 ) {
				ppiPi[i32PiNo].include = static_cast<int16_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ptTcp->numlayers * ppiPi[i32PiNo].step_l) * sizeof( int16_t ) ));
				if ( !ppiPi[i32PiNo].include ) {
					pi_destroy( ppiPi, _pcCp, _i32TileNo );
					return NULL;
				}
			}
			else {
				ppiPi[i32PiNo].include = ppiPi[i32PiNo - 1].include;
			}
			
			// Generation of boundaries for each cProg flag.
			if ( ptTcp->POC && (_pcCp->cinema || (!_pcCp->cinema && (_t2Mode == CJ2k::FINAL_PASS))) ) {
				ptTcp->pocs[i32PiNo].compS = ptTcp->pocs[i32PiNo].compno0;
				ptTcp->pocs[i32PiNo].compE = ptTcp->pocs[i32PiNo].compno1;
				ptTcp->pocs[i32PiNo].resS = ptTcp->pocs[i32PiNo].resno0;
				ptTcp->pocs[i32PiNo].resE = ptTcp->pocs[i32PiNo].resno1;
				ptTcp->pocs[i32PiNo].layE = ptTcp->pocs[i32PiNo].layno1;
				ptTcp->pocs[i32PiNo].prg = ptTcp->pocs[i32PiNo].prg1;
				if ( i32PiNo > 0 ) {
					ptTcp->pocs[i32PiNo].layS = ( ptTcp->pocs[i32PiNo].layE > ptTcp->pocs[i32PiNo - 1].layE ) ? ptTcp->pocs[i32PiNo - 1].layE : 0;
				}
			}
			else {
				ptTcp->pocs[i32PiNo].compS = 0;
				ptTcp->pocs[i32PiNo].compE = _pImage->numcomps;
				ptTcp->pocs[i32PiNo].resS = 0;
				ptTcp->pocs[i32PiNo].resE = i32MaxRes;
				ptTcp->pocs[i32PiNo].layS = 0;
				ptTcp->pocs[i32PiNo].layE = ptTcp->numlayers;
				ptTcp->pocs[i32PiNo].prg = ptTcp->prg;
			}
			ptTcp->pocs[i32PiNo].prcS = 0;
			ptTcp->pocs[i32PiNo].prcE = i32MaxPrec;;
			ptTcp->pocs[i32PiNo].txS = ppiPi[i32PiNo].tx0;
			ptTcp->pocs[i32PiNo].txE = ppiPi[i32PiNo].tx1;
			ptTcp->pocs[i32PiNo].tyS = ppiPi[i32PiNo].ty0;
			ptTcp->pocs[i32PiNo].tyE = ppiPi[i32PiNo].ty1;
			ptTcp->pocs[i32PiNo].dx = ppiPi[i32PiNo].dx;
			ptTcp->pocs[i32PiNo].dy = ppiPi[i32PiNo].dy;
		}
		return ppiPi;
	}

	/**
	 * Modify the packet iterator for enabling tile part generation.
	 *
	 * \param _ppiPi Handle to the packet iterator generated in pi_initialise_encode.
	 * \param _pcCp Coding parameters.
	 * \param _i32TileNo Number that identifies the tile for which to list the packets.
	 * \param _i32PiNo Iterator index for _ppiPi.
	 * \param _i32TpNum Tile part number of the current tile.
	 * \param _i32TpPos The position of the tile part flag in the progression order.
	 * \param _t2Mode If 0 we are in the threshold calculation.  If 1 we are in the final pass.
	 * \param _i32CurTotTp The total number of tile parts in the current tile.
	 * \return Returns true if an error is detected.
	 */
	LSBOOL LSE_CALL CPi::pi_create_encode( opj_pi_iterator_t * _ppiPi, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo, int32_t _i32PiNo, int32_t _i32TpNum, int32_t _i32TpPos, CJ2k::J2K_T2_MODE _t2Mode, int32_t _i32CurTotTp ) {
		char cProg[4];
		int32_t I;
		int32_t i32IncTop = 1, i32ResetX = 0;
		CJ2k::opj_tcp_t * ptTcps = &_pcCp->tcps[_i32TileNo];
		CJpegCommonStructs::opj_poc_t * ptTcp = &ptTcps->pocs[_i32PiNo];

		_ppiPi[_i32PiNo].first = 1;
		_ppiPi[_i32PiNo].poc.prg = ptTcp->prg;

		switch( ptTcp->prg ) {
			case CPRL : {
				CStd::StrNCpy( cProg, "CPRL", 4 );
				break;
			}
			case LRCP : {
				CStd::StrNCpy( cProg, "LRCP", 4 );
				break;
			}
			case PCRL : {
				CStd::StrNCpy( cProg, "PCRL", 4 );
				break;
			}
			case RLCP : {
				CStd::StrNCpy( cProg, "RLCP", 4 );
				break;
			}
			case RPCL : {
				CStd::StrNCpy( cProg, "RPCL", 4 );
				break;
			}
			case PROG_UNKNOWN : { return true; }
		}

		if ( !(_pcCp->tp_on && ((!_pcCp->cinema && (_t2Mode == CJ2k::FINAL_PASS)) || _pcCp->cinema)) ) {
			_ppiPi[_i32PiNo].poc.resno0 = ptTcp->resS;
			_ppiPi[_i32PiNo].poc.resno1 = ptTcp->resE;
			_ppiPi[_i32PiNo].poc.compno0 = ptTcp->compS;
			_ppiPi[_i32PiNo].poc.compno1 = ptTcp->compE;
			_ppiPi[_i32PiNo].poc.layno0 = ptTcp->layS;
			_ppiPi[_i32PiNo].poc.layno1 = ptTcp->layE;
			_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prcS;
			_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prcE;
			_ppiPi[_i32PiNo].poc.tx0 = ptTcp->txS;
			_ppiPi[_i32PiNo].poc.ty0 = ptTcp->tyS;
			_ppiPi[_i32PiNo].poc.tx1 = ptTcp->txE;
			_ppiPi[_i32PiNo].poc.ty1 = ptTcp->tyE;
		}
		else {
			if ( _i32TpNum < _i32CurTotTp ) {
				for ( I = 3; I >= 0; I-- ) {
					switch( cProg[I] ) {
						case 'C' : {
							if ( I > _i32TpPos ) {
								_ppiPi[_i32PiNo].poc.compno0 = ptTcp->compS;
								_ppiPi[_i32PiNo].poc.compno1 = ptTcp->compE;
							}
							else {
								if ( _i32TpNum == 0 ) {
									ptTcp->comp_t = ptTcp->compS;
									_ppiPi[_i32PiNo].poc.compno0 = ptTcp->comp_t;
									_ppiPi[_i32PiNo].poc.compno1 = ptTcp->comp_t + 1;
									ptTcp->comp_t += 1;
								}
								else {
									if ( i32IncTop == 1 ) {
										if ( ptTcp->comp_t ==ptTcp->compE ) {
											ptTcp->comp_t = ptTcp->compS;
											_ppiPi[_i32PiNo].poc.compno0 = ptTcp->comp_t;
											_ppiPi[_i32PiNo].poc.compno1 = ptTcp->comp_t + 1;
											ptTcp->comp_t += 1;
											i32IncTop = 1;
										}
										else {
											_ppiPi[_i32PiNo].poc.compno0 = ptTcp->comp_t;
											_ppiPi[_i32PiNo].poc.compno1 = ptTcp->comp_t + 1;
											ptTcp->comp_t += 1;
											i32IncTop = 0;
										}
									}
									else {
										_ppiPi[_i32PiNo].poc.compno0 = ptTcp->comp_t - 1;
										_ppiPi[_i32PiNo].poc.compno1 = ptTcp->comp_t;
									}
								}
							}
							break;
						}
						case 'R' : {
							if ( I > _i32TpPos ) {
								_ppiPi[_i32PiNo].poc.resno0 = ptTcp->resS;
								_ppiPi[_i32PiNo].poc.resno1 = ptTcp->resE;
							}
							else {
								if ( _i32TpNum == 0 ) {
									ptTcp->res_t = ptTcp->resS;
									_ppiPi[_i32PiNo].poc.resno0 = ptTcp->res_t;
									_ppiPi[_i32PiNo].poc.resno1 = ptTcp->res_t + 1;
									ptTcp->res_t += 1;
								}
								else {
									if ( i32IncTop == 1 ) {
										if ( ptTcp->res_t==ptTcp->resE ) {
											ptTcp->res_t = ptTcp->resS;
											_ppiPi[_i32PiNo].poc.resno0 = ptTcp->res_t;
											_ppiPi[_i32PiNo].poc.resno1 = ptTcp->res_t + 1;
											ptTcp->res_t += 1;
											i32IncTop = 1;
										}
										else {
											_ppiPi[_i32PiNo].poc.resno0 = ptTcp->res_t;
											_ppiPi[_i32PiNo].poc.resno1 = ptTcp->res_t + 1;
											ptTcp->res_t += 1;
											i32IncTop = 0;
										}
									}
									else {
										_ppiPi[_i32PiNo].poc.resno0 = ptTcp->res_t - 1;
										_ppiPi[_i32PiNo].poc.resno1 = ptTcp->res_t;
									}
								}
							}
							break;
						}
						case 'L' : {
							if ( I > _i32TpPos ) {
								_ppiPi[_i32PiNo].poc.layno0 = ptTcp->layS;
								_ppiPi[_i32PiNo].poc.layno1 = ptTcp->layE;
							}
							else {
								if ( _i32TpNum == 0 ) {
									ptTcp->lay_t = ptTcp->layS;
									_ppiPi[_i32PiNo].poc.layno0 = ptTcp->lay_t;
									_ppiPi[_i32PiNo].poc.layno1 = ptTcp->lay_t + 1;
									ptTcp->lay_t += 1;
								}
								else {
									if ( i32IncTop == 1 ) {
										if ( ptTcp->lay_t == ptTcp->layE ) {
											ptTcp->lay_t = ptTcp->layS;
											_ppiPi[_i32PiNo].poc.layno0 = ptTcp->lay_t;
											_ppiPi[_i32PiNo].poc.layno1 = ptTcp->lay_t + 1;
											ptTcp->lay_t += 1;
											i32IncTop = 1;
										}
										else {
											_ppiPi[_i32PiNo].poc.layno0 = ptTcp->lay_t;
											_ppiPi[_i32PiNo].poc.layno1 = ptTcp->lay_t + 1;
											ptTcp->lay_t += 1;
											i32IncTop = 0;
										}
									}
									else {
										_ppiPi[_i32PiNo].poc.layno0 = ptTcp->lay_t - 1;
										_ppiPi[_i32PiNo].poc.layno1 = ptTcp->lay_t;
									}
								}
							}
							break;
						}
						case 'P' : {
							switch( ptTcp->prg ) {
								case LRCP : {}
								case RLCP : {
									if ( I > _i32TpPos ) {
										_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prcS;
										_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prcE;
									}
									else {
										if ( _i32TpNum == 0 ) {
											ptTcp->prc_t = ptTcp->prcS;
											_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prc_t;
											_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prc_t + 1;
											ptTcp->prc_t += 1;
										}
										else {
											if ( i32IncTop == 1 ) {
												if ( ptTcp->prc_t == ptTcp->prcE ) {
													ptTcp->prc_t = ptTcp->prcS;
													_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prc_t;
													_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prc_t + 1;
													ptTcp->prc_t += 1;
													i32IncTop = 1;
												}
												else {
													_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prc_t;
													_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prc_t + 1;
													ptTcp->prc_t += 1;
													i32IncTop = 0;
												}
											}
											else {
												_ppiPi[_i32PiNo].poc.precno0 = ptTcp->prc_t - 1;
												_ppiPi[_i32PiNo].poc.precno1 = ptTcp->prc_t;
											}
										}
									}
									break;
								}
								default : {
									if ( I > _i32TpPos ) {
										_ppiPi[_i32PiNo].poc.tx0 = ptTcp->txS;
										_ppiPi[_i32PiNo].poc.ty0 = ptTcp->tyS;
										_ppiPi[_i32PiNo].poc.tx1 = ptTcp->txE;
										_ppiPi[_i32PiNo].poc.ty1 = ptTcp->tyE;
									}
									else {
										if ( _i32TpNum == 0 ) {
											ptTcp->tx0_t = ptTcp->txS;
											ptTcp->ty0_t = ptTcp->tyS;
											_ppiPi[_i32PiNo].poc.tx0 = ptTcp->tx0_t;
											_ppiPi[_i32PiNo].poc.tx1 = ptTcp->tx0_t + ptTcp->dx - (ptTcp->tx0_t % ptTcp->dx);
											_ppiPi[_i32PiNo].poc.ty0 = ptTcp->ty0_t;
											_ppiPi[_i32PiNo].poc.ty1 = ptTcp->ty0_t + ptTcp->dy - (ptTcp->ty0_t % ptTcp->dy);
											ptTcp->tx0_t = _ppiPi[_i32PiNo].poc.tx1;
											ptTcp->ty0_t = _ppiPi[_i32PiNo].poc.ty1;
										}
										else {
											if ( i32IncTop == 1 ) {
												if ( ptTcp->tx0_t >= ptTcp->txE ) {
													if ( ptTcp->ty0_t >= ptTcp->tyE ) {
														ptTcp->ty0_t = ptTcp->tyS;
														_ppiPi[_i32PiNo].poc.ty0 = ptTcp->ty0_t;
														_ppiPi[_i32PiNo].poc.ty1 = ptTcp->ty0_t + ptTcp->dy - (ptTcp->ty0_t % ptTcp->dy);
														ptTcp->ty0_t = _ppiPi[_i32PiNo].poc.ty1;
														i32IncTop = 1;
														i32ResetX = 1;
													}
													else {
														_ppiPi[_i32PiNo].poc.ty0 = ptTcp->ty0_t;
														_ppiPi[_i32PiNo].poc.ty1 = ptTcp->ty0_t + ptTcp->dy - (ptTcp->ty0_t % ptTcp->dy);
														ptTcp->ty0_t = _ppiPi[_i32PiNo].poc.ty1;
														i32IncTop = 0;
														i32ResetX = 1;
													}
													if ( i32ResetX == 1 ) {
														ptTcp->tx0_t = ptTcp->txS;
														_ppiPi[_i32PiNo].poc.tx0 = ptTcp->tx0_t;
														_ppiPi[_i32PiNo].poc.tx1 = ptTcp->tx0_t + ptTcp->dx - (ptTcp->tx0_t % ptTcp->dx);
														ptTcp->tx0_t = _ppiPi[_i32PiNo].poc.tx1;
													}
												}
												else {
													_ppiPi[_i32PiNo].poc.tx0 = ptTcp->tx0_t;
													_ppiPi[_i32PiNo].poc.tx1 = ptTcp->tx0_t + ptTcp->dx - (ptTcp->tx0_t % ptTcp->dx);
													ptTcp->tx0_t = _ppiPi[_i32PiNo].poc.tx1;
													_ppiPi[_i32PiNo].poc.ty0 = ptTcp->ty0_t - ptTcp->dy - (ptTcp->ty0_t % ptTcp->dy);
													_ppiPi[_i32PiNo].poc.ty1 = ptTcp->ty0_t;
													i32IncTop = 0;
												}
											}
											else {
												_ppiPi[_i32PiNo].poc.tx0 = ptTcp->tx0_t - ptTcp->dx - (ptTcp->tx0_t % ptTcp->dx);
												_ppiPi[_i32PiNo].poc.tx1 = ptTcp->tx0_t;
												_ppiPi[_i32PiNo].poc.ty0 = ptTcp->ty0_t - ptTcp->dy - (ptTcp->ty0_t % ptTcp->dy);
												_ppiPi[_i32PiNo].poc.ty1 = ptTcp->ty0_t;
											}
										}
									}
									// End default case.
								}
							}
							break;
						}
					}		
				} 
			}
		}	
		return false;
	}

	/**
	 * Creates a packet iterator for the decoder.
	 *
	 * \param _piImage Raw image for which the packets will be listed.
	 * \param _pcCp Coding parameters.
	 * \param _i32TileNo Number that identifies the tile for which to list the packets.
	 * \return Returns a packet iterator that points to the first packet of the tile.
	 * \see pi_destroy
	 */
	CPi::opj_pi_iterator_t * LSE_CALL CPi::pi_create_decode( CJpegImage::opj_image_t * _piImage, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo ) {
		int32_t i32P, i32Q;
		int32_t i32CompNo, i32ResNo, i32PiNo;
		opj_pi_iterator_t * ppiPi = NULL;
		CJ2k::opj_tcp_t * ptTcp = NULL;
		CJ2k::opj_tccp_t * ptTccp = NULL;


		ptTcp = &_pcCp->tcps[_i32TileNo];

		ppiPi = static_cast<opj_pi_iterator_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ptTcp->numpocs + 1) * sizeof( opj_pi_iterator_t ) ));
		if ( !ppiPi ) { return NULL; }

		for ( i32PiNo = 0; i32PiNo < ptTcp->numpocs + 1; i32PiNo++ ) {	// Change.
			int32_t i32MaxRes = 0;
			int32_t i32MaxPrec = 0;
			i32P = _i32TileNo % _pcCp->tw;
			i32Q = _i32TileNo / _pcCp->tw;

			ppiPi[i32PiNo].tx0 = CJpegInt::int_max( _pcCp->tx0 + i32P * _pcCp->tdx, _piImage->x0 );
			ppiPi[i32PiNo].ty0 = CJpegInt::int_max( _pcCp->ty0 + i32Q * _pcCp->tdy, _piImage->y0 );
			ppiPi[i32PiNo].tx1 = CJpegInt::int_min( _pcCp->tx0 + (i32P + 1) * _pcCp->tdx, _piImage->x1 );
			ppiPi[i32PiNo].ty1 = CJpegInt::int_min( _pcCp->ty0 + (i32Q + 1) * _pcCp->tdy, _piImage->y1 );
			ppiPi[i32PiNo].numcomps = _piImage->numcomps;

			ppiPi[i32PiNo].comps = static_cast<opj_pi_comp_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_piImage->numcomps) * sizeof( opj_pi_comp_t ) ));
			if ( !ppiPi[i32PiNo].comps ) {
				pi_destroy( ppiPi, _pcCp, _i32TileNo );
				return NULL;
			}
			
			for ( i32CompNo = 0; i32CompNo < ppiPi->numcomps; i32CompNo++ ) {
				int32_t i32TcX0, i32TcY0, i32TcX1, i32TcY1;
				opj_pi_comp_t * ppcComp = &ppiPi[i32PiNo].comps[i32CompNo];
				ptTccp = &ptTcp->tccps[i32CompNo];
				ppcComp->dx = _piImage->comps[i32CompNo].dx;
				ppcComp->dy = _piImage->comps[i32CompNo].dy;
				ppcComp->numresolutions = ptTccp->numresolutions;

				ppcComp->resolutions = static_cast<opj_pi_resolution_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(ppcComp->numresolutions) * sizeof( opj_pi_resolution_t ) ));
				if ( !ppcComp->resolutions ) {
					pi_destroy( ppiPi, _pcCp, _i32TileNo );
					return NULL;
				}

				i32TcX0 = CJpegInt::int_ceildiv( ppiPi->tx0, ppcComp->dx );
				i32TcY0 = CJpegInt::int_ceildiv( ppiPi->ty0, ppcComp->dy );
				i32TcX1 = CJpegInt::int_ceildiv( ppiPi->tx1, ppcComp->dx );
				i32TcY1 = CJpegInt::int_ceildiv( ppiPi->ty1, ppcComp->dy );
				if ( ppcComp->numresolutions > i32MaxRes ) {
					i32MaxRes = ppcComp->numresolutions;
				}

				for ( i32ResNo = 0; i32ResNo < ppcComp->numresolutions; i32ResNo++ ) {
					int32_t i32LevelNo;
					int32_t i32Rx0, i32Ry0, i32Rx1, i32Ry1;
					int32_t i32Px0, i32Py0, i32Px1, i32Py1;
					opj_pi_resolution_t * pprRes = &ppcComp->resolutions[i32ResNo];
					if ( ptTccp->csty & J2K_CCP_CSTY_PRT ) {
						pprRes->pdx = ptTccp->prcw[i32ResNo];
						pprRes->pdy = ptTccp->prch[i32ResNo];
					}
					else {
						pprRes->pdx = 15;
						pprRes->pdy = 15;
					}
					i32LevelNo = ppcComp->numresolutions - 1 - i32ResNo;
					i32Rx0 = CJpegInt::int_ceildivpow2( i32TcX0, i32LevelNo );
					i32Ry0 = CJpegInt::int_ceildivpow2( i32TcY0, i32LevelNo );
					i32Rx1 = CJpegInt::int_ceildivpow2( i32TcX1, i32LevelNo );
					i32Ry1 = CJpegInt::int_ceildivpow2( i32TcY1, i32LevelNo );
					i32Px0 = CJpegInt::int_floordivpow2( i32Rx0, pprRes->pdx ) << pprRes->pdx;
					i32Py0 = CJpegInt::int_floordivpow2( i32Ry0, pprRes->pdy ) << pprRes->pdy;
					i32Px1 = CJpegInt::int_ceildivpow2( i32Rx1, pprRes->pdx ) << pprRes->pdx;
					i32Py1 = CJpegInt::int_ceildivpow2( i32Ry1, pprRes->pdy ) << pprRes->pdy;
					pprRes->pw = (i32Rx0 == i32Rx1) ? 0 : ((i32Px1 - i32Px0) >> pprRes->pdx);
					pprRes->ph = (i32Ry0 == i32Ry1) ? 0 : ((i32Py1 - i32Py0) >> pprRes->pdy);
					
					if ( pprRes->pw * pprRes->ph > i32MaxPrec ) {
						i32MaxPrec = pprRes->pw * pprRes->ph;
					}
					
				}
			}
			
			ptTccp = &ptTcp->tccps[0];
			ppiPi[i32PiNo].step_p = 1;
			ppiPi[i32PiNo].step_c = i32MaxPrec * ppiPi[i32PiNo].step_p;
			ppiPi[i32PiNo].step_r = _piImage->numcomps * ppiPi[i32PiNo].step_c;
			ppiPi[i32PiNo].step_l = i32MaxRes * ppiPi[i32PiNo].step_r;
			
			if ( i32PiNo == 0 ) {
				ppiPi[i32PiNo].include = static_cast<int16_t *>(CMemLib::CMAlloc( static_cast<uint32_t>(_piImage->numcomps * i32MaxRes * ptTcp->numlayers * i32MaxPrec) * sizeof( int16_t ) ));
				if ( !ppiPi[i32PiNo].include ) {
					pi_destroy( ppiPi, _pcCp, _i32TileNo );
					return NULL;
				}
			}
			else {
				ppiPi[i32PiNo].include = ppiPi[i32PiNo-1].include;
			}
			
			if ( ptTcp->POC == 0 ) {
				ppiPi[i32PiNo].first = 1;
				ppiPi[i32PiNo].poc.resno0 = 0;
				ppiPi[i32PiNo].poc.compno0 = 0;
				ppiPi[i32PiNo].poc.layno1 = ptTcp->numlayers;
				ppiPi[i32PiNo].poc.resno1 = i32MaxRes;
				ppiPi[i32PiNo].poc.compno1 = _piImage->numcomps;
				ppiPi[i32PiNo].poc.prg = ptTcp->prg;
			}
			else {
				ppiPi[i32PiNo].first = 1;
				ppiPi[i32PiNo].poc.resno0 = ptTcp->pocs[i32PiNo].resno0;
				ppiPi[i32PiNo].poc.compno0 = ptTcp->pocs[i32PiNo].compno0;
				ppiPi[i32PiNo].poc.layno1 = ptTcp->pocs[i32PiNo].layno1;
				ppiPi[i32PiNo].poc.resno1 = ptTcp->pocs[i32PiNo].resno1;
				ppiPi[i32PiNo].poc.compno1 = ptTcp->pocs[i32PiNo].compno1;
				ppiPi[i32PiNo].poc.prg = ptTcp->pocs[i32PiNo].prg;
			}
			ppiPi[i32PiNo].poc.layno0 = 0;
			ppiPi[i32PiNo].poc.precno0 = 0; 
			ppiPi[i32PiNo].poc.precno1 = i32MaxPrec;
		}
		
		return ppiPi;
	}

	/**
	 * Destroy a packet iterator.
	 *
	 * \param _ppiPi Previously created packet iterator.
	 * \param _pcCp Coding parameters.
	 * \param _i32TileNo Number that identifies the tile for which the packets were listed.
	 * \see pi_create
	 */
	void LSE_CALL CPi::pi_destroy( opj_pi_iterator_t * _ppiPi, CJ2k::opj_cp_t * _pcCp, int32_t _i32TileNo ) {
		int32_t i32CompNo, i32PiNo;
		CJ2k::opj_tcp_t * ptTcp = &_pcCp->tcps[_i32TileNo];
		if ( _ppiPi ) {
			for ( i32PiNo = 0; i32PiNo < ptTcp->numpocs + 1; i32PiNo++ ) {	
				if ( _ppiPi[i32PiNo].comps ) {
					for ( i32CompNo = 0; i32CompNo < _ppiPi->numcomps; i32CompNo++ ) {
						opj_pi_comp_t * ppcComp = &_ppiPi[i32PiNo].comps[i32CompNo];
						if ( ppcComp->resolutions ) {
							CMemLib::Free( ppcComp->resolutions );
						}
					}
					CMemLib::Free( _ppiPi[i32PiNo].comps );
				}
			}
			if ( _ppiPi->include ) {
				CMemLib::Free( _ppiPi->include );
			}
			CMemLib::Free( _ppiPi );
		}
	}

	/**
	 * Modify the packet iterator to point to the next packet.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet or else returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next( opj_pi_iterator_t * _ppiPi ) {
		switch ( _ppiPi->poc.prg ) {
			case LRCP : { return pi_next_lrcp( _ppiPi ); }
			case RLCP : { return pi_next_rlcp( _ppiPi ); }
			case RPCL : { return pi_next_rpcl( _ppiPi ); }
			case PCRL : { return pi_next_pcrl( _ppiPi ); }
			case CPRL : { return pi_next_cprl( _ppiPi ); }
			case PROG_UNKNOWN : { return false; }
		}
		
		return false;
	}

	/**
	 * Gets next packet in layer-resolution-component-precinct order.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next_lrcp( opj_pi_iterator_t * _ppiPi ) {
		opj_pi_comp_t * ppcComp = NULL;
		opj_pi_resolution_t * pprRes = NULL;
		int32_t i32Index = 0;
		
		if ( !_ppiPi->first ) {
			ppcComp = &_ppiPi->comps[_ppiPi->compno];
			pprRes = &ppcComp->resolutions[_ppiPi->resno];
			goto Skip;
		}
		else {
			_ppiPi->first = 0;
		}

		for ( _ppiPi->layno = _ppiPi->poc.layno0; _ppiPi->layno < _ppiPi->poc.layno1; _ppiPi->layno++ ) {
			for ( _ppiPi->resno = _ppiPi->poc.resno0; _ppiPi->resno < _ppiPi->poc.resno1; _ppiPi->resno++ ) {
				for ( _ppiPi->compno = _ppiPi->poc.compno0; _ppiPi->compno < _ppiPi->poc.compno1; _ppiPi->compno++ ) {
					ppcComp = &_ppiPi->comps[_ppiPi->compno];
					if ( _ppiPi->resno >= ppcComp->numresolutions ) { continue; }
					pprRes = &ppcComp->resolutions[_ppiPi->resno];
					if ( !_ppiPi->tp_on ) {
						_ppiPi->poc.precno1 = pprRes->pw * pprRes->ph;
					}
					for ( _ppiPi->precno = _ppiPi->poc.precno0; _ppiPi->precno < _ppiPi->poc.precno1; _ppiPi->precno++ ) {
						i32Index = _ppiPi->layno * _ppiPi->step_l + _ppiPi->resno * _ppiPi->step_r + _ppiPi->compno * _ppiPi->step_c + _ppiPi->precno * _ppiPi->step_p;
						if ( !_ppiPi->include[i32Index] ) {
							_ppiPi->include[i32Index] = 1;
							return true;
						}
	Skip :
						;
					}
				}
			}
		}
		
		return false;
	}

	/**
	 * Gets next packet in resolution-layer-component-precinct order.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next_rlcp( opj_pi_iterator_t * _ppiPi ) {
		opj_pi_comp_t * ppcComp = NULL;
		opj_pi_resolution_t * pprRes = NULL;
		int32_t i32Index = 0;

		if ( !_ppiPi->first ) {
			ppcComp = &_ppiPi->comps[_ppiPi->compno];
			pprRes = &ppcComp->resolutions[_ppiPi->resno];
			goto Skip;
		}
		else {
			_ppiPi->first = 0;
		}

		for ( _ppiPi->resno = _ppiPi->poc.resno0; _ppiPi->resno < _ppiPi->poc.resno1; _ppiPi->resno++ ) {
			for ( _ppiPi->layno = _ppiPi->poc.layno0; _ppiPi->layno < _ppiPi->poc.layno1; _ppiPi->layno++ ) {
				for ( _ppiPi->compno = _ppiPi->poc.compno0; _ppiPi->compno < _ppiPi->poc.compno1; _ppiPi->compno++ ) {
					ppcComp = &_ppiPi->comps[_ppiPi->compno];
					if ( _ppiPi->resno >= ppcComp->numresolutions ) { continue; }
					pprRes = &ppcComp->resolutions[_ppiPi->resno];
					if ( !_ppiPi->tp_on ) {
						_ppiPi->poc.precno1 = pprRes->pw * pprRes->ph;
					}
					for ( _ppiPi->precno = _ppiPi->poc.precno0; _ppiPi->precno < _ppiPi->poc.precno1; _ppiPi->precno++ ) {
						i32Index = _ppiPi->layno * _ppiPi->step_l + _ppiPi->resno * _ppiPi->step_r + _ppiPi->compno * _ppiPi->step_c + _ppiPi->precno * _ppiPi->step_p;
						if ( !_ppiPi->include[i32Index] ) {
							_ppiPi->include[i32Index] = 1;
							return true;
						}
	Skip :
						;
					}
				}
			}
		}
		
		return false;
	}

	/**
	 * Gets next packet in resolution-precinct-component-layer order.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next_rpcl( opj_pi_iterator_t * _ppiPi ) {
		opj_pi_comp_t * ppcComp = NULL;
		opj_pi_resolution_t * pprRes = NULL;
		int32_t i32Index = 0;

		if ( !_ppiPi->first ) { goto Skip; }
		else {
			int32_t i32CompNo, i32ResNo;
			_ppiPi->first = 0;
			_ppiPi->dx = 0;
			_ppiPi->dy = 0;
			for ( i32CompNo = 0; i32CompNo < _ppiPi->numcomps; i32CompNo++ ) {
				ppcComp = &_ppiPi->comps[i32CompNo];
				for ( i32ResNo = 0; i32ResNo < ppcComp->numresolutions; i32ResNo++ ) {
					int32_t i32Dx, i32Dy;
					pprRes = &ppcComp->resolutions[i32ResNo];
					i32Dx = ppcComp->dx * (1 << (pprRes->pdx + ppcComp->numresolutions - 1 - i32ResNo));
					i32Dy = ppcComp->dy * (1 << ( pprRes->pdy + ppcComp->numresolutions - 1 - i32ResNo));
					_ppiPi->dx = !_ppiPi->dx ? i32Dx : CJpegInt::int_min( _ppiPi->dx, i32Dx );
					_ppiPi->dy = !_ppiPi->dy ? i32Dy : CJpegInt::int_min( _ppiPi->dy, i32Dy );
				}
			}
		}
		if ( !_ppiPi->tp_on ) {
			_ppiPi->poc.ty0 = _ppiPi->ty0;
			_ppiPi->poc.tx0 = _ppiPi->tx0;
			_ppiPi->poc.ty1 = _ppiPi->ty1;
			_ppiPi->poc.tx1 = _ppiPi->tx1;
		}
		for ( _ppiPi->resno = _ppiPi->poc.resno0; _ppiPi->resno < _ppiPi->poc.resno1; _ppiPi->resno++ ) {
			for ( _ppiPi->y = _ppiPi->poc.ty0; _ppiPi->y < _ppiPi->poc.ty1; _ppiPi->y += _ppiPi->dy - (_ppiPi->y % _ppiPi->dy) ) {
				for ( _ppiPi->x = _ppiPi->poc.tx0; _ppiPi->x < _ppiPi->poc.tx1; _ppiPi->x += _ppiPi->dx - (_ppiPi->x % _ppiPi->dx) ) {
					for ( _ppiPi->compno = _ppiPi->poc.compno0; _ppiPi->compno < _ppiPi->poc.compno1; _ppiPi->compno++ ) {
						int32_t i32LevelNo;
						int32_t i32TrX0, i32TrY0;
						int32_t i32TrX1, i32TrY1;
						int32_t i32RpX, i32RpY;
						int32_t i32PrcI, i32PrcJ;
						ppcComp = &_ppiPi->comps[_ppiPi->compno];
						if ( _ppiPi->resno >= ppcComp->numresolutions ) { continue; }
						pprRes = &ppcComp->resolutions[_ppiPi->resno];
						i32LevelNo = ppcComp->numresolutions - 1 - _ppiPi->resno;
						i32TrX0 = CJpegInt::int_ceildiv( _ppiPi->tx0, ppcComp->dx << i32LevelNo );
						i32TrY0 = CJpegInt::int_ceildiv( _ppiPi->ty0, ppcComp->dy << i32LevelNo );
						i32TrX1 = CJpegInt::int_ceildiv( _ppiPi->tx1, ppcComp->dx << i32LevelNo );
						i32TrY1 = CJpegInt::int_ceildiv( _ppiPi->ty1, ppcComp->dy << i32LevelNo );
						i32RpX = pprRes->pdx + i32LevelNo;
						i32RpY = pprRes->pdy + i32LevelNo;
						if ( !((_ppiPi->y % (ppcComp->dy << i32RpY) == 0) || ((_ppiPi->y == _ppiPi->ty0) && ((i32TrY0 << i32LevelNo) % (1 << i32RpY)))) ) { continue; }
						if ( !((_ppiPi->x % (ppcComp->dx << i32RpX) == 0) || ((_ppiPi->x == _ppiPi->tx0) && ((i32TrX0 << i32LevelNo) % (1 << i32RpX)))) ) { continue; }
						
						if ( (pprRes->pw == 0) || (pprRes->ph == 0) ) { continue; }
						
						if ( (i32TrX0 == i32TrX1) || (i32TrY0 == i32TrY1) ) { continue; }
						
						i32PrcI = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->x, ppcComp->dx << i32LevelNo ), pprRes->pdx ) -
							CJpegInt::int_floordivpow2( i32TrX0, pprRes->pdx );
						i32PrcJ = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->y, ppcComp->dy << i32LevelNo ), pprRes->pdy ) -
							CJpegInt::int_floordivpow2( i32TrY0, pprRes->pdy );
						_ppiPi->precno = i32PrcI + i32PrcJ * pprRes->pw;
						for ( _ppiPi->layno = _ppiPi->poc.layno0; _ppiPi->layno < _ppiPi->poc.layno1; _ppiPi->layno++ ) {
							i32Index = _ppiPi->layno * _ppiPi->step_l + _ppiPi->resno * _ppiPi->step_r + _ppiPi->compno * _ppiPi->step_c + _ppiPi->precno * _ppiPi->step_p;
							if ( !_ppiPi->include[i32Index] ) {
								_ppiPi->include[i32Index] = 1;
								return true;
							}
	Skip :
							;
						}
					}
				}
			}
		}
		
		return false;
	}

	/**
	 * Gets next packet in precinct-component-resolution-layer order.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next_pcrl( opj_pi_iterator_t * _ppiPi ) {
		opj_pi_comp_t * ppcComp = NULL;
		opj_pi_resolution_t * pprRes = NULL;
		int32_t i32Index = 0;

		if ( !_ppiPi->first ) {
			ppcComp = &_ppiPi->comps[_ppiPi->compno];
			goto Skip;
		}
		else {
			int32_t i32CompNo, i32ResNo;
			_ppiPi->first = 0;
			_ppiPi->dx = 0;
			_ppiPi->dy = 0;
			for ( i32CompNo = 0; i32CompNo < _ppiPi->numcomps; i32CompNo++ ) {
				ppcComp = &_ppiPi->comps[i32CompNo];
				for ( i32ResNo = 0; i32ResNo < ppcComp->numresolutions; i32ResNo++ ) {
					int32_t dx, dy;
					pprRes = &ppcComp->resolutions[i32ResNo];
					dx = ppcComp->dx * ( 1 << ( pprRes->pdx + ppcComp->numresolutions - 1 - i32ResNo ) );
					dy = ppcComp->dy * ( 1 << ( pprRes->pdy + ppcComp->numresolutions - 1 - i32ResNo ) );
					_ppiPi->dx = !_ppiPi->dx ? dx : CJpegInt::int_min( _ppiPi->dx, dx );
					_ppiPi->dy = !_ppiPi->dy ? dy : CJpegInt::int_min( _ppiPi->dy, dy );
				}
			}
		}
		if ( !_ppiPi->tp_on ) {
			_ppiPi->poc.ty0 = _ppiPi->ty0;
			_ppiPi->poc.tx0 = _ppiPi->tx0;
			_ppiPi->poc.ty1 = _ppiPi->ty1;
			_ppiPi->poc.tx1 = _ppiPi->tx1;
		}
		for ( _ppiPi->y = _ppiPi->poc.ty0; _ppiPi->y < _ppiPi->poc.ty1; _ppiPi->y += _ppiPi->dy - (_ppiPi->y % _ppiPi->dy) ) {
			for ( _ppiPi->x = _ppiPi->poc.tx0; _ppiPi->x < _ppiPi->poc.tx1; _ppiPi->x += _ppiPi->dx - (_ppiPi->x % _ppiPi->dx) ) {
				for ( _ppiPi->compno = _ppiPi->poc.compno0; _ppiPi->compno < _ppiPi->poc.compno1; _ppiPi->compno++ ) {
					ppcComp = &_ppiPi->comps[_ppiPi->compno];
					for ( _ppiPi->resno = _ppiPi->poc.resno0; _ppiPi->resno < CJpegInt::int_min( _ppiPi->poc.resno1, ppcComp->numresolutions ); _ppiPi->resno++ ) {
						int32_t i32LevelNo;
						int32_t i32TrX0, i32TrY0;
						int32_t i32TrX1, i32TrY1;
						int32_t i32RpX, i32RpY;
						int32_t i32PrcI, i32PrcJ;
						pprRes = &ppcComp->resolutions[_ppiPi->resno];
						i32LevelNo = ppcComp->numresolutions - 1 - _ppiPi->resno;
						i32TrX0 = CJpegInt::int_ceildiv( _ppiPi->tx0, ppcComp->dx << i32LevelNo );
						i32TrY0 = CJpegInt::int_ceildiv( _ppiPi->ty0, ppcComp->dy << i32LevelNo );
						i32TrX1 = CJpegInt::int_ceildiv( _ppiPi->tx1, ppcComp->dx << i32LevelNo );
						i32TrY1 = CJpegInt::int_ceildiv( _ppiPi->ty1, ppcComp->dy << i32LevelNo );
						i32RpX = pprRes->pdx + i32LevelNo;
						i32RpY = pprRes->pdy + i32LevelNo;
						if ( !((_ppiPi->y % (ppcComp->dy << i32RpY) == 0) || ((_ppiPi->y == _ppiPi->ty0) && ((i32TrY0 << i32LevelNo) % (1 << i32RpY)))) ) { continue; }
						if ( !((_ppiPi->x % (ppcComp->dx << i32RpX) == 0) || ((_ppiPi->x == _ppiPi->tx0) && ((i32TrX0 << i32LevelNo) % (1 << i32RpX)))) ) { continue; }
						
						if ( (pprRes->pw == 0) || (pprRes->ph == 0) ) { continue; }
						
						if ( (i32TrX0 == i32TrX1) || (i32TrY0 == i32TrY1) ) { continue; }
						
						i32PrcI = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->x, ppcComp->dx << i32LevelNo ), pprRes->pdx ) -
							CJpegInt::int_floordivpow2( i32TrX0, pprRes->pdx );
						i32PrcJ = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->y, ppcComp->dy << i32LevelNo ), pprRes->pdy ) -
							CJpegInt::int_floordivpow2( i32TrY0, pprRes->pdy );
						_ppiPi->precno = i32PrcI + i32PrcJ * pprRes->pw;
						for ( _ppiPi->layno = _ppiPi->poc.layno0; _ppiPi->layno < _ppiPi->poc.layno1; _ppiPi->layno++ ) {
							i32Index = _ppiPi->layno * _ppiPi->step_l + _ppiPi->resno * _ppiPi->step_r + _ppiPi->compno * _ppiPi->step_c + _ppiPi->precno * _ppiPi->step_p;
							if ( !_ppiPi->include[i32Index] ) {
								_ppiPi->include[i32Index] = 1;
								return true;
							}	
	Skip :
							;
						}
					}
				}
			}
		}
		
		return false;
	}

	/**
	 * Gets next packet in component-precinct-resolution-layer order.
	 *
	 * \param _ppiPi Packet iterator to modify.
	 * \return Returns false if _ppiPi pointed to the last packet, otherwise returns true.
	 */
	LSBOOL LSE_CALL CPi::pi_next_cprl( opj_pi_iterator_t * _ppiPi ) {
		opj_pi_comp_t * ppcComp = NULL;
		opj_pi_resolution_t * pprRes = NULL;
		int32_t i32Index = 0;

		if ( !_ppiPi->first ) {
			ppcComp = &_ppiPi->comps[_ppiPi->compno];
			goto Skip;
		}
		else {
			_ppiPi->first = 0;
		}

		for ( _ppiPi->compno = _ppiPi->poc.compno0; _ppiPi->compno < _ppiPi->poc.compno1; _ppiPi->compno++ ) {
			int32_t i32ResNo;
			ppcComp = &_ppiPi->comps[_ppiPi->compno];
			_ppiPi->dx = 0;
			_ppiPi->dy = 0;
			for ( i32ResNo = 0; i32ResNo < ppcComp->numresolutions; i32ResNo++ ) {
				int32_t i32Dx, i32Dy;
				pprRes = &ppcComp->resolutions[i32ResNo];
				i32Dx = ppcComp->dx * (1 << (pprRes->pdx + ppcComp->numresolutions - 1 - i32ResNo));
				i32Dy = ppcComp->dy * (1 << (pprRes->pdy + ppcComp->numresolutions - 1 - i32ResNo));
				_ppiPi->dx = !_ppiPi->dx ? i32Dx : CJpegInt::int_min( _ppiPi->dx, i32Dx );
				_ppiPi->dy = !_ppiPi->dy ? i32Dy : CJpegInt::int_min( _ppiPi->dy, i32Dy );
			}
			if ( !_ppiPi->tp_on ) {
				_ppiPi->poc.ty0 = _ppiPi->ty0;
				_ppiPi->poc.tx0 = _ppiPi->tx0;
				_ppiPi->poc.ty1 = _ppiPi->ty1;
				_ppiPi->poc.tx1 = _ppiPi->tx1;
			}
			for ( _ppiPi->y = _ppiPi->poc.ty0; _ppiPi->y < _ppiPi->poc.ty1; _ppiPi->y += _ppiPi->dy - ( _ppiPi->y % _ppiPi->dy ) ) {
				for ( _ppiPi->x = _ppiPi->poc.tx0; _ppiPi->x < _ppiPi->poc.tx1; _ppiPi->x += _ppiPi->dx - ( _ppiPi->x % _ppiPi->dx ) ) {
					for ( _ppiPi->resno = _ppiPi->poc.resno0; _ppiPi->resno < CJpegInt::int_min( _ppiPi->poc.resno1, ppcComp->numresolutions ); _ppiPi->resno++ ) {
						int32_t i32LevelNo;
						int32_t i32TrX0, i32TrY0;
						int32_t i32TrX1, i32TrY1;
						int32_t i32RpX, i32RpY;
						int32_t i32PrcI, i32PrcJ;
						pprRes = &ppcComp->resolutions[_ppiPi->resno];
						i32LevelNo = ppcComp->numresolutions - 1 - _ppiPi->resno;
						i32TrX0 = CJpegInt::int_ceildiv( _ppiPi->tx0, ppcComp->dx << i32LevelNo );
						i32TrY0 = CJpegInt::int_ceildiv( _ppiPi->ty0, ppcComp->dy << i32LevelNo );
						i32TrX1 = CJpegInt::int_ceildiv( _ppiPi->tx1, ppcComp->dx << i32LevelNo );
						i32TrY1 = CJpegInt::int_ceildiv( _ppiPi->ty1, ppcComp->dy << i32LevelNo );
						i32RpX = pprRes->pdx + i32LevelNo;
						i32RpY = pprRes->pdy + i32LevelNo;
						if ( !((_ppiPi->y % (ppcComp->dy << i32RpY) == 0) || ((_ppiPi->y == _ppiPi->ty0) && ((i32TrY0 << i32LevelNo) % (1 << i32RpY)))) ) { continue; }
						if ( !((_ppiPi->x % (ppcComp->dx << i32RpX) == 0) || ((_ppiPi->x == _ppiPi->tx0) && ((i32TrX0 << i32LevelNo) % (1 << i32RpX)))) ) { continue; }
						
						if ( (pprRes->pw == 0) || (pprRes->ph == 0) ) { continue; }
						
						if ( (i32TrX0 == i32TrX1) || (i32TrY0 == i32TrY1) ) { continue; }
						
						i32PrcI = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->x, ppcComp->dx << i32LevelNo ), pprRes->pdx ) -
							CJpegInt::int_floordivpow2( i32TrX0, pprRes->pdx );
						i32PrcJ = CJpegInt::int_floordivpow2( CJpegInt::int_ceildiv( _ppiPi->y, ppcComp->dy << i32LevelNo ), pprRes->pdy ) -
							CJpegInt::int_floordivpow2( i32TrY0, pprRes->pdy );
						_ppiPi->precno = i32PrcI + i32PrcJ * pprRes->pw;
						for ( _ppiPi->layno = _ppiPi->poc.layno0; _ppiPi->layno < _ppiPi->poc.layno1; _ppiPi->layno++ ) {
							i32Index = _ppiPi->layno * _ppiPi->step_l + _ppiPi->resno * _ppiPi->step_r + _ppiPi->compno * _ppiPi->step_c + _ppiPi->precno * _ppiPi->step_p;
							if ( !_ppiPi->include[i32Index] ) {
								_ppiPi->include[i32Index] = 1;
								return true;
							}
	Skip :
							;
						}
					}
				}
			}
		}
		
		return false;
	}

}	// namespace lsi
