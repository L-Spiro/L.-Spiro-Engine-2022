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
 * Description: The base of strings.  There are two types of strings: char and wchar_t.  The string always
 *	ends with a NULL character, and may additionally contain more NULL characters.  No formatting is done on
 *	the string; new-lines and line-feeds (etc.) are not formatted per operating system, white space is not
 *	reduced, etc.  The string is entirely binary in nature, and can be used to reliably hold non-textual data
 *	in addition to regular printable strings.
 */


#ifndef __LSTL_STRINGBASE_H__
#define __LSTL_STRINGBASE_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"
#include "../Vector/LSTLVectorPoD.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Do we want the operators to throw exceptions?  Define this if so.
//#define __LSTL_STRINGS_THROW_EXCEPTIONS__

#if defined( __LSTL_STRINGS_THROW_EXCEPTIONS__ )
#define LSTL_STRINGS_THROW_DECL												throw( ... )
#define LSTL_STRINGS_THROW( VAL )											throw( VAL )
#else
#define LSTL_STRINGS_THROW_DECL
#define LSTL_STRINGS_THROW( VAL )
#endif	// #if defined( __LSTL_STRINGS_THROW_EXCEPTIONS__ )


namespace lstl {

	// == Enumerations.
	// String exceptions.
	enum LSTL_STRING_EXCEPTIONS {
		LSTL_SE_OUTOFMEMORY
	};

	/**
	 * Class CStringBase
	 *
	 * Description: The base of strings.
	 */
	template <typename _tDerived, typename _tDataType, unsigned _uAllocSize = 32>
	class CStringBase : protected CVectorPoD<_tDataType, uint32_t, _uAllocSize> {
	public :
		// == Various constructors.
		explicit LSE_CALLCTOR												CStringBase( CAllocator * _paAllocator = NULL ) :
			Parent( _paAllocator ) {
		}
		LSE_CALLCTOR														CStringBase( const _tDataType &_dtValue, CAllocator * _paAllocator = NULL ) :
			Parent( _paAllocator ) {
			Append( _dtValue );
		}
		LSE_CALLCTOR														CStringBase( const _tDataType * _pdtValue, CAllocator * _paAllocator = NULL ) :
			Parent( _paAllocator ) {
			Set( _pdtValue );
		}
		LSE_CALLCTOR														CStringBase( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString, CAllocator * _paAllocator = NULL ) :
			Parent( _paAllocator ) {
			Set( _sbString );
		}


		// == Types.
		/**
		 * A range of characters.
		 */
		typedef struct LSSTD_RANGE {
			/**
			 * Beginning character.
			 */
			uint32_t														ui32Start;

			/**
			 * End character non-inclusive.
			 */
			uint32_t														ui32End;
		} * LPLSSTD_RANGE, * const LPCLSSTD_RANGE;


		// == Operators.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// +
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Append this string with a character and return the result as a new string.
		 * The returned string uses the same allocator as this string.
		 *
		 * \param _dtValue The character to append.
		 * \return Returns a new string representing the concatenation of this string and the given character.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> LSE_CALL			operator + ( const _tDataType &_dtValue ) LSTL_STRINGS_THROW_DECL {
			CStringBase sbRet( (*this), Parent::m_paOurAllocator );
			if ( !sbRet.Append( _dtValue ) ) {
				LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY );
			}
			return sbRet;
		}

		/**
		 * Append this string with a NULL-terminated character array and return the result as a new string.
		 * The returned string uses the same allocator as this string.
		 *
		 * \param _pdtValue The character array to append.
		 * \return Returns a new string representing the concatenation of this string and the given NULL-terminated string.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> LSE_CALL			operator + ( const _tDataType * _pdtValue ) LSTL_STRINGS_THROW_DECL {
			CStringBase sbRet( (*this), Parent::m_paOurAllocator );
			if ( !sbRet.Append( _pdtValue ) ) {
				LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY );
			}
			return sbRet;
		}

		/**
		 * Append this string with another string and return the result as a new string.
		 * The returned string uses the same allocator as this string.
		 *
		 * \param _sbString The string to append.
		 * \return Returns a new string representing the concatenation of this string and the given string.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> LSE_CALL			operator + ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) LSTL_STRINGS_THROW_DECL {
			CStringBase sbRet( (*this), Parent::m_paOurAllocator );
			if ( !sbRet.Append( _sbString ) ) {
				LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY );
			}
			return sbRet;
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// +=
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Append a character to this string and return this string as the result.
		 *
		 * \param _dtValue The character to append to this string.
		 * \return Returns this string with the given character appended.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> & LSE_CALL			operator += ( const _tDataType &_dtValue ) LSTL_STRINGS_THROW_DECL {
			if ( !Append( _dtValue ) ) { LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY ); }
			return (*this);
		}

		/**
		 * Append a NULL-terminated character array to this string and return this string as the result.
		 *
		 * \param _pdtValue The NULL-terminated character array to append to this string.
		 * \return Returns this string with the given character array appended.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> & LSE_CALL			operator += ( const _tDataType * _pdtValue ) LSTL_STRINGS_THROW_DECL {
			if ( !Append( _pdtValue ) ) { LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY ); }
			return (*this);
		}

		/**
		 * Append a string to this string and return this string as the result.
		 *
		 * \param _sbString The string to append to this string.
		 * \return Returns this string with the given string appended.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> & LSE_CALL			operator += ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) LSTL_STRINGS_THROW_DECL {
			if ( !Append( _sbString ) ) { LSTL_STRINGS_THROW( LSTL_SE_OUTOFMEMORY ); }
			return (*this);
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// COMPARISONS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Do we equal another NULL-terminated character array?
		 *
		 * \param _pdtValue The character array against which this string is to be compared for a match.
		 * \return Returns true if the given character array matches this string exactly, false otherwise.
		 */
		LSBOOL LSE_CALL														operator == ( const _tDataType * _pdtValue ) const {
			const _tDataType * pdtCopy = _pdtValue;
			uint32_t ui32Len = 0;
			while ( (*pdtCopy) && ui32Len < Length() ) {
				if ( (*pdtCopy++) != Parent::m_ptData[ui32Len++] ) { return false; }
			}
			// One or both of the strings ended.  If their lengths are the same, they are equal.
			if ( (*pdtCopy) ) { return false; }
			return static_cast<uint32_t>(pdtCopy - _pdtValue) == Length();
		}

