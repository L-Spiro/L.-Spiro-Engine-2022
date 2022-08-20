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
 * Description: When decoding, the dictionary format for dictionary entries in the LZW compression
 *	scheme are different from when encoding.  The codes are referenced only by code during
 *	decompression, allowing us to store the entries linearly during decompression.  Our loop-up and
 *	usage functiosn for the codes are different from the ones used during encryption.
 */


#ifndef __LSC_LZWDECODINGDICT_H__
#define __LSC_LZWDECODINGDICT_H__

#include "../LSCCompressionLib.h"
#include "String/LSTLString.h"

namespace lsc {

	/**
	 * Class CLzwDecodingDict
	 * \brief A dictionary for decoding an LZW bytestream.
	 *
	 * Description: When decoding, the dictionary format for dictionary entries in the LZW compression
	 *	scheme are different from when encoding.  The codes are referenced only by code during
	 *	decompression, allowing us to store the entries linearly during decompression.  Our loop-up and
	 *	usage functiosn for the codes are different from the ones used during encryption.
	 */
	class CLzwDecodingDict {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLzwDecodingDict();
		LSE_CALLCTOR							~CLzwDecodingDict();


		// == Types.
		/**
		 * A code.
		 */
		typedef uint32_t						LSN_LZW_CODE;

		/**
		 * A dictionary entry.  The code of the item is its index in the array.
		 */
		typedef struct LSN_LZW_DICT_ENTRY {
			// The code on which this is based.
			uint32_t							ui32BaseCode	: 24;
			
			// The character appended to the base code which creates this entry.
			uint32_t							ui8Char			: 8;
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
		 * Sets the initial dictionary where each entry is a root.
		 *
		 * \param _ui32Len Number of root entries in the dictionary.
		 * \param _ui32MaxBits Maximum number of bits in any entry in the dictionary.
		 */
		void LSE_CALL							InitializeDictionary( uint32_t _ui32Len, uint32_t _ui32MaxBits );

		/**
		 * Is the given code the clear code?
		 *
		 * \param _lcCode The code to be tested against the clear code.
		 * \return Returns true if the code is the clear code.
		 */
		LSBOOL LSE_CALL							IsClearCode( LSN_LZW_CODE _lcCode ) const;
		
		/**
		 * Is the given code the end-of-input code?
		 *
		 * \param _lcCode The code to be tested against the end-of-input code.
		 * \return Returns true if the code is the end-of-input code.
		 */
		LSBOOL LSE_CALL							IsEndOfInputCode( LSN_LZW_CODE _lcCode ) const;

		/**
		 * Gets the size of the dictionary.
		 *
		 * \return Returns the number of codes in the dictionary.
		 */
		uint32_t LSE_CALL						GetTotal() const;

		/**
		 * Adds a code to the dictionary.  Fails if the dictionary is full.
		 *
		 * \param _lcCode The code to be added.
		 * \param _ui8Char The character associated with this code.
		 */
		LSBOOL LSE_CALL							AddCode( LSN_LZW_CODE _lcCode, uint8_t _ui8Char );

		/**
		 * Gets a string from the dictionary.  Translates all codes to characters and returns the length
		 *	of the string. The string is returned in reverse order.
		 *
		 * \param _lcCode The code to translate into a byte string.
		 * \param _sString Upone return, this string contains the returned bytes.
		 * \return Returns true if the string was extracted successfully.  If any unknown codes are
		 *	encountered, false is returned.
		 */
		LSBOOL LSE_CALL							GetString( LSN_LZW_CODE _lcCode, CString &_sString ) const;

		/**
		 * Gets the number of bits needed to access any code.
		 *
		 * \return Returns the minimum number of bits needed to access any code in the dictionary.
		 */
		LSE_INLINE uint32_t LSE_CALL			GetBitsNeeded() const;


	protected :
		// == Members.
		/**
		 * Our allocator.
		 */
		CAllocator								m_aAllocator;

		/**
		 * The dictionary.
		 */
		LPLSN_LZW_DICT_ENTRY					m_lpldeDictionary;

		/**
		 * The size of the dictionary.
		 */
		uint32_t								m_ui32Total,
												m_ui32Alloc;

		/**
		 * Size of the root.
		 */
		uint32_t								m_ui32RootLen;

		/**
		 * Maximum size.
		 */
		uint32_t								m_ui32MaxSize;

		/**
		 * Maximum number of bits in an entry.
		 */
		uint32_t								m_ui32MaxBits;

		/**
		 * The bits needed for the current code.
		 */
		uint32_t								m_ui32BitsNeeded;


		// == Functions.
		/**
		 * Allocates the full set of codes and initialize the roots.
		 *
		 * \return Returns false if allocation fails or if m_ui32RootLen is 0.
		 */
		LSBOOL LSE_CALL							Alloc();

		/**
		 * Derive the total number of bits needed to read any code in the dictionary.
		 *
		 * \return Returns the number of bits needed for a code in the stream given the current
		 *	size of the dictionary.
		 */
		uint32_t LSE_CALL						DeriveBitsNeeded() const;
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
	LSE_INLINE uint32_t LSE_CALL CLzwDecodingDict::GetBitsNeeded() const {
		return m_ui32BitsNeeded;
	}

}	// namespace lsc

#endif	// __LSC_LZWDECODINGDICT_H__
