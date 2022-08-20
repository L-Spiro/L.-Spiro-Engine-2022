/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
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

#include "LSIMqc.h"
#include "LSIT1.h"


namespace lsi {

	// == Members.
	/**
	 * This array defines all the possible states for a context.
	 */
	CMqc::opj_mqc_state_t CMqc::mqc_states[47*2] = {
		{ 0x5601, 0, &mqc_states[2], &mqc_states[3] },
		{ 0x5601, 1, &mqc_states[3], &mqc_states[2] },
		{ 0x3401, 0, &mqc_states[4], &mqc_states[12] },
		{ 0x3401, 1, &mqc_states[5], &mqc_states[13] },
		{ 0x1801, 0, &mqc_states[6], &mqc_states[18] },
		{ 0x1801, 1, &mqc_states[7], &mqc_states[19] },
		{ 0x0AC1, 0, &mqc_states[8], &mqc_states[24] },
		{ 0x0AC1, 1, &mqc_states[9], &mqc_states[25] },
		{ 0x0521, 0, &mqc_states[10], &mqc_states[58] },
		{ 0x0521, 1, &mqc_states[11], &mqc_states[59] },
		{ 0x0221, 0, &mqc_states[76], &mqc_states[66] },
		{ 0x0221, 1, &mqc_states[77], &mqc_states[67] },
		{ 0x5601, 0, &mqc_states[14], &mqc_states[13] },
		{ 0x5601, 1, &mqc_states[15], &mqc_states[12] },
		{ 0x5401, 0, &mqc_states[16], &mqc_states[28] },
		{ 0x5401, 1, &mqc_states[17], &mqc_states[29] },
		{ 0x4801, 0, &mqc_states[18], &mqc_states[28] },
		{ 0x4801, 1, &mqc_states[19], &mqc_states[29] },
		{ 0x3801, 0, &mqc_states[20], &mqc_states[28] },
		{ 0x3801, 1, &mqc_states[21], &mqc_states[29] },
		{ 0x3001, 0, &mqc_states[22], &mqc_states[34] },
		{ 0x3001, 1, &mqc_states[23], &mqc_states[35] },
		{ 0x2401, 0, &mqc_states[24], &mqc_states[36] },
		{ 0x2401, 1, &mqc_states[25], &mqc_states[37] },
		{ 0x1C01, 0, &mqc_states[26], &mqc_states[40] },
		{ 0x1C01, 1, &mqc_states[27], &mqc_states[41] },
		{ 0x1601, 0, &mqc_states[58], &mqc_states[42] },
		{ 0x1601, 1, &mqc_states[59], &mqc_states[43] },
		{ 0x5601, 0, &mqc_states[30], &mqc_states[29] },
		{ 0x5601, 1, &mqc_states[31], &mqc_states[28] },
		{ 0x5401, 0, &mqc_states[32], &mqc_states[28] },
		{ 0x5401, 1, &mqc_states[33], &mqc_states[29] },
		{ 0x5101, 0, &mqc_states[34], &mqc_states[30] },
		{ 0x5101, 1, &mqc_states[35], &mqc_states[31] },
		{ 0x4801, 0, &mqc_states[36], &mqc_states[32] },
		{ 0x4801, 1, &mqc_states[37], &mqc_states[33] },
		{ 0x3801, 0, &mqc_states[38], &mqc_states[34] },
		{ 0x3801, 1, &mqc_states[39], &mqc_states[35] },
		{ 0x3401, 0, &mqc_states[40], &mqc_states[36] },
		{ 0x3401, 1, &mqc_states[41], &mqc_states[37] },
		{ 0x3001, 0, &mqc_states[42], &mqc_states[38] },
		{ 0x3001, 1, &mqc_states[43], &mqc_states[39] },
		{ 0x2801, 0, &mqc_states[44], &mqc_states[38] },
		{ 0x2801, 1, &mqc_states[45], &mqc_states[39] },
		{ 0x2401, 0, &mqc_states[46], &mqc_states[40] },
		{ 0x2401, 1, &mqc_states[47], &mqc_states[41] },
		{ 0x2201, 0, &mqc_states[48], &mqc_states[42] },
		{ 0x2201, 1, &mqc_states[49], &mqc_states[43] },
		{ 0x1C01, 0, &mqc_states[50], &mqc_states[44] },
		{ 0x1C01, 1, &mqc_states[51], &mqc_states[45] },
		{ 0x1801, 0, &mqc_states[52], &mqc_states[46] },
		{ 0x1801, 1, &mqc_states[53], &mqc_states[47] },
		{ 0x1601, 0, &mqc_states[54], &mqc_states[48] },
		{ 0x1601, 1, &mqc_states[55], &mqc_states[49] },
		{ 0x1401, 0, &mqc_states[56], &mqc_states[50] },
		{ 0x1401, 1, &mqc_states[57], &mqc_states[51] },
		{ 0x1201, 0, &mqc_states[58], &mqc_states[52] },
		{ 0x1201, 1, &mqc_states[59], &mqc_states[53] },
		{ 0x1101, 0, &mqc_states[60], &mqc_states[54] },
		{ 0x1101, 1, &mqc_states[61], &mqc_states[55] },
		{ 0x0AC1, 0, &mqc_states[62], &mqc_states[56] },
		{ 0x0AC1, 1, &mqc_states[63], &mqc_states[57] },
		{ 0x09C1, 0, &mqc_states[64], &mqc_states[58] },
		{ 0x09C1, 1, &mqc_states[65], &mqc_states[59] },
		{ 0x08A1, 0, &mqc_states[66], &mqc_states[60] },
		{ 0x08A1, 1, &mqc_states[67], &mqc_states[61] },
		{ 0x0521, 0, &mqc_states[68], &mqc_states[62] },
		{ 0x0521, 1, &mqc_states[69], &mqc_states[63] },
		{ 0x0441, 0, &mqc_states[70], &mqc_states[64] },
		{ 0x0441, 1, &mqc_states[71], &mqc_states[65] },
		{ 0x02A1, 0, &mqc_states[72], &mqc_states[66] },
		{ 0x02A1, 1, &mqc_states[73], &mqc_states[67] },
		{ 0x0221, 0, &mqc_states[74], &mqc_states[68] },
		{ 0x0221, 1, &mqc_states[75], &mqc_states[69] },
		{ 0x0141, 0, &mqc_states[76], &mqc_states[70] },
		{ 0x0141, 1, &mqc_states[77], &mqc_states[71] },
		{ 0x0111, 0, &mqc_states[78], &mqc_states[72] },
		{ 0x0111, 1, &mqc_states[79], &mqc_states[73] },
		{ 0x0085, 0, &mqc_states[80], &mqc_states[74] },
		{ 0x0085, 1, &mqc_states[81], &mqc_states[75] },
		{ 0x0049, 0, &mqc_states[82], &mqc_states[76] },
		{ 0x0049, 1, &mqc_states[83], &mqc_states[77] },
		{ 0x0025, 0, &mqc_states[84], &mqc_states[78] },
		{ 0x0025, 1, &mqc_states[85], &mqc_states[79] },
		{ 0x0015, 0, &mqc_states[86], &mqc_states[80] },
		{ 0x0015, 1, &mqc_states[87], &mqc_states[81] },
		{ 0x0009, 0, &mqc_states[88], &mqc_states[82] },
		{ 0x0009, 1, &mqc_states[89], &mqc_states[83] },
		{ 0x0005, 0, &mqc_states[90], &mqc_states[84] },
		{ 0x0005, 1, &mqc_states[91], &mqc_states[85] },
		{ 0x0001, 0, &mqc_states[90], &mqc_states[86] },
		{ 0x0001, 1, &mqc_states[91], &mqc_states[87] },
		{ 0x5601, 0, &mqc_states[92], &mqc_states[92] },
		{ 0x5601, 1, &mqc_states[93], &mqc_states[93] },
	};

