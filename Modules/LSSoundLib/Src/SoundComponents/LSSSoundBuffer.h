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


#ifndef __LSS_SOUNDBUFFER_H__
#define __LSS_SOUNDBUFFER_H__

#include "../LSSSoundLib.h"
#include "Allocator/LSTLAllocator.h"
#include <cassert>

namespace lss {

	/**
	 * Class CSoundBuffer
	 * \brief Shared raw sound data.
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
	class CSoundBuffer {
		friend class CSoundManager;
		friend class CStaticSoundSource;		// It needs access to m_uiAlBufferId, which cannot be
												//	exposed via accessors because its type and meaning
												//	are specific to OpenAL.
		friend class CStreamingSoundSource;

	public :
		// == Various constructors.
		virtual LSE_CALLCTOR					~CSoundBuffer();


		// == Enumerations.
		/**
		 * An invalid buffer ID (not related to OpenAL etc.)
		 */
		enum LSS_ERROR {
			LSS_E_INVALID_BUFFER				= 0,
		};


		// == Functions.
		/**
		 * Gets our ID.
		 *
		 * \return Returns the unique ID of this buffer.
		 */
		uint32_t LSE_CALL						GetId() const;

		/**
		 * Increase our reference count.
		 */
		void LSE_CALL							IncRef() const;

		/**
		 * Decrease our reference count.
		 */
		void LSE_CALL							DecRef() const;

		/**
		 * Gets our reference count.
		 *
		 * \return Returns the reference count of this object.
		 */
		uint32_t LSE_CALL						GetRef() const;

		/**
		 * Gets the length of this buffer in microseconds.
		 *
		 * \return Returns the length of the buffer in microseconds.
		 */
		uint64_t LSE_CALL						GetLengthInMicros() const;

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
		virtual LSBOOL LSE_CALL					CanSoftUnload() const;


	protected :
		// == Various constructors.
		LSE_CALLCTOR							CSoundBuffer( const uint8_t * _pui8Data, uint32_t _ui32Length,
			uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, CAllocator * _paAllocator, LSBOOL _bRetain, uintptr_t _uiptrUserData );


		// == Enumerations.
#ifdef LSS_USEOPENAL
		// An invalid OpenAL buffer ID.  Used internally.
		enum {
			LSS_INVALID_BUFFER					= 0,
		};
#endif	// #ifdef LSS_USEOPENAL
		

		// == Members.
		// Length of this buffer in microseconds.
		uint64_t								m_ui64BufferTime;

		// Our buffered data in PCM format.
		uint8_t *								m_pui8Data;

		// Size of the data.
		uint32_t								m_ui32Size;

		// Do we retain the data after sending it to the hardware?
		LSBOOL									m_bRetain;

		// Frequency in Hz.
		uint32_t								m_ui32Freq;

		// Bit depth of buffer.
		uint16_t								m_ui16BitDepth;

		// Number of channels.
		uint16_t								m_ui16Channels;

		// Allocator we use.  May not be NULL.
		CAllocator *							m_paAllocator;

		// Our ID.
		uint32_t								m_ui32Id;

		// Reference count.
		mutable int32_t							m_i32Ref;

		// ID counter.
		static uint32_t							m_ui32MasterId;

#ifdef LSS_USEOPENAL
		// The OpenAL ID for this buffer.
		// Created with ::alGenBuffers().
		ALuint									m_uiAlBufferId;

#endif	// #ifdef LSS_USEOPENAL

	private :
		// == Functions.
		/**
		 * Notifies us that the given device has become active and
		 *	we should send our buffer to it.  A return of false from this
		 *	function indicates the buffer could not be uploaded.  This is
		 *	considered a critical error by the engine.
		 *
		 * \return Returns true if the buffer could be reloaded.
		 */
		virtual LSBOOL LSE_CALL					DeviceActivated();

		/**
		 * Notifies us that the device is about to be lost (changed).  We need
		 *	to sever our connections to it, although we may retain our data/
		 *	settings.
		 */
		virtual void LSE_CALL					DeviceWillClose();

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
		virtual LSBOOL LSE_CALL					GetOriginalData();
	};

}	// namespace lss

#endif	// __LSS_SOUNDBUFFER_H__
