/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 * If you wish to use this code in a commercial product contact yogurtemperor@hotmail.com.
 *
 * Description: Holds the joints in a skeleton.
 */

#include "LSXSkeleton.h"


namespace lsx {

	// == Various constructors.
	LSE_CALL CSkeleton::CJoint::CJoint() {
	}

	// == Various constructors.
	LSE_CALL CSkeleton::CSkeleton() {
	}

	// == Functions.
	/**
	 * Extract the joint information from a node.
	 *
	 * \param _psNode The joint node whose information is to be extracted.
	 * \param _psScene The scene.
	 * \return Returns true if there was enough memory to extract all information.
	 */
	LSBOOL LSE_CALL CSkeleton::CJoint::ExtractJoint( const FbxSkeleton * _psNode, FbxScene * _psScene ) {
		if ( !_psNode ) { return false; }
		m_ui64Id = reinterpret_cast<LSUINT64>(_psNode->GetNode());

		m_sName = _psNode->GetNode()->GetName();
		for ( LSUINT32 I = 0UL; I < static_cast<LSUINT32>(_psNode->GetNode()->GetChildCount()); ++I ) {
			FbxNode * pnNode = _psNode->GetNode()->GetChild( I );
			if ( !pnNode->GetNodeAttribute() ) { continue; }

//			const char * pcChildNodename = pnNode->GetName();
			FbxNodeAttribute::EType atAttType = pnNode->GetNodeAttribute()->GetAttributeType();
			if ( atAttType == FbxNodeAttribute::eSkeleton ) {
				const FbxSkeleton * psthisChild = reinterpret_cast<const FbxSkeleton *>(pnNode->GetNodeAttribute());

				CJoint jThisChild;
				if ( !jThisChild.ExtractJoint( psthisChild, _psScene ) ) {
					CStd::DebugPrintA( "Error extracting child joint.\r\n" );
					return false;
				}

				if ( !m_vChildren.Push( jThisChild ) ) {
					CStd::DebugPrintA( "Error adding child joint.  Not enough RAM.\r\n" );
					return false;
				}
			}
			else {
				CStd::DebugPrintA( "Invalid node type for skeleton: " );
				char szBuffer[32];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%u", atAttType );
				CStd::DebugPrintA( szBuffer );
				CStd::DebugPrintA( ".\r\n" );
			}
		}

		FbxAMatrix & xmTrans = _psScene->GetAnimationEvaluator()->GetNodeGlobalTransform( const_cast<FbxNode *>(_psNode->GetNode()) );
		m_mWorldTrans = CMisc::ConvertMatrix( xmTrans );

#if 0
		CMatrix4x4 mLocalScale;
		mLocalScale.MatrixScaling( static_cast<LSREAL>(_psNode->GetNode()->LclScaling.Get()[0]),
			static_cast<LSREAL>(_psNode->GetNode()->LclScaling.Get()[1]),
			static_cast<LSREAL>(_psNode->GetNode()->LclScaling.Get()[2]) );
		//CMatrix4x4 mLocalRot;
		CVector3 vRot( static_cast<LSREAL>(_psNode->GetNode()->LclRotation.Get()[0]) * LSM_M_DEG2RAD,
			static_cast<LSREAL>(_psNode->GetNode()->LclRotation.Get()[1]) * LSM_M_DEG2RAD,
			static_cast<LSREAL>(_psNode->GetNode()->LclRotation.Get()[2]) * LSM_M_DEG2RAD );
		/*mLocalRot.MatrixRotationYawPitchRoll( vRot.x,
			vRot.y,
			vRot.z );*/

		CMatrix4x4 mX;
		CMatrix4x4 mY;
		CMatrix4x4 mZ;
		mX.MatrixRotationX( vRot.x );
		mY.MatrixRotationY( vRot.y );
		mZ.MatrixRotationZ( vRot.z );

		// Well my MatrixRotationYawPitchRoll() is not working right now.
		// TODO: Combine in the order specified by the node.
		m_mLocalTrans = mLocalScale * mX * mY * mZ;

		//m_mLocalTrans = mLocalScale * mLocalRot;

		m_mLocalTrans._41 = static_cast<LSREAL>(_psNode->GetNode()->LclTranslation.Get()[0]);
		m_mLocalTrans._42 = static_cast<LSREAL>(_psNode->GetNode()->LclTranslation.Get()[1]);
		m_mLocalTrans._43 = static_cast<LSREAL>(_psNode->GetNode()->LclTranslation.Get()[2]);
#endif	// #if 0

		return true;
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CSkeleton::CJoint::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// Write the joint name.
		if ( !CMisc::WriteString( m_sName, _bBuffer ) ) { return false; }

		// Write the unique ID of the joint.
		if ( !CMisc::WriteBytes( reinterpret_cast<LSUINT8 *>(&m_ui64Id), sizeof( m_ui64Id ), _bBuffer ) ) { return false; }

		// Write the index of the joint.
		if ( !CMisc::WriteBytes( reinterpret_cast<LSUINT8 *>(&m_ui32Index), sizeof( m_ui32Index ), _bBuffer ) ) { return false; }

		// Write the local matrix.
		if ( !CMisc::WriteMatrixSimple( m_mLocalTrans, _bBuffer ) ) { return false; }

		// Write the number of children.
		LSUINT32 ui32Len = m_vChildren.Length();
		if ( ui32Len > 0xFFFF ) {
			::printf( "Joint %s has over 65,535 children.\r\n", m_sName.CStr() );
			return false;
		}
		if ( !CMisc::WriteBytes( reinterpret_cast<LSUINT8 *>(&ui32Len), 2UL, _bBuffer ) ) { return false; }

		// Export each child.
		for ( LSUINT32 I = 0UL; I < m_vChildren.Length(); ++I ) {
			if ( !m_vChildren[I].SaveToBuffer( _bBuffer ) ) { return false; }
		}

		return true;
	}

