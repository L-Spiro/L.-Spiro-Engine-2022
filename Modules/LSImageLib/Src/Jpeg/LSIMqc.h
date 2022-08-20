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


#ifndef __LSI_MQC_H__
#define __LSI_MQC_H__

#include "../LSIImageLib.h"
#include "LSIJpegCommonStructs.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define MQC_NUMCTXS								19
#define mqc_setcurctx( _pmMqc, _i32ContextNo )	(_pmMqc)->curctx = &(_pmMqc)->ctxs[static_cast<int32_t>(_i32ContextNo)]
#define MQC_PERF_OPT

namespace lsi {

	/**
	 * Class CMqc
	 * \brief Implementation of an MQ-Coder (MQC).
	 *
	 * Description: Implementation of an MQ-Coder (MQC).
	 */
	class CMqc {
	public :
		// == Types.
		/**
		 * This struct defines the state of a context.
		 */
		typedef struct opj_mqc_state_t {
			/** The probability of the Least Probable Symbol (0.75->0x8000, 1.5->0xFFFF). */
			uint32_t							qeval;
			/** The Most Probable Symbol (0 or 1). */
			int32_t								mps;
			/** Next state if the next encoded symbol is the MPS. */
			opj_mqc_state_t *					nmps;
			/** Next state if the next encoded symbol is the LPS. */
			opj_mqc_state_t *					nlps;
		} * LPopj_mqc_state_t, * const LPCopj_mqc_state_t;

		/**
		 * MQ coder
		 */
		typedef struct opj_mqc {
			uint32_t							c;
			uint32_t							a;
			uint32_t							ct;
			uint8_t *							bp;
			uint8_t *							start;
			uint8_t *							end;
			opj_mqc_state_t *					ctxs[MQC_NUMCTXS];
			opj_mqc_state_t **					curctx;
#ifdef MQC_PERF_OPT
			uint8_t *							buffer;
#endif	// #ifdef MQC_PERF_OPT
		} opj_mqc_t;


		// == Functions.
		/**
		 * Creates a new MQC handle.
		 *
		 * \return Returns a new MQC handle if successful, returns NULL otherwise.
		 */
		static opj_mqc_t * LSE_CALL				mqc_create();
		
		/**
		 * Destroy a previously created MQC handle.
		 *
		 * \param _pmMqc MQC handle to destroy.
		 */
		static void LSE_CALL					mqc_destroy( opj_mqc_t * _pmMqc );
		
		/**
		 * Return the number of bytes written/read since initialisation.
		 *
		 * \param _pmMqc MQC handle.
		 * \return Returns the number of bytes already encoded.
		 */
		static int32_t LSE_CALL					mqc_numbytes( opj_mqc_t * _pmMqc );
		
		/**
		 * Reset the states of all the context of the coder/decoder
		 *	(each context is set to a state where 0 and 1 are more or less equiprobable).
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_resetstates( opj_mqc_t * _pmMqc );
		
		/**
		 * Sets the state of a particular context.
		 *
		 * \param _pmMqc MQC handle.
		 * \param _i32ContextNo Number that identifies the context.
		 * \param _i32Msb The MSB of the new state of the context.
		 * \param _i32Prob Number that identifies the probability of the symbols for the new state of the context.
		 */
		static void LSE_CALL					mqc_setstate( opj_mqc_t * _pmMqc, int32_t _i32ContextNo, int32_t _i32Msb, int32_t _i32Prob );
		
		/**
		 * Initialize the encoder.
		 *
		 * \param _pmMqc MQC handle.
		 * \param _pui8Buffer Pointer to the start of the buffer where the bytes will be written.
		 */
		static void LSE_CALL					mqc_init_enc( opj_mqc_t * _pmMqc, uint8_t * _pui8Buffer );
		
		/**
		 * Encode a symbol using the MQ-coder.
		 *
		 * \param _pmMqc MQC handle.
		 * \param _i32D The symbol to be encoded (0 or 1).
		 */
		static void LSE_CALL					mqc_encode( opj_mqc_t * _pmMqc, int32_t _i32D );
		
