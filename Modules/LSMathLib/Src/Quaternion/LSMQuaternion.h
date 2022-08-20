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
 * Description: Quaternion in XYZW format.
 */


#ifndef __LSM_QUATERNION_H__
#define __LSM_QUATERNION_H__

#include "../LSMMathLib.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Vector/LSMVector3.h"

#pragma warning( push )

// warning C4201: nonstandard extension used : nameless struct/union
#pragma warning( disable : 4201 )


namespace lsm {

	/**
	 * Class CQuaternion
	 * \brief A quaternion.
	 *
	 * Description: Quaternion in XYZW format.
	 */
	class CQuaternion {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR					CQuaternion();
		LSE_INLINE LSE_CALLCTOR					CQuaternion( LSREAL _fX, LSREAL _fY, LSREAL _fZ, LSREAL _fW );
		LSE_INLINE LSE_CALLCTOR					CQuaternion( const CQuaternion &_qQuat );


		// == Operators.
		/**
		 * Addition assignment operator.
		 *
		 * \param _qQuat The quaternion to add to this quaternion.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		operator += ( const CQuaternion &_qQuat );

		/**
		 * Subtraction assignment operator.
		 *
		 * \param _qQuat The quaternion to subtract from this quaternion.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		operator -= ( const CQuaternion &_qQuat );

		/**
		 * Muliplication assignment operator.
		 *
		 * \param _qQuat The quaternion to multiply by this quaternion.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		operator *= ( const CQuaternion &_qQuat );

		/**
		 * Muliplication assignment operator on a per-component basis.
		 *
		 * \param _fVal The value by which to multiply each component in this quaternion.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		operator *= ( LSREAL _fVal );

		/**
		 * Division assignment operator on a per-component basis.
		 *
		 * \param _fVal The value by which to divide each component in this quaternion.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		operator /= ( LSREAL _fVal );

		/**
		 * Unary +.
		 *
		 * \return Returns this quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator + () const;

		/**
		 * Unary -.
		 *
		 * \return Returns the negative of this quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator - () const;

		/**
		 * Addition operator.
		 *
		 * \param _qQuat The operand.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator + ( const CQuaternion &_qQuat ) const;
		
		/**
		 * Subtraction operator.
		 *
		 * \param _qQuat The operand.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator - ( const CQuaternion &_qQuat ) const;

		/**
		 * Multiplication operator.
		 *
		 * \param _qQuat The operand.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator * ( const CQuaternion &_qQuat ) const;

		/**
		 * Multiplication operator.
		 *
		 * \param _fVal The operand.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator * ( LSREAL _fVal ) const;

		/**
		 * Division operator.
		 *
		 * \param _fVal The operand.
		 * \return Returns the resulting quaternion.
		 */
		LSE_INLINE CQuaternion	LSE_FCALL		operator / ( LSREAL _fVal ) const;

		/**
		 * Equality test.
		 *
		 * \param _qQuat Object against which to test for equality.
		 * \return Returns true if all components on this quaternion are the same as on _qQuat.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( const CQuaternion &_qQuat ) const;

		/**
		 * Inequality test.
		 *
		 * \param _qQuat Object against which to test for inequality.
		 * \return Returns true if any component on this quaternion are different from those on _qQuat.
		 */
		LSE_INLINE bool LSE_FCALL				operator != ( const CQuaternion &_qQuat ) const;

		/**
		 * Give array access.
		 *
		 * \param _ui32Index Index of the component to get.
		 * \return Returns the component at the given index.
		 */
		LSE_INLINE LSREAL & LSE_FCALL			operator [] ( uint32_t _ui32Index );

		/**
		 * Give array access.
		 *
		 * \param _ui32Index Index of the component to get.
		 * \return Returns the component at the given index.
		 */
		LSE_INLINE LSREAL LSE_FCALL				operator [] ( uint32_t _ui32Index ) const;


		// == Functions.
		/**
		 * Functions on self.
		 * Sets this quaternion to an identity quaternion and returns itself.
		 *
		 * \return Returns this object after setting it to an identity quaternion.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		Identity();
		
		/**
		 * Returns the conjugate of this quaternion.
		 *
		 * \return Returns the conjugate of this quaternion.
		 */
		LSE_INLINE CQuaternion LSE_FCALL		Conjugate() const;
		
		/**
		 * Returns the dot product of this quaternion with another.
		 *
		 * \param _qQuat The operand.
		 * \return Returns the dot product of this quaternion with another.
		 */
		LSE_INLINE LSREAL LSE_FCALL				Dot( const CQuaternion &_qQuat ) const;
		
