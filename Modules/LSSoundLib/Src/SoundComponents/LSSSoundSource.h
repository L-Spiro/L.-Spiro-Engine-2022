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


#ifndef __LSS_SOUNDSOURCE_H__
#define __LSS_SOUNDSOURCE_H__

#include "../LSSSoundLib.h"
#include "LSSSoundSourceTypes.h"
#include "Vector/LSMVector3.h"
#include <cassert>

namespace lss {

	/**
	 * Sound ID starting point for custom sound ID's.
	 */
	enum {
		LSS_UNIQUE_ID									= 0x10000
	};

	/**
	 * Class CSoundSource
	 * \brief Base functionality for sound sources.
	 *
	 * Description: Base functionality for sound sources.  Sources are like instances of 3D models.  The shared
	 *	data is part of the CSoundBuffer class and this class maintains references to them.  This class is just
	 *	the base class for sound sources.  There are 2 types of sources: static and streaming.  They connect to
	 *	buffers differently (actually streaming uses its own buffers), but share all of the same properties.
	 * This class provides all of the similar properties and the CStaticSoundSource and CStreamingSoundSource
	 *	inherit from this and add their own methods for connecting to buffers.
	 */
	class CSoundSource {
		friend class									CSoundManager;

	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CSoundSource( uint32_t _ui32Id );
		virtual LSE_INLINE LSE_CALLCTOR					~CSoundSource();


		// == Enumerations.
		/**
		 * An invalid buffer ID (not related to OpenAL etc.)
		 */
		enum LSS_ERROR {
			LSS_E_INVALID_BUFFER						= 0,
		};

		/**
		 * Our state.  Playing, paused, or stopped.
		 */
		enum LSS_SOUND_STATE {
			LSS_SS_PLAYING,
			LSS_SS_PAUSED,
			LSS_SS_STOPPED,
		};

		/**
		 * Loop mode.
		 */
		enum LSS_LOOP_MODE {
			LSS_LM_INFINITE								= ~0,
			LSS_LM_FORCEDWORD							= 0x7FFFFFFFUL,
		};

		// Maximum QWORD value is used to represent a non-expirable sound.
#define LSS_NONEXPIRABLE								static_cast<uint64_t>(LSE_MAXU64)


		// == Functions.
		/**
		 * Gets the type of this source.  Returns one of the LSS_SOUND_SOURCE values.
		 *
		 * \return Returns an LSS_SOUND_SOURCE enumerated value indicating the type of sound
		 *	source this object is.  Source types 1D, 2D, or 3D, and are static or streaming.
		 */
		// 
		virtual LSS_SOUND_SOURCE LSE_CALL				GetType() const = 0;

		/**
		 * Is this source static (non-streaming)?
		 *
		 * \return Returns true or false to indicate whether the source is streaming or not.
		 */
		virtual LSBOOL LSE_CALL							Static() const = 0;

		/**
		 * Gets our ID.
		 *
		 * \return Returns the unique identifier for this sound source.
		 */
		uint32_t LSE_CALL								GetId() const;

		/**
		 * Gets our state.
		 *
		 * \return Returns the state of this source.
		 */
		LSS_SOUND_STATE LSE_CALL						GetState() const;

		/**
		 * Begin playing the sound.  If there is no buffer, returns false.  The sound plays regardless
		 *	of whether or not there is a sound device.  If there is no sound device, the sound will begin
		 *	playing at the correct position when a device becomes active.  The sound begins playing on the
		 *	next tick event.
		 *
		 * \param _ui32LoopCount Number of times to loop the sound or LSS_LM_INFINITE to for a never-ending loop.
		 * \return Returns false if there was an error playing the sound, true otherwise.  Typically the only
		 *	condition that can cause a false return is if there is no buffer attached to the source.
		 */
		virtual LSBOOL LSE_CALL							Play( uint32_t _ui32LoopCount = 1 ) = 0;