	/**
	 * Sets the index of the joint.  Children are set with increasing indices.
	 *
	 * \param _ui32Index The index to set.
	 */
	LSVOID LSE_CALL CSkeleton::CJoint::SetIndex( LSUINT32 &_ui32Index ) {
		m_ui32Index = _ui32Index++;
		for ( LSUINT32 I = 0UL; I < m_vChildren.Length(); ++I ) {
			m_vChildren[I].SetIndex( _ui32Index );
		}
	}

	/**
	 * Extract the skeleton from a given scene.
	 *
	 * \param _psScene The scene from which to extract the skeleton.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CSkeleton::ExtractSkeleton( FbxScene * _psScene ) {
		// Remove all existing joints, if there are any.
		m_vRoots.ResetNoDealloc();

		// Get an array of all joints.
		CMisc::LSX_NODE_ARRAY nlNodes;
		CMisc::GatherNodes( _psScene->GetRootNode(), FbxNodeAttribute::eSkeleton, nlNodes, true );
		if ( !m_vRoots.Allocate( nlNodes.Length() ) ) { return false; }

		for ( LSUINT32 I = 0UL; I < nlNodes.Length(); ++I ) {
			if ( !nlNodes[I]->GetNodeAttribute() ) { continue; }
			if ( !reinterpret_cast<const FbxSkeleton *>(nlNodes[I]->GetNodeAttribute())->IsSkeletonRoot() ) { continue; }
			CJoint jNewJoint;
			if ( !jNewJoint.ExtractJoint( reinterpret_cast<const FbxSkeleton *>(nlNodes[I]->GetNodeAttribute()), _psScene ) ) {
				return false;
			}

			if ( !m_vRoots.Push( jNewJoint ) ) {
				CStd::DebugPrintA( "Error adding root joint.  Not enough RAM.\r\n" );
				return false;
			}
		}

		JointWorldToLocal();
		LSUINT32 ui32Index = 0UL;
		for ( LSUINT32 I = 0UL; I < m_vRoots.Length(); ++I ) {
			m_vRoots[I].SetIndex( ui32Index );
		}
		return true;
	}

	/**
	 * Go to the bind pose.
	 *
	 * \param _psScene The scene.
	 */
	LSVOID LSE_CALL CSkeleton::GoToBindPose( FbxScene * _psScene ) {
		// Get skin clusters.
		// Start by getting all of the mesh nodes.
		CMisc::LSX_NODE_ARRAY naMeshes;
		CMisc::GatherNodes( _psScene->GetRootNode(), FbxNodeAttribute::eMesh, naMeshes, true );
		for ( LSUINT32 I = naMeshes.Length(); I--; ) {
			if ( !naMeshes[I]->GetNodeAttribute() ) { continue; }

			const FbxMesh * pmMesh = reinterpret_cast<const FbxMesh *>(naMeshes[I]->GetNodeAttribute());


			LSUINT32 ui32TotalSkins = pmMesh->GetDeformerCount( FbxDeformer::eSkin );
//			FbxSkin * psSkinDeformer = reinterpret_cast<FbxSkin *>(pmMesh->GetDeformer( 0UL, FbxDeformer::eSkin ));

			for ( LSUINT32 J = ui32TotalSkins; J--; ) {
				FbxSkin * psSkin = reinterpret_cast<FbxSkin *>(pmMesh->GetDeformer( J, FbxDeformer::eSkin ));


				LSUINT32 ui32ClusterCount = psSkin->GetClusterCount();
				for ( LSUINT32 K = ui32ClusterCount; K--; ) {
					FbxCluster * pcCluster = psSkin->GetCluster( K );
					FbxNode * pnLinkNode = pcCluster->GetLink();
					if ( pnLinkNode ) {

						CString sName = pnLinkNode->GetName();
						CJoint * pjThis = GetJointByName( sName );
						if ( pjThis ) {

							FbxAMatrix mLinkMatrix;
							pcCluster->GetTransformLinkMatrix( mLinkMatrix );
							for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
								for ( LSUINT32 X = 0UL; X < 4UL; ++X ) {
									pjThis->m_mWorldTrans( X, Y ) = static_cast<LSREAL>(mLinkMatrix.Get( X, Y ));
								}
							}
						}
					}
				}
			}
		}

