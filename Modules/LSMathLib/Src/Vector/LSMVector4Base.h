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
 * Description: A template for any type of 4D vector.
 */


#ifndef __LSM_VECTOR4BASE_H__
#define __LSM_VECTOR4BASE_H__

#include "../LSMMathLib.h"
#include "LSMVector3Base.h"

namespace lsm {

	/**
	 * Class CVector4Base
	 * \brief A template for any type of 4D vector.
	 *
	 * Description: A template for any type of 4D vector.
	 */
	template <typename _tType, typename _tVec3>
	class CVector4Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVector4Base() {
		}
		LSE_CALLCTOR							CVector4Base( _tType _fX, _tType _fY, _tType _fZ, _tType _fW ) :
			x( _fX ),
			y( _fY ),
			z( _fZ ),
			w( _fW ) {
		}
		LSE_CALLCTOR							CVector4Base( _tType * _pfValues ) :
			x( _pfValues[0] ),
			y( _pfValues[1] ),
			z( _pfValues[2] ),
			w( _pfValues[3] ) {
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
		 * Unary -.
		 *
		 * \return Returns a copy of this vector with each axis negated.
		 */
		CVector4Base<_tType, _tVec3> LSE_FCALL	operator - () const {
			return CVector4Base<_tType, _tVec3>( -x, -y, -z, -w );
		}

		/**
		 * Binary addition.
		 *
		 * \param _v4bOther The vector to add to this one componentwise.
		 * \return Returns a new vector representing the addition.
		 */
		CVector4Base<_tType, _tVec3> LSE_FCALL	operator + ( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return CVector4Base<_tType, _tVec3>( x + _v4bOther.x, y + _v4bOther.y, z + _v4bOther.z, w + _v4bOther.w );
		}

		/**
		 * Binary subtraction.
		 *
		 * \param _v4bOther The vector to subtract from this one componentwise.
		 * \return Returns a new vector representing the difference.
		 */
		CVector4Base<_tType, _tVec3> LSE_FCALL	operator - ( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return CVector4Base<_tType, _tVec3>( x - _v4bOther.x, y - _v4bOther.y, z - _v4bOther.z, w - _v4bOther.w );
		}

		/**
		 * Binary multiplication.
		 *
		 * \param _tValue The amount by which to multiply each component in this vector.
		 * \return Returns a new vector representing the product.
		 */
		CVector4Base<_tType, _tVec3> LSE_FCALL	operator * ( _tType _tValue ) const {
			return CVector4Base<_tType, _tVec3>( x * _tValue, y * _tValue, z * _tValue, w * _tValue );
		}

		/**
		 * Binary division.
		 *
		 * \param _tValue The amount by which to divide each component in this vector.
		 * \return Returns a new vector representing the quotient.
		 */
		CVector4Base<_tType, _tVec3> LSE_FCALL	operator / ( _tType _tValue ) const {
			return (*this) * (_tType( 1.0 ) / _tValue);
		}

		/**
		 * Adds a vector to this vector, storing the result in this vector.
		 *
		 * \param _v4bOther The vector to add to this vector.
		 * \return Returns this vector, after the addition operation has been applied.
		 */
		CVector4Base<_tType, _tVec3> & LSE_FCALL
												operator += ( const CVector4Base<_tType, _tVec3> &_v4bOther ) {
			x += _v4bOther.x;
			y += _v4bOther.y;
			z += _v4bOther.z;
			w += _v4bOther.w;
			return (*this);
		}

		/**
		 * Subtract a vector from this vector, storing the result in this vector.
		 *
		 * \param _v4bOther The vector to subtract from this vector.
		 * \return Returns this vector, after the subtraction operation has been applied.
		 */
		CVector4Base<_tType, _tVec3> & LSE_FCALL
												operator -= ( const CVector4Base<_tType, _tVec3> &_v4bOther ) {
			x -= _v4bOther.x;
			y -= _v4bOther.y;
			z -= _v4bOther.z;
			w -= _v4bOther.w;
			return (*this);
		}

		/**
		 * Multiply the vector by a given amount, storing the result in this vector.
		 *
		 * \param _tValue The amount by which to multiply each component in this vector.
		 * \return Returns this vector, after the muliplication operation has been applied.
		 */
		CVector4Base<_tType, _tVec3> & LSE_FCALL
												operator *= ( _tType _tValue ) {
			x *= _tValue;
			y *= _tValue;
			z *= _tValue;
			w *= _tValue;
			return (*this);
		}

		/**
		 * Divide the vector by a given amount, storing the result in this vector.
		 *
		 * \param _tValue The amount by which to divide each component in this vector.
		 * \return Returns this vector, after the division operation has been applied.
		 */
		CVector4Base<_tType, _tVec3> & LSE_FCALL
												operator /= ( _tType _tValue ) {
			return (*this) *= (_tType( 1.0 ) / _tValue);
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		_tType LSE_FCALL						operator * ( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return x * _v4bOther.x + y * _v4bOther.y + z * _v4bOther.z + w * _v4bOther.w;
		}

		/**
		 * Equality.
		 *
		 * \param _v4bOther The vector to check for equality.
		 * \return Returns true if both vectors are equal.
		 */
		LSBOOL LSE_FCALL						operator == ( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return x == _v4bOther.x && y == _v4bOther.y && z == _v4bOther.z && w == _v4bOther.w;
		}

		/**
		 * Inequality.
		 *
		 * \param _v4bOther The vector to check for inequality.
		 * \return Returns true if both vectors are not equal.
		 */
		LSBOOL LSE_FCALL						operator != ( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return x != _v4bOther.x || y != _v4bOther.y || z != _v4bOther.z || w != _v4bOther.w;
		}


		// == Functions.
		/**
		 * Apply the given values to each of the axes on this vector.
		 *
		 * \param _fX The new X-axis value.
		 * \param _fY The new Y-axis value.
		 * \param _fZ The new Z-axis value.
		 * \param _fW The new W-axis value.
		 */
		void LSE_FCALL						Set( _tType _fX, _tType _fY, _tType _fZ, _tType _fW ) {
			x = _fX;
			y = _fY;
			z = _fZ;
			w = _fW;
		}

		/**
		 * Sets all axes on this vector to 0.
		 */
		void LSE_FCALL						Clear() {
			x = y = z = w = _tType( 0 );
		}

		/**
		 * Negates each axis on this vector.
		 */
		void LSE_FCALL						Invert() {
			x = -x;
			y = -y;
			z = -z;
			w = -w;
		}

		/**
		 * Calculates the length of the vector.
		 *
		 * \return Returns the length of the vector.
		 */
		_tType LSE_FCALL						Len() const {
			register LSREAL fSquared = x * x + y * y + z * z + w * w;
			return fSquared * CMathLib::InvSqrt( fSquared );
		}

		/**
		 * Calculates the squared length of the vector.
		 *
		 * \return Returns the squared length of the vector.
		 */
		_tType LSE_FCALL						LenSq() const {
			return x * x + y * y + z * z + w * w;
		}

		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		void LSE_FCALL						Normalize() {
			LSREAL fInvLen = CMathLib::InvSqrt( x * x + y * y + z * z + w * w );
			x *= fInvLen;
			y *= fInvLen;
			z *= fInvLen;
			w *= fInvLen;
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		_tType LSE_FCALL						Dot( const CVector4Base<_tType, _tVec3> &_v4bOther ) const {
			return x * _v4bOther.x + y * _v4bOther.y + z * _v4bOther.z + w * _v4bOther.w;
		}

		/**
		 * Gets the maximum component.
		 *
		 * \return Returns the maximum component.
		 */
		_tType LSE_FCALL						Max() const {
			return CStd::Max( CStd::Max( CStd::Max( x, y ), z ), w );
		}

		/**
		 * Gets the minimum component.
		 *
		 * \return Returns the minimum component.
		 */
		_tType LSE_FCALL						Min() const {
			return CStd::Min( CStd::Min( CStd::Min( x, y ), z ), w );
		}

		/**
		 * Gets the X, Y, and Z values of this vector as a 3D vector.
		 *
		 * \return Returns a 3D vector representing the XYZ of this vector.
		 */
		_tVec3 LSE_FCALL						Vec3() const {
			return _tVec3( x, y, z );
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bLeft The left operand.
		 * \param _v4bRight The right operand.
		 * \return Returns the dot product between the two vectors.
		 */
		static _tType LSE_FCALL					DotProduct( const CVector4Base<_tType, _tVec3> &_v4bLeft, const CVector4Base<_tType, _tVec3> &_v4bRight ) {
			return _v4bLeft.x * _v4bRight.x + _v4bLeft.y * _v4bRight.y + _v4bLeft.z * _v4bRight.z + _v4bLeft.w * _v4bRight.w;
		}

		/**
		 * Linearly interpolate between two vectors, returning the resulting vector.
		 *
		 * \param _v4bLeft Left operand.
		 * \param _v4bRight Right operand.
		 * \param _fFrac Amount by which to interpolate between the vectors.
		 * \return Returns the vector interpolated between _v4bLeft and _v4bRight by _fFrac.
		 */
		static CVector4Base<_tType, _tVec3> LSE_FCALL	Lerp( const CVector4Base<_tType, _tVec3> &_v4bLeft, const CVector4Base<_tType, _tVec3> &_v4bRight, _tType _fFrac ) {
			return ((_v4bRight - _v4bLeft) * _fFrac) + _v4bLeft;
		}

		/**
		 * Perform a Hermite spline interpolation between the supplied vectors.
		 *
		 * \param _v4bLeft Left operand.
		 * \param _v4bT0 Left tangent.
		 * \param _v4bRight Right operand.
		 * \param _v4bT1 Right tangent.
		 * \param _fFrac Amount by which to interpolate between the vectors.
		 * \return Returns the vector interpolated between _v4bLeft and _v4bRight by _fFrac.
		 */
		static CVector4Base<_tType, _tVec3> LSE_FCALL	Hermite( const CVector4Base<_tType, _tVec3> &_v4bLeft, const CVector4Base<_tType, _tVec3> &_v4bT0,
			const CVector4Base<_tType, _tVec3> &_v4bRight, const CVector4Base<_tType, _tVec3> &_v4bT1,
			_tType _fFrac ) {
			_tType fS2	= _fFrac * _fFrac;
			_tType fS3	= fS2 * _fFrac;
			_tType f2s2	= static_cast<_tType>(2.0) * fS2;
			_tType f2s3	= static_cast<_tType>(2.0) * fS3;
			_tType f3s2	= static_cast<_tType>(3.0) * fS2;

			return (_v4bLeft * (f2s3 - f3s2 + LSM_ONE)) +
				(_v4bRight * (-f2s3 + f3s2)) +
				(_v4bT0 * (fS3 - f2s2 + _fFrac)) +
				(_v4bT1 * (fS3 - fS2));
		}

		/**
		 * Perform a Catmull-Rom spline interpolation between the supplied vectors
		 *
		 * \param _v4bV0 Input point 0.
		 * \param _v4bV1 Input point 1.
		 * \param _v4bV2 Input point 2.
		 * \param _v4bV3 Input point 3.
		 * \param _fFrac Amount by which to interpolate between _v4bV1 and _v4bV2.
		 * \return Returns the vector interpolated between _v4bV1 and _v4bV2 by _fFrac.
		 */
		static CVector4Base<_tType, _tVec3> LSE_FCALL	CatmullRom( const CVector4Base<_tType, _tVec3> &_v4bV0, const CVector4Base<_tType, _tVec3> &_v4bV1,
			const CVector4Base<_tType, _tVec3> &_v4bV2, const CVector4Base<_tType, _tVec3> &_v4bV3,
			_tType _fFrac ) {
			_tType fS2	= _fFrac * _fFrac;
			return ((_v4bV1 * static_cast<_tType>(2.0)) +
				(-_v4bV0 + _v4bV2) * _fFrac +
				(_v4bV0 * static_cast<_tType>(2.0) - _v4bV1 * static_cast<_tType>(5.0) + _v4bV2 * static_cast<_tType>(4.0) - _v4bV3) * fS2 +
				(-_v4bV0 + _v4bV1 * static_cast<_tType>(3.0) - _v4bV2 * static_cast<_tType>(3.0) + _v4bV3) * fS2 * _fFrac) * LSM_HALF;
		}


		// == Members.
#pragma pack( push, 1 )
		/** The X value. */
		_tType											x;

		/** The Y value. */
		_tType											y;

		/** The Z value. */
		_tType											z;

		/** The W value. */
		_tType											w;
#pragma pack( pop )
	};

}	// namespace lsm

#endif	// __LSM_VECTOR4BASE_H__

