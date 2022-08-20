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

#include "LSMQuaternion.h"

namespace lsm {

	// == Members.
	uint32_t CQuaternion::m_ui32Next[3] = { 1, 2, 0 };

	// == Functions.
	/**
	 * Returns the exponential of this quaternion.
	 *
	 * \return Returns the exponential of this quaternion.
	 */
	CQuaternion LSE_FCALL CQuaternion::Exp() const {
		// If q = A*(u.x*i+u.y*j+u.z*k) where (u.x,u.y,u.z) is unit length, then
		//	exp(q) = cos(A)+sin(A)*(u.x*i+u.y*j+u.z*k).  If sin(A) is near zero,
		//	use exp(q) = cos(A)+A*(u.x*i+u.y*j+u.z*k) since A/sin(A) has limit 1.

		CQuaternion qRet;

		LSREAL fAngle = CMathLib::Sqrt( u.x * u.x +
			u.y * u.y +
			u.z * u.z );

		LSREAL fSin = CMathLib::Sin( fAngle );
		qRet[3] = CMathLib::Cos( fAngle );

		if ( CMathLib::Abs( fSin ) >= LSM_REAL_EPSILON ) {
			LSREAL fCoeff = fSin / fAngle;
			qRet[0] = fCoeff * u.x;
			qRet[1] = fCoeff * u.y;
			qRet[2] = fCoeff * u.z;
		}
		else {
			qRet[0] = u.x;
			qRet[1] = u.y;
			qRet[2] = u.z;
		}

		return qRet;
	}

	/**
	 * Returns the natural log of this quaternion.
	 *
	 * \return Returns the natural log of this quaternion.
	 */
	CQuaternion LSE_FCALL CQuaternion::Log() const {
		// If q = cos(A)+sin(A)*(u.x*i+u.y*j+u.z*k) where (u.x,u.y,u.z) is unit length, then
		//	log(q) = A*(u.x*i+u.y*j+u.z*k).  If sin(A) is near zero, use log(q) =
		//	sin(A)*(u.x*i+u.y*j+u.z*k) since sin(A)/A has limit 1.

		CQuaternion qRet;
		qRet[3] = LSM_ZERO;

		if ( CMathLib::Abs( u.w ) < LSM_ONE ) {
			LSREAL fAngle = CMathLib::ACos( u.w );
			LSREAL fSin = CMathLib::Sin( fAngle );
			if ( CMathLib::Abs( fSin ) >= LSM_REAL_EPSILON ) {
				LSREAL fCoeff = fAngle / fSin;
				qRet[0] = fCoeff * u.x;
				qRet[1] = fCoeff * u.y;
				qRet[2] = fCoeff * u.z;
				return qRet;
			}
		}

		qRet[0] = u.x;
		qRet[1] = u.y;
		qRet[2] = u.z;
		return qRet;
	}

