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


#ifndef __LSTL_STRING_H__
#define __LSTL_STRING_H__

#include "../LSTLib.h"
#include "LSTLStringBase.h"

namespace lstl {

	/**
	 * Class CString
	 * \brief A standard string class with an optional allocator.
	 *
	 * Description: A standard string class with an optional allocator.  Simply specializes the CStringBase
	 *	template with a char.  In this way, this class provides an example of how to create a string of any
	 *	base type (char LSUTFX, unsigned short, etc.)
	 */
	class CString : public CStringBase<CString, char> {
	public :
		// == Various constructors.
		explicit LSE_CALLCTOR						CString( CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CString( const char * _pcString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CString( const LSUTFX * _pwcString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CString( const CString &_sString, CAllocator * _paAllocator = NULL );
		LSE_CALLCTOR								CString( const CStringBase<CString, char> &_sbStringBase, CAllocator * _paAllocator = NULL );


		// == Operators.
		/**
		 * Copy a string.
		 *
		 * \param _pcString
		 * \return Returns this string after the copy.
		 */
		CString & LSE_CALL							operator = ( const char * _pcString );

		/**
		 * Copy a string.
		 *
		 * \param _strString
		 * \return Returns this string after the copy.
		 */
		CString & LSE_CALL							operator = ( const CString &_strString );


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
		 * Creates a string from a UTF input string.  Whether the input string is UTF-16 or UTF-32 depends on the platform.
		 *
		 * \param _pwcString The input string to be converted to a UTF-8 CString.
		 * \param _paAllocator Optional allocator.
		 * \return Returns the created CString or an empty string if there is not enough memory.
		 */
		static CString LSE_CALL						CStringFromUtfX( const LSUTFX * _pwcString, CAllocator * _paAllocator = NULL );

		/**
		 * Creates a new string from an unsigned 64-bit integer value.
		 *
		 * \param _ui64Value The value to convert to the string.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the string holding the converted representation of the number or empty if there was not
		 *	enough memory to perform the operation.
		 */
		static CString LSE_CALL						CreateFromUI64( uint64_t _ui64Value, uint32_t _ui32Radix = 10 );

		/**
		 * Creates a new string from a signed 64-bit integer value.
		 *
		 * \param _ui64Value The value to convert to the string.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the string holding the converted representation of the number or empty if there was not
		 *	enough memory to perform the operation.
		 */
		static CString LSE_CALL						CreateFromI64( int64_t _i64Value, uint32_t _ui32Radix = 10 );

		/**
		 * Creates a new string from a double value.
		 *
		 * \param _dValue The value to convert to the string.
		 * \param _bScientific If true, scientific notation is used to express the number.  A lower-case e is used
		 *	for the exponent.
		 * \return Returns the string holding the converted representation of the double number or empty if there was not
		 *	enough memory to perform the operation.
		 */
		static CString LSE_CALL						CreateFromDouble( double _dValue, LSBOOL _bScientific = false );

	protected :
		// == Functions.
		/**
		 * Gets the length of a string the fast way.  Override the default single-character
		 *	counter.
		 *
		 * \param _pcValue The string whose length is to be obtained.
		 * \return Returns the length of the string.
		 */
		uint32_t LSE_CALL							StrLen( const char * _pcValue ) const;

		/**
		 * The base class needs to access our StrLen() function.
		 */
		friend class								CStringBase<CString, char>;

	private :
		typedef CStringBase<CString, char>		Parent;
	};

}	// namespace lstl

#endif	// __LSTL_STRING_H__
