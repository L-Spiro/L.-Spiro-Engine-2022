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
 * Description: A source used for streaming.  Provides an interface for continuous loading of buffer data in
 *	order to keep the sound playing.  By default, streaming sounds ignore soft resets.
 */

#include "LSSStreamingSoundSource.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// The number of buffers we use.
#define LSS_STREAMINGBUFFERS				4

namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CStreamingSoundSource::CStreamingSoundSource( uint32_t _ui32Id ) :
		m_paAllocator( NULL ),
		m_pssssSource( NULL ),
		m_ui64BufferSize( 0ULL ),
		m_ui64Queued( 0ULL ),
		m_bHaveDevice( false ),
		Parent( _ui32Id ) {
	}
	LSE_CALLCTOR CStreamingSoundSource::~CStreamingSoundSource() {
		Reset();
	}

	// == Functions.
	/**
	 * Gets the type of this source.
	 *
	 * \return Returns one of the LSS_SOUND_SOURCE values.
	 */
	LSS_SOUND_SOURCE LSE_CALL CStreamingSoundSource::GetType() const {
		return LSS_SS_STREAMING;
	}

	/**
	 * Is this source static (non-streaming)?
	 *
	 * \return Returns true if the source is static, false if streaming.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::Static() const {
		return false;
	}

	/**
	 * This class uses its own private buffers internally.  We override this function to make it a
	 *	do-nothing function.
	 *
	 * \param _psbBuffer Not used.
	 */
	void LSE_CALL CStreamingSoundSource::SetBuffer( const CSoundBuffer * /*_psbBuffer*/ ) {
		// Do nothing.
	}

	/**
	 * Creates a stream of audio using the provided IStreamingSoundSourceStreamer interface.
	 *
	 * \param _pssssSource The interface that provides this class with the data it needs to stream audio.
	 * \param _paAllocator The allocator to be used for all allocations made by this class.  Must not be NULL.
	 * \return Returns true if the creation of the sound stream succeeded.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::CreateStream( const IStreamingSoundSourceStreamer * _pssssSource,
		CAllocator * _paAllocator ) {

		assert( _paAllocator );

		Reset();

		SetSource( _pssssSource );
		m_paAllocator = _paAllocator;
		m_sblBuffers.SetAllocator( m_paAllocator );

		if ( !_pssssSource ) { return true; }

		// Determine how many bytes are needed to fill one buffer.
		m_ui64BufferSize = LSS_STREAMING_BUFFER_SIZE * (_pssssSource->GetBits() >> 3) * _pssssSource->GetChannels() *
			_pssssSource->GetFrequency() / 1000000ULL;

		/*
		uint64_t ui64TotalBytes = _pssssSource->GetAudioLength() * (_pssssSource->GetBits() >> 3) * _pssssSource->GetChannels() *
			_pssssSource->GetFrequency() / 1000000ULL;

		uint32_t ui32Buffers = static_cast<uint32_t>(ui64TotalBytes / ui64BytesForBuffer);
		if ( ui64TotalBytes % ui64BytesForBuffer != 0ULL ) {
			ui32Buffers++;
		}
		ui32Buffers = CStd::Min<uint32_t>( ui32Buffers, LSS_STREAMINGBUFFERS );*/
		if ( !m_sblBuffers.Resize( LSS_STREAMINGBUFFERS ) ) {
			return false;
		}
		for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
			m_sblBuffers[I].bUsed = false;
			m_sblBuffers[I].psbBuffer = NULL;
		}
		m_ui64SoundTime = _pssssSource->GetAudioLength();
		// We have queued nothing.
		m_ui64Queued = 0ULL;
		return true;
	}

	/**
	 * Reset the stream.  Deallocates everything.
	 */
	void LSE_CALL CStreamingSoundSource::Reset() {
		SetSource( NULL );

		if ( m_paAllocator ) {
			for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
				if ( m_sblBuffers[I].psbBuffer ) {
					UnBuffer( I );
				}
			}
		}
		m_sblBuffers.Reset();

		m_ui64Time = 0ULL;
		m_ui64Queued = 0ULL;
		m_bDirty = true;
	}

	/**
	 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
	 * Time is given in microseconds.
	 *
	 * \param _ui64TimeSinceLastUpdate Microseconds by which to update.
	 */
	void LSE_CALL CStreamingSoundSource::Tick( uint64_t _ui64TimeSinceLastUpdate ) {
		// Remove buffers.
		CheckUnqueue();

		// Add buffers.
		CheckQueue();


		Parent::Tick( _ui64TimeSinceLastUpdate );
	}

	/**
	 * Begin playing the sound.  If there is no data source, returns false.  The sound plays regardless
	 *	of whether or not there is a sound device.  If there is no sound device, the sound will begin
	 *	playing at the correct position when a device becomes active.  The sound begins playing on the
	 *	next tick event.
	 *
	 * \param _ui32LoopCount Number of times to loop the sound.
	 * \return Returns true if the play was successful.  A return of false indicates that no data source was
	 *	applied to this source.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::Play( uint32_t _ui32LoopCount ) {
		if ( !m_pssssSource ) { return false; }
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
	 * Gets the streamer associated with this sound stream.
	 *
	 * \return Returns the streamer used by this source, which may be NULL.
	 */
	const IStreamingSoundSourceStreamer * LSE_CALL CStreamingSoundSource::GetStreamer() const {
		return m_pssssSource;
	}

	/**
	 * Sets the value indicating whether this source should resist soft resets or not.  A soft reset
	 *	is a reset that stops sources that do not resist soft resets, then releases all unreferenced
	 *	data (sources/buffers).  This is not the same as a soft unload, which can happen at any time
	 *	just to save RAM and works only on expired data.  A soft reset will stop sources that do not
	 *	resist the stopping action by returning true via this function.
	 *
	 * \return Returns false.  Override this class to return any other value.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::CanSoftReset() const {
		return false;
	}

	/**
	 * Sets the source source.
	 *
	 * \param _pssssSource The source to use for the data we need to operate as a streaming sound source.
	 */
	void LSE_CALL CStreamingSoundSource::SetSource( const IStreamingSoundSourceStreamer * _pssssSource ) {
		if ( m_pssssSource ) {
			m_pssssSource->DecRef();
		}

		m_pssssSource = _pssssSource;
		if ( m_pssssSource ) {
			m_pssssSource->IncRef();
		}
	}

	/**
	 * Check to see if we need to queue any buffers.  If so, buffers are automatically queued.
	 */
	void LSE_CALL CStreamingSoundSource::CheckQueue() {
		if ( !m_pssssSource ) { return; }
		if ( !m_ui64SoundTime ) { return; }
		if ( m_ssState != LSS_SS_PLAYING ) { return; }

		// How far into the sound should we have queued?
		uint64_t ui64MicrosShouldHaveQueued = m_ui64Time;
		// Round up to the nearest full chunk.
		ui64MicrosShouldHaveQueued /= LSS_STREAMING_BUFFER_SIZE;
		ui64MicrosShouldHaveQueued++;
		ui64MicrosShouldHaveQueued *= LSS_STREAMING_BUFFER_SIZE;
		if ( m_ui64Time % LSS_STREAMING_BUFFER_SIZE != 0ULL ) {
			ui64MicrosShouldHaveQueued += LSS_STREAMING_BUFFER_SIZE;
		}


		uint64_t ui64SoundTimeRemaining;
		if ( m_ui32LoopCount != LSS_LM_INFINITE ) {
			// Max time in the sound.
			ui64SoundTimeRemaining = m_ui64SoundTime * m_ui32LoopCount;

			// Clamp the time we should queue.
			ui64MicrosShouldHaveQueued = CStd::Min( ui64MicrosShouldHaveQueued, ui64SoundTimeRemaining );
		}

		// Queue all that need to be queued.
		uint64_t ui64QueueMe = 0ULL;

		for ( uint64_t ui64CurQueue = m_ui64Queued; ui64CurQueue < ui64MicrosShouldHaveQueued;
			ui64MicrosShouldHaveQueued -= ui64QueueMe ) {
			ui64QueueMe = ui64MicrosShouldHaveQueued - ui64CurQueue;
			if ( ui64QueueMe > LSS_STREAMING_BUFFER_SIZE ) {
				ui64QueueMe = LSS_STREAMING_BUFFER_SIZE;
			}
			if ( !Buffer( m_ui64Queued, ui64QueueMe ) ) { return; }
		}
	}

	/**
	 * Check for buffers that need to be unqueued.  If any are found, they are unqueued.
	 */
	void LSE_CALL CStreamingSoundSource::CheckUnqueue() {
		if ( !m_paAllocator ) { return; }
#ifdef LSS_USEOPENAL
		if ( m_uiAlSourceId == LSS_INVALID_BUFFER ) { return; }

		::alGetError();
		ALint iVal = 0;
		::alGetSourcei( m_uiAlSourceId, AL_BUFFERS_PROCESSED, &iVal );

		if ( ::alGetError() != AL_NO_ERROR ) { return; }
		if ( iVal == 0 ) {
			return;
		}

		ALuint uiBuffers[LSS_STREAMINGBUFFERS];
		ALsizei sCnt = CStd::Min<ALsizei>( iVal, LSS_STREAMINGBUFFERS );
		// Clear the error flag.
		::alGetError();

		// Perform the unqueue on the hardware.
		::alSourceUnqueueBuffers( m_uiAlSourceId, sCnt, uiBuffers );


		ALenum eError = ::alGetError();
		if ( eError != AL_NO_ERROR ) {
			CStd::DebugPrintA( "CStreamingSoundSource::CheckUnqueue(): Buffers not removed.\r\n" );
		}
		else {
			for ( ALsizei I = 0; I < sCnt; ++I ) {
				LSBOOL bFound = false;
				for ( uint32_t J = m_sblBuffers.Length(); J--; ) {
					if ( m_sblBuffers[J].psbBuffer ) {
						if ( m_sblBuffers[J].psbBuffer->m_uiAlBufferId == uiBuffers[I] ) {
							UnBuffer( J );
							bFound = true;
							break;
						}
					}
				}
				if ( !bFound ) {
					CStd::DebugPrintA( "CStreamingSoundSource::CheckUnqueue(): Buffer not found in master list?\r\n" );
				}
			}
		}

#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Gets the oldest buffer still active.
	 *
	 * \return Returns the index of the running buffer with the lowest time stamp.  If no buffers are living, LSE_MAXU32 is returned.
	 */
	uint32_t LSE_CALL CStreamingSoundSource::GetOldestBufferIndex() const {
		uint32_t ui32Ret = LSE_MAXU32;
		uint64_t ui64Lowest = LSE_MAXU64;
		for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
			if ( !m_sblBuffers[I].bUsed ) { continue; }
			if ( m_sblBuffers[I].ui64StartTime <= ui64Lowest ) {
				ui32Ret = I;
				ui64Lowest = m_sblBuffers[I].ui64StartTime;
			}
		}
		return ui32Ret;
	}

	/**
	 * Find the first free buffer.
	 *
	 * \return Returns the first free buffer.
	 */
	uint32_t LSE_CALL CStreamingSoundSource::GetFreeBuffer() const {
		for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
			if ( !m_sblBuffers[I].bUsed ) { return I; }
		}
		return LSE_MAXU32;
	}

	/**
	 * Buffer a given amount of time in the sound.  This function finds the first available buffer
	 *	and attempts to buffer the requested data into it.
	 *
	 * \param _ui64From Time at the beginning of the buffer, in microseconds.
	 * \param _ui64Length Length to buffer, in microseconds.
	 * \return If buffering failed or there are no available buffers, returns false.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::Buffer( uint64_t _ui64From, uint64_t _ui64Length ) {
		// We must have a data source and an allocator.
		assert( m_pssssSource );
		assert( m_paAllocator );
		assert( _ui64Length );

		// First find a free buffer.
		uint32_t ui32Index = GetFreeBuffer();
		if ( ui32Index == LSE_MAXU32 ) {
			//CStd::DebugPrintA( "CStreamingSoundSource::Buffer(): No free slots.\r\n" );
			return false;
		}

		uint8_t * pui8Buffer( NULL );
		uint64_t ui64Len = _ui64Length;
		// Get the bytes.  GetData() converts _ui64Length to bytes on return.
		if ( !m_pssssSource->GetData( _ui64From, _ui64Length, pui8Buffer, m_paAllocator ) ) {
			CStd::DebugPrintA( "CStreamingSoundSource::Buffer(): Failed to get data.\r\n" );
			return false;
		}

		// Got the data.  Construct the buffer.
		m_sblBuffers[ui32Index].psbBuffer = static_cast<CSoundBuffer *>(m_paAllocator->Alloc( sizeof( (*m_sblBuffers[ui32Index].psbBuffer) ) ));
		if ( !m_sblBuffers[ui32Index].psbBuffer ) {
			// Failed to allocate the buffer.  Release what we allocated before and end.
			m_paAllocator->Free( pui8Buffer );
			CStd::DebugPrintA( "CStreamingSoundSource::Buffer(): Failed to allocate a buffer.\r\n" );
			return false;
		}

		new( m_sblBuffers[ui32Index].psbBuffer ) CSoundBuffer( pui8Buffer, static_cast<uint32_t>(_ui64Length),
			m_pssssSource->GetFrequency(), m_pssssSource->GetBits(), m_pssssSource->GetChannels(),
			m_paAllocator, true, 0 );
		m_paAllocator->Free( pui8Buffer );
		m_sblBuffers[ui32Index].ui64StartTime = _ui64From;
		m_sblBuffers[ui32Index].ui64Length = ui64Len;
		m_sblBuffers[ui32Index].bUsed = true;
		if ( m_bHaveDevice ) {
			if ( !m_sblBuffers[ui32Index].psbBuffer->DeviceActivated() ) {
				CStd::DebugPrintA( "CStreamingSoundSource::Buffer(): Buffer failed to register with hardware.\r\n" );
				UnBuffer( ui32Index );
				return false;
			}
		}

#ifdef LSS_USEOPENAL
		if ( m_uiAlSourceId != LSS_INVALID_BUFFER ) {
			::alGetError();
			::alSourceQueueBuffers( m_uiAlSourceId, 1, &m_sblBuffers[ui32Index].psbBuffer->m_uiAlBufferId );

			ALuint uiError = ::alGetError();
			if ( uiError != AL_NO_ERROR ) {
				CStd::DebugPrintA( "CStreamingSoundSource::Buffer(): Applying buffer to streaming source failed.\r\n" );
				UnBuffer( ui32Index );
				return false;
			}
			m_bPlayStateDirty = true;
		}
#endif	// #ifdef LSS_USEOPENAL


		m_ui64Queued += ui64Len;
		return true;
	}

	/**
	 * Unbuffer an a buffer.
	 *
	 * \param _ui32Index The index of the buffer to remove.
	 */
	void LSE_CALL CStreamingSoundSource::UnBuffer( uint32_t _ui32Index ) {
		if ( !m_sblBuffers[_ui32Index].psbBuffer ) { return; }
		assert( m_paAllocator );

		m_sblBuffers[_ui32Index].psbBuffer->~CSoundBuffer();
		m_paAllocator->Free( m_sblBuffers[_ui32Index].psbBuffer );
		m_sblBuffers[_ui32Index].psbBuffer = NULL;
		m_sblBuffers[_ui32Index].bUsed = false;
	}

	/**
	 * A device has been created.  We can register with the hardware.
	 * The sound manager notifies us of this event after the device has
	 *	been changed.  If there is a device already when we are created,
	 *	we are notified via this function immediately after being created.
	 *
	 * \return Returns false if there is an error uploading our sound data to the hardware.
	 */
	LSBOOL LSE_CALL CStreamingSoundSource::DeviceActivated() {
		m_bHaveDevice = true;
		for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
			if ( m_sblBuffers[I].psbBuffer ) {
				m_sblBuffers[I].psbBuffer->DeviceActivated();
			}
		}
		return Parent::DeviceActivated();
	}

	/**
	 * Notifies us that the device is about to be lost (changed).  We need
	 *	to sever our connections to it, although we may retain our data/
	 *	settings.
	 */
	void LSE_CALL CStreamingSoundSource::DeviceWillClose() {
		m_bHaveDevice = false;
		for ( uint32_t I = m_sblBuffers.Length(); I--; ) {
			if ( m_sblBuffers[I].psbBuffer ) {
				m_sblBuffers[I].psbBuffer->DeviceWillClose();
			}
		}
		Parent::DeviceWillClose();
	}

}	// namespace lss
