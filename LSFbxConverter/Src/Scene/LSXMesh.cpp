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
 * Description: Loads mesh data.
 */

#include "LSXMesh.h"
#include "../Misc/LSXMisc.h"
#include "TriangleCache/LSCTriangleCacheOptimizer.h"
#include "TriangleStrips/LSCTriangleStripper.h"
#include <map>


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CMesh::CMesh() :
		m_pfmMesh( NULL ),
		m_ui32Flags( 0 ),
		m_ui32TexCoords( 0 ),
		m_ui32Triangles( 0 ),
		m_ui32Index( 0 ) {
	}
	LSE_CALLCTOR CMesh::~CMesh() {
	}

	// == Operators.
	/**
	 * Equality operator.
	 *
	 * \param _mOther Mesh to which to check for euqlity.
	 * \return Returns true if the meshes have all the same vertex and index data.
	 */
	LSBOOL LSE_CALL CMesh::operator == ( const CMesh &_mOther ) const {
		if ( m_ui32Flags != _mOther.m_ui32Flags ) { return false; }
		if ( m_ui32TexCoords != _mOther.m_ui32TexCoords ) { return false; }
		if ( m_ui32Triangles != _mOther.m_ui32Triangles ) { return false; }
		if ( m_mpaIndexBuffers.Length() != _mOther.m_mpaIndexBuffers.Length() ) { return false; }
		if ( m_vbVertexBuffer.Length() != _mOther.m_vbVertexBuffer.Length() ) { return false; }
		// Check each index buffer.
		for ( LSUINT32 I = m_mpaIndexBuffers.Length(); I--; ) {
			// Index base.
			if ( m_mpaIndexBuffers[I].ui32Base != _mOther.m_mpaIndexBuffers[I].ui32Base ) { return false; }

			// Material.
			if ( !(m_mpaIndexBuffers[I].mkMaterial == _mOther.m_mpaIndexBuffers[I].mkMaterial) ) {
				return false;
			}

			// Indices.
			if ( m_mpaIndexBuffers[I].ibIndexBuffer.Length() != _mOther.m_mpaIndexBuffers[I].ibIndexBuffer.Length() ) { return false; }
			if ( m_mpaIndexBuffers[I].ibIndexBuffer.Length() ) {
				if ( !CStd::MemCmpF( &m_mpaIndexBuffers[I].ibIndexBuffer[0], &_mOther.m_mpaIndexBuffers[I].ibIndexBuffer[0],
					m_mpaIndexBuffers[I].ibIndexBuffer.Length() * sizeof( m_mpaIndexBuffers[I].ibIndexBuffer[0] ) ) ) { return false; }
			}
		}
		// Check the vertex buffer.
		if ( m_vbVertexBuffer.Length() ) {
			if ( !CStd::MemCmpF( &m_vbVertexBuffer[0], &_mOther.m_vbVertexBuffer[0],
				m_vbVertexBuffer.Length() * sizeof( m_vbVertexBuffer[0] ) ) ) { return false; }
		}
		return true;
	}

	// == Functions.
	/**
	 * Loads a mesh.
	 *
	 * \param _pfmMesh The mesh node to load.
	 * \param _sMaterials Materials referenced by this object.
	 * \return Returns true if the mesh was loaded.
	 */
	LSBOOL LSE_CALL CMesh::LoadMesh( FbxMesh * _pfmMesh, CSet<FbxSurfaceMaterial *> &_sMaterials ) {
		m_pfmMesh = _pfmMesh;
		LSUINT32 ui32TotalLayers = m_pfmMesh->GetLayerCount();
		if ( !ui32TotalLayers ) { return true; }
		LSUINT32 ui32TotalNormals = m_pfmMesh->GetLayer( 0 )->GetNormals() ? m_pfmMesh->GetLayer( 0 )->GetNormals()->GetDirectArray().GetCount() :
			0UL;
		LSUINT32 ui32TotalColors = m_pfmMesh->GetLayer( 0 )->GetVertexColors() ? m_pfmMesh->GetLayer( 0 )->GetVertexColors()->GetDirectArray().GetCount() :
			0UL;
		if ( ui32TotalNormals ) { m_ui32Flags |= LSX_NORMALS; }
		if ( ui32TotalColors ) { m_ui32Flags |= LSX_COLORS; }
		// Each layer may have 1 or 0 diffuse texture UV’s.  Count them layer-by-layer.
		for ( LSUINT32 I = 0UL; I < ui32TotalLayers; ++I ) {
			const FbxLayerElementUV * pleuUvs = m_pfmMesh->GetLayer( I )->GetUVs( FbxLayerElement::eTextureDiffuse );
			m_ui32TexCoords += pleuUvs ? 1UL : 0UL;
		}
		m_ui32TexCoords = CStd::Min<LSUINT32>( m_ui32TexCoords, LSX_MAX_UVS );

		LSBOOL bNormalMapping = false;

		// Determine if there is skinning and if so build a table.
		LSX_SKIN_TABLE stSkinTable;
		if ( !MakeSkinTable( m_pfmMesh, stSkinTable ) ) { return false; }
		if ( stSkinTable.ui32Joints ) { m_ui32Flags |= LSX_SKIN; }

		CLinearMap<LSX_MATERIAL_KEY, LSX_FACE_VERTEX_PAIR> mBuckets;
		CVector<LSX_FACE::LSX_FACE_VERTEX, LSUINT32> vVertexPool;
		LSUINT32 ui32FaceCount = m_pfmMesh->GetPolygonCount();
		LSX_ADD_VERTEX favAddVertex = { 0UL };
		for ( LSUINT32 I = 0; I < ui32FaceCount; ++I ) {
			//LSUINT32 ui32PolyTotal = m_pfmMesh->GetPolygonSize( I );
			//LSUINT32 ui32PolyStart = m_pfmMesh->GetPolygonVertexIndex( I );

			LSX_MATERIAL_KEY fmkThisFaceMaterials;
			for ( LSUINT32 J = 0; J < LSX_MAX_UVS; ++J ) {
				// Be sure to fill a value into every pcMaterial[], even if there are not enough
				//	layers on the mesh.
				if ( J >= ui32TotalLayers ) {
					fmkThisFaceMaterials.pcMaterial[J] = NULL;
					fmkThisFaceMaterials.psmMaterialPtr[J] = NULL;
					continue;
				}
				// There is a layer here.  Get its material name.
				const FbxLayerElementMaterial * plemMaterials = m_pfmMesh->GetLayer( J )->GetMaterials();
				if ( !plemMaterials || plemMaterials->GetMappingMode() == FbxGeometryElement::eAllSame ) {
					// No material for this layer or all layers use material 0.
					plemMaterials = m_pfmMesh->GetLayer( 0 )->GetMaterials();
				}
				if ( !plemMaterials ) {
					fmkThisFaceMaterials.pcMaterial[J] = NULL;
					fmkThisFaceMaterials.psmMaterialPtr[J] = NULL;
					continue;
				}
				// Get the index of the material for this face and layer.
				LSUINT32 ui32MaterialIndex = plemMaterials->GetIndexArray().GetAt( I );
				// Index into the mesh's array of materials to get the name.
				FbxSurfaceMaterial * pfsmMaterial = reinterpret_cast<FbxSurfaceMaterial *>(m_pfmMesh->GetNode()->GetMaterial( ui32MaterialIndex ));
				fmkThisFaceMaterials.pcMaterial[J] = pfsmMaterial->GetName();
				fmkThisFaceMaterials.psmMaterialPtr[J] = pfsmMaterial;
				_sMaterials.Insert( pfsmMaterial );
				
				// Check for normal mapping or bump mapping.
				FbxProperty pNormalMap = pfsmMaterial->FindProperty( FbxLayerElement::sTextureChannelNames[FbxLayerElement::eTextureNormalMap-FbxLayerElement::eTextureDiffuse] );
				if ( !pNormalMap.IsValid() || !pNormalMap.GetSrcObjectCount<FbxFileTexture>() ) {
					pNormalMap = pfsmMaterial->FindProperty( FbxLayerElement::sTextureChannelNames[FbxLayerElement::eTextureBump-FbxLayerElement::eTextureDiffuse] );
				}
				if ( pNormalMap.IsValid() && pNormalMap.GetSrcObjectCount<FbxFileTexture>() ) {
					m_ui32Flags |= LSX_TANGENT | LSX_BITANGENT;
					bNormalMapping = true;
				}
				/*if ( CStd::StrLen( pfsmMaterial->GetName() ) == 0 ) {
					::printf( "0x%X (MATERIAL)\r\n", pfsmMaterial );
				}*/
				//::printf( "%s\r\n", pfsmMaterial->ShadingModel.Get().Buffer() );
			}

			if ( !AddFace( I, mBuckets, vVertexPool, fmkThisFaceMaterials, favAddVertex ) ) {
				return false;
			}
		}

		// Now we can triangulate the faces using our intermediate faces.
		//	Note that it is possible to convert a whole face to our intermediate
		//	format and then to convert that face just afterwards, but we
		//	keep these processes separate for clarity here.
		// Go over each bucket.
		for ( LSUINT32 I = 0; I < mBuckets.Length(); ++I ) {
			// Go over each face in the bucket.
			//for ( CVector<LSX_FACE>::iterator J = I->second.vFaces.begin(); J != I->second.vFaces.end(); ++J ) {
			for ( LSUINT32 J = 0; J < mBuckets.GetByIndex( I ).vFaces.Length(); ++J ) {
				// Go over each vertex on the face.
				LSUINT32 ui32PolyTotal = mBuckets.GetByIndex( I ).vFaces[J].ui32TotalVertices;
				LSUINT32 ui32PolyStart = mBuckets.GetByIndex( I ).vFaces[J].ui32StartVertex;
				// Every triangle's first vertex is at index 0.
				//	Each next vertex is at J and the each final vertex is at J + 1.
				//	Hence the loop goes from 1 to ui32PolyTotal - 1.
				LSUINT32 ui32LoopEnd = ui32PolyTotal - 1;
				for ( LSUINT32 J = 1; J < ui32LoopEnd; ++J ) {
					// 0 added for clarity.  Each index is offset by ui32PolyStart.
					LSUINT32 ui32A = 0 + ui32PolyStart;
					LSUINT32 ui32B = J + ui32PolyStart;
					LSUINT32 ui32C = J + 1 + ui32PolyStart;
					
					if ( !AddVertex( ui32A, mBuckets.GetByIndex( I ).vVertices, vVertexPool, stSkinTable ) ) { return false; }
					if ( !AddVertex( ui32C, mBuckets.GetByIndex( I ).vVertices, vVertexPool, stSkinTable ) ) { return false; }
					if ( !AddVertex( ui32B, mBuckets.GetByIndex( I ).vVertices, vVertexPool, stSkinTable ) ) { return false; }
					if ( bNormalMapping ) {
						// Generate binormals.
						LSX_RAW_VERTEX * prvTriStart = &mBuckets.GetByIndex( I ).vVertices[mBuckets.GetByIndex( I ).vVertices.Length()-3];
						const LSX_VECTOR & vA = prvTriStart[0].vPosition;
						const LSX_VECTOR & vB = prvTriStart[1].vPosition;
						const LSX_VECTOR & vC = prvTriStart[2].vPosition;

						const LSX_VECTOR & vUv0 = LSX_VECTOR( static_cast<LSFLOAT>(prvTriStart[0].vUvs[0][0]), static_cast<LSFLOAT>(prvTriStart[0].vUvs[0][1]), 0.0f );
						const LSX_VECTOR & vUv1 = LSX_VECTOR( static_cast<LSFLOAT>(prvTriStart[1].vUvs[0][0]), static_cast<LSFLOAT>(prvTriStart[1].vUvs[0][1]), 0.0f );
						const LSX_VECTOR & vUv2 = LSX_VECTOR( static_cast<LSFLOAT>(prvTriStart[2].vUvs[0][0]), static_cast<LSFLOAT>(prvTriStart[2].vUvs[0][1]), 0.0f );

						const LSX_VECTOR vBa = vB - vA;
						const LSX_VECTOR vCa = vC - vA;

						LSX_VECTOR vNormal = vBa % vCa;

						const LSX_VECTOR vDeltaUv0 = vUv1 - vUv0;
						const LSX_VECTOR vDeltaUv1 = vUv2 - vUv0;

						LSFLOAT fR = 1.0f / (vDeltaUv0.x * vDeltaUv1.y - vDeltaUv0.y * vDeltaUv1.x);
						LSX_VECTOR vTangent = (vBa * vDeltaUv1.y - vCa * vDeltaUv0.y) * fR;
						LSX_VECTOR vBiTangent = (vCa * vDeltaUv0.x - vBa * vDeltaUv1.x) * fR;
						vTangent.Normalize();
						vBiTangent.Normalize();

						if ( ((vNormal % vTangent) * vBiTangent) < 0.0f ) {
							vTangent = -vTangent;
						}


						if ( m_ui32Flags & LSX_TANGENT ) {
							prvTriStart[0].vTangent = vTangent;
							prvTriStart[1].vTangent = vTangent;
							prvTriStart[2].vTangent = vTangent;
						}
						if ( m_ui32Flags & LSX_BITANGENT ) {
							prvTriStart[0].vBiTangent = vBiTangent;
							prvTriStart[1].vBiTangent = vBiTangent;
							prvTriStart[2].vBiTangent = vBiTangent;
						}
					}
				}
			}
			// We no longer need this intermediate face list.  Remove
			//	it to save memory.
			mBuckets.GetByIndex( I ).vFaces.Reset();
		}
		// No longer need the pool of vertices.  Remove it to save memory.
		vVertexPool.Reset();


		// We will have a vector of index buffer/vertex buffer pairs
		//	such that each element in the vector is a one-to-one
		//	correlation to the materials in mBuckets.
		CVector<LSX_INDEX_VERTEX_PAIR, LSUINT32> vIndexVertexBuffers;
		// The possible exception should be handled.
		vIndexVertexBuffers.Resize( mBuckets.Length() );

		// Go over each vertex buffer in mBuckets and convert
		//	to an index buffer/vertex pair.
		LSUINT32 ui32NewIdx = 0;
		for ( LSUINT32 I = 0; I < mBuckets.Length(); ++I, ++ui32NewIdx ) {
			if ( !CreateIndexBuffer( mBuckets.GetByIndex( I ).vVertices, vIndexVertexBuffers[ui32NewIdx].vVertices, vIndexVertexBuffers[ui32NewIdx].vIndices ) ) {
				return false;
			}
			// We no longer need the expanded vertex buffer.
			mBuckets.GetByIndex( I ).vVertices.Reset();
		}

		// Combine all the vertices and indices into one vertex
		//	buffer and one index buffer, and keep track of ranges
		//	into the index buffers where the vertices for a material are.
		LSX_INDEX_BUFFER_RANGE_ARRAY ibraRangeTable;
		LSX_INDEX_BUFFER ibMainIndexBuffer;
		ibraRangeTable.Allocate( mBuckets.Length() );
		for ( LSUINT32 I = 0; I < mBuckets.Length(); ++I ) {
			// Store the range of indices for this material.
			// Handling the possible exception is left to the reader.
			LSX_INDEX_BUFFER_RANGE ibrRange = {
				ibMainIndexBuffer.Length(),
				vIndexVertexBuffers[I].vIndices.Length(),
				mBuckets.GetKeyByIndex( I )
			};
			
			if ( !ibraRangeTable.Push( ibrRange ) ) { return false; }

			// The vertices can be added to the final vertex buffer
			//	via a straight copy, but the indices need to be
			//	adjusted to point to the new locations of the
			//	vertices to which they originally point.
			// Add the indices one-by-one and apply that offset
			//	to each as they are added to the final index buffer.
			// m_vbVertexBuffer.Length() is the offset
			//	amount.
			for ( LSUINT32 J = 0; J < vIndexVertexBuffers[I].vIndices.Length(); ++J ) {
				if ( !ibMainIndexBuffer.Push( vIndexVertexBuffers[I].vIndices[J] + m_vbVertexBuffer.Length() ) ) { return false; }
			}
			// Now add the vertices.
			for ( LSUINT32 J = 0; J < vIndexVertexBuffers[I].vVertices.Length(); ++J ) {
				if ( !m_vbVertexBuffer.Push( vIndexVertexBuffers[I].vVertices[J] ) ) { return false; }
			}

		}

		m_ui32Triangles = ibMainIndexBuffer.Length() / 3;


		// Currently we have 1 large vertex buffer, 1 large index buffer, and an array of ranges into that index buffer,
		//	each representing one draw call for the material associated with it.
		// Next we need to break the index buffers here into segments small enough to hold 16-bit indices.
		m_mpaIndexBuffers.Allocate( ibraRangeTable.Length() );
		for ( LSUINT32 I = 0; I < ibraRangeTable.Length(); ++I ) {
			LSUINT32 ui32Base = 0;
			LSBOOL bBeenHere = false;
			while ( ui32Base != LSE_MAXU32 ) {
				ui32Base = IndexBufferLowestIndex( ibMainIndexBuffer, ibraRangeTable[I] );
				if ( ui32Base == LSE_MAXU32 ) { break; }

				if ( bBeenHere ) {
					::printf( "\t\t(reducing index buffer size)\r\n", m_pfmMesh->GetName() );
				}
				bBeenHere = true;
				//ui32Base = (ui32Base / 3) * 3;	// Round to the bottom of the triangle.
				if ( !m_mpaIndexBuffers.Push( LSX_MESH_PART() ) ) { return false; }
				LSX_MESH_PART & mpThis = m_mpaIndexBuffers[m_mpaIndexBuffers.Length()-1];
				mpThis.mkMaterial = ibraRangeTable[I].mkMaterial;
				mpThis.ui32Base = ui32Base;
				mpThis.ui32IndexSize = 2;
				// Gather all the triangles we can in this range.
				LSUINT32 ui32Tris = ibraRangeTable[I].ui32Total / 3;
				LSUINT32 ui32TriRangeStart = ibraRangeTable[I].ui32Start / 3;
				for ( LSUINT32 J = 0; J < ui32Tris; ++J ) {
					if ( TriIs16Bit( ui32Base, ibMainIndexBuffer, J + ui32TriRangeStart ) ) {
						// Add 3 indices to the current index buffer and blank them from the old index buffer.
						for ( LSUINT32 K = 0; K < 3; ++K ) {
							LSUINT32 ui32ThisIndex = (J * 3) + ibraRangeTable[I].ui32Start + K;
							if ( !mpThis.ibIndexBuffer.Push( ibMainIndexBuffer[ui32ThisIndex] - ui32Base ) ) { return false; }
							ibMainIndexBuffer[ui32ThisIndex] = LSE_MAXU32;
						}
					}
				}
			}
		}

		LSUINT32 ui32MaxIndexBufferLen = 0;
		for ( LSUINT32 I = 0; I < m_mpaIndexBuffers.Length(); ++I ) {
			LSX_MESH_PART & mpThis = m_mpaIndexBuffers[I];
			mpThis.maPrimType = LSD_MA_TRIANGLE_LIST;
			if ( mpThis.ibIndexBuffer.Length() ) {
				CMesh::LSX_INDEX_BUFFER ibOut;
				if ( ibOut.Resize( mpThis.ibIndexBuffer.Length() ) ) {
					CTriangleCacheOptimizer tcoOptimizer;
					if ( tcoOptimizer.Optimize( &mpThis.ibIndexBuffer[0], mpThis.ibIndexBuffer.Length(), &ibOut[0] ) ) {
						CStd::MemCpy( &mpThis.ibIndexBuffer[0], &ibOut[0], sizeof( ibOut[0] ) * ibOut.Length() );
					}
				}


				LSUINT32 ui32Faces = mpThis.ibIndexBuffer.Length() / 3;
				CTriangleStripper tsStripper;
				tsStripper.InitTriangleStripper( ui32Faces,
					&mpThis.ibIndexBuffer[0],
					NULL );
				if ( tsStripper.Compute() ) {
					LSDOUBLE dRatio = static_cast<LSDOUBLE>(tsStripper.TotalIndices()) / static_cast<LSDOUBLE>(mpThis.ibIndexBuffer.Length());
					if ( dRatio < 0.3 ) {
						::printf( "\t\tStrip: %f (%u -> %u).\r\n", dRatio, mpThis.ibIndexBuffer.Length(), tsStripper.TotalIndices() );
						mpThis.ibIndexBuffer.Resize( tsStripper.TotalIndices() );
						CStd::MemCpy( &mpThis.ibIndexBuffer[0], tsStripper.IndicesAsDoubleWords(), sizeof( mpThis.ibIndexBuffer[0] ) * tsStripper.TotalIndices() );
						mpThis.maPrimType = LSD_MA_TRIANGLE_STRIP;
					}
				}
			}

			ui32MaxIndexBufferLen = CStd::Max( ui32MaxIndexBufferLen, mpThis.ibIndexBuffer.Length() );
			//for ( LSUINT32 J = 0; J < mpThis.ibIndexBuffer.Length(); ++J ) {
			//	ui32MaxIndexBufferLen = CStd::Max( ui32MaxIndexBufferLen, mpThis.
			//}
		}


		::printf( "Loaded mesh \"%s\".\r\n", m_pfmMesh->GetName() );
		if ( m_mpaIndexBuffers.Length() == 1 ) {
			::printf( "\t%u vertices, %u indices.\r\n",
				m_vbVertexBuffer.Length(),
				ibMainIndexBuffer.Length() );
		}
		else {
			 ::printf( "\t%u vertices, %u indices, %u index buffers, max index-buffer length: %u.\r\n",
				m_vbVertexBuffer.Length(),
				ibMainIndexBuffer.Length(),
				m_mpaIndexBuffers.Length(),
				ui32MaxIndexBufferLen );
		}

		return true;
	}

	/**
	 * Save the mesh to a binary buffer.
	 *
	 * \param _bBuffer The buffer where the data should be written.
	 * \param _ui32Index The index of this mesh.
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CMesh::SaveMesh( lsc::CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32Index ) {
#define LSX_STRIDE		16
		m_ui32Index = _ui32Index;
		// Size of the vertex buffer.
		if ( !CMisc::WriteUInt( m_vbVertexBuffer.Length(), _bBuffer ) ) { return false; }
		// Write flags.
		if ( !CMisc::WriteUChar( static_cast<LSUINT8>(m_ui32Flags), _bBuffer ) ) { return false; }
		// Total texture coordinates.
		if ( !CMisc::WriteUChar( static_cast<LSUINT8>(m_ui32TexCoords), _bBuffer ) ) { return false; }
		// Total skin indices (for verification).
		if ( !CMisc::WriteUChar( LSX_MAX_SKIN_WEIGHTS, _bBuffer ) ) { return false; }
		// Vertex-buffer stride (bytes between vertices).
		if ( !CMisc::WriteUChar( LSX_STRIDE, _bBuffer ) ) { return false; }
		// Pad.
		//if ( !CMisc::Pad( _bBuffer, 16 ) ) { return false; }

		lsc::CMisc::LSC_BUFFER bTemp;

		// Write the vertex buffer.
		for ( LSUINT32 I = 0; I < m_vbVertexBuffer.Length(); ++I ) {
			if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vPosition[0]), bTemp ) ) { return false; }
			if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vPosition[1]), bTemp ) ) { return false; }
			if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vPosition[2]), bTemp ) ) { return false; }

			if ( m_ui32Flags & LSX_NORMALS ) {
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vNormal[0]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vNormal[1]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vNormal[2]), bTemp ) ) { return false; }
			}
			if ( m_ui32Flags & LSX_COLORS ) {
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vColor[0]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vColor[1]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vColor[2]), bTemp ) ) { return false; }
			}
			for ( LSUINT32 J = 0; J < m_ui32TexCoords; ++J ) {
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vUvs[J][0]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( 1.0f - static_cast<LSFLOAT>(m_vbVertexBuffer[I].vUvs[J][1]), bTemp ) ) { return false; }
			}
			if ( m_ui32Flags & LSX_SKIN ) {
				for ( LSUINT32 J = 0; J < LSX_MAX_SKIN_WEIGHTS; ++J ) {
					if ( !CMisc::WriteUChar( m_vbVertexBuffer[I].ui8SkinIndices[J], bTemp ) ) { return false; }
				}
				for ( LSUINT32 J = 0; J < LSX_MAX_SKIN_WEIGHTS; ++J ) {
					if ( !CMisc::WriteFloat( m_vbVertexBuffer[I].fSkinWeights[J], bTemp ) ) { return false; }
				}
			}

			if ( m_ui32Flags & LSX_TANGENT ) {
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vTangent[0]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vTangent[1]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vTangent[2]), bTemp ) ) { return false; }
			}
			if ( m_ui32Flags & LSX_BITANGENT ) {
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vBiTangent[0]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vBiTangent[1]), bTemp ) ) { return false; }
				if ( !CMisc::WriteFloat( static_cast<LSFLOAT>(m_vbVertexBuffer[I].vBiTangent[2]), bTemp ) ) { return false; }
			}
			if ( !CMisc::Pad( bTemp, LSX_STRIDE ) ) { return false; }
		}
		if ( !CMisc::WriteBytes( &bTemp[0], bTemp.Length(), _bBuffer ) ) { return false; }
		bTemp.Reset();

		// Total number of index buffers.
		if ( !CMisc::WriteUInt( m_mpaIndexBuffers.Length(), _bBuffer ) ) { return false; }
		for ( LSUINT32 I = 0; I < m_mpaIndexBuffers.Length(); ++I ) {
			// Material 0 ID.
			LSUINT64 ui64MatId = (*reinterpret_cast<LSUINTPTR *>(&m_mpaIndexBuffers[I].mkMaterial.psmMaterialPtr[0]));
			if ( !CMisc::WriteUInt64( ui64MatId, _bBuffer ) ) { return false; }
			// Number of indices.
			if ( !CMisc::WriteUInt( m_mpaIndexBuffers[I].ibIndexBuffer.Length(), _bBuffer ) ) { return false; }
			// Base.
			if ( !CMisc::WriteUInt( m_mpaIndexBuffers[I].ui32Base, _bBuffer ) ) { return false; }
			// Strips or lists.
			if ( !CMisc::WriteUInt16( static_cast<LSUINT16>(m_mpaIndexBuffers[I].maPrimType), _bBuffer ) ) { return false; }
			// Index size.
			if ( !CMisc::WriteUInt16( static_cast<LSUINT16>(m_mpaIndexBuffers[I].ui32IndexSize), _bBuffer ) ) { return false; }

			
			// Pad to 16 bytes.
			//if ( !CMisc::Pad( _bBuffer, 16 ) ) { return false; }
			// Write the indices.
			for ( LSUINT32 J = 0; J < m_mpaIndexBuffers[I].ibIndexBuffer.Length(); ++J ) {
				if ( !CMisc::WriteUInt16( static_cast<LSUINT16>(m_mpaIndexBuffers[I].ibIndexBuffer[J]), _bBuffer ) ) { return false; }
			}
		}
		//if ( !CMisc::Pad( _bBuffer, 32 ) ) { return false; }

		return true;
#undef LSX_STRIDE
	}

	/**
	 * Adds a vertex to a vertex pool and adds an intermediate face to the proper bucket.
	 *
	 * \param _ui32FaceIndex Index of the face to add.
	 * \param _mBuckets The buckets to which to add the face.
	 * \param _vVertices The pool to which to add the vertices.
	 * \param _fmkKey The key for _mBuffers to know to which vertex buffer to add the vertex.
	 * \param _favAddVert In/out parameters for adding the vertex.
	 * \return Returns true if the vertex was added.
	 */
	LSBOOL LSE_CALL CMesh::AddFace( LSUINT32 _ui32FaceIndex,
		CLinearMap<LSX_MATERIAL_KEY, LSX_FACE_VERTEX_PAIR> &_mBuckets,
		CVector<LSX_FACE::LSX_FACE_VERTEX, LSUINT32> &_vVertices,
		const LSX_MATERIAL_KEY &_fmkKey,
		LSX_ADD_VERTEX &_favAddVert ) const {

		LSUINT32 ui32PolyTotal = m_pfmMesh->GetPolygonSize( _ui32FaceIndex );
		LSUINT32 ui32PolyStart = m_pfmMesh->GetPolygonVertexIndex( _ui32FaceIndex );
		int * piFacePool = m_pfmMesh->GetPolygonVertices();

		// Create the intermediate face.
		LSX_FACE ffFace = {
			_vVertices.Length(),					// ffFace.ui32StartVertex.
			ui32PolyTotal,							// ffFace.ui32TotalVertices
		};
		// Add the face to the appropriate bucket.
		if ( !_mBuckets[_fmkKey].vFaces.Push( ffFace ) ) { return false; }

		// Go over each vertex in the face.
		for ( LSUINT32 I = 0UL; I < ui32PolyTotal; ++I ) {
			LSX_FACE::LSX_FACE_VERTEX ffvThisVert;
			LSUINT32 ui32VertIndex = piFacePool[ui32PolyStart+I];
			ffvThisVert.ui32VertexIndex = ui32VertIndex;

			if ( m_ui32Flags & LSX_NORMALS ) {
				ffvThisVert.ui32NormalIndex = 0UL;
				const FbxLayerElementNormal * plenNormals = m_pfmMesh->GetLayer( 0 )->GetNormals();
				switch ( plenNormals->GetMappingMode() ) {
					case FbxGeometryElement::eByControlPoint : {
						// The normal index is based on the vertex index.
						if ( plenNormals->GetReferenceMode() == FbxLayerElement::eDirect ) {
							// The normal index is the same as the vertex index.
							ffvThisVert.ui32NormalIndex = ui32VertIndex;
						}
						else if ( plenNormals->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							// The normal index is found by plugging the vertex index into an array of indices.
							ffvThisVert.ui32NormalIndex = plenNormals->GetIndexArray().GetAt( ui32VertIndex );
						}
						if ( ffvThisVert.ui32NormalIndex >= static_cast<LSUINT32>(plenNormals->GetDirectArray().GetCount()) ) {
							::printf( "Invalid normal index.\r\n" );
						}
						break;
					}
					case FbxGeometryElement::eByPolygonVertex : {
						// The normal index is based off _favAddVert.ui32IndexCount, which is
						//	incremented once for each vertex over which we loop.
						if ( plenNormals->GetReferenceMode() == FbxLayerElement::eDirect ) {
							// It is just _favAddVert.ui32IndexCount.
							ffvThisVert.ui32NormalIndex = _favAddVert.ui32IndexCount;
						}
						else if ( plenNormals->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							// The normal index is found by plugging _favAddVert.ui32IndexCount into an array of indices.
							ffvThisVert.ui32NormalIndex = plenNormals->GetIndexArray().GetAt( _favAddVert.ui32IndexCount );
						}
						break;
					}
					default : {
						::printf( "Unsupported normal mapping mode.\r\n" );
					}
				}
			}

			if ( m_ui32Flags & LSX_COLORS ) {
				ffvThisVert.ui32ColorIndex = 0UL;
				const FbxLayerElementVertexColor * plevcColors = m_pfmMesh->GetLayer( 0 )->GetVertexColors();
				switch ( plevcColors->GetMappingMode() ) {
					case FbxGeometryElement::eByControlPoint : {
						if ( plevcColors->GetReferenceMode() == FbxLayerElement::eDirect ) {
							ffvThisVert.ui32ColorIndex = ui32VertIndex;
						}
						else if ( plevcColors->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							ffvThisVert.ui32ColorIndex = plevcColors->GetIndexArray().GetAt( ui32VertIndex );
						}
						if ( ffvThisVert.ui32ColorIndex >= static_cast<LSUINT32>(plevcColors->GetDirectArray().GetCount()) ) {
							::printf( "Invalid vertex color index.\r\n" );
						}
						break;
					}
					case FbxGeometryElement::eByPolygonVertex : {
						if ( plevcColors->GetReferenceMode() == FbxLayerElement::eDirect ) {
							ffvThisVert.ui32ColorIndex = _favAddVert.ui32IndexCount;
						}
						else if ( plevcColors->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							ffvThisVert.ui32ColorIndex = plevcColors->GetIndexArray().GetAt( _favAddVert.ui32IndexCount );
						}
						break;
					}
					default : {
						::printf( "Unsupported vertex color mapping mode.\r\n" );
					}
				}
			}

			LSUINT32 ui32TexIndex = 0UL;
			LSUINT32 ui32TotalLayers = m_pfmMesh->GetLayerCount();
			for ( LSUINT32 K = 0UL; K < ui32TotalLayers && ui32TexIndex < m_ui32TexCoords; ++K ) {
				const FbxLayerElementUV * pleuvUvElements = m_pfmMesh->GetLayer( K )->GetUVs( FbxLayerElement::eTextureDiffuse );
				if ( !pleuvUvElements ) { continue; }
			
				LSUINT32 ui32Uv = 0;
				switch ( pleuvUvElements->GetMappingMode() ) {
					case FbxGeometryElement::eByControlPoint : {
						if ( pleuvUvElements->GetReferenceMode() == FbxLayerElement::eDirect ) {
							ui32Uv = ui32VertIndex;
						}
						else if ( pleuvUvElements->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							ui32Uv = pleuvUvElements->GetIndexArray().GetAt( ui32VertIndex );
						}
						break;
					}
					case FbxGeometryElement::eByPolygonVertex : {
						if ( pleuvUvElements->GetReferenceMode() == FbxLayerElement::eDirect ) {
							ui32Uv = _favAddVert.ui32IndexCount;
						}
						else if ( pleuvUvElements->GetReferenceMode() == FbxLayerElement::eIndexToDirect ) {
							ui32Uv = pleuvUvElements->GetIndexArray().GetAt( _favAddVert.ui32IndexCount );
						}
						break;
					}
					default : {
						::printf( "Unsupported UV mapping mode.\r\n" );
					}
				}
				ffvThisVert.ui32UVIndex[ui32TexIndex++] = ui32Uv;
			}

			// Add the vertex to the pool.  Normally you should handle the exception
			//	this could throw, but it is removed for brevity here.
			if ( !_vVertices.Push( ffvThisVert ) ) { return false; }
			++_favAddVert.ui32IndexCount;
		}
		return true;
	}

	/**
	 * Adds a (non-intermediate) vertex to a vertex buffer.  These vertices are
	 *	in fully expanded LSX_RAW_VERTEX form.
	 *
	 * \param _ui32FaceIndex Index of the vertex to add.
	 * \param _vDest The the CVector to which to add the vertex.
	 * \param _vVertices The pool of intermediate vertices from which to read.
	 * \param _stSkinTable The skinning table.
	 * \return Returns true if the vertex was added.
	 */
	LSBOOL LSE_CALL CMesh::AddVertex( LSUINT32 _ui32VertIndex,
		LSX_VERTEX_BUFFER &_vDest,
		const CVector<LSX_FACE::LSX_FACE_VERTEX, LSUINT32> &_vVertices,
		const LSX_SKIN_TABLE &_stSkinTable ) const {
		// For clarity.
		const LSX_FACE::LSX_FACE_VERTEX & fvThisVert = _vVertices[_ui32VertIndex];
		// Initialize the vertex so that the position is filled and
		//	the rest are all 0's.
		LSX_RAW_VERTEX fvbBuffer = {
			LSX_VECTOR( static_cast<LSFLOAT>(m_pfmMesh->GetControlPoints()[fvThisVert.ui32VertexIndex][0]),
			static_cast<LSFLOAT>(m_pfmMesh->GetControlPoints()[fvThisVert.ui32VertexIndex][1]),
			static_cast<LSFLOAT>(m_pfmMesh->GetControlPoints()[fvThisVert.ui32VertexIndex][2]) )
		};

		if ( m_ui32Flags & LSX_NORMALS ) {
			const FbxLayerElementNormal * plenNormals = m_pfmMesh->GetLayer( 0 )->GetNormals();
			fvbBuffer.vNormal = LSX_VECTOR( static_cast<LSFLOAT>(plenNormals->GetDirectArray().GetAt( fvThisVert.ui32NormalIndex )[0]),
				static_cast<LSFLOAT>(plenNormals->GetDirectArray().GetAt( fvThisVert.ui32NormalIndex )[1]),
				static_cast<LSFLOAT>(plenNormals->GetDirectArray().GetAt( fvThisVert.ui32NormalIndex )[2]) );
		}

		if ( m_ui32Flags & LSX_COLORS ) {
			const FbxLayerElementVertexColor * plevcColors = m_pfmMesh->GetLayer( 0 )->GetVertexColors();
			fvbBuffer.vColor = LSX_VECTOR( static_cast<LSFLOAT>(plevcColors->GetDirectArray().GetAt( fvThisVert.ui32ColorIndex )[0]),
				static_cast<LSFLOAT>(plevcColors->GetDirectArray().GetAt( fvThisVert.ui32ColorIndex )[1]),
				static_cast<LSFLOAT>(plevcColors->GetDirectArray().GetAt( fvThisVert.ui32ColorIndex )[2]) );
		}

		LSUINT32 ui32TexIndex = 0UL;
		LSUINT32 ui32TotalLayers = m_pfmMesh->GetLayerCount();
		for ( LSUINT32 K = 0UL; K < ui32TotalLayers && ui32TexIndex < m_ui32TexCoords; ++K ) {
			const FbxLayerElementUV * pleuvUvElements = m_pfmMesh->GetLayer( K )->GetUVs( FbxLayerElement::eTextureDiffuse );
			if ( !pleuvUvElements ) { continue; }
			fvbBuffer.vUvs[ui32TexIndex] = FbxDouble3( pleuvUvElements->GetDirectArray().GetAt( fvThisVert.ui32UVIndex[ui32TexIndex] )[0],
				pleuvUvElements->GetDirectArray().GetAt( fvThisVert.ui32UVIndex[ui32TexIndex] )[1],
				pleuvUvElements->GetDirectArray().GetAt( fvThisVert.ui32UVIndex[ui32TexIndex] )[2] );
			++ui32TexIndex;
		}

		if ( m_ui32Flags & LSX_SKIN ) {
			// Gather all skin weights on entry fvThisVert.ui32VertexIndex of the table, max of
			//	LSX_MAX_SKIN_WEIGHTS.
			// Make a copy of that row of weights because the way we will find the top LSX_MAX_SKIN_WEIGHTS
			//	heaviest weights will involve setting each weight to 0 as we find them.
			LSFLOAT fWeights[LSX_MAX_INTERNAL_SKIN_WEIGHTS];
			CStd::MemCpy( fWeights, _stSkinTable.vWeights[fvThisVert.ui32VertexIndex].fWeights, sizeof( LSFLOAT ) * _stSkinTable.ui32Joints );
			LSFLOAT fTotalWeights = 0.0f;
			for ( LSUINT32 I = 0; I < LSX_MAX_SKIN_WEIGHTS; ++I ) {
				LSUINT32 ui32Heaviest = HighestWeight( fWeights, _stSkinTable.ui32Joints );
				if ( ui32Heaviest == LSE_MAXU32 ) { break; }
				// ui32Heaviest is the index into the list of weights for this vertex.
				fTotalWeights += fWeights[ui32Heaviest];
				fvbBuffer.fSkinWeights[I] = fWeights[ui32Heaviest];
				fvbBuffer.ui8SkinIndices[I] = static_cast<LSUINT8>(ui32Heaviest);
				// Make sure it doesn't get found on the next iteration.
				fWeights[ui32Heaviest] = 0.0f;
			}
			if ( HighestWeight( fWeights, _stSkinTable.ui32Joints ) != LSE_MAXU32 ) {
				CStd::DebugPrintA( "More than LSX_MAX_SKIN_WEIGHTS weights per vertex.\r\n" );
			}

			// Normalize the weights so they add up to 1.
			if ( fTotalWeights ) {
				LSFLOAT fNorm = 1.0f / fTotalWeights;
				for ( LSUINT32 I = 0; I < LSX_MAX_SKIN_WEIGHTS; ++I ) {
					fvbBuffer.fSkinWeights[I] *= fNorm;
				}
			}
		}

		// You should handle the possible exception here which
		//	has been omitted for brevity.
		return _vDest.Push( fvbBuffer );
	}

	/**
	 * Takes a raw triangle-list vertex buffer as input and outputs the reduced vertex
	 *	buffer and new index buffer.
	 *
	 * \param _vInputVerts Input vertex buffer.
	 * \param _vOutputVerts The output vertex buffer.
	 * \param _vIndices The output index buffer.
	 * \return Returns true if the index buffer was created successfully.
	 */
	LSBOOL LSE_CALL CMesh::CreateIndexBuffer( const LSX_VERTEX_BUFFER &_vInputVerts,
		LSX_VERTEX_BUFFER &_vOutputVerts,
		LSX_INDEX_BUFFER &_vIndices ) {
		// Keep track of which vertices have been added to the output vertex buffer and
		//	the index where they are in that buffer.
		//CMap<LSX_RAW_VERTEX, LSUINT32> mUsedVerts;
		std::map<LSX_RAW_VERTEX, LSUINT32> mUsedVerts;
		//mUsedVerts.Allocate( _vInputVerts.Length() );
		
		// For each vertex.
		for ( LSUINT32 I = 0; I < _vInputVerts.Length(); ++I ) {
			// Has it already been added?
			LSUINT32 ui32Index;
			std::map<LSX_RAW_VERTEX, LSUINT32>::const_iterator iIndex = mUsedVerts.find( _vInputVerts[I] );
			
			if ( iIndex == mUsedVerts.end() ) {
				// Has not yet been added.  Add it to the end of the
				//	list and store its index.
				ui32Index = static_cast<LSUINT32>(_vOutputVerts.Length());
				// You should handle the possible exceptions here,
				//	which have been unhandled here for brevity.
				if ( !_vOutputVerts.Push( _vInputVerts[I] ) ) { return false; }
				mUsedVerts[_vInputVerts[I]] = ui32Index;
				//if ( !mUsedVerts.Insert( _vInputVerts[I], ui32Index ) ) { return false; }
			}
			else {
				// It has been added.  The index that goes
				//	into the index buffer is the index of
				//	the already-added vertex.
				ui32Index = iIndex->second;
			}

			/*LSUINT32 ui32ExistingIndex;
			
			if ( !mUsedVerts.GetValue( _vInputVerts[I], ui32ExistingIndex ) ) {
				// Has not yet been added.  Add it to the end of the
				//	list and store its index.
				ui32Index = static_cast<LSUINT32>(_vOutputVerts.Length());
				// You should handle the possible exceptions here,
				//	which have been unhandled here for brevity.
				if ( !_vOutputVerts.Push( _vInputVerts[I] ) ) { return false; }
				//mUsedVerts[_vInputVerts[I]] = ui32Index;
				if ( !mUsedVerts.Insert( _vInputVerts[I], ui32Index ) ) { return false; }
			}
			else {
				// It has been added.  The index that goes
				//	into the index buffer is the index of
				//	the already-added vertex.
				ui32Index = ui32ExistingIndex;
			}*/

			/*if ( !mUsedVerts.ItemExists( _vInputVerts[I], ui32ExistingIndex ) ) {
				// Has not yet been added.  Add it to the end of the
				//	list and store its index.
				ui32Index = static_cast<LSUINT32>(_vOutputVerts.Length());
				// You should handle the possible exceptions here,
				//	which have been unhandled here for brevity.
				if ( !_vOutputVerts.Push( _vInputVerts[I] ) ) { return false; }
				//mUsedVerts[_vInputVerts[I]] = ui32Index;
				if ( !mUsedVerts.InsertUnsafe( _vInputVerts[I], ui32Index, ui32ExistingIndex ) ) { return false; }
			}
			else {
				// It has been added.  The index that goes
				//	into the index buffer is the index of
				//	the already-added vertex.
				ui32Index = mUsedVerts.GetByIndex( ui32ExistingIndex );
			}*/
			// Add the index to the index buffer, handling the possible
			//	exception in your code.
			if ( !_vIndices.Push( ui32Index ) ) { return false; }
		}
		return true;
	}

	/**
	 * Gets the index of the index-buffer value with the lowest value.
	 *
	 * \param _vIndices Index buffer.
	 * \param _ibrRange Range to consider inside the index buffer.
	 * \return Returns the index of the index-buffer value with the lowest value.
	 */
	LSUINT32 LSE_CALL CMesh::IndexBufferLowestIndex( const LSX_INDEX_BUFFER &_vIndices,
		const LSX_INDEX_BUFFER_RANGE &_ibrRange ) {
		LSUINT32 ui32Low = LSE_MAXU32;
		LSUINT32 ui32End = _ibrRange.ui32Start + _ibrRange.ui32Total;
		for ( LSUINT32 I = _ibrRange.ui32Start; I < ui32End; ++I ) {
			ui32Low = CStd::Min( ui32Low, _vIndices[I] );
		}
		return ui32Low;
	}

	/**
	 * Generates a table of per-vertex weights, each associated with an FBX cluster identified by its "ID" (node pointer
	 *	represented in integer form).  Only the first skin, if any, is used.
	 *
	 * \param _pfmMesh The mesh from which to generate the skinning data.
	 * \param _stTable The returned table of skinning information.
	 * \return Returns true if all allocations succeed.
	 */
	LSBOOL LSE_CALL CMesh::MakeSkinTable( const FbxMesh * _pfmMesh,
		LSX_SKIN_TABLE &_stTable ) {
		const FbxGeometry * pfgGeometry = _pfmMesh;
		_stTable.vWeights.Reset();
		CStd::MemSet( _stTable.ui64Ids, 0, sizeof( _stTable.ui64Ids ) );
		_stTable.ui32Joints = 0;

		int iSkins = pfgGeometry->GetDeformerCount( FbxDeformer::eSkin );
		// If there are no skins, leave the table blank and return true.
		if ( !iSkins ) { return true; }

		int iClusters = static_cast<FbxSkin *>(pfgGeometry->GetDeformer( 0, FbxDeformer::eSkin ))->GetClusterCount();
		// If there are no clusters, leave the table blank and return true.
		if ( !iClusters ) { return true; }

		// Potentially warn if there are more clusters than we are designed to process.
		//	This happens when more than LSX_MAX_INTERNAL_SKIN_WEIGHTS bones are influencing
		//	a single mesh.
		iClusters = CStd::Min<int>( iClusters, LSX_MAX_INTERNAL_SKIN_WEIGHTS );

		// We have a skin with 1 or more clusters.  Perform allocation.  This tells the
		//	caller that there is skinning data.
		if ( !_stTable.vWeights.Resize( _pfmMesh->GetControlPointsCount() ) ) { return false; }
		// Each cluster should have an array of weights associated with the control-point (vertex).
		//	Go over each cluster and put the weights into the proper slots in the skinning table.
		// Each cluster represents 1 bone/joint.
		LSUINT32 ui32EntryIndex = 0;
		for ( int I = 0; I < iClusters; ++I ) {
			FbxCluster * pfcCluster = static_cast<FbxSkin *>(pfgGeometry->GetDeformer( 0, FbxDeformer::eSkin ))->GetCluster( I );
			// If the node is not a join, skip it.
			if ( !pfcCluster->GetLink() || !lsx::CMisc::NodeIsType( pfcCluster->GetLink(), FbxNodeAttribute::eSkeleton ) ) { continue; }

			// Associate the cluster with its source (a bone/joint).
			_stTable.ui64Ids[ui32EntryIndex] = reinterpret_cast<LSUINT64>(pfcCluster->GetLink());
			int iTotalInfluences = pfcCluster->GetControlPointIndicesCount();
            int * piIndices = pfcCluster->GetControlPointIndices();
            double * pdWeights = pfcCluster->GetControlPointWeights();
			for ( int J = 0; J < iTotalInfluences; ++J ) {
				if ( static_cast<LSUINT32>(piIndices[J]) >= _stTable.vWeights.Length() ) { return false; }
				_stTable.vWeights[piIndices[J]].fWeights[ui32EntryIndex] = static_cast<LSFLOAT>(pdWeights[J]);
			}

			++ui32EntryIndex;
		}
		_stTable.ui32Joints = ui32EntryIndex;
		return true;
	}

}	// namespace lsx