		/**
		 * Do we equal another string?
		 *
		 * \param _sbString The string against which this string is to be compared for a match.
		 * \return Returns true if the given string matches this string exactly, false otherwise.
		 */
		LSBOOL LSE_CALL														operator == ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			return Equals( _sbString );
		}

		/**
		 * Do we not equal another NULL-terminated character array?
		 *
		 * \param _pdtValue The character array against which this string is to be compared for a match.
		 * \return Returns true if the given character array does not matche this string exactly, false otherwise.
		 */
		LSBOOL LSE_CALL														operator != ( const _tDataType * _pdtValue ) const {
			const _tDataType * pdtCopy = _pdtValue;
			uint32_t ui32Len = 0;
			while ( (*pdtCopy) && ui32Len < Length() ) {
				if ( (*pdtCopy++) != Parent::m_ptData[ui32Len++] ) { return true; }
			}
			// One or both of the strings ended.  If their lengths are the same, they are equal.
			return ui32Len != Length();
		}

		/**
		 * Do we not equal another string?
		 *
		 * \param _sbString The string against which this string is to be compared for a match.
		 * \return Returns true if the given string does not matche this string exactly, false otherwise.
		 */
		LSBOOL LSE_CALL														operator != ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			return !Equals( _sbString );
		}

		/**
		 * Are we lexicographically less than another string?  Comparison is case-sensitive.
		 *
		 * \param _sbString The string against which we are to be compared.
		 * \return Returns true if this string object is less than the given string object when compared
		 *	lexicographically.
		 */
		LSBOOL LSE_CALL														operator < ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			return Cmp( _sbString ) < 0;
		}

		/**
		 * Are we lexicographically gretaer than another string?  Comparison is case-sensitive.
		 *
		 * \param _sbString The string against which we are to be compared.
		 * \return Returns true if this string object is greater than the given string object when compared
		 *	lexicographically.
		 */
		LSBOOL LSE_CALL														operator > ( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			return Cmp( _sbString ) > 0;
		}


		// == Functions.
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// ALLOCATION
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Allocates the given number of characters.
		 *
		 * \param _ui32Total The new total number of values to allocate.
		 * \return Returns false if there is not enough memory to allocate the requested amount.
		 */
		LSBOOL LSE_CALL														AllocateAtLeast( uint32_t _ui32Total ) {
			if ( _ui32Total <= Parent::m_tLen + 1 ) { return true; }	// Nothing to do.
			return Parent::Allocate( _ui32Total );
		}

		/**
		 * Gets the amount allocated.
		 *
		 * \return Returns the number of characters allocated.
		 */
		uint32_t LSE_CALL													Allocated() const {
			return Parent::Allocated();
		}

		/**
		 * Reset everything.  Deallocates memory and pretends it never existed.  The allocator is not
		 *	changed by this call and does not need to be re-applied.
		 */
		void LSE_CALL														Reset() { Parent::Reset(); }

		/**
		 * Trash the string.  Resets the string entirely without deallocating any memory.  This must be used
		 *	only in conjunction with trashable heaps.
		 */
		void LSE_CALL														Trash() {
			Parent::m_tLen = Parent::m_tAllocated = 0;
			Parent::m_ptData = NULL;
		}

		/**
		 * Resets the string without deallocating memory.  This is the fastest way to clear the string.  This
		 *	cannot cause a memory leak; the existing allocated area will be used for future string operations.
		 */
		void LSE_CALL														ResetNoDealloc() { Parent::ResetNoDealloc(); }

		/**
		 * Sets the allocator.  Calls Reset() internally, so the string must not contain any useful content.
		 *
		 * \param _paAllocator Pointer to the new allocator to be used by this object.  Can be NULL.
		 */
		void LSE_CALL														SetAllocator( CAllocator * _paAllocator ) { Parent::SetAllocator( _paAllocator ); }

		/**
		 * Gets the allocator used by this object.
		 *
		 * \return Returns a pointer to the allocator used by this object, which may be NULL.
		 */
		CAllocator * LSE_CALL												GetAllocator() { return Parent::GetAllocator(); }

		/**
		 * Snap the amount of allocated data to fit the string exactly.  When creating strings, extra
		 *	bytes are normally allocated so that appending operations will be faster.  Additionally, extra
		 *	bytes may be trailing after removing characters.  Use this to free unused memory.
		 */
		void LSE_CALL														Snap() {
			this->Allocate( Parent::m_tLen + 1 );
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// C-STRING
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Gets the length of the string.
		 *
		 * \return Returns the length of the string in characters.
		 */
		uint32_t LSE_CALL													Length() const { return Parent::Length(); }

		/**
		 * Gets the C-string pointer.  For read only!
		 
		 * \return Returns a pointer to the string data in this object that can be used in regular string
		 *	functions.
		 */
		const _tDataType * LSE_CALL											CStr() const {
			// An empty string handles the cases where our string is empty.  When our string is
			//	empty the data pointer may also be NULL.  Avoid trouble by returning a valid 0-length
			//	character array when our length is 0.
			static const _tDataType dtDefault( 0 );
			return Length() ? Parent::m_ptData : &dtDefault;
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// APPEND/REMOVE/SET
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Append a character.  Returns false if the append failed (due to memory).  NULL characters CAN be
		 *	appended to the string.
		 *
		 * \param _dtValue The character to append.
		 * \return Returns false if there is a memory failure preventing the append operation.
		 */
		LSBOOL LSE_CALL														Append( const _tDataType &_dtValue ) {
			uint32_t ui32NewLen = Parent::m_tLen + 2;
			if ( ui32NewLen >= Parent::m_tAllocated ) {
				if ( !Parent::Allocate( ui32NewLen + _uAllocSize ) ) { return false; }
			}
			Parent::m_ptData[Parent::m_tLen++] = _dtValue;
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );
			return true;
		}

		/**
		 * Append a string of characters.  Returns false if the append failed (due to memory).  The string
		 *	of characters can contain any characters, including NULL characters.
		 *
		 * \param _pdtValue Pointer to the characters to append to this string.
		 * \param _ui32Len Number of characters to append to this string.
		 * \return Returns false if there is a memory failure preventing the append operation.
		 */
		LSBOOL LSE_CALL														Append( const _tDataType * _pdtValue, uint32_t _ui32Len ) {
			uint32_t ui32NewLen = Parent::m_tLen + _ui32Len + 1;
			if ( ui32NewLen >= Parent::m_tAllocated ) {
				if ( !Parent::Allocate( ui32NewLen + _uAllocSize ) ) { return false; }
			}
			CStd::MemCpy( &Parent::m_ptData[Parent::m_tLen], _pdtValue, sizeof( _tDataType ) * _ui32Len );
			Parent::m_tLen += _ui32Len;
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );
			return true;
		}

		/**
		 * Append a string of characters.  Returns false if the append failed (due to memory).  The string
		 *	must be NULL-terminated.
		 *
		 * \param _pdtValue Pointer to the NULL-terminated characters to append to this string.
		 * \return Returns false if there is a memory failure preventing the append operation.
		 */
		LSBOOL LSE_CALL														Append( const _tDataType * _pdtValue ) {
			// Redirect.
			return Append( _pdtValue, static_cast<_tDerived *>(this)->StrLen( _pdtValue ) );
		}

		/**
		 * Append another string object.  Returns false if the append failed (due to memory).
		 *
		 * \param _sbString The string object to append to this string.
		 * \return Returns false if there is a memory failure preventing the append operation.
		 */
		LSBOOL LSE_CALL														Append( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) {
			// Redirect.
			return Append( _sbString.Parent::m_ptData, _sbString.Length() );
		}

		/**
		 * Removes a character at a given index.  If the index is out of bounds, no action is performed.
		 *
		 * \param _ui32Index The index of the character to remove.
		 */
		void LSE_CALL														RemChar( uint32_t _ui32Index ) {
			if ( _ui32Index >= Length() ) { return; }
			Parent::RemoveNoDealloc( _ui32Index );
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );
		}

		/**
		 * Removes a series of characters beginning at a given index.
		 *	Any characters not in bounds are ignored.
		 *
		 * \param _ui32Index The starting index of the characters to be removed.
		 * \param _ui32Total The total number of characters to remove.
		 */
		void LSE_CALL														RemChars( uint32_t _ui32Index, uint32_t _ui32Total ) {
			if ( _ui32Index >= Length() ) { return; }
			if ( _ui32Index + _ui32Total > Length() ) {
				_ui32Total = Length() - _ui32Index;
			}
			Parent::RemoveRangeNoDealloc( _ui32Index, _ui32Total );
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );
		}

		/**
		 * Removes a range of characters.
		 *
		 * \param _rRange Range of characters to remove.
		 */
		void LSE_CALL														RemRange( const LSSTD_RANGE &_rRange ) {
			RemChars( _rRange.ui32Start, _rRange.ui32End - _rRange.ui32Start );
		}

		/**
		 * Removes the last character from the string, if there is one.
		 *
		 * \return Returns true if a character was removed.
		 */
		LSBOOL LSE_CALL														RemLastChar() {
			if ( Parent::m_tLen == 0 ) { return false; }
			RemChar( Parent::m_tLen - 1 );
			return true;
		}

		/**
		 * Sets the string value.  The new value may contain any characters, including NULL characters.
		 *	The new string is always NULL-terminated.  Returns false if the set failed (due to memory).
		 *
		 * \param _pdtValue Pointer to the characters to which to set this string.
		 * \param _ui32Len The number of characters to which _pdtValue points.
		 * \return Returns false if there is a memory failure preventing the copy operation.
		 */
		LSBOOL LSE_CALL														Set( const _tDataType * _pdtValue, uint32_t _ui32Len ) {
			if ( (_ui32Len + 1) > Parent::m_tAllocated ) {
				if ( !Parent::Allocate( (_ui32Len + 1) ) ) { return false; }
			}
			CStd::MemCpy( &Parent::m_ptData[0], _pdtValue, sizeof( _tDataType ) * _ui32Len );
			Parent::m_tLen = _ui32Len;
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );
			return true;
		}

		/**
		 * Sets the string value.  The given string must be NULL-terminated.  Returns false if the set
		 *	failed (due to memory).
		 *
		 * \param _pdtValue Pointer to the NULL-terminated characters to which to set this string.
		 * \return Returns false if there is a memory failure preventing the copy operation.
		 */
		LSBOOL LSE_CALL														Set( const _tDataType * _pdtValue ) {
			return Set( _pdtValue, static_cast<_tDerived *>(this)->StrLen( _pdtValue ) );
		}

		/**
		 * Sets the string value from another string object.  Returns false if the set failed (due to
		 *	memory).
		 *
		 * \param _sbString The string to which to set this string.
		 * \return Returns false if there is a memory failure preventing the copy operation.
		 */
		LSBOOL LSE_CALL														Set( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) {
			return Set( _sbString.Parent::m_ptData, _sbString.Length() );
		}

		/**
		 * Sets a range of characters to a new value.  The range of characters is removed, then the new value is inserted at the start
		 *	position of the range.
		 *
		 * \param _rRange Range of characters to replace with the given set of characters.
		 * \param _pdtValue Pointer to the characters to replace in this string.
		 * \param _ui32Len The number of characters to which _pdtValue points.
		 * \return Returns false if there is a memory failure preventing the replace operation.
		 */
		LSBOOL LSE_CALL														Replace( const LSSTD_RANGE &_rRange,
			const _tDataType * _pdtValue, uint32_t _ui32Len ) {
			// Firstly, if the start of the range is beyond the last character, do nothing.
			if ( _rRange.ui32Start > Parent::m_tLen ) {
				return false;
			}
			// If adding to the end, just append.
			if ( _rRange.ui32Start == Parent::m_tLen ) {
				return Append( _pdtValue, _ui32Len );
			}


			uint32_t ui32End = CStd::Min( Parent::m_tLen - _rRange.ui32Start, _rRange.ui32End - _rRange.ui32Start );
			// If the new value has the same length as the old, simply copy the value over.
			if ( ui32End == _ui32Len ) {
				CStd::MemCpy( &Parent::m_ptData[_rRange.ui32Start], _pdtValue, sizeof( _tDataType ) * _ui32Len );
				return true;
			}
			// Remove the given range.
			RemRange( _rRange );
			// Insert the given string.
			return Insert( _rRange.ui32Start, _pdtValue, _ui32Len );
		}

		/**
		 * Sets a range of characters to a new value.  The range of characters is removed, then the new value is inserted at the start
		 *	position of the range.
		 *
		 * \param _rRange Range of characters to replace with the given set of characters.
		 * \param _pdtValue Pointer to the NULL-terminated characters to which to replace the range of characters in this string.
		 * \return Returns false if there is a memory failure preventing the replace operation.
		 */
		LSBOOL LSE_CALL														Replace( const LSSTD_RANGE &_rRange,
			const _tDataType * _pdtValue ) {
			return Replace( _rRange, _pdtValue, static_cast<_tDerived *>(this)->StrLen( _pdtValue ) );
		}

		/**
		 * Insert characters at a given position in the string.
		 *
		 * \param _ui32Pos Position at which to insert the characters.
		 * \param _pdtValue Pointer to the characters to insert into this string.
		 * \param _ui32Len The number of characters to which _pdtValue points.
		 * \return Returns false if there is a memory failure preventing the insert operation or if the insert position is beyond the
		 *	length of the string.
		 */
		LSBOOL LSE_CALL														Insert( uint32_t _ui32Pos,
			const _tDataType * _pdtValue, uint32_t _ui32Len ) {
			if ( !_pdtValue ) { return false; }
			if ( _ui32Pos > Parent::m_tLen ) {
				return false;
			}
			if ( _ui32Pos == Parent::m_tLen ) {
				return Append( _pdtValue, _ui32Len );
			}

			// Expand the string to give us enough room to insert.
			uint32_t ui32NewLen = Parent::m_tLen + _ui32Len + 1;
			if ( ui32NewLen >= Parent::m_tAllocated ) {
				if ( !Parent::Allocate( ui32NewLen + _uAllocSize ) ) { return false; }
			}
			CStd::MemMove( &Parent::m_ptData[_ui32Pos+_ui32Len], &Parent::m_ptData[_ui32Pos], sizeof( _tDataType ) * (Parent::m_tLen - _ui32Pos) );
			CStd::MemCpy( &Parent::m_ptData[_ui32Pos], _pdtValue, sizeof( _tDataType ) * _ui32Len );
			Parent::m_tLen += _ui32Len;
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 ); 
			return true;
		}

		/**
		 * Insert characters at a given position in the string.  The string to which _pdtValue points must be NULL-terminated.
		 *
		 * \param _ui32Pos Position at which to insert the characters.
		 * \param _pdtValue Pointer to the characters to insert into this string. which must be NULL-terminated.
		 * \return Returns false if there is a memory failure preventing the insert operation or if the insert position is beyond the
		 *	length of the string.
		 */
		LSBOOL LSE_CALL														Insert( uint32_t _ui32Pos,
			const _tDataType * _pdtValue ) {
			if ( !_pdtValue ) { return false; }
			return Insert( _ui32Pos, _pdtValue, StrLen( _pdtValue ) );
		}

		/**
		 * Insert a single character at a given position.
		 *
		 * \param _ui32Pos Position at which to insert the character.
		 * \param _dtValue The value to insert into this string.
		 * \return Returns false if there is a memory failure preventing the insert operation or if the insert position is beyond the
		 *	length of the string.
		 */
		LSBOOL LSE_CALL														Insert( uint32_t _ui32Pos,
			_tDataType _dtValue ) {
			
			return Insert( _ui32Pos, &_dtValue, 1 );
		}

		/**
		 * Gets the last character in the string  If no such character exists, the zero character is returned.
		 *
		 * \return Returns the last character in the string or 0 if the string is empty.
		 */
		_tDataType LSE_CALL													GetLastChar() const {
			if ( !Parent::m_tLen ) { return _tDataType( 0 ); }
			return Parent::m_ptData[Parent::m_tLen-1];
		}

		/**
		 * Gets a token from this string.
		 *
		 * \param _tDelimiter The token delimiter
		 * \param _ui32Index Index of the token to get.
		 * \return Returns a new string containing the result.  If no such token exists, an empty string is returned.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> LSE_CALL			GetToken( _tDataType _tDelimiter, uint32_t _ui32Index ) const {
			CStringBase<_tDerived, _tDataType, _uAllocSize> sbRet;

			uint32_t I = 0;
			for ( ++_ui32Index; _ui32Index > 0; --_ui32Index ) {
				if ( I == Parent::m_tLen ) { break; }

				// Skip the delimiter tokens, if any.
				_tDataType tThis = Parent::m_ptData[I++];
				while ( tThis == _tDelimiter && I < Parent::m_tLen ) {
					tThis = Parent::m_ptData[I++];
				}

				// The next set of characters make a token until the next delimiter.
				//	If _ui32Index is 1, we store these characters, otherwise not.
				while ( tThis != _tDelimiter ) {
					if ( _ui32Index == 1 ) {
						sbRet += tThis;
					}

					if ( I == Parent::m_tLen ) {
						break;
					}
					tThis = Parent::m_ptData[I++];
				}
			}

			return sbRet;
		}

		/**
		 * Finds the first occurrence of the given input string from the given starting position.  If the given string is not found,
		 *	the ui32End member of the return is set to 0 and the ui32Start member is set to the length of the string.
		 *
		 * \param _ui32Start The location from where to search, inclusive.
		 * \param _sbString The string to locate within this string object.
		 * \return Returns the range of the located string.
		 */
		LSSTD_RANGE LSE_CALL												FindStringMatchCase( uint32_t _ui32Start,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			LSSTD_RANGE rRet;
			rRet.ui32Start = Parent::m_tLen;
			rRet.ui32End = 0;

			// Sanity check.
			if ( _sbString.Length() + _ui32Start > Parent::m_tLen || _ui32Start >= Parent::m_tLen ) {
				return rRet;
			}
			
			rRet.ui32Start = _ui32Start;
			uint32_t ui32End = Parent::m_tLen - _sbString.Length();
			while ( rRet.ui32Start <= ui32End ) {
				if ( CStd::MemCmpF( &Parent::m_ptData[rRet.ui32Start], _sbString.CStr(), sizeof( _tDataType ) * _sbString.Length() ) ) {
					rRet.ui32End = rRet.ui32Start + _sbString.Length();
					return rRet;
				}
				++rRet.ui32Start;
			}
			rRet.ui32Start = Parent::m_tLen;
			return rRet;
		}

		/**
		 * Finds the first occurrence of the given input string from the given starting position.  If the given string is not found,
		 *	the ui32End member of the return is set to 0 and the ui32Start member is set to the length of the string.
		 * The search is case-insensitive.
		 *
		 * \param _ui32Start The location from where to search, inclusive.
		 * \param _sbString The string to locate within this string object.
		 * \return Returns the range of the located string.
		 */
		LSSTD_RANGE LSE_CALL												FindString( uint32_t _ui32Start,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			LSSTD_RANGE rRet;
			rRet.ui32Start = Parent::m_tLen;
			rRet.ui32End = 0;

			// Sanity check.
			if ( _sbString.Length() + _ui32Start > Parent::m_tLen || _ui32Start >= Parent::m_tLen ) {
				return rRet;
			}
			
			rRet.ui32Start = _ui32Start;
			uint32_t ui32End = Parent::m_tLen - _sbString.Length();
			while ( rRet.ui32Start <= ui32End ) {
				if ( ICmpSubString( rRet.ui32Start, _sbString ) == 0 ) {
					rRet.ui32End = rRet.ui32Start + _sbString.Length();
					return rRet;
				}
				++rRet.ui32Start;
			}
			rRet.ui32Start = Parent::m_tLen;
			return rRet;
		}

		/**
		 * Finds the first occurrence of the given input wildcard string from the given starting position.  If the given wildcard string
		 *	is not found, the ui32End member of the return is set to 0 and the ui32Start member is set to the length of the string.
		 *
		 * \param _ui32Start The location from where to search, inclusive.
		 * \param _sbString The string to locate within this string object.
		 * \return Returns the range of the located string.
		 */
		LSSTD_RANGE LSE_CALL												FindWildcardStringMatchCase( uint32_t _ui32Start,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			LSSTD_RANGE rRet;
			rRet.ui32Start = Parent::m_tLen;
			rRet.ui32End = 0;

			// Sanity check.
			if ( _sbString.Length() + _ui32Start > Parent::m_tLen || _ui32Start >= Parent::m_tLen ) {
				return rRet;
			}
			
			rRet.ui32Start = _ui32Start;
			uint32_t ui32End = Parent::m_tLen - _sbString.Length();
			while ( rRet.ui32Start <= ui32End ) {
				uint32_t ui32Len = CmpWildcard( rRet.ui32Start, _sbString, 2048UL );
				if ( ui32Len ) {
					rRet.ui32End = rRet.ui32Start + ui32Len;
					return rRet;
				}
				++rRet.ui32Start;
			}
			rRet.ui32Start = Parent::m_tLen;
			return rRet;
		}

		/**
		 * Finds and replaces all occurrences of the given wildcard string.  Comparisons are case-sensitive.
		 *
		 * \param _sbReplaceMe The string to replace within this string.  String is in wildcard format.
		 * \param _sbWithMe The string with which to replace matches of _sbReplaceMe within this string.
		 * \param _bWholeWords If true, only whole words are replaced.
		 * \return Returns the number of replacements made.
		 */
		uint32_t LSE_CALL													FindAndReplaceWildcardStringMatchCase( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbReplaceMe,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbWithMe,
			LSBOOL _bWholeWords = false ) {

			LSSTD_RANGE rRange = {
				0,
				0
			};
			uint32_t ui32Count = 0;
			while ( rRange.ui32Start < Parent::m_tLen ) {
				rRange = FindWildcardStringMatchCase( rRange.ui32Start, _sbReplaceMe );
				if ( rRange.ui32End ) {
					if ( !_bWholeWords || IsWholeWord( rRange ) ) {
						if ( Replace( rRange, _sbWithMe.CStr(), _sbWithMe.Length() ) ) {
							++ui32Count;
						}
					}
					rRange.ui32Start += _sbWithMe.Length();
				}
			}
			return ui32Count;
		}

		/**
		 * Finds and replaces all occurrences of the given character.
		 *
		 * \param _tReplaceMe The character to find.
		 * \param _tWithMe The character to which to replace the found character.
		 * \return Returns the number of characters replaced.
		 */
		uint32_t LSE_CALL													FindAndReplaceChar( _tDataType _tReplaceMe, _tDataType _tWithMe ) {
			uint32_t ui32Count = 0;
			for ( uint32_t I = Length(); I--; ) {
				if ( Parent::m_ptData[I] == _tReplaceMe ) {
					Parent::m_ptData[I] = _tWithMe;
					++ui32Count;
				}
			}
			return ui32Count;
		}

		/**
		 * Checks if the given range surrounds a whole word or not.
		 *
		 * \param _rRange The range to check.
		 * \return Returns true if the given range specifies a whole word or false otherwise.
		 */
		LSBOOL LSE_CALL														IsWholeWord( LSSTD_RANGE _rRange ) const {
			// Firstly, is the range valid?
			if ( _rRange.ui32Start >= Parent::m_tLen ) { return true; }
			if ( _rRange.ui32End >= Parent::m_tLen ) { return true; }
			if ( _rRange.ui32Start >= _rRange.ui32End ) { return false; }

			// Try to eliminate by the data to the left of the start.
			_tDataType dtValue = Parent::m_ptData[_rRange.ui32Start];
			// If either the left- or the right- most characters are not word characters, return true immediately.
			if ( !((dtValue >= static_cast<_tDataType>('a') && dtValue <= static_cast<_tDataType>('z')) ||
				(dtValue >= static_cast<_tDataType>('A') && dtValue <= static_cast<_tDataType>('Z')) ||
				(dtValue >= static_cast<_tDataType>('0') && dtValue <= static_cast<_tDataType>('9')) ||			
				dtValue == static_cast<_tDataType>('_')) ) {
				return true;
			}
			dtValue = Parent::m_ptData[_rRange.ui32End-1];
			if ( !((dtValue >= static_cast<_tDataType>('a') && dtValue <= static_cast<_tDataType>('z')) ||
				(dtValue >= static_cast<_tDataType>('A') && dtValue <= static_cast<_tDataType>('Z')) ||
				(dtValue >= static_cast<_tDataType>('0') && dtValue <= static_cast<_tDataType>('9')) ||			
				dtValue == static_cast<_tDataType>('_')) ) {
				return true;
			}

			// Both the left and right border characters are word characters, so check the next character to the left and right.
			if ( _rRange.ui32Start ) {
				dtValue = Parent::m_ptData[_rRange.ui32Start-1];
				if ( ((dtValue >= static_cast<_tDataType>('a') && dtValue <= static_cast<_tDataType>('z')) ||
					(dtValue >= static_cast<_tDataType>('A') && dtValue <= static_cast<_tDataType>('Z')) ||
					(dtValue >= static_cast<_tDataType>('0') && dtValue <= static_cast<_tDataType>('9')) ||			
					dtValue == static_cast<_tDataType>('_')) ) {
					return false;
				}
			}
			if ( _rRange.ui32End < Parent::m_tLen ) {
				dtValue = Parent::m_ptData[_rRange.ui32End];
				if ( ((dtValue >= static_cast<_tDataType>('a') && dtValue <= static_cast<_tDataType>('z')) ||
					(dtValue >= static_cast<_tDataType>('A') && dtValue <= static_cast<_tDataType>('Z')) ||
					(dtValue >= static_cast<_tDataType>('0') && dtValue <= static_cast<_tDataType>('9')) ||			
					dtValue == static_cast<_tDataType>('_')) ) {
					return false;
				}
			}

			// 
			return true;
		}

		/**
		 * Removes whitespace characters from the front and read of the string.  Whitespace characters are characters returned by CStd::IsWhitespace().
		 *
		 * \return Returns this string object.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> & LSE_CALL			RemoveWhitespace() {
			if ( !Parent::m_ptData ) { return (*this); }

			// Start at the end of the string since removing characters from there is faster.
			while ( Parent::m_tLen && CStd::IsWhitespace( Parent::m_ptData[Parent::m_tLen-1] ) ) {
				--Parent::m_tLen;
			}
			Parent::m_ptData[Parent::m_tLen] = _tDataType( 0 );


			// Count the number of characters from the front that need to be removed.
			uint32_t ui32Total = 0;
			for ( ; ui32Total < Parent::m_tLen; ++ui32Total ) {
				if ( !CStd::IsWhitespace( Parent::m_ptData[ui32Total] ) ) { break; }
			}
			if ( ui32Total ) {
				RemChars( 0, ui32Total );
			}

			return (*this);
		}

		/**
		 * Creates a string that represents the character-converted version of this string.  Character conversions
		 *	can be made by using the \xXX where XX are 2 hexadecimal digits that represent the actual binary character
		 *	to place at that position in the string.  This string is the input, and should be in the form containing
		 *	the \xXX characters.  The output is the binary form of this string after the \XX characters have been
		 *	converted.
		 * For example, if this string is:
		 *	The\x0D\x0AApple
		 * then the output will be:
		 *	The
		 *	Apple
		 * Characters \x0D and \x0A expanded into a new-line pair of characters (0x0D and 0x0A).
		 *	The allocator used in the new string is the same as the one used in this string.
		 * Additional characters are:
		 *	\a
		 *	\b
		 *	\f
		 *	\r (0x0D)
		 *	\n (0x0A)
		 *	\t (0x09)
		 *	\v
		 *	\'
		 *	\"
		 *	\?
		 *	\\ (0x5C)
		 *
		 * \return Returns the converted string.
		 */
		CStringBase<_tDerived, _tDataType, _uAllocSize> LSE_CALL			DecodeEscapeSequences() const {
			CStringBase<_tDerived, _tDataType, _uAllocSize> sbRet( Parent::m_paOurAllocator );
			
			uint32_t ui32Size;
			for ( uint32_t I = 0; I < Parent::m_tLen; I += ui32Size ) {
				sbRet.Append( DecodeEscape( &Parent::m_ptData[I], Parent::m_tLen - I, ui32Size ) );
			}
			return sbRet;
		}

		/**
		 * Decode a single escape sequence.
		 *
		 * \param _pdtValue Pointer to the escape sequence to decode.
		 * \param _ui32Len Number of characters in the sequence to which _pdtValue points.
		 * \param _ui32Size Upon return this holds the number of characters consumed by the escape sequence.
		 * \return Returns the value of the escape sequence.
		 */
		static _tDataType LSE_CALL											DecodeEscape( const _tDataType * _pdtValue, uint32_t _ui32Len, uint32_t &_ui32Size ) {
			struct {
				/**
				 * The actual character to find.
				 */
				_tDataType				dtEscape;

				/**
				 * The character to write to the string.
				 */
				_tDataType				dtValue;
			} sEscapes[] = {
				{ 'a', '\a' },
				{ 'b', '\b' },
				{ 'f', '\f' },
				{ 'n', '\n' },
				{ 'r', '\r' },
				{ 't', '\t' },
				{ 'v', '\v' },
				{ '\\', '\\' },
				{ '\'', '\'' },
				{ '"', '\"' },
				{ '?', '\?' },
				{ ' ', ' ' },
			};

			if ( _ui32Len == 0 ) {
				_ui32Size = 0;
				return static_cast<_tDataType>(0);
			}
			// If the length is 1 or the next character is not a back slash, the next character is the return.
			_ui32Size = 1;
			if ( _ui32Len == 1 || _pdtValue[0] != static_cast<_tDataType>('\\') ) {
				return _pdtValue[0];
			}
			// Possible sequence here.
			switch ( _pdtValue[1] ) {
				case 'x' : {
					// Hex sequence.  Decode.
					if ( _ui32Len == 2 ) {
						return _pdtValue[0];
					}
					_tDataType dtReturn = 0;
					_tDataType dtThis;
					// Decode the hex characters (maximum of 2).
					for ( uint32_t I = 2; I < _ui32Len && I < 4; I++ ) {
						if ( _pdtValue[I] >= static_cast<_tDataType>('0') && _pdtValue[I] <= static_cast<_tDataType>('9') ) {
							dtThis = _pdtValue[I] - static_cast<_tDataType>('0');
						}
						else if ( _pdtValue[I] >= static_cast<_tDataType>('A') && _pdtValue[I] <= static_cast<_tDataType>('F') ) {
							dtThis = _pdtValue[I] - static_cast<_tDataType>('A') + 0xA;
						}
						else if ( _pdtValue[I] >= static_cast<_tDataType>('a') && _pdtValue[I] <= static_cast<_tDataType>('f') ) {
							dtThis = _pdtValue[I] - static_cast<_tDataType>('a') + 0xA;
						}
						else {
							// No characters were eaten?
							if ( I == 2 ) { return _pdtValue[0]; }

							// Eat the x.
							++_ui32Size;
							return dtReturn;
						}
						dtReturn <<= 4;
						dtReturn |= dtThis;
						++_ui32Size;
					}
					// Eat the x.
					++_ui32Size;
					return dtReturn;
				}
				default : {
					if ( _pdtValue[1] >= '0' && _pdtValue[1] <= '7' ) {
						// Decode octal sequences.
						_tDataType dtReturn = 0;
						_tDataType dtThis;
						for ( uint32_t I = 1; I < _ui32Len && I < 4; I++ ) {
							if ( _pdtValue[I] >= '0' && _pdtValue[I] <= '7' ) {
								dtThis = _pdtValue[I] - '0';
							}
							else {
								// No characters were eaten?
								if ( I == 1 ) { return _pdtValue[0]; }
								return dtReturn;
							}
							dtReturn <<= 3;
							dtReturn |= dtThis;
							++_ui32Size;
						}
						return dtReturn;
					}
					else {
						for ( uint32_t J = 0; sEscapes[J].dtEscape != ' '; J++ ) {
							if ( _pdtValue[1] == sEscapes[J].dtEscape ) {
								++_ui32Size;
								return sEscapes[J].dtValue;
							}
						}
					}
				}
			}
			return _pdtValue[0];
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// COMPARISONS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Lexicographically compare this string with another.  Both strings are compared to their
		 *	fullest length until a mismatched character is found.  This means that NULL characters inside
		 *	the string do not stop the comparison (unless, of course, the other string does not have a NULL
		 *	at the same position).  If one string is shorter than the other, but the other string contains
		 *	only NULL characters beyond the length of the shorter string, both strings are considered equal.
		 *	For this reason, Cmp() can only be used to determine equality when standard-style strings are
		 *	being used.  When neither string has a NULL character aside from the terminating NULL, this
		 *	function behaves exactly like the standard C strcmp().
		 *
		 * \param _sbString The string against which to compare this string.
		 * \return Returns the lexicographic relationship between the strings.
		 */
		int32_t LSE_CALL													Cmp( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			uint32_t ui32Max = CStd::Max( _sbString.Length(), Length() );
			for ( uint32_t I = 0; I < ui32Max; ++I ) {
				_tDataType dtLeft = I < Length() ? Parent::m_ptData[I] : _tDataType( 0 );
				_tDataType dtRight = I < _sbString.Length() ? _sbString.Parent::m_ptData[I] : _tDataType( 0 );
				int32_t i32Dif = dtLeft - dtRight;
				if ( i32Dif != 0 ) {
					// Something somewhere needs a change.  They'll need a crane!
					return i32Dif;
				}
			}
			if ( _sbString.Length() == Length() ) { return 0; }
			return _sbString.Length() > Length() ? -_sbString[Length()] : Parent::m_ptData[_sbString.Length()];
		}

		/**
		 * Same as Cmp() but uses case-insensitivity to perform the comparison.
		 *
		 * \param _sbString The string against which to compare this string.
		 * \return Returns the lexicographic relationship between the strings.
		 */
		int32_t LSE_CALL													ICmp( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			uint32_t ui32Max = CStd::Max( _sbString.Length(), Length() );
			for ( uint32_t I = 0; I < ui32Max; ++I ) {
				_tDataType dtLeft = I < Length() ? Parent::m_ptData[I] : _tDataType( 0 );
				_tDataType dtRight = I < _sbString.Length() ? _sbString.Parent::m_ptData[I] : _tDataType( 0 );
				int32_t i32Dif = CStd::ToLower( dtLeft ) - CStd::ToLower( dtRight );
				if ( i32Dif != 0 ) {
					// Somewhere something needs a change.  They'll need a crane!
					return i32Dif;
				}
			}
			if ( _sbString.Length() == Length() ) { return 0; }
			return _sbString.Length() > Length() ? -_sbString[Length()] : Parent::m_ptData[_sbString.Length()];
		}

		/**
		 * Same as Cmp() but uses case-insensitivity to perform the comparison.
		 *
		 * \param _ui32Loc Index from which to begin testing characters within this string.
		 * \param _sbString The string against which to compare this string.
		 * \return Returns the lexicographic relationship between the strings.
		 */
		int32_t LSE_CALL													ICmpSubString( uint32_t _ui32Loc,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			if ( _ui32Loc >= Length() ) {
				return -1;
			}
			uint32_t ui32Max = CStd::Min( _sbString.Length(), Length() - _ui32Loc );
			for ( uint32_t I = 0; I < ui32Max; ++I ) {
				_tDataType dtLeft = I < Length() ? Parent::m_ptData[_ui32Loc+I] : _tDataType( 0 );
				_tDataType dtRight = I < _sbString.Length() ? _sbString.Parent::m_ptData[I] : _tDataType( 0 );
				int32_t i32Dif = CStd::ToLower( dtLeft ) - CStd::ToLower( dtRight );
				if ( i32Dif != 0 ) {
					// Somewhere something needs a change.  They'll need a crane!
					return i32Dif;
				}
			}
			return -!(_sbString.Length() == Length() - _ui32Loc);
		}

		/**
		 * Perform a wildcard comparison at the given position within this string.
		 *
		 * \param _ui32Start The location from where to search, inclusive.
		 * \param _sbString The wildcard string to locate within this string object.
		 * \param _ui32MaxWildcardLen The wildcard string to locate within this string object.
		 * \return Returns the length of the match if there is one or 0.
		 */
		uint32_t LSE_CALL													CmpWildcard( uint32_t _ui32Start,
			const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString, uint32_t _ui32MaxWildcardLen ) const {
			if ( _ui32Start >= Length() ) {
				return 0;
			}
			uint32_t ui32BufPos = 0;
			uint32_t I = 0;
			uint32_t ui32Len = Length() - _ui32Start;
			_tDataType * pdtBuffer = &Parent::m_ptData[_ui32Start];
			for ( ; I < _sbString.Length(); I++, ui32BufPos++ ) {
				if ( ui32BufPos == ui32Len ) { goto End; }
				switch ( _sbString.CStr()[I] ) {
					case '?' : { continue; }
					case '*' : {
						// Skip all the * and ? characters.
						while ( I < _sbString.Length() && (_sbString.CStr()[I] == static_cast<_tDataType>('*') || _sbString.CStr()[I] == static_cast<_tDataType>('?')) ) { I++; }
						if ( I == _sbString.Length() ) { goto End; }

						// Within the wildcard match length, find the next character.
						uint8_t ui8This = _sbString.CStr()[I];

						for ( uint32_t J = 0; J < _ui32MaxWildcardLen && ui32BufPos < ui32Len && pdtBuffer[ui32BufPos] != ui8This; ui32BufPos++ ) {}

						// If this caused us to reach the end of the buffer, fail.
						if ( ui32BufPos == ui32Len ) { return 0; }

						// Everything is good.
						break;
					}
					default : {
						if ( _sbString.CStr()[I] != pdtBuffer[ui32BufPos] ) { return 0; }
					}
				}
			}
End :
			// Skip all the * characters.
			while ( I < _sbString.Length() && _sbString.CStr()[I] == static_cast<_tDataType>('*') ) { I++; }

			return I == _sbString.Length() ? ui32BufPos : 0;
		}

		/**
		 * Cmp() is not the best (or fastest) way to determine equality of strings strictly.  Use Equals()
		 *	for this, which simply returns true or false.
		 *
		 * \param _sbString The string against which to compare this string.
		 * \return Returns true if the strings are an exact match, false otherwise.
		 */
		LSBOOL LSE_CALL														Equals( const CStringBase<_tDerived, _tDataType, _uAllocSize> &_sbString ) const {
			if ( Length() != _sbString.Length() ) { return false; }
			return CStd::MemCmpF( Parent::m_ptData, _sbString.Parent::m_ptData, sizeof( _tDataType ) * Length() );
		}


	protected :
		// == Members.


		// == Functions.
		/**
		 * Override this for improved performance.  char arrays can use a much faster algorithm
		 *	for determining string length.
		 */
		uint32_t LSE_CALL													StrLen( const _tDataType * _pdtValue ) const {
			const _tDataType * pdtCopy = _pdtValue;
			while ( (*pdtCopy++) ) {}
			return static_cast<uint32_t>((pdtCopy - _pdtValue) - 1);
		}

	private :
		typedef CVectorPoD<_tDataType, uint32_t, _uAllocSize>				Parent;
	};

}	// namespace lstl

#endif	// __LSTL_STRINGBASE_H__
