/*
 * Copyright (c) 2005, HervÅEDrolon, FreeImage Team
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

#include "LSIEvent.h"
#include <cstdio>
#include <cstdarg>


namespace lsi {

	// == Functions.
	/**
	 * Write formatted data to a string and send the string to a user callback.
	 *
	 * \param _cpInfoC Codec context info.
	 * \param _i32EventType Event type or callback to use to send the message.
	 * \param _pcFormat Format-control string (plus optional arguments).
	 * \return Returns true if successful, returns false otherwise.
	 */
	LSBOOL LSE_CCALL CEvent::opj_event_msg( CJpegCommonStructs::opj_common_ptr _cpInfoC, int32_t _i32EventType, const char * _pcFormat, ... ) {
#define MSG_SIZE 512		// 512 bytes should be more than enough for a short message.
		CJpegCommonStructs::opj_msg_callback mcMsgHandler = NULL;

		CJpegCommonStructs::opj_event_mgr_t * pemEventMgr = _cpInfoC->event_mgr;
		if ( pemEventMgr != NULL ) {
			switch( _i32EventType ) {
				case EVT_ERROR : {
					mcMsgHandler = pemEventMgr->error_handler;
					break;
				}
				case EVT_WARNING : {
					mcMsgHandler = pemEventMgr->warning_handler;
					break;
				}
				case EVT_INFO : {
					mcMsgHandler = pemEventMgr->info_handler;
					break;
				}
				default : {
					break;
				}
			}
			if ( mcMsgHandler == NULL ) {
				return false;
			}
		}
		else { return false; }

		if ( (_pcFormat != NULL) && (pemEventMgr != NULL) ) {
			va_list vaArg;
			int32_t i32StrLen;
			char szMessage[MSG_SIZE];
			CStd::MemSet( szMessage, 0, MSG_SIZE );
			// Initialize the optional parameter list.
			va_start( vaArg, _pcFormat );
			// Check the length of the format string.
			i32StrLen = static_cast<int32_t>((CStd::StrLen( _pcFormat ) > MSG_SIZE) ? MSG_SIZE : CStd::StrLen( _pcFormat ));
			// Parse the format string and put the result in "szMessage".
#ifdef LSE_WINDOWS
			::vsprintf_s( szMessage, _pcFormat, vaArg );
#else
			::vsprintf( szMessage, _pcFormat, vaArg );
#endif	// #ifdef LSE_WINDOWS
			// Deinitialize the optional parameter list.
			va_end( vaArg );

			// Output the szMessage to the user program.
			mcMsgHandler( szMessage, _cpInfoC->client_data );
		}

		return true;
	}

}	// namespace lsi
