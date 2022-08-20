/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2007, Jonathan Ballard <dzonatas@dzonux.net>
 * Copyright (c) 2007, Callum Lerwick <seg@haxxed.com>
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

#include "LSIDwt.h"
#include "LSIJpegFixed.h"
#include "LSIJpegInt.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define S(I)						_pi32A[(I)*2]
#define D(I)						_pi32A[(1+(I)*2)]
#define S_(I)						((I) < 0 ? S( 0 ) : ((I) >= _i32Sn ? S( _i32Sn - 1 ) : S( I )))
#define D_(I)						((I) < 0 ? D( 0 ) : ((I) >= _i32Dn ? D( _i32Dn - 1 ) : D( I )))

#define SS_(I)						((I) < 0 ? S( 0 ) : ((I) >= _i32Dn ? S( _i32Dn - 1 ) : S( I )))
#define DD_(I)						((I) < 0 ? D( 0 ) : ((I) >= _i32Sn ? D( _i32Sn - 1 ) : D( I )))


namespace lsi {

	// == Members.
	/**
	 * This table contains the norms of the 5-3 wavelets for different bands.
	 */
	const double CDwt::dwt_norms[4][10] = {
		{ 1.000, 1.500, 2.750, 5.375, 10.68, 21.34, 42.67, 85.33, 170.7, 341.3 },
		{ 1.038, 1.592, 2.919, 5.703, 11.33, 22.64, 45.25, 90.48, 180.9 },
		{ 1.038, 1.592, 2.919, 5.703, 11.33, 22.64, 45.25, 90.48, 180.9 },
		{ 0.7186, 0.9218, 1.586, 3.043, 6.019, 12.01, 24.00, 47.97, 95.93 }
	};

	/**
	 * This table contains the norms of the 9-7 wavelets for different bands.
	 */
	const double CDwt::dwt_norms_real[4][10] = {
		{ 1.000, 1.965, 4.177, 8.403, 16.90, 33.84, 67.69, 135.3, 270.6, 540.9 },
		{ 2.022, 3.989, 8.355, 17.04, 34.27, 68.63, 137.3, 274.6, 549.0 },
		{ 2.022, 3.989, 8.355, 17.04, 34.27, 68.63, 137.3, 274.6, 549.0 },
		{ 2.080, 3.865, 8.307, 17.18, 34.71, 69.59, 139.3, 278.6, 557.2 }
	};

	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_fK = 1.230174105f; // 10078.
	
	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_c13318 = 1.625732422f;

	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_dwt_alpha = 1.586134342f; // 12994

	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_dwt_beta = 0.052980118f; // 434

	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_dwt_gamma = -0.882911075f; // -7233

	/**
	 * Undocumented value from the OpenJpeg Library.
	 */
	const float CDwt::m_dwt_delta = -0.443506852f; // -3633