	/**
	 * Spherical linear interpolation.  Returns this object.
	 *
	 * \param _qLeft The starting quaternion.
	 * \param _qRight The ending quaternion.
	 * \param _fT Amount of interpolation.
	 * \return Returns this object after interpolation.
	 */
	CQuaternion & LSE_FCALL CQuaternion::Slerp( const CQuaternion &_qLeft, const CQuaternion &_qRight, LSREAL _fT ) {
		/*if ( _fT <= LSM_ZERO ) {
			(*this) = _qLeft;
			return (*this);
		}
		if ( _fT >= LSM_ONE ) {
			(*this) = _qRight;
			return (*this);
		}*/

		CQuaternion qTemp = _qRight;
		LSREAL fC = _qLeft.Dot( qTemp );

		if ( fC < LSM_ZERO ) {
			qTemp = -qTemp;
			fC = -fC;
		}

		if ( fC >= LSM_ONE - static_cast<LSREAL>(0.001) ) {
			return Lerp( _qLeft, qTemp, _fT );
		}

		LSREAL fA = CMathLib::ACos( fC ); // ACos() clamps input to [-1, 1].
		(*this) = (_qLeft * CMathLib::Sin( (LSM_ONE - _fT) * fA ) + qTemp * CMathLib::Sin( _fT * fA ) ) / CMathLib::Sin( fA );
		return (*this);

		/*LSREAL fW1, fW2;

		LSREAL fCos		= _qLeft.Dot( _qRight );
		LSREAL fAngle	= CMathLib::ACos( fCos );
		LSREAL fSin		= CMathLib::Sin( fAngle );

		if ( fSin > LSM_REAL_EPSILON ) {
			fW1 = CMathLib::Sin( (LSM_ONE - _fT) * fAngle ) / fSin;
			fW2 = CMathLib::Sin( _fT*fAngle ) / fSin;
		}
		else {
			fW1 = LSM_ONE - _fT;
			fW2 = _fT;
		}

		(*this) = _qLeft * fW1 + _qRight * fW2;
		return (*this);*/

		/*LSREAL fCos = _qLeft.Dot( _qRight );
		LSREAL fAngle = CMathLib::ACos( fCos );

		if ( CMathLib::Abs( fAngle ) >= LSM_REAL_EPSILON ) {
			LSREAL fSin = CMathLib::Sin( fAngle );
			LSREAL fInvSin = LSM_ONE / fSin;
			LSREAL fTAngle = _fT * fAngle;
			LSREAL fCoeff0 = CMathLib::Sin( fAngle - fTAngle ) * fInvSin;
			LSREAL fCoeff1 = CMathLib::Sin( fTAngle ) * fInvSin;

			u.w = fCoeff0 * _qLeft[3] + fCoeff1 * _qRight[3];
			u.x = fCoeff0 * _qLeft[0] + fCoeff1 * _qRight[0];
			u.y = fCoeff0 * _qLeft[1] + fCoeff1 * _qRight[1];
			u.z = fCoeff0 * _qLeft[2] + fCoeff1 * _qRight[2];
		}
		else {
			u.w = _qLeft[3];
			u.x = _qLeft[0];
			u.y = _qLeft[1];
			u.z = _qLeft[2];
		}

		return (*this);*/
	}

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
	CQuaternion & LSE_FCALL CQuaternion::Squad( const CQuaternion &_qQ, const CQuaternion &_qA, const CQuaternion &_qB, const CQuaternion &_qC, LSREAL _fT ) {
		LSREAL fSlerpT = LSM_TWO * _fT * (LSM_ONE - _fT);
		CQuaternion qSlerpP = Slerp( _qQ, _qC, _fT );
		CQuaternion qSlerpQ = Slerp( _qA, _qB, _fT );
		return Slerp( qSlerpP, qSlerpQ, fSlerpT );

	}

	/**
	 * Builds from the rotation part of a 4-by-4 matrix.  Matrix is row-major.
	 *
	 * \param _mMat The matrix from which to build this quaternion.
	 * \return Returns this object after building it.
	 */
	CQuaternion & LSE_FCALL CQuaternion::FromRotationMatrix( const CMatrix4x4 &_mMat ) {
		LSREAL fTrace = _mMat( 0, 0 ) + _mMat( 1, 1 ) + _mMat( 2, 2 );
		LSREAL fRoot;

		if ( fTrace > LSM_ZERO ) {
			// |u.w| > 1/2; may as well choose u.w > 1/2.
			fRoot = CMathLib::Sqrt( fTrace + LSM_ONE );	// 2u.w.
			u.w = LSM_HALF * fRoot;
			fRoot = LSM_HALF / fRoot;  // 1/(4u.w).
			u.x = (_mMat( 1, 2 ) - _mMat( 2, 1 )) * fRoot;
			u.y = (_mMat( 2, 0 ) - _mMat( 0, 2 )) * fRoot;
			u.z = (_mMat( 0, 1 ) - _mMat( 1, 0 )) * fRoot;
		}
		else {
			// |u.w| <= 1/2.
			uint32_t I = 0;
			if ( _mMat( 1, 1 ) > _mMat( 0, 0 ) ) { I = 1; }
			if ( _mMat( 2, 2 ) > _mMat( I, I ) ) { I = 2; }
			uint32_t J = m_ui32Next[I];
			uint32_t K = m_ui32Next[J];

			fRoot = CMathLib::Sqrt( _mMat( I, I ) - _mMat( J, J ) - _mMat( K, K ) + LSM_ONE );
			LSREAL * pfQuat[3] = { &u.x, &u.y, &u.z };
			(*pfQuat[I]) = LSM_HALF * fRoot;
			fRoot = LSM_HALF / fRoot;
			u.w = (_mMat( K, J ) - _mMat( J, K )) * fRoot;
			(*pfQuat[J]) = (_mMat( I, J ) + _mMat( J, I )) * fRoot;
			(*pfQuat[K]) = (_mMat( I, K ) + _mMat( K, I )) * fRoot;
		}

		return (*this);
	}