		/**
		 * Pause the sound.  If there is no buffer, returns false.
		 *
		 * \return Returns false if there is no buffer attached to the source.
		 */
		virtual LSBOOL LSE_CALL							Pause() = 0;

		/**
		 * Stop the sound.  Sets the position back to 0 and waits for a play command.
		 *
		 * \return Returns false if there is no buffer, true otherwise.
		 */
		virtual LSBOOL LSE_CALL							Stop() = 0;

		/**
		 * Gets our position.
		 *
		 * \return Returns the position of the source.  The position is always returned as a 3D vector, even
		 *	though the source may be only 2D or 1D.
		 */
		const CVector3 & LSE_CALL						GetPos() const;

		/**
		 * Sets our position.
		 *
		 * \param _vPos A variable-dimensional vector indicating the position of the source.
		 */
		void LSE_CALL									SetPos( const CVector3 &_vPos );

		/**
		 * Gets our velocity.
		 *
		 * \return Returns a reference to a vector indicating the velocity of the source.
		 */
		const CVector3 & LSE_CALL						GetVel() const;

		/**
		 * Sets our velocity.
		 *
		 * \param _vVel The velocity of the source.
		 */
		void LSE_CALL									SetVel( const CVector3 &_vVel );

		/**
		 * Gets our direction.
		 *
		 * \return Returns a vector indicating the direction of the source.
		 */
		const CVector3 & LSE_CALL						GetDir() const;

		/**
		 * Sets our direction.
		 *
		 * \param _vDir The new direction of the source.
		 */
		void LSE_CALL									SetDir( const CVector3 &_vDir );

		/**
		 * Gets our distance.
		 *
		 * \return Returns the maximum distance value for this source.
		 */
		LSSREAL LSE_CALL								GetMaxDist() const;

		/**
		 * Sets our distance.
		 *
		 * \param _fDist The maximum distance for this source.
		 */
		void LSE_CALL									SetMaxDist( LSSREAL _fDist );

		/**
		 * Gets our pitch multiplier.
		 *
		 * \return Returns the pitch multiplier for this source.
		 */
		LSSREAL LSE_CALL								GetPitchMultiplier() const;

		/**
		 * Sets our pitch multiplier.
		 *
		 * \param _fPitch The pitch multiplier for this source.
		 */
		void LSE_CALL									SetPitchMultiplier( LSSREAL _fPitch );

		/**
		 * Gets our gain.
		 *
		 * \return Returns the gain for this source.
		 */
		LSSREAL LSE_CALL								GetGain() const;

		/**
		 * Sets our gain.
		 *
		 * \param _fGain The gain for this source.
		 */
		void LSE_CALL									SetGain( LSSREAL _fGain );

		/**
		 * Gets our roll-off.
		 *
		 * \return Returns the roll-off for this source.
		 */
		LSSREAL LSE_CALL								GetRolloff() const;

		/**
		 * Sets our roll-off.
		 *
		 * \param _fRolloff The roll-off for this source.
		 */
		void LSE_CALL									SetRolloff( LSSREAL _fRolloff );

		/**
		 * Gets our minimum gain.
		 *
		 * \return Returns the minimum gain for this source.
		 */
		LSSREAL LSE_CALL								GetMinGain() const;

		/**
		 * Sets our minimum gain.
		 *
		 * \param _fMinGain The minimum gain for this source.
		 */
		void LSE_CALL									SetMinGain( LSSREAL _fMinGain );

		/**
		 * Gets our maximum gain.
		 *
		 * \return Returns the maximum gain for this source.
		 */
		LSSREAL LSE_CALL								GetMaxGain() const;

		/**
		 * Sets our maximum gain.
		 *
		 * \param _fMaxGain The maximum gain for this source.
		 */
		void LSE_CALL									SetMaxGain( LSSREAL _fMaxGain );

		/**
		 * Gets our out-of-cone gain.
		 *
		 * \return Returns the gain value applied to the region outside the cone angle for this source.
		 */
		LSSREAL LSE_CALL								GetOuterConeGain() const;

