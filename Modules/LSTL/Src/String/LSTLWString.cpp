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
 * Description: A standard wide-character string class with an optional allocator.  Simply specializes the
 *	CStringBase template with a wchar_t.  In this way, this class provides an example of how to create a
 *	string of any base type (char, wchar_t, unsigned short, etc.)
 */

#include "LSTLWString.h"


namespace lstl {

	// == Various constructors.
	LSE_CALLCTOR CWString::CWString( CAllocator * _paAllocator ) :
		Parent( _paAllocator ) {
	}
	LSE_CALLCTOR CWString::CWString( const char * _pcString, CAllocator * _paAllocator ) :
		Parent( _paAllocator ) {
		// Allocate enough to hold the given string.
		register uint32_t ui32StrLen = CStd::StrLen( _pcString );
		Allocate( ui32StrLen + 1 );
		while ( (*_pcString) ) {
			Append( static_cast<wchar_t>(*_pcString++) );
		}
	}
	LSE_CALLCTOR CWString::CWString( const wchar_t * _pwcString, CAllocator * _paAllocator ) :
		Parent( _pwcString, _paAllocator ) {
	}
	LSE_CALLCTOR CWString::CWString( const CWString &_sString, CAllocator * _paAllocator ) :
		Parent( _sString, _paAllocator ) {
	}
	LSE_CALLCTOR CWString::CWString( const CStringBase<CWString, wchar_t> &_sbStringBase, CAllocator * _paAllocator ) :
		Parent( _sbStringBase, _paAllocator ) {
	}

	// == Operators.
	/**
	 * Copy a string.
	 *
	 * \param _pwcString
	 * \return Returns this string after the copy.
	 */
	CWString & LSE_CALL CWString::operator = ( const wchar_t * _pwcString ) {
		Set( _pwcString, StrLen( _pwcString ) );
		return (*this);
	}

	/**
	 * Copy a string.
	 *
	 * \param _wstrString
	 * \return Returns this string after the copy.
	 */
	CWString & LSE_CALL CWString::operator = ( const CWString &_wstrString ) {
		Set( _wstrString.CStr(), _wstrString.Length() );
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
	LSBOOL LSE_CALL CWString::FromUI64( uint64_t _ui64Value, uint32_t _ui32Radix ) {
		wchar_t szBuffer[32];
		return Set( CStd::UI64toW( _ui64Value, szBuffer, _ui32Radix ) );
	}

	/**
	 * Converts a signed integer to a string.
	 *
	 * \param _i64Value The value to convert to the string.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns true if there was enough memory to convert the value.
	 */
	LSBOOL LSE_CALL CWString::FromI64( int64_t _i64Value, uint32_t _ui32Radix ) {
		wchar_t szBuffer[32];
		return Set( CStd::I64toW( static_cast<int64_t>(_i64Value), szBuffer, _ui32Radix ) );
	}

	/**
	 * Converts a double to a string.
	 *
	 * \param _dValue The value to convert to the string.
	 * \param _bScientific If true, scientific notation is used to express the number.  A lower-case e is used
	 *	for the exponent.
	 * \return Returns true if there was enough memory to convert the value.
	 */
	LSBOOL LSE_CALL CWString::FromDouble( double _dValue, LSBOOL _bScientific ) {
		wchar_t szBuffer[1024+16+8];	// Buffer large enough to hold any double value.
		if ( !Set( CStd::FtoW( szBuffer, LSE_ELEMENTS( szBuffer ), _dValue, 1024, _bScientific ? 'e' : 'f' ) ) ) {
			return false;
		}
		// Remove trailing 0's.
		while ( GetLastChar() == L'0' ) {
			RemLastChar();
		}
		if ( GetLastChar() == L'.' ) {
			return Append( L'0' );
		}
		return true;
	}

	/**
	 * Create from a UTF-8 string.
	 *
	 * \param _pu8Src The string from which to create a UTFX CWString() object.
	 * \return Returns the created string.
	 */
	CWString LSE_CALL CWString::FromUtf8( const LSUTF8 * _pu8Src ) {
		CWString wReturn;
		uint32_t ui32Len = CStd::Utf8StrLenAsUtfX( _pu8Src ) / sizeof( wchar_t );
		if ( !wReturn.Resize( ui32Len ) ) { return wReturn; }
		CStd::Utf8ToUtfX( &wReturn[0], _pu8Src, ui32Len );
		return wReturn;
	}

}	// namespace lstl
