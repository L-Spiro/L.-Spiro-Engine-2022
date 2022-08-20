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


#ifndef __LSS_SOUNDMANAGER_H__
#define __LSS_SOUNDMANAGER_H__

#include "../LSSSoundLib.h"
#include "../SoundComponents/LSSSoundBuffer.h"
#include "../SoundComponents/LSSSoundListener.h"
#include "../SoundComponents/LSSSoundSource.h"
#include "../SoundComponents/LSSSoundSourceLoadParms.h"
#include "../SoundComponents/LSSSoundSourceTypes.h"
#include "../SoundComponents/LSSStaticSoundSource2d.h"
#include "../SoundComponents/LSSStaticSoundSource3d.h"
#include "../SoundComponents/LSSStreamingSoundSource2d.h"
#include "../SoundComponents/LSSStreamingSoundSourceStreamer.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "Map/LSTLLinearMap.h"
#include "Search/LSSTDSearch.h"
#include "String/LSTLString.h"
#include "Time/LSSTDTime.h"
#include "Vector/LSTLVectorPoD.h"
#include <cassert>

namespace lss {

	/**
	 * Class CSoundManager
	 *
	 * Description: The basic manager for sound throughout the engine.
	 */
	class CSoundManager {
		friend class CSoundLib;

	public :
		// == Various constructors.
		LSE_CALLCTOR									CSoundManager();
		virtual LSE_CALLCTOR							~CSoundManager();


		// == Enumerations.
		/**
		 * Error conditions.
		 */
		enum LSS_ERRORS {
			LSS_E_INVALID_BUFFER						= 0,
		};

		/**
		 * Event types.
		 */
		enum LSS_EVENT_TYPES {
			LSS_ET_SOUND_JUST_STARTED,
			LSS_ET_SOUND_JUST_ENDED,
		};


		// == Types.
		/**
		 * Callback function prototype to handle sound events synchronously.
		 *
		 * \param _uiptrUserPtr User data passed to the synchronous update routine in the sound manager.
		 * \param _pssSource The sound source that raised the event.
		 * \param _ui32EventType The type of event raised.
		 * \param _psmManager A pointer to the sound manager that owns the sound.
		 */
		typedef void (LSE_CALL *						PfSoundEventHandler)( uintptr_t _uiptrUserPtr, CSoundSource * _pssSource,
			uint32_t _ui32EventType, CSoundManager * _psmManager );


		// == Functions.
		/**
		 * Sets the allocator used by the sound system.  This causes all resources
		 *	to be reset and lost.  This should be done only once at start-up,
		 *	before any resources are created.  _paAllocator may be NULL.
		 *
		 * \param _paAllocator The allocator to be used by this class or NULL to use the global heap.
		 */
		virtual void LSE_CALL							SetAllocator( CAllocator * _paAllocator );

		/**
		 * Gets our allocator.
		 *
		 * \return Returns the allocator used by the sound manager.
		 */
		CAllocator * LSE_CALL							GetAllocator();

		/**
		 * Sets the current device.  Causes all existing resources to be reloaded
		 *	automatically.  Only one device may be set at a time.  If _pcDevice
		 *	is NULL or an empty string, the default device is used.
		 *
		 * \param _pcDevice The device to set.
		 * \param _pstrError Returned error if any.
		 * \return Returns true if the device was set.
		 */
		virtual LSBOOL LSE_CALL							SetDevice( const char * _pcDevice, CString * _pstrError = NULL );

		/**
		 * Runs the sound manager loop.  The loop is run on a new thread.
		 * This causes the sound manager to tick repeatedly, updating sources and the listener until requested
		 *	to stop.
		 * During a single tick, sources and the listener may not be accessed.  The loop rests for 16 milliseconds
		 *	between ticks, making spins/waits unlikely.
		 */
		void LSE_CALL									Run();
		
		/**
		 * Stops the sound thread.  Does not return until the thread as been fully stopped and closed.
		 */
		void LSE_CALL									Stop();

		/**
		 * Tick once.  Updates time and all objects.
		 */
		void LSE_CALL									Tick();

		/**
		 * Perform a synchronous update of all sound sources.  This is always executed on the main game thread and is used
		 *	for issuing synchronous game events such as the beginning/ending of sounds.
		 *
		 * \param _uiptrUserPtr User data passed during the update on the main thread.
		 * \param _pfHandler Pointer to a function used to handle events synchronously.
		 */
		void LSE_CALL									SynchronousUpdate( uintptr_t _uiptrUserPtr,
			PfSoundEventHandler _pfHandler );

