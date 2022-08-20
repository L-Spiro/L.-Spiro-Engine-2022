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


#ifndef __LSX_MESHINSTANCE_H__
#define __LSX_MESHINSTANCE_H__

#include "../LSXFbxConverter.h"
#include "BoundingBoxes/LSPAabb.h"
#include "LSXMesh.h"
#include "LSXSkeleton.h"
#include "Sphere/LSMSphere.h"

namespace lsx {

	/**
	 * Class CMeshInstance
	 * \brief An instance of a mesh.
	 *
	 * Description: An instance of a mesh.  Simply gives a name and transform to a CMesh object, which may be shared.
	 *	CMesh objects are only saved to the file if they are unique.  This class keeps a CMesh object, which may be
	 *	actually be a duplicate, but during the saving process, it only saves the unique ID of the CMesh object.  CMesh
	 *	objects are saved in a separate pass.
	 */
	class CMeshInstance {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CMeshInstance();
		LSE_CALLCTOR								~CMeshInstance();


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
		LSBOOL LSE_CALL								CreateMeshInstance( FbxNode * _pnMesh, FbxScene * _psScene, CSet<FbxSurfaceMaterial *> &_sMaterials,
			const CSkeleton &_sSkeleton );

		/**
		 * Get our mesh for read and write.
		 *
		 * \return Returns our mesh for read and write.
		 */
		CMesh & LSE_CALL							GetMesh();

		/**
		 * Save the mesh instance.
		 *
		 * \param _bBuffer The buffer to which to save the mesh instance.
		 * \return Returns true if there is enough memory to save the mesh instance data.
		 */
		LSBOOL LSE_CALL								SaveMeshInstance( lsc::CMisc::LSC_BUFFER &_bBuffer );

	protected :
		// == Members.
		/** Our transform. */
		CMatrix4x4									m_mTrans;

		/** Our local transform. */
		CMatrix4x4									m_mLocalTrans;

		/** Our CMesh object. */
		CMesh										m_mMesh;

		/** Our name. */
		CString										m_sName;

		/** Local Euler rotations. */
		CVector3									m_vEulerRot;

		/** Local Euler scale. */
		CVector3									m_vEulerScale;

		/** Local Euler position. */
		CVector3									m_vEulerPos;

		/** Rotation order. */
		FbxPropertyT<EFbxRotationOrder>				m_froRotOrder;

		/** Our unique ID. */
		LSUINT64									m_ui64Id;

		/** Our parent ID. */
		LSUINT64									m_ui64ParentId;

		/** Our bounding box, not translated. */
		CAabb										m_aAabbBox;

		/** Our bounding sphere, not translated. */
		CSphere										m_sBoundingSphere;


		// == Functions.
		/**
		 * Gets the first parent of the given node that is either a joint or a mesh.
		 *
		 * \param _pnMesh The mesh whose parent is to be found.
		 * \return Returns the first parent of the given node that is either a joint or a mesh.
		 */
		FbxNode * LSE_CALL							GetFirstParent( FbxNode * _pnMesh );

		/**
		 * Get the world matrix of a node.
		 *
		 * \param _pnNode The node for which to get the world matrix.
		 * \param _psScene The scene containing the node.
		 * \return Returns the world matrix of a node.
		 */
		static CMatrix4x4 LSE_CALL					GetWorldMatrix( FbxNode * _pnNode, FbxScene * _psScene );

		/**
		 * Manually get the world matrix of a node.
		 *
		 * \param _pnNode The node for which to get the world matrix.
		 * \param _psScene The scene containing the node.
		 * \return Returns the world matrix of a node.
		 */
		static CMatrix4x4 LSE_CALL					GetWorldMatrixManual( FbxNode * _pnNode, FbxScene * _psScene );

		/**
		 * Manually get the local matrix of a node.
		 *
		 * \param _pnNode The node for which to get the local matrix.
		 * \param _psScene The scene containing the node.
		 * \return Returns the local matrix of a node.
		 */
		static CMatrix4x4 LSE_CALL					GetLocalMatrixManual( FbxNode * _pnNode, FbxScene * _psScene );

		/**
		 * Gets the local geometric matrix.
		 *
		 * \param _pnNode The node for which to get the local geometric matrix.
		 * \return Returns the local geometric matrix transform.
		 */
		static CMatrix4x4 LSE_CALL					GetLocalGeometricTransform( FbxNode * _pnNode );

	};

}	// namespace lsx

#endif	// __LSX_MESHINSTANCE_H__
