/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2002-2003, Yannick Verschueren
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


#ifndef __LSI_JPT_H__
#define __LSI_JPT_H__

#include "../LSIImageLib.h"
#include "LSICio.h"
#include "LSIJpegCommonStructs.h"

namespace lsi {

	/**
	 * Class CJpt
	 * \brief JPT-stream reader (JPEG 2000, JPIP).
	 *
	 * Description: JPT-stream reader (JPEG 2000, JPIP).
	 */
	class CJpt {
	public :
		// == Types.
		/**
		 * Message Header JPT stream structure
		 */
		typedef struct opj_jpt_msg_header_t {
			/** In-class Identifier. */
			uint32_t							Id;
			/** Last byte information. */
			uint32_t							last_byte;	
			/** Class Identifier. */
			uint32_t							Class_Id;	
			/** CSn: index identifier. */
			uint32_t							CSn_Id;
			/** Message offset. */
			uint32_t							Msg_offset;
			/** Message length. */
			uint32_t							Msg_length;
			/** Auxiliary for JPP case. */
			uint32_t							Layer_nb;
		} * LPopj_jpt_msg_header_t, * const LPCopj_jpt_msg_header_t;


		// == Functions.
		/**
		 * Initialize the value of the message header structure.
		 *
		 * \param _pjmhHeader Message header structure.
		 */
		static void LSE_CALL					jpt_init_msg_header( opj_jpt_msg_header_t * _pjmhHeader );

		/**
		 * Read the message header for a JPP/JPT - stream.
		 *
		 * \param _cpInfo Codec context info.
		 * \param _pcCio CIO handle.
		 * \param _pjmhHeader Message header structure.
		 */
		static void LSE_CALL					jpt_read_msg_header( CJpegCommonStructs::opj_common_ptr _cpInfo, CCio::opj_cio_t * _pcCio, opj_jpt_msg_header_t * _pjmhHeader );


	protected :
		// == Functions.
		/**
		 * Read the information contains in VBAS [JPP/JPT stream message header].
		 * Store information (7 bits) in value.
		 *
		 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
		 * \param _ui32Value Undocumented parameter from the OpenJpeg Library.
		 */
		static uint32_t LSE_CALL				ReadVbasInfo( CCio::opj_cio_t * _pcCio, uint32_t _ui32Value );

		/**
		 * Re-initialize the value of the message header structure.
		 * Only parameters always present in message header.
		 *
		 * \param _pjmhHeader Undocumented parameter from the OpenJpeg Library.
		 */
		static void LSE_CALL					ReInitMsgHeader( opj_jpt_msg_header_t * _pjmhHeader );

	};

}	// namespace lsi

#endif	// __LSI_JPT_H__