		/**
		 * Stop and remove all sources and buffers, and apply default parameters to the listener.
		 */
		virtual void LSE_CALL							ResetAllSounds();

		/**
		 * Perform a soft reset.  This removes unreferenced data and stops sounds that are not
		 *	soft-reset-safe.
		 */
		virtual void LSE_CALL							SoftReset();

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
		uint32_t LSE_CALL								AddBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
			uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain = true, uintptr_t _uiptrUserData = 0, CString * _pstrError = NULL );

		/**
		 * Removes a buffer by ID.
		 *	** Must not be locked.
		 *
		 * \param _ui32Id ID of teh buffer to remove.
		 */
		void LSE_CALL									RemBuffer( uint32_t _ui32Id );

		/**
		 * Removes all unreferenced buffers.
		 *	** Must not be locked.
		 *
		 * \return Returns the total number of buffers that are referenced by static sources and were not removed.
		 */
		uint32_t LSE_CALL								RemAllUnreferencedBuffers();

		/**
		 * Determine if all references to a buffer are from streaming sources.
		 *
		 * \param _ui32BufferId The buffer ID to check.
		 * \return Returns false if any static sources reference the given buffer ID.
		 */
		LSBOOL LSE_CALL									AllReferencesToBufferAreStreaming( uint32_t _ui32BufferId ) const;

		/**
		 * Find a buffer given its ID.  Uses a binary search.
		 *	** Must be locked.
		 *
		 * \param _ui32Id The ID of the buffer to be found.
		 * \return Returns a pointer to the buffer if found, NULL otherwise.
		 */
		const CSoundBuffer * LSE_CALL					FindBufferById( uint32_t _ui32Id ) const;

		/**
		 * Find a buffer given its ID.  Uses a binary search.  If the buffer pointer is returned, this object remains locked.
		 *	When done with the returned pointer, call UnLock().  The returned pointer must not be used after calling UnLock().
		 *
		 * \param _ui32Id The ID of the buffer to be found.
		 * \return Returns a pointer to the buffer if found, NULL otherwise.  If NULL is returned, do not call UnLock().
		 */
		const CSoundBuffer * LSE_CALL					FindBufferByIdAndLock( uint32_t _ui32Id ) const;

		/**
		 * Lock this object for thread-safe access to its contents.  Accessing buffers, sources, and the listener
		 *	while not locked will result in failure.
		 */
		void LSE_CALL									Lock() const;

		/**
		 * Unlock its contents.  Items cannot be added while locked.
		 */
		void LSE_CALL									UnLock() const;

		/**
		 * Gets the listener.  This allows reading and writing, so you can set its position/volume/etc.
		 *	directly.  Changes will take place on the next tick of the sound manager.
		 *	** Must be locked.
		 *
		 * \return Returns a reference to the listener that can be used to modify its state.
		 */
		CSoundListener & LSE_CALL						GetListener();

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
		CSoundSource * LSE_CALL							AddSourceAndLock( LSS_SOUND_SOURCE _ssType,
			const LSS_SOUND_SOURCE_LOAD_PARMS &_sslpParms, CString * _pstrError = NULL );

		/**
		 * Removes a source by ID.
		 *	** Must not be locked.
		 *
		 * \param _ui32Id ID of the source to remove.
		 */
		void LSE_CALL									RemSource( uint32_t _ui32Id );

		/**
		 * Gets a source by ID.  If found, returns a pointer to the source.
		 * If not found, NULL is returned.
		 *	** Must be locked.
		 *
		 * \param _ui32Id ID of the source to find.
		 * \return Returns the found source or NULL.
		 */
		CSoundSource * LSE_CALL							FindSourceById( uint32_t _ui32Id );

		/**
		 * Same as FindSourceById(), except that it automatically locks the sound
		 *	manager.  If NULL is returned, the sound manager is unlocked.  If a pointer
		 *	is returned, the sound manager must be unlocked as soon as the pointer is done being used.
		 *
		 * \param _ui32Id ID of the source to find.
		 * \return Returns the found source or NULL.
		 */
		CSoundSource * LSE_CALL							FindSourceByIdAndLock( uint32_t _ui32Id );

		/**
		 * Attach a buffer to a static source.  Does not need to be locked.  Returns false if the attachment
		 *	was not made for any reason (source or buffer ID's are invalid).
		 *
		 * \param _ui32SrcId ID of the source.
		 * \param _ui32BufferId ID of the buffer to attach to the source.
		 * \return Returns true if both ID's are valid.
		 */
		LSBOOL LSE_CALL									AttachBufferToStaticSource( uint32_t _ui32SrcId,
			uint32_t _ui32BufferId );

