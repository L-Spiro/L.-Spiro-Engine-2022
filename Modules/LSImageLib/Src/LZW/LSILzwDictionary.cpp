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

#include "LSILzwDictionary.h"
#include <cassert>

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsi {

	// == Various constructors.
	LSE_CALLCTOR CLzwDictionary::CLzwDictionary() :
		m_ui32MaxSize( 0 ),
		m_ui32ClearCode( 0 ),
		m_ui32EndOfInput( 0 ) {
	}
	LSE_CALLCTOR CLzwDictionary::~CLzwDictionary() {
		Reset();
	}
	
	// == Functions.
	/**
	 * Sets the allocator FIRST.
	 *
	 * \param _paAllocator The allocator to be used by this object.  Must be set before any
	 *	action on this object takes place.
	 */
	void LSE_CALL CLzwDictionary::SetAllocator( CAllocator * _paAllocator ) {
		Reset();
		m_vDecompDictionary.SetAllocator( _paAllocator );
	}
	
	/**
	 * Sets the initial dictionary where each entry is a root.
	 *
	 * \param _ui32Len Number of codes in the base dictionary.  No more than 256.
	 * \param _ui32MaxBits Maximum number of bits for any code.
	 *	since this class only performs decompression, this must always be true.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CLzwDictionary::InitializeDictionary( uint32_t _ui32Len, uint32_t _ui32MaxBits ) {
		Reset();
	
		// Determine the table size.
		m_ui32MaxSize = (1 << (m_ui32MaxBits = _ui32MaxBits));
		if ( m_ui32MaxSize < _ui32Len + 2 ) { return false; }
		
		if ( !Alloc( m_ui32MaxSize ) ) { return false; }

		// Each root node is just its own code + its own character code.
		for ( uint32_t I = 0; I < _ui32Len; ++I ) {
			LSI_LZW_DICT_ENTRY_DECOMPRESS ldedDecomp = {
				I,
				static_cast<uint8_t>(I)
			};
			m_vDecompDictionary.Push( ldedDecomp );
		}
		
		// Append the clear-code.
		{
			m_ui32ClearCode = _ui32Len++;
			LSI_LZW_DICT_ENTRY_DECOMPRESS ldedDecomp = {
				m_ui32ClearCode,
				static_cast<uint8_t>(m_ui32ClearCode)
			};
			m_vDecompDictionary.Push( ldedDecomp );
		}
		// Append the end-on-input code.
		{
			m_ui32EndOfInput = _ui32Len++;
			LSI_LZW_DICT_ENTRY_DECOMPRESS ldedDecomp = {
				m_ui32EndOfInput,
				static_cast<uint8_t>(m_ui32EndOfInput)
			};
			m_vDecompDictionary.Push( ldedDecomp );
		}
		
		m_ui32BitsNeeded = DeriveBitsNeeded();
		return true;
	}
	
	/**
	 * Is the given code the clear code?
	 *
	 * \param _lcCode The code to be tested against the clear code.
	 * \return Returns true if the code is the clear code.
	 */
	LSBOOL LSE_CALL CLzwDictionary::IsClearCode( LSI_LZW_CODE _lcCode ) const {
		return _lcCode == m_ui32ClearCode;
	}
	
	/**
	 * Is the given code the end-of-input code?
	 *
	 * \param _lcCode The code to be tested against the end-of-input code.
	 * \return Returns true if the code is the end-of-input code.
	 */
	LSBOOL LSE_CALL CLzwDictionary::IsEndOfInputCode( LSI_LZW_CODE _lcCode ) const {
		return _lcCode == m_ui32EndOfInput;
	}
	
	/**
	 * Gets the size of the dictionary.
	 *
	 * \return Returns the number of codes in the dictionary.
	 */
	uint32_t LSE_CALL CLzwDictionary::GetTotal() const {
		return m_vDecompDictionary.Length();
	}
	
	/**
	 * Gets an entry index by its code only.  Used when decompressing.
	 *
	 * \param _lcCode The code whose index is to be obtained.
	 * \return Returns the index of the code, which is always the code itself.
	 */
	uint32_t LSE_CALL CLzwDictionary::GetEntryIndex( LSI_LZW_CODE _lcCode ) const {
		return _lcCode;
	}

	/**
	 * Adds a code during decompression.
	 *
	 * \param _lcCode The code to be added.
	 * \param _ui8Char The character associated with this code.
	 */
	void LSE_CALL CLzwDictionary::AddCode( LSI_LZW_CODE _lcCode, uint8_t _ui8Char ) {
		if ( m_vDecompDictionary.Length() == m_ui32MaxSize ) { return; }

		uint32_t ui32Toal = m_vDecompDictionary.Length();
		LSI_LZW_DICT_ENTRY_DECOMPRESS ldedDecomp = {
			_lcCode,
			_ui8Char
		};
		m_vDecompDictionary.Push( ldedDecomp );

		
		if ( ui32Toal == static_cast<uint32_t>((1 << m_ui32BitsNeeded) - 1) ) {
			++m_ui32BitsNeeded;
		}
	}

	/**
	 * Gets a string from the dictionary.  Translates all codes to characters and returns the length of the string.
	 *	The string is returned in reverse order.
	 *
	 * \param _lcCode Code for which to get the string.
	 * \param _pui8String Holds the returned string in reverse order.
	 * \return Returns the number of characters in the returned string.
	 */
	uint32_t LSE_CALL CLzwDictionary::GetString( LSI_LZW_CODE _lcCode, uint8_t * _pui8String ) const {
		uint32_t ui32Len = 0;
		while ( true ) {
			if ( _lcCode >= m_vDecompDictionary.Length() ) {
				return ui32Len;
			}
			(*_pui8String++) = m_vDecompDictionary[_lcCode].ui8Char;
			++ui32Len;
			if ( m_vDecompDictionary[_lcCode].lcBaseCode == _lcCode ) {
				return ui32Len;
			}
			_lcCode = m_vDecompDictionary[_lcCode].lcBaseCode;
		}
	}
	
	/**
	 * Gets bits from a stream.  _ui64BitIndex is increased by _ui32Total upon return.
	 *
	 * \param _pui8Stream Stream of data from which to read the bits.
	 * \param _ui64BitIndex Bit index from which to start reading.
	 * \param _ui32Total Number of bits to get.
	 * \return Returns the read bits as an unsigned 32-bit integer.
	 */
	uint32_t LSE_CALL CLzwDictionary::GetBits( const uint8_t * _pui8Stream, uint64_t &_ui64BitIndex, uint32_t _ui32Total ) {
		uint32_t ui32Ret = 0;
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint32_t ui32Index = static_cast<uint32_t>(_ui64BitIndex >> 0x3);
			uint8_t ui8Byte = _pui8Stream[ui32Index];
			ui8Byte >>= (_ui64BitIndex & 0x7);
			
			//ui32Ret <<= 1;
			ui32Ret |= (ui8Byte & 0x1UL) << I;
			
			++_ui64BitIndex;
		}
		return ui32Ret;
	}
	
	/**
	 * Allocates the given amount of entries.
	 *
	 * \param _ui32Total Total number of entries to allocate.
	 * \return Returns true if there is enough RAM to complete the operation.
	 */
	LSBOOL LSE_CALL CLzwDictionary::Alloc( uint32_t _ui32Total ) {
		// If allocating 0 bytes, just reset the list.
		if ( !_ui32Total ) {
			Reset();
			return true;
		}
		
		if ( !m_vDecompDictionary.Allocate( _ui32Total ) ) {
			return false;
		}
		m_ui32BitsNeeded = DeriveBitsNeeded();
		return true;
	}
	
	/**
	 * Reset everything.
	 */
	void LSE_CALL CLzwDictionary::Reset() {
		m_vDecompDictionary.Reset();

		m_ui32MaxSize = m_ui32ClearCode = m_ui32EndOfInput = 0;
	}

	/**
	 * Derive the total number of bits needed to read any code in the dictionary.
	 *
	 * \return Returns the number of bits needed to read any code in the dictionary.
	 */
	uint32_t LSE_CALL CLzwDictionary::DeriveBitsNeeded() const {
		uint32_t ui32Bit = 31;
		uint32_t ui32Total = m_vDecompDictionary.Length() - 1;
		while ( ui32Bit ) {
			if ( ui32Total & (1 << ui32Bit) ) {
				return ui32Bit + 1;
			}
			--ui32Bit;
		}

		return 1;
	}

}	// namespace lsi

#pragma warning( pop )
