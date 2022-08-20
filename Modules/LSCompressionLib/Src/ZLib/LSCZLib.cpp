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
 * Description: A primitive and light-weight implementation of a zlib inflation routine.  This class cannot
 *	be used to compress/deflate data.
 */

#include "LSCZLib.h"


namespace lsc {

	/**
	 * Default lengths.
	 */
	uint8_t CZLib::m_ui8DefaultLengths[288] = { 0 };

	/**
	 * Default distances.
	 */
	uint8_t CZLib::m_ui8DefaultDistances[32] = { 0 };

	/**
	 * Critcal section for updating the deafult values;
	 */
	CCriticalSection CZLib::m_scCrit;

	// == Various constructors.
	LSE_CALLCTOR CZLib::CZLib() :
		m_ui32BufferedBits( 0 ),
		m_ui32TotalBitsBuffered( 0 ) {
	}

	// == Functions.
	/**
	 * Decodes a ZLib buffer, optionally skipping the header as per certain iPhone PNG formats.
	 *
	 * \param _pui8Buffer The compressed buffer to decompress.
	 * \param _uiptrLen Length of the buffer to decompress.
	 * \param _bSkipHeaders If true, headers are skipped.
	 * \param _zbOut Holds the returned decompressed data.
	 * \return Returns true if the input buffer is valid and if there are no memory problems.
	 */
	LSBOOL LSE_CALL CZLib::DecodeZLib( const uint8_t * _pui8Buffer, uintptr_t _uiptrLen,
		LSBOOL _bSkipHeaders, LSC_ZLIB_BUFFER &_zbOut ) {
		CExternalByteStream ebsStream( _pui8Buffer, _uiptrLen );
		if ( !_bSkipHeaders ) {
			if ( !CheckHeader( ebsStream ) ) { return false; }
		}

		uint32_t ui32Final;
		do {
			if ( !Receive( ebsStream, 1, ui32Final ) ) { return false; }
			uint32_t ui32Type;
			if ( !Receive( ebsStream, 2, ui32Type ) ) { return false; }
			if ( ui32Type == 0 ) {
				if ( !ParseUncompressed( ebsStream, _zbOut ) ) { return false; }
			}
			else if ( ui32Type == 3 ) {
				return false;
			}
			else {
				if ( ui32Type == 1 ) {
					SetDefaults();
					if ( !BuildHuffman( m_hLength, m_ui8DefaultLengths, LSE_ELEMENTS( m_ui8DefaultLengths ) ) ) { return false; }
					if ( !BuildHuffman( m_hDistance, m_ui8DefaultDistances, LSE_ELEMENTS( m_ui8DefaultDistances ) ) ) { return false; }
				}
				else {
					if ( !ComputeHuffmanCodes( ebsStream ) ) { return false; }
				}

				if ( !ParseHuffmanBlock( ebsStream, _zbOut ) ) { return false; }
			}

		} while ( !ui32Final );

		return true;
	}

	/**
	 * Fill our bit buffer.
	 *
	 * \param _ebsStream The stream from which to take bits.
	 * \return Returns false if the stream runs out of bits.
	 */
	LSBOOL LSE_CALL CZLib::FillBits( const CExternalByteStream &_ebsStream ) {
		do {
			uint8_t ui8This;
			if ( !ReadUi8( _ebsStream, ui8This ) ) { return false; }
			m_ui32BufferedBits |= static_cast<uint32_t>(ui8This) << m_ui32TotalBitsBuffered;
			m_ui32TotalBitsBuffered += 8;
		} while ( m_ui32TotalBitsBuffered <= 24UL );
		return true;
	}

	/**
	 * Read X number of bits.
	 *
	 * \param _ebsStream The stream from which to take bits.
	 * \param _ui32Total The number of bits to read.
	 * \param _ui32Return Holds the returned value.
	 * \return Returns true if all bits were read.
	 */
	LSBOOL LSE_CALL CZLib::Receive( const CExternalByteStream &_ebsStream, uint32_t _ui32Total, uint32_t &_ui32Return ) {
		if ( m_ui32TotalBitsBuffered < _ui32Total ) {
			if ( !FillBits( _ebsStream ) ) { return false; }
		}
		_ui32Return = m_ui32BufferedBits & ((1 << _ui32Total) - 1);
		m_ui32BufferedBits >>= _ui32Total;
		m_ui32TotalBitsBuffered -= _ui32Total;
		return true;
	}

