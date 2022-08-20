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
 * Description: The basic manager for sound throughout the engine.  This is the hotspot that organizes and
 *	manages all sound resources, sound objects, and sound devices.
 *
 * Run() and Stop() should be called from the same thread.
 */

#include "LSSSoundManager.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CSoundManager::CSoundManager() :
		m_paAllocator( NULL ),
		m_bAllocatorIsPrivate( true ),
		m_i32LockCount( 0 ),
		m_bRun( true )
#ifdef LSS_USEOPENAL
		, m_pdDevice( NULL )
		, m_pcContext( NULL )
#endif	// #ifdef LSS_USEOPENAL
		{

		CStd::MemSet( &m_hThread, 0, sizeof( m_hThread ) );
	}
	LSE_CALLCTOR CSoundManager::~CSoundManager() {
		ShutDown();
	}

	// == Functions.
	/**
	 * Sets the allocator used by the sound system.  This causes all resources
	 *	to be reset and lost.  This should be done only once at start-up,
	 *	before any resources are created.  _paAllocator may be NULL.
	 *
	 * \param _paAllocator The allocator to be used by this class or NULL to use the global heap.
	 */
	void LSE_CALL CSoundManager::SetAllocator( CAllocator * _paAllocator ) {
		ShutDown();

		if ( !_paAllocator ) {
			// If this throws an exception, fine.  We cannot run the sound engine
			//	without this allocator anyway.
			m_paAllocator = LSENEW CAllocator();
			m_bAllocatorIsPrivate = true;
		}
		else {
			m_bAllocatorIsPrivate = false;
			m_paAllocator = _paAllocator;
		}

		m_psbBuffers.SetAllocator( m_paAllocator );
		m_pssSources.SetAllocator( m_paAllocator );
		m_pssssStreamers.SetAllocator( m_paAllocator );
	}

	/**
	 * Gets our allocator.
	 *
	 * \return Returns the allocator used by the sound manager.
	 */
	CAllocator * LSE_CALL CSoundManager::GetAllocator() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		return m_paAllocator;
	}

	/**
	 * Sets the current device.  Causes all existing resources to be reloaded
	 *	automatically.  Only one device may be set at a time.  If _pcDevice
	 *	is NULL or an empty string, the default device is used.
	 *
	 * \param _pcDevice The device to set.
	 * \param _pstrError Returned error if any.
	 * \return Returns true if the device was set.
	 */
	LSBOOL LSE_CALL CSoundManager::SetDevice( const char * _pcDevice, CString * _pstrError ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		ResetDevice();

#ifdef LSS_USEOPENAL

		// Clear the error flag.
		::alGetError();
		
		const char * pcFinalDevice = _pcDevice;
		if ( pcFinalDevice ) {
			if ( !CStd::StrLen( pcFinalDevice ) ) {
				pcFinalDevice = NULL;
			}
		}
		m_pdDevice = ::alcOpenDevice( pcFinalDevice );

		if ( !m_pdDevice ) {
			if ( _pstrError ) {
				(*_pstrError) = "Error creating sound device.\r\n";
			}
			return false;
		}

		m_pcContext = ::alcCreateContext( m_pdDevice, NULL );
		if ( !m_pcContext ) {
			if ( _pstrError ) {
				(*_pstrError) = "Error creating context for sound device.\r\n";
			}
			ResetDevice();
			return false;
		}
		::alcMakeContextCurrent( m_pcContext );

		//::alDistanceModel( AL_NONE );

#endif	// #ifdef LSS_USEOPENAL


		// Inform all buffers that a new device has been set.
		for ( uint32_t I = 0; I < m_psbBuffers.Length(); ++I ) {
			m_psbBuffers[I]->DeviceActivated();
		}

		// Inform all sources that a new device has been set.
		for ( uint32_t I = 0; I < m_pssSources.Length(); ++I ) {
			m_pssSources.GetByIndex( I )->DeviceActivated();
		}

		// Inform the listener that a new device has been set.
		m_slListener.DeviceActivated();

		return true;
	}

	/**
	 * Runs the sound manager loop.  The loop is run on a new thread.
	 * This causes the sound manager to tick repeatedly, updating sources and the listener until requested
	 *	to stop.
	 * During a single tick, sources and the listener may not be accessed.  The loop rests for 16 milliseconds
	 *	between ticks, making spins/waits unlikely.
	 */
	void LSE_CALL CSoundManager::Run() {
		Stop();

		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		// Set the flag before continuing.
		m_bRun = true;

		if ( !CThreadLib::CreateThread( RunThread, this, &m_hThread ) ) {
			m_bRun = false;
			return;
		}
		
	}

	/**
	 * Stops the sound thread.  Does not return until the thread as been fully stopped and closed.
	 */
	void LSE_CALL CSoundManager::Stop() {
		if ( CThreadLib::ThreadHandleIsValid( m_hThread ) ) {
			// Tell the other thread to stop.
			m_csCriticalSection.Lock();
			m_bRun = false;
			m_csCriticalSection.UnLock();

			// Wait for the thread to stop.
			CThreadLib::WaitForThread( m_hThread, NULL );

			m_csCriticalSection.Lock();
			CThreadLib::MakeInvalidHandle( m_hThread );
			m_csCriticalSection.UnLock();
		}
	}

	/**
	 * Tick once.  Updates time and all objects.
	 */
	void LSE_CALL CSoundManager::Tick() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		// Tick the listener.
		m_slListener.Tick();

		// Tick the sources.
		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			m_pssSources.GetByIndex( I )->Tick( m_tTime.GetMicrosSinceLastFrame() );
			if ( m_pssSources.GetByIndex( I )->Expired() ) {
				DeleteSource( m_pssSources.GetByIndex( I ) );
				// Remove it from the list.
				m_pssSources.Remove( I );
			}
		}

		// Update time.
		m_tTime.Update( false );
	}

	/**
	 * Perform a synchronous update of all sound sources.  This is always executed on the main game thread and is used
	 *	for issuing synchronous game events such as the beginning/ending of sounds.
	 *
	 * \param _uiptrUserPtr User data passed during the update on the main thread.
	 * \param _pfHandler Pointer to a function used to handle events synchronously.
	 */
	void LSE_CALL CSoundManager::SynchronousUpdate( uintptr_t _uiptrUserPtr,
		PfSoundEventHandler _pfHandler ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		// Tick the sources.
		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			m_pssSources.GetByIndex( I )->SynchronousUpdate( _uiptrUserPtr );
			if ( _pfHandler ) {
				// Sounds must never be deleted from event handlers.
				if ( m_pssSources.GetByIndex( I )->JustStarted() ) {
					_pfHandler( _uiptrUserPtr, m_pssSources.GetByIndex( I ), LSS_ET_SOUND_JUST_STARTED, this );
				}
				if ( m_pssSources.GetByIndex( I )->JustStopped() ) {
					_pfHandler( _uiptrUserPtr, m_pssSources.GetByIndex( I ), LSS_ET_SOUND_JUST_ENDED, this );
				}
			}
		}
	}

	/**
	 * Stop and remove all sources and buffers, and apply default parameters to the listener.
	 */
	void LSE_CALL CSoundManager::ResetAllSounds() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		assert( m_i32LockCount == 0 );

		// Delete all sources.
		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			DeleteSource( m_pssSources.GetByIndex( I ) );
		}
		m_pssSources.Reset();


		// Delete all buffers.
		for ( uint32_t I = m_psbBuffers.Length(); I--; ) {
			if ( m_psbBuffers[I]->GetRef() != 0 ) {
				CStd::DebugPrintA( "Sound buffer not released!" );
			}

			DeleteBuffer( m_psbBuffers[I] );
		}
		m_psbBuffers.Reset();

		// Delete all streamers.
		for ( uint32_t I = m_pssssStreamers.Length(); I--; ) {
			DeleteStreamer( m_pssssStreamers[I] );
		}
		m_pssssStreamers.Reset();

		m_slListener.SetPos( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0) ) );
		m_slListener.SetVel( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0) ) );
		m_slListener.SetOrientation( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(1.0) ),
			CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(1.0), static_cast<LSSREAL>(0.0) ) );
		m_slListener.SetGain( static_cast<LSSREAL>(1.0) );
	}

	/**
	 * Perform a soft reset.  This removes unreferenced data and stops sounds that are not
	 *	soft-reset-safe.
	 */
	void LSE_CALL CSoundManager::SoftReset() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		assert( m_i32LockCount == 0 );

		// Stop and delete sources that cannot withstand a soft reset.
		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			if ( m_pssSources.GetByIndex( I )->CanSoftReset() ) {
				DeleteSource( m_pssSources.GetByIndex( I ) );
				m_pssSources.Remove( I );
			}
		}
		if ( !m_pssSources.Length() ) {
			m_pssSources.Reset();
		}

		// Delete all unreferenced buffers.
		for ( uint32_t I = m_psbBuffers.Length(); I--; ) {
			if ( m_psbBuffers[I]->GetRef() == 0 ) {
				DeleteBuffer( m_psbBuffers[I] );
				m_psbBuffers.Remove( I );
			}
		}
		if ( !m_psbBuffers.Length() ) {
			m_psbBuffers.Reset();
		}

		// Delete all unreferenced streamers.
		for ( uint32_t I = m_pssssStreamers.Length(); I--; ) {
			if ( m_pssssStreamers[I]->GetRef() == 0 ) {
				DeleteStreamer( m_pssssStreamers[I] );
				m_pssssStreamers.Remove( I );
			}
		}
		if ( !m_pssssStreamers.Length() ) {
			m_pssssStreamers.Reset();
		}

		// Reset the listener, with the exception of the gain.
		m_slListener.SetPos( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0) ) );
		m_slListener.SetVel( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0) ) );
		m_slListener.SetOrientation( CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(1.0) ),
			CVector3( static_cast<LSSREAL>(0.0), static_cast<LSSREAL>(1.0), static_cast<LSSREAL>(0.0) ) );
	}

	/**
	 * Adds a buffer with the given data.  No device needs to be set.  The data will be uploaded to the hardware
	 *	when a device is available.  Returns the ID of the buffer or CSoundBuffer::LSS_E_INVALID_BUFFER.
	 * _uiptrUserData is user data to be supplied to the created buffer.  It has no meaning in the context of
	 *	this library.  Use it to pass special application-specific data to a custom (subclassed) CSoundBuffer
	 *	object such as a file name etc.
	 *	** Must not be locked.
	 *
	 * \param _pui8Data The buffer data.
	 * \param _ui32Length Number of bytes in the buffer.
	 * \param _ui32Freq Frquency of the buffer.
	 * \param _ui32Bits Buffer bit count.
	 * \param _ui32Channels Number of channels in the buffer.
	 * \param _bRetain If true, the buffer will keep a copy of the buffer even after it has sent it to the
	 *	hardware.  This uses more RAM, but if the device is lost the buffer can reset its data on its own.
	 * \param _uiptrUserData User data.
	 * \param _pstrError Holds the optional returned error string if an error occurs.
	 * \return Returns the ID of the buffer or CSoundBuffer::LSS_E_INVALID_BUFFER.
	 */
	uint32_t LSE_CALL CSoundManager::AddBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
		uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain, uintptr_t _uiptrUserData, CString * _pstrError ) {

		// Create the new buffer with the data.
		CSoundBuffer * psbBuffer = NewBuffer( _pui8Data, _ui32Length,
			_ui32Freq, _ui32Bits, _ui32Channels, _bRetain, _uiptrUserData );
		if ( !psbBuffer ) {
			if ( _pstrError ) {
				(*_pstrError) = "Failed to allocate buffer object (memory failure).\r\n";
			}
			return CSoundBuffer::LSS_E_INVALID_BUFFER;
		}

		if ( psbBuffer->GetId() == CSoundBuffer::LSS_E_INVALID_BUFFER ) {
			if ( _pstrError ) {
				(*_pstrError) = "Failed to copy sound buffer to buffer object (memory failure).\r\n";
			}
			DeleteBuffer( psbBuffer );
			return CSoundBuffer::LSS_E_INVALID_BUFFER;
		}

		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		// Must be locked.
		if ( m_i32LockCount != 0 ) {
			if ( _pstrError ) {
				(*_pstrError) = "Sound manager is locked!  Unable to add resources while locked.\r\n";
			}
			DeleteBuffer( psbBuffer );
			return CSoundBuffer::LSS_E_INVALID_BUFFER;
		}

		if ( !m_psbBuffers.Push( psbBuffer ) ) {
			if ( _pstrError ) {
				(*_pstrError) = "Failed to add buffer object to master list (memory failure).\r\n";
			}
			DeleteBuffer( psbBuffer );
			return CSoundBuffer::LSS_E_INVALID_BUFFER;
		}


		uint32_t ui32Id = psbBuffer->GetId();

#ifdef LSS_USEOPENAL
		if ( m_pdDevice ) {
			if ( !psbBuffer->DeviceActivated() ) {
				if ( _pstrError ) {
					(*_pstrError) = "Buffer object failed to upload data to hardware.\r\n";
				}
				m_psbBuffers.Pop();
				DeleteBuffer( psbBuffer );
				return CSoundBuffer::LSS_E_INVALID_BUFFER;
			}
		}

#endif	// #ifdef LSS_USEOPENAL


		return ui32Id;
	}

	/**
	 * Removes a buffer by ID.
	 *	** Must not be locked.
	 *
	 * \param _ui32Id ID of teh buffer to remove.
	 */
	void LSE_CALL CSoundManager::RemBuffer( uint32_t _ui32Id ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount == 0 );


		if ( !m_psbBuffers.Length() ) { return; }

		// Find the ID.
		CSoundBuffer ** ppsbBuffer = static_cast<CSoundBuffer **>(CSearch::BSearch( &_ui32Id,
			&m_psbBuffers[0],
			m_psbBuffers.Length(),
			sizeof( m_psbBuffers[0] ),
			reinterpret_cast<CSearch::PfBSearchFunc>( BufferCompare ),
			NULL ));
		if ( !ppsbBuffer ) { return; }

		// Get its index.
		uintptr_t uiptrIndex = static_cast<uintptr_t>(ppsbBuffer - &m_psbBuffers[0]);

		// Destroy it.
		DeleteBuffer( (*ppsbBuffer) );
		// Remove it from the list.
		m_psbBuffers.Remove( static_cast<uint32_t>(uiptrIndex) );
	}

	/**
	 * Removes all unreferenced buffers.
	 *	** Must not be locked.
	 *
	 * \return Returns the total number of buffers that are referenced by static sources and were not removed.
	 */
	uint32_t LSE_CALL CSoundManager::RemAllUnreferencedBuffers() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount == 0 );

		uint32_t ui32Total = 0;
		for ( uint32_t I = m_psbBuffers.Length(); I--; ) {
			if ( m_psbBuffers[I]->GetRef() == 0 ) {
				// Unreferenced.  Delete it.
				DeleteBuffer( m_psbBuffers[I] );
				// Remove it from the list.
				m_psbBuffers.Remove( I );
			}
			else if ( !AllReferencesToBufferAreStreaming( m_psbBuffers[I]->GetId() ) ) {
				// Referenced by a static buffer.
				++ui32Total;
			}
		}

		return ui32Total;
	}

	/**
	 * Determine if all references to a buffer are from streaming sources.
	 *
	 * \param _ui32BufferId The buffer ID to check.
	 * \return Returns false if any static sources reference the given buffer ID.
	 */
	LSBOOL LSE_CALL CSoundManager::AllReferencesToBufferAreStreaming( uint32_t _ui32BufferId ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			if ( m_pssSources.GetByIndex( I )->GetType() & LSS_SS_STATIC ) {
				CStaticSoundSource * psssThis = reinterpret_cast<CStaticSoundSource *>(m_pssSources.GetByIndex( I ));
				if ( psssThis->GetBufferId() == _ui32BufferId ) { return false; }
			}
		}

		return true;
	}

	/**
	 * Find a buffer given its ID.  Uses a binary search.
	 *	** Must be locked.
	 *
	 * \param _ui32Id The ID of the buffer to be found.
	 * \return Returns a pointer to the buffer if found, NULL otherwise.
	 */
	const CSoundBuffer * LSE_CALL CSoundManager::FindBufferById( uint32_t _ui32Id ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount >= 0 );

		if ( !m_psbBuffers.Length() ) { return NULL; }
		// Do the binary search.
		CSoundBuffer ** ppsbBuffer = static_cast<CSoundBuffer **>(CSearch::BSearch( &_ui32Id,
			&m_psbBuffers[0],
			m_psbBuffers.Length(),
			sizeof( m_psbBuffers[0] ),
			reinterpret_cast<CSearch::PfBSearchFunc>(BufferCompare),
			NULL ));
		if ( !ppsbBuffer ) { return NULL; }

		return (*ppsbBuffer);
	}

	/**
	 * Find a buffer given its ID.  Uses a binary search.  If the buffer pointer is returned, this object remains locked.
	 *	When done with the returned pointer, call UnLock().  The returned pointer must not be used after calling UnLock().
	 *
	 * \param _ui32Id The ID of the buffer to be found.
	 * \return Returns a pointer to the buffer if found, NULL otherwise.  If NULL is returned, do not call UnLock().
	 */
	const CSoundBuffer * LSE_CALL CSoundManager::FindBufferByIdAndLock( uint32_t _ui32Id ) const {
		Lock();

		const CSoundBuffer * psbRet = FindBufferById( _ui32Id );
		if ( psbRet ) {
			// Stay locked.
			return psbRet;
		}
		// Unlock and die.
		UnLock();
		return NULL;
	}

	/**
	 * Lock this object for thread-safe access to its contents.  Accessing buffers, sources, and the listener
	 *	while not locked will result in failure.
	 */
	void LSE_CALL CSoundManager::Lock() const {
		m_csCriticalSection.Lock();
		++m_i32LockCount;
	}

	/**
	 * Unlock its contents.  Items cannot be added while locked.
	 */
	void LSE_CALL CSoundManager::UnLock() const {
		--m_i32LockCount;
		assert( m_i32LockCount >= 0 );
		m_csCriticalSection.UnLock();
	}

	/**
	 * Gets the listener.  This allows reading and writing, so you can set its position/volume/etc.
	 *	directly.  Changes will take place on the next tick of the sound manager.
	 *	** Must be locked.
	 *
	 * \return Returns a reference to the listener that can be used to modify its state.
	 */
	CSoundListener & LSE_CALL CSoundManager::GetListener() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount >= 0 );
		return m_slListener;
	}

	/**
	 * Adds a source with the given data.  No buffer is attached to the source at this point, and the source
	 *	cannot yet be used to play sounds.  Returns the pointer to the created source.
	 * If the returned pointer is not NULL, you must call UnLock() when you are done with the returned
	 *	pointer.
	 *
	 * \param _ssType The type of the source to create.  Can be LSS_SS_3DSTATIC, LSS_SS_2DSTATIC, LSS_SS_1DSTATIC
	 *	LSS_SS_3DSTREAMING, LSS_SS_2DSTREAMING, or LSS_SS_1DSTREAMING.
	 * \param _sslpParms Parameters to apply to the loaded source.
	 * \param _pstrError If not NULL and there is an error creating the source, this will be filled with the error
	 *	text.
	 * \return Returns a pointer to the added source or NULL upon failure.  If NULL is returned, do not call
	 *	UnLock().
	 */
	CSoundSource * LSE_CALL CSoundManager::AddSourceAndLock( LSS_SOUND_SOURCE _ssType,
		const LSS_SOUND_SOURCE_LOAD_PARMS &_sslpParms, CString * _pstrError ) {
		// Lock.
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		CSoundSource * pssSource = NewSoundSource( _ssType, _sslpParms.ui32Id );
		if ( !pssSource ) {
			if ( _pstrError ) {
				(*_pstrError) = "Failed to create the new source.\r\n";
			}
			return NULL;
		}

		// Sources can actually have non-unique ID's at this point.
		// If the user has specified an ID, any existing sources with the same ID need to be removed.
		RemSource( pssSource->GetId() );
		assert( m_pssSources.ItemExists( pssSource->GetId() ) == false );
		m_pssSources.Insert( pssSource->GetId(), pssSource );

		pssSource->SetInnerConeAngle( _sslpParms.fInnerConeAngle );
		pssSource->SetConeAngle( _sslpParms.fConeAngle );
		pssSource->SetGain( _sslpParms.fGain );
		pssSource->SetOuterConeGain( _sslpParms.fOuterGain );
		pssSource->SetMaxGain( _sslpParms.fMaxGain );
		pssSource->SetMinGain( _sslpParms.fMinGain );
		pssSource->SetPitchMultiplier( _sslpParms.fPitch );
		pssSource->SetRolloff( _sslpParms.fRollOff );
		pssSource->SetPos( _sslpParms.vPos );
		pssSource->SetDir( _sslpParms.vDir );
		pssSource->SetVel( _sslpParms.vVel );
		pssSource->SetMaxDist( _sslpParms.fMaxDist );


#ifdef LSS_USEOPENAL
		if ( m_pdDevice ) {
			if ( !pssSource->DeviceActivated() ) {
				if ( _pstrError ) {
					(*_pstrError) = "Source object failed to upload data to hardware.\r\n";
				}
				uint32_t ui32Index = pssSource->GetId();
				RemSource( ui32Index );
				return NULL;
			}
		}

#endif	// #ifdef LSS_USEOPENAL

		// Remain locked while returning the pointer.
		Lock();
		return pssSource;
	}

	/**
	 * Removes a source by ID.
	 *	** Must not be locked.
	 *
	 * \param _ui32Id ID of the source to remove.
	 */
	void LSE_CALL CSoundManager::RemSource( uint32_t _ui32Id ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount == 0 );

		uint32_t ui32Index;
		if ( m_pssSources.ItemExists( _ui32Id, ui32Index ) ) {
			// Destroy it.
			DeleteSource( m_pssSources.GetByIndex( ui32Index ) );
			m_pssSources.RemoveByIndex( ui32Index );
		}

	}

	/**
	 * Gets a source by ID.  If found, returns a pointer to the source.
	 * If not found, NULL is returned.
	 *	** Must be locked.
	 *
	 * \param _ui32Id ID of the source to find.
	 * \return Returns the found source or NULL.
	 */
	CSoundSource * LSE_CALL CSoundManager::FindSourceById( uint32_t _ui32Id ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount >= 0 );

		uint32_t ui32Index;
		if ( !m_pssSources.GetItemIndex( _ui32Id, ui32Index ) ) { return NULL; }
		return m_pssSources.GetByIndex( _ui32Id );
	}

	/**
	 * Same as FindSourceById(), except that it automatically locks the sound
	 *	manager.  If NULL is returned, the sound manager is unlocked.  If a pointer
	 *	is returned, the sound manager must be unlocked as soon as the pointer is done being used.
	 *
	 * \param _ui32Id ID of the source to find.
	 * \return Returns the found source or NULL.
	 */
	CSoundSource * LSE_CALL CSoundManager::FindSourceByIdAndLock( uint32_t _ui32Id ) {
		Lock();

		CSoundSource * pssRet = FindSourceById( _ui32Id );
		if ( pssRet ) {
			// Stay locked.
			return pssRet;
		}
		// Unlock and die.
		UnLock();
		return NULL;
	}

	/**
	 * Attach a buffer to a static source.  Does not need to be locked.  Returns false if the attachment
	 *	was not made for any reason (source or buffer ID's are invalid).
	 *
	 * \param _ui32SrcId ID of the source.
	 * \param _ui32BufferId ID of the buffer to attach to the source.
	 * \return Returns true if both ID's are valid.
	 */
	LSBOOL LSE_CALL CSoundManager::AttachBufferToStaticSource( uint32_t _ui32SrcId,
		uint32_t _ui32BufferId ) {
		Lock();

		// Sound must exist.
		CSoundSource * pssSource = FindSourceById( _ui32SrcId );
		if ( !pssSource ) {
			UnLock();
			return false;
		}

		// Must be a static type.
		if ( !pssSource->Static() ) {
			UnLock();
			return false;
		}


		// Buffer must exist.
		const CSoundBuffer * psbBuffer = FindBufferById( _ui32BufferId );
		if ( !psbBuffer ) {
			UnLock();
			return false;
		}

		
		static_cast<CStaticSoundSource *>(pssSource)->SetBuffer( psbBuffer );
		UnLock();
		return true;
	}

	/**
	 * Attach a buffer to a static source.  Returns false if the attachment was not made for any reason
	 *	(source is not static or buffer ID is invalid).
	 *	** Must be locked.
	 *
	 * \param _pssSource Source to which to attach the buffer.
	 * \param _ui32BufferId ID of the buffer to attach to the source.
	 * \return Returns true if the buffer ID is valid.
	 */
	LSBOOL LSE_CALL CSoundManager::AttachBufferToStaticSource( CSoundSource * _pssSource,
		uint32_t _ui32BufferId ) {
		Lock();

		// Must be a static type.
		if ( !_pssSource->Static() ) {
			UnLock();
			return false;
		}

		// Buffer must exist.
		const CSoundBuffer * psbBuffer = FindBufferById( _ui32BufferId );
		if ( !psbBuffer ) {
			UnLock();
			return false;
		}

		
		static_cast<CStaticSoundSource *>(_pssSource)->SetBuffer( psbBuffer );
		UnLock();
		return true;
	}

	/**
	 * Stop and delete all static sources.
	 *	** Must not be locked.
	 */
	void LSE_CALL CSoundManager::StopAndDeleteAllStaticSources() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		assert( m_i32LockCount == 0 );

		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			if ( m_pssSources.GetByIndex( I )->GetType() & LSS_SS_STATIC ) {
				// It is a static source so delete it.
				DeleteSource( m_pssSources.GetByIndex( I ) );
				m_pssSources.Remove( I );
			}
		}
	}

	/**
	 * The most common function performed on the listener is to set its position.  This function makes
	 *	doing so easy.
	 * Does not need to be locked.
	 *
	 * \param _vPos New listener position.
	 */
	void LSE_CALL CSoundManager::SetListenerPos( const CVector3 &_vPos ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		m_slListener.SetPos( _vPos );
	}

	/**
	 * Setting the listener velocity is common.
	 * Does not need to be locked.
	 *
	 * \param _vVel New listener velocity.
	 */
	void LSE_CALL CSoundManager::SetListenerVel( const CVector3 &_vVel ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		m_slListener.SetVel( _vVel );
	}

	/**
	 * This function allows setting both position and velocity at the same time on the listener.
	 * Does not need to be locked.
	 *
	 * \param _vPos New listener position.
	 * \param _vVel New listener velocity.
	 */
	void LSE_CALL CSoundManager::SetListenerPosAndVel( const CVector3 &_vPos, const CVector3 &_vVel ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		m_slListener.SetPos( _vPos );
		m_slListener.SetVel( _vVel );
	}

	/**
	 * Sets the master volume.  Ths sets the gain on the listener internally.
	 * Does not need to be locked.
	 *
	 * \param _fVolume New master volume.
	 */
	void LSE_CALL CSoundManager::SetMasterVolume( LSSREAL _fVolume ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		m_slListener.SetGain( _fVolume );
	}

	/**
	 * Gets the listener for read only.  Must call UnLock() when done with the listener!
	 *
	 * \return Returns the lister.
	 */
	const CSoundListener & LSE_CALL CSoundManager::GetListenerAndLock() const {
		Lock();
		return m_slListener;
	}

	/**
	 * Gets the listener for read and write.  Must call UnLock() when done with the listener!
	 *
	 * \return Returns the lister.
	 */
	CSoundListener & LSE_CALL CSoundManager::GetListenerAndLock() {
		Lock();
		return m_slListener;
	}

	/**
	 * Gets a default source-load parameter structure.  Modify after as you please.
	 *
	 * \return Returns a LSS_SOUND_SOURCE_LOAD_PARMS structure with default parameters.
	 */
	LSS_SOUND_SOURCE_LOAD_PARMS LSE_CALL CSoundManager::DefaultSourceLoad() {
		static const LSS_SOUND_SOURCE_LOAD_PARMS sslpDefault = {
			CVector3( static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(0.0) ),	// vPos
			CVector3( static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(0.0) ),	// vVel
			CVector3( static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(0.0),
				static_cast<LSSREAL>(-1.0) ),	// vDir
			static_cast<LSSREAL>(1.0),			// fPitch
			static_cast<LSSREAL>(1.0),			// fGain
			static_cast<LSSREAL>(256.0),		// fMaxDist
			static_cast<LSSREAL>(1.0),			// fRollOff
			static_cast<LSSREAL>(0.0),			// fMinGain
			static_cast<LSSREAL>(1.0),			// fMaxGain
			static_cast<LSSREAL>(0.0),			// fOuterGain
			static_cast<LSSREAL>(360.0),		// fInnerConeAngle
			static_cast<LSSREAL>(360.0),		// fConeAngle
			0									// ui32Id
		};

		return sslpDefault;
	}

	/**
	 * Close the current device, if any.  All buffers and sources are destroyed.
	 * But sound resources are not lost.  Only their connections to the hardware are.
	 */
	void LSE_CALL CSoundManager::ResetDevice() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		assert( m_i32LockCount == 0 );