		/**
		 * Returns the exponential of this quaternion.
		 *
		 * \return Returns the exponential of this quaternion.
		 */
		CQuaternion LSE_FCALL					Exp() const;
		
		/**
		 * Returns the natural log of this quaternion.
		 *
		 * \return Returns the natural log of this quaternion.
		 */
		CQuaternion LSE_FCALL					Log() const;
		
		/**
		 * Returns the squared length of this quaternion.
		 *
		 * \return Returns the squared length of this quaternion.
		 */
		LSE_INLINE LSREAL LSE_FCALL				LenSq() const;
		
		/**
		 * Returns the length of this quaternion.
		 *
		 * \return Returns the length of this quaternion.
		 */
		LSE_INLINE LSREAL LSE_FCALL				Len() const;
		
		/**
		 * Normalizes this quaternion.
		 */
		LSE_INLINE void LSE_FCALL				Normalize();
		
		/**
		 * Returns the inverse of this quaternion.
		 *
		 * \return Returns the inverse of this quaternion.
		 */
		LSE_INLINE CQuaternion LSE_FCALL		Inverse();
		
		/**
		 * Linear interpolation.  Returns this object.
		 *
		 * \param _qLeft Starting point.
		 * \param _qRight Ending point.
		 * \param _fT Amount to interpolate between the starting and ending points.
		 * \return Returns this object after interpolating linearly.
		 */
		LSE_INLINE CQuaternion & LSE_FCALL		Lerp( const CQuaternion &_qLeft, const CQuaternion &_qRight, LSREAL _fT );
		
		/**
		 * Spherical linear interpolation.  Returns this object.
		 *
		 * \param _qLeft The starting quaternion.
		 * \param _qRight The ending quaternion.
		 * \param _fT Amount of interpolation.
		 * \return Returns this object after interpolation.
		 */
		CQuaternion & LSE_FCALL					Slerp( const CQuaternion &_qLeft, const CQuaternion &_qRight, LSREAL _fT );
		
		/**
		 * Spherical quadrangle interpolation.  Returns this object.
		 *
		 * \param _qQ Operand 0.
		 * \param _qA Operand 1.
		 * \param _qB Operand 2.
		 * \param _qC Operand 3.
		 * \param _fT Amount to interpolate between the starting and ending points.
		 * \return Returns this object after interpolation.
		 */
		CQuaternion & LSE_FCALL					Squad( const CQuaternion &_qQ, const CQuaternion &_qA, const CQuaternion &_qB, const CQuaternion &_qC, LSREAL _fT );

		/**
		 * Builds from the rotation part of a 4-by-4 matrix.  Matrix is row-major.
		 *
		 * \param _mMat The matrix from which to build this quaternion.
		 * \return Returns this object after building it.
		 */
		CQuaternion & LSE_FCALL					FromRotationMatrix( const CMatrix4x4 &_mMat );
		
		/**
		 * Create the rotation part of a 4-by-4 row-major matrix.  The matrix members not related to the rotation
		 *	are not modified.
		 *
		 * \param _mMat The matrix whose rotational components are to be built.
		 */
		void LSE_FCALL						ToRotationMatrix( CMatrix4x4 &_mMat ) const;

		/**
		 * Builds from 3 vectors.
		 *
		 * \param _vVectors The right, up, and forward vectors to use for building of the quaternion.
		 * \return Returns this object after building it.
		 */
		CQuaternion & LSE_FCALL					FromRotationVectors( const CVector3 _vVectors[3] );

		/**
		 * Adds a scaled vector.
		 *
		 * \param _vVec The vector to add after being scaled by _fTime.
		 * \param _fTime The amount by which to scale _vVec before adding it to this queternion.
		 * \return Returns this quaternion after the addition.
		 */
		CQuaternion & LSE_FCALL					AddScaledVector( const CVector3 &_vVec, LSREAL _fTime );
		
		/**
		 * Gets the X value for read and write.
		 *
		 * \return Returns the X value for read and write.
		 */
		LSE_INLINE LSREAL & LSE_FCALL			X();
		
		/**
		 * Gets the Y value for read and write.
		 *
		 * \return Returns the Y value for read and write.
		 */
		LSE_INLINE LSREAL & LSE_FCALL			Y();
		
		/**
		 * Gets the Z value for read and write.
		 *
		 * \return Returns the Z value for read and write.
		 */
		LSE_INLINE LSREAL & LSE_FCALL			Z();
		
		/**
		 * Gets the W value for read and write.
		 *
		 * \return Returns the W value for read and write.
		 */
		LSE_INLINE LSREAL & LSE_FCALL			W();