	// == Functions.
	/**
	 * Forward 5-3 wavelet tranform in 2-D.
	 *
	 * Apply a reversible DWT transform to a component of an image.
	 * \param _pttTileC Tile component information (current tile)
	 */
	void LSE_CALL CDwt::dwt_encode( CTcd::opj_tcd_tilecomp_t * _pttTileC ) {
		int32_t I, J, K;
		int32_t * pi32A = NULL;
		int32_t * pi32Aj = NULL;
		int32_t * pi32Bj = NULL;
		int32_t i32W, i32L;
		
		i32W = _pttTileC->x1-_pttTileC->x0;
		i32L = _pttTileC->numresolutions-1;
		pi32A = _pttTileC->data;
		
		for ( I = 0; I < i32L; I++ ) {
			int32_t i32Rw;			// Width of the resolution level computed.
			int32_t i32Rh;			// Height of the resolution level computed.
			int32_t i32Rw1;			// Width of the resolution level once lower than computed one.
			int32_t i32Rh1;			// Height of the resolution level once lower than computed one.
			int32_t i32CasCol;		// 0 = non inversion on horizontal filtering 1 = inversion between low-pass and high-pass filtering.
			int32_t i32CasRow;		// 0 = non inversion on vertical filtering 1 = inversion between low-pass and high-pass filtering.
			int32_t i32Dn, i32Sn;
			
			i32Rw = _pttTileC->resolutions[i32L-I].x1 - _pttTileC->resolutions[i32L-I].x0;
			i32Rh = _pttTileC->resolutions[i32L-I].y1 - _pttTileC->resolutions[i32L-I].y0;
			i32Rw1 = _pttTileC->resolutions[i32L-I-1].x1 - _pttTileC->resolutions[i32L-I-1].x0;
			i32Rh1 = _pttTileC->resolutions[i32L-I-1].y1 - _pttTileC->resolutions[i32L-I-1].y0;
			
			i32CasRow = _pttTileC->resolutions[i32L-I].x0 % 2;
			i32CasCol = _pttTileC->resolutions[i32L-I].y0 % 2;
	        
			i32Sn = i32Rh1;
			i32Dn = i32Rh - i32Rh1;
			pi32Bj = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Rh) * sizeof( int32_t ) ));
			for ( J = 0; J < i32Rw; J++ ) {
				pi32Aj = pi32A + J;
				for ( K = 0; K < i32Rh; K++ ) { pi32Bj[K] = pi32Aj[K*i32W]; }
				dwt_encode_1( pi32Bj, i32Dn, i32Sn, i32CasCol );
				dwt_deinterleave_v( pi32Bj, pi32Aj, i32Dn, i32Sn, i32W, i32CasCol );
			}
			CMemLib::Free( pi32Bj );
			
			i32Sn = i32Rw1;
			i32Dn = i32Rw - i32Rw1;
			pi32Bj = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Rw) * sizeof( int32_t ) ));
			for ( J = 0; J < i32Rh; J++ ) {
				pi32Aj = pi32A + J * i32W;
				for ( K = 0; K < i32Rw; K++ ) { pi32Bj[K] = pi32Aj[K]; }
				dwt_encode_1( pi32Bj, i32Dn, i32Sn, i32CasRow );
				dwt_deinterleave_h( pi32Bj, pi32Aj, i32Dn, i32Sn, i32CasRow );
			}
			CMemLib::Free( pi32Bj );
		}
	}

	/**
	 * Inverse 5-3 wavelet tranform in 2-D.
	 *	Apply a reversible inverse DWT transform to a component of an image.
	 *
	 * \param _pttTileC Tile component information (current tile).
	 * \param _i32NumRes Number of resolution levels to decode.
	 */
	void LSE_CALL CDwt::dwt_decode( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32NumRes ) {
		dwt_decode_tile( _pttTileC, _i32NumRes, &dwt_decode_1 );
	}

	/**
	 * Gets the gain of a subband for the reversible 5-3 DWT.
	 *
	 * \param _i32Orient Number that identifies the subband (0->LL, 1->HL, 2->LH, 3->HH).
	 * \return Returns 0 if _i32Orient = 0, returns 1 if _i32Orient = 1 or 2, returns 2 otherwise.
	 */
	int32_t LSE_CALL CDwt::dwt_getgain( int32_t _i32Orient ) {
		if ( _i32Orient == 0 ) { return 0; }
		if ( _i32Orient == 1 || _i32Orient == 2 ) { return 1; }
		return 2;
	}

	/**
	 * Gets the norm of a wavelet function of a subband at a specified level for the reversible 5-3 DWT.
	 *
	 * \param _i32Level Level of the wavelet function.
	 * \param _i32Orient Band of the wavelet function.
	 * \return Returns the norm of the wavelet function.
	 */
	double LSE_CALL CDwt::dwt_getnorm( int32_t _i32Level, int32_t _i32Orient ) {
		return dwt_norms[_i32Orient][_i32Level];
	}

	/**
	 * Forward 9-7 wavelet transform in 2-D.
	 *
	 * Apply an irreversible DWT transform to a component of an image.
	 * \param _pttTileC Tile component information (current tile).
	 */
	void LSE_CALL CDwt::dwt_encode_real( CTcd::opj_tcd_tilecomp_t * _pttTileC ) {
		int32_t I, J, K;
		int32_t *pi32A = NULL;
		int32_t *pi32Aj = NULL;
		int32_t *pi32Bj = NULL;
		int32_t i32W, i32L;
		
		i32W = _pttTileC->x1-_pttTileC->x0;
		i32L = _pttTileC->numresolutions-1;
		pi32A = _pttTileC->data;
		
		for ( I = 0; I < i32L; I++ ) {
			int32_t i32Rw;			/* Width of the resolution level computed. */
			int32_t i32Rh;			/* Height of the resolution level computed. */
			int32_t i32Rw1;			/* Width of the resolution level once lower than computed one. */
			int32_t i32Rh1;			/* Height of the resolution level once lower than computed one. */
			int32_t i32CasCol;		/* 0 = non inversion on horizontal filtering 1 = inversion between low-pass and high-pass filtering. */
			int32_t i32CasRow;		/* 0 = non inversion on vertical filtering 1 = inversion between low-pass and high-pass filtering. */
			int32_t i32Dn, i32Sn;
			
			i32Rw = _pttTileC->resolutions[i32L-I].x1 - _pttTileC->resolutions[i32L-I].x0;
			i32Rh = _pttTileC->resolutions[i32L-I].y1 - _pttTileC->resolutions[i32L-I].y0;
			i32Rw1 = _pttTileC->resolutions[i32L-I-1].x1 - _pttTileC->resolutions[i32L-I-1].x0;
			i32Rh1 = _pttTileC->resolutions[i32L-I-1].y1 - _pttTileC->resolutions[i32L-I-1].y0;
			
			i32CasRow = _pttTileC->resolutions[i32L-I].x0 % 2;
			i32CasCol = _pttTileC->resolutions[i32L-I].y0 % 2;
			
			i32Sn = i32Rh1;
			i32Dn = i32Rh - i32Rh1;
			pi32Bj = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Rh) * sizeof( int32_t ) ));
			for ( J = 0; J < i32Rw; J++ ) {
				pi32Aj = pi32A + J;
				for ( K = 0; K < i32Rh; K++ ) { pi32Bj[K] = pi32Aj[K*i32W]; }
				dwt_encode_1_real( pi32Bj, i32Dn, i32Sn, i32CasCol );
				dwt_deinterleave_v( pi32Bj, pi32Aj, i32Dn, i32Sn, i32W, i32CasCol );
			}
			CMemLib::Free( pi32Bj );
			
			i32Sn = i32Rw1;
			i32Dn = i32Rw - i32Rw1;
			pi32Bj = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(i32Rw) * sizeof( int32_t ) ));
			for ( J = 0; J < i32Rh; J++ ) {
				pi32Aj = pi32A + J * i32W;
				for ( K = 0; K < i32Rw; K++ ) { pi32Bj[K] = pi32Aj[K]; }
				dwt_encode_1_real( pi32Bj, i32Dn, i32Sn, i32CasRow );
				dwt_deinterleave_h( pi32Bj, pi32Aj, i32Dn, i32Sn, i32CasRow );
			}
			CMemLib::Free( pi32Bj );
		}
	}

	/**
	 * Inverse 9-7 wavelet transform in 2-D.
	 *
	 * Apply an irreversible inverse DWT transform to a component of an image.
	 * \param _pttTileC Tile component information (current tile).
	 * \param _i32NumRes Number of resolution levels to decode.
	 */
	void LSE_CALL CDwt::dwt_decode_real( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32NumRes ) {
		v4dwt_t vH;
		v4dwt_t vV;

		CTcd::opj_tcd_resolution_t * ptrRes = _pttTileC->resolutions;

		int32_t i32Rw = ptrRes->x1 - ptrRes->x0;		// Width of the resolution level computed.
		int32_t i32Rh = ptrRes->y1 - ptrRes->y0;		// Height of the resolution level computed.

		int32_t i32W = _pttTileC->x1 - _pttTileC->x0;

		vH.wavelet = static_cast<v4 *>(CMemLib::MAlloc( (static_cast<uint32_t>(dwt_decode_max_resolution( ptrRes, _i32NumRes )) + 5) * sizeof( v4 ) ));
		vV.wavelet = vH.wavelet;

		while ( --_i32NumRes ) {
			float * LSE_RESTRICT pfAj = reinterpret_cast<float *>(_pttTileC->data);
			int32_t i32BufSize = (_pttTileC->x1 - _pttTileC->x0) * (_pttTileC->y1 - _pttTileC->y0);
			int32_t J;

			vH.sn = i32Rw;
			vV.sn = i32Rh;

			++ptrRes;

			i32Rw = ptrRes->x1 - ptrRes->x0;			// Width of the resolution level computed.
			i32Rh = ptrRes->y1 - ptrRes->y0;			// Height of the resolution level computed.

			vH.dn = i32Rw - vH.sn;
			vH.cas = ptrRes->x0 % 2;

			for ( J = i32Rh; J > 3; J -= 4 ) {
				int32_t K;
				v4dwt_interleave_h( &vH, pfAj, i32W, i32BufSize );
				v4dwt_decode( &vH );
				for ( K = i32Rw; --K >= 0; ) {
					pfAj[K] = vH.wavelet[K].f[0];
					pfAj[K+i32W] = vH.wavelet[K].f[1];
					pfAj[K+i32W*2] = vH.wavelet[K].f[2];
					pfAj[K+i32W*3] = vH.wavelet[K].f[3];
				}
				pfAj += i32W*4;
				i32BufSize -= i32W*4;
			}
			if ( i32Rh & 0x03 ) {
				int32_t K;
				J = i32Rh & 0x03;
				v4dwt_interleave_h( &vH, pfAj, i32W, i32BufSize );
				v4dwt_decode( &vH );
				for ( K = i32Rw; --K >= 0; ) {
					switch( J ) {
						case 3 : { pfAj[K+i32W*2] = vH.wavelet[K].f[2]; }
						case 2 : { pfAj[K+i32W] = vH.wavelet[K].f[1]; }
						case 1 : { pfAj[K] = vH.wavelet[K].f[0]; }
					}
				}
			}

			vV.dn = i32Rh - vV.sn;
			vV.cas = ptrRes->y0 % 2;

			pfAj = reinterpret_cast<float *>(_pttTileC->data);
			for ( J = i32Rw; J > 3; J -= 4 ) {
				int32_t K;
				v4dwt_interleave_v( &vV, pfAj, i32W );
				v4dwt_decode( &vV );
				for ( K = 0; K < i32Rh; ++K ) {
					CStd::MemCpy( &pfAj[K*i32W], &vV.wavelet[K], 4 * sizeof( float ) );
				}
				pfAj += 4;
			}
			if ( i32Rw & 0x03 ) {
				int32_t K;
				J = i32Rw & 0x03;
				v4dwt_interleave_v( &vV, pfAj, i32W );
				v4dwt_decode( &vV );
				for ( K = 0; K < i32Rh; ++K ) {
					CStd::MemCpy( &pfAj[K*i32W], &vV.wavelet[K], static_cast<uint32_t>(J) * sizeof( float ) );
				}
			}
		}

		CMemLib::Free( vH.wavelet );
	}

	/**
	 * Gets the gain of a subband for the irreversible 9-7 DWT.
	 *
	 * \param _i32Orient Number that identifies the subband (0->LL, 1->HL, 2->LH, 3->HH).
	 * \return Returns the gain of the 9-7 wavelet transform.
	 */
	int32_t LSE_CALL CDwt::dwt_getgain_real( int32_t /*_i32Orient*/ ) {
		return 0;
	}

	/**
	 * Gets the norm of a wavelet function of a subband at a specified level for the irreversible 9-7 DWT.
	 *
	 * \param _i32Level Level of the wavelet function.
	 * \param _i32Orient Band of the wavelet function.
	 * \return Returns the norm of the 9-7 wavelet.
	 */
	double LSE_CALL CDwt::dwt_getnorm_real( int32_t _i32Level, int32_t _i32Orient ) {
		return dwt_norms_real[_i32Orient][_i32Level];
	}

	/**
	 * Explicit calculation of the Quantization Stepsizes.
	 *
	 * \param _ptTccp Tile-component coding parameters.
	 * \param _i32Prec Precint analyzed.
	 */
	void LSE_CALL CDwt::dwt_calc_explicit_stepsizes( CJ2k::opj_tccp_t * _ptTccp, int32_t _i32Prec ) {
		int32_t i32NumBands, i32BandNo;
		i32NumBands = 3 * _ptTccp->numresolutions - 2;
		for ( i32BandNo = 0; i32BandNo < i32NumBands; i32BandNo++ ) {
			double dStepSize;
			int32_t i32ResNo, i32Level, i32Orient, i32Gain;

			i32ResNo = (i32BandNo == 0) ? 0 : ((i32BandNo - 1) / 3 + 1);
			i32Orient = (i32BandNo == 0) ? 0 : ((i32BandNo - 1) % 3 + 1);
			i32Level = _ptTccp->numresolutions - 1 - i32ResNo;
			i32Gain = (_ptTccp->qmfbid == 0) ? 0 : ((i32Orient == 0) ? 0 : (((i32Orient == 1) || (i32Orient == 2)) ? 1 : 2));
			if ( _ptTccp->qntsty == J2K_CCP_QNTSTY_NOQNT ) {
				dStepSize = 1.0;
			}
			else {
				double norm = dwt_norms_real[i32Orient][i32Level];
				dStepSize = (1 << i32Gain) / norm;
			}
			dwt_encode_stepsize( static_cast<int32_t>(::floor( dStepSize * 8192.0 )), _i32Prec + i32Gain, &_ptTccp->stepsizes[i32BandNo] );
		}
	}

	/**
	 * Forward lazy transform (horizontal).
	 *
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32B Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_deinterleave_h( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas ) {
		int32_t I;
		for ( I = 0; I < _i32Sn; I++ ) { _pi32B[I] = _pi32A[2*I+_i32Cas]; }
		for ( I = 0; I < _i32Dn; I++ ) { _pi32B[_i32Sn+I] = _pi32A[2*I+1-_i32Cas]; }
	}

	/**
	 * Forward lazy transform (vertical).
	 *
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32B Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_deinterleave_v( int32_t * _pi32A, int32_t * _pi32B, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32X, int32_t _i32Cas ) {
		int32_t I;
		for ( I = 0; I < _i32Sn; I++ ) { _pi32B[I*_i32X] = _pi32A[2*I+_i32Cas]; }
		for ( I = 0; I < _i32Dn; I++ ) { _pi32B[(_i32Sn+I)*_i32X] = _pi32A[( 2*I+1-_i32Cas )]; }
	}

	/**
	 * Inverse lazy transform (horizontal).
	 *
	 * \param _pdH Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_interleave_h( dwt_t * _pdH, int32_t * _pi32A ) {
		int32_t * pi32Ai = _pi32A;
		int32_t * pi32Bi = _pdH->mem + _pdH->cas;
		int32_t I = _pdH->sn;
		while ( I-- ) {
			(*pi32Bi) = (*(pi32Ai++));
			pi32Bi += 2;
		}
		pi32Ai = _pi32A + _pdH->sn;
		pi32Bi = _pdH->mem + 1 - _pdH->cas;
		I = _pdH->dn ;
		while ( I-- ) {
			(*pi32Bi) = (*(pi32Ai++));
			pi32Bi += 2;
		}
	}

	/**
	 * Inverse lazy transform (vertical).
	 *
	 * \param _pdV Undocumented parameter from the OpenJpeg Library.
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_interleave_v( dwt_t * _pdV, int32_t * _pi32A, int32_t _i32X ) {
		int32_t * pi32Ai = _pi32A;
		int32_t * pi32Bi = _pdV->mem + _pdV->cas;
		int32_t  I = _pdV->sn;
		while ( I-- ) {
			(*pi32Bi) = (*pi32Ai);
			pi32Bi += 2;
			pi32Ai += _i32X;
		}
		pi32Ai = _pi32A + (_pdV->sn * _i32X);
		pi32Bi = _pdV->mem + 1 - _pdV->cas;
		I = _pdV->dn ;
		while ( I-- ) {
			(*pi32Bi) = (*pi32Ai);
			pi32Bi += 2;  
			pi32Ai += _i32X;
		}
	}

	/**
	 * Forward 5-3 wavelet transform in 1-D.
	 *
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_encode_1( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas ) {
		int32_t I;
	
		if ( !_i32Cas ) {
			if ( (_i32Dn > 0) || (_i32Sn > 1) ) {
				for ( I = 0; I < _i32Dn; I++ ) { D( I ) -= ( S_( I ) + S_( I + 1 ) ) >> 1; }
				for ( I = 0; I < _i32Sn; I++ ) { S( I ) += ( D_( I - 1 ) + D_( I ) + 2 ) >> 2; }
			}
		}
		else {
			if ( !_i32Sn && _i32Dn == 1 ) {
				S( 0 ) *= 2;
			}
			else {
				for ( I = 0; I < _i32Dn; I++ ) { S( I ) -= (DD_( I ) + DD_( I - 1 )) >> 1; }
				for ( I = 0; I < _i32Sn; I++ ) { D( I ) += (SS_( I ) + SS_( I + 1 ) + 2) >> 2; }
			}
		}
	}

	/**
	 * Inverse 5-3 wavelet transform in 1-D.
	 *
	 * \param _pdV Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_decode_1( dwt_t * _pdV ) {
		dwt_decode_1_( _pdV->mem, _pdV->dn, _pdV->sn, _pdV->cas );
	}

	/**
	 * Forward 9-7 wavelet transform in 1-D.
	 *
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_encode_1_real( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas ) {
		int32_t I;
		if ( !_i32Cas ) {
			if ( (_i32Dn > 0) || (_i32Sn > 1) ) {
				for ( I = 0; I < _i32Dn; I++ ) {
					D( I ) -= CJpegFixed::Mul( S_( I ) + S_( I + 1 ), 12993 );
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					S( I ) -= CJpegFixed::Mul( D_( I - 1 ) + D_( I ), 434 );
				}
				for ( I = 0; I < _i32Dn; I++ ) {
					D( I ) += CJpegFixed::Mul( S_( I ) + S_( I + 1 ), 7233 );
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					S( I ) += CJpegFixed::Mul( D_( I - 1 ) + D_( I ), 3633 );
				}
				for ( I = 0; I < _i32Dn; I++ ) {
					D( I ) = CJpegFixed::Mul( D( I ), 5038 );	// 5038.
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					S( I ) = CJpegFixed::Mul( S( I ), 6659 );	// 6660.
				}
			}
		}
		else {
			if ( ( _i32Sn > 0 ) || ( _i32Dn > 1 ) ) {
				for ( I = 0; I < _i32Dn; I++ ) {
					S( I ) -= CJpegFixed::Mul( DD_( I ) + DD_( I - 1 ), 12993 );
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					D( I ) -= CJpegFixed::Mul( SS_( I ) + SS_( I + 1 ), 434 );
				}
				for ( I = 0; I < _i32Dn; I++ ) {
					S( I ) += CJpegFixed::Mul( DD_( I ) + DD_( I - 1 ), 7233 );
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					D( I ) += CJpegFixed::Mul( SS_( I ) + SS_( I + 1 ), 3633 );
				}
				for ( I = 0; I < _i32Dn; I++ ) {
					S( I ) = CJpegFixed::Mul( S( I ), 5038 );	// 5038.
				}
				for ( I = 0; I < _i32Sn; I++ ) {
					D( I ) = CJpegFixed::Mul( D( I ), 6659 );	// 6660.
				}
			}
		}
	}

	/**
	 * Explicit calculation of the Quantization Stepsizes .
	 *
	 * \param _i32StepSize Undocumented parameter from the OpenJpeg Library.
	 * \param _i32NumBps Undocumented parameter from the OpenJpeg Library.
	 * \param _psBandNoStepSize Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_encode_stepsize( int32_t _i32StepSize, int32_t _i32NumBps, CJ2k::opj_stepsize_t * _psBandNoStepSize ) {
		int32_t i32P, i32N;
		i32P = CJpegInt::int_floorlog2( _i32StepSize ) - 13;
		i32N = 11 - CJpegInt::int_floorlog2( _i32StepSize );
		_psBandNoStepSize->mant = (i32N < 0 ? _i32StepSize >> -i32N : _i32StepSize << i32N) & 0x7FF;
		_psBandNoStepSize->expn = _i32NumBps - i32P;
	}

	/**
	 * Inverse wavelet transform in 2-D.
	 *
	 * \param _pttTileC Undocumented parameter from the OpenJpeg Library.
	 * \param _i32I Undocumented parameter from the OpenJpeg Library.
	 * \param _dFn Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_decode_tile( CTcd::opj_tcd_tilecomp_t * _pttTileC, int32_t _i32I, DWT1DFN _dFn ) {
		dwt_t dH;
		dwt_t dV;

		CTcd::opj_tcd_resolution_t* ptrTr = _pttTileC->resolutions;

		int32_t i32Rw = ptrTr->x1 - ptrTr->x0;	/* width of the resolution level computed */
		int32_t i32Rh = ptrTr->y1 - ptrTr->y0;	/* height of the resolution level computed */

		int32_t i32W = _pttTileC->x1 - _pttTileC->x0;

		dH.mem = static_cast<int32_t *>(CMemLib::MAlloc( static_cast<uint32_t>(dwt_decode_max_resolution( ptrTr, _i32I )) * sizeof( int32_t ) ));
		dV.mem = dH.mem;

		while ( --_i32I ) {
			int32_t * LSE_RESTRICT pi32TileDp = _pttTileC->data;
			int32_t J;

			++ptrTr;
			dH.sn = i32Rw;
			dV.sn = i32Rh;

			i32Rw = ptrTr->x1 - ptrTr->x0;
			i32Rh = ptrTr->y1 - ptrTr->y0;

			dH.dn = i32Rw - dH.sn;
			dH.cas = ptrTr->x0 % 2;

			for ( J = 0; J < i32Rh; ++J ) {
				dwt_interleave_h( &dH, &pi32TileDp[J*i32W] );
				_dFn( &dH );
				CStd::MemCpy( &pi32TileDp[J*i32W], dH.mem, static_cast<uint32_t>(i32Rw) * sizeof( int32_t ) );
			}

			dV.dn = i32Rh - dV.sn;
			dV.cas = ptrTr->y0 % 2;

			for ( J = 0; J < i32Rw; ++J ) {
				int32_t K;
				dwt_interleave_v( &dV, &pi32TileDp[J], i32W );
				_dFn( &dV );
				for ( K = 0; K < i32Rh; ++K ) {
					pi32TileDp[K*i32W+J] = dV.mem[K];
				}
			}
		}
		CMemLib::Free( dH.mem );
	}

	/**
	 * Determine maximum computed resolution level for inverse wavelet transform.
	 *
	 * \param _ptrR Undocumented parameter from the OpenJpeg Library.
	 * \param _i32I Undocumented parameter from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CDwt::dwt_decode_max_resolution( CTcd::opj_tcd_resolution_t * LSE_RESTRICT _ptrR, int32_t _i32I ) {
		int32_t i32Mr = 1;
		int32_t i32W;
		while ( --_i32I ) {
			_ptrR++;
			if ( i32Mr < (i32W = _ptrR->x1 - _ptrR->x0) ) { i32Mr = i32W; }
			if ( i32Mr < (i32W = _ptrR->y1 - _ptrR->y0) ) { i32Mr = i32W; }
		}
		return i32Mr;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pvW Undocumented parameter from the OpenJpeg Library.
	 * \param _pfA Undocumented parameter from the OpenJpeg Library.
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Size Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::v4dwt_interleave_h( v4dwt_t * LSE_RESTRICT _pvW, float * LSE_RESTRICT _pfA, int32_t _i32X, int32_t _i32Size ) {
		float * LSE_RESTRICT pfBi = reinterpret_cast<float *>(_pvW->wavelet + _pvW->cas);
		int32_t i32Count = _pvW->sn;
		int32_t I, K;
		for ( K = 0; K < 2; ++K ) {
			if ( i32Count + 3 * _i32X < _i32Size && (reinterpret_cast<uintptr_t>(_pfA) & 0x0F) == 0 && (reinterpret_cast<uintptr_t>(pfBi) & 0x0F) == 0 && (_i32X & 0x0F) == 0 ) {
				// Fast code path.
				for ( I = 0; I < i32Count; ++I ) {
					int32_t i32J = I;
					pfBi[I*8] = _pfA[i32J];
					i32J += _i32X;
					pfBi[I*8+1] = _pfA[i32J];
					i32J += _i32X;
					pfBi[I*8+2] = _pfA[i32J];
					i32J += _i32X;
					pfBi[I*8+3] = _pfA[i32J];
				}
			}
			else {
				// Slow code path.
				for ( I = 0; I < i32Count; ++I ) {
					int32_t i32J = I;
					pfBi[I*8] = _pfA[i32J];
					i32J += _i32X;
					if ( i32J > _i32Size ) { continue; }
					pfBi[I*8+1] = _pfA[i32J];
					i32J += _i32X;
					if ( i32J > _i32Size ) { continue; }
					pfBi[I*8+2] = _pfA[i32J];
					i32J += _i32X;
					if ( i32J > _i32Size ) { continue; }
					pfBi[I*8+3] = _pfA[i32J];
				}
			}
			pfBi = reinterpret_cast<float *>(_pvW->wavelet + 1 - _pvW->cas);
			_pfA += _pvW->sn;
			_i32Size -= _pvW->sn;
			i32Count = _pvW->dn;
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pvV Undocumented parameter from the OpenJpeg Library.
	 * \param _pfA Undocumented parameter from the OpenJpeg Library.
	 * \param _i32X Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::v4dwt_interleave_v( v4dwt_t * LSE_RESTRICT _pvV, float * LSE_RESTRICT _pfA, int32_t _i32X ) {
		v4 * LSE_RESTRICT pvBi = _pvV->wavelet + _pvV->cas;
		int32_t I;
		for ( I = 0; I < _pvV->sn; ++I ) {
			CStd::MemCpy( &pvBi[I<<1], &_pfA[I*_i32X], 4 * sizeof( float ) );
		}
		_pfA += _pvV->sn * _i32X;
		pvBi = _pvV->wavelet + 1 - _pvV->cas;
		for ( I = 0; I < _pvV->dn; ++I ) {
			CStd::MemCpy( &pvBi[I<<1], &_pfA[I*_i32X], 4 * sizeof( float ) );
		}
	}

	/**
	 * Inverse 9-7 wavelet transform in 1-D.
	 *
	 * \param _pvDwt Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::v4dwt_decode( v4dwt_t * LSE_RESTRICT _pvDwt ) {
		int32_t i32A, i32B;
		if ( _pvDwt->cas == 0 ) {
			if ( !((_pvDwt->dn > 0) || (_pvDwt->sn > 1)) ) { return; }
			i32A = 0;
			i32B = 1;
		}
		else {
			if ( !((_pvDwt->sn > 0) || (_pvDwt->dn > 1)) ) { return; }
			i32A = 1;
			i32B = 0;
		}
		v4dwt_decode_step1( _pvDwt->wavelet+i32A, _pvDwt->sn, m_fK );
		v4dwt_decode_step1( _pvDwt->wavelet+i32B, _pvDwt->dn, m_c13318 );
		v4dwt_decode_step2( _pvDwt->wavelet+i32B, _pvDwt->wavelet+i32A+1, _pvDwt->sn, CJpegInt::int_min( _pvDwt->sn, _pvDwt->dn-i32A ), m_dwt_delta );
		v4dwt_decode_step2( _pvDwt->wavelet+i32A, _pvDwt->wavelet+i32B+1, _pvDwt->dn, CJpegInt::int_min( _pvDwt->dn, _pvDwt->sn-i32B ), m_dwt_gamma );
		v4dwt_decode_step2( _pvDwt->wavelet+i32B, _pvDwt->wavelet+i32A+1, _pvDwt->sn, CJpegInt::int_min( _pvDwt->sn, _pvDwt->dn-i32A ), m_dwt_beta );
		v4dwt_decode_step2( _pvDwt->wavelet+i32A, _pvDwt->wavelet+i32B+1, _pvDwt->dn, CJpegInt::int_min( _pvDwt->dn, _pvDwt->sn-i32B ), m_dwt_alpha );
	}

	/**
	 * Inverse 5-3 wavelet transform in 1-D.
	 *
	 * \param _pi32A Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Dn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Sn Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Cas Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::dwt_decode_1_( int32_t * _pi32A, int32_t _i32Dn, int32_t _i32Sn, int32_t _i32Cas ) {
		int32_t I;
	
		if ( !_i32Cas ) {
			if ( (_i32Dn > 0) || (_i32Sn > 1) ) {
				for ( I = 0; I < _i32Sn; I++ ) { S( I ) -= (D_( I - 1 ) + D_( I ) + 2) >> 2; }
				for ( I = 0; I < _i32Dn; I++ ) { D( I ) += (S_( I ) + S_( I + 1 )) >> 1; }
			}
		}
		else {
			if ( !_i32Sn  && _i32Dn == 1 ) {
				S( 0 ) /= 2;
			}
			else {
				for ( I = 0; I < _i32Sn; I++ ) { D( I ) -= (SS_( I ) + SS_( I + 1 ) + 2) >> 2; }
				for ( I = 0; I < _i32Dn; I++ ) { S( I ) += (DD_( I ) + DD_( I - 1 )) >> 1; }
			}
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pvW Undocumented parameter from the OpenJpeg Library.
	 * \param _i32Count Undocumented parameter from the OpenJpeg Library.
	 * \param _fC Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::v4dwt_decode_step1( v4 * _pvW, int32_t _i32Count, const float _fC ) {
		float * LSE_RESTRICT pfFw = reinterpret_cast<float *>(_pvW);
		int32_t I;
		for ( I = 0; I < _i32Count; ++I ) {
			float fTemp1 = pfFw[I*8];
			float fTemp2 = pfFw[I*8+1];
			float fTemp3 = pfFw[I*8+2];
			float fTemp4 = pfFw[I*8+3];
			pfFw[I*8] = fTemp1 * _fC;
			pfFw[I*8+1] = fTemp2 * _fC;
			pfFw[I*8+2] = fTemp3 * _fC;
			pfFw[I*8+3] = fTemp4 * _fC;
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pvL Undocumented parameter from the OpenJpeg Library.
	 * \param _pvW Undocumented parameter from the OpenJpeg Library.
	 * \param _i32K Undocumented parameter from the OpenJpeg Library.
	 * \param _i32M Undocumented parameter from the OpenJpeg Library.
	 * \param _fC Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CDwt::v4dwt_decode_step2( v4 * _pvL, v4 * _pvW, int32_t _i32K, int32_t _i32M, float _fC ) {
		float * LSE_RESTRICT fl = reinterpret_cast<float *>(_pvL);
		float * LSE_RESTRICT fw = reinterpret_cast<float *>(_pvW);
		int32_t I;
		for ( I = 0; I < _i32M; ++I ) {
			float fTemp1_1 = fl[0];
			float fTemp1_2 = fl[1];
			float fTemp1_3 = fl[2];
			float fTemp1_4 = fl[3];
			float fTemp2_1 = fw[-4];
			float fTemp2_2 = fw[-3];
			float fTemp2_3 = fw[-2];
			float fTemp2_4 = fw[-1];
			float fTemp3_1 = fw[0];
			float fTemp3_2 = fw[1];
			float fTemp3_3 = fw[2];
			float fTemp3_4 = fw[3];
			fw[-4] = fTemp2_1 + ((fTemp1_1 + fTemp3_1) * _fC);
			fw[-3] = fTemp2_2 + ((fTemp1_2 + fTemp3_2) * _fC);
			fw[-2] = fTemp2_3 + ((fTemp1_3 + fTemp3_3) * _fC);
			fw[-1] = fTemp2_4 + ((fTemp1_4 + fTemp3_4) * _fC);
			fl = fw;
			fw += 8;
		}
		if ( _i32M < _i32K ) {
			float fC1;
			float fC2;
			float fC3;
			float fC4;
			_fC += _fC;
			fC1 = fl[0] * _fC;
			fC2 = fl[1] * _fC;
			fC3 = fl[2] * _fC;
			fC4 = fl[3] * _fC;
			for ( ; _i32M < _i32K; ++_i32M ) {
				float fTemp1 = fw[-4];
				float fTemp2 = fw[-3];
				float fTemp3 = fw[-2];
				float fTemp4 = fw[-1];
				fw[-4] = fTemp1 + fC1;
				fw[-3] = fTemp2 + fC2;
				fw[-2] = fTemp3 + fC3;
				fw[-1] = fTemp4 + fC4;
				fw += 8;
			}
		}
	}

}	// namespace lsi
