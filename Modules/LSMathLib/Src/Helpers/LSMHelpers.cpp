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
 * Description: Helper/high-level functions.
 */

#include "LSMHelpers.h"


namespace lsm {

	/**
	 * Decompose a matrix into a quaternion and 2 vectors.
	 *
	 * \param _mMatrix The matrix to decompose.
	 * \param _qRot Holds the returned quaternion representing the rotation part
	 *	of the matrix.
	 * \param _vScale Holds the returned vector representing the scale part of the
	 *	matrix.
	 * \param _vPos Holds the returned vector representing the position part of the
	 *	matrix.
	 */
	void LSE_FCALL CHelpers::DecomposeMatrix( const CMatrix4x4 &_mMatrix,
		CQuaternion &_qRot,
		CVector3 &_vScale,
		CVector3 &_vPos ) {


		// Extract the rotation.
		CVector4 vRotTemp[3];
		_mMatrix.GetRow( 0, vRotTemp[0] );
		_mMatrix.GetRow( 1, vRotTemp[1] );
		_mMatrix.GetRow( 2, vRotTemp[2] );

		if ( vRotTemp[0].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[0].w;
			vRotTemp[0].x *= fDet;
			vRotTemp[0].y *= fDet;
			vRotTemp[0].z *= fDet;
		}
		if ( vRotTemp[1].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[1].w;
			vRotTemp[1].x *= fDet;
			vRotTemp[1].y *= fDet;
			vRotTemp[1].z *= fDet;
		}
		if ( vRotTemp[2].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[2].w;
			vRotTemp[2].x *= fDet;
			vRotTemp[2].y *= fDet;
			vRotTemp[2].z *= fDet;
		}
		// Position.
		CVector4 vPos;
		_mMatrix.GetRow( 3, vPos );
		LSREAL fDet = vPos.w ? LSM_ONE / vPos.w : LSM_ONE;
		_vPos.x = vPos.x * fDet;
		_vPos.y = vPos.y * fDet;
		_vPos.z = vPos.z * fDet;

		// Get the scale.
		_vScale.x = vRotTemp[0].Len();
		_vScale.y = vRotTemp[1].Len();
		_vScale.z = vRotTemp[2].Len();

		if ( _mMatrix.Determ() < LSM_ZERO ) {
			_vScale = -_vScale;
		}
		
		// Normalize the rotation.
		CVector3 vRot[3];
		vRot[0] = vRotTemp[0].Vec3();
		vRot[1] = vRotTemp[1].Vec3();
		vRot[2] = vRotTemp[2].Vec3();

		vRot[0].Normalize();
		vRot[1].Normalize();
		vRot[2].Normalize();
		_qRot.FromRotationVectors( vRot );
	}

	/**
	 * Decompose a matrix into 3 rotation vectors, 1 scale vector, and 1 position vector.
	 *
	 * \param _mMatrix The matrix to decompose.
	 * \param _vRor Holds the returned 3-part vector array representing the rotation part
	 *	of the matrix.
	 * \param _vScale Holds the returned vector representing the scale part of the
	 *	matrix.
	 * \param _vPos Holds the returned vector representing the position part of the
	 *	matrix.
	 */
	void LSE_FCALL CHelpers::DecomposeMatrix( const CMatrix4x4 &_mMatrix,
		CVector3 _vRot[3],
		CVector3 &_vScale,
		CVector3 &_vPos ) {
		// Extract the rotation.
		CVector4 vRotTemp[3];
		_mMatrix.GetRow( 0, vRotTemp[0] );
		_mMatrix.GetRow( 1, vRotTemp[1] );
		_mMatrix.GetRow( 2, vRotTemp[2] );

		if ( vRotTemp[0].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[0].w;
			vRotTemp[0].x *= fDet;
			vRotTemp[0].y *= fDet;
			vRotTemp[0].z *= fDet;
		}
		if ( vRotTemp[1].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[1].w;
			vRotTemp[1].x *= fDet;
			vRotTemp[1].y *= fDet;
			vRotTemp[1].z *= fDet;
		}
		if ( vRotTemp[2].w ) {
			LSREAL fDet = LSM_ONE / vRotTemp[2].w;
			vRotTemp[2].x *= fDet;
			vRotTemp[2].y *= fDet;
			vRotTemp[2].z *= fDet;
		}
		// Position.
		CVector4 vPos;
		_mMatrix.GetRow( 3, vPos );
		LSREAL fDet = vPos.w ? LSM_ONE / vPos.w : LSM_ONE;
		_vPos.x = vPos.x * fDet;
		_vPos.y = vPos.y * fDet;
		_vPos.z = vPos.z * fDet;

		// Get the scale.
		_vScale.x = vRotTemp[0].Len();
		_vScale.y = vRotTemp[1].Len();
		_vScale.z = vRotTemp[2].Len();

		if ( _mMatrix.Determ() < LSM_ZERO ) {
			_vScale = -_vScale;
		}
		
		// Normalize the rotation.
		_vRot[0] = vRotTemp[0].Vec3();
		_vRot[1] = vRotTemp[1].Vec3();
		_vRot[2] = vRotTemp[2].Vec3();

		_vRot[0].Normalize();
		_vRot[1].Normalize();
		_vRot[2].Normalize();
	}

