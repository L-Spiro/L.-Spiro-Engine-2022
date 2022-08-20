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


#ifndef __LSTL_WSTRING_H__
#define __LSTL_WSTRING_H__

#include "../LSTLib.h"
#include "LSTLStringBase.h"

namespace lstl {

	/**
	 * Class CWString
	 * \brief A standard string class with an optional allocator.
	 *
	 * Description: A standard wide-character string class with an optional allocator.  Simply specializes the
	 *	CStringBase template with a wchar_t.  In this way, this class provides an example of how to create a
	 *	string of any base type (char, wchar_t, unsigned short, etc.)
	 */
	class CWString : public CStringBase<CWString, wchar_t> {
	public :
		// == Various constructors.
		explicit LSE_CALLCTOR						CWString( CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CWString( const char * _pcString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CWString( const wchar_t * _pwcString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CWString( const CWString &_sString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CWString( const CStringBase<CWString, wchar_t> &_sbStringBase, CAllocator * _paAllocator = NULL );


		// == Operators.
		/**
		 * Copy a string.
		 *
		 * \param _pwcString
		 * \return Returns this string after the copy.
		 */
		CWString & LSE_CALL							operator = ( const wchar_t * _pwcString );

		/**
		 * Copy a string.
		 *
		 * \param _wstrString
		 * \return Returns this string after the copy.
		 */
		CWString & LSE_CALL							operator = ( const CWString &_wstrString );
		
		
		// == Functions.
		/**
		 * Converts an unsigned integer to a string.
		 *
		 * \param _ui64Value The value to convert to the string.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns true if there was enough memory to convert the value.
		 */
		LSBOOL LSE_CALL								FromUI64( uint64_t _ui64Value, uint32_t _ui32Radix = 10 );

		/**
		 * Converts a signed integer to a string.
		 *
		 * \param _i64Value The value to convert to the string.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns true if there was enough memory to convert the value.
		 */
		LSBOOL LSE_CALL								FromI64( int64_t _i64Value, uint32_t _ui32Radix = 10 );

		/**
		 * Converts a double to a string.
		 *
		 * \param _dValue The value to convert to the string.
		 * \param _bScientific If true, scientific notation is used to express the number.  A lower-case e is used
		 *	for the exponent.
		 * \return Returns true if there was enough memory to convert the value.
		 */
		LSBOOL LSE_CALL								FromDouble( double _dValue, LSBOOL _bScientific = false );

		/**
		 * Create from a UTF-8 string.
		 *
		 * \param _pu8Src The string from which to create a UTFX CWString() object.
		 * \return Returns the created string.
		 */
		static CWString LSE_CALL					FromUtf8( const LSUTF8 * _pu8Src );

	private :
		typedef CStringBase<CWString, wchar_t>		Parent;
	};

}	// namespace lstl

#endif	// __LSTL_WSTRING_H__
