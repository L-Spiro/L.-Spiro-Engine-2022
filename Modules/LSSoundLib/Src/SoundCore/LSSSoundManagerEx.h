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


#ifndef __LSS_CORESOUNDMANAGER_H__
#define __LSS_CORESOUNDMANAGER_H__

#include "../LSSSoundLib.h"
#include "../Ogg/LSSOggStreamingSoundSourceStreamer.h"
#include "../SoundComponents/LSSStaticSoundSource2d.h"
#include "../SoundComponents/LSSStaticSoundSource3d.h"
#include "../SoundComponentsEx/LSSSoundBufferEx.h"
#include "../Wave/LSSWaveStreamingSoundSourceStreamer.h"
#include "LSSSoundManager.h"
//#include "LSESfxData.h"
#include "Search/LSSTDSearch.h"
#include "Vector/LSMVector2.h"

namespace lss {

	/**
	 * Class CSoundManagerEx
	 * \brief Add core functionality to the sound library.
	 *
	 * Description: Add our own functionality to the sound library.  We extend it to support loading of files
	 *	and a few special effects.  We also make it easy to play sounds and include the ability to delay-load
	 *	sounds and perform automatic unloading of sounds. 
	 */
	class CSoundManagerEx : public CSoundManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR									CSoundManagerEx();
		virtual LSE_CALLCTOR							~CSoundManagerEx();


		// == Functions.
		/**
		 * Sets the allocator used by the sound system.  This causes all resources
		 *	to be reset and lost.  This should be done only once at start-up,
		 *	before any resources are created.  _paAllocator may be NULL.
		 *
		 * \param _paAllocator The allocator to be used by the sound engine.
		 */
		virtual void LSE_CALL							SetAllocator( CAllocator * _paAllocator );
		
		/**
		 * Creates a buffer from a file.  If the buffer has already been created, no operation is performed.
		 *	** Must not be locked.
		 *
		 * \param _pcFile Sound file to load.
		 * \return Returns the ID of the buffer on success, or 0 on failure.
		 */
		uint32_t LSE_CALL								CreateBuffer( const char * _pcFile );

		/**
		 * Find a buffer by path.
		 *
		 * \param _pwcPath The path for which to search the loaded buffers.
		 * \return Returns the buffer ID or 0 on failure.
		 */
		uint32_t LSE_CALL								FindBufferByPath( const char * _pwcPath ) const;

		/**
		 * Gets a buffer path by buffer ID.  _pcRet must be at least LSF_MAX_PATH characters long.
		 *
		 * \param _ui32Id ID of the buffer whose path is to be returned.
		 * \param _pcRet The string to be filled with the returned path, if any.
		 * \return Returns NULL if the ID is invalid or if the buffer was not created via a path,
		 *	otherwise _pcRet is filled with the path and returned.
		 */
		char * LSE_CALL								GetBufferPathById( uint32_t _ui32Id, char * _pcRet ) const;

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
		CStaticSoundSource3d * LSE_CALL					Create3dStaticSoundSourceAndLock( const CVector3 &_vPos,
			LSREAL _fGain = LSM_ONE,
			LSREAL _fMaxDistance = static_cast<LSREAL>(256.0),
			const char * _pcFile = NULL,
			uint32_t _ui32Id = 0 );
		
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
		CStaticSoundSource2d * LSE_CALL					Create2dStaticSoundSourceAndLock( const CVector2 &_vPos,
			LSREAL _fGain = LSM_ONE,
			const char * _pcFile = NULL );
		
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
		/*uint32_t LSE_CALL								Play3dSound( uint32_t _ui32SfxId,
			const CVector3 &_vPos,
			LSREAL _fGain = LSM_ONE,
			LSREAL _fMaxDistance = static_cast<LSREAL>(256.0) );*/

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
		/*uint32_t LSE_CALL								Play2dSound( uint32_t _ui32SfxId,
			const CVector2 &_vPos,
			LSREAL _fGain = LSM_ONE );*/

		/**
		 * Register a sound-effect file.  Returns the unique ID of the data which can be used to play the sound later.
		 * If registration fails (due to memory), 0 is returned.
		 *
		 * \param _sdData An LSE_SFX_DATA structure containing the parameters for the sound effect.
		 * \return The ID of the registered sound effect or 0 upon failure.
		 */
		//uint32_t LSE_CALL								RegisterSfx( const LSE_SFX_DATA &_sdData );

		/**
		 * Unregister a sound effect by its unique ID.
		 *
		 * \param _ui32SfxId The ID of the registered sound effect to unregister.
		 * \return Returns true if the given ID matches a registered sound effect.
		 */
		//LSBOOL LSE_CALL								UnRegisterSfx( uint32_t _ui32Id );

		/**
		 * Register a streaming sound in a wave file.  The file must be valid and contain valid PCM data in .wav format.
		 *
		 * \param _pcFile The UTF-8 path to the file.
		 * \return Returns true if the sound was created.
		 */
		LSBOOL LSE_CALL									RegisterWaveForStream( const char * _pcFile );

		/**
		 * Register a streaming sound in an Ogg file.  The file must be a valid .ogg file.
		 *
		 * \param _pcFile The UTF-8 path to the file.
		 * \return Returns true if the sound was created.
		 */
		LSBOOL LSE_CALL									RegisterOggForStream( const char * _pcFile );

		/**
		 * Register a streaming sound in a supported sound file.  The file must be a valid sound file.
		 *
		 * \param _pcFile The UTF-8 path to the file.
		 * \return Returns true if the sound was created.
		 */
		LSBOOL LSE_CALL									RegisterSoundForStream( const char * _pcFile );