	/**
	 * Interpolate linearly between a given quaternion, scale, and position, and a
	 *	target matrix.  The matrix is decomposed internally and this function is
	 *	expensive.
	 *
	 * \param _mMatrix The matrix to compose.
	 * \param _qRot The original orientation.
	 * \param _vScale The original scale.
	 * \param _vPos The original position.
	 * \param _mTarget The target transform stored as a matrix.
	 * \param _fFrac Fraction between 0 and 1 indicating how far to interpolate between the
	 *	source and target orientations.
	 * \return The given matrix is returned.  In this way, the function can be used
	 *	as a parameter for another function.
	 */
	CMatrix4x4 & LSE_FCALL CHelpers::TransformInterp( CMatrix4x4 &_mMatrix,
		const CQuaternion &_qRot,
		const CVector3 &_vScale,
		const CVector3 &_vPos,
		const CMatrix4x4 &_mTarget,
		LSREAL _fFrac ) {

		CQuaternion qTargetOrientation;
		CVector3 vTargetScale;
		CVector3 vTargetPos;
		DecomposeMatrix( _mTarget, qTargetOrientation, vTargetScale, vTargetPos );

		CQuaternion qFinal;
		qFinal.Lerp( _qRot, qTargetOrientation, _fFrac );
		CVector3 vFinalScale;
		vFinalScale.CVector3::Lerp( _vScale, vTargetScale, _fFrac );
		CVector3 vFinalPos;
		vFinalPos.CVector3::Lerp( _vPos, vTargetPos, _fFrac );

		return ComposeMatrix( _mMatrix,
			qFinal, vFinalScale, vFinalPos );
	}

	/**
	 * Interpolate linearly between one orientation and another.  The result is a transform
	 *	matrix.
	 *
	 * \param _mMatrix The matrix to compose.
	 * \param _qRot The original orientation.
	 * \param _vScale The original scale.
	 * \param _vPos The original position.
	 * \param _qTargetRot The target orientation.
	 * \param _vTargetScale The target scale.
	 * \param _vTargetPos The target position.
	 * \param _fFrac Fraction between 0 and 1 indicating how far to interpolate between the
	 *	source and target orientations.
	 * \return The given matrix is returned.  In this way, the function can be used
	 *	as a parameter for another function.
	 */
	CMatrix4x4 & LSE_FCALL CHelpers::TransformInterp( CMatrix4x4 &_mMatrix,
		const CQuaternion &_qRot,
		const CVector3 &_vScale,
		const CVector3 &_vPos,
		const CQuaternion &_qTargetRot,
		const CVector3 &_vTargetScale,
		const CVector3 &_vTargetPos,
		LSREAL _fFrac ) {

		CQuaternion qFinal;
		qFinal.Slerp( _qRot, _qTargetRot, _fFrac );
		CVector3 vFinalScale;
		vFinalScale.CVector3::Lerp( _vScale, _vTargetScale, _fFrac );
		CVector3 vFinalPos;
		vFinalPos.CVector3::Lerp( _vPos, _vTargetPos, _fFrac );

		return ComposeMatrix( _mMatrix,
			qFinal, vFinalScale, vFinalPos );
	}

	/**
	 * Returns the interpolated point between the non-uniform 4-sided shape.
	 *
	 * \param _vP0 The upper-left corner of the shape.
	 * \param _vP1 The upper-right corner of the shape.
	 * \param _vP2 The lower-left corner of the shape.
	 * \param _vP3 The lower-right corner of the shape.
	 * \param _fX The interpolation along the X axis.
	 * \param _fY The interpolation along the Y axis.
	 * \return Returns the point at the requested interpolation values.
	 */
	CVector2 LSE_CALL CHelpers::InterpolateNonUniformQuad( const CVector2 &_vP0, const CVector2 &_vP1, const CVector2 &_vP2, const CVector2 &_vP3,
		LSREAL _fX, LSREAL _fY ) {
		// Handle the X.
		LSREAL fStart = _vP1.x - _vP0.x;
		LSREAL fEnd = _vP3.x - _vP2.x;
		LSREAL fX = fStart * _fX + (fEnd - fStart) * _fY;
		fStart = _vP2.x - _vP0.x;
		fX = (_vP0.x + fStart * _fY) + fX;


		// Handle the Y.
		fStart = _vP2.y - _vP0.y;
		fEnd = _vP3.y - _vP1.y;
		LSREAL fY = fStart * _fY + (fEnd - fStart) * _fX;
		fStart = _vP1.y - _vP0.y;
		fY = (_vP0.y + fStart * _fX) + fY;

		return CVector2( fX, fY );
	}

}	// namespace lsm
