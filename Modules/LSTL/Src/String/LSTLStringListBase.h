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
 * Description: An array of strings with some useful additions.
 */


#ifndef __LSTL_STRINGLISTBASE_H__
#define __LSTL_STRINGLISTBASE_H__

#include "../LSTLib.h"
#include "../Vector/LSTLVector.h"

namespace lstl {

	/**
	 * Class CStringListBase
	 * \brief An array of strings with some useful additions..
	 *
	 * Description: An array of strings with some useful additions..
	 */
	template <typename _tStringType>
	class CStringListBase : public CVector<_tStringType, uint32_t, 32> {
	public :
		// == Various constructors.
		LSE_CALLCTOR														CStringListBase() {
		}


		// == Functions.
		/**
		 * Creates a string list in which each line of the given text is a new string in the
		 *	array.  New-line characters may optionally be discarded.  \n is considered the
		 *	only new-line character, however \r is stripped as well if stripping is requested.
		 *
		 * \param _pcText The text to be converted to a multi-string list.
		 * \param _ui32Len Length of the input string to which _pcText points.
		 * \param _bKeepNewLines If true, new-line characters \r and \n are kept.
		 * \return Returns true if there was enough memory to perform the full conversion.
		 */
		LSBOOL LSE_CALL														MakeFromUtf8( const char * _pcText, uint32_t _ui32Len,
			LSBOOL _bKeepNewLines = false ) {
			if ( !this->Push( _tStringType() ) ) { return false; }

			uint32_t ui32Size = 0;
			uint32_t ui32Char = CStd::NextUtf8Char( reinterpret_cast<const LSUTF8 *>(_pcText), _ui32Len, &ui32Size );
			while ( _ui32Len ) {
				if ( _bKeepNewLines || (ui32Char != '\r' && ui32Char != '\n') ) {
					// Either we are keeping new lines or this is not a new-line character.
					// Add the characters that make up this UTF-8 code.
					for ( uint32_t I = 0; I < ui32Size; ++I ) {
						if ( !(*this)[Parent::Length()-1].Append( (*_pcText++) ) ) {
							return false;
						}
					}
				}
				else {
					// Discarding new-line characters.
					_pcText += ui32Size;
				}
				if ( ui32Char == '\n' ) {
					if ( !this->Push( _tStringType() ) ) { return false; }
				}

				_ui32Len -= ui32Size;
				ui32Char = CStd::NextUtf8Char( reinterpret_cast<const LSUTF8 *>(_pcText), _ui32Len, &ui32Size );
			}

			return true;
		}

		/**
		 * Creates a single string from this array of strings.  The final string is the result of appending
		 *	all of the strings in this list to each other.  New-line characters may optionally be added
		 *	between strings.  New-line characters consist of \r\n here.
		 *
		 * \param _bAddNewLines If true, the new-line \r\n combo is inserted between strings when creating the final
		 *	string.
		 * \return Returns the new string, which represents the result of appending each string in this
		 *	list one after another.
		 */
		_tStringType LSE_CALL												ToString( LSBOOL _bAddNewLines ) {
			_tStringType tRet;

			for ( uint32_t I = 0; I < Parent::Length(); ++I ) {
				if ( !tRet.Append( (*this)[I].CStr(), (*this)[I].Length() ) ) {
					tRet.Reset();
					return tRet;
				}
				if ( _bAddNewLines ) {
					if ( !tRet.Append( '\r' ) ) {
						tRet.Reset();
						return tRet;
					}
					if ( !tRet.Append( '\n' ) ) {
						tRet.Reset();
						return tRet;
					}
				}
			}
			return tRet;
		}
		
		/**
		 * Creates a string array by tokenizing the given character array.  If the tokenizer appears twice in
		 *	a row within the input string, an empty string entry is added to the string array.
		 *
		 * \param _ptString Pointer to an array of characters to be tokenized.  The array is not considered
		 *	to be in any specific UTF format, and the string is tokenized on a per-character basis.
		 * \param _ui32Length Length, in characters, of the array to which _ptString points.
		 * \param _tTokenizer Character by which to tokenize the string.
		 * \return Returns true if the string was successfully tokenized.
		 */
		 template <typename _tCharType>
		 LSBOOL LSE_CALL													MakeByTokenizing( const _tCharType * _ptString,
			uint32_t _ui32Length, _tCharType _tTokenizer ) {
			if ( !_ptString ) { return false; }
			
			// The string holding the temporary result.
			_tStringType tTempString;
			for ( uint32_t I = 0; I < _ui32Length; ++I ) {
				if ( _ptString[I] == _tTokenizer ) {	// It is the tokenizer.
					if ( !Parent::Push( tTempString ) ) { return false; }
					tTempString.ResetNoDealloc();
				}
				else {									// Any other character.
					if ( !tTempString.Append( _ptString[I] ) ) { return false; }
				}
			}
			if ( tTempString.Length() ) {
				if ( !Parent::Push( tTempString ) ) { return false; }
			}
			return true;
		}


	protected :
		// == Members.
		
	private :
		typedef CVector<_tStringType, uint32_t, 32>						Parent;
	};

}	// namespace lstl

#endif	// __LSTL_STRINGLISTBASE_H__
