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
 * Description: A template for any type of 3D vector.
 */


#ifndef __LSM_VECTOR3BASE_H__
#define __LSM_VECTOR3BASE_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CVector3Base
	 * \brief A template for any type of 3D vector.
	 *
	 * Description: A template for any type of 3D vector.
	 */
	template <typename _tType, typename _tInher>
	class CVector3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVector3Base() {
		}
		LSE_CALLCTOR							CVector3Base( const _tInher &_v3bOther ) :
			x( _v3bOther.x ),
			y( _v3bOther.y ),
			z( _v3bOther.z ) {
		}
		LSE_CALLCTOR							CVector3Base( _tType _fX, _tType _fY, _tType _fZ ) :
			x( _fX ),
			y( _fY ),
			z( _fZ ) {
		}
		LSE_CALLCTOR							CVector3Base( _tType * _pfValues ) :
			x( _pfValues[0] ),
			y( _pfValues[1] ),
			z( _pfValues[2] ) {
		}

		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		_tType LSE_FCALL						operator [] ( uint32_t _ui32Index ) const {
			return reinterpret_cast<const _tType *>(this)[_ui32Index];
		}

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		_tType & LSE_FCALL						operator [] ( uint32_t _ui32Index ) {
			return reinterpret_cast<_tType *>(this)[_ui32Index];
		}

		/**
		 * Assignment.
		 *
		 * \param _v3bOther The vector to copy.
		 * \return Returns the copied vector.
		 */
		_tInher & LSE_FCALL						operator = ( const _tInher &_v3bOther ) {
			x = _v3bOther.x;
			y = _v3bOther.y;
			z = _v3bOther.z;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Equality.
		 *
		 * \param _v3bOther The vector to check for equality.
		 * \return Returns true if both vectors are equal.
		 */
		LSBOOL LSE_FCALL						operator == ( const _tInher &_v3bOther ) const {
			return x == _v3bOther.x && y == _v3bOther.y && z == _v3bOther.z;
		}

		/**
		 * Inequality.
		 *
		 * \param _v3bOther The vector to check for inequality.
		 * \return Returns true if both vectors are not equal.
		 */
		LSBOOL LSE_FCALL						operator != ( const _tInher &_v3bOther ) const {
			return x != _v3bOther.x || y != _v3bOther.y || z != _v3bOther.z;
		}

		/**
		 * Unary -.
		 *
		 * \return Returns a copy of this vector with each axis negated.
		 */
		_tInher LSE_FCALL						operator - () const {
			return _tInher( -x, -y, -z );
		}

		/**
		 * Binary addition.
		 *
		 * \param _v3bOther The vector to add to this one componentwise.
		 * \return Returns a new vector representing the addition.
		 */
		_tInher LSE_FCALL						operator + ( const _tInher &_v3bOther ) const {
			return _tInher( x + _v3bOther.x, y + _v3bOther.y, z + _v3bOther.z );
		}

		/**
		 * Binary subtraction.
		 *
		 * \param _v3bOther The vector to subtract from this one componentwise.
		 * \return Returns a new vector representing the difference.
		 */
		_tInher LSE_FCALL						operator - ( const _tInher &_v3bOther ) const {
			return _tInher( x - _v3bOther.x, y - _v3bOther.y, z - _v3bOther.z );
		}

		/**
		 * Binary multiplication.
		 *
		 * \param _tValue The amount by which to multiply each component in this vector.
		 * \return Returns a new vector representing the product.
		 */
		_tInher LSE_FCALL						operator * ( _tType _tValue ) const {
			return _tInher( x * _tValue, y * _tValue, z * _tValue );
		}

		/**
		 * Binary division.
		 *
		 * \param _tValue The amount by which to divide each component in this vector.
		 * \return Returns a new vector representing the quotient.
		 */
		_tInher LSE_FCALL						operator / ( _tType _tValue ) const {
			return (*static_cast<const _tInher *>(this)) * (_tType( 1 ) / _tValue);
		}

		/**
		 * Adds a vector to this vector, storing the result in this vector.
		 *
		 * \param _v3bOther The vector to add to this vector.
		 * \return Returns this vector, after the addition operation has been applied.
		 */
		_tInher & LSE_FCALL						operator += ( const _tInher &_v3bOther ) {
			x += _v3bOther.x;
			y += _v3bOther.y;
			z += _v3bOther.z;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Adds a scalar to this vector, storing the result in this vector.
		 *
		 * \param _tValue The amount to add to this vector.
		 * \return Returns this vector, after the addition operation has been applied.
		 */
		_tInher & LSE_FCALL						operator += ( _tType _tValue ) {
			x += _tValue;
			y += _tValue;
			z += _tValue;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Subtract a vector from this vector, storing the result in this vector.
		 *
		 * \param _v3bOther The vector to subtract from this vector.
		 * \return Returns this vector, after the subtraction operation has been applied.
		 */
		_tInher & LSE_FCALL						operator -= ( const _tInher &_v3bOther ) {
			x -= _v3bOther.x;
			y -= _v3bOther.y;
			z -= _v3bOther.z;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Subtracts a scalar from this vector, storing the result in this vector.
		 *
		 * \param _tValue The amount to subtract from this vector.
		 * \return Returns this vector, after the subtraction operation has been applied.
		 */
		_tInher & LSE_FCALL						operator -= ( _tType _tValue ) {
			x -= _tValue;
			y -= _tValue;
			z -= _tValue;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Multiply the vector by a given amount, storing the result in this vector.
		 *
		 * \param _tValue The amount by which to multiply each component in this vector.
		 * \return Returns this vector, after the muliplication operation has been applied.
		 */
		_tInher & LSE_FCALL						operator *= ( _tType _tValue ) {
			x *= _tValue;
			y *= _tValue;
			z *= _tValue;
			return (*static_cast<_tInher *>(this));
		}

		/**
		 * Divide the vector by a given amount, storing the result in this vector.
		 *
		 * \param _tValue The amount by which to divide each component in this vector.
		 * \return Returns this vector, after the division operation has been applied.
		 */
		_tInher & LSE_FCALL						operator /= ( _tType _tValue ) {
			return (*static_cast<_tInher *>(this)) *= (_tType( 1 ) / _tValue);
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v3bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		_tType LSE_FCALL						operator * ( const _tInher &_v3bOther ) const {
			return x * _v3bOther.x + y * _v3bOther.y + z * _v3bOther.z;
		}

		/**
		 * Performs a cross-product operation.  The vector produced is perpendicular to both of the vectors being multiplied
		 *	and normal to the plane containing them.
		 *
		 * \param _v3bOther The vector against which the cross product is to be determined.
		 * \return Returns the cross product between the two vectors.
		 */
		_tInher LSE_FCALL						operator % ( const _tInher &_v3bOther ) const {
			return _tInher( y * _v3bOther.z - z * _v3bOther.y,
				z * _v3bOther.x - x * _v3bOther.z,
				x * _v3bOther.y - y * _v3bOther.x );
		}


		// == Functions.
		/**
		 * Apply the given values to each of the axes on this vector.
		 *
		 * \param _fX The new X-axis value.
		 * \param _fY The new Y-axis value.
		 * \param _fZ The new Z-axis value.
		 */
		void LSE_FCALL						Set( _tType _fX, _tType _fY, _tType _fZ ) {
			x = _fX;
			y = _fY;
			z = _fZ;
		}

		/**
		 * Sets all axes on this vector to 0.
		 */
		void LSE_FCALL						Clear() {
			x = y = z = _tType( 0 );
		}

		/**
		 * Negates each axis on this vector.
		 */
		void LSE_FCALL						Invert() {
			x = -x;
			y = -y;
			z = -z;
		}

		/**
		 * Calculates the length of the vector.
		 *
		 * \return Returns the length of the vector.
		 */
		_tType LSE_FCALL						Len() const {
			return static_cast<_tType>(::sqrt( static_cast<double>(x * x + y * y + z * z) ));
		}

		/**
		 * Calculates the squared length of the vector.
		 *
		 * \return Returns the squared length of the vector.
		 */
		_tType LSE_FCALL						LenSq() const {
			return x * x + y * y + z * z;
		}

		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		void LSE_FCALL						Normalize() {
			_tType fInvLen = static_cast<_tType>(1.0 / ::sqrt( static_cast<double>(x * x + y * y + z * z) ));
			x *= fInvLen;
			y *= fInvLen;
			z *= fInvLen;
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v3bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		_tType LSE_FCALL						Dot( const _tInher &_v3bOther ) const {
			return x * _v3bOther.x + y * _v3bOther.y + z * _v3bOther.z;
		}

		/**
		 * Performs a cross-product operation.  The vector produced is perpendicular to both of the vectors being multiplied
		 *	and normal to the plane containing them.
		 *
		 * \param _v3bOther The vector against which the cross product is to be determined.
		 * \return Returns the cross product between the two vectors.
		 */
		_tInher LSE_FCALL						Cross( const _tInher &_v3bOther ) const {
			return _tInher( y * _v3bOther.z - z * _v3bOther.y,
				z * _v3bOther.x - x * _v3bOther.z,
				x * _v3bOther.y - y * _v3bOther.x );
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v3bLeft The left operand.
		 * \param _v3bRight The right operand.
		 * \return Returns the dot product between the two vectors.
		 */
		static _tType LSE_FCALL					DotProduct( const _tInher &_v3bLeft, const _tInher &_v3bRight ) {
			return _v3bLeft.x * _v3bRight.x + _v3bLeft.y * _v3bRight.y + _v3bLeft.z * _v3bRight.z;
		}

		/**
		 * Performs a cross-product operation.  The vector produced is perpendicular to both of the vectors being multiplied
		 *	and normal to the plane containing them.
		 *
		 * \param _v3bLeft The left operand.
		 * \param _v3bRight The right operand.
		 * \return Returns the cross product between the two vectors.
		 */
		static _tInher LSE_FCALL				CrossProduct( const _tInher &_v3bLeft, const _tInher &_v3bRight ) {
			return _tInher( _v3bLeft.y * _v3bRight.z - _v3bLeft.z * _v3bRight.y,
				_v3bLeft.z * _v3bRight.x - _v3bLeft.x * _v3bRight.z,
				_v3bLeft.x * _v3bRight.y - _v3bLeft.y * _v3bRight.x );
		}

		/**
		 * Linearly interpolate between two vectors, returning the resulting vector.
		 *
		 * \param _v3bLeft Left operand.
		 * \param _v3bRight Right operand.
		 * \param _fFrac Amount by which to interpolate between the vectors.
		 * \return Returns the vector interpolated between _v3bLeft and _v3bRight by _fFrac.
		 */
		static _tInher LSE_FCALL				Lerp( const _tInher &_v3bLeft, const _tInher &_v3bRight, _tType _fFrac ) {
			return ((_v3bRight - _v3bLeft) * _fFrac) + _v3bLeft;
		}

		/**
		 * Perform a Hermite spline interpolation between the supplied vectors.
		 *
		 * \param _v3bLeft Left operand.
		 * \param _v3bT0 Left tangent.
		 * \param _v3bRight Right operand.
		 * \param _v3bT1 Right tangent.
		 * \param _fFrac Amount by which to interpolate between the vectors.
		 * \return Returns the vector interpolated between _v3bLeft and _v3bRight by _fFrac.
		 */
		static _tInher LSE_FCALL				Hermite( const _tInher &_v3bLeft, const _tInher &_v3bT0,
			const _tInher &_v3bRight, const _tInher &_v3bT1,
			_tType _fFrac ) {
			_tType fS2	= _fFrac * _fFrac;
			_tType fS3	= fS2 * _fFrac;
			_tType f2s2	= static_cast<_tType>(2.0) * fS2;
			_tType f2s3	= static_cast<_tType>(2.0) * fS3;
			_tType f3s2	= static_cast<_tType>(3.0) * fS2;

			return (_v3bLeft * (f2s3 - f3s2 + LSM_ONE)) +
				(_v3bRight * (-f2s3 + f3s2)) +
				(_v3bT0 * (fS3 - f2s2 + _fFrac)) +
				(_v3bT1 * (fS3 - fS2));
		}

		/**
		 * Perform a Catmull-Rom spline interpolation between the supplied vectors
		 *
		 * \param _v3bV0 Input point 0.
		 * \param _v3bV1 Input point 1.
		 * \param _v3bV2 Input point 2.
		 * \param _v3bV3 Input point 3.
		 * \param _fFrac Amount by which to interpolate between _v3bV1 and _v3bV2.
		 * \return Returns the vector interpolated between _v3bV1 and _v3bV2 by _fFrac.
		 */
		static _tInher LSE_FCALL				CatmullRom( const _tInher &_v3bV0, const _tInher &_v3bV1,
			const _tInher &_v3bV2, const _tInher &_v3bV3,
			_tType _fFrac ) {
			_tType fS2	= _fFrac * _fFrac;
			return ((_v3bV1 * static_cast<_tType>(2.0)) +
				(-_v3bV0 + _v3bV2) * _fFrac +
				(_v3bV0 * static_cast<_tType>(2.0) - _v3bV1 * static_cast<_tType>(5.0) + _v3bV2 * static_cast<_tType>(4.0) - _v3bV3) * fS2 +
				(-_v3bV0 + _v3bV1 * static_cast<_tType>(3.0) - _v3bV2 * static_cast<_tType>(3.0) + _v3bV3) * fS2 * _fFrac) * LSM_HALF;
		}


		// == Members.
#pragma pack( push, 1 )
		/**
		 * The X value.
		 */
		_tType									x;

		/**
		 * The Y value.
		 */
		_tType									y;

		/**
		 * The Z value.
		 */
		_tType									z;
#pragma pack( pop )
	};

}	// namespace lsm

#endif	// __LSM_VECTOR3BASE_H__