		/**
		 * Flush the encoder, so that all remaining data is written.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_flush( opj_mqc_t * _pmMqc );
		
		/**
		 * BYPASS mode switch, initialization operation.
		 * JPEG 2000 p 505. 
		 * <h2>Not fully implemented and tested!</h2>.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_bypass_init_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * BYPASS mode switch, coding operation.
		 * JPEG 2000 p 505.
		 * <h2>Not fully implemented and tested!</h2>.
		 *
		 * \param _pmMqc MQC handle.
		 * \param _i32D The symbol to be encoded (0 or 1).
		 */
		static void LSE_CALL					mqc_bypass_enc( opj_mqc_t * _pmMqc, int32_t _i32D );
		
		/**
		 * BYPASS mode switch, flush operation.
		 * <h2>Not fully implemented and tested!</h2>.
		 *
		 * \param _pmMqc MQC handle.
		 * \return Returns 1 (always).
		 */
		static int32_t LSE_CALL					mqc_bypass_flush_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * RESET mode switch.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_reset_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * RESTART mode switch (TERMALL).
		 *
		 * \param _pmMqc MQC handle.
		 * \return Returns 1 (always).
		 */
		static int32_t LSE_CALL					mqc_restart_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * RESTART mode switch (TERMALL) reinitialisation.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_restart_init_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * ERTERM mode switch (PTERM).
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_erterm_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * SEGMARK mode switch (SEGSYM).
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_segmark_enc( opj_mqc_t * _pmMqc );
		
		/**
		 * Initialize the decoder.
		 *
		 * \param _pmMqc MQC handle.
		 * \param _pui8Buffer Pointer to the start of the buffer from which the bytes will be read.
		 * \param _i32Len Length of the input buffer.
		 */
		static void LSE_CALL					mqc_init_dec( opj_mqc_t * _pmMqc, uint8_t * _pui8Buffer, int32_t _i32Len );
		
		/**
		 * Decode a symbol.
		 *
		 * \param _pmMqc MQC handle.
		 * \return Returns the decoded symbol (0 or 1).
		 */
		static int32_t LSE_CALL					mqc_decode( opj_mqc_t * const _pmMqc );


	protected :
		// == Members.
		/**
		 * This array defines all the possible states for a context.
		 */
		static opj_mqc_state_t					mqc_states[47*2];


		// == Functions.
		/**
		 * Output a byte, doing bit-stuffing if necessary.
		 * After a 0xFF byte, the next byte must be smaller than 0x90.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_byteout( opj_mqc_t * _pmMqc );
		
		/**
		 * Renormalize _pmMqc->a and _pmMqc->c while encoding, so that _pmMqc->a stays between 0x8000 and 0x10000.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_renorme( opj_mqc_t * _pmMqc );
		
		/**
		 * Encode the most probable symbol.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_codemps( opj_mqc_t * _pmMqc );
		
		/**
		 * Encode the most least symbol.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_codelps( opj_mqc_t * _pmMqc );
		
		/**
		 * Fill _pmMqc->c with 1's for flushing.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_setbits( opj_mqc_t * _pmMqc );
		
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pmMqc MQC handle.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL					mqc_mpsexchange( opj_mqc_t * const _pmMqc );
		
		/**
		 * Undocumented function from the OpenJpeg Library.
		 *
		 * \param _pmMqc MQC handle.
		 * \return Undocumented return from the OpenJpeg Library.
		 */
		static int32_t LSE_CALL					mqc_lpsexchange( opj_mqc_t * const _pmMqc );
		
		/**
		 * Input a byte.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_bytein( opj_mqc_t * const _pmMqc );
		
		/**
		 * Renormalize _pmMqc->a and _pmMqc->c while decoding.
		 *
		 * \param _pmMqc MQC handle.
		 */
		static void LSE_CALL					mqc_renormd( opj_mqc_t * const _pmMqc );
	};

}	// namespace lsi

#endif	// __LSI_MQC_H__
