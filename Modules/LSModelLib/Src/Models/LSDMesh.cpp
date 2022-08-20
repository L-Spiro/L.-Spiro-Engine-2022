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

#include "LSDMesh.h"
#include "LSDModelAttributes.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CMesh::CMesh() :
		m_ui16Flags( 0 ),
		m_ui16Uvs( 0 ) {
	}
	LSE_CALLCTOR CMesh::~CMesh() {
	}

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
	LSSTD_ERRORS LSE_CALL CMesh::LoadFromStream( const CExternalByteStream &_ebsStream,
		const CModelMaterial * _pmMaterials, uint32_t _ui32Materials, uint32_t _ui32Version ) {
		const CExternalByteStream ebsThis( _ebsStream.GetCurBufferPointer(), _ebsStream.Length() - _ebsStream.GetPos() );
		// Read the header.
		LSD_LOAD_DATA ldData;
		if ( ebsThis.ReadBytes( reinterpret_cast<uint8_t *>(&ldData), sizeof( ldData ) ) != sizeof( ldData ) ) { return LSSTD_E_INVALIDDATA; }
		//ebsThis.PadRead( 16 );

		// Create the vertex format.
		LSG_VERTEX_ELEMENT_DESC vedDesc[LSG_MAX_VERTEX_ELEMENT_COUNT];
		uint32_t ui32VertIndex = 0;
		uint32_t ui32Size = 0;
		m_ui16Flags = ldData.ui8Flags;
		m_ui16Uvs = ldData.ui8TextureCoords;


		vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32_F;
		vedDesc[ui32VertIndex].ui32Offset = ui32Size;
		vedDesc[ui32VertIndex].ui32Semantic = LSG_S_POSITION;
		vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
		ui32Size += sizeof( float ) * 3;
		if ( ldData.ui8Flags & LSD_MA_NORMALS ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_NORMAL;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( float ) * 3;
		}
		if ( ldData.ui8Flags & LSD_MA_COLORS ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_COLOR;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( float ) * 3;
		}
		uint32_t ui32TexCoords = ldData.ui8TextureCoords;				// Texture coordinates.
		for ( uint32_t I = 0; I < ui32TexCoords; ++I ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_TEXCOORD;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = I;
			ui32Size += sizeof( float ) * 2;
		}

		if ( ldData.ui8Flags & LSD_MA_SKIN ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R8G8B8A8_UI;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_BLENDINDICES;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( uint8_t ) * 4;
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32A32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_BLENDWEIGHT;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( float ) * 4;
		}

		if ( ldData.ui8Flags & LSD_MA_TANGENT ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_TANGENT;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( float ) * 3;
		}

		if ( ldData.ui8Flags & LSD_MA_BITANGENT ) {
			vedDesc[ui32VertIndex].ui32Format = LSG_VE_R32G32B32_F;
			vedDesc[ui32VertIndex].ui32Offset = ui32Size;
			vedDesc[ui32VertIndex].ui32Semantic = LSG_S_BINORMAL;
			vedDesc[ui32VertIndex++].ui32SemanticIndex = 0;
			ui32Size += sizeof( float ) * 3;
		}

		ui32Size = ((ui32Size + (ldData.ui8VertexStride - 1)) / ldData.ui8VertexStride) * ldData.ui8VertexStride;
		ldData.ui8VertexStride = static_cast<uint8_t>(ui32Size);

		ui32Size *= ldData.ui32Vertices;

		if ( ui32Size ) {
			if ( !m_vbVertexBuffer.CreateVertexBuffer( ebsThis.GetCurBufferPointer(),
				vedDesc, ui32VertIndex,
				true,
				ldData.ui8VertexStride,
				ldData.ui32Vertices,
				LSG_VBUT_SETONLY,
				LSG_PT_TRIANGLELIST ) ) {
				return LSSTD_E_OUTOFMEMORY;
			}
		}

		ebsThis.SkipBytes( ui32Size );

		// Read the number of sub-meshes.
		uint32_t ui32SubMeshes = ebsThis.ReadUInt32();
		if ( !m_vSubMeshes.Resize( ui32SubMeshes ) ) { return LSSTD_E_OUTOFMEMORY; }
		for ( uint32_t I = 0; I < ui32SubMeshes; ++I ) {
			LSSTD_ERRORS eError = m_vSubMeshes[I].LoadFromStream( ebsThis, &m_vbVertexBuffer,_pmMaterials, _ui32Materials, _ui32Version );
			if ( eError != LSSTD_E_SUCCESS ) { return eError; }
		}
		//ebsThis.PadRead( 32 );

		_ebsStream.SkipBytes( ebsThis.GetPos() );
		return LSSTD_E_SUCCESS;
	}

}	// namespace lsd
