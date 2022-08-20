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
 * Description: A mesh.  A shared model is composed of 1 or more meshes.  Each mesh contains 1 vertex buffer
 *	and 1 or more sub-meshes.  Each sub-mesh has an index buffer and offset used with the mesh's vertex buffer.
 */


#ifndef __LSD_MESH_H__
#define __LSD_MESH_H__

#include "../LSDModelLib.h"
#include "LSDModelMaterial.h"
#include "LSDSubMesh.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "Vector/LSTLVector.h"

namespace lsd {

	/**
	 * Class CMesh
	 * \brief A mesh.
	 *
	 * Description: A mesh.  A shared model is composed of 1 or more meshes.  Each mesh contains 1 vertex buffer
	 *	and 1 or more sub-meshes.  Each sub-mesh has an index buffer and offset used with the mesh's vertex buffer.
	 */
	class CMesh {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMesh();
		LSE_CALLCTOR							~CMesh();


		// == Functions.
		/**
		 * Loads a sub-mesh from a byte stream.
		 *
		 * \param _ebsStream Byte stream from which to read.
		 * \param _pmMaterials Array of materials.
		 * \param _ui32Materials Total materials.
		 * \param _ui32Version File version number.
		 * \return Returns an error code indicating the result of the load.
		 */
		LSSTD_ERRORS LSE_CALL					LoadFromStream( const CExternalByteStream &_ebsStream,
			const CModelMaterial * _pmMaterials, uint32_t _ui32Materials, uint32_t _ui32Version );

		/**
		 * Gets a constant reference to the array of sub-meshes on this mesh.
		 *
		 * \return Returns a constant reference to the array of sub-meshes on this mesh.
		 */
		LSE_INLINE const CVector<CSubMesh> & LSE_CALL
												SubMeshes() const;

		/**
		 * Gets a reference to the array of sub-meshes on this mesh.
		 *
		 * \return Returns a reference to the array of sub-meshes on this mesh.
		 */
		LSE_INLINE CVector<CSubMesh> & LSE_CALL	SubMeshes();

		/**
		 * Gets a constant reference to the vertex buffer for this mesh.
		 *
		 * \return Returns a constant reference to the vertex buffer for this mesh.
		 */
		LSE_INLINE const CVertexBuffer & LSE_CALL
												VertexBuffer() const;

		/**
		 * Gets a reference to the vertex buffer for this mesh.
		 *
		 * \return Returns a reference to the vertex buffer for this mesh.
		 */
		LSE_INLINE CVertexBuffer & LSE_CALL		VertexBuffer();

		/**
		 * Gets the vertex-buffer flags.
		 *
		 * \return Returns the vertex-buffer flags.
		 */
		LSE_INLINE uint32_t LSE_CALL			Flags() const;

		/**
		 * Gets the number of UV sets in the vertex buffer.
		 *
		 * \return Returns the number of UV sets in the vertex buffer.
		 */
		LSE_INLINE uint32_t LSE_CALL			UvSets() const;

	protected :
		// == Types.
		/** Load data. */
#pragma pack( push, 1 )
		typedef struct LSD_LOAD_DATA {
			uint32_t							ui32Vertices;			/**< Number of vertices in the vertex buffer. */
			uint8_t								ui8Flags;				/**< Vertex-buffer flags. */
			uint8_t								ui8TextureCoords;		/**< Total texture coordinates (layers). */
			uint8_t								ui8SkinWeights;			/**< Total skin weights (must match LSD_MA_MAX_SKINS). */
			uint8_t								ui8VertexStride;		/**< Vertex-buffer stride. */
		} * LPLSD_LOAD_DATA, * const LPCLSD_LOAD_DATA;
#pragma pack( pop )

		// == Members.
		/** Array of sub-meshes. */
		CVector<CSubMesh>						m_vSubMeshes;

		/** The vertex buffer for this mesh. */
		CVertexBuffer							m_vbVertexBuffer;

		/** Vertex-buffer flags. */
		uint16_t								m_ui16Flags;

		/** Number of UV coordinates. */
		uint16_t								m_ui16Uvs;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a constant reference to the array of sub-meshes on this mesh.
	 *
	 * \return Returns a constant reference to the array of sub-meshes on this mesh.
	 */
	LSE_INLINE const CVector<CSubMesh> & LSE_CALL CMesh::SubMeshes() const { return m_vSubMeshes; }

	/**
	 * Gets a reference to the array of sub-meshes on this mesh.
	 *
	 * \return Returns a reference to the array of sub-meshes on this mesh.
	 */
	LSE_INLINE CVector<CSubMesh> & LSE_CALL CMesh::SubMeshes() { return m_vSubMeshes; }

	/**
	 * Gets a constant reference to the vertex buffer for this mesh.
	 *
	 * \return Returns a constant reference to the vertex buffer for this mesh.
	 */
	LSE_INLINE const CVertexBuffer & LSE_CALL CMesh::VertexBuffer() const { return m_vbVertexBuffer; }

	/**
	 * Gets a reference to the vertex buffer for this mesh.
	 *
	 * \return Returns a reference to the vertex buffer for this mesh.
	 */
	LSE_INLINE CVertexBuffer & LSE_CALL CMesh::VertexBuffer() { return m_vbVertexBuffer; }

	/**
	 * Gets the vertex-buffer flags.
	 *
	 * \return Returns the vertex-buffer flags.
	 */
	LSE_INLINE uint32_t LSE_CALL CMesh::Flags() const { return m_ui16Flags; }

	/**
	 * Gets the number of UV sets in the vertex buffer.
	 *
	 * \return Returns the number of UV sets in the vertex buffer.
	 */
	LSE_INLINE uint32_t LSE_CALL CMesh::UvSets() const { return m_ui16Uvs; }

}	// namespace lsd

#endif	// __LSD_MESH_H__
