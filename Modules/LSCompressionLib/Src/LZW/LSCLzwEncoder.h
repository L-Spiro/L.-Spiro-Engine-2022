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


#ifndef __LSC_LZWENCODER_H__
#define __LSC_LZWENCODER_H__

#include "../LSCCompressionLib.h"
#include "../BitStream/LSCBitStream.h"
#include "LSCLzwEncodingDict.h"

namespace lsc {

	/**
	 * Class CLzwEncoder
	 * \brief Encodes a stream of bytes into an LZW compressed stream of bits.
	 *
	 * Description: Encodes a stream of bytes into an LZW compressed stream of bits.
	 */
	class CLzwEncoder {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLzwEncoder();


		// == Functions.
		/**
		 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
		 *
		 * \param _psaAllocator The allocator to use by this class.
		 */
		void LSE_CALL							SetAllocator( CStdAllocator * _psaAllocator );

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
		uint32_t LSE_CALL						Encode( const uint8_t * _pui8Bytes, uint64_t _ui64Size,
			uint32_t _ui32DictionaryStartingSize, uint32_t _ui32MaxBits,
			CBitStream &_bsStream, LSBOOL _bResetDictionary = false, uint32_t _ui32PrevBits = 0, uint32_t _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Append a bit stream with a stream-end code using the current dictionary.  If the dictionary is empty
		 *	the function fails.
		 *
		 * \param _bsStream The stream to which to append the end-of-stream code.
		 * \return Returns true if there is a dictionary and the append succeeded.
		 */
		LSBOOL LSE_CALL							EndStream( CBitStream &_bsStream ) const;


	protected :
		// == Members.
		/**
		 * The dictionary we use.
		 */
		CLzwEncodingDict						m_ledDict;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsc

#endif	// __LSC_LZWENCODER_H__
