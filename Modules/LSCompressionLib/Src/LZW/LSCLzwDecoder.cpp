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
 * Description: Decodes a bitstream previously encoded with CLzwEncoder::Encode().
 */

#include "LSCLzwDecoder.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CLzwDecoder::CLzwDecoder() {
	}

	// == Functions.
	/**
	 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
	 *
	 * \param _psaAllocator The allocator to use by this class.
	 */
	void LSE_CALL CLzwDecoder::SetAllocator( CStdAllocator * _psaAllocator ) {
		m_lddDict.SetAllocator( _psaAllocator );
	}

	/**
	 * Decode raw bytes of LZW compressed data.
	 *
	 * \param _bsStream Bitstream containing data compressed with the LZW algorithm.
	 * \param _ui32DictionaryStartingSize Starting size of the dictionary.
	 * \param _ui32MaxBits Maximum bits possible in the dictionary.
	 * \param _sStream The stream to hold the decoded bytes.
	 * \param _bResetDictionary If true, the dictionary is reset for the decoding of this
	 *	chunk of bytes.  This is optional so as to allow breaking up the decoding into multiple
	 *	chunks.
	 * \return Returns the number of bits needed to read the next code from the input buffer.
	 *	This can be used in a subsequent call to Decode() to continue decoding data over several
	 *	chunks.  If 0 is returned, an error occurred.
	 */
	uint32_t LSE_CALL CLzwDecoder::Decode( const CBitStream &_bsStream,
		uint32_t _ui32DictionaryStartingSize, uint32_t _ui32MaxBits,
		lsstd::CStream &_sStream, LSBOOL _bResetDictionary ) {

		// The dictionary cannot be fewer than 2 characters.
		if ( _ui32DictionaryStartingSize < 2 ) {
			_ui32DictionaryStartingSize = 2;
		}
		if ( _bResetDictionary ) {
			m_lddDict.Reset();
		}

		if ( m_lddDict.GetTotal() == 0 ) {
			m_lddDict.InitializeDictionary( _ui32DictionaryStartingSize, _ui32MaxBits );
		}

		uint32_t ui32Old = LSE_MAXU32;
		uint32_t ui32Char = ui32Old;

		uint32_t ui32BitsToRead = 0;
		CString sTemp;
		uint64_t ui64BitsRead = 0ULL;
		while ( true ) {
			ui32BitsToRead = m_lddDict.GetBitsNeeded();
			if ( ui64BitsRead + ui32BitsToRead > (_bsStream.GetLengthInBytes() << 3) ) {
				// Done with the stream.
				break;
			}
			uint32_t ui32Value = 0;

			// Get the next code.
			if ( !_bsStream.ReadBits( reinterpret_cast<uint8_t *>(&ui32Value), ui32BitsToRead ) ) {
				return 0;
			}
			ui64BitsRead += ui32BitsToRead;

			if ( m_lddDict.IsEndOfInputCode( static_cast<CLzwDecodingDict::LSN_LZW_CODE>(ui32Value) ) ) {
				break;
			}
			else if ( m_lddDict.IsClearCode( static_cast<CLzwDecodingDict::LSN_LZW_CODE>(ui32Value) ) ) {
				m_lddDict.InitializeDictionary( _ui32DictionaryStartingSize, _ui32MaxBits );
				ui32Old = LSE_MAXU32;
				continue;
			}

			if ( ui32Value >= m_lddDict.GetTotal() ) {
				if ( ui32Old == LSE_MAXU32 ) {
					// Unable to handle this case.  The old code is invalid.
					return 0;
				}
				if ( ui32Value != m_lddDict.GetTotal() ) {
					// The unknown code must be the next code we will add.
					return 0;
				}
				// Code not found in the dictionary.
				if ( !m_lddDict.GetString( static_cast<CLzwDecodingDict::LSN_LZW_CODE>(ui32Old), sTemp ) ) {
					return 0;
				}

				// Output the string followed by the current character.
				for ( uint32_t I = sTemp.Length(); I--; ) {
					//pui8DecompressBuffer[ui32DecompressIndex++] = ui8DecompressedStrings[I];
					if ( !_sStream.WriteUInt8( static_cast<uint8_t>(sTemp.CStr()[I]) ) ) {
						return 0;
					}
				}
				//pui8DecompressBuffer[ui32DecompressIndex++] = static_cast<uint8_t>(ui32Char);
				if ( !_sStream.WriteUInt8( static_cast<uint8_t>(ui32Char) ) ) {
					return 0;
				}
			}
			else {
				// It was in the table.
				// Output its string normally.
				if ( !m_lddDict.GetString( static_cast<CLzwDecodingDict::LSN_LZW_CODE>(ui32Value), sTemp ) ) {
					return 0;
				}

				// Output the string.
				for ( uint32_t I = sTemp.Length(); I--; ) {
					//pui8DecompressBuffer[ui32DecompressIndex++] = ui8DecompressedStrings[I];
					if ( !_sStream.WriteUInt8( static_cast<uint8_t>(sTemp.CStr()[I]) ) ) {
						return 0;
					}
				}
			}

			ui32Char = static_cast<uint32_t>(sTemp.CStr()[sTemp.Length()-1]);
			if ( ui32Old != LSE_MAXU32 ) {
				if ( m_lddDict.GetTotal() == 0 ) {
					m_lddDict.InitializeDictionary( _ui32DictionaryStartingSize, _ui32MaxBits );
				}
				if ( !m_lddDict.AddCode( static_cast<CLzwDecodingDict::LSN_LZW_CODE>(ui32Old), static_cast<uint8_t>(ui32Char) ) ) {
					return 0;
				}
			}
			ui32Old = ui32Value;
		}

		return ui32BitsToRead;
	}
}	// namespace lsc

#pragma warning( pop )
