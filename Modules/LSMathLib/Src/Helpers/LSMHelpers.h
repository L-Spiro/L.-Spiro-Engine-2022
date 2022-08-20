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



#ifndef __LSM_HELPERS_H__
#define __LSM_HELPERS_H__

#include "../LSMMathLib.h"
#include "../Matrix/LSMMatrix3x3.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Quaternion/LSMQuaternion.h"
#include "../Vector/LSMVector2.h"
#include "../Vector/LSMVector3.h"

namespace lsm {

	/**
	 * Class CCamera
	 * \brief Helper/high-level functions.
	 *
	 * Description: Contains useful math routines for general-purpose use.
	 */
	class CHelpers {
	public :
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
		static void LSE_FCALL			DecomposeMatrix( const CMatrix4x4 &_mMatrix,
			CQuaternion &_qRot,
			CVector3 &_vScale,
			CVector3 &_vPos );

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
		static void LSE_FCALL			DecomposeMatrix( const CMatrix4x4 &_mMatrix,
			CVector3 _vRot[3],
			CVector3 &_vScale,
			CVector3 &_vPos );

		/**
		 * Compose a matrix from a quaternion and a positional vector.  The matrix
		 *	has no scaling component.
		 *
		 * \param _mMatrix The matrix to compose.
		 * \param _qRot The orientation.
		 * \param _vPos The position.
		 * \return The given matrix is returned.  In this way, the function can be used
		 *	as a parameter for another function.
		 */
		static CMatrix4x4 & LSE_FCALL	ComposeMatrix( CMatrix4x4 &_mMatrix,
			const CQuaternion &_qRot,
			const CVector3 &_vPos );

		/**
		 * Compose a matrix from a quaternion, scaling vector, and a positional vector.
		 *
		 * \param _mMatrix The matrix to compose.
		 * \param _qRot The orientation.
		 * \param _vScale The scale.
		 * \param _vPos The position.
		 * \return The given matrix is returned.  In this way, the function can be used
		 *	as a parameter for another function.
		 */
		static CMatrix4x4 & LSE_FCALL	ComposeMatrix( CMatrix4x4 &_mMatrix,
			const CQuaternion &_qRot,
			const CVector3 &_vScale,
			const CVector3 &_vPos );

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
		static CMatrix4x4 & LSE_FCALL	TransformInterp( CMatrix4x4 &_mMatrix,
			const CQuaternion &_qRot,
			const CVector3 &_vScale,
			const CVector3 &_vPos,
			const CMatrix4x4 &_mTarget,
			LSREAL _fFrac );

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
		static CMatrix4x4 & LSE_FCALL	TransformInterp( CMatrix4x4 &_mMatrix,
			const CQuaternion &_qRot,
			const CVector3 &_vScale,
			const CVector3 &_vPos,
			const CQuaternion &_qTargetRot,
			const CVector3 &_vTargetScale,
			const CVector3 &_vTargetPos,
			LSREAL _fFrac );

		/**
		 * Transform an inertia tensor matrix by a quaternion.
		 *
		 * \param _mWorld Holds the result of the transform.
		 * \param _mInvInertiaTensor The inverse inertia tensor.
		 * \param _mRot A rotation matrix.
		 */
		static void LSE_FCALL			TransformInertiaTensor( CMatrix3x3 &_mWorld,
			const CMatrix3x3 &_mInvInertiaTensor,
			const CMatrix4x4 &_mRot );

		/**
		 * Create an orthonormal basis in which the X vector is given (and assumed
		 *	normalized).  By being orthonormal, the resulting 3 vectors will be
		 *	perpendiular to each other and unit length.
		 *
		 * \param _vX The input X vector.
		 * \param _vY The output Y vector.
		 * \param _vZ The output Z vector.
		 */
		static void LSE_FCALL			MakeOrthonormalBasis( const CVector3 &_vX,
			CVector3 &_vY,
			CVector3 &_vZ );

		/**
		 * Create an orthonormal basis in which the X vector is given (and assumed
		 *	normalized).  By being orthonormal, the resulting 3 vectors will be
		 *	perpendiular to each other and unit length.
		 *
		 * \param _vX The input X vector.
		 * \param _vY The output Y vector.
		 * \param _vZ The output Z vector.
		 */
		template <typename _tType, typename _tVec3Type>
		static void LSE_FCALL			MakeOrthonormalBasisT( const _tVec3Type &_vX,
			_tVec3Type &_vY,
			_tVec3Type &_vZ );

		/**
		 * Create an orthonormal basis in which the X vector is given (and assumed
		 *	normalized).  By being orthonormal, the resulting 3 vectors will be
		 *	perpendiular to each other and unit length.  This is more accurate than
		 *	MakeOrthonormalBasis().
		 *
		 * \param _vX The input X vector.
		 * \param _vY The output Y vector.
		 * \param _vZ The output Z vector.
		 */
		static void LSE_FCALL			MakeOrthonormalBasisSlow( const CVector3 &_vX,
			CVector3 &_vY,
			CVector3 &_vZ );

