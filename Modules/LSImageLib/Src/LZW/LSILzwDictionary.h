/**
 * Copyright L. Spiro 2008
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A dictionary of codes + characters for use with LZW compression and
 *	decompression.
 */


#ifndef __LSI_LZWDICTIONARY_H__
#define __LSI_LZWDICTIONARY_H__

#include "../LSIImageLib.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsi {

	/**
	 * Class CLzwDictionary
	 *
	 * Description: A dictionary of codes + characters for use with LZW compression and
	 *	decompression.
	 */
	class CLzwDictionary {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CLzwDictionary();
		LSE_CALLCTOR						~CLzwDictionary();

		
		// == Types.
		/**
		 * A code.
		 */
		typedef uint16_t					LSI_LZW_CODE;

		/**
		 * Decompression uses a smaller dictionary.
		 */
		typedef struct LSI_LZW_DICT_ENTRY_DECOMPRESS {
			/**
			 * The root code for this character.  The code is the same as the code
			 *	for this entry if it is a root node.  Otherwise it is the code for
			 *	the entry from which this entry was derived.
			 */
			uint32_t						lcBaseCode	: 12;

			/**
			 * The character appended to the base code which creates the unique identity
			 *	for this entry.
			 */
			uint32_t						ui8Char		: 8;
		} * LPLSI_LZW_DICT_ENTRY_DECOMPRESS, * const LPCLSI_LZW_DICT_ENTRY_DECOMPRESS;


		// == Functions.
		/**
		 * Sets the allocator FIRST.
		 *
		 * \param _paAllocator The allocator to be used by this object.  Must be set before any
		 *	action on this object takes place.
		 */
		void LSE_CALL						SetAllocator( CAllocator * _paAllocator );
		
		/**
		 * Sets the initial dictionary where each entry is a root.
		 *
		 * \param _ui32Len Number of codes in the base dictionary.  No more than 256.
		 * \param _ui32MaxBits Maximum number of bits for any code.
		 *	since this class only performs decompression, this must always be true.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL						InitializeDictionary( uint32_t _ui32Len, uint32_t _ui32MaxBits );
		
		/**
		 * Is the given code the clear code?
		 *
		 * \param _lcCode The code to be tested against the clear code.
		 * \return Returns true if the code is the clear code.
		 */
		LSBOOL LSE_CALL						IsClearCode( LSI_LZW_CODE _lcCode ) const;
		
		/**
		 * Is the given code the end-of-input code?
		 *
		 * \param _lcCode The code to be tested against the end-of-input code.
		 * \return Returns true if the code is the end-of-input code.
		 */
		LSBOOL LSE_CALL						IsEndOfInputCode( LSI_LZW_CODE _lcCode ) const;
		
		/**
		 * Gets the size of the dictionary.
		 *
		 * \return Returns the number of codes in the dictionary.
		 */
		uint32_t LSE_CALL					GetTotal() const;

		/**
		 * Gets an entry index by its code only.  Used when decompressing.
		 *
		 * \param _lcCode The code whose index is to be obtained.
		 * \return Returns the index of the code, which is always the code itself.
		 */
		uint32_t LSE_CALL					GetEntryIndex( LSI_LZW_CODE _lcCode ) const;

		/**
		 * Gets the number of bits needed to access any code.
		 *
		 * \return Returns the minimum number of bits needed to access any code in the dictionary.
		 */
		LSE_INLINE uint32_t LSE_CALL		GetBitsNeeded() const;

		/**
		 * Adds a code during decompression.
		 *
		 * \param _lcCode The code to be added.
		 * \param _ui8Char The character associated with this code.
		 */
		void LSE_CALL						AddCode( LSI_LZW_CODE _lcCode, uint8_t _ui8Char );

		/**
		 * Gets a string from the dictionary.  Translates all codes to characters and returns the length of the string.
		 *	The string is returned in reverse order.
		 *
		 * \param _lcCode Code for which to get the string.
		 * \param _pui8String Holds the returned string in reverse order.
		 * \return Returns the number of characters in the returned string.
		 */
		uint32_t LSE_CALL					GetString( LSI_LZW_CODE _lcCode, uint8_t * _pui8String ) const;
		
		/**
		 * Gets bits from a stream.  _ui64BitIndex is increased by _ui32Total upon return.
		 *
		 * \param _pui8Stream Stream of data from which to read the bits.
		 * \param _ui64BitIndex Bit index from which to start reading.
		 * \param _ui32Total Number of bits to get.
		 * \return Returns the read bits as an unsigned 32-bit integer.
		 */
		static uint32_t LSE_CALL			GetBits( const uint8_t * _pui8Stream, uint64_t &_ui64BitIndex, uint32_t _ui32Total );

	protected :
		// == Members.
		/**
		 * The dictionary array.
		 */
		CVectorPoD<LSI_LZW_DICT_ENTRY_DECOMPRESS, uint32_t>
											m_vDecompDictionary;
		
		/**
		 * Maximum size.
		 */
		uint32_t							m_ui32MaxSize;
		
		/**
		 * Maximum bits.
		 */
		uint32_t							m_ui32MaxBits;
		
		/**
		 * Clear-code index.
		 */
		uint32_t							m_ui32ClearCode;
		
		/**
		 * End-of-input index.
		 */
		uint32_t							m_ui32EndOfInput;

		/**
		 * The number of bits required to read any code in the list.
		 */
		uint32_t							m_ui32BitsNeeded;
		
		
		// == Functions.
		/**
		 * Allocates the given amount of entries.
		 *
		 * \param _ui32Total Total number of entries to allocate.
		 * \return Returns true if there is enough RAM to complete the operation.
		 */
		LSBOOL LSE_CALL						Alloc( uint32_t _ui32Total );
		
		/**
		 * Reset everything.
		 */
		void LSE_CALL						Reset();

		/**
		 * Derive the total number of bits needed to read any code in the dictionary.
		 *
		 * \return Returns the number of bits needed to read any code in the dictionary.
		 */
		uint32_t LSE_CALL					DeriveBitsNeeded() const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the number of bits needed to access any code.
	 *
	 * \return Returns the minimum number of bits needed to access any code in the dictionary.
	 */
	LSE_INLINE uint32_t LSE_CALL CLzwDictionary::GetBitsNeeded() const {
		return CStd::Min<uint32_t>( m_ui32BitsNeeded, m_ui32MaxBits );
	}

}	// namespace lsi

#endif	// __LSI_LZWDICTIONARY_H__
