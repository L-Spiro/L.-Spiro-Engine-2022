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


#ifndef __LSI_RAW_H__
#define __LSI_RAW_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CRaw
	 * \brief Implementation of operations for raw encoding (RAW).
	 *
	 * Description: Implementation of operations for raw encoding (RAW).
	 */
	class CRaw {
	public :
		// == Types.
		/**
		 * RAW encoding operations
		 */
		typedef struct opj_raw {
			/** Temporary buffer where bits are coded or decoded. */
			uint8_t								c;
			/** Number of bits already read or free to write. */
			uint32_t							ct;
			/** Maximum length to decode. */
			uint32_t							lenmax;
			/** Length decoded. */
			uint32_t							len;
			/** Pointer to the current position in the buffer. */
			uint8_t *							bp;
			/** Pointer to the start of the buffer. */
			uint8_t *							start;
			/** Pointer to the end of the buffer. */
			uint8_t *							end;
		} opj_raw_t;


		// == Functions.
		/**
		 * Creates a new RAW handle.
		 *
		 * \return Returns a new RAW handle if successful, returns NULL otherwise.
		 */
		static opj_raw_t * LSE_CALL				raw_create();
		
		/**
		 * Destroy a previously created RAW handle.
		 *
		 * \param _prRaw RAW handle to destroy.
		 */
		static void LSE_CALL					raw_destroy( opj_raw_t * _prRaw );
		
		/**
		 * Return the number of bytes written/read since initialisation.
		 *
		 * \param _prRaw RAW handle to destroy.
		 * \return Returns the number of bytes already encoded.
		 */
		static int32_t LSE_CALL					raw_numbytes( opj_raw_t * _prRaw );
		
		/**
		 * Initialize the decoder.
		 *
		 * \param _prRaw RAW handle.
		 * \param _pui8Buffer Pointer to the start of the buffer from which the bytes will be read.
		 * \param _i32Len Length of the input buffer.
		 */
		static void LSE_CALL					raw_init_dec( opj_raw_t * _prRaw, uint8_t * _pui8Buffer, int32_t _i32Len );
		
		/**
		 * Decode a symbol using _prRaw-decoder. Cfr p.506 TAUBMAN.
		 *
		 * \param _prRaw RAW handle.
		 * \return Returns the decoded symbol (0 or 1).
		 */
		static int32_t LSE_CALL					raw_decode( opj_raw_t * _prRaw );


		// == Members.
	};

}	// namespace lsi

#endif	// __LSI_RAW_H__
