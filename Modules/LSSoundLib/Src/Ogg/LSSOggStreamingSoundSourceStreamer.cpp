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
 *	from an Ogg file into a CStreamingSoundSource object.
 */

#include "LSSOggStreamingSoundSourceStreamer.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR COggStreamingSoundSourceStreamer::COggStreamingSoundSourceStreamer( const char * _pcFile,
		CAllocator * _paAllocator ) :
		m_pcPath( NULL ),
		m_paAllocator( _paAllocator ),
		m_ui64SoundTime( 0ULL ),
		m_ui64Pcms( 0ULL ),
		m_pviInfo( NULL ) {

		m_pcPath = CStd::StrDup( _pcFile );
		if ( CVorbisFile::OvFOpen( _pcFile, &m_ovfFile ) == 0 ) {
			m_pviInfo = CVorbisFile::OvInfo( &m_ovfFile, -1 );
			m_ui64Pcms = CVorbisFile::GetPcmCount( m_ovfFile );
			m_ui64SoundTime = (m_ui64Pcms * 1000000ULL)
				/ m_pviInfo->rate;
		}
	}
	LSE_CALLCTOR COggStreamingSoundSourceStreamer::~COggStreamingSoundSourceStreamer() {
		CVorbisFile::OvClear( &m_ovfFile );
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
	LSBOOL LSE_CALL COggStreamingSoundSourceStreamer::GetData( uint64_t _ui64From, uint64_t &_ui64Length,
		uint8_t * &_pui8Data, CAllocator * _paAllocator ) const {
		if ( !m_ui64SoundTime || !_paAllocator || !m_pviInfo ) { return false; }

		_ui64From %= m_ui64SoundTime;

		// Convert the time to byte offsets.
		uint64_t ui64Start = _ui64From * 2 * m_pviInfo->channels *
			m_pviInfo->rate / 1000000ULL;
		uint64_t ui64End = (_ui64From + _ui64Length) * 2 * m_pviInfo->channels *
			m_pviInfo->rate / 1000000ULL;



		// Align the start byte to an actual sample.
		ui64Start = ui64Start / 2 / m_pviInfo->channels;
		ui64Start = ui64Start * 2 * m_pviInfo->channels;

		ui64End = ui64End / 2 / m_pviInfo->channels;
		ui64End = ui64End * 2 * m_pviInfo->channels;

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
		int32_t i32BitStream = -1;
		uint64_t ui64Length = (m_ui64Pcms * m_pviInfo->channels) << 1ULL;
		for ( ; ui32Size; ui32Size -= ui32Buffer ) {
			// Normalize the starting position.
			ui64Start %= ui64Length;

			
			// Clamp to the end of the buffer.
			ui32Buffer = ui32Size;
			if ( ui32Buffer + ui64Start > ui64Length ) {
				ui32Buffer = static_cast<uint32_t>(ui64Length - ui64Start);
			}

			// Convert the position to a PCM.
			CVorbisFile::OvPcmSeek( const_cast<CVorbisFile::LSS_OV_FILE *>(&m_ovfFile), (ui64Start / m_pviInfo->channels) >> 1 );
			uint32_t ui32Total = 0;
			do {
				int32_t i32Ret = CVorbisFile::OvRead( const_cast<CVorbisFile::LSS_OV_FILE *>(&m_ovfFile),
					reinterpret_cast<char *>(&_pui8Data[ui32Index]), ui32Buffer - ui32Total,
					false, sizeof( uint16_t ), true, &i32BitStream );
				if ( !i32Ret ) { break; }
				ui32Total += i32Ret;
				ui32Index += i32Ret;
			} while ( ui32Total < ui32Buffer );
			if ( ui32Total != ui32Buffer ) {
				_paAllocator->Free( _pui8Data );
				_pui8Data = NULL;
				return false;
			}
			ui64Start += ui32Buffer;
		}


		return true;
	}

	/**
	 * Gets the type of this streamer.  Returns LSS_ST_OGGFILE along with the parent type.
	 *
	 * \return Returns the bitmask representing the type of streamer this instance is.
	 */
	uint32_t LSE_CALL COggStreamingSoundSourceStreamer::GetType() const {
		return Parent::GetType() | LSS_ST_OGGFILE;
	}

	/**
	 * Gets the name of the streamer.  Here it is the file path.
	 *
	 * \return Returns a constant char * to the name of the streamer.
	 */
	const char * LSE_CALL COggStreamingSoundSourceStreamer::GetName() const {
		return m_pcPath;
	}

	/**
	 * Gets the frequency of the data.
	 *
	 * \return Returns the frequency of the data provided by this class.
	 */
	uint32_t LSE_CALL COggStreamingSoundSourceStreamer::GetFrequency() const {
		return m_pviInfo ? m_pviInfo->rate : 0;
	}

	/**
	 * Gets the number of bits per channel of the data.
	 *
	 * \return Returns the number of bits per channel of the data provided by this class.
	 */
	uint32_t LSE_CALL COggStreamingSoundSourceStreamer::GetBits() const {
		return 16;
	}

	/**
	 * Gets the number of channels of the data.
	 *
	 * \return Returns the number of channels of the data provided by this class.
	 */
	uint32_t LSE_CALL COggStreamingSoundSourceStreamer::GetChannels() const {
		return m_pviInfo ? m_pviInfo->channels : 0;
	}

	/**
	 * Gets the length of the data in microseconds.
	 *
	 * \return Returns the length, in microseconds, of the audio provided by this class.
	 */
	uint64_t LSE_CALL COggStreamingSoundSourceStreamer::GetAudioLength() const {
		return m_ui64SoundTime;
	}

}	// namespace lss
