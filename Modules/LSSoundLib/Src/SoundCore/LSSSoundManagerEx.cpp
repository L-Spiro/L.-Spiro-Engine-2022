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
 * Description: Add our own functionality to the sound library.  We extend it to support loading of files
 *	and a few special effects.  We also make it easy to play sounds and include the ability to delay-load
 *	sounds and perform automatic unloading of sounds.
 */

#include "LSSSoundManagerEx.h"
#include "../Ogg/LSSVorbisFile.h"
#include "../Wave/LSSWaveLoader.h"


namespace lss {

	// == Members.
	/**
	 * The unique ID for registered SFX data structures.
	 */
	uint32_t CSoundManagerEx::m_ui32SfxId = 0;


	// == Various constructors.
	LSE_CALLCTOR CSoundManagerEx::CSoundManagerEx() {
	}
	LSE_CALLCTOR CSoundManagerEx::~CSoundManagerEx() {
		ShutDown();
	}

	// == Functions.
	/**
	 * Sets the allocator used by the sound system.  This causes all resources
	 *	to be reset and lost.  This should be done only once at start-up,
	 *	before any resources are created.  _paAllocator may be NULL.
	 *
	 * \param _paAllocator The allocator to be used by the sound engine.
	 */
	void LSE_CALL CSoundManagerEx::SetAllocator( CAllocator * _paAllocator ) {
		Parent::SetAllocator( _paAllocator );
		m_csdSfxData.SetAllocator( m_paAllocator );
	}
	
	/**
	 * Creates a buffer from a file.  If the buffer has already been created, no operation is performed.
	 *	** Must not be locked.
	 *
	 * \param _pcFile Sound file to load.
	 * \return Returns the ID of the buffer on success, or 0 on failure.
	 */
	uint32_t LSE_CALL CSoundManagerEx::CreateBuffer( const char * _pcFile ) {
		// Get the fully expanded path.
		char szFullPath[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcFile, szFullPath );
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		for ( uint32_t I = 0; I < m_psbBuffers.Length(); ++I ) {
			CSoundBufferEx * pcsbBuffer = static_cast<CSoundBufferEx *>(m_psbBuffers[I]);
			if ( CStd::StrICmp( pcsbBuffer->GetPath(), szFullPath ) == 0 ) {
				// Buffer exists.  Return its ID.
				return pcsbBuffer->GetId();
			}
		}

		// Buffer not found.  Try loading the file.
		uint8_t * pui8Data;
		uint32_t ui32Length;
		uint32_t ui32Freq, ui32Bits, ui32Channels;
		if ( !CWaveLoader::LoadWavFile( szFullPath,
			pui8Data, ui32Length, ui32Freq, ui32Bits, ui32Channels,
			m_paAllocator ) ) {
			if ( !CVorbisFile::LoadOggFile( szFullPath,
				pui8Data, ui32Length, ui32Freq, ui32Bits, ui32Channels,
				m_paAllocator ) ) {
				// TODO: Try other formats.
				return 0;
			}
		}

		// File loaded.  Buffer it.
		uint32_t ui32Ret = AddBuffer( pui8Data, ui32Length,
			ui32Freq, ui32Bits, ui32Channels,
			false,											// No retaining because we can
															//	get it from the same file later.
			reinterpret_cast<uintptr_t>(szFullPath) );		// But we have to tell it which file
															//	to load, so pass the file name here.

		// Also free our copy of the data (the buffer keeps its own).
		m_paAllocator->Free( pui8Data );
		return ui32Ret;
	}

	/**
	 * Find a buffer by path.
	 *
	 * \param _pwcPath The path for which to search the loaded buffers.
	 * \return Returns the buffer ID or 0 on failure.
	 */
	uint32_t LSE_CALL CSoundManagerEx::FindBufferByPath( const char * _pwcPath ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		for ( uint32_t I = 0; I < m_psbBuffers.Length(); ++I ) {
			CSoundBufferEx * pcsbBuffer = static_cast<CSoundBufferEx *>(m_psbBuffers[I]);
			if ( CStd::StrICmp( pcsbBuffer->GetPath(), _pwcPath ) == 0 ) { return pcsbBuffer->GetId(); }
		}
		return 0;
	}

