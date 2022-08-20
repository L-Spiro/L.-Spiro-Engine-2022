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
 * Description: Buffers are the shared sound data.  Load a WAV or OGG (etc.) file and its raw PCM data will
 *	be held in a buffer which sound sources (CSoundSource) can use to play.  This is shared data, following
 *	exactly the same concept as with 3D models, which are loaded once and then instanced.  This class forms
 *	the shared sound data and CSoundSource forms the instances of the sound data played at various locations
 *	in the game world.
 * This class cannot be instanced directly.  Use CSoundManager to create instances of these.
 *
 * There are 2 possible system states when these objects are created: One where there is a valid device
 *	already set and one without a valid device.  Additionally, these buffers must support changing devices
 *	during run-time.  Normally we could impose a restriction that requires a device to be set when buffers
 *	are created, but the fact is, because we need a system that allows changing devices anyway, it becomes
 *	very simple to modify the class just slightly so that it can handle both cases instead of only one.
 *	This allows us to impose no restrictions on the device state with no extra development time.  Nice!
 *
 * When the buffer is created it will be given a pointer to data which it will copy and store.  The buffer
 *	can be set to keep the copy for forever or to free it once it has sent the copy to the hardware.
 *	Creating a buffer does not automatically make it try to send anything to hardware.  The sound manager
 *	will, as soon as a device is ready (which will be immediately if there is already a device) notify
 *	the buffer that there is a device, and the buffer will then upload its contents.  If set to free its
 *	copy it will do so then.
 * The same notification is used to tell the buffer that a new device has been set, and the buffer will
 *	react the same way by uploading and optionally freeing its copy.
 *
 * When the buffer needs to upload its copy to the hardware, it may have a NULL copy as a result of having
 *	uploaded and freed it previously.  In this case the buffer calls the virtual function GetOriginalData().
 *	the default implementation returns failure and the re-upload will fail.  Therefore, if this class is not
 *	subclassed, and the user has the ability to change devices, the buffers must never be set to automatically
 *	free their copies.
 * If the device can be changed, in order to save RAM this class should be subclassed so that the buffers may
 *	be flushed after being sent to the sound system.
 */

#include "LSSSoundBuffer.h"


namespace lss {

	// == Members.
	// ID counter.
	uint32_t CSoundBuffer::m_ui32MasterId = LSS_E_INVALID_BUFFER;


	// == Various constructors.
	LSE_CALLCTOR CSoundBuffer::CSoundBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
		uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, CAllocator * _paAllocator, LSBOOL _bRetain, uintptr_t /*_uiptrUserData*/ ) :
		m_ui64BufferTime( 0ULL ),
		m_pui8Data( NULL ),
		m_bRetain( _bRetain ),
		m_ui32Freq( _ui32Freq ),
		m_ui16BitDepth( static_cast<uint16_t>(_ui32Bits) ),
		m_ui16Channels( static_cast<uint16_t>(_ui32Channels) ),
#ifdef LSS_USEOPENAL
		m_uiAlBufferId( LSS_INVALID_BUFFER ),