	// == Functions.
	/**
	 * Creates a new MQC handle.
	 *
	 * \return Returns a new MQC handle if successful, returns NULL otherwise.
	 */
	CMqc::opj_mqc_t * LSE_CALL CMqc::mqc_create() {
		opj_mqc_t * pmMqc = static_cast<opj_mqc_t *>(CMemLib::MAlloc( sizeof( opj_mqc_t ) ));
#ifdef MQC_PERF_OPT
		pmMqc->buffer = NULL;
#endif	// #ifdef MQC_PERF_OPT
		return pmMqc;
	}

	/**
	 * Destroy a previously created MQC handle.
	 *
	 * \param _pmMqc MQC handle to destroy.
	 */
	void LSE_CALL CMqc::mqc_destroy( opj_mqc_t * _pmMqc ) {
		if ( _pmMqc ) {
#ifdef MQC_PERF_OPT
			if ( _pmMqc->buffer ) {
				CMemLib::Free( _pmMqc->buffer );
			}
#endif	// #ifdef MQC_PERF_OPT
			CMemLib::Free( _pmMqc );
		}
	}

	/**
	 * Return the number of bytes written/read since initialisation.
	 *
	 * \param _pmMqc MQC handle.
	 * \return Returns the number of bytes already encoded.
	 */
	int32_t LSE_CALL CMqc::mqc_numbytes( opj_mqc_t * _pmMqc ) {
		return static_cast<int32_t>(_pmMqc->bp - _pmMqc->start);
	}

