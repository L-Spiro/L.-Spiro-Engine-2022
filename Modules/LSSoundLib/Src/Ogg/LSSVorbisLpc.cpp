/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS ( C ) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

  function: LPC low level routines
  last mod: $Id: pdLpc.h 16037 2009-05-26 21:10:58Z xiphmont $

 ********************************************************************/

#include "LSSVorbisLpc.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfData Undocumented parameter from the Vorbis library.
	 * \param _pfLpcI Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32M Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float CVorbisLpc::vorbis_lpc_from_data( float * _pfData, float * _pfLpcI, int32_t _i32N, int32_t _i32M ) {
		double * pdAut = static_cast<double *>(::alloca( sizeof( (*pdAut) ) * (_i32M + 1) ));
		double * pdLpc = static_cast<double *>(::alloca( sizeof( (*pdLpc) ) * _i32M ));
		double dError;
		double dEpsilon;
		int32_t I, J;

		// Autocorrelation, i32P+1 lag coefficients.
		J = _i32M + 1;
		while ( J-- ) {
			double dDepth = 0; // double needed for accumulator depth.
			for ( I = J; I < _i32N; I++ ) { dDepth += static_cast<double>(_pfData[I]) * static_cast<double>(_pfData[I-J]); }
			pdAut[J] = dDepth;
		}

		// Generate pdLpc coefficients from autocorr values.

		// Set our noise floor to about -100dB.
		dError = pdAut[0] * (1.0 + 1e-10);
		dEpsilon = 1e-9 * pdAut[0] + 1e-10;

		for ( I = 0; I < _i32M; I++ ) {
			double dR = -pdAut[I+1];

			if ( dError < dEpsilon ) {
				CStd::MemSet( pdLpc + I, 0, (_i32M - I) * sizeof( (*pdLpc) ) );
				goto Done;
			}

			// Sum up this iteration's reflection coefficient; note that in
			//	Vorbis we don't save it.  If anyone wants to recycle this code
			//	and needs reflection coefficients, save the results of 'dR' from
			//	each iteration.

			for ( J = 0; J < I; J++ ) { dR -= pdLpc[J] * pdAut[I-J]; }
			dR /= dError;

			// Update LPC coefficients and total error.

			pdLpc[I] = dR;
			for ( J = 0; J < I / 2; J++ ) {
				double dTmp = pdLpc[J];

				pdLpc[J] += dR * pdLpc[I-1-J];
				pdLpc[I-1-J] += dR * dTmp;
			}
			if ( I & 1 ) { pdLpc[J] += pdLpc[J] * dR; }

			dError *= 1.0 - dR * dR;

		}

	Done :

		// Slightly dDamp the filter.
		{
			double dG = 0.99;
			double dDamp = dG;
			for ( J = 0; J < _i32M; J++ ) {
				pdLpc[J] *= dDamp;
				dDamp *= dG;
			}
		}

		for ( J = 0; J < _i32M; J++ ) { _pfLpcI[J] = static_cast<float >(pdLpc[J]); }

		// We need the error value to know how big an impulse to hit the
		//	filter with later.

		return static_cast<float>(dError);
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfCoeff Undocumented parameter from the Vorbis library.
	 * \param _pfPrime Undocumented parameter from the Vorbis library.
	 * \param _i32M Undocumented parameter from the Vorbis library.
	 * \param _pfData Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 */
	void CVorbisLpc::vorbis_lpc_predict( float * _pfCoeff, float * _pfPrime, int32_t _i32M, 
		float * _pfData, int32_t _i32N ) {

		// In: _pfCoeff[0..._i32M-1] LPC coefficients.
		//	_pfPrime[0..._i32M-1] initial values (allocated size of _i32N + _i32M - 1).
		// Out: data[0..._i32N-1] data samples.

		int32_t I, J, i32O, i32P;
		float fY;
		float * pfWork = static_cast<float *>(::alloca( sizeof( (*pfWork) ) * (_i32M + _i32N) ));

		if ( !_pfPrime ) {
			for ( I = 0; I < _i32M; I++ ) {
				pfWork[I] = 0.0f;
			}
		}
		else {
			for ( I = 0; I < _i32M; I++ ) {
				pfWork[I] = _pfPrime[I];
			}
		}

		for ( I = 0; I < _i32N; I++ ) {
			fY = 0;
			i32O = I;
			i32P = _i32M;
			for ( J = 0; J < _i32M; J++ ) {
				fY -= pfWork[i32O++] * _pfCoeff[--i32P];
			}

			_pfData[I] = pfWork[i32O] = fY;
		}
	}

}	// namespace lss
