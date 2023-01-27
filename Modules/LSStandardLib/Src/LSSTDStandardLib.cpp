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
 * Description: Standard routines and types.  This library is just a bunch of common helpful routines normally
 *	found in the C-runtime (etc.)
 */

#include "LSSTDStandardLib.h"
#include "LargeInteger/LSSTDLargeInteger.h"
#include <cstdarg>
#include <cstdio>
#ifdef __LSE_CRT_MEMFUNCS__
#include <cstring>
#endif	// #ifdef __LSE_CRT_MEMFUNCS__
#ifdef __LSE_STD_FLOAT__
#include <cstdlib>
#endif	// #ifdef __LSE_STD_FLOAT__

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

// warning C4701: potentially uninitialized local variable 'uValue' used
#pragma warning( disable : 4701 )

//  warning C4309: 'static_cast' : truncation of constant value
#pragma warning( disable : 4309 )

//  warning C4293: '<<': shift count negative or too big, undefined behavior
#pragma warning( disable : 4293 )

// Have some OS-specific files to include here.
#ifdef LSE_WINDOWS
// Windows.
// Set the Windows version.
#ifndef WINVER
#define WINVER         0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0501
#endif

#define NOMINMAX				// We do not want min/max as macros.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
// Non-Windows (use Posix).
#include <sched.h>				// For ::sched_yield().
#include <stdarg.h>				// for va_list (etc.)
#include <time.h>				// For ::nanosleep().
#include <wchar.h>				// for ::vswprintf (etc.)

#endif	// #ifdef LSE_WINDOWS


namespace lsstd {
	
	// == Functions.
	/**
	 * Compare a wide-character string against another lexicographically.
	 *
	 * \param _pwcLeft NULL-terminated string to compare.
	 * \param _pwcRight NULL-terminated string to compare.
	 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
	 *	_pwcLeft is greater than _pwcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::WStrCmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight ) {
		for ( uint32_t I = 0; true; ++I ) {
			LSUTFX wcLeft = _pwcLeft[I];
			int32_t i32Dif = wcLeft - _pwcRight[I];
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( wcLeft == L'\0' ) { return 0; }
		}
	}
	
	/**
	 * Compare a wide-character string case-insensitively against another lexicographically.
	 *
	 * \param _pwcLeft NULL-terminated string to compare.
	 * \param _pwcRight NULL-terminated string to compare.
	 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
	 *	_pwcLeft is greater than _pwcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::WStrICmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight ) {
		for ( uint32_t I = 0; true; ++I ) {
			LSUTFX wcLeft = ToLower( _pwcLeft[I] );
			int32_t i32Dif = wcLeft - ToLower( _pwcRight[I] );
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( wcLeft == L'\0' ) { return 0; }
		}
	}
	
	/**
	 * Compare a wide-character string against another lexicographically.  Compares at most _ui32Max characters.
	 *
	 * \param _pwcLeft String to compare.
	 * \param _pwcRight String to compare.
	 * \param _ui32Max Maximum number of characters to compare.
	 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
	 *	_pwcLeft is greater than _pwcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::WStrNCmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max ) {
		for ( uint32_t I = 0; I < _ui32Max; ++I ) {
			LSUTFX wcLeft = _pwcLeft[I];
			int32_t i32Dif = wcLeft - _pwcRight[I];
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( wcLeft == L'\0' ) { return 0; }
		}
		return 0;
	}

	/**
	 * Compare a wide-character string case-insensitively against another lexicographically.  Compares at most _ui32Max characters.
	 *
	 * \param _pwcLeft String to compare.
	 * \param _pwcRight String to compare.
	 * \param _ui32Max Maximum number of characters to compare.
	 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
	 *	_pwcLeft is greater than _pwcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::WStrNICmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max ) {
		for ( uint32_t I = 0; I < _ui32Max; ++I ) {
			LSUTFX wcLeft = ToLower( _pwcLeft[I] );
			int32_t i32Dif = wcLeft - ToLower( _pwcRight[I] );
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( wcLeft == L'\0' ) { return 0; }
		}
		return 0;
	}

	/**
	 * Copy one wide-character string to another.
	 *
	 * \param _pwcDst Destination string.
	 * \param _pwcSrc Source string.
	 * \return Returns the destination string, which must be long enough to contain the copy,
	 *	including the terminating NULL.
	 */
	LSUTFX * LSE_CALL CStd::WStrCpy( LSUTFX * _pwcDst, const LSUTFX * _pwcSrc ) {
		LSUTFX * pwcCopy = _pwcDst;
		while ( (*_pwcSrc) ) {
			(*pwcCopy++) = (*_pwcSrc++);
		}
		(*pwcCopy) = L'\0';
		return _pwcDst;
	}

	/**
	 * Copy one wide-character string to another.  Copies at most _ui32Max characters.
	 *
	 * \param _pwcDst Destination string.
	 * \param _pwcSrc Source string.
	 * \param _ui32Max Maximum number of characters to copy.
	 * \return Returns the destination string, which must be long enough to contain the copy,
	 *	including the terminating NULL.
	 */
	LSUTFX * LSE_CALL CStd::WStrNCpy( LSUTFX * _pwcDst, const LSUTFX * _pwcSrc, uint32_t _ui32Max ) {
		if ( !_ui32Max ) { return NULL; }
		LSUTFX * pwcCopy = _pwcDst;
		while ( (*_pwcSrc) && _ui32Max-- != 0 ) {
			(*pwcCopy++) = (*_pwcSrc++);
		}
		if ( _ui32Max ) {
			// Can append.
			(*pwcCopy) = L'\0';
		}
		else {
			// Truncate the last character.
			(*--pwcCopy) = L'\0';
		}
		return _pwcDst;
	}

	/**
	 * Gets the length of a wide-character string.
	 *
	 * \param _pwcStr NULL-terminated string whose length is to be obtained.
	 * \return Returns the length of the string.
	 */
	uint32_t LSE_CALL CStd::WStrLen( const LSUTFX * _pwcStr ) {
		// Size of LSUTFX changes per compiler so no fancy tricks are likely
		//	to work here, unlike with StrLen().
		const LSUTFX * pwcCopy = _pwcStr;
		for ( ; (*pwcCopy++); ) {}
		return static_cast<uint32_t>(pwcCopy - _pwcStr - 1);
	}

	/**
	 * Combine 2 wide-character strings.
	 *
	 * \param _pwcLeft NULL-terminated destination string.
	 * \param _pwcRight NULL-terminated source string.
	 * \return Returns the pointer to the first parameter.
	 */
	LSUTFX * LSE_CALL CStd::WStrCat( LSUTFX * _pwcLeft, const LSUTFX * _pwcRight ) {
		WStrCpy( _pwcLeft + WStrLen( _pwcLeft ), _pwcRight );
		return _pwcLeft;
	}

	/**
	 * Combine 2 wide-character strings.
	 *
	 * \param _pwcLeft NULL-terminated destination string.
	 * \param _pwcRight NULL-terminated source string.
	 * \param _ui32Max Maximum number of characters to append.
	 * \return Returns the pointer to the first parameter.
	 */
	LSUTFX * LSE_CALL CStd::WStrNCat( LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max ) {
		WStrNCpy( _pwcLeft + WStrLen( _pwcLeft ), _pwcRight, _ui32Max );
		return _pwcLeft;
	}

	/**
	 * Returns the index of the first occurrence of a character in a string that belongs to a set of characters.
	 *
	 * \param _pwcString NULL-terminated searched string.
	 * \param _pwcCharSet NULL-terminated character set.
	 * \return Returns an integer value specifying the length of the initial segment of _pwcString that consists entirely
	 *	of characters not in _pwcCharSet.  If _pwcString begins with a character that is in _pwcCharSet, the function returns
	 *	0.  No return value is reserved to indicate an error.
	 */
	uint32_t LSE_CALL CStd::WStrCSpn( const LSUTFX * _pwcString, const LSUTFX * _pwcCharSet ) {
		uint32_t ui32Ret = 0;
		// For every character in the string, including the NULL.
		for ( ; true; ++ui32Ret ) {
			// For every character in the character set, including the NULL.
			for ( const LSUTFX * pcSet = _pwcCharSet; true; ++pcSet ) {
				// Because the NULL character from the character set and the NULL character from
				//	the string are included in the search, the outer loop is guaranteed to terminate.
				if ( (*pcSet) == _pwcString[ui32Ret] ) { return ui32Ret; }

				// Perform the NULL check after the character check.
				if ( !(*pcSet) ) { break; }
			}
		}
		return ui32Ret;
	}

	/**
	 * Compare a string against another lexicographically.
	 *
	 * \param _pcLeft NULL-terminated string to compare.
	 * \param _pcRight NULL-terminated string to compare.
	 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
	 *	_pcLeft is greater than _pcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::StrCmp( const char * _pcLeft, const char * _pcRight ) {
		for ( uint32_t I = 0; true; ++I ) {
			char cLeft = _pcLeft[I];
			int32_t i32Dif = cLeft - _pcRight[I];
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( cLeft == '\0' ) { return 0; }
		}
	}

	/**
	 * Compare a string case-insensitively against another lexicographically.
	 *
	 * \param _pcLeft NULL-terminated string to compare.
	 * \param _pcRight NULL-terminated string to compare.
	 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
	 *	_pcLeft is greater than _pcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::StrICmp( const char * _pcLeft, const char * _pcRight ) {
		for ( uint32_t I = 0; true; ++I ) {
			char cLeft = ToLower( _pcLeft[I] );
			int32_t i32Dif = cLeft - ToLower( _pcRight[I] );
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( cLeft == '\0' ) { return 0; }
		}
	}

	/**
	 * Compare a string against another lexicographically.  Compares at most _ui32Max characters.
	 *
	 * \param _pcLeft NULL-terminated string to compare.
	 * \param _pcRight NULL-terminated string to compare.
	 * \param _ui32Max Maximum number of characters to compare.
	 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
	 *	_pcLeft is greater than _pcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::StrNCmp( const char * _pcLeft, const char * _pcRight, uint32_t _ui32Max ) {
		for ( uint32_t I = 0; I < _ui32Max; ++I ) {
			char cLeft = _pcLeft[I];
			int32_t i32Dif = cLeft - _pcRight[I];
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( cLeft == '\0' ) { return 0; }
		}
		return 0;
	}

	/**
	 * Compare a string case-insensitively against another lexicographically.  Compares at most _ui32Max characters.
	 *
	 * \param _pcLeft NULL-terminated string to compare.
	 * \param _pcRight NULL-terminated string to compare.
	 * \param _ui32Max Maximum number of characters to compare.
	 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
	 *	_pcLeft is greater than _pcRight, lexicographically.
	 */
	int32_t LSE_CALL CStd::StrNICmp( const char * _pcLeft, const char * _pcRight, uint32_t _ui32Max ) {
		for ( uint32_t I = 0; I < _ui32Max; ++I ) {
			char cLeft = ToLower( _pcLeft[I] );
			int32_t i32Dif = cLeft - ToLower( _pcRight[I] );
			if ( i32Dif != 0 ) { return i32Dif; }
			
			if ( cLeft == '\0' ) { return 0; }
		}
		return 0;
	}

	/**
	 * Copy one string to another.
	 *
	 * \param _pcDst Destination string.
	 * \param _pcSrc Source string.
	 * \return Returns the destination string, which must be long enough to contain the copy,
	 *	including the terminating NULL.
	 */
	char * LSE_CALL CStd::StrCpy( char * _pcDst, const char * _pcSrc ) {
		char * pcCopy = _pcDst;
		while ( (*_pcSrc) ) {
			(*pcCopy++) = (*_pcSrc++);
		}
		(*pcCopy) = '\0';
		return _pcDst;
	}

	/**
	 * Copy one string to another.  Copies at most _ui32Max characters.
	 *
	 * \param _pcDst Destination string.
	 * \param _pcSrc Source string.
	 * \param _ui32Max Maximum number of characters to copy.
	 * \return Returns the destination string, which must be long enough to contain the copy,
	 *	including the terminating NULL.
	 */
	char * LSE_CALL CStd::StrNCpy( char * _pcDst, const char * _pcSrc, uint32_t _ui32Max ) {
		if ( !_ui32Max ) { return NULL; }
		char * pcCopy = _pcDst;
		while ( (*_pcSrc) && _ui32Max-- ) {
			(*pcCopy++) = (*_pcSrc++);
		}
		if ( _ui32Max ) {
			// Can append.
			(*pcCopy) = '\0';
		}
		else {
			// Truncate the last character.
			(*--pcCopy) = '\0';
		}
		return _pcDst;
	}

