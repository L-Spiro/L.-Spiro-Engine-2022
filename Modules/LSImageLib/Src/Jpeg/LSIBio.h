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


#ifndef __LSI_BIO_H__
#define __LSI_BIO_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CBio
	 * \brief Implementation of an individual bit input-output (BIO).
	 *
	 * Description: Implementation of an individual bit input-output (BIO).
	 */
	class CBio {
	public :
		// == Types.
		/**
		 * Individual bit input-output stream (BIO).
		 */
		typedef struct opj_bio_t {
			/** Pointer to the start of the buffer. */
			uint8_t *							start;
			/** Pointer to the end of the buffer. */
			uint8_t *							end;
			/** Pointer to the present position in the buffer. */
			uint8_t *							bp;
			/** Temporary place where each byte is read or written. */
			uint32_t							buf;
			/** Coder: number of bits free to write. Decoder: number of bits read. */
			int32_t								ct;
		} * LPopj_bio_t, * const LPCopj_bio_t;

		
		// == Functions.
		/**
		 * Creates a new BIO handle.
		 *
		 * \return Returns a new BIO handle if successful, returns NULL otherwise.
		 */
		static opj_bio_t * LSE_CALL				bio_create();

		/**
		 * Destroy a previously created BIO handle.
		 *
		 * \param _pbBio BIO handle to destroy.
		 */
		static void LSE_CALL					bio_destroy( opj_bio_t *  _pbBio );

		/**
		 * Number of bytes written.
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns the number of bytes written.
		 */
		static int32_t LSE_CALL					bio_numbytes( opj_bio_t * _pbBio );

		/**
		 * Init encoder.
		 *
		 * \param _pbBio BIO handle.
		 * \param _pui8Bp Output buffer.
		 * \param _i32Len Output buffer length.
		 */
		static void LSE_CALL					bio_init_enc( opj_bio_t * _pbBio, uint8_t * _pui8Bp, int32_t _i32Len );

		/**
		 * Init decoder.
		 *
		 * \param _pbBio BIO handle.
		 * \param _pui8Bp Input buffer.
		 * \param _i32Len Input buffer length.
		 */
		static void LSE_CALL					bio_init_dec( opj_bio_t * _pbBio, uint8_t * _pui8Bp, int32_t _i32Len );

		/**
		 * Write bits.
		 *
		 * \param _pbBio BIO handle.
		 * \param _i32V Value of bits.
		 * \param _i32N Number of bits to write.
		 */
		static void LSE_CALL					bio_write( opj_bio_t * _pbBio, int32_t _i32V, int32_t _i32N );

		/**
		 * Read bits.
		 *
		 * \param _pbBio BIO handle.
		 * \param _i32N Number of bits to read.
		 * \return Returns the corresponding read number.
		 */
		static int32_t LSE_CALL					bio_read( opj_bio_t * _pbBio, int32_t _i32N );

		/**
		 * Flush bits.
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns 1 if successful, returns 0 otherwise.
		 */
		static int32_t LSE_CALL					bio_flush( opj_bio_t * _pbBio );

		/**
		 * Passes the ending bits (coming from flushing).
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns 1 if successful, returns 0 otherwise.
		 */
		static int32_t LSE_CALL					bio_inalign( opj_bio_t * _pbBio );


	protected :
		// == Functions.
		/**
		 * Write a bit.
		 *
		 * \param _pbBio BIO handle.
		 * \param _i32B Bit to write (0 or 1).
		 */
		static void LSE_CALL					bio_putbit( opj_bio_t *_pbBio, int32_t _i32B );

		/**
		 * Read a bit.
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns the read bit.
		 */
		static int32_t LSE_CALL					bio_getbit( opj_bio_t * _pbBio );

		/**
		 * Write a byte.
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns 0 if successful, returns 1 otherwise.
		 */
		static int32_t LSE_CALL					bio_byteout( opj_bio_t * _pbBio );

		/**
		 * Read a byte.
		 *
		 * \param _pbBio BIO handle.
		 * \return Returns 0 if successful, returns 1 otherwise.
		 */
		static int32_t LSE_CALL					bio_bytein( opj_bio_t * _pbBio );

	};

}	// namespace lsi

#endif	// __LSI_BIO_H__