	/**
	 * Builds from 3 vectors.
	 *
	 * \param _vVectors The right, up, and forward vectors to use for building of the quaternion.
	 * \return Returns this object after building it.
	 */
	CQuaternion & LSE_FCALL CQuaternion::FromRotationVectors( const CVector3 _vVectors[3] ) {
		LSREAL fTrace = _vVectors[0][0] + _vVectors[1][1] + _vVectors[2][2];
		LSREAL fRoot;


		if ( fTrace > LSM_ZERO ) {
			// |u.w| > 1/2; may as well choose u.w > 1/2.
			fRoot = CMathLib::Sqrt( fTrace + LSM_ONE );	// 2u.w.
			u.w = LSM_HALF * fRoot;
			fRoot = LSM_HALF / fRoot;  // 1/(4u.w).
			u.x = (_vVectors[1][2] - _vVectors[2][1]) * fRoot;
			u.y = (_vVectors[2][0] - _vVectors[0][2]) * fRoot;
			u.z = (_vVectors[0][1] - _vVectors[1][0]) * fRoot;
		}
		else {
			// |u.w| <= 1/2.
			uint32_t I = 0;
			if ( _vVectors[1][1] > _vVectors[0][0] ) {
				I = 1;
			}
			if ( _vVectors[2][2] > _vVectors[I][I] ) {
				I = 2;
			}
			uint32_t J = m_ui32Next[I];
			uint32_t K = m_ui32Next[J];

			fRoot = CMathLib::Sqrt( _vVectors[I][I] - _vVectors[J][J] - _vVectors[K][K] + LSM_ONE );
			LSREAL * pfQuat[3] = { &u.x, &u.y, &u.z };
			(*pfQuat[I]) = LSM_HALF * fRoot;
			fRoot = LSM_HALF / fRoot;
			u.w = (_vVectors[J][K] - _vVectors[K][J]) * fRoot;
			(*pfQuat[J]) = (_vVectors[I][J] + _vVectors[J][I]) * fRoot;
			(*pfQuat[K]) = (_vVectors[I][K] + _vVectors[K][I]) * fRoot;
		}
		return (*this);
	}

	/**
	 * Adds a scaled vector.
	 *
	 * \param _vVec The vector to add after being scaled by _fTime.
	 * \param _fTime The amount by which to scale _vVec before adding it to this queternion.
	 * \return Returns this quaternion after the addition.
	 */
	CQuaternion & LSE_FCALL CQuaternion::AddScaledVector( const CVector3 &_vVec, LSREAL _fTime ) {
		CQuaternion qTemp( _vVec.x * _fTime,
			_vVec.y * _fTime,
			_vVec.z * _fTime,
			LSM_ZERO );
		qTemp *= (*this);
		W() += qTemp.W() * LSM_HALF;
		X() += qTemp.X() * LSM_HALF;
		Y() += qTemp.Y() * LSM_HALF;
		Z() += qTemp.Z() * LSM_HALF;
		return (*this);
	}

}	// namespace lsm
