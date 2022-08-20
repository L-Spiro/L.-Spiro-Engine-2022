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
 * Description: Base functionality for sound sources.  Sources are like instances of 3D models.  The shared
 *	data is part of the CSoundBuffer class and this class maintains references to them.  This class is just
 *	the base class for sound sources.  There are 2 types of sources: static and streaming.  They connect to
 *	buffers differently (actually streaming uses its own buffers), but share all of the same properties.
 * This class provides all of the similar properties and the CStaticSoundSource and CStreamingSoundSource
 *	inherit from this and add their own methods for connecting to buffers.
 */

#include "LSSSoundSource.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {

	// == Members.
	// The unique ID counter.
	uint32_t CSoundSource::m_ui32IdCounter = CSoundSource::LSS_E_INVALID_BUFFER;

	// == Functions.
	/**
	 * Gets our ID.
	 *
	 * \return Returns the unique identifier for this sound source.
	 */
	uint32_t LSE_CALL CSoundSource::GetId() const {
		return m_ui32Id;
	}

	/**
	 * Gets our state.
	 *
	 * \return Returns the state of this source.
	 */
	CSoundSource::LSS_SOUND_STATE LSE_CALL CSoundSource::GetState() const {
		return m_ssState;
	}

	/**
	 * Gets our position.
	 *
	 * \return Returns the position of the source.  The position is always returned as a 3D vector, even
	 *	though the source may be only 2D or 1D.
	 */
	const CVector3 & LSE_CALL CSoundSource::GetPos() const {
		return m_vPos;
	}

	/**
	 * Sets our position.
	 *
	 * \param _vPos A variable-dimensional vector indicating the position of the source.
	 */
	void LSE_CALL CSoundSource::SetPos( const CVector3 &_vPos ) {
		m_vPos = _vPos;
		m_bDirty = true;
	}

	/**
	 * Gets our velocity.
	 *
	 * \return Returns a reference to a vector indicating the velocity of the source.
	 */
	const CVector3 & LSE_CALL CSoundSource::GetVel() const {
		return m_vVel;
	}

	/**
	 * Sets our velocity.
	 *
	 * \param _vVel The velocity of the source.
	 */
	void LSE_CALL CSoundSource::SetVel( const CVector3 &_vVel ) {
		m_vVel = _vVel;
		m_bDirty = true;
	}

	/**
	 * Gets our direction.
	 *
	 * \return Returns a vector indicating the direction of the source.
	 */
	const CVector3 & LSE_CALL CSoundSource::GetDir() const {
		return m_vDir;
	}

	/**
	 * Sets our direction.
	 *
	 * \param _vDir The new direction of the source.
	 */
	void LSE_CALL CSoundSource::SetDir( const CVector3 &_vDir ) {
		m_vDir = _vDir;
		m_bDirty = true;
	}

	/**
	 * Gets our distance.
	 *
	 * \return Returns the maximum distance value for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetMaxDist() const {
		return m_fDist;
	}

	/**
	 * Sets our distance.
	 *
	 * \param _fDist The maximum distance for this source.
	 */
	void LSE_CALL CSoundSource::SetMaxDist( LSSREAL _fDist ) {
		if ( m_fDist != _fDist ) {
			m_fDist = _fDist;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our pitch multiplier.
	 *
	 * \return Returns the pitch multiplier for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetPitchMultiplier() const {
		return m_fPitch;
	}

	/**
	 * Sets our pitch multiplier.
	 *
	 * \param _fPitch The pitch multiplier for this source.
	 */
	void LSE_CALL CSoundSource::SetPitchMultiplier( LSSREAL _fPitch ) {
		if ( m_fPitch != _fPitch ) {
			m_fPitch = _fPitch;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our gain.
	 *
	 * \return Returns the gain for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetGain() const {
		return m_fGain;
	}

	/**
	 * Sets our gain.
	 *
	 * \param _fGain The gain for this source.
	 */
	void LSE_CALL CSoundSource::SetGain( LSSREAL _fGain ) {
		if ( m_fGain != _fGain ) {
			m_fGain = _fGain;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our roll-off.
	 *
	 * \return Returns the roll-off for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetRolloff() const {
		return m_fRolloff;
	}

	/**
	 * Sets our roll-off.
	 *
	 * \param _fRolloff The roll-off for this source.
	 */
	void LSE_CALL CSoundSource::SetRolloff( LSSREAL _fRolloff ) {
		if ( m_fRolloff != _fRolloff ) {
			m_fRolloff = _fRolloff;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our minimum gain.
	 *
	 * \return Returns the minimum gain for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetMinGain() const {
		return m_fMinGain;
	}

	/**
	 * Sets our minimum gain.
	 *
	 * \param _fMinGain The minimum gain for this source.
	 */
	void LSE_CALL CSoundSource::SetMinGain( LSSREAL _fMinGain ) {
		if ( m_fMinGain != _fMinGain ) {
			m_fMinGain = _fMinGain;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our maximum gain.
	 *
	 * \return Returns the maximum gain for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetMaxGain() const {
		return m_fMaxGain;
	}

	/**
	 * Sets our maximum gain.
	 *
	 * \param _fMaxGain The maximum gain for this source.
	 */
	void LSE_CALL CSoundSource::SetMaxGain( LSSREAL _fMaxGain ) {
		if ( m_fMaxGain != _fMaxGain ) {
			m_fMaxGain = _fMaxGain;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our out-of-cone gain.
	 *
	 * \return Returns the gain value applied to the region outside the cone angle for this source.
	 */
	LSSREAL LSE_CALL CSoundSource::GetOuterConeGain() const {
		return m_fOuterGain;
	}

	/**
	 * Sets our out-of-cone gain.
	 *
	 * \param _fOuterConeGain The gain used in the region outside the cone angle of this source.
	 */
	void LSE_CALL CSoundSource::SetOuterConeGain( LSSREAL _fOuterConeGain ) {
		if ( m_fOuterGain != _fOuterConeGain ) {
			m_fOuterGain = _fOuterConeGain;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our cone inner angle in degrees.
	 *
	 * \return Returns the inner cone angle for this source in degrees.
	 */
	LSSREAL LSE_CALL CSoundSource::GetInnerConeAngle() const {
		return m_fInnerAngle;
	}

	/**
	 * Sets our cone inner angle.
	 *
	 * \param _fInnerConeGain The inner cone angle.
	 */
	void LSE_CALL CSoundSource::SetInnerConeAngle( LSSREAL _fInnerConeGain ) {
		if ( m_fInnerAngle != _fInnerConeGain ) {
			m_fInnerAngle = _fInnerConeGain;
			m_bDirty = true;
		}
	}

	/**
	 * Gets our cone angle in degrees.
	 *
	 * \return Returns the outer cone angle for this source in degrees.
	 */
	LSSREAL LSE_CALL CSoundSource::GetConeAngle() const {
		return m_fConeAngle;
	}

	/**
	 * Sets our cone angle in degrees.
	 *
	 * \param _fConeAngle The new cone angle for this source in degrees.
	 */
	void LSE_CALL CSoundSource::SetConeAngle( LSSREAL _fConeAngle ) {
		if ( m_fConeAngle != _fConeAngle ) {
			m_fConeAngle = _fConeAngle;
			m_bDirty = true;
		}
	}

	/**
	 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
	 * Time is given in microseconds.
	 *
	 * \param _ui64TimeSinceLastUpdate Microseconds by which to update this source.
	 */
	void LSE_CALL CSoundSource::Tick( uint64_t _ui64TimeSinceLastUpdate ) {
		// Update the loop mode if changed.
		LSBOOL bIsPlaying = m_ssState != LSS_SS_STOPPED;
		UpdateLoop();
		if ( m_ssState == LSS_SS_PLAYING ) {
			m_ui64Time += _ui64TimeSinceLastUpdate;
		}
		else if ( m_ssState == LSS_SS_STOPPED ) {
			if ( bIsPlaying ) {
				EventSoundEndedAsync();
			}
			m_ui64DeadTime += _ui64TimeSinceLastUpdate;
		}
	}

	/**
	 * A synchronous update on the main game thread.  This function is guaranteed always to be called on the
	 *	main game thread, typically at the start of a logical update.
	 *
	 * \param _uiptrUserPtr User data passed during the update on the main thread.
	 */
	void LSE_CALL CSoundSource::SynchronousUpdate( uintptr_t /*_uiptrUserPtr*/ ) {
		LSS_SOUND_STATE ssState = m_ssLastStateSynchronous;
		m_ssLastStateSynchronous = m_ssState;

		m_bJustBeganSynchronous = ssState != LSS_SS_STOPPED && m_ssLastStateSynchronous == LSS_SS_STOPPED;
		m_bJustEndedSynchronous = ssState == LSS_SS_STOPPED && m_ssLastStateSynchronous != LSS_SS_STOPPED;
	}

	/**
	 * Sets the loop count.
	 *
	 * \param _ui32Count The loop count.  1 = no repeat.  LSS_LM_INFINITE = repeat for forever.
	 */
	void LSE_CALL CSoundSource::SetLoopCount( uint32_t _ui32Count ) {
		m_ui32LoopCount = _ui32Count;
	}

	/**
	 * Are we set to loop?
	 *
	 * \return Returns true if we will loop again after the current play is finished.  Returns false if
	 *	the sound will stop after the current play is finished.
	 */
	LSBOOL LSE_CALL CSoundSource::Loop() const {
		if ( m_ui32LoopCount == LSS_LM_INFINITE ) { return true; }
		uint32_t ui32LoopCount = TotalLoops() + 1;
		return ui32LoopCount < m_ui32LoopCount;
	}

	/**
	 * How many times have we looped?
	 *
	 * \return Returns the total number of times the sound has looped since the last play command.
	 */
	uint32_t LSE_CALL CSoundSource::TotalLoops() const {
		if ( !m_ui64SoundTime ) { return 0; }
		return static_cast<uint32_t>(m_ui64Time / m_ui64SoundTime);
	}

	/**
	 * Have we expired?  We expire when our dead time exceeds the expiration date.  This is used for
	 *	(optional) automatical removal.
	 *
	 * \return Returns true if this source has been dead (in the LSS_SS_STOPPED state) for at least as
	 *	long as its expiration date value.
	 */
	LSBOOL LSE_CALL CSoundSource::Expired() const {
		if ( m_ssState != LSS_SS_STOPPED ) {
			return false;
		}
		// Additionally we cannot expire until all events have been handled.
		if ( m_bJustEndedSynchronous || m_bJustEndedSynchronous ) { return false; }
		return m_ui64DeadTime >= m_ui64ExpirationDate;
	}

	/**
	 * Sets the expiration date for this source.  The expiration date is the number of microseconds the
	 *	sound can remain in the LSS_SS_STOPPED state before being automatically deleted.
	 *
	 * \param _ui64Micros Number of microseconds the sound can spend in the LSS_SS_STOPPED state before
	 *	being automatically deleted.
	 */
	void LSE_CALL CSoundSource::SetExpirationDate( const uint64_t &_ui64Micros ) {
		m_ui64ExpirationDate = _ui64Micros;
	}

	/**
	 * Sets the value indicating whether this source should resist soft resets or not.  A soft reset
	 *	is a reset that stops sources that do not resist soft resets, then releases all unreferenced
	 *	data (sources/buffers).  This is not the same as a soft unload, which can happen at any time
	 *	just to save RAM and works only on expired data.  A soft reset will stop sources that do not
	 *	resist the stopping action by returning true via this function.
	 *
	 * \return Returns true.  Override this class to return any other value.
	 */
	LSBOOL LSE_CALL CSoundSource::CanSoftReset() const {
		return true;
	}

	/**
	 * Handles the expiration of the sound.  This virtual function should be overridden by an inheriting
	 *	class to handle the event where the sound stops playing.  This function is called immediately
	 *	when the end of the sound is detected.  This means this function could be called on any thread
	 *	at any time, so care must be taken to handle synchronization issues from this function.
	 */
	void LSE_CALL CSoundSource::EventSoundEndedAsync() {
	}

	/**
	 * Apply states to the hardware or sound system (OpenAL for example).
	 * Does not clear the dirty flag.
	 */
	void LSE_CALL CSoundSource::ApplyStates() {
#ifdef LSS_USEOPENAL
		// We will update all properties but not clear the dirty flag.  The
		//	subclasses should clear this when they have uploaded all of their
		//	data.
		ALuint uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_PITCH, m_fPitch );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_GAIN, m_fGain );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_MAX_DISTANCE, m_fDist );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_ROLLOFF_FACTOR, m_fRolloff );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_REFERENCE_DISTANCE, static_cast<ALfloat>(1.0)/*m_fDist * static_cast<LSSREAL>(0.5)*/ );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_MIN_GAIN, m_fMinGain );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_MAX_GAIN, m_fMaxGain );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_CONE_OUTER_GAIN, CStd::Max<LSSREAL>( m_fOuterGain, LSM_FLT_EPSILON ) );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_CONE_INNER_ANGLE, m_fInnerAngle );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		::alSourcef( m_uiAlSourceId, AL_CONE_OUTER_ANGLE, m_fConeAngle );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		const LSSREAL fPos[] = {
			static_cast<LSSREAL>(m_vPos[0]),
			static_cast<LSSREAL>(m_vPos[1]),
			static_cast<LSSREAL>(m_vPos[2]),
		};
		::alSourcefv( m_uiAlSourceId, AL_POSITION, fPos );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		const LSSREAL fVel[] = {
			static_cast<LSSREAL>(m_vVel[0]),
			static_cast<LSSREAL>(m_vVel[1]),
			static_cast<LSSREAL>(m_vVel[2]),
		};
		::alSourcefv( m_uiAlSourceId, AL_VELOCITY, fVel );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );
		const LSSREAL fDir[] = {
			static_cast<LSSREAL>(m_vDir[0]),
			static_cast<LSSREAL>(m_vDir[1]),
			static_cast<LSSREAL>(m_vDir[2]),
		};
		::alSourcefv( m_uiAlSourceId, AL_DIRECTION, fDir );
		uiError = ::alGetError();
		assert( uiError == AL_NO_ERROR );

		UpdateLoop();
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Update the looping status.
	 */
	void LSE_CALL CSoundSource::UpdateLoop() {
		LSBOOL bLoop = Loop();
		if ( bLoop && !m_bSoundSystemLooping ) {
#ifdef LSS_USEOPENAL
			// Actually streaming sounds always use non-looping.
			if ( !Static() ) {
				::alSourcei( m_uiAlSourceId, AL_LOOPING, AL_FALSE );
			}
			else {
				::alSourcei( m_uiAlSourceId, AL_LOOPING, AL_TRUE );
			}
#endif	// #ifdef LSS_USEOPENAL
			m_bSoundSystemLooping = true;
		}
		else if ( !bLoop && m_bSoundSystemLooping ) {
#ifdef LSS_USEOPENAL
			::alSourcei( m_uiAlSourceId, AL_LOOPING, AL_FALSE );
#endif	// #ifdef LSS_USEOPENAL
			m_bSoundSystemLooping = false;
		}
	}

	/**
	 * A device has been created.  We can register with the hardware.
	 * The sound manager notifies us of this event after the device has
	 *	been changed.  If there is a device already when we are created,
	 *	we are notified via this function immediately after being created.
	 *
	 * \return Returns false if there is an error uploading our sound data to the hardware.
	 */
	LSBOOL LSE_CALL CSoundSource::DeviceActivated() {
#ifdef LSS_USEOPENAL
		::alGetError();

		// Delete existing source (this never actually happens).
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			EnterDeletableState();
			::alDeleteSources( 1, &m_uiAlSourceId );
			assert( ::alIsSource( m_uiAlSourceId ) == AL_FALSE );
			m_uiAlSourceId = LSS_INVALID_BUFFER;
		}

		// Create the new source.
		::alGenSources( 1, &m_uiAlSourceId );

		if ( ::alGetError() != AL_NO_ERROR ) {
			m_uiAlSourceId = LSS_INVALID_BUFFER;
			return false;
		}
#endif	// #ifdef LSS_USEOPENAL

		ApplyStates();
		return true;
	}

	/**
	 * Notifies us that the device is about to be lost (changed).  We need
	 *	to sever our connections to it, although we may retain our data/
	 *	settings.
	 */
	void LSE_CALL CSoundSource::DeviceWillClose() {
		EnterDeletableState();
#ifdef LSS_USEOPENAL
		// Delete existing source.
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alDeleteSources( 1, &m_uiAlSourceId );
			assert( ::alIsSource( m_uiAlSourceId ) == AL_FALSE );
			m_uiAlSourceId = LSS_INVALID_BUFFER;
		}
#endif	// #ifdef LSS_USEOPENAL


		// When the device is closed, our looping status is no longer registered with the sound system.
		m_bSoundSystemLooping = false;
	}

	/**
	 * Enter a deletable state.  In some sound systems, the sound sources
	 *	cannot be removed from the system under some circumstances, such as
	 *	when they are playing.  Use this function to enter a state that
	 *	allows deletion of the object.
	 */
	void LSE_CALL CSoundSource::EnterDeletableState() const {
#ifdef LSS_USEOPENAL
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alSourceStop( m_uiAlSourceId );
			// Stopping the source does not immediately stop it.
			//	We have to spin.
			// Clear the hell out of the error flag.
			::alGetError();
			while ( true ) {
				ALint iState;
				::alGetSourcei( m_uiAlSourceId, AL_SOURCE_STATE, &iState );
				// Check for error first since an error inside ::alGetSourcei() coulod leave iState
				//	uninitialized.
				if ( ::alGetError() != AL_NO_ERROR ) { return; }	// Oops!
																	// Hope for the best!
				if ( iState == AL_STOPPED || iState == AL_INITIAL ) { return; }
			}
		}
#endif	// #ifdef LSS_USEOPENAL
	}

}	// namespace lss

#pragma warning( pop )