		/**
		 * Attach a buffer to a static source.  Returns false if the attachment was not made for any reason
		 *	(source is not static or buffer ID is invalid).
		 *	** Must be locked.
		 *
		 * \param _pssSource Source to which to attach the buffer.
		 * \param _ui32BufferId ID of the buffer to attach to the source.
		 * \return Returns true if the buffer ID is valid.
		 */
		LSBOOL LSE_CALL									AttachBufferToStaticSource( CSoundSource * _pssSource,
			uint32_t _ui32BufferId );

		/**
		 * Stop and delete all static sources.
		 *	** Must not be locked.
		 */
		void LSE_CALL									StopAndDeleteAllStaticSources();

		/**
		 * The most common function performed on the listener is to set its position.  This function makes
		 *	doing so easy.
		 * Does not need to be locked.
		 *
		 * \param _svPos New listener position.
		 */
		void LSE_CALL									SetListenerPos( const CVector3 &_svPos );

		/**
		 * Setting the listener velocity is common.
		 * Does not need to be locked.
		 *
		 * \param _svVel New listener velocity.
		 */
		void LSE_CALL									SetListenerVel( const CVector3 &_svVel );

		/**
		 * This function allows setting both position and velocity at the same time on the listener.
		 * Does not need to be locked.
		 *
		 * \param _svPos New listener position.
		 * \param _svVel New listener velocity.
		 */
		void LSE_CALL									SetListenerPosAndVel( const CVector3 &_svPos, const CVector3 &_svVel );

		/**
		 * Sets the master volume.  Ths sets the gain on the listener internally.
		 * Does not need to be locked.
		 *
		 * \param _fVolume New master volume.
		 */
		void LSE_CALL									SetMasterVolume( LSSREAL _fVolume );

		/**
		 * Gets the listener for read only.  Must call UnLock() when done with the listener!
		 *
		 * \return Returns the lister.
		 */
		const CSoundListener & LSE_CALL					GetListenerAndLock() const;

		/**
		 * Gets the listener for read and write.  Must call UnLock() when done with the listener!
		 *
		 * \return Returns the lister.
		 */
		CSoundListener & LSE_CALL						GetListenerAndLock();

		/**
		 * Gets a default source-load parameter structure.  Modify after as you please.
		 *
		 * \return Returns a LSS_SOUND_SOURCE_LOAD_PARMS structure with default parameters.
		 */
		static LSS_SOUND_SOURCE_LOAD_PARMS LSE_CALL		DefaultSourceLoad();


	protected :
		// == Members.
		// Our critical section for thread safety.
		mutable CCriticalSection						m_csCriticalSection;

		// Allocator we use.  May not be NULL.
		CAllocator *									m_paAllocator;

		// Is our allocator private?
		LSBOOL											m_bAllocatorIsPrivate;

		// Lock count.
		mutable int32_t									m_i32LockCount;

		// Our time manager.
		CTime											m_tTime;

		// Flag to tell us to stop ticking in our loop.
		volatile LSBOOL									m_bRun;

		// The thread on which the sound manager is running.
		LSH_THREAD										m_hThread;

#ifdef LSS_USEOPENAL
		// The current device.
		ALCdevice *										m_pdDevice;

		// The current context.
		ALCcontext *									m_pcContext;
#endif	// #ifdef LSS_USEOPENAL

		// List of buffers.
		CVectorPoD<CSoundBuffer *, uint32_t>			m_psbBuffers;

		// List of sources.
		CLinearMap<uint32_t, CSoundSource *>			m_pssSources;

		// List of streamers.
		CVectorPoD<IStreamingSoundSourceStreamer *>		m_pssssStreamers;

		// The listener.  There is only one.
		CSoundListener									m_slListener;


		// == Functions.
		/**
		 * Close the current device, if any.  All buffers and sources are destroyed.
		 * But sound resources are not lost.  Only their connections to the hardware are.
		 */
		virtual void LSE_CALL							ResetDevice();

