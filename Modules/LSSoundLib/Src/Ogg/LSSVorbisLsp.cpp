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

  function: LSP ( also called LSF ) conversion routines
  last mod: $Id: lsp.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisLsp.h"
#include "LSSVorbisCodecInternal.h"
#include "Search/LSSTDSearch.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {
	// == Definitions.
#define LSS_LSP_EPSILON					10e-7			// Just for this file.

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfLpc Undocumented parameter from the Vorbis library.
	 * \param _pfLsp Undocumented parameter from the Vorbis library.
	 * \param _i32M Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisLsp::vorbis_lpc_to_lsp( float * _pfLpc, float * _pfLsp, int32_t _i32M ) {
		int32_t i32Order2=( _i32M+1 ) >> 1;
		int32_t i32G1Order, i32G2Order;
		float * pfG1 = static_cast<float *>(::alloca( sizeof( (*pfG1) ) * (i32Order2 + 1) ));
		float * pfG2 = static_cast<float *>(::alloca( sizeof( (*pfG2) ) * (i32Order2 + 1) ));
		float * pfG1r = static_cast<float *>(::alloca( sizeof( (*pfG1r) ) * (i32Order2 + 1) ));
		float * pfG2r = static_cast<float *>(::alloca( sizeof( (*pfG2r) ) * (i32Order2 + 1) ));
		int32_t I;

		// Even and odd are slightly different base cases.
		i32G1Order = (_i32M + 1) >> 1;
		i32G2Order = (_i32M) >> 1;

		// Computes the lengths of the x polynomials.
		// Computes the first half of K & R F1 & F2 polynomials.
		// Compute half of the symmetric and antisymmetric polynomials.
		// Remove the roots at +1 and -1.

		pfG1[i32G1Order] = 1.0f;
		for ( I = 1; I <= i32G1Order; I++ ) { pfG1[i32G1Order-I] = _pfLpc[I-1] + _pfLpc[_i32M-I]; }
		pfG2[i32G2Order] = 1.0f;
		for ( I = 1; I <= i32G2Order; I++ ) { pfG2[i32G2Order-I] = _pfLpc[I-1] - _pfLpc[_i32M-I]; }

		if ( i32G1Order>i32G2Order ) {
			for ( I = 2; I <= i32G2Order; I++ ) { pfG2[i32G2Order-I] += pfG2[i32G2Order-I+2]; }
		}
		else {
			for ( I = 1; I <= i32G1Order; I++ ) { pfG1[i32G1Order-I] -= pfG1[i32G1Order-I+1]; }
			for ( I = 1; I <= i32G2Order; I++ ) { pfG2[i32G2Order-I] += pfG2[i32G2Order-I+1]; }
		}

		// Convert into polynomials in ::cos( alpha ).
		cheby( pfG1, i32G1Order );
		cheby( pfG2, i32G2Order );

		// Find the roots of the 2 even polynomials.
		if ( Laguerre_With_Deflation( pfG1, i32G1Order, pfG1r ) ||
			Laguerre_With_Deflation( pfG2, i32G2Order, pfG2r ) ) {
			return -1;
		}

		Newton_Raphson( pfG1, i32G1Order, pfG1r ); // If it fails, it leaves pfG1r alone.
		Newton_Raphson( pfG2, i32G2Order, pfG2r ); // If it fails, it leaves pfG2r alone.

		CSearch::QSort( pfG1r, i32G1Order, sizeof( (*pfG1r) ), comp, NULL );
		CSearch::QSort( pfG2r, i32G2Order, sizeof( (*pfG2r) ), comp, NULL );

		for ( I = 0; I < i32G1Order; I++ ) {
			_pfLsp[I*2] = ::acosf( pfG1r[I] );
		}

		for ( I = 0; I < i32G2Order; I++ ) {
			_pfLsp[I*2+1] = ::acosf( pfG2r[I] );
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfCurve Undocumented parameter from the Vorbis library.
	 * \param _pi32Map Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32Ln Undocumented parameter from the Vorbis library.
	 * \param _pfLsp Undocumented parameter from the Vorbis library.
	 * \param _i32M Undocumented parameter from the Vorbis library.
	 * \param _fAmp Undocumented parameter from the Vorbis library.
	 * \param _fAmpOffset Undocumented parameter from the Vorbis library.
	 */
	void CVorbisLsp::vorbis_lsp_to_curve( float * _pfCurve, int32_t * _pi32Map, int32_t _i32N, int32_t _i32Ln, 
		float * _pfLsp, int32_t _i32M, 
		float _fAmp, float _fAmpOffset ) {
		int32_t I;
		float fWDel = static_cast<float>(LSM_PI_DOUBLE / _i32Ln);
		for ( I = 0; I < _i32M; I++ ) { _pfLsp[I] = 2.0f * ::cosf( _pfLsp[I] ); }

		I = 0;
		while ( I < _i32N ) {
			int32_t J, i32K = _pi32Map[I];
			float fP = 0.5f;
			float fQ = 0.5f;
			float fW = 2.0f * ::cosf( fWDel * i32K );
			for ( J = 1;J<_i32M;J+=2 ) {
				fQ *= fW-_pfLsp[J-1];
				fP *= fW-_pfLsp[J];
			}
			if ( J == _i32M ) {
				// Odd order filter; slightly assymetric.
				// The last coefficient.
				fQ *= fW - _pfLsp[J-1];
				fP *= fP * (4.0f - fW * fW);
				fQ *= fQ;
			}
			else {
				// Even order filter; still symmetric.
				fP *= fP * (2.0f - fW);
				fQ *= fQ * (2.0f + fW);
			}

			fQ = static_cast<float>(fromdB( _fAmp / ::sqrt( fP + fQ ) - _fAmpOffset ));

			_pfCurve[I] *= fQ;
			while ( _pi32Map[++I] == i32K ) { _pfCurve[I] *= fQ; }
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfG Undocumented parameter from the Vorbis library.
	 * \param _i32Ord Undocumented parameter from the Vorbis library.
	 */
	void CVorbisLsp::cheby( float * _pfG, int32_t _i32Ord ) {
		int32_t I, J;

		_pfG[0] *= 0.5f;
		for ( I = 2; I <= _i32Ord; I++ ) {
			for ( J = _i32Ord; J >= I; J-- ) {
				_pfG[J-2] -= _pfG[J];
				_pfG[J] += _pfG[J];
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _i32Ord Undocumented parameter from the Vorbis library.
	 * \param _pfR Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisLsp::Laguerre_With_Deflation( float * _pfA, int32_t _i32Ord, float * _pfR ) {
		int32_t I, i32M;
		double dLastDelta = 0.f;
		double * pdDefl = static_cast<double *>(::alloca( sizeof( (*pdDefl) ) * (_i32Ord + 1) ));
		for ( I = 0; I <= _i32Ord; I++ ) { pdDefl[I] = _pfA[I]; }

		for ( i32M = _i32Ord; i32M > 0; i32M-- ) {
			double dNew = 0.0, dDelta;

			// Iterate a pdRoot.
			while ( true ) {
				double fP = pdDefl[i32M], dPp = 0.0, dPpp = 0.0, dDenom;

				// Eval the polynomial and its first two derivatives.
				for ( I = i32M; I > 0; I-- ) {
					dPpp = dNew * dPpp + dPp;
					dPp = dNew * dPp + fP;
					fP = dNew * fP + pdDefl[I-1];
				}

				// Laguerre's method.
				dDenom = (i32M - 1) * ((i32M - 1) * dPp * dPp - i32M * fP * dPpp);
				if ( dDenom < 0 ) {
					return -1;	// Complex pdRoot!  The LPC generator handed us a bad filter.
				}

				if ( dPp > 0 ) {
					dDenom = dPp + ::sqrt( dDenom );
					if ( dDenom < LSS_LSP_EPSILON ) { dDenom = LSS_LSP_EPSILON; }
				}
				else {
					dDenom = dPp - ::sqrt( dDenom );
					if ( dDenom > -LSS_LSP_EPSILON ) { dDenom = -LSS_LSP_EPSILON; }
				}

				dDelta = i32M * fP / dDenom;
				dNew -= dDelta;

				if ( dDelta < 0.0 ) { dDelta *= -1.0; }

				if ( ::fabs( dDelta / dNew ) < 10e-12 ) { break; }
				dLastDelta = dDelta;
			}

			_pfR[i32M-1] = static_cast<float>(dNew);

			// Forward deflation.
			for ( I = i32M; I > 0; I-- ) {
				pdDefl[I-1] += dNew * pdDefl[I];
			}
			pdDefl++;

		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfA Undocumented parameter from the Vorbis library.
	 * \param _i32Ord Undocumented parameter from the Vorbis library.
	 * \param _pfR Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisLsp::Newton_Raphson( float * _pfA, int32_t _i32Ord, float * _pfR ) {
		int32_t I, i32K, i32Count = 0;
		double dError = 1.0;
		double * pdRoot = static_cast<double *>(::alloca( _i32Ord * sizeof( (*pdRoot) ) ));

		for ( I = 0; I < _i32Ord; I++ ) { pdRoot[I] = _pfR[I]; }

		while ( dError > 1e-20 ) {
			dError = 0;

			for ( I = 0; I < _i32Ord; I++ ) {	// Update each point.
				double dPp = 0.0, dDelta;
				double dRootI = pdRoot[I];
				double fP = _pfA[_i32Ord];
				for ( i32K = _i32Ord - 1; i32K >= 0; i32K-- ) {
					dPp = dPp * dRootI + fP;
					fP = fP * dRootI + _pfA[i32K];
				}

				dDelta = fP / dPp;
				pdRoot[I] -= dDelta;
				dError += dDelta * dDelta;
			}

			if ( i32Count > 40 ) { return -1; }

			i32Count++;
		}

		// Replaced the original bubble sort with a real sort.  With your
		//	help, we can eliminate the bubble sort in our lifetime. --Monty

		for ( I = 0; I < _i32Ord; I++ ) { _pfR[I] = static_cast<float>(pdRoot[I]); }
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvContext Unused.
	 * \param _pvA Undocumented parameter from the Vorbis library.
	 * \param _pvB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t LSE_CCALL CVorbisLsp::comp( void * /*_pvContext*/, const void * _pvA, const void * _pvB ) {
		return ((*static_cast<const float *>(_pvA)) < (*static_cast<const float *>(_pvB))) -
			((*static_cast<const float *>(_pvA)) > (*static_cast<const float *>(_pvB)));
	}

#undef LSS_LSP_EPSILON

}	// namespace lss

#pragma warning( pop )
