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
 * Description: A 128-bit integer class.  This is a high-performance 128-bit signed integer class.  Use this
 *	when you do not need larger values than 128 bits can hold and require performance.
 */


#ifndef __LSE_128BITINTEGER_H__
#define __LSE_128BITINTEGER_H__

#include "../LSSTDStandardLib.h"

namespace lsstd {

	/**
	 * Class C128BitInteger
	 * \brief A 128-bit integer class.
	 *
	 * Description: A 128-bit integer class.  This is a high-performance 128-bit signed integer class.  Use this
	 *	when you do not need larger values than 128 bits can hold and require performance.
	 */
	class C128BitInteger {
	public :
		// == Various constructors.
		LSE_CALLCTOR								C128BitInteger() {}
		LSE_CALLCTOR								C128BitInteger( const C128BitInteger &_biOp ) {
			m_ui32Digits[0] = _biOp.m_ui32Digits[0];
			m_ui32Digits[1] = _biOp.m_ui32Digits[1];
			m_ui32Digits[2] = _biOp.m_ui32Digits[2];
			m_ui32Digits[3] = _biOp.m_ui32Digits[3];
		}
		LSE_CALLCTOR								C128BitInteger( int64_t _i64Src ) {
			m_ui32Digits[0] = static_cast<uint32_t>(_i64Src & 0xFFFFFFFFULL);
			m_ui32Digits[1] = static_cast<uint32_t>(_i64Src >> 32ULL);
			_i64Src = (_i64Src >> 63);

			m_ui32Digits[2] = m_ui32Digits[3] = static_cast<uint32_t>(_i64Src);
		}
		LSE_CALLCTOR								C128BitInteger( uint64_t _ui64Src ) {
			m_ui32Digits[0] = static_cast<uint32_t>(_ui64Src & 0xFFFFFFFFULL);
			m_ui32Digits[1] = static_cast<uint32_t>(_ui64Src >> 32ULL);
			m_ui32Digits[2] = m_ui32Digits[3] = 0;
		}
		LSE_CALLCTOR								C128BitInteger( const uint32_t * _pui32Src ) {
			uint32_t ui32Len = 4;
			while ( ui32Len && !_pui32Src[--ui32Len] ) {}
			++ui32Len;
			uint32_t I, J = ui32Len - 1;
			for ( I = 0; I < ui32Len; ++I, --J ) {
				m_ui32Digits[I] = _pui32Src[J];
			}
			for ( ; I < 4; ++I ) {
				m_ui32Digits[I] = 0;
			}
		}


		// == Operators.
		/**
		 * Perform unary negation on this number.  The negated value is returned.
		 *
		 * \return Returns the 2's compliment of this number.
		 */
		C128BitInteger LSE_CALL						operator - () const {
			C128BitInteger biReturn;
			Negate( (*this), biReturn );

			return biReturn;
		}

		/**
		 * Adds two 128-bit integers to create a third 128-bit integer holding the result.
		 *
		 * \param _biOp The right operand.
		 * \return Returns the result as a new 128-bit integer.
		 */
		C128BitInteger LSE_CALL						operator + ( const C128BitInteger &_biOp ) const {
			C128BitInteger biReturn;
			Add( (*this), _biOp, biReturn );

			return biReturn;
		}

		/**
		 * Multiply two large integers to create a third 128-bit integer holding the result.
		 *
		 * \param _biOp The right operand.
		 * \return Returns the result as a new 128-bit integer.
		 */
		C128BitInteger LSE_CALL						operator * ( const C128BitInteger &_biOp ) const {
			C128BitInteger biReturn;
			Multiply( (*this), _biOp, biReturn );

			return biReturn;
		}

		/**
		 * Multiply this large integer by other, assiging the result to this large integer.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _biOp The right operand.
		 * \return Returns this large integer, which will be filled with the result.
		 */
		C128BitInteger LSE_CALL						operator *= ( const C128BitInteger &_biOp ) {
			C128BitInteger biReturn;
			Multiply( (*this), _biOp, biReturn );
			(*this) = biReturn;

			return (*this);
		}

