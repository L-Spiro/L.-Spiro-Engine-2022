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
 * Description: Loads a wave file to RAM using the given allocator.  Returns information regarding the loaded
 *	file.
 */

#include "LSSWaveLoader.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {

	// == Functions.
	/**
	 * Gets information about a wave file from a loaded file.
	 *
	 * \param _pui8File Loaded file buffer.  The entire file should be loaded for this to succeed.
	 * \param _ui32DataLen Length of the loaded buffer.
	 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
	 * \return Returns false if the buffer is not a valid (or recognized) .wav file.
	 */
	LSBOOL LSE_CALL CWaveLoader::GetWaveInfo( const uint8_t * _pui8File, uint32_t _ui32FileLen,
		LSE_WAVE_FILE_INFO &_wfiInfo ) {

		if ( _ui32FileLen < sizeof( LSE_WAVE_FILE_HEADER ) ) { return false; }
		
		const LSE_WAVE_FILE_HEADER * pwfhHeader = reinterpret_cast<const LSE_WAVE_FILE_HEADER *>(_pui8File);

		if ( CStd::StrNICmp( reinterpret_cast<const char *>(pwfhHeader->ui8Header), "RIFF", 4 ) != 0 ) { return false; }
		if ( pwfhHeader->ui32Size != _ui32FileLen - 8 ) { return false; }


		// Get the "fmt " chunk.
		const LPLSE_WAVE_FILE_CHUNK_HEADER lpwfchFmt = FindChunk( "fmt ",
			_pui8File, _ui32FileLen );
		if ( !lpwfchFmt ) { return false; }
		if ( lpwfchFmt->ui32Size < 16 ) { return false; }

		// Get the "data" chunk.
		const LPLSE_WAVE_FILE_CHUNK_HEADER lpwfchData = FindChunk( "data",
			_pui8File, _ui32FileLen );
		if ( !lpwfchData ) { return false; }

		// Verify that it is long enough to interpret as a LSE_WAVE_FILE_FMT_DATA structure.
		uintptr_t uiptrOffset = reinterpret_cast<uint8_t *>(lpwfchFmt) - _pui8File;
		if ( uiptrOffset + lpwfchFmt->ui32Size + 8 > _ui32FileLen ) { return false; }

		
		const LPLSE_WAVE_FILE_FMT_DATA lpwffdFmtData = reinterpret_cast<const LPLSE_WAVE_FILE_FMT_DATA>(&lpwfchFmt->ui8Data[0]);
		if ( lpwffdFmtData->ui16Compression == LSE_WE_EXTENSIBLE ) {
			if ( lpwfchFmt->ui32Size < sizeof( LSE_WAVE_FILE_FMT_DATA ) ) { return false; }
		}


		if ( !GetWaveInfo( (*pwfhHeader),
			(*lpwffdFmtData),
			(*lpwfchData),
			_ui32FileLen,
			_wfiInfo ) ) { return false; }
		_wfiInfo.uiptrOffsetToWaveData += reinterpret_cast<const uint8_t *>(lpwfchData) - _pui8File;


		return true;
	}

	/**
	 * Gets information about a wave file from a mapped file.
	 *
	 * \param _fmMap The mapped file.
	 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
	 * \return Returns false if the buffer is not a valid (or recognized) .wav file.
	 */
	LSBOOL LSE_CALL CWaveLoader::GetWaveInfo( const CFileMap &_fmMap,
		LSE_WAVE_FILE_INFO &_wfiInfo ) {

		// Start with the header.
		if ( _fmMap.GetFileSize() < sizeof( LSE_WAVE_FILE_HEADER ) ) { return false; }

		LSE_WAVE_FILE_HEADER wfhHeader;
		if ( _fmMap.ReadBytes( &wfhHeader, 0ULL, sizeof( LSE_WAVE_FILE_HEADER ) ) != sizeof( LSE_WAVE_FILE_HEADER ) ) {
			return false;
		}

		if ( CStd::StrNICmp( reinterpret_cast<const char *>(wfhHeader.ui8Header), "RIFF", 4 ) != 0 ) { return false; }
		if ( wfhHeader.ui32Size != _fmMap.GetFileSize() - 8ULL ) { return false; }

		// Get the "fmt " chunk.
		uint64_t ui64Fmt = FindChunk( "fmt ",
			_fmMap );
		if ( !ui64Fmt ) { return false; }
		LSE_WAVE_FILE_CHUNK_HEADER wfchFmt;
		if ( _fmMap.ReadBytes( &wfchFmt, ui64Fmt, sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) != sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) {
			return false;
		}
		if ( wfchFmt.ui32Size < 16 ) { return false; }

		// Get the "data" chunk.
		uint64_t ui64Data = FindChunk( "data",
			_fmMap );
		if ( !ui64Data ) { return false; }
		LSE_WAVE_FILE_CHUNK_HEADER wfchData;
		if ( _fmMap.ReadBytes( &wfchData, ui64Data, sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) != sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) {
			return false;
		}

		// Read the format data.
		// No need to verify the size beforehand as it will be verified for us as we read the bytes.
		uint64_t ui64Offset = &wfchFmt.ui8Data[0] - reinterpret_cast<uint8_t *>(&wfchFmt);
		LSE_WAVE_FILE_FMT_DATA wffdFmtData;
		if ( _fmMap.ReadBytes( &wffdFmtData, ui64Fmt + ui64Offset, sizeof( LSE_WAVE_FILE_FMT_DATA ) ) != sizeof( LSE_WAVE_FILE_FMT_DATA ) ) {
			return false;
		}
		if ( wffdFmtData.ui16Compression == LSE_WE_EXTENSIBLE ) {
			if ( wfchFmt.ui32Size < sizeof( LSE_WAVE_FILE_FMT_DATA ) ) { return false; }
		}


		if ( !GetWaveInfo( wfhHeader,
			wffdFmtData,
			wfchData,
			_fmMap.GetFileSize(),
			_wfiInfo ) ) { return false; }
		_wfiInfo.uiptrOffsetToWaveData = static_cast<uintptr_t>(_wfiInfo.uiptrOffsetToWaveData + ui64Data);

		return true;
	}

	/**
	 * Load a WAVE file.  Returns the PCM sound data, data length,
	 *	frequency, bit depth, and channels.  PCM data is allocated via
	 *	the given allocator.
	 *
	 * \param _pcFile The file to load.
	 * \param _pui8Data Pointer filled with the file data allocated with _paAllocator.
	 * \param _ui32Length Length of the wave data in bytes.
	 * \param _ui32Freq Frequency of the wave data.
	 * \param _ui32Bits Bits per sample of the wave data.
	 * \param _ui32Channels Channels in the wave data.
	 * \param _paAllocator Allocator used to allocate _pui8Data, or NULL to use LSENEW.
	 * \return Returns false if the load fails or the file is not recognized as
	 *	a valid WAVE file.
	 */
	LSBOOL LSE_CALL CWaveLoader::LoadWavFile( const char * _pcFile,
		uint8_t * &_pui8Data, uint32_t &_ui32Length,
		uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator ) {

		// Load the file to main RAM.
		uint8_t * pui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, pui8Buffer, &uiptrSize ) ) {
			return false;
		}

		LSBOOL bRet = LoadWavFile( pui8Buffer, static_cast<uint32_t>(uiptrSize),
			_pui8Data, _ui32Length,
			_ui32Freq, _ui32Bits, _ui32Channels, _paAllocator );

		CFilesEx::FreeFile( pui8Buffer );
		return bRet;
	}

	/**
	 * Load a WAVE file that has already been loaded to RAM.
	 *
	 * \param _pui8File The file data.
	 * \param _ui32FileLen Length of the file data.
	 * \param _pui8Data Pointer filled with the file data allocated with _paAllocator.
	 * \param _ui32Length Length of the wave data in bytes.
	 * \param _ui32Freq Frequency of the wave data.
	 * \param _ui32Bits Bits per sample of the wave data.
	 * \param _ui32Channels Channels in the wave data.
	 * \param _paAllocator Allocator used to allocate _pui8Data, or NULL to use LSENEW.
	 * \return Returns false if the load fails or the file is not recognized as
	 *	a valid WAVE file.
	 */
	LSBOOL LSE_CALL CWaveLoader::LoadWavFile( const uint8_t * _pui8File, uint32_t _ui32FileLen,
		uint8_t * &_pui8Data, uint32_t &_ui32Length,
		uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator ) {
		LSE_WAVE_FILE_INFO wfiInfo;
		if ( !GetWaveInfo( _pui8File, _ui32FileLen,
			wfiInfo ) ) { return false; }


		// Find the channel arrangement that matches the channels and bit-depth we have here.
		CSoundLib::LSS_CHANNELS cChannel = FindSoundLibraryFormat( wfiInfo.ui32Channels, wfiInfo.ui32Bits );
		if ( cChannel == CSoundLib::LSS_C_TOTAL ) { return false; }


		// Ensure that our speaker arrangement matches the found type.
		if ( wfiInfo.weCompression == LSE_WE_EXTENSIBLE ) {
			// Match our arrangements against the CSoundLib::LSS_CHANNELS enumeration.
			static const uint32_t ui32Arrangements[] = {
				static_cast<uint32_t>(LSS_SPEAKER_ANY),							//		LSS_C_MONO8,
				static_cast<uint32_t>(LSS_SPEAKER_ANY),							//		LSS_C_MONO16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT,				//		LSS_C_MONO_FLOAT32,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT,				//		LSS_C_STEREO8,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT,				//		LSS_C_STEREO16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT,				//		LSS_C_STEREO_FLOAT32,
				static_cast<uint32_t>(LSS_SPEAKER_ANY),							//		LSS_C_MONO_IMA4,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT,				//		LSS_C_STEREO_IMA4,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,				//		LSS_C_QUAD8_LOKI,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,				//		LSS_C_QUAD16_LOKI,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,				//		LSS_C_QUAD8,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,				//		LSS_C_QUAD16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,				//		LSS_C_QUAD32,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY,		//		LSS_C_51CHN8,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY,		//		LSS_C_51CHN16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY,		//		LSS_C_51CHN32,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_BACK_CENTER,									//		LSS_C_61CHN8,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_BACK_CENTER,									//		LSS_C_61CHN16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_BACK_CENTER,									//		LSS_C_61CHN32,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_SIDE_LEFT | LSS_SPEAKER_SIDE_RIGHT,				//		LSS_C_71CHN8,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_SIDE_LEFT | LSS_SPEAKER_SIDE_RIGHT,				//		LSS_C_71CHN16,
				LSS_SPEAKER_FRONT_LEFT | LSS_SPEAKER_FRONT_RIGHT |
					LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT |
					LSS_SPEAKER_FRONT_CENTER | LSS_SPEAKER_LOW_FREQUENCY |
					LSS_SPEAKER_SIDE_LEFT | LSS_SPEAKER_SIDE_RIGHT,				//		LSS_C_71CHN32,
				LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,					//		LSS_C_REAR8,
				LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,					//		LSS_C_REAR16,
				LSS_SPEAKER_BACK_LEFT | LSS_SPEAKER_BACK_RIGHT,					//		LSS_C_REAR32,
			};

			if ( ui32Arrangements[cChannel] != LSS_SPEAKER_ANY ) {
				if ( wfiInfo.ui32ChannelMask != ui32Arrangements[cChannel] ) { return false; }
			}
		}

		_ui32Freq = wfiInfo.ui32Freq;
		_ui32Bits = wfiInfo.ui32Bits;
		_ui32Channels = wfiInfo.ui32Channels;
		_ui32Length = wfiInfo.ui32WaveLength;


		_pui8Data = _paAllocator ?
			reinterpret_cast<uint8_t *>(_paAllocator->Alloc( _ui32Length )) :
			LSENEW uint8_t[_ui32Length];
		if ( !_pui8Data ) { return false; }
		CStd::MemCpy( _pui8Data, &_pui8File[wfiInfo.uiptrOffsetToWaveData], _ui32Length );

		return true;
	}

	/**
	 * Find a chunk header in a valid WAVE file.
	 *
	 * \param _pcName Name of the chunk to find.
	 * \param _pui8Data Buffered file data.
	 * \param _ui32Len Size of the buffered file data.
	 * \return Returns NULL if not found.  Otherwise returns a pointer to the start
	 *	of the chunk within the file data.
	 */
	const CWaveLoader::LPLSE_WAVE_FILE_CHUNK_HEADER LSE_CALL CWaveLoader::FindChunk( const char * _pcName, const uint8_t * _pui8Data, uint32_t _ui32Len ) {
		uint32_t ui32Offset = sizeof( LSE_WAVE_FILE_HEADER );
		if ( ui32Offset >= _ui32Len ) { return NULL; }

		// So retarded.
		uint8_t * pui8Data = const_cast<uint8_t *>(_pui8Data);

		while ( true ) {
			// The remaining size must be larger than the chunk header.
			if ( _ui32Len <= sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) + ui32Offset ) { return NULL; }
			// This is a chunk and we have enough room to read from it.
			LPLSE_WAVE_FILE_CHUNK_HEADER lpwfchHeader = reinterpret_cast<LPLSE_WAVE_FILE_CHUNK_HEADER>(&pui8Data[ui32Offset]);
			if ( CStd::StrNICmp( reinterpret_cast<const char *>(lpwfchHeader->ui8Name), _pcName, 4 ) == 0 ) { return lpwfchHeader; }

			// Increase by an even amount.
			ui32Offset += (lpwfchHeader->ui32Size + (lpwfchHeader->ui32Size & 0x01)) + 8;
		}
	}

	/**
	 * Find a chunk header in a valid WAVE file.
	 *
	 * \param _pcName Name of the chunk to find.
	 * \param _fmMap Buffered file data.
	 * \return Returns NULL if not found.  Otherwise returns an offset to the start
	 *	of the chunk within the file data.
	 */
	const uint64_t LSE_CALL CWaveLoader::FindChunk( const char * _pcName, const CFileMap &_fmMap ) {
		uint64_t ui64Offset = sizeof( LSE_WAVE_FILE_HEADER );
		if ( ui64Offset >= _fmMap.GetFileSize() ) { return 0ULL; }

		while ( true ) {
			// The remaining size must be larger than the chunk header.
			if ( _fmMap.GetFileSize() <= sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) + ui64Offset ) { return 0ULL; }
			// This is a chunk and we have enough room to read from it.
			LSE_WAVE_FILE_CHUNK_HEADER wfchHeader;
			if ( _fmMap.ReadBytes( &wfchHeader, ui64Offset, sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) != sizeof( LSE_WAVE_FILE_CHUNK_HEADER ) ) {
				return 0ULL;
			}
			if ( CStd::StrNICmp( reinterpret_cast<const char *>(wfchHeader.ui8Name), _pcName, 4 ) == 0 ) {
				return ui64Offset;
			}

			// Increase by an even amount.
			ui64Offset += (wfchHeader.ui32Size + (wfchHeader.ui32Size & 0x01)) + 8;
		}
	}

	/**
	 * Does the format of the WAVE have an exact (and supported) sound-library format?
	 * Returns CSoundLib::LSS_C_TOTAL if not.
	 *
	 * \param _ui32Channels Number of channels.
	 * \param _ui32Bits Number of bits.
	 * \return Returns a matching CSoundLib::LSS_CHANNELS for the given channel and bit combination.
	 */
	CSoundLib::LSS_CHANNELS LSE_CALL CWaveLoader::FindSoundLibraryFormat( uint32_t _ui32Channels, uint32_t _ui32Bits ) {
		for ( uint32_t I = 0; I < CSoundLib::LSS_C_TOTAL; ++I ) {
			if ( CSoundLib::ChannelArrangementSupported( static_cast<CSoundLib::LSS_CHANNELS>(I) ) ) {
				if ( CSoundLib::GetChannels( static_cast<CSoundLib::LSS_CHANNELS>(I) ) == _ui32Channels &&
					CSoundLib::GetBitsPerChannel( static_cast<CSoundLib::LSS_CHANNELS>(I) ) == _ui32Bits ) { return static_cast<CSoundLib::LSS_CHANNELS>(I); }
			}
		}
		return CSoundLib::LSS_C_TOTAL;
	}

	/**
	 * Gets information about a wave file from key parts of the file header.
	 *
	 * \param _wfhHeader Structure containing the data in the header of the wave file.
	 * \param _wffdFmt The format data of the format chunk.
	 * \param _wfchData The data chunk of the file data.
	 * \param _ui32FileLen The length of the file.
	 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
	 * \return Returns false if the data is not a valid (or recognized) .wav file.
	 */
	LSBOOL LSE_CALL CWaveLoader::GetWaveInfo( const LSE_WAVE_FILE_HEADER &_wfhHeader,
		const LSE_WAVE_FILE_FMT_DATA &_wffdFmt,
		const LSE_WAVE_FILE_CHUNK_HEADER &_wfchData,
		uint64_t _ui64FileLen,
		LSE_WAVE_FILE_INFO &_wfiInfo ) {

		if ( CStd::StrNICmp( reinterpret_cast<const char *>(_wfhHeader.ui8Header), "RIFF", 4 ) != 0 ) { return false; }
		if ( _wfhHeader.ui32Size != _ui64FileLen - 8ULL ) { return false; }


		// Check that we support the given format.
		switch ( _wffdFmt.ui16Compression ) {
			case LSE_WE_EXTENSIBLE : {}
			case LSE_WE_PCM : { break; }
			default : { return false; }
		}


		uint32_t ui32Size = (_wffdFmt.ui16Channels * _wffdFmt.ui16BitsPerSample) >> 3;
		_wfiInfo.ui32Freq = _wffdFmt.ui32Frequency;
		_wfiInfo.ui32Bits = _wffdFmt.ui16BitsPerSample;
		_wfiInfo.ui32Channels = _wffdFmt.ui16Channels;
		_wfiInfo.weCompression = static_cast<LSE_WAVE_EXTENSIBLE>(_wffdFmt.ui16Compression);
		_wfiInfo.ui32ChannelMask = _wffdFmt.ui32ChannelMask;

		uint32_t ui32TotalSamples = _wfchData.ui32Size / ui32Size;
		_wfiInfo.ui32WaveLength = ui32TotalSamples * ui32Size;

		if ( _wfchData.ui32Size < _wfiInfo.ui32WaveLength ) { return false; }

		_wfiInfo.uiptrOffsetToWaveData = &_wfchData.ui8Data[0] - reinterpret_cast<const uint8_t *>(&_wfchData);

		return true;
	}

}	// namespace lss

#pragma warning( pop )