	/**
	 * Gets a buffer path by buffer ID.  _pcRet must be at least LSF_MAX_PATH characters long.
	 *
	 * \param _ui32Id ID of the buffer whose path is to be returned.
	 * \param _pcRet The string to be filled with the returned path, if any.
	 * \return Returns NULL if the ID is invalid or if the buffer was not created via a path,
	 *	otherwise _pcRet is filled with the path and returned.
	 */
	char * LSE_CALL CSoundManagerEx::GetBufferPathById( uint32_t _ui32Id, char * _pcRet ) const {
		_pcRet[0] = '\0';
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		const CSoundBuffer * pscBuffer = FindBufferByIdAndLock( _ui32Id );
		if ( !pscBuffer ) { return NULL; }

		const CSoundBufferEx * pcsbBuffer = static_cast<const CSoundBufferEx *>(pscBuffer);
		if ( !pcsbBuffer ) { return NULL; }

		if ( !pcsbBuffer->GetPath() ) {
			UnLock();
			return NULL;
		}
		CStd::StrCpy( _pcRet, pcsbBuffer->GetPath() );
		UnLock();
		return _pcRet;
	}

	/**
	 * Load (but do not play) a sound at a specified 3D location.  The loaded sound is returned and UnLock()
	 *	must be called when the sound pointer is no longer needed (after all settings have been applied to it).
	 * If the given file has not been loaded, it is loaded.  The buffer to be associated with the source is
	 *	optional; if provided the file will automatically be loaded (if not already) and added as a buffer.
	 * Common parameters for 3D sounds are provided here.  Others can be set via the returned pointer.
	 *
	 * \param _vPos The 3D position of the source.
	 * \param _fGain The volume of the source from 0 to 1 inclusive.
	 * \param _fMaxDistance Maximum distance at which the source can be heard.
	 * \param _pcFile The file containing the sound for this source.  If already loaded, the existing buffer
	 *	is used.
	 * \param _ui32Id The unique ID for this sound source or 0 to have a new ID generated.  Unique ID's begin at LSS_UNIQUE_ID.
	 *	If a sound source with the given unique ID exists it will be stopped and deleted, unless NULL is returned.
	 * \return Returns a pointer to the created 3D static source or NULL on error.  If not NULL, call UnLock()
	 *	when done with the pointer.
	 */
	CStaticSoundSource3d * LSE_CALL CSoundManagerEx::Create3dStaticSoundSourceAndLock( const CVector3 &_vPos,
		LSREAL _fGain,
		LSREAL _fMaxDistance,
		const char * _pcFile,
		uint32_t _ui32Id ) {

		uint32_t ui32BufferId = 0;
		if ( _pcFile ) {
			if ( (ui32BufferId = CreateBuffer( _pcFile )) == 0 ) { return NULL; }
		}

		LSS_SOUND_SOURCE_LOAD_PARMS sslpDefaults = CSoundManager::DefaultSourceLoad();
		sslpDefaults.vPos = _vPos;
		sslpDefaults.fGain = static_cast<LSSREAL>(_fGain);
		sslpDefaults.fMaxDist = static_cast<LSSREAL>(_fMaxDistance);
		sslpDefaults.ui32Id = _ui32Id;

		CSoundSource * pssSource = AddSourceAndLock( LSS_SS_3DSTATIC,
			sslpDefaults );
		if ( !pssSource ) { return NULL; }

		// We check that we were not locked before by checking the lock count now.
		// It should be 1 if we were not locked before.
		assert( m_i32LockCount == 1 );

		// After this point, if returning NULL, be sure to UnLock().

		if ( ui32BufferId != 0 ) {
			if ( !AttachBufferToStaticSource( pssSource, ui32BufferId ) ) {
				uint32_t ui32Id = pssSource->GetId();
				// Stay in the critical section but unlock.
				CCriticalSection::CLocker lLockMe( m_csCriticalSection );
				UnLock();
				// We were not locked before so this will succeed.
				RemSource( ui32Id );
				return NULL;
			}
		}

		// We remain locked from AddSourceAndLock().
		return static_cast<CStaticSoundSource3d *>(pssSource);
	}
	
