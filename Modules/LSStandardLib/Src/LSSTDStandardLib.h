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


#ifndef __LSSTD_STANDARDLIB_H__
#define __LSSTD_STANDARDLIB_H__

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// OPERATING SYSTEM
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )
#define LSE_WINDOWS
#if defined( WIN32 ) || defined( _WIN32 )
#define LSE_WIN32
#endif	// #ifdef defined( WIN32 ) || defined( _WIN32 )
#ifdef _WIN64
#define LSE_WIN64
#endif	// #ifdef _WIN64
#elif defined( MACOS_X ) || defined( __APPLE__ )
#define LSE_MAC				// Any Macintosh product (specifically iOS and Macintosh OS X).
#define LSE_POSIX			// Macintosh products use POSIX.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#define LSE_IPHONE
#else
#define LSE_OSX
#endif	// #if TARGET_OS_IPHONE
#elif defined( __linux__ )
#define LSE_LINUX
#endif	// defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// COMPILER
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifdef _MSC_VER
// Microsoft Visual Studio
#define LSE_INLINE						__forceinline
#define LSE_CALL						__stdcall
#define LSE_CALLCTOR					__stdcall
#define LSE_CCALL						__cdecl
#define LSE_FCALL						__fastcall
#define LSE_RESTRICT					__restrict
#define LSE_ALIGN( ALIGN )				__declspec( align( ALIGN ) )
#define LSE_POSTALIGN( ALIGN )			
#ifndef _WIN64
#define LSE_UNALIGNED			
#else
#define LSE_UNALIGNED					__unaligned
#endif	// #ifndef _WIN64

#include <cstdint>
#include <cstring>
#include <smmintrin.h>
#include <xmmintrin.h>

LSE_INLINE __m128 operator + ( const __m128 &_mL, const __m128 &_mR ) {
	return _mm_add_ps( _mL, _mR );
}
LSE_INLINE __m128 operator - ( const __m128 &_mL, const __m128 &_mR ) {
	return _mm_sub_ps( _mL, _mR );
}
LSE_INLINE __m128 operator * ( const __m128 &_mL, const __m128 &_mR ) {
	return _mm_mul_ps( _mL, _mR );
}
LSE_INLINE __m128 operator / ( const __m128 &_mL, const __m128 &_mR ) {
	return _mm_div_ps( _mL, _mR );
}

#define LSE_ASM_BEGIN					__asm {
#define LSE_ASM_END						}
#ifdef _M_IX86
#define LSE_X86							1
#elif defined( _M_X64 )
#define LSE_X64							1
#endif	// #ifdef _M_IX86
#define LSE_EXPECT( COND, VAL )			(COND)
#define LSE_PREFETCH_LINE( ADDR )		_mm_prefetch( reinterpret_cast<const char *>(ADDR), _MM_HINT_T0 )
#define LSE_PREFETCH_LINE_WRITE( ADDR )

#define LSE_VISUALSTUDIO
#ifndef _WCHAR_T_DEFINED
typedef unsigned short					wchar_t;
#define _WCHAR_T_DEFINED
#endif	// #ifndef _WCHAR_T_DEFINED
#elif defined( __GNUC__ )
// GCC.
#define LSE_INLINE						inline
#define LSE_CALL						
#define LSE_CALLCTOR					
#define LSE_CCALL						
#define LSE_FCALL						
#define LSE_RESTRICT					__restrict
#define LSE_ALIGN( ALIGN )
#define LSE_POSTALIGN( ALIGN )			__attribute__( (aligned( ALIGN )) )
#define LSE_UNALIGNED
#if __cplusplus > 199711L
#define register						// Deprecated in LLVM 5.1.
#endif	// #if __cplusplus > 199711L

#include <cstring>

#define LSE_ASM_BEGIN					__asm {
#define LSE_ASM_END						}
#ifdef _M_IX86
#define LSE_X86							1
#elif defined( _M_X64 )
#define LSE_X64							1
#endif	// #ifdef _M_IX86
#define LSE_EXPECT( COND, VAL )			__builtin_expect( COND, VAL )
#define LSE_PREFETCH_LINE( ADDR )		__builtin_prefetch( reinterpret_cast<const void *>(ADDR), 0, 1 )
#define LSE_PREFETCH_LINE_WRITE( ADDR )	__builtin_prefetch( reinterpret_cast<const void *>(ADDR), 1, 1 )

#define LSE_GCC
#else
#define LSE_ASM_BEGIN			
#define LSE_ASM_END				
#endif	// #if defined( _MSC_VER )



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * Debug.
 */
#if defined( DEBUG ) && !defined( _DEBUG )
#define _DEBUG
#endif	// #if defined( DEBUG ) && !defined( _DEBUG )

#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif	// #if defined( _DEBUG ) && !defined( DEBUG )

#if !defined( DEBUG ) && !defined( _DEBUG ) && !defined( NDEBUG )
#define NDEBUG
#endif	// #if !defined( DEBUG ) && !defined( _DEBUG ) && !defined( NDEBUG )

#if defined( NDEBUG ) && defined( __GNUC__ )
#define LSE_DEBUG_ONLY __attribute__((unused))
#else
#define LSE_DEBUG_ONLY
#endif

/**
 * Enable this macro to use CRT versions of ::strlen, ::memcmp, ::memset, ::memcpy, and ::memmove.
 * In some situations they may be faster, but they will also increase the bulk of the code by
 *	being included.  Additionally, our custom memory functions are ready for 64-bit upgrades.
 */
//#define __LSE_CRT_MEMFUNCS__

/**
 * Force-enables the use of memcmp().
 */
#define __LSE_CRT_MEMCMP__

/** Enable this macro to use the standard-library routines for CStd::FtoA() and CStd::AtoF(). */
#define __LSE_STD_FLOAT__

/** Round up to the next nearest Xth, where X is a power of 2. */
#define LSE_ROUND_UP( VALUE, X )		((VALUE) + (((X) - (VALUE) & ((X) - 1)) & ((X) - 1)))

/** Round up to the next nearest Xth, where X is any number. */
#define LSE_ROUND_UP_ANY( VALUE, X )	(((VALUE + ((X) - 1)) / (X)) * (X))

/** Number of elements in an array. */
#define LSE_ELEMENTS( ARRAY )			(sizeof( (ARRAY) ) / sizeof( (ARRAY)[0] ))

/** Debug printing.  If not defined, DebugPrintA() and DebugPrintW() do not print messages. */
#define LSE_DEBUG

/** Maximum unsigned 32-bit value. */
#define LSE_MAXU32						static_cast<uint32_t>(~0)

/** Maximum unsigned 64-bit value. */
#define LSE_MAXU64						static_cast<uint64_t>(~0ULL)

/** NULL. */
#ifndef NULL
#define NULL							0
#endif	// #ifndef NULL

/** Miniature HRESULT emulation. */
#ifndef LSE_WINDOWS
typedef long HRESULT;
#define _HRESULT_DEFINED

#define S_OK							static_cast<HRESULT>(0L)
#define S_FALSE							static_cast<HRESULT>(1)
#define FAILED( _hRes )					(static_cast<HRESULT>(_hRes) < 0)
#define SUCCEEDED( _hRes )				(static_cast<HRESULT>(_hRes) >= 0)
#define E_OUTOFMEMORY					static_cast<HRESULT>(0x80000000 | LSSTD_E_OUTOFMEMORY)
#define E_FILENOTFOUND					static_cast<HRESULT>(0x80000000 | LSSTD_E_FILENOTFOUND)
#endif	// #ifndef LSE_WINDOWS

/** Unlikely branch prediction. */
#define LSE_UNLIKELY( COND )			LSE_EXPECT( COND, false )

/** Likely branch prediction. */
#define LSE_LIKELY( COND )				LSE_EXPECT( COND, true )

/** Makes a four-character-code. */
#define LSE_MAKEFOURCC( A, B, C, D )	(static_cast<uint32_t>(A) | (static_cast<uint32_t>(B) << 8) | (static_cast<uint32_t>(C) << 16) | (static_cast<uint32_t>(D) << 24))



namespace lsstd {

#if defined( LSE_VISUALSTUDIO )
#define LSS_WINDOWS
#elif defined( LSE_GCC )

#endif	// #if defined( LSE_VISUALSTUDIO )

	/** Boolean type. */
	typedef int32_t						LSBOOL;

	/** UTF types. */
	typedef uint8_t						LSUTF8;
	typedef uint16_t					LSUTF16;
	typedef uint32_t					LSUTF32;
	
	/**
	 * wchar_t is 16 bits on Windows and 32 bits on everything else.
	 *	This means L"" string literals are of variable size.  The X here
	 *	indicates the variable number of bits, and should remind you not to
	 *	send data of this format to files or over networks.
	 */
	typedef wchar_t						LSUTFX;

	/** An invalid Unicode character used to indicate an error in UTF encoding. */
#define LSSUTF_INVALID					~static_cast<uint32_t>(0)

	/** Maximum signed 32-bit value. */
#define LSSTD_MAX_INT32					2147483647L

	/** Maximum unsigned 32-bit value. */
#define LSSTD_MAX_UINT32				LSE_MAXU32