		// == Members.
#pragma pack( push, 1 )
		struct {
			/**
			 * The components of the quaternion.
			 */
			LSREAL								x,
												y,
												z,
												w;
		} u;
#pragma pack( pop )

	protected :
		static uint32_t							m_ui32Next[3];
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CQuaternion::CQuaternion() {}
	LSE_INLINE LSE_CALLCTOR CQuaternion::CQuaternion( LSREAL _fX, LSREAL _fY, LSREAL _fZ, LSREAL _fW ) {
		u.x = _fX;
		u.y = _fY;
		u.z = _fZ;
		u.w = _fW;
	}
	LSE_INLINE LSE_CALLCTOR CQuaternion::CQuaternion( const CQuaternion &_qQuat ) {
		u.x = _qQuat[0];
		u.y = _qQuat[1];
		u.z = _qQuat[2];
		u.w = _qQuat[3];
	}


	// == Operators.
	/**
	 * Addition assignment operator.
	 *
	 * \param _qQuat The quaternion to add to this quaternion.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::operator += ( const CQuaternion &_qQuat ) {
		u.x += _qQuat[0];
		u.y += _qQuat[1];
		u.z += _qQuat[2];
		u.w += _qQuat[3];
		return (*this);
	}

	/**
	 * Subtraction assignment operator.
	 *
	 * \param _qQuat The quaternion to subtract from this quaternion.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::operator -= ( const CQuaternion &_qQuat ) {
		u.x -= _qQuat[0];
		u.y -= _qQuat[1];
		u.z -= _qQuat[2];
		u.w -= _qQuat[3];
		return (*this);
	}

	/**
	 * Muliplication assignment operator.
	 *
	 * \param _qQuat The quaternion to multiply by this quaternion.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::operator *= ( const CQuaternion &_qQuat ) {
		CQuaternion qProd;

		qProd[3] =
			u.w * _qQuat[3] -
			u.x * _qQuat[0] -
			u.y * _qQuat[1] -
			u.z * _qQuat[2];

		qProd[0] =
			u.w * _qQuat[0] +
			u.x * _qQuat[3] +
			u.y * _qQuat[2] -
			u.z * _qQuat[1];

		qProd[1] =
			u.w * _qQuat[1] +
			u.y * _qQuat[3] +
			u.z * _qQuat[0] -
			u.x * _qQuat[2];

		qProd[2] =
			u.w * _qQuat[2] +
			u.z * _qQuat[3] +
			u.x * _qQuat[1] -
			u.y * _qQuat[0];

		(*this) = qProd;
		return (*this);
	}

	/**
	 * Muliplication assignment operator on a per-component basis.
	 *
	 * \param _fVal The value by which to multiply each component in this quaternion.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::operator *= ( LSREAL _fVal ) {
		u.x *= _fVal;
		u.y *= _fVal;
		u.z *= _fVal;
		u.w *= _fVal;
		return (*this);
	}

	/**
	 * Division assignment operator on a per-component basis.
	 *
	 * \param _fVal The value by which to divide each component in this quaternion.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::operator /= ( LSREAL _fVal ) {
		_fVal = LSM_ONE / _fVal;
		u.x *= _fVal;
		u.y *= _fVal;
		u.z *= _fVal;
		u.w *= _fVal;
		return (*this);
	}


	/**
	 * Unary +.
	 *
	 * \return Returns this quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator + () const {
		return CQuaternion( +u.x, +u.y, +u.z, +u.w );
	}

	/**
	 * Unary -.
	 *
	 * \return Returns the negative of this quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator - () const {
		return CQuaternion( -u.x, -u.y, -u.z, -u.w );
	}


	/**
	 * Addition operator.
	 *
	 * \param _qQuat The operand.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator + ( const CQuaternion &_qQuat ) const {
		return CQuaternion( u.x + _qQuat.u.x, u.y + _qQuat.u.y, u.z + _qQuat.u.z, u.w + _qQuat.u.w );
	}

	/**
	 * Subtraction operator.
	 *
	 * \param _qQuat The operand.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator - ( const CQuaternion &_qQuat ) const {
		return CQuaternion( u.x - _qQuat.u.x, u.y - _qQuat.u.y, u.z - _qQuat.u.z, u.w - _qQuat.u.w );
	}

	/**
	 * Multiplication operator.
	 *
	 * \param _qQuat The operand.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator * ( const CQuaternion &_qQuat ) const {
		/*return CQuaternion( u.w * _qQuat.u.x +
			u.x * _qQuat.u.w +
			u.y * _qQuat.u.z -
			u.z * _qQuat.u.y,
			u.w * _qQuat.u.y +
			u.y * _qQuat.u.w +
			u.z * _qQuat.u.x -
			u.x * _qQuat.u.z,
			u.w * _qQuat.u.z +
			u.z * _qQuat.u.w +
			u.x * _qQuat.u.y -
			u.y * _qQuat.u.x,
			u.w * _qQuat.u.w -
			u.x * _qQuat.u.x -
			u.y * _qQuat.u.y -
			u.z * _qQuat.u.z );*/
		return CQuaternion( u.w * _qQuat.u.x +
			u.x * _qQuat.u.w -
			u.y * _qQuat.u.z -
			u.z * _qQuat.u.y,
			u.w * _qQuat.u.y -
			u.x * _qQuat.u.z +
			u.y * _qQuat.u.w -
			u.z * _qQuat.u.x,
			u.w * _qQuat.u.z +
			u.x * _qQuat.u.y -
			u.y * _qQuat.u.x +
			u.z * _qQuat.u.w,
			u.w * _qQuat.u.w -
			u.x * _qQuat.u.x -
			u.y * _qQuat.u.y -
			u.z * _qQuat.u.z );
	}

