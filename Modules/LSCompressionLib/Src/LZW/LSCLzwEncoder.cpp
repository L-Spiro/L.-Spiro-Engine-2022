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
 * Description: Encodes a stream of bytes into an LZW compressed stream of bits.
 */

#include "LSCLzwEncoder.h"


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CLzwEncoder::CLzwEncoder() {
	}

	// == Functions.
	/**
	 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
	 *
	 * \param _psaAllocator The allocator to use by this class.
	 */
	void LSE_CALL CLzwEncoder::SetAllocator( CStdAllocator * _psaAllocator ) {
		m_ledDict.SetAllocator( _psaAllocator );
	}

	/**
	 * Encode an array of bytes.  The encoded data is output at the current position in the
	 *	given bitstream.  This allows multiple encodings to be packed together into the same
	 *	stream.
	 *
	 * \param _pui8Bytes Pointer to the bytes to encode.
	 * \param _ui64Size Size of the buffer to which _pui8Bytes points.
	 * \param _ui32DictionaryStartingSize Starting size of the dictionary.
	 * \param _ui32MaxBits Maximum bits possible in the dictionary.
	 * \param _bsStream Stream to which the compressed bits are to be written.
	 * \param _bResetDictionary If true, the dictionary will be reset and the first code output to the
	 *	bitstream will be a dictionary-reset code of _ui32PrevBits bits.
	 * \param _ui32PrevBits If bits were already compressed to _bsStream, this informs this function of the
	 *	size of the next command, in bits, that would be read from the stream if decompressing.  This is
	 *	required if _bsStream is not empty.
	 * \param _ui32MaxSize Size at which to stop compression.  If the size of the compressed data exceeds this
	 *	amount then compression is ended.
	 * \return Returns the size of the next command that would be written to _bsStream if compression were
	 *	to continue.  This can be used in a subsequent call to Encode().  On error, 0 is returned.
	 */
	uint32_t LSE_CALL CLzwEncoder::Encode( const uint8_t * _pui8Bytes, uint64_t _ui64Size,
		uint32_t _ui32DictionaryStartingSize, uint32_t _ui32MaxBits,
		CBitStream &_bsStream, LSBOOL _bResetDictionary, uint32_t _ui32PrevBits, uint32_t _ui32MaxSize ) {
		// If the stream has data in it, we are continuing a compression.  In this case
		//	_ui32PrevBits must not be 0.
		if ( _bsStream.GetLengthInBytes() ) {
			if ( _ui32PrevBits == 0 ) { return 0; }
		}
		uint32_t ui32PrevLen = _bsStream.GetLengthInBytes();

		// The dictionary cannot be fewer than 2 characters.
		if ( _ui32DictionaryStartingSize < 2 ) {
			_ui32DictionaryStartingSize = 2;
		}
		if ( _bResetDictionary ) {
			m_ledDict.Reset();
		}

		if ( m_ledDict.TotalEntries() == 0 ) {
			m_ledDict.InitDict( _ui32DictionaryStartingSize, _ui32MaxBits );
			uint32_t ui32Clear = m_ledDict.GetResetCode();
			if ( !_bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32Clear), m_ledDict.GetBits() ) ) {
				return 0;
			}
		}


		// Begin compression.
		if ( _ui64Size == 0ULL ) {
			return m_ledDict.GetBits();
		}


		uint64_t ui64Index = 0ULL;
		uint32_t ui32String = _pui8Bytes[ui64Index++];
		while ( ui64Index < _ui64Size ) {
			uint32_t ui32Char = _pui8Bytes[ui64Index++];
			uint32_t ui32EntryIndex = m_ledDict.GetEntryIndex( ui32String, ui32Char );
			// Does this combination exist?
			if ( m_ledDict.GetEntry( ui32EntryIndex ).ui32Code != CLzwEncodingDict::LSN_LZW_INVALID_CODE ) {
				// It exists so extend the code in an attempt to find the longest code that does not
				//	exist in the dictionary.
				ui32String = m_ledDict.GetEntry( ui32EntryIndex ).ui32Code;
			}
			else {
				// It does not exist so write the code to the stream.
				uint32_t ui32Bits = m_ledDict.GetBits();
				if ( !_bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32String), ui32Bits ) ) {
					return 0;
				}
				m_ledDict.AddEntry( ui32EntryIndex, ui32String, ui32Char );
				ui32String = ui32Char;

				// If the dictionary gets too big, reset it (along with a code in the stream to tell the
				//	decompressor to reset).
				if ( m_ledDict.TotalEntries() == m_ledDict.GetMaxDictionarySize() - 2 ) {
					uint32_t ui32Clear = m_ledDict.GetResetCode();
					if ( !_bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32Clear), m_ledDict.GetBits() ) ) {
						return 0;
					}
					m_ledDict.InitDict( _ui32DictionaryStartingSize, _ui32MaxBits );
				}
				if ( _bsStream.GetLengthInBytes() - ui32PrevLen > _ui32MaxSize ) { return m_ledDict.GetBits(); }
			}
		}

		if ( !_bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32String), m_ledDict.GetBits() ) ) {
			return 0;
		}

		// There is a catch.  The last code can increase the decoder's dictionary size if it just so happens that the
		//	dictionary is just one code away from increasing its bit size.
		//	To avoid having this happen, we write a clear code.
		uint32_t ui32Max = (1 << CStd::HighestBit( m_ledDict.TotalEntries() ));
		if ( m_ledDict.TotalEntries() == ui32Max ) {
			uint32_t ui32Clear = m_ledDict.GetResetCode();
			// Remember to write an extra bit.
			if ( !_bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32Clear), m_ledDict.GetBits() + 1 ) ) {
				return 0;
			}
			m_ledDict.InitDict( _ui32DictionaryStartingSize, _ui32MaxBits );
		}

		return m_ledDict.GetBits();
	}

	/**
	 * Append a bit stream with a stream-end code using the current dictionary.  If the dictionary is empty
	 *	the function fails.
	 *
	 * \param _bsStream The stream to which to append the end-of-stream code.
	 * \return Returns true if there is a dictionary and the append succeeded.
	 */
	LSBOOL LSE_CALL CLzwEncoder::EndStream( CBitStream &_bsStream ) const {
		if ( !m_ledDict.TotalEntries() ) { return false; }

		uint32_t ui32Code = m_ledDict.GetEndStreamCode();
		return _bsStream.WriteBits( reinterpret_cast<uint8_t *>(&ui32Code), m_ledDict.GetBits() );
	}

}	// namespace lsc
