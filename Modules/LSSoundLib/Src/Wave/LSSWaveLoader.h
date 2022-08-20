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


#ifndef __LSS_WAVELOADER_H__
#define __LSS_WAVELOADER_H__

#include "../LSSSoundLib.h"
#include "Allocator/LSTLAllocator.h"
#include "HighLevel/LSFFileMap.h"
#include "HighLevel/LSFFilesEx.h"

namespace lss {

	/**
	 * Class CWaveLoader
	 *
	 * Description: Loads a wave file to RAM using the given allocator.
	 */
	class CWaveLoader {
	public :
		// == Enumerations.
		/**
		 * The compression values.
		 */
		enum LSE_WAVE_EXTENSIBLE {
			LSE_WE_PCM											= 1,
			LSE_WE_EXTENSIBLE									= 0xFFFE,
			LSE_WE_EXTPERIMENTAL								= 0xFFFF,
		};

		// == Types.
		/**
		 * Stores information about a wave file.
		 */
		typedef struct LSE_WAVE_FILE_INFO {
			/**
			 * Offset into the file where wave data is stored.
			 */
			uintptr_t											uiptrOffsetToWaveData;

			/**
			 * Length of the wave data.
			 */
			uint32_t											ui32WaveLength;

			/**
			 * Frequency of the wave data.
			 */
			uint32_t											ui32Freq;

			/**
			 * Bits per sample of the wave data.
			 */
			uint32_t											ui32Bits;

			/**
			 * Number of channels.
			 */
			uint32_t											ui32Channels;

			/**
			 * Compression.
			 */
			LSE_WAVE_EXTENSIBLE									weCompression;

			/**
			 * Channel mask.
			 */
			uint32_t											ui32ChannelMask;
		} * LPLSE_WAVE_FILE_INFO, * const LPCLSE_WAVE_FILE_INFO;


		// == Functions.
		/**
		 * Gets information about a wave file from a loaded file.
		 *
		 * \param _pui8File Loaded file buffer.  The entire file should be loaded for this to succeed.
		 * \param _ui32DataLen Length of the loaded buffer.
		 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
		 * \return Returns false if the buffer is not a valid (or recognized) .wav file.
		 */
		static LSBOOL LSE_CALL									GetWaveInfo( const uint8_t * _pui8File, uint32_t _ui32FileLen,
			LSE_WAVE_FILE_INFO &_wfiInfo );

		/**
		 * Gets information about a wave file from a mapped file.
		 *
		 * \param _fmMap The mapped file.
		 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
		 * \return Returns false if the buffer is not a valid (or recognized) .wav file.
		 */
		static LSBOOL LSE_CALL									GetWaveInfo( const CFileMap &_fmMap,
			LSE_WAVE_FILE_INFO &_wfiInfo );

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
		static LSBOOL LSE_CALL									LoadWavFile( const char * _pcFile,
			uint8_t * &_pui8Data, uint32_t &_ui32Length,
			uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator );

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
		static LSBOOL LSE_CALL									LoadWavFile( const uint8_t * _pui8File, uint32_t _ui32FileLen,
			uint8_t * &_pui8Data, uint32_t &_ui32Length,
			uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator );


	protected :
		// == Enumerations.
		/**
		 * The speaker arrangements found in wave files.
		 * These can be found in the non-portable ksmedia.h.
		 */
		enum LSS_WAVE_SPEAKERS {
			LSS_SPEAKER_FRONT_LEFT								= (1 << 0),
			LSS_SPEAKER_FRONT_RIGHT								= (1 << 1),
			LSS_SPEAKER_FRONT_CENTER							= (1 << 2),
			LSS_SPEAKER_LOW_FREQUENCY							= (1 << 3),
			LSS_SPEAKER_BACK_LEFT								= (1 << 4),
			LSS_SPEAKER_BACK_RIGHT								= (1 << 5),
			LSS_SPEAKER_FRONT_LEFT_OF_CENTER					= (1 << 6),
			LSS_SPEAKER_FRONT_RIGHT_OF_CENTER					= (1 << 7),
			LSS_SPEAKER_BACK_CENTER								= (1 << 8),
			LSS_SPEAKER_SIDE_LEFT								= (1 << 9),
			LSS_SPEAKER_SIDE_RIGHT								= (1 << 10),
			LSS_SPEAKER_TOP_CENTER								= (1 << 11),
			LSS_SPEAKER_TOP_FRONT_LEFT							= (1 << 12),
			LSS_SPEAKER_TOP_FRONT_CENTER						= (1 << 13),
			LSS_SPEAKER_TOP_FRONT_RIGHT							= (1 << 14),
			LSS_SPEAKER_TOP_BACK_LEFT							= (1 << 15),
			LSS_SPEAKER_TOP_BACK_CENTER							= (1 << 16),
			LSS_SPEAKER_TOP_BACK_RIGHT							= (1 << 17),
			LSS_SPEAKER_ANY										= 0x80000000,
		};


