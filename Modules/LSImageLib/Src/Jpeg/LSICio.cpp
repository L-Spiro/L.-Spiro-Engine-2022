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

#include "LSICio.h"
#include "LSIEvent.h"
#include "LSIJ2k.h"
#include "LSIJp2.h"


namespace lsi {

	// == Functions.
	/**
	 * Open and allocate a memory stream for read/write.
	 * On reading, the user must provide a buffer containing encoded data. The buffer will be
	 * wrapped by the returned CIO handle.
	 * On writing, buffer parameters must be set to 0: a buffer will be allocated by the library
	 * to contain encoded data.
	 *
	 * \param _cpInfoC Codec context info.
	 * \param _pui8Buffer Reading: buffer address. Writing: NULL.
	 * \param _i32Length Reading: buffer length. Writing: 0.
	 * \return Returns a CIO handle if successful, returns NULL otherwise.
	 */
	CCio::opj_cio_t * LSE_CALL CCio::opj_cio_open( CJpegCommonStructs::opj_common_ptr _cpInfoC, uint8_t * _pui8Buffer, int32_t _i32Length ) {
		CJ2k::opj_cp_t * pcCp = NULL;
		CCio::opj_cio_t * pcCio = static_cast<opj_cio_t *>(CMemLib::MAlloc( sizeof( opj_cio_t ) ));
		if ( !pcCio ) { return NULL; }
		pcCio->cinfo = _cpInfoC;
		if ( _pui8Buffer && _i32Length ) {
			// Wrap a user buffer containing the encoded image.
			pcCio->openmode = OPJ_STREAM_READ;
			pcCio->buffer = _pui8Buffer;
			pcCio->length = _i32Length;
		}
		else if ( !_pui8Buffer && !_i32Length && _cpInfoC ) {
			// Allocate a buffer for the encoded image.
			pcCio->openmode = OPJ_STREAM_WRITE;
			switch( _cpInfoC->codec_format ) {
				case CODEC_J2K : {
					pcCp = static_cast<CJ2k::opj_j2k_t *>(_cpInfoC->j2k_handle)->cp;
					break;
				}
				case CODEC_JP2 : {
					pcCp = static_cast<CJp2::opj_jp2_t *>(_cpInfoC->jp2_handle)->j2k->cp;
					break;
				}
				default : {
					CMemLib::Free( pcCio );
					return NULL;
				}
			}
			pcCio->length = static_cast<int32_t>((0.1625 * pcCp->img_size + 2000)); // 0.1625 = 1.3/8 and 2000 bytes as a minimum for headers.
			pcCio->buffer = static_cast<uint8_t *>(CMemLib::MAlloc( static_cast<LSA_SIZE>(pcCio->length) ));
			if ( !pcCio->buffer ) {
				CEvent::opj_event_msg( pcCio->cinfo, EVT_ERROR, "Error allocating memory for compressed bitstream.\r\n" );
				CMemLib::Free( pcCio );
				return NULL;
			}
		}
		else {
			CMemLib::Free( pcCio );
			return NULL;
		}

		// Initialize byte IO.
		pcCio->start = pcCio->buffer;
		pcCio->end = pcCio->buffer + pcCio->length;
		pcCio->bp = pcCio->buffer;

		return pcCio;
	}

	/**
	 * Close and free a CIO handle.
	 *
	 * \param _pcCio CIO handle to free.
	 */
	void LSE_CALL CCio::opj_cio_close( opj_cio_t * _pcCio ) {
		if ( _pcCio ) {
			if ( _pcCio->openmode == OPJ_STREAM_WRITE ) {
				// Destroy the allocated buffer.
				CMemLib::Free( _pcCio->buffer );
			}
			// Destroy the CIO.
			CMemLib::Free( _pcCio );
		}
	}

	/**
	 * Gets the position in given byte stream.
	 *
	 * \param _pcCio CIO handle.
	 * \return Returns the position in bytes.
	 */
	int32_t LSE_CALL CCio::cio_tell( opj_cio_t * _pcCio ) {
		return static_cast<int32_t>(_pcCio->bp - _pcCio->start);
	}