		/**
		 * Given an X and a Y, returns a positive Z that creates a unit-length vector.
		 *
		 * \param _fX The Z value.
		 * \param _fY The Y value.
		 * \return Returns a positive Z that creates a unit-length vector.
		 */
		static LSREAL LSE_FCALL			GetNormalizedZ( LSREAL _fX, LSREAL _fY );

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
		static CVector2 LSE_CALL		InterpolateNonUniformQuad( const CVector2 &_vP0, const CVector2 &_vP1, const CVector2 &_vP2, const CVector2 &_vP3,
			LSREAL _fX, LSREAL _fY );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Compose a matrix from a quaternion and a positional vector.  The matrix
	 *	has no scaling component.
	 *
	 * \param _mMatrix The matrix to compose.
	 * \param _qRot The orientation.
	 * \param _vPos The position.
	 * \return The given matrix is returned.  In this way, the function can be used
	 *	as a parameter for another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_FCALL CHelpers::ComposeMatrix( CMatrix4x4 &_mMatrix,
		const CQuaternion &_qRot,
		const CVector3 &_vPos ) {

		_qRot.ToRotationMatrix( _mMatrix );
		_mMatrix( 0, 3 ) = LSM_ZERO;
		_mMatrix( 1, 3 ) = LSM_ZERO;
		_mMatrix( 2, 3 ) = LSM_ZERO;
		_mMatrix( 3, 3 ) = LSM_ONE;
		_mMatrix.SetRow( 3, _vPos );
		return _mMatrix;
	}

	/**
	 * Compose a matrix from a quaternion, scaling vector, and a positional vector.
	 *
	 * \param _mMatrix The matrix to compose.
	 * \param _qRot The orientation.
	 * \param _vScale The scale.
	 * \param _vPos The position.
	 * \return The given matrix is returned.  In this way, the function can be used
	 *	as a parameter for another function.
	 */
	LSE_INLINE CMatrix4x4 & LSE_FCALL CHelpers::ComposeMatrix( CMatrix4x4 &_mMatrix,
		const CQuaternion &_qRot,
		const CVector3 &_vScale,
		const CVector3 &_vPos ) {
		_qRot.ToRotationMatrix( _mMatrix );
		_mMatrix( 0, 0 ) *= _vScale.x;
		_mMatrix( 0, 1 ) *= _vScale.x;
		_mMatrix( 0, 2 ) *= _vScale.x;
		_mMatrix( 1, 0 ) *= _vScale.y;
		_mMatrix( 1, 1 ) *= _vScale.y;
		_mMatrix( 1, 2 ) *= _vScale.y;
		_mMatrix( 2, 0 ) *= _vScale.z;
		_mMatrix( 2, 1 ) *= _vScale.z;
		_mMatrix( 2, 2 ) *= _vScale.z;
		_mMatrix( 0, 3 ) = LSM_ZERO;
		_mMatrix( 1, 3 ) = LSM_ZERO;
		_mMatrix( 2, 3 ) = LSM_ZERO;
		_mMatrix( 3, 3 ) = LSM_ONE;
		_mMatrix.SetRow( 3, _vPos );
		return _mMatrix;
	}

