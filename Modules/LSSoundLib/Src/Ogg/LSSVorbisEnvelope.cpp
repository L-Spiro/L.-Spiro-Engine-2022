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

 function: PCM data envelope analysis and manipulation
 i32Last mod: $Id: envelope.h 16227 2009-07-08 06:58:46Z xiphmont $

 ********************************************************************/

#include "LSSVorbisEnvelope.h"
#include "LSSVorbisCodecInternal.h"


namespace lss {

	// == Functions.
	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pelE Undocumented parameter from the Vorbis library.
	 * \param _piVi Undocumented parameter from the Vorbis library.
	 */
	void CVorbisEnvelope::_ve_envelope_init( envelope_lookup * _pelE, CVorbisCodec::vorbis_info * _piVi ) {
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(_piVi->codec_setup);
		CVorbisPsy::vorbis_info_psy_global * pvipgGi = &pcsiCi->psy_g_param;
		int32_t i32Ch = _piVi->channels;
		int32_t I, J;
		int32_t i32N = _pelE->winlength = 128;
		_pelE->searchstep = 64;				// Not random.

		_pelE->minenergy = pvipgGi->preecho_minenergy;
		_pelE->ch = i32Ch;
		_pelE->storage = 128;
		_pelE->cursor = pcsiCi->blocksizes[1] / 2;
		_pelE->mdct_win = static_cast<float *>(CMemLib::CMAlloc( i32N * sizeof( (*_pelE->mdct_win) ) ));
		CVorbisMdct::mdct_init( &_pelE->mdct, i32N );

		for ( I = 0; I < i32N; I++ ) {
			_pelE->mdct_win[I] = static_cast<float>(::sin( I / (i32N - 1.0f) * LSM_PI_DOUBLE ));
			_pelE->mdct_win[I] *= _pelE->mdct_win[I];
		}

		// Magic follows.
		_pelE->band[0].begin = 2;  _pelE->band[0].end = 4;
		_pelE->band[1].begin = 4;  _pelE->band[1].end = 5;
		_pelE->band[2].begin = 6;  _pelE->band[2].end = 6;
		_pelE->band[3].begin = 9;  _pelE->band[3].end = 8;
		_pelE->band[4].begin = 13;  _pelE->band[4].end = 8;
		_pelE->band[5].begin = 17;  _pelE->band[5].end = 8;
		_pelE->band[6].begin = 22;  _pelE->band[6].end = 8;

		for ( J = 0; J < VE_BANDS; J++ ) {
			i32N = _pelE->band[J].end;
			_pelE->band[J].window = static_cast<float *>(CMemLib::MAlloc( i32N * sizeof( (*_pelE->band[0].window) ) ));
			for ( I = 0; I < i32N; I++ ) {
				_pelE->band[J].window[I] = static_cast<float>(::sin( (I + 0.5) / i32N * LSM_PI_DOUBLE ));
				_pelE->band[J].total += _pelE->band[J].window[I];
			}
			_pelE->band[J].total = 1.0f / _pelE->band[J].total;
		}

		_pelE->filter = static_cast<envelope_filter_state *>(CMemLib::CMAlloc( VE_BANDS * i32Ch * sizeof( (*_pelE->filter) ) ));
		_pelE->mark = static_cast<int32_t *>(CMemLib::CMAlloc( _pelE->storage * sizeof( (*_pelE->mark) ) ));
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pelE Undocumented parameter from the Vorbis library.
	 */
	void CVorbisEnvelope::_ve_envelope_clear( envelope_lookup * _pelE ) {
		int32_t I;
		CVorbisMdct::mdct_clear( &_pelE->mdct );
		for ( I = 0; I < VE_BANDS; I++ ) {
			CMemLib::Free( _pelE->band[I].window );
		}
		CMemLib::Free( _pelE->mdct_win );
		CMemLib::Free( _pelE->filter );
		CMemLib::Free( _pelE->mark );
		CStd::MemSet( _pelE, 0, sizeof( (*_pelE) ) );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisEnvelope::_ve_envelope_search( CVorbisCodec::vorbis_dsp_state * _pvdsV ) {
		CVorbisCodec::vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		CVorbisPsy::vorbis_info_psy_global * pvipgGi = &pcsiCi->psy_g_param;
		envelope_lookup * pelVe = (static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state))->ve;
		int32_t I, J;

		int32_t i32First = pelVe->current / pelVe->searchstep;
		int32_t i32Last = _pvdsV->pcm_current / pelVe->searchstep - VE_WIN;
		if ( i32First < 0 ) { i32First = 0; }

		/* make sure we have enough storage to match the PCM */
		if ( i32Last + VE_WIN + VE_POST > pelVe->storage ) {
			pelVe->storage = i32Last + VE_WIN + VE_POST;
			pelVe->mark = static_cast<int32_t *>(CMemLib::MReAlloc( pelVe->mark, pelVe->storage * sizeof( *pelVe->mark ) ));
		}

		for ( J = i32First; J < i32Last; J++ ) {
			int32_t i32Ret = 0;

			pelVe->stretch++;
			if ( pelVe->stretch > VE_MAXSTRETCH * 2 )
				pelVe->stretch = VE_MAXSTRETCH * 2;

			for ( I = 0; I < pelVe->ch; I++ ) {
				float * pfPcm = _pvdsV->pcm[I] + pelVe->searchstep * J;
				i32Ret |= _ve_amp( pelVe, pvipgGi, pfPcm, pelVe->band, pelVe->filter + I * VE_BANDS );
			}

			pelVe->mark[J+VE_POST] = 0;
			if ( i32Ret & 1 ) {
				pelVe->mark[J] = 1;
				pelVe->mark[J+1] = 1;
			}

			if ( i32Ret & 2 ) {
				pelVe->mark[J] = 1;
				if ( J > 0 ) { pelVe->mark[J-1] = 1; }
			}

			if ( i32Ret & 4 ) { pelVe->stretch = -1; }
		}

		pelVe->current = i32Last * pelVe->searchstep;

		{
			int32_t i32CenterW = _pvdsV->centerW;
			int32_t i32TestW = 
				i32CenterW +
				pcsiCi->blocksizes[_pvdsV->W] / 4 +
				pcsiCi->blocksizes[1] / 2 +
				pcsiCi->blocksizes[0] / 4;

			J = pelVe->cursor;

			while ( J<pelVe->current-( pelVe->searchstep ) ) {
				if ( J >= i32TestW ) { return 1; }

				pelVe->cursor = J;

				if ( pelVe->mark[J/pelVe->searchstep] ) {
					if ( J>i32CenterW ) {
						pelVe->curmark = J;
						if ( J >= i32TestW ) { return 1; }
						return 0;
					}
				}
				J += pelVe->searchstep;
			}
		}

		return -1;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pelE Undocumented parameter from the Vorbis library.
	 * \param _i32Shift Undocumented parameter from the Vorbis library.
	 */
	void CVorbisEnvelope::_ve_envelope_shift( envelope_lookup * _pelE, int32_t _i32Shift ) {
		int32_t i32SmallSize = _pelE->current/_pelE->searchstep+VE_POST; // Adjust for placing marks ahead of pelVe->current.
		int32_t i32SmallShift = _i32Shift/_pelE->searchstep;

		CStd::MemMove( _pelE->mark, _pelE->mark + i32SmallShift, (i32SmallSize - i32SmallShift) * sizeof( (*_pelE->mark) ) );


		_pelE->current -= _i32Shift;
		if ( _pelE->curmark >= 0 ) {
			_pelE->curmark -= _i32Shift;
		}
		_pelE->cursor -= _i32Shift;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvdsV Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisEnvelope::_ve_envelope_mark( CVorbisCodec::vorbis_dsp_state * _pvdsV ) {
		envelope_lookup * pelVe = (static_cast<CVorbisCodecInternal::private_state *>(_pvdsV->backend_state))->ve;
		CVorbisCodec::vorbis_info * pviVi = _pvdsV->vi;
		CVorbisCodecInternal::codec_setup_info * pcsiCi = static_cast<CVorbisCodecInternal::codec_setup_info *>(pviVi->codec_setup);
		int32_t i32CenterW = _pvdsV->centerW;
		int32_t i32BeginW = i32CenterW-pcsiCi->blocksizes[_pvdsV->W] / 4;
		int32_t i32EndW = i32CenterW+pcsiCi->blocksizes[_pvdsV->W] / 4;
		if ( _pvdsV->W ) {
			i32BeginW -= pcsiCi->blocksizes[_pvdsV->lW] / 4;
			i32EndW += pcsiCi->blocksizes[_pvdsV->nW] / 4;
		}
		else {
			i32BeginW -= pcsiCi->blocksizes[0] / 4;
			i32EndW += pcsiCi->blocksizes[0] / 4;
		}

		if ( pelVe->curmark >= i32BeginW && pelVe->curmark<i32EndW ) { return 1; }
		{
			int32_t i32First = i32BeginW / pelVe->searchstep;
			int32_t i32Last = i32EndW / pelVe->searchstep;
			int32_t I;
			for ( I = i32First; I < i32Last; I++ ) {
				if ( pelVe->mark[I] ) { return 1; }
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pelVe Undocumented parameter from the Vorbis library.
	 * \param _pvisgGi Undocumented parameter from the Vorbis library.
	 * \param _pfData Undocumented parameter from the Vorbis library.
	 * \param _pebBands Undocumented parameter from the Vorbis library.
	 * \param _pefsFilters Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisEnvelope::_ve_amp( envelope_lookup * _pelVe, 
		CVorbisPsy::vorbis_info_psy_global * _pvisgGi, 
		float * _pfData, 
		envelope_band * _pebBands, 
		envelope_filter_state * _pefsFilters ) {
		int32_t i32N = _pelVe->winlength;
		int32_t i32Ret = 0;
		int32_t I, J;
		float fDecay;

		// We want to have a "minimum bar" for energy, or else we're just
		//	basing blocks on quantization noise that outweighs the signal
		//	itself (for low power signals).

		float fMinV = _pelVe->minenergy;
		float * pfVec = static_cast<float *>(::alloca( i32N * sizeof( (*pfVec) ) ));

		// Stretch is used to gradually lengthen the number of windows
		//	considered prevoius-to-potential-trigger.
		int32_t i32Stretch = CStd::Max<int32_t>(VE_MINSTRETCH, _pelVe->stretch/2 );
		float fPenalty = _pvisgGi->stretch_penalty-( _pelVe->stretch/2-VE_MINSTRETCH );
		if ( fPenalty < 0.0f ) { fPenalty = 0.0f; }
		if ( fPenalty > _pvisgGi->stretch_penalty ) { fPenalty = _pvisgGi->stretch_penalty; }

		/*_analysis_output_always( "lpcm", seq2, _pfData, i32N, 0, 0, 
			totalshift+pos*_pelVe->searchstep );*/

		// Window and transform.
		for ( I = 0; I < i32N; I++ ) {
			pfVec[I] = _pfData[I] * _pelVe->mdct_win[I];
		}
		CVorbisMdct::mdct_forward( &_pelVe->mdct, pfVec, pfVec );

		/*_analysis_output_always( "mdct", seq2, pfVec, i32N/2, 0, 1, 0 ); */

		// Near-DC spreading function; this has nothing to do with
		//	psychoacoustics, just sidelobe leakage and window size.
		{
			float fTemp = pfVec[0]*pfVec[0]+0.7f*pfVec[1]*pfVec[1]+0.2f*pfVec[2]*pfVec[2];
			int32_t i32Ptr = _pefsFilters->nearptr;

			// The accumulation is regularly refreshed from scratch to avoid
			//	floating point creep.
			if ( i32Ptr == 0 ) {
				fDecay = _pefsFilters->nearDC_acc = _pefsFilters->nearDC_partialacc + fTemp;
				_pefsFilters->nearDC_partialacc = fTemp;
			}
			else {
				fDecay = _pefsFilters->nearDC_acc += fTemp;
				_pefsFilters->nearDC_partialacc += fTemp;
			}
			_pefsFilters->nearDC_acc -= _pefsFilters->nearDC[i32Ptr];
			_pefsFilters->nearDC[i32Ptr] = fTemp;

			fDecay *= (1.0f / (VE_NEARDC + 1));
			_pefsFilters->nearptr++;
			if ( _pefsFilters->nearptr >= VE_NEARDC ) { _pefsFilters->nearptr = 0; }
			fDecay = CVorbisConstants::todB( &fDecay ) * 0.5f - 15.0f;
		}

		// Perform spreading and limiting, also smooth the spectrum.  Yes, 
		//	the MDCT results in all real coefficients, but it still behaves
		//	like real/imaginary pairs.
		for ( I = 0; I < i32N / 2; I += 2 ) {
			float fVal = pfVec[I] * pfVec[I] + pfVec[I+1] * pfVec[I+1];
			fVal = CVorbisConstants::todB( &fVal ) * 0.5f;
			if ( fVal < fDecay ) { fVal = fDecay; }
			if ( fVal < fMinV ) { fVal = fMinV; }
			pfVec[I>>1] = fVal;
			fDecay -= 8.0f;
		}

		/*_analysis_output_always( "spread", seq2++, pfVec, i32N/4, 0, 0, 0 );*/

		// Perform preecho/postecho triggering by band.
		for ( J = 0; J < VE_BANDS; J++ ) {
			float fAcc = 0.0f;
			float fValMax, fValMin;

			// Accumulate amplitude.
			for ( I = 0; I < _pebBands[J].end; I++ ) {
				fAcc += pfVec[I+_pebBands[J].begin]*_pebBands[J].window[I];
			}

			fAcc *= _pebBands[J].total;

			// Convert amplitude to delta.
			{
				int32_t i32P, i32This = _pefsFilters[J].ampptr;
				float fPostMax, fPostMin, fPreMax = -99999.0f, fPreMin = 99999.0f;

				i32P = i32This;
				i32P--;
				if ( i32P < 0 ) { i32P += VE_AMP; }
				fPostMax = CStd::Max( fAcc, _pefsFilters[J].ampbuf[i32P] );
				fPostMin = CStd::Min( fAcc, _pefsFilters[J].ampbuf[i32P] );

				for ( I = 0; I < i32Stretch; I++ ) {
					i32P--;
					if ( i32P < 0 ) { i32P += VE_AMP; }
					fPreMax = CStd::Max( fPreMax, _pefsFilters[J].ampbuf[i32P] );
					fPreMin = CStd::Min( fPreMin, _pefsFilters[J].ampbuf[i32P] );
				}

				fValMin = fPostMin - fPreMin;
				fValMax = fPostMax - fPreMax;

				/*_pefsFilters[J].markers[pos] = fValMax;*/
				_pefsFilters[J].ampbuf[i32This] = fAcc;
				_pefsFilters[J].ampptr++;
				if ( _pefsFilters[J].ampptr >= VE_AMP ) { _pefsFilters[J].ampptr = 0; }
			}

			// Look at min/max, decide trigger.
			if ( fValMax > _pvisgGi->preecho_thresh[J] + fPenalty ) {
				i32Ret |= 1;
				i32Ret |= 4;
			}
			if ( fValMin < _pvisgGi->postecho_thresh[J] - fPenalty ) { i32Ret |= 2; }
		}

		return i32Ret;
	}

}	// namespace lss