	/**
	 * Reset the states of all the context of the coder/decoder
	 *	(each context is set to a state where 0 and 1 are more or less equiprobable).
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_resetstates( opj_mqc_t * _pmMqc ) {
		for ( int32_t I = 0; I < MQC_NUMCTXS; I++ ) {
			_pmMqc->ctxs[I] = mqc_states;
		}
	}

	/**
	 * Sets the state of a particular context.
	 *
	 * \param _pmMqc MQC handle.
	 * \param _i32ContextNo Number that identifies the context.
	 * \param _i32Msb The MSB of the new state of the context.
	 * \param _i32Prob Number that identifies the probability of the symbols for the new state of the context.
	 */
	void LSE_CALL CMqc::mqc_setstate( opj_mqc_t * _pmMqc, int32_t _i32ContextNo, int32_t _i32Msb, int32_t _i32Prob ) {
		_pmMqc->ctxs[_i32ContextNo] = &mqc_states[_i32Msb+(_i32Prob<<1)];
	}

	/**
	 * Initialize the encoder.
	 *
	 * \param _pmMqc MQC handle.
	 * \param _pui8Buffer Pointer to the start of the buffer where the bytes will be written.
	 */
	void LSE_CALL CMqc::mqc_init_enc( opj_mqc_t * _pmMqc, uint8_t * _pui8Buffer ) {
		mqc_setcurctx( _pmMqc, 0 );
		_pmMqc->a = 0x8000;
		_pmMqc->c = 0;
		_pmMqc->bp = _pui8Buffer - 1;
		_pmMqc->ct = 12;
		if ( (*_pmMqc->bp) == 0xFF ) {
			_pmMqc->ct = 13;
		}
		_pmMqc->start = _pui8Buffer;
	}

	/**
	 * Encode a symbol using the MQ-coder.
	 *
	 * \param _pmMqc MQC handle.
	 * \param _i32D The symbol to be encoded (0 or 1).
	 */
	void LSE_CALL CMqc::mqc_encode( opj_mqc_t * _pmMqc, int32_t _i32D ) {
		if ( (*_pmMqc->curctx)->mps == _i32D ) {
			mqc_codemps( _pmMqc );
		}
		else {
			mqc_codelps( _pmMqc );
		}
	}

