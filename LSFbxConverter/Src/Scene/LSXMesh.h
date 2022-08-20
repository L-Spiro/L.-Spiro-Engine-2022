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


#ifndef __LSX_MESH_H__
#define __LSX_MESH_H__

#include "../LSXFbxConverter.h"
#include "Map/LSTLMap.h"
#include "Models/LSDModelAttributes.h"
#include "Set/LSTLSet.h"

namespace lsx {

	/**
	 * Class CMesh
	 * \brief Loads mesh data.
	 *
	 * Description: Loads mesh data.
	 */
	class CMesh {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMesh();
		LSE_CALLCTOR							~CMesh();


		// == Enumerations.
		enum {
			LSX_MAX_UVS							= 1,						/**< Maximum UV coordinates. */
			LSX_MAX_INTERNAL_SKIN_WEIGHTS		= 256,						/**< Maximum skin weights internally. */
			LSX_MAX_SKIN_WEIGHTS				= LSD_MA_MAX_SKINS,			/**< Maximum skin weights exported. */
			LSX_NORMALS							= LSD_MA_NORMALS,			/**< Normal data is included. */
			LSX_COLORS							= LSD_MA_COLORS,			/**< Color data is included. */
			LSX_SKIN							= LSD_MA_SKIN,				/**< Skin data is included. */
			LSX_TANGENT							= LSD_MA_TANGENT,			/**< Tangent data is included. */
			LSX_BITANGENT						= LSD_MA_BITANGENT,			/**< Bitangent data is included. */
		};

		// == Types.
		/** A raw fully expanded vertex buffer. */
		typedef struct LSX_RAW_VERTEX {
			LSX_VECTOR							vPosition;					/**< Vertex position. */
			LSX_VECTOR							vColor;						/**< Vertex color. */
			LSX_VECTOR							vNormal;					/**< Vertex normal. */
			FbxDouble2							vUvs[LSX_MAX_UVS];			/**< Texture coordinates. */
			/** Skin indices into matrices. */
			LSUINT8								ui8SkinIndices[LSX_MAX_SKIN_WEIGHTS];
			/** Skin weights. */
			LSFLOAT								fSkinWeights[LSX_MAX_SKIN_WEIGHTS];
			LSX_VECTOR							vTangent;					/**< Tangent for normal mapping. */
			LSX_VECTOR							vBiTangent;					/**< Bitangent for normal mapping. */


			// == Operators.
			/** Less-than operator. */
			bool LSE_FCALL						operator < ( const LSX_RAW_VERTEX &_vbOther ) const {
				return CStd::MemCmp( this, &_vbOther, sizeof( LSX_RAW_VERTEX ) ) < 0;
			}

			/** Equality operator. */
			bool LSE_FCALL						operator == ( const LSX_RAW_VERTEX &_vbOther ) const {
				return CStd::MemCmp( this, &_vbOther, sizeof( LSX_RAW_VERTEX ) ) == 0;
			}

		} * LPLSX_VERTEX_BUFFER, * const LPCLSX_RAW_VERTEX;