	/**
	 * Computes the Huffman codes.
	 *
	 * \param _ebsStream The stream.
	 * \return Returns false if the stream is not formatted properly.
	 */
	LSBOOL LSE_CALL CZLib::ComputeHuffmanCodes( const CExternalByteStream &_ebsStream ) {
		static uint8_t ui8LenDeZigZag[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

		LSC_HUFFMAN hCodeLen;

		uint8_t ui8LenCodes[286+32+137];
		uint8_t ui8CodeLenSizes[19];

		uint32_t ui32Lit, ui32Clen, ui32Dist;
		if ( !Receive( _ebsStream, 5, ui32Lit ) ) { return false; }
		ui32Lit += 257UL;
		if ( !Receive( _ebsStream, 5, ui32Dist ) ) { return false; }
		ui32Dist += 1;
		if ( !Receive( _ebsStream, 4, ui32Clen ) ) { return false; }
		ui32Clen += 4;

		CStd::MemSet( ui8CodeLenSizes, 0, sizeof( ui8CodeLenSizes ) );

		for ( uint32_t I = 0; I < ui32Clen; ++I ) {
			uint32_t ui32S;
			if ( !Receive( _ebsStream, 3, ui32S ) ) { return false; }
			ui8CodeLenSizes[ui8LenDeZigZag[I]] = static_cast<uint8_t>(ui32S);
		}

		if ( !BuildHuffman( hCodeLen, ui8CodeLenSizes, LSE_ELEMENTS( ui8CodeLenSizes ) ) ) { return false; }
		uint32_t ui32N = 0;


		while ( ui32N < ui32Lit + ui32Dist ) {
			int32_t i32Code = DecodeHuffman( _ebsStream, hCodeLen );
			if ( !(i32Code >= 0 && i32Code < 19) ) { return false; }
			if ( i32Code < 16 ) {
				ui8LenCodes[ui32N++] = static_cast<uint8_t>(i32Code);
			}
			else if ( i32Code == 16 ) {
				if ( !Receive( _ebsStream, 2, reinterpret_cast<uint32_t &>(i32Code) ) ) { return false; }
				i32Code += 3;
				CStd::MemSet( &ui8LenCodes[ui32N], ui8LenCodes[ui32N-1], static_cast<uint32_t>(i32Code) );
				ui32N += static_cast<uint32_t>(i32Code);
			}
			else if ( i32Code == 17 ) {
				if ( !Receive( _ebsStream, 3, reinterpret_cast<uint32_t &>(i32Code) ) ) { return false; }
				i32Code += 3;
				CStd::MemSet( &ui8LenCodes[ui32N], 0, static_cast<uint32_t>(i32Code) );
				ui32N += static_cast<uint32_t>(i32Code);
			}
			else {
				if ( i32Code != 18 ) { return false; }
				if ( !Receive( _ebsStream, 7, reinterpret_cast<uint32_t &>(i32Code) ) ) { return false; }
				i32Code += 11;
				CStd::MemSet( &ui8LenCodes[ui32N], 0, static_cast<uint32_t>(i32Code) );
				ui32N += static_cast<uint32_t>(i32Code);
			}
		}


		if ( ui32N != ui32Lit + ui32Dist ) { return false; }
		if ( !BuildHuffman( m_hLength, ui8LenCodes, ui32Lit ) ) { return false; }
		if ( !BuildHuffman( m_hDistance, ui8LenCodes + ui32Lit, ui32Dist ) ) { return false; }

		return true;
	}

	/**
	 * Builds Huffman codes.
	 *
	 * \param _hHuffman The target Huffman buffer.
	 * \param _pui8Codes Litst of codes.
	 * \param _ui32Total Length of the list of codes.
	 * \return Returns false if the stream is not formatted properly.
	 */
	LSBOOL LSE_CALL CZLib::BuildHuffman( LSC_HUFFMAN &_hHuffman,
		const uint8_t * _pui8Codes, uint32_t _ui32Total ) {
		int32_t i32Sizes[17] = { 0 }, i32NextCode[16];
		CStd::MemSet( _hHuffman.ui16Fast, 0xFF, sizeof( _hHuffman.ui16Fast ) );
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			++i32Sizes[_pui8Codes[I]];
		}

		i32Sizes[0] = 0;

		for ( uint32_t I = 16; I-- > 1; ) {
			if ( !(static_cast<uint32_t>(i32Sizes[I]) <= (1U << I)) ) { return false; }
		}

		int32_t i32Code = 0, i32K = 0;
		for ( uint32_t I = 1; I < 16; ++I ) {
			i32NextCode[I] = i32Code;
			_hHuffman.ui16FirstCode[I] = static_cast<uint16_t>(i32Code);
			_hHuffman.ui16FirstSymbol[I] = static_cast<uint16_t>(i32K);
			i32Code += i32Sizes[I];
			if ( i32Code - 1 >= static_cast<int32_t>(1 << I) ) { return false; }

			_hHuffman.i32MaxCode[I] = i32Code << (16 - I);
			i32Code <<= 1;
			i32K += i32Sizes[I];
		}


		_hHuffman.i32MaxCode[16] = 0x10000;
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			int32_t i32S = _pui8Codes[I];
			if ( i32S ) {
				int32_t i32ThisCode = i32NextCode[i32S] - _hHuffman.ui16FirstCode[i32S] + _hHuffman.ui16FirstSymbol[i32S];
				_hHuffman.ui8Size[i32ThisCode] = static_cast<uint8_t>(i32S);
				_hHuffman.ui16Values[i32ThisCode] = static_cast<uint16_t>(I);

				if ( i32S < LSC_FAST_BITS ) {
					uint32_t ui32K = ReverseBits( static_cast<uint16_t>(i32NextCode[i32S]), static_cast<uint32_t>(i32S) );
					while ( ui32K < (1 << LSC_FAST_BITS) ) {
						_hHuffman.ui16Fast[ui32K] = static_cast<uint16_t>(i32ThisCode);
						ui32K += (1 << i32S);
					}
				}
				++i32NextCode[i32S];
			}
		}