	/**
	 * Flush the encoder, so that all remaining data is written.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_flush( opj_mqc_t * _pmMqc ) {
		mqc_setbits( _pmMqc );
		_pmMqc->c <<= _pmMqc->ct;
		mqc_byteout( _pmMqc );
		_pmMqc->c <<= _pmMqc->ct;
		mqc_byteout( _pmMqc );
		
		if ( (*_pmMqc->bp) != 0xFF ) { _pmMqc->bp++; }
	}

	/**
	 * BYPASS mode switch, initialization operation.
	 * JPEG 2000 p 505. 
	 * <h2>Not fully implemented and tested!</h2>.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_bypass_init_enc( opj_mqc_t * _pmMqc ) {
		_pmMqc->c = 0;
		_pmMqc->ct = 8;
	}

	/**
	 * BYPASS mode switch, coding operation.
	 * JPEG 2000 p 505.
	 * <h2>Not fully implemented and tested!</h2>.
	 *
	 * \param _pmMqc MQC handle.
	 * \param _i32D The symbol to be encoded (0 or 1).
	 */
	void LSE_CALL CMqc::mqc_bypass_enc( opj_mqc_t * _pmMqc, int32_t _i32D ) {
		_pmMqc->ct--;
		_pmMqc->c = _pmMqc->c + (static_cast<uint32_t>(_i32D) << _pmMqc->ct);
		if ( _pmMqc->ct == 0 ) {
			_pmMqc->bp++;
			(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c);
			_pmMqc->ct = 8;
			if ( (*_pmMqc->bp) == 0xFF ) { _pmMqc->ct = 7; }
			_pmMqc->c = 0;
		}
	}

	/**
	 * BYPASS mode switch, flush operation.
	 * <h2>Not fully implemented and tested!</h2>.
	 *
	 * \param _pmMqc MQC handle.
	 * \return Returns 1 (always).
	 */
	int32_t LSE_CALL CMqc::mqc_bypass_flush_enc( opj_mqc_t * _pmMqc ) {
		uint8_t ui8BitPadding;
		
		ui8BitPadding = 0;
		
		if ( _pmMqc->ct != 0 ) {
			while ( _pmMqc->ct > 0 ) {
				_pmMqc->ct--;
				_pmMqc->c += static_cast<uint32_t>(ui8BitPadding) << _pmMqc->ct;
				ui8BitPadding = (ui8BitPadding + 1) & 0x01;
			}
			_pmMqc->bp++;
			(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c);
			_pmMqc->ct = 8;
			_pmMqc->c = 0;
		}
		
		return 1;
	}

	/**
	 * RESET mode switch.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_reset_enc( opj_mqc_t * _pmMqc ) {
		mqc_resetstates( _pmMqc );
		mqc_setstate( _pmMqc, T1_CTXNO_UNI, 0, 46 );
		mqc_setstate( _pmMqc, T1_CTXNO_AGG, 0, 3 );
		mqc_setstate( _pmMqc, T1_CTXNO_ZC, 0, 4 );
	}

	/**
	 * RESTART mode switch (TERMALL).
	 *
	 * \param _pmMqc MQC handle.
	 * \return Returns 1 (always).
	 */
	int32_t LSE_CALL CMqc::mqc_restart_enc( opj_mqc_t * _pmMqc ) {
		int32_t i32Correction = 1;
		
		int32_t i32N = 27 - 15 - static_cast<int32_t>(_pmMqc->ct);
		_pmMqc->c <<= _pmMqc->ct;
		while ( i32N > 0 ) {
			mqc_byteout( _pmMqc );
			i32N -= _pmMqc->ct;
			_pmMqc->c <<= _pmMqc->ct;
		}
		mqc_byteout( _pmMqc );
		
		return i32Correction;
	}

