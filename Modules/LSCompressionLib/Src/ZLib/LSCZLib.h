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


#ifndef __LSC_ZLIB_H__
#define __LSC_ZLIB_H__

#include "../LSCCompressionLib.h"
#include "../BitStream/LSCBitStream.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsc {

	/**
	 * Class CZLib
	 * \brief A primitive and light-weight implementation of a zlib inflation routine.
	 *
	 * Description: A primitive and light-weight implementation of a zlib inflation routine.  This class cannot
	 *	be used to compress/deflate data.
	 */
	class CZLib {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CZLib();


		// == Types.
		/**
		 * The output buffer.
		 */
		typedef CVectorPoD<uint8_t, uint32_t, 0xFFFFUL>
										LSC_ZLIB_BUFFER;

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
		LSBOOL LSE_CALL					DecodeZLib( const uint8_t * _pui8Buffer, uintptr_t _uiptrLen,
			LSBOOL _bSkipHeaders, LSC_ZLIB_BUFFER &_zbOut );


	protected :
		// == Enumerations.
		/**
		 * Huffman metrics.
		 */
		enum {
			LSC_FAST_BITS				= 9,
			LSC_MASK					= (1 << LSC_FAST_BITS) - 1,
		};


		// == Types.
		/**
		 * Huffman table.
		 */
		typedef struct LSC_HUFFMAN {
			/**
			 * Max codes.
			 */
			int32_t						i32MaxCode[17];

			/**
			 * Fast data.
			 */
			uint16_t					ui16Fast[1<<LSC_FAST_BITS];

			/**
			 * First code.
			 */
			uint16_t					ui16FirstCode[16];

			/**
			 * First symbol.
			 */
			uint16_t					ui16FirstSymbol[16];

			/**
			 * Values.
			 */
			uint16_t					ui16Values[288];

			/**
			 * Sizes.
			 */
			uint8_t						ui8Size[288];
		} * LPLSC_HUFFMAN, * const LPCLSC_HUFFMAN;

		// == Members.
		/**
		 * Buffered bits.
		 */
		uint32_t						m_ui32BufferedBits;

		/**
		 * Total bits buffered.
		 */
		uint32_t						m_ui32TotalBitsBuffered;

		/**
		 * Length Huffman buffer.
		 */
		LSC_HUFFMAN						m_hLength;

		/**
		 * Distance Huffman buffer.
		 */
		LSC_HUFFMAN						m_hDistance;

		/**
		 * Default lengths.
		 */
		static uint8_t					m_ui8DefaultLengths[288];

		/**
		 * Default distances.
		 */
		static uint8_t					m_ui8DefaultDistances[32];

		/**
		 * Critcal section for updating the deafult values;
		 */
		static CCriticalSection			m_scCrit;


		// == Functions.
		/**
		 * Fill our bit buffer.
		 *
		 * \param _ebsStream The stream from which to take bits.
		 * \return Returns false if the stream runs out of bits.
		 */
		LSBOOL LSE_CALL					FillBits( const CExternalByteStream &_ebsStream );

		/**
		 * Read X number of bits.
		 *
		 * \param _ebsStream The stream from which to take bits.
		 * \param _ui32Total The number of bits to read.
		 * \param _ui32Return Holds the returned value.
		 * \return Returns true if all bits were read.
		 */
		LSBOOL LSE_CALL					Receive( const CExternalByteStream &_ebsStream, uint32_t _ui32Total, uint32_t &_ui32Return );

		/**
		 * Computes the Huffman codes.
		 *
		 * \param _ebsStream The stream.
		 * \return Returns false if the stream is not formatted properly.
		 */
		LSBOOL LSE_CALL					ComputeHuffmanCodes( const CExternalByteStream &_ebsStream );

		/**
		 * Builds Huffman codes.
		 *
		 * \param _hHuffman The target Huffman buffer.
		 * \param _pui8Codes Litst of codes.
		 * \param _ui32Total Length of the list of codes.
		 * \return Returns false if the stream is not formatted properly.
		 */
		LSBOOL LSE_CALL					BuildHuffman( LSC_HUFFMAN &_hHuffman,
			const uint8_t * _pui8Codes, uint32_t _ui32Total );

		/**
		 * Decode a Huffman buffer.
		 *
		 * \param _ebsStream The stream.
		 * \param _hHuffman The Huffman buffer.
		 * \return Returns the next code or -1 on error.
		 */
		int32_t LSE_CALL				DecodeHuffman( const CExternalByteStream &_ebsStream, LSC_HUFFMAN &_hHuffman );

		/**
		 * Parse a Huffman block.
		 *
		 * \param _ebsStream The stream.
		 * \param _zbOut The output.
		 * \return Returns true if the Huffman block is valid.
		 */
		LSBOOL LSE_CALL					ParseHuffmanBlock( const CExternalByteStream &_ebsStream, LSC_ZLIB_BUFFER &_zbOut );

		/**
		 * Parse and uncompressed block.
		 *
		 * \param _ebsStream The stream.
		 * \param _zbOut The output.
		 * \return Returns true if the Huffman block is valid.
		 */
		LSBOOL LSE_CALL					ParseUncompressed( const CExternalByteStream &_ebsStream, LSC_ZLIB_BUFFER &_zbOut );

		/**
		 * Check the ZLib header.
		 *
		 * \param _ebsStream The stream containing the ZLib-compressed buffer.
		 * \return Returns true if the header is valid.
		 */
		static LSBOOL LSE_CALL			CheckHeader( const CExternalByteStream &_ebsStream );

		/**
		 * Reads a single 8-bit value from the given stream.
		 *
		 * \param _ebsStream The stream from which to read an 8-bit value.
		 * \param _ui8Dst Destination for the read value.
		 * \return Returns true if the value was read from the stream.
		 */
		static LSBOOL LSE_CALL			ReadUi8( const CExternalByteStream &_ebsStream, uint8_t &_ui8Dst );

		/**
		 * Reverse bits and shift off some.  The reverse is assumed to be on a 16-bit value.
		 *
		 * \param _ui16ReverseMe The value to reverse.
		 * \param _ui32Shift The number of bits to keep from the reversed value.
		 * \return Returns the 16-bit reverse and shift of the given value.
		 */
		static LSE_INLINE uint16_t LSE_CALL
										ReverseBits( uint16_t _ui16ReverseMe, uint32_t _ui32Shift );

		/**
		 * Apply default sizes and distances.
		 */
		static void LSE_CALL			SetDefaults();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Reverse bits and shift off some.  The reverse is assumed to be on a 16-bit value.
	 *
	 * \param _ui16ReverseMe The value to reverse.
	 * \param _ui32Shift The number of bits to keep from the reversed value.
	 * \return Returns the 16-bit reverse and shift of the given value.
	 */
	 LSE_INLINE uint16_t LSE_CALL CZLib::ReverseBits( uint16_t _ui16ReverseMe, uint32_t _ui32Shift ) {
		 return static_cast<uint16_t>(CStd::BitSwap16( _ui16ReverseMe ) >> (16 - _ui32Shift));
	 }

}	// namespace lsc

#endif	// __LSC_ZLIB_H__