	/**
	 * Load (but do not play) a sound at a specified 2D location.  The loaded sound is returned and UnLock()
	 *	must be called when the sound pointer is no longer needed (after all settings have been applied to it).
	 * If the given file has not been loaded, it is loaded.  The buffer to be associated with the source is
	 *	optional; if provided the file will automatically be loaded (if not already) and added as a buffer.
	 * Common parameters for 2D sounds are provided here.  Others can be set via the returned pointer.
	 *
	 * \param _vPos The 2D position of the source.
	 * \param _fGain The volume of the source from 0 to 1 inclusive.
	 * \param _pcFile The file containing the sound for this source.  If already loaded, the existing buffer
	 *	is used.
	 * \return Returns a pointer to the created 2D static source or NULL on error.  If not NULL, call UnLock()
	 *	when done with the pointer.
	 */
	CStaticSoundSource2d * LSE_CALL CSoundManagerEx::Create2dStaticSoundSourceAndLock( const CVector2 &_vPos,
		LSREAL _fGain,
		const char * _pcFile ) {

		uint32_t ui32BufferId = 0;
		if ( _pcFile ) {
			if ( (ui32BufferId = CreateBuffer( _pcFile )) == 0 ) { return NULL; }
		}

		LSS_SOUND_SOURCE_LOAD_PARMS sslpDefaults = CSoundManager::DefaultSourceLoad();
		sslpDefaults.vPos = CVector3( _vPos.x, _vPos.y, LSM_ONE );
		sslpDefaults.fGain = static_cast<LSSREAL>(_fGain);

		CSoundSource * pssSource = AddSourceAndLock( LSS_SS_2DSTATIC,
			sslpDefaults );
		if ( !pssSource ) { return NULL; }
		
		// We check that we were not locked before by checking the lock count now.
		// It should be 1 if we were not locked before.
		assert( m_i32LockCount == 1 );

		// After this point, if returning NULL, be sure to UnLock().

		if ( ui32BufferId != 0 ) {
			if ( !AttachBufferToStaticSource( pssSource, ui32BufferId ) ) {
				uint32_t ui32Id = pssSource->GetId();
				// Stay in the critical section but unlock.
				CCriticalSection::CLocker lLockMe( m_csCriticalSection );
				UnLock();
				// We were not locked before so this will succeed.
				RemSource( ui32Id );
				return NULL;
			}
		}

		// We remain locked from AddSourceAndLock().
		return static_cast<CStaticSoundSource2d *>(pssSource);
	}
	
	/**
	 * Play a sound effect at a given 3D position using a returned ID from a registered LSE_SFX_DATA structure.
	 *  Buffer is loaded if not loaded already.  Loop count and expiration dates are set according to the data
	 *	registered with the system.
	 *
	 * \param _ui32SfxId The ID of the registered sound effect containing the parameters for playing the sound.
	 * \param _vPos The 3D position of the source.
	 * \param _fGain The volume of the source from 0 to 1 inclusive.
	 * \param _fMaxDistance Maximum distance at which the source can be heard.
	 * \return The ID of the created source is returned, which can be used to make modifications to the sound after loading.
	 */
	//uint32_t LSE_CALL CSoundManagerEx::Play3dSound( uint32_t _ui32SfxId,
	//	const CVector3 &_vPos,
	//	LSREAL _fGain,
	//	LSREAL _fMaxDistance ) {

	//	CCriticalSection::CLocker lLockMe( m_csCriticalSection );
	//	const LSE_CORE_SFX_DATA * pcsdData = FindSfxData( _ui32SfxId );
	//	if ( !pcsdData ) { return 0; }
	//	
	//	CStaticSoundSource3d * pcsss3Source = Create3dStaticSoundSourceAndLock( _vPos,
	//		_fGain,
	//		_fMaxDistance,
	//		NULL );
	//	
	//	if ( !pcsss3Source ) { return 0; }
	//	uint32_t ui32SourceId = pcsss3Source->GetId();

	//	// If this a load-on-demand sound, we need to load it.
	//	if ( !pcsdData->ui32BufferId ) {
	//		const_cast<LSE_CORE_SFX_DATA *>(pcsdData)->ui32BufferId = CreateBuffer( pcsdData->sdData.pwcFile );
	//	}
	//	if ( !PlayStaticSource( pcsss3Source, pcsdData ) ) {
	//		UnLock();
	//		RemSource( ui32SourceId );
	//		return 0;
	//	}
	//	UnLock();
	//	return ui32SourceId;
	//}

	/**
	 * Play a sound effect at a given 2D position using a returned ID from a registered LSE_SFX_DATA structure.
	 *  Buffer is loaded if not loaded already.  Loop count and expiration dates are set according to the data
	 *	registered with the system.
	 *
	 * \param _ui32SfxId The ID of the registered sound effect containing the parameters for playing the sound.
	 * \param _vPos The 2D position of the source.
	 * \param _fGain The volume of the source from 0 to 1 inclusive.
	 * \return The ID of the created source is returned, which can be used to make modifications to the sound after loading.
	 */
	//uint32_t LSE_CALL CSoundManagerEx::Play2dSound( uint32_t _ui32SfxId,
	//	const CVector2 &_vPos,
	//	LSREAL _fGain ) {
	//	if ( !_ui32SfxId ) { return 0; }

