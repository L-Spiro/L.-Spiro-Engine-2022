/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
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

#include "LSIRaw.h"


namespace lsi {

	/**
	 * Creates a new RAW handle.
	 *
	 * \return Returns a new RAW handle if successful, returns NULL otherwise.
	 */
	CRaw::opj_raw_t * LSE_CALL CRaw::raw_create() {
		return static_cast<opj_raw_t *>(CMemLib::MAlloc( sizeof( opj_raw_t ) ));
	}
	
	/**
	 * Destroy a previously created RAW handle.
	 *
	 * \param _prRaw RAW handle to destroy.
	 */
	void LSE_CALL CRaw::raw_destroy( opj_raw_t * _prRaw ) {
		CMemLib::Free( _prRaw );
	}
	
	/**
	 * Return the number of bytes written/read since initialisation.
	 *
	 * \param _prRaw RAW handle to destroy.
	 * \return Returns the number of bytes already encoded.
	 */
	int32_t LSE_CALL CRaw::raw_numbytes( opj_raw_t * _prRaw ) {
		return static_cast<int32_t>(_prRaw->bp - _prRaw->start);
	}
	
	/**
	 * Initialize the decoder.
	 *
	 * \param _prRaw RAW handle.
	 * \param _pui8Buffer Pointer to the start of the buffer from which the bytes will be read.
	 * \param _i32Len Length of the input buffer.
	 */
	void LSE_CALL CRaw::raw_init_dec( opj_raw_t * _prRaw, uint8_t * _pui8Buffer, int32_t _i32Len ) {
		_prRaw->start = _pui8Buffer;
		_prRaw->lenmax = static_cast<uint32_t>(_i32Len);
		_prRaw->len = 0;
		_prRaw->c = 0;
		_prRaw->ct = 0;
	}
	
	/**
	 * Decode a symbol using _prRaw-decoder. Cfr p.506 TAUBMAN.
	 *
	 * \param _prRaw RAW handle.
	 * \return Returns the decoded symbol (0 or 1).
	 */
	int32_t LSE_CALL CRaw::raw_decode( opj_raw_t * _prRaw ) {
		int32_t i32D;
		if ( _prRaw->ct == 0 ) {
			_prRaw->ct = 8;
			if ( _prRaw->len == _prRaw->lenmax ) {
				_prRaw->c = 0xFF;
			}
			else {
				if ( _prRaw->c == 0xFF ) {
					_prRaw->ct = 7;
				}
				_prRaw->c = (*(_prRaw->start + _prRaw->len));
				_prRaw->len++;
			}
		}
		_prRaw->ct--;
		i32D = (_prRaw->c >> _prRaw->ct) & 0x01;
		
		return i32D;
	}

}	// namespace lsi
