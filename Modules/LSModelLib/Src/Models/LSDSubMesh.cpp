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

#include "LSDSubMesh.h"
#include "LSDModelAttributes.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CSubMesh::CSubMesh() :
		m_pvbVerts( NULL ),
		m_pmmMaterial( NULL ) {
	}
	LSE_CALLCTOR CSubMesh::~CSubMesh() {
	}

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
	LSSTD_ERRORS LSE_CALL CSubMesh::LoadFromStream( const CExternalByteStream &_ebsStream, CVertexBuffer * _pvbVerts,
		const CModelMaterial * _pmMaterials, uint32_t _ui32Materials, uint32_t /*_ui32Version*/ ) {
		m_pvbVerts = _pvbVerts;
		_ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&m_smdData), sizeof( LSD_SUB_MESH_DATA ) );
		//_ebsStream.PadRead( 16 );

		m_pmmMaterial = NULL;
		for ( uint32_t I = _ui32Materials; I--; ) {
			if ( _pmMaterials[I].Id() == m_smdData.ui64MaterialId ) {
				m_pmmMaterial = &_pmMaterials[I];
				break;
			}
		}

		if ( !m_ibIndices.CreateIndexBuffer( _ebsStream.GetCurBufferPointer(),
			m_smdData.ui32Indices,
			m_smdData.ui16IndexSize,
			true,
			LSG_IBUT_SETONLY,
			(m_smdData.ui16PrimType == LSD_MA_TRIANGLE_STRIP) ? LSG_PT_TRIANGLESTRIP : LSG_PT_TRIANGLELIST ) ) { return LSSTD_E_OUTOFMEMORY; }
		_ebsStream.SkipBytes( m_smdData.ui32Indices * 2 );

		return LSSTD_E_SUCCESS;
	}

}	// namespace lsd
