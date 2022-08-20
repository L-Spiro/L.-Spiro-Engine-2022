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
 * Description: The listener hears all the sounds in the world.  There is only one listener in the game at a
 *	time.  The gain property controls the master volume of all sound in the game.
 */

#include "LSSSoundListener.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CSoundListener::CSoundListener() :
		m_bDirty( true ),
		m_fGain( static_cast<LSSREAL>(1.0)) {
		m_vPos[0] = m_vPos[1] = m_vPos[2] = LSM_ZERO;
		m_vVel[0] = m_vVel[1] = m_vVel[2] = LSM_ZERO;

		// -Z = forward.
		m_vForwardDir[0] = m_vForwardDir[1] = LSM_ZERO;
		m_vForwardDir[2] = static_cast<LSSREAL>(-1.0);

		// Y = up.
		m_vUpDir[0] = m_vUpDir[2] = LSM_ZERO;
		m_vUpDir[1] = LSM_ONE;
	}
	LSE_CALLCTOR CSoundListener::~CSoundListener() {
	}

	// == Functions.
	/**
	 * Gets our position.
	 *
	 * \return Returns the position of the listener.
	 */
	const CVector3 & LSE_CALL CSoundListener::GetPos() const {
		return m_vPos;
	}

	/**
	 * Sets our position.
	 *
	 * \param _vPos Position to set.
	 */
	void LSE_CALL CSoundListener::SetPos( const CVector3 &_vPos ) {
		m_vPos = _vPos;
		m_bDirty = true;
	}

	/**
	 * Gets our velocity.
	 *
	 * \return Returns the velocity of the listener.
	 */
	const CVector3 & LSE_CALL CSoundListener::GetVel() const {
		return m_vVel;
	}

	/**
	 * Sets our velocity.
	 *
	 * \param _vVel Velocity to set.
	 */
	void LSE_CALL CSoundListener::SetVel( const CVector3 &_vVel ) {
		m_vVel = _vVel;
		m_bDirty = true;
	}

	/**
	 * Gets our forward orientation value.
	 *
	 * \return Returns the forward orientation value of the listener.
	 */
	const CVector3 & LSE_CALL CSoundListener::GetForward() const {
		return m_vForwardDir;
	}

	/**
	 * Gets our up orientation value.
	 *
	 * \return Returns the up orientation value of the listener.
	 */
	const CVector3 & LSE_CALL CSoundListener::GetUp() const {
		return m_vUpDir;
	}

	/**
	 * Sets our orientation using forward/up vectors.
	 *
	 * \param _vFor Forward vector.
	 * \param _vUp Up vector.
	 */
	void LSE_CALL CSoundListener::SetOrientation( const CVector3 &_vFor, const CVector3 &_vUp ) {
		m_vForwardDir = _vFor;
		m_vUpDir = _vUp;
		m_bDirty = true;
	}

	/**
	 * Gets our gain (volume).
	 *
	 * \return Returns the gain of the listener.
	 */
	LSSREAL LSE_CALL CSoundListener::GetGain() const {
		return m_fGain;
	}

	/**
	 * Sets our gain (volume).
	 *
	 * \param _fGain The new gain value.
	 */
	void LSE_CALL CSoundListener::SetGain( LSSREAL _fGain ) {
		if ( m_fGain != _fGain ) {
			m_fGain = _fGain;
			m_bDirty = true;
		}
	}

	/**
	 * Update the listener.  Just updates the hardware/sound system if dirty and clears the dirty flag.
	 */
	void LSE_CALL CSoundListener::Tick() {
		if ( m_bDirty ) {
			Update();
		}
	}

	/**
	 * Update our data.  Send it to the hardware or sound system.
	 *
	 * \return Returns true if there are no errors related to the sound system after the update.
	 */
	LSBOOL LSE_CALL CSoundListener::Update() {
#ifdef LSS_USEOPENAL
		// Clear error flag.
		::alGetError();

		// Send it our data.
		::alListenerf( AL_GAIN, m_fGain );
		const LSSREAL fPos[] = {
			static_cast<LSSREAL>(m_vPos[0]),
			static_cast<LSSREAL>(m_vPos[1]),
			static_cast<LSSREAL>(m_vPos[2]),
		};
		::alListenerfv( AL_POSITION, fPos );
		const LSSREAL fVel[] = {
			static_cast<LSSREAL>(m_vVel[0]),
			static_cast<LSSREAL>(m_vVel[1]),
			static_cast<LSSREAL>(m_vVel[2]),
		};
		::alListenerfv( AL_VELOCITY, fVel );

		const LSSREAL fOrientation[] = {
			static_cast<LSSREAL>(m_vForwardDir[0]), static_cast<LSSREAL>(m_vForwardDir[1]), static_cast<LSSREAL>(m_vForwardDir[2]),
			static_cast<LSSREAL>(m_vUpDir[0]), static_cast<LSSREAL>(m_vUpDir[1]), static_cast<LSSREAL>(m_vUpDir[2])
		};
		::alListenerfv( AL_ORIENTATION, fOrientation );

		// We are fully updated.
		m_bDirty = false;

		return ::alGetError() == AL_NO_ERROR;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Notifies us that the given device has become active and
	 *	we should send our buffer to it.  A return of false from this
	 *	function indicates the buffer could not be uploaded.  This is
	 *	considered a critical error by the engine.
	 *
	 * \return Returns true if the buffer could be reloaded.
	 */
	LSBOOL LSE_CALL CSoundListener::DeviceActivated() {
#ifdef LSS_USEOPENAL
		return Update();
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Notifies us that the device is about to be lost (changed).  We need
	 *	to sever our connections to it, although we may retain our data/
	 *	settings.
	 */
	void LSE_CALL CSoundListener::DeviceWillClose() {
#ifdef LSS_USEOPENAL
		// In OpenAL, there is nothing to do here.
#endif	// #ifdef LSS_USEOPENAL
	}

}	// namespace lss