		return true;
	}

	/**
	 * Decode a Huffman buffer.
	 *
	 * \param _ebsStream The stream.
	 * \param _hHuffman The Huffman buffer.
	 * \return Returns the next code or -1 on error.
	 */
	int32_t LSE_CALL CZLib::DecodeHuffman( const CExternalByteStream &_ebsStream, LSC_HUFFMAN &_hHuffman ) {
		if ( m_ui32TotalBitsBuffered < 16 ) {
			if ( !FillBits( _ebsStream ) ) { return -1; }
		}


		int32_t i32B = _hHuffman.ui16Fast[m_ui32BufferedBits&LSC_MASK];
		if ( i32B < 0xFFFF ) {
			int32_t i32Size = _hHuffman.ui8Size[i32B];
			m_ui32BufferedBits >>= i32Size;
			m_ui32TotalBitsBuffered -= static_cast<uint32_t>(i32Size);
			return _hHuffman.ui16Values[i32B];
		}


		// Not resolved by the fast table.
		uint16_t ui16K = ReverseBits( static_cast<uint16_t>(m_ui32BufferedBits), 16 );
		uint32_t I;
		for ( I = LSC_FAST_BITS; ; ++I ) {
			if ( ui16K < _hHuffman.i32MaxCode[I] ) { break; }
		}

		if ( I == 16 ) { return -1; }

		i32B = (ui16K >> (16 - I)) - _hHuffman.ui16FirstCode[I] + _hHuffman.ui16FirstSymbol[I];
		if ( _hHuffman.ui8Size[i32B] != I ) { return -1; }

		m_ui32BufferedBits >>= I;
		m_ui32TotalBitsBuffered -= I;
		return _hHuffman.ui16Values[i32B];
	}

	/**
	 * Parse a Huffman block.
	 *
	 * \param _ebsStream The stream.
	 * \param _zbOut The output.
	 * \return Returns true if the Huffman block is valid.
	 */
	LSBOOL LSE_CALL CZLib::ParseHuffmanBlock( const CExternalByteStream &_ebsStream, LSC_ZLIB_BUFFER &_zbOut ) {
		static const int32_t i32LenBase[31] = {
			3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
		};

		static const int32_t i32LenExtra[31] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 0, 0 };

		static const uint32_t ui32DistBase[32] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 
			257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };

		static const uint32_t ui32DistExtra[32] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };


		for ( ; ; ) {
			int32_t i32Z = DecodeHuffman( _ebsStream, m_hLength );
			if ( i32Z < 0 ) { return false; }
			if ( i32Z < 256 ) {
				// Add the code to the output.
				if ( !_zbOut.Push( static_cast<uint8_t>(i32Z) ) ) { return false; }
			}
			else {
				// Repeat some part of the previous output.
				if ( i32Z == 256 ) { return true; }
				i32Z -= 257;
				if ( i32Z >= LSE_ELEMENTS( i32LenBase ) ) { return false; }
				int32_t i32Len = i32LenBase[i32Z];


				uint32_t ui32Temp;
				if ( i32LenExtra[i32Z] ) {
					if ( !Receive( _ebsStream, static_cast<uint32_t>(i32LenExtra[i32Z]), ui32Temp ) ) { return false; }
					i32Len += ui32Temp;
				}
				i32Z = DecodeHuffman( _ebsStream, m_hDistance );
				if ( i32Z < 0 || i32Z >= LSE_ELEMENTS( ui32DistBase ) ) { return false; }


				uint32_t ui32Dist = ui32DistBase[i32Z];
				if ( ui32DistExtra[i32Z] ) {
					if ( !Receive( _ebsStream, ui32DistExtra[i32Z], ui32Temp ) ) { return false; }
					ui32Dist += ui32Temp;
				}

				if ( _zbOut.Length() < ui32Dist ) { return false; }

				while ( i32Len-- ) {
					if ( !_zbOut.Push( _zbOut[_zbOut.Length()-ui32Dist] ) ) { return false; }
				}
			}
		}
	}

	/**
	 * Parse and uncompressed block.
	 *
	 * \param _ebsStream The stream.
	 * \param _zbOut The output.
	 * \return Returns true if the Huffman block is valid.
	 */
	LSBOOL LSE_CALL CZLib::ParseUncompressed( const CExternalByteStream &_ebsStream, LSC_ZLIB_BUFFER &_zbOut ) {
		if ( m_ui32TotalBitsBuffered & 7 ) {
			// Discard some bits.  Moves the input to the next byte boundary.
			uint32_t ui32Temp;
			if ( !Receive( _ebsStream, m_ui32TotalBitsBuffered & 7, ui32Temp ) ) { return false; }
		}

		uint32_t ui32K = 0;
		uint8_t ui8Header[4];
		// Start the header with any remaining bits in the buffer.
		while ( m_ui32TotalBitsBuffered ) {
			ui8Header[ui32K++] = static_cast<uint8_t>(m_ui32BufferedBits & 0xFF);
			m_ui32TotalBitsBuffered -= 8;
			m_ui32BufferedBits >>= 8;
		}


		// Fill the rest of the header with data from the stream.
		while ( ui32K < 4 ) {
			if ( !ReadUi8( _ebsStream, ui8Header[ui32K++] ) ) { return false; }
		}

		uint32_t ui32Len = ui8Header[1] * 256 + ui8Header[0];
		uint32_t ui32LenN = ui8Header[3] * 256 + ui8Header[2];
		if ( ui32LenN != (ui32Len & 0xFFFFUL) ) { return false; }
		// Copy from the current position of the input buffer.
		uint32_t ui32End = _zbOut.Length();
		if ( !_zbOut.ResizeOver( ui32End + ui32Len ) ) { return false; }
		return _ebsStream.ReadBytes( &_zbOut[ui32End], ui32Len ) != 0;
	}

	/**
	 * Check the ZLib header.
	 *
	 * \param _ebsStream The stream containing the ZLib-compressed buffer.
	 * \return Returns true if the header is valid.
	 */
	LSBOOL LSE_CALL CZLib::CheckHeader( const CExternalByteStream &_ebsStream ) {
		uint8_t ui8CmF;
		if ( !ReadUi8( _ebsStream, ui8CmF ) ) { return false; }
		uint8_t ui8Cm = ui8CmF & 0xF;

		uint8_t ui8Flag;
		if ( !ReadUi8( _ebsStream, ui8Flag ) ) { return false; }
		if ( (ui8CmF * 256 + ui8Flag) % 31 != 0 ) { return false; }
		if ( ui8Flag & 32 ) { return false; }
		if ( ui8Cm != 8 ) { return false; }
		return true;
	}

	/**
	 * Reads a single 8-bit value from the given stream.
	 *
	 * \param _ebsStream The stream from which to read an 8-bit value.
	 * \param _ui8Dst Destination for the read value.
	 * \return Returns true if the value was read from the stream.
	 */
	LSBOOL LSE_CALL CZLib::ReadUi8( const CExternalByteStream &_ebsStream, uint8_t &_ui8Dst ) {
		return _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&_ui8Dst), sizeof( _ui8Dst ) ) != 0;
	}

	/**
	 * Apply default sizes and distances.
	 */
	void LSE_CALL CZLib::SetDefaults() {
		CCriticalSection::CLocker lLockMe( m_scCrit );
		if ( !m_ui8DefaultDistances[0] ) {
			uint32_t I;
			for ( I = 0; I <= 143; ++I ) {
				m_ui8DefaultLengths[I] = 8;
			}
			for ( ; I <= 255; ++I ) {
				m_ui8DefaultLengths[I] = 9;
			}
			for ( ; I <= 279; ++I ) {
				m_ui8DefaultLengths[I] = 7;
			}
			for ( ; I <= 287; ++I ) {
				m_ui8DefaultLengths[I] = 8;
			}

			for ( I = 0; I <= 31; ++I ) {
				m_ui8DefaultDistances[I] = 5;
			}
		}
	}

}	// namespace lsc