		/**
		 * Reset the whole sound engine.  Releases all resources.
		 */
		virtual void LSE_CALL							ShutDown();

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
		virtual CSoundBuffer * LSE_CALL					NewBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
			uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, LSBOOL _bRetain, uintptr_t _uiptrUserData ) const;

		/**
		 * Destroy a buffer the same way it was created.
		 *
		 * \param _psbBuffer The buffer to destroy.
		 */
		virtual void LSE_CALL							DeleteBuffer( CSoundBuffer * &_psbBuffer ) const;

		/**
		 * Creates a new source.  There are a few types of sources.  If you want to add more,
		 *	override this function in a subclass and add them.  This function's only job is
		 *	to create the instances.  It does not link them to the master list etc.
		 *
		 * \param _ssType Type of source to create.
		 * \param _ui32Id Id of the sound to create.  0 is used to generate a unique ID.
		 * \return Returns a pointer to the created sound source.
		 */
		virtual CSoundSource * LSE_CALL					NewSoundSource( LSS_SOUND_SOURCE _ssType, uint32_t _ui32Id ) const;

		/**
		 * Destroy a sound source the same way it was created.Destroy a sound source the same way it was created.
		 *
		 * \param _pssSource The sound source to destroy.
		 */
		virtual void LSE_CALL							DeleteSource( CSoundSource * &_pssSource ) const;

		/**
		 * Adds a streamer to our internal list of streamers.  If the streamer does not have a unique name, the results are
		 *	unpredictable when later referencing this streamer by name.
		 *
		 * \_pssssSteamer The streamer to add.  Must have been created externally.
		 * \return Returns true if there is enough memory to add the streamer.
		 */
		virtual LSBOOL LSE_CALL							AddStreamer( IStreamingSoundSourceStreamer * _pssssSteamer );

		/**
		 * Gets the index in the streamer list where a streamer with the given name should be added. 
		 *
		 * \param _pcStreamerName Name of the streamer whose index is to be located.
		 * \return Returns the index in the streamer list where a streamer with the given name should be added.
		 */
		uint32_t LSE_CALL								GetStreamerIndex( const char * _pcStreamerName ) const;

		/**
		 * Find a streamer by name.  Performs a binary search
		 *
		 * \param _pcName the name of the streamer to find.
		 * \return Returns the index of the found streamer or LSE_MAXU32 if none is found.
		 */
		uint32_t LSE_CALL								FindStreamerByName( const char * _pcStreamerName ) const;

		/**
		 * Creation of the streamers is done by the overriding class.  Thus deletion must also be handled by them.
		 *
		 * \param _pssssSteamer The streamer to be deleted.
		 */
		virtual void LSE_CALL							DeleteStreamer( IStreamingSoundSourceStreamer * _pssssSteamer ) = 0;

		/**
		 * Find a unique ID and assign it to the given sound source.  If no ID could be found, the sound source is deleted.
		 *
		 * \param _pssSource The sound source for which to find a unique ID.
		 */
		void LSE_CALL									FindUniqueSourceId( CSoundSource * &_pssSource ) const;


	private :
		/**
		 * Our search function for buffers by ID.  Used in CSearch::BSearch.
		 *
		 * \param _pvContext Unused.
		 * \param _pui32Left Buffer ID to find.
		 * \param _ppsbRight Buffer to check.
		 * \return Returns the comparison between the two buffers.
		 */
		static int32_t LSE_CCALL						BufferCompare( void * _pvContext,
			const uint32_t * _pui32Left, const CSoundBuffer ** _ppsbRight );

		/**
		 * Our search function for sources by ID.  Used in CSearch::BSearch.
		 *
		 * \param _pvContext Unused
		 * \param _pui32Left Pointer to an uint32_t holding the ID to find.
		 * \param _ppssRight The current object being compared.
		 * \return Returns the lexicographical relationship between the ID and the object.
		 */
		static int32_t LSE_CCALL						SourceCompare( void * _pvContext,
			const uint32_t * _pui32Left, const CSoundSource ** _ppssRight );

		/**
		 * Our search function for streamers by name.  Used in CSearch::BSearch.
		 *
		 * \param _pvContext Unused
		 * \param _pcName Name of the streamer to find.
		 * \param _ppssssRight The current object being compared.
		 * \return Returns the lexicographical relationship between the ID and the object.
		 */
		static int32_t LSE_CCALL						StreamerCompare( void * _pvContext,
			const char * _pcName, const IStreamingSoundSourceStreamer ** _ppssssRight );

		/**
		 * Our second thread.
		 *
		 * \param _lpParameter Pointer to the sound manager.
		 * \return Returns the thread exit code.
		 */
		static uint32_t LSE_CALL						RunThread( void * _lpParameter );

	};

}	// namespace lss

#endif	// __LSS_SOUNDMANAGER_H__
