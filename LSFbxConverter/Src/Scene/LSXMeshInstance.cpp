/**
 * Copyright L. Spiro 2011
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An instance of a mesh.  Simply gives a name and transform to a CMesh object, which may be shared.
 *	CMesh objects are only saved to the file if they are unique.  This class keeps a CMesh object, which may be
 *	actually be a duplicate, but during the saving process, it only saves the unique ID of the CMesh object.  CMesh
 *	objects are saved in a separate pass.
 */

#include "LSXMeshInstance.h"
#include "Misc/LSPPhysUtils.h"
#include "Orientation/LSMEulerOrientation.h"


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CMeshInstance::CMeshInstance() {
	}
	LSE_CALLCTOR CMeshInstance::~CMeshInstance() {
	}

	// == Functions.
	/**
	 * Create a mesh instance from a node object.
	 *
	 * \param _pnMesh The node from which geometry information is to be extracted.
	 * \param _psScene The scene.
	 * \param _sMaterials Materials referenced by this object.
	 * \param _sSkeleton The skeleton.
	 * \return Returns true if there were no problems creating the mesh.
	 */
	LSBOOL LSE_CALL CMeshInstance::CreateMeshInstance( FbxNode * _pnMesh, FbxScene * _psScene, CSet<FbxSurfaceMaterial *> &_sMaterials,
		const CSkeleton &_sSkeleton ) {
		//if ( !m_mMesh.CreateMesh( _pnMesh, _psScene, _sMaterials, _sSkeleton ) ) { return false; }
		if ( !m_mMesh.LoadMesh( _pnMesh->GetMesh(), _sMaterials ) ) {
			return false;
		}

		// Get the mesh name.
		m_sName = _pnMesh->GetName();


		// Get the global transform of the mesh.
		//m_mTrans = GetWorldMatrixManual( _pnMesh, _psScene );
		m_mTrans = GetWorldMatrix( _pnMesh, _psScene );
		m_mLocalTrans = m_mTrans;//.Identity();
		FbxNode * pfmParent = _pnMesh->GetParent();//GetFirstParent( _pnMesh );
		if ( pfmParent ) {
			//CMatrix4x4 mParentTrans = GetWorldMatrixManual( pfmParent, _psScene ).InverseAccurate();
			CMatrix4x4 mParentTrans = GetWorldMatrix( pfmParent, _psScene ).InverseAccurate();
			m_mLocalTrans = m_mTrans * mParentTrans;
		}
		//m_mLocalTrans = lsx::CMisc::ConvertMatrix( _pnMesh->EvaluateLocalTransform() );

#if 0
		CMatrix4x4 mLocal = GetLocalMatrixManual( _pnMesh, _psScene );// * GetLocalGeometricTransform( _pnMesh );
		CMatrix4x4 mLocalThem = m_mLocalTrans;//lsx::CMisc::ConvertMatrix( _pnMesh->EvaluateLocalTransform() );
		//CMatrix4x4 mLocalThem = lsx::CMisc::ConvertMatrix( _pnMesh->EvaluateLocalTransform() );
		//CMatrix4x4 mLocalThem = lsx::CMisc::ConvertMatrix( _psScene->GetAnimationEvaluator()->GetNodeLocalTransform( _pnMesh ) );
		LSCHAR szBuffer[512];
		for ( LSUINT32 I = 0; I < 4; ++I ) {
			CVector4 vRow, vRowThem, vTrans;
			mLocal.GetRow( I, vRow );
			mLocalThem.GetRow( I, vRowThem );
			m_mTrans.GetRow( I, vTrans );
			CStd::SPrintF( szBuffer, 512, "%f	%f	%f		%f	%f	%f		%f	%f	%f		%f	%f	%f\r\n",
				vRow.x, vRow.y, vRow.z,
				vRowThem.x, vRowThem.y, vRowThem.z,
				vRow.x - vRowThem.x, vRow.y - vRowThem.y, vRow.z - vRowThem.z,
				vTrans.x, vTrans.y, vTrans.z );
			CStd::DebugPrintA( szBuffer );
		}
		CStd::DebugPrintA( m_sName.CStr() );
		CStd::DebugPrintA( "\r\n" );
#endif	// #if 0

		m_ui64Id = reinterpret_cast<LSUINT64>(_pnMesh);

		m_ui64ParentId = reinterpret_cast<LSUINT64>(pfmParent);


		if ( m_mMesh.Vertices().Length() ) {
			const CMesh::LSX_VERTEX_BUFFER * pvbBuffer = &m_mMesh.Vertices();
			const CMesh::LSX_RAW_VERTEX * prvRawVert = &((*pvbBuffer)[0]);
			m_aAabbBox.ComputeAabbFromPointArray<LSX_VECTOR>( reinterpret_cast<const LSUINT8 *>(&prvRawVert->vPosition),
				m_mMesh.Vertices().Length(),
				sizeof( CMesh::LSX_RAW_VERTEX ) );
			m_sBoundingSphere = CPhysUtils::RitterEigenSphere<LSX_VECTOR>( reinterpret_cast<const LSUINT8 *>(&m_mMesh.Vertices()[0]),
				m_mMesh.Vertices().Length(),
				sizeof( CMesh::LSX_RAW_VERTEX ) );
		}
		else {
			m_aAabbBox.MakeInfinitelySmall();
			m_sBoundingSphere.c = CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO );
			m_sBoundingSphere.r = LSM_ZERO;
		}

		FbxVector4 vVec = _pnMesh->LclRotation;
		m_vEulerRot = CVector3( static_cast<LSREAL>(vVec[0]), static_cast<LSREAL>(vVec[1]), static_cast<LSREAL>(vVec[2]) );
		vVec = _pnMesh->LclScaling;
		m_vEulerScale = CVector3( static_cast<LSREAL>(vVec[0]), static_cast<LSREAL>(vVec[1]), static_cast<LSREAL>(vVec[2]) );
		vVec = _pnMesh->LclTranslation;
		m_vEulerPos = CVector3( static_cast<LSREAL>(vVec[0]), static_cast<LSREAL>(vVec[1]), static_cast<LSREAL>(vVec[2]) );
		
		m_froRotOrder = _pnMesh->RotationOrder;

		return true;
	}

	/**
	 * Get our mesh for read and write.
	 *
	 * \return Returns our mesh for read and write.
	 */
	CMesh & LSE_CALL CMeshInstance::GetMesh() {
		return m_mMesh;
	}

	/**
	 * Save the mesh instance.
	 *
	 * \param _bBuffer The buffer to which to save the mesh instance.
	 * \return Returns true if there is enough memory to save the mesh instance data.
	 */
	LSBOOL LSE_CALL CMeshInstance::SaveMeshInstance( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// Just write our data.  It will be compressed outside of here.
		if ( !lsx::CMisc::WriteString( m_sName, _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteMatrix( m_mTrans, _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteUInt( GetMesh().GetIndex(), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteUInt64( m_ui64Id, _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteUInt64( m_ui64ParentId, _bBuffer ) ) { return false; }
		////if ( !lsx::CMisc::WriteMatrix( m_mLocalTrans, _bBuffer ) ) { return false; }
		// Eulers.
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerRot.x), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerRot.y), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerRot.z), _bBuffer ) ) { return false; }

		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerScale.x), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerScale.y), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerScale.z), _bBuffer ) ) { return false; }

		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerPos.x), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerPos.y), _bBuffer ) ) { return false; }
		if ( !lsx::CMisc::WriteFloat( static_cast<LSFLOAT>(m_vEulerPos.z), _bBuffer ) ) { return false; }

		switch ( m_froRotOrder ) {
			case eEulerXZY : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_XZY, _bBuffer ) ) { return false; }
				break;
			}
			case eEulerYZX : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_YZX, _bBuffer ) ) { return false; }
				break;
			}
			case eEulerYXZ : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_YXZ, _bBuffer ) ) { return false; }
				break;
			}
			case eEulerZXY : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_ZXY, _bBuffer ) ) { return false; }
				break;
			}
			case eEulerZYX : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_ZYX, _bBuffer ) ) { return false; }
				break;
			}
			default : {
				if ( !lsx::CMisc::WriteUInt( CEulerOrientation::LSM_ERO_XYZ, _bBuffer ) ) { return false; }
			}
		}

		// AABB.
		if ( !lsx::CMisc::WriteBytes( reinterpret_cast<LSUINT8 *>(&m_aAabbBox), sizeof( m_aAabbBox ), _bBuffer ) ) { return false; }

		// Sphere.
		if ( !lsx::CMisc::WriteBytes( reinterpret_cast<LSUINT8 *>(&m_sBoundingSphere), sizeof( m_sBoundingSphere ), _bBuffer ) ) { return false; }

		return true;
	}

	/**
	 * Get the world matrix of a node.
	 *
	 * \param _pnNode The node for which to get the world matrix.
	 * \param _psScene The scene containing the node.
	 * \return Returns the world matrix of a node.
	 */
	CMatrix4x4 LSE_CALL CMeshInstance::GetWorldMatrix( FbxNode * _pnNode, FbxScene * _psScene ) {
		return lsx::CMisc::ConvertMatrix( _psScene->GetAnimationEvaluator()->GetNodeGlobalTransform( const_cast<FbxNode *>(_pnNode) ) ) *
			GetLocalGeometricTransform( _pnNode );
	}

	/**
	 * Manually get the world matrix of a node.
	 *
	 * \param _pnNode The node for which to get the world matrix.
	 * \param _psScene The scene containing the node.
	 * \return Returns the world matrix of a node.
	 */
	CMatrix4x4 LSE_CALL CMeshInstance::GetWorldMatrixManual( FbxNode * _pnNode, FbxScene * _psScene ) {
		CMatrix4x4 mRet;
		CMatrix4x4 mLocal = GetLocalMatrixManual( _pnNode, _psScene );//lsx::CMisc::ConvertMatrix( _pnNode->EvaluateLocalTransform() );
		if ( !_pnNode->GetParent() ) {
			// Its local translation is its world translation.
			mRet = mLocal;
		}
		else {
			mRet = mLocal * GetWorldMatrixManual( _pnNode->GetParent(), _psScene );
		}
		
		CMatrix4x4 mTheirWorld = GetWorldMatrix( _pnNode, _psScene );
		/*if ( !mRet.CompEpsilon( mTheirWorld, LSM_FLT_EPSILON * 5.0f ) ) {
			volatile int kjhg = 0;
		}*/

		
		if ( _pnNode->GetParent() ) {
			CMatrix4x4 mInvParent = GetWorldMatrixManual( _pnNode->GetParent(), _psScene ).InverseAccurate();
			CMatrix4x4 mTestLocal = mRet * mInvParent;
			/*if ( !mTestLocal.CompEpsilon( mLocal, LSM_FLT_EPSILON * 4.0f ) ) {
				volatile int kjhg = 0;
			}*/
		}
		/*if ( !mLocal.CompEpsilon( GetLocalMatrixManual( _pnNode, _psScene ), LSM_FLT_EPSILON * 4.0f ) ) {
			volatile int kjhg = 0;
		}*/
		

		return mRet;
	}

	/**
	 * Manually get the local matrix of a node.
	 *
	 * \param _pnNode The node for which to get the local matrix.
	 * \param _psScene The scene containing the node.
	 * \return Returns the local matrix of a node.
	 */
	CMatrix4x4 LSE_CALL CMeshInstance::GetLocalMatrixManual( FbxNode * _pnNode, FbxScene * _psScene ) {
#if 0
		CMatrix4x4 mInvScalePivot;
		CMatrix4x4 mScalePivot;
		CMatrix4x4 mScaleVal;
		mScalePivot.Identity();
		mScalePivot._41 = static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[0]);
		mScalePivot._42 = static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[1]);
		mScalePivot._43 = static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[2]);
		mInvScalePivot = mScalePivot.InverseAccurate();
		mScaleVal.Identity();
		FbxVector4 vScale = _psScene->GetAnimationEvaluator()->GetNodeLocalScaling( _pnNode );
		mScaleVal._11 = static_cast<LSREAL>(vScale[0]);
		mScaleVal._22 = static_cast<LSREAL>(vScale[1]);
		mScaleVal._33 = static_cast<LSREAL>(vScale[2]);

		CMatrix4x4 mScale = mInvScalePivot * mScaleVal * mScalePivot;
		mScale._41 += static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[0]);
		mScale._42 += static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[1]);
		mScale._43 += static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[2]);

		CMatrix4x4 mInvRotPivot;
		CMatrix4x4 mRotPivot;
		CMatrix4x4 mRotVal;
		mRotPivot.Identity();
		mRotPivot._41 = static_cast<LSREAL>(_pnNode->RotationPivot.Get()[0]);
		mRotPivot._42 = static_cast<LSREAL>(_pnNode->RotationPivot.Get()[1]);
		mRotPivot._43 = static_cast<LSREAL>(_pnNode->RotationPivot.Get()[2]);
		mInvRotPivot = mRotPivot.InverseAccurate();
		//double fX = _pnNode->LclRotation.Get()[0];
		FbxVector4 vRot = _psScene->GetAnimationEvaluator()->GetNodeLocalRotation( _pnNode );
		mRotVal.MatrixRotationXYZLH( static_cast<LSREAL>(vRot[0]) * LSM_DEG2RAD,
			static_cast<LSREAL>(vRot[1]) * LSM_DEG2RAD,
			static_cast<LSREAL>(vRot[2]) * LSM_DEG2RAD );
		/*CMatrix4x4 mRotX, mRotY, mRotZ;
		mRotX.MatrixRotationXLH( static_cast<LSREAL>(_pnNode->LclRotation.Get()[0]) * LSM_DEG2RAD );
		mRotY.MatrixRotationYLH( static_cast<LSREAL>(_pnNode->LclRotation.Get()[1]) * LSM_DEG2RAD );
		mRotZ.MatrixRotationZLH( static_cast<LSREAL>(_pnNode->LclRotation.Get()[2]) * LSM_DEG2RAD );
		CMatrix4x4 mRotXyz = mRotX * mRotY * mRotZ;*/
		//mRotVal.MatrixRotationYawPitchRoll( _pnNode->LclRotation.Get()[1], _pnNode->LclRotation.Get()[0], _pnNode->LclRotation.Get()[2] );

		CMatrix4x4 mRot = mInvRotPivot * mRotVal * mRotPivot;
		mRot._41 += static_cast<LSREAL>(_pnNode->RotationOffset.Get()[0]);
		mRot._42 += static_cast<LSREAL>(_pnNode->RotationOffset.Get()[1]);
		mRot._43 += static_cast<LSREAL>(_pnNode->RotationOffset.Get()[2]);
		/*if ( _pnNode->ScalingPivot.Get()[0] ||
			_pnNode->ScalingPivot.Get()[1] ||
			_pnNode->ScalingPivot.Get()[2] ||

			_pnNode->ScalingOffset.Get()[0] ||
			_pnNode->ScalingOffset.Get()[1] ||
			_pnNode->ScalingOffset.Get()[2] ||

			_pnNode->RotationPivot.Get()[0] ||
			_pnNode->RotationPivot.Get()[1] ||
			_pnNode->RotationPivot.Get()[2] ||

			_pnNode->RotationOffset.Get()[0] ||
			_pnNode->RotationOffset.Get()[1] ||
			_pnNode->RotationOffset.Get()[2] ||

			_pnNode->PreRotation.Get()[0] ||
			_pnNode->PreRotation.Get()[1] ||
			_pnNode->PreRotation.Get()[2] ||
		
			_pnNode->PostRotation.Get()[0] ||
			_pnNode->PostRotation.Get()[1] ||
			_pnNode->PostRotation.Get()[2]) {
			volatile int GHgjh = -0;
			volatile int G3Hgjh = -0;
		}*/

		/*FbxAMatrix aRotX;
		aRotX.SetR( FbxVector4( 0.0, 0.0, 32.2 ) );
		FbxAMatrix fxmRes = aRotX.Transpose();

		fX = _pnNode->LclScaling.Get()[0];*/
		//double fX2 = _pnNode->Scaling.Get()[0];
		CMatrix4x4 mTrans;
		//FbxPropertyT<FbxDouble3> vTrans = _pnNode->LclTranslation;
		FbxVector4 vTrans = _psScene->GetAnimationEvaluator()->GetNodeLocalTranslation( _pnNode );
		mTrans.MatrixTranslation( static_cast<LSREAL>(vTrans[0]),
			static_cast<LSREAL>(vTrans[1]),
			static_cast<LSREAL>(vTrans[2]) );
		return mScale * mRot * mTrans;