		/**
		 * Divide two large integers to create a third large integer holding the result.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _biOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		C128BitInteger LSE_CALL						operator / ( const C128BitInteger &_biOp ) const {
			C128BitInteger biReturn;
			Divide( (*this), _biOp, biReturn );

			return biReturn;
		}

		/**
		 * Divide this large integer by other, assiging the result to this large integer.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _biOp The right operand.
		 * \return Returns this large integer, which will be filled with the result.
		 */
		C128BitInteger LSE_CALL						operator /= ( const C128BitInteger &_biOp ) {
			C128BitInteger biReturn;
			Divide( (*this), _biOp, biReturn );
			(*this) = biReturn;

			return (*this);
		}

		/**
		 * Modulus two large integers to create a third large integer holding the result.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _biOp The right operand.
		 * \return Returns the result as a new large integer.
		 */
		C128BitInteger LSE_CALL						operator % ( const C128BitInteger &_biOp ) const {
			C128BitInteger biReturn;
			Modulus( (*this), _biOp, biReturn );

			return biReturn;
		}

		/**
		 * Subtract two large integers to create a third 128-bit integer holding the result.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _biOp The right operand.
		 * \return Returns the result as a new 128-bit integer.
		 */
		C128BitInteger LSE_CALL						operator - ( const C128BitInteger &_biOp ) const {
			C128BitInteger biReturn;
			Subtract( (*this), _biOp, biReturn );

			return biReturn;
		}

		/**
		 * Shift bits left by the given amount.  The shifted value is returned.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the result as a new large integer.
		 */
		C128BitInteger LSE_CALL						operator << ( uint32_t _ui32Shift ) const {
			C128BitInteger biReturn( (*this) );
			ShiftLeft( biReturn.m_ui32Digits, 4, _ui32Shift );

			return biReturn;
		}

		/**
		 * Shift bits left by the given amount, storing the result in this instance.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the this instance, which holds the result.
		 */
		C128BitInteger & LSE_CALL		operator <<= ( uint32_t _ui32Shift ) {
			ShiftLeft( m_ui32Digits, 4, _ui32Shift );

			return (*this);
		}

		/**
		 * Shift bits right by the given amount.  The shifted value is returned.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the result as a new large integer.
		 */
		C128BitInteger LSE_CALL						operator >> ( uint32_t _ui32Shift ) const {
			C128BitInteger biReturn( (*this) );
			ShiftRight( biReturn.m_ui32Digits, 4, _ui32Shift );

			// Fix sign bits on negative numbers.
			if ( m_ui32Digits[4-1] & 0x80000000UL ) {
				for ( uint32_t I = 4 - 1; I >= 4; --I ) {
					biReturn.m_ui32Digits[I] = 0xFFFFFFFF;
				}

				uint32_t ui32Mask = 0x80000000UL;
				for ( uint32_t I = 0; I < 32; ++I ) {
					if ( biReturn.m_ui32Digits[4-1] & ui32Mask ) {
						break;
					}
					biReturn.m_ui32Digits[4-1] |= ui32Mask;
					ui32Mask >>= 1;
				}
			}

			return biReturn;
		}