	/**
	 * Sets the position in the given byte stream.
	 *
	 * \param _pcCio CIO handle.
	 * \param _i32Pos Position, in number of bytes, from the beginning of the stream.
	 */
	void LSE_CALL CCio::cio_seek( opj_cio_t * _pcCio, int32_t _i32Pos ) {
		_pcCio->bp = _pcCio->start + _i32Pos;
	}

	/**
	 * Number of bytes left before the end of the stream.
	 *
	 * \param _pcCio CIO handle.
	 * \return Returns the number of bytes before the end of the stream.
	 */
	int32_t LSE_CALL CCio::cio_numbytesleft( opj_cio_t * _pcCio ) {
		return static_cast<int32_t>(_pcCio->end - _pcCio->bp);
	}

	/**
	 * Gets pointer to the current position in the stream.
	 *
	 * \param _pcCio CIO handle.
	 * \return Returns a pointer to the current position.
	 */
	uint8_t * LSE_CALL CCio::cio_getbp( opj_cio_t * _pcCio ) {
		return _pcCio->bp;
	}

	/**
	 * Write some bytes to the given stream.
	 *
	 * \param _pcCio CIO handle.
	 * \param _ui32V Value to write.
	 * \param _ui32N Number of bytes to write.
	 * \return Returns the number of bytes written or 0 if an error occured.
	 */
	uint32_t LSE_CALL CCio::cio_write( opj_cio_t * _pcCio, uint32_t _ui32V, uint32_t _ui32N ) {
		for ( uint32_t I = _ui32N; I--; ) {
			if ( !cio_byteout( _pcCio, static_cast<uint8_t>((_ui32V >> (I << 3)) & 0xFF) ) ) { return 0; }
		}
		return _ui32N;
	}
	
	/**
	 * Write some bytes to the given stream.
	 *
	 * \param _pcCio CIO handle.
	 * \param _i32V Value to write.
	 * \param _ui32N Number of bytes to write.
	 * \return Returns the number of bytes written or 0 if an error occured.
	 */
	uint32_t LSE_CALL CCio::cio_write( opj_cio_t * _pcCio, int32_t _i32V, uint32_t _ui32N ) {
		for ( uint32_t I = _ui32N; I--; ) {
			if ( !cio_byteout( _pcCio, static_cast<uint8_t>((_i32V >> (I << 3)) & 0xFF) ) ) { return 0; }
		}
		return _ui32N;
	}

	/**
	 * Read some bytes from the given stream.
	 *
	 * \param _pcCio CIO handle.
	 * \param _ui32N Number of bytes to read.
	 * \return Returns the value of the _i32N bytes read.
	 */
	uint32_t LSE_CALL CCio::cio_read( opj_cio_t * _pcCio, uint32_t _ui32N ) {
		uint32_t I;
		uint32_t ui32V;
		ui32V = 0;
		for ( I = _ui32N; --I; ) {
			ui32V += static_cast<uint32_t>(cio_bytein( _pcCio ) << (I << 3));
		}
		return ui32V;
	}

	/**
	 * Skip some bytes.
	 *
	 * \param _pcCio CIO handle.
	 * \param _i32N Number of bytes to skip.
	 */
	void LSE_CALL CCio::cio_skip( opj_cio_t * _pcCio, int32_t _i32N ) {
		_pcCio->bp += _i32N;
	}

	/*
	 * Write a byte.
	 *
	 * \param _pcCio CIO handle.
	 * \param _ui8V Value to write.
	 */
	LSBOOL LSE_CALL CCio::cio_byteout( opj_cio_t * _pcCio, uint8_t _ui8V ) {
		if ( _pcCio->bp >= _pcCio->end ) {
			CEvent::opj_event_msg( _pcCio->cinfo, EVT_ERROR, "Write error.\r\n" );
			return false;
		}
		(*_pcCio->bp++) = _ui8V;
		return true;
	}

	/*
	 * Read a byte.
	 *
	 * \param _pcCio CIO handle.
	 */
	uint8_t LSE_CALL CCio::cio_bytein( opj_cio_t * _pcCio ) {
		if ( _pcCio->bp >= _pcCio->end ) {
			CEvent::opj_event_msg( _pcCio->cinfo, EVT_ERROR, "Read error: Passed the end of the codestream (start = %d, current = %d, end = %d).\r\n", _pcCio->start, _pcCio->bp, _pcCio->end );
			return 0;
		}
		return (*_pcCio->bp++);
	}

}	// namespace lsi