	/**
	 * Multiplication operator.
	 *
	 * \param _fVal The operand.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator * ( LSREAL _fVal ) const {
		return CQuaternion( u.x * _fVal, u.y * _fVal, u.z * _fVal, u.w * _fVal );
	}

	/**
	 * Division operator.
	 *
	 * \param _fVal The operand.
	 * \return Returns the resulting quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::operator / ( LSREAL _fVal ) const {
		register LSREAL fVal = LSM_ONE / _fVal;
		return CQuaternion( u.x * fVal, u.y * fVal, u.z * fVal, u.w * fVal );
	}

	/**
	 * Equality test.
	 *
	 * \param _qQuat Object against which to test for equality.
	 * \return Returns true if all components on this quaternion are the same as on _qQuat.
	 */
	LSE_INLINE bool LSE_FCALL CQuaternion::operator == ( const CQuaternion &_qQuat ) const {
		return u.x == _qQuat[0] && u.y == _qQuat[1] && u.z == _qQuat[2] && u.w == _qQuat[3];
	}

	/**
	 * Inequality test.
	 *
	 * \param _qQuat Object against which to test for inequality.
	 * \return Returns true if any component on this quaternion are different from those on _qQuat.
	 */
	LSE_INLINE bool LSE_FCALL CQuaternion::operator != ( const CQuaternion &_qQuat ) const {
		return u.x != _qQuat[0] || u.y != _qQuat[1] || u.z != _qQuat[2] || u.w != _qQuat[3];
	}

	/**
	 * Give array access.
	 *
	 * \param _ui32Index Index of the component to get.
	 * \return Returns the component at the given index.
	 */
	LSE_INLINE LSREAL & LSE_FCALL CQuaternion::operator [] ( uint32_t _ui32Index ) {
		return reinterpret_cast<LSREAL *>(this)[_ui32Index];
	}

	/**
	 * Give array access.
	 *
	 * \param _ui32Index Index of the component to get.
	 * \return Returns the component at the given index.
	 */
	LSE_INLINE LSREAL LSE_FCALL CQuaternion::operator [] ( uint32_t _ui32Index ) const {
		return reinterpret_cast<const LSREAL *>(this)[_ui32Index];
	}


	/**
	 * Functions on self.
	 * Sets this quaternion to an identity quaternion and returns itself.
	 *
	 * \return Returns this object after setting it to an identity quaternion.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::Identity() {
		u.x = u.y = u.z = LSM_ZERO;
		u.w = LSM_ONE;
		return (*this);
	}

	/**
	 * Returns the conjugate of this quaternion.
	 *
	 * \return Returns the conjugate of this quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::Conjugate() const {
		return CQuaternion( -u.x, -u.y, -u.z, u.w );
	}

	/**
	 * Returns the dot product of this quaternion with another.
	 *
	 * \param _qQuat The operand.
	 * \return Returns the dot product of this quaternion with another.
	 */
	LSE_INLINE LSREAL LSE_FCALL CQuaternion::Dot( const CQuaternion &_qQuat ) const {
		return (u.x * _qQuat[0]) +
			(u.y * _qQuat[1]) +
			(u.z * _qQuat[2]) +
			(u.w * _qQuat[3]);
	}

	/**
	 * Returns the squared length of this quaternion.
	 *
	 * \return Returns the squared length of this quaternion.
	 */
	LSE_INLINE LSREAL LSE_FCALL CQuaternion::LenSq() const {
		return u.x * u.x +
			u.y * u.y +
			u.z * u.z +
			u.w * u.w;
	}