		/**
		 * Shift bits right by the given amount, storing the result in this instance.
		 *
		 * \param _ui32Shift The amount by which to shift.
		 * \return Returns the this instance, which holds the result.
		 */
		C128BitInteger & LSE_CALL		operator >>= ( uint32_t _ui32Shift ) {
			uint32_t ui32Used = ShiftRight( m_ui32Digits, 4, _ui32Shift );
			ui32Used = CStd::Max<uint32_t>( ui32Used, 1 );

			// Fix sign bits on negative numbers.
			if ( m_ui32Digits[4-1] & 0x80000000UL ) {
				for ( uint32_t I = 4 - 1; I >= ui32Used; --I ) {
					m_ui32Digits[I] = 0xFFFFFFFF;
				}

				uint32_t ui32Mask = 0x80000000UL;
				for ( uint32_t I = 0; I < 32; ++I ) {
					if ( m_ui32Digits[ui32Used-1] & ui32Mask ) {
						break;
					}
					m_ui32Digits[ui32Used-1] |= ui32Mask;
					ui32Mask >>= 1;
				}
				ui32Used = 4;
			}

			return (*this);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _biOp The right operand.
		 * \return Returns true if this large integer is greater than the given large integer.
		 */
		bool LSE_CALL								operator > ( const C128BitInteger &_biOp ) const {
			int32_t i32Pos = 4 - 1;

			if ( (m_ui32Digits[i32Pos] & 0x80000000UL) && !(_biOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return false;
			}
			if ( !(m_ui32Digits[i32Pos] & 0x80000000UL) && (_biOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return true;
			}

			// Skip past all digits that are the same.
			for ( i32Pos = 4; i32Pos >= 0 && m_ui32Digits[i32Pos] == _biOp.m_ui32Digits[i32Pos]; --i32Pos ) {}

			// If we found a different digit.
			if ( i32Pos >= 0 ) {
				return m_ui32Digits[i32Pos] > _biOp.m_ui32Digits[i32Pos];
			}

			// They are the same.
			return false;
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _biOp The right operand.
		 * \return Returns true if this large integer is less than the given large integer.
		 */
		bool LSE_CALL								operator < ( const C128BitInteger &_biOp ) const {
			int32_t i32Pos = 4 - 1;

			if ( (m_ui32Digits[i32Pos] & 0x80000000UL) && !(_biOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return true;
			}
			if ( !(m_ui32Digits[i32Pos] & 0x80000000UL) && (_biOp.m_ui32Digits[i32Pos] & 0x80000000UL) ) {
				return false;
			}

			// Skip past all digits that are the same.
			for ( i32Pos = 3; i32Pos >= 0 && m_ui32Digits[i32Pos] == _biOp.m_ui32Digits[i32Pos]; --i32Pos ) {}

			// If we found a different digit.
			if ( i32Pos >= 0 ) {
				return m_ui32Digits[i32Pos] < _biOp.m_ui32Digits[i32Pos];
			}

			// They are the same.
			return false;
		}


		// == Functions.
		/**
		 * Negate a 128-bit integer.
		 *
		 * \param _biValue The value to negate.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Negate( const C128BitInteger &_biValue,
			C128BitInteger &_biResult ) {
			// Handle negation of 0 manually.
			if ( _biValue.m_ui32Digits[0] == 0ULL && _biValue.m_ui32Digits[1] == 0ULL &&
				_biValue.m_ui32Digits[2] == 0ULL && _biValue.m_ui32Digits[3] == 0ULL ) {
				_biResult.m_ui32Digits[0] = _biResult.m_ui32Digits[1] = _biResult.m_ui32Digits[2] = _biResult.m_ui32Digits[3] = 0ULL;
				return;
			}

			// Perform 1's compliment.
			for ( int32_t I = 4; --I >= 0; ) {
				_biResult.m_ui32Digits[I] = ~_biValue.m_ui32Digits[I];
			}

			// Add 1 to the result.
			uint64_t ui64Value, ui64Carry = 1;
			for ( uint32_t I = 0; ui64Carry && I < 4; ++I ) {
				ui64Value = _biResult.m_ui32Digits[I] + 1;
				_biResult.m_ui32Digits[I] = static_cast<uint32_t>(ui64Value & 0xFFFFFFFFULL);
				ui64Carry = ui64Value >> 32ULL;
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

		/**
		 * Adds two 128-bit integers together and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Add( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biResult ) {

			uint64_t ui64Carry = 0ULL;
			for ( uint32_t I = 0; I < 4; ++I ) {
				uint64_t ui64Sum = static_cast<uint64_t>(_biLeft.m_ui32Digits[I]) + static_cast<uint64_t>(_biRight.m_ui32Digits[I]) + ui64Carry;
				ui64Carry = ui64Sum >> 32ULL;
				_biResult.m_ui32Digits[I] = static_cast<uint32_t>(ui64Sum & 0xFFFFFFFFULL);
			}
			// Carries are simply lost.  This class behaves like a real 128-bit integer would.
		}

		/**
		 * Subtract two 128-bit integers together and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Subtract( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biResult ) {
			uint32_t ui32LeftUsed = 4;
			while ( ui32LeftUsed && !_biLeft.m_ui32Digits[--ui32LeftUsed] ) {}
			++ui32LeftUsed;
			uint32_t ui32RightUsed = 4;
			while ( ui32RightUsed && !_biRight.m_ui32Digits[--ui32RightUsed] ) {}
			++ui32RightUsed;
			uint32_t ui32Total = CStd::Max( ui32LeftUsed, ui32RightUsed );

			int64_t i64Carry = 0;
			uint32_t I;
			for ( I = 0; I < ui32Total; ++I ) {
				int64_t i64Dif = static_cast<int64_t>(_biLeft.m_ui32Digits[I]) - static_cast<int64_t>(_biRight.m_ui32Digits[I]) - i64Carry;
				_biResult.m_ui32Digits[I] = static_cast<uint32_t>(i64Dif & 0xFFFFFFFF);

				if ( i64Dif < 0LL ) {
					i64Carry = 1LL;
				}
				else {
					i64Carry = 0LL;
				}
			}

			if ( i64Carry ) {
				for ( ; I < 4; ++I ) {
					_biResult.m_ui32Digits[I] = 0xFFFFFFFFUL;
				}
			}
			else {
				for ( ; I < 4; ++I ) {
					_biResult.m_ui32Digits[I] = 0;
				}
			}
		}

		/**
		 * Multiply two 128-bit integers and store the result in a third.  An exception is
		 *	thrown on overflow.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Multiply( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biResult ) {
			const uint32_t ui32LastPos = 4 - 1;

			// We work on positive values only, then negate at the end.
			LSBOOL bLeftNeg = false, bRightNeg = false;

			C128BitInteger biLeftAbs( _biLeft );
			C128BitInteger biRightAbs( _biRight );

			if ( _biLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bLeftNeg = true;
				biLeftAbs = -_biLeft;
			}
			if ( _biRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bRightNeg = true;
				biRightAbs = -_biRight;
			}

			// Clear the return.
			_biResult = C128BitInteger( 0ULL );


			// Multiply.
			for ( uint32_t I = 0; I < 4; ++I ) {
				if ( biLeftAbs.m_ui32Digits[I] == 0 ) { continue; }
				uint64_t ui64Carry = 0ULL;
				for ( uint32_t J = 0, K = I; J < 4; ++J, ++K ) {
					uint64_t ui64Value = static_cast<uint64_t>(biLeftAbs.m_ui32Digits[I]) * static_cast<uint64_t>(biRightAbs.m_ui32Digits[J]) +
						static_cast<uint64_t>(_biResult.m_ui32Digits[K]) + ui64Carry;

					_biResult.m_ui32Digits[K] = static_cast<uint32_t>(ui64Value & 0xFFFFFFFF);

					ui64Carry = ui64Value >> 32ULL;
				}
				if ( ui64Carry ) {
					_biResult.m_ui32Digits[I+4] = static_cast<uint32_t>(ui64Carry);
				}
			}

			// Reverse the signs if necessary.
			if ( bLeftNeg != bRightNeg ) {
				Negate( _biResult, _biResult );
			}
		}

		/**
		 * Divide two 128-bit integers and store the result in a third.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Divide( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biResult ) {
			if ( !_biRight.m_ui32Digits[0] && !_biRight.m_ui32Digits[1] && !_biRight.m_ui32Digits[2] && !_biRight.m_ui32Digits[3] ) {
				_biResult = C128BitInteger( 0ULL );
				return;
			}
			const uint32_t ui32LastPos = 4 - 1;

			

			C128BitInteger biNewLeft( _biLeft );
			C128BitInteger biNewRight( _biRight );
			
			LSBOOL bDivisorNeg = false, bDividendNeg = false;
			if ( _biLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDividendNeg = true;
				biNewLeft = -biNewLeft;
			}
			if ( _biRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDivisorNeg = true;
				biNewRight = -biNewRight;
			}

			if ( biNewLeft < biNewRight ) {
				_biResult = C128BitInteger( 0ULL );
				return;
			}

			// Get the highest used value.
			uint32_t ui32Used = 4;
			while ( ui32Used && !biNewRight.m_ui32Digits[--ui32Used] ) {}
			++ui32Used;

			// Hold the results.
			C128BitInteger biRem;
			if ( ui32Used == 1 ) {
				DivideS( biNewLeft, biNewRight,
					_biResult, biRem );
			}
			else {
				DivideM( biNewLeft, biNewRight,
					_biResult, biRem );
			}

			if ( bDivisorNeg != bDividendNeg ) {
				// Flip the sign.
				_biResult = -_biResult;
			}
		}

		/**
		 * Obtain the modulus of two 128-bit integers and store the result in a third.  An exception is
		 *	thrown if the right operand is 0.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biResult Result.
		 */
		static void LSE_CALL						Modulus( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biResult ) {
			if ( !_biRight.m_ui32Digits[0] && !_biRight.m_ui32Digits[1] && !_biRight.m_ui32Digits[2] && !_biRight.m_ui32Digits[3] ) {
				_biResult = C128BitInteger( 0ULL );
				return;
			}
			const uint32_t ui32LastPos = 4 - 1;

			

			C128BitInteger biNewLeft( _biLeft );
			C128BitInteger biNewRight( _biRight );
			
			LSBOOL bDividendNeg = false;
			if ( _biLeft.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				bDividendNeg = true;
				biNewLeft = -biNewLeft;
			}
			if ( _biRight.m_ui32Digits[ui32LastPos] & 0x80000000UL ) {
				biNewRight = -biNewRight;
			}

			if ( biNewLeft < biNewRight ) {
				_biResult = _biLeft;
				return;
			}

			// Hold the results.
			// Get the highest used value.
			uint32_t ui32Used = 4;
			while ( ui32Used && !biNewRight.m_ui32Digits[--ui32Used] ) {}
			++ui32Used;

			C128BitInteger biRes;
			if ( ui32Used == 1 ) {
				DivideS( biNewLeft, biNewRight,
					biRes, _biResult );
			}
			else {
				DivideM( biNewLeft, biNewRight,
					biRes, _biResult );
			}

			if ( bDividendNeg ) {
				// Flip the sign.
				_biResult = -_biResult;
			}
		}


	protected :
		// == Members.
		/**
		 * The 2 32-bit components that compose this 128-bit integer.
		 */
		uint32_t									m_ui32Digits[4];


		// == Functions.
		/**
		 * Divide the left operand by the right, storing the result in _biQuotient and _biRemainder.  The
		 *	right operand must have more than one digit, and must not be 0.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biQuotient The quotiant part of the result.
		 * \param _biRemainder The remainder of the result.
		 */
		static void LSE_CALL						DivideM( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biQuotient,
			C128BitInteger &_biRemainder ) {

			C128BitInteger biResult = C128BitInteger( 0ULL );
			uint32_t ui32Remainder[4+1];

			uint32_t ui32Mask = 0x80000000UL;
			uint32_t ui32Used = 4;
			while ( ui32Used && !_biRight.m_ui32Digits[--ui32Used] ) {}
			++ui32Used;
			uint32_t ui32Value = _biRight.m_ui32Digits[ui32Used-1];
			uint32_t ui32Shift = 0, ui32ResPos = 0;
			while ( ui32Mask && (ui32Value & ui32Mask) == 0 ) {
				++ui32Shift;
				ui32Mask >>= 1;
			}

			for ( uint32_t I = 4; I--; ) {
				ui32Remainder[I] = _biLeft.m_ui32Digits[I];
			}
			ui32Remainder[4] = 0;

			ShiftLeft( ui32Remainder, 5, ui32Shift );
			C128BitInteger biNewRight = _biRight << ui32Shift;

			uint32_t ui32RemainderUsed = 4;
			while ( ui32RemainderUsed && !ui32Remainder[--ui32RemainderUsed] ) {}
			++ui32RemainderUsed;
			uint32_t ui32NewRightUsed = 4;
			while ( ui32NewRightUsed && !biNewRight.m_ui32Digits[--ui32NewRightUsed] ) {}
			++ui32NewRightUsed;

			uint32_t ui32J = ui32RemainderUsed - ui32NewRightUsed;
			uint32_t ui32Pos = ui32RemainderUsed - 1;

			uint32_t ui32FirstDivisorByte = biNewRight.m_ui32Digits[ui32NewRightUsed-1];
			uint32_t ui32SecondDivisorByte = biNewRight.m_ui32Digits[ui32NewRightUsed-2];

			uint32_t ui32DivisorLen = ui32NewRightUsed + 1;
			uint32_t ui32DividendPart[5] = { 0 };

			for ( ; ui32J > 0; --ui32J ) {
				uint64_t ui64Dividend = (static_cast<uint64_t>(ui32Remainder[ui32Pos]) << 32ULL) + ui32Remainder[ui32Pos-1];

				uint64_t ui64Q = ui64Dividend / ui32FirstDivisorByte;
				uint64_t ui64R = ui64Dividend % ui32FirstDivisorByte;

				LSBOOL bDone = false;
				while ( !bDone ) {
					bDone = true;

					if ( ui64Q == 0x100000000ULL ||
						(ui64Q * ui32SecondDivisorByte) > ((ui64R << 32ULL) + ui32Remainder[ui32Pos-2]) ) {
						--ui64Q;
						ui64R += ui32FirstDivisorByte;
						if ( ui64R < 0x100000000ULL ) {
							bDone = false;
						}
					}
				}

				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					ui32DividendPart[I] = ui32Remainder[ui32Pos-I];
				}

				// Again on the heap instead of on the stack.
				C128BitInteger biK( ui32DividendPart );
				C128BitInteger biS( biNewRight * C128BitInteger( static_cast<uint64_t>(ui64Q) ) );

				while ( biS > biK ) {
					--ui64Q;
					biS = biS - biNewRight;
				}

				C128BitInteger liY = biK - biS;
				for ( uint32_t I = 0; I < ui32DivisorLen; ++I ) {
					ui32Remainder[ui32Pos-I] = liY.m_ui32Digits[ui32NewRightUsed-I];
				}

				biResult.m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Q);
				--ui32Pos;
			}

			uint32_t Y = 0;
			for ( int32_t X = static_cast<int32_t>(ui32ResPos) - 1; X >= 0; --X, ++Y ) {
				_biQuotient.m_ui32Digits[Y] = biResult.m_ui32Digits[X];
			}

			for ( ; Y < 4; ++Y ) {
				_biQuotient.m_ui32Digits[Y] = 0;
			}

			uint32_t ui32Size = ShiftRight( ui32Remainder, ui32RemainderUsed, ui32Shift );
			for ( Y = ui32Size; Y < 4; ++Y ) {
				ui32Remainder[Y] = 0;
			}
			uint64_t * pui64Dest = reinterpret_cast<uint64_t *>(_biRemainder.m_ui32Digits);
			uint64_t * pui64Src = reinterpret_cast<uint64_t *>(ui32Remainder);
			(*pui64Dest++) = (*pui64Src++);
			(*pui64Dest) = (*pui64Src);
		}

		/**
		 * Divide the left operand by the right, storing the result in _biQuotient and _biRemainder.  The
		 *	right operand must have one digit, and must not be 0.
		 *
		 * \param _biLeft Left operand.
		 * \param _biRight Right operand.
		 * \param _biQuotient The quotiant part of the result.
		 * \param _biRemainder The remainder of the result.
		 */
		static void LSE_CALL						DivideS( const C128BitInteger &_biLeft,
			const C128BitInteger &_biRight,
			C128BitInteger &_biQuotient,
			C128BitInteger &_biRemainder ) {

			C128BitInteger liResult( 0ULL );
			uint32_t ui32ResPos = 0;

			// Copy dividend to remainder.
			_biRemainder = _biLeft;
			int32_t i32Pos = 4;
			while ( i32Pos && !_biRemainder.m_ui32Digits[--i32Pos] ) {}
			++i32Pos;

			uint64_t ui64Divisor = _biRight.m_ui32Digits[0];
			uint64_t ui64Dividend = _biRemainder.m_ui32Digits[i32Pos];

			if ( ui64Dividend >= ui64Divisor ) {
				uint64_t ui64Quotient = ui64Dividend / ui64Divisor;
				liResult.m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Quotient);
				_biRemainder.m_ui32Digits[i32Pos] = static_cast<uint32_t>(ui64Dividend % ui64Divisor);
			}
			--i32Pos;

			while ( i32Pos >= 0 ) {
				ui64Dividend = (static_cast<uint64_t>(_biRemainder.m_ui32Digits[i32Pos+1]) << 32ULL) + _biRemainder.m_ui32Digits[i32Pos];
				uint64_t ui64Quotient = ui64Dividend / ui64Divisor;
				liResult.m_ui32Digits[ui32ResPos++] = static_cast<uint32_t>(ui64Quotient);

				_biRemainder.m_ui32Digits[i32Pos+1] = 0;
				_biRemainder.m_ui32Digits[i32Pos--] = static_cast<uint32_t>(ui64Dividend % ui64Divisor);
			}

			// Copy the result over the output quotient.
			uint32_t J = 0;
			for ( int32_t I = static_cast<int32_t>(ui32ResPos) - 1; I >= 0; --I, ++J ) {
				_biQuotient.m_ui32Digits[J] = liResult.m_ui32Digits[I];
			}

			for ( ; J < 4; ++J ) {
				_biQuotient.m_ui32Digits[J] = 0;
			}
		}

	};

}	// namespace lsstd

#endif	// __LSE_128BITINTEGER_H__
