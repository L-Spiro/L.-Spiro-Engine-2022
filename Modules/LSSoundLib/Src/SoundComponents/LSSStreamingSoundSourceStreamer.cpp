/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A streaming sound source needs an abstract way to stream data from a source (file/memory/
 *	network/etc.)  This class provides an interface for the retrieval of sound data for use by the
 *	CStreamingSoundSource class.
 */

#include "LSSStreamingSoundSourceStreamer.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR IStreamingSoundSourceStreamer::IStreamingSoundSourceStreamer() :
		m_ui32RefCount( 0 ) {
	}
	LSE_CALLCTOR IStreamingSoundSourceStreamer::~IStreamingSoundSourceStreamer() {
	}

	// == Functions.
	/**
	 * Gets the type of this streamer.  The sound engine defines only this interface. which is of type
	 *	LSS_DSS_BASE.
	 *
	 * \return Returns the bitmask representing the type of streamer this instance is.
	 */
	uint32_t LSE_CALL IStreamingSoundSourceStreamer::GetType() const {
		return LSS_DSS_BASE;
	}

	/**
	 * Increase the reference count.
	 */
	void LSE_CALL IStreamingSoundSourceStreamer::IncRef() const {
		++m_ui32RefCount;
	}

	/**
	 * Decrease the reference count.
	 */
	void LSE_CALL IStreamingSoundSourceStreamer::DecRef() const {
		--m_ui32RefCount;
	}

	/**
	 * Gets the reference count.
	 *
	 * \return Returns the reference count on this object.
	 */
	uint32_t LSE_CALL IStreamingSoundSourceStreamer::GetRef() const {
		return m_ui32RefCount;
	}

}	// namespace lss
