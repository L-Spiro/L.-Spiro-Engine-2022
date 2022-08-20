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

#include "LSIBio.h"


namespace lsi {

	// == Functions.
	/**
	 * Creates a new BIO handle.
	 *
	 * \return Returns a new BIO handle if successful, returns NULL otherwise.
	 */
	CBio::opj_bio_t * LSE_CALL CBio::bio_create() {
		opj_bio_t * pbBio = static_cast<opj_bio_t *>(CMemLib::MAlloc( sizeof( opj_bio_t ) ));
		return pbBio;
	}

	/**
	 * Destroy a previously created BIO handle.
	 *
	 * \param _pbBio BIO handle to destroy.
	 */
	void LSE_CALL CBio::bio_destroy( opj_bio_t *  _pbBio ) {
		if ( _pbBio ) {
			CMemLib::Free( _pbBio );
		}
	}

	/**
	 * Number of bytes written.
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns the number of bytes written.
	 */
	int32_t LSE_CALL CBio::bio_numbytes( opj_bio_t * _pbBio ) {
		return static_cast<int32_t>(_pbBio->bp - _pbBio->start);
	}

	/**
	 * Init encoder.
	 *
	 * \param _pbBio BIO handle.
	 * \param _pui8Bp Output buffer.
	 * \param _i32Len Output buffer length.
	 */
	void LSE_CALL CBio::bio_init_enc( opj_bio_t * _pbBio, uint8_t * _pui8Bp, int32_t _i32Len ) {
		_pbBio->start = _pui8Bp;
		_pbBio->end = _pui8Bp + _i32Len;
		_pbBio->bp = _pui8Bp;
		_pbBio->buf = 0;
		_pbBio->ct = 8;
	}

	/**
	 * Init decoder.
	 *
	 * \param _pbBio BIO handle.
	 * \param _pui8Bp Input buffer.
	 * \param _i32Len Input buffer length.
	 */
	void LSE_CALL CBio::bio_init_dec( opj_bio_t * _pbBio, uint8_t * _pui8Bp, int32_t _i32Len ) {
		_pbBio->start = _pui8Bp;
		_pbBio->end = _pui8Bp + _i32Len;
		_pbBio->bp = _pui8Bp;
		_pbBio->buf = 0;
		_pbBio->ct = 0;
	}

	/**
	 * Write bits.
	 *
	 * \param _pbBio BIO handle.
	 * \param _i32V Value of bits.
	 * \param _i32N Number of bits to write.
	 */
	void LSE_CALL CBio::bio_write( opj_bio_t * _pbBio, int32_t _i32V, int32_t _i32N ) {
		int32_t I;
		for ( I = _i32N - 1; I >= 0; I-- ) {
			bio_putbit( _pbBio, (_i32V >> I) & 1 );
		}
	}

	/**
	 * Read bits.
	 *
	 * \param _pbBio BIO handle.
	 * \param _i32N Number of bits to read.
	 * \return Returns the corresponding read number.
	 */
	int32_t LSE_CALL CBio::bio_read( opj_bio_t * _pbBio, int32_t _i32N ) {
		int32_t I, i32V;
		i32V = 0;
		for ( I = _i32N - 1; I >= 0; I-- ) {
			i32V += bio_getbit( _pbBio ) << I;
		}
		return i32V;
	}

	/**
	 * Flush bits.
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns 1 if successful, returns 0 otherwise.
	 */
	int32_t LSE_CALL CBio::bio_flush( opj_bio_t * _pbBio ) {
		_pbBio->ct = 0;
		if ( bio_byteout( _pbBio ) ) {
			return 1;
		}
		if ( _pbBio->ct == 7 ) {
			_pbBio->ct = 0;
			if ( bio_byteout( _pbBio ) ) {
				return 1;
			}
		}
		return 0;
	}

	/**
	 * Passes the ending bits (coming from flushing).
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns 1 if successful, returns 0 otherwise.
	 */
	int32_t LSE_CALL CBio::bio_inalign( opj_bio_t * _pbBio ) {
		_pbBio->ct = 0;
		if ( (_pbBio->buf & 0xFF) == 0xFF ) {
			if ( bio_bytein( _pbBio ) ) {
				return 1;
			}
			_pbBio->ct = 0;
		}
		return 0;
	}

	/**
	 * Write a bit.
	 *
	 * \param _pbBio BIO handle.
	 * \param _i32B Bit to write (0 or 1).
	 */
	void LSE_CALL CBio::bio_putbit( opj_bio_t *_pbBio, int32_t _i32B ) {
		if ( _pbBio->ct == 0 ) {
			bio_byteout(_pbBio);
		}
		_pbBio->ct--;
		_pbBio->buf |= static_cast<uint32_t>(_i32B << _pbBio->ct);
	}

	/**
	 * Read a bit.
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns the read bit.
	 */
	int32_t LSE_CALL CBio::bio_getbit( opj_bio_t * _pbBio ) {
		if ( _pbBio->ct == 0 ) {
			bio_bytein(_pbBio);
		}
		_pbBio->ct--;
		return (_pbBio->buf >> _pbBio->ct) & 1;
	}

	/**
	 * Write a byte.
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns 0 if successful, returns 1 otherwise.
	 */
	int32_t LSE_CALL CBio::bio_byteout( opj_bio_t * _pbBio ) {
		_pbBio->buf = (_pbBio->buf << 8) & 0xFFFF;
		_pbBio->ct = _pbBio->buf == 0xFF00 ? 7 : 8;
		if ( _pbBio->bp >= _pbBio->end ) { return 1; }
		(*_pbBio->bp++) = static_cast<uint8_t>(_pbBio->buf >> 8);
		return 0;
	}

	/**
	 * Read a byte.
	 *
	 * \param _pbBio BIO handle.
	 * \return Returns 0 if successful, returns 1 otherwise.
	 */
	int32_t LSE_CALL CBio::bio_bytein( opj_bio_t * _pbBio ) {
		_pbBio->buf = (_pbBio->buf << 8) & 0xFFFF;
		_pbBio->ct = _pbBio->buf == 0xFF00 ? 7 : 8;
		if ( _pbBio->bp >= _pbBio->end ) { return 1; }
		_pbBio->buf |= (*_pbBio->bp++);
		return 0;
	}

}	// namespace lsi
