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
 * Description: A static sound emenating from somewhere in the world.  The world positions are not
 *	defined here.  Does not use streaming.
 */

#include "LSSStaticSoundSource.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CStaticSoundSource::CStaticSoundSource( uint32_t _ui32Id ) :
		m_psbBuffer( NULL ),
		m_bPlayStateDirty( true ),
		Parent( _ui32Id ) {
	}
	LSE_CALLCTOR CStaticSoundSource::~CStaticSoundSource() {
		SetBuffer( NULL );
	}

	// == Functions.
	/**
	 * Gets the type of this source.
	 *
	 * \return Returns one of the LSS_SOUND_SOURCE values.
	 */
	LSS_SOUND_SOURCE LSE_CALL CStaticSoundSource::GetType() const {
		return LSS_SS_STATIC;
	}

	/**
	 * Gets the ID of the buffer we reference, if any.
	 *
	 * \return Returns the unique ID of the buffer we reference, or CSoundSource::LSS_E_INVALID_BUFFER if there is none.
	 */
	uint32_t LSE_CALL CStaticSoundSource::GetBufferId() const {
		if ( !m_psbBuffer ) { return CSoundSource::LSS_E_INVALID_BUFFER; }
		return m_psbBuffer->GetId();
	}

	/**
	 * Begin playing the sound.  If there is no buffer, returns false.  The sound plays regardless
	 *	of whether or not there is a sound device.  If there is no sound device, the sound will begin
	 *	playing at the correct position when a device becomes active.  The sound begins playing on the
	 *	next tick event.
	 *
	 * \param _ui32LoopCount Number of times to loop the sound.
	 * \return Returns true if the play was successful.  A return of false indicates that no buffer was
	 *	applied to this source.
	 */
	LSBOOL LSE_CALL CStaticSoundSource::Play( uint32_t _ui32LoopCount ) {
		if ( Static() && !m_psbBuffer ) { return false; }
		SetLoopCount( _ui32LoopCount );
		if ( m_ssState != LSS_SS_PAUSED ) {
			// If not paused, play from the beginning.
			m_ui64Time = 0ULL;
		}
		m_ssState = LSS_SS_PLAYING;
		m_ui64DeadTime = 0ULL;

		// Update the play state on the next tick.
		m_bPlayStateDirty = true;

		// A little trick to get the looping status to update on the next tick (which is when the
		//	sound will start playing).
		m_bSoundSystemLooping = !Loop();

		return true;
	}

	/**
	 * Pause the sound.  If there is no buffer, returns false.
	 *
	 * \return Returns true if a buffer has been assigned to this source.
	 */
	LSBOOL LSE_CALL CStaticSoundSource::Pause() {
		if ( Static() && !m_psbBuffer ) { return false; }

		m_ssState = LSS_SS_PAUSED;

		// Update the play state on the next tick.
		m_bPlayStateDirty = true;

		return true;
	}

	/**
	 * Stop the sound.  Sets the position back to 0 and waits for a play command.
	 *
	 * \return Returns true if a buffer has been assigned to this source.
	 */
	LSBOOL LSE_CALL CStaticSoundSource::Stop() {
		if ( Static() && !m_psbBuffer ) { return false; }

		m_ssState = LSS_SS_STOPPED;

		// Update the play state on the next tick.
		m_bPlayStateDirty = true;
		m_ui64Time = 0ULL;
		return true;
	}

	/**
	 * Is this source static (non-streaming)?
	 *
	 * \return Returns true if the source is static, false if streaming.
	 */
	LSBOOL LSE_CALL CStaticSoundSource::Static() const {
		return true;
	}

	/**
	 * Sets the buffer.  Does not cause playing.  NULL is valid.
	 *
	 * \param _psbBuffer The possibly shared buffer to be used by this source.
	 */
	void LSE_CALL CStaticSoundSource::SetBuffer( const CSoundBuffer * _psbBuffer ) {
		if ( m_psbBuffer ) {
			m_psbBuffer->DecRef();
		}

		m_psbBuffer = _psbBuffer;
		m_ui64Time = 0ULL;

		if ( m_psbBuffer ) {
			m_psbBuffer->IncRef();

			m_ui64SoundTime = m_psbBuffer->GetLengthInMicros();
		}

#ifdef LSS_USEOPENAL
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			if ( !m_psbBuffer ) {
				EnterDeletableState();
			}
			::alSourcei( m_uiAlSourceId, AL_BUFFER, m_psbBuffer ? static_cast<ALint>(m_psbBuffer->m_uiAlBufferId) : 0 );
#ifdef _DEBUG
			ALenum eError = ::alGetError();
			assert( eError == AL_NO_ERROR );
#endif	// #ifdef _DEBUG
		}
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
	 * Time is given in microseconds.
	 *
	 * \param _ui64TimeSinceLastUpdate Microseconds by which to update.
	 */
	void LSE_CALL CStaticSoundSource::Tick( uint64_t _ui64TimeSinceLastUpdate ) {
		// If we have run out of loops, stop.
		if ( m_ssState == LSS_SS_PLAYING && TotalLoops() >= m_ui32LoopCount ) {
			Stop();
		}

#ifdef LSS_USEOPENAL
		// If we have no OpenAL ID, do nothing.
		if ( m_uiAlSourceId == LSS_INVALID_BUFFER ) {
			Parent::Tick( _ui64TimeSinceLastUpdate );
			return;
		}
#endif	// #ifdef LSS_USEOPENAL

		if ( m_bDirty ) {
			ApplyStates();
			m_bDirty = false;
		}

		UpdatePlayState();

		Parent::Tick( _ui64TimeSinceLastUpdate );
	}

	/**
	 * Notifies us that the given device has become active and
	 *	we should send our buffer to it.  A return of false from this
	 *	function indicates the buffer could not be uploaded.  This is
	 *	considered a critical error by the engine.
	 *
	 * \return Returns true if the buffer could be reloaded.
	 */
	LSBOOL LSE_CALL CStaticSoundSource::DeviceActivated() {
		if ( !Parent::DeviceActivated() ) { return false; }
#ifdef LSS_USEOPENAL
		// Apply additional settings to the source object.
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alSourcei( m_uiAlSourceId, AL_BUFFER, m_psbBuffer ? static_cast<ALint>(m_psbBuffer->m_uiAlBufferId) : 0 );
#ifdef _DEBUG
			ALenum eError = ::alGetError();
			assert( eError == AL_NO_ERROR );
#endif	// #ifdef _DEBUG
		}
