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
 * Description: Fixed-point math template class.
 */


#ifndef __LSM_FIXED_H__
#define __LSM_FIXED_H__

#include "LSSTDStandardLib.h"
using namespace ::lsstd;

#include <cassert>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * Use rounding for better accuracy?  Little slower.
 */
#define LSM_FIXED_ROUNDING

namespace lsm {

	/**
	 * Class CFixed
	 *
	 * Description: Fixed-point template.
	 */
	template <typename _tType, unsigned _uBits = 12>
	class CFixed {
	public :
		// This class allows us to define a type that is not meant to be modified
		//	when passed to the constructor of CFixed.
		class CFixedRawType {
		public :
			LSE_CALLCTOR						CFixedRawType( _tType _tVal ) :
				m_tValue( _tVal ) {}

			_tType								m_tValue;
		};

		// == Various constructors.
		LSE_CALLCTOR							CFixed() {
		}
		LSE_CALLCTOR							CFixed( const CFixedRawType &_frtRaw ) :
			m_tValue( _frtRaw.m_tValue ) {
		}
		LSE_CALLCTOR							CFixed( const CFixed<_tType, _uBits> &_tVal ) :
			m_tValue( _tVal.m_tValue ) {
		}
		LSE_CALLCTOR							CFixed( int8_t _i8Val ) :
			m_tValue( static_cast<_tType>(_i8Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( uint8_t _ui8Val ) :
			m_tValue( static_cast<_tType>(_ui8Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( int16_t _i16Val ) :
			m_tValue( static_cast<_tType>(_i16Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( uint16_t _ui16Val ) :
			m_tValue( static_cast<_tType>(_ui16Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( int32_t _i32Val ) :
			m_tValue( static_cast<_tType>(_i32Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( uint32_t _ui32Val ) :
			m_tValue( static_cast<_tType>(_ui32Val) << _uBits ) {
		}
		LSE_CALLCTOR							CFixed( int64_t _i64Val ) :
			m_tValue( static_cast<_tType>(_i64Val << _uBits) ) {
		}
		LSE_CALLCTOR							CFixed( uint64_t _ui64Val ) :
			m_tValue( static_cast<_tType>(_ui64Val << _uBits) ) {
		}
		LSE_CALLCTOR							CFixed( float _fVal ) :
			m_tValue( static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits)) ) {
		}
		LSE_CALLCTOR							CFixed( double _dVal ) :
			m_tValue( static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits)) ) {
		}

		// == Functions.
		/**
		 * Gets the number of bits in the fractional part.
		 *
		 * \return Returns the number of bits in the fractional part.
		 */
		static LSE_INLINE _tType LSE_FCALL		Bits()		{ return _uBits; }

		/**
		 * Gets 1 expressed as a fixed-point number.
		 *
		 * \return Returns 1 expressed as a fixed-point number.
		 */
		static LSE_INLINE _tType LSE_FCALL		One()		{ return (static_cast<_tType>(1) << _uBits); }

		/**
		 * Gets a bit mask that masks all bits but the sign bit.
		 *
		 * \return Returns a bit mask that masks all bits but the sign bit.
		 */
		static LSE_INLINE _tType LSE_FCALL		SignMask()	{
			return static_cast<_tType>(1) << (sizeof( _tType ) * 8 - 1);
		}

		/**
		 * Gets a bit mask that masks the sign bit.
		 *
		 * \return Returns a bit mask that masks the sign bit.
		 */
		static LSE_INLINE _tType LSE_FCALL		DigitMask()	{ return ~SignMask(); }

		/**
		 * Gets the reciprocal as float.
		 *
		 * \return Returns the reciprocal as float.
		 */
		static LSE_INLINE float LSE_FCALL		RecFloat()	{ return 1.0f / One(); }

		/**
		 * Gets the reciprocal as double.
		 *
		 * \return Returns the reciprocal as double.
		 */
		static LSE_INLINE double LSE_FCALL	RecDouble()	{ return 1.0 / One(); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _i8Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( int8_t _i8Val )	{ return static_cast<_tType>(_i8Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _ui8Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( uint8_t _ui8Val )	{ return static_cast<_tType>(_ui8Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _i16Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( int16_t _i16Val )	{ return static_cast<_tType>(_i16Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _ui16Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( uint16_t _ui16Val )	{ return static_cast<_tType>(_ui16Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _i32Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( int32_t _i32Val )	{ return static_cast<_tType>(_i32Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _ui32Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( uint32_t _ui32Val )	{ return static_cast<_tType>(_ui32Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _i64Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( int64_t _i64Val )	{ return static_cast<_tType>(_i64Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _ui64Val The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( uint64_t _ui64Val )	{ return static_cast<_tType>(_ui64Val); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _fVal The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( float _fVal )		{ return static_cast<_tType>(_fVal); }

		/**
		 * Cast a value to our base type (removes warnings when casting from higher-precision values
		 *	down to lower-precision values).
		 *
		 * \param _dVal The value to cast to the fixed-point type.
		 * \return Returns the cast value.
		 */
		static LSE_INLINE _tType LSE_FCALL		Cast( double _dVal )		{ return static_cast<_tType>(_dVal); }
		

		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _tVal The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( const CFixed<_tType, _uBits> &_tVal ) {
			m_tValue = _tVal.m_tValue;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _i8Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( int8_t _i8Val ) {
			m_tValue = static_cast<_tType>(_i8Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _ui8Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( uint8_t _ui8Val ) {
			m_tValue = static_cast<_tType>(_ui8Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _i16Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( int16_t _i16Val ) {
			m_tValue = static_cast<_tType>(_i16Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _ui16Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( uint16_t _ui16Val ) {
			m_tValue = static_cast<_tType>(_ui16Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _i32Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( int32_t _i32Val ) {
			m_tValue = static_cast<_tType>(_i32Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _ui32Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( uint32_t _ui32Val ) {
			m_tValue = static_cast<_tType>(_ui32Val) << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _i64Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( int64_t _i64Val ) {
			m_tValue = _i64Val << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _ui64Val The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( uint64_t _ui64Val ) {
			m_tValue = _ui64Val << _uBits;
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _fVal The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( float _fVal ) {
			m_tValue = static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Assignment operator.
		 *
		 * \param _dVal The value to assign to this fixed-point number.
		 * \return Returns this object after the assignment.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator = ( double _dVal ) {
			m_tValue += static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Addition.
		 *
		 * \param _tVal The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( const CFixed<_tType, _uBits> &_tVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + _tVal.m_tValue ) );
		}

		/**
		 * Addition.
		 *
		 * \param _i8Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( int8_t _i8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_i8Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _ui8Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( uint8_t _ui8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_ui8Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _i16Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( int16_t _i16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_i16Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _ui16Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( uint16_t _ui16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_ui16Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _i32Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( int32_t _i32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_i32Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _ui32Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( uint32_t _ui32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (static_cast<_tType>(_ui32Val) << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _i64Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( int64_t _i64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (_i64Val << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _ui64Val The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( uint64_t _ui64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + (_ui64Val << _uBits) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _fVal The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( float _fVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits)) ) );
		}

		/**
		 * Addition.
		 *
		 * \param _dVal The number to add to this number.
		 * \return Returns the sum of this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + ( double _dVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue + static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits)) ) );
		}

		/**
		 * Addition assignment.
		 *
		 * \param _tVal The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( const CFixed<_tType, _uBits> &_tVal ) {
			m_tValue += _tVal.m_tValue;
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _i8Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( int8_t _i8Val ) {
			m_tValue += (static_cast<_tType>(_i8Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _ui8Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( uint8_t _ui8Val ) {
			m_tValue += (static_cast<_tType>(_ui8Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _i16Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( int16_t _i16Val ) {
			m_tValue += (static_cast<_tType>(_i16Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _ui16Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( uint16_t _ui16Val ) {
			m_tValue += (static_cast<_tType>(_ui16Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _i32Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( int32_t _i32Val ) {
			m_tValue += (static_cast<_tType>(_i32Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _ui32Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( uint32_t _ui32Val ) {
			m_tValue += (static_cast<_tType>(_ui32Val) << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _i64Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( int64_t _i64Val ) {
			m_tValue += (_i64Val << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _ui64Val The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( uint64_t _ui64Val ) {
			m_tValue += (_ui64Val << _uBits);
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _fVal The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( float _fVal ) {
			m_tValue += static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Addition assignment.
		 *
		 * \param _dVal The number to add to this number.
		 * \return This number is increased by the given amount and returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator += ( double _dVal ) {
			m_tValue += static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Subtraction.
		 *
		 * \param _tVal The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( const CFixed<_tType, _uBits> &_tVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - _tVal.m_tValue ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _i8Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( int8_t _i8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_i8Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _ui8Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( uint8_t _ui8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_ui8Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _i16Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( int16_t _i16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_i16Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _ui16Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( uint16_t _ui16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_ui16Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _i32Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( int32_t _i32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_i32Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _ui32Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( uint32_t _ui32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (static_cast<_tType>(_ui32Val) << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _i64Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( int64_t _i64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (_i64Val << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _ui64Val The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( uint64_t _ui64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - (_ui64Val << _uBits) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _fVal The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( float _fVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits)) ) );
		}

		/**
		 * Subtraction.
		 *
		 * \param _dVal The value to subtract from this number.
		 * \return Returns difference between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - ( double _dVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue - static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits)) ) );
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _tVal The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( const CFixed<_tType, _uBits> &_tVal ) {
			m_tValue -= _tVal.m_tValue;
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _i8Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( int8_t _i8Val ) {
			m_tValue -= (static_cast<_tType>(_i8Val) << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _ui8Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( uint8_t _ui8Val ) {
			m_tValue -= (static_cast<_tType>(_ui8Val) << _uBits);
			return (*this);
		}
		
		
		/**
		 * Subtract and assign.
		 *
		 * \param _i16Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( int16_t _i16Val ) {
			m_tValue -= (static_cast<_tType>(_i16Val) << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _ui16Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( uint16_t _ui16Val ) {
			m_tValue -= (static_cast<_tType>(_ui16Val) << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _i32Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( int32_t _i32Val ) {
			m_tValue -= (static_cast<_tType>(_i32Val) << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _ui32Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( uint32_t _ui32Val ) {
			m_tValue -= (static_cast<_tType>(_ui32Val) << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _i64Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( int64_t _i64Val ) {
			m_tValue -= (_i64Val << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _ui64Val The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( uint64_t _ui64Val ) {
			m_tValue -= (_ui64Val << _uBits);
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _fVal The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( float _fVal ) {
			m_tValue -= static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Subtract and assign.
		 *
		 * \param _dVal The value to subtract from this number.
		 * \return Reduces this number by the given amount and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator -= ( double _dVal ) {
			m_tValue -= static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
			return (*this);
		}

		/**
		 * Multiplication.
		 *
		 * \param _tVal Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( const CFixed<_tType, _uBits> &_tVal ) const {
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) * _tVal.m_tValue);
			i64Interm += i64Interm & (1LL << (_uBits - 1));
#ifdef _DEBUG
			uint32_t ui32Final = HighestBit( i64Interm );
			if ( ui32Final >= (sizeof( _tType ) * 8) + _uBits ) {
				assert( 0 );
			}
#endif	// #ifdef _DEBUG
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(i64Interm >> _uBits) ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _i8Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( int8_t _i8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _i8Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _ui8Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( uint8_t _ui8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _ui8Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _i16Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( int16_t _i16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _i16Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _ui16Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( uint16_t _ui16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _ui16Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _i32Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( int32_t _i32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _i32Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _ui32Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( uint32_t _ui32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _ui32Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _i64Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( int64_t _i64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue * _i64Val ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _ui64Val Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( uint64_t _ui64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue * _ui64Val) ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _fVal Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( float _fVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue * _fVal) ) );
		}

		/**
		 * Multiplication.
		 *
		 * \param _dVal Value by which to multiply this number.
		 * \return Returns the result of multiplication between this number and the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator * ( double _dVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue * _dVal) ) );
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _tVal Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( const CFixed<_tType, _uBits> &_tVal ) {
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) * _tVal.m_tValue);
			i64Interm += i64Interm & (1LL << (_uBits - 1));
#ifdef _DEBUG
			uint32_t ui32Final = HighestBit( i64Interm );
			if ( ui32Final >= (sizeof( _tType ) * 8) + _uBits ) {
				assert( 0 );
			}
#endif	// #ifdef _DEBUG
			m_tValue = static_cast<_tType>(i64Interm >> _uBits);
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _i8Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( int8_t _i8Val ) {
			m_tValue = m_tValue * _i8Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _ui8Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( uint8_t _ui8Val ) {
			m_tValue = m_tValue * _ui8Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _i16Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( int16_t _i16Val ) {
			m_tValue = m_tValue * _i16Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _ui16Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( uint16_t _ui16Val ) {
			m_tValue = m_tValue * _ui16Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _i32Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( int32_t _i32Val ) {
			m_tValue = m_tValue * _i32Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _ui32Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( uint32_t _ui32Val ) {
			m_tValue = m_tValue * _ui32Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _i64Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( int64_t _i64Val ) {
			m_tValue = m_tValue * _i64Val;
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _ui64Val Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( uint64_t _ui64Val ) {
			m_tValue = static_cast<_tType>(m_tValue * _ui64Val);
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _fVal Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( float _fVal ) {
			m_tValue = static_cast<_tType>(m_tValue * _fVal);
			return (*this);
		}

		/**
		 * Multiply and assign.
		 *
		 * \param _dVal Value by which to multiply this number.
		 * \return This number is multiplied by the given number and then returned.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator *= ( double _dVal ) {
			m_tValue = static_cast<_tType>(m_tValue * _dVal);
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _tVal Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( const CFixed<_tType, _uBits> &_tVal ) const {
#ifdef LSM_FIXED_ROUNDING
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) << (_uBits + 1));
			i64Interm /= _tVal.m_tValue;
			// If the bottom bit is set, add 1 to the final result.
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>((i64Interm >> 1) + (i64Interm & 1)) ) );
#else
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) << _uBits);
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(i64Interm / _tVal.m_tValue) ) );
#endif	// #ifdef LSM_FIXED_ROUNDING
		}

		/**
		 * Division.
		 *
		 * \param _i8Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( int8_t _i8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _i8Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _ui8Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( uint8_t _ui8Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _ui8Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _i16Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( int16_t _i16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _i16Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _ui16Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( uint16_t _ui16Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _ui16Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _i32Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( int32_t _i32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _i32Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _ui32Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( uint32_t _ui32Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _ui32Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _i64Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( int64_t _i64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue / _i64Val ) );
		}

		/**
		 * Division.
		 *
		 * \param _ui64Val Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( uint64_t _ui64Val ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue / _ui64Val) ) );
		}

		/**
		 * Division.
		 *
		 * \param _fVal Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( float _fVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue / _fVal) ) );
		}

		/**
		 * Division.
		 *
		 * \param _dVal Value by which to divide this number.
		 * \return Returns the quotient of this number divided by the given number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator / ( double _dVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( static_cast<_tType>(m_tValue / _dVal) ) );
		}

		/**
		 * Division.
		 *
		 * \param _tVal Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( const CFixed<_tType, _uBits> &_tVal ) {
#ifdef LSM_FIXED_ROUNDING
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) << (_uBits + 1));
			i64Interm /= _tVal.m_tValue;
			// If the bottom bit is set, add 1 to the final result.
			m_tValue = static_cast<_tType>((i64Interm >> 1) + (i64Interm & 1));
#else
			register int64_t i64Interm = (static_cast<int64_t>(m_tValue) << _uBits);
			m_tValue = static_cast<_tType>(i64Interm / _tVal.m_tValue);
#endif	// #ifdef LSM_FIXED_ROUNDING
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _i8Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( int8_t _i8Val ) {
			m_tValue = m_tValue / _i8Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _ui8Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( uint8_t _ui8Val ) {
			m_tValue = m_tValue / _ui8Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _i16Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( int16_t _i16Val ) {
			m_tValue = m_tValue / _i16Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _ui16Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( uint16_t _ui16Val ) {
			m_tValue = m_tValue / _ui16Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _i32Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( int32_t _i32Val ) {
			m_tValue = m_tValue / _i32Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _ui32Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( uint32_t _ui32Val ) {
			m_tValue = m_tValue / _ui32Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _i64Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( int64_t _i64Val ) {
			m_tValue = m_tValue / _i64Val;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _ui64Val Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( uint64_t _ui64Val ) {
			m_tValue = static_cast<_tType>(m_tValue / _ui64Val);
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _fVal Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( float _fVal ) {
			m_tValue /= _fVal;
			return (*this);
		}

		/**
		 * Division.
		 *
		 * \param _dVal Value by which to divide this number.
		 * \return Divides this number by the given number and then returns this number.
		 */
		LSE_INLINE CFixed & LSE_FCALL			operator /= ( double _dVal ) {
			m_tValue /= _dVal;
			return (*this);
		}


		// Modulus.
		/*LSE_INLINE CFixed LSE_FCALL				operator % ( const CFixed<_tType, _uBits> &_tVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue % _tVal.m_tValue ) );
		}
		LSE_INLINE CFixed LSE_FCALL				operator % ( const _tType &_tVal ) const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue % (_tVal << _uBits) ) );
		}*/

		/**
		 * Unary negation.
		 *
		 * \return Returns the inverse of this number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator - () const {
			return CFixed<_tType, _uBits>( CFixedRawType( -m_tValue ) );
		}

		/**
		 * Unary positive.
		 *
		 * \return Returns this number.
		 */
		LSE_INLINE CFixed LSE_FCALL				operator + () const {
			return CFixed<_tType, _uBits>( CFixedRawType( m_tValue ) );
		}

		/**
		 * Equality.
		 *
		 * \param _tVal Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue == _tVal.m_tValue;
		}

		/**
		 * Equality.
		 *
		 * \param _i8Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( int8_t _i8Val ) const {
			return m_tValue == (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _ui8Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( uint8_t _ui8Val ) const {
			return m_tValue == (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _i16Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( int16_t _i16Val ) const {
			return m_tValue == (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _ui16Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( uint16_t _ui16Val ) const {
			return m_tValue == (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _i32Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( int32_t _i32Val ) const {
			return m_tValue == (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _ui32Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( uint32_t _ui32Val ) const {
			return m_tValue == (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _i64Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( int64_t _i64Val ) const {
			return m_tValue == (_i64Val << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _ui64Val Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( uint64_t _ui64Val ) const {
			return m_tValue == (_ui64Val << _uBits);
		}

		/**
		 * Equality.
		 *
		 * \param _fVal Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( float _fVal ) const {
			return m_tValue == static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Equality.
		 *
		 * \param _dVal Value against which to check for equality.
		 * \return Returns true if both values, cast to a fixed-point value, are equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( double _dVal ) const {
			return m_tValue == static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Inequality.
		 *
		 * \param _tVal Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue != _tVal.m_tValue;
		}

		/**
		 * Inequality.
		 *
		 * \param _i8Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( int8_t _i8Val ) const {
			return m_tValue != (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _ui8Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( uint8_t _ui8Val ) const {
			return m_tValue != (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _i16Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( int16_t _i16Val ) const {
			return m_tValue != (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _ui16Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( uint16_t _ui16Val ) const {
			return m_tValue != (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _i32Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( int32_t _i32Val ) const {
			return m_tValue != (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _ui32Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( uint32_t _ui32Val ) const {
			return m_tValue != (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _i64Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( int64_t _i64Val ) const {
			return m_tValue != (_i64Val << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _ui64Val Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( uint64_t _ui64Val ) const {
			return m_tValue != (_ui64Val << _uBits);
		}

		/**
		 * Inequality.
		 *
		 * \param _fVal Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( float _fVal ) const {
			return m_tValue != static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Inequality.
		 *
		 * \param _dVal Value against which to check for inequality.
		 * \return Returns true if both values, cast to a fixed-point value, are not equal.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( double _dVal ) const {
			return m_tValue != static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _tVal Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue > _tVal.m_tValue;
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _i8Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( int8_t _i8Val ) const {
			return m_tValue > (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _ui8Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( uint8_t _ui8Val ) const {
			return m_tValue > (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _i16Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( int16_t _i16Val ) const {
			return m_tValue > (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _ui16Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( uint16_t _ui16Val ) const {
			return m_tValue > (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _i32Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( int32_t _i32Val ) const {
			return m_tValue > (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _ui32Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( uint32_t _ui32Val ) const {
			return m_tValue > (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _i64Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( int64_t _i64Val ) const {
			return m_tValue > (_i64Val << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _ui64Val Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( uint64_t _ui64Val ) const {
			return m_tValue > (_ui64Val << _uBits);
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _fVal Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( float _fVal ) const {
			return m_tValue > static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Greater-than comparison.
		 *
		 * \param _dVal Value against which to check.
		 * \return Returns true if this value is greater than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator > ( double _dVal ) const {
			return m_tValue > static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _tVal Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue >= _tVal.m_tValue;
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _i8Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( int8_t _i8Val ) const {
			return m_tValue >= (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _ui8Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( uint8_t _ui8Val ) const {
			return m_tValue >= (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _i16Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( int16_t _i16Val ) const {
			return m_tValue >= (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _ui16Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( uint16_t _ui16Val ) const {
			return m_tValue >= (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _i32Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( int32_t _i32Val ) const {
			return m_tValue >= (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _ui32Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( uint32_t _ui32Val ) const {
			return m_tValue >= (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _i64Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( int64_t _i64Val ) const {
			return m_tValue >= (_i64Val << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _ui64Val Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( uint64_t _ui64Val ) const {
			return m_tValue >= (_ui64Val << _uBits);
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _fVal Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( float _fVal ) const {
			return m_tValue >= static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Greater-than or equal-to comparison.
		 *
		 * \param _dVal Value against which to check.
		 * \return Returns true if this value is greater than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator >= ( double _dVal ) const {
			return m_tValue >= static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _tVal Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue < _tVal.m_tValue;
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _i8Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( int8_t _i8Val ) const {
			return m_tValue < (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _ui8Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( uint8_t _ui8Val ) const {
			return m_tValue < (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _i16Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( int16_t _i16Val ) const {
			return m_tValue < (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _ui16Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( uint16_t _ui16Val ) const {
			return m_tValue < (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _i32Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( int32_t _i32Val ) const {
			return m_tValue < (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _ui32Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( uint32_t _ui32Val ) const {
			return m_tValue < (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _i64Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( int64_t _i64Val ) const {
			return m_tValue < (_i64Val << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _ui64Val Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( uint64_t _ui64Val ) const {
			return m_tValue < (_ui64Val << _uBits);
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _fVal Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( float _fVal ) const {
			return m_tValue < static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Less-than comparison.
		 *
		 * \param _dVal Value against which to check.
		 * \return Returns true if this value is less than the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( double _dVal ) const {
			return m_tValue < static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _tVal Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( const CFixed<_tType, _uBits> &_tVal ) const {
			return m_tValue <= _tVal.m_tValue;
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _i8Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( int8_t _i8Val ) const {
			return m_tValue <= (static_cast<_tType>(_i8Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _ui8Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( uint8_t _ui8Val ) const {
			return m_tValue <= (static_cast<_tType>(_ui8Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _i16Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( int16_t _i16Val ) const {
			return m_tValue <= (static_cast<_tType>(_i16Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _ui16Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( uint16_t _ui16Val ) const {
			return m_tValue <= (static_cast<_tType>(_ui16Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _i32Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( int32_t _i32Val ) const {
			return m_tValue <= (static_cast<_tType>(_i32Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _ui32Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( uint32_t _ui32Val ) const {
			return m_tValue <= (static_cast<_tType>(_ui32Val) << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _i64Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( int64_t _i64Val ) const {
			return m_tValue <= (_i64Val << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _ui64Val Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( uint64_t _ui64Val ) const {
			return m_tValue <= (_ui64Val << _uBits);
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _fVal Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( float _fVal ) const {
			return m_tValue <= static_cast<_tType>(_fVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Less-than or equal-to comparison.
		 *
		 * \param _dVal Value against which to check.
		 * \return Returns true if this value is Less than or equal to the given value.
		 */
		LSE_INLINE bool LSE_FCALL				operator <= ( double _dVal ) const {
			return m_tValue <= static_cast<_tType>(_dVal * (static_cast<_tType>(1) << _uBits));
		}

		/**
		 * Casting to int8_t.
		 *
		 * \return Returns this value cast to int8_t.
		 */
		LSE_INLINE LSE_FCALL operator 			int8_t() const {
			return static_cast<int8_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to int16_t.
		 *
		 * \return Returns this value cast to int16_t.
		 */
		LSE_INLINE LSE_FCALL operator 			int16_t() const {
			return static_cast<int16_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to int32_t.
		 *
		 * \return Returns this value cast to int32_t.
		 */
		LSE_INLINE LSE_FCALL operator 			int32_t() const {
			return static_cast<int32_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to int64_t.
		 *
		 * \return Returns this value cast to int64_t.
		 */
		LSE_INLINE LSE_FCALL operator 			int64_t() const {
			return static_cast<int64_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to float.
		 *
		 * \return Returns this value cast to float.
		 */
		LSE_INLINE LSE_FCALL operator 			float() const {
			return RecFloat() * m_tValue;
		}

		/**
		 * Casting to double.
		 *
		 * \return Returns this value cast to double.
		 */
		LSE_INLINE LSE_FCALL operator 			double() const {
			return RecDouble() * m_tValue;
		}

		/**
		 * Casting to bool.
		 *
		 * \return Returns this value cast to bool.
		 */
		LSE_INLINE LSE_FCALL operator 			bool() const {
			return m_tValue != 0;
		}

		/**
		 * Casting to uint8_t.
		 *
		 * \return Returns this value cast to uint8_t.
		 */
		LSE_INLINE LSE_FCALL operator 			uint8_t() const {
			return static_cast<uint8_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to uint16_t.
		 *
		 * \return Returns this value cast to uint16_t.
		 */
		LSE_INLINE LSE_FCALL operator 			uint16_t() const {
			return static_cast<uint16_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to uint32_t.
		 *
		 * \return Returns this value cast to uint32_t.
		 */
		LSE_INLINE LSE_FCALL operator 			uint32_t() const {
			return static_cast<uint32_t>(m_tValue >> _uBits);
		}

		/**
		 * Casting to uint64_t.
		 *
		 * \return Returns this value cast to uint64_t.
		 */
		LSE_INLINE LSE_FCALL operator 			uint64_t() const {
			return static_cast<uint64_t>(m_tValue >> _uBits);
		}

		/**
		 * Returns the raw value for this fixed-point instance.
		 *
		 * \return Returns the raw value for this fixed-point instance.
		 */
		LSE_INLINE _tType LSE_FCALL				GetRawValue() const {
			return m_tValue;
		}

	protected :
		// == Members.
		// Our actual value.
		_tType									m_tValue;

		
		// == Functions.
		/**
		 * Gets the highest bit set in a raw value.
		 *
		 * \param _tValue The value whose highest unsigned bit is to be checked.
		 *	If the value is negative, the highest bit set is based off its positive counterpart.
		 * \return Returns the index of the highest bit set in the value.
		 */
		static uint32_t LSE_FCALL				HighestBit( int64_t _i64Value ) {
			if ( _i64Value < 0LL ) {
				_i64Value = -_i64Value;
			}
#ifdef LSE_X86
			// On x86 processors there is an instruction that gets the highest-
			//	set bit automatically.
			uint32_t ui32Ret;
			int32_t i32High = static_cast<int32_t>(_i64Value >> 32ULL);
			LSE_ASM_BEGIN
				mov eax, 0
				bsr eax, i32High
				mov ui32Ret, eax
			LSE_ASM_END
			if ( ui32Ret ) {
				return ui32Ret + 32;
			}

			i32High = static_cast<int32_t>(_i64Value & 0xFFFFFFFFULL);
			LSE_ASM_BEGIN
				mov eax, 0
				bsr eax, i32High
				mov ui32Ret, eax
			LSE_ASM_END
			return ui32Ret;
#else	// LSE_X86
			// Get it the hard way.
			uint32_t ui32Ret = (sizeof( _i64Value ) << 3);
			for ( ; ui32Ret--;  ) {
				if ( _i64Value & (1LL << ui32Ret) ) { return ui32Ret; }
			}
			return 0;
#endif	// LSE_X86
		}
	};

}	// namespace lsm


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// GLOBAL OPERATORS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * Addition.
 *
 * \param _i8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( int8_t _i8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _i8Val;
}

/**
 * Addition.
 *
 * \param _ui8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( uint8_t _ui8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _ui8Val;
}

/**
 * Addition.
 *
 * \param _i16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( int16_t _i16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _i16Val;
}

/**
 * Addition.
 *
 * \param _ui16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( uint16_t _ui16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _ui16Val;
}

/**
 * Addition.
 *
 * \param _i32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( int32_t _i32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _i32Val;
}

/**
 * Addition.
 *
 * \param _ui32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( uint32_t _ui32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _ui32Val;
}

/**
 * Addition.
 *
 * \param _i64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( int64_t _i64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _i64Val;
}

/**
 * Addition.
 *
 * \param _ui64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( uint64_t _ui64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _ui64Val;
}

/**
 * Addition.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _fVal;
}

/**
 * Addition.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the sum of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator + ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal + _dVal;
}

/**
 * Subtraction.
 *
 * \param _i8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( int8_t _i8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i8Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _ui8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( uint8_t _ui8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui8Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _i16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( int16_t _i16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i16Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _ui16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( uint16_t _ui16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui16Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _i32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( int32_t _i32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i32Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _ui32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( uint32_t _ui32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui32Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _i64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( int64_t _i64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i64Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _ui64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( uint64_t _ui64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui64Val ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) - _tVal;
}

/**
 * Subtraction.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the difference between the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator - ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) - _tVal;
}

/**
 * Multiplication.
 *
 * \param _i8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( int8_t _i8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _i8Val;
}

/**
 * Multiplication.
 *
 * \param _ui8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( uint8_t _ui8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _ui8Val;
}

/**
 * Multiplication.
 *
 * \param _i16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( int16_t _i16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _i16Val;
}

/**
 * Multiplication.
 *
 * \param _ui16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( uint16_t _ui16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _ui16Val;
}

/**
 * Multiplication.
 *
 * \param _i32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( int32_t _i32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _i32Val;
}

/**
 * Multiplication.
 *
 * \param _ui32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( uint32_t _ui32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _ui32Val;
}

/**
 * Multiplication.
 *
 * \param _i64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( int64_t _i64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _i64Val;
}

/**
 * Multiplication.
 *
 * \param _ui64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( uint64_t _ui64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _ui64Val;
}

/**
 * Multiplication.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _fVal;
}

/**
 * Multiplication.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator * ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal * _dVal;
}

/**
 * Division.
 *
 * \param _i8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( int8_t _i8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i8Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _ui8Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( uint8_t _ui8Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui8Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _i16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( int16_t _i16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i16Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _ui16Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( uint16_t _ui16Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui16Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _i32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( int32_t _i32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i32Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _ui32Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( uint32_t _ui32Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui32Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _i64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( int64_t _i64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _i64Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _ui64Val Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( uint64_t _ui64Val, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _ui64Val ) / _tVal;
}

/**
 * Division.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) / _tVal;
}

/**
 * Division.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns the product of the given numbers.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE lsm::CFixed<_tType, _uBits> operator / ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) / _tVal;
}

/**
 * Equality.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the values are equal.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator == ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal == _fVal;
}

/**
 * Equality.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the values are equal.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator == ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal == _dVal;
}

/**
 * Inequality.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the values are not equal.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator != ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal != _fVal;
}

/**
 * Inequality.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the values are not equal.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator != ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return _tVal != _dVal;
}

/**
 * Greater-than comparison.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is greater than the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator > ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) > _tVal;
}

/**
 * Greater-than comparison.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is greater than the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator > ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) > _tVal;
}

/**
 * Greater-than or equal-to comparison.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is greater than or equal to the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator >= ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) >= _tVal;
}

/**
 * Greater-than or equal-to comparison.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is greater than or equal to the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator >= ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) >= _tVal;
}

/**
 * Less-than or comparison.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is less than the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator < ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) < _tVal;
}

/**
 * Less-than or comparison.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is less than the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator < ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) < _tVal;
}

/**
 * Less-than or equal-to comparison.
 *
 * \param _fVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is less than or equal to the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator <= ( float _fVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _fVal ) <= _tVal;
}

/**
 * Less-than or equal-to comparison.
 *
 * \param _dVal Left operand.
 * \param _tVal Right operand.
 * \return Returns true if the left value is less than or equal to the right.
 */
template <typename _tType, unsigned _uBits>
LSE_INLINE bool operator <= ( double _dVal, lsm::CFixed<_tType, _uBits> _tVal ) {
	return lsm::CFixed<_tType, _uBits>( _dVal ) <= _tVal;
}

#endif	// __LSM_FIXED_H__