		// == Types.

#pragma pack( push, 1 )
		/**
		 * The wave-file header.
		 */
		typedef struct LSE_WAVE_FILE_HEADER {
			/**
			 * RIFF header.
			 */
			uint8_t												ui8Header[4];

			/**
			 * Size of the chunk.
			 */
			uint32_t											ui32Size;
			
			/**
			 * WAVE.
			 */
			uint8_t												ui8Wave[4];
		} * LPLSE_WAVE_FILE_HEADER, * const LPCLSE_WAVE_FILE_HEADER;

		/**
		 * A chunk header.
		 */
		typedef struct LSE_WAVE_FILE_CHUNK_HEADER {
			/**
			 * Header name.
			 */
			uint8_t												ui8Name[4];
			
			/**
			 * Size.
			 */
			uint32_t											ui32Size;
			
			/**
			 * Data.
			 */
			uint8_t												ui8Data[1];
		} * LPLSE_WAVE_FILE_CHUNK_HEADER, * const LPCLSE_WAVE_FILE_CHUNK_HEADER;

		/**
		 * The "fmt " data part.
		 */
		typedef struct LSE_WAVE_FILE_FMT_DATA {
			/**
			 * Compression.
			 */
			uint16_t											ui16Compression;
			
			/**
			 * Number of channels.
			 */
			uint16_t											ui16Channels;
			
			/**
			 * Frequency.
			 */
			uint32_t											ui32Frequency;
			
			/**
			 * Average bytes per second.
			 */
			uint32_t											ui32BytesPerSecond;
			
			/**
			 * Block align.
			 */
			uint16_t											ui16Align;
			
			/**
			 * Bits per sample.
			 */
			uint16_t											ui16BitsPerSample;

			/**
			 * For extensible waves.
			 */
			union {
				uint16_t										ui16ValidBitsPerSample;
				uint16_t										ui16SamplesPerBlock;
				uint16_t										ui16Reserved;
			}													uSamples;
			uint16_t											ui16Pad;
			
			/**
			 * Speaker mask.
			 */
			uint32_t											ui32ChannelMask;
		} * LPLSE_WAVE_FILE_FMT_DATA, * const LPCLSE_WAVE_FILE_FMT_DATA;
#pragma pack( pop )


		// == Functions.
		/**
		 * Find a chunk header in a valid WAVE file.
		 *
		 * \param _pcName Name of the chunk to find.
		 * \param _pui8Data Buffered file data.
		 * \param _ui32Len Size of the buffered file data.
		 * \return Returns NULL if not found.  Otherwise returns a pointer to the start
		 *	of the chunk within the file data.
		 */
		static const LPLSE_WAVE_FILE_CHUNK_HEADER LSE_CALL		FindChunk( const char * _pcName, const uint8_t * _pui8Data, uint32_t _ui32Len );

		/**
		 * Find a chunk header in a valid WAVE file.
		 *
		 * \param _pcName Name of the chunk to find.
		 * \param _fmMap Buffered file data.
		 * \return Returns 0ULL if not found.  Otherwise returns an offset to the start
		 *	of the chunk within the file data.
		 */
		static const uint64_t LSE_CALL							FindChunk( const char * _pcName, const CFileMap &_fmMap );

		/**
		 * Does the format of the WAVE have an exact (and supported) sound-library format?
		 * Returns CSoundLib::LSS_C_TOTAL if not.
		 *
		 * \param _ui32Channels Number of channels.
		 * \param _ui32Bits Number of bits.
		 * \return Returns a matching CSoundLib::LSS_CHANNELS for the given channel and bit combination.
		 */
		static CSoundLib::LSS_CHANNELS LSE_CALL					FindSoundLibraryFormat( uint32_t _ui32Channels, uint32_t _ui32Bits );

		/**
		 * Gets information about a wave file from key parts of the file header.
		 *
		 * \param _wfhHeader Structure containing the data in the header of the wave file.
		 * \param _wffdFmt The format data of the format chunk.
		 * \param _wfchData The data chunk of the file data.
		 * \param _ui64FileLen The length of the file.
		 * \param _wfiInfo Reference to a LSE_WAVE_FILE_INFO structure containing the returned data.
		 * \return Returns false if the data is not a valid (or recognized) .wav file.
		 */
		static LSBOOL LSE_CALL									GetWaveInfo( const LSE_WAVE_FILE_HEADER &_wfhHeader,
			const LSE_WAVE_FILE_FMT_DATA &_wffdFmt,
			const LSE_WAVE_FILE_CHUNK_HEADER &_wfchData,
			uint64_t _ui64FileLen,
			LSE_WAVE_FILE_INFO &_wfiInfo );
	};

}	// namespace lss

#endif	// __LSS_WAVELOADER_H__
