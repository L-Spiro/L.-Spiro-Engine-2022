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

#ifndef __LSI_CIO_H__
#define __LSI_CIO_H__

#include "../LSIImageLib.h"
#include "LSIJpegCommonStructs.h"

namespace lsi {

	/**
	 * Class CCio
	 * \brief Implementation of a byte input-output process (CIO).
	 *
	 * Description: Implementation of a byte input-output process (CIO).
	 */
	class CCio {
	public :
		// == Types.
		/**
		 * Byte input-output stream (CIO).
		 */
		typedef struct opj_cio {
			/** Codec context. */
			CJpegCommonStructs::opj_common_ptr						cinfo;

			/** Open mode (read/write) either OPJ_STREAM_READ or OPJ_STREAM_WRITE. */
			int32_t													openmode;
			/** Pointer to the start of the buffer. */
			uint8_t *												buffer;
			/** Buffer size in bytes. */
			int32_t													length;

			/** Pointer to the start of the stream. */
			uint8_t *												start;
			/** Pointer to the end of the stream. */
			uint8_t *												end;
			/** Pointer to the current position. */
			uint8_t *												bp;
		} opj_cio_t;

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
		static opj_cio_t * LSE_CALL									opj_cio_open( CJpegCommonStructs::opj_common_ptr _cpInfoC, uint8_t * _pui8Buffer, int32_t _i32Length );

		/**
		 * Close and free a CIO handle.
		 *
		 * \param _pcCio CIO handle to free.
		 */
		static void LSE_CALL										opj_cio_close( opj_cio_t * _pcCio );

		/**
		 * Gets the position in given byte stream.
		 *
		 * \param _pcCio CIO handle.
		 * \return Returns the position in bytes.
		 */
		static int32_t LSE_CALL										cio_tell( opj_cio_t * _pcCio );

		/**
		 * Sets the position in the given byte stream.
		 *
		 * \param _pcCio CIO handle.
		 * \param _i32Pos Position, in number of bytes, from the beginning of the stream.
		 */
		static void LSE_CALL										cio_seek( opj_cio_t * _pcCio, int32_t _i32Pos );
		
		/**
		 * Number of bytes left before the end of the stream.
		 *
		 * \param _pcCio CIO handle.
		 * \return Returns the number of bytes before the end of the stream.
		 */
		static int32_t LSE_CALL										cio_numbytesleft( opj_cio_t * _pcCio );
		
		/**
		 * Gets pointer to the current position in the stream.
		 *
		 * \param _pcCio CIO handle.
		 * \return Returns a pointer to the current position.
		 */
		static uint8_t * LSE_CALL									cio_getbp( opj_cio_t * _pcCio );
		
		/**
		 * Write some bytes to the given stream.
		 *
		 * \param _pcCio CIO handle.
		 * \param _ui32V Value to write.
		 * \param _ui32N Number of bytes to write.
		 * \return Returns the number of bytes written or 0 if an error occured.
		 */
		static uint32_t LSE_CALL									cio_write( opj_cio_t * _pcCio, uint32_t _i32V, uint32_t _ui32N );
		
		/**
		 * Write some bytes to the given stream.
		 *
		 * \param _pcCio CIO handle.
		 * \param _i32V Value to write.
		 * \param _ui32N Number of bytes to write.
		 * \return Returns the number of bytes written or 0 if an error occured.
		 */
		static uint32_t LSE_CALL									cio_write( opj_cio_t * _pcCio, int32_t _i32V, uint32_t _ui32N );
		
		/**
		 * Read some bytes from the given stream.
		 *
		 * \param _pcCio CIO handle.
		 * \param _ui32N Number of bytes to read.
		 * \return Returns the value of the _i32N bytes read.
		 */
		static uint32_t LSE_CALL									cio_read( opj_cio_t * _pcCio, uint32_t _ui32N );
		
		/**
		 * Skip some bytes.
		 *
		 * \param _pcCio CIO handle.
		 * \param _i32N Number of bytes to skip.
		 */
		static void LSE_CALL										cio_skip( opj_cio_t * _pcCio, int32_t _i32N );


	protected :
		/*
		 * Write a byte.
		 *
		 * \param _pcCio CIO handle.
		 * \param _ui8V Value to write.
		 */
		static LSBOOL LSE_CALL										cio_byteout( opj_cio_t * _pcCio, uint8_t _ui8V );

		/*
		 * Read a byte.
		 *
		 * \param _pcCio CIO handle.
		 */
		static uint8_t LSE_CALL										cio_bytein( opj_cio_t * _pcCio );
	};

}	// namespace lsi

#endif	// __LSI_CIO_H__