	/**
	 * Returns the length of this quaternion.
	 *
	 * \return Returns the length of this quaternion.
	 */
	LSE_INLINE LSREAL LSE_FCALL CQuaternion::Len() const {
		return CMathLib::Sqrt( u.x * u.x +
			u.y * u.y +
			u.z * u.z +
			u.w * u.w );
	}

	/**
	 * Normalizes this quaternion.
	 */
	LSE_INLINE void LSE_FCALL CQuaternion::Normalize() {
		LSREAL fInvLen = CMathLib::InvSqrt( u.x * u.x +
			u.y * u.y +
			u.z * u.z +
			u.w * u.w );
		u.x *= fInvLen;
		u.y *= fInvLen;
		u.z *= fInvLen;
		u.w *= fInvLen;
	}

	/**
	 * Returns the inverse of this quaternion.
	 *
	 * \return Returns the inverse of this quaternion.
	 */
	LSE_INLINE CQuaternion LSE_FCALL CQuaternion::Inverse() {
		CQuaternion qRet;

		LSREAL fInvLen = -CMathLib::InvSqrt( u.x * u.x +
			u.y * u.y +
			u.z * u.z +
			u.w * u.w );

		qRet[3] = u.w * -fInvLen;
		qRet[0] = u.x * fInvLen;
		qRet[1] = u.y * fInvLen;
		qRet[2] = u.z * fInvLen;

		return qRet;
	}

	/**
	 * Linear interpolation.  Returns this object.
	 *
	 * \param _qLeft Starting point.
	 * \param _qRight Ending point.
	 * \param _fT Amount to interpolate between the starting and ending points.
	 * \return Returns this object after interpolating linearly.
	 */
	LSE_INLINE CQuaternion & LSE_FCALL CQuaternion::Lerp( const CQuaternion &_qLeft, const CQuaternion &_qRight, LSREAL _fT ) {
		(*this) = _qLeft + (_qRight - _qLeft) * _fT;
		Normalize();
		return (*this);
	}

	/**
	 * Create the rotation part of a 4-by-4 row-major matrix.  The matrix members not related to the rotation
	 *	are not modified.
	 *
	 * \param _mMat The matrix whose rotational components are to be built.
	 */
	LSE_INLINE void LSE_FCALL CQuaternion::ToRotationMatrix( CMatrix4x4 &_mMat ) const {
		LSREAL fTx  = LSM_TWO * u.x;
		LSREAL fTy  = LSM_TWO * u.y;
		LSREAL fTz  = LSM_TWO * u.z;
		LSREAL fTwx = fTx * u.w;
		LSREAL fTwy = fTy * u.w;
		LSREAL fTwz = fTz * u.w;
		LSREAL fTxx = fTx * u.x;
		LSREAL fTxy = fTy * u.x;
		LSREAL fTxz = fTz * u.x;
		LSREAL fTyy = fTy * u.y;
		LSREAL fTyz = fTz * u.y;
		LSREAL fTzz = fTz * u.z;

		_mMat( 0, 0 ) = LSM_ONE - (fTyy + fTzz);
		_mMat( 1, 0 ) = fTxy - fTwz;
		_mMat( 2, 0 ) = fTxz + fTwy;
		_mMat( 0, 1 ) = fTxy + fTwz;
		_mMat( 1, 1 ) = LSM_ONE - (fTxx + fTzz);
		_mMat( 2, 1 ) = fTyz - fTwx;
		_mMat( 0, 2 ) = fTxz - fTwy;
		_mMat( 1, 2 ) = fTyz + fTwx;
		_mMat( 2, 2 ) = LSM_ONE - (fTxx + fTyy);
	}
	
	/**
	 * Gets the X value for read and write.
	 *
	 * \return Returns the X value for read and write.
	 */
	LSE_INLINE LSREAL & LSE_FCALL CQuaternion::X() {
		return u.x;
	}
	
	/**
	 * Gets the Y value for read and write.
	 *
	 * \return Returns the Y value for read and write.
	 */
	LSE_INLINE LSREAL & LSE_FCALL CQuaternion::Y() {
		return u.y;
	}
	
	/**
	 * Gets the Z value for read and write.
	 *
	 * \return Returns the Z value for read and write.
	 */
	LSE_INLINE LSREAL & LSE_FCALL CQuaternion::Z() {
		return u.z;
	}
	
	/**
	 * Gets the W value for read and write.
	 *
	 * \return Returns the W value for read and write.
	 */
	LSE_INLINE LSREAL & LSE_FCALL CQuaternion::W() {
		return u.w;
	}

}	// namespace lsm

#pragma warning( pop )

#endif	// __LSM_QUATERNION_H__
