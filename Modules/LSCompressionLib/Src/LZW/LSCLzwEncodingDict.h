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
 * Description: Description: When encoding bytes, the dictionary for the LZW algorithm is stored differently from when the
 *	bytes are being decoded.  During encryption, codes are referenced via (code+char) combination which
 *	requires a hash look-up.  For this reason, we cannot store the codes in such a way that index = code.
 *	During decoding, however, codes are referenced only by code, so we are able to store the codes linearly
 *	such that index = code.  This saves space over the encoding algorithm and gives us cause to keep the
 *	two dictionaries separate.
 * The dictionary is created with a maximum size at the beginning, which depends on the maximum number of
 *	bits a code may have.  This class can output variable-sized codes or predefined-sized codes as per the
 *	desires of the user.  Variable-sized codes simply means that the smallest number of bits needed to
 *	access any code in the dictionary are output instead of using a fixed size such as 15 bits for every
 *	code.  This is the typical method for encoding.
 */


#ifndef __LSC_LZWENCODINGDICT_H__
#define __LSC_LZWENCODINGDICT_H__

#include "../LSCCompressionLib.h"
#include "Allocator/LSTLAllocator.h"

namespace lsc {

	/**
	 * Class CLzwEncodingDict
	 * \brief A dictionary for the encoding of a bytestream using the LZW algorithm.
	 *
	 * Description: When encoding bytes, the dictionary for the LZW algorithm is stored differently from when the
	 *	bytes are being decoded.  During encryption, codes are referenced via (code+char) combination which
	 *	requires a hash look-up.  For this reason, we cannot store the codes in such a way that index = code.
	 *	During decoding, however, codes are referenced only by code, so we are able to store the codes linearly
	 *	such that index = code.  This saves space over the encoding algorithm and gives us cause to keep the
	 *	two dictionaries separate.
	 * The dictionary is created with a maximum size at the beginning, which depends on the maximum number of
	 *	bits a code may have.  This class can output variable-sized codes or predefined-sized codes as per the
	 *	desires of the user.  Variable-sized codes simply means that the smallest number of bits needed to
	 *	access any code in the dictionary are output instead of using a fixed size such as 15 bits for every
	 *	code.  This is the typical method for encoding.
	 */
	class CLzwEncodingDict {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLzwEncodingDict();
		LSE_CALLCTOR							~CLzwEncodingDict();


		// == Enumerations.
		/**
		 * Flags we use.
		 */
		enum LSN_LZW_INVALID_CODE {
			LSN_LZW_INVALID_CODE				= LSE_MAXU32,
			LSN_LZW_FORCE_DWORD					= 0x7FFFFFFFUL,
		};


		// == Types.
		/**
		 * A dictionary entry.
		 */
		typedef struct LSN_LZW_DICT_ENTRY {
			/**
			 * The code for this entry.
			 */
			uint32_t							ui32Code;
			
			/**
			 * The code on which this is based.
			 */
			uint32_t							ui32BaseCode;
			
			/**
			 * The character appended to the base code which creates this entry.
			 */
			uint8_t								ui8Char;
		} * LPLSN_LZW_DICT_ENTRY, * const LPCLSN_LZW_DICT_ENTRY;


		// == Functions.
		/**
		 * Reset everything.  Deallocates all resources.
		 */
		void  LSE_CALL						Reset();

		/**
		 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
		 *
		 * \param _psaAllocator The allocator to use by this class.
		 */
		void LSE_CALL							SetAllocator( CStdAllocator * _psaAllocator );
		
		/**
		 * Begin a compression.  We need to know the maximum number of bits to create the dictionary.
		 *	The dictionary can be cleared and recreated many times during compression.
		 *
		 * \param _ui32RootCodes The default dictionary is composed of this number of root codes.
		 * \param _psaAllocator The allocator to use by this class.
		 * \return Returns false if allocation of the dictionary failed, or true otherwise.
		 */
		LSBOOL LSE_CALL							InitDict( uint32_t _ui32RootCodes, uint32_t _ui32MaxBits );
		