	//	CCriticalSection::CLocker lLockMe( m_csCriticalSection );
	//	const LSE_CORE_SFX_DATA * pcsdData = FindSfxData( _ui32SfxId );
	//	if ( !pcsdData ) { return 0; }
	//	
	//	CStaticSoundSource2d * pcsss2Source = Create2dStaticSoundSourceAndLock( _vPos,
	//		_fGain,
	//		NULL );
	//	
	//	if ( !pcsss2Source ) { return 0; }
	//	uint32_t ui32SourceId = pcsss2Source->GetId();

	//	// If this a load-on-demand sound, we need to load it.
	//	if ( !pcsdData->ui32BufferId ) {
	//		const_cast<LSE_CORE_SFX_DATA *>(pcsdData)->ui32BufferId = CreateBuffer( pcsdData->sdData.pwcFile );
	//	}
	//	if ( !PlayStaticSource( pcsss2Source, pcsdData ) ) {
	//		UnLock();
	//		RemSource( ui32SourceId );
	//		return 0;
	//	}
	//	UnLock();
	//	return ui32SourceId;
	//}
	
	/**
	 * Register a sound-effect file.  Returns the unique ID of the data which can be used to play the sound later.
	 * If registration fails (due to memory), 0 is returned.
	 *
	 * \param _sdData An LSE_SFX_DATA structure containing the parameters for the sound effect.
	 * \return The ID of the registered sound effect or 0 upon failure.
	 */
	//uint32_t LSE_CALL CSoundManagerEx::RegisterSfx( const LSE_SFX_DATA &_sdData ) {
	//	CCriticalSection::CLocker lLockMe( m_csCriticalSection );

	//	LSE_CORE_SFX_DATA csdNewData = {
	//		++m_ui32SfxId,
	//		0,
	//		_sdData
	//	};
	//	char szFullPath[LSF_MAX_PATH];
	//	CFileLib::ExpandPath( _sdData.pwcFile, szFullPath );

	//	if ( !_sdData.bLoadOnDemand ) {
	//		csdNewData.ui32BufferId = CreateBuffer( szFullPath );
	//		if ( !csdNewData.ui32BufferId ) {
	//			return 0;
	//		}
	//	}
	//	
	//	csdNewData.sdData.pwcFile = CUtils::WStrDup( szFullPath, m_paAllocator );	// Freed in ResetAllSounds().
	//	if ( !m_csdSfxData.Push( csdNewData ) ) {
	//		m_paAllocator->Free( const_cast<char *>(csdNewData.sdData.pwcFile) );
	//		return 0;
	//	}

	//	return csdNewData.ui32Id;
	//}

	/**
	 * Unregister a sound effect by its unique ID.
	 *
	 * \param _ui32SfxId The ID of the registered sound effect to unregister.
	 * \return Returns true if the given ID matches a registered sound effect.
	 */
	/*LSBOOL LSE_CALL CSoundManagerEx::UnRegisterSfx( uint32_t _ui32Id ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = m_csdSfxData.Length(); I--; ) {
			if ( m_csdSfxData[I].ui32Id == _ui32Id ) {
				m_csdSfxData.RemoveNoDealloc( I );
				++ui32Total;
			}
		}
		return ui32Total != 0;
	}*/

	/**
	 * Register a streaming sound in a wave file.  The file must be valid and contain valid PCM data in .wav format.
	 *
	 * \param _pcFile The UTF-8 path to the file.
	 * \return Returns true if the sound was created.
	 */
	LSBOOL LSE_CALL CSoundManagerEx::RegisterWaveForStream( const char * _pcFile ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		// Is it already loaded?
		if ( FindStreamerByName( _pcFile ) != LSE_MAXU32 ) { return true; }

		CWaveStreamingSoundSourceStreamer * pwssssThis = static_cast<CWaveStreamingSoundSourceStreamer *>(m_paAllocator->Alloc( sizeof( CWaveStreamingSoundSourceStreamer ) ));
		if ( !pwssssThis ) { return false; }
		new( pwssssThis ) CWaveStreamingSoundSourceStreamer( _pcFile, m_paAllocator );
		if ( !pwssssThis->GetChannels() || !pwssssThis->GetAudioLength() ) {
			// Failed.  Delete and return false.
			DeleteStreamer( pwssssThis );
			return false;
		}

		// Succeeded.
		if ( !AddStreamer( pwssssThis ) ) {
			// Damn it.
			DeleteStreamer( pwssssThis );
			return false;
		}

		return true;
	}

