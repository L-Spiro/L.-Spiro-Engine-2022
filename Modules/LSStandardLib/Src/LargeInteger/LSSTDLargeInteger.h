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
 * Description: A large-integer template class.  Represents a number containing at most _uMaxDigits digits in
 *	radix 10.  Provides standard operators synthesized in software mode.  This class is not intended for use
 *	in any high-performance situation.  Use this class when precision is more important than speed.
 *
 * Multiple instances of this class can cause a heavy hit to the stack unless allocated to the heap.
 */


#ifndef __LSSTD_LARGEINTEGER_H__
#define __LSSTD_LARGEINTEGER_H__

#include "../LSSTDStandardLib.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lsstd {

	/**
	 * Exceptions that can be thrown by this class.
	 */
	enum LSSTD_LARGE_INTEGER_EXCEPTIONS {
		LSSTD_LIE_OVERFLOW,
		LSSTD_LIE_DIVISIONBYZERO,
		LSSTD_LIE_INVALIDCONVERSION,
	};

	/**
	 * Class CLargeInteger
	 * \brief A large-integer template class.
	 *
	 * Description: A large-integer template class.  Represents a number containing at most _uMaxDigits digits in
	 *	radix 10.  Provides standard operators synthesized in software mode.  This class is not intended for use
	 *	in any high-performance situation.  Use this class when precision is more important than speed.
	 *
	 * Multiple instances of this class can cause a heavy hit to the stack unless allocated to the heap.
	 */
	template <unsigned _uMaxDigits = 1024>
	class CLargeInteger {
	public :
		// == Various constructors.
		/**
		 * Creates a large integer.  The integer is initialized to 0.
		 */
		LSE_CALLCTOR								CLargeInteger() :
			m_ui32UsedDigits( 1 ) {
			CStd::MemSet( m_ui32Digits, 0, sizeof( m_ui32Digits ) );
		}

		/**
		 * Creates a large integer as a copy of another.
		 *
		 * \param _liOther The source large integer.
		 */
		LSE_CALLCTOR								CLargeInteger( const CLargeInteger<_uMaxDigits> &_liOther ) :
			m_ui32UsedDigits( _liOther.m_ui32UsedDigits ) {
			CStd::MemCpy( m_ui32Digits, _liOther.m_ui32Digits, sizeof( m_ui32Digits ) );
		}

		/**
		 * Creates a large integer from a signed 64-bit value.
		 *
		 * \param _i64Value The value to be converted to a large integer.
		 */
		LSE_CALLCTOR								CLargeInteger( int64_t _i64Value ) {
			uint32_t ui32Pos = 0;
			while ( _i64Value && ui32Pos < _uMaxDigits ) {
				m_ui32Digits[ui32Pos++] = static_cast<uint32_t>(_i64Value & 0xFFFFFFFFULL);
				_i64Value >>= 32;
			}
			m_ui32UsedDigits = ui32Pos;
			// Fill the rest of our buffer with 0's.
			for ( ; ui32Pos < _uMaxDigits; ++ui32Pos ) {
				m_ui32Digits[ui32Pos] = 0;
			}

			if ( m_ui32UsedDigits == 0 ) {
				m_ui32UsedDigits = 1;
			}
		}

		/**
		 * Creates a large integer from an unsigned 64-bit value.
		 *
		 * \param _ui64Value The value to be converted to a large integer.
		 */
		LSE_CALLCTOR								CLargeInteger( uint64_t _ui64Value ) {
			uint32_t ui32Pos = 0;
			while ( _ui64Value ) {
				m_ui32Digits[ui32Pos++] = static_cast<uint32_t>(_ui64Value & 0xFFFFFFFFULL);
				_ui64Value >>= 32;
			}
			m_ui32UsedDigits = ui32Pos;
			// Fill the rest of our buffer with 0's.
			for ( ; ui32Pos < _uMaxDigits; ++ui32Pos ) {
				m_ui32Digits[ui32Pos] = 0;
			}

			if ( m_ui32UsedDigits == 0 ) {
				m_ui32UsedDigits = 1;
			}
		}

		/**
		 * Creates a large integer from a signed 32-bit value.
		 *
		 * \param _i32Value The value to be converted to a large integer.
		 */
		LSE_CALLCTOR								CLargeInteger( int32_t _i32Value ) {
			uint32_t ui32Pos = 0;
			int64_t i64Val = _i32Value;
			while ( i64Val && ui32Pos < _uMaxDigits ) {
				m_ui32Digits[ui32Pos++] = static_cast<uint32_t>(i64Val & 0xFFFFFFFFULL);
				i64Val >>= 32LL;
			}
			m_ui32UsedDigits = ui32Pos;
			// Fill the rest of our buffer with 0's.
			for ( ; ui32Pos < _uMaxDigits; ++ui32Pos ) {
				m_ui32Digits[ui32Pos] = 0;
			}

			if ( m_ui32UsedDigits == 0 ) {
				m_ui32UsedDigits = 1;
			}
		}

		/**
		 * Creates a large integer from an unsigned 32-bit value.
		 *
		 * \param _ui32Value The value to be converted to a large integer.
		 */
		LSE_CALLCTOR								CLargeInteger( uint32_t _ui32Value ) {
			uint32_t ui32Pos = 0;
			
			m_ui32Digits[ui32Pos++] = static_cast<uint32_t>(_ui32Value & 0xFFFFFFFFULL);
			m_ui32UsedDigits = ui32Pos;
			// Fill the rest of our buffer with 0's.
			for ( ; ui32Pos < _uMaxDigits; ++ui32Pos ) {
				m_ui32Digits[ui32Pos] = 0;
			}
		}

		/**
		 * Creates a large integer from an array of bytes.
		 *
		 * \param _pui8Array The array of bytes.
		 * \param _ui32Length The number of bytes in the array.
		 */
		LSE_CALLCTOR								CLargeInteger( const uint32_t * _pui8Array, uint32_t _ui32Length ) :
			m_ui32UsedDigits( _ui32Length ) {
			if ( m_ui32UsedDigits > _uMaxDigits ) {
				throw LSSTD_LIE_OVERFLOW;
			}

			for ( int32_t I = static_cast<int32_t>(m_ui32UsedDigits) - 1, J = 0; I >= 0; --I, ++J ) {
				m_ui32Digits[J] = _pui8Array[I];
			}
			while ( m_ui32UsedDigits > 1 && m_ui32Digits[m_ui32UsedDigits-1] == 0 ) {
				--m_ui32UsedDigits;
			}
			for ( uint32_t I = m_ui32UsedDigits; I < _uMaxDigits; ++I ) {
				m_ui32Digits[I] = 0;
			}
		}


		// == Operators.
		/**
		 * Adds two large integers to create a third large integer holding the result.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator + ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liReturn;
			Add( (*this), _liOp, liReturn );

			return liReturn;
		}

		/**
		 * Subtract two large integers to create a third large integer holding the result.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator - ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liReturn;
			Subtract( (*this), _liOp, liReturn );

			return liReturn;
		}

		/**
		 * Multiply two large integers to create a third large integer holding the result.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator * ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liReturn;
			Multiply( (*this), _liOp, liReturn );

			return liReturn;
		}

		/**
		 * Multiply this large integer by other, assiging the result to this large integer.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liOp The right operand.
		 * \return Returns this large integer, which will be filled with the result.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator *= ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			CLargeInteger<_uMaxDigits> liReturn;
			Multiply( (*this), _liOp, liReturn );
			(*this) = liReturn;

			return (*this);
		}

		/**
		 * Divide two large integers to create a third large integer holding the result.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator / ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liReturn;
			Divide( (*this), _liOp, liReturn );

			return liReturn;
		}

		/**
		 * Divide this large integer by other, assiging the result to this large integer.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _liOp The right operand.
		 * \return Returns this large integer, which will be filled with the result.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator /= ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			CLargeInteger<_uMaxDigits> liReturn;
			Divide( (*this), _liOp, liReturn );
			(*this) = liReturn;

			return (*this);
		}

		/**
		 * Modulus two large integers to create a third large integer holding the result.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator % ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liReturn;
			Modulus( (*this), _liOp, liReturn );

			return liReturn;
		}

		/**
		 * Perform unary negation on this number.  The negated value is returned.
		 *
		 * \return Returns the 2's compliment of this number.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator - () const {
			CLargeInteger<_uMaxDigits> liReturn;
			Negate( (*this), liReturn );

			return liReturn;
		}

		/**
		 * Copy the value of another large integer into this one.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the copied number.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator = ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			m_ui32UsedDigits = _liOp.m_ui32UsedDigits;
			CStd::MemCpy( m_ui32Digits, _liOp.m_ui32Digits, sizeof( m_ui32Digits ) );

			return (*this);
		}

		/**
		 * Shift bits left by the given amount.  The shifted value is returned.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator << ( uint32_t _ui32Shift ) const {
			CLargeInteger<_uMaxDigits> liReturn( (*this) );
			liReturn.m_ui32UsedDigits = ShiftLeft( liReturn.GetBytes(), _uMaxDigits, _ui32Shift );

			return liReturn;
		}

		/**
		 * Shift bits left by the given amount, storing the result in this instance.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the this instance, which holds the result.
		 */
		CLargeInteger<_uMaxDigits> & LSE_CALL		operator <<= ( uint32_t _ui32Shift ) {
			m_ui32UsedDigits = ShiftLeft( GetBytes(), _uMaxDigits, _ui32Shift );

			return (*this);
		}

		/**
		 * Shift bits right by the given amount.  The shifted value is returned.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator >> ( uint32_t _ui32Shift ) const {
			CLargeInteger<_uMaxDigits> liReturn( (*this) );
			liReturn.m_ui32UsedDigits = ShiftRight( liReturn.GetBytes(), liReturn.m_ui32UsedDigits, _ui32Shift );
			liReturn.m_ui32UsedDigits = CStd::Max<uint32_t>( liReturn.m_ui32UsedDigits, 1 );

			// Fix sign bits on negative numbers.
			if ( m_ui32Digits[_uMaxDigits-1] & 0x80000000UL ) {
				for ( uint32_t I = _uMaxDigits - 1; I >= liReturn.m_ui32UsedDigits; --I ) {
					liReturn.m_ui32Digits[I] = 0xFFFFFFFF;
				}

				uint32_t ui32Mask = 0x80000000UL;
				for ( uint32_t I = 0; I < 32; ++I ) {
					if ( liReturn.m_ui32Digits[liReturn.m_ui32UsedDigits-1] & ui32Mask ) {
						break;
					}
					liReturn.m_ui32Digits[liReturn.m_ui32UsedDigits-1] |= ui32Mask;
					ui32Mask >>= 1;
				}
				liReturn.m_ui32UsedDigits = _uMaxDigits;
			}

			return liReturn;
		}

		/**
		 * Shift bits right by the given amount, storing the result in this instance.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the this instance, which holds the result.
		 */
		CLargeInteger<_uMaxDigits> & LSE_CALL		operator >>= ( uint32_t _ui32Shift ) {
			m_ui32UsedDigits = ShiftRight( GetBytes(), m_ui32UsedDigits, _ui32Shift );
			m_ui32UsedDigits = CStd::Max<uint32_t>( m_ui32UsedDigits, 1 );

			// Fix sign bits on negative numbers.
			if ( m_ui32Digits[_uMaxDigits-1] & 0x80000000UL ) {
				for ( uint32_t I = _uMaxDigits - 1; I >= m_ui32UsedDigits; --I ) {
					m_ui32Digits[I] = 0xFFFFFFFF;
				}

				uint32_t ui32Mask = 0x80000000UL;
				for ( uint32_t I = 0; I < 32; ++I ) {
					if ( m_ui32Digits[m_ui32UsedDigits-1] & ui32Mask ) {
						break;
					}
					m_ui32Digits[m_ui32UsedDigits-1] |= ui32Mask;
					ui32Mask >>= 1;
				}
				m_ui32UsedDigits = _uMaxDigits;
			}

			return (*this);
		}

		/**
		 * Bitwise AND operator.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator & ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liResult;
			liResult.m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = liResult.m_ui32UsedDigits; I--; ) {
				liResult.m_ui32Digits[I] = m_ui32Digits[I] & _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( liResult.m_ui32UsedDigits > 1 && liResult.m_ui32Digits[liResult.m_ui32UsedDigits-1] == 0 ) {
				--liResult.m_ui32UsedDigits;
			}
			return liResult;
		}

		/**
		 * Bitwise AND operator with assignent.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the this instance, which holds the result.
		 */
		CLargeInteger<_uMaxDigits> & LSE_CALL		operator &= ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = m_ui32UsedDigits; I--; ) {
				m_ui32Digits[I] = m_ui32Digits[I] & _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( m_ui32UsedDigits > 1 && m_ui32Digits[m_ui32UsedDigits-1] == 0 ) {
				--m_ui32UsedDigits;
			}
			return (*this);
		}

		/**
		 * Bitwise OR operator.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator | ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liResult;
			liResult.m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = liResult.m_ui32UsedDigits; I--; ) {
				liResult.m_ui32Digits[I] = m_ui32Digits[I] | _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( liResult.m_ui32UsedDigits > 1 && liResult.m_ui32Digits[liResult.m_ui32UsedDigits-1] == 0 ) {
				--liResult.m_ui32UsedDigits;
			}
			return liResult;
		}

		/**
		 * Bitwise OR operator with assignent.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the this instance, which holds the result.
		 */
		CLargeInteger<_uMaxDigits> & LSE_CALL		operator |= ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = m_ui32UsedDigits; I--; ) {
				m_ui32Digits[I] = m_ui32Digits[I] | _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( m_ui32UsedDigits > 1 && m_ui32Digits[m_ui32UsedDigits-1] == 0 ) {
				--m_ui32UsedDigits;
			}
			return (*this);
		}

		/**
		 * Bitwise XOR operator.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		CLargeInteger<_uMaxDigits> LSE_CALL			operator ^ ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			CLargeInteger<_uMaxDigits> liResult;
			liResult.m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = liResult.m_ui32UsedDigits; I--; ) {
				liResult.m_ui32Digits[I] = m_ui32Digits[I] ^ _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( liResult.m_ui32UsedDigits > 1 && liResult.m_ui32Digits[liResult.m_ui32UsedDigits-1] == 0 ) {
				--liResult.m_ui32UsedDigits;
			}
			return liResult;
		}

		/**
		 * Bitwise XOR operator with assignent.  The result is returned.
		 *
		 * \param _liOp The right operand.
		 * \return Returns the this instance, which holds the result.
		 */
		CLargeInteger<_uMaxDigits> & LSE_CALL		operator ^= ( const CLargeInteger<_uMaxDigits> &_liOp ) {
			m_ui32UsedDigits = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			for ( uint32_t I = m_ui32UsedDigits; I--; ) {
				m_ui32Digits[I] = m_ui32Digits[I] ^ _liOp.m_ui32Digits[I];
			}

			// Remove opening 0's.
			while ( m_ui32UsedDigits > 1 && m_ui32Digits[m_ui32UsedDigits-1] == 0 ) {
				--m_ui32UsedDigits;
			}
			return (*this);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is greater than the given large integer.
		 */
		bool LSE_CALL								operator > ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			int32_t i32Pos = _uMaxDigits - 1;

			if ( (m_ui32Digits[i32Pos] & 0x80000000UL) && !(_liOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return false;
			}
			if ( !(m_ui32Digits[i32Pos] & 0x80000000UL) && (_liOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return true;
			}

			uint32_t ui32Len = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			// Skip past all digits that are the same.
			for ( i32Pos = static_cast<int32_t>(ui32Len) - 1; i32Pos >= 0 && m_ui32Digits[i32Pos] == _liOp.m_ui32Digits[i32Pos]; --i32Pos ) {}

			// If we found a different digit.
			if ( i32Pos >= 0 ) {
				return m_ui32Digits[i32Pos] > _liOp.m_ui32Digits[i32Pos];
			}

			// They are the same.
			return false;
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is less than the given large integer.
		 */
		bool LSE_CALL								operator < ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			int32_t i32Pos = _uMaxDigits - 1;

			if ( (m_ui32Digits[i32Pos] & 0x80000000UL) && !(_liOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return true;
			}
			if ( !(m_ui32Digits[i32Pos] & 0x80000000UL) && (_liOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return false;
			}

			uint32_t ui32Len = CStd::Max( m_ui32UsedDigits, _liOp.m_ui32UsedDigits );
			// Skip past all digits that are the same.
			for ( i32Pos = static_cast<int32_t>(ui32Len) - 1; i32Pos >= 0 && m_ui32Digits[i32Pos] == _liOp.m_ui32Digits[i32Pos]; --i32Pos ) {}

			// If we found a different digit.
			if ( i32Pos >= 0 ) {
				return m_ui32Digits[i32Pos] < _liOp.m_ui32Digits[i32Pos];
			}

			// They are the same.
			return false;
		}

		/**
		 * Equality comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is equal to the given large integer.
		 */
		bool LSE_CALL								operator == ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			if ( m_ui32UsedDigits != _liOp.m_ui32UsedDigits ) { return false; }
			return CStd::MemCmpF( m_ui32Digits, _liOp.m_ui32Digits, sizeof( m_ui32Digits[0] ) * m_ui32UsedDigits ) != 0;
		}

		/**
		 * Inequality comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is not equal to the given large integer.
		 */
		bool LSE_CALL								operator != ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			if ( m_ui32UsedDigits != _liOp.m_ui32UsedDigits ) { return true; }
			return !CStd::MemCmpF( m_ui32Digits, _liOp.m_ui32Digits, sizeof( m_ui32Digits[0] ) * m_ui32UsedDigits );
		}

		/**
		 * Greater-than-or-equal-to comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is greater than or equal to the given large integer.
		 */
		bool LSE_CALL								operator >= ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			return (*this) > _liOp || (*this) == _liOp;
		}

		/**
		 * Less-than-or-equal-to comparison.
		 *
		 * \param _liOp The right operand.
		 * \return Returns true if this large integer is less than or equal to the given large integer.
		 */
		bool LSE_CALL								operator <= ( const CLargeInteger<_uMaxDigits> &_liOp ) const {
			return (*this) < _liOp || (*this) == _liOp;
		}


		// == Functions.
		/**
		 * Gets the value of this large integer as an unsigned 64-bit integer.  Throws an exception if the
		 *	value of this integer is too large to be held by a 64-bit integer.
		 *
		 * \return Returns the value of this large integer as a 64-bit integer.
		 */
		uint64_t LSE_CALL							GetAs64bitUInt() const {
			if ( m_ui32UsedDigits > sizeof( uint64_t ) ) {
				throw LSSTD_LIE_INVALIDCONVERSION;
			}
			uint64_t ui64Ret = 0ULL;
			CStd::MemCpy( &ui64Ret, m_ui32Digits, CStd::Min( sizeof( ui64Ret ), m_ui32UsedDigits * sizeof( uint32_t ) ) );
			return ui64Ret;
		}

		/**
		 * Gets the value of this large integer as an unsigned 32-bit integer.  Throws an exception if the
		 *	value of this integer is too large to be held by a 32-bit integer.
		 *
		 * \return Returns the value of this large integer as a 32-bit integer.
		 */
		uint32_t LSE_CALL							GetAs32bitUInt() const {
			if ( m_ui32UsedDigits > sizeof( uint32_t ) ) {
				throw LSSTD_LIE_INVALIDCONVERSION;
			}
			uint32_t ui32Ret = 0;
			CStd::MemCpy( &ui32Ret, m_ui32Digits, CStd::Min<uint32_t>( sizeof( ui32Ret ), m_ui32UsedDigits * sizeof( uint32_t ) ) );
			return ui32Ret;
		}

		/**
		 * Gets our bytes.
		 *
		 * \return Returns the buffer used to store the bytes in this instance.  For read and write.
		 */
		uint32_t * LSE_CALL							GetBytes() {
			return m_ui32Digits;
		}

		/**
		 * Gets the length of our bytes.
		 *
		 * \return Returns the length of this digit.
		 */
		uint32_t LSE_CALL							GetLength() const {
			return m_ui32UsedDigits;
		}

		/**
		 * Sets the length.  Never use this.
		 *
		 * \param _ui32Length The new length of the large integer.
		 */
		void LSE_CALL								SetLength( uint32_t _ui32Length ) {
			m_ui32UsedDigits = _ui32Length;
		}

		/**
		 * Print this number to a string.  Returns the number of characters printed or the number
		 *	of characters needed to print the whole numbr if _ptString is NULL.
		 *
		 * \param _ptString The output string.
		 * \param _ui32MaxLen The maximum number of characters to print.  Ignored if _ptString is NULL.
		 * \param _ui32Radix The radix used for printing.
		 * \return Returns the number of characters printed (not including the NULL character) or
		 *	the number of characters needed to print the whole number (including the NULL character).
		 */
		template <typename _tType>
		uint32_t LSE_CALL							ToString( _tType * _ptString, uint32_t _ui32MaxLen, uint32_t _ui32Radix ) {
			if ( _ptString && !_ui32MaxLen ) {
				// There is a string but there is no room to put anything into it.
				return 0;
			}
			LSBOOL bNegative = (m_ui32Digits[_uMaxDigits-1] & 0x80000000UL) != 0;

			CLargeInteger<_uMaxDigits> liNewVal = bNegative ? -(*this) : (*this);

			if ( bNegative ) {
				if ( _ptString ) {
					// Sanity check was done above.
					(*_ptString++) = '-';
					--_ui32MaxLen;
				}
			}


			// Get the actual number of digits.  There is always at least one.
			uint32_t ui32Len = 0;
			CLargeInteger<_uMaxDigits> liNewValCopy = liNewVal;
			while ( true ) {
				liNewValCopy /= _ui32Radix;
				if ( liNewValCopy == 0ULL ) { break; }
				++ui32Len;
			}

			// Sanity check.
			if ( !_ptString ) {
				// We only need to return the length of the string needed, including the NULL terminator.
				return (ui32Len + 1) + static_cast<uint32_t>(bNegative) + 1;
			}

			// Copy the characters from the end to the front.
			liNewValCopy = liNewVal;
			uint32_t ui32ThisLen = ui32Len;
			if ( ui32ThisLen + 1 < _ui32MaxLen ) {
				_ptString[ui32ThisLen+1] = '\0';
			}
			for ( uint32_t I = 0; I <= ui32Len; ++I ) {
				uint32_t ui32This = (liNewValCopy % _ui32Radix).GetAs32bitUInt();
				_tType tThisChar = static_cast<_tType>(ui32This + '0');
				if ( tThisChar > '9' ) {
					// Radix and number are both greater than 10.
					tThisChar = static_cast<_tType>((ui32This - 10) + 'A');
				}
				if ( ui32ThisLen < _ui32MaxLen ) {
					_ptString[ui32ThisLen--] = tThisChar;
				}
				liNewValCopy /= _ui32Radix;
			}


			return CStd::Min<uint32_t>( (ui32Len + 1) + static_cast<uint32_t>(bNegative), _ui32MaxLen );
		}

		/**
		 * Adds two large integers together and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Add( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			const uint32_t ui32LastPos = _uMaxDigits - 1;

			// Store the sign bits.
			uint32_t ui32SignLeft = (_liLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL);
			uint32_t ui32SignRight = (_liRight.m_ui32Digits[ui32LastPos] & 0x80000000UL);

			_liResult.m_ui32UsedDigits = CStd::Max( _liLeft.m_ui32UsedDigits, _liRight.m_ui32UsedDigits );
			uint64_t ui64Carry = 0ULL;
			for ( uint32_t I = 0; I < _liResult.m_ui32UsedDigits; ++I ) {
				uint64_t ui64Sum = static_cast<uint64_t>(_liLeft.m_ui32Digits[I]) + static_cast<uint64_t>(_liRight.m_ui32Digits[I]) + ui64Carry;
				ui64Carry = ui64Sum >> 32ULL;
				_liResult.m_ui32Digits[I] = static_cast<uint32_t>(ui64Sum & 0xFFFFFFFF);
			}

			// Add any remaining carries.
			if ( ui64Carry ) {
				if ( _liResult.m_ui32UsedDigits == _uMaxDigits ) {
					throw LSSTD_LIE_OVERFLOW;
				}
				else {
					_liResult.m_ui32Digits[_liResult.m_ui32UsedDigits++] = static_cast<uint32_t>(ui64Carry);
				}
			}

			// Remove opening 0's.
			while ( _liResult.m_ui32UsedDigits > 1 && _liResult.m_ui32Digits[_liResult.m_ui32UsedDigits-1] == 0 ) {
				--_liResult.m_ui32UsedDigits;
			}

			// Check for overflow.
			if ( ui32SignLeft == ui32SignRight &&
				(_liResult.m_ui32Digits[ui32LastPos] & 0x80000000UL) != ui32SignLeft ) {
				throw LSSTD_LIE_OVERFLOW;
			}
		}

		/**
		 * Subtract two large integers together and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Subtract( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			const uint32_t ui32LastPos = _uMaxDigits - 1;

			// Store the sign bits.
			uint32_t ui32SignLeft = (_liLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL);
			uint32_t ui32SignRight = (_liRight.m_ui32Digits[ui32LastPos] & 0x80000000UL);

			_liResult.m_ui32UsedDigits = CStd::Max( _liLeft.m_ui32UsedDigits, _liRight.m_ui32UsedDigits );
			int64_t i64Carry = 0;
			uint32_t I;
			for ( I = 0; I < _liResult.m_ui32UsedDigits; ++I ) {
				int64_t i64Dif = static_cast<int64_t>(_liLeft.m_ui32Digits[I]) - static_cast<int64_t>(_liRight.m_ui32Digits[I]) - i64Carry;
				_liResult.m_ui32Digits[I] = static_cast<uint32_t>(i64Dif & 0xFFFFFFFF);

				if ( i64Dif < 0 ) {
					i64Carry = 1;
				}
				else {
					i64Carry = 0;
				}
			}

			if ( i64Carry ) {
				for ( ; I < _uMaxDigits; ++I ) {
					_liResult.m_ui32Digits[I] = 0xFFFFFFFFUL;
				}
				_liResult.m_ui32UsedDigits = _uMaxDigits;
			}

			// Check for overflow.
			if ( ui32SignLeft != ui32SignRight &&
				(_liResult.m_ui32Digits[ui32LastPos] & 0x80000000UL) != ui32SignLeft ) {
				throw LSSTD_LIE_OVERFLOW;
			}
		}

		/**
		 * Multiply two large integers and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Multiply( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			const uint32_t ui32LastPos = _uMaxDigits - 1;

			// We work on positive values only, then negate at the end.
			LSBOOL bLeftNeg = false, bRightNeg = false;

			CLargeInteger<_uMaxDigits> liLeftAbs( _liLeft );
			CLargeInteger<_uMaxDigits> liRightAbs( _liRight );

			if ( _liLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bLeftNeg = true;
				liLeftAbs = -_liLeft;
			}
			if ( _liRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bRightNeg = true;
				liRightAbs = -_liRight;
			}

			// Clear the return.
			_liResult = CLargeInteger<_uMaxDigits>();


			// Multiply.
			for ( uint32_t I = 0; I < liLeftAbs.m_ui32UsedDigits; ++I ) {
				if ( liLeftAbs.m_ui32Digits[I] == 0 ) { continue; }
				uint64_t ui64Carry = 0ULL;
				for ( uint32_t J = 0, K = I; J < liRightAbs.m_ui32UsedDigits; ++J, ++K ) {
					uint64_t ui64Value = static_cast<uint64_t>(liLeftAbs.m_ui32Digits[I]) * static_cast<uint64_t>(liRightAbs.m_ui32Digits[J]) +
						static_cast<uint64_t>(_liResult.m_ui32Digits[K]) + ui64Carry;

					_liResult.m_ui32Digits[K] = static_cast<uint32_t>(ui64Value & 0xFFFFFFFF);

					ui64Carry = ui64Value >> 32ULL;
				}
				if ( ui64Carry ) {
					_liResult.m_ui32Digits[I+liRightAbs.m_ui32UsedDigits] = static_cast<uint32_t>(ui64Carry);
				}
			}

			_liResult.m_ui32UsedDigits = liLeftAbs.m_ui32UsedDigits + liRightAbs.m_ui32UsedDigits;
			_liResult.m_ui32UsedDigits = static_cast<uint32_t>(CStd::Min<uint32_t>( _liResult.m_ui32UsedDigits, _uMaxDigits ));
			// Remove opening 0's.
			while ( _liResult.m_ui32UsedDigits > 1 && _liResult.m_ui32Digits[_liResult.m_ui32UsedDigits-1] == 0 ) {
				--_liResult.m_ui32UsedDigits;
			}

			// Check for overflow.
			if ( _liResult.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				if ( bLeftNeg != bRightNeg && _liResult.m_ui32Digits[ui32LastPos] == 0x80000000UL ) {
					// Handle the special case in which multiplication results in a maximum negative
					//	number in 2's compliment.
					if ( _liResult.m_ui32UsedDigits == 1 ) {
						return;
					}
					LSBOOL bMaxNeg = true;
					for ( uint32_t I = 0; I < _liResult.m_ui32UsedDigits - 1; ++I ) {
						if ( _liResult.m_ui32Digits[I] != 0 ) {
							bMaxNeg = false;
							break;
						}
					}
					if ( bMaxNeg ) {
						return;
					}
				}
				throw LSSTD_LIE_OVERFLOW;
			}

			// Reverse the signs if necessary.
			if ( bLeftNeg != bRightNeg ) {
				Negate( _liResult, _liResult );
			}
		}

		/**
		 * Divide two large integers and store the result in a third.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Divide( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			if ( _liRight.m_ui32UsedDigits == 1 && _liRight.m_ui32Digits[0] == 0x00000000UL ) {
				throw LSSTD_LIE_DIVISIONBYZERO;
			}
			const uint32_t ui32LastPos = _uMaxDigits - 1;

			
			CLargeInteger<_uMaxDigits> liTemp0;
			CLargeInteger<_uMaxDigits> liTemp1;
			CLargeInteger<_uMaxDigits> * pliNewLeft;
			CLargeInteger<_uMaxDigits> * pliNewRight;
			if ( _uMaxDigits < 128 ) {
				liTemp0 = CLargeInteger<_uMaxDigits>( _liLeft );
				liTemp1 = CLargeInteger<_uMaxDigits>( _liRight );
				pliNewLeft = &liTemp0;
				pliNewRight = &liTemp1;
			}
			else {
				pliNewLeft = new CLargeInteger<_uMaxDigits>( _liLeft );
				pliNewRight = new CLargeInteger<_uMaxDigits>( _liRight );
			}
			
			LSBOOL bDivisorNeg = false, bDividendNeg = false;
			if ( _liLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDividendNeg = true;
				(*pliNewLeft) = -(*pliNewLeft);
			}
			if ( _liRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDivisorNeg = true;
				(*pliNewRight) = -(*pliNewRight);
			}

			if ( (*pliNewLeft) < (*pliNewRight) ) {
				_liResult = CLargeInteger<_uMaxDigits>( 0ULL );
				if ( _uMaxDigits < 128 ) {
				}
				else {
					delete pliNewLeft;
					delete pliNewRight;
				}
				return;
			}

			// Hold the results.
			CLargeInteger<_uMaxDigits> * pliRem = new CLargeInteger<_uMaxDigits>();
			if ( (*pliNewRight).m_ui32UsedDigits == 1 ) {
				DivideS( (*pliNewLeft), (*pliNewRight),
					_liResult, (*pliRem) );
			}
			else {
				DivideM( (*pliNewLeft), (*pliNewRight),
					_liResult, (*pliRem) );
			}

			if ( bDivisorNeg != bDividendNeg ) {
				// Flip the sign.
				_liResult = -_liResult;
			}

			delete pliRem;
			if ( _uMaxDigits < 128 ) {
			}
			else {
				delete pliNewLeft;
				delete pliNewRight;
			}
		}

		/**
		 * Obtain the modulus of two large integers and store the result in a third.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Modulus( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			if ( _liRight.m_ui32UsedDigits == 1 && _liRight.m_ui32Digits[0] == 0x00000000UL ) {
				throw LSSTD_LIE_DIVISIONBYZERO;
			}
			const uint32_t ui32LastPos = _uMaxDigits - 1;

			

			CLargeInteger<_uMaxDigits> * pliNewLeft = new CLargeInteger<_uMaxDigits>( _liLeft );
			CLargeInteger<_uMaxDigits> * pliNewRight = new CLargeInteger<_uMaxDigits>( _liRight );
			
			LSBOOL bDividendNeg = false;
			if ( _liLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDividendNeg = true;
				(*pliNewLeft) = -(*pliNewLeft);
			}
			if ( _liRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				(*pliNewRight) = -(*pliNewRight);
			}

			if ( (*pliNewLeft) < (*pliNewRight) ) {
				_liResult = _liLeft;
				delete pliNewLeft;
				delete pliNewRight;
				return;
			}

			// Hold the results.
			CLargeInteger<_uMaxDigits> * pliRes = new CLargeInteger<_uMaxDigits>();
			if ( (*pliNewRight).m_ui32UsedDigits == 1 ) {
				DivideS( (*pliNewLeft), (*pliNewRight),
					(*pliRes), _liResult );
			}
			else {
				DivideM( (*pliNewLeft), (*pliNewRight),
					(*pliRes), _liResult );
			}

			if ( bDividendNeg ) {
				// Flip the sign.
				_liResult = -_liResult;
			}

			delete pliRes;
			delete pliNewLeft;
			delete pliNewRight;
		}

		/**
		 * Negate a large integer.
		 *
		 * \param _liValue The value to negate.
		 * \param _liResult Result.
		 */
		static void LSE_CALL						Negate( const CLargeInteger<_uMaxDigits> &_liValue,
			CLargeInteger<_uMaxDigits> &_liResult ) {
			// Handle negation of 0 manually.
			if ( _liValue.m_ui32UsedDigits == 0 && _liValue.m_ui32Digits[0] == 0 ) {
				for ( int32_t I = _uMaxDigits; --I >= 0; ) {
					_liResult.m_ui32Digits[I] = 0;
				}
				_liResult.m_ui32UsedDigits = 1;
				return;
			}

			// Store the sign bit before processing.
			uint32_t ui32SignBit = (_liValue.m_ui32Digits[_uMaxDigits-1] & 0x80000000UL);

			// Perform 1's compliment.
			for ( int32_t I = _uMaxDigits; --I >= 0; ) {
				_liResult.m_ui32Digits[I] = ~_liValue.m_ui32Digits[I];
			}

			// Add 1 to the result.
			uint64_t ui64Value, ui64Carry = 1;
			for ( uint32_t I = 0; ui64Carry && I < _uMaxDigits; ++I ) {
				ui64Value = _liResult.m_ui32Digits[I] + 1ULL;
				_liResult.m_ui32Digits[I] = static_cast<uint32_t>(ui64Value & 0xFFFFFFFF);
				ui64Carry = ui64Value >> 32ULL;
			}

			// Check for overflow.
			if ( ui32SignBit == (_liResult.m_ui32Digits[_uMaxDigits-1] & 0x80000000UL) ) {
				throw LSSTD_LIE_OVERFLOW;
			}

			_liResult.m_ui32UsedDigits = _uMaxDigits;
			// Remove opening 0's.
			while ( _liResult.m_ui32UsedDigits > 1 && _liResult.m_ui32Digits[_liResult.m_ui32UsedDigits-1] == 0 ) {
				--_liResult.m_ui32UsedDigits;
			}
		}

		/**
		 * Shift bits left (up).
		 *
		 * \param _pui32Data Acts as both the input array of bytes and the output.
		 * \param _ui32Len Length of the buffer.
		 * \param _ui32Amount Amount by which to shift.
		 * \return Returns the number of used digits.
		 */
		static uint32_t LSE_CALL					ShiftLeft( uint32_t * _pui32Data, uint32_t _ui32Len,
			uint32_t _ui32Amount ) {

			uint32_t ui32ShiftAmt = 32;
			uint32_t ui32Used = _ui32Len;

			while ( ui32Used > 1 && _pui32Data[ui32Used-1] == 0 ) {
				--ui32Used;
			}


			for ( uint32_t I = _ui32Amount; I > 0; ) {
				if ( I < ui32ShiftAmt ) {
					ui32ShiftAmt = I;
				}

				uint64_t ui64Carry = 0ULL;

				for ( uint32_t J = 0; J < ui32Used; ++J ) {
					uint64_t ui64Value = static_cast<uint64_t>(_pui32Data[J]) << ui32ShiftAmt;
					ui64Value |= ui64Carry;

					_pui32Data[J] = static_cast<uint32_t>(ui64Value);
					ui64Carry = ui64Value >> 32ULL;
				}

				if ( ui64Carry ) {
					if ( ui32Used + 1 <= _ui32Len ) {
						_pui32Data[ui32Used++] = static_cast<uint32_t>(ui64Carry);
					}
				}

				I -= ui32ShiftAmt;
			}
			return ui32Used;
		}

		/**
		 * Shift bits right (down).
		 *
		 * \param _pui32Data Acts as both the input array of bytes and the output.
		 * \param _ui32Len Length of the buffer.
		 * \param _ui32Amount Amount by which to shift.
		 * \return Returns the number of used digits.
		 */
		static uint32_t LSE_CALL					ShiftRight( uint32_t * _pui32Data, uint32_t _ui32Len,
			uint32_t _ui32Amount ) {

			uint32_t ui32ShiftAmt = 32;
			uint32_t ui32InvShift = 0;
			uint32_t ui32Used = _ui32Len;

			while ( ui32Used > 1 && _pui32Data[ui32Used-1] == 0 ) {
				--ui32Used;
			}


			for ( uint32_t I = _ui32Amount; I > 0; ) {
				if ( I < ui32ShiftAmt ) {
					ui32ShiftAmt = I;
					ui32InvShift = 32 - ui32ShiftAmt;
				}

				uint64_t ui64Carry = 0ULL;

				for ( int32_t J = static_cast<int32_t>(ui32Used - 1); J >= 0; --J ) {
					uint64_t ui64Value = static_cast<uint64_t>(_pui32Data[J]) >> static_cast<uint64_t>(ui32ShiftAmt);
					ui64Value |= ui64Carry;

					ui64Carry = _pui32Data[J] << ui32InvShift;
					_pui32Data[J] = static_cast<uint32_t>(ui64Value);
				}

				I -= ui32ShiftAmt;
			}

			// Remove opening 0's.
			while ( ui32Used > 1 && _pui32Data[ui32Used-1] == 0 ) {
				--ui32Used;
			}

			return ui32Used;
		}


	protected :
		// == Members.
		// The digits we maintain.
		uint32_t									m_ui32Digits[_uMaxDigits];

		// Number of digits we actually use.
		uint32_t									m_ui32UsedDigits;

	private :
		/**
		 * Divide the left operand by the right, storing the result in _liQuotient and _liRemainder.  The
		 *	right operand must have more than one digit, and must not be 0.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liQuotient The quotiant part of the result.
		 * \param _liRemainder The remainder of the result.
		 */
		static void LSE_CALL						DivideM( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liQuotient,
			CLargeInteger<_uMaxDigits> &_liRemainder ) {

			// Allocate on the heap to help avoid stack overflow.
			//	A bit slower, but division is slow, and this is not a high-performance routine.
			CLargeInteger<_uMaxDigits> liTemp0;
			CLargeInteger<_uMaxDigits+1> liTemp1;
			CLargeInteger<_uMaxDigits> * pliResult;
			CLargeInteger<_uMaxDigits+1> * pliRemainder;
			if ( _uMaxDigits < 128 ) {
				pliResult = &liTemp0;
				pliRemainder = &liTemp1;
			}
			else {
				pliResult = new CLargeInteger<_uMaxDigits>();
				pliRemainder = new CLargeInteger<_uMaxDigits+1>();
			}

			uint32_t ui32Mask = 0x80000000UL;
			uint32_t ui32Value = _liRight.m_ui32Digits[_liRight.m_ui32UsedDigits-1];
			uint32_t ui32Shift = 0, ui32ResPos = 0;
			while ( ui32Mask && (ui32Value & ui32Mask) == 0 ) {
				++ui32Shift;
				ui32Mask >>= 1;
			}

			for ( uint32_t I = _liLeft.m_ui32UsedDigits; I--; ) {
				pliRemainder->GetBytes()[I] = _liLeft.m_ui32Digits[I];
			}
			pliRemainder->SetLength( _liLeft.m_ui32UsedDigits + 1 );

			ShiftLeft( pliRemainder->GetBytes(), pliRemainder->GetLength(), ui32Shift );
			CLargeInteger<_uMaxDigits> liNewRight = _liRight << ui32Shift;

			uint32_t ui32J = pliRemainder->GetLength() - liNewRight.m_ui32UsedDigits;
			uint32_t ui32Pos = pliRemainder->GetLength() - 1;

			uint32_t ui32FirstDivisorByte = liNewRight.m_ui32Digits[liNewRight.m_ui32UsedDigits-1];
			uint32_t ui32SecondDivisorByte = liNewRight.m_ui32Digits[liNewRight.m_ui32UsedDigits-2];

			uint32_t ui32DivisorLen = liNewRight.m_ui32UsedDigits + 1;
			uint32_t * pui32DividendPart = new uint32_t[ui32DivisorLen];

			for ( ; ui32J > 0; --ui32J ) {
				uint64_t ui64Dividend = (static_cast<uint64_t>(pliRemainder->GetBytes()[ui32Pos]) << 32ULL) + pliRemainder->GetBytes()[ui32Pos-1];

				uint64_t ui64Q = ui64Dividend / ui32FirstDivisorByte;
				uint64_t ui64R = ui64Dividend % ui32FirstDivisorByte;

				LSBOOL bDone = false;
				while ( !bDone ) {
					bDone = true;

					if ( ui64Q == 0x100000000ULL ||
						(ui64Q * ui32SecondDivisorByte) > ((ui64R << 32ULL) + pliRemainder->GetBytes()[ui32Pos-2]) ) {
						--ui64Q;
						ui64R += ui32FirstDivisorByte;
						if ( ui64R < 0x100000000ULL ) {
							bDone = false;
						}
					}
				}

				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					pui32DividendPart[I] = pliRemainder->GetBytes()[ui32Pos-I];
				}

				// Again on the heap instead of on the stack.
				CLargeInteger<_uMaxDigits> liTemp2;
				CLargeInteger<_uMaxDigits> * pliK;
				if ( _uMaxDigits < 128 ) {
					liTemp2 = CLargeInteger<_uMaxDigits>( pui32DividendPart, ui32DivisorLen );
					pliK = &liTemp2;
				}
				else {
					pliK = new CLargeInteger<_uMaxDigits>( pui32DividendPart, ui32DivisorLen );
				}
				CLargeInteger<_uMaxDigits> liS( liNewRight * CLargeInteger<_uMaxDigits>( static_cast<int64_t>(ui64Q) ) );

				while ( liS > (*pliK) ) {
					--ui64Q;
					liS = liS - liNewRight;
				}

				CLargeInteger<_uMaxDigits> liY = (*pliK) - liS;
				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					pliRemainder->GetBytes()[ui32Pos-I] = liY.m_ui32Digits[liNewRight.m_ui32UsedDigits-I];
				}

				if ( _uMaxDigits < 128 ) {
				}
				else {
					delete pliK;
				}

				pliResult->m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Q);
				--ui32Pos;
			}

			_liQuotient.m_ui32UsedDigits = ui32ResPos;

			uint32_t Y = 0;
			for ( int32_t X = static_cast<int32_t>(_liQuotient.m_ui32UsedDigits) - 1; X >= 0; --X, ++Y ) {
				_liQuotient.m_ui32Digits[Y] = pliResult->m_ui32Digits[X];
			}

			for ( ; Y < _uMaxDigits; ++Y ) {
				_liQuotient.m_ui32Digits[Y] = 0;
			}

			while ( _liQuotient.m_ui32UsedDigits > 1 && _liQuotient.m_ui32Digits[_liQuotient.m_ui32UsedDigits-1] == 0 ) {
				--_liQuotient.m_ui32UsedDigits;
			}

			_liQuotient.m_ui32UsedDigits = CStd::Max<uint32_t>( _liQuotient.m_ui32UsedDigits, 1 );

			_liRemainder.m_ui32UsedDigits = ShiftRight( pliRemainder->GetBytes(), pliRemainder->GetLength(), ui32Shift );
			CStd::MemCpy( _liRemainder.m_ui32Digits, pliRemainder->GetBytes(), sizeof( _liRemainder.m_ui32Digits[0] ) * _liRemainder.m_ui32UsedDigits );
			for ( Y = _liRemainder.m_ui32UsedDigits; Y < _uMaxDigits; ++Y ) {
				_liRemainder.m_ui32Digits[Y] = 0;
			}

			delete pui32DividendPart;
			if ( _uMaxDigits < 128 ) {
			}
			else {
				delete pliRemainder;
				delete pliResult;
			}
		}

		/**
		 * Divide the left operand by the right, storing the result in _liQuotient and _liRemainder.  The
		 *	right operand must have one digit, and must not be 0.
		 *
		 * \param _liLeft Left operand.
		 * \param _liRight Right operand.
		 * \param _liQuotient The quotiant part of the result.
		 * \param _liRemainder The remainder of the result.
		 */
		static void LSE_CALL						DivideS( const CLargeInteger<_uMaxDigits> &_liLeft,
			const CLargeInteger<_uMaxDigits> &_liRight,
			CLargeInteger<_uMaxDigits> &_liQuotient,
			CLargeInteger<_uMaxDigits> &_liRemainder ) {

			CLargeInteger<_uMaxDigits> liResult;
			uint32_t ui32ResPos = 0;

			// Copy dividend to remainder.
			_liRemainder = _liLeft;

			// Remove opening 0's.
			while ( _liRemainder.m_ui32UsedDigits > 1 && _liRemainder.m_ui32Digits[_liRemainder.m_ui32UsedDigits-1] == 0 ) {
				--_liRemainder.m_ui32UsedDigits;
			}

			uint64_t ui64Divisor = _liRight.m_ui32Digits[0];
			int32_t i32Pos = static_cast<int32_t>(_liRemainder.m_ui32UsedDigits - 1);
			uint64_t ui64Dividend = _liRemainder.m_ui32Digits[i32Pos];

			if ( ui64Dividend >= ui64Divisor ) {
				uint64_t ui64Quotient = ui64Dividend / ui64Divisor;
				liResult.m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Quotient);
				_liRemainder.m_ui32Digits[i32Pos] = static_cast<uint32_t>(ui64Dividend % ui64Divisor);
			}
			--i32Pos;

			while ( i32Pos >= 0 ) {
				ui64Dividend = (static_cast<uint64_t>(_liRemainder.m_ui32Digits[i32Pos+1]) << 32ULL) + _liRemainder.m_ui32Digits[i32Pos];
				uint64_t ui64Quotient = ui64Dividend / ui64Divisor;
				liResult.m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Quotient);

				_liRemainder.m_ui32Digits[i32Pos+1] = 0;
				_liRemainder.m_ui32Digits[i32Pos--] = static_cast<uint32_t>(ui64Dividend % ui64Divisor);
			}

			// Copy the result over the output quotient.
			_liQuotient.m_ui32UsedDigits = ui32ResPos;
			
			uint32_t J = 0;
			for ( int32_t I = static_cast<int32_t>(_liQuotient.m_ui32UsedDigits) - 1; I >= 0; --I, ++J ) {
				_liQuotient.m_ui32Digits[J] = liResult.m_ui32Digits[I];
			}

			for ( ; J < _uMaxDigits; ++J ) {
				_liQuotient.m_ui32Digits[J] = 0;
			}

			// Fix it.
			while ( _liQuotient.m_ui32UsedDigits > 1 && _liQuotient.m_ui32Digits[_liQuotient.m_ui32UsedDigits-1] == 0 ) {
				--_liQuotient.m_ui32UsedDigits;
			}
			_liQuotient.m_ui32UsedDigits = CStd::Max<uint32_t>( _liQuotient.m_ui32UsedDigits, 1 );

			while ( _liRemainder.m_ui32UsedDigits > 1 && _liRemainder.m_ui32Digits[_liRemainder.m_ui32UsedDigits-1] == 0 ) {
				--_liRemainder.m_ui32UsedDigits;
			}

		}
	};

}	// namespace lsstd

#pragma warning( pop )

#endif	// __LSSTD_LARGEINTEGER_H__