		/**
		 * Sets our out-of-cone gain.
		 *
		 * \param _fOuterConeGain The gain used in the region outside the cone angle of this source.
		 */
		void LSE_CALL									SetOuterConeGain( LSSREAL _fOuterConeGain );

		/**
		 * Gets our cone inner angle in degrees.
		 *
		 * \return Returns the inner cone angle for this source in degrees.
		 */
		LSSREAL LSE_CALL								GetInnerConeAngle() const;

		/**
		 * Sets our cone inner angle.
		 *
		 * \param _fInnerConeGain The inner cone angle.
		 */
		void LSE_CALL									SetInnerConeAngle( LSSREAL _fInnerConeGain );

		/**
		 * Gets our cone angle in degrees.
		 *
		 * \return Returns the outer cone angle for this source in degrees.
		 */
		LSSREAL LSE_CALL								GetConeAngle() const;

		/**
		 * Sets our cone angle in degrees.
		 *
		 * \param _fConeAngle The new cone angle for this source in degrees.
		 */
		void LSE_CALL									SetConeAngle( LSSREAL _fConeAngle );

		/**
		 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
		 * Time is given in microseconds.
		 *
		 * \param _ui64TimeSinceLastUpdate Microseconds by which to update this source.
		 */
		virtual void LSE_CALL							Tick( uint64_t _ui64TimeSinceLastUpdate );

		/**
		 * A synchronous update on the main game thread.  This function is guaranteed always to be called on the
		 *	main game thread, typically at the start of a logical update.
		 *
		 * \param _uiptrUserPtr User data passed during the update on the main thread.
		 */
		virtual void LSE_CALL							SynchronousUpdate( uintptr_t _uiptrUserPtr );

		/**
		 * Sets the loop count.
		 *
		 * \param _ui32Count The loop count.  1 = no repeat.  LSS_LM_INFINITE = repeat for forever.
		 */
		void LSE_CALL									SetLoopCount( uint32_t _ui32Count );

		/**
		 * Are we set to loop?
		 *
		 * \return Returns true if we will loop again after the current play is finished.  Returns false if
		 *	the sound will stop after the current play is finished.
		 */
		LSBOOL LSE_CALL									Loop() const;

		/**
		 * How many times have we looped?
		 *
		 * \return Returns the total number of times the sound has looped since the last play command.
		 */
		uint32_t LSE_CALL								TotalLoops() const;

		/**
		 * Have we expired?  We expire when our dead time exceeds the expiration date.  This is used for
		 *	(optional) automatical removal.
		 *
		 * \return Returns true if this source has been dead (in the LSS_SS_STOPPED state) for at least as
		 *	long as its expiration date value.
		 */
		LSBOOL LSE_CALL									Expired() const;

		/**
		 * Sets the expiration date for this source.  The expiration date is the number of microseconds the
		 *	sound can remain in the LSS_SS_STOPPED state before being automatically deleted.
		 *
		 * \param _ui64Micros Number of microseconds the sound can spend in the LSS_SS_STOPPED state before
		 *	being automatically deleted.
		 */
		void LSE_CALL									SetExpirationDate( const uint64_t &_ui64Micros );

		/**
		 * Sets the value indicating whether this source should resist soft resets or not.  A soft reset
		 *	is a reset that stops sources that do not resist soft resets, then releases all unreferenced
		 *	data (sources/buffers).  This is not the same as a soft unload, which can happen at any time
		 *	just to save RAM and works only on expired data.  A soft reset will stop sources that do not
		 *	resist the stopping action by returning true via this function.
		 *
		 * \return Returns false.  Override this class to return any other value.
		 */
		virtual LSBOOL LSE_CALL							CanSoftReset() const;

		/**
		 * Handles the expiration of the sound.  This virtual function should be overridden by an inheriting
		 *	class to handle the event where the sound stops playing.  This function is called immediately
		 *	when the end of the sound is detected.  This means this function could be called on any thread
		 *	at any time, so care must be taken to handle synchronization issues from this function.
		 */
		virtual void LSE_CALL							EventSoundEndedAsync();