	/**
	 * Register a streaming sound in an Ogg file.  The file must be a valid .ogg file.
	 *
	 * \param _pcFile The UTF-8 path to the file.
	 * \return Returns true if the sound was created.
	 */
	LSBOOL LSE_CALL CSoundManagerEx::RegisterOggForStream( const char * _pcFile ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		// Is it already loaded?
		if ( FindStreamerByName( _pcFile ) != LSE_MAXU32 ) { return true; }

		COggStreamingSoundSourceStreamer * pwssssThis = static_cast<COggStreamingSoundSourceStreamer *>(m_paAllocator->Alloc( sizeof( COggStreamingSoundSourceStreamer ) ));
		if ( !pwssssThis ) { return false; }
		new( pwssssThis ) COggStreamingSoundSourceStreamer( _pcFile, m_paAllocator );
		if ( !pwssssThis->GetChannels() || !pwssssThis->GetAudioLength() ) {
			// Failed.  Delete and return false.
			DeleteStreamer( pwssssThis );
			return false;
		}

		// Succeeded.
		if ( !AddStreamer( pwssssThis ) ) {
			// Damn it.
			DeleteStreamer( pwssssThis );
			return false;
		}

		return true;
	}

	/**
	 * Register a streaming sound in a supported sound file.  The file must be a valid sound file.
	 *
	 * \param _pcFile The UTF-8 path to the file.
	 * \return Returns true if the sound was created.
	 */
	LSBOOL LSE_CALL CSoundManagerEx::RegisterSoundForStream( const char * _pcFile ) {
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcFile, szBuffer );
		return RegisterOggForStream( szBuffer ) || RegisterWaveForStream( szBuffer );
	}

	/**
	 * Stream a 2D sound.  The sound is referenced by name.  If registered with Register*ForStream(), the name is the file path.
	 *	** Must not be locked.
	 *
	 * \param _pcName Name of the registered stream to begin playing.
	 * \return Returns the created streaming sound if successful, NULL otherwise.  If NULL is not returned, UnLock() must be called when
	 *	done with the pointer.
	 */
	CStreamingSoundSource * LSE_CALL CSoundManagerEx::Create2dStreamSoundSourceAndLock( const char * _pcName, const CVector2 &_vPos, LSREAL _fGain ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		uint32_t ui32Index = FindStreamerByName( _pcName );
		if ( ui32Index == LSE_MAXU32 ) { return NULL; }

		LSS_SOUND_SOURCE_LOAD_PARMS sslpDefaults = CSoundManager::DefaultSourceLoad();
		sslpDefaults.vPos = CVector3( _vPos.x, _vPos.y, LSM_ONE );
		sslpDefaults.fGain = static_cast<LSSREAL>(_fGain);

		CSoundSource * pssSource = AddSourceAndLock( LSS_SS_2DSTREAMING,
			sslpDefaults );
		if ( !pssSource ) { return NULL; }
		
		// We check that we were not locked before by checking the lock count now.
		// It should be 1 if we were not locked before.
		assert( m_i32LockCount == 1 );

		CStreamingSoundSource * psssThis = static_cast<CStreamingSoundSource *>(pssSource);

		if ( !psssThis->CreateStream( m_pssssStreamers[ui32Index], m_paAllocator ) ) {
			uint32_t ui32Id = pssSource->GetId();
			// Stay in the critical section but unlock.
			CCriticalSection::CLocker lLockMeAgain( m_csCriticalSection );
			UnLock();
			// We were not locked before so this will succeed.
			RemSource( ui32Id );
			return NULL;
		}

		// We remain locked from AddSourceAndLock().
		return psssThis;
	}

	/**
	 * Play a registered stream at a given 2D position.  Returns the ID of the created sound source which can be used to make
	 *	modifications to the sound after loading.
	 *	** Must not be locked.
	 *
	 * \param _pcName Name of the stream, previously registered with a call to Register*ForStream() to play.
	 * \param _vPos Position where to play the stream.
	 * \param _fGain The volume of the source from 0 to 1 inclusive.
	 * \param _ui32Loop The number of times to loop the sound.
	 * \param _bOnlyPlayIfNotPlaying If true, the sound will not play if already being played.
	 * \param _bClearAllStreamsIfPlayed If true, and the sound is actually played, all other streaming sounds will
	 *	be stopped.
	 * \return The ID of the created source is returned, which can be used to make modifications to the sound after loading.
	 */
	uint32_t LSE_CALL CSoundManagerEx::Play2dStreamEx( const char * _pcName,
		const CVector2 &_vPos,
		LSREAL _fGain,
		uint32_t _ui32Loop,
		LSBOOL _bOnlyPlayIfNotPlaying,
		LSBOOL _bClearAllStreamsIfPlayed ) {
		
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcName, szBuffer );

		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		// If we should only play if not already playing, check.
		if ( _bOnlyPlayIfNotPlaying ) {
			uint32_t ui32Id = SoundIsStreaming( szBuffer );
			if ( ui32Id ) {
				return ui32Id;
			}
		}

		// Sound is not set to play only if not playing or it is but the sound is not playing.
		// From here, if playing succeeds, check the _bClearAllStreamsIfPlayed flag.
		CStreamingSoundSource * psssSource = Create2dStreamSoundSourceAndLock( szBuffer, _vPos, _fGain );
		if ( !psssSource ) { return 0; }

		uint32_t ui32Id = psssSource->GetId();
		// From here out we need to UnLock() before leaving.
		// Creation succeeded so remove other streams if necessary.
		if ( _bClearAllStreamsIfPlayed ) {
			ClearAllStreamsExcept( ui32Id );
		}
		psssSource->Play( _ui32Loop );

		UnLock();

		return ui32Id;
	}

	/**
	 * Determine if the given sound is already streaming.
	 *
	 * \param _pcName name of the stream to check.  This name must match one of the names provided in a call to one of the Register*ForStream() functions.
	 * \return Returns the ID of the sound if it is already playing.  If the sound does not exist or exists but is not in the playing or paused states,
	 *	0 is returned.
	 */
	uint32_t LSE_CALL CSoundManagerEx::SoundIsStreaming( const char * _pcName ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			if ( !m_pssSources.GetByIndex( I ) ) { continue; }
			if ( m_pssSources.GetByIndex( I )->Static() ) { continue; }
			CStreamingSoundSource * psssThis = static_cast<CStreamingSoundSource *>(m_pssSources.GetByIndex( I ));
			if ( !psssThis->GetStreamer() ) { continue; }

			if ( psssThis->GetState() != CSoundSource::LSS_SS_PAUSED && psssThis->GetState() != CSoundSource::LSS_SS_PLAYING ) {
				continue;
			}
			if ( CStd::StrCmp( psssThis->GetStreamer()->GetName(), _pcName ) == 0 ) { return psssThis->GetId(); }
		}
		// Not found.
		return 0;
	}

	/**
	 * Clear all straming sounds whose ID's do not match the given ID.
	 *
	 * \param _ui32Id ID of the sound not to clear.
	 */
	void LSE_CALL CSoundManagerEx::ClearAllStreamsExcept( uint32_t _ui32Id ) {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		for ( uint32_t I = m_pssSources.Length(); I--; ) {
			if ( !m_pssSources.GetByIndex( I ) ) { continue; }
			if ( m_pssSources.GetByIndex( I )->Static() ) { continue; }
			if ( m_pssSources.GetByIndex( I )->GetId() == _ui32Id ) { continue; }

			DeleteSource( m_pssSources.GetByIndex( I ) );
			// Remove it from the list.
			m_pssSources.Remove( I );
		}
	}

	/**
	 * Stop and remove all sources and buffers, and apply default parameters to the listener.
	 */
	void LSE_CALL CSoundManagerEx::ResetAllSounds() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );

		assert( m_i32LockCount == 0 );


		/*for ( uint32_t I = m_csdSfxData.Length(); I--; ) {
			m_paAllocator->Free( const_cast<char *>(m_csdSfxData[I].sdData.pwcFile) );
			m_csdSfxData[I].sdData.pwcFile = NULL;
		}
		m_csdSfxData.Reset();*/

		Parent::ResetAllSounds();
	}

	/**
	 * Perform a soft reset.  This removes unreferenced data and stops sounds that are not
	 *	soft-reset-safe.
	 */
	void LSE_CALL CSoundManagerEx::SoftReset() {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		// Must be unlocked.
		assert( m_i32LockCount == 0 );
		Parent::SoftReset();

		// Remove registered sound effects.
		/*for ( uint32_t I = m_csdSfxData.Length(); I--; ) {
			m_paAllocator->Free( const_cast<char *>(m_csdSfxData[I].sdData.pwcFile) );
			m_csdSfxData[I].sdData.pwcFile = NULL;
		}
		m_csdSfxData.Reset();*/
	}

	/**
	 * Use extended buffers we have the ability to get data from files when needed
	 *	(when the device changes).
	 *
	 * \param _pui8Data Sound data to pass to the buffer.
	 * \param _ui32Length Length, in bytes, of the data passed to the buffer.
	 * \param _ui32Freq Frequency of the data passed to the sound buffer.
	 * \param _ui32Bits Bits per channel of the data passed to the sound buffer.
	 * \param _ui32Channels Channels in the data passed to the sound buffer.
	 * \param _bRetain If true, the buffer will keep a local copy of the data.  Otherwise it will dump it after
	 *	sending it to the hardware to save memory.
	 * \param _uiptrUserData A character pointer to a string indicating the path from which the buffer was created.
	 * \return Returns true if a new buffer could be created.
	 */
	CSoundBuffer * LSE_CALL CSoundManagerEx::NewBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
		uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain, uintptr_t _uiptrUserData ) const {
		void * pvNew = m_paAllocator->Alloc( sizeof( CSoundBufferEx ) );
		if ( !pvNew ) { return NULL; }
		CSoundBufferEx * pcsbBuffer = static_cast<CSoundBufferEx *>(pvNew);
		return new( pcsbBuffer ) CSoundBufferEx( _pui8Data, _ui32Length,
			_ui32Freq, _ui32Bits, _ui32Channels, m_paAllocator, _bRetain, _uiptrUserData );
	}

	/**
	 * Destroy a buffer the same way it was created.
	 *
	 * \param _psbBuffer The buffer to destroy.
	 */
	void LSE_CALL CSoundManagerEx::DeleteBuffer( CSoundBuffer * &_psbBuffer ) const {
		if ( !_psbBuffer ) { return; }

		CSoundBufferEx * _pcsbCoreBuffer = static_cast<CSoundBufferEx *>(_psbBuffer);
		// You do know that casting may cause the pointer to change, right?  We have to check
		//	for NULL again with the new value.
		if ( !_pcsbCoreBuffer ) { return; }

		_pcsbCoreBuffer->~CSoundBufferEx();
		m_paAllocator->Free( _pcsbCoreBuffer );
		_psbBuffer = NULL;
	}

	/**
	 * Create extended sound sources not available in the case sound manager.
	 *
	 * \param _ssType The type of sound source to create.
	 * \param _ui32Id Id of the sound to create.  0 is used to generate a unique ID.
	 * \return Returns a pointer to the created sound source of the given type.
	 */
	CSoundSource * LSE_CALL CSoundManagerEx::NewSoundSource( LSS_SOUND_SOURCE _ssType, uint32_t _ui32Id ) const {
		CSoundSource * pssSource = NULL;
		switch ( _ssType ) {
			case LSS_SS_2DSTREAMING : {
				void * pvNew = m_paAllocator->Alloc( sizeof( CStreamingSoundSource2d ) );
				if ( !pvNew ) { return NULL; }
				CStreamingSoundSource2d * pcsssBuffer = static_cast<CStreamingSoundSource2d *>(pvNew);
				pssSource = new( pcsssBuffer ) CStreamingSoundSource2d( _ui32Id );
				break;
			}
			default : {}
		}

		if ( pssSource ) {
			// Adjust the ID to be unique.
			if ( _ui32Id == 0 ) {
				CSoundManager::FindUniqueSourceId( pssSource );
			}
			return pssSource;
		}
		return Parent::NewSoundSource( _ssType, _ui32Id );
	}

	/**
	 * Destroy a sound source the same way it was created.
	 *
	 * \param _pssSource The sound source to delete.
	 */
	void LSE_CALL CSoundManagerEx::DeleteSource( CSoundSource * &_pssSource ) const {
		Parent::DeleteSource( _pssSource );
	}

	/**
	 * Creation of the streamers is done by the overriding class.  Thus deletion must also be handled by them.
	 *
	 * \param _pssssSteamer The streamer to be deleted.
	 */
	void LSE_CALL CSoundManagerEx::DeleteStreamer( IStreamingSoundSourceStreamer * _pssssSteamer ) {
		if ( _pssssSteamer->GetType() & LSS_ST_WAVEFILE ) {
			CWaveStreamingSoundSourceStreamer * pwssssThis = static_cast<CWaveStreamingSoundSourceStreamer *>(_pssssSteamer);
			pwssssThis->~CWaveStreamingSoundSourceStreamer();
			m_paAllocator->Free( static_cast<void *>(pwssssThis) );
			return;
		}
		if ( _pssssSteamer->GetType() & LSS_ST_OGGFILE ) {
			COggStreamingSoundSourceStreamer * pwssssThis = static_cast<COggStreamingSoundSourceStreamer *>(_pssssSteamer);
			pwssssThis->~COggStreamingSoundSourceStreamer();
			m_paAllocator->Free( static_cast<void *>(pwssssThis) );
			return;
		}
	}
	
	// Find SFX data by ID fast.  Returns NULL if not found.
	/*const CSoundManagerEx::LSE_CORE_SFX_DATA * LSE_CALL CSoundManagerEx::FindSfxData( uint32_t _ui32Id ) const {
		CCriticalSection::CLocker lLockMe( m_csCriticalSection );
		if ( !m_csdSfxData.Length() ) { return NULL; }
		
		LSE_CORE_SFX_DATA * pcsdData = static_cast<LSE_CORE_SFX_DATA *>(CSearch::BSearch( &_ui32Id,
			&m_csdSfxData[0],
			m_csdSfxData.Length(),
			sizeof( m_csdSfxData[0] ),
			reinterpret_cast<CSearch::PfBSearchFunc>(SfxCompare),
			NULL ));
		if ( !pcsdData ) { return NULL; }
		
		return pcsdData;
	}*/
	
	// Our search function for SFX data by ID.  Used in CSearch::BSearch.
	//int32_t LSE_CCALL CSoundManagerEx::SfxCompare( void * _pvContext,
	//	const uint32_t * _pui32Left, const LSE_CORE_SFX_DATA * _pcsdRight ) {
	//	// Remove warning C4100.
	//	static_cast<void *>(_pvContext);
	//	if ( (*_pui32Left) == (*_pcsdRight).ui32Id ) { return 0; }
	//	return (*_pui32Left) < (*_pcsdRight).ui32Id ? -1 : 1;
	//}

	/**
	 * Play any static source registered as a sound effect.  Assumes thread safety is ensured from the caller.
	 *
	 * \param _pssSource The source to play.
	 * \param _pcsdData Registered sound-effect data to apply to the source.
	 * \return Returns true on success or false otherwise.
	 */
	//LSBOOL LSE_CALL CSoundManagerEx::PlayStaticSource( CSoundSource * _pssSource,
	//	const LSE_CORE_SFX_DATA * _pcsdData ) {
	//	// If the buffer was already loaded, use its ID directly.
	//	uint32_t ui32BufferId = _pcsdData->ui32BufferId;
	//	if ( ui32BufferId == 0 ) {
	//		// Not loaded or loaded on demand.
	//		// Slow search required.
	//		ui32BufferId = FindBufferByPath( _pcsdData->sdData.pwcFile );

	//		if ( ui32BufferId == 0 ) {
	//			// Not already loaded.  We have to load it.
	//			UnLock();
	//			// Unlocked, but still in a critical section.  Still thread-safe.
	//			ui32BufferId = CreateBuffer( _pcsdData->sdData.pwcFile );
	//			Lock();
	//			// Did it load?
	//			if ( ui32BufferId == 0 ) {
	//				// Nope!
	//				return false;
	//			}

	//			// Set the buffer to respond to soft unloads.
	//			CSoundBufferEx * pssbBuffer = const_cast<CSoundBufferEx *>(reinterpret_cast<const CSoundBufferEx *>(FindBufferById( ui32BufferId )));
	//			pssbBuffer->SetSoftUnloading( true );
	//		}
	//	}


	//	// ui32BufferId is the ID of the buffer to attach to the source.
	//	// Attach it.
	//	if ( !AttachBufferToStaticSource( _pssSource, ui32BufferId ) ) {
	//		return false;
	//	}

	//	_pssSource->SetExpirationDate( _pcsdData->sdData.ui64ExpirationDate );
	//	if ( !_pssSource->Play( _pcsdData->sdData.ui32Loops ) ) {
	//		return false;
	//	}

	//	return true;
	//}

}	// namespace lss