	/**
	 * RESTART mode switch (TERMALL) reinitialisation.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_restart_init_enc( opj_mqc_t * _pmMqc ) {
		mqc_setcurctx( _pmMqc, 0 );
		_pmMqc->a = 0x8000;
		_pmMqc->c = 0;
		_pmMqc->ct = 12;
		_pmMqc->bp--;
		if ( (*_pmMqc->bp) == 0xFF ) {
			_pmMqc->ct = 13;
		}
	}

	/**
	 * ERTERM mode switch (PTERM).
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_erterm_enc( opj_mqc_t * _pmMqc ) {
		int32_t i32K = 11 - static_cast<int32_t>(_pmMqc->ct) + 1;
		
		while ( i32K > 0 ) {
			_pmMqc->c <<= _pmMqc->ct;
			_pmMqc->ct = 0;
			mqc_byteout( _pmMqc );
			i32K -= _pmMqc->ct;
		}
		
		if ( (*_pmMqc->bp) != 0xFF ) {
			mqc_byteout( _pmMqc );
		}
	}

	/**
	 * SEGMARK mode switch (SEGSYM).
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_segmark_enc( opj_mqc_t * _pmMqc ) {
		mqc_setcurctx( _pmMqc, 18 );
		
		for ( int32_t I = 1; I < 5; I++ ) {
			mqc_encode( _pmMqc, I % 2 );
		}
	}

	/**
	 * Initialize the decoder.
	 *
	 * \param _pmMqc MQC handle.
	 * \param _pui8Buffer Pointer to the start of the buffer from which the bytes will be read.
	 * \param _i32Len Length of the input buffer.
	 */
	void LSE_CALL CMqc::mqc_init_dec( opj_mqc_t * _pmMqc, uint8_t * _pui8Buffer, int32_t _i32Len ) {
		mqc_setcurctx( _pmMqc, 0 );
		_pmMqc->start = _pui8Buffer;
		_pmMqc->end = _pui8Buffer + _i32Len;
		_pmMqc->bp = _pui8Buffer;
		if ( _i32Len == 0 ) { _pmMqc->c = 0xFF << 16; }
		else { _pmMqc->c = static_cast<uint32_t>((*_pmMqc->bp) << 16); }

#ifdef MQC_PERF_OPT
		{
			uint32_t ui32C;
			uint32_t * pui32Ip;
			uint8_t * pui8End = _pmMqc->end - 1;
			_pmMqc->buffer = static_cast<uint8_t *>(CMemLib::MReAlloc( _pmMqc->buffer, static_cast<uint32_t>(2 * _i32Len + 1) * sizeof( uint32_t ) ));
			pui32Ip = reinterpret_cast<uint32_t *>(_pmMqc->buffer);

			while ( _pui8Buffer != pui8End ) {
				ui32C = (*(_pui8Buffer + 1));
				if ( (*_pui8Buffer) == 0xFF ) {
					if ( ui32C > 0x8F ) {
						(*pui32Ip) = 0x0000FF18;
					}
					else {
						_pui8Buffer++;
						(*pui32Ip) = 0x00000017 | (ui32C << 9);
					}
				}
				else {
					_pui8Buffer++;
					(*pui32Ip) = 0x00000018 | (ui32C << 8);
				}
				pui32Ip++;
			}

			// Handle last byte of data.
			ui32C = 0xFF;
			if ( (*_pui8Buffer) == 0xFF ) {
				(*pui32Ip) = 0x0000FF18;
			}
			else {
				_pui8Buffer++;
				(*pui32Ip) = 0x00000018 | (ui32C << 8);
			}
			pui32Ip++;

			(*pui32Ip) = 0x0000FF08;
			_pmMqc->bp = _pmMqc->buffer;
		}
#endif	// #ifdef MQC_PERF_OPT
		mqc_bytein( _pmMqc );
		_pmMqc->c <<= 7;
		_pmMqc->ct -= 7;
		_pmMqc->a = 0x8000;
	}

