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

 function: random psychoacoustics (not including preecho)
 last mod: $Id: psy.h 16946 2010-03-03 16:12:40Z xiphmont $

 ********************************************************************/

#include "LSSVorbisPsy.h"
#include "LSSVorbisCodecInternal.h"
#include "Search/LSSTDSearch.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pipVi Undocumented parameter from the Vorbis library.
	 * \param _pipgGi Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32Rate Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_psy_init( vorbis_look_psy * _plpP, vorbis_info_psy * _pipVi, 
		vorbis_info_psy_global * _pipgGi, int32_t _i32N, int32_t _i32Rate ) {
		int32_t I, J, i32Lo = -99, i32Hi = 1;
		int32_t i32Maxoc;
		CStd::MemSet( _plpP, 0, sizeof( (*_plpP) ) );

		_plpP->eighth_octave_lines = _pipgGi->eighth_octave_lines;
		_plpP->shiftoc = static_cast<int32_t>(rint( ::log( _pipgGi->eighth_octave_lines * 8.0f ) / ::log( 2.0f ) ) - 1);

		_plpP->firstoc = static_cast<int32_t>(toOC( 0.25f * _i32Rate * 0.5f / _i32N ) * (1 << (_plpP->shiftoc + 1)) - _pipgGi->eighth_octave_lines);
		i32Maxoc = static_cast<int32_t>(toOC( (_i32N + 0.25f) * _i32Rate * 0.5f / _i32N ) * (1 << (_plpP->shiftoc + 1)) + 0.5f);
		_plpP->total_octave_lines = i32Maxoc - _plpP->firstoc + 1;
		_plpP->ath = static_cast<float *>(CMemLib::MAlloc( _i32N * sizeof( (*_plpP->ath) ) ));

		_plpP->octave = static_cast<int32_t *>(CMemLib::MAlloc( _i32N * sizeof( (*_plpP->octave) ) ));
		_plpP->bark = static_cast<int32_t *>(CMemLib::MAlloc( _i32N * sizeof( (*_plpP->bark) ) ));
		_plpP->vi = _pipVi;
		_plpP->n = _i32N;
		_plpP->rate = _i32Rate;

		// AoTuV HF weighting.
		_plpP->m_val = 1.;
		if ( _i32Rate < 26000 ) { _plpP->m_val = 0; }
		else if ( _i32Rate < 38000 ) { _plpP->m_val = 0.94f; }		// 32kHz.
		else if ( _i32Rate > 46000 ) { _plpP->m_val = 1.275f; }		// 48kHz.

		// Set up the lookups for a given blocksize and sample rate.
		for ( I = 0, J = 0; I < MAX_ATH - 1; I++ ) {
			int32_t i32EndPos = static_cast<int32_t>(rint( fromOC( (I + 1) * 0.125f - 2.0f) * 2 * _i32N / _i32Rate ));
			float i32Base = CVorbisMasking::ATH[I];
			if ( J < i32EndPos ) {
				float fDelta = (CVorbisMasking::ATH[I+1] - i32Base) / (i32EndPos - J);
				for ( ; J < i32EndPos && J < _i32N; J++ ) {
					_plpP->ath[J] = i32Base + 100.0f;
					i32Base += fDelta;
				}
			}
		}

		for ( ; J < _i32N; J++ ) {
			_plpP->ath[J] = _plpP->ath[J-1];
		}

		for ( I = 0; I < _i32N; I++ ) {
			float fBark = static_cast<float>(toBARK( _i32Rate / (2 * _i32N) * I ));

			for ( ; i32Lo + _pipVi->noisewindowlomin < I &&
				toBARK( _i32Rate / (2 * _i32N) * i32Lo ) < (fBark - _pipVi->noisewindowlo); i32Lo++ ) { ; }

			for ( ; i32Hi <= _i32N && (i32Hi < I + _pipVi->noisewindowhimin ||
				toBARK( _i32Rate / (2 * _i32N) * i32Hi ) < (fBark + _pipVi->noisewindowhi)); i32Hi++ ) { ; }

			_plpP->bark[I] = ((i32Lo - 1) << 16) + (i32Hi - 1);

		}

		for ( I = 0; I < _i32N; I++ ) {
			_plpP->octave[I] = static_cast<int32_t>(toOC( (I + 0.25f) * 0.5f * _i32Rate / _i32N ) * (1 << (_plpP->shiftoc + 1)) + 0.5f);
		}

		_plpP->tonecurves = setup_tone_curves( _pipVi->toneatt, _i32Rate * 0.5f / _i32N, _i32N, 
			_pipVi->tone_centerboost, _pipVi->tone_decay );

		// Set up rolling _pfNoise median.
		_plpP->noiseoffset = static_cast<float **>(CMemLib::MAlloc( P_NOISECURVES * sizeof( (*_plpP->noiseoffset) ) ));
		for ( I = 0; I < P_NOISECURVES; I++ ) {
			_plpP->noiseoffset[I] = static_cast<float *>(CMemLib::MAlloc( _i32N * sizeof( (*(*_plpP->noiseoffset)) ) ));
		}

		for ( I = 0; I < _i32N; I++ ) {
			float fHalfOc = static_cast<float>(toOC( (I + 0.5f) * _i32Rate / (2.0f * _i32N) ) * 2.0f);
			int32_t i32IntHalfOc;
			float fDel;

			if ( fHalfOc < 0 ) { fHalfOc = 0; }
			if ( fHalfOc >= P_BANDS - 1 ) { fHalfOc = P_BANDS - 1; }
			i32IntHalfOc = static_cast<int32_t>(fHalfOc);
			fDel = fHalfOc - i32IntHalfOc;

			for ( J = 0; J < P_NOISECURVES; J++ ) {
				_plpP->noiseoffset[J][I] = 
					_plpP->vi->noiseoff[J][i32IntHalfOc] * (1.0f - fDel) +
					_plpP->vi->noiseoff[J][i32IntHalfOc+1] * fDel;
			}

		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_psy_clear( vorbis_look_psy * _plpP ) {
		int32_t I, J;
		if ( _plpP ) {
			if ( _plpP->ath ) { CMemLib::Free( _plpP->ath ); }
			if ( _plpP->octave ) { CMemLib::Free( _plpP->octave ); }
			if ( _plpP->bark ) { CMemLib::Free( _plpP->bark ); }
			if ( _plpP->tonecurves ) {
				for ( I = 0; I < P_BANDS; I++ ) {
					for ( J = 0; J < P_LEVELS; J++ ) {
						CMemLib::Free( _plpP->tonecurves[I][J] );
					}
					CMemLib::Free( _plpP->tonecurves[I] );
				}
				CMemLib::Free( _plpP->tonecurves );
			}
			if ( _plpP->noiseoffset ) {
				for ( I = 0; I < P_NOISECURVES; I++ ) {
					CMemLib::Free( _plpP->noiseoffset[I] );
				}
				CMemLib::Free( _plpP->noiseoffset );
			}
			CStd::MemSet( _plpP, 0, sizeof( (*_plpP) ) );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param I Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vi_psy_free( vorbis_info_psy * _pipI ) {
		if ( _pipI ) {
			CStd::MemSet( _pipI, 0, sizeof( (*_pipI) ) );
			CMemLib::Free( _pipI );
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
	 * \param _pfLogMask Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_noisemask( vorbis_look_psy * _plpP, 
		float * _pfLogMdct, 
		float * _pfLogMask ) {

		int32_t I, i32N = _plpP->n;
		float * pfWork = static_cast<float *>(::alloca( i32N * sizeof( (*pfWork) ) ));

		bark_noise_hybridmp( i32N, _plpP->bark, _pfLogMdct, _pfLogMask,
			140.0f, -1 );

		for ( I = 0; I < i32N; I++ ) {
			pfWork[I] = _pfLogMdct[I]-_pfLogMask[I];
		}

		bark_noise_hybridmp( i32N, _plpP->bark, pfWork, _pfLogMask, 0.0f, 
			_plpP->vi->noisewindowfixed );

		for ( I = 0; I < i32N; I++ ) { pfWork[I] = _pfLogMdct[I] - pfWork[I]; }

		for ( I = 0; I < i32N; I++ ) {
			int32_t i32Db = static_cast<int32_t>(_pfLogMask[I] + 0.5f);
			if ( i32Db >= NOISE_COMPAND_LEVELS ) { i32Db = NOISE_COMPAND_LEVELS - 1; }
			if ( i32Db < 0 ) { i32Db = 0; }
			_pfLogMask[I] = pfWork[I] + _plpP->vi->noisecompand[i32Db];
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pfLogFft Undocumented parameter from the Vorbis library.
	 * \param _pfLogMask Undocumented parameter from the Vorbis library.
	 * \param _fGlobalSpecMax Undocumented parameter from the Vorbis library.
	 * \param _fLocalSpecMax Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_tonemask( vorbis_look_psy * _plpP,
		float * _pfLogFft,
		float * _pfLogMask,
		float _fGlobalSpecMax,
		float _fLocalSpecMax ) {
		int32_t I, i32N = _plpP->n;

		float * pfSeed = static_cast<float *>(::alloca( sizeof( (*pfSeed) ) * _plpP->total_octave_lines ));
		float fAtt = _fLocalSpecMax + _plpP->vi->ath_adjatt;
		for ( I = 0; I < _plpP->total_octave_lines; I++ ) {
			pfSeed[I] = NEGINF;
		}

		// Set the ATH (floating below localmax, not global max by a
		//	specified att).
		if ( fAtt<_plpP->vi->ath_maxatt ) {
			fAtt = _plpP->vi->ath_maxatt;
		}

		for ( I = 0; I < i32N; I++ ) {
			_pfLogMask[I] = _plpP->ath[I] + fAtt;
		}

		// Tone masking.
		seed_loop( _plpP, const_cast<const float ***>(_plpP->tonecurves), _pfLogFft, _pfLogMask, pfSeed, _fGlobalSpecMax );
		max_seeds( _plpP, pfSeed, _pfLogMask );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pfNoise Undocumented parameter from the Vorbis library.
	 * \param _pfTone Undocumented parameter from the Vorbis library.
	 * \param _i32OffsetSelect Undocumented parameter from the Vorbis library.
	 * \param _pfLogMask Undocumented parameter from the Vorbis library.
	 * \param _pfMdct Undocumented parameter from the Vorbis library.
	 * \param _pfLogMdct Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_offset_and_mix( vorbis_look_psy * _plpP, 
		float * _pfNoise, 
		float * _pfTone, 
		int32_t _i32OffsetSelect, 
		float * _pfLogMask, 
		float * _pfMdct, 
		float * _pfLogMdct ) {
		int32_t I, i32N = _plpP->n;
		float fDe, fCoefFi, fCx;	// AoTuV.
		float fToneAtt = _plpP->vi->tone_masteratt[_i32OffsetSelect];

		fCx = _plpP->m_val;

		for ( I = 0; I < i32N; I++ ) {
			float fVal = _pfNoise[I] + _plpP->noiseoffset[_i32OffsetSelect][I];
			if ( fVal>_plpP->vi->noisemaxsupp ) { fVal = _plpP->vi->noisemaxsupp; }
			_pfLogMask[I] = CStd::Max( fVal, _pfTone[I] + fToneAtt );


			// AoTuV.
			/** @ M1 **
				The following codes improve a noise problem.
				A fundamental idea uses the value of masking and carries out
				the relative compensation of the MDCT.
				However, this code is not perfect and all noise problems cannot be solved.
				by Aoyumi @ 2004/04/18
			*/

			if ( _i32OffsetSelect == 1 ) {
				fCoefFi = -17.2f;						// fCoefFi is a -17.2dB threshold.
				fVal = fVal - _pfLogMdct[I];			// fVal == _pfMdct line value relative to floor in i32Db.

				if ( fVal > fCoefFi ) {
					// MDCT value is > -17.2 dB below floor.

					fDe = 1.0f - ((fVal - fCoefFi) * 0.005f * fCx);
					// Pro-rated attenuation:
					//	-0.00 dB boost if _pfMdct value is -17.2dB (relative to floor).
					//	-0.77 dB boost if _pfMdct value is 0dB (relative to floor).
					//	-1.64 dB boost if _pfMdct value is +17.2dB (relative to floor).
					//	etc.

					if ( fDe < 0 ) { fDe = 0.0001f; }
				}
				else {
					// MDCT value is <= -17.2 i32Db below floor.
					fDe = 1.0f - ((fVal - fCoefFi) * 0.0003f * fCx);
				}
				// Pro-rated attenuation:
				//	+0.00 i32Db atten if _pfMdct value is -17.2dB ( relative to floor ).
				//	+0.45 i32Db atten if _pfMdct value is -34.4dB ( relative to floor ).
				//	etc.

				_pfMdct[I] *= fDe;

			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _fAmp Undocumented parameter from the Vorbis library.
	 * \param _pdsVd Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float CVorbisPsy::_vp_ampmax_decay( float _fAmp, CVorbisCodec::vorbis_dsp_state * _pdsVd ) {
		CVorbisCodec::vorbis_info * pipVi = _pdsVd->vi;
		CVorbisCodecInternal::codec_setup_info * psiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pipVi->codec_setup);
		vorbis_info_psy_global *pipgGi = &psiCi->psy_g_param;

		int32_t i32N = psiCi->blocksizes[_pdsVd->W] / 2;
		float fSecs = static_cast<float>(i32N / pipVi->rate);

		_fAmp += fSecs * pipgGi->ampmax_att_per_sec;
		if ( _fAmp < -9999.0f ) { _fAmp = -9999.0f; }
		return _fAmp;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32BlobNo Undocumented parameter from the Vorbis library.
	 * \param _pipgG Undocumented parameter from the Vorbis library.
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pipVi Undocumented parameter from the Vorbis library.
	 * \param _pfMdct Undocumented parameter from the Vorbis library.
	 * \param _ppi32Work Undocumented parameter from the Vorbis library.
	 * \param _pi32NonZero Undocumented parameter from the Vorbis library.
	 * \param _i32SlidingLowPass Undocumented parameter from the Vorbis library.
	 * \param _i32Ch Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::_vp_couple_quantize_normalize( int32_t _i32BlobNo, 
		vorbis_info_psy_global * _pipgG,
		vorbis_look_psy * _plpP,
		CVorbisStructs::vorbis_info_mapping0 * _pipVi,
		float ** _pfMdct,
		int32_t ** _ppi32Work, 
		int32_t * _pi32NonZero, 
		int32_t _i32SlidingLowPass, 
		int32_t _i32Ch ) {


		int32_t I;
		int32_t i32N = _plpP->n;
		int32_t i32Partition = (_plpP->vi->normal_p ? _plpP->vi->normal_partition : 16);
		int32_t i32Limit = _pipgG->coupling_pointlimit[_plpP->vi->blockflag][_i32BlobNo];
		float fPrePoint = static_cast<float>(CVorbisConstants::stereo_threshholds[_pipgG->coupling_prepointamp[_i32BlobNo]]);
		float fPostPoint = static_cast<float>(CVorbisConstants::stereo_threshholds[_pipgG->coupling_postpointamp[_i32BlobNo]]);
		//float fDe = 0.1f * _plpP->m_val;		// A blend of the AoTuV M2 and M3 code here and below.

		// _pfMdct is our ppfRaw MDCT output, floor not removed.
		// In/out passes in the ifloor, passes back quantized result.

		// Unquantized energy (negative indicates amplitude has negative sign).
		float ** ppfRaw = static_cast<float **>(::alloca( _i32Ch * sizeof( (*ppfRaw) ) ));

		// Dual pupose; quantized energy (if _pi32Flag set), othersize ::fabs( ppfRaw ).
		float ** ppfQuant = static_cast<float **>(::alloca( _i32Ch * sizeof( (*ppfQuant) ) ));

		// Floor energy.
		float ** ppfFloor = static_cast<float **>(::alloca( _i32Ch * sizeof( (*ppfFloor) ) ));

		// Flags indicating ppfRaw/quantized status of elements in ppfRaw vector.
		int32_t ** ppi32Flag = static_cast<int32_t **>(::alloca( _i32Ch * sizeof( (*ppi32Flag) ) ));

		// Non-zero ppi32Flag working vector.
		int32_t * pi32Nz = static_cast<int32_t *>(::alloca( _i32Ch * sizeof( (*pi32Nz) ) ));

		// Energy surplus/defecit tracking.
		float * pfAcc = static_cast<float *>(::alloca( (_i32Ch + _pipVi->coupling_steps) * sizeof( (*pfAcc) ) ) );

		// The threshold of a stereo is changed with the size of i32N.
		if ( i32N > 1000 ) {
			fPostPoint = static_cast<float>(CVorbisConstants::stereo_threshholds_limited[_pipgG->coupling_postpointamp[_i32BlobNo]] );
		}

		ppfRaw[0] = static_cast<float *>(::alloca( _i32Ch * i32Partition * sizeof( (*(*ppfRaw)) ) ));
		ppfQuant[0] = static_cast<float *>(::alloca( _i32Ch*i32Partition * sizeof( (*(*ppfQuant)) ) ));
		ppfFloor[0] = static_cast<float *>(::alloca( _i32Ch*i32Partition * sizeof( (*(*ppfFloor)) ) ));
		ppi32Flag[0] = static_cast<int32_t *>(::alloca( _i32Ch*i32Partition * sizeof( (*(*ppi32Flag)) ) ));

		for ( I = 1; I < _i32Ch; I++ ) {
			ppfRaw[I] = &ppfRaw[0][i32Partition*I];
			ppfQuant[I] = &ppfQuant[0][i32Partition*I];
			ppfFloor[I] = &ppfFloor[0][i32Partition*I];
			ppi32Flag[I] = &ppi32Flag[0][i32Partition*I];
		}
		for ( I = 0; I < _i32Ch + _pipVi->coupling_steps; I++ )
			pfAcc[I] = 0.0f;

		for ( I = 0; I < i32N; I += i32Partition ) {
			int32_t K, J, i32Jn = i32Partition > i32N - I ? i32N - I : i32Partition;
			int32_t i32Step, i32Track = 0;

			CStd::MemCpy( pi32Nz, _pi32NonZero, sizeof( (*pi32Nz) ) * _i32Ch );

			// Prefill.
			CStd::MemSet( ppi32Flag[0], 0, _i32Ch * i32Partition * sizeof( (*(*ppi32Flag)) ) );
			for ( K = 0; K < _i32Ch; K++ ) {
				int32_t * pi32IOut = &_ppi32Work[K][I];
				if ( pi32Nz[K] ) {
					for ( J = 0; J < i32Jn; J++ ) {
						ppfFloor[K][J] = CVorbisConstants::FLOOR1_fromdB_LOOKUP[pi32IOut[J]];
					}

					flag_lossless( i32Limit, fPrePoint, fPostPoint, &_pfMdct[K][I], ppfFloor[K], ppi32Flag[K], I, i32Jn );

					for ( J = 0; J < i32Jn; J++ ) {
						ppfQuant[K][J] = ppfRaw[K][J] = _pfMdct[K][I+J] * _pfMdct[K][I+J];
						if ( _pfMdct[K][I+J] < 0.0f ) { ppfRaw[K][J] *= -1.0f; }
						ppfFloor[K][J] *= ppfFloor[K][J];
					}

					pfAcc[i32Track] = noise_normalize( _plpP, i32Limit, ppfRaw[K], ppfQuant[K], ppfFloor[K], NULL, pfAcc[i32Track], I, i32Jn, pi32IOut );

				}
				else {
					for ( J = 0; J < i32Jn; J++ ) {
						ppfFloor[K][J] = 1e-10f;
						ppfRaw[K][J] = 0.0f;
						ppfQuant[K][J] = 0.0f;
						ppi32Flag[K][J] = 0;
						pi32IOut[J] = 0;
					}
					pfAcc[i32Track] = 0.0f;
				}
				i32Track++;
			}

			// Coupling.
			for ( i32Step = 0; i32Step < _pipVi->coupling_steps; i32Step++ ) {
				int32_t i32Mi = _pipVi->coupling_mag[i32Step];
				int32_t i32Ai = _pipVi->coupling_ang[i32Step];
				int32_t * pi32Im = &_ppi32Work[i32Mi][I];
				int32_t * pi32Ia = &_ppi32Work[i32Ai][I];
				float * pfReM = ppfRaw[i32Mi];
				float * pfReA = ppfRaw[i32Ai];
				float * pfQeM = ppfQuant[i32Mi];
				float * pfQeA = ppfQuant[i32Ai];
				float * pfFloorM = ppfFloor[i32Mi];
				float * pfFloorA = ppfFloor[i32Ai];
				int32_t * pi32Fm = ppi32Flag[i32Mi];
				int32_t * pi32Fa = ppi32Flag[i32Ai];

				if ( pi32Nz[i32Mi] || pi32Nz[i32Ai] ) {
					pi32Nz[i32Mi] = pi32Nz[i32Ai] = 1;

					for ( J = 0; J < i32Jn; J++ ) {

						if ( J < _i32SlidingLowPass - I ) {
							if ( pi32Fm[J] || pi32Fa[J] ) {
								// Lossless coupling.

								pfReM[J] = ::fabsf( pfReM[J] ) + ::fabsf( pfReA[J] );
								pfQeM[J] = pfQeM[J] + pfQeA[J];
								pi32Fm[J] = pi32Fa[J] = 1;

								{
									int32_t i32A = pi32Im[J];
									int32_t i32B = pi32Ia[J];

									if ( ::abs( i32A ) > ::abs( i32B ) ) {
										pi32Ia[J] = (i32A > 0 ? i32A - i32B : i32B - i32A);
									}
									else {
										pi32Ia[J] = (i32B > 0 ? i32A - i32B : i32B - i32A);
										pi32Im[J] = i32B;
									}

									// Collapse two equivalent tuples to one.
									if ( pi32Ia[J] >= ::abs( pi32Im[J] ) * 2 ) {
										pi32Ia[J] = -pi32Ia[J];
										pi32Im[J] = -pi32Im[J];
									}

								}

							}
							else {
								// Lossy (point) coupling.
								if ( J < i32Limit - I ) {
									// Dipole.
									pfReM[J] += pfReA[J];
									pfQeM[J] = ::fabsf( pfReM[J] );
								}
								else {
									// AoTuV.
									/** @ M2 **
										The boost problem by the combination of _pfNoise normalization and point stereo is eased.
										However, this is a temporary patch.
										by Aoyumi @ 2004/04/18
									*/
									/*float fDerate = (1.0f - fDe * ( static_cast<float>(J - i32Limit + I) / static_cast<float>(i32N - i32Limit) ));*/
									/* Elliptical 
									if ( pfReM[J] + pfReA[J] < 0 ) {
										pfReM[J] = -(pfQeM[J] = (::fabs( pfReM[J] ) + ::fabs( pfReA[J] )) * fDerate * fDerate);
									}
									else {
										pfReM[J] = (pfQeM[J] = (::fabs( pfReM[J] ) + ::fabs( pfReA[J] )) * fDerate * fDerate);
									}*/

									// Elliptical.
									if ( pfReM[J] + pfReA[J] < 0 ) {
										pfReM[J] = -(pfQeM[J] = ::fabsf( pfReM[J] ) + ::fabsf( pfReA[J] ));
									}
									else {
										pfReM[J] = (pfQeM[J] = ::fabsf( pfReM[J] ) + ::fabsf( pfReA[J] ));
									}


								}
								pfReA[J] = pfQeA[J] = 0.0f;
								pi32Fa[J] = 1;
								pi32Ia[J] = 0;
							}
						}
						pfFloorM[J] = pfFloorA[J] = pfFloorM[J] + pfFloorA[J];
					}
					// Normalize the resulting mag vector.
					pfAcc[i32Track] = noise_normalize( _plpP, i32Limit, ppfRaw[i32Mi], ppfQuant[i32Mi], ppfFloor[i32Mi], ppi32Flag[i32Mi], pfAcc[i32Track], I, i32Jn, pi32Im );
					i32Track++;
				}
			}
		}

		for ( I = 0; I < _pipVi->coupling_steps; I++ ) {
			// Make sure coupling a zero and a _pi32NonZero channel results in two
			//	non-zero channels.
			if ( _pi32NonZero[_pipVi->coupling_mag[I]] ||
				 _pi32NonZero[_pipVi->coupling_ang[I]] ) {
				_pi32NonZero[_pipVi->coupling_mag[I]] = 1;
				_pi32NonZero[_pipVi->coupling_ang[I]] = 1;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _fCurveAttDb Undocumented parameter from the Vorbis library.
	 * \param _fBinHz Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _fCenterBoost Undocumented parameter from the Vorbis library.
	 * \param _fCenterDecayRate Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float *** CVorbisPsy::setup_tone_curves( float _fCurveAttDb[P_BANDS], float _fBinHz, int32_t _i32N, 
		float _fCenterBoost, float _fCenterDecayRate ) {
		int32_t I, J, K, M;
		float fAth[EHMER_MAX];
		float fWorkC[P_BANDS][P_LEVELS][EHMER_MAX];
		float fAthC[P_LEVELS][EHMER_MAX];
		float * pfBruteBuffer = static_cast<float *>(::alloca( _i32N * sizeof( (*pfBruteBuffer) ) ));

		float *** pppfRet = static_cast<float ***>(CMemLib::MAlloc( sizeof( (*pppfRet) ) * P_BANDS ));

		CStd::MemSet( fWorkC, 0, sizeof( fWorkC ) );

		for ( I = 0; I < P_BANDS; I++ ) {
			// We add back in the ATH to avoid low level curves falling off to
			//	-infinity and unnecessarily cutting off high level _pppfCurves in the
			//	curve limiting (last step).

			// A half-band's settings must be valid over the whole band, and
			//	it's better to mask too little than too much.
			int32_t i32AthOffset = I * 4;
			for ( J = 0; J < EHMER_MAX; J++ ) {
				float fMin = 999.0f;
				for ( K = 0; K < 4; K++ ) {
					if ( J + K + i32AthOffset < MAX_ATH ) {
						if ( fMin > CVorbisMasking::ATH[J+K+i32AthOffset] ) { fMin = CVorbisMasking::ATH[J+K+i32AthOffset]; }
					}
					else {
						if ( fMin > CVorbisMasking::ATH[MAX_ATH-1] ) { fMin = CVorbisMasking::ATH[MAX_ATH-1]; }
					}
				}
				fAth[J] = fMin;
			}

			// Copy _pppfCurves into working space, replicate the 50dB curve to 30 and 40, replicate the 100dB curve to 110.
			for ( J = 0; J < 6; J++ ) {
				CStd::MemCpy( fWorkC[I][J+2], CVorbisMasking::tonemasks[I][J], EHMER_MAX * sizeof( (*CVorbisMasking::tonemasks[I][J]) ) );
			}
			CStd::MemCpy( fWorkC[I][0], CVorbisMasking::tonemasks[I][0], EHMER_MAX * sizeof( (*CVorbisMasking::tonemasks[I][0]) ) );
			CStd::MemCpy( fWorkC[I][1], CVorbisMasking::tonemasks[I][0], EHMER_MAX * sizeof( (*CVorbisMasking::tonemasks[I][0]) ) );

			// Apply centered curve boost/decay.
			for ( J = 0; J < P_LEVELS; J++ ) {
				for ( K = 0; K < EHMER_MAX; K++ ) {
					float fAdj = _fCenterBoost + ::abs( EHMER_OFFSET - K ) * _fCenterDecayRate;
					if ( fAdj < 0.0f && _fCenterBoost > 0.0f ) { fAdj = 0.0f; }
					if ( fAdj > 0.0f && _fCenterBoost < 0.0f ) { fAdj = 0.0f; }
					fWorkC[I][J][K] += fAdj;
				}
			}

			// Normalize curves so the driving amplitude is 0dB.
			// Make temp curves with the ATH overlayed.
			for ( J = 0; J < P_LEVELS; J++ ) {
				attenuate_curve( fWorkC[I][J], _fCurveAttDb[I] + 100.0f - (J < 2 ? 2 : J) * 10.0f - P_LEVEL_0 );
				CStd::MemCpy( fAthC[J], fAth, EHMER_MAX * sizeof( (*(*fAthC)) ) );
				attenuate_curve( fAthC[J], +100.0f - J * 10.0f - P_LEVEL_0 );
				max_curve( fAthC[J], fWorkC[I][J] );
			}

			// Now limit the louder curves.
			//	The idea is this: We don't know what the playback attenuation
			//	will be; 0dB SL moves every time the user twiddles the volume
			//	knob. So that means we have to use a single 'most pessimal' curve
			//	for all masking amplitudes, right?	Wrong.  The *loudest* sound
			//	can be in ( we assume ) a range of ...+100dB] SL.  However, sounds
			//	20dB down will be in a range ...+80], 40dB down is from ...+60], 
			//	etc.

			for ( J = 1; J < P_LEVELS; J++ ) {
				min_curve( fAthC[J], fAthC[J-1] );
				min_curve( fWorkC[I][J], fAthC[J] );
			}
		}

		for ( I = 0; I < P_BANDS; I++ ) {
			int32_t i32HiCurve, i32LoCurve, i32Bin;
			pppfRet[I] = static_cast<float **>(CMemLib::MAlloc( sizeof( (*(*pppfRet)) ) * P_LEVELS ) );

			// Low frequency curves are measured with greater resolution than
			//	the MDCT/FFT will actually give us; we want the curve applied
			//	to the _pfTone data to be pessimistic and thus apply the minimum
			//	masking possible for a given i32Bin.  That means that a single i32Bin
			//	could span more than one octave and that the curve will be a
			//	composite of multiple octaves.  It also may mean that a single
			//	i32Bin may span > an eighth of an octave and that the eighth
			//	octave values may also be composited.

			// Which octave curves will we be compositing?
			i32Bin = static_cast<int32_t>(::floor( fromOC( I * 0.5f ) / _fBinHz ));
			i32LoCurve = static_cast<int32_t>(::ceil( toOC( i32Bin * _fBinHz + 1 ) * 2 ));
			i32HiCurve = static_cast<int32_t>(::floor( toOC( (i32Bin + 1) * _fBinHz ) * 2 ));
			if ( i32LoCurve > I ) { i32LoCurve = I; }
			if ( i32LoCurve < 0 ) { i32LoCurve = 0; }
			if ( i32HiCurve >= P_BANDS ) { i32HiCurve = P_BANDS - 1; }

			for ( M = 0; M < P_LEVELS; M++ ) {
				pppfRet[I][M] = static_cast<float *>(CMemLib::MAlloc( sizeof( (*(*(*pppfRet))) ) * (EHMER_MAX + 2) ));

				for ( J = 0; J < _i32N; J++ ) { pfBruteBuffer[J] = 999.0f; }

				// Render the curve into bins, then pull values back into curve.
				//	The point is that any inherent subsampling aliasing results in
				//	a safe minimum.
				for ( K = i32LoCurve; K <= i32HiCurve; K++ ) {
					int32_t i32L = 0;

					for ( J = 0; J < EHMER_MAX; J++ ) {
						int32_t i32LoBin = static_cast<int32_t>(fromOC( J * 0.125f + K * 0.5f - 2.0625f ) / _fBinHz);
						int32_t i32HiBin = static_cast<int32_t>(fromOC( J * 0.125f + K * 0.5f - 1.9375f ) / _fBinHz + 1.0f);

						if ( i32LoBin < 0 ) { i32LoBin = 0; }
						if ( i32LoBin > _i32N ) { i32LoBin = _i32N; }
						if ( i32LoBin < i32L ) { i32L = i32LoBin; }
						if ( i32HiBin < 0 ) { i32HiBin = 0; }
						if ( i32HiBin > _i32N ) { i32HiBin = _i32N; }

						for ( ; i32L < i32HiBin && i32L < _i32N; i32L++ ) {
							if ( pfBruteBuffer[i32L] > fWorkC[K][M][J] ) {
								pfBruteBuffer[i32L] = fWorkC[K][M][J];
							}
						}
					}

					for ( ; i32L < _i32N; i32L++ ) {
						if ( pfBruteBuffer[i32L] > fWorkC[K][M][EHMER_MAX-1] ) {
							pfBruteBuffer[i32L] = fWorkC[K][M][EHMER_MAX-1];
						}
					}

				}

				// Be equally paranoid about being valid up to next half ocatve.
				if ( I + 1 < P_BANDS ) {
					int32_t i32L = 0;
					K = I + 1;
					for ( J = 0; J < EHMER_MAX; J++ ) {
						int32_t i32LoBin = static_cast<int32_t>(fromOC( J * 0.125f + I * 0.5f - 2.0625f ) / _fBinHz);
						int32_t i32HiBin = static_cast<int32_t>(fromOC( J * 0.125f + I * 0.5f - 1.9375f ) / _fBinHz + 1.0f);

						if ( i32LoBin < 0 ) { i32LoBin = 0; }
						if ( i32LoBin > _i32N ) { i32LoBin = _i32N; }
						if ( i32LoBin < i32L ) { i32L = i32LoBin; }
						if ( i32HiBin < 0 ) { i32HiBin = 0; }
						if ( i32HiBin > _i32N ) { i32HiBin = _i32N; }

						for ( ; i32L < i32HiBin && i32L < _i32N; i32L++ ) {
							if ( pfBruteBuffer[i32L] > fWorkC[K][M][J] ) {
								pfBruteBuffer[i32L] = fWorkC[K][M][J];
							}
						}
					}

					for ( ; i32L < _i32N; i32L++ )
						if ( pfBruteBuffer[i32L] > fWorkC[K][M][EHMER_MAX-1] ) {
							pfBruteBuffer[i32L] = fWorkC[K][M][EHMER_MAX-1];
						}

				}


				for ( J = 0; J < EHMER_MAX; J++ ) {
					int32_t i32Bin2 = static_cast<int32_t>(fromOC( J * 0.125f + I * 0.5f - 2.0f ) / _fBinHz);
					if ( i32Bin2 < 0 ) {
						pppfRet[I][M][J+2] = -999.0f;
					}
					else {
						if ( i32Bin2 >= _i32N ) {
							pppfRet[I][M][J+2] = -999.0f;
						}
						else {
							pppfRet[I][M][J+2] = pfBruteBuffer[i32Bin2];
						}
					}
				}

				// Add fenceposts.
				for ( J = 0; J < EHMER_OFFSET; J++ ) {
					if ( pppfRet[I][M][J+2] > -200.0f ) { break; }
				}
				pppfRet[I][M][0] = static_cast<float>(J);

				for ( J = EHMER_MAX - 1; J > EHMER_OFFSET + 1; J-- ) {
					if ( pppfRet[I][M][J+2] > -200.0f ) { break; }
				}
				pppfRet[I][M][1] = static_cast<float>(J);

			}
		}

		return pppfRet;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfC Undocumented parameter from the Vorbis library.
	 * \param _pfC2 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::min_curve( float * _pfC, float * _pfC2 ) {
		int32_t I;
		for ( I = 0; I < EHMER_MAX; I++ ) {
			if ( _pfC2[I]<_pfC[I] ) {
				_pfC[I] = _pfC2[I];
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfC Undocumented parameter from the Vorbis library.
	 * \param _pfC2 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::max_curve( float * _pfC, float * _pfC2 ) {
		int32_t I;
		for ( I = 0; I < EHMER_MAX; I++ ) {
			if ( _pfC2[I]>_pfC[I] ) {
				_pfC[I] = _pfC2[I];
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfC Undocumented parameter from the Vorbis library.
	 * \param _fAtt Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::attenuate_curve( float * _pfC, float _fAtt ) {
		int32_t I;
		for ( I = 0; I < EHMER_MAX; I++ ) {
			_pfC[I] += _fAtt;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pi32B Undocumented parameter from the Vorbis library.
	 * \param _pfF Undocumented parameter from the Vorbis library.
	 * \param _pfNoise Undocumented parameter from the Vorbis library.
	 * \param _fOffset Undocumented parameter from the Vorbis library.
	 * \param _i32Fixed Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::bark_noise_hybridmp( int32_t _i32N, const int32_t * _pi32B, 
		const float * _pfF, 
		float * _pfNoise, 
		const float _fOffset, 
		const int32_t _i32Fixed ) {

		float *pfN = static_cast<float *>(::alloca( _i32N * sizeof( (*pfN) ) ));
		float *pfX = static_cast<float *>(::alloca( _i32N * sizeof( (*pfN) ) ));
		float *pfXx = static_cast<float *>(::alloca( _i32N * sizeof( (*pfN) ) ));
		float *pfY = static_cast<float *>(::alloca( _i32N * sizeof( (*pfN) ) ));
		float *pfXy = static_cast<float *>(::alloca( _i32N * sizeof( (*pfN) ) ));

		float fTn, fTx, fTxx, fTy, fTxy;
		int32_t I;

		int32_t i32Lo, i32Hi;
		float fR = 0.0f;
		float fA = 0.0f;
		float fB = 0.0f;
		float fD = 1.0f;
		float fW, fX, fY;

		fTn = fTx = fTxx = fTy = fTxy = 0.0f;

		fY = _pfF[0] + _fOffset;
		if ( fY < 1.0f ) { fY = 1.0f; }

		fW = fY * fY * 0.5f;

		fTn += fW;
		fTx += fW;
		fTy += fW * fY;

		pfN[0] = fTn;
		pfX[0] = fTx;
		pfXx[0] = fTxx;
		pfY[0] = fTy;
		pfXy[0] = fTxy;

		for ( I = 1, fX = 1.0f; I < _i32N; I++, fX += 1.0f ) {

			fY = _pfF[I] + _fOffset;
			if ( fY < 1.0f ) { fY = 1.0f; }

			fW = fY * fY;

			fTn += fW;
			fTx += fW * fX;
			fTxx += fW * fX * fX;
			fTy += fW * fY;
			fTxy += fW * fX * fY;

			pfN[I] = fTn;
			pfX[I] = fTx;
			pfXx[I] = fTxx;
			pfY[I] = fTy;
			pfXy[I] = fTxy;
		}

		for ( I = 0, fX = 0.0f; ; I++, fX += 1.0f ) {

			i32Lo = _pi32B[I] >> 16;
			if ( i32Lo >= 0 ) { break; }
			i32Hi = _pi32B[I] & 0xFFFF;

			fTn = pfN[i32Hi] + pfN[-i32Lo];
			fTx = pfX[i32Hi] - pfX[-i32Lo];
			fTxx = pfXx[i32Hi] + pfXx[-i32Lo];
			fTy = pfY[i32Hi] + pfY[-i32Lo];
			fTxy = pfXy[i32Hi] - pfXy[-i32Lo];

			fA = fTy * fTxx - fTx * fTxy;
			fB = fTn * fTxy - fTx * fTy;
			fD = fTn * fTxx - fTx * fTx;
			fR = (fA + fX * fB) / fD;
			if ( fR < 0.0f ) { fR = 0.0f; }

			_pfNoise[I] = fR - _fOffset;
		}

		for ( ; ; I++, fX += 1.0f ) {

			i32Lo = _pi32B[I] >> 16;
			i32Hi = _pi32B[I] & 0xFFFF;
			if ( i32Hi >= _i32N ) { break; }

			fTn = pfN[i32Hi] - pfN[i32Lo];
			fTx = pfX[i32Hi] - pfX[i32Lo];
			fTxx = pfXx[i32Hi] - pfXx[i32Lo];
			fTy = pfY[i32Hi] - pfY[i32Lo];
			fTxy = pfXy[i32Hi] - pfXy[i32Lo];

			fA = fTy * fTxx - fTx * fTxy;
			fB = fTn * fTxy - fTx * fTy;
			fD = fTn * fTxx - fTx * fTx;
			fR = (fA + fX * fB) / fD;
			if ( fR < 0.0f ) { fR = 0.0f; }

			_pfNoise[I] = fR - _fOffset;
		}
		for ( ; I < _i32N; I++, fX += 1.0f ) {

			fR = (fA + fX * fB) / fD;
			if ( fR < 0.0f ) { fR = 0.0f; }

			_pfNoise[I] = fR - _fOffset;
		}

		if ( _i32Fixed <= 0 ) { return; }

		for ( I = 0, fX = 0.0f; ; I++, fX += 1.0f ) {
			i32Hi = I + _i32Fixed / 2;
			i32Lo = i32Hi - _i32Fixed;
			if ( i32Lo >= 0 ) { break; }

			fTn = pfN[i32Hi] + pfN[-i32Lo];
			fTx = pfX[i32Hi] - pfX[-i32Lo];
			fTxx = pfXx[i32Hi] + pfXx[-i32Lo];
			fTy = pfY[i32Hi] + pfY[-i32Lo];
			fTxy = pfXy[i32Hi] - pfXy[-i32Lo];


			fA = fTy * fTxx - fTx * fTxy;
			fB = fTn * fTxy - fTx * fTy;
			fD = fTn * fTxx - fTx * fTx;
			fR = (fA + fX * fB) / fD;

			if ( fR - _fOffset < _pfNoise[I] ) { _pfNoise[I] = fR - _fOffset; }
		}
		for ( ; ; I++, fX += 1.0f ) {

			i32Hi = I + _i32Fixed / 2;
			i32Lo = i32Hi - _i32Fixed;
			if ( i32Hi >= _i32N ) { break; }

			fTn = pfN[i32Hi] - pfN[i32Lo];
			fTx = pfX[i32Hi] - pfX[i32Lo];
			fTxx = pfXx[i32Hi] - pfXx[i32Lo];
			fTy = pfY[i32Hi] - pfY[i32Lo];
			fTxy = pfXy[i32Hi] - pfXy[i32Lo];

			fA = fTy * fTxx - fTx * fTxy;
			fB = fTn * fTxy - fTx * fTy;
			fD = fTn * fTxx - fTx * fTx;
			fR = (fA + fX * fB) / fD;

			if ( fR - _fOffset < _pfNoise[I] ) { _pfNoise[I] = fR - _fOffset; }
		}
		for ( ; I < _i32N; I++, fX += 1.0f ) {
			fR = (fA + fX * fB) / fD;
			if ( fR - _fOffset < _pfNoise[I] ) { _pfNoise[I] = fR - _fOffset; }
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param _pppfCurves Undocumented parameter from the Vorbis library.
	 * \param _pfF Undocumented parameter from the Vorbis library.
	 * \param _pfFlr Undocumented parameter from the Vorbis library.
	 * \param _pfSeed Undocumented parameter from the Vorbis library.
	 * \param _fSpecMax Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::seed_loop( vorbis_look_psy * _plpP, 
		const float *** _pppfCurves, 
		const float * _pfF, 
		const float * _pfFlr, 
		float * _pfSeed, 
		float _fSpecMax ) {
		vorbis_info_psy * pipVi = _plpP->vi;
		int32_t i32N = _plpP->n, I;
		float fDbOffset = pipVi->max_curve_dB - _fSpecMax;

		// Prime the working vector with peak values.
		for ( I = 0; I < i32N; I++ ) {
			float fMax = _pfF[I];
			int32_t i32Oc = _plpP->octave[I];
			while ( I + 1 < i32N && _plpP->octave[I+1] == i32Oc ) {
				I++;
				if ( _pfF[I] > fMax ) {
					fMax = _pfF[I];
				}
			}

			if ( fMax + 6.0f > _pfFlr[I] ) {
				i32Oc = i32Oc >> _plpP->shiftoc;

				if ( i32Oc >= P_BANDS ) { i32Oc = P_BANDS - 1; }
				if ( i32Oc < 0 ) { i32Oc = 0; }

				seed_curve( _pfSeed, 
					_pppfCurves[i32Oc], 
					fMax, 
					_plpP->octave[I] - _plpP->firstoc, 
					_plpP->total_octave_lines, 
					_plpP->eighth_octave_lines, 
					fDbOffset );
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param pfSeed Undocumented parameter from the Vorbis library.
	 * \param _pfFlr Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::max_seeds( vorbis_look_psy * _plpP, 
		float * _pfSeed, 
		float * _pfFlr ) {
		int32_t i32N = _plpP->total_octave_lines;
		int32_t i32LinesPer = _plpP->eighth_octave_lines;
		int32_t i32LinPos = 0;
		int32_t i32Pos;

		seed_chase( _pfSeed, i32LinesPer, i32N );	// For masking.

		i32Pos = _plpP->octave[0] - _plpP->firstoc - (i32LinesPer >> 1);

		while ( i32LinPos + 1 < _plpP->n ) {
			float fMinV = _pfSeed[i32Pos];
			int32_t i32End = ((_plpP->octave[i32LinPos] + _plpP->octave[i32LinPos+1]) >> 1) - _plpP->firstoc;
			if ( fMinV > _plpP->vi->tone_abs_limit ) {
				fMinV = _plpP->vi->tone_abs_limit;
			}
			while ( i32Pos + 1 <= i32End ) {
				i32Pos++;
				if ( (_pfSeed[i32Pos] > NEGINF && _pfSeed[i32Pos] < fMinV) || fMinV == NEGINF ) {
					fMinV = _pfSeed[i32Pos];
				}
			}

			i32End = i32Pos + _plpP->firstoc;
			for ( ; i32LinPos < _plpP->n && _plpP->octave[i32LinPos] <= i32End; i32LinPos++ ) {
				if ( _pfFlr[i32LinPos] < fMinV ) {
					_pfFlr[i32LinPos] = fMinV;
				}
			}
		}

		{
			float fMinV = _pfSeed[_plpP->total_octave_lines-1];
			for ( ;i32LinPos < _plpP->n; i32LinPos++ ) {
				if ( _pfFlr[i32LinPos] < fMinV ) { _pfFlr[i32LinPos] = fMinV; }
			}
		}

	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfSeed Undocumented parameter from the Vorbis library.
	 * \param _pppfCurves Undocumented parameter from the Vorbis library.
	 * \param _fAmp Undocumented parameter from the Vorbis library.
	 * \param _i32Oc Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _i32LinesPer Undocumented parameter from the Vorbis library.
	 * \param _fDbOffset Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::seed_curve( float * _pfSeed, 
		const float ** _pppfCurves, 
		float _fAmp, 
		int32_t _i32Oc, int32_t _i32N, 
		int32_t _i32LinesPer, 
		float _fDbOffset ) {
		int32_t I, i32Post1;
		int32_t i32SeedPtr;
		const float * pfPosts, * pfCurve;

		int32_t i32Choice = static_cast<int32_t>((_fAmp + _fDbOffset - P_LEVEL_0) * 0.1f);
		i32Choice = CStd::Max<int32_t>( i32Choice, 0 );
		i32Choice = CStd::Min<int32_t>( i32Choice, P_LEVELS - 1 );
		pfPosts = _pppfCurves[i32Choice];
		pfCurve = pfPosts + 2;
		i32Post1 = static_cast<int32_t>(pfPosts[1]);
		i32SeedPtr = static_cast<int32_t>(_i32Oc + (pfPosts[0] - EHMER_OFFSET) * _i32LinesPer - (_i32LinesPer >> 1));

		for ( I = static_cast<int32_t>(pfPosts[0]); I < i32Post1; I++ ) {
			if ( i32SeedPtr > 0 ) {
				float fLin = _fAmp + pfCurve[I];
				if ( _pfSeed[i32SeedPtr] < fLin ) { _pfSeed[i32SeedPtr] = fLin; }
			}
			i32SeedPtr += _i32LinesPer;
			if ( i32SeedPtr >= _i32N ) { break; }
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfSeeds Undocumented parameter from the Vorbis library.
	 * \param _i32LinesPer Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::seed_chase( float * _pfSeeds, int32_t _i32LinesPer, int32_t _i32N ) {
		int32_t * pi32PosStack = static_cast<int32_t *>(::alloca( _i32N * sizeof( (*pi32PosStack) ) ));
		float * pi32AmpStack = static_cast<float *>(::alloca( _i32N * sizeof( (*pi32AmpStack) ) ));
		int32_t	 i32Stack = 0;
		int32_t	 i32Pos = 0;
		int32_t	 I;

		for ( I = 0; I < _i32N; I++ ) {
			if ( i32Stack < 2 ) {
				pi32PosStack[i32Stack] = I;
				pi32AmpStack[i32Stack++] = _pfSeeds[I];
			}
			else {
				while ( true ) {
					if ( _pfSeeds[I] < pi32AmpStack[i32Stack-1] ) {
						pi32PosStack[i32Stack] = I;
						pi32AmpStack[i32Stack++] = _pfSeeds[I];
						break;
					}
					else {
						if ( I < pi32PosStack[i32Stack-1] + _i32LinesPer ) {
							if ( i32Stack > 1 && pi32AmpStack[i32Stack-1] <= pi32AmpStack[i32Stack-2] &&
								 I < pi32PosStack[i32Stack-2] + _i32LinesPer ) {
								// We completely overlap, making stack-1 irrelevant.  Pop it.
								i32Stack--;
								continue;
							}
						}
						pi32PosStack[i32Stack] = I;
						pi32AmpStack[i32Stack++] = _pfSeeds[I];
						break;

					}
				}
			}
		}

		// The stack now contains only the positions that are relevant.  Scan
		//	them straight through.

		for ( I = 0; I < i32Stack; I++ ) {
			int32_t i32EndPos;
			if ( I < i32Stack - 1 && pi32AmpStack[I+1] > pi32AmpStack[I] ) {
				i32EndPos = pi32PosStack[I+1];
			}
			else {
				i32EndPos = pi32PosStack[I] + _i32LinesPer + 1;	// +1 is important, else bin 0 is discarded in short frames.
			}
			if ( i32EndPos > _i32N ) { i32EndPos = _i32N; }
			for ( ; i32Pos < i32EndPos; i32Pos++ ) {
				_pfSeeds[i32Pos] = pi32AmpStack[I];
			}
		}

	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32Limit Undocumented parameter from the Vorbis library.
	 * \param _fPrePoint Undocumented parameter from the Vorbis library.
	 * \param _fPostPoint Undocumented parameter from the Vorbis library.
	 * \param _pfMdct Undocumented parameter from the Vorbis library.
	 * \param _pfFloor Undocumented parameter from the Vorbis library.
	 * \param _pi32Flag Undocumented parameter from the Vorbis library.
	 * \param _i32I Undocumented parameter from the Vorbis library.
	 * \param _i32Jn Undocumented parameter from the Vorbis library.
	 */
	void CVorbisPsy::flag_lossless( int32_t _i32Limit, float _fPrePoint, float _fPostPoint, float * _pfMdct, 
		float * _pfFloor, int32_t * _pi32Flag, int32_t _i32I, int32_t _i32Jn ) {
		int32_t J;
		for ( J = 0; J < _i32Jn; J++ ) {
			float fPoint = J >= _i32Limit - _i32I ? _fPostPoint : _fPrePoint;
			float fR = ::fabsf( _pfMdct[J] ) / _pfFloor[J];
			if ( fR < fPoint ) {
				_pi32Flag[J] = 0;
			}
			else {
				_pi32Flag[J] = 1;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _plpP Undocumented parameter from the Vorbis library.
	 * \param i32Limit Undocumented parameter from the Vorbis library.
	 * \param _pfR Undocumented parameter from the Vorbis library.
	 * \param _pfQ Undocumented parameter from the Vorbis library.
	 * \param _pfF Undocumented parameter from the Vorbis library.
	 * \param _pi32Flags Undocumented parameter from the Vorbis library.
	 * \param _pfAcc Undocumented parameter from the Vorbis library.
	 * \param _i32I Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \param _pi32Out Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	float CVorbisPsy::noise_normalize( vorbis_look_psy * _plpP, int32_t i32Limit, float * _pfR, float * _pfQ, float * _pfF, int32_t * _pi32Flags, float _pfAcc, int32_t _i32I, int32_t _i32N, int32_t * _pi32Out ) {
		vorbis_info_psy * pipVi = _plpP->vi;
		float ** ppfSort = static_cast<float **>(::alloca( _i32N * sizeof( (*ppfSort) ) ));
		int32_t J, i32Count = 0;
		int32_t i32Start = (pipVi->normal_p ? pipVi->normal_start - _i32I : _i32N);
		if ( i32Start > _i32N ) { i32Start = _i32N; }

		// Force classic behavior where only energy in the current band is considered.
		_pfAcc = 0.0f;

		// Still responsible for populating *_pi32Out where noise norm not in
		//	effect.  There's no need to [re]populate *_pfQ in these areas.
		for ( J = 0; J < i32Start; J++ ) {
			if ( !_pi32Flags || !_pi32Flags[J] ) {	// Lossless coupling already quantized.
													//	Don't touch; requantizing based on
													//	energy would be incorrect.
				float fVe = _pfQ[J] / _pfF[J];
				if ( _pfR[J] < 0 ) {
					_pi32Out[J] = static_cast<int32_t>(-rint( ::sqrt( fVe ) ));
				}
				else {
					_pi32Out[J] = static_cast<int32_t>(rint( ::sqrt( fVe ) ));
				}
			}
		}

		// Sort magnitudes for noise norm portion of i32Partition.
		for ( ; J < _i32N; J++ ) {
			if ( !_pi32Flags || !_pi32Flags[J] ) {					// Can't noise norm elements that have
																	//	already been loslessly coupled; we can
																	//	only account for their energy error.
				float fVe = _pfQ[J] / _pfF[J];
				// Despite all the new, more capable coupling code, for now we
				//	implement noise norm as it has been up to this point. Only
				//	consider promotions to unit magnitude from 0.  In addition
				//	the only energy error counted is quantizations to zero.
				// Also, the original point code only applied noise norm at > pointlimit.
				if ( fVe < 0.25f && (!_pi32Flags || J >= i32Limit - _i32I) ) {
					_pfAcc += fVe;
					ppfSort[i32Count++] = _pfQ+J; // _pfQ is ::fabs( _pfR ) for unflagged element.
				}
				else {
					// For now: no acc adjustment for non-zero quantization. populate *_pi32Out and _pfQ as this value is final.
					if ( _pfR[J] < 0 ) {
						_pi32Out[J] = static_cast<int32_t>(-rint( ::sqrt( fVe ) ) );
					}
					else {
						_pi32Out[J] = static_cast<int32_t>(rint( ::sqrt( fVe ) ) );
					}
					_pfQ[J] = _pi32Out[J] * _pi32Out[J] * _pfF[J];
				}
			}
		}

		if ( i32Count ) {
			// Noise norm to do.
			CSearch::QSort( ppfSort, i32Count, sizeof( *ppfSort ), apsort, NULL );
			for ( J = 0; J < i32Count; J++ ) {
				uintptr_t K = ppfSort[J] - _pfQ;
				if ( _pfAcc >= pipVi->normal_thresh ) {
					_pi32Out[K] = static_cast<int32_t>(CVorbisConstants::unitnorm( _pfR[K] ));
					_pfAcc -= 1.0f;
					_pfQ[K] = _pfF[K];
				}
				else {
					_pi32Out[K] = 0;
					_pfQ[K] = 0.0f;
				}
			}
		}

		return _pfAcc;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvContext Unused.
	 * \param _pvA Undocumented parameter from the Vorbis library.
	 * \param _pvB Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t LSE_CCALL CVorbisPsy::apsort( void * /*_pvContext*/, const void * _pvA, const void * _pvB ) {
		float f1 = (*(*static_cast<const float * const *>(_pvA )));
		float f2 = (*(*static_cast<const float * const *>(_pvB )));
		return (f1 < f2) - (f1 > f2);
	}

}	// namespace lss

#pragma warning( pop )
