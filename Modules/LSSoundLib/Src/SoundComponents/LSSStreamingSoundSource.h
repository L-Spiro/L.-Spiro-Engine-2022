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


#ifndef __LSS_STREAMINGSOUNDSOURCE_H__
#define __LSS_STREAMINGSOUNDSOURCE_H__

#include "../LSSSoundLib.h"
#include "LSSStaticSoundSource.h"
#include "LSSStreamingSoundSourceStreamer.h"
#include "Vector/LSTLVectorPoD.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// The length, in microseconds of the buffers used for streaming.
#ifndef LSS_STREAMING_BUFFER_SIZE
#define LSS_STREAMING_BUFFER_SIZE							((1ULL * 1000000ULL) / 2)
#endif	// #ifndef LSS_STREAMING_BUFFER_SIZE

namespace lss {

	/**
	 * Class CStreamingSoundSource
	 * \brief A source used with streaming audio data.
	 *
	 * Description: A source used for streaming.  Provides an interface for continuous loading of buffer data in
	 *	order to keep the sound playing.  By default, streaming sounds ignore soft resets.
	 */
	class CStreamingSoundSource : public CStaticSoundSource {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CStreamingSoundSource( uint32_t _ui32Id );
		virtual LSE_CALLCTOR								~CStreamingSoundSource();

		// == Functions.
		/**
		 * Gets the type of this source.
		 *
		 * \return Returns one of the LSS_SOUND_SOURCE values.
		 */
		virtual LSS_SOUND_SOURCE LSE_CALL					GetType() const;

		/**
		 * Is this source static (non-streaming)?
		 *
		 * \return Returns true if the source is static, false if streaming.
		 */
		virtual LSBOOL LSE_CALL								Static() const;

		/**
		 * This class uses its own private buffers internally.  We override this function to make it a
		 *	do-nothing function.
		 *
		 * \param _psbBuffer Not used.
		 */
		virtual void LSE_CALL								SetBuffer( const CSoundBuffer * _psbBuffer );

		/**
		 * Creates a stream of audio using the provided IStreamingSoundSourceStreamer interface.
		 *
		 * \param _pssssSource The interface that provides this class with the data it needs to stream audio.
		 * \param _paAllocator The allocator to be used for all allocations made by this class.  Must not be NULL.
		 * \return Returns true if the creation of the sound stream succeeded.
		 */
		LSBOOL LSE_CALL										CreateStream( const IStreamingSoundSourceStreamer * _pssssSource,
			CAllocator * _paAllocator );

		/**
		 * Reset the stream.  Deallocates everything.
		 */
		void LSE_CALL										Reset();

		/**
		 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
		 * Time is given in microseconds.
		 *
		 * \param _ui64TimeSinceLastUpdate Microseconds by which to update.
		 */
		virtual void LSE_CALL								Tick( uint64_t _ui64TimeSinceLastUpdate );

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
		virtual LSBOOL LSE_CALL								Play( uint32_t _ui32LoopCount );

		/**
		 * Gets the streamer associated with this sound stream.
		 *
		 * \return Returns the streamer used by this source, which may be NULL.
		 */
		const IStreamingSoundSourceStreamer * LSE_CALL		GetStreamer() const;

		/**
		 * Sets the value indicating whether this source should resist soft resets or not.  A soft reset
		 *	is a reset that stops sources that do not resist soft resets, then releases all unreferenced
		 *	data (sources/buffers).  This is not the same as a soft unload, which can happen at any time
		 *	just to save RAM and works only on expired data.  A soft reset will stop sources that do not
		 *	resist the stopping action by returning true via this function.
		 *
		 * \return Returns false.  Override this class to return any other value.
		 */
		virtual LSBOOL LSE_CALL								CanSoftReset() const;


	protected :
		// == Types.
		/**
		 * The buffers used by CStreamingSoundSource.
		 */
		typedef struct LSS_STREAMING_BUFFER {
			/**
			 * The buffer.
			 */
			CSoundBuffer *									psbBuffer;

			/**
			 * The start of the buffer in microseconds.
			 */
			uint64_t										ui64StartTime;

			/**
			 * Length of the buffer in microseconds.
			 */
			uint64_t										ui64Length;

			/**
			 * Is the buffer in use?
			 */
			LSBOOL											bUsed;
		} * LPLSS_STREAMING_BUFFER, * const LPCLSS_STREAMING_BUFFER;

		/**
		 * A list of LSS_STREAMING_BUFFER objects.
		 */
		typedef CVectorPoD<LSS_STREAMING_BUFFER>			LSS_STREAMING_BUFFER_LIST, * LPLSS_STREAMING_BUFFER_LIST, * const LPCLSS_STREAMING_BUFFER_LIST;


		// == Members.
		/**
		 * The 4 buffers we used for streaming.
		 */
		LSS_STREAMING_BUFFER_LIST							m_sblBuffers;

		/**
		 * Allocator we use.  May not be NULL.
		 */
		CAllocator *										m_paAllocator;

		// The source of audio data for our stream.
		const IStreamingSoundSourceStreamer *				m_pssssSource;

		// The number of bytes in each buffer.
		uint64_t											m_ui64BufferSize;

		// How far into the sound have we queued?  In microseconds.
		uint64_t											m_ui64Queued;

		// Is the device activated?
		LSBOOL												m_bHaveDevice;


		// == Functions.
		/**
		 * Sets the source source.
		 *
		 * \param _pssssSource The source to use for the data we need to operate as a streaming sound source.
		 */
		void LSE_CALL									SetSource( const IStreamingSoundSourceStreamer * _pssssSource );

		/**
		 * Check to see if we need to queue any buffers.  If so, buffers are automatically queued.
		 */
		void LSE_CALL									CheckQueue();

		/**
		 * Check for buffers that need to be unqueued.  If any are found, they are unqueued.
		 */
		void LSE_CALL									CheckUnqueue();

		/**
		 * Gets the oldest buffer still active.
		 *
		 * \return Returns the index of the running buffer with the lowest time stamp.  If no buffers are living, LSE_MAXU32 is returned.
		 */
		uint32_t LSE_CALL								GetOldestBufferIndex() const;

		/**
		 * Find the first free buffer.
		 *
		 * \return Returns the first free buffer.
		 */
		uint32_t LSE_CALL								GetFreeBuffer() const;

		/**
		 * Buffer a given amount of time in the sound.  This function finds the first available buffer
		 *	and attempts to buffer the requested data into it.
		 *
		 * \param _ui64From Time at the beginning of the buffer, in microseconds.
		 * \param _ui64Length Length to buffer, in microseconds.
		 * \return If buffering failed or there are no available buffers, returns false.
		 */
		LSBOOL LSE_CALL									Buffer( uint64_t _ui64From, uint64_t _ui64Length );

		/**
		 * Unbuffer an a buffer.
		 *
		 * \param _ui32Index The index of the buffer to remove.
		 */
		void LSE_CALL										UnBuffer( uint32_t _ui32Index );

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
		virtual void LSE_CALL								DeviceWillClose();


	private :
		typedef CStaticSoundSource							Parent;
	};

}	// namespace lss

#endif	// __LSS_STREAMINGSOUNDSOURCE_H__