#ifdef LSS_USEOPENAL
		if ( m_pdDevice ) {
			// Shut off the listener.
			m_slListener.DeviceWillClose();

			// Inform all sources that the device is about to be lost.
			for ( uint32_t I = 0; I < m_pssSources.Length(); ++I ) {
				m_pssSources.GetByIndex( I )->DeviceWillClose();
			}

			// Inform all buffers that the device is about to be lost.
			for ( uint32_t I = 0; I < m_psbBuffers.Length(); ++I ) {
				m_psbBuffers[I]->DeviceWillClose();
			}
		}


		if ( m_pcContext ) {
			::alcMakeContextCurrent( NULL );
			::alcDestroyContext( m_pcContext );
			m_pcContext = NULL;
		}
		if ( m_pdDevice ) {
			::alcCloseDevice( m_pdDevice );
			m_pdDevice = NULL;
		}

#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Reset the whole sound engine.  Releases all resources.
	 */
	void LSE_CALL CSoundManager::ShutDown() {
		Stop();
		ResetDevice();

		ResetAllSounds();


		if ( m_bAllocatorIsPrivate && m_paAllocator ) {
			LSEDELETE m_paAllocator;
		}

		m_paAllocator = NULL;
	}

	/**
	 * Creates a new buffer.  If you subclass CSoundBuffer, you must override this to create the
	 *	class of your new type.  This function's job is only to create instances of buffer
	 *	objects.  This function does not link them to the list of buffers, etc.
	 *
	 * \param _pui8Data The buffer data.
	 * \param _ui32Length Number of bytes in the buffer.
	 * \param _ui32Freq Frquency of the buffer.
	 * \param _ui32Bits Buffer bit count.
	 * \param _ui32Channels Number of channels in the buffer.
	 * \param _bRetain If true, the buffer will keep a copy of the buffer even after it has sent it to the
	 *	hardware.  This uses more RAM, but if the device is lost the buffer can reset its data on its own.
	 * \param _uiptrUserData User data.
	 * \return Returns a pointer to the created sound buffer.
	 */
	CSoundBuffer * LSE_CALL CSoundManager::NewBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
		uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain, uintptr_t _uiptrUserData ) const {
		void * pvNew = m_paAllocator->Alloc( sizeof( CSoundBuffer ) );
		if ( !pvNew ) { return NULL; }
		CSoundBuffer * psbBuffer = static_cast<CSoundBuffer *>(pvNew);
		return new( psbBuffer ) CSoundBuffer( _pui8Data, _ui32Length,
			_ui32Freq, _ui32Bits, _ui32Channels, m_paAllocator, _bRetain, _uiptrUserData );
	}

	/**
	 * Destroy a buffer the same way it was created.
	 *
	 * \param _psbBuffer The buffer to destroy.
	 */
	void LSE_CALL CSoundManager::DeleteBuffer( CSoundBuffer * &_psbBuffer ) const {
		if ( !_psbBuffer ) { return; }

		_psbBuffer->~CSoundBuffer();
		m_paAllocator->Free( _psbBuffer );
		// If m_paAllocator->Free() returns false, it means you subclassed NewBuffer() but did not
		//	subclass DeleteBuffer()!  If you make a new NewBuffer() you must also make a new DeleteBuffer()!
		_psbBuffer = NULL;
	}

	/**
	 * Creates a new source.  There are a few types of sources.  If you want to add more,
	 *	override this function in a subclass and add them.  This function's only job is
	 *	to create the instances.  It does not link them to the master list etc.
	 *
	 * \param _ssType Type of source to create.
	 * \param _ui32Id Id of the sound to create.  0 is used to generate a unique ID.
	 * \return Returns a pointer to the created sound source.
	 */
	CSoundSource * LSE_CALL CSoundManager::NewSoundSource( LSS_SOUND_SOURCE _ssType, uint32_t _ui32Id ) const {
		CSoundSource * pssSource = NULL;
		switch ( _ssType ) {
			case LSS_SS_3DSTATIC : {
				void * pvNew = m_paAllocator->Alloc( sizeof( CStaticSoundSource3d ) );
				if ( !pvNew ) { return NULL; }
				CStaticSoundSource3d * psssBuffer = static_cast<CStaticSoundSource3d *>(pvNew);
				pssSource = new( psssBuffer ) CStaticSoundSource3d( _ui32Id );
				break;
			}
			case LSS_SS_2DSTATIC : {
				void * pvNew = m_paAllocator->Alloc( sizeof( CStaticSoundSource2d ) );
				if ( !pvNew ) { return NULL; }
				CStaticSoundSource2d * psssBuffer = static_cast<CStaticSoundSource2d *>(pvNew);
				pssSource = new( psssBuffer ) CStaticSoundSource2d( _ui32Id );
				break;
			}
			default : {}
		}

		if ( pssSource ) {
			// Adjust the ID to be unique.
			if ( _ui32Id == 0 ) {
				CSoundManager::FindUniqueSourceId( pssSource );
			}
		}
		return pssSource;
	}

	/**
	 * Destroy a sound source the same way it was created.Destroy a sound source the same way it was created.
	 *
	 * \param _pssSource The sound source to destroy.
	 */
	void LSE_CALL CSoundManager::DeleteSource( CSoundSource * &_pssSource ) const {
		if ( !_pssSource ) { return; }

		_pssSource->~CSoundSource();
		m_paAllocator->Free( _pssSource );
		// If m_paAllocator->Free() returns false, it means you subclassed NewSoundSource() but did not
		//	subclass DeleteSource()!  If you make a new NewSoundSource() you must also make a new DeleteSource()!
		_pssSource = NULL;
	}

	/**
	 * Adds a streamer to our internal list of streamers.  If the streamer does not have a unique name, the results are
	 *	unpredictable when later referencing this streamer by name.
	 *
	 * \_pssssSteamer The streamer to add.  Must have been created externally.
	 * \return Returns true if there is enough memory to add the streamer.
	 */
	LSBOOL LSE_CALL CSoundManager::AddStreamer( IStreamingSoundSourceStreamer * _pssssSteamer ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		return m_pssssStreamers.Insert( _pssssSteamer, GetStreamerIndex( _pssssSteamer->GetName() ) );
	}

	/**
	 * Gets the index in the streamer list where a streamer with the given name should be added. 
	 *
	 * \param _pcStreamerName Name of the streamer whose index is to be located.
	 * \return Returns the index in the streamer list where a streamer with the given name should be added.
	 */
	uint32_t LSE_CALL CSoundManager::GetStreamerIndex( const char * _pcStreamerName ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		for ( uint32_t I = 0; I < m_pssssStreamers.Length(); ++I ) {
			if ( CStd::StrICmp( _pcStreamerName, m_pssssStreamers[I]->GetName() ) < 0 ) {
				return I;
			}
		}
		return m_pssssStreamers.Length();
	}

	/**
	 * Find a streamer by name.  Performs a binary search
	 *
	 * \param _pcName the name of the streamer to find.
	 * \return Returns the index of the found streamer or LSE_MAXU32 if none is found.
	 */
	uint32_t LSE_CALL CSoundManager::FindStreamerByName( const char * _pcStreamerName ) const {
		// Assumes we are already thread-safe.

		if ( !m_pssssStreamers.Length() ) { return LSE_MAXU32; }

		// Find the ID.
		IStreamingSoundSourceStreamer ** ppssssStreamer = static_cast<IStreamingSoundSourceStreamer **>(CSearch::BSearch( _pcStreamerName,
			&m_pssssStreamers[0],
			m_pssssStreamers.Length(),
			sizeof( m_pssssStreamers[0] ),
			reinterpret_cast<CSearch::PfBSearchFunc>( StreamerCompare ),
			NULL ));
		if ( !ppssssStreamer ) { return LSE_MAXU32; }

		return static_cast<uint32_t>(ppssssStreamer - &m_pssssStreamers[0]);
	}

	/**
	 * Find a unique ID and assign it to the given sound source.  If no ID could be found, the sound source is deleted.
	 *
	 * \param _pssSource The sound source for which to find a unique ID.
	 */
	void LSE_CALL CSoundManager::FindUniqueSourceId( CSoundSource * &_pssSource ) const {
		uint32_t ui32Id = _pssSource->GetId();
		// Find find any unique value above 0 and below LSS_UNIQUE_ID.
		if ( const_cast<CSoundManager *>(this)->FindSourceByIdAndLock( _pssSource->GetId() ) ) {
			for ( uint32_t I = _pssSource->GetId() + 1; I < LSS_UNIQUE_ID; ++I ) {
				if ( !const_cast<CSoundManager *>(this)->FindSourceByIdAndLock( I ) ) {
					_pssSource->m_ui32Id = ui32Id = I;
					break;
				}
				UnLock();
			}

			// If we did not find a unique ID above, search from 0 to the current value non-inclusively.
			if ( ui32Id == 0 ) {
				for ( uint32_t I = 1; I < _pssSource->GetId(); ++I ) {
					if ( !const_cast<CSoundManager *>(this)->FindSourceByIdAndLock( I ) ) {
						_pssSource->m_ui32Id = ui32Id = I;
						break;
					}
					UnLock();
				}
			}
		}
		else {
			ui32Id = _pssSource->GetId();
		}

		if ( ui32Id != _pssSource->GetId() ) {
			// A unique ID could not be found.  That means all ID's from 1 to LSS_UNIQUE_ID are still alive and active!
			// No choice but to delete the new sound.
			CStd::DebugPrintA( "CSoundManager::FindUniqueSourceId(): No unique sound source ID is available!  There are too many sound sources active!\r\n" );
			DeleteSource( _pssSource );
		}
	}

	/**
	 * Our search function for buffers by ID.  Used in CSearch::BSearch.
	 *
	 * \param _pvContext Unused.
	 * \param _pui32Left Buffer ID to find.
	 * \param _ppsbRight Buffer to check.
	 * \return Returns the comparison between the two buffers.
	 */
	int32_t LSE_CCALL CSoundManager::BufferCompare( void * /*_pvContext*/,
		const uint32_t * _pui32Left, const CSoundBuffer ** _ppsbRight ) {
		if ( (*_pui32Left) == (*_ppsbRight)->GetId() ) { return 0; }
		return (*_pui32Left) < (*_ppsbRight)->GetId() ? -1 : 1;
	}

	/**
	 * Our search function for sources by ID.  Used in CSearch::BSearch.
	 *
	 * \param _pvContext Unused
	 * \param _pui32Left Pointer to an uint32_t holding the ID to find.
	 * \param _ppssRight The current object being compared.
	 * \return Returns the lexicographical relationship between the ID and the object.
	 */
	int32_t LSE_CCALL CSoundManager::SourceCompare( void * /*_pvContext*/,
		const uint32_t * _pui32Left, const CSoundSource ** _ppssRight ) {
		if ( (*_pui32Left) == (*_ppssRight)->GetId() ) { return 0; }
		return (*_pui32Left) < (*_ppssRight)->GetId() ? -1 : 1;
	}

	/**
	 * Our search function for streamers by name.  Used in CSearch::BSearch.
	 *
	 * \param _pvContext Unused
	 * \param _pcName Name of the streamer to find.
	 * \param _ppssssRight The current object being compared.
	 * \return Returns the lexicographical relationship between the ID and the object.
	 */
	int32_t LSE_CCALL CSoundManager::StreamerCompare( void * /*_pvContext*/,
		const char * _pcName, const IStreamingSoundSourceStreamer ** _ppssssRight ) {
		return CStd::StrICmp( _pcName, (*_ppssssRight)->GetName() );
	}

	/**
	 * Our second thread.
	 *
	 * \param _lpParameter Pointer to the sound manager.
	 * \return Returns the thread exit code.
	 */
	uint32_t LSE_CALL CSoundManager::RunThread( void * _lpParameter ) {
		CSoundManager * psmManager = static_cast<CSoundManager *>(_lpParameter);
		// m_bRun must already be set!
		while ( psmManager->m_bRun ) {
			psmManager->Tick();

			CStd::Sleep( 16 );
		}

		return 0;
	}

}	// namespace lss