#else
		FbxVector4 vScale = _psScene->GetAnimationEvaluator()->GetNodeLocalScaling( _pnNode );
		FbxVector4 vRot = _psScene->GetAnimationEvaluator()->GetNodeLocalRotation( _pnNode );
		FbxVector4 vTrans = _psScene->GetAnimationEvaluator()->GetNodeLocalTranslation( _pnNode );

		CMatrix4x4 mScalePivot, mScalePivotInv, mScaleOffset, mScale, mScaleFinal;
		mScalePivot.MatrixTranslation( static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[0]), static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[1]), static_cast<LSREAL>(_pnNode->ScalingPivot.Get()[2]) );
		mScalePivotInv = mScalePivot.InverseAccurate();
		mScaleOffset.MatrixTranslation( static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[0]), static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[1]), static_cast<LSREAL>(_pnNode->ScalingOffset.Get()[2]) );
		mScale.MatrixScaling( static_cast<LSREAL>(vScale[0]), static_cast<LSREAL>(vScale[1]), static_cast<LSREAL>(vScale[2]) );

		mScaleFinal = mScalePivotInv * mScale * mScalePivot * mScaleOffset;


		CMatrix4x4 mRotPivot, mRotPivotInv, mPreRot, mPostRot, mRotOffset, mRot, mRotFinal;
		mRotPivot.MatrixTranslation( static_cast<LSREAL>(_pnNode->RotationPivot.Get()[0]), static_cast<LSREAL>(_pnNode->RotationPivot.Get()[1]), static_cast<LSREAL>(_pnNode->RotationPivot.Get()[2]) );
		mRotPivotInv = mRotPivot.InverseAccurate();
		//mPostRot.MatrixRotationXYZLH( m_vPostRot.x * LSM_DEG2RAD, m_vPostRot.y * LSM_DEG2RAD, m_vPostRot.z * LSM_DEG2RAD );
		mPostRot.Identity();
		//mPreRot.MatrixRotationXYZLH( m_vPreRot.x * LSM_DEG2RAD, m_vPreRot.y * LSM_DEG2RAD, m_vPreRot.z * LSM_DEG2RAD );
		mPreRot.Identity();
		mRotOffset.MatrixTranslation( static_cast<LSREAL>(_pnNode->RotationOffset.Get()[0]), static_cast<LSREAL>(_pnNode->RotationOffset.Get()[1]), static_cast<LSREAL>(_pnNode->RotationOffset.Get()[2]) );
		mRot.MatrixRotationXYZLH( static_cast<LSREAL>(vRot[0]) * LSM_DEG2RAD, static_cast<LSREAL>(vRot[1]) * LSM_DEG2RAD, static_cast<LSREAL>(vRot[2]) * LSM_DEG2RAD );

		mRotFinal = mRotPivotInv * mPostRot * mRot * mPreRot * mRotPivot * mRotOffset;

		CMatrix4x4 mTrans;
		mTrans.MatrixTranslation( static_cast<LSREAL>(vTrans[0]), static_cast<LSREAL>(vTrans[1]), static_cast<LSREAL>(vTrans[2]) );

		return mScaleFinal * mRotFinal * mTrans;