	/**
	 * Decode a symbol.
	 *
	 * \param _pmMqc MQC handle.
	 * \return Returns the decoded symbol (0 or 1).
	 */
	int32_t LSE_CALL CMqc::mqc_decode( opj_mqc_t * const _pmMqc ) {
		int32_t i32D;
		_pmMqc->a -= (*_pmMqc->curctx)->qeval;
		if ( (_pmMqc->c >> 16) < (*_pmMqc->curctx)->qeval ) {
			i32D = mqc_lpsexchange( _pmMqc );
			mqc_renormd( _pmMqc );
		}
		else {
			_pmMqc->c -= (*_pmMqc->curctx)->qeval << 16;
			if ( (_pmMqc->a & 0x8000) == 0 ) {
				i32D = mqc_mpsexchange( _pmMqc );
				mqc_renormd( _pmMqc );
			}
			else {
				i32D = (*_pmMqc->curctx)->mps;
			}
		}

		return i32D;
	}

	/**
	 * Output a byte, doing bit-stuffing if necessary.
	 * After a 0xFF byte, the next byte must be smaller than 0x90.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_byteout( opj_mqc_t * _pmMqc ) {
		if ( (*_pmMqc->bp) == 0xFF ) {
			_pmMqc->bp++;
			(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c >> 20);
			_pmMqc->c &= 0xFFFFF;
			_pmMqc->ct = 7;
		}
		else {
			if ( (_pmMqc->c & 0x8000000) == 0 ) {
				_pmMqc->bp++;
				(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c >> 19);
				_pmMqc->c &= 0x7FFFF;
				_pmMqc->ct = 8;
			}
			else {
				(*_pmMqc->bp)++;
				if ( (*_pmMqc->bp) == 0xFF ) {
					_pmMqc->c &= 0x7FFFFFF;
					_pmMqc->bp++;
					(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c >> 20);
					_pmMqc->c &= 0xFFFFF;
					_pmMqc->ct = 7;
				}
				else {
					_pmMqc->bp++;
					(*_pmMqc->bp) = static_cast<uint8_t>(_pmMqc->c >> 19);
					_pmMqc->c &= 0x7FFFF;
					_pmMqc->ct = 8;
				}
			}
		}
	}

	/**
	 * Renormalize _pmMqc->a and _pmMqc->c while encoding, so that _pmMqc->a stays between 0x8000 and 0x10000.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_renorme( opj_mqc_t * _pmMqc ) {
		do {
			_pmMqc->a <<= 1;
			_pmMqc->c <<= 1;
			_pmMqc->ct--;
			if ( _pmMqc->ct == 0 ) {
				mqc_byteout( _pmMqc );
			}
		} while ( (_pmMqc->a & 0x8000) == 0 );
	}

	/**
	 * Encode the most probable symbol.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_codemps( opj_mqc_t * _pmMqc ) {
		_pmMqc->a -= (*_pmMqc->curctx)->qeval;
		if ( ( _pmMqc->a & 0x8000 ) == 0 ) {
			if ( _pmMqc->a < (*_pmMqc->curctx)->qeval ) {
				_pmMqc->a = (*_pmMqc->curctx)->qeval;
			}
			else {
				_pmMqc->c += (*_pmMqc->curctx)->qeval;
			}
			(*_pmMqc->curctx) = (*_pmMqc->curctx)->nmps;
			mqc_renorme( _pmMqc );
		}
		else {
			_pmMqc->c += (*_pmMqc->curctx)->qeval;
		}
	}

	/**
	 * Encode the most least symbol.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_codelps( opj_mqc_t * _pmMqc ) {
		_pmMqc->a -= (*_pmMqc->curctx)->qeval;
		if ( _pmMqc->a < (*_pmMqc->curctx)->qeval ) {
			_pmMqc->c += (*_pmMqc->curctx)->qeval;
		}
		else {
			_pmMqc->a = (*_pmMqc->curctx)->qeval;
		}
		(*_pmMqc->curctx) = (*_pmMqc->curctx)->nlps;
		mqc_renorme( _pmMqc );
	}

	/**
	 * Fill _pmMqc->c with 1's for flushing.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_setbits( opj_mqc_t * _pmMqc ) {
		uint32_t ui32Temp = _pmMqc->c + _pmMqc->a;
		_pmMqc->c |= 0xFFFF;
		if ( _pmMqc->c >= ui32Temp ) {
			_pmMqc->c -= 0x8000;
		}
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pmMqc MQC handle.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CMqc::mqc_mpsexchange( opj_mqc_t * const _pmMqc ) {
		int32_t i32D;
		if ( _pmMqc->a < (*_pmMqc->curctx)->qeval ) {
			i32D = 1 - (*_pmMqc->curctx)->mps;
			(*_pmMqc->curctx) = (*_pmMqc->curctx)->nlps;
		}
		else {
			i32D = (*_pmMqc->curctx)->mps;
			(*_pmMqc->curctx) = (*_pmMqc->curctx)->nmps;
		}
		
		return i32D;
	}

	/**
	 * Undocumented function from the OpenJpeg Library.
	 *
	 * \param _pmMqc MQC handle.
	 * \return Undocumented return from the OpenJpeg Library.
	 */
	int32_t LSE_CALL CMqc::mqc_lpsexchange( opj_mqc_t * const _pmMqc ) {
		int32_t i32D;
		if ( _pmMqc->a < (*_pmMqc->curctx)->qeval ) {
			_pmMqc->a = (*_pmMqc->curctx)->qeval;
			i32D = (*_pmMqc->curctx)->mps;
			(*_pmMqc->curctx) = (*_pmMqc->curctx)->nmps;
		}
		else {
			_pmMqc->a = (*_pmMqc->curctx)->qeval;
			i32D = 1 - (*_pmMqc->curctx)->mps;
			(*_pmMqc->curctx) = (*_pmMqc->curctx)->nlps;
		}
		
		return i32D;
	}

