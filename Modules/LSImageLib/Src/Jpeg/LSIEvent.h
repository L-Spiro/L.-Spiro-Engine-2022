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


#ifndef __LSI_EVENT_H__
#define __LSI_EVENT_H__

#include "../LSIImageLib.h"
#include "LSIJpegCommonStructs.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define EVT_ERROR	1						/**< Error event type. */
#define EVT_WARNING	2						/**< Warning event type. */
#define EVT_INFO	4						/**< Debug event type. */

namespace lsi {

	/**
	 * Class CEvent
	 * \brief Implementation of the event callback system.
	 *
	 * Description: Implementation of the event callback system.
	 */
	class CEvent {
	public :
		// == Functions.
		/**
		 * Write formatted data to a string and send the string to a user callback.
		 *
		 * \param _cpInfoC Codec context info.
		 * \param _i32EventType Event type or callback to use to send the message.
		 * \param _pcFormat Format-control string (plus optional arguments).
		 * \return Returns true if successful, returns false otherwise.
		 */
		static LSBOOL LSE_CCALL				opj_event_msg( CJpegCommonStructs::opj_common_ptr _cpInfoC, int32_t _i32EventType, const char * _pcFormat, ... );
	};

}	// namespace lsi

#endif	// __LSI_EVENT_H__
