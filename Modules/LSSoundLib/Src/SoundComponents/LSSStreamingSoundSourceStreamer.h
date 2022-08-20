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
 * Description: A streaming sound source needs an abstract way to stream data from a source (file/memory/
 *	network/etc.)  This class provides an interface for the retrieval of sound data for use by the
 *	CStreamingSoundSource class.
 */


#ifndef __LSS_STREAMINGSOUNDSOURCESTREAMER_H__
#define __LSS_STREAMINGSOUNDSOURCESTREAMER_H__

#include "../LSSSoundLib.h"
#include "Allocator/LSTLAllocator.h"

namespace lss {

	/**
	 * Types of data-source streamers.  Extend this to add custom ones.
	 */
	enum LSS_DATA_SOURCE_STREAMERS {
		LSS_DSS_BASE								= (1 << 0),
	};

	/**
	 * Class IStreamingSoundSourceStreamer
	 * \brief An interface for the retrieval of sound data for use by the CStreamingSoundSource class.
	 *
	 * Description: A streaming sound source needs an abstract way to stream data from a source (file/memory/
	 *	network/etc.)  This class provides an interface for the retrieval of sound data for use by the
	 *	CStreamingSoundSource class.
	 */
	class IStreamingSoundSourceStreamer {
	public :
		// == Various constructors.
		LSE_CALLCTOR								IStreamingSoundSourceStreamer();
		virtual LSE_CALLCTOR						~IStreamingSoundSourceStreamer();


		// == Functions.
		/**
		 * Retrieve sound data for use by the CStreamingSoundSource class.
		 *
		 * \param _ui64From The offset within the data source to get the data, in microseconds.
		 * \param _ui64Length The length of the data to get, in microseconds.  If the function succeeds,
		 *	this holds the returned length of the data in bytes.
		 * \param _pui8Data The returned data.
		 * \param _paAllocator The allocator to be used to allocate the data to which _pui8Data will point
		 *	on a successful return.
		 * \return Returns true if successful, false otherwise.
		 */
		virtual LSBOOL LSE_CALL						GetData( uint64_t _ui64From, uint64_t &_ui64Length,
			uint8_t * &_pui8Data, CAllocator * _paAllocator ) const = 0;

		/**
		 * Gets the type of this streamer.  The sound engine defines only this interface. which is of type
		 *	LSS_DSS_BASE.
		 *
		 * \return Returns the bitmask representing the type of streamer this instance is.
		 */
		virtual uint32_t LSE_CALL					GetType() const = 0;

		/**
		 * Gets the name of the streamer.  This must uniquely identify the streamer.
		 *
		 * \return Returns a constant char * to the name of the streamer.
		 */
		virtual const char * LSE_CALL				GetName() const = 0;

		/**
		 * Gets the frequency of the data.
		 *
		 * \return Returns the frequency of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetFrequency() const = 0;

		/**
		 * Gets the number of bits per channel of the data.
		 *
		 * \return Returns the number of bits per channel of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetBits() const = 0;

		/**
		 * Gets the number of channels of the data.
		 *
		 * \return Returns the number of channels of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetChannels() const = 0;

		/**
		 * Gets the length of the data in microseconds.
		 *
		 * \return Returns the length, in microseconds, of the audio provided by this class.
		 */
		virtual uint64_t LSE_CALL					GetAudioLength() const = 0;

		/**
		 * Increase the reference count.
		 */
		void LSE_CALL								IncRef() const;

		/**
		 * Decrease the reference count.
		 */
		void LSE_CALL								DecRef() const;

		/**
		 * Gets the reference count.
		 *
		 * \return Returns the reference count on this object.
		 */
		uint32_t LSE_CALL							GetRef() const;


	protected :
		// == Members.
		/**
		 * The reference count on this object.
		 */
		mutable uint32_t							m_ui32RefCount;

	};

}	// namespace lss

#endif	// __LSS_STREAMINGSOUNDSOURCESTREAMER_H__
