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

#include "LSIJpt.h"
#include "LSIEvent.h"


namespace lsi {

	// == Functions.
	/**
	 * Initialize the value of the message header structure.
	 *
	 * \param _pjmhHeader Message header structure.
	 */
	void LSE_CALL CJpt::jpt_init_msg_header( opj_jpt_msg_header_t * _pjmhHeader ) {
		_pjmhHeader->Id = 0;			// In-class Identifier.
		_pjmhHeader->last_byte = 0;		// Last byte information.
		_pjmhHeader->Class_Id = 0;		// Class Identifier.
		_pjmhHeader->CSn_Id = 0;		// CSn: index identifier.
		_pjmhHeader->Msg_offset = 0;	// Message offset.
		_pjmhHeader->Msg_length = 0;	// Message length.
		_pjmhHeader->Layer_nb = 0;		// Auxiliary for JPP case.
	}

	/**
	 * Read the message header for a JPP/JPT - stream.
	 *
	 * \param _cpInfo Codec context info.
	 * \param _pcCio CIO handle.
	 * \param _pjmhHeader Message header structure.
	 */
	void LSE_CALL CJpt::jpt_read_msg_header( CJpegCommonStructs::opj_common_ptr _cpInfo, CCio::opj_cio_t * _pcCio, opj_jpt_msg_header_t * _pjmhHeader ) {
		uint8_t ui8Element, ui8Class = 0, ui8CSn = 0;
		ReInitMsgHeader( _pjmhHeader );

		/* ------------ */
		/* VBAS: Bin-ID */
		/* ------------ */
		ui8Element = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 ));

		// See for ui8Class and ui8CSn.
		switch ( (ui8Element >> 5) & 0x03 ) {
			case 0 : {
				CEvent::opj_event_msg( _cpInfo, EVT_ERROR, "Forbidden value encountered in message header!\r\n" );
				break;
			}
			case 1 : {
				ui8Class = 0;
				ui8CSn = 0;
				break;
			}
			case 2 : {
				ui8Class = 1;
				ui8CSn = 0;
				break;
			}
			case 3 : {
				ui8Class = 1;
				ui8CSn = 1;
				break;
			}
			default : {
				break;
			}
		}

		// See information on bits 'c' [p 10 : A.2.1 general, ISO/IEC FCD 15444-9].
		if ( ((ui8Element >> 4) & 0x01) == 1 ) {
			_pjmhHeader->last_byte = 1;
		}


		// In-class identifier.
		_pjmhHeader->Id |= (ui8Element & 0x0F);
		if ( (ui8Element >> 7) == 1 ) {
			_pjmhHeader->Id = ReadVbasInfo( _pcCio, _pjmhHeader->Id );
		}


		/* ----------- */
		/* VBAS: Class */
		/* ----------- */
		if ( ui8Class == 1 ) {
			_pjmhHeader->Class_Id = 0;
			_pjmhHeader->Class_Id = ReadVbasInfo( _pcCio, _pjmhHeader->Class_Id );
		}

		/* --------- */
		/* VBAS: CSn */
		/* --------- */
		if ( ui8CSn == 1 ) {
			_pjmhHeader->CSn_Id = 0;
			_pjmhHeader->CSn_Id = ReadVbasInfo( _pcCio, _pjmhHeader->CSn_Id );
		}

		/* ---------------- */
		/* VBAS: Msg_offset */
		/* ---------------- */
		_pjmhHeader->Msg_offset = ReadVbasInfo( _pcCio, _pjmhHeader->Msg_offset );

		/* ---------------- */
		/* VBAS: Msg_length */
		/* ---------------- */
		_pjmhHeader->Msg_length = ReadVbasInfo( _pcCio, _pjmhHeader->Msg_length );

		/* --------- */
		/* VBAS: Aux */
		/* --------- */
		if ( (_pjmhHeader->Class_Id & 0x01) == 1 ) {
			_pjmhHeader->Layer_nb = 0;
			_pjmhHeader->Layer_nb = ReadVbasInfo( _pcCio, _pjmhHeader->Layer_nb );
		}
	}

	/**
	 * Read the information contains in VBAS [JPP/JPT stream message header].
	 * Store information (7 bits) in value.
	 *
	 * \param _pcCio Undocumented parameter from the OpenJpeg Library.
	 * \param _ui32Value Undocumented parameter from the OpenJpeg Library.
	 */
	uint32_t LSE_CALL CJpt::ReadVbasInfo( CCio::opj_cio_t * _pcCio, uint32_t _ui32Value ) {
		uint8_t ui8Element;

		ui8Element = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 ));
		while ( (ui8Element >> 7) == 1 ) {
			_ui32Value = (_ui32Value << 7);
			_ui32Value |= (ui8Element & 0x7f);
			ui8Element = static_cast<uint8_t>(CCio::cio_read( _pcCio, 1 ));
		}
		_ui32Value = (_ui32Value << 7);
		_ui32Value |= (ui8Element & 0x7f);

		return _ui32Value;
	}

	/**
	 * Re-initialize the value of the message header structure.
	 * Only parameters always present in message header.
	 *
	 * \param _pjmhHeader Undocumented parameter from the OpenJpeg Library.
	 */
	void LSE_CALL CJpt::ReInitMsgHeader( opj_jpt_msg_header_t * _pjmhHeader ) {
		_pjmhHeader->Id = 0;			// In-class Identifier.
		_pjmhHeader->last_byte = 0;		// Last byte information.
		_pjmhHeader->Msg_offset = 0;	// Message offset.
		_pjmhHeader->Msg_length = 0;	// Message length.
	}

}	// namespace lsi