#endif	// #ifdef LSS_USEOPENAL
		m_paAllocator( _paAllocator ),
		m_i32Ref( 0 ) {
		m_ui32Id = ++m_ui32MasterId;

		_ui32Length = (_ui32Length / _ui32Channels) / (m_ui16BitDepth >> 3);
		_ui32Length = _ui32Length * _ui32Channels * (m_ui16BitDepth >> 3);
		m_ui32Size = _ui32Length;

		m_pui8Data = static_cast<uint8_t *>(m_paAllocator->Alloc( _ui32Length ));
		if ( m_pui8Data ) {
			CStd::MemCpy( m_pui8Data, _pui8Data, _ui32Length );

			m_ui64BufferTime = (((_ui32Length / _ui32Channels) * 1000000ULL) / (m_ui16BitDepth >> 3))
				/ m_ui32Freq;
		}
		else {
			m_ui32Id = LSS_E_INVALID_BUFFER;
		}

	}
	LSE_CALLCTOR CSoundBuffer::~CSoundBuffer() {
#ifdef LSS_USEOPENAL
		if ( m_uiAlBufferId != LSS_INVALID_BUFFER ) {
			::alDeleteBuffers( 1, &m_uiAlBufferId );
			m_uiAlBufferId = LSS_INVALID_BUFFER;
		}
#endif	// #ifdef LSS_USEOPENAL
		if ( m_paAllocator ) {	// Always passes.
			m_paAllocator->Free( m_pui8Data );
			m_pui8Data = NULL;
		}
	}
	
	// == Functions.
	/**
	 * Gets our ID.
	 *
	 * \return Returns the unique ID of this buffer.
	 */
	uint32_t LSE_CALL CSoundBuffer::GetId() const {
		return m_ui32Id;
	}

	/**
	 * Increase our reference count.
	 */
	void LSE_CALL CSoundBuffer::IncRef() const {
		++m_i32Ref;
	}

	/**
	 * Decrease our reference count.
	 */
	void LSE_CALL CSoundBuffer::DecRef() const {
		m_i32Ref--;
		assert( m_i32Ref >= 0 );
	}

	/**
	 * Gets our reference count.
	 *
	 * \return Returns the reference count of this object.
	 */
	uint32_t LSE_CALL CSoundBuffer::GetRef() const {
		return static_cast<uint32_t>(m_i32Ref);
	}

	/**
	 * Gets the length of this buffer in microseconds.
	 *
	 * \return Returns the length of the buffer in microseconds.
	 */
	uint64_t LSE_CALL CSoundBuffer::GetLengthInMicros() const {
		return m_ui64BufferTime;
	}

	/**
	 * Sets the value indicating whether this buffer should resist soft unloads or not.  A soft unload is
	 *	an unload that is used to save RAM even though it is assumed the buffer may be needed (and will be
	 *	reloaded) later.  A hard unload is performed to force all buffers out of memory.  A hard unload
	 *	always destroys and releases the buffer, whereas a soft unload is optional and can only be done on
	 *	buffers with a zero reference count.  For a soft unload to work, there must be a way to restore the
	 *	buffer to its current state after being completely unloaded.  Because the core sound library
	 *	provides no way to do this, this function always returns false.
	 *
	 * \return Returns false.  Override this class to return any other value.
	 */
	LSBOOL LSE_CALL CSoundBuffer::CanSoftUnload() const { return false; }

	/**
	 * Notifies us that the given device has become active and
	 *	we should send our buffer to it.  A return of false from this
	 *	function indicates the buffer could not be uploaded.  This is
	 *	considered a critical error by the engine.
	 *
	 * \return Returns true if the buffer could be reloaded.
	 */
	LSBOOL LSE_CALL CSoundBuffer::DeviceActivated() {
		if ( !m_pui8Data ) {
			if ( !GetOriginalData() || !m_pui8Data ) {
				CStd::DebugPrintA( "CSoundBuffer::DeviceActivated(): No data.\r\n" );
				return false;
			}
		}

#ifdef LSS_USEOPENAL
		// Clear error flag.
		::alGetError();

		// m_pui8Data is not NULL.  Upload it.
		::alGenBuffers( 1, &m_uiAlBufferId );
		if ( ::alGetError() != AL_NO_ERROR ) {
			m_uiAlBufferId = LSS_INVALID_BUFFER;
			CStd::DebugPrintA( "CSoundBuffer::DeviceActivated(): Error in ::alGenBuffers().\r\n" );
			return false;
		}

		// Find the OpenAL format that matches the bit-depth and number of channels we have.
		ALenum eType = 0;
		for ( uint32_t I = 0; I < CSoundLib::LSS_C_TOTAL; ++I ) {
			if ( CSoundLib::ChannelArrangementSupported( static_cast<CSoundLib::LSS_CHANNELS>(I) ) ) {
				if ( CSoundLib::GetBitsPerChannel( static_cast<CSoundLib::LSS_CHANNELS>(I) ) == m_ui16BitDepth ) {
					if ( CSoundLib::GetChannels( static_cast<CSoundLib::LSS_CHANNELS>(I) ) == m_ui16Channels ) {
						eType = CSoundLib::GetOsChannelEnum( static_cast<CSoundLib::LSS_CHANNELS>(I) );
						break;
					}
				}
			}
		}

		if ( !eType ) {
			CStd::DebugPrintA( "CSoundBuffer::DeviceActivated(): Unable to find matching channel arrangement.\r\n" );
			return false;
		}


		// Upload to hardware.
		::alBufferData( m_uiAlBufferId, eType, m_pui8Data, m_ui32Size, m_ui32Freq );
		ALenum eError = ::alGetError();
		if ( eError != AL_NO_ERROR ) {
			::alDeleteBuffers( 1, &m_uiAlBufferId );
			m_uiAlBufferId = LSS_INVALID_BUFFER;
			CStd::DebugPrintA( "CSoundBuffer::DeviceActivated(): Error in ::alBufferData().\r\n" );
			return false;
		}

		// Remove our local copy if we are not meant to retain the data.
		if ( !m_bRetain ) {
			m_paAllocator->Free( m_pui8Data );
			m_pui8Data = NULL;
		}
#endif	// #ifdef LSS_USEOPENAL

		return true;
	}

	/**
	 * Notifies us that the device is about to be lost (changed).  We need
	 *	to sever our connections to it, although we may retain our data/
	 *	settings.
	 */
	void LSE_CALL CSoundBuffer::DeviceWillClose() {
#ifdef LSS_USEOPENAL
		if ( m_uiAlBufferId != LSS_INVALID_BUFFER ) {
			::alDeleteBuffers( 1, &m_uiAlBufferId );
			m_uiAlBufferId = LSS_INVALID_BUFFER;
		}
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Override this to retrieve the buffer data to be sent to the hardware.
	 *	This is called when we free our original buffer and then the device
	 *	changes, requiring us to upload the data again.  This class does not
	 *	know from where the data came, so it cannot retrieve it.  Override
	 *	this function and add a method to get the data back (typically from
	 *	a file), or set this object to retain its buffer.
	 *
	 * \return Returns true if the original data could be retrieved.
	 */
	LSBOOL LSE_CALL CSoundBuffer::GetOriginalData() {
		return false;
	}

}	// namespace lss
