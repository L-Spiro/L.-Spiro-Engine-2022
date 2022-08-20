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
 * Description: A standard string class with an optional allocator.  Simply specializes the CStringBase
 *	template with a char.  In this way, this class provides an example of how to create a string of any
 *	base type (char LSUTFX, unsigned short, etc.)
 */

#include "LSTLString.h"


namespace lstl {

	// == Various constructors.
	LSE_CALLCTOR CString::CString( CAllocator * _paAllocator ) :
		Parent( _paAllocator ) {
	}
	LSE_CALLCTOR CString::CString( const char * _pcString, CAllocator * _paAllocator ) :
		Parent( _pcString, _paAllocator ) {
	}
	LSE_CALLCTOR CString::CString( const LSUTFX * _pwcString, CAllocator * _paAllocator ) :
		Parent( _paAllocator ) {
		// Allocate enough to hold the given string.
		register uint32_t ui32StrLen = CStd::WStrLen( _pwcString );
		Allocate( ui32StrLen + 1 );
		while ( (*_pwcString) ) {
			Append( static_cast<char>(*_pwcString++) );
		}
	}
	LSE_CALLCTOR CString::CString( const CString &_sString, CAllocator * _paAllocator ) :
		Parent( _sString, _paAllocator ) {
	}
	LSE_CALLCTOR CString::CString( const CStringBase<CString, char> &_sbStringBase, CAllocator * _paAllocator ) :
		Parent( _sbStringBase, _paAllocator ) {
	}

	// == Operators.
	/**
	 * Copy a string.
	 *
	 * \param _pcString
	 * \return Returns this string after the copy.
	 */
	CString & LSE_CALL CString::operator = ( const char * _pcString ) {
		Set( _pcString, StrLen( _pcString ) );
		return (*this);
	}

	/**
	 * Copy a string.
	 *
	 * \param _strString
	 * \return Returns this string after the copy.
	 */
	CString & LSE_CALL CString::operator = ( const CString &_strString ) {
		Set( _strString.CStr(), _strString.Length() );
		return (*this);
	}

	// == Functions.
	/**
	 * Converts an unsigned integer to a string.
	 *
	 * \param _ui64Value The value to convert to the string.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns true if there was enough memory to convert the value.
	 */
	LSBOOL LSE_CALL CString::FromUI64( uint64_t _ui64Value, uint32_t _ui32Radix ) {
		char szBuffer[32];
		return Set( CStd::UI64toA( _ui64Value, szBuffer, _ui32Radix ) );
	}

	/**
	 * Converts a signed integer to a string.
	 *
	 * \param _i64Value The value to convert to the string.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns true if there was enough memory to convert the value.
	 */
	LSBOOL LSE_CALL CString::FromI64( int64_t _i64Value, uint32_t _ui32Radix ) {
		char szBuffer[32];
		return Set( CStd::I64toA( _i64Value, szBuffer, _ui32Radix ) );
	}

	/**
	 * Converts a double to a string.
	 *
	 * \param _dValue The value to convert to the string.
	 * \param _bScientific If true, scientific notation is used to express the number.  A lower-case e is used
	 *	for the exponent.
	 * \return Returns true if there was enough memory to convert the value.
	 */
	LSBOOL LSE_CALL CString::FromDouble( double _dValue, LSBOOL _bScientific ) {
		char szBuffer[1024];	// Buffer large enough to hold any double value.
		if ( !Set( CStd::FtoA( szBuffer, LSE_ELEMENTS( szBuffer ) - 16, _dValue, 400, _bScientific ? 'e' : 'f' ) ) ) {
			return false;
		}

		// Remove trailing 0's.
		while ( GetLastChar() == '0' ) {
			RemLastChar();
		}
		if ( GetLastChar() == '.' ) {
			return Append( '0' );
		}
		return true;
	}

	/**
	 * Creates a string from a UTF input string.  Whether the input string is UTF-16 or UTF-32 depends on the platform.
	 *
	 * \param _pwcString The input string to be converted to a UTF-8 CString.
	 * \param _paAllocator Optional allocator.
	 * \return Returns the created CString or an empty string if there is not enough memory.
	 */
	CString LSE_CALL CString::CStringFromUtfX( const LSUTFX * _pwcString, CAllocator * _paAllocator ) {
		// Convert the UTF-X string to UTF-8, first by finding out how long the resulting string will be.
		uint32_t ui32Len = CStd::UtfXToUtf8Len( _pwcString );
		// Allocate characters temporarily.
		char * pcTemp = LSENEW char[ui32Len];
		if ( !pcTemp ) {
			return CString( _paAllocator );
		}

		CStd::UtfXToUtf8( _pwcString, pcTemp, ui32Len );

		CString sRet( pcTemp, _paAllocator );
		LSEDELETE [] pcTemp;
		return sRet;
	}

	/**
	 * Creates a new string from an unsigned 64-bit integer value.
	 *
	 * \param _ui64Value The value to convert to the string.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the string holding the converted representation of the number or empty if there was not
	 *	enough memory to perform the operation.
	 */
	CString LSE_CALL CString::CreateFromUI64( uint64_t _ui64Value, uint32_t _ui32Radix ) {
		CString sRet;
		if ( !sRet.FromUI64( _ui64Value, _ui32Radix ) ) { return CString(); }
		return sRet;
	}

	/**
	 * Creates a new string from a signed 64-bit integer value.
	 *
	 * \param _ui64Value The value to convert to the string.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the string holding the converted representation of the number or empty if there was not
	 *	enough memory to perform the operation.
	 */
	CString LSE_CALL CString::CreateFromI64( int64_t _i64Value, uint32_t _ui32Radix ) {
		CString sRet;
		if ( !sRet.FromI64( _i64Value, _ui32Radix ) ) { return CString(); }
		return sRet;
	}

	/**
	 * Creates a new string from a double value.
	 *
	 * \param _dValue The value to convert to the string.
	 * \param _bScientific If true, scientific notation is used to express the number.  A lower-case e is used
	 *	for the exponent.
	 * \return Returns the string holding the converted representation of the double number or empty if there was not
	 *	enough memory to perform the operation.
	 */
	CString LSE_CALL CString::CreateFromDouble( double _dValue, LSBOOL _bScientific ) {
		CString sRet;
		if ( !sRet.FromDouble( _dValue, _bScientific ) ) { return CString(); }
		return sRet;
	}

	/**
	 * Gets the length of a string the fast way.  Override the default single-character
	 *	counter.
	 *
	 * \param _pcValue The string whose length is to be obtained.
	 * \return Returns the length of the string.
	 */
	uint32_t LSE_CALL CString::StrLen( const char * _pcValue ) const {
		return CStd::StrLen( _pcValue );
	}

}	// namespace lstl