	/** Maximum signed 64-bit value. */
#define LSSTD_MAX_INT64					9223372036854775807LL

	/** Maximum unsigned 64-bit value. */
#define LSSTD_MAX_UINT64				LSE_MAXU64

	/**
	 * Error codes.
	 */
	enum LSSTD_ERRORS {
		LSSTD_E_SUCCESS,				/**< No error. */
		LSSTD_E_OUTOFMEMORY,			/**< Out of memory. */
		LSSTD_E_FILENOTFOUND,			/**< File was not found at the given path. */
		LSSTD_E_INVALIDWRITEPERMISSIONS,/**< Unable to write to a file. */
		LSSTD_E_NODISKSPACE,			/**< Unable to write to a file (disk space). */
		LSSTD_E_INVALIDFILETYPE,		/**< File exists but is not in the expected format. */
		LSSTD_E_INVALIDCALL,			/**< Invalid call. */
		LSSTD_E_INVALIDDATA,			/**< Invalid input data. */
		LSSTD_E_INTERNALERROR,			/**< Internal error. */
		LSSTD_E_FEATURENOTSUPPORTED,	/**< Feature not yet supported. */
		LSSTD_E_PARTIALFAILURE,			/**< Within multiple tasks, one or more failed. */
		LSSTD_E_BADVERSION,				/**< Unsupported version (of a file etc.) */
		LSSTD_E_FILEOVERFLOW,			/**< The file exceeded the maximum size supported by the system. */
		LSSTD_E_FILEWRITEERROR,			/**< An error occurred while writing the file. */
	};


	/**
	 * Class CStd
	 * \brief Standard CRT-type functions.
	 *
	 * Description: Standard routines and types.  This library is just a bunch of common helpful routines normally
	 *	found in the C-runtime (etc.)  These functions should be used instead of their CRT counterparts.  Reasons
	 *	include speed, consistency and portability.
	 */
	class CStd {
	public :
		// == Functions.
		/**
		 * Compare a wide-character string against another lexicographically.
		 *
		 * \param _pwcLeft NULL-terminated string to compare.
		 * \param _pwcRight NULL-terminated string to compare.
		 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
		 *	_pwcLeft is greater than _pwcRight, lexicographically.
		 */
		static int32_t LSE_CALL			WStrCmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight );
		