		/**
		 * Stream a 2D sound.  The sound is referenced by name.  If registered with Register*ForStream(), the name is the file path.
		 *	** Must not be locked.
		 *
		 * \param _pcName Name of the registered stream to begin playing.
		 * \return Returns the created streaming sound if successful, NULL otherwise.  If NULL is not returned, UnLock() must be called when
		 *	done with the pointer.
		 */
		CStreamingSoundSource * LSE_CALL				Create2dStreamSoundSourceAndLock( const char * _pcName, const CVector2 &_vPos, LSREAL _fGain = LSM_ONE );

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
		uint32_t LSE_CALL								Play2dStreamEx( const char * _pcName,
			const CVector2 &_vPos = CVector2( LSM_ZERO, LSM_ZERO ),
			LSREAL _fGain = LSM_ONE,
			uint32_t _ui32Loop = CSoundSource::LSS_LM_INFINITE,
			LSBOOL _bOnlyPlayIfNotPlaying = true,
			LSBOOL _bClearAllStreamsIfPlayed = true );

		/**
		 * Determine if the given sound is already streaming.
		 *
		 * \param _pcName name of the stream to check.  This name must match one of the names provided in a call to one of the Register*ForStream() functions.
		 * \return Returns the ID of the sound if it is already playing.  If the sound does not exist or exists but is not in the playing or paused states,
		 *	0 is returned.
		 */
		uint32_t LSE_CALL								SoundIsStreaming( const char * _pcName ) const;

		/**
		 * Clear all straming sounds whose ID's do not match the given ID.
		 *
		 * \param _ui32Id ID of the sound not to clear.
		 */
		void LSE_CALL									ClearAllStreamsExcept( uint32_t _ui32Id );

		/**
		 * Stop and remove all sources and buffers, and apply default parameters to the listener.
		 */
		virtual void LSE_CALL							ResetAllSounds();

		/**
		 * Perform a soft reset.  This removes unreferenced data and stops sounds that are not
		 *	soft-reset-safe.
		 */
		virtual void LSE_CALL							SoftReset();

	protected :
		// == Types.
		// SFX data.  When the user wants to register a sound-effect for easy playing later.
		// We extend LSE_SFX_DATA to add an ID that we can use to look up the data.
		typedef struct LSE_CORE_SFX_DATA {
			// The unique ID of for this data.
			uint32_t									ui32Id;
			
			// The ID of the buffer or 0 if not loaded.
			uint32_t									ui32BufferId;
			
			// The data.
			//LSE_SFX_DATA								sdData;
		} * LPLSE_CORE_SFX_DATA, * LPCLSE_CORE_SFX_DATA;


		// == Members.
		// The SFX data registered with the system.  Sorted by ID.
		CVectorPoD<LSE_CORE_SFX_DATA>					m_csdSfxData;
		
		/**
		 * The unique ID for registered SFX data structures.
		 */
		static uint32_t									m_ui32SfxId;


		// == Functions.
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
		virtual CSoundBuffer * LSE_CALL					NewBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
			uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain, uintptr_t _uiptrUserData ) const;

		/**
		 * Destroy a buffer the same way it was created.
		 *
		 * \param _psbBuffer The buffer to destroy.
		 */
		virtual void LSE_CALL							DeleteBuffer( CSoundBuffer * &_psbBuffer ) const;

		/**
		 * Create extended sound sources not available in the case sound manager.
		 *
		 * \param _ssType The type of sound source to create.
		 * \param _ui32Id Id of the sound to create.  0 is used to generate a unique ID.
		 * \return Returns a pointer to the created sound source of the given type.
		 */
		virtual CSoundSource * LSE_CALL					NewSoundSource( LSS_SOUND_SOURCE _ssType, uint32_t _ui32Id ) const;

		/**
		 * Destroy a sound source the same way it was created.
		 *
		 * \param _pssSource The sound source to delete.
		 */
		virtual void LSE_CALL							DeleteSource( CSoundSource * &_pssSource ) const;

		/**
		 * Creation of the streamers is done by the overriding class.  Thus deletion must also be handled by them.
		 *
		 * \param _pssssSteamer The streamer to be deleted.
		 */
		virtual void LSE_CALL							DeleteStreamer( IStreamingSoundSourceStreamer * _pssssSteamer );

		// Find SFX data by ID fast.  Returns NULL if not found.
		//const LSE_CORE_SFX_DATA * LSE_CALL				FindSfxData( uint32_t _ui32Id ) const;

		//// Our search function for SFX data by ID.  Used in CSearch::BSearch.
		//static int32_t LSE_CCALL						SfxCompare( void * _pvContext,
		//	const uint32_t * _pui32Left, const LSE_CORE_SFX_DATA * _pcsdRight );


	private :
		typedef CSoundManager							Parent;


		// == Functions.
		/**
		 * Play any static source registered as a sound effect.  Assumes thread safety is ensured from the caller.
		 *
		 * \param _pssSource The source to play.
		 * \param _pcsdData Registered sound-effect data to apply to the source.
		 * \return Returns true on success or false otherwise.
		 */
		/*LSBOOL LSE_CALL									PlayStaticSource( CSoundSource * _pssSource,
			const LSE_CORE_SFX_DATA * _pcsdData );*/
	};

}	// namespace lss

#endif	// __LSS_CORESOUNDMANAGER_H__
