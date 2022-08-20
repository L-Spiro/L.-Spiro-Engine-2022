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
 * Description: A base class for all objects that can have an orientation in 3D.
 */

#include "LSMOrientation.h"


namespace lsm {


	/**
	 * Sets the orientation to identity.
	 */
	void LSE_FCALL COrientation::SetIdentity() {
		m_vRight.Set( LSM_ONE, LSM_ZERO, LSM_ZERO );
		m_vUp.Set( LSM_ZERO, LSM_ONE, LSM_ZERO );
		m_vForward.Set( LSM_ZERO, LSM_ZERO, LSM_ONE );
		m_vPos.Set( LSM_ZERO, LSM_ZERO, LSM_ZERO );
		m_vScale.Set( LSM_ONE, LSM_ONE, LSM_ONE );
		m_ui32Dirty |= LSM_ODF_POSITION | LSM_ODF_ROTATION | LSM_ODF_SCALE;
	}

	/**
	 * Sets the position to [0,0,0].
	 */
	void LSE_FCALL COrientation::ClearPosition() {
		m_vPos.Set( LSM_ZERO, LSM_ZERO, LSM_ZERO );
		m_ui32Dirty |= LSM_ODF_POSITION;
	}

	/**
	 * Sets the rotation to identity.
	 */
	void LSE_FCALL COrientation::ClearRotation() {
		m_vRight.Set( LSM_ONE, LSM_ZERO, LSM_ZERO );
		m_vUp.Set( LSM_ZERO, LSM_ONE, LSM_ZERO );
		m_vForward.Set( LSM_ZERO, LSM_ZERO, LSM_ONE );
		m_ui32Dirty |= LSM_ODF_ROTATION;
	}

	/**
	 * Sets the scale to [1,1,1].
	 */
	void LSE_FCALL COrientation::ClearScale() {
		m_vScale.Set( LSM_ONE, LSM_ONE, LSM_ONE );
		m_ui32Dirty |= LSM_ODF_SCALE;
	}

	/**
	 * Sets the absolute position.  Only causes a change if the given position is not the same as the
	 *	current position.
	 *
	 * \param _vPos The position to apply to this orientation.
	 */
	void LSE_FCALL COrientation::SetPosU( const CVector3 &_vPos ) {
		if ( m_vPos == _vPos ) { return; }
		Pos() = _vPos;
	}

	/**
	 * Sets the position of this orientation relative to its facing direction.
	 *
	 * \param _vPos The position to apply to this orientation.
	 */
	void LSE_FCALL COrientation::SetRelPos( const CVector3 &_vPos ) {
		m_vPos = (m_vRight * _vPos.x) +
			(m_vUp * _vPos.y) +
			(m_vForward * _vPos.z);
		m_ui32Dirty |= LSM_ODF_POSITION;
	}

	/**
	 * Offset the position of this orientation relative to its facing direction.
	 *
	 * \param _vPos The position to add to this orientation.
	 */
	void LSE_FCALL COrientation::AddRelPos( const CVector3 &_vPos ) {
		m_vPos += (m_vRight * _vPos.x) +
			(m_vUp * _vPos.y) +
			(m_vForward * _vPos.z);
		m_ui32Dirty |= LSM_ODF_POSITION;
	}

