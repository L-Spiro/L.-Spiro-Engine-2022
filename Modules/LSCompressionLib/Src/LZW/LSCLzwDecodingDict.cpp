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

#include "LSCLzwDecodingDict.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CLzwDecodingDict::CLzwDecodingDict() :
		m_lpldeDictionary( NULL ),
		m_ui32Total( 0 ),
		m_ui32Alloc( 0 ),
		m_ui32RootLen( 0 ),
		m_ui32MaxSize( 0 ),
		m_ui32MaxBits( 0 ),
		m_ui32BitsNeeded( 0 ) {
	}
	LSE_CALLCTOR CLzwDecodingDict::~CLzwDecodingDict() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.  Deallocates all resources.
	 */
	void  LSE_CALL CLzwDecodingDict::Reset() {
		if ( m_lpldeDictionary ) {
			m_aAllocator.Free( m_lpldeDictionary );
		}

		m_ui32Total = m_ui32Alloc = m_ui32MaxSize = m_ui32BitsNeeded = 0;
		m_lpldeDictionary = NULL;
	}

	/**
	 * Sets the allocator.  If different from the current allocator, the dictionary is reset.
	 *
	 * \param _psaAllocator The allocator to use by this class.
	 */
	void LSE_CALL CLzwDecodingDict::SetAllocator( CStdAllocator * _psaAllocator ) {
		if ( m_aAllocator.GetAllocator() != _psaAllocator ) {
			Reset();
			m_aAllocator.SetAllocator( _psaAllocator );
		}
	}

	/**
	 * Sets the initial dictionary where each entry is a root.
	 *
	 * \param _ui32Len Number of root entries in the dictionary.
	 * \param _ui32MaxBits Maximum number of bits in any entry in the dictionary.
	 */
	void LSE_CALL CLzwDecodingDict::InitializeDictionary( uint32_t _ui32Len, uint32_t _ui32MaxBits ) {
		Reset();
		m_ui32MaxBits = _ui32MaxBits;
		m_ui32RootLen = _ui32Len;
		Alloc();
		m_ui32MaxSize = (1 << (m_ui32MaxBits));
		if ( m_ui32MaxSize < _ui32Len + 2 ) {
			CStd::DebugPrintA( "CLzwDecodingDict::InitializeDictionary(): Maximum bits is too small for the given root length.\r\n" );
		}
	}

	/**
	 * Is the given code the clear code?
	 *
	 * \param _lcCode The code to be tested against the clear code.
	 * \return Returns true if the code is the clear code.
	 */
	LSBOOL LSE_CALL CLzwDecodingDict::IsClearCode( LSN_LZW_CODE _lcCode ) const {
		return _lcCode == m_ui32RootLen + 1;
	}
	
	/**
	 * Is the given code the end-of-input code?
	 *
	 * \param _lcCode The code to be tested against the end-of-input code.
	 * \return Returns true if the code is the end-of-input code.
	 */
	LSBOOL LSE_CALL CLzwDecodingDict::IsEndOfInputCode( LSN_LZW_CODE _lcCode ) const {
		return _lcCode == m_ui32RootLen;
	}

	/**
	 * Gets the size of the dictionary.
	 *
	 * \return Returns the number of codes in the dictionary.
	 */
	uint32_t LSE_CALL CLzwDecodingDict::GetTotal() const {
		return m_ui32Total;
	}

	/**
	 * Adds a code to the dictionary.  Fails if the dictionary is full.
	 *
	 * \param _lcCode The code to be added.
	 * \param _ui8Char The character associated with this code.
	 */
	LSBOOL LSE_CALL CLzwDecodingDict::AddCode( LSN_LZW_CODE _lcCode, uint8_t _ui8Char ) {
		if ( m_ui32Total == m_ui32MaxSize ) { return false; }
		m_lpldeDictionary[m_ui32Total].ui32BaseCode = _lcCode;
		m_lpldeDictionary[m_ui32Total].ui8Char = _ui8Char;

		
		if ( m_ui32Total >= static_cast<uint32_t>((1 << m_ui32BitsNeeded) - 1) ) {
			++m_ui32BitsNeeded;
		}
		++m_ui32Total;
		return true;
	}

	/**
	 * Gets a string from the dictionary.  Translates all codes to characters and returns the length
	 *	of the string. The string is returned in reverse order.
	 *
	 * \param _lcCode The code to translate into a byte string.
	 * \param _sString Upone return, this string contains the returned bytes.
	 * \return Returns true if the string was extracted successfully.  If any unknown codes are
	 *	encountered, false is returned.
	 */
	LSBOOL LSE_CALL CLzwDecodingDict::GetString( LSN_LZW_CODE _lcCode, CString &_sString ) const {
		_sString.ResetNoDealloc();
		while ( true ) {
			if ( _lcCode >= m_ui32Total ) {
				return false;
			}
			_sString += static_cast<char>(m_lpldeDictionary[_lcCode].ui8Char);
			if ( m_lpldeDictionary[_lcCode].ui32BaseCode == _lcCode ) {
				return true;
			}
			_lcCode = m_lpldeDictionary[_lcCode].ui32BaseCode;
		}
	}

	/**
	 * Allocates the full set of codes and initialize the roots.
	 *
	 * \return Returns false if allocation fails or if m_ui32RootLen is 0.
	 */
	LSBOOL LSE_CALL CLzwDecodingDict::Alloc() {
		if ( m_ui32RootLen == 0 ) { return false; }

		Reset();
		uint32_t ui32Len = 1 << m_ui32MaxBits;
		m_lpldeDictionary = static_cast<LPLSN_LZW_DICT_ENTRY>(m_aAllocator.Alloc( sizeof( (*m_lpldeDictionary) ) * ui32Len ));
		if ( !m_lpldeDictionary ) { return false; }
		m_ui32Alloc = ui32Len;

		// Initialize the roots.
		uint32_t ui32Total = m_ui32RootLen + 2;
		m_ui32MaxSize = ui32Total;
		m_ui32Total = 0;
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			AddCode( I, static_cast<uint8_t>(I) );
		}

		//m_ui32BitsNeeded = DeriveBitsNeeded();
		return true;
	}

	/**
	 * Derive the total number of bits needed to read any code in the dictionary.
	 *
	 * \return Returns the number of bits needed for a code in the stream given the current
	 *	size of the dictionary.
	 */
	uint32_t LSE_CALL CLzwDecodingDict::DeriveBitsNeeded() const {
		uint32_t ui32Bit = 31UL;
		uint32_t ui32Total = m_ui32Total - 1;
		while ( ui32Bit ) {
			if ( ui32Total & (1 << ui32Bit) ) {
				return ui32Bit + 1;
			}
			--ui32Bit;
		}

		return 1;
	}

}	// namespace lsc

#pragma warning( pop )
