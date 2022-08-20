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


#ifndef __LSC_LZWDECODER_H__
#define __LSC_LZWDECODER_H__

#include "../LSCCompressionLib.h"
#include "../BitStream/LSCBitStream.h"
#include "LSCLzwDecodingDict.h"

namespace lsc {

	/**
	 * Class CLzwDecoder
	 * \brief Decodes a bitstream previously encoded with CLzwEncoder::Encode().
	 *
	 * Description: Decodes a bitstream previously encoded with CLzwEncoder::Encode().
	 */
	class CLzwDecoder {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLzwDecoder();


		// == Functions.
		/**
		 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
		 *
		 * \param _psaAllocator The allocator to use by this class.
		 */
		void LSE_CALL							SetAllocator( CStdAllocator * _psaAllocator );

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
		uint32_t LSE_CALL						Decode( const CBitStream &_bsStream,
			uint32_t _ui32DictionaryStartingSize, uint32_t _ui32MaxBits,
			lsstd::CStream &_sStream, LSBOOL _bResetDictionary = false );


	protected :
		// == Members.
		/**
		 * Our dictionary.
		 */
		CLzwDecodingDict						m_lddDict;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsc

#endif	// __LSC_LZWDECODER_H__
