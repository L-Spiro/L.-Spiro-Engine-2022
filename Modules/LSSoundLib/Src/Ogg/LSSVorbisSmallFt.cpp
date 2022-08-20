/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: fft transform
 last mod: $Id: smallft.h 13293 2007-07-24 00:09:47Z xiphmont $

 ********************************************************************/

#include "LSSVorbisSmallFt.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookup Undocumented parameter from the Vorbis library.
	 * \param _pfData Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drft_forward( drft_lookup * _plLookup, float * _pfData ) {
		if ( _plLookup->n == 1 ) { return; }
		drftf1( _plLookup->n, _pfData, _plLookup->trigcache, _plLookup->trigcache + _plLookup->n, _plLookup->splitcache );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookup Undocumented parameter from the Vorbis library.
	 * \param _pfData Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drft_backward( drft_lookup * _plLookup, float * _pfData ) {
		if ( _plLookup->n == 1 ) { return; }
		drftb1( _plLookup->n, _pfData, _plLookup->trigcache, _plLookup->trigcache + _plLookup->n, _plLookup->splitcache );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookup Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drft_init( drft_lookup * _plLookup, int32_t _i32N ) {
		_plLookup->n = _i32N;
		_plLookup->trigcache = static_cast<float *>(CMemLib::CMAlloc( 3 * _i32N * sizeof( (*_plLookup->trigcache) ) ));
		_plLookup->splitcache = static_cast<int32_t *>(CMemLib::CMAlloc( 32 * sizeof( (*_plLookup->splitcache) ) ));
		fdrffti( _i32N, _plLookup->trigcache, _plLookup->splitcache );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plLookup Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drft_clear( drft_lookup * _plLookup ) {
		if ( _plLookup ) {
			if ( _plLookup->trigcache ) { CMemLib::Free( _plLookup->trigcache ); }
			if ( _plLookup->splitcache ) { CMemLib::Free( _plLookup->splitcache ); }
			CStd::MemSet( _plLookup, 0, sizeof( (*_plLookup) ) );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pfC Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa Undocumented parameter from the Vorbis library.
	 * \param _pi32Fac Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drftf1( int32_t _i32N, float * _pfC, float * _pfCh, float * _pfWa, int32_t * _pi32Fac ) {
		int32_t I, i32K1, i32L1, i32L2;
		int32_t i32Na, i32Kh, i32Nf;
		int32_t i32Ip, i32Iw, i32Ido, i32IdL1, i32X2, i32X3;

		i32Nf = _pi32Fac[1];
		i32Na = 1;
		i32L2 = _i32N;
		i32Iw = _i32N;

		for ( i32K1 = 0; i32K1 < i32Nf; i32K1++ ) {
			i32Kh = i32Nf - i32K1;
			i32Ip = _pi32Fac[i32Kh+1];
			i32L1 = i32L2 / i32Ip;
			i32Ido = _i32N / i32L2;
			i32IdL1 = i32Ido * i32L1;
			i32Iw -= (i32Ip - 1) * i32Ido;
			i32Na = 1 - i32Na;

			if ( i32Ip != 4 ) { goto L102; }

			i32X2 = i32Iw + i32Ido;
			i32X3 = i32X2 + i32Ido;
			if ( i32Na != 0 ) {
				dradf4( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1 );
			}
			else {
				dradf4( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1 );
			}
			goto L110;

	L102 :
			if ( i32Ip != 2 ) { goto L104; }
			if ( i32Na != 0 ) { goto L103; }

			dradf2( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1 );
			goto L110;

	L103 :
			dradf2( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1 );
			goto L110;

	L104 :
			if ( i32Ido == 1 ) { i32Na = 1 - i32Na; }
			if ( i32Na != 0 ) { goto L109; }

			dradfg( i32Ido, i32Ip, i32L1, i32IdL1, _pfC, _pfC, _pfC, _pfCh, _pfCh, _pfWa + i32Iw - 1 );
			i32Na = 1;
			goto L110;

	L109 :
			dradfg( i32Ido, i32Ip, i32L1, i32IdL1, _pfCh, _pfCh, _pfCh, _pfC, _pfC, _pfWa + i32Iw - 1 );
			i32Na = 0;

	L110 :
			i32L2 = i32L1;
		}

		if ( i32Na == 1 ) { return; }

		for ( I = 0; I < _i32N; I++ ) { _pfC[I] = _pfCh[I]; }
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pfC Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa Undocumented parameter from the Vorbis library.
	 * \param _pi32Fac Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drftb1( int32_t _i32N, float * _pfC, float * _pfCh, float * _pfWa, int32_t * _pi32Fac ) {
		int32_t I, i32K1, i32L1, i32L2;
		int32_t i32Na;
		int32_t i32Nf, i32Ip, i32Iw, i32X2, i32X3, i32Ido, i32IdL1;

		i32Nf = _pi32Fac[1];
		i32Na = 0;
		i32L1 = 1;
		i32Iw = 1;

		for ( i32K1 = 0; i32K1 < i32Nf; i32K1++ ) {
			i32Ip = _pi32Fac[i32K1+2];
			i32L2 = i32Ip * i32L1;
			i32Ido = _i32N / i32L2;
			i32IdL1 = i32Ido * i32L1;
			if ( i32Ip != 4 ) { goto L103; }
			i32X2 = i32Iw + i32Ido;
			i32X3 = i32X2 + i32Ido;

			if ( i32Na != 0 ) {
				dradb4( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1 );
			}
			else {
				dradb4( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1 );
			}
			i32Na = 1 - i32Na;
			goto L115;

	L103 :
			if ( i32Ip != 2 ) { goto L106; }

			if ( i32Na != 0 ) {
				dradb2( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1 );
			}
			else {
				dradb2( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1 );
			}
			i32Na = 1 - i32Na;
			goto L115;

	L106 :
			if ( i32Ip != 3 ) { goto L109; }

			i32X2 = i32Iw + i32Ido;
			if ( i32Na != 0 ) {
				dradb3( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1 );
			}
			else {
				dradb3( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1 );
			}
			i32Na = 1 - i32Na;
			goto L115;

	L109 :
	// The radix five case can be translated later.
	/*		if ( i32Ip != 5 ) { goto L112; }

			i32X2 = i32Iw + i32Ido;
			i32X3 = i32X2 + i32Ido;
			ix4 = i32X3 + i32Ido;
			if ( i32Na != 0 ) {
				dradb5( i32Ido, i32L1, _pfCh, _pfC, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1, _pfWa + ix4 - 1 );
			}
			else {
				dradb5( i32Ido, i32L1, _pfC, _pfCh, _pfWa + i32Iw - 1, _pfWa + i32X2 - 1, _pfWa + i32X3 - 1, _pfWa + ix4 - 1 );
			}
			i32Na = 1 - i32Na;
			goto L115;

	L112 :*/
			if ( i32Na != 0 ) {
				dradbg( i32Ido, i32Ip, i32L1, i32IdL1, _pfCh, _pfCh, _pfCh, _pfC, _pfC, _pfWa + i32Iw - 1 );
			}
			else {
				dradbg( i32Ido, i32Ip, i32L1, i32IdL1, _pfC, _pfC, _pfC, _pfCh, _pfCh, _pfWa + i32Iw - 1 );
			}
			if ( i32Ido == 1 ) { i32Na = 1 - i32Na; }

	L115 :
			i32L1 = i32L2;
			i32Iw += (i32Ip - 1) * i32Ido;
		}

		if ( i32Na == 0 ) { return; }

		for ( I = 0; I < _i32N; I++ ) { _pfC[I] = _pfCh[I]; }
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pfWSave Undocumented parameter from the Vorbis library.
	 * \param _pi32Fac Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::fdrffti( int32_t _i32N, float * _pfWSave, int32_t * _pi32Fac ) {
		if ( _i32N == 1 ) { return; }
		drfti1( _i32N, _pfWSave + _i32N, _pi32Fac );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param i32L1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa1 Undocumented parameter from the Vorbis library.
	 * \param _pfWa2 Undocumented parameter from the Vorbis library.
	 * \param _pfWa3 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradf4( int32_t _i32Ido, int32_t i32L1, float * _pfCc, float * _pfCh, float * _pfWa1, 
		float * _pfWa2, float * _pfWa3 ) {
		static const float fSqt2Half = 0.70710678118654752f;
		int32_t I, K, i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6;
		float fCi2, fCi3, fCi4, fCr2, fCr3, fCr4, fTi1, fTi2, fTi3, fTi4, fTr1, fTr2, fTr3, fTr4;
		i32T0 = i32L1 * _i32Ido;

		i32T1 = i32T0;
		i32T4 = i32T1 << 1;
		i32T2 = i32T1 + (i32T1 << 1);
		i32T3 = 0;

		for ( K = 0; K < i32L1; K++ ) {
			fTr1 = _pfCc[i32T1] + _pfCc[i32T2];
			fTr2 = _pfCc[i32T3] + _pfCc[i32T4];

			_pfCh[i32T5=i32T3<<2] = fTr1 + fTr2;
			_pfCh[(_i32Ido<<2)+i32T5-1] = fTr2 - fTr1;
			_pfCh[(i32T5+=(_i32Ido<<1))-1] = _pfCc[i32T3] - _pfCc[i32T4];
			_pfCh[i32T5] = _pfCc[i32T2] - _pfCc[i32T1];

			i32T1 += _i32Ido;
			i32T2 += _i32Ido;
			i32T3 += _i32Ido;
			i32T4 += _i32Ido;
		}

		if ( _i32Ido < 2 ) { return; }
		if ( _i32Ido == 2 ) { goto L105; }


		i32T1 = 0;
		for ( K = 0; K < i32L1; K++ ) {
			i32T2 = i32T1;
			i32T4 = i32T1 << 2;
			i32T5 = (i32T6 = _i32Ido << 1) + i32T4;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T3 = (i32T2 += 2);
				i32T4 += 2;
				i32T5 -= 2;

				i32T3 += i32T0;
				fCr2 = _pfWa1[I-2] * _pfCc[i32T3-1] + _pfWa1[I-1] * _pfCc[i32T3];
				fCi2 = _pfWa1[I-2] * _pfCc[i32T3] - _pfWa1[I-1] * _pfCc[i32T3-1];
				i32T3 += i32T0;
				fCr3 = _pfWa2[I-2] * _pfCc[i32T3-1] + _pfWa2[I-1] * _pfCc[i32T3];
				fCi3 = _pfWa2[I-2] * _pfCc[i32T3] - _pfWa2[I-1] * _pfCc[i32T3-1];
				i32T3 += i32T0;
				fCr4 = _pfWa3[I-2] * _pfCc[i32T3-1] + _pfWa3[I-1] * _pfCc[i32T3];
				fCi4 = _pfWa3[I-2] * _pfCc[i32T3] - _pfWa3[I-1] * _pfCc[i32T3-1];

				fTr1 = fCr2 + fCr4;
				fTr4 = fCr4 - fCr2;
				fTi1 = fCi2 + fCi4;
				fTi4 = fCi2 - fCi4;

				fTi2 = _pfCc[i32T2] + fCi3;
				fTi3 = _pfCc[i32T2] - fCi3;
				fTr2 = _pfCc[i32T2-1] + fCr3;
				fTr3 = _pfCc[i32T2-1] - fCr3;

				_pfCh[i32T4-1] = fTr1 + fTr2;
				_pfCh[i32T4] = fTi1 + fTi2;

				_pfCh[i32T5-1] = fTr3 - fTi4;
				_pfCh[i32T5] = fTr4 - fTi3;

				_pfCh[i32T4+i32T6-1] = fTi4 + fTr3;
				_pfCh[i32T4+i32T6] = fTr4 + fTi3;

				_pfCh[i32T5+i32T6-1] = fTr2 - fTr1;
				_pfCh[i32T5+i32T6] = fTi1 - fTi2;
			}
			i32T1 += _i32Ido;
		}
		if ( _i32Ido & 1 ) { return; }

	L105 :

		i32T2 = (i32T1 = i32T0 + _i32Ido - 1) + (i32T0 << 1);
		i32T3 = _i32Ido << 2;
		i32T4 = _i32Ido;
		i32T5 = _i32Ido << 1;
		i32T6 = _i32Ido;

		for ( K = 0; K < i32L1; K++ ) {
			fTi1 = -fSqt2Half * (_pfCc[i32T1] + _pfCc[i32T2]);
			fTr1 = fSqt2Half * (_pfCc[i32T1] - _pfCc[i32T2]);

			_pfCh[i32T4-1] = fTr1 + _pfCc[i32T6-1];
			_pfCh[i32T4+i32T5-1] = _pfCc[i32T6-1] - fTr1;

			_pfCh[i32T4] = fTi1 - _pfCc[i32T1+i32T0];
			_pfCh[i32T4+i32T5] = fTi1 + _pfCc[i32T1+i32T0];

			i32T1 += _i32Ido;
			i32T2 += _i32Ido;
			i32T4 += i32T3;
			i32T6 += _i32Ido;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa1 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradf2( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1 ) {
		int32_t I, K;
		float fTi2, fTr2;
		int32_t i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6;

		i32T1 = 0;
		i32T0 = (i32T2 = _i32L1 * _i32Ido);
		i32T3 = _i32Ido << 1;
		for ( K = 0; K < _i32L1; K++ ) {
			_pfCh[i32T1<<1] = _pfCc[i32T1] + _pfCc[i32T2];
			_pfCh[(i32T1<<1)+i32T3-1] = _pfCc[i32T1] - _pfCc[i32T2];
			i32T1 += _i32Ido;
			i32T2 += _i32Ido;
		}

		if ( _i32Ido < 2 ) { return; }
		if ( _i32Ido == 2 ) { goto L105; }

		i32T1 = 0;
		i32T2 = i32T0;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T3 = i32T2;
			i32T4 = (i32T1 << 1) + (_i32Ido << 1);
			i32T5 = i32T1;
			i32T6 = i32T1 + i32T1;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T3 += 2;
				i32T4 -= 2;
				i32T5 += 2;
				i32T6 += 2;
				fTr2 = _pfWa1[I-2] * _pfCc[i32T3-1] + _pfWa1[I-1] * _pfCc[i32T3];
				fTi2 = _pfWa1[I-2] * _pfCc[i32T3] - _pfWa1[I-1] * _pfCc[i32T3-1];
				_pfCh[i32T6] = _pfCc[i32T5] + fTi2;
				_pfCh[i32T4] = fTi2 - _pfCc[i32T5];
				_pfCh[i32T6-1] = _pfCc[i32T5-1] + fTr2;
				_pfCh[i32T4-1] = _pfCc[i32T5-1] - fTr2;
			}
			i32T1 += _i32Ido;
			i32T2 += _i32Ido;
		}

		if ( _i32Ido % 2 == 1 ) { return; }

	 L105 :
		i32T3 = (i32T2 = (i32T1 = _i32Ido) - 1);
		i32T2 += i32T0;
		for ( K = 0; K < _i32L1; K++ ) {
			_pfCh[i32T1] =  - _pfCc[i32T2];
			_pfCh[i32T1-1] = _pfCc[i32T3];
			i32T1 += _i32Ido << 1;
			i32T2 += _i32Ido;
			i32T3 += _i32Ido;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32Ip Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param _i32IdL1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfC1 Undocumented parameter from the Vorbis library.
	 * \param _pfC2 Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfCh2 Undocumented parameter from the Vorbis library.
	 * \param _pfWa Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradfg( int32_t _i32Ido, int32_t _i32Ip, int32_t _i32L1, int32_t _i32IdL1, float * _pfCc, float * _pfC1, 
		float * _pfC2, float * _pfCh, float * _pfCh2, float * _pfWa ) {

		static const float fTwoPi = 6.283185307179586f;
		int32_t i32IdIj, i32IpPh, I, J, K, i32L, i32Ic, i32Ik, i32Is;
		int32_t i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6, i32T7, i32T8, i32T9, i32T10;
		float fDc2, fAi1, fAi2, fAr1, fAr2, fDs2;
		int32_t i32Nbd;
		float fDcp, fArg, fDsp, fAr1h, fAr2h;
		int32_t i32Idp2, i32Ipp2;

		fArg = fTwoPi / static_cast<float>(_i32Ip);
		fDcp = ::cosf( fArg );
		fDsp = ::sinf( fArg );
		i32IpPh = (_i32Ip + 1) >> 1;
		i32Ipp2 = _i32Ip;
		i32Idp2 = _i32Ido;
		i32Nbd = (_i32Ido - 1) >> 1;
		i32T0 = _i32L1 * _i32Ido;
		i32T10 = _i32Ip * _i32Ido;

		if ( _i32Ido == 1 ) { goto L119; }
		for ( i32Ik = 0; i32Ik < _i32IdL1; i32Ik++ ) { _pfCh2[i32Ik] = _pfC2[i32Ik]; }

		i32T1 = 0;
		for ( J = 1; J < _i32Ip; J++ ) {
			i32T1 += i32T0;
			i32T2 = i32T1;
			for ( K = 0; K < _i32L1; K++ ) {
				_pfCh[i32T2] = _pfC1[i32T2];
				i32T2 += _i32Ido;
			}
		}

		i32Is = -_i32Ido;
		i32T1 = 0;
		if ( i32Nbd > _i32L1 ) {
			for ( J = 1; J < _i32Ip; J++ ) {
				i32T1 += i32T0;
				i32Is += _i32Ido;
				i32T2 = -_i32Ido + i32T1;
				for ( K = 0; K < _i32L1; K++ ) {
					i32IdIj = i32Is - 1;
					i32T2 += _i32Ido;
					i32T3 = i32T2;
					for ( I = 2; I < _i32Ido; I += 2 ) {
						i32IdIj += 2;
						i32T3 += 2;
						_pfCh[i32T3-1] = _pfWa[i32IdIj-1] * _pfC1[i32T3-1] + _pfWa[i32IdIj] * _pfC1[i32T3];
						_pfCh[i32T3] = _pfWa[i32IdIj-1] * _pfC1[i32T3] - _pfWa[i32IdIj] * _pfC1[i32T3-1];
					}
				}
			}
		}
		else {

			for ( J = 1; J < _i32Ip; J++ ) {
				i32Is += _i32Ido;
				i32IdIj = i32Is-1;
				i32T1 += i32T0;
				i32T2 = i32T1;
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32IdIj += 2;
					i32T2 += 2;
					i32T3 = i32T2;
					for ( K = 0; K < _i32L1; K++ ) {
						_pfCh[i32T3-1] = _pfWa[i32IdIj-1] * _pfC1[i32T3-1] + _pfWa[i32IdIj] * _pfC1[i32T3];
						_pfCh[i32T3] = _pfWa[i32IdIj-1] * _pfC1[i32T3] - _pfWa[i32IdIj] * _pfC1[i32T3-1];
						i32T3 += _i32Ido;
					}
				}
			}
		}

		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		if ( i32Nbd < _i32L1 ) {
			for ( J = 1; J < i32IpPh; J++ ) {
				i32T1 += i32T0;
				i32T2 -= i32T0;
				i32T3 = i32T1;
				i32T4 = i32T2;
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32T3 += 2;
					i32T4 += 2;
					i32T5 = i32T3-_i32Ido;
					i32T6 = i32T4-_i32Ido;
					for ( K = 0; K < _i32L1; K++ ) {
						i32T5 += _i32Ido;
						i32T6 += _i32Ido;
						_pfC1[i32T5-1] = _pfCh[i32T5-1] + _pfCh[i32T6-1];
						_pfC1[i32T6-1] = _pfCh[i32T5] - _pfCh[i32T6];
						_pfC1[i32T5] = _pfCh[i32T5] + _pfCh[i32T6];
						_pfC1[i32T6] = _pfCh[i32T6-1] - _pfCh[i32T5-1];
					}
				}
			}
		}
		else {
			for ( J = 1; J < i32IpPh; J++ ) {
				i32T1 += i32T0;
				i32T2 -= i32T0;
				i32T3 = i32T1;
				i32T4 = i32T2;
				for ( K = 0; K < _i32L1; K++ ) {
					i32T5 = i32T3;
					i32T6 = i32T4;
					for ( I = 2; I < _i32Ido; I += 2 ) {
						i32T5 += 2;
						i32T6 += 2;
						_pfC1[i32T5-1] = _pfCh[i32T5-1] + _pfCh[i32T6-1];
						_pfC1[i32T6-1] = _pfCh[i32T5] - _pfCh[i32T6];
						_pfC1[i32T5] = _pfCh[i32T5] + _pfCh[i32T6];
						_pfC1[i32T6] = _pfCh[i32T6-1] - _pfCh[i32T5-1];
					}
					i32T3 += _i32Ido;
					i32T4 += _i32Ido;
				}
			}
		}

	L119 :
		for ( i32Ik = 0; i32Ik < _i32IdL1; i32Ik++ ) { _pfC2[i32Ik] = _pfCh2[i32Ik]; }

		i32T1 = 0;
		i32T2 = i32Ipp2 * _i32IdL1;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1 - _i32Ido;
			i32T4 = i32T2 - _i32Ido;
			for ( K = 0; K < _i32L1; K++ ) {
				i32T3 += _i32Ido;
				i32T4 += _i32Ido;
				_pfC1[i32T3] = _pfCh[i32T3] + _pfCh[i32T4];
				_pfC1[i32T4] = _pfCh[i32T4] - _pfCh[i32T3];
			}
		}

		fAr1 = 1.0f;
		fAi1 = 0.0f;
		i32T1 = 0;
		i32T2 = i32Ipp2 * _i32IdL1;
		i32T3 = (_i32Ip - 1) * _i32IdL1;
		for ( i32L = 1; i32L < i32IpPh; i32L++ ) {
			i32T1 += _i32IdL1;
			i32T2 -= _i32IdL1;
			fAr1h = fDcp * fAr1 - fDsp * fAi1;
			fAi1 = fDcp * fAi1 + fDsp * fAr1;
			fAr1 = fAr1h;
			i32T4 = i32T1;
			i32T5 = i32T2;
			i32T6 = i32T3;
			i32T7 = _i32IdL1;

			for ( i32Ik = 0; i32Ik < _i32IdL1; i32Ik++ ) {
				_pfCh2[i32T4++] = _pfC2[i32Ik] + fAr1 * _pfC2[i32T7++];
				_pfCh2[i32T5++] = fAi1 * _pfC2[i32T6++];
			}

			fDc2 = fAr1;
			fDs2 = fAi1;
			fAr2 = fAr1;
			fAi2 = fAi1;

			i32T4 = _i32IdL1;
			i32T5 = (i32Ipp2 - 1) * _i32IdL1;
			for ( J = 2; J < i32IpPh; J++ ) {
				i32T4 += _i32IdL1;
				i32T5 -= _i32IdL1;

				fAr2h = fDc2 * fAr2 - fDs2 * fAi2;
				fAi2 = fDc2 * fAi2 + fDs2 * fAr2;
				fAr2 = fAr2h;

				i32T6 = i32T1;
				i32T7 = i32T2;
				i32T8 = i32T4;
				i32T9 = i32T5;
				for ( i32Ik = 0; i32Ik < _i32IdL1; i32Ik++ ) {
					_pfCh2[i32T6++] += fAr2 * _pfC2[i32T8++];
					_pfCh2[i32T7++] += fAi2 * _pfC2[i32T9++];
				}
			}
		}

		i32T1 = 0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += _i32IdL1;
			i32T2 = i32T1;
			for ( i32Ik = 0; i32Ik < _i32IdL1; i32Ik++ ) { _pfCh2[i32Ik] += _pfC2[i32T2++]; }
		}

		if ( _i32Ido<_i32L1 ) { goto L132; }

		i32T1 = 0;
		i32T2 = 0;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T3 = i32T1;
			i32T4 = i32T2;
			for ( I = 0; I < _i32Ido; I++ ) { _pfCc[i32T4++] = _pfCh[i32T3++]; }
			i32T1 += _i32Ido;
			i32T2 += i32T10;
		}

		goto L135;

	L132 :
		for ( I = 0; I < _i32Ido; I++ ) {
			i32T1 = I;
			i32T2 = I;
			for ( K = 0; K < _i32L1; K++ ) {
				_pfCc[i32T2] = _pfCh[i32T1];
				i32T1 += _i32Ido;
				i32T2 += i32T10;
			}
		}

	L135 :
		i32T1 = 0;
		i32T2 = _i32Ido << 1;
		i32T3 = 0;
		i32T4 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {

			i32T1 += i32T2;
			i32T3 += i32T0;
			i32T4 -= i32T0;

			i32T5 = i32T1;
			i32T6 = i32T3;
			i32T7 = i32T4;

			for ( K = 0; K < _i32L1; K++ ) {
				_pfCc[i32T5-1] = _pfCh[i32T6];
				_pfCc[i32T5] = _pfCh[i32T7];
				i32T5 += i32T10;
				i32T6 += _i32Ido;
				i32T7 += _i32Ido;
			}
		}

		if ( _i32Ido == 1 ) { return; }
		if ( i32Nbd < _i32L1 ) { goto L141; }

		i32T1 = -_i32Ido;
		i32T3 = 0;
		i32T4 = 0;
		i32T5 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T2;
			i32T3 += i32T2;
			i32T4 += i32T0;
			i32T5 -= i32T0;
			i32T6 = i32T1;
			i32T7 = i32T3;
			i32T8 = i32T4;
			i32T9 = i32T5;
			for ( K = 0; K < _i32L1; K++ ) {
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32Ic = i32Idp2-I;
					_pfCc[I+i32T7-1] = _pfCh[I+i32T8-1] + _pfCh[I+i32T9-1];
					_pfCc[i32Ic+i32T6-1] = _pfCh[I+i32T8-1] - _pfCh[I+i32T9-1];
					_pfCc[I+i32T7] = _pfCh[I+i32T8] + _pfCh[I+i32T9];
					_pfCc[i32Ic+i32T6] = _pfCh[I+i32T9] - _pfCh[I+i32T8];
				}
				i32T6 += i32T10;
				i32T7 += i32T10;
				i32T8 += _i32Ido;
				i32T9 += _i32Ido;
			}
		}
		return;

	L141 :

		i32T1 = -_i32Ido;
		i32T3 = 0;
		i32T4 = 0;
		i32T5 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T2;
			i32T3 += i32T2;
			i32T4 += i32T0;
			i32T5 -= i32T0;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T6 = i32Idp2 + i32T1 - I;
				i32T7 = I + i32T3;
				i32T8 = I + i32T4;
				i32T9 = I + i32T5;
				for ( K = 0; K < _i32L1; K++ ) {
					_pfCc[i32T7-1] = _pfCh[i32T8-1] + _pfCh[i32T9-1];
					_pfCc[i32T6-1] = _pfCh[i32T8-1] - _pfCh[i32T9-1];
					_pfCc[i32T7] = _pfCh[i32T8] + _pfCh[i32T9];
					_pfCc[i32T6] = _pfCh[i32T9] - _pfCh[i32T8];
					i32T6 += i32T10;
					i32T7 += i32T10;
					i32T8 += _i32Ido;
					i32T9 += _i32Ido;
				}
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa1 Undocumented parameter from the Vorbis library.
	 * \param _pfWa2 Undocumented parameter from the Vorbis library.
	 * \param _pfWa3 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradb4( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1, 
		float * _pfWa2, float * _pfWa3 ) {
		static const float fSqrt2 = 1.414213562373095f;
		int32_t I, K, i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6, i32T7, i32T8;
		float fCi2, fCi3, fCi4, fCr2, fCr3, fCr4, fTi1, fTi2, fTi3, fTi4, fTr1, fTr2, fTr3, fTr4;
		i32T0 = _i32L1 * _i32Ido;

		i32T1 = 0;
		i32T2 = _i32Ido << 2;
		i32T3 = 0;
		i32T6 = _i32Ido << 1;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T4 = i32T3 + i32T6;
			i32T5 = i32T1;
			fTr3 = _pfCc[i32T4-1] + _pfCc[i32T4-1];
			fTr4 = _pfCc[i32T4] + _pfCc[i32T4];
			fTr1 = _pfCc[i32T3] - _pfCc[(i32T4+=i32T6)-1];
			fTr2 = _pfCc[i32T3] + _pfCc[i32T4-1];
			_pfCh[i32T5] = fTr2+fTr3;
			_pfCh[i32T5+=i32T0] = fTr1 - fTr4;
			_pfCh[i32T5+=i32T0] = fTr2 - fTr3;
			_pfCh[i32T5+=i32T0] = fTr1 + fTr4;
			i32T1 += _i32Ido;
			i32T3 += i32T2;
		}

		if ( _i32Ido < 2 ) { return; }
		if ( _i32Ido == 2 ) { goto L105; }

		i32T1 = 0;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T5 = (i32T4 = (i32T3 = (i32T2 = i32T1 << 2) + i32T6)) + i32T6;
			i32T7 = i32T1;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T2 += 2;
				i32T3 += 2;
				i32T4 -= 2;
				i32T5 -= 2;
				i32T7 += 2;
				fTi1 = _pfCc[i32T2] + _pfCc[i32T5];
				fTi2 = _pfCc[i32T2] - _pfCc[i32T5];
				fTi3 = _pfCc[i32T3] - _pfCc[i32T4];
				fTr4 = _pfCc[i32T3] + _pfCc[i32T4];
				fTr1 = _pfCc[i32T2-1] - _pfCc[i32T5-1];
				fTr2 = _pfCc[i32T2-1] + _pfCc[i32T5-1];
				fTi4 = _pfCc[i32T3-1] - _pfCc[i32T4-1];
				fTr3 = _pfCc[i32T3-1] + _pfCc[i32T4-1];
				_pfCh[i32T7-1] = fTr2+fTr3;
				fCr3 = fTr2-fTr3;
				_pfCh[i32T7] = fTi2+fTi3;
				fCi3 = fTi2-fTi3;
				fCr2 = fTr1-fTr4;
				fCr4 = fTr1+fTr4;
				fCi2 = fTi1+fTi4;
				fCi4 = fTi1-fTi4;

				_pfCh[(i32T8=i32T7+i32T0)-1] = _pfWa1[I-2] * fCr2 - _pfWa1[I-1]*fCi2;
				_pfCh[i32T8] = _pfWa1[I-2] * fCi2 + _pfWa1[I-1] * fCr2;
				_pfCh[(i32T8+=i32T0)-1] = _pfWa2[I-2] * fCr3 - _pfWa2[I-1] * fCi3;
				_pfCh[i32T8] = _pfWa2[I-2] * fCi3 + _pfWa2[I-1] * fCr3;
				_pfCh[( i32T8+=i32T0)-1] = _pfWa3[I-2] * fCr4 - _pfWa3[I-1] * fCi4;
				_pfCh[i32T8] = _pfWa3[I-2] * fCi4 + _pfWa3[I-1] * fCr4;
			}
			i32T1 += _i32Ido;
		}

		if ( _i32Ido%2 == 1 ) { return; }

	L105 :

		i32T1 = _i32Ido;
		i32T2 = _i32Ido << 2;
		i32T3 = _i32Ido - 1;
		i32T4 = _i32Ido + (_i32Ido << 1);
		for ( K = 0; K < _i32L1; K++ ) {
			i32T5 = i32T3;
			fTi1 = _pfCc[i32T1] + _pfCc[i32T4];
			fTi2 = _pfCc[i32T4] - _pfCc[i32T1];
			fTr1 = _pfCc[i32T1-1] - _pfCc[i32T4-1];
			fTr2 = _pfCc[i32T1-1] + _pfCc[i32T4-1];
			_pfCh[i32T5] = fTr2 + fTr2;
			_pfCh[i32T5 += i32T0] = fSqrt2 * (fTr1 - fTi1);
			_pfCh[i32T5 += i32T0] = fTi2 + fTi2;
			_pfCh[i32T5 += i32T0] = -fSqrt2 * (fTr1 + fTi1);

			i32T3 += _i32Ido;
			i32T1 += i32T2;
			i32T4 += i32T2;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa1 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradb2( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1 ) {
		int32_t I, K, i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6;
		float fTi2, fTr2;

		i32T0 = _i32L1 * _i32Ido;

		i32T1 = 0;
		i32T2 = 0;
		i32T3 = (_i32Ido << 1) - 1;
		for ( K = 0; K < _i32L1; K++ ) {
			_pfCh[i32T1] = _pfCc[i32T2] + _pfCc[i32T3+i32T2];
			_pfCh[i32T1+i32T0] = _pfCc[i32T2] - _pfCc[i32T3+i32T2];
			i32T2 = (i32T1 += _i32Ido) << 1;
		}

		if ( _i32Ido < 2 ) { return; }
		if ( _i32Ido == 2 ) { goto L105; }

		i32T1 = 0;
		i32T2 = 0;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T3 = i32T1;
			i32T5 = ( i32T4 = i32T2 )+( _i32Ido << 1 );
			i32T6 = i32T0+i32T1;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T3 += 2;
				i32T4 += 2;
				i32T5 -= 2;
				i32T6 += 2;
				_pfCh[i32T3-1] = _pfCc[i32T4-1] + _pfCc[i32T5-1];
				fTr2 = _pfCc[i32T4-1] - _pfCc[i32T5-1];
				_pfCh[i32T3] = _pfCc[i32T4] - _pfCc[i32T5];
				fTi2 = _pfCc[i32T4] + _pfCc[i32T5];
				_pfCh[i32T6-1] = _pfWa1[I-2] * fTr2 - _pfWa1[I-1]*fTi2;
				_pfCh[i32T6] = _pfWa1[I-2] * fTi2 + _pfWa1[I-1]*fTr2;
			}
			i32T2 = (i32T1 += _i32Ido) << 1;
		}

		if ( _i32Ido % 2 == 1 ) { return; }

	L105 :
		i32T1 = _i32Ido - 1;
		i32T2 = _i32Ido - 1;
		for ( K = 0; K < _i32L1; K++ ) {
			_pfCh[i32T1] = _pfCc[i32T2] + _pfCc[i32T2];
			_pfCh[i32T1+i32T0] = -(_pfCc[i32T2+1] + _pfCc[i32T2+1]);
			i32T1 += _i32Ido;
			i32T2 += _i32Ido << 1;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfWa1 Undocumented parameter from the Vorbis library.
	 * \param _pfWa2 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradb3( int32_t _i32Ido, int32_t _i32L1, float * _pfCc, float * _pfCh, float * _pfWa1, 
		float *_pfWa2 ) {
		static const float fTaur = -0.5f;
		static const float fTaui = 0.8660254037844386f;
		int32_t I, K, i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6, i32T7, i32T8, i32T9, i32T10;
		float fCi2, fCi3, fDi2, fDi3, fCr2, fCr3, fDr2, fDr3, fTi2, fTr2;
		i32T0 = _i32L1 * _i32Ido;

		i32T1 = 0;
		i32T2 = i32T0 << 1;
		i32T3 = _i32Ido << 1;
		i32T4 = _i32Ido + (_i32Ido << 1);
		i32T5 = 0;
		for ( K = 0; K < _i32L1; K++ ) {
			fTr2 = _pfCc[i32T3-1] + _pfCc[i32T3-1];
			fCr2 = _pfCc[i32T5] + (fTaur * fTr2);
			_pfCh[i32T1] = _pfCc[i32T5] + fTr2;
			fCi3 = fTaui * (_pfCc[i32T3] + _pfCc[i32T3]);
			_pfCh[i32T1+i32T0] = fCr2 - fCi3;
			_pfCh[i32T1+i32T2] = fCr2 + fCi3;
			i32T1 += _i32Ido;
			i32T3 += i32T4;
			i32T5 += i32T4;
		}

		if ( _i32Ido == 1 ) { return; }

		i32T1 = 0;
		i32T3 = _i32Ido << 1;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T7 = i32T1 + (i32T1 << 1);
			i32T6 = (i32T5 = i32T7 + i32T3);
			i32T8 = i32T1;
			i32T10 = (i32T9 = i32T1 + i32T0) + i32T0;

			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T5 += 2;
				i32T6 -= 2;
				i32T7 += 2;
				i32T8 += 2;
				i32T9 += 2;
				i32T10 += 2;
				fTr2 = _pfCc[i32T5-1] + _pfCc[i32T6-1];
				fCr2 = _pfCc[i32T7-1] + (fTaur * fTr2);
				_pfCh[i32T8-1] = _pfCc[i32T7-1] + fTr2;
				fTi2 = _pfCc[i32T5] - _pfCc[i32T6];
				fCi2 = _pfCc[i32T7] + (fTaur * fTi2);
				_pfCh[i32T8] = _pfCc[i32T7] + fTi2;
				fCr3 = fTaui * (_pfCc[i32T5-1] - _pfCc[i32T6-1]);
				fCi3 = fTaui * (_pfCc[i32T5] + _pfCc[i32T6]);
				fDr2 = fCr2 - fCi3;
				fDr3 = fCr2 + fCi3;
				fDi2 = fCi2 + fCr3;
				fDi3 = fCi2 - fCr3;
				_pfCh[i32T9-1] = _pfWa1[I-2] * fDr2 - _pfWa1[I-1] * fDi2;
				_pfCh[i32T9] = _pfWa1[I-2] * fDi2 + _pfWa1[I-1] * fDr2;
				_pfCh[i32T10-1] = _pfWa2[I-2] * fDr3 - _pfWa2[I-1] * fDi3;
				_pfCh[i32T10] = _pfWa2[I-2] * fDi3 + _pfWa2[I-1] * fDr3;
			}
			i32T1 += _i32Ido;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Ido Undocumented parameter from the Vorbis library.
	 * \param _i32Ip Undocumented parameter from the Vorbis library.
	 * \param _i32L1 Undocumented parameter from the Vorbis library.
	 * \param i32IdL1 Undocumented parameter from the Vorbis library.
	 * \param _pfCc Undocumented parameter from the Vorbis library.
	 * \param _pfC1 Undocumented parameter from the Vorbis library.
	 * \param _pfC2 Undocumented parameter from the Vorbis library.
	 * \param _pfCh Undocumented parameter from the Vorbis library.
	 * \param _pfCh2 Undocumented parameter from the Vorbis library.
	 * \param _pfWa Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::dradbg( int32_t _i32Ido, int32_t _i32Ip, int32_t _i32L1, int32_t i32IdL1, float * _pfCc, float * _pfC1, 
		float * _pfC2, float * _pfCh, float * _pfCh2, float * _pfWa ) {
		static float fTwoPi = 6.283185307179586f;
		int32_t i32IdIj, i32IpPh, I, J, K, i32L, i32Ik, i32Is, i32T0, i32T1, i32T2, i32T3, i32T4, i32T5, i32T6, i32T7, i32T8, i32T9, i32T10, 
			i32T11, i32T12;
		float fDc2, fAi1, fAi2, fAr1, fAr2, fDs2;
		int32_t i32Nbd;
		float fDcp, fArg, fDsp, fAr1h, fAr2h;
		int32_t i32Ipp2;

		i32T10 = _i32Ip * _i32Ido;
		i32T0 = _i32L1 * _i32Ido;
		fArg = fTwoPi / static_cast<float>(_i32Ip);
		fDcp = ::cosf( fArg );
		fDsp = ::sinf( fArg );
		i32Nbd = (_i32Ido - 1) >> 1;
		i32Ipp2 = _i32Ip;
		i32IpPh = (_i32Ip + 1) >> 1;
		if ( _i32Ido < _i32L1 ) { goto L103; }

		i32T1 = 0;
		i32T2 = 0;
		for ( K = 0; K < _i32L1; K++ ) {
			i32T3 = i32T1;
			i32T4 = i32T2;
			for ( I = 0; I < _i32Ido; I++ ) {
				_pfCh[i32T3] = _pfCc[i32T4];
				i32T3++;
				i32T4++;
			}
			i32T1 += _i32Ido;
			i32T2 += i32T10;
		}
		goto L106;

	L103 :
		i32T1 = 0;
		for ( I = 0; I < _i32Ido; I++ ) {
			i32T2 = i32T1;
			i32T3 = i32T1;
			for ( K = 0; K < _i32L1; K++ ) {
				_pfCh[i32T2] = _pfCc[i32T3];
				i32T2 += _i32Ido;
				i32T3 += i32T10;
			}
			i32T1++;
		}

	L106 :
		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		i32T7 = (i32T5 = _i32Ido << 1);
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;
			i32T6 = i32T5;
			for ( K = 0; K < _i32L1; K++ ) {
				_pfCh[i32T3] = _pfCc[i32T6-1] + _pfCc[i32T6-1];
				_pfCh[i32T4] = _pfCc[i32T6] + _pfCc[i32T6];
				i32T3 += _i32Ido;
				i32T4 += _i32Ido;
				i32T6 += i32T10;
			}
			i32T5 += i32T7;
		}

		if ( _i32Ido == 1 ) { goto L116; }
		if ( i32Nbd < _i32L1 ) { goto L112; }

		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		i32T7 = 0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;

			i32T7 += (_i32Ido << 1);
			i32T8 = i32T7;
			for ( K = 0; K < _i32L1; K++ ) {
				i32T5 = i32T3;
				i32T6 = i32T4;
				i32T9 = i32T8;
				i32T11 = i32T8;
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32T5 += 2;
					i32T6 += 2;
					i32T9 += 2;
					i32T11 -= 2;
					_pfCh[i32T5-1] = _pfCc[i32T9-1] + _pfCc[i32T11-1];
					_pfCh[i32T6-1] = _pfCc[i32T9-1] - _pfCc[i32T11-1];
					_pfCh[i32T5] = _pfCc[i32T9] - _pfCc[i32T11];
					_pfCh[i32T6] = _pfCc[i32T9] + _pfCc[i32T11];
				}
				i32T3 += _i32Ido;
				i32T4 += _i32Ido;
				i32T8 += i32T10;
			}
		}
		goto L116;

	L112 :
		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		i32T7 = 0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;
			i32T7 += (_i32Ido << 1);
			i32T8 = i32T7;
			i32T9 = i32T7;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T3 += 2;
				i32T4 += 2;
				i32T8 += 2;
				i32T9 -= 2;
				i32T5 = i32T3;
				i32T6 = i32T4;
				i32T11 = i32T8;
				i32T12 = i32T9;
				for ( K = 0; K < _i32L1; K++ ) {
					_pfCh[i32T5-1] = _pfCc[i32T11-1] + _pfCc[i32T12-1];
					_pfCh[i32T6-1] = _pfCc[i32T11-1] - _pfCc[i32T12-1];
					_pfCh[i32T5] = _pfCc[i32T11] - _pfCc[i32T12];
					_pfCh[i32T6] = _pfCc[i32T11] + _pfCc[i32T12];
					i32T5 += _i32Ido;
					i32T6 += _i32Ido;
					i32T11 += i32T10;
					i32T12 += i32T10;
				}
			}
		}

	L116 :
		fAr1 = 1.0f;
		fAi1 = 0.0f;
		i32T1 = 0;
		i32T9 = (i32T2 = i32Ipp2 * i32IdL1);
		i32T3 = (_i32Ip - 1) * i32IdL1;
		for ( i32L = 1; i32L < i32IpPh; i32L++ ) {
			i32T1 += i32IdL1;
			i32T2 -= i32IdL1;

			fAr1h = fDcp * fAr1 - fDsp * fAi1;
			fAi1 = fDcp * fAi1 + fDsp * fAr1;
			fAr1 = fAr1h;
			i32T4 = i32T1;
			i32T5 = i32T2;
			i32T6 = 0;
			i32T7 = i32IdL1;
			i32T8 = i32T3;
			for ( i32Ik = 0; i32Ik < i32IdL1; i32Ik++ ) {
				_pfC2[i32T4++] = _pfCh2[i32T6++] + fAr1 * _pfCh2[i32T7++];
				_pfC2[i32T5++] = fAi1 * _pfCh2[i32T8++];
			}
			fDc2 = fAr1;
			fDs2 = fAi1;
			fAr2 = fAr1;
			fAi2 = fAi1;

			i32T6 = i32IdL1;
			i32T7 = i32T9 - i32IdL1;
			for ( J = 2; J < i32IpPh; J++ ) {
				i32T6 += i32IdL1;
				i32T7 -= i32IdL1;
				fAr2h = fDc2 * fAr2 - fDs2 * fAi2;
				fAi2 = fDc2 * fAi2 + fDs2 * fAr2;
				fAr2 = fAr2h;
				i32T4 = i32T1;
				i32T5 = i32T2;
				i32T11 = i32T6;
				i32T12 = i32T7;
				for ( i32Ik = 0; i32Ik < i32IdL1; i32Ik++ ) {
					_pfC2[i32T4++] += fAr2 * _pfCh2[i32T11++];
					_pfC2[i32T5++] += fAi2 * _pfCh2[i32T12++];
				}
			}
		}

		i32T1 = 0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32IdL1;
			i32T2 = i32T1;
			for ( i32Ik = 0; i32Ik < i32IdL1; i32Ik++ ) { _pfCh2[i32Ik] += _pfCh2[i32T2++]; }
		}

		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;
			for ( K = 0; K < _i32L1; K++ ) {
				_pfCh[i32T3] = _pfC1[i32T3] - _pfC1[i32T4];
				_pfCh[i32T4] = _pfC1[i32T3] + _pfC1[i32T4];
				i32T3 += _i32Ido;
				i32T4 += _i32Ido;
			}
		}

		if ( _i32Ido == 1 ) { goto L132; }
		if ( i32Nbd < _i32L1 ) { goto L128; }

		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;
			for ( K = 0; K < _i32L1; K++ ) {
				i32T5 = i32T3;
				i32T6 = i32T4;
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32T5 += 2;
					i32T6 += 2;
					_pfCh[i32T5-1] = _pfC1[i32T5-1] - _pfC1[i32T6];
					_pfCh[i32T6-1] = _pfC1[i32T5-1] + _pfC1[i32T6];
					_pfCh[i32T5] = _pfC1[i32T5] + _pfC1[i32T6-1];
					_pfCh[i32T6] = _pfC1[i32T5] - _pfC1[i32T6-1];
				}
				i32T3 += _i32Ido;
				i32T4 += _i32Ido;
			}
		}
		goto L132;

	L128 :
		i32T1 = 0;
		i32T2 = i32Ipp2 * i32T0;
		for ( J = 1; J < i32IpPh; J++ ) {
			i32T1 += i32T0;
			i32T2 -= i32T0;
			i32T3 = i32T1;
			i32T4 = i32T2;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T3 += 2;
				i32T4 += 2;
				i32T5 = i32T3;
				i32T6 = i32T4;
				for ( K = 0; K < _i32L1; K++ ) {
					_pfCh[i32T5-1] = _pfC1[i32T5-1] - _pfC1[i32T6];
					_pfCh[i32T6-1] = _pfC1[i32T5-1] + _pfC1[i32T6];
					_pfCh[i32T5] = _pfC1[i32T5] + _pfC1[i32T6-1];
					_pfCh[i32T6] = _pfC1[i32T5] - _pfC1[i32T6-1];
					i32T5 += _i32Ido;
					i32T6 += _i32Ido;
				}
			}
		}

	L132 :
		if ( _i32Ido == 1 ) { return; }

		for ( i32Ik = 0; i32Ik < i32IdL1; i32Ik++ ) { _pfC2[i32Ik] = _pfCh2[i32Ik]; }

		i32T1 = 0;
		for ( J = 1; J < _i32Ip; J++ ) {
			i32T2 = (i32T1 += i32T0);
			for ( K = 0; K < _i32L1; K++ ) {
				_pfC1[i32T2] = _pfCh[i32T2];
				i32T2 += _i32Ido;
			}
		}

		if ( i32Nbd > _i32L1 ) { goto L139; }

		i32Is = -_i32Ido - 1;
		i32T1 = 0;
		for ( J = 1; J < _i32Ip; J++ ) {
			i32Is += _i32Ido;
			i32T1 += i32T0;
			i32IdIj = i32Is;
			i32T2 = i32T1;
			for ( I = 2; I < _i32Ido; I += 2 ) {
				i32T2 += 2;
				i32IdIj += 2;
				i32T3 = i32T2;
				for ( K = 0; K < _i32L1; K++ ) {
					_pfC1[i32T3-1] = _pfWa[i32IdIj-1] * _pfCh[i32T3-1] - _pfWa[i32IdIj] * _pfCh[i32T3];
					_pfC1[i32T3] = _pfWa[i32IdIj-1] * _pfCh[i32T3] + _pfWa[i32IdIj] * _pfCh[i32T3-1];
					i32T3 += _i32Ido;
				}
			}
		}
		return;

	L139 :
		i32Is = -_i32Ido - 1;
		i32T1 = 0;
		for ( J = 1; J < _i32Ip; J++ ) {
			i32Is += _i32Ido;
			i32T1 += i32T0;
			i32T2 = i32T1;
			for ( K = 0; K < _i32L1; K++ ) {
				i32IdIj = i32Is;
				i32T3 = i32T2;
				for ( I = 2; I < _i32Ido; I += 2 ) {
					i32IdIj += 2;
					i32T3 += 2;
					_pfC1[i32T3-1] = _pfWa[i32IdIj-1] * _pfCh[i32T3-1] - _pfWa[i32IdIj] * _pfCh[i32T3];
					_pfC1[i32T3] = _pfWa[i32IdIj-1] * _pfCh[i32T3] + _pfWa[i32IdIj] * _pfCh[i32T3-1];
				}
				i32T2 += _i32Ido;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pfWa Undocumented parameter from the Vorbis library.
	 * \param _pi32Fac Undocumented parameter from the Vorbis library.
	 */
	void CVorbisSmallFt::drfti1( int32_t _i32N, float * _pfWa, int32_t * _pi32Fac ) {
		static const int32_t i32NtRyh[4] = { 4, 2, 3, 5 };
		static const float fTwoPi = 6.28318530717958648f;
		float fArg, fArgH, fArgLd, fFi;
		int32_t i32NtRy = 0, I, J = -1;
		int32_t i32K1, i32L1, i32L2, i32Ib;
		int32_t i32Ld, i32Ii, i32Ip, i32Is, i32Nq, i32Nr;
		int32_t i32Ido, i32Ipm, i32Nfm1;
		int32_t i32Nl = _i32N;
		int32_t i32Nf = 0;

	L101 :
		J++;
		if ( J < 4 ) {
			i32NtRy = i32NtRyh[J];
		}
		else {
			i32NtRy += 2;
		}

	L104 :
		i32Nq = i32Nl / i32NtRy;
		i32Nr = i32Nl - i32NtRy * i32Nq;
		if ( i32Nr != 0 ) { goto L101; }

		i32Nf++;
		_pi32Fac[i32Nf+1] = i32NtRy;
		i32Nl = i32Nq;
		if ( i32NtRy != 2 ) { goto L107; }
		if ( i32Nf == 1 ) { goto L107; }

		for ( I = 1; I < i32Nf; I++ ) {
			i32Ib = i32Nf - I + 1;
			_pi32Fac[i32Ib+1] = _pi32Fac[i32Ib];
		}
		_pi32Fac[2] = 2;

	L107 :
		if ( i32Nl != 1 ) { goto L104; }
		_pi32Fac[0] = _i32N;
		_pi32Fac[1] = i32Nf;
		fArgH = fTwoPi / _i32N;
		i32Is = 0;
		i32Nfm1 = i32Nf-1;
		i32L1 = 1;

		if ( i32Nfm1 == 0 ) { return; }

		for ( i32K1 = 0; i32K1 < i32Nfm1; i32K1++ ) {
			i32Ip = _pi32Fac[i32K1+2];
			i32Ld = 0;
			i32L2 = i32L1 * i32Ip;
			i32Ido = _i32N / i32L2;
			i32Ipm = i32Ip - 1;

			for ( J = 0; J < i32Ipm; J++ ) {
				i32Ld += i32L1;
				I = i32Is;
				fArgLd = static_cast<float>(i32Ld) * fArgH;
				fFi = 0.0f;
				for ( i32Ii = 2; i32Ii < i32Ido; i32Ii += 2 ) {
					fFi += 1.0f;
					fArg = fFi * fArgLd;
					_pfWa[I++] = ::cosf( fArg );
					_pfWa[I++] = ::sinf( fArg );
				}
				i32Is += i32Ido;
			}
			i32L1 = i32L2;
		}
	}

}	// namespace lss