	/**
	 * Transform an inertia tensor matrix by a quaternion.
	 *
	 * \param _mWorld Holds the result of the transform.
	 * \param _mInvInertiaTensor The inverse inertia tensor.
	 * \param _mRot A rotation matrix.
	 */
	LSE_INLINE void LSE_FCALL CHelpers::TransformInertiaTensor( CMatrix3x3 &_mWorld,
		const CMatrix3x3 &_mInvInertiaTensor,
		const CMatrix4x4 &_mRot ) {

		LSREAL fT0 = _mRot( 0, 0 ) * _mInvInertiaTensor( 0, 0 ) +
			_mRot( 1, 0 ) * _mInvInertiaTensor( 0, 1 ) +
			_mRot( 2, 0 ) * _mInvInertiaTensor( 0, 2 );
		LSREAL fT1 = _mRot( 0, 0 ) * _mInvInertiaTensor( 1, 0 ) +
			_mRot( 1, 0 ) * _mInvInertiaTensor( 1, 1 ) +
			_mRot( 2, 0 ) * _mInvInertiaTensor( 1, 2 );
		LSREAL fT2 = _mRot( 0, 0 ) * _mInvInertiaTensor( 2, 0 ) +
			_mRot( 1, 0 ) * _mInvInertiaTensor( 2, 1 ) +
			_mRot( 2, 0 ) * _mInvInertiaTensor( 2, 2 );
		LSREAL fT3 = _mRot( 0, 1 ) * _mInvInertiaTensor( 0, 0 ) +
			_mRot( 1, 1 ) * _mInvInertiaTensor( 0, 1 ) +
			_mRot( 2, 1 ) * _mInvInertiaTensor( 0, 2 );
		LSREAL fT4 = _mRot( 0, 1 ) * _mInvInertiaTensor( 1, 0 ) +
			_mRot( 1, 1 ) * _mInvInertiaTensor( 1, 1 ) +
			_mRot( 2, 1 ) * _mInvInertiaTensor( 1, 2 );
		LSREAL fT5 = _mRot( 0, 1 ) * _mInvInertiaTensor( 2, 0 ) +
			_mRot( 1, 1 ) * _mInvInertiaTensor( 2, 1 ) +
			_mRot( 2, 1 ) * _mInvInertiaTensor( 2, 2 );
		LSREAL fT6 = _mRot( 0, 2 ) * _mInvInertiaTensor( 0, 0 ) +
			_mRot( 1, 2 ) * _mInvInertiaTensor( 0, 1 ) +
			_mRot( 2, 2 ) * _mInvInertiaTensor( 0, 2 );
		LSREAL fT7 = _mRot( 0, 2 ) * _mInvInertiaTensor( 1, 0 ) +
			_mRot( 1, 2 ) * _mInvInertiaTensor( 1, 1 ) +
			_mRot( 2, 2 ) * _mInvInertiaTensor( 1, 2 );
		LSREAL fT8 = _mRot( 0, 2 ) * _mInvInertiaTensor( 2, 0 ) +
			_mRot( 1, 2 ) * _mInvInertiaTensor( 2, 1 ) +
			_mRot( 2, 2 ) * _mInvInertiaTensor( 2, 2 );

		_mWorld( 0, 0 ) = fT0 * _mRot( 0, 0 ) +
			fT1 * _mRot( 1, 0 ) +
			fT2 * _mRot( 2, 0 );
		_mWorld( 1, 0 ) = fT0 * _mRot( 0, 1 ) +
			fT1 * _mRot( 1, 1 ) +
			fT2 * _mRot( 2, 1 );
		_mWorld( 2, 0 ) = fT0 * _mRot( 0, 2 ) +
			fT1 * _mRot( 1, 2 ) +
			fT2 * _mRot( 2, 2 );
		_mWorld( 0, 1 ) = fT3 * _mRot( 0, 0 ) +
			fT4 * _mRot( 1, 0 ) +
			fT5 * _mRot( 2, 0 );
		_mWorld( 1, 1 ) = fT3 * _mRot( 0, 1 ) +
			fT4 * _mRot( 1, 1 ) +
			fT5 * _mRot( 2, 1 );
		_mWorld( 2, 1 ) = fT3 * _mRot( 0, 2 ) +
			fT4 * _mRot( 1, 2 ) +
			fT5 * _mRot( 2, 2 );
		_mWorld( 0, 2 ) = fT6 * _mRot( 0, 0 ) +
			fT7 * _mRot( 1, 0 ) +
			fT8 * _mRot( 2, 0 );
		_mWorld( 1, 2 ) = fT6 * _mRot( 0, 1 ) +
			fT7 * _mRot( 1, 1 ) +
			fT8 * _mRot( 2, 1 );
		_mWorld( 2, 2 ) = fT6 * _mRot( 0, 2 ) +
			fT7 * _mRot( 1, 2 ) +
			fT8 * _mRot( 2, 2 );
	}

	/**
	 * Create an orthonormal basis in which the X vector is given (and assumed
	 *	normalized).  By being orthonormal, the resulting 3 vectors will be
	 *	perpendiular to each other and unit length.
	 *
	 * \param _vX The input X vector.
	 * \param _vY The output Y vector.
	 * \param _vZ The output Z vector.
	 */
	LSE_INLINE void LSE_FCALL CHelpers::MakeOrthonormalBasis( const CVector3 &_vX,
		CVector3 &_vY,
		CVector3 &_vZ ) {
		// Check whether the X axis is nearer to the X or Y axis.
		if ( CMathLib::Abs( _vX.x ) > CMathLib::Abs( _vX.y ) ) { 
			// Scaling factor to ensure the results are normalized.
			const LSREAL fS = -CMathLib::InvSqrt( _vX.z * _vX.z +
				_vX.x * _vX.x );

			// The new Y axis is at right angles to the world Y axis.
			_vZ.x = _vX.z * fS;
			_vZ.y = LSM_ZERO;
			_vZ.z = -_vX.x * fS;

			// The new Z axis is at right angles to the new X- and Y- axes.
			_vY.x = -_vX.y * _vZ.x;
			_vY.y = -(_vX.z * _vZ.x -
				_vX.x * _vZ.z);
			_vY.z = _vX.y * _vZ.x;
		}
		else {
			// Scaling factor to ensure the results are normalized.
			const LSREAL fS = -CMathLib::InvSqrt( _vX.z * _vX.z + 
				_vX.y * _vX.y );

			// The new Y axis is at right angles to the world X axis.
			_vZ.x = LSM_ZERO;
			_vZ.y = -_vX.z * fS;
			_vZ.z = _vX.y * fS;

			// The new Z axis is at right angles to the new X- and Y- axes.
			_vY.x = -(_vX.y * _vZ.z - 
				_vX.z * _vZ.y);
			_vY.y = _vX.x * _vZ.z;
			_vY.z = -_vX.x * _vZ.y;
		}
	}