		/**
		 * Did this sound just begin playing as of the last synchronous update?
		 *
		 * \return Returns true if the sound started playing since the last main-thread update.
		 */
		LSBOOL LSE_CALL									JustStarted() const;

		/**
		 * Did this sound just stop playing as of the last synchronous update?
		 *
		 * \return Returns true if the sound stopped playing since the last main-thread update.
		 */
		LSBOOL LSE_CALL									JustStopped() const;


	protected :
		// == Enumerations.
#ifdef LSS_USEOPENAL
		// An invalid OpenAL buffer ID.  Used internally.
		enum {
			LSS_INVALID_BUFFER							= 0,
		};
#endif	// #ifdef LSS_USEOPENAL


		// == Members.
		// Live time for this sound.  In microseconds.
		uint64_t										m_ui64Time;

		// Dead time for this sound.  In microseconds.  Resets on each play command.
		//	Only accumulates while stopped.
		uint64_t										m_ui64DeadTime;

		// Length of the sound we are playing.
		uint64_t										m_ui64SoundTime;

		// Expiration time.  If we are dead for this amount of time then we are eligable
		//	for automatic removal.
		uint64_t										m_ui64ExpirationDate;

		// Position of the source.
		CVector3										m_vPos;

		// Velocity of the source.
		CVector3										m_vVel;

		// Direction of the source.
		CVector3										m_vDir;

		// Pitch multiplier.
		LSSREAL											m_fPitch;

		// Gain.
		LSSREAL											m_fGain;

		// Max distance.
		LSSREAL											m_fDist;

		// Rolloff.
		LSSREAL											m_fRolloff;

		// Minimum gain.
		LSSREAL											m_fMinGain;

		// Maximum gain.
		LSSREAL											m_fMaxGain;

		// Gain outside of the oriented cone.
		LSSREAL											m_fOuterGain;

		// Inner cone angle.
		LSSREAL											m_fInnerAngle;

		// Cone angle in degrees.
		LSSREAL											m_fConeAngle;

		// Are we dirty?  Update on the next tick if we are.
		LSBOOL											m_bDirty;

		// Our unique ID.
		uint32_t										m_ui32Id;

		// Our state.
		LSS_SOUND_STATE									m_ssState;

		// Last state (synchronous with the main game thread).
		LSS_SOUND_STATE									m_ssLastStateSynchronous;

		// Did we just begin (synchronous with the main game thread)?
		LSBOOL											m_bJustBeganSynchronous;

		// Did we just end (synchronous with the main game thread)?
		LSBOOL											m_bJustEndedSynchronous;

		// Repeat count.  Use 1 for no looping.  Use LSS_LM_INFINITE
		//	for infinite looping.
		uint32_t										m_ui32LoopCount;

		// Last loop mode sent to the sound system.  Sound systems only have loop or not loop.
		LSBOOL											m_bSoundSystemLooping;

#ifdef LSS_USEOPENAL
		// The OpenAL ID for this buffer.
		// Created with ::alGenBuffers().
		ALuint											m_uiAlSourceId;

#endif	// #ifdef LSS_USEOPENAL

		// The unique ID counter.
		static uint32_t									m_ui32IdCounter;


		// == Functions.
		/**
		 * Apply states to the hardware or sound system (OpenAL for example).
		 * Does not clear the dirty flag.
		 */
		virtual void LSE_CALL							ApplyStates();

		/**
		 * Update the looping status.
		 */
		void LSE_CALL									UpdateLoop();

	
		// == Functions.
		/**
		 * A device has been created.  We can register with the hardware.
		 * The sound manager notifies us of this event after the device has
		 *	been changed.  If there is a device already when we are created,
		 *	we are notified via this function immediately after being created.
		 *
		 * \return Returns false if there is an error uploading our sound data to the hardware.
		 */
		virtual LSBOOL LSE_CALL							DeviceActivated();