	/**
	 * Gets the length of a string.
	 *
	 * \param _pcStr NULL-terminated string whose length is to be obtained.
	 * \return Returns the length of the string.
	 */
	uint32_t LSE_CALL CStd::StrLen( const char * _pcStr ) {
#ifdef __LSE_CRT_MEMFUNCS__
		return static_cast<uint32_t>(::strlen( _pcStr ));
#else
		// Once we get to an X-byte boundary we can do full X-byte comparisons.
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		const char * pTemp = _pcStr;
		uintptr_t uiptrA = reinterpret_cast<uintptr_t>(_pcStr);
		uintptr_t uiptrC = ((uiptrA + (sizeof( uintptr_t ) - 1)) & (~(sizeof( uintptr_t ) - 1))) - uiptrA;
		uintptr_t uiptrS = uiptrC;
		
		for ( ; static_cast<int32_t>(uiptrC) > 0; --uiptrC ) {
			if ( !(*pTemp++) ) { return static_cast<uint32_t>(pTemp - _pcStr - 1); }
		}
		
		// Now pTemp is aligned on an X-byte boundary.
		// Cast to uintptr_t * to allow X-byte-stepping over it.
		const uintptr_t * puiptrAp = reinterpret_cast<const uintptr_t *>(pTemp);
		uintptr_t uiptrD = reinterpret_cast<uintptr_t>(pTemp) + sizeof( uintptr_t );
		while ( true ) {
			uintptr_t uiptrI = (*puiptrAp++);
			uiptrC = (uiptrI - static_cast<uintptr_t>(0x0101010101010101ULL)) & (~uiptrI & static_cast<uintptr_t>(0x8080808080808080ULL));
			if ( uiptrC ) {
				break;
			}
		}
		
		while ( !(uiptrC & 0x80) ) {
			uiptrC >>= 8;
			++uiptrS;
		}
		
		return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(puiptrAp) - uiptrD + uiptrS);
#endif	// #ifdef __LSE_CRT_MEMFUNCS__
	}

	/**
	 * Combine 2 strings.
	 *
	 * \param _pcLeft NULL-terminated destination string.
	 * \param _pcRight NULL-terminated source string.
	 * \return Returns the pointer to the first parameter.
	 */
	char * LSE_CALL CStd::StrCat( char * _pcLeft, const char * _pcRight ) {
		StrCpy( _pcLeft + StrLen( _pcLeft ), _pcRight );
		return _pcLeft;
	}

	/**
	 * Combine 2 strings.
	 *
	 * \param _pcLeft NULL-terminated destination string.
	 * \param _pcRight NULL-terminated source string.
	 * \param _ui32Max Maximum number of characters to append.
	 * \return Returns the pointer to the first parameter.
	 */
	char * LSE_CALL CStd::StrNCat( char * _pcLeft, const char * _pcRight, uint32_t _ui32Max ) {
		StrNCpy( _pcLeft + StrLen( _pcLeft ), _pcRight, _ui32Max );
		return _pcLeft;
	}

	/**
	 * Returns the index of the first occurrence of a character in a string that belongs to a set of characters.
	 *
	 * \param _pcString NULL-terminated searched string.
	 * \param _pcCharSet NULL-terminated character set.
	 * \return Returns an integer value specifying the length of the initial segment of _pcString that consists entirely
	 *	of characters not in _pcCharSet.  If _pcString begins with a character that is in _pcCharSet, the function returns
	 *	0.  No return value is reserved to indicate an error.
	 */
	uint32_t LSE_CALL CStd::StrCSpn( const char * _pcString, const char * _pcCharSet ) {
		uint32_t ui32Ret = 0;
		// For every character in the string, including the NULL.
		for ( ; true; ++ui32Ret ) {
			// For every character in the character set, including the NULL.
			for ( const char * pcSet = _pcCharSet; true; ++pcSet ) {
				// Because the NULL character from the character set and the NULL character from
				//	the string are included in the search, the outer loop is guaranteed to terminate.
				if ( (*pcSet) == _pcString[ui32Ret] ) { return ui32Ret; }

				// Perform the NULL check after the character check.
				if ( !(*pcSet) ) { break; }
			}
		}
		return ui32Ret;
	}

	/**
	 * Duplicate a wide-character string.
	 *
	 * \param _pwcSrc The string to duplicate.
	 * \return Returns the duplicated string, which must be freed with WStrDel() when no longer needed.
	 */
	LSUTFX * LSE_CALL CStd::WStrDup( const LSUTFX * _pwcSrc ) {
		uint32_t ui32Len = CStd::WStrLen( _pwcSrc ) + 1;
		LSUTFX * pwcNew = new LSUTFX[ui32Len];
		if ( !pwcNew ) { return NULL; }
		CStd::WStrCpy( pwcNew, _pwcSrc );
		return pwcNew;
	}

	/**
	 * Delete a string created by a previous call to WStrDup().
	 *
	 * \param _pwcStr The string to delete.
	 */
	void LSE_CALL CStd::WStrDel( LSUTFX * _pwcStr ) {
		delete [] _pwcStr;
	}

	/**
	 * Duplicate a character string.
	 *
	 * \param _pcSrc The string to duplicate.
	 * \return Returns the duplicated string, which must be freed with StrDel() when no longer needed.
	 */
	char * LSE_CALL CStd::StrDup( const char * _pcSrc ) {
		uint32_t ui32Len = CStd::StrLen( _pcSrc ) + 1;
		char * pcNew = new char[ui32Len];
		if ( !pcNew ) { return NULL; }
		CStd::StrCpy( pcNew, _pcSrc );
		return pcNew;
	}

	/**
	 * Delete a string created by a previous call to StrDup().
	 *
	 * \param _pcStr The string to delete.
	 */
	void LSE_CALL CStd::StrDel( char * _pcStr ) {
		delete [] _pcStr;
	}
	
	/**
	 * Gets the lower-case value of an alpha character (wide).
	 *
	 * \param _wcChar The character to be converted to lower case.
	 * \return The lower-case value of the input character.
	 */
	LSUTFX LSE_CALL CStd::ToLower( LSUTFX _wcChar ) {
		if ( _wcChar >= static_cast<LSUTFX>(L'A') && _wcChar <=  static_cast<LSUTFX>(L'Z') ) { return static_cast<LSUTFX>(_wcChar | ~0xFFFFFFDF); }
		return _wcChar;
	}
	
	/**
	 * Gets the upper-case value of an alpha character (wide).
	 *
	 * \param _wcChar The character to be converted to upper case.
	 * \return The upper-case value of the input character.
	 */
	LSUTFX LSE_CALL CStd::ToUpper( LSUTFX _wcChar ) {
		if ( _wcChar >= static_cast<LSUTFX>(L'a') && _wcChar <= static_cast<LSUTFX>(L'z') ) { return static_cast<LSUTFX>(_wcChar & 0xFFFFFFDF); }
		return _wcChar;
	}
	
	/**
	 * Gets the lower-case value of an alpha character.
	 *
	 * \param _cChar The character to be converted to lower case.
	 * \return The lower-case value of the input character.
	 */
	char LSE_CALL CStd::ToLower( char _cChar ) {
		if ( _cChar >= 'A' && _cChar <= 'Z' ) { return static_cast<char>(_cChar | ~0xDF); }
		return _cChar;
	}
	
	/**
	 * Gets the upper-case value of an alpha character.
	 *
	 * \param _cChar The character to be converted to upper case.
	 * \return The upper-case value of the input character.
	 */
	char LSE_CALL CStd::ToUpper( char _cChar ) {
		if ( _cChar >= 'a' && _cChar <= 'z' ) { return _cChar & static_cast<char>(0xDF); }
		return _cChar;
	}

	/**
	 * Converts a wide-character string to a signed 32-bit integer.
	 *
	 * \param _pwcString String to be converted.
	 * \return Returns the int32_t value produced by interpreting the input characters as a number.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int32_t LSE_CALL CStd::WtoI32( const LSUTFX * _pwcString ) {
		if ( !_pwcString ) { return 0; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pwcString) ) ) { ++_pwcString; }

		LSBOOL bNegate = _pwcString[0] == L'-' ? true : false;
		if ( _pwcString[0] == L'+' || _pwcString[0] == L'-' ) {
			++_pwcString;
		}
		int32_t i32Ret = 0;
		LSUTFX wcThis = (*_pwcString);
		while ( wcThis ) {
			if ( wcThis >= L'0' && wcThis <= L'9' ) {
				i32Ret *= 10;
				i32Ret += wcThis - L'0';
				wcThis = (*++_pwcString);
				continue;
			}
			// Non-numeric character encountered.  Stop.
			break;
		}
		return bNegate ? -i32Ret : i32Ret;
	}

	/**
	 * Converts a string to a signed 32-bit integer.
	 *
	 * \param _pcString String to be converted.
	 * \return Returns the int32_t value produced by interpreting the input characters as a number.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int32_t LSE_CALL CStd::AtoI32( const char * _pcString ) {
		if ( !_pcString ) { return 0; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pcString) ) ) { ++_pcString; }

		LSBOOL bNegate = _pcString[0] == '-' ? true : false;
		if ( _pcString[0] == '+' || _pcString[0] == '-' ) {
			++_pcString;
		}
		int32_t i32Ret = 0;
		char cThis = (*_pcString);
		while ( cThis ) {
			if ( cThis >= '0' && cThis <= '9' ) {
				i32Ret *= 10;
				i32Ret += cThis - '0';
				cThis = (*++_pcString);
				continue;
			}
			// Non-numeric character encountered.  Stop.
			break;
		}
		return bNegate ? -i32Ret : i32Ret;
	}

	/**
	 * Converts a wide-character string to a signed 64-bit integer.
	 *
	 * \param _pwcString String to be converted.
	 * \return Returns the int64_t value produced by interpreting the input characters as a number.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int64_t LSE_CALL CStd::WtoI64( const LSUTFX * _pwcString ) {
		if ( !_pwcString ) { return 0ULL; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pwcString) ) ) { ++_pwcString; }

		LSBOOL bNegate = _pwcString[0] == L'-' ? true : false;
		if ( _pwcString[0] == L'+' || _pwcString[0] == L'-' ) {
			++_pwcString;
		}
		int64_t i64Ret = 0ULL;
		LSUTFX wcThis = (*_pwcString);
		while ( wcThis ) {
			if ( wcThis >= L'0' && wcThis <= L'9' ) {
				i64Ret *= 10;
				i64Ret += wcThis - L'0';
				wcThis = (*++_pwcString);
				continue;
			}
			// Non-numeric character encountered.  Stop.
			break;
		}
		return bNegate ? -i64Ret : i64Ret;
	}

	/**
	 * Converts a string to a signed 64-bit integer.
	 *
	 * \param _pcString String to be converted.
	 * \return Returns the int64_t value produced by interpreting the input characters as a number.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int64_t LSE_CALL CStd::AtoI64( const char * _pcString ) {
		if ( !_pcString ) { return 0ULL; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pcString) ) ) { ++_pcString; }

		LSBOOL bNegate = _pcString[0] == '-' ? true : false;
		if ( _pcString[0] == '+' || _pcString[0] == '-' ) {
			++_pcString;
		}
		int64_t i64Ret = 0ULL;
		char cThis = (*_pcString);
		while ( cThis ) {
			if ( cThis >= '0' && cThis <= '9' ) {
				i64Ret *= 10;
				i64Ret += cThis - '0';
				cThis = (*++_pcString);
				continue;
			}
			// Non-numeric character encountered.  Stop.
			break;
		}
		return bNegate ? -i64Ret : i64Ret;
	}

	/**
	 * Converts a hexadecimal string to a signed 64-bit integer.
	 *
	 * \param _pwcString String to be converted.
	 * \return Returns the int64_t value produced by interpreting the input characters as a number in hexadecimal format.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int64_t LSE_CALL CStd::WHtoI64( const LSUTFX * _pwcString ) {
		if ( !_pwcString ) { return 0ULL; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pwcString) ) ) { ++_pwcString; }

		LSBOOL bNegate = _pwcString[0] == L'-' ? true : false;
		if ( _pwcString[0] == L'+' || _pwcString[0] == L'-' ) {
			++_pwcString;
		}
		int64_t i64Ret = 0ULL;
		LSUTFX wcThis = (*_pwcString);
		while ( wcThis ) {
			if ( wcThis >= L'0' && wcThis <= L'9' ) {
				i64Ret *= 10;
				i64Ret += wcThis - L'0';
				wcThis = (*++_pwcString);
				continue;
			}
			else if ( wcThis >= L'A' && wcThis <= L'F' ) {
				i64Ret *= 16;
				i64Ret += wcThis - L'A' + 10;
				wcThis = (*++_pwcString);
				continue;
			}
			else if ( wcThis >= L'a' && wcThis <= L'f' ) {
				i64Ret *= 16;
				i64Ret += wcThis - L'a' + 10;
				wcThis = (*++_pwcString);
				continue;
			}
			// Non-numeric character encountered.  Stop.
			break;
		}
		return bNegate ? -i64Ret : i64Ret;
	}

	/**
	 * Converts a hexadecimal string to a signed 64-bit integer.
	 *
	 * \param _pcString String to be converted.
	 * \return Returns the int64_t value produced by interpreting the input characters as a number in hexadecimal format.
	 *	If the input cannot be converted to a value of that type, the return value is 0.
	 */
	int64_t LSE_CALL CStd::AHtoI64( const char * _pcString ) {
		if ( !_pcString ) { return 0ULL; }
		// Skip whitespace.
		while ( IsWhitespace( (*_pcString) ) ) { ++_pcString; }

		LSBOOL bNegate = _pcString[0] == '-' ? true : false;
		if ( _pcString[0] == '+' || _pcString[0] == '-' ) {
			++_pcString;
		}
		int64_t i64Ret = 0ULL;
		char cThis = (*_pcString);
		while ( cThis ) {
			if ( cThis >= '0' && cThis <= '9' ) {
				i64Ret *= 16;
				i64Ret += cThis - '0';
				cThis = (*++_pcString);
				continue;
			}
			else if ( cThis >= 'A' && cThis <= 'F' ) {
				i64Ret *= 16;
				i64Ret += cThis - 'A' + 10;
				cThis = (*++_pcString);
				continue;
			}
			else if ( cThis >= 'a' && cThis <= 'f' ) {
				i64Ret *= 16;
				i64Ret += cThis - 'a' + 10;
				cThis = (*++_pcString);
				continue;
			}
			// Non-hexadecimal character encountered.  Stop.
			break;
		}
		return bNegate ? -i64Ret : i64Ret;
	}

	/**
	 * Converts an integer to a wide-character string using the given radix.
	 * Unlike the standard CRT functions, this will put a negative sign at the front
	 *	of any negative number in any radix.  The CRT functions will only negate numbers in radix 10.
	 *
	 * \param _i32Value Number to be converted.
	 * \param _pwcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	LSUTFX * LSE_CALL CStd::I32toW( int32_t _i32Value, LSUTFX * _pwcRet, uint32_t _ui32Radix ) {
		return I64toW( static_cast<int64_t>(_i32Value), _pwcRet, _ui32Radix );
	}

	/**
	 * Converts an integer to a string using the given radix.
	 * Unlike the standard CRT functions, this will put a negative sign at the front
	 *	of any negative number in any radix.  The CRT functions will only negate numbers in radix 10.
	 *
	 * \param _i32Value Number to be converted.
	 * \param _pcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	char * LSE_CALL CStd::I32toA( int32_t _i32Value, char * _pcRet, uint32_t _ui32Radix ) {
		return I64toA( static_cast<int64_t>(_i32Value), _pcRet, _ui32Radix );
	}

	/**
	 * Converts an integer to a wide-character string using the given radix.
	 * Unlike the standard CRT functions, this will put a negative sign at the front
	 *	of any negative number in any radix.  The CRT functions will only negate numbers in radix 10.
	 *
	 * \param _i64Value Number to be converted.
	 * \param _pwcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	LSUTFX * LSE_CALL CStd::I64toW( int64_t _i64Value, LSUTFX * _pwcRet, uint32_t _ui32Radix ) {
		// If the value is below 0, prepend the string with a minus sign and
		//	work with the absolute value.
		LSBOOL bNegate = _i64Value < 0 ? true : false;
		if ( bNegate ) { _i64Value = -_i64Value; }

		// Get the actual number of digits.  There is always at least one.
		uint64_t ui64Len = 0;
		uint64_t ui64ValueCopy = static_cast<uint64_t>(_i64Value);
		while ( true ) {
			ui64ValueCopy /= _ui32Radix;
			if ( ui64ValueCopy == 0 ) { break; }
			++ui64Len;
		}

		// Keep a copy of the value we will return.
		LSUTFX * pwcRetCopy = _pwcRet;
		// If the string should begin with a negative sign, add it now.
		if ( bNegate ) {
			(*_pwcRet++) = L'-';
		}
		
		// Now add each digit.
		_pwcRet += ui64Len;
		_pwcRet[1] = L'\0';
		ui64ValueCopy = static_cast<uint64_t>(_i64Value);
		for ( uint64_t I = 0; I <= ui64Len; ++I ) {
			uint32_t ui32This = static_cast<uint32_t>(ui64ValueCopy % _ui32Radix);
			LSUTFX wcThisChar = static_cast<LSUTFX>(ui32This + L'0');
			if ( wcThisChar > L'9' ) {
				// Radix and number are both greater than 10.
				wcThisChar = static_cast<LSUTFX>((ui32This - 10) + L'A');
			}
			(*_pwcRet--) = wcThisChar;
			ui64ValueCopy /= _ui32Radix;
		}

		return pwcRetCopy;
	}

	/**
	 * Converts an integer to a string using the given radix.
	 * Unlike the standard CRT functions, this will put a negative sign at the front
	 *	of any negative number in any radix.  The CRT functions will only negate numbers in radix 10.
	 *
	 * \param _i64Value Number to be converted.
	 * \param _pcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	char * LSE_CALL CStd::I64toA( int64_t _i64Value, char * _pcRet, uint32_t _ui32Radix ) {
		// If the value is below 0, prepend the string with a minus sign and
		//	work with the absolute value.
		LSBOOL bNegate = _i64Value < 0 ? true : false;
		if ( bNegate ) { _i64Value = -_i64Value; }

		// Get the actual number of digits.  There is always at least one.
		uint64_t ui64Len = 0;
		uint64_t ui64ValueCopy = static_cast<uint64_t>(_i64Value);
		while ( true ) {
			ui64ValueCopy /= _ui32Radix;
			if ( ui64ValueCopy == 0 ) { break; }
			++ui64Len;
		}

		// Keep a copy of the value we will return.
		char * pcRetCopy = _pcRet;
		// If the string should begin with a negative sign, add it now.
		if ( bNegate ) {
			(*_pcRet++) = '-';
		}

		// Now add each digit.
		_pcRet += ui64Len;
		_pcRet[1] = '\0';
		ui64ValueCopy = static_cast<uint64_t>(_i64Value);
		for ( uint64_t I = 0; I <= ui64Len; ++I ) {
			uint32_t ui32This = static_cast<uint32_t>(ui64ValueCopy % _ui32Radix);
			uint32_t ui32ThisChar = ui32This + '0';
			if ( ui32ThisChar > '9' ) {
				// Radix and number are both greater than 10.
				ui32ThisChar = (ui32This - 10) + 'A';
			}
			(*_pcRet--) = static_cast<char>(ui32ThisChar);
			ui64ValueCopy /= _ui32Radix;
		}

		return pcRetCopy;
	}

	/**
	 * Converts an unsigned integer to a wide-character string using the given radix.
	 *
	 * \param _i32Value Number to be converted.
	 * \param _pwcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	LSUTFX * LSE_CALL CStd::UI32toW( uint32_t _ui32Value, LSUTFX * _pwcRet, uint32_t _ui32Radix ) {
		return UI64toW( static_cast<uint64_t>(_ui32Value), _pwcRet, _ui32Radix );
	}

	/**
	 * Converts an unsigned integer to a string using the given radix.
	 *
	 * \param _i32Value Number to be converted.
	 * \param _pcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	char * LSE_CALL CStd::UI32toA( uint32_t _ui32Value, char * _pcRet, uint32_t _ui32Radix ) {
		return UI64toA( static_cast<uint64_t>(_ui32Value), _pcRet, _ui32Radix );
	}

	/**
	 * Converts an unsigned integer to a wide-character string using the given radix.
	 *
	 * \param _ui64Value Number to be converted.
	 * \param _pwcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	LSUTFX * LSE_CALL CStd::UI64toW( uint64_t _ui64Value, LSUTFX * _pwcRet, uint32_t _ui32Radix ) {
		// Get the actual number of digits.  There is always at least one.
		uint64_t ui64Len = 0;
		uint64_t ui64ValueCopy = _ui64Value;
		while ( true ) {
			ui64ValueCopy /= _ui32Radix;
			if ( ui64ValueCopy == 0 ) { break; }
			++ui64Len;
		}

		// Keep a copy of the value we will return.
		LSUTFX * pwcRetCopy = _pwcRet;
		
		// Now add each digit.
		_pwcRet += ui64Len;
		_pwcRet[1] = L'\0';
		ui64ValueCopy = _ui64Value;
		for ( uint64_t I = 0; I <= ui64Len; ++I ) {
			uint32_t ui32This = static_cast<uint32_t>(ui64ValueCopy % _ui32Radix);
			LSUTFX wcThisChar = static_cast<LSUTFX>(ui32This + L'0');
			if ( wcThisChar > L'9' ) {
				// Radix and number are both greater than 10.
				wcThisChar = static_cast<LSUTFX>((ui32This - 10) + L'A');
			}
			(*_pwcRet--) = wcThisChar;
			ui64ValueCopy /= _ui32Radix;
		}

		return pwcRetCopy;
	}

	/**
	 * Converts an unsigned integer to a string using the given radix.
	 *
	 * \param _ui64Value Number to be converted.
	 * \param _pcRet String result.
	 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
	 * \return Returns the given string.
	 */
	char * LSE_CALL CStd::UI64toA( uint64_t _ui64Value, char * _pcRet, uint32_t _ui32Radix ) {
		// Get the actual number of digits.  There is always at least one.
		uint64_t ui64Len = 0;
		uint64_t ui64ValueCopy = _ui64Value;
		while ( true ) {
			ui64ValueCopy /= _ui32Radix;
			if ( ui64ValueCopy == 0 ) { break; }
			++ui64Len;
		}

		// Keep a copy of the value we will return.
		char * pcRetCopy = _pcRet;

		// Now add each digit.
		_pcRet += ui64Len;
		_pcRet[1] = '\0';
		ui64ValueCopy = _ui64Value;
		for ( uint64_t I = 0; I <= ui64Len; ++I ) {
			uint32_t ui32This = static_cast<uint32_t>(ui64ValueCopy % _ui32Radix);
			uint32_t ui32ThisChar = ui32This + '0';
			if ( ui32ThisChar > '9' ) {
				// Radix and number are both greater than 10.
				ui32ThisChar = (ui32This - 10) + 'A';
			}
			(*_pcRet--) = static_cast<char>(ui32ThisChar);
			ui64ValueCopy /= _ui32Radix;
		}

		return pcRetCopy;
	}

	/**
	 * Converts a string to double.
	 *
	 * \param _pwcString String to be converted.
	 * \return Returns the double value produced by interpreting the input characters as a number.
	 *	The return value is 0.0 if the input cannot be converted to a value of that type.
	 */
	double LSE_CALL CStd::WtoF( const LSUTFX * _pwcString ) {
		if ( !_pwcString ) {
			return 0.0;
		}

#if defined( __LSE_STD_FLOAT__ ) && !defined( LSE_IPHONE ) && !defined( LSE_MAC )
		return ::_wtof( _pwcString );
#else
		// We match input in the following form:
		//	[whitespace][sign][digits][.digits][{d|D|e|E}[sign]digits]
		// If no digits appear before the decimal point, at least one must appear after the decimal point.

		double dRet = 0.0;
		// One step at a time.
		// Skip whitespace.
		// [whitespace]
		while ( IsWhitespace( (*_pwcString) ) ) {
			++_pwcString;
		}
		if ( !(*_pwcString) ) {
			// String was entirely whitespace.
			return dRet;
		}

		// Whitespace skipped.  Next character must be a sign or digits.
		// [sign]
		LSBOOL bNegate = false;
		if ( (*_pwcString) == L'-' ) {
			++_pwcString;
			bNegate = true;
		}
		else if ( (*_pwcString) == L'+' ) {
			++_pwcString;
		}

		// Sign handled.
		//	If digits are next, handle them.
		// [digits]
		LSBOOL bHasIntegral = IsDigit( (*_pwcString) );
		if ( bHasIntegral ) {
			// Convert the integral part.
			for ( ; IsDigit( (*_pwcString) ); ++_pwcString ) {
				double dThis = (*_pwcString) - L'0';
				dRet *= 10.0;
				dRet += dThis;
			}
		}

		// If a decimal is next, handle it.  bHasIntegral is used to ensure that
		//	if there is a decimal with no digits in front, there must be digits behind.
		// [.digits]
		if ( (*_pwcString) == L'.' ) {
			double dExp = 10.0;
			++_pwcString;
			if ( !bHasIntegral && !IsDigit( (*_pwcString) ) ) {
				// No digits in front of the decimal and none behind it.
				return 0.0;
			}

			// Add the decimals.
			for ( ; IsDigit( (*_pwcString) ); ++_pwcString ) {
				double dThis = (*_pwcString) - L'0';
				// Using division instead of reciprical multiplication.  Slower but necessary for accuracy.
				dRet += dThis / dExp;
				dExp *= 10.0;
			}
		}

		// There may be an exponent.
		// {d|D|e|E}
		if ( ToLower( (*_pwcString) ) == L'd' || ToLower( (*_pwcString) ) == L'e' ) {
			// Going to raise the number to a power.
			++_pwcString;

			// Next character may be a sign.
			// [sign]
			LSBOOL bNegExp = false;
			if ( (*_pwcString) == L'-' ) {
				++_pwcString;
				bNegExp = true;
			}
			else if ( (*_pwcString) == L'+' ) {
				++_pwcString;
			}

			// What remains must be digits.
			// If there are no digits, just assume the exponent and sign were part of some other text
			//	attached to the end of a [whitespace][sign][digits][.digits] string.
			if ( !IsDigit( (*_pwcString) ) ) { return bNegate ? -dRet : dRet; }
			// digits
			// Count the digit places and get the exponent.
			uint32_t ui32Digits = 1;
			uint32_t ui32Exponent = 0;
			for ( ; IsDigit( (*_pwcString) ); ++_pwcString ) {
				ui32Digits *= 10;
				ui32Exponent *= 10;
				ui32Exponent += static_cast<uint32_t>((*_pwcString) - static_cast<LSUTFX>(L'0'));
			}
			ui32Digits /= 10;

			// Now for each 10's place, iteratively multiple the powers.
			while ( ui32Exponent ) {
				double dOp = 1;
				for ( uint32_t I = 0; I < ui32Digits; ++I ) {
					dOp *= 10.0;
				}
				for ( ; ui32Exponent >= ui32Digits; ui32Exponent -= ui32Digits ) {
					if ( bNegExp ) {
						dRet /= dOp;
					}
					else {
						dRet *= dOp;
					}
				}
				ui32Digits /= 10;
			}
		}

		return bNegate ? -dRet : dRet;
#endif	// #if defined( __LSE_STD_FLOAT__ ) && !defined( LSE_IPHONE )
	}

	/**
	 * Converts a string to double.
	 *
	 * \param _pcString String to be converted.
	 * \return Returns the double value produced by interpreting the input characters as a number.
	 *	The return value is 0.0 if the input cannot be converted to a value of that type.
	 */
	double LSE_CALL CStd::AtoF( const char * _pcString ) {
		if ( !_pcString ) {
			return 0.0;
		}

#ifdef __LSE_STD_FLOAT__
		return ::atof( _pcString );
#else
		// We match input in the following form:
		//	[whitespace][sign][digits][.digits][{d|D|e|E}[sign]digits]
		// If no digits appear before the decimal point, at least one must appear after the decimal point.

		double dRet = 0.0;
		// One step at a time.
		// Skip whitespace.
		// [whitespace]
		while ( IsWhitespace( (*_pcString) ) ) {
			++_pcString;
		}
		if ( !(*_pcString) ) {
			// String was entirely whitespace.
			return dRet;
		}

		// Whitespace skipped.  Next character must be a sign or digits.
		// [sign]
		LSBOOL bNegate = false;
		if ( (*_pcString) == '-' ) {
			++_pcString;
			bNegate = true;
		}
		else if ( (*_pcString) == '+' ) {
			++_pcString;
		}

		// Sign handled.
		//	If digits are next, handle them.
		// [digits]
		LSBOOL bHasIntegral = IsDigit( (*_pcString) );
		if ( bHasIntegral ) {
			// Convert the integral part.
			for ( ; IsDigit( (*_pcString) ); ++_pcString ) {
				double dThis = (*_pcString) - '0';
				dRet *= 10.0;
				dRet += dThis;
			}
		}

		// If a decimal is next, handle it.  bHasIntegral is used to ensure that
		//	if there is a decimal with no digits in front, there must be digits behind.
		// [.digits]
		if ( (*_pcString) == '.' ) {
			double dExp = 10.0;
			++_pcString;
			if ( !bHasIntegral && !IsDigit( (*_pcString) ) ) {
				// No digits in front of the decimal and none behind it.
				return 0.0;
			}

			// Add the decimals.
			for ( ; IsDigit( (*_pcString) ); ++_pcString ) {
				double dThis = (*_pcString) - '0';
				// Using division instead of reciprical multiplication.  Slower but necessary for accuracy.
				dRet += dThis / dExp;
				dExp *= 10.0;
			}
		}

		// There may be an exponent.
		// {d|D|e|E}
		if ( ToLower( (*_pcString) ) == 'd' || ToLower( (*_pcString) ) == 'e' ) {
			// Going to raise the number to a power.
			++_pcString;

			// Next character may be a sign.
			// [sign]
			LSBOOL bNegExp = false;
			if ( (*_pcString) == '-' ) {
				++_pcString;
				bNegExp = true;
			}
			else if ( (*_pcString) == '+' ) {
				++_pcString;
			}

			// What remains must be digits.
			// If there are no digits, just assume the exponent and sign were part of some other text
			//	attached to the end of a [whitespace][sign][digits][.digits] string.
			if ( !IsDigit( (*_pcString) ) ) { return bNegate ? -dRet : dRet; }
			// digits
			// Count the digit places and get the exponent.
			uint32_t ui32Digits = 1;
			uint32_t ui32Exponent = 0;
			for ( ; IsDigit( (*_pcString) ); ++_pcString ) {
				ui32Digits *= 10;
				ui32Exponent *= 10;
				ui32Exponent += (*_pcString) - '0';
			}
			ui32Digits /= 10;

			// Now for each 10's place, iteratively multiple the powers.
			while ( ui32Exponent ) {
				double dOp = 1;
				for ( uint32_t I = 0; I < ui32Digits; ++I ) {
					dOp *= 10.0;
				}
				for ( ; ui32Exponent >= ui32Digits; ui32Exponent -= ui32Digits ) {
					if ( bNegExp ) {
						dRet /= dOp;
					}
					else {
						dRet *= dOp;
					}
				}
				ui32Digits /= 10;
			}
		}

		return bNegate ? -dRet : dRet;
#endif	// #ifdef __LSE_STD_FLOAT__
	}

	/**
	 * Converts a floating-point value to a string.
	 *
	 * \param _pwcBuffer The buffer to fill with the decoded string.  A NULL character is always appended
	 *	unless _ui32MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _dValue The value to decode.
	 * \param _ui32Precision The precision to use during decoding.  If _i8Format is 'e' or 'E', _ui32Precision
	 *	specifies the number of digits to be printed after the decimal point.  If _i8Format is 'g' or 'G',
	 *	_ui32Precision specifies the maximum number of significant digits printed.  If _i8Format is 'f',
	 *	_ui32Precision specifies the number of digits after the decimal point.
	 * \param _i8Format Can be 'e', 'E', 'f', 'g', or 'G'.
	 * \return Returns _pwcBuffer.
	 */
	LSUTFX * LSE_CALL CStd::FtoW( LSUTFX * _pwcBuffer, uint32_t _ui32MaxLen, double _dValue,
		uint32_t _ui32Precision, int8_t _i8Format ) {
#ifdef __LSE_STD_FLOAT__
		LSUTFX szBuffer[32];
		LSUTFX szNumBuffer[32];
		CStd::WStrCpy( szBuffer, L"%." );
		CStd::WStrCat( szBuffer, CStd::UI32toW( _ui32Precision, szNumBuffer ) );
		szNumBuffer[0] = _i8Format;
		szNumBuffer[1] = L'\0';
		CStd::WStrNCat( szBuffer, szNumBuffer, 1 );
		::swprintf( _pwcBuffer, _ui32MaxLen, szBuffer, _dValue );
		return _pwcBuffer;
#else
		return FtoA<11UL, 52, 1023, 64, LSUTFX>( _pwcBuffer, _ui32MaxLen, reinterpret_cast<uint8_t *>(&_dValue), _ui32Precision, _i8Format );
#endif	// #ifdef __LSE_STD_FLOAT__
	}

	/**
	 * Converts a floating-point value to a string.
	 *
	 * \param _pcBuffer The buffer to fill with the decoded string.  A NULL character is always appended
	 *	unless _ui32MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _dValue The value to decode.
	 * \param _ui32Precision The precision to use during decoding.  If _i8Format is 'e' or 'E', _ui32Precision
	 *	specifies the number of digits to be printed after the decimal point.  If _i8Format is 'g' or 'G',
	 *	_ui32Precision specifies the maximum number of significant digits printed.  If _i8Format is 'f',
	 *	_ui32Precision specifies the number of digits after the decimal point.
	 * \param _i8Format Can be 'e', 'E', 'f', 'g', or 'G'.
	 * \return Returns _pwcBuffer.
	 */
	char * LSE_CALL CStd::FtoA( char * _pcBuffer, uint32_t _ui32MaxLen, double _dValue,
		uint32_t _ui32Precision, int8_t _i8Format ) {
#ifdef __LSE_STD_FLOAT__
		char szBuffer[32];
		char szNumBuffer[32];
		CStd::StrCpy( szBuffer, "%." );
		CStd::StrCat( szBuffer, CStd::UI32toA( _ui32Precision, szNumBuffer ) );
		uint32_t ui32Len = CStd::StrLen( szBuffer );
		szBuffer[ui32Len++] = _i8Format;
		szBuffer[ui32Len] = '\0';
#if defined( LSE_IPHONE ) || defined( LSE_MAC )
		char szNewBuffer[1048];
		::sprintf( szNewBuffer, szBuffer, _dValue );
		CStd::StrNCpy( _pcBuffer, szNewBuffer, _ui32MaxLen );
#else
		::sprintf_s( _pcBuffer, _ui32MaxLen, szBuffer, _dValue ); // Only available in Visual Studio 2005 and later.
#endif	// #ifdef LSE_IPHONE
		return _pcBuffer;
#else
		return FtoA<11UL, 52, 1023, 64, char>( _pcBuffer, _ui32MaxLen, reinterpret_cast<uint8_t *>(&_dValue), _ui32Precision, _i8Format );
#endif	// #ifdef __LSE_STD_FLOAT__
	}
	
	/**
	 * Converts a const char string to a LSUTFX string.  _pwcRet must be long enough to hold the converted string.
	 *
	 * \param _pcSrc The source string.
	 * \param _pwcRet The destination string.
	 * \return Returns the converted string.
	 */
	LSUTFX * LSE_CALL CStd::CharToWChar( const char * _pcSrc, LSUTFX * _pwcRet ) {
		LSUTFX * pwcCopy = _pwcRet;
		while ( (*_pcSrc) ) {
			(*pwcCopy++) = static_cast<LSUTFX>(*_pcSrc++);
		}
		(*pwcCopy++) = L'\0';
		return _pwcRet;
	}
	
	/**
	 * Converts a const LSUTFX string to a char string.  _pcRet must be long enough to hold the converted string.
	 *
	 * \param _pwcSrc The source string.
	 * \param _pcRet The destination string.
	 * \return Returns the converted string.
	 */
	char * LSE_CALL CStd::WCharToChar( const LSUTFX * _pwcSrc, char * _pcRet ) {
		char * pcCopy = _pcRet;
		while ( (*_pwcSrc) ) {
			(*pcCopy++) = static_cast<char>(*_pwcSrc++);
		}
		(*pcCopy++) = '\0';
		return _pcRet;
	}

	/**
	 * Converts a UTF-X LSUTFX string to a UTF-8 char string.  X is 16 on Windows and 32 on Macintosh.  These
	 *	sizes match the bits in the LSUTFX type on each platform.
	 *
	 * \param _pwcSrc The source string.
	 * \param _pcRet The destination string.
	 * \param _ui32RetLen The length, in bytes, of the buffer to which _pcRet points.
	 * \return Returns the converted string.
	 */
	char * LSE_CALL CStd::UtfXToUtf8( const LSUTFX * _pwcSrc, char * _pcRet, uint32_t _ui32RetLen ) {
		if ( !_ui32RetLen ) { return _pcRet; }	// Nothing to copy if there is no buffer.
#ifdef LSE_WINDOWS
		Utf16ToUtf8( reinterpret_cast<LSUTF8 *>(_pcRet), reinterpret_cast<const LSUTF16 *>(_pwcSrc), _ui32RetLen );
		return _pcRet;
#elif defined( LSE_MAC )
		Utf32ToUtf8( reinterpret_cast<LSUTF8 *>(_pcRet), reinterpret_cast<const LSUTF32 *>(_pwcSrc), _ui32RetLen );
		return _pcRet;
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Gets the number of bytes needed to contain a string converted from UTF-X to UTF-8.  X is 16 on Windows and 32 on Macintosh.  These
	 *	sizes match the bits in the LSUTFX type on each platform.
	 *
	 * \param _pwcSrc The source string.
	 * \return Returns the number of bytes needed to store the converted string, including the terminating NULL character.
	 */
	uint32_t LSE_CALL CStd::UtfXToUtf8Len( const LSUTFX * _pwcSrc ) {
#ifdef LSE_WINDOWS
		return Utf16StrLenAsUtf8( reinterpret_cast<const LSUTF16 *>(_pwcSrc) );
#elif defined( LSE_MAC )
		// LSUTFX is 4 bytes on Macintosh.
		return Utf32StrLenAsUtf8( reinterpret_cast<const LSUTF32 *>(_pwcSrc) );
		/*CFStringRef srStr = ::CFStringCreateWithBytes( NULL, reinterpret_cast<const UInt8 *>(_pwcSrc), CStd::WStrLen( _pwcSrc ) * sizeof( LSUTFX ), kCFStringEncodingUTF32LE, false );
		if ( !srStr ) { return 0; }

		CFIndex iNewLen;
		CFIndex iLen = ::CFStringGetLength( srStr );
		// Get the length of the converted string.
		if ( ::CFStringGetBytes( srStr, ::CFRangeMake( 0, iLen ), kCFStringEncodingUTF8, '?', false, NULL, 0x7FFFFFFF, &iNewLen ) == 0 ) {
			// Conversion failed.
			::CFRelease( srStr );
			return 0;
		}
		::CFRelease( srStr );
		return iNewLen + 1;*/
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Gets the next UTF-16 character from a UTF-16 string.
	 *
	 * \param _putf16Char Pointer to the next character to decode.  String must be in UTF-16 format.
	 * \param _ui32Len Length, in LSUTF16 units, of the string.  This is not the number of Unicode
	 *	characters, but actual the number of LSUTF16 characters in the buffer.
	 * \param _pui32Size If not NULL, this holds the returned size of the character in LSUTF16 units.
	 * \return Returns an uint32_t value representing the decoded Unicode character.  Also returns the
	 *	size of the character in LSUTF16 units.  Returns LSSUTF_INVALID for invalid characters.
	 */
	uint32_t CStd::NextUtf16Char( const LSUTF16 * _putf16Char, uint32_t _ui32Len, uint32_t * _pui32Size ) {
		if ( _ui32Len == 0 ) { return 0; }

		// Get the low bits (which may be all there are).
		uint32_t ui32Ret = (*_putf16Char);

		uint32_t ui32Top = ui32Ret & 0xFC00UL;
		// Check to see if this is a surrogate pair.
		if ( ui32Top == 0xD800UL ) {
			if ( _ui32Len < 2 ) {
				// Not enough space to decode correctly.
				if ( _pui32Size ) {
					(*_pui32Size) = 1;
				}
				return LSSUTF_INVALID;
			}

			// Need to add the next character to it.
			// Remove the 0xD800.
			ui32Ret &= ~0xD800UL;
			ui32Ret <<= 10;

			// Get the second set of bits.
			uint32_t ui32Next = (*++_putf16Char);
			if ( (ui32Next & 0xFC00UL) != 0xDC00UL ) {
				// Invalid second character.
				// Standard defines this as an error.
				if ( _pui32Size ) {
					(*_pui32Size) = 1;
				}
				return LSSUTF_INVALID;
			}
			if ( _pui32Size ) {
				(*_pui32Size) = 2;
			}

			ui32Next &= ~0xDC00UL;

			// Add the second set of bits.
			ui32Ret |= ui32Next;

			return ui32Ret + 0x10000UL;
		}

		if ( _pui32Size ) {
			(*_pui32Size) = 1;
		}
		return ui32Ret;
	}

	/**
	 * Gets the next UTF-8 character from a UTF-8 string.
	 *
	 * \param _putf8Char Pointer to the next character to decode.  String must be in UTF-8 format.
	 * \param _ui32Len Length, in LSUTF8 units, of the string.  This is not the number of Unicode
	 *	characters, but actual the number of LSUTF8 characters in the buffer.
	 * \param _pui32Size If not NULL, this holds the returned size of the character in LSUTF8 units.
	 * \return Returns an uint32_t value representing the decoded Unicode character.  Also returns the
	 *	size of the character in LSUTF8 units.  Returns LSSUTF_INVALID for invalid characters.
	 */
	uint32_t CStd::NextUtf8Char( const LSUTF8 * _putf8Char, uint32_t _ui32Len, uint32_t * _pui32Size ) {
		if ( _ui32Len == 0 ) { return 0; }

		// Get the low bits (which may be all there are).
		uint32_t ui32Ret = (*_putf8Char);

		// The first byte is a special case.
		if ( (ui32Ret & 0x80UL) == 0 ) {
			// We are done.
			if ( _pui32Size ) {
				(*_pui32Size) = 1;
			}
			return ui32Ret;
		}

		// We are in a multi-byte sequence.  Get bits from the top, starting
		//	from the second bit.
		uint32_t I = 0x20;
		uint32_t ui32Len = 2;
		uint32_t ui32Mask = 0xC0UL;
		while ( ui32Ret & I ) {
			// Add this bit to the mask to be removed later.
			ui32Mask |= I;
			I >>= 1;
			++ui32Len;
			if ( I == 0 ) {
				// Invalid sequence.
				if ( _pui32Size ) {
					(*_pui32Size) = 1;
				}
				return LSSUTF_INVALID;
			}
		}

		// Bounds checking.
		if ( ui32Len > _ui32Len ) {
			if ( _pui32Size ) {
				(*_pui32Size) = _ui32Len;
			}
			return LSSUTF_INVALID;
		}

		// We know the size now, so set it.
		// Even if we return an invalid character we want to return the correct number of
		//	bytes to skip.
		if ( _pui32Size ) {
			(*_pui32Size) = ui32Len;
		}

		// If the length is greater than 4, it is invalid.
		if ( ui32Len > 4 ) {
			// Invalid sequence.
			return LSSUTF_INVALID;
		}

		// Mask out the leading bits.
		ui32Ret &= ~ui32Mask;

		// For every trailing bit, add it to the final value.
		for ( I = ui32Len - 1; I--; ) {
			uint32_t ui32This = (*++_putf8Char);
			// Validate the byte.
			if ( (ui32This & 0xC0UL) != 0x80UL ) {
				// Invalid.
				return LSSUTF_INVALID;
			}

			ui32Ret <<= 6;
			ui32Ret |= (ui32This & 0x3F);
		}

		// Finally done.
		return ui32Ret;
	}

	/**
	 * Converts a raw 32-bit Unicode character to a UTF-16 character.  Returns the UTF-16 character as
	 *	an uint32_t value.  The returned length is the number LSUTF16 characters returned.
	 *
	 * \param _ui32Raw The raw Unicode value to convert.
	 * \param _ui32Len The lengh, in LSUTF16 characters, of the converted value.
	 * \return Returns the converted character in uint32_t form along with the length, in units of
	 *	LSUTF16, of the returned value.
	 */
	uint32_t LSE_CALL CStd::RawUnicodeToUtf16Char( uint32_t _ui32Raw, uint32_t &_ui32Len ) {
		if ( (_ui32Raw & 0xFFFF0000) == 0 ) {
			_ui32Len = 1;
			return _ui32Raw;
		}

		_ui32Len = 2;

		// Break into surrogate pairs.
		_ui32Raw -= 0x10000UL;
		uint32_t ui32Hi = (_ui32Raw >> 10) & 0x3FF;
		uint32_t ui32Low = _ui32Raw & 0x3FF;

		return (0xD800 | ui32Hi) |
			((0xDC00 | ui32Low) << 16);
	}

	/**
	 * Converts a raw 32-bit Unicode character to a UTF-8 character.  Returns the UTF-8 character as
	 *	an uint32_t value.  The returned length is the number LSUTF8 characters returned.
	 *
	 * \param _ui32Raw The raw Unicode value to convert.
	 * \param _ui32Len The lengh, in LSUTF8 characters, of the converted value.
	 * \return Returns the converted character in uint32_t form along with the length, in units of
	 *	LSUTF8, of the returned value.
	 */
	uint32_t LSE_CALL CStd::RawUnicodeToUtf8Char( uint32_t _ui32Raw, uint32_t &_ui32Len ) {
		// Handle the single-character case separately since it is a special case.
		if ( _ui32Raw < 0x80UL ) {
			_ui32Len = 1;
			return _ui32Raw;
		}

		// Upper bounds checking.
		if ( _ui32Raw > 0x10FFFFUL ) {
			// Invalid character.  How do we handle it?
			// Return a default character.
			_ui32Len = 1;
			return '?';
		}

		// Every other case uses bit markers.
		// Start from the lowest encoding and check upwards.
		uint32_t ui32High = 0x00000800UL;
		uint32_t ui32Mask = 0xC0;
		_ui32Len = 2;
		while ( _ui32Raw >= ui32High ) {
			ui32High <<= 5;
			ui32Mask = (ui32Mask >> 1) | 0x80UL;
			++_ui32Len;
		}

		// Encode the first byte.
		uint32_t ui32BottomMask = ~((ui32Mask >> 1) | 0xFFFFFF80UL);
		uint32_t ui32Ret = ui32Mask | ((_ui32Raw >> ((_ui32Len - 1) * 6)) & ui32BottomMask);
		// Now fill in the rest of the bits.
		uint32_t ui32Shift = 8;
		for ( uint32_t I = _ui32Len - 1; I--; ) {
			// Shift down, mask off 6 bits, and add the 10xxxxxx flag.
			uint32_t ui32This = ((_ui32Raw >> (I * 6)) & 0x3F) | 0x80;

			ui32Ret |= ui32This << ui32Shift;
			ui32Shift += 8;
		}

		return ui32Ret;
	}

	/**
	 * Converts a UTF-32 string to a 32-bit string where every character is stored as a 32-bit value.
	 *	The converted string is always appended with a NULL unless it is of 0 length.
	 *
	 * \param _pui32Dst The destination where to store the converted string.
	 * \param _putf32Src The UTF-32 string to be converted.
	 * \param _ui32MaxLen The maximum number of characters to store.
	 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
	 */
	uint32_t LSE_CALL CStd::Utf32ToUnicodeRaw( uint32_t * _pui32Dst,
		const LSUTF32 * _putf32Src, uint32_t _ui32MaxLen ) {

		uint32_t I = 0;
		for ( ; I < _ui32MaxLen; ++I ) {
			_pui32Dst[I] = NextUtf32Char( _putf32Src++, 1 );
			if ( !_pui32Dst[I] ) { return I; }
		}

		if ( _ui32MaxLen ) {
			_pui32Dst[_ui32MaxLen-1] = 0;
		}
		return I;
	}

	/**
	 * Converts a UTF-16 string to a 32-bit string where every character is stored as a 32-bit value.
	 *	The converted string is always appended with a NULL unless it is of 0 length.
	 *
	 * \param _pui32Dst The destination where to store the converted string.
	 * \param _putf16Src The NULL-terminated UTF-16 string to be converted.
	 * \param _ui32MaxLen The maximum number of characters to store.
	 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
	 */
	uint32_t LSE_CALL CStd::Utf16ToUnicodeRaw( uint32_t * _pui32Dst,
		const LSUTF16 * _putf16Src, uint32_t _ui32MaxLen ) {

		// Get the number of LSUTF16 characters in the buffer (including the NULL).
		const LSUTF16 * putf16Len = _putf16Src;
		while ( (*putf16Len++) ) {}

		uint32_t ui32Len = static_cast<uint32_t>(putf16Len - _putf16Src);

		// I represents the length of the destination buffer.
		uint32_t I = 0;
		for ( ; I < _ui32MaxLen; ++I ) {
			uint32_t ui32Size;
			_pui32Dst[I] = NextUtf16Char( _putf16Src, ui32Len, &ui32Size );
			if ( !_pui32Dst[I] ) { return I; }

			// Advance the source string.
			ui32Len -= ui32Size;
			_putf16Src += ui32Size;
		}

		if ( _ui32MaxLen ) {
			_pui32Dst[_ui32MaxLen-1] = 0;
		}
		return I;
	}

	/**
	 * Converts a UTF-8 string to a 32-bit string where every character is stored as a 32-bit value.
	 *	The converted string is always appended with a NULL unless it is of 0 length.
	 *
	 * \param _pui32Dst The destination where to store the converted string.
	 * \param _putf8Src The NULL-terminated UTF-8 string to be converted.
	 * \param _ui32MaxLen The maximum number of characters to store.
	 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
	 */
	uint32_t LSE_CALL CStd::Utf8ToUnicodeRaw( uint32_t * _pui32Dst,
		const LSUTF8 * _putf8Src, uint32_t _ui32MaxLen ) {

		// Get the number of LSUTF8 characters in the buffer (including the NULL).
		const LSUTF8 * putf8Len = _putf8Src;
		while ( (*putf8Len++) ) {}

		uint32_t ui32Len = static_cast<uint32_t>(putf8Len - _putf8Src);

		// I represents the length of the destination buffer.
		uint32_t I = 0;
		for ( ; I < _ui32MaxLen; ++I ) {
			uint32_t ui32Size;
			_pui32Dst[I] = NextUtf8Char( _putf8Src, ui32Len, &ui32Size );
			if ( !_pui32Dst[I] ) { return I; }

			// Advance the source string.
			ui32Len -= ui32Size;
			_putf8Src += ui32Size;
		}

		if ( _ui32MaxLen ) {
			_pui32Dst[_ui32MaxLen-1] = 0;
		}
		return I;
	}

	/**
	 * Converts a raw 32-bit Unicode string to a UTF-32 string.  The input string must be NULL-terminated,
	 *	and the output string will always be appended with a NULL.
	 *
	 * \param _putf32Dst The destination buffer.
	 * \param _pui32Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf8Dst can store.  This is the total number
	 *	of LSUTF32 characters allocated in the buffer.
	 * \return Returns the number of LSUTF32 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF32 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::RawUnicodeToUtf32( LSUTF32 * _putf32Dst,
		const uint32_t * _pui32Src, uint32_t _ui32MaxLen ) {
		uint32_t _ui32Len;
		for ( uint32_t I = 0; I < _ui32MaxLen; ++I ) {
			(*_putf32Dst) = RawUnicodeToUtf32Char( (*_pui32Src++), _ui32Len );
			if ( !(*_putf32Dst++) ) {
				return I;
			}
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf32Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a raw 32-bit Unicode string to a UTF-16 string.  The input string must be NULL-terminated,
	 *	and the output string will always be appended with a NULL.
	 *
	 * \param _putf16Dst The destination buffer.
	 * \param _pui32Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf16Dst can store.  This is the total number
	 *	of LSUTF16 characters allocated in the buffer.
	 * \return Returns the number of LSUTF16 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF16 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::RawUnicodeToUtf16( LSUTF16 * _putf16Dst,
		const uint32_t * _pui32Src, uint32_t _ui32MaxLen ) {
		uint32_t _ui32Len;
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32Value = RawUnicodeToUtf16Char( (*_pui32Src++), _ui32Len );
			if ( !ui32Value ) {
				_putf16Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( _ui32Len-- && I < _ui32MaxLen ) {
				_putf16Dst[I++] = static_cast<LSUTF16>(ui32Value & 0xFFFFUL);
				ui32Value >>= 16;
			}
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf16Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a raw 32-bit Unicode string to a UTF-8 string.  The input string must be NULL-terminated,
	 *	and the output string will always be appended with a NULL.
	 *
	 * \param _putf8Dst The destination buffer.
	 * \param _pui32Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf8Dst can store.  This is the total number
	 *	of LSUTF8 characters allocated in the buffer.
	 * \return Returns the number of LSUTF8 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF8 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::RawUnicodeToUtf8( LSUTF8 * _putf8Dst,
		const uint32_t * _pui32Src, uint32_t _ui32MaxLen ) {
		uint32_t _ui32Len;
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32Value = RawUnicodeToUtf8Char( (*_pui32Src++), _ui32Len );
			if ( !ui32Value ) {
				_putf8Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( _ui32Len-- && I < _ui32MaxLen ) {
				_putf8Dst[I++] = static_cast<LSUTF8>(ui32Value & 0xFFUL);
				ui32Value >>= 8;
			}
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf8Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-32 string to UTF-8.
	 *
	 * \param _putf8Dst The destination buffer.
	 * \param _putf32Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf8Dst can store.  This is the total number
	 *	of LSUTF8 characters allocated in the buffer.
	 * \return Returns the number of LSUTF8 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF8 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf32ToUtf8( LSUTF8 * _putf8Dst, const LSUTF32 * _putf32Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32ThisLen;
			uint32_t ui32Value = RawUnicodeToUtf8Char( NextUtf32Char( _putf32Src++, 1 ), ui32ThisLen );
			if ( !ui32Value ) {
				_putf8Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( ui32ThisLen-- && I < _ui32MaxLen ) {
				_putf8Dst[I++] = static_cast<LSUTF8>(ui32Value & 0xFFUL);
				ui32Value >>= 8;
			}
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf8Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-32 string to UTF-16.
	 *
	 * \param _putf16Dst The destination buffer.
	 * \param _putf32Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf16Dst can store.  This is the total number
	 *	of LSUTF16 characters allocated in the buffer.
	 * \return Returns the number of LSUTF16 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF16 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf32ToUtf16( LSUTF16 * _putf16Dst, const LSUTF32 * _putf32Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32ThisLen;
			uint32_t ui32Value = RawUnicodeToUtf16Char( NextUtf32Char( _putf32Src++, 1 ), ui32ThisLen );
			if ( !ui32Value ) {
				_putf16Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( ui32ThisLen-- && I < _ui32MaxLen ) {
				_putf16Dst[I++] = static_cast<LSUTF16>(ui32Value & 0xFFFFUL);
				ui32Value >>= 16;
			}
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf16Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-16 string to UTF-8.
	 *
	 * \param _putf8Dst The destination buffer.
	 * \param _putf16Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf8Dst can store.  This is the total number
	 *	of LSUTF8 characters allocated in the buffer.
	 * \return Returns the number of LSUTF8 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF8 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf16ToUtf8( LSUTF8 * _putf8Dst, const LSUTF16 * _putf16Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32ThisLen;
			uint32_t ui32SrcLen;
			uint32_t ui32Value = RawUnicodeToUtf8Char( NextUtf16Char( _putf16Src, 2, &ui32SrcLen ), ui32ThisLen );
			if ( !ui32Value ) {
				_putf8Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( ui32ThisLen-- && I < _ui32MaxLen ) {
				_putf8Dst[I++] = static_cast<LSUTF8>(ui32Value & 0xFFUL);
				ui32Value >>= 8;
			}

			_putf16Src += ui32SrcLen;
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf8Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-16 string to UTF-32.
	 *
	 * \param _putf32Dst The destination buffer.
	 * \param _putf16Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf32Dst can store.  This is the total number
	 *	of LSUTF32 characters allocated in the buffer.
	 * \return Returns the number of LSUTF32 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF32 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf16ToUtf32( LSUTF32 * _putf32Dst, const LSUTF16 * _putf16Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ++I ) {
			uint32_t ui32ThisLen;
			uint32_t ui32SrcLen;
			_putf32Dst[I] = RawUnicodeToUtf32Char( NextUtf16Char( _putf16Src, 2, &ui32SrcLen ), ui32ThisLen );
			if ( !_putf32Dst[I] ) {
				_putf32Dst[I] = 0;
				return I;
			}
			_putf16Src += ui32SrcLen;
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf32Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	

	/**
	 * Converts a UTF-8 string to UTF-16.
	 *
	 * \param _putf16Dst The destination buffer.
	 * \param _putf8Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf16Dst can store.  This is the total number
	 *	of LSUTF16 characters allocated in the buffer.
	 * \return Returns the number of LSUTF16 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF16 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf8ToUtf16( LSUTF16 * _putf16Dst, const LSUTF8 * _putf8Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ) {
			uint32_t ui32ThisLen;
			uint32_t ui32SrcLen;
			uint32_t ui32Value = RawUnicodeToUtf16Char( NextUtf8Char( _putf8Src, 4, &ui32SrcLen ), ui32ThisLen );
			if ( !ui32Value ) {
				_putf16Dst[I] = 0;
				return I;
			}

			// Add the parts.
			while ( ui32ThisLen-- && I < _ui32MaxLen ) {
				_putf16Dst[I++] = static_cast<LSUTF16>(ui32Value & 0xFFFFUL);
				ui32Value >>= 16;
			}

			_putf8Src += ui32SrcLen;
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf16Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-8 string to UTF-32.
	 *
	 * \param _putf32Dst The destination buffer.
	 * \param _putf8Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putf32Dst can store.  This is the total number
	 *	of LSUTF32 characters allocated in the buffer.
	 * \return Returns the number of LSUTF32 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF32 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf8ToUtf32( LSUTF32 * _putf32Dst, const LSUTF8 * _putf8Src,
		uint32_t _ui32MaxLen ) {
		for ( uint32_t I = 0; I < _ui32MaxLen; ++I ) {
			uint32_t ui32ThisLen;
			uint32_t ui32SrcLen;
			_putf32Dst[I] = RawUnicodeToUtf32Char( NextUtf8Char( _putf8Src, 4, &ui32SrcLen ), ui32ThisLen );
			if ( !_putf32Dst[I] ) {
				_putf32Dst[I] = 0;
				return I;
			}
			_putf8Src += ui32SrcLen;
		}

		// Ran out of buffer before reaching the terminating NULL.
		if ( _ui32MaxLen ) {
			_putf32Dst[_ui32MaxLen-1] = 0;
			return _ui32MaxLen - 1;
		}
		return 0;
	}

	/**
	 * Converts a UTF-8 string to a UTF-X string.  X is 16 on Windows and 32 on other platforms.
	 *
	 * \param _putfxDst The destination buffer.
	 * \param _putf8Src The NULL-terminated source string.
	 * \param _ui32MaxLen The maximum number of characters _putfxDst can store.  This is the total number
	 *	of LSUTFX characters allocated in the buffer.
	 * \return Returns the number of LSUTF32 characters output to the buffer, such that the return value,
	 *	treated as an index into an array of LSUTF32 characters, will index the NULL character in the
	 *	returned string.
	 */
	uint32_t LSE_CALL CStd::Utf8ToUtfX( LSUTFX * _putfxDst, const LSUTF8 * _putf8Src,
		uint32_t _ui32MaxLen ) {
#ifdef LSS_WINDOWS
		return Utf8ToUtf16( reinterpret_cast<LSUTF16 *>(_putfxDst), _putf8Src, _ui32MaxLen );
#else
		return Utf8ToUtf32( reinterpret_cast<LSUTF32 *>(_putfxDst), _putf8Src, _ui32MaxLen );
#endif	// #ifdef LSS_WINDOWS
	}

	/**
	 * Gets the length of a UTF-32 string in Unicode characters.
	 *
	 * \param _putf32String The string whose length is to be determined.
	 * \return Returns the number of Unicode characters in the given UTF-32 string.
	 */
	uint32_t LSE_CALL CStd::Utf32StrLen( const LSUTF32 * _putf32String ) {
		uint32_t ui32Ret = 0;
		uint32_t ui32ThisLen;
		uint32_t ui32Char = NextUtf32Char( _putf32String, 1, &ui32ThisLen );
		while ( ui32Char ) {
			++ui32Ret;
			_putf32String += ui32ThisLen;
			ui32Char = NextUtf32Char( _putf32String, 1, &ui32ThisLen );
		}
		return ui32Ret;
	}

	/**
	 * Gets the length of a UTF-16 string in Unicode characters.
	 *
	 * \param _putf16String The string whose length is to be determined.
	 * \return Returns the number of Unicode characters in the given UTF-16 string.
	 */
	uint32_t LSE_CALL CStd::Utf16StrLen( const LSUTF16 * _putf16String ) {
		uint32_t ui32Ret = 0;
		uint32_t ui32ThisLen;
		uint32_t ui32Char = NextUtf16Char( _putf16String, 2, &ui32ThisLen );
		while ( ui32Char ) {
			++ui32Ret;
			_putf16String += ui32ThisLen;
			ui32Char = NextUtf16Char( _putf16String, 2, &ui32ThisLen );
		}
		return ui32Ret;
	}

	/**
	 * Gets the length of a UTF-8 string in Unicode characters.
	 *
	 * \param _putf8String The string whose length is to be determined.
	 * \return Returns the number of Unicode characters in the given UTF-8 string.
	 */
	uint32_t LSE_CALL CStd::Utf8StrLen( const LSUTF8 * _putf8String ) {
		uint32_t ui32Ret = 0;
		uint32_t ui32ThisLen;
		uint32_t ui32Char = NextUtf8Char( _putf8String, 4, &ui32ThisLen );
		while ( ui32Char ) {
			++ui32Ret;
			_putf8String += ui32ThisLen;
			ui32Char = NextUtf8Char( _putf8String, 4, &ui32ThisLen );
		}
		return ui32Ret;
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-32 string in UTF-8 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf32String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-32 string converted to UTF-8,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf32StrLenAsUtf8( const LSUTF32 * _putf32String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-32 to raw.
			ui32Char = NextUtf32Char( _putf32String, 1, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-8.
			RawUnicodeToUtf8Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf32String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total;
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-32 string in UTF-16 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf32String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-32 string converted to UTF-16,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf32StrLenAsUtf16( const LSUTF32 * _putf32String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-32 to raw.
			ui32Char = NextUtf32Char( _putf32String, 1, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-16.
			RawUnicodeToUtf16Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf32String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total * sizeof( LSUTF16 );
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-16 string in UTF-8 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf16String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-16 string converted to UTF-8,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf16StrLenAsUtf8( const LSUTF16 * _putf16String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-16 to raw.
			ui32Char = NextUtf16Char( _putf16String, 2, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-8.
			RawUnicodeToUtf8Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf16String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total;
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-16 string in UTF-32 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf16String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-16 string converted to UTF-32,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf16StrLenAsUtf32( const LSUTF16 * _putf16String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-16 to raw.
			ui32Char = NextUtf16Char( _putf16String, 2, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-32.
			RawUnicodeToUtf32Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf16String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total * sizeof( LSUTF32 );
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-8 string in UTF-16 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf8String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-16,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf8StrLenAsUtf16( const LSUTF8 * _putf8String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-8 to raw.
			ui32Char = NextUtf8Char( _putf8String, 4, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-16.
			RawUnicodeToUtf16Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf8String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total * sizeof( LSUTF16 );
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-8 string in UTF-32 format, including the terminating
	 *	NULL.
	 *
	 * \param _putf8String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-32,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf8StrLenAsUtf32( const LSUTF8 * _putf8String ) {
		uint32_t ui32Total = 0;

		uint32_t ui32Char;
		do {
			uint32_t ui32ThisLen;
			// From UTF-8 to raw.
			ui32Char = NextUtf8Char( _putf8String, 4, &ui32ThisLen );

			uint32_t ui32NewSize;
			// Get the number of characters needed to encode this as UTF-32.
			RawUnicodeToUtf32Char( ui32Char, ui32NewSize );
			ui32Total += ui32NewSize;

			_putf8String += ui32ThisLen;
		} while ( ui32Char );
		return ui32Total * sizeof( LSUTF32 );
	}

	/**
	 * Gets the number of bytes needed to hold a UTF-8 string in UTF-X format, including the terminating
	 *	NULL.
	 *
	 * \param _putf8String The string whose buffer size after conversion is to be determined.
	 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-X,
	 *	including the terminating NULL.  This value should be used for allocation.
	 */
	uint32_t LSE_CALL CStd::Utf8StrLenAsUtfX( const LSUTF8 * _putf8String ) {
#ifdef LSE_VISUALSTUDIO
		return Utf8StrLenAsUtf16( _putf8String );
#else
		return Utf8StrLenAsUtf32( _putf8String );
#endif	// #ifdef LSE_VISUALSTUDIO
	}

	/**
	 * Determines if the given wide character is a whitespace character.
	 *
	 * \param _wcChar Character to test.
	 * \return Returns a non-zero value if _wcChar is a white-space character (0x09-0x0D or 0x20). 
	 */
	LSBOOL LSE_CALL CStd::IsWhitespace( LSUTFX _wcChar ) {
		return (_wcChar >= L'\x09' && _wcChar <= L'\x0D') || _wcChar == L' ';
	}

	/**
	 * Determines if the given character is a whitespace character.
	 *
	 * \param _cChar Character to test.
	 * \return Returns a non-zero value if _cChar is a white-space character (0x09-0x0D or 0x20). 
	 */
	LSBOOL LSE_CALL CStd::IsWhitespace( char _cChar ) {
		return (_cChar >= '\x09' && _cChar <= '\x0D') || _cChar == ' ';
	}

	/**
	 * Determines if the given wide character is an alpha character.
	 *
	 * \param _wcChar Character to test.
	 * \return Returns a non-zero value if _wcChar is within the ranges A-Z or a-z.
	 */
	LSBOOL LSE_CALL CStd::IsAlpha( LSUTFX _wcChar ) {
		register LSUTFX wcThis = static_cast<LSUTFX>(_wcChar & 0xFFFFFFDF);
		return (wcThis >= L'A' && wcThis <= L'Z');
	}

	/**
	 * Determines if the given character is an alpha character.
	 *
	 * \param _cChar Character to test.
	 * \return Returns a non-zero value if _cChar is within the ranges A-Z or a-z.
	 */
	LSBOOL LSE_CALL CStd::IsAlpha( char _cChar ) {
		register char cThis = _cChar & static_cast<char>(0xDF);
		return (cThis >= 'A' && cThis <= 'Z');
	}

	/**
	 * Determines if the given wide character is a digit.
	 *
	 * \param _wcChar Character to test.
	 * \return Returns a non-zero value if _wcChar is a decimal digit (0-9).
	 */
	LSBOOL LSE_CALL CStd::IsDigit( LSUTFX _wcChar ) {
		return (_wcChar >= L'0' && _wcChar <= L'9');
	}

	/**
	 * Determines if the given character is a digit.
	 *
	 * \param _cChar Character to test.
	 * \return Returns a non-zero value if _cChar is a decimal digit (0-9).
	 */
	LSBOOL LSE_CALL CStd::IsDigit( char _cChar ) {
		return (_cChar >= '0' && _cChar <= '9');
	}

	/**
	 * Determines if the given wide character is alpha-numeric.
	 *
	 * \param _wcChar Character to test.
	 * \return Returns a non-zero value if either IsAlpha() or IsDigit() is true for _wcChar, that is, if _wcChar is within the ranges A-Z, a-z, or 0-9.
	 */
	LSBOOL LSE_CALL CStd::IsAlNum( LSUTFX _wcChar ) {
		return IsAlpha( _wcChar ) || IsDigit( _wcChar );
	}

	/**
	 * Determines if the given character is alpha-numeric.
	 *
	 * \param _cChar Character to test.
	 * \return Returns a non-zero value if either IsAlpha() or IsDigit() is true for _cChar, that is, if _cChar is within the ranges A-Z, a-z, or 0-9.
	 */
	LSBOOL LSE_CALL CStd::IsAlNum( char _cChar ) {
		return IsAlpha( _cChar ) || IsDigit( _cChar );
	}

	/**
	 * Determines if the given NULL-terminated string is a valid identifier.  Identifiers can consist only of characters _, A-Z, a-z, and 0-9,
	 *	and the first character may not be a digit.
	 *
	 * \param _pwcStr The string to check for being an identifier.
	 * \return Returns true if the given string is a valid identifier.
	 */
	LSBOOL LSE_CALL CStd::IsIdentifier( const LSUTFX * _pwcStr ) {
		if ( !_pwcStr ) { return false; }
		if ( IsDigit( _pwcStr[0] ) ) { return false; }
		while ( (*_pwcStr) ) {
			if ( (*_pwcStr) != L'_' && !IsAlNum( (*_pwcStr) ) ) { return false; }
			++_pwcStr;
		}
		return true;
	}

	/**
	 * Determines if the given NULL-terminated string is a valid identifier.  Identifiers can consist only of characters _, A-Z, a-z, and 0-9,
	 *	and the first character may not be a digit.
	 *
	 * \param _pcStr The string to check for being an identifier.
	 * \return Returns true if the given string is a valid identifier.
	 */
	LSBOOL LSE_CALL CStd::IsIdentifier( const char * _pcStr ) {
		if ( !_pcStr ) { return false; }
		if ( IsDigit( _pcStr[0] ) ) { return false; }
		while ( (*_pcStr) ) {
			if ( (*_pcStr) != '_' && !IsAlNum( (*_pcStr) ) ) { return false; }
			++_pcStr;
		}
		return true;
	}

	/**
	 * Copy memory.  Does not check for overlap.  If memory can overlap, use MemMove() instead.
	 * The destination buffer must be large enough to hold its new contents.
	 *
	 * \param _pvDst Destination buffer.
	 * \param _pvSrc Buffer from which to copy.
	 * \param _uiptrSize Number of bytes to copy.
	 * \return Returns the destination buffer.
	 */
	void * LSE_CALL CStd::MemCpy( void * _pvDst, const void * _pvSrc, uintptr_t _uiptrSize ) {
#ifdef __LSE_CRT_MEMFUNCS__
		return ::memcpy( _pvDst, _pvSrc, _uiptrSize );
#else
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		//
		// We copy in 3 stages.
		// #1: Get onto an X-byte boundary.
		uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(_pvDst);
		const uint8_t * pui8Source = reinterpret_cast<const uint8_t *>(_pvSrc);
		uintptr_t uiptrX = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Dest) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		uintptr_t uiptrY = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Source) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		if ( uiptrX != uiptrY ) {
			// It is impossible to get both of them on an X-byte boundary, so it is not safe to
			//	perform X-byte operations.  Just do the slow 1-byte operations.
			for ( register uintptr_t I = 0; I < _uiptrSize; ++I ) {
				(*pui8Dest++) = (*pui8Source++);
			}
			return _pvDst;
		}
		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		_uiptrSize -= uiptrX;
		// Basic byte copy.
		for ( ; uiptrX > 0; --uiptrX ) {
			(*pui8Dest++) = (*pui8Source++);
		}

		// #2: Copy X bytes at a time until such a copy is too large.
		// pui8Dest is now aligned on an X-byte boundary, so just step in units of X.
		uintptr_t * puiptrDest = reinterpret_cast<uintptr_t *>(pui8Dest);
		const uintptr_t * puiptrSource = reinterpret_cast<const uintptr_t *>(pui8Source);
		for ( ; _uiptrSize >= sizeof( uintptr_t ); _uiptrSize -= sizeof( uintptr_t ) ) {
			(*puiptrDest++) = (*puiptrSource++);
		}

		// #3: Copy any trailing bytes.
		pui8Dest = reinterpret_cast<uint8_t *>(puiptrDest);
		pui8Source = reinterpret_cast<const uint8_t *>(puiptrSource);
		for ( ; _uiptrSize > 0; --_uiptrSize ) {
			(*pui8Dest++) = (*pui8Source++);
		}

		return _pvDst;
#endif	// #ifdef __LSE_CRT_MEMFUNCS__
	}

	/**
	 * Copy overlapping memory.  The destination buffer must be large enough to hold its new contents.
	 *
	 * \param _pvDst Destination buffer.
	 * \param _pvSrc Buffer from which to copy.
	 * \param _uiptrSize Number of bytes to copy.
	 * \return Returns the destination buffer.
	 */
	void * LSE_CALL CStd::MemMove( void * _pvDst, const void * _pvSrc, uintptr_t _uiptrSize ) {
#ifdef __LSE_CRT_MEMFUNCS__
		return ::memmove( _pvDst, _pvSrc, _uiptrSize );
#else
		uintptr_t uiptrDest = reinterpret_cast<uintptr_t>(_pvDst);
		uintptr_t uiptrSource = reinterpret_cast<uintptr_t>(_pvSrc);
		if ( uiptrDest < uiptrSource || uiptrDest >= uiptrSource + _uiptrSize ) {
			return MemCpy( _pvDst, _pvSrc, _uiptrSize );
		}

		// Copy downwards to prevent propagation.
		// #1: Go to the nearest X-byte boundary.
		uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(_pvDst) + _uiptrSize;
		const uint8_t * pui8Source = reinterpret_cast<const uint8_t *>(_pvSrc) + _uiptrSize;
		uintptr_t uiptrX = reinterpret_cast<uintptr_t>(pui8Dest) & (sizeof( uintptr_t ) - 1);
		uintptr_t uiptrY = reinterpret_cast<uintptr_t>(pui8Source) & (sizeof( uintptr_t ) - 1);
		if ( uiptrX != uiptrY ) {
			// It is impossible to get both of them on an X-byte boundary, so it is not safe to
			//	perform X-byte operations.  Just do the slow 1-byte operations.
			for ( register uintptr_t I = 0; I < _uiptrSize; ++I ) {
				(*--pui8Dest) = (*--pui8Source);
			}
			return _pvDst;
		}
		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		_uiptrSize -= uiptrX;
		// Basic byte copy.
		for ( ; uiptrX > 0; --uiptrX ) {
			(*--pui8Dest) = (*--pui8Source);
		}

		// #2: Copy X bytes at a time until such a copy is too large.
		// pui8Dest is now aligned on an X-byte boundary, so just step in units of X.
		uintptr_t * puiptrDest = reinterpret_cast<uintptr_t *>(pui8Dest);
		const uintptr_t * puiptrSource = reinterpret_cast<const uintptr_t *>(pui8Source);
		for ( ; _uiptrSize >= sizeof( uintptr_t ); _uiptrSize -= sizeof( uintptr_t ) ) {
			(*--puiptrDest) = (*--puiptrSource);
		}

		// #3: Copy any trailing bytes.
		pui8Dest = reinterpret_cast<uint8_t *>(puiptrDest);
		pui8Source = reinterpret_cast<const uint8_t *>(puiptrSource);
		for ( ; _uiptrSize > 0; --_uiptrSize ) {
			(*--pui8Dest) = (*--pui8Source);
		}


		return _pvDst;
#endif	// #ifdef __LSE_CRT_MEMFUNCS__
	}

#if !(defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ ))
	/**
	 * Compare the memory at one location with the memory at another location.  Returns only true or
	 *	false.  Does not perform a lexicographic comparison.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns true if the buffers' contents are an exact match, false otherwise.
	 */
	LSBOOL LSE_CALL CStd::MemCmpF( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		//
		// We compare in 3 stages.
		// #1: Get onto an X-byte boundary.
		const uint8_t * pui8Dest = reinterpret_cast<const uint8_t *>(_pvBuf0);
		const uint8_t * pui8Source = reinterpret_cast<const uint8_t *>(_pvBuf1);
		uintptr_t uiptrX = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Dest) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		uintptr_t uiptrY = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Source) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		if ( uiptrX != uiptrY ) {
			// It is impossible to get both of them on an X-byte boundary, so it is not safe to
			//	perform X-byte operations.  Just do the slow 1-byte operations.
			for ( register uintptr_t I = 0; I < _uiptrSize; ++I ) {
				if ( (*pui8Dest++) != (*pui8Source++) ) { return false; }
			}
			return true;
		}

		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		// uiptrX = bytes until next aligned address.
		// Basic byte compare.
		for ( register uintptr_t I = 0; I < uiptrX; ++I ) {
			if ( (*pui8Dest++) != (*pui8Source++) ) { return false; }
		}
		if ( uiptrX == _uiptrSize ) { return true; }


		// #2: Compare in chunks of X bytes.
		uiptrY = (_uiptrSize - uiptrX) >> (4 - 8 / sizeof( uintptr_t ));
		// uiptrY = total full x-byte chunks remaining.
		const uintptr_t * puiptrDest = reinterpret_cast<const uintptr_t *>(pui8Dest);
		const uintptr_t * puiptrSource = reinterpret_cast<const uintptr_t *>(pui8Source);
		for ( register uintptr_t I = 0; I < uiptrY; ++I ) {
			if ( (*puiptrDest++) != (*puiptrSource++) ) { return false; }
		}

		// #3: Finish in single-byte steps.
		uintptr_t uiptrZ = _uiptrSize - uiptrX - (uiptrY << (4 - 8 / sizeof( uintptr_t )));
		pui8Dest = reinterpret_cast<const uint8_t *>(puiptrDest);
		pui8Source = reinterpret_cast<const uint8_t *>(puiptrSource);
		for ( register uintptr_t I = 0; I < uiptrZ; ++I ) {
			if ( (*pui8Dest++) != (*pui8Source++) ) { return false; }
		}

		return true;
	}
#endif	// #if !(defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ ))

#if !(defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ ))
	/**
	 * Compare the memory at one location with the memory at another location.  Performs a lexicographic comparison.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns the lexicographic relationship between the given buffers.
	 */
	int32_t LSE_CALL CStd::MemCmp( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		//
		// We compare in 3 stages.
		// #1: Get onto an X-byte boundary.
		const uint8_t * pui8Dest = reinterpret_cast<const uint8_t *>(_pvBuf0);
		const uint8_t * pui8Source = reinterpret_cast<const uint8_t *>(_pvBuf1);
		uintptr_t uiptrX = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Dest) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		uintptr_t uiptrY = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Source) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		if ( uiptrX != uiptrY ) {
			// It is impossible to get both of them on an X-byte boundary, so it is not safe to
			//	perform X-byte operations.  Just do the slow 1-byte operations.
			for ( register uintptr_t I = 0; I < _uiptrSize; ++I ) {
				if ( (*pui8Dest) < (*pui8Source) ) { return -1; }
				if ( (*pui8Dest++) > (*pui8Source++) ) { return 1; }
			}
			return true;
		}

		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		// uiptrX = bytes until next aligned address.
		// Basic byte compare.
		for ( register uintptr_t I = uiptrX; I--; ) {
			if ( (*pui8Dest) < (*pui8Source) ) { return -1; }
			if ( (*pui8Dest++) > (*pui8Source++) ) { return 1; }
		}
		if ( uiptrX == _uiptrSize ) { return true; }


		// #2: Compare in chunks of X bytes.
		uiptrY = (_uiptrSize - uiptrX) >> (4 - 8 / sizeof( uintptr_t ));
		// uiptrY = total full x-byte chunks remaining.
		const uintptr_t * puiptrDest = reinterpret_cast<const uintptr_t *>(pui8Dest);
		const uintptr_t * puiptrSource = reinterpret_cast<const uintptr_t *>(pui8Source);
		for ( register uintptr_t I = uiptrY; I--; ) {
			if ( (*puiptrDest) != (*puiptrSource) ) {
				pui8Dest = reinterpret_cast<const uint8_t *>(puiptrDest);
				pui8Source = reinterpret_cast<const uint8_t *>(puiptrSource);
				for ( uint32_t J = 0; J < sizeof( uintptr_t ); ++J ) {
					if ( (*pui8Dest) < (*pui8Source) ) { return -1; }
					if ( (*pui8Dest++) > (*pui8Source++) ) { return 1; }
				}
			}
			puiptrDest++;
			puiptrSource++;
		}

		// #3: Finish in single-byte steps.
		uintptr_t uiptrZ = _uiptrSize - uiptrX - (uiptrY << (4 - 8 / sizeof( uintptr_t )));
		pui8Dest = reinterpret_cast<const uint8_t *>(puiptrDest);
		pui8Source = reinterpret_cast<const uint8_t *>(puiptrSource);
		for ( register uintptr_t I = uiptrZ; I--; ) {
			if ( (*pui8Dest) < (*pui8Source) ) { return -1; }
			if ( (*pui8Dest++) > (*pui8Source++) ) { return 1; }
		}

		return 0;
	}
#endif	// #if !(defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ ))

	/**
	 * Fill memory with a given byte value.
	 *
	 * \param _pvDst Buffer to fill.
	 * \param _ui8Value Value with which to fill the buffer.
	 * \param _uiptrSize Number of bytes to fill.
	 */
	void LSE_CALL CStd::MemSet( void * _pvDst, uint8_t _ui8Value, uintptr_t _uiptrSize ) {
#ifdef __LSE_CRT_MEMFUNCS__
		::memset( _pvDst, _ui8Value, _uiptrSize );
		return;
#else
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		//
		// We compare in 3 stages.
		// #1: Get onto an X-byte boundary.
		uint8_t * pui8Dest = reinterpret_cast<uint8_t *>(_pvDst);
		uintptr_t uiptrX = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(pui8Dest) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		// uiptrX = bytes until next aligned address.
		// Basic byte compare.
		for ( register uintptr_t I = 0; I < uiptrX; ++I ) {
			(*pui8Dest++) = _ui8Value;
		}


		// #2: Set in chunks of X bytes.
		uintptr_t uiptrY = (_uiptrSize - uiptrX) >> (4 - 8 / sizeof( uintptr_t ));
		// uiptrY = total full x-byte chunks remaining.
		uintptr_t * puiptrDest = reinterpret_cast<uintptr_t *>(pui8Dest);
		if ( uiptrY ) {
			union {
				uintptr_t	uiptrValue;
				uint8_t		ui8Value[sizeof( uintptr_t )];
			} uValue;
			uValue.uiptrValue = static_cast<uintptr_t>((_ui8Value << 8) | _ui8Value);
			uValue.uiptrValue |= uValue.uiptrValue << 16;
			for ( register uintptr_t I = sizeof( uint32_t ); I < sizeof( uintptr_t ); ++I ) {
				uValue.ui8Value[I] = _ui8Value;
			}
			for ( register uintptr_t I = 0; I < uiptrY; ++I ) {
				(*puiptrDest++) = uValue.uiptrValue;
			}
		}

		// #3: Finish in single-byte steps.
		uintptr_t uiptrZ = _uiptrSize - uiptrX - (uiptrY << (4 - 8 / sizeof( uintptr_t )));
		pui8Dest = reinterpret_cast<uint8_t *>(puiptrDest);
		for ( register uintptr_t I = 0; I < uiptrZ; ++I ) {
			(*pui8Dest++) = _ui8Value;
		}

#endif	// #ifdef __LSE_CRT_MEMFUNCS__
	}

	/**
	 * Searches within the first _uiptrSize bytes of the block of memory to which _pvBuffer points for the first occurrence of _ui8Value, and returns a pointer to it.
	 *
	 * \param _pvBuffer The buffer to search.
	 * \param _ui8Value The value to find.
	 * \param _uiptrSize Maximum number of bytes to search.
	 * \return Returns a pointer to the found byte or NULL if it was not found.
	 */
	const void * LSE_CALL CStd::MemChr( const void * _pvBuffer, uint8_t _ui8Value, uintptr_t _uiptrSize ) {
#ifdef __LSE_CRT_MEMFUNCS__
		return ::memchr( _pvBuffer, _ui8Value, _uiptrSize );
#else
		// X = bytes-per-address on the current operating system.  We use uintptr_t
		//	for this.
		//
		// We search in 3 stages.
		// #1: Get onto an X-byte boundary.
		const uint8_t * pui8Src = reinterpret_cast<const uint8_t *>(_pvBuffer);
		uintptr_t uiptrX = ((sizeof( uintptr_t ) - reinterpret_cast<uintptr_t>(_pvBuffer) & (sizeof( uintptr_t ) - 1)) & (sizeof( uintptr_t ) - 1));
		if ( uiptrX > _uiptrSize ) { uiptrX = _uiptrSize; }
		// uiptrX = bytes until next aligned address.
		// Basic byte compare.
		for ( register uintptr_t I = 0; I < uiptrX; ++I ) {
			if ( (*pui8Src) == _ui8Value ) { return pui8Src; }
			++pui8Src;
		}


		// #2: Check X bytes at a time.
		// Start by spreading the single-byte value across all of the bytes of the uintptr_t type, which can
		//	be either 4 bytes or 8 bytes.
		uintptr_t uiptrY = (_uiptrSize - uiptrX) >> (4 - 8 / sizeof( uintptr_t ));
		if ( uiptrY ) {
			uintptr_t uiptrFindMe = static_cast<uintptr_t>((_ui8Value << 8) | _ui8Value);
			uiptrFindMe |= (uiptrFindMe << 16);
			// Loop should be optimized away in 32-bit builds.
			for ( register uint32_t I = sizeof( uintptr_t ); I > sizeof( uint32_t ); I -= 2 ) {
				uiptrFindMe |= (uiptrFindMe << 16);
			}

			// Now check.
			for ( register uintptr_t I = 0; I < uiptrY; ++I ) {
				// This gets messy without #defines.
#define LSSTD_CHECKCHAR		((*reinterpret_cast<const uintptr_t *>(pui8Src)) ^ uiptrFindMe)
#define LSSTD_CHECKME		((LSSTD_CHECKCHAR - static_cast<uintptr_t>(0x0101010101010101ULL)) & ~LSSTD_CHECKCHAR & static_cast<uintptr_t>(0x8080808080808080ULL))
				if ( LSSTD_CHECKME ) { break; }	// Do not return, just break and let stage 3 handle it.
				pui8Src += sizeof( uintptr_t );
#undef LSSTD_CHECKME
#undef LSSTD_CHECKCHAR
			}
		}


		// #3: Finish in single-byte steps.
		const uint8_t * pui8End = reinterpret_cast<const uint8_t *>(_pvBuffer) + _uiptrSize;
		while ( pui8Src != pui8End ) {
			if ( (*pui8Src) == _ui8Value ) { return pui8Src; }
			++pui8Src;
		}

		return NULL;

#endif	// #ifdef __LSE_CRT_MEMFUNCS__
	}

	/**
	 * Searches within the first _uiptrSize bytes of the block of memory to which _pvBuffer points for the first occurrence of _ui8Value, and returns a pointer to it.
	 *
	 * \param _pvBuffer The buffer to search.
	 * \param _ui8Value The value to find.
	 * \param _uiptrSize Maximum number of bytes to search.
	 * \return Returns a pointer to the found byte or NULL if it was not found.
	 */
	void * LSE_CALL CStd::MemChr( void * _pvBuffer, uint8_t _ui8Value, uintptr_t _uiptrSize ) {
		return const_cast<void *>(MemChr( const_cast<const void *>(_pvBuffer), _ui8Value, _uiptrSize ));
	}

	/**
	 * Print formatted text to a string.
	 *
	 * \param _pwcDest Storage location for output.
	 * \param _uiptrMaxLen Maximum number of characters to store.
	 * \param _pwcFormat Format-control string.
	 * \return Returns the number of characters printed, not including the
	 *	terminating NULL.
	 */
	uint32_t LSE_CALL CStd::WSPrintF( LSUTFX * _pwcDest, uintptr_t _uiptrMaxLen, const LSUTFX * _pwcFormat, ... ) {
		va_list lList;
		va_start( lList, _pwcFormat );
		uint32_t ui32Ret = static_cast<uint32_t>(::vswprintf( _pwcDest, _uiptrMaxLen, _pwcFormat, lList ));
		va_end( lList );
		return ui32Ret;
	}

	/**
	 * Print formatted text to a string.
	 *
	 * \param _pcDest Storage location for output.
	 * \param _uiptrMaxLen Maximum number of characters to store.
	 * \param _pcFormat Format-control string.
	 * \return Returns the number of characters printed, not including the
	 *	terminating NULL.
	 */
	uint32_t LSE_CALL CStd::SPrintF( char * _pcDest, uintptr_t _uiptrMaxLen, const char * _pcFormat, ... ) {
		va_list lList;
		va_start( lList, _pcFormat );
#ifdef LSE_WINDOWS
		uint32_t ui32Ret = static_cast<uint32_t>(::vsnprintf_s( _pcDest, _uiptrMaxLen, _uiptrMaxLen - 1, _pcFormat, lList ));
#else
		uint32_t ui32Ret = static_cast<uint32_t>(::vsnprintf( _pcDest, _uiptrMaxLen, _pcFormat, lList ));
#endif	// #ifdef LSE_WINDOWS
		va_end( lList );
		return ui32Ret;
	}
	
	/**
	 * Sleep for the given number of milliseconds.  Passing 0 causes the thread to relinquish
	 *	the remainder of its time until it is scheduled to run again, at which time it will continue.
	 *
	 * \param _ui32Milliseconds Milliseconds to sleep.
	 */
	void LSE_CALL CStd::Sleep( uint32_t _ui32Milliseconds ) {
#ifdef LSE_WINDOWS
		::Sleep( _ui32Milliseconds );
#else
		// Posix makes this a bit annoying.  Oh well.
		// Manually check for 0 (sigh).
		if ( _ui32Milliseconds == 0 ) {
			// Give up control.
			::sched_yield();
		}
		else {
			// Sleep for the given period of time.
			timespec tsTime;
			tsTime.tv_sec = static_cast<__darwin_time_t>(_ui32Milliseconds / 1000UL);
			tsTime.tv_nsec = static_cast<long>((_ui32Milliseconds % 1000UL) * 1000000UL);
			::nanosleep( &tsTime, NULL );
			// Whew!
		}
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Print a wide-character string to the debug console.
	 *
	 * \param _pwcString String to print.
	 */
	void LSE_CALL CStd::DebugPrintW( const LSUTFX * _pwcString ) {
#ifdef LSE_DEBUG
#if defined( LSE_WINDOWS )
		::OutputDebugStringW( _pwcString );
#else
		::wprintf( _pwcString );
#endif	// #if defined( LSE_WINDOWS )
#endif	// #ifdef LSE_DEBUG
	}

	/**
	 * Print a UTF-8 string to the debug console.
	 *
	 * \param _pcString String to print.
	 */
	void LSE_CALL CStd::DebugPrintA( const char * _pcString ) {
#ifdef LSE_DEBUG
		// Do not allocate for every string.  Try to use a pre-defined buffer for short ones.
		LSUTFX uBuffer[512];
		uint32_t ui32Len;
		ui32Len = CStd::Utf8StrLenAsUtfX( reinterpret_cast<const LSUTF8 *>(_pcString) );
		if ( ui32Len < LSE_ELEMENTS( uBuffer ) ) {
			CStd::Utf8ToUtfX( uBuffer, reinterpret_cast<const LSUTF8 *>(_pcString), ui32Len );
			DebugPrintW( uBuffer );
			return;
		}
		LSUTFX * puBuffer = new LSUTFX[ui32Len];
		CStd::Utf8ToUtfX( puBuffer, reinterpret_cast<const LSUTF8 *>(_pcString), ui32Len );
		DebugPrintW( puBuffer );
		delete [] puBuffer;
#endif	// #ifdef LSE_DEBUG
	}

	/**
	 * Prints an error given an error code.  LSSTD_E_SUCCESS prints nothing.
	 *
	 * \param _eError The error code to print to the debug console.
	 */
	void LSE_CALL CStd::PrintError( LSSTD_ERRORS _eError ) {
		if ( _eError == LSSTD_E_SUCCESS ) { return; }

		static const char * pcTable[] = {
			"",														// LSSTD_E_SUCCESS
			"Out of memory.",										// LSSTD_E_OUTOFMEMORY
			"File not found.",										// LSSTD_E_FILENOTFOUND
			"Invalid write permissions.",							// LSSTD_E_INVALIDWRITEPERMISSIONS
			"Not enough disk space for file write operation.",		// LSSTD_E_NODISKSPACE
			"File exists but is in an unexpected format.",			// LSSTD_E_INVALIDFILETYPE
			"Invalid call.",										// LSSTD_E_INVALIDCALL
			"Invalid data.",										// LSSTD_E_INVALIDDATA
			"Internal error.",										// LSSTD_E_INTERNALERROR
			"Feature not yet supported.",							// LSSTD_E_FEATURENOTSUPPORTED
			"One or more tasks failed.",							// LSSTD_E_PARTIALFAILURE
			"Invalid version.",										// LSSTD_E_BADVERSION
			"File overflow.",										// LSSTD_E_FILEOVERFLOW
			"File write error.",									// LSSTD_E_FILEWRITEERROR
			"Bad data format.",										// LSSTD_E_BADFORMAT
		};
		DebugPrintA( pcTable[_eError] );
	}

	/**
	 * Converts a 32-bit float to text.
	 *
	 * \param _pcOutBuffer The output buffer, which must be at least _ui32MaxLen characters in length.  The buffer is always
	 *	terminated with a NULL character unless _ui32MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _fVal The value to print.
	 * \return Returns the input string.
	 */
	char * LSE_CALL CStd::PrintFloat32( char * _pcOutBuffer, uint32_t _ui32MaxLen, float _fVal ) {
		return CStd::FtoA<23, 8, 1, 32, char>( _pcOutBuffer, _ui32MaxLen, reinterpret_cast<const uint8_t *>(&_fVal) );
	}

	/**
	 * Converts a 64-bit float to text.
	 *
	 * \param _pcOutBuffer The output buffer, which must be at least _ui64MaxLen characters in length.  The buffer is always
	 *	terminated with a NULL character unless _ui64MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _dVal The value to print.
	 * \return Returns the input string.
	 */
	char * LSE_CALL CStd::PrintFloat64( char * _pcOutBuffer, uint32_t _ui32MaxLen, double _dVal ) {
		return CStd::FtoA<52, 11, 1, 64, char>( _pcOutBuffer, _ui32MaxLen, reinterpret_cast<const uint8_t *>(&_dVal) );
	}

	/**
	 * Converts a 80-bit float to text.
	 *
	 * \param _pcOutBuffer The output buffer, which must be at least _ui64MaxLen characters in length.  The buffer is always
	 *	terminated with a NULL character unless _ui64MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _pui8Value Pointer to the 10-byte value to print.
	 * \return Returns the input string.
	 */
	char * LSE_CALL CStd::PrintFloat80( char * _pcOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value ) {
		return CStd::FtoA<64, 15, 0, 80, char>( _pcOutBuffer, _ui32MaxLen, _pui8Value );
	}

	/**
	 * Converts a floating-point number to text.
	 *
	 * \param _ptOutBuffer The output buffer, which must be at least _ui32MaxLen characters in length.  The buffer is always
	 *	terminated with a NULL character unless _ui32MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _pui8Value The floating-point value to decode, which can be of variable sizes in this templated function.
	 * \param _ui32Precision The precision to use in decoding the value.  The usage changes depending on _i8Format.
	 * \param _i8Format Must be 'g', 'G', 'e', 'E', or 'f'.
	 * \return Returns _ptOutBuffer after decoding the value into it.
	 */
	template <unsigned _uLookUpBits, unsigned _uExpPos, unsigned _uBias, unsigned _uTotalBits, typename _tCharType>
	_tCharType * LSE_CALL CStd::FtoA( _tCharType * _ptOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value, uint32_t _ui32Precision, int8_t /*_i8Format*/ ) {
		// We have a challenge here.
		//	_pui8Value can be any size, including over 64 bits (this function supports 80-bit values as well).  So we cannot
		//	simply cast to uint64_t and use bit operations to get the bits we need.
		// We define a few macros for this.
		typedef CLargeInteger<((1 << _uLookUpBits) + (_uExpPos << 1)) / 32>	LSSTD_LARGE_TYPE;
		
		// Get the byte and bit combination needed to access the given bit index.
#define LSSTD_GET_BYTE_FROM_BIT( BIT )		((BIT) >> 3)
#define LSSTD_GET_BIT_FROM_BIT( BIT )		((BIT) & 7UL)
		
		// Get a bit.
#define LSSTD_GET_BIT( BIT )				((_pui8Value[LSSTD_GET_BYTE_FROM_BIT( BIT )] >> LSSTD_GET_BIT_FROM_BIT( BIT )) & 1)

		// Get multiple bits.  Must pass a CLargeInteger type for RET.
#define LSSTD_GET_BITS( FROM, TOTAL, RET ) {	uint64_t ui64Val = 0ULL;											\
			for ( uint32_t I = (TOTAL); I--; ) {																	\
				ui64Val <<= 1ULL;																					\
				ui64Val |= static_cast<uint64_t>(LSSTD_GET_BIT( I + (FROM) ));										\
			}																										\
			(RET) = ui64Val;																						\
		}

		// The buffer may actually be NULL, in which case we return the length of the string if it were printed.
		// Another macro to handle appending characters.  It also does bounds checking.
		uint32_t ui32CurPos = 0;
#define LSSTD_APP_CHAR( THISCHAR )			if ( _ptOutBuffer ) {								\
				if ( ui32CurPos != _ui32MaxLen ) {												\
					_ptOutBuffer[ui32CurPos++] = static_cast<_tCharType>(THISCHAR);				\
				}																				\
			}																					\
			else { ++ui32CurPos; }

		// And to cap the buffer (if it exists).
#define LSSTD_CAP_BUFFER()					LSSTD_APP_CHAR( 0 )

		// And finally to return, we need to return the actual number of characters if _ptOutBuffer is NULL.
#define LSSTD_RET							LSSTD_CAP_BUFFER(); return _ptOutBuffer ? _ptOutBuffer : reinterpret_cast<_tCharType *>(static_cast<uintptr_t>(ui32CurPos))

		// Some values we need frequently in order to avoid constant re-construction.
		LSSTD_LARGE_TYPE liOne( 1ULL );
		LSSTD_LARGE_TYPE liTen( 10ULL );

		// Get the values we need.
		int32_t i32Sign = LSSTD_GET_BIT( _uTotalBits - 1 );
		LSSTD_LARGE_TYPE liExponent;
		LSSTD_GET_BITS( _uExpPos, _uLookUpBits, liExponent );
		LSSTD_LARGE_TYPE liMantissa;
		LSSTD_GET_BITS( 0, _uExpPos, liMantissa );

		// Print the sign value.
		if ( i32Sign ) {
			LSSTD_APP_CHAR( '-' );
		}

		// Check for special numbers.
		if ( liExponent == (1ULL << _uLookUpBits) - 1ULL ) {
			// If the fraction is 0, this is infinity.
			if ( liMantissa == 0ULL ) {
				static const _tCharType szBuffer[] = {
					'I',
					'n',
					'F',
				};
				for ( uint32_t I = 0; I < LSE_ELEMENTS( szBuffer ); ++I ) {
					LSSTD_APP_CHAR( szBuffer[I] );
				}

				LSSTD_RET;
			}
			else {
				// It is NaN.
				static const _tCharType szBuffer[] = {
					'N',
					'a',
					'N',
				};
				for ( uint32_t I = 0; I < LSE_ELEMENTS( szBuffer ); ++I ) {
					LSSTD_APP_CHAR( szBuffer[I] );
				}

				LSSTD_RET;
			}
		}

		// Decode the exponent.  Do it first because we will adjust it for denormalized numbers.
		LSSTD_LARGE_TYPE liDecPos( liExponent - LSSTD_LARGE_TYPE( static_cast<uint64_t>(_uBias) ) );


		if ( liExponent == 0ULL ) {
			// Handle 0.
			if ( liMantissa == 0ULL ) {
				static const _tCharType szBuffer[] = {
					'0',
				};
				for ( uint32_t I = 0; I < LSE_ELEMENTS( szBuffer ); ++I ) {
					LSSTD_APP_CHAR( szBuffer[I] );
				}
				if ( _ui32Precision ) {
					LSSTD_APP_CHAR( '.' );
					while ( _ui32Precision-- ) {
						LSSTD_APP_CHAR( '0' );
					}
				}
				LSSTD_RET;
			}
			else {
				// Handle denormalized numbers.
				liDecPos = -(_uBias - 1LL);
			}
		}
		else {
			// Not 0 and not denormalized.
			//	Add the implicit bit to the mantissa.
			liMantissa |= (1ULL << _uExpPos);
		}


		/*uint64_t ui64Mantissa = liMantissa.GetAs64bitUInt();
		uint64_t ui32Digit0 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit0 << _uExpPos));
		uint64_t ui32Digit1 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit1 << _uExpPos));
		uint64_t ui32Digit2 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit2 << _uExpPos));
		uint64_t ui32Digit3 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit3 << _uExpPos));
		uint64_t ui32Digit4 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit4 << _uExpPos));
		uint64_t ui32Digit5 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit5 << _uExpPos));
		uint64_t ui32Digit6 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit6 << _uExpPos));
		uint64_t ui32Digit7 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit7 << _uExpPos));
		uint64_t ui32Digit8 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit8 << _uExpPos));
		uint64_t ui32Digit9 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit9 << _uExpPos));
		uint64_t ui32Digit10 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit10 << _uExpPos));
		uint64_t ui32Digit11 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit11 << _uExpPos));
		uint64_t ui32Digit12 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit12 << _uExpPos));
		uint64_t ui32Digit13 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit13 << _uExpPos));
		uint64_t ui32Digit14 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit14 << _uExpPos));
		uint64_t ui32Digit15 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit15 << _uExpPos));
		uint64_t ui32Digit16 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit16 << _uExpPos));
		uint64_t ui32Digit17 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit17 << _uExpPos));
		uint64_t ui32Digit18 = (ui64Mantissa >> _uExpPos);
		ui64Mantissa = 10 * (ui64Mantissa - (ui32Digit18 << _uExpPos));*/

		LSSTD_LARGE_TYPE liIntPart;
		LSSTD_LARGE_TYPE liFracPart;
		LSSTD_LARGE_TYPE liDiv;
		uint32_t ui32MaxPrec;
		if ( liDecPos >= LSSTD_LARGE_TYPE( static_cast<uint64_t>(_uExpPos) ) ) {
			// Bits are shifted so high there is no fractional part.
			liIntPart = liMantissa << (liDecPos - static_cast<uint64_t>(_uExpPos)).GetAs32bitUInt();
			liFracPart = 0ULL;
			liDiv = liOne;
			ui32MaxPrec = 0ULL;
		}
		else if ( liDecPos >= 0LL ) {
			// There is a fraction and an integer.
			liIntPart = liMantissa >> (LSSTD_LARGE_TYPE( static_cast<uint64_t>(_uExpPos) ) - liDecPos).GetAs32bitUInt();
			liFracPart = (liMantissa << (liDecPos + liOne).GetAs32bitUInt()) & ((1ULL << (_uExpPos + 1ULL)) - 1ULL);
			liDiv = liOne << (_uExpPos + 1ULL);
			ui32MaxPrec = ((liDecPos + 1ULL).GetAs32bitUInt() + _uExpPos + 1ULL);
		}
		else {
			// Bits shifted down.  No integral part.
			liIntPart = 0ULL;
			liFracPart = (liMantissa & ((1ULL << (_uExpPos + 1ULL)) - 1ULL));
			liDiv = (liOne << (_uExpPos + 1ULL)) * (liOne << (-liDecPos - liOne).GetAs32bitUInt());
			ui32MaxPrec = ((-liDecPos - liOne).GetAs32bitUInt() + _uExpPos + 1ULL);
		}

		// Print the integral part first.
		ui32CurPos += liIntPart.ToString( _ptOutBuffer, _ui32MaxLen - ui32CurPos, 10 );

		uint32_t ui32Printed = 0;
		if ( liFracPart != 0ULL ) {
			LSSTD_APP_CHAR( '.' );

			LSSTD_LARGE_TYPE liCopy( liFracPart );

			
			// Keep going until there is an overflow.
			try {
				uint32_t ui32Prec = CStd::Min( _ui32Precision, ui32MaxPrec );
				while ( ui32Prec-- ) {
					liCopy *= liTen;
					LSSTD_APP_CHAR( ((liCopy / liDiv) % liTen).GetAs32bitUInt() + '0' );
					++ui32Printed;
					if ( _ptOutBuffer && ui32CurPos == _ui32MaxLen ) {
						break;
					}
				}
			}
			catch ( ... ) {
			}

		}
		else if ( _ui32Precision ) {
			LSSTD_APP_CHAR( '.' );
		}

		// Fill the remaining precision with 0's.
		while ( ui32Printed++ < _ui32Precision ) {
			LSSTD_APP_CHAR( '0' );
		}
		/*if ( !ui32Printed ) {
			LSSTD_APP_CHAR( '0' );
		}*/

		LSSTD_RET;
#undef LSSTD_RET
#undef LSSTD_CAP_BUFFER
#undef LSSTD_APP_CHAR
#undef LSSTD_GET_BITS
#undef LSSTD_GET_BIT
#undef LSSTD_GET_BIT_FROM_BIT
#undef LSSTD_GET_BYTE_FROM_BIT
	}

	/**
	 * Converts a floating-point number to text.  
	 *
	 * \param _ptOutBuffer The output buffer, which must be at least _ui32MaxLen characters in length.  The buffer is always
	 *	terminated with a NULL character unless _ui32MaxLen is 0.
	 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
	 * \param _pui8Value The floating-point value to decode, which can be of variable sizes in this templated function.
	 * \return Returns _ptOutBuffer after decoding the value into it.
	 */
	template <unsigned _uMantissaBits, unsigned _uExpBits, unsigned _uImplicitBit, unsigned _uTotalBits, typename _tCharType>
	_tCharType * LSE_CALL CStd::FtoA( _tCharType * _ptOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value ) {
		typedef CLargeInteger<(_uMantissaBits + 1 + ((1 << (_uExpBits - 1)) - 1)) / 16>	LSSTD_LARGE_TYPE;
		const int32_t i32ExpOffset = (1 << (_uExpBits - 1)) - 1;

		// Get the byte and bit combination needed to access the given bit index.
#define LSSTD_GET_BYTE_FROM_BIT( BIT )		((BIT) >> 3)	// BIT / 8.
#define LSSTD_GET_BIT_FROM_BIT( BIT )		((BIT) & 7UL)	// BIT % 8.

		// Get a bit.
#define LSSTD_GET_BIT( BIT )				((_pui8Value[LSSTD_GET_BYTE_FROM_BIT( BIT )] >> LSSTD_GET_BIT_FROM_BIT( BIT )) & 1)

		// Get multiple bits.
#define LSSTD_GET_BITS( FROM, TOTAL, RET ) {	uint64_t ui64Val = 0ULL;											\
			for ( uint32_t I = (TOTAL); I--; ) {																	\
				ui64Val <<= 1ULL;																					\
				uint32_t ui32Index = I + (FROM);																	\
				ui64Val |= static_cast<uint64_t>(LSSTD_GET_BIT( ui32Index ));										\
			}																										\
			(RET) = ui64Val;																						\
		}
		// The buffer may actually be NULL, in which case we return the length of the string if it were printed.
		// Another macro to handle appending characters.  It also does bounds checking.
		uint32_t ui32CurPos = 0;
#define LSSTD_APP_CHAR( THISCHAR )			if ( _ptOutBuffer ) {								\
				if ( ui32CurPos != _ui32MaxLen ) {												\
					_ptOutBuffer[ui32CurPos++] = static_cast<_tCharType>(THISCHAR);				\
				}																				\
			}																					\
			else { ++ui32CurPos; }

		// And to cap the buffer (if it exists).
#define LSSTD_CAP_BUFFER()					LSSTD_APP_CHAR( 0 )

		// And finally to return, we need to return the actual number of characters if _ptOutBuffer is NULL.
#define LSSTD_RET							LSSTD_CAP_BUFFER(); return _ptOutBuffer ? _ptOutBuffer : reinterpret_cast<_tCharType *>(static_cast<uintptr_t>(ui32CurPos + 1))


		// Get the values we need.
		int32_t i32Sign = LSSTD_GET_BIT( _uTotalBits - 1 );
		int64_t i64Exponent;
		LSSTD_GET_BITS( _uMantissaBits, _uExpBits, i64Exponent );
		LSSTD_LARGE_TYPE liMantissa;
		LSSTD_GET_BITS( 0, _uMantissaBits, liMantissa );
		
		// Print the sign value.
		if ( i32Sign ) {
			LSSTD_APP_CHAR( '-' );
		}

		// Check for special numbers.
		if ( i64Exponent == (1 << _uExpBits) - 1 ) {
			// If the fraction is 0, this is infinity.
			if ( liMantissa == 0ULL ) {
				static const _tCharType szBuffer[] = {
					'I',
					'n',
					'F',
				};
				for ( uint32_t I = 0; I < LSE_ELEMENTS( szBuffer ); ++I ) {
					LSSTD_APP_CHAR( szBuffer[I] );
				}

				LSSTD_RET;
			}
			else {
				// It is NaN.
				static const _tCharType szBuffer[] = {
					'N',
					'a',
					'N',
				};
				for ( uint32_t I = 0; I < LSE_ELEMENTS( szBuffer ); ++I ) {
					LSSTD_APP_CHAR( szBuffer[I] );
				}

				LSSTD_RET;
			}
		}

		// Get the fixed-point multiplier.
		LSSTD_LARGE_TYPE liFixedPoint = 1;
		liFixedPoint <<= i32ExpOffset;
		liMantissa <<= 1;	// Mantissa * 2.
		
		if ( i64Exponent ) {
			i64Exponent -= i32ExpOffset;
			// Normalized numbers.
			if ( _uImplicitBit ) {
				liMantissa |= (1ULL << (_uMantissaBits + 1ULL));
			}
			// Go to fixed-point.
			//liMantissa <<= i32ExpOffset;
			liMantissa *= liFixedPoint;
			if ( i64Exponent > 0 ) {
				liMantissa <<= static_cast<uint32_t>(i64Exponent);
			}
			else if ( i64Exponent < 0 ) {
				liMantissa >>= static_cast<uint32_t>(-i64Exponent);
			}
		}
		else {
			// Denormalized.
			// Go to fixed-point by:
			//	liMantissa <<= i32ExpOffset
			//	But handle the denormal by:
			//	liMantissa >>= i32ExpOffset - 1
			// Leaves use with:
			liMantissa <<= 1;
		}

		

		// Values to the left of the decimal.
		LSSTD_LARGE_TYPE liHigh = liMantissa / liFixedPoint;
		liHigh >>= (_uMantissaBits + _uImplicitBit);

		liMantissa = liMantissa - liFixedPoint * (liHigh << (_uMantissaBits + _uImplicitBit));
		
		const LSSTD_LARGE_TYPE liTen = 10;

		LSSTD_LARGE_TYPE liMul = liTen;
		uint32_t ui32PosIters = 1;
		while ( liMul <= liHigh ) {
			liMul *= liTen;
			++ui32PosIters;
		}
		liMul /= liTen;

		while ( ui32PosIters-- ) {
			LSSTD_LARGE_TYPE liTemp = (liHigh / liMul) % liTen;
			liMul /= liTen;
			_tCharType cThis = '0' + static_cast<_tCharType>(liTemp.GetAs32bitUInt());
			LSSTD_APP_CHAR( cThis );
		}
		LSSTD_APP_CHAR( '.' );

		// Values to the right of the decimal.
		LSBOOL bPrinted = false;
		while ( true ) {
			liMantissa = liMantissa * liTen;
			LSSTD_LARGE_TYPE liTemp = (liMantissa) / liFixedPoint;
			liTemp >>= (_uMantissaBits + _uImplicitBit);
			liTemp = liTemp % liTen;
			liMantissa = liMantissa - liFixedPoint * (liTemp << (_uMantissaBits + _uImplicitBit));
			if ( liMantissa != 0 ) {
				_tCharType cThis = '0' + static_cast<_tCharType>(liTemp.GetAs32bitUInt());
				LSSTD_APP_CHAR( cThis );
				bPrinted = true;
			}
			else {
				break;
			}
		}
		if ( !bPrinted ) {
			LSSTD_APP_CHAR( '0' );
		}

		LSSTD_RET;

#undef LSSTD_RET
#undef LSSTD_CAP_BUFFER
#undef LSSTD_APP_CHAR
#undef LSSTD_GET_BIT
#undef LSSTD_GET_BITS
#undef LSSTD_GET_BIT_FROM_BIT
#undef LSSTD_GET_BYTE_FROM_BIT
	}
	
}	// namespace lsstd

#pragma warning( pop )