		/** A set of materials which acts as a key to a map. */
		typedef struct LSX_MATERIAL_KEY {
			const LSCHAR *						pcMaterial[LSX_MAX_UVS];	/**< A set of materials. */
			FbxSurfaceMaterial *				psmMaterialPtr[LSX_MAX_UVS];/**< Pointer to the actual material. */


			// == Operators.
			LSBOOL LSE_FCALL					operator < ( const LSX_MATERIAL_KEY &_fmkOther ) const {
				// Inlined for speed.
				for ( LSUINT32 I = 0UL; I < LSX_MAX_UVS; ++I ) {
					// If we are NULL and they are not, we are below them.
					if ( !pcMaterial[I] && _fmkOther.pcMaterial[I] ) { return true; }
					// If they are NULL and we are not, we are above them.
					if ( pcMaterial[I] && !_fmkOther.pcMaterial[I] ) { return false; }
					// If we are both non-NULL, use a string compare.
					if ( pcMaterial[I] && _fmkOther.pcMaterial[I] ) {
						int iCmp = CStd::StrCmp( pcMaterial[I], _fmkOther.pcMaterial[I] );
						if ( iCmp < 0 ) { return true; }
						if ( iCmp > 0 ) { return false; }
						// Strings are the same.  Continue.
					}
					// This case means we are both NULL or are the same string.  Continue with the loop.
				}
				// We are the same all the way.
				return false;
			}

			LSBOOL LSE_FCALL					operator == ( const LSX_MATERIAL_KEY &_fmkOther ) const {
				// Inlined for speed.
				for ( LSUINT32 I = 0UL; I < LSX_MAX_UVS; ++I ) {
					// Just check for anything being different.
					if ( !pcMaterial[I] && _fmkOther.pcMaterial[I] ) { return false; }
					if ( pcMaterial[I] && !_fmkOther.pcMaterial[I] ) { return false; }
					if ( pcMaterial[I] && _fmkOther.pcMaterial[I] ) {
						if ( CStd::StrCmp( pcMaterial[I], _fmkOther.pcMaterial[I] ) != 0 ) { return false; }
					}
				}
				// Found no differences.
				return true;
			}
		} * LPLSX_MATERIAL_KEY, * const LPCLSX_MATERIAL_KEY;

		/** An index buffer. */
		typedef CVector<LSUINT32, LSUINT32>		LSX_INDEX_BUFFER;

		/** A vertex buffer. */
		typedef CVector<LSX_RAW_VERTEX, LSUINT32>
												LSX_VERTEX_BUFFER;


		// == Operators.
		/**
		 * Equality operator.
		 *
		 * \param _mOther Mesh to which to check for euqlity.
		 * \return Returns true if the meshes have all the same vertex and index data.
		 */
		LSBOOL LSE_CALL							operator == ( const CMesh &_mOther ) const;


		// == Functions.
		/**
		 * Loads a mesh.
		 *
		 * \param _pfmMesh The mesh node to load.
		 * \param _sMaterials Materials referenced by this object.
		 * \return Returns true if the mesh was loaded.
		 */
		LSBOOL LSE_CALL							LoadMesh( FbxMesh * _pfmMesh, CSet<FbxSurfaceMaterial *> &_sMaterials );

		/**
		 * Gets the total number of triangles in the mesh.
		 *
		 * \return Returns the number of triangles in the mesh.
		 */
		LSE_INLINE LSUINT32 LSE_CALL			Triangles() const;

		/**
		 * Gets the array of vertices.
		 *
		 * \return Returns a const reference to the array of vertices.
		 */
		LSE_INLINE const LSX_VERTEX_BUFFER & LSE_CALL
												Vertices() const;

		/**
		 * Save the mesh to a binary buffer.
		 *
		 * \param _bBuffer The buffer where the data should be written.
		 * \param _ui32Index The index of this mesh.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							SaveMesh( lsc::CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32Index );

		/**
		 * Set our unique index.
		 *
		 * \param _ui32Index The index to set on this object.
		 */
		LSE_INLINE LSVOID LSE_CALL				SetIndex( LSUINT32 _ui32Index );

		/**
		 * Get our index.
		 *
		 * \return Returns our index.
		 */
		LSE_INLINE LSUINT32 LSE_CALL			GetIndex() const;


	protected :
		// == Members.
		/** Our own intermediate representation of a face/polygon. */
		typedef struct LSX_FACE {
			/** A nested structure representing one vertex on a face. */
			typedef struct LSX_FACE_VERTEX {
				LSUINT32						ui32VertexIndex;			/**< Vertex index. */
				LSUINT32						ui32NormalIndex;			/**< Normal index. */
				LSUINT32						ui32ColorIndex;				/**< Color index. */
				LSUINT32						ui32UVIndex[LSX_MAX_UVS];	/**< UV index. */
			} * LPLSX_FACE_VERTEX;

			/** In order to save memory and time, a single large pool
				of LSX_FACE_VERTEX will be maintained.  All faces will simply
				reference parts of it, so all we need to store on this
				structure is the starting index and the total vertices. */
			LSUINT32							ui32StartVertex;
			LSUINT32							ui32TotalVertices;
		} * LPLSX_FACE;