		/**
		 * Gets the index of an entry in the dictionary.  If the entry does not exist, the returned
		 *	index is where it should be added to the dictionary.
		 *
		 * \param _ui32Code The code of the entry to find.
		 * \param _ui32Char The allocator to use by this class.
		 * \return Returns false if allocation of the dictionary failed, or true otherwise.
		 */
		uint32_t LSE_CALL						GetEntryIndex( uint32_t _ui32Code, uint32_t _ui32Char ) const;

		/**
		 * Gets an entry by index for read only.  The entry is not guaranteed to contain a valid code.
		 *
		 * \param _ui32Index Index of the entry to retrieve.
		 * \return Returns a constant reference to the entry at the given index.
		 */
		const LSN_LZW_DICT_ENTRY & LSE_CALL		GetEntry( uint32_t _ui32Index ) const;
		
		/**
		 * Adds an entry at a given index.  The index must have been returned by a previous call to
		 *	GetEntryIndex() and must be unused.
		 *
		 * \param _ui32Index Index, returned by a previous call to GetEntryIndex(), where the code is to be
		 *	added.
		 * \param _ui32BaseCode The base code of the new code to be added.  The base code is the code of an
		 *	existing entry off which this entry is based.  This entry extends the base code by one character,
		 *	given in _ui32Char.
		 * \param _ui32Char The character by which the new code extends the base code.
		 */
		void LSE_CALL							AddEntry( uint32_t _ui32Index, uint32_t _ui32BaseCode, uint32_t _ui32Char );

		/**
		 * Gets the size of the dictionary.
		 *
		 * \return Returns the number of entries in the dictionary.
		 */
		uint32_t LSE_CALL						TotalEntries() const;

		/**
		 * Gets the maximum dictionary size.
		 *
		 * \return Returns the maximum dictionary size.
		 */
		uint32_t LSE_CALL						GetMaxDictionarySize() const;

		/**
		 * Gets the number of bits needed at the beginning of the dictionary (when the dictionary is set to
		 *	its root size).
		 *
		 * \return Returns the number of bits needed to access any code in the dictionary when the dictionary
		 *	is in its default state.
		 */
		uint32_t LSE_CALL						GetStartingBits() const;

		/**
		 * Gets the reset-dictionary code.
		 *
		 * \return Returns the reset-dictionary code.
		 */
		uint32_t LSE_CALL						GetResetCode() const;

		/**
		 * Gets the end-stream code.
		 *
		 * \return Returns the end-stream code.
		 */
		uint32_t LSE_CALL						GetEndStreamCode() const;

		/**
		 * Gets the bits needed to read any code in the dictionary.
		 *
		 * \return Returns the bits needed to read any code in the dictionary.
		 */
		uint32_t LSE_CALL						GetBits() const;


	protected :
		// == Members.
		/**
		 * Our allocator.
		 */
		CAllocator								m_caAllocator;
		
		/**
		 * Allocated size of the dictionary.
		 */
		uint32_t								m_ui32AllocatedDictSize;

		/**
		 * Maximum bits in the dictionary entries.
		 */
		uint32_t								m_ui32MaxBits;
		
		/**
		 * Actual number of entries in the dictionary.
		 */
		uint32_t								m_ui32DictSize;

		/**
		 * The minimum number of bits needed to access every entry in the dictionary.
		 */
		uint16_t								m_ui16MinBits;

		/**
		 * The starting size of each entry in bits.
		 */
		uint16_t								m_ui16StartingBits;

		/**
		 * Root codes.
		 */
		uint32_t								m_ui32RootCodes;

		/**
		 * The dictionary.
		 */
		LPLSN_LZW_DICT_ENTRY					m_lpldeDictionary;
		
	};

}	// namespace lsc

#endif	// __LSC_LZWENCODINGDICT_H__
