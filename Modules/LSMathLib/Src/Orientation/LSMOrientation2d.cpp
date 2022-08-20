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
 * Description: A 2D orientation.
 */

#include "LSMOrientation2d.h"


namespace lsm {


	/**
	 * Sets the orientation to identity.
	 */
	void LSE_FCALL COrientation2d::SetIdentity() {
		m_fRot = LSM_ZERO;
		m_vPos.Set( LSM_ZERO, LSM_ZERO );
		m_vScale.Set( LSM_ONE, LSM_ONE );
		m_ui32Dirty = LSM_2DODF_POSITION | LSM_2DODF_ROTATION | LSM_2DODF_SCALE;
	}

	/**
	 * Sets the position to [0,0].
	 */
	void LSE_FCALL COrientation2d::ClearPosition() {
		m_vPos.Set( LSM_ZERO, LSM_ZERO );
		m_ui32Dirty |= LSM_2DODF_POSITION;
	}

	/**
	 * Sets the rotation to identity.
	 */
	void LSE_FCALL COrientation2d::ClearRotation() {
		if ( m_fRot ) {
			m_fRot = LSM_ZERO;
			m_ui32Dirty |= LSM_2DODF_ROTATION;
		}
	}

	/**
	 * Sets the scale to [1,1].
	 */
	void LSE_FCALL COrientation2d::ClearScale() {
		m_vScale.Set( LSM_ONE, LSM_ONE );
		m_ui32Dirty |= LSM_2DODF_SCALE;
	}

	/**
	 * Sets the absolute position.  Only causes a change if the given position is not the same as the
	 *	current position.
	 *
	 * \param _vPos The position to apply to this orientation.
	 */
	void LSE_FCALL COrientation2d::SetPosU( const CVector2 &_vPos ) {
		if ( m_vPos == _vPos ) { return; }
		Pos() = _vPos;
	}

	/**
	 * Sets the position of this orientation relative to its facing direction.
	 *
	 * \param _vPos The position to apply to this orientation.
	 */
	void LSE_FCALL COrientation2d::SetRelPos( const CVector2 &_vPos ) {
		CVector2 vR, vU;
		GetVectors( vR, vU );
		m_vPos = (vR * _vPos.x) +
			(vU * _vPos.y);
		m_ui32Dirty |= LSM_2DODF_POSITION;
	}

	/**
	 * Offset the position of this orientation relative to its facing direction.
	 *
	 * \param _vPos The position to add to this orientation.
	 */
	void LSE_FCALL COrientation2d::AddRelPos( const CVector2 &_vPos ) {
		CVector2 vR, vU;
		GetVectors( vR, vU );
		m_vPos += (vR * _vPos.x) +
			(vU * _vPos.y);
		m_ui32Dirty |= LSM_2DODF_POSITION;
	}

	/**
	 * Builds a matrix from our data.
	 *
	 * \param _mRet Holds the created matrix.
	 */
	void LSE_FCALL COrientation2d::BuildMatrix( CMatrix3x3 &_mRet ) const {
		CVector2 vR, vU;
		GetVectors( vR, vU );

		_mRet._11 = vR.x * m_vScale.x;
		_mRet._12 = vR.y * m_vScale.x;
		_mRet._13 = LSM_ZERO;

		_mRet._21 = vU.x * m_vScale.y;
		_mRet._22 = vU.y * m_vScale.y;
		_mRet._23 = LSM_ZERO;

		_mRet._31 = m_vPos.x;
		_mRet._32 = m_vPos.y;
		_mRet._33 = LSM_ONE;
	}

	/**
	 * Builds a 4-by-4 matrix from our data.
	 *
	 * \param _mRet Holds the created matrix.
	 * \param _mRet The Z position.
	 */
	void LSE_FCALL COrientation2d::BuildMatrix( CMatrix4x4 &_mRet, LSREAL _fZ ) const {
		CVector2 vR, vU;
		GetVectors( vR, vU );

		_mRet._11 = vR.x * m_vScale.x;
		_mRet._12 = vR.y * m_vScale.x;
		_mRet._13 = LSM_ZERO;
		_mRet._14 = LSM_ZERO;

		_mRet._21 = vU.x * m_vScale.y;
		_mRet._22 = vU.y * m_vScale.y;
		_mRet._23 = LSM_ZERO;
		_mRet._24 = LSM_ZERO;

		_mRet._31 = LSM_ZERO;
		_mRet._32 = LSM_ZERO;
		_mRet._33 = LSM_ONE;
		_mRet._34 = LSM_ZERO;

		_mRet._41 = m_vPos.x;
		_mRet._42 = m_vPos.y;
		_mRet._43 = _fZ;
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
	LSBOOL LSE_FCALL COrientation2d::UpdateMatrix( CMatrix3x3 &_mRet, LSBOOL _bClearDirty ) const {
		if ( m_ui32Dirty != 0 ) {
			BuildMatrix( _mRet );
			if ( _bClearDirty ) { m_ui32Dirty = 0; }
			return true;
		}
		return false;
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
	 * \param _mRet The Z position.
	 * \return Returns true if an update was performed.
	 */
	LSBOOL LSE_FCALL COrientation2d::UpdateMatrix( CMatrix4x4 &_mRet, LSBOOL _bClearDirty, LSREAL _fZ ) const {
		if ( m_ui32Dirty != 0 ) {
			BuildMatrix( _mRet, _fZ );
			if ( _bClearDirty ) { m_ui32Dirty = 0; }
			return true;
		}
		return false;
	}

}	// namespace lsm
