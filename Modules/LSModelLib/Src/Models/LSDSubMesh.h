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
 * Description: A piece of a mesh.  Contains a vertex buffer pointer (the vertex buffer is owned by the mesh and
 *	shared among all sub-meshes), its own index buffer, and a pointer to the material (materials are owned by the
 *	model).
 */


#ifndef __LSD_SUBMESH_H__
#define __LSD_SUBMESH_H__

#include "../LSDModelLib.h"
#include "IndexBuffer/LSGIndexBuffer.h"
#include "LSDModelMaterial.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "VertexBuffer/LSGVertexBuffer.h"

namespace lsd {

	/**
	 * Class CSubMesh
	 * \brief A piece of a mesh.
	 *
	 * Description: A piece of a mesh.  Contains a vertex buffer pointer (the vertex buffer is owned by the mesh and
	 *	shared among all sub-meshes), its own index buffer, and a pointer to the material (materials are owned by the
	 *	model).
	 */
	class CSubMesh {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSubMesh();
		LSE_CALLCTOR							~CSubMesh();


		// == Operators.
		/**
		 * Assignment operator.
		 *
		 * \param _smOther The object to copy.
		 */
		LSE_INLINE CSubMesh & LSE_CALL			operator = ( const CSubMesh &_smOther );


		// == Functions.
		/**
		 * Loads a sub-mesh from a byte stream.
		 *
		 * \param _ebsStream Byte stream from which to read.
		 * \param _pvbVerts The vertex buffer to use.
		 * \param _pmMaterials Array of materials.
		 * \param _ui32Materials Total materials.
		 * \param _ui32Version File version number.
		 * \return Returns an error code indicating the result of the load.
		 */
		LSSTD_ERRORS LSE_CALL					LoadFromStream( const CExternalByteStream &_ebsStream, CVertexBuffer * _pvbVerts,
			const CModelMaterial * _pmMaterials, uint32_t _ui32Materials, uint32_t _ui32Version );

		/**
		 * Gets a constant reference to the index buffer for this sub-mesh.
		 *
		 * \return Returns a constant reference to the index buffer for this sub-mesh.
		 */
		LSE_INLINE const CIndexBuffer & LSE_CALL
												IndexBuffer() const;

		/**
		 * Gets a reference to the index buffer for this sub-mesh.
		 *
		 * \return Returns a reference to the index buffer for this sub-mesh.
		 */
		LSE_INLINE CIndexBuffer & LSE_CALL		IndexBuffer();

		/**
		 * Gets the index-buffer base offset.
		 *
		 * \return Returns the index-buffer base offset.
		 */
		LSE_INLINE uint32_t LSE_CALL			BaseOffset() const;

		/**
		 * Gets a constant pointer to the material for this sub-mesh.
		 *
		 * \return Returns a constant pointer to the material for this sub-mesh.
		 */
		LSE_INLINE const CModelMaterial * LSE_CALL
												Material() const;


	protected :
		// == Types.
		/** Data for the sub-mesh. */
#pragma pack( push, 1 )
		typedef struct LSD_SUB_MESH_DATA {
			uint64_t							ui64MaterialId;					/**< Material ID. */
			uint32_t							ui32Indices;					/**< Total indices in the index buffer. */
			uint32_t							ui32Base;						/**< Base offset. */
			uint16_t							ui16PrimType;					/**< Strips or lists. */
			uint16_t							ui16IndexSize;					/**< Size of each index. */
		} * LPLSD_SUB_MESH_DATA, * const LPCLSD_SUB_MESH_DATA;
#pragma pack( pop )

		// == Members.
		/** Our index buffer. */
		CIndexBuffer							m_ibIndices;

		/** The vertex buffer (owned by the mesh class) used by this class. */
		CVertexBuffer *							m_pvbVerts;

		/** The material used for this sub-mesh. */
		const CModelMaterial *					m_pmmMaterial;

		/** Sub-mesh data. */
		LSD_SUB_MESH_DATA 						m_smdData;
		
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	* Assignment operator.
	*
	* \param _smOther The object to copy.
	*/
	LSE_INLINE CSubMesh & LSE_CALL CSubMesh::operator = ( const CSubMesh &_smOther ) {
		m_ibIndices.Reset();	// Index buffer has to be recreated.
		m_pvbVerts = _smOther.m_pvbVerts;
		m_pmmMaterial = _smOther.m_pmmMaterial;
		m_smdData = _smOther.m_smdData;
		return (*this);
	}

	// == Functions.
	/**
	 * Gets a constant reference to the index buffer for this sub-mesh.
	 *
	 * \return Returns a constant reference to the index buffer for this sub-mesh.
	 */
	LSE_INLINE const CIndexBuffer & LSE_CALL CSubMesh::IndexBuffer() const { return m_ibIndices; }

	/**
	 * Gets a reference to the index buffer for this sub-mesh.
	 *
	 * \return Returns a reference to the index buffer for this sub-mesh.
	 */
	LSE_INLINE CIndexBuffer & LSE_CALL CSubMesh::IndexBuffer() { return m_ibIndices; }

	/**
	 * Gets the index-buffer base offset.
	 *
	 * \return Returns the index-buffer base offset.
	 */
	LSE_INLINE uint32_t LSE_CALL CSubMesh::BaseOffset() const { return m_smdData.ui32Base; }

	/**
	 * Gets a constant pointer to the material for this sub-mesh.
	 *
	 * \return Returns a constant pointer to the material for this sub-mesh.
	 */
	LSE_INLINE const CModelMaterial * LSE_CALL CSubMesh::Material() const { return m_pmmMaterial; }

}	// namespace lsd

#endif	// __LSD_SUBMESH_H__