		/**
		 * Notifies us that the device is about to be lost (changed).  We need
		 *	to sever our connections to it, although we may retain our data/
		 *	settings.
		 */
		virtual void LSE_CALL							DeviceWillClose();

		/**
		 * Enter a deletable state.  In some sound systems, the sound sources
		 *	cannot be removed from the system under some circumstances, such as
		 *	when they are playing.  Use this function to enter a state that
		 *	allows deletion of the object.
		 */
		virtual void LSE_CALL							EnterDeletableState() const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CSoundSource::CSoundSource( uint32_t _ui32Id ) :
		m_fPitch( static_cast<LSSREAL>(1.0) ),
		m_fGain( static_cast<LSSREAL>(0.0) ),
		m_fDist( static_cast<LSSREAL>(100.0) ),
		m_fRolloff( static_cast<LSSREAL>(1.0) ),
		m_fMinGain( static_cast<LSSREAL>(0.0) ),
		m_fMaxGain( static_cast<LSSREAL>(1.0) ),
		m_fOuterGain( static_cast<LSSREAL>(0.0) ),
		m_fInnerAngle( static_cast<LSSREAL>(360.0) ),
		m_fConeAngle( static_cast<LSSREAL>(360.0) ),
		m_bDirty( true ),
		m_ui32Id( LSS_E_INVALID_BUFFER ),
		m_ui64Time( 0ULL ),
		m_ui64DeadTime( 0ULL ),
		m_ui64SoundTime( 0ULL ),
		m_ui64ExpirationDate( LSS_NONEXPIRABLE ),
		m_ssState( LSS_SS_STOPPED ),
		m_bJustBeganSynchronous( false ),
		m_bJustEndedSynchronous( false ),
		m_ui32LoopCount( 1 ),
		m_bSoundSystemLooping( false ),
#ifdef LSS_USEOPENAL
		m_uiAlSourceId( LSS_INVALID_BUFFER )
#endif	// #ifdef LSS_USEOPENAL
		{
		m_vPos[0] = m_vPos[1] = m_vPos[2] = LSM_ZERO;
		m_vVel[0] = m_vVel[1] = m_vVel[2] = LSM_ZERO;
		m_vDir[0] = m_vDir[1] = LSM_ZERO;
		m_vDir[2] = -LSM_ONE;

		if ( _ui32Id != 0ULL ) {
			m_ui32IdCounter = _ui32Id;
			if ( _ui32Id < LSS_UNIQUE_ID ) {
				CStd::DebugPrintA( "CSoundSource::CSoundSource(): The unique ID given must be equal to or above LSS_UNIQUE_ID.\r\n" );
				assert( 0 );
			}
		}
		else {
			m_ui32IdCounter = ((m_ui32IdCounter + 1) % (LSS_UNIQUE_ID - 1)) + 1;
			m_ui32Id = m_ui32IdCounter;
		}
	}
	LSE_INLINE LSE_CALLCTOR CSoundSource::~CSoundSource() {
		EnterDeletableState();
#ifdef LSS_USEOPENAL
		// Delete existing source.
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alDeleteSources( 1, &m_uiAlSourceId );
			assert( ::alIsSource( m_uiAlSourceId ) == AL_FALSE );
			m_uiAlSourceId = LSS_INVALID_BUFFER;
		}
#endif	// #ifdef LSS_USEOPENAL
	}

	// == Functions.
	/**
	 * Did this sound just begin playing as of the last synchronous update?
	 *
	 * \return Returns true if the sound started playing since the last main-thread update.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSoundSource::JustStarted() const {
		return m_bJustBeganSynchronous;
	}

	/**
	 * Did this sound just stop playing as of the last synchronous update?
	 *
	 * \return Returns true if the sound stopped playing since the last main-thread update.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSoundSource::JustStopped() const {
		return m_bJustEndedSynchronous;
	}

}	// namespace lss

#endif	// __LSS_SOUNDSOURCE_H__