		/** Pairing our intermediate faces with the vertex buffers they will
		 *	later be used to fill. */
		typedef struct LSX_FACE_VERTEX_PAIR {
			CVector<LSX_FACE, LSUINT32>			vFaces;						/**< The faces in this bucket. */
			LSX_VERTEX_BUFFER					vVertices;					/**< The vertices created by the faces. */
		} * LPLSX_FACE_VERTEX_PAIR;

		/** Pairing a vertex buffer with an index buffer. */
		typedef struct LSX_INDEX_VERTEX_PAIR {
			LSX_INDEX_BUFFER					vIndices;					/**< The indices. */
			LSX_VERTEX_BUFFER					vVertices;					/**< The vertices. */
		} * LPLSX_INDEX_VERTEX_PAIR;

		/** Index buffer ranges associated with materials. */
		typedef struct LSX_INDEX_BUFFER_RANGE {
			LSUINT32							ui32Start;					/**< The start index. */
			LSUINT32							ui32Total;					/**< Total indices. */
			LSX_MATERIAL_KEY					mkMaterial;					/**< The material key to which this range is associated. */
		} * LPLSX_INDEX_BUFFER_RANGE;

		/** An array of ranges. */
		typedef CVector<LSX_INDEX_BUFFER_RANGE>	LSX_INDEX_BUFFER_RANGE_ARRAY;

		/** Data to pass to AddFace(). */
		typedef struct LSX_ADD_VERTEX {
			LSUINT32							ui32IndexCount;				/**< Index count (in/out). */
		} * LPLSX_ADD_VERTEX;

		/** A single mesh part. */
		typedef struct LSX_MESH_PART {
			LSX_INDEX_BUFFER					ibIndexBuffer;				/**< Index buffer. */
			LSUINT32							ui32Base;					/**< Base of the vertex buffer. */
			LSD_MODEL_ATTRIBUTES				maPrimType;					/**< Triangle strips/lists. */
			LSUINT32							ui32IndexSize;				/**< 2 or 4 bytes per index. */
			LSX_MATERIAL_KEY					mkMaterial;					/**< The material key to which this index buffer is associated. */
		} * LPLSX_MESH_PART, * const LPCLSX_MESH_PART;

		/** An array of mesh parts. */
		typedef CVector<LSX_MESH_PART, LSUINT32>LSX_MESH_PART_ARRAY;

		/** A skinning entry. */
		typedef struct LSX_SKIN_ENTRY {
			/** Skin weights for this entry. */
			LSFLOAT								fWeights[LSX_MAX_INTERNAL_SKIN_WEIGHTS];
		} * LPLSX_SKIN_ENTRY, * const LPCLSX_SKIN_ENTRY;

		/** A skinning table. */
		typedef struct LSX_SKIN_TABLE {
			CVector<LSX_SKIN_ENTRY, LSUINT32>	vWeights;					/**< Array of weights, one for each control point. */
			/** One ID for each set of weights. */
			LSUINT64							ui64Ids[LSX_MAX_INTERNAL_SKIN_WEIGHTS];
			/** Number of joints. */
			LSUINT32							ui32Joints;
		} * LPLSX_SKIN_TABLE, * const LPCLSX_SKIN_TABLE;


		// == Members.
		FbxMesh *								m_pfmMesh;					/**< The source mesh. */
		LSUINT32								m_ui32Flags;				/**< Which data is included in the vertex buffer? */
		LSUINT32								m_ui32TexCoords;			/**< How many texture coordinates are there? */
		LSX_VERTEX_BUFFER						m_vbVertexBuffer;			/**< The vertex buffer. */
		LSX_MESH_PART_ARRAY						m_mpaIndexBuffers;			/**< All the necessary index buffers for this mesh. */
		LSUINT32								m_ui32Triangles;			/**< Total triangles. */
		LSUINT32								m_ui32Index;				/**< Our index. */


