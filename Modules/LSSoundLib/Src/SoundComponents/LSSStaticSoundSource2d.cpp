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
 * Description: A static sound emenating from somewhere in a 2D world.  Does not use streaming.  The source
 *	is not relative to the listener.
 */

#include "LSSStaticSoundSource2d.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CStaticSoundSource2d::CStaticSoundSource2d( uint32_t _ui32Id ) :
		Parent( _ui32Id ) {
		//m_fRolloff = LSM_ZERO;
	}
	LSE_CALLCTOR CStaticSoundSource2d::~CStaticSoundSource2d() {
	}

	// == Functions.
	/**
	 * Gets the type of this source.
	 *
	 * \return Returns one of the LSS_SOUND_SOURCE values.
	 */
	LSS_SOUND_SOURCE LSE_CALL CStaticSoundSource2d::GetType() const {
		return static_cast<LSS_SOUND_SOURCE>(LSS_SS_2DSTATIC | Parent::GetType());
	}

	/**
	 * Apply states to the hardware or sound system (OpenAL for example).
	 * Does not clear the dirty flag.
	 */
	void LSE_CALL CStaticSoundSource2d::ApplyStates() {
		// Since we are 2D, eliminate depth from our position and velocity.
		m_vPos[2] = m_vVel[2] = LSM_ZERO;

		// For 1D sounds, use X = (pan-PAN_LEFT) * 2.0 / (ALfloat)(PAN_RIGHT-PAN_LEFT) - 1.0;
		Parent::ApplyStates();
	}

	/**
	 * A device has been created.  We can register with the hardware.
	 * The sound manager notifies us of this event after the device has
	 *	been changed.  If there is a device already when we are created,
	 *	we are notified via this function immediately after being created.
	 *
	 * \return Returns false if there is an error uploading our sound data to the hardware.
	 */
	LSBOOL LSE_CALL CStaticSoundSource2d::DeviceActivated() {
		if ( !Parent::DeviceActivated() ) { return false; }
#ifdef LSS_USEOPENAL
		// Apply additional settings to the source object.
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alSourcei( m_uiAlSourceId, AL_SOURCE_RELATIVE, AL_TRUE );
		}
#endif	// #ifdef LSS_USEOPENAL

		return true;
	}

}	// namespace lss