#endif	// 0
	}

	/**
	 * Gets the local geometric matrix.
	 *
	 * \param _pnNode The node for which to get the local geometric matrix.
	 * \return Returns the local geometric matrix transform.
	 */
	CMatrix4x4 LSE_CALL CMeshInstance::GetLocalGeometricTransform( FbxNode * _pnNode ) {
		FbxAMatrix xmGeometry;
		FbxVector4 vTranslation, vRotation, vScaling;
		vTranslation = _pnNode->GetGeometricTranslation( FbxNode::eSourcePivot );
		vRotation = _pnNode->GetGeometricRotation( FbxNode::eSourcePivot );
		vScaling = _pnNode->GetGeometricScaling( FbxNode::eSourcePivot );
		xmGeometry.SetT( vTranslation );
		xmGeometry.SetR( vRotation );
		xmGeometry.SetS( vScaling );
#if 0
		if ( vTranslation.SquareLength() || vRotation.SquareLength() || (vScaling.SquareLength() != 3.0) ) {
			volatile int gjhasgd = 0;
		}
#endif	// #if 0
		return lsx::CMisc::ConvertMatrix( xmGeometry );
	}

	/**
	 * Gets the first parent of the given node that is either a joint or a mesh.
	 *
	 * \param _pnMesh The mesh whose parent is to be found.
	 * \return Returns the first parent of the given node that is either a joint or a mesh.
	 */
	FbxNode * LSE_CALL CMeshInstance::GetFirstParent( FbxNode * _pnMesh ) {
		FbxNode * pfnParent = _pnMesh->GetParent();
		while ( pfnParent ) {
			if ( pfnParent->GetNodeAttribute() ) {
				if ( pfnParent->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton ||
					pfnParent->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh ) {
					return pfnParent;
				}
				else {
					if ( pfnParent->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eNull ) {
						::printf( "Parent type: %u.\r\n", pfnParent->GetNodeAttribute()->GetAttributeType() );
					}
				}
			}
			pfnParent = pfnParent->GetParent();
		}
		return NULL;
	}

}	// namespace lsx