		// == Functions.
		/**
		 * Adds a vertex to a vertex pool and adds an intermediate face to the proper bucket.
		 *
		 * \param _u32FaceIndex Index of the face to add.
		 * \param _mBuckets The buckets to which to add the face.
		 * \param _vVertices The pool to which to add the vertices.
		 * \param _fmkKey The key for _mBuffers to know to which vertex buffer to add the vertex.
		 * \param _favAddVert In/out parameters for adding the vertex.
		 * \return Returns true if the vertex was added.
		 */
		LSBOOL LSE_CALL							AddFace( LSUINT32 _u32FaceIndex,
			CLinearMap<LSX_MATERIAL_KEY, LSX_FACE_VERTEX_PAIR> &_mBuckets,
			CVector<LSX_FACE::LSX_FACE_VERTEX, LSUINT32> &_vVertices,
			const LSX_MATERIAL_KEY &_fmkKey,
			LSX_ADD_VERTEX &_favAddVert ) const;

		/**
		 * Adds a (non-intermediate) vertex to a vertex buffer.  These vertices are
		 *	in fully expanded LSX_RAW_VERTEX form.
		 *
		 * \param _u32FaceIndex Index of the vertex to add.
		 * \param _vDest The the CVector to which to add the vertex.
		 * \param _vVertices The pool of intermediate vertices from which to read.
		 * \param _stSkinTable The skinning table.
		 * \return Returns true if the vertex was added.
		 */
		LSBOOL LSE_CALL							AddVertex( LSUINT32 _u32VertIndex,
			LSX_VERTEX_BUFFER &_vDest,
			const CVector<LSX_FACE::LSX_FACE_VERTEX, LSUINT32> &_vVertices,
			const LSX_SKIN_TABLE &_stSkinTable ) const;

		/**
		 * Takes a raw triangle-list vertex buffer as input and outputs the reduced vertex
		 *	buffer and new index buffer.
		 *
		 * \param _vInputVerts Input vertex buffer.
		 * \param _vOutputVerts The output vertex buffer.
		 * \param _vIndices The output index buffer.
		 * \return Returns true if the index buffer was created successfully.
		 */
		static LSBOOL LSE_CALL					CreateIndexBuffer( const LSX_VERTEX_BUFFER &_vInputVerts,
			LSX_VERTEX_BUFFER &_vOutputVerts,
			LSX_INDEX_BUFFER &_vIndices );

		/**
		 * Gets the index of the index-buffer value with the lowest value.
		 *
		 * \param _vIndices Index buffer.
		 * \param _ibrRange Range to consider inside the index buffer.
		 * \return Returns the index of the index-buffer value with the lowest value.
		 */
		static LSUINT32 LSE_CALL				IndexBufferLowestIndex( const LSX_INDEX_BUFFER &_vIndices,
			const LSX_INDEX_BUFFER_RANGE &_ibrRange );

		/**
		 * Determines if all 3 indices of the triangle (by triangle index) given are in the range from 0x0000 to 0xFFFF, inclusive.
		 *	The purpose is to determine if the triangle can be made with the 3 indices while the indices themselves remain 16-bit.
		 *
		 * \param _ui32Base Base offset subtracted from the indices in the index buffer.
		 * \param _vIndices The index buffer.
		 * \param _ui32TriIndex Index of the triangle to check.
		 * \return Returns true if the 3 indices that compose a triangle are, after having the base value subtracted from them,
		 *	all within 16-bit range.
		 */
		static LSE_INLINE LSBOOL LSE_CALL		TriIs16Bit( LSUINT32 _ui32Base,
			const LSX_INDEX_BUFFER &_vIndices,
			LSUINT32 _ui32TriIndex );

		/**
		 * Generates a table of per-vertex weights, each associated with an FBX cluster identified by its "ID" (node pointer
		 *	represented in integer form).  Only the first skin, if any, is used.
		 *
		 * \param _pfmMesh The mesh from which to generate the skinning data.
		 * \param _stTable The returned table of skinning information.
		 * \return Returns true if all allocations succeed.
		 */
		static LSBOOL LSE_CALL					MakeSkinTable( const FbxMesh * _pfmMesh,
			LSX_SKIN_TABLE &_stTable );