		/**
		 * Compare a wide-character string case-insensitively against another lexicographically.
		 *
		 * \param _pwcLeft NULL-terminated string to compare.
		 * \param _pwcRight NULL-terminated string to compare.
		 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
		 *	_pwcLeft is greater than _pwcRight, lexicographically.
		 */
		static int32_t LSE_CALL			WStrICmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight );

		/**
		 * Compare a wide-character string against another lexicographically.  Compares at most _ui32Max characters.
		 *
		 * \param _pwcLeft String to compare.
		 * \param _pwcRight String to compare.
		 * \param _ui32Max Maximum number of characters to compare.
		 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
		 *	_pwcLeft is greater than _pwcRight, lexicographically.
		 */
		static int32_t LSE_CALL			WStrNCmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max );
		
		/**
		 * Compare a wide-character string case-insensitively against another lexicographically.  Compares at most _ui32Max characters.
		 *
		 * \param _pwcLeft String to compare.
		 * \param _pwcRight String to compare.
		 * \param _ui32Max Maximum number of characters to compare.
		 * \return Returns < 0 if _pwcLeft is less than _pwcRight, 0 if they are equal, and > 0 if
		 *	_pwcLeft is greater than _pwcRight, lexicographically.
		 */
		static int32_t LSE_CALL			WStrNICmp( const LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max );

		/**
		 * Copy one wide-character string to another.
		 *
		 * \param _pwcDst Destination string.
		 * \param _pwcSrc Source string.
		 * \return Returns the destination string, which must be long enough to contain the copy,
		 *	including the terminating NULL.
		 */
		static LSUTFX * LSE_CALL		WStrCpy( LSUTFX * _pwcDst, const LSUTFX * _pwcSrc );
		
		/**
		 * Copy one wide-character string to another.  Copies at most _ui32Max characters.
		 *
		 * \param _pwcDst Destination string.
		 * \param _pwcSrc Source string.
		 * \param _ui32Max Maximum number of characters to copy.
		 * \return Returns the destination string, which must be long enough to contain the copy,
		 *	including the terminating NULL.
		 */
		static LSUTFX * LSE_CALL		WStrNCpy( LSUTFX * _pwcDst, const LSUTFX * _pwcSrc, uint32_t _ui32Max );

		/**
		 * Gets the length of a wide-character string.
		 *
		 * \param _pwcStr NULL-terminated string whose length is to be obtained.
		 * \return Returns the length of the string.
		 */
		static uint32_t LSE_CALL		WStrLen( const LSUTFX * _pwcStr );

		/**
		 * Combine 2 wide-character strings.
		 *
		 * \param _pwcLeft NULL-terminated destination string.
		 * \param _pwcRight NULL-terminated source string.
		 * \return Returns the pointer to the first parameter.
		 */
		static LSUTFX * LSE_CALL		WStrCat( LSUTFX * _pwcLeft, const LSUTFX * _pwcRight );
		
		/**
		 * Combine 2 wide-character strings.
		 *
		 * \param _pwcLeft NULL-terminated destination string.
		 * \param _pwcRight NULL-terminated source string.
		 * \param _ui32Max Maximum number of characters to append.
		 * \return Returns the pointer to the first parameter.
		 */
		static LSUTFX * LSE_CALL		WStrNCat( LSUTFX * _pwcLeft, const LSUTFX * _pwcRight, uint32_t _ui32Max );

		/**
		 * Returns the index of the first occurrence of a character in a string that belongs to a set of characters.
		 *
		 * \param _pwcString NULL-terminated searched string.
		 * \param _pwcCharSet NULL-terminated character set.
		 * \return Returns an integer value specifying the length of the initial segment of _pwcString that consists entirely
		 *	of characters not in _pwcCharSet.  If _pwcString begins with a character that is in _pwcCharSet, the function returns
		 *	0.  No return value is reserved to indicate an error.
		 */
		static uint32_t LSE_CALL		WStrCSpn( const LSUTFX * _pwcString, const LSUTFX * _pwcCharSet );
		
		/**
		 * Compare a string against another lexicographically.
		 *
		 * \param _pcLeft NULL-terminated string to compare.
		 * \param _pcRight NULL-terminated string to compare.
		 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
		 *	_pcLeft is greater than _pcRight, lexicographically.
		 */
		static int32_t LSE_CALL			StrCmp( const char * _pcLeft, const char * _pcRight );
		
		/**
		 * Compare a string case-insensitively against another lexicographically.
		 *
		 * \param _pcLeft NULL-terminated string to compare.
		 * \param _pcRight NULL-terminated string to compare.
		 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
		 *	_pcLeft is greater than _pcRight, lexicographically.
		 */
		static int32_t LSE_CALL			StrICmp( const char * _pcLeft, const char * _pcRight );

		/**
		 * Compare a string against another lexicographically.  Compares at most _ui32Max characters.
		 *
		 * \param _pcLeft NULL-terminated string to compare.
		 * \param _pcRight NULL-terminated string to compare.
		 * \param _ui32Max Maximum number of characters to compare.
		 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
		 *	_pcLeft is greater than _pcRight, lexicographically.
		 */
		static int32_t LSE_CALL			StrNCmp( const char * _pcLeft, const char * _pcRight, uint32_t _ui32Max );
		
		/**
		 * Compare a string case-insensitively against another lexicographically.  Compares at most _ui32Max characters.
		 *
		 * \param _pcLeft NULL-terminated string to compare.
		 * \param _pcRight NULL-terminated string to compare.
		 * \param _ui32Max Maximum number of characters to compare.
		 * \return Returns < 0 if _pcLeft is less than _pcRight, 0 if they are equal, and > 0 if
		 *	_pcLeft is greater than _pcRight, lexicographically.
		 */
		static int32_t LSE_CALL			StrNICmp( const char * _pcLeft, const char * _pcRight, uint32_t _ui32Max );

		/**
		 * Copy one string to another.
		 *
		 * \param _pcDst Destination string.
		 * \param _pcSrc Source string.
		 * \return Returns the destination string, which must be long enough to contain the copy,
		 *	including the terminating NULL.
		 */
		static char * LSE_CALL			StrCpy( char * _pcDst, const char * _pcSrc );
		
		/**
		 * Copy one string to another.  Copies at most _ui32Max characters.
		 *
		 * \param _pcDst Destination string.
		 * \param _pcSrc Source string.
		 * \param _ui32Max Maximum number of characters to copy.
		 * \return Returns the destination string, which must be long enough to contain the copy,
		 *	including the terminating NULL.
		 */
		static char * LSE_CALL			StrNCpy( char * _pcDst, const char * _pcSrc, uint32_t _ui32Max );

		/**
		 * Gets the length of a string.
		 *
		 * \param _pcStr NULL-terminated string whose length is to be obtained.
		 * \return Returns the length of the string.
		 */
		static uint32_t LSE_CALL		StrLen( const char * _pcStr );

		/**
		 * Combine 2 strings.
		 *
		 * \param _pcLeft NULL-terminated destination string.
		 * \param _pcRight NULL-terminated source string.
		 * \return Returns the pointer to the first parameter.
		 */
		static char * LSE_CALL			StrCat( char * _pcLeft, const char * _pcRight );
		
		/**
		 * Combine 2 strings.
		 *
		 * \param _pcLeft NULL-terminated destination string.
		 * \param _pcRight NULL-terminated source string.
		 * \param _ui32Max Maximum number of characters to append.
		 * \return Returns the pointer to the first parameter.
		 */
		static char * LSE_CALL			StrNCat( char * _pcLeft, const char * _pcRight, uint32_t _ui32Max );

		/**
		 * Returns the index of the first occurrence of a character in a string that belongs to a set of characters.
		 *
		 * \param _pcString NULL-terminated searched string.
		 * \param _pcCharSet NULL-terminated character set.
		 * \return Returns an integer value specifying the length of the initial segment of _pcString that consists entirely
		 *	of characters not in _pcCharSet.  If _pcString begins with a character that is in _pcCharSet, the function returns
		 *	0.  No return value is reserved to indicate an error.
		 */
		static uint32_t LSE_CALL		StrCSpn( const char * _pcString, const char * _pcCharSet );

		/**
		 * Duplicate a wide-character string.
		 *
		 * \param _pwcSrc The string to duplicate.
		 * \return Returns the duplicated string, which must be freed with WStrDel() when no longer needed.
		 */
		static LSUTFX * LSE_CALL		WStrDup( const LSUTFX * _pwcSrc );

		/**
		 * Delete a string created by a previous call to WStrDup().
		 *
		 * \param _pwcStr The string to delete.
		 */
		static void LSE_CALL			WStrDel( LSUTFX * _pwcStr );

		/**
		 * Duplicate a character string.
		 *
		 * \param _pcSrc The string to duplicate.
		 * \return Returns the duplicated string, which must be freed with StrDel() when no longer needed.
		 */
		static char * LSE_CALL			StrDup( const char * _pcSrc );

		/**
		 * Delete a string created by a previous call to StrDup().
		 *
		 * \param _pcStr The string to delete.
		 */
		static void LSE_CALL			StrDel( char * _pcStr );
		
		/**
		 * Gets the lower-case value of an alpha character (wide).
		 *
		 * \param _wcChar The character to be converted to lower case.
		 * \return The lower-case value of the input character.
		 */
		static LSUTFX LSE_CALL			ToLower( LSUTFX _wcChar );
		
		/**
		 * Gets the upper-case value of an alpha character (wide).
		 *
		 * \param _wcChar The character to be converted to upper case.
		 * \return The upper-case value of the input character.
		 */
		static LSUTFX LSE_CALL			ToUpper( LSUTFX _wcChar );
		
		/**
		 * Gets the lower-case value of an alpha character.
		 *
		 * \param _cChar The character to be converted to lower case.
		 * \return The lower-case value of the input character.
		 */
		static char LSE_CALL			ToLower( char _cChar );
		
		/**
		 * Gets the upper-case value of an alpha character.
		 *
		 * \param _cChar The character to be converted to upper case.
		 * \return The upper-case value of the input character.
		 */
		static char LSE_CALL			ToUpper( char _cChar );

		/**
		 * Converts a wide-character string to a signed 32-bit integer.
		 *
		 * \param _pwcString String to be converted.
		 * \return Returns the int32_t value produced by interpreting the input characters as a number.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int32_t LSE_CALL			WtoI32( const LSUTFX * _pwcString );

		/**
		 * Converts a string to a signed 32-bit integer.
		 *
		 * \param _pcString String to be converted.
		 * \return Returns the int32_t value produced by interpreting the input characters as a number.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int32_t LSE_CALL			AtoI32( const char * _pcString );

		/**
		 * Converts a wide-character string to a signed 64-bit integer.
		 *
		 * \param _pwcString String to be converted.
		 * \return Returns the int64_t value produced by interpreting the input characters as a number.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int64_t LSE_CALL			WtoI64( const LSUTFX * _pwcString );

		/**
		 * Converts a string to a signed 64-bit integer.
		 *
		 * \param _pcString String to be converted.
		 * \return Returns the int64_t value produced by interpreting the input characters as a number.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int64_t LSE_CALL			AtoI64( const char * _pcString );

		/**
		 * Converts a hexadecimal string to a signed 64-bit integer.
		 *
		 * \param _pwcString String to be converted.
		 * \return Returns the int64_t value produced by interpreting the input characters as a number in hexadecimal format.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int64_t LSE_CALL			WHtoI64( const LSUTFX * _pwcString );

		/**
		 * Converts a hexadecimal string to a signed 64-bit integer.
		 *
		 * \param _pcString String to be converted.
		 * \return Returns the int64_t value produced by interpreting the input characters as a number in hexadecimal format.
		 *	If the input cannot be converted to a value of that type, the return value is 0.
		 */
		static int64_t LSE_CALL			AHtoI64( const char * _pcString );

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
		static LSUTFX * LSE_CALL		I32toW( int32_t _i32Value, LSUTFX * _pwcRet, uint32_t _ui32Radix = 10 );

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
		static char * LSE_CALL			I32toA( int32_t _i32Value, char * _pcRet, uint32_t _ui32Radix = 10 );

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
		static LSUTFX * LSE_CALL		I64toW( int64_t _i64Value, LSUTFX * _pwcRet, uint32_t _ui32Radix = 10 );

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
		static char * LSE_CALL			I64toA( int64_t _i64Value, char * _pcRet, uint32_t _ui32Radix = 10 );

		/**
		 * Converts an unsigned integer to a wide-character string using the given radix.
		 *
		 * \param _i32Value Number to be converted.
		 * \param _pwcRet String result.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the given string.
		 */
		static LSUTFX * LSE_CALL		UI32toW( uint32_t _ui32Value, LSUTFX * _pwcRet, uint32_t _ui32Radix = 10 );

		/**
		 * Converts an unsigned integer to a string using the given radix.
		 *
		 * \param _i32Value Number to be converted.
		 * \param _pcRet String result.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the given string.
		 */
		static char * LSE_CALL			UI32toA( uint32_t _ui32Value, char * _pcRet, uint32_t _ui32Radix = 10 );

		/**
		 * Converts an unsigned integer to a wide-character string using the given radix.
		 *
		 * \param _ui64Value Number to be converted.
		 * \param _pwcRet String result.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the given string.
		 */
		static LSUTFX * LSE_CALL		UI64toW( uint64_t _ui64Value, LSUTFX * _pwcRet, uint32_t _ui32Radix = 10 );

		/**
		 * Converts an unsigned integer to a string using the given radix.
		 *
		 * \param _ui64Value Number to be converted.
		 * \param _pcRet String result.
		 * \param _ui32Radix Base of value, which must be in the range from 2 to 36.
		 * \return Returns the given string.
		 */
		static char * LSE_CALL			UI64toA( uint64_t _ui64Value, char * _pcRet, uint32_t _ui32Radix = 10 );

		/**
		 * Converts a string to double.
		 *
		 * \param _pwcString String to be converted.
		 * \return Returns the double value produced by interpreting the input characters as a number.
		 *	The return value is 0.0 if the input cannot be converted to a value of that type.
		 */
		static double LSE_CALL			WtoF( const LSUTFX * _pwcString );

		/**
		 * Converts a string to double.
		 *
		 * \param _pcString String to be converted.
		 * \return Returns the double value produced by interpreting the input characters as a number.
		 *	The return value is 0.0 if the input cannot be converted to a value of that type.
		 */
		static double LSE_CALL			AtoF( const char * _pcString );

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
		static LSUTFX * LSE_CALL		FtoW( LSUTFX * _pwcBuffer, uint32_t _ui32MaxLen, double _dValue,
			uint32_t _ui32Precision = 6, int8_t _i8Format = 'f' );

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
		static char * LSE_CALL			FtoA( char * _pcBuffer, uint32_t _ui32MaxLen, double _dValue,
			uint32_t _ui32Precision = 6, int8_t _i8Format = 'f' );
		
		/**
		 * Converts a const char string to a LSUTFX string.  _pwcRet must be long enough to hold the converted string.
		 *
		 * \param _pcSrc The source string.
		 * \param _pwcRet The destination string.
		 * \return Returns the converted string.
		 */
		static LSUTFX * LSE_CALL		CharToWChar( const char * _pcSrc, LSUTFX * _pwcRet );
		
		/**
		 * Converts a const LSUTFX string to a char string.  _pcRet must be long enough to hold the converted string.
		 *
		 * \param _pwcSrc The source string.
		 * \param _pcRet The destination string.
		 * \return Returns the converted string.
		 */
		static char * LSE_CALL			WCharToChar( const LSUTFX * _pwcSrc, char * _pcRet );

		/**
		 * Converts a UTF-X LSUTFX string to a UTF-8 char string.  X is 16 on Windows and 32 on Macintosh.  These
		 *	sizes match the bits in the LSUTFX type on each platform.
		 *
		 * \param _pwcSrc The source string.
		 * \param _pcRet The destination string.
		 * \param _ui32RetLen The length, in bytes, of the buffer to which _pcRet points.
		 * \return Returns the converted string.
		 */
		static char * LSE_CALL			UtfXToUtf8( const LSUTFX * _pwcSrc, char * _pcRet, uint32_t _ui32RetLen );

		/**
		 * Gets the number of bytes needed to contain a string converted from UTF-X to UTF-8.  X is 16 on Windows and 32 on Macintosh.  These
		 *	sizes match the bits in the LSUTFX type on each platform.
		 *
		 * \param _pwcSrc The source string.
		 * \return Returns the number of bytes needed to store the converted string, including the terminating NULL character.
		 */
		static uint32_t LSE_CALL		UtfXToUtf8Len( const LSUTFX * _pwcSrc );

		/**
		 * Gets the next UTF-32 character from a UTF-32 string.
		 *
		 * \param _putf32Char Pointer to the next character to decode.  String must be in UTF-32 format.
		 * \param _ui32Len Length, in LSUTF32 units, of the string.  This is not the number of Unicode
		 *	characters, but actual the number of LSUTF32 characters in the buffer.
		 * \param _pui32Size If not NULL, this holds the returned size of the character in LSUTF32 units.
		 *	However, the UTF-32 coding scheme always uses 1 LSUTF32 character per Unicode character;
		 *	therefore, this value, if not NULL, will always be set to 1.  It is here only for compatibility
		 *	with the other UTF functions.
		 * \return Returns an uint32_t value representing the decoded Unicode character.  Also returns the
		 *	size of the character in LSUTF32 units.
		 */
		static uint32_t LSE_CALL		NextUtf32Char( const LSUTF32 * _putf32Char, uint32_t _ui32Len, uint32_t * _pui32Size = NULL );

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
		static uint32_t LSE_CALL		NextUtf16Char( const LSUTF16 * _putf16Char, uint32_t _ui32Len, uint32_t * _pui32Size = NULL );

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
		static uint32_t LSE_CALL		NextUtf8Char( const LSUTF8 * _putf8Char, uint32_t _ui32Len, uint32_t * _pui32Size = NULL );

		/**
		 * Converts a raw 32-bit Unicode character to a UTF-32 character.  Returns the UTF-32 character as
		 *	an uint32_t value.  The returned length is the number LSUTF32 characters returned, which is
		 *	always 1.
		 *
		 * \param _ui32Raw The raw Unicode value to convert.
		 * \param _ui32Len The lengh, in LSUTF32 characters, of the converted value.  Always 1.
		 * \return Returns the converted character in uint32_t form along with the length, in units of
		 *	LSUTF32, of the returned value.  Because the mapping between UTF-32 and raw 32-bit Unicode values
		 *	is one-to-one, this value is always 1.
		 */
		static uint32_t LSE_CALL		RawUnicodeToUtf32Char( uint32_t _ui32Raw, uint32_t &_ui32Len );

		/**
		 * Converts a raw 32-bit Unicode character to a UTF-16 character.  Returns the UTF-16 character as
		 *	an uint32_t value.  The returned length is the number LSUTF16 characters returned.
		 *
		 * \param _ui32Raw The raw Unicode value to convert.
		 * \param _ui32Len The lengh, in LSUTF16 characters, of the converted value.
		 * \return Returns the converted character in uint32_t form along with the length, in units of
		 *	LSUTF16, of the returned value.
		 */
		static uint32_t LSE_CALL		RawUnicodeToUtf16Char( uint32_t _ui32Raw, uint32_t &_ui32Len );

		/**
		 * Converts a raw 32-bit Unicode character to a UTF-8 character.  Returns the UTF-8 character as
		 *	an uint32_t value.  The returned length is the number LSUTF8 characters returned.
		 *
		 * \param _ui32Raw The raw Unicode value to convert.
		 * \param _ui32Len The lengh, in LSUTF8 characters, of the converted value.
		 * \return Returns the converted character in uint32_t form along with the length, in units of
		 *	LSUTF8, of the returned value.
		 */
		static uint32_t LSE_CALL		RawUnicodeToUtf8Char( uint32_t _ui32Raw, uint32_t &_ui32Len );

		/**
		 * Converts a UTF-32 string to a 32-bit string where every character is stored as a 32-bit value.
		 *	The converted string is always appended with a NULL unless it is of 0 length.
		 *
		 * \param _pui32Dst The destination where to store the converted string.
		 * \param _putf32Src The NULL-terminated UTF-32 string to be converted.
		 * \param _ui32MaxLen The maximum number of characters to store.
		 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
		 */
		static uint32_t LSE_CALL		Utf32ToUnicodeRaw( uint32_t * _pui32Dst,
			const LSUTF32 * _putf32Src, uint32_t _ui32MaxLen );

		/**
		 * Converts a UTF-16 string to a 32-bit string where every character is stored as a 32-bit value.
		 *	The converted string is always appended with a NULL unless it is of 0 length.
		 *
		 * \param _pui32Dst The destination where to store the converted string.
		 * \param _putf16Src The NULL-terminated UTF-16 string to be converted.
		 * \param _ui32MaxLen The maximum number of characters to store.
		 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
		 */
		static uint32_t LSE_CALL		Utf16ToUnicodeRaw( uint32_t * _pui32Dst,
			const LSUTF16 * _putf16Src, uint32_t _ui32MaxLen );

		/**
		 * Converts a UTF-8 string to a 32-bit string where every character is stored as a 32-bit value.
		 *	The converted string is always appended with a NULL unless it is of 0 length.
		 *
		 * \param _pui32Dst The destination where to store the converted string.
		 * \param _putf8Src The NULL-terminated UTF-8 string to be converted.
		 * \param _ui32MaxLen The maximum number of characters to store.
		 * \return Returns the number of characters stored in the buffer, not including the terminating NULL.
		 */
		static uint32_t LSE_CALL		Utf8ToUnicodeRaw( uint32_t * _pui32Dst,
			const LSUTF8 * _putf8Src, uint32_t _ui32MaxLen );

		/**
		 * Converts a raw 32-bit Unicode string to a UTF-32 string.  The input string must be NULL-terminated,
		 *	and the output string will always be appended with a NULL.
		 *
		 * \param _putf32Dst The destination buffer.
		 * \param _pui32Src The NULL-terminated source string.
		 * \param _ui32MaxLen The maximum number of characters _putf32Dst can store.  This is the total number
		 *	of LSUTF32 characters allocated in the buffer.
		 * \return Returns the number of LSUTF32 characters output to the buffer, such that the return value,
		 *	treated as an index into an array of LSUTF32 characters, will index the NULL character in the
		 *	returned string.
		 */
		static uint32_t LSE_CALL		RawUnicodeToUtf32( LSUTF32 * _putf32Dst,
			const uint32_t * _pui32Src, uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		RawUnicodeToUtf16( LSUTF16 * _putf16Dst,
			const uint32_t * _pui32Src, uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		RawUnicodeToUtf8( LSUTF8 * _putf8Dst,
			const uint32_t * _pui32Src, uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf32ToUtf8( LSUTF8 * _putf8Dst, const LSUTF32 * _putf32Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf32ToUtf16( LSUTF16 * _putf16Dst, const LSUTF32 * _putf32Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf16ToUtf8( LSUTF8 * _putf8Dst, const LSUTF16 * _putf16Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf16ToUtf32( LSUTF32 * _putf32Dst, const LSUTF16 * _putf16Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf8ToUtf16( LSUTF16 * _putf16Dst, const LSUTF8 * _putf8Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf8ToUtf32( LSUTF32 * _putf32Dst, const LSUTF8 * _putf8Src,
			uint32_t _ui32MaxLen );

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
		static uint32_t LSE_CALL		Utf8ToUtfX( LSUTFX * _putfxDst, const LSUTF8 * _putf8Src,
			uint32_t _ui32MaxLen );

		/**
		 * Gets the length of a UTF-32 string in Unicode characters.
		 *
		 * \param _putf32String The string whose length is to be determined.
		 * \return Returns the number of Unicode characters in the given UTF-32 string.
		 */
		static uint32_t LSE_CALL		Utf32StrLen( const LSUTF32 * _putf32String );

		/**
		 * Gets the length of a UTF-16 string in Unicode characters.
		 *
		 * \param _putf16String The string whose length is to be determined.
		 * \return Returns the number of Unicode characters in the given UTF-16 string.
		 */
		static uint32_t LSE_CALL		Utf16StrLen( const LSUTF16 * _putf16String );

		/**
		 * Gets the length of a UTF-8 string in Unicode characters.
		 *
		 * \param _putf8String The string whose length is to be determined.
		 * \return Returns the number of Unicode characters in the given UTF-8 string.
		 */
		static uint32_t LSE_CALL		Utf8StrLen( const LSUTF8 * _putf8String );

		/**
		 * Gets the number of bytes needed to hold a UTF-32 string in UTF-8 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf32String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-32 string converted to UTF-8,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf32StrLenAsUtf8( const LSUTF32 * _putf32String );

		/**
		 * Gets the number of bytes needed to hold a UTF-32 string in UTF-16 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf32String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-32 string converted to UTF-16,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf32StrLenAsUtf16( const LSUTF32 * _putf32String );

		/**
		 * Gets the number of bytes needed to hold a UTF-16 string in UTF-8 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf16String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-16 string converted to UTF-8,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf16StrLenAsUtf8( const LSUTF16 * _putf16String );

		/**
		 * Gets the number of bytes needed to hold a UTF-16 string in UTF-32 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf16String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-16 string converted to UTF-32,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf16StrLenAsUtf32( const LSUTF16 * _putf16String );

		/**
		 * Gets the number of bytes needed to hold a UTF-8 string in UTF-16 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf8String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-16,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf8StrLenAsUtf16( const LSUTF8 * _putf8String );

		/**
		 * Gets the number of bytes needed to hold a UTF-8 string in UTF-32 format, including the terminating
		 *	NULL.
		 *
		 * \param _putf8String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-32,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf8StrLenAsUtf32( const LSUTF8 * _putf8String );

		/**
		 * Gets the number of bytes needed to hold a UTF-8 string in UTF-X format, including the terminating
		 *	NULL.
		 *
		 * \param _putf8String The string whose buffer size after conversion is to be determined.
		 * \return Returns the total number of bytes needed to hold the UTF-8 string converted to UTF-X,
		 *	including the terminating NULL.  This value should be used for allocation.
		 */
		static uint32_t LSE_CALL		Utf8StrLenAsUtfX( const LSUTF8 * _putf8String );

		/**
		 * Determines if the given wide character is a whitespace character.
		 *
		 * \param _wcChar Character to test.
		 * \return Returns a non-zero value if _wcChar is a white-space character (0x09-0x0D or 0x20). 
		 */
		static LSBOOL LSE_CALL			IsWhitespace( LSUTFX _wcChar );

		/**
		 * Determines if the given character is a whitespace character.
		 *
		 * \param _cChar Character to test.
		 * \return Returns a non-zero value if _cChar is a white-space character (0x09-0x0D or 0x20). 
		 */
		static LSBOOL LSE_CALL			IsWhitespace( char _cChar );

		/**
		 * Determines if the given wide character is an alpha character.
		 *
		 * \param _wcChar Character to test.
		 * \return Returns a non-zero value if _wcChar is within the ranges A-Z or a-z.
		 */
		static LSBOOL LSE_CALL			IsAlpha( LSUTFX _wcChar );

		/**
		 * Determines if the given character is an alpha character.
		 *
		 * \param _cChar Character to test.
		 * \return Returns a non-zero value if _cChar is within the ranges A-Z or a-z.
		 */
		static LSBOOL LSE_CALL			IsAlpha( char _cChar );

		/**
		 * Determines if the given wide character is a digit.
		 *
		 * \param _wcChar Character to test.
		 * \return Returns a non-zero value if _wcChar is a decimal digit (0-9).
		 */
		static LSBOOL LSE_CALL			IsDigit( LSUTFX _wcChar );

		/**
		 * Determines if the given character is a digit.
		 *
		 * \param _cChar Character to test.
		 * \return Returns a non-zero value if _cChar is a decimal digit (0-9).
		 */
		static LSBOOL LSE_CALL			IsDigit( char _cChar );

		/**
		 * Determines if the given wide character is alpha-numeric.
		 *
		 * \param _wcChar Character to test.
		 * \return Returns a non-zero value if either IsAlpha() or IsDigit() is true for _wcChar, that is, if _wcChar is within the ranges A-Z, a-z, or 0-9.
		 */
		static LSBOOL LSE_CALL			IsAlNum( LSUTFX _wcChar );

		/**
		 * Determines if the given character is alpha-numeric.
		 *
		 * \param _cChar Character to test.
		 * \return Returns a non-zero value if either IsAlpha() or IsDigit() is true for _cChar, that is, if _cChar is within the ranges A-Z, a-z, or 0-9.
		 */
		static LSBOOL LSE_CALL			IsAlNum( char _cChar );

		/**
		 * Determines if the given NULL-terminated string is a valid identifier.  Identifiers can consist only of characters _, A-Z, a-z, and 0-9,
		 *	and the first character may not be a digit.
		 *
		 * \param _pwcStr The string to check for being an identifier.
		 * \return Returns true if the given string is a valid identifier.
		 */
		static LSBOOL LSE_CALL			IsIdentifier( const LSUTFX * _pwcStr );

		/**
		 * Determines if the given NULL-terminated string is a valid identifier.  Identifiers can consist only of characters _, A-Z, a-z, and 0-9,
		 *	and the first character may not be a digit.
		 *
		 * \param _pcStr The string to check for being an identifier.
		 * \return Returns true if the given string is a valid identifier.
		 */
		static LSBOOL LSE_CALL			IsIdentifier( const char * _pcStr );

		/**
		 * Copy memory.  Does not check for overlap.  If memory can overlap, use MemMove() instead.
		 * The destination buffer must be large enough to hold its new contents.
		 *
		 * \param _pvDst Destination buffer.
		 * \param _pvSrc Buffer from which to copy.
		 * \param _uiptrSize Number of bytes to copy.
		 * \return Returns the destination buffer.
		 */
		static void * LSE_CALL			MemCpy( void * _pvDst, const void * _pvSrc, uintptr_t _uiptrSize );

		/**
		 * Copy overlapping memory.  The destination buffer must be large enough to hold its new contents.
		 *
		 * \param _pvDst Destination buffer.
		 * \param _pvSrc Buffer from which to copy.
		 * \param _uiptrSize Number of bytes to copy.
		 * \return Returns the destination buffer.
		 */
		static void * LSE_CALL			MemMove( void * _pvDst, const void * _pvSrc, uintptr_t _uiptrSize );

		/**
		 * Compare the memory at one location with the memory at another location.  Returns only true or
		 *	false.  Does not perform a lexicographic comparison.
		 *
		 * \param _pvBuf0 Buffer to compare.
		 * \param _pvBuf1 Buffer to compare.
		 * \param _uiptrSize Number of bytes to compare.
		 * \return Returns true if the buffers' contents are an exact match, false otherwise.
		 */
#if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )
		static LSE_INLINE LSBOOL LSE_CALL
										MemCmpF( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );
#else
		static LSBOOL LSE_CALL			MemCmpF( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );
#endif	// #if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )

		/**
		 * Compares the memory at one location with the memory at another location.  Returns only true or
		 *	false.  Does not perform a lexicographic comparison.  The addresses are assumed to be aligned to 4-bytes
		 *	and the size to be a multiple of 4.  Internally, with these assumptions, this always performs a 32-bit
		 *	compare across the provided data.
		 *
		 * \param _pvBuf0 Buffer to compare.
		 * \param _pvBuf1 Buffer to compare.
		 * \param _uiptrSize Number of bytes to compare.
		 * \return Returns true if the buffers' contents are an exact match, false otherwise.
		 */
		static LSE_INLINE LSBOOL LSE_CALL
										MemCmpF32( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );

		/**
		 * Compares the memory at one location with the memory at another location.  Returns only true or
		 *	false.  Does not perform a lexicographic comparison.  The addresses are assumed to be aligned to the native
		 *	pointer size and the size a multiple of the native machine pointer size.  Internally, with these assumptions,
		 *	it performs a 32-bit-per-comparison check on 32-bit machines and 64-bits-per-check on 64-bit machines.
		 *
		 * \param _pvBuf0 Buffer to compare.
		 * \param _pvBuf1 Buffer to compare.
		 * \param _uiptrSize Number of bytes to compare.
		 * \return Returns true if the buffers' contents are an exact match, false otherwise.
		 */
		static LSE_INLINE LSBOOL LSE_CALL
										MemCmpFUPtr( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );

		/**
		 * Compare the memory at one location with the memory at another location.  Performs a lexicographic comparison.
		 *
		 * \param _pvBuf0 Buffer to compare.
		 * \param _pvBuf1 Buffer to compare.
		 * \param _uiptrSize Number of bytes to compare.
		 * \return Returns the lexicographic relationship between the given buffers.
		 */
#if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )
		static LSE_INLINE int32_t LSE_CALL
										MemCmp( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );
#else
		static int32_t LSE_CALL			MemCmp( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize );
#endif	// #if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )

		/**
		 * Fill memory with a given byte value.
		 *
		 * \param _pvDst Buffer to fill.
		 * \param _ui8Value Value with which to fill the buffer.
		 * \param _uiptrSize Number of bytes to fill.
		 */
		static void LSE_CALL			MemSet( void * _pvDst, uint8_t _ui8Value, uintptr_t _uiptrSize );

		/**
		 * Searches within the first _uiptrSize bytes of the block of memory to which _pvBuffer points for the first occurrence of _ui8Value, and returns a pointer to it.
		 *
		 * \param _pvBuffer The buffer to search.
		 * \param _ui8Value The value to find.
		 * \param _uiptrSize Maximum number of bytes to search.
		 * \return Returns a pointer to the found byte or NULL if it was not found.
		 */
		static const void * LSE_CALL	MemChr( const void * _pvBuffer, uint8_t _ui8Value, uintptr_t _uiptrSize );

		/**
		 * Searches within the first _uiptrSize bytes of the block of memory to which _pvBuffer points for the first occurrence of _ui8Value, and returns a pointer to it.
		 *
		 * \param _pvBuffer The buffer to search.
		 * \param _ui8Value The value to find.
		 * \param _uiptrSize Maximum number of bytes to search.
		 * \return Returns a pointer to the found byte or NULL if it was not found.
		 */
		static void * LSE_CALL			MemChr( void * _pvBuffer, uint8_t _ui8Value, uintptr_t _uiptrSize );

		/**
		 * Print formatted text to a string.
		 *
		 * \param _pwcDest Storage location for output.
		 * \param _uiptrMaxLen Maximum number of characters to store.
		 * \param _pwcFormat Format-control string.
		 * \return Returns the number of characters printed, not including the
		 *	terminating NULL.
		 */
		static uint32_t LSE_CALL		WSPrintF( LSUTFX * _pwcDest, uintptr_t _uiptrMaxLen, const LSUTFX * _pwcFormat, ... );

		/**
		 * Print formatted text to a string.
		 *
		 * \param _pcDest Storage location for output.
		 * \param _uiptrMaxLen Maximum number of characters to store.
		 * \param _pcFormat Format-control string.
		 * \return Returns the number of characters printed, not including the
		 *	terminating NULL.
		 */
		static uint32_t LSE_CALL		SPrintF( char * _pcDest, uintptr_t _uiptrMaxLen, const char * _pcFormat, ... );
		
		// Read formatted data from a string.
		// Suspended because Microsoft Visual Studio does not expose vsscanf() (WTF).
		//static int32_t LSE_CALL		SScanF( const char * _pcBuffer, const char * _pcFormat, 
		
		/**
		 * Sleep for the given number of milliseconds.  Passing 0 causes the thread to relinquish
		 *	the remainder of its time until it is scheduled to run again, at which time it will continue.
		 *
		 * \param _ui32Milliseconds Milliseconds to sleep.
		 */
		static void LSE_CALL			Sleep( uint32_t _ui32Milliseconds );

		/**
		 * Byte-swap an 8-byte value.
		 *
		 * \param _ui64Value Value to byte-swap.
		 * \return Returns the byte-swapped value.
		 */
		static uint64_t LSE_CALL		ByteSwap64( uint64_t _ui64Value );

		/**
		 * Byte-swap a 4-byte value.
		 *
		 * \param _ui32Value Value to byte-swap.
		 * \return Returns the byte-swapped value.
		 */
		static uint32_t LSE_CALL		ByteSwap32( uint32_t _ui32Value );

		/**
		 * Byte-swap a 2-byte value.
		 *
		 * \param _ui16Value Value to byte-swap.
		 * \return Returns the byte-swapped value.
		 */
		static uint16_t LSE_CALL		ByteSwap16( uint16_t _ui16Value );

		/**
		 * Swap the bits in a single byte.
		 *
		 * \param _ui8Byte The byte to be bitswapped.
		 * \return Returns the bitswapped byte.
		 */
		static uint8_t LSE_CALL			BitSwap8( uint8_t _ui8Byte );

		/**
		 * Swap the bits in a 16-bit value.
		 *
		 * \param _ui16Value The value to be bitswapped.
		 * \return Returns the bitswapped value.
		 */
		static uint16_t LSE_CALL		BitSwap16( uint16_t _ui16Value );

		/**
		 * Swap the bits in a 32-bit value.
		 *
		 * \param _ui32Value The value to be bitswapped.
		 * \return Returns the bitswapped value.
		 */
		static uint32_t LSE_CALL		BitSwap32( uint32_t _ui32Value );

		/**
		 * Swap the bits in a 64-bit value.
		 *
		 * \param _ui64Value The value to be bitswapped.
		 * \return Returns the bitswapped value.
		 */
		static uint64_t LSE_CALL		BitSwap64( uint64_t _ui64Value );

		/**
		 * Counts the number of bits in a given 32-bit value.
		 *
		 * \param _ui32Value The value whose total number of set bits is to be calculated.
		 * \return Returns the number of bits set in the given value.
		 */
		static uint32_t LSE_CALL		CountBits( uint32_t _ui32Value );

		/**
		 * Counter the number of contiguous 0 bits starting from the least-significant bit in a given 64-bit value.
		 *
		 * \param _ui64Value The value whose least-significant 0 bits are to be counted.
		 * \return Returns the number of contiguous 0 bits starting from the least-significant bit.
		 */
		static uint32_t LSE_CALL		Count0Bits( uint64_t _ui64Value );

		/**
		 * Gets the lowest power-of-2 value not below the given input value.
		 *
		 * \param _ui32Value Value for which to derive the lowest power-of-2 value not under this value.
		 * \return Returns the lowest power-of-2 value not below the given input value.
		 */
		static uint32_t LSE_FCALL		GetLowestPo2( uint32_t _ui32Value );

		/**
		 * Print a wide-character string to the debug console.
		 *
		 * \param _pwcString String to print.
		 */
		static void LSE_CALL			DebugPrintW( const LSUTFX * _pwcString );

		/**
		 * Print a UTF-8 string to the debug console.
		 *
		 * \param _pcString String to print.
		 */
		static void LSE_CALL			DebugPrintA( const char * _pcString );

		/**
		 * Prints an error given an error code.  LSSTD_E_SUCCESS prints nothing.
		 *
		 * \param _eError The error code to print to the debug console.
		 */
		static void LSE_CALL			PrintError( LSSTD_ERRORS _eError );

		/**
		 * Swap 2 values.
		 *
		 * \param _tLeft Left value.
		 * \param _tRight Right value.
		 */
		template <class T>
		static void LSE_CALL			Swap( T &_tLeft, T &_tRight ) {
			T tTemp = _tLeft;
			_tLeft = _tRight;
			_tRight = tTemp;
		}

		/**
		 * Minimum between 2 values.
		 *
		 * \param _tLeft Left value.
		 * \param _tRight Right value.
		 * \return Returns the minimum between the two values.
		 */
		template <class T>
		static T LSE_CALL				Min( T _tLeft, T _tRight ) {
			return _tLeft < _tRight ? _tLeft : _tRight;
		}

		/**
		 * Maximum between 2 values.
		 *
		 * \param _tLeft Left value.
		 * \param _tRight Right value.
		 * \return Returns the maximum between the two values.
		 */
		template <class T>
		static T LSE_CALL				Max( T _tLeft, T _tRight ) {
			return _tLeft > _tRight ? _tLeft : _tRight;
		}

		/**
		 * Clamps the given value between the given range.
		 *
		 * \param _tValue The value to clamp.
		 * \param _tLow Lower limit for the clamped value.
		 * \param _tHigh Upper limit for the clamped value.
		 * \return Returns the given value clamped within the given range.
		 */
		template <class T>
		static T LSE_CALL				Clamp( T _tValue, T _tLow, T _tHigh ) {
			if ( _tValue < _tLow ) { return _tLow; }
			if ( _tValue > _tHigh ) { return _tHigh; }
			return _tValue;
		}

		/**
		 * Implicit type-cast.
		 *
		 * \param _tType The value whose type will be used for the cast.
		 * \param _otVal The value to cast to the type of _tType.
		 * \return Returns the input value cast to _tType's type as specified by the template parameters.
		 */
		template <typename _tType, typename _tOtherType>
		static _tType LSE_FCALL			ImpCast( _tType /*_tType*/, _tOtherType _otVal ) {
			return static_cast<_tType>(_otVal);
		}

		/**
		 * Rounds the given value to the nearest integer value using current rounding direction.
		 *
		 * \param _dValue The value to round.
		 * \return Returns the rounded value.
		 */
		static LSE_INLINE int64_t LSE_CALL
										RoundInt( double _dValue );

		/**
		 * Rounds the given value to the nearest integer value using current rounding direction.
		 *
		 * \param _fValue The value to round.
		 * \return Returns the rounded value.
		 */
		static LSE_INLINE int64_t LSE_CALL
										RoundInt( float _fValue );

		/**
		 * Gets the highest bit set in a raw value.
		 *
		 * \param _ui64Value The value whose highest unsigned bit is to be checked.
		 * \return Returns the highest bit set in the value.
		 */
		static LSE_INLINE uint32_t LSE_FCALL
										HighestBit( uint64_t _ui64Value );

		/**
		 * Gets the highest bit set in a raw value.
		 *
		 * \param _ui32Value The value whose highest unsigned bit is to be checked.
		 * \return Returns the highest bit set in the value.
		 */
		static LSE_INLINE uint32_t LSE_FCALL
										HighestBit( uint32_t _ui32Value );

		/**
		 * Converts a 32-bit float to text.
		 *
		 * \param _pcOutBuffer The output buffer, which must be at least _ui32MaxLen characters in length.  The buffer is always
		 *	terminated with a NULL character unless _ui32MaxLen is 0.
		 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
		 * \param _fVal The value to print.
		 * \return Returns the input string.
		 */
		static char * LSE_CALL			PrintFloat32( char * _pcOutBuffer, uint32_t _ui32MaxLen, float _fVal );

		/**
		 * Converts a 64-bit float to text.
		 *
		 * \param _pcOutBuffer The output buffer, which must be at least _ui64MaxLen characters in length.  The buffer is always
		 *	terminated with a NULL character unless _ui64MaxLen is 0.
		 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
		 * \param _dVal The value to print.
		 * \return Returns the input string.
		 */
		static char * LSE_CALL			PrintFloat64( char * _pcOutBuffer, uint32_t _ui32MaxLen, double _dVal );

		/**
		 * Converts a 80-bit float to text.
		 *
		 * \param _pcOutBuffer The output buffer, which must be at least _ui64MaxLen characters in length.  The buffer is always
		 *	terminated with a NULL character unless _ui64MaxLen is 0.
		 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
		 * \param _pui8Value Pointer to the 10-byte value to print.
		 * \return Returns the input string.
		 */
		static char * LSE_CALL			PrintFloat80( char * _pcOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value );

	private :
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
		static _tCharType * LSE_CALL	FtoA( _tCharType * _ptOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value, uint32_t _ui32Precision, int8_t _i8Format );

		/**
		 * Converts a floating-point number to text.  
		 *
		 * \param _ptOutBuffer The output buffer, which must be at least _ui32MaxLen characters in length.  The buffer is always
		 *	terminated with a NULL character unless _ui32MaxLen is 0.
		 * \param _ui32MaxLen The maximum number of characters to write, including the terminating NULL.
		 * \param _pui8Value The floating-point value to decode, which can be of variable sizes in this templated function.
		 * \return Returns _ptOutBuffer after decoding the value into it.
		 */
		template < unsigned _uMantissaBits, unsigned _uExpBits, unsigned _uImplicitBit, unsigned _uTotalBits, typename _tCharType>
		static _tCharType * LSE_CALL	FtoA( _tCharType * _ptOutBuffer, uint32_t _ui32MaxLen, const uint8_t * _pui8Value );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the next UTF-32 character from a UTF-32 string.
	 *
	 * \param _putf32Char Pointer to the next character to decode.  String must be in UTF-32 format.
	 * \param _ui32Len Length, in LSUTF32 units, of the string.  This is not the number of Unicode
	 *	characters, but actual the number of LSUTF32 characters in the buffer.
	 * \param _pui32Size If not NULL, this holds the returned size of the character in LSUTF32 units.
	 *	However, the UTF-32 coding scheme always uses 1 LSUTF32 character per Unicode character;
	 *	therefore, this value, if not NULL, will always be set to 1.  It is here only for compatibility
	 *	with the other UTF functions.
	 * \return Returns an uint32_t value representing the decoded Unicode character.  Also returns the
	 *	size of the character in LSUTF32 units.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::NextUtf32Char( const LSUTF32 * _putf32Char, uint32_t _ui32Len, uint32_t * _pui32Size ) {
		if ( _ui32Len == 0 ) { return 0; }
		if ( _pui32Size ) {
			(*_pui32Size) = 1;
		}
		uint32_t ui32Ret = (*_putf32Char);
		if ( ui32Ret & 0xFFE00000 ) { return LSSUTF_INVALID; }
		return ui32Ret;
	}

	/**
	 * Converts a raw 32-bit Unicode character to a UTF-32 character.  Returns the UTF-32 character as
	 *	an uint32_t value.  The returned length is the number LSUTF32 characters returned, which is
	 *	always 1.
	 *
	 * \param _ui32Raw The raw Unicode value to convert.
	 * \param _ui32Len The lengh, in LSUTF32 characters, of the converted value.  Always 1.
	 * \return Returns the converted character in uint32_t form along with the length, in units of
	 *	LSUTF32, of the returned value.  Because the mapping between UTF-32 and raw 32-bit Unicode values
	 *	is one-to-one, this value is always 1.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::RawUnicodeToUtf32Char( uint32_t _ui32Raw, uint32_t &_ui32Len ) {
		_ui32Len = 1;
		return _ui32Raw;
	}

#if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )
	/**
	 * Compare the memory at one location with the memory at another location.  Performs a lexicographic comparison.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns the lexicographic relationship between the given buffers.
	 */
	LSE_INLINE int32_t LSE_CALL CStd::MemCmp( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		return ::memcmp( _pvBuf0, _pvBuf1, _uiptrSize );
	}
#endif	// #if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )

#if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )
	/**
	 * Compare the memory at one location with the memory at another location.  Returns only true or
	 *	false.  Does not perform a lexicographic comparison.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns true if the buffers' contents are an exact match, false otherwise.
	 */
	LSE_INLINE LSBOOL LSE_CALL CStd::MemCmpF( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		return ::memcmp( _pvBuf0, _pvBuf1, _uiptrSize ) == 0;
	}
#endif	// #if defined( __LSE_CRT_MEMFUNCS__ ) || defined( __LSE_CRT_MEMCMP__ )

	/**
	 * Compares the memory at one location with the memory at another location.  Returns only true or
	 *	false.  Does not perform a lexicographic comparison.  The addresses are assumed to be aligned to 4-bytes
	 *	and the size to be a multiple of 4.  Internally, with these assumptions, this always performs a 32-bit
	 *	compare across the provided data.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns true if the buffers' contents are an exact match, false otherwise.
	 */
	LSE_INLINE LSBOOL LSE_CALL CStd::MemCmpF32( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		const uint32_t * pui32Buf0 = reinterpret_cast<const uint32_t *>(_pvBuf0);
		const uint32_t * pui32Buf1 = reinterpret_cast<const uint32_t *>(_pvBuf1);
		for ( uintptr_t I = _uiptrSize >> 2; I--; ) {
			if ( (*pui32Buf0++) != (*pui32Buf1++) ) { return false; }
		}
		return true;
	}

	/**
	 * Compares the memory at one location with the memory at another location.  Returns only true or
	 *	false.  Does not perform a lexicographic comparison.  The addresses are assumed to be aligned to the native
	 *	pointer size and the size a multiple of the native machine pointer size.  Internally, with these assumptions,
	 *	it performs a 32-bit-per-comparison check on 32-bit machines and 64-bits-per-check on 64-bit machines.
	 *
	 * \param _pvBuf0 Buffer to compare.
	 * \param _pvBuf1 Buffer to compare.
	 * \param _uiptrSize Number of bytes to compare.
	 * \return Returns true if the buffers' contents are an exact match, false otherwise.
	 */
	LSE_INLINE LSBOOL LSE_CALL CStd::MemCmpFUPtr( const void * _pvBuf0, const void * _pvBuf1, uintptr_t _uiptrSize ) {
		const uintptr_t * puiptrBuf0 = reinterpret_cast<const uintptr_t *>(_pvBuf0);
		const uintptr_t * puiptrBuf1 = reinterpret_cast<const uintptr_t *>(_pvBuf1);
		for ( uintptr_t I = _uiptrSize / sizeof( uintptr_t ); I--; ) {
			if ( (*puiptrBuf0++) != (*puiptrBuf1++) ) { return false; }
		}
		return true;
	}

	/**
	 * Byte-swap an 8-byte value.
	 *
	 * \param _ui64Value Value to byte-swap.
	 * \return Returns the byte-swapped value.
	 */
	LSE_INLINE uint64_t LSE_CALL CStd::ByteSwap64( uint64_t _ui64Value ) {
		return ((_ui64Value >> 56) & 0x00000000000000FFULL) |
			((_ui64Value >> 40) & 0x000000000000FF00ULL) |
			((_ui64Value >> 24) & 0x0000000000FF0000ULL) |
			((_ui64Value >> 8) & 0x00000000FF000000ULL) |
			((_ui64Value << 8) & 0x000000FF00000000ULL) |
			((_ui64Value << 24) & 0x0000FF0000000000ULL) |
			((_ui64Value << 40) & 0x00FF000000000000ULL) |
			((_ui64Value << 56) & 0xFF00000000000000ULL);
	}

	/**
	 * Byte-swap a 4-byte value.
	 *
	 * \param _ui32Value Value to byte-swap.
	 * \return Returns the byte-swapped value.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::ByteSwap32( uint32_t _ui32Value ) {
		return ((_ui32Value >> 24) & 0x000000FF) |
			((_ui32Value >> 8) & 0x0000FF00) |
			((_ui32Value << 8) & 0x00FF0000) |
			((_ui32Value << 24) & 0xFF000000);
	}

	/**
	 * Byte-swap a 2-byte value.
	 *
	 * \param _ui16Value Value to byte-swap.
	 * \return Returns the byte-swapped value.
	 */
	LSE_INLINE uint16_t LSE_CALL CStd::ByteSwap16( uint16_t _ui16Value ) { 
		return ((_ui16Value >> 8) & 0x00FF) |
			((_ui16Value << 8) & 0xFF00);
	}

	/**
	 * Swap the bits in a single byte.
	 *
	 * \param _ui8Byte The byte to be bitswapped.
	 * \return Returns the bitswapped byte.
	 */
	LSE_INLINE uint8_t LSE_CALL CStd::BitSwap8( uint8_t _ui8Byte ) {
		_ui8Byte = static_cast<uint8_t>((_ui8Byte & 0xF0) >> 4 | (_ui8Byte & 0x0F) << 4);
		_ui8Byte = static_cast<uint8_t>((_ui8Byte & 0xCC) >> 2 | (_ui8Byte & 0x33) << 2);
		_ui8Byte = static_cast<uint8_t>((_ui8Byte & 0xAA) >> 1 | (_ui8Byte & 0x55) << 1);
		return _ui8Byte;
	}

	/**
	 * Swap the bits in a 16-bit value.
	 *
	 * \param _ui16Value The value to be bitswapped.
	 * \return Returns the bitswapped value.
	 */
	LSE_INLINE uint16_t LSE_CALL CStd::BitSwap16( uint16_t _ui16Value ) {
		_ui16Value = static_cast<uint16_t>(((_ui16Value & 0xAAAA) >>  1) | ((_ui16Value & 0x5555) << 1));
		_ui16Value = static_cast<uint16_t>(((_ui16Value & 0xCCCC) >>  2) | ((_ui16Value & 0x3333) << 2));
		_ui16Value = static_cast<uint16_t>(((_ui16Value & 0xF0F0) >>  4) | ((_ui16Value & 0x0F0F) << 4));
		return static_cast<uint16_t>(((_ui16Value & 0xFF00) >>  8) | ((_ui16Value & 0x00FF) << 8));
	}

	/**
	 * Swap the bits in a 32-bit value.
	 *
	 * \param _ui32Value The value to be bitswapped.
	 * \return Returns the bitswapped value.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::BitSwap32( uint32_t _ui32Value ) {
		_ui32Value = ((_ui32Value >> 1) & 0x55555555UL) | ((_ui32Value << 1) & 0xAAAAAAAAUL);
		_ui32Value = ((_ui32Value >> 2) & 0x33333333UL) | ((_ui32Value << 2) & 0xCCCCCCCCUL);
		_ui32Value = ((_ui32Value >> 4) & 0x0F0F0F0FUL) | ((_ui32Value << 4) & 0xF0F0F0F0UL);
		_ui32Value = ((_ui32Value >> 8) & 0x00FF00FFUL) | ((_ui32Value << 8) & 0xFF00FF00UL);
		return ((_ui32Value >> 16) & 0x0000FFFFUL) | ((_ui32Value << 16) & 0xFFFF0000UL);
	}

	/**
	 * Swap the bits in a 64-bit value.
	 *
	 * \param _ui64Value The value to be bitswapped.
	 * \return Returns the bitswapped value.
	 */
	LSE_INLINE uint64_t LSE_CALL CStd::BitSwap64( uint64_t _ui64Value ) {
		_ui64Value = ((_ui64Value & 0x5555555555555555ULL) >> 1ULL) | ((_ui64Value & 0xAAAAAAAAAAAAAAAAULL) << 1ULL);
		_ui64Value = ((_ui64Value & 0xCCCCCCCCCCCCCCCCULL) >> 2) | ((_ui64Value & 0x3333333333333333ULL) << 2);
		_ui64Value = ((_ui64Value & 0xF0F0F0F0F0F0F0F0ULL) >> 4ULL) | ((_ui64Value & 0x0F0F0F0F0F0F0F0FULL) << 4ULL);
		_ui64Value = ((_ui64Value & 0x00FF00FF00FF00FFULL) >> 8ULL) | ((_ui64Value & 0xFF00FF00FF00FF00ULL) << 8ULL);
		_ui64Value = ((_ui64Value & 0x0000FFFF0000FFFFULL) >> 16ULL) | ((_ui64Value & 0xFFFF0000FFFF0000ULL) << 16ULL);
		return ((_ui64Value & 0xFFFFFFFF00000000ULL) >> 32ULL) | ((_ui64Value & 0x00000000FFFFFFFFULL) << 32ULL);
	}

	/**
	 * Counts the number of bits in a given 32-bit value.
	 *
	 * \param _ui32Value The value whose total number of set bits is to be calculated.
	 * \return Returns the number of bits set in the given value.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::CountBits( uint32_t _ui32Value ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = static_cast<uint32_t>(1U << ((sizeof( _ui32Value ) << 3) - 1)); I != 0; I >>= 1 ) {
			if ( _ui32Value & I ) { ++ui32Total; }
		}
		return ui32Total;
	}

	/**
	 * Counter the number of contiguous 0 bits starting from the least-significant bit in a given 64-bit value.
	 *
	 * \param _ui64Value The value whose least-significant 0 bits are to be counted.
	 * \return Returns the number of contiguous 0 bits starting from the least-significant bit.
	 */
	LSE_INLINE uint32_t LSE_CALL CStd::Count0Bits( uint64_t _ui64Value ) {
		uint32_t ui32Total = 0;
		if ( _ui64Value ) {
			while ( !(_ui64Value & (1ULL << ui32Total)) ) { ++ui32Total; }
		}
		return ui32Total;
	}

	/**
	 * Gets the lowest power-of-2 value not below the given input value.
	 *
	 * \param _ui32Value Value for which to derive the lowest power-of-2 value not under this value.
	 * \return Returns the lowest power-of-2 value not below the given input value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CStd::GetLowestPo2( uint32_t _ui32Value ) {
		if ( !_ui32Value ) { return 0; }
#ifdef LSE_X86
		// On x86 processors there is an instruction that gets the highest-
		//	set bit automatically.
		uint32_t ui32Ret;
		LSE_ASM_BEGIN
			xor eax, eax
			bsr eax, _ui32Value
			mov ui32Ret, eax
		LSE_ASM_END
		ui32Ret = 1 << ui32Ret;
		return (ui32Ret == _ui32Value) ? ui32Ret : ui32Ret << 1;
#else	// LSE_X86
		// Get it the hard way.
		uint32_t ui32Ret = 1;
		while ( ui32Ret < _ui32Value ) { ui32Ret <<= 1; }

		// By now, ui32Ret either equals _ui32Value or is the next power of 2 up.
		// If they are equal, _ui32Value is already a power of 2.
		return ui32Ret;
#endif	// LSE_X86
	}

	/**
	 * Rounds the given value to the nearest integer value using current rounding direction.
	 *
	 * \param _dValue The value to round.
	 * \return Returns the rounded value.
	 */
	LSE_INLINE int64_t LSE_CALL CStd::RoundInt( double _dValue ) {
		return static_cast<int64_t>(_dValue >= 0.0 ? _dValue + 0.5 : _dValue - 0.5);
	}

	/**
	 * Rounds the given value to the nearest integer value using current rounding direction.
	 *
	 * \param _fValue The value to round.
	 * \return Returns the rounded value.
	 */
	LSE_INLINE int64_t LSE_CALL CStd::RoundInt( float _fValue ) {
		return static_cast<int64_t>(_fValue >= 0.0f ? _fValue + 0.5f : _fValue - 0.5f);
	}

	/**
	 * Gets the highest bit set in a raw value.
	 *
	 * \param _ui64Value The value whose highest unsigned bit is to be checked.
	 * \return Returns the highest bit set in the value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CStd::HighestBit( uint64_t _ui64Value ) {
		if ( !_ui64Value ) { return 0; }
#ifdef LSE_X86
		// On x86 processors there is an instruction that gets the highest-
		//	set bit automatically.
		uint32_t ui32Ret;
		int32_t i32High = static_cast<int32_t>(_ui64Value >> 32ULL);
		LSE_ASM_BEGIN
			xor eax, eax
			bsr eax, i32High
			mov ui32Ret, eax
		LSE_ASM_END
		if ( ui32Ret ) {
			return ui32Ret + 32;
		}

		i32High = static_cast<int32_t>(_ui64Value & 0xFFFFFFFFULL);
		LSE_ASM_BEGIN
			bsr eax, i32High
			mov ui32Ret, eax
		LSE_ASM_END
		return ui32Ret;
#else	// LSE_X86
		// Get it the hard way.
		uint32_t ui32Ret = (sizeof( _ui64Value ) << 3);
		for ( ; ui32Ret--;  ) {
			if ( _ui64Value & (1ULL << ui32Ret) ) { return ui32Ret; }
		}
		return 0;
#endif	// LSE_X86
	}

	/**
	 * Gets the highest bit set in a raw value.
	 *
	 * \param _ui32Value The value whose highest unsigned bit is to be checked.
	 * \return Returns the highest bit set in the value.
	 */
	LSE_INLINE uint32_t LSE_FCALL CStd::HighestBit( uint32_t _ui32Value ) {
		if ( !_ui32Value ) { return 0; }
#ifdef LSE_X86
		// On x86 processors there is an instruction that gets the highest-
		//	set bit automatically.
		uint32_t ui32Ret;
		LSE_ASM_BEGIN
			xor eax, eax
			bsr eax, _ui32Value
			mov ui32Ret, eax
		LSE_ASM_END
		return ui32Ret;
#else	// LSE_X86
		// Get it the hard way.
		uint32_t ui32Ret = (sizeof( _ui32Value ) << 3);
		for ( ; ui32Ret--;  ) {
			if ( _ui32Value & (1 << ui32Ret) ) { return ui32Ret; }
		}
		return 0;
#endif	// LSE_X86
	}

}	// namespace lsstd

#endif	// __LSSTD_STANDARDLIB_H__
