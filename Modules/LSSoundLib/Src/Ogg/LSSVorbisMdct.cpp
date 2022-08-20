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

 function: modified discrete cosine transform prototypes
 last mod: $Id: mdct.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisMdct.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookUp Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_init( mdct_lookup * _plLookUp, int32_t _i32N ) {
		int32_t * pi32BitRev = static_cast<int32_t *>(CMemLib::MAlloc( sizeof( (*pi32BitRev) ) * ( _i32N / 4 ) ));
		DATA_TYPE * pdtT = static_cast<DATA_TYPE *>(CMemLib::MAlloc( sizeof( (*pdtT) ) * ( _i32N + _i32N / 4 ) ));

		int32_t I;
		int32_t i32N2 = _i32N >> 1;
		int32_t i32LogN2 = _plLookUp->log2n = static_cast<int32_t>(rint( ::log( static_cast<float>(_i32N ) ) / ::log( 2.0f ) ));
		_plLookUp->n = _i32N;
		_plLookUp->trig = pdtT;
		_plLookUp->bitrev = pi32BitRev;

		// Trig look-ups.
		for ( I = 0; I < _i32N / 4; I++ ) {
			pdtT[I*2] = FLOAT_CONV( ::cos( (static_cast<float>(LSM_PI_DOUBLE) / _i32N) * (4 * I) ) );
			pdtT[I*2+1] = FLOAT_CONV( -::sin( (static_cast<float>(LSM_PI_DOUBLE) / _i32N) * (4 * I) ) );
			pdtT[i32N2+I*2] = FLOAT_CONV( ::cos( (static_cast<float>(LSM_PI_DOUBLE) / (2 * _i32N)) * (2 * I + 1) ) );
			pdtT[i32N2+I*2+1] = FLOAT_CONV( ::sin( (static_cast<float>(LSM_PI_DOUBLE) / (2 * _i32N)) * (2 * I + 1) ) );
		}
		for ( I = 0; I < _i32N / 8; I++ ) {
			pdtT[_i32N+I*2] = FLOAT_CONV( ::cos( (static_cast<float>(LSM_PI_DOUBLE) / _i32N) * (4 * I + 2) ) * 0.5 );
			pdtT[_i32N+I*2+1] = FLOAT_CONV( -::sin( (static_cast<float>(LSM_PI_DOUBLE) / _i32N) * (4 * I + 2) ) * 0.5 );
		}

		// Bit-reverse look-up.
		{
			int32_t i32Mask = (1 << (i32LogN2 - 1)) - 1, J;
			int32_t i32Msb = 1 << (i32LogN2 - 2);
			for ( I = 0; I < _i32N / 8; I++ ) {
				int32_t i32Acc = 0;
				for ( J = 0; i32Msb >> J; J++ ) {
					if ( (i32Msb >> J) & I ) { i32Acc|= 1 << J; }
				}
				pi32BitRev[I*2] = ((~i32Acc) & i32Mask) - 1;
				pi32BitRev[I*2+1] = i32Acc;

			}
		}
		_plLookUp->scale = FLOAT_CONV( 4.0f / _i32N );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookUp Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_clear( mdct_lookup * _plLookUp ) {
		if ( _plLookUp ) {
			if ( _plLookUp->trig ) { CMemLib::Free( _plLookUp->trig ); }
			if ( _plLookUp->bitrev ) { CMemLib::Free( _plLookUp->bitrev ); }
			CStd::MemSet( _plLookUp, 0, sizeof( *_plLookUp ) );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plInit Undocumented parameter from the Vorbis library.
	 * \param _pdtIn Undocumented parameter from the Vorbis library.
	 * \param _pdtOut Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_forward( mdct_lookup * _plInit, DATA_TYPE * _pdtIn, DATA_TYPE * _pdtOut ) {
		int32_t _i32N = _plInit->n;
		int32_t i32N2 = _i32N >> 1;
		int32_t i32N4 = _i32N >> 2;
		int32_t i32N8 = _i32N >> 3;
		DATA_TYPE * pdtW = static_cast<DATA_TYPE *>(::alloca( _i32N * sizeof( (*pdtW) ) ) ); // Forward needs working space.
		DATA_TYPE * pdtW2 = pdtW + i32N2;

		// Rotate.
		// Window + rotate + step 1.
		REG_TYPE rtR0;
		REG_TYPE rtR1;
		DATA_TYPE * pdtX0 = _pdtIn + i32N2 + i32N4;
		DATA_TYPE * pdtX1 = pdtX0 + 1;
		DATA_TYPE *pdtT = _plInit->trig + i32N2;

		int32_t I = 0;

		for ( I = 0; I < i32N8; I += 2 ) {
			pdtX0 -= 4;
			pdtT -= 2;
			rtR0 = pdtX0[2] + pdtX1[0];
			rtR1 = pdtX0[0] + pdtX1[2];
			pdtW2[I] = MULT_NORM( rtR1 * pdtT[1] + rtR0 * pdtT[0] );
			pdtW2[I+1] = MULT_NORM( rtR1 * pdtT[0] - rtR0 * pdtT[1] );
			pdtX1 += 4;
		}

		pdtX1 = _pdtIn + 1;

		for ( ; I < i32N2 - i32N8; I += 2 ) {
			pdtT -= 2;
			pdtX0 -= 4;
			rtR0 = pdtX0[2] - pdtX1[0];
			rtR1 = pdtX0[0] - pdtX1[2];
			pdtW2[I] = MULT_NORM( rtR1 * pdtT[1] + rtR0 * pdtT[0] );
			pdtW2[I+1] = MULT_NORM( rtR1 * pdtT[0] - rtR0 * pdtT[1] );
			pdtX1 += 4;
		}

		pdtX0 = _pdtIn + _i32N;

		for ( ; I < i32N2; I += 2 ) {
			pdtT -= 2;
			pdtX0 -= 4;
			rtR0 = -pdtX0[2] - pdtX1[0];
			rtR1 = -pdtX0[0] - pdtX1[2];
			pdtW2[I] = MULT_NORM( rtR1 * pdtT[1] + rtR0 * pdtT[0] );
			pdtW2[I+1] = MULT_NORM( rtR1 * pdtT[0] - rtR0 * pdtT[1] );
			pdtX1 += 4;
		}


		mdct_butterflies( _plInit, pdtW + i32N2, i32N2 );
		mdct_bitreverse( _plInit, pdtW );

		// Rotate + window.

		pdtT = _plInit->trig + i32N2;
		pdtX0 = _pdtOut + i32N2;

		for ( I = 0; I < i32N4; I++ ) {
			pdtX0--;
			_pdtOut[I] = MULT_NORM( (pdtW[0] * pdtT[0] + pdtW[1] * pdtT[1]) * _plInit->scale );
			pdtX0[0] = MULT_NORM( (pdtW[0] * pdtT[1] - pdtW[1] * pdtT[0]) * _plInit->scale );
			pdtW += 2;
			pdtT += 2;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plInit Undocumented parameter from the Vorbis library.
	 * \param _pdtIn Undocumented parameter from the Vorbis library.
	 * \param _pdtOut Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_backward( mdct_lookup * _plInit, DATA_TYPE * _pdtIn, DATA_TYPE * _pdtOut ) {
		int32_t i32N = _plInit->n;
		int32_t i32N2 = i32N >> 1;
		int32_t i32N4 = i32N >> 2;

		// Rotate.

		DATA_TYPE * pdtIx = _pdtIn + i32N2 - 7;
		DATA_TYPE * pdtOx = _pdtOut + i32N2 + i32N4;
		DATA_TYPE * pdtT = _plInit->trig + i32N4;

		do {
			pdtOx -= 4;
			pdtOx[0] = MULT_NORM( -pdtIx[2] * pdtT[3] - pdtIx[0] * pdtT[2] );
			pdtOx[1] = MULT_NORM( pdtIx[0] * pdtT[3] - pdtIx[2] * pdtT[2] );
			pdtOx[2] = MULT_NORM( -pdtIx[6] * pdtT[1] - pdtIx[4] * pdtT[0] );
			pdtOx[3] = MULT_NORM( pdtIx[4] * pdtT[1] - pdtIx[6] * pdtT[0] );
			pdtIx -= 8;
			pdtT += 4;
		} while ( pdtIx >= _pdtIn );

		pdtIx = _pdtIn + i32N2 - 8;
		pdtOx = _pdtOut + i32N2 + i32N4;
		pdtT = _plInit->trig + i32N4;

		do {
			pdtT -= 4;
			pdtOx[0] = 	MULT_NORM( pdtIx[4] * pdtT[3] + pdtIx[6] * pdtT[2] );
			pdtOx[1] = 	MULT_NORM( pdtIx[4] * pdtT[2] - pdtIx[6] * pdtT[3] );
			pdtOx[2] = 	MULT_NORM( pdtIx[0] * pdtT[1] + pdtIx[2] * pdtT[0] );
			pdtOx[3] = 	MULT_NORM( pdtIx[0] * pdtT[0] - pdtIx[2] * pdtT[1] );
			pdtIx -= 8;
			pdtOx += 4;
		} while ( pdtIx >= _pdtIn );

		mdct_butterflies( _plInit, _pdtOut + i32N2, i32N2 );
		mdct_bitreverse( _plInit, _pdtOut );

		// Rotate + window.
		{
			DATA_TYPE * pdtOx1 = _pdtOut + i32N2 + i32N4;
			DATA_TYPE * pdtOx2 = _pdtOut + i32N2 + i32N4;
			DATA_TYPE * pdtIx2 = _pdtOut;
			pdtT = _plInit->trig + i32N2;

			do {
				pdtOx1 -= 4;

				pdtOx1[3] = MULT_NORM( pdtIx2[0] * pdtT[1] - pdtIx2[1] * pdtT[0] );
				pdtOx2[0] = -MULT_NORM( pdtIx2[0] * pdtT[0] + pdtIx2[1] * pdtT[1] );

				pdtOx1[2] = MULT_NORM( pdtIx2[2] * pdtT[3] - pdtIx2[3] * pdtT[2] );
				pdtOx2[1] = -MULT_NORM( pdtIx2[2] * pdtT[2] + pdtIx2[3] * pdtT[3] );

				pdtOx1[1] = MULT_NORM( pdtIx2[4] * pdtT[5] - pdtIx2[5] * pdtT[4] );
				pdtOx2[2] = -MULT_NORM( pdtIx2[4] * pdtT[4] + pdtIx2[5] * pdtT[5] );

				pdtOx1[0] = MULT_NORM( pdtIx2[6] * pdtT[7] - pdtIx2[7] * pdtT[6] );
				pdtOx2[3] = -MULT_NORM( pdtIx2[6] * pdtT[6] + pdtIx2[7] * pdtT[7] );

				pdtOx2 += 4;
				pdtIx2 += 8;
				pdtT += 8;
			} while ( pdtIx2 < pdtOx1 );

			pdtIx2 = _pdtOut + i32N2 + i32N4;
			pdtOx1 = _pdtOut + i32N4;
			pdtOx2 = pdtOx1;

			do {
				pdtOx1 -= 4;
				pdtIx2 -= 4;

				pdtOx2[0] = -(pdtOx1[3] = pdtIx2[3]);
				pdtOx2[1] = -(pdtOx1[2] = pdtIx2[2]);
				pdtOx2[2] = -(pdtOx1[1] = pdtIx2[1]);
				pdtOx2[3] = -(pdtOx1[0] = pdtIx2[0]);

				pdtOx2 += 4;
			} while ( pdtOx2 < pdtIx2 );

			pdtIx2 = _pdtOut + i32N2 + i32N4;
			pdtOx1 = _pdtOut + i32N2 + i32N4;
			pdtOx2 = _pdtOut + i32N2;
			do {
				pdtOx1 -= 4;
				pdtOx1[0] = pdtIx2[3];
				pdtOx1[1] = pdtIx2[2];
				pdtOx1[2] = pdtIx2[1];
				pdtOx1[3] = pdtIx2[0];
				pdtIx2 += 4;
			} while ( pdtOx1 > pdtOx2 );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plInit Undocumented parameter from the Vorbis library.
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 * \param _i32Points Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterflies( mdct_lookup * _plInit, 
		DATA_TYPE * _pdtX, 
		int32_t _i32Points ) {

		DATA_TYPE * pdtT = _plInit->trig;
		int32_t i32Stages = _plInit->log2n-5;
		int32_t I, J;

		if ( --i32Stages > 0 ) {
			mdct_butterfly_first( pdtT, _pdtX, _i32Points );
		}

		for ( I = 1; --i32Stages > 0; I++ ) {
			for ( J = 0; J < (1 << I); J++ ) {
				mdct_butterfly_generic( pdtT, _pdtX + (_i32Points >> I) * J, _i32Points >> I, 4 << I );
			}
		}

		for ( J = 0; J < _i32Points; J += 32 ) {
			mdct_butterfly_32( _pdtX + J );
		}

	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plInit Undocumented parameter from the Vorbis library.
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_bitreverse( mdct_lookup * _plInit, 
		DATA_TYPE * _pdtX ) {
		int32_t _i32N = _plInit->n;
		int32_t * pi32Bit = _plInit->bitrev;
		DATA_TYPE * pdtW0 = _pdtX;
		DATA_TYPE * pdtW1 = _pdtX = pdtW0 + (_i32N >> 1);
		DATA_TYPE * pdtT = _plInit->trig + _i32N;

		do {
			DATA_TYPE * pdtX0 = _pdtX + pi32Bit[0];
			DATA_TYPE * pdtX1 = _pdtX + pi32Bit[1];

			REG_TYPE rtR0 = pdtX0[1] - pdtX1[1];
			REG_TYPE rtR1 = pdtX0[0] + pdtX1[0];
			REG_TYPE rtR2 = MULT_NORM( rtR1 * pdtT[0] + rtR0 * pdtT[1] );
			REG_TYPE rtR3 = MULT_NORM( rtR1 * pdtT[1] - rtR0 * pdtT[0] );

			pdtW1 -= 4;

			rtR0 = HALVE( pdtX0[1] + pdtX1[1] );
			rtR1 = HALVE( pdtX0[0] - pdtX1[0] );

			pdtW0[0] = rtR0 + rtR2;
			pdtW1[2] = rtR0 - rtR2;
			pdtW0[1] = rtR1 + rtR3;
			pdtW1[3] = rtR3 - rtR1;

			pdtX0 = _pdtX + pi32Bit[2];
			pdtX1 = _pdtX + pi32Bit[3];

			rtR0 = pdtX0[1] - pdtX1[1];
			rtR1 = pdtX0[0] + pdtX1[0];
			rtR2 = MULT_NORM( rtR1 * pdtT[2] + rtR0 * pdtT[3] );
			rtR3 = MULT_NORM( rtR1 * pdtT[3] - rtR0 * pdtT[2] );

			rtR0 = HALVE( pdtX0[1] + pdtX1[1] );
			rtR1 = HALVE( pdtX0[0] - pdtX1[0] );

			pdtW0[2] = rtR0 + rtR2;
			pdtW1[0] = rtR0 - rtR2;
			pdtW0[3] = rtR1 + rtR3;
			pdtW1[1] = rtR3 - rtR1;

			pdtT += 4;
			pi32Bit += 4;
			pdtW0 += 4;

		}
		while ( pdtW0 < pdtW1 );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pdtT Undocumented parameter from the Vorbis library.
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 * \param _i32Points Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterfly_first( DATA_TYPE * _pdtT, 
		DATA_TYPE * _pdtX, 
		int32_t _i32Points ) {

		DATA_TYPE * pdtX1 = _pdtX + _i32Points - 8;
		DATA_TYPE * pdtX2 = _pdtX + (_i32Points >> 1) - 8;
		REG_TYPE rtR0;
		REG_TYPE rtR1;

		do {

			rtR0 = pdtX1[6] - pdtX2[6];
			rtR1 = pdtX1[7] - pdtX2[7];
			pdtX1[6] += pdtX2[6];
			pdtX1[7] += pdtX2[7];
			pdtX2[6] = MULT_NORM( rtR1 * _pdtT[1] + rtR0 * _pdtT[0] );
			pdtX2[7] = MULT_NORM( rtR1 * _pdtT[0] - rtR0 * _pdtT[1] );

			rtR0 = pdtX1[4] - pdtX2[4];
			rtR1 = pdtX1[5] - pdtX2[5];
			pdtX1[4] += pdtX2[4];
			pdtX1[5] += pdtX2[5];
			pdtX2[4] = MULT_NORM( rtR1 * _pdtT[5] + rtR0 * _pdtT[4] );
			pdtX2[5] = MULT_NORM( rtR1 * _pdtT[4] - rtR0 * _pdtT[5] );

			rtR0 = pdtX1[2] - pdtX2[2];
			rtR1 = pdtX1[3] - pdtX2[3];
			pdtX1[2] += pdtX2[2];
			pdtX1[3] += pdtX2[3];
			pdtX2[2] = MULT_NORM( rtR1 * _pdtT[9] + rtR0 * _pdtT[8] );
			pdtX2[3] = MULT_NORM( rtR1 * _pdtT[8] - rtR0 * _pdtT[9] );

			rtR0 = pdtX1[0] - pdtX2[0];
			rtR1 = pdtX1[1] - pdtX2[1];
			pdtX1[0] += pdtX2[0];
			pdtX1[1] += pdtX2[1];
			pdtX2[0] = MULT_NORM( rtR1 * _pdtT[13] + rtR0 * _pdtT[12] );
			pdtX2[1] = MULT_NORM( rtR1 * _pdtT[12] - rtR0 * _pdtT[13] );

			pdtX1 -= 8;
			pdtX2 -= 8;
			_pdtT += 16;

		}
		while ( pdtX2 >= _pdtX );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pdtT Undocumented parameter from the Vorbis library.
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 * \param _i32Points Undocumented parameter from the Vorbis library.
	 * \param _i32TrigInt Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterfly_generic( DATA_TYPE *_pdtT, 
		DATA_TYPE *_pdtX, 
		int32_t _i32Points, 
		int32_t _i32TrigInt ) {

		DATA_TYPE * pdtX1 = _pdtX + _i32Points - 8;
		DATA_TYPE * pdtX2 = _pdtX + (_i32Points >> 1) - 8;
		REG_TYPE rtR0;
		REG_TYPE rtR1;

		do {

			rtR0 = pdtX1[6] - pdtX2[6];
			rtR1 = pdtX1[7] - pdtX2[7];
			pdtX1[6] += pdtX2[6];
			pdtX1[7] += pdtX2[7];
			pdtX2[6] = MULT_NORM( rtR1 * _pdtT[1] + rtR0 * _pdtT[0] );
			pdtX2[7] = MULT_NORM( rtR1 * _pdtT[0] - rtR0 * _pdtT[1] );

			_pdtT += _i32TrigInt;

			rtR0 = pdtX1[4] - pdtX2[4];
			rtR1 = pdtX1[5] - pdtX2[5];
			pdtX1[4] += pdtX2[4];
			pdtX1[5] += pdtX2[5];
			pdtX2[4] = MULT_NORM( rtR1 * _pdtT[1] + rtR0 * _pdtT[0] );
			pdtX2[5] = MULT_NORM( rtR1 * _pdtT[0] - rtR0 * _pdtT[1] );

			_pdtT += _i32TrigInt;

			rtR0 = pdtX1[2] - pdtX2[2];
			rtR1 = pdtX1[3] - pdtX2[3];
			pdtX1[2] += pdtX2[2];
			pdtX1[3] += pdtX2[3];
			pdtX2[2] = MULT_NORM( rtR1 * _pdtT[1] + rtR0 * _pdtT[0] );
			pdtX2[3] = MULT_NORM( rtR1 * _pdtT[0] - rtR0 * _pdtT[1] );

			_pdtT += _i32TrigInt;

			rtR0 = pdtX1[0] - pdtX2[0];
			rtR1 = pdtX1[1] - pdtX2[1];
			pdtX1[0] += pdtX2[0];
			pdtX1[1] += pdtX2[1];
			pdtX2[0] = MULT_NORM( rtR1 * _pdtT[1] + rtR0 * _pdtT[0] );
			pdtX2[1] = MULT_NORM( rtR1 * _pdtT[0] - rtR0 * _pdtT[1] );

			_pdtT += _i32TrigInt;
			pdtX1 -= 8;
			pdtX2 -= 8;

		}
		while ( pdtX2 >= _pdtX );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterfly_32( DATA_TYPE * _pdtX ) {
		REG_TYPE rtR0 = _pdtX[30] - _pdtX[14];
		REG_TYPE rtR1 = _pdtX[31] - _pdtX[15];

		_pdtX[30] += _pdtX[14];
		_pdtX[31] += _pdtX[15];
		_pdtX[14] = rtR0;
		_pdtX[15] = rtR1;

		rtR0 = _pdtX[28] - _pdtX[12];
		rtR1 = _pdtX[29] - _pdtX[13];
		_pdtX[28] += _pdtX[12];
		_pdtX[29] += _pdtX[13];
		_pdtX[12] = MULT_NORM( rtR0 * cPI1_8 - rtR1 * cPI3_8 );
		_pdtX[13] = MULT_NORM( rtR0 * cPI3_8 + rtR1 * cPI1_8 );

		rtR0 = _pdtX[26] - _pdtX[10];
		rtR1 = _pdtX[27] - _pdtX[11];
		_pdtX[26] += _pdtX[10];
		_pdtX[27] += _pdtX[11];
		_pdtX[10] = MULT_NORM( (rtR0 - rtR1) * cPI2_8 );
		_pdtX[11] = MULT_NORM( (rtR0 + rtR1) * cPI2_8 );

		rtR0 = _pdtX[24] - _pdtX[8];
		rtR1 = _pdtX[25] - _pdtX[9];
		_pdtX[24] += _pdtX[8];
		_pdtX[25] += _pdtX[9];
		_pdtX[8] = MULT_NORM( rtR0 * cPI3_8 - rtR1 * cPI1_8 );
		_pdtX[9] = MULT_NORM( rtR1 * cPI3_8 + rtR0 * cPI1_8 );

		rtR0 = _pdtX[22] - _pdtX[6];
		rtR1 = _pdtX[7] - _pdtX[23];
		_pdtX[22] += _pdtX[6];
		_pdtX[23] += _pdtX[7];
		_pdtX[6] = rtR1;
		_pdtX[7] = rtR0;

		rtR0 = _pdtX[4] - _pdtX[20];
		rtR1 = _pdtX[5] - _pdtX[21];
		_pdtX[20] += _pdtX[4];
		_pdtX[21] += _pdtX[5];
		_pdtX[4] = MULT_NORM( rtR1 * cPI1_8 + rtR0 * cPI3_8 );
		_pdtX[5] = MULT_NORM( rtR1 * cPI3_8 - rtR0 * cPI1_8 );

		rtR0 = _pdtX[2] - _pdtX[18];
		rtR1 = _pdtX[3] - _pdtX[19];
		_pdtX[18] += _pdtX[2];
		_pdtX[19] += _pdtX[3];
		_pdtX[2] = MULT_NORM( (rtR1 + rtR0) * cPI2_8 );
		_pdtX[3] = MULT_NORM( (rtR1 - rtR0) * cPI2_8 );

		rtR0 = _pdtX[0] - _pdtX[16];
		rtR1 = _pdtX[1] - _pdtX[17];
		_pdtX[16] += _pdtX[0];
		_pdtX[17] += _pdtX[1];
		_pdtX[0] = MULT_NORM( rtR1 * cPI3_8 + rtR0 * cPI1_8 );
		_pdtX[1] = MULT_NORM( rtR1 * cPI1_8 - rtR0 * cPI3_8 );

		mdct_butterfly_16( _pdtX );
		mdct_butterfly_16( _pdtX + 16 );

	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterfly_16( DATA_TYPE * _pdtX ) {
		REG_TYPE rtR0 = _pdtX[1] - _pdtX[9];
		REG_TYPE rtR1 = _pdtX[0] - _pdtX[8];

		_pdtX[8] += _pdtX[0];
		_pdtX[9] += _pdtX[1];
		_pdtX[0] = MULT_NORM( (rtR0 + rtR1) * cPI2_8 );
		_pdtX[1] = MULT_NORM( (rtR0 - rtR1) * cPI2_8 );

		rtR0 = _pdtX[3] - _pdtX[11];
		rtR1 = _pdtX[10] - _pdtX[2];
		_pdtX[10] += _pdtX[2];
		_pdtX[11] += _pdtX[3];
		_pdtX[2] = rtR0;
		_pdtX[3] = rtR1;

		rtR0 = _pdtX[12] - _pdtX[4];
		rtR1 = _pdtX[13] - _pdtX[5];
		_pdtX[12] += _pdtX[4];
		_pdtX[13] += _pdtX[5];
		_pdtX[4] = MULT_NORM( (rtR0 - rtR1) * cPI2_8 );
		_pdtX[5] = MULT_NORM( (rtR0 + rtR1) * cPI2_8 );

		rtR0 = _pdtX[14] - _pdtX[6];
		rtR1 = _pdtX[15] - _pdtX[7];
		_pdtX[14] += _pdtX[6];
		_pdtX[15] += _pdtX[7];
		_pdtX[6]  = rtR0;
		_pdtX[7]  = rtR1;

		mdct_butterfly_8( _pdtX );
		mdct_butterfly_8( _pdtX + 8 );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pdtX Undocumented parameter from the Vorbis library.
	 */
	void CVorbisMdct::mdct_butterfly_8( DATA_TYPE * _pdtX ) {
		REG_TYPE rtR0 = _pdtX[6] + _pdtX[2];
		REG_TYPE rtR1 = _pdtX[6] - _pdtX[2];
		REG_TYPE rtR2 = _pdtX[4] + _pdtX[0];
		REG_TYPE rtR3 = _pdtX[4] - _pdtX[0];

		_pdtX[6] = rtR0 + rtR2;
		_pdtX[4] = rtR0 - rtR2;

		rtR0 = _pdtX[5] - _pdtX[1];
		rtR2 = _pdtX[7] - _pdtX[3];
		_pdtX[0] = rtR1 + rtR0;
		_pdtX[2] = rtR1 - rtR0;

		rtR0 = _pdtX[5] + _pdtX[1];
		rtR1 = _pdtX[7] + _pdtX[3];
		_pdtX[3] = rtR2 + rtR3;
		_pdtX[1] = rtR2 - rtR3;
		_pdtX[7] = rtR1 + rtR0;
		_pdtX[5] = rtR1 - rtR0;

	}

}	// namespace lss
