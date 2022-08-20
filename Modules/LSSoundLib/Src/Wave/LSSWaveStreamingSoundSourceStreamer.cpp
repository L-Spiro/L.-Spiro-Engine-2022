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

#include "LSSWaveStreamingSoundSourceStreamer.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CWaveStreamingSoundSourceStreamer::CWaveStreamingSoundSourceStreamer( const char * _pcFile,
		CAllocator * _paAllocator ) :
		m_pcPath( NULL ),
		m_paAllocator( _paAllocator ),
		m_ui64SoundTime( 0ULL ) {

		m_pcPath = CStd::StrDup( _pcFile );

		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcFile, szBuffer );

		m_wfiInfo.ui32Freq = m_wfiInfo.ui32Channels = m_wfiInfo.ui32Bits = 0;
		if ( m_fmMap.CreateFileMap( szBuffer ) ) {
			if ( CWaveLoader::GetWaveInfo( m_fmMap, m_wfiInfo ) && m_wfiInfo.ui32Freq ) {
				m_ui64SoundTime = (((m_wfiInfo.ui32WaveLength / m_wfiInfo.ui32Channels) * 1000000ULL) / (m_wfiInfo.ui32Bits >> 3))
					/ m_wfiInfo.ui32Freq;
				// Round up.
				/*if ( m_ui64SoundTime % 10ULL >= 5ULL ) {
					m_ui64SoundTime /= 10ULL;
					m_ui64SoundTime++;
				}
				else {
					m_ui64SoundTime /= 10ULL;
				}*/
			}
		}
	}
	LSE_CALLCTOR CWaveStreamingSoundSourceStreamer::~CWaveStreamingSoundSourceStreamer() {
		CStd::StrDel( m_pcPath );
		m_pcPath = NULL;
	}

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
	LSBOOL LSE_CALL CWaveStreamingSoundSourceStreamer::GetData( uint64_t _ui64From, uint64_t &_ui64Length,
		uint8_t * &_pui8Data, CAllocator * _paAllocator ) const {
		if ( !m_ui64SoundTime || !_paAllocator ) { return false; }

		uint64_t ui64Bytes = (m_wfiInfo.ui32Bits >> 3);
		_ui64From %= m_ui64SoundTime;

		// Convert the time to byte offsets.
		uint64_t ui64Start = _ui64From * ui64Bytes * m_wfiInfo.ui32Channels *
			m_wfiInfo.ui32Freq / 1000000ULL;
		uint64_t ui64End = (_ui64From + _ui64Length) * ui64Bytes * m_wfiInfo.ui32Channels *
			m_wfiInfo.ui32Freq / 1000000ULL;



		// Align the start byte to an actual sample.
		ui64Start = ui64Start / ui64Bytes / m_wfiInfo.ui32Channels;
		ui64Start = ui64Start * ui64Bytes * m_wfiInfo.ui32Channels;

		ui64End = ui64End / ui64Bytes / m_wfiInfo.ui32Channels;
		ui64End = ui64End * ui64Bytes * m_wfiInfo.ui32Channels;

		uint32_t ui32Size = static_cast<uint32_t>(ui64End - ui64Start);
		
		// Release previous data if any.  That means the pointer must either be valid or NULL.
		if ( _pui8Data ) {
			_paAllocator->Free( _pui8Data );
			_pui8Data = NULL;
		}

		// Allocate the request amount.
		_pui8Data = static_cast<uint8_t *>(_paAllocator->Alloc( ui32Size ));
		if ( !_pui8Data ) { return false; }

		// Loop breaks the data into pieces which correctly loop from the end of the sound data back to
		//	the beginning.
		uint32_t ui32Buffer = 0;
		uint32_t ui32Index = 0;
		_ui64Length = ui32Size;
		for ( ; ui32Size; ui32Size -= ui32Buffer ) {
			// Normalize the starting position.
			ui64Start %= m_wfiInfo.ui32WaveLength;

			
			// Clamp to the end of the buffer.
			ui32Buffer = ui32Size;
			if ( ui32Buffer + ui64Start > m_wfiInfo.ui32WaveLength ) {
				ui32Buffer = static_cast<uint32_t>(m_wfiInfo.ui32WaveLength - ui64Start);
			}

			if ( !m_fmMap.ReadBytes( &_pui8Data[ui32Index], ui64Start + m_wfiInfo.uiptrOffsetToWaveData, ui32Buffer ) ) {
				_paAllocator->Free( _pui8Data );
				_pui8Data = NULL;
				return false;
			}

			ui32Index += ui32Buffer;
			ui64Start += ui32Buffer;
		}


		return true;
	}

	/**
	 * Gets the type of this streamer.  Returns LSS_ST_WAVEFILE along with the parent type.
	 *
	 * \return Returns the bitmask representing the type of streamer this instance is.
	 */
	uint32_t LSE_CALL CWaveStreamingSoundSourceStreamer::GetType() const {
		return Parent::GetType() | LSS_ST_WAVEFILE;
	}

	/**
	 * Gets the name of the streamer.  Here it is the file path.
	 *
	 * \return Returns a constant char * to the name of the streamer.
	 */
	const char * LSE_CALL CWaveStreamingSoundSourceStreamer::GetName() const {
		return m_pcPath;
	}

	/**
	 * Gets the frequency of the data.
	 *
	 * \return Returns the frequency of the data provided by this class.
	 */
	uint32_t LSE_CALL CWaveStreamingSoundSourceStreamer::GetFrequency() const {
		return m_wfiInfo.ui32Freq;
	}

	/**
	 * Gets the number of bits per channel of the data.
	 *
	 * \return Returns the number of bits per channel of the data provided by this class.
	 */
	uint32_t LSE_CALL CWaveStreamingSoundSourceStreamer::GetBits() const {
		return m_wfiInfo.ui32Bits;
	}

	/**
	 * Gets the number of channels of the data.
	 *
	 * \return Returns the number of channels of the data provided by this class.
	 */
	uint32_t LSE_CALL CWaveStreamingSoundSourceStreamer::GetChannels() const {
		return m_wfiInfo.ui32Channels;
	}

	/**
	 * Gets the length of the data in microseconds.
	 *
	 * \return Returns the length, in microseconds, of the audio provided by this class.
	 */
	uint64_t LSE_CALL CWaveStreamingSoundSourceStreamer::GetAudioLength() const {
		return m_ui64SoundTime;
	}

}	// namespace lss
