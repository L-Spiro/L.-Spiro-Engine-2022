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
 * Description: Uses the IStreamingSoundSourceStreamer interface to provide a way to stream audio data
 *	from a wave file into a CStreamingSoundSource object.
 */


#ifndef __LSS_WAVESTREAMINGSOUNDSOURCESTREAMER_H__
#define __LSS_WAVESTREAMINGSOUNDSOURCESTREAMER_H__

#include "../LSSSoundLib.h"
#include "../SoundComponents/LSSStreamingSoundSourceStreamer.h"
#include "../SoundComponentsEx/LSSStreamerTypes.h"
#include "HighLevel/LSFFileMap.h"
#include "LSSWaveLoader.h"

namespace lss {

	/**
	 * Class CWaveStreamingSoundSourceStreamer
	 * \brief A way to stream audio data from .wav files.
	 *
	 * Description: Uses the IStreamingSoundSourceStreamer interface to provide a way to stream audio data
	 *	from a wave file into a CStreamingSoundSource object.
	 */
	class CWaveStreamingSoundSourceStreamer : public IStreamingSoundSourceStreamer {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CWaveStreamingSoundSourceStreamer( const char * _pcFile,
			CAllocator * _paAllocator );
		virtual LSE_CALLCTOR						~CWaveStreamingSoundSourceStreamer();


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
			uint8_t * &_pui8Data, CAllocator * _paAllocator ) const;

		/**
		 * Gets the type of this streamer.  Returns LSS_ST_WAVEFILE along with the parent type.
		 *
		 * \return Returns the bitmask representing the type of streamer this instance is.
		 */
		virtual uint32_t LSE_CALL					GetType() const;

		/**
		 * Gets the name of the streamer.  Here it is the file path.
		 *
		 * \return Returns a constant char * to the name of the streamer.
		 */
		virtual const char * LSE_CALL				GetName() const;

		/**
		 * Gets the frequency of the data.
		 *
		 * \return Returns the frequency of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetFrequency() const;

		/**
		 * Gets the number of bits per channel of the data.
		 *
		 * \return Returns the number of bits per channel of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetBits() const;

		/**
		 * Gets the number of channels of the data.
		 *
		 * \return Returns the number of channels of the data provided by this class.
		 */
		virtual uint32_t LSE_CALL					GetChannels() const;

		/**
		 * Gets the length of the data in microseconds.
		 *
		 * \return Returns the length, in microseconds, of the audio provided by this class.
		 */
		virtual uint64_t LSE_CALL					GetAudioLength() const;


	protected :
		// == Members.
		/**
		 * The path to this wave file in UTF-8 format.  This also serves as the name
		 *	of the object.
		 */
		char *									m_pcPath;

		/**
		 * The allocator we use.
		 */
		CAllocator *								m_paAllocator;

		/**
		 * The file map we use to get data from the file.
		 */
		CFileMap									m_fmMap;

		/**
		 * The wave file information.
		 */
		CWaveLoader::LSE_WAVE_FILE_INFO				m_wfiInfo;

		/**
		 * Length of the sound in microseconds.
		 */
		uint64_t									m_ui64SoundTime;

	private :
		typedef IStreamingSoundSourceStreamer		Parent;
	};

}	// namespace lss

#endif	// __LSS_WAVESTREAMINGSOUNDSOURCESTREAMER_H__
