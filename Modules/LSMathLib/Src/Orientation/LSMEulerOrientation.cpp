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
 * Description: Euler orientation as a helper for a standard orientation.  Its primary use is in animation,
 *	where rotations are expressed as degrees around axes and each rotation is updated by tracks.  This data
 *	is then applied to a COrientation instance for standard processing.
 */

#include "LSMEulerOrientation.h"
#include "../Matrix/LSMMatrix4x4.h"
#include "../Orientation/LSMOrientation.h"


namespace lsm {

	// == Various constructors.
	LSE_CALLCTOR CEulerOrientation::CEulerOrientation() :
		m_vPos( CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ) ),
		m_vScale( CVector3( LSM_ONE, LSM_ONE, LSM_ONE ) ),
		m_vRot( CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ) ),
		m_ui32Dirty( 0 ),
		m_ui32RotType( LSM_ERO_XYZ ) {
	}

	// == Functions.
	/**
	 * Applies this orientation to a COrientation object, updating only things that
	 *	have been modified on this object.  Also clears the dirty flag.
	 *
	 * \param _oOrientation The orientation to update with the values in this
	 *	Euler orientation that have changed.
	 */
	void LSE_CALL CEulerOrientation::UpdateOrientation( COrientation &/*_oOrientation*/ ) const {
		// Position, only what changed.
		/*if ( m_ui32Dirty & LSM_ED_POS_X ) {
			_oOrientation.Pos().x = m_vPos.x;
		}
		if ( m_ui32Dirty & LSM_ED_POS_Y ) {
			_oOrientation.Pos().y = m_vPos.y;
		}
		if ( m_ui32Dirty & LSM_ED_POS_Z ) {
			_oOrientation.Pos().z = m_vPos.z;
		}*/

		// Scale, only what changed.
		/*if ( m_ui32Dirty & LSM_ED_SCALE_X ) {
			_oOrientation.Scale().x = m_vScale.x;
		}
		if ( m_ui32Dirty & LSM_ED_SCALE_Y ) {
			_oOrientation.Scale().y = m_vScale.y;
		}
		if ( m_ui32Dirty & LSM_ED_SCALE_Z ) {
			_oOrientation.Scale().z = m_vScale.z;
		}*/

		// Rotation (any changes).
		/*if ( m_ui32Dirty & (LSM_ED_ROT_X | LSM_ED_ROT_Y | LSM_ED_ROT_Z) ) {
			CMatrix4x4 mMat;
			_oOrientation.SetRotation( CreateRotMatrix( mMat ) );
		}*/
		m_ui32Dirty = 0;
	}

	/**
	 * Creates a rotation matrix given the Euler angles and order of rotation.
	 *
	 * \param _mMat The matrix to be filled with the result.
	 * \return Returns _mMat.
	 */
	CMatrix4x4 & LSE_CALL CEulerOrientation::CreateRotMatrix( CMatrix4x4 &_mMat ) const {
		CMatrix4x4 mX, mY, mZ;
		mX.MatrixRotationXLH( m_vRot.x * LSM_DEG2RAD );
		mY.MatrixRotationYLH( m_vRot.y * LSM_DEG2RAD );
		mZ.MatrixRotationZLH( m_vRot.z * LSM_DEG2RAD );
		switch ( m_ui32RotType ) {
			case LSM_ERO_XYZ : {
				//_mMat = mX * mY * mZ;
				_mMat.MatrixRotationXYZLH( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
			case LSM_ERO_YZX : {
				_mMat = mY * mZ * mX;
				//_mMat.MatrixRotationYZX( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
			case LSM_ERO_ZXY : {
				_mMat = mZ * mX * mY;
				//_mMat.MatrixRotationZXY( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
			case LSM_ERO_XZY : {
				_mMat = mX * mZ * mY;
				//_mMat.MatrixRotationXZY( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
			case LSM_ERO_YXZ : {
				_mMat = mY * mX * mZ;
				//_mMat.MatrixRotationYXZ( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
			case LSM_ERO_ZYX : {
				_mMat = mZ * mY * mX;
				//_mMat.MatrixRotationZYX( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
				break;
			}
		}
		return _mMat;
	}

}	// namespace lsm