	/**
	 * Create an orthonormal basis in which the X vector is given (and assumed
	 *	normalized).  By being orthonormal, the resulting 3 vectors will be
	 *	perpendiular to each other and unit length.
	 *
	 * \param _vX The input X vector.
	 * \param _vY The output Y vector.
	 * \param _vZ The output Z vector.
	 */
	template <typename _tType, typename _tVec3Type>
	void LSE_FCALL CHelpers::MakeOrthonormalBasisT( const _tVec3Type &_vX,
		_tVec3Type &_vY,
		_tVec3Type &_vZ ) {
		// Check whether the X axis is nearer to the X or Y axis.
		if ( CMathLib::AbsT( _vX.x ) > CMathLib::AbsT( _vX.y ) ) { 
			// Scaling factor to ensure the results are normalized.
			const _tType fS = _tType( -1.0 ) / _tType( ::sqrt( _vX.z * _vX.z +
				_vX.x * _vX.x ) );

			// The new Y axis is at right angles to the world Y axis.
			_vZ.x = _vX.z * fS;
			_vZ.y = _tType( 0.0 );
			_vZ.z = -_vX.x * fS;

			// The new Z axis is at right angles to the new X- and Y- axes.
			_vY.x = -_vX.y * _vZ.x;
			_vY.y = -(_vX.z * _vZ.x -
				_vX.x * _vZ.z);
			_vY.z = _vX.y * _vZ.x;
		}
		else {
			// Scaling factor to ensure the results are normalized.
			const _tType fS = _tType( -1.0 ) / _tType( ::sqrt( _vX.z * _vX.z + 
				_vX.y * _vX.y ) );

			// The new Y axis is at right angles to the world X axis.
			_vZ.x = _tType( 0.0 );
			_vZ.y = -_vX.z * fS;
			_vZ.z = _vX.y * fS;

			// The new Z axis is at right angles to the new X- and Y- axes.
			_vY.x = -(_vX.y * _vZ.z - 
				_vX.z * _vZ.y);
			_vY.y = _vX.x * _vZ.z;
			_vY.z = -_vX.x * _vZ.y;
		}
	}

	/**
	 * Create an orthonormal basis in which the X vector is given (and assumed
	 *	normalized).  By being orthonormal, the resulting 3 vectors will be
	 *	perpendiular to each other and unit length.  This is more accurate than
	 *	MakeOrthonormalBasis().
	 *
	 * \param _vX The input X vector.
	 * \param _vY The output Y vector.
	 * \param _vZ The output Z vector.
	 */
	LSE_INLINE void LSE_FCALL CHelpers::MakeOrthonormalBasisSlow( const CVector3 &_vX,
		CVector3 &_vY,
		CVector3 &_vZ ) {

		// Check whether the Z axis is nearer to the X or Y axis.
		if ( CMathLib::Abs( _vX.x ) > CMathLib::Abs( _vX.y ) ) {
			_vZ = _vX % CVector3( LSM_ZERO, LSM_ONE, LSM_ZERO );
		}
		else {
			_vZ = _vX % CVector3( LSM_ONE, LSM_ZERO, LSM_ZERO );
		}
		_vY = _vZ % _vX;

		_vZ.Normalize();
		_vY.Normalize();
	}

	/**
	 * Given an X and a Y, returns a positive Z that creates a unit-length vector.
	 *
	 * \param _fX The Z value.
	 * \param _fY The Y value.
	 * \return Returns a positive Z that creates a unit-length vector.
	 */
	LSE_INLINE LSREAL LSE_FCALL CHelpers::GetNormalizedZ( LSREAL _fX, LSREAL _fY ) {
		LSREAL fTemp = LSM_ONE - ((_fX * _fX) + (_fY * _fY));
		if ( CMathLib::Equals( fTemp, LSM_ZERO, LSM_REAL_EPSILON ) ) { return LSM_ZERO; }
		return CMathLib::Sqrt( fTemp );
	}

}	// namespace lsm

#endif	// __LSM_HELPERS_H__