	/**
	 * Input a byte.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_bytein( opj_mqc_t * const _pmMqc ) {
#ifdef MQC_PERF_OPT
		uint32_t ui32I = (*reinterpret_cast<uint32_t *>(_pmMqc->bp));
		_pmMqc->c += ui32I & 0xFFFF00;
		_pmMqc->ct = ui32I & 0x0F;
		_pmMqc->bp += (ui32I >> 2) & 0x04;
#else
		if ( _pmMqc->bp != _pmMqc->end ) {
			uint32_t ui32C;
			if ( _pmMqc->bp + 1 != _pmMqc->end ) {
				ui32C = (*(_pmMqc->bp + 1));
			}
			else {
				ui32C = 0xFF;
			}
			if ( (*_pmMqc->bp) == 0xFF ) {
				if ( ui32C > 0x8F ) {
					_pmMqc->c += 0xFF00;
					_pmMqc->ct = 8;
				}
				else {
					_pmMqc->bp++;
					_pmMqc->c += ui32C << 9;
					_pmMqc->ct = 7;
				}
			}
			else {
				_pmMqc->bp++;
				_pmMqc->c += ui32C << 8;
				_pmMqc->ct = 8;
			}
		}
		else {
			_pmMqc->c += 0xFF00;
			_pmMqc->ct = 8;
		}
#endif	// #ifdef MQC_PERF_OPT
	}

	/**
	 * Renormalize _pmMqc->a and _pmMqc->c while decoding.
	 *
	 * \param _pmMqc MQC handle.
	 */
	void LSE_CALL CMqc::mqc_renormd( opj_mqc_t * const _pmMqc ) {
		do {
			if ( _pmMqc->ct == 0 ) {
				mqc_bytein( _pmMqc );
			}
			_pmMqc->a <<= 1;
			_pmMqc->c <<= 1;
			_pmMqc->ct--;
		} while ( _pmMqc->a < 0x8000 );
	}

}	// namespace lsi