	/**
	 * Sets the rotation component of this orientation.
	 *
	 * \param _mMat The full rotational component to apply to this object.
	 */
	void LSE_FCALL COrientation::SetRotation( const CMatrix4x4 &_mMat ) {
		m_vRight.Set( _mMat._11, _mMat._12, _mMat._13 );
		m_vUp.Set( _mMat._21, _mMat._22, _mMat._23 );
		m_vForward.Set( _mMat._31, _mMat._32, _mMat._33 );
		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Adds a rotation to the existing rotation of this object.
	 *
	 * \param _mMat The rotation, in matrix form, to add to the existing rotation of this object.
	 */
	void LSE_FCALL COrientation::AddRotation( const CMatrix4x4 &_mMat ) {
		m_vRight.Set(
			CVector3::DotProduct( m_vRight, CVector3( _mMat._11, _mMat._21, _mMat._31 ) ),
			CVector3::DotProduct( m_vRight, CVector3( _mMat._12, _mMat._22, _mMat._32 ) ),
			CVector3::DotProduct( m_vRight, CVector3( _mMat._13, _mMat._23, _mMat._33 ) ) );

		m_vForward.Set(
			CVector3::DotProduct( m_vForward, CVector3( _mMat._11, _mMat._21, _mMat._31 ) ),
			CVector3::DotProduct( m_vForward, CVector3( _mMat._12, _mMat._22, _mMat._32 ) ),
			CVector3::DotProduct( m_vForward, CVector3( _mMat._13, _mMat._23, _mMat._33 ) ) );

		m_vUp.Set(
			CVector3::DotProduct( m_vUp, CVector3( _mMat._11, _mMat._21, _mMat._31 ) ),
			CVector3::DotProduct( m_vUp, CVector3( _mMat._12, _mMat._22, _mMat._32 ) ),
			CVector3::DotProduct( m_vUp, CVector3( _mMat._13, _mMat._23, _mMat._33 ) ) );

		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Sets the rotation component of this object from forward and up vectors.  The right vector
	 *	is derived automatically.
	 *
	 * \param _vForward The forward component of the rotation.
	 * \param _vUp The up component of the rotation.
	 */
	void LSE_FCALL COrientation::SetRotation( const CVector3 &_vForward, const CVector3 &_vUp ) {
		m_vForward = _vForward;
		m_vUp = _vUp;
		m_vRight = _vUp % _vForward;
		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Sets the rotation component of this object from forward and up vectors.  The right vector
	 *	is derived automatically and the up vector is adjusted.
	 *
	 * \param _vForward The forward component of the rotation.
	 * \param _vUp The up component of the rotation.
	 */
	void LSE_FCALL COrientation::SetRotationEasy( const CVector3 &_vForward, const CVector3 &_vUp ) {
		m_vForward = _vForward;
		m_vRight = _vUp % _vForward;
		m_vUp = _vForward % m_vRight;
		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Adds a rotation to this object from forward and up vectors.  The right vector is derived
	 *	automatically.
	 *
	 * \param _vForward The forward component of the rotation to add.
	 * \param _vUp The up component of the rotation to add.
	 */
	void LSE_FCALL COrientation::AddRotation( const CVector3 &_vForward, const CVector3 &_vUp ) {
		CVector3 vRight( _vUp % _vForward );

		m_vRight.Set(
			CVector3::DotProduct( m_vRight, CVector3( vRight.x, _vUp.x, _vForward.x ) ),
			CVector3::DotProduct( m_vRight, CVector3( vRight.y, _vUp.y, _vForward.y ) ),
			CVector3::DotProduct( m_vRight, CVector3( vRight.z, _vUp.z, _vForward.z ) ) );

		m_vForward.Set(
			CVector3::DotProduct( m_vForward, CVector3( vRight.x, _vUp.x, _vForward.x ) ),
			CVector3::DotProduct( m_vForward, CVector3( vRight.y, _vUp.y, _vForward.y ) ),
			CVector3::DotProduct( m_vForward, CVector3( vRight.z, _vUp.z, _vForward.z ) ) );

		m_vUp.Set(
			CVector3::DotProduct( m_vUp, CVector3( vRight.x, _vUp.x, _vForward.x ) ),
			CVector3::DotProduct( m_vUp, CVector3( vRight.y, _vUp.y, _vForward.y ) ),
			CVector3::DotProduct( m_vUp, CVector3( vRight.z, _vUp.z, _vForward.z ) ) );
		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Yaw the orientation by the given amount.  The object always rotates around axis [0,1,0].
	 *
	 * \param _fAmount Amount, in radians, to yaw.
	 */
	void LSE_FCALL COrientation::Yaw( LSREAL _fAmount ) {
		CMatrix4x4 mRot;
		mRot.MatrixRotationYLH( _fAmount );
		AddRotation( mRot );
	}

	/**
	 * Sets the yaw to the given amount.  The object always rotates around axis [0,1,0].
	 *
	 * \param _fAmount Amount, in radians, to which to set the yaw.
	 */
	void LSE_FCALL COrientation::SetYaw( LSREAL _fAmount ) {
		LSREAL fS;
		LSREAL fC;
		CMathLib::SinCos( _fAmount, fS, fC );

		CVector3 vForward;
		vForward.x = fS;
		vForward.y = LSM_ZERO;
		vForward.x = fC;

		m_vRight = vForward % m_vUp;
		m_vForward = m_vUp % m_vRight;
		m_vUp = m_vRight % m_vUp;

		m_ui32Dirty |= (LSM_ODF_ROTATION | LSM_ODF_NORMALIZATION);
	}

	/**
	 * Pitch the orientation by the given amount.  The object always rotates around its right axis.
	 *
	 * \param _fAmount Amount, in radians, to pitch.
	 */
	void LSE_FCALL COrientation::Pitch( LSREAL _fAmount ) {
		CMatrix4x4 mRot;
		mRot.MatrixRotationAxis( m_vRight, _fAmount );
		AddRotation( mRot );
	}

	/**
	 * Roll the orientation by the given amount.  The object always rotates around its forward axis.
	 *
	 * \param _fAmount Amount, in radians, to roll.
	 */
	void LSE_FCALL COrientation::Roll( LSREAL _fAmount ) {
		CMatrix4x4 mRot;
		mRot.MatrixRotationAxis( m_vForward, _fAmount );
		AddRotation( mRot );
	}

	/**
	 * Sets the rotation by the given yaw, pitch, and roll.
	 *
	 * \param _fYaw Yaw around the Y axis, in radians.
	 * \param _fPitch Pitch around the X axis, in radians.
	 * \param _fRoll Roll around the Z axis, in radians.
	 */
	void LSE_FCALL COrientation::SetRotationYawPitchRoll( LSREAL _fYaw, LSREAL _fPitch, LSREAL _fRoll ) {
		CMatrix4x4 mRot;
		mRot.MatrixRotationYawPitchRoll( _fYaw, _fPitch, _fRoll );
		SetRotation( mRot );
	}

	/**
	 * Builds a matrix from our data.
	 *
	 * \param _mRet Holds the created matrix.
	 */
	void LSE_FCALL COrientation::BuildMatrix( CMatrix4x4 &_mRet ) const {
		_mRet._11 = m_vRight.x * m_vScale.x;
		_mRet._12 = m_vRight.y * m_vScale.x;
		_mRet._13 = m_vRight.z * m_vScale.x;
		_mRet._14 = LSM_ZERO;

		_mRet._21 = m_vUp.x * m_vScale.y;
		_mRet._22 = m_vUp.y * m_vScale.y;
		_mRet._23 = m_vUp.z * m_vScale.y;
		_mRet._24 = LSM_ZERO;

		_mRet._31 = m_vForward.x * m_vScale.z;
		_mRet._32 = m_vForward.y * m_vScale.z;
		_mRet._33 = m_vForward.z * m_vScale.z;
		_mRet._34 = LSM_ZERO;

		_mRet._41 = m_vPos.x;
		_mRet._42 = m_vPos.y;
		_mRet._43 = m_vPos.z;
		_mRet._44 = LSM_ONE;
	}

	/**
	 * Updates, if necessary, the orientation, filling the given matrix with the result.  If the orientation
	 *	has not been modified since the last update, no action is performed.
	 *
	 * \param _mRet Holds the created matrix.
	 * \param _bClearDirty Determines whether the dirty flag will be cleared after this update.  This
	 *	function only updates the given matrix if the dirty flag is set, so clearing the dirty flag
	 *	results in no more updates until the orientation is modified in some way.
	 *	In this manner, updates are only performed when needed.
	 * \return Returns true if an update was performed.
	 */
	LSBOOL LSE_FCALL COrientation::UpdateMatrix( CMatrix4x4 &_mRet, LSBOOL _bClearDirty ) const {
		if ( m_ui32Dirty != 0 ) {
			UndirtyRotations();
			BuildMatrix( _mRet );
			if ( _bClearDirty ) { m_ui32Dirty = 0; }
			return true;
		}
		return false;
	}

}	// namespace lsm
