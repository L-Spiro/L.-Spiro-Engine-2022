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
 * Description: The base class for all model objects (any object loaded from a standard model file).
 */

#include "LSEModelObjectBase.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CModelObjectBase::CModelObjectBase() :
		m_ui64Id( 0ULL ),
		m_ui64ParentId( 0ULL ),
		m_pcName( "" ) {
#define LSE_VEC0 CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO )
#define LSE_VEC1 CVector3( LSM_ONE, LSM_ONE, LSM_ONE )
		m_vScalePivot = LSE_VEC0;
		m_vScale = LSE_VEC1;
		m_vScaleOffset = LSE_VEC0;
		m_vRotPivot = LSE_VEC0;
		m_vPostRot = LSE_VEC0;
		m_vRot = LSE_VEC0;
		m_vPreRot = LSE_VEC0;
		m_vRotOffset = LSE_VEC0;
		m_vTranslation = LSE_VEC0;

#undef LSE_VEC1
#undef LSE_VEC0
	}
	LSE_CALLCTOR CModelObjectBase::~CModelObjectBase() {
	}

	// == Functions.
	/**
	 * Sets the data based off the given hierarchy structure from a shared model.
	 *
	 * \param _mhHier the hierarchy structure to clone.
	 */
	void LSE_CALL CModelObjectBase::CreateBaseData( const LSD_MESH_HIERARCHY &_mhHier ) {
		m_ui64Id = _mhHier.ui64Id;
		m_ui64ParentId = _mhHier.ui64ParentId;

#define LSE_COPYVEC( NAME )		m_v##NAME.x = static_cast<LSREAL>(_mhHier.f##NAME[0]);	\
	m_v##NAME.y = static_cast<LSREAL>(_mhHier.f##NAME[1]);								\
	m_v##NAME.z = static_cast<LSREAL>(_mhHier.f##NAME[2])
		LSE_COPYVEC( ScalePivot );
		LSE_COPYVEC( Scale );
		LSE_COPYVEC( ScaleOffset );
		LSE_COPYVEC( RotPivot );
		LSE_COPYVEC( PostRot );
		LSE_COPYVEC( Rot );
		LSE_COPYVEC( PreRot );
		LSE_COPYVEC( RotOffset );
		LSE_COPYVEC( Translation );
#undef LSE_COPYVEC
	}

	/**
	 * Undirty the local transform.
	 */
	void LSE_CALL CModelObjectBase::UndirtyLocal() const {
		if ( m_oOrientation.IsDirty() ) {
			LocalWillChange();

			//m_eoEulerOrientation.UpdateOrientation( m_oOrientation );
			//m_oOrientation.UpdateMatrix( m_mLocalTrans );
#if 1
			
			/*CMatrix4x4 mScalePivot, mScalePivotInv, mScaleOffset, mScale, mScaleFinal;
			mScalePivot.MatrixTranslation( m_vScalePivot.x, m_vScalePivot.y, m_vScalePivot.z );
			mScalePivotInv = mScalePivot.InverseAccurate();
			mScaleOffset.MatrixTranslation( m_vScaleOffset.x, m_vScaleOffset.y, m_vScaleOffset.z );
			mScale.MatrixScaling( m_vScale.x, m_vScale.y, m_vScale.z );

			mScaleFinal = mScalePivotInv * mScale * mScalePivot * mScaleOffset;*/
			CMatrix4x4 mScaleFinal;
			mScaleFinal._11 = m_vScale.x;
			mScaleFinal._12 = LSM_ZERO;
			mScaleFinal._13 = LSM_ZERO;
			mScaleFinal._14 = LSM_ZERO;

			mScaleFinal._21 = LSM_ZERO;
			mScaleFinal._22 = m_vScale.y;
			mScaleFinal._23 = LSM_ZERO;
			mScaleFinal._24 = LSM_ZERO;

			mScaleFinal._31 = LSM_ZERO;
			mScaleFinal._32 = LSM_ZERO;
			mScaleFinal._33 = m_vScale.z;
			mScaleFinal._34 = LSM_ZERO;

			mScaleFinal._41 = -m_vScalePivot.x * m_vScale.x + m_vScalePivot.x + m_vScaleOffset.x;
			mScaleFinal._42 = -m_vScalePivot.y * m_vScale.y + m_vScalePivot.y + m_vScaleOffset.y;
			mScaleFinal._43 = -m_vScalePivot.z * m_vScale.z + m_vScalePivot.z + m_vScaleOffset.z;
			mScaleFinal._44 = LSM_ONE;


			CMatrix4x4 mRotPivot, mRotPivotInv, mPreRot, mPostRot, mRotOffset, mRot, mRotFinal;
			mRotPivot.MatrixTranslation( m_vRotPivot.x, m_vRotPivot.y, m_vRotPivot.z );
			mRotPivotInv = mRotPivot.InverseAccurate();
			mPostRot.MatrixRotationXYZLH( m_vPostRot.x * LSM_DEG2RAD, m_vPostRot.y * LSM_DEG2RAD, m_vPostRot.z * LSM_DEG2RAD );
			mPreRot.MatrixRotationXYZLH( m_vPreRot.x * LSM_DEG2RAD, m_vPreRot.y * LSM_DEG2RAD, m_vPreRot.z * LSM_DEG2RAD );
			mRotOffset.MatrixTranslation( m_vRotOffset.x, m_vRotOffset.y, m_vRotOffset.z );
			mRot.MatrixRotationXYZLH( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );

			mRotFinal = mRotPivotInv * mPostRot * mRot * mPreRot * mRotPivot * mRotOffset;

			CMatrix4x4 mTrans;
			mTrans.MatrixTranslation( m_vTranslation.x, m_vTranslation.y, m_vTranslation.z );

			m_oOrientation.UpdateMatrix( m_mPostOrientation );
			m_mLocalTrans = mScaleFinal * mRotFinal * mTrans * m_mPostOrientation;


			/*
			CMatrix4x4 mScaleAll;
			mScaleAll._11 = m_vScale.x;
			mScaleAll._12 = LSM_ZERO;
			mScaleAll._13 = LSM_ZERO;
			mScaleAll._14 = LSM_ZERO;

			mScaleAll._21 = LSM_ZERO;
			mScaleAll._22 = m_vScale.y;
			mScaleAll._23 = LSM_ZERO;
			mScaleAll._24 = LSM_ZERO;

			mScaleAll._31 = LSM_ZERO;
			mScaleAll._32 = LSM_ZERO;
			mScaleAll._33 = m_vScale.z;
			mScaleAll._34 = LSM_ZERO;

			mScaleAll._41 = -m_vScalePivot.x * m_vScale.x + m_vScalePivot.x + m_vScaleOffset.x;
			mScaleAll._42 = -m_vScalePivot.y * m_vScale.y + m_vScalePivot.y + m_vScaleOffset.y;
			mScaleAll._43 = -m_vScalePivot.z * m_vScale.z + m_vScalePivot.z + m_vScaleOffset.z;
			mScaleAll._44 = LSM_ONE;

			CMatrix4x4 mRotPivotInv;
			mRotPivotInv.MatrixTranslation( -m_vRotPivot.x, -m_vRotPivot.y, -m_vRotPivot.z );
			CMatrix4x4 mPostRot;
			mPostRot.MatrixRotationXYZLH( m_vPostRot.x * LSM_DEG2RAD, m_vPostRot.y * LSM_DEG2RAD, m_vPostRot.z * LSM_DEG2RAD );
			CMatrix4x4 mRot;
			mRot.MatrixRotationXYZLH( m_vRot.x * LSM_DEG2RAD, m_vRot.y * LSM_DEG2RAD, m_vRot.z * LSM_DEG2RAD );
			CMatrix4x4 mPreRot;
			mPreRot.MatrixRotationXYZLH( m_vPreRot.x * LSM_DEG2RAD, m_vPreRot.y * LSM_DEG2RAD, m_vPreRot.z * LSM_DEG2RAD );

			CMatrix4x4 mRotAndTrans = mRotPivotInv * mPostRot * mRot * mPreRot;
			mRotAndTrans._41 += m_vRotPivot.x + m_vRotOffset.x + m_vTranslation.x;
			mRotAndTrans._42 += m_vRotPivot.y + m_vRotOffset.x + m_vTranslation.y;
			mRotAndTrans._43 += m_vRotPivot.z + m_vRotOffset.z + m_vTranslation.z;

			m_mLocalTrans = mScaleAll * mRotAndTrans;


			m_oOrientation.Undirty();
			*/
			
			//}
#endif	// #if 0

			
			DirtyChildrenWorldTrans();

			LocalDidChange();
		}
	}

}	// namespace lse
