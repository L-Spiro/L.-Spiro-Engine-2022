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

#include "LSCLzwEncodingDict.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CLzwEncodingDict::CLzwEncodingDict() :
		m_ui32AllocatedDictSize( 0 ),
		m_ui32DictSize( 0 ),
		m_ui32MaxBits( 0 ),
		m_ui16MinBits( 0 ),
		m_ui16StartingBits( 0 ),
		m_ui32RootCodes( 0 ),
		m_lpldeDictionary( NULL ) {
	}
	LSE_CALLCTOR CLzwEncodingDict::~CLzwEncodingDict() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.  Deallocates all resources.
	 */
	void  LSE_CALL CLzwEncodingDict::Reset() {
		//m_bsData.Reset();
		m_ui32AllocatedDictSize = m_ui32DictSize = m_ui32MaxBits = m_ui32RootCodes = 0;
		m_ui16MinBits = m_ui16StartingBits = 0;

		if ( m_lpldeDictionary ) {
			m_caAllocator.Free( m_lpldeDictionary );
		}
		m_lpldeDictionary = NULL;
	}

	/**
	 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
	 *
	 * \param _psaAllocator The allocator to use by this class.
	 */
	void LSE_CALL CLzwEncodingDict::SetAllocator( CStdAllocator * _psaAllocator ) {
		if ( m_caAllocator.GetAllocator() != _psaAllocator ) {
			Reset();
			m_caAllocator.SetAllocator( _psaAllocator );
		}
	}

	/**
	 * Begin a compression.  We need to know the maximum number of bits to create the dictionary.
	 *	The dictionary can be cleared and recreated many times during compression.
	 *
	 * \param _ui32RootCodes The default dictionary is composed of this number of root codes.
	 * \param _psaAllocator The allocator to use by this class.
	 * \return Returns false if allocation of the dictionary failed, or true otherwise.
	 */
	LSBOOL LSE_CALL CLzwEncodingDict::InitDict( uint32_t _ui32RootCodes, uint32_t _ui32MaxBits ) {
		// If the maximum dictionary size has changed, we need to reallocate it.
		uint32_t ui32MaxDictSize;
		// A table of bits and dictionary sizes (where each dictionary size is a prime number).
		static const uint32_t ui32DictSizes[] = {
			1,			// 0
			3,			// 1
			7,			// 2
			13,			// 3
			29,			// 4
			53,			// 5
			97,			// 6
			193,		// 7
			389,		// 8
			769,		// 9
			1543,		// 10
			3079,		// 11
			6151,		// 12
			12289,		// 13
			24593,		// 14
			49157,		// 15
			98317,		// 16
			196613,		// 17
			393241,		// 18
			786433,		// 19
			1572869,	// 20
			3145739,	// 21
			6291469,	// 22
			12582917,	// 23
			25165843,	// 24
		};
		if ( _ui32MaxBits >= LSE_ELEMENTS( ui32DictSizes ) ) { return false; }
		ui32MaxDictSize = ui32DictSizes[_ui32MaxBits];
		if ( ui32MaxDictSize != m_ui32AllocatedDictSize ) {
			// Changed size.  Reallocate.
			LPLSN_LZW_DICT_ENTRY lpldeCopy = static_cast<LPLSN_LZW_DICT_ENTRY>(m_caAllocator.ReAlloc( m_lpldeDictionary, sizeof( LSN_LZW_DICT_ENTRY ) * ui32MaxDictSize ));
			if ( !lpldeCopy ) { return false; }
			m_ui32AllocatedDictSize = ui32MaxDictSize;
			m_lpldeDictionary = lpldeCopy;
		}

		m_ui16MinBits = 0;
		m_ui32MaxBits = _ui32MaxBits;
		m_ui32DictSize = 0;
		
		for ( uint32_t I = m_ui32AllocatedDictSize; I--; ) {
			m_lpldeDictionary[I].ui32Code = static_cast<uint32_t>(LSN_LZW_INVALID_CODE);
		}


		// Add the root codes.  Must be done in order.
		// We add 2 codes to the end to represent the dictionary-reset and stream-end codes.
		m_ui32RootCodes = _ui32RootCodes;
		_ui32RootCodes += 2;
		for ( uint32_t I = 0; I < _ui32RootCodes; ++I ) {
			uint32_t ui32Index = GetEntryIndex( I, I );
			AddEntry( ui32Index, I, I );
		}

		m_ui16StartingBits = m_ui16MinBits;

		return true;
	}

	/**
	 * Gets the index of an entry in the dictionary.  If the entry does not exist, the returned
	 *	index is where it should be added to the dictionary.
	 *
	 * \param _ui32Code The code of the entry to find.
	 * \param _ui32Char The allocator to use by this class.
	 * \return Returns false if allocation of the dictionary failed, or true otherwise.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetEntryIndex( uint32_t _ui32Code, uint32_t _ui32Char ) const {
		assert( m_lpldeDictionary );	// Of course a dictionary must have been made.
		uint32_t ui32RootIndex = ((_ui32Char << (m_ui32MaxBits - 8)) ^ _ui32Code);
		ui32RootIndex %= m_ui32AllocatedDictSize;
		uint32_t ui32Offset;
		if ( ui32RootIndex == 0 ) {
			ui32Offset = 1;
		}
		else {
			ui32Offset = ui32Offset = m_ui32AllocatedDictSize - ui32RootIndex;
		}

		while ( true ) {
			 
			if ( m_lpldeDictionary[ui32RootIndex].ui32Code == LSN_LZW_INVALID_CODE ) {
				// Found the place where the code should go in the dictionary.
				return ui32RootIndex;
			}

			// If it is a root code, continue searching.
			if ( m_lpldeDictionary[ui32RootIndex].ui32BaseCode != m_lpldeDictionary[ui32RootIndex].ui32Code ) {
				if ( m_lpldeDictionary[ui32RootIndex].ui32BaseCode == _ui32Code && m_lpldeDictionary[ui32RootIndex].ui8Char == _ui32Char ) {
					// Found a match.
					return ui32RootIndex;
				}
			}

			// Not an available entry and not a match.  Try the next index base off our hashing algorithm.
			ui32RootIndex -= ui32Offset;
			if ( ui32RootIndex >= m_ui32AllocatedDictSize ) {
				// Went below 0 (unsigned translation).
				ui32RootIndex += m_ui32AllocatedDictSize;
			}
		}
	}

	/**
	 * Gets an entry by index for read only.  The entry is not guaranteed to contain a valid code.
	 *
	 * \param _ui32Index Index of the entry to retrieve.
	 * \return Returns a constant reference to the entry at the given index.
	 */
	const CLzwEncodingDict::LSN_LZW_DICT_ENTRY & LSE_CALL CLzwEncodingDict::GetEntry( uint32_t _ui32Index ) const {
		assert( _ui32Index < m_ui32AllocatedDictSize );
		return m_lpldeDictionary[_ui32Index];
	}

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
	void LSE_CALL CLzwEncodingDict::AddEntry( uint32_t _ui32Index, uint32_t _ui32BaseCode, uint32_t _ui32Char ) {
		assert( m_lpldeDictionary[_ui32Index].ui32Code == LSN_LZW_INVALID_CODE );

		if ( m_ui32DictSize == static_cast<uint32_t>(1 << m_ui16MinBits) ) {
			++m_ui16MinBits;
		}

		m_lpldeDictionary[_ui32Index].ui32BaseCode = _ui32BaseCode;
		m_lpldeDictionary[_ui32Index].ui8Char = static_cast<uint8_t>(_ui32Char);
		m_lpldeDictionary[_ui32Index].ui32Code = m_ui32DictSize++;
	}

	/**
	 * Gets the size of the dictionary.
	 *
	 * \return Returns the number of entries in the dictionary.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::TotalEntries() const {
		return m_ui32DictSize;
	}

	/**
	 * Gets the maximum dictionary size.
	 *
	 * \return Returns the maximum dictionary size.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetMaxDictionarySize() const {
		return 1 << m_ui32MaxBits;
	}

	/**
	 * Gets the number of bits needed at the beginning of the dictionary (when the dictionary is set to
	 *	its root size).
	 *
	 * \return Returns the number of bits needed to access any code in the dictionary when the dictionary
	 *	is in its default state.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetStartingBits() const {
		return m_ui16StartingBits;
	}

	/**
	 * Gets the reset-dictionary code.
	 *
	 * \return Returns the reset-dictionary code.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetResetCode() const {
		return m_ui32RootCodes + 1;
	}

	/**
	 * Gets the end-stream code.
	 *
	 * \return Returns the end-stream code.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetEndStreamCode() const {
		return m_ui32RootCodes;
	}

	/**
	 * Gets the bits needed to read any code in the dictionary.
	 *
	 * \return Returns the bits needed to read any code in the dictionary.
	 */
	uint32_t LSE_CALL CLzwEncodingDict::GetBits() const {
		return m_ui16MinBits;
	}

}	// namespace lsc

#pragma warning( pop )