#endif	// #ifdef LSS_USEOPENAL

		// Force-update the play state.
		m_bPlayStateDirty = true;
		UpdatePlayState();
		UpdateLoop();
		// In fact we want to update the play state again on the next tick, so
		//	keep m_bPlayStateDirty true;
		// This is because the delay caused by changing the device can be large and
		//	when we update the play position here our time might be far behind the
		//	current time.  After one tick, our time will be updated and more accurate,
		//	and we can adjust the playback position one more time to ensure it is
		//	accurate.
		m_bPlayStateDirty = true;

		m_bDirty = false;
		return true;
	}

	/**
	 * Update the play state.
	 */
	void LSE_CALL CStaticSoundSource::UpdatePlayState() {
		if ( m_ui64SoundTime == 0 ) { return; }

		// Begin, end, or pause playing.
		if ( m_bPlayStateDirty ) {
#ifdef LSS_USEOPENAL
			if ( m_uiAlSourceId == LSS_INVALID_BUFFER ) { return; }

			typedef void (AL_APIENTRY * pfStateFunc)( ALuint uiId );
			static const pfStateFunc pfFuncs[] = {
				::alSourcePlay,
				::alSourcePause,
				::alSourceStop,
			};
			static const ALint iAlStates[] = {
				AL_PLAYING,
				AL_PAUSED,
				AL_STOPPED,
			};

			// Set the position of the sound if we are not stopping it.
			if ( m_ssState != LSS_SS_STOPPED ) {
				if ( Static() ) {
					::alSourcef( m_uiAlSourceId, AL_SEC_OFFSET, static_cast<ALfloat>(m_ui64Time % m_ui64SoundTime) * static_cast<ALfloat>(1.0 / 1000000.0) );
				}
			}
			if ( TotalLoops() < m_ui32LoopCount ) {
				ALint iVal;
				::alGetSourcei( m_uiAlSourceId, AL_SOURCE_STATE, &iVal );
				if ( ::alGetError() == AL_NO_ERROR ) {
					if ( iVal != iAlStates[m_ssState] ) {
						pfFuncs[m_ssState]( m_uiAlSourceId );
					}
				}
				else {
					pfFuncs[m_ssState]( m_uiAlSourceId );
				}
			}
			else {
				::alSourceStop( m_uiAlSourceId );
			}

#endif	// #ifdef LSS_USEOPENAL

			m_bPlayStateDirty = false;
		}
	}

}	// namespace lss