		/**
		 * Gets the index of the heighest non-zero weight in the given array of weights, up to LSX_MAX_INTERNAL_SKIN_WEIGHTS
		 *	long.
		 *
		 * \param _fWeights The array of weights, up to LSX_MAX_INTERNAL_SKIN_WEIGHTS in length.
		 * \param _ui32Total Total weights in the array.
		 * \return Returns the index of the highest non-zero weight in the given array or LSE_MAXU32 if there are none.
		 */
		static LSE_INLINE LSUINT32 LSE_CALL		HighestWeight( const LSFLOAT _fWeights[LSX_MAX_INTERNAL_SKIN_WEIGHTS],
			LSUINT32 _ui32Total );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total number of triangles in the mesh.
	 *
	 * \return Returns the number of triangles in the mesh.
	 */
	 LSE_INLINE LSUINT32 LSE_CALL CMesh::Triangles() const {
		 return m_ui32Triangles;
	 }

	 /**
	 * Gets the array of vertices.
	 *
	 * \return Returns a const reference to the array of vertices.
	 */
	LSE_INLINE const CMesh::LSX_VERTEX_BUFFER & LSE_CALL CMesh::Vertices() const {
		return m_vbVertexBuffer;
	}

	/**
	 * Set our unique index.
	 *
	 * \param _ui32Index The index to set on this object.
	 */
	LSE_INLINE LSVOID LSE_CALL CMesh::SetIndex( LSUINT32 _ui32Index ) {
		m_ui32Index = _ui32Index;
	}

	/**
	 * Get our index.
	 *
	 * \return Returns our index.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CMesh::GetIndex() const {
		return m_ui32Index;
	}

	 /**
	 * Determines if all 3 indices of the triangle (by triangle index) given are in the range from 0x0000 to 0xFFFF, inclusive.
	 *	The purpose is to determine if the triangle can be made with the 3 indices while the indices themselves remain 16-bit.
	 *
	 * \param _ui32Base Base offset subtracted from the indices in the index buffer.
	 * \param _vIndices The index buffer.
	 * \param _ui32TriIndex Index of the triangle to check.
	 * \return Returns true if the 3 indices that compose a triangle are, after having the base value subtracted from them,
	 *	all within 16-bit range.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMesh::TriIs16Bit( LSUINT32 _ui32Base,
		const LSX_INDEX_BUFFER &_vIndices,
		LSUINT32 _ui32TriIndex ) {
		if ( _vIndices.Length() / 3 <= _ui32TriIndex ) { return false; }
		_ui32TriIndex *= 3;
		for ( LSUINT32 I = 0; I < 3; ++I ) {
			LSUINT32 ui32ThisIndex = _vIndices[_ui32TriIndex+I] - _ui32Base;
			if ( ui32ThisIndex > 0xFFFF ) { return false; }
		}
		return true;
	}

	/**
	 * Gets the index of the heighest non-zero weight in the given array of weights, up to LSX_MAX_INTERNAL_SKIN_WEIGHTS
	 *	long.
	 *
	 * \param _fWeights The array of weights, up to LSX_MAX_INTERNAL_SKIN_WEIGHTS in length.
	 * \param _ui32Total Total weights in the array.
	 * \return Returns the index of the highest non-zero weight in the given array or LSE_MAXU32 if there are none.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CMesh::HighestWeight( const LSFLOAT _fWeights[LSX_MAX_INTERNAL_SKIN_WEIGHTS],
		LSUINT32 _ui32Total ) {
		LSFLOAT fHigh = 0.0f;
		LSUINT32 ui32Index = LSE_MAXU32;
		for ( LSUINT32 I = _ui32Total; I--; ) {
			if ( _fWeights[I] != 0.0f && _fWeights[I] > fHigh ) {
				fHigh = _fWeights[I];
				ui32Index = I;
			}
		}
		return ui32Index;
	}

}	// namespace lsx

#endif	// __LSX_MESH_H__