		// Continue by searching for bind-pose data.

		// Find the bind pose.
		for ( LSUINT32 I = _psScene->GetPoseCount(); I--; ) {
			FbxPose * ppPose = _psScene->GetPose( I );
			if ( ppPose->IsBindPose() ) {
				for ( LSUINT32 J = ppPose->GetCount(); J--; ) {
					CString sName = ppPose->GetNodeName( J ).GetCurrentName();
					CJoint * pjThis = GetJointByName( sName );
					if ( pjThis ) {

						FbxMatrix mMatrix = ppPose->GetMatrix( J );
						for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
							for ( LSUINT32 X = 0UL; X < 4UL; ++X ) {
								pjThis->m_mWorldTrans( X, Y ) = static_cast<LSREAL>(mMatrix.Get( X, Y ));
							}
						}
					}
				}
				break;
			}
		}

		JointWorldToLocal();
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CSkeleton::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		lsc::CMisc::LSC_BUFFER bDecompressed;
		// Write the total number of joints.
		if ( !CMisc::WriteUInt( TotalJoints(), bDecompressed ) ) { return false; }

		// Write the number of roots.
		if ( !CMisc::WriteUInt( m_vRoots.Length(), bDecompressed ) ) { return false; }
		
		// Export each child.
		for ( LSUINT32 I = 0UL; I < m_vRoots.Length(); ++I ) {
			if ( !m_vRoots[I].SaveToBuffer( bDecompressed ) ) { return false; }
		}

		if ( !lsc::CMisc::CompressToBuffer( reinterpret_cast<const LSUINT8 *>(&bDecompressed[0]), bDecompressed.Length(), _bBuffer ) ) { return false; }

		return true;
	}

	/**
	 * Gets a joint by name, if it exists.
	 *
	 * \param _pcName Name of the joint to get.
	 * \return Returns a constant pointer to the given joint.
	 */
	const CSkeleton::CJoint * LSE_CALL CSkeleton::GetJoint( const LSCHAR * _pcName ) const {
		return const_cast<CSkeleton *>(this)->GetJointByName( _pcName );
	}

	/**
	 * Get a joint by name.
	 *
	 * \param _sName Name of the joint to find.
	 * \return Returns the joint with the given name.
	 */
	CSkeleton::CJoint * LSE_CALL CSkeleton::GetJointByName( const CString &_sName ) {
		for ( LSUINT32 I = m_vRoots.Length(); I--; ) {
			CJoint * pjThis = GetJointByName( &m_vRoots[I], _sName );
			if ( pjThis ) { return pjThis; }
		}
		return NULL;
	}

	/**
	 * Get a joint by name.
	 *
	 * \param _pjJoint The joint to search.
	 * \param _sName Name of the joint to find.
	 * \return Returns the joint with the given name.
	 */
	CSkeleton::CJoint * LSE_CALL CSkeleton::GetJointByName( CJoint * _pjJoint, const CString &_sName ) {
		if ( _pjJoint->m_sName == _sName ) {
			return _pjJoint;
		}
		for ( LSUINT32 I = _pjJoint->m_vChildren.Length(); I--; ) {
			CJoint * pjThis = GetJointByName( &_pjJoint->m_vChildren[I], _sName );
			if ( pjThis ) { return pjThis; }
		}
		return NULL;
	}

	/**
	 * Gets the total number of joints in the skeleton.
	 *
	 * \param _pjJoint The joint to search.
	 * \return Returns the total number of joints in the skeleton.
	 */
	LSUINT32 LSE_CALL CSkeleton::TotalJoints( const CJoint * _pjJoint ) const {
		LSUINT32 ui32Total = 1UL;
		for ( LSUINT32 I = _pjJoint->m_vChildren.Length(); I--; ) {
			ui32Total += TotalJoints( &_pjJoint->m_vChildren[I] );
		}
		return ui32Total;
	}

	/**
	 * Convert joint world coordinates to local coordinates.
	 */
	LSVOID LSE_CALL CSkeleton::JointWorldToLocal() {
		for ( LSUINT32 I = m_vRoots.Length(); I--; ) {
			JointWorldToLocal( m_vRoots[I], NULL );
		}
	}

	/**
	 * Convert a joint's and all of its children's world coordinates to local coordinates.
	 *
	 * \param _jJoint The joint to which to apply the changes.
	 * \param _pjParent Parent joint or NULL.
	 */
	LSVOID LSE_CALL CSkeleton::JointWorldToLocal( CJoint &_jJoint, CJoint * _pjParent ) {
		if ( _pjParent ) {
			_jJoint.m_mLocalTrans = _jJoint.m_mWorldTrans * _pjParent->m_mWorldTrans.InverseAccurate();
		}
		else {
			_jJoint.m_mLocalTrans = _jJoint.m_mWorldTrans;
		}


		for ( LSUINT32 I = _jJoint.m_vChildren.Length(); I--; ) {
			JointWorldToLocal( _jJoint.m_vChildren[I], &_jJoint );
		}
	}

}	// namespace lsx
