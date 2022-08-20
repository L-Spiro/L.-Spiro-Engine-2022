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
 * Description: An instance of a mesh in a shared model.  The model holds an array of unique meshes into which
 *	a mesh indexes.  A mesh instance also includes a transform, name, parent/children, etc.
 */


#ifndef __LSD_MESHINSTANCE_H__
#define __LSD_MESHINSTANCE_H__

#include "../LSDModelLib.h"
#include "BoundingBoxes/LSPAabb.h"
#include "Gfx/LSGGfx.h"
#include "LSDMesh.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Sphere/LSMSphere.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "String/LSTLString.h"

namespace lsd {

	class CMesh;

	/**
	 * Class CMeshInstance
	 * \brief An instance of a mesh in a shared model.
	 *
	 * Description: An instance of a mesh in a shared model.  The model holds an array of unique meshes into which
	 *	a mesh indexes.  A mesh instance also includes a transform, name, parent/children, etc.
	 */
	class CMeshInstance {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMeshInstance();
		LSE_CALLCTOR							~CMeshInstance();


				// == Types.
		/** Instance data from the loaded file. */
#pragma pack( push, 1 )
		typedef struct LSD_MESH_INSTANCE {
			CMatrix4x4							mLocalTrans;
			uint32_t							ui32Index;
			uint64_t							ui64Id;
			uint64_t							ui64ParentId;
			CVector3							vEulerRot;
			CVector3							vEulerScale;
			CVector3							vEulerPos;
			uint32_t							ui32RotType;
			CAabb								aAabb;
			CSphere								sSphere;
		} * LPLSD_MESH_INSTANCE, * const LPCLSD_MESH_INSTANCE;
#pragma pack( pop )


		// == Functions.
		/**
		 * Loads a sub-mesh from a byte stream.
		 *
		 * \param _ebsStream Byte stream from which to read.
		 * \param _pmMeshes Array of meshes.
		 * \param _ui32Meshes Total meshes.
		 * \param _ui32Version File version number.
		 * \return Returns an error code indicating the result of the load.
		 */
		LSSTD_ERRORS LSE_CALL					LoadFromStream( const CExternalByteStream &_ebsStream,
			CMesh * _pmMeshes, uint32_t _ui32Meshes, uint32_t _ui32Version );

		/**
		 * Gets a constant reference to the transform for this instance.
		 *
		 * \return Returns a constant reference to the transform for this instance.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	Transform() const;

		/**
		 * Gets a pointer to the mesh used by this instance.
		 *
		 * \return Returns a pointer to the mesh used by this instance.
		 */
		LSE_INLINE CMesh * LSE_CALL				Mesh();

		/**
		 * Gets a constant pointer to the mesh used by this instance.
		 *
		 * \return Returns a constant pointer to the mesh used by this instance.
		 */
		LSE_INLINE const CMesh * LSE_CALL		Mesh() const;

		/**
		 * Gets the mesh instance data.
		 *
		 * \return Returns the instance data for this mesh instance.
		 */
		LSE_INLINE const LSD_MESH_INSTANCE & LSE_CALL
												Data() const;

		/**
		 * Gets a constant pointer to the name of the mesh instance.
		 *
		 * \return Returns a constant pointer to the name of the mesh instance.
		 */
		LSE_INLINE const char * LSE_CALL		Name() const;


	protected :
		// == Members.
		/** Mesh instance data. */
		LSD_MESH_INSTANCE						m_miData;

		/** Mesh pointer. */
		CMesh *									m_pmMesh;

		/** Mesh name. */
		CString									m_sName;

		
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a constant reference to the transform for this instance.
	 *
	 * \return Returns a constant reference to the transform for this instance.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CMeshInstance::Transform() const { return m_miData.mLocalTrans; }

	/**
	 * Gets a pointer to the mesh used by this instance.
	 *
	 * \return Returns a pointer to the mesh used by this instance.
	 */
	LSE_INLINE CMesh * LSE_CALL CMeshInstance::Mesh() { return m_pmMesh; }

	/**
	 * Gets a constant pointer to the mesh used by this instance.
	 *
	 * \return Returns a constant pointer to the mesh used by this instance.
	 */
	LSE_INLINE const CMesh * LSE_CALL CMeshInstance::Mesh() const { return m_pmMesh; }

	/**
	 * Gets the mesh instance data.
	 *
	 * \return Returns the instance data for this mesh instance.
	 */
	LSE_INLINE const CMeshInstance::LSD_MESH_INSTANCE & LSE_CALL CMeshInstance::Data() const { return m_miData; }

	/**
	 * Gets a constant pointer to the name of the mesh instance.
	 *
	 * \return Returns a constant pointer to the name of the mesh instance.
	 */
	LSE_INLINE const char * LSE_CALL CMeshInstance::Name() const { return m_sName.CStr(); }

}	// namespace lsd

#endif	// __LSD_MESHINSTANCE_H__
