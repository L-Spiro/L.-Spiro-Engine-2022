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
 * Description: A shared model.  Model instances reference this for vertex and index buffers, etc.
 */


#ifndef __LSD_SHAREDMODEL_H__
#define __LSD_SHAREDMODEL_H__

#include "../LSDModelLib.h"
#include "../Shaders/LSDShaderManager.h"
#include "../Textures/LSDModelTextureManager.h"
#include "../Textures/LSDSharedTexture2d.h"
#include "Atomic/LSSTDAtomic.h"
#include "LSDMesh.h"
#include "LSDMeshHierarchy.h"
#include "LSDMeshInstance.h"
#include "LSDModelAttributes.h"
#include "LSDModelMaterial.h"
#include "Set/LSTLSet.h"
#include "String/LSTLString.h"
#include "Vector/LSTLVector.h"

namespace lsd {

	class CModelManager;

	/**
	 * Class CSharedModel
	 * \brief A shared model.
	 *
	 * Description: A shared model.  Model instances reference this for vertex and index buffers, etc.
	 */
	class CSharedModel {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSharedModel();
		LSE_CALLCTOR							~CSharedModel();


		// == Functions.
		/**
		 * Loads model data from a shared file pointer.
		 *
		 * \param _sfpFile Shared pointer to an in-memory file from which to load the model.
		 * \param _uiptrLoadEnd upon return, holds an offset within the file data at which a successful load ended.
		 * \return Returns an error code indicating the result of the load.
		 */
		LSSTD_ERRORS LSE_CALL					LoadFromMemory( CSharedFilePtr _sfpFile, uintptr_t &_uiptrLoadEnd );

		/**
		 * Resets everything back to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets a constant reference to the array of meshes.
		 *
		 * \return Returns a constant reference to the array of meshes.
		 */
		LSE_INLINE const CVector<CMesh> & LSE_CALL
												Meshes() const;

		/**
		 * Gets a reference to the array of meshes.
		 *
		 * \return Returns a reference to the array of meshes.
		 */
		LSE_INLINE CVector<CMesh> & LSE_CALL	Meshes();

		/**
		 * Gets a reference to the array of mesh instances.
		 *
		 * \return Returns a reference to the array of mesh instances.
		 */
		LSE_INLINE CVector<CMeshInstance> & LSE_CALL
												MeshInstances();

		/**
		 * Gets the total number of sub-meshes in the model.
		 *
		 * \return Returns the total number of sub-meshes in the model.
		 */
		uint32_t LSE_CALL						TotalSubMeshes() const;

		/**
		 * Gets a mesh instance by its ID.  Returns NULL if no such mesh instance exists.
		 *
		 * \param _ui64Id ID of the mesh to get.
		 * \return Returns a pointer to the mesh instance with the given ID or NULL if no mesh instance
		 *	with the given ID exists.
		 */
		LSE_INLINE const CMeshInstance * LSE_CALL
												FindMeshById( uint64_t _ui64Id ) const;

		/**
		 * Gets a constant pointer to the first hierarchical structure in the model.
		 *
		 * \return Returns a constant pointer to the first hierarchical structure in the model.
		 */
		LSE_INLINE const LSD_MESH_HIERARCHY * LSE_CALL
												Hierarchy() const;

		/**
		 * Gets the total hierarchy structures in the model.
		 *
		 * \return Returns the total hierarchy structures in the model.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalHierarchyObjects() const;

		/**
		 * Gets the object count of the given object type within the hierarchy.
		 *
		 * \param _maType The object type to count.
		 * \return Returns the total for the given object type.
		 */
		LSE_INLINE uint32_t LSE_CALL			CountObject( LSD_MODEL_ATTRIBUTES _maType );

		/**
		 * Gets a constant pointer to a hierarchy entry for the given ID.
		 *
		 * \param _ui64Id The ID of the hierarchy entry to find.
		 * \return Returns a pointer to the hierarchy entry to find or NULL if it does not exist.
		 */
		const LSD_MESH_HIERARCHY * LSE_CALL		FindHierarchyById( uint64_t _ui64Id ) const;

		/**
		 * Creates a standard shader key given a mesh and a material.
		 *
		 * \param _skKey The key to create based off the inputs in the mesh and the data in the material.
		 * \param _mMesh A constant reference to the mesh whose input data will be used to create the shader key.
		 * \param _mmMaterial A constant reference to the material whose texture data will be used to create the shader key.
		 * \return Returns _skKey.
		 */
		static LSD_SHADERKEY & LSE_FCALL		CreateStandardShaderKey( LSD_SHADERKEY &_skKey, const CMesh &_mMesh, const CModelMaterial &_mmMaterial );

		/**
		 * Creates a standard shader key given a mesh and a material, excluding the input data.
		 *
		 * \param _skKey The key to create based off the inputs in the mesh and the data in the material.
		 * \param _mMesh A constant reference to the mesh whose input data will be used to create the shader key.
		 * \param _mmMaterial A constant reference to the material whose texture data will be used to create the shader key.
		 * \return Returns _skKey.
		 */
		static LSE_INLINE LSD_SHADERKEY & LSE_FCALL
												CreateStandardShaderKeyQuick( LSD_SHADERKEY &_skKey, const CMesh &_mMesh, const CModelMaterial &_mmMaterial );

		/**
		 * Fills only the flags of a shader key.
		 *
		 * \param _skKey The key whose flags are to be filled.
		 * \param _mmMaterial A constant reference to the material whose texture data will be used to fill the shader key's flags.
		 * \return Returns _skKey.
		 */
		static LSE_INLINE LSD_SHADERKEY & LSE_FCALL
												SetShaderKeyFlags( LSD_SHADERKEY &_skKey, const CModelMaterial &_mmMaterial );

		/**
		 * Fills only the inputs of a shader key.
		 *
		 * \param _skKey The key whose inputs are to be filled.
		 * \param _mMesh A constant reference to the mesh that will be used to fill the shader key's inputs.
		 * \return Returns _skKey.
		 */
		static LSE_INLINE LSD_SHADERKEY & LSE_FCALL
												SetShaderKeyInputs( LSD_SHADERKEY &_skKey, const CMesh &_mMesh );

		/**
		 * Fills only the texture data of a shader key.
		 *
		 * \param _skKey The key whose texture data is to be filled.
		 * \param _mmMaterial A constant reference to the material that will be used to fill the shader key's texture data.
		 * \param _ui8TotalUvs Total UV's.
		 * \return Returns _skKey.
		 */
		static LSE_INLINE LSD_SHADERKEY & LSE_FCALL
												SetShaderKeyTextures( LSD_SHADERKEY &_skKey, const CModelMaterial &_mmMaterial, uint8_t _ui8TotalUvs );

		/**
		 * Fills only the render states of a shader key.
		 *
		 * \param _skKey The key whose render states are to be filled.
		 * \return Returns _skKey.
		 */
		static LSE_INLINE LSD_SHADERKEY & LSE_FCALL
												SetShaderKeyRenderState( LSD_SHADERKEY &_skKey );

		/**
		 * Creates a shared file pointer given a file path.
		 *
		 * \param _pcPath UTF-8 path to the file to load and store in a CSharedFilePtr object.
		 * \return Returns the created CSharedFilePtr object, which will be invalid on failure.
		 */
		static CSharedFilePtr LSE_CALL			SharedFilePtr( const char * _pcPath );

		/**
		 * Reads a string from an external byte stream.
		 *
		 * \param _ebsStream The byte stream from which to read the string.
		 * \param _sString The string read from the byte stream.
		 * \return Returns true if the string was fully read from the stream.
		 */
		static LSE_INLINE LSBOOL LSE_CALL		ReadString( const CExternalByteStream &_ebsStream, CString &_sString );

	protected :
		// == Members.
		/** The in-memory file from which to load the shared model.  Must have been allocated with LSENEW []. */
		CSharedFilePtr							m_sfpFile;

		/** Array of meshes. */
		CVector<CMesh>							m_vMeshes;

		/** Array of meshe instances. */
		CVector<CMeshInstance>					m_vMeshInstances;

		/** Array of materials. */
		CVector<CModelMaterial, uint32_t>		m_vMaterials;

		/** Set of textures. */
		LSD_SHARED_TEXTURE2D_SET				m_stsTextures;

		/** The hierarchy. */
		CVectorPoD<LSD_MESH_HIERARCHY, uint32_t>m_vHierarchy;

		/** Shared model reference counter. Counts how many shared models exist. */
		static CAtomic::ATOM					m_aSharedModelCount;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** A shared model shared pointer. */
	typedef CSharedPtr<CSharedModel>			CSharedSharedModelPtr;

	// == Functions.
	/**
	 * Gets a constant reference to the array of meshes.
	 *
	 * \return Returns a constant reference to the array of meshes.
	 */
	LSE_INLINE const CVector<CMesh> & LSE_CALL CSharedModel::Meshes() const { return m_vMeshes; }

	/**
	 * Gets a reference to the array of meshes.
	 *
	 * \return Returns a reference to the array of meshes.
	 */
	LSE_INLINE CVector<CMesh> & LSE_CALL CSharedModel::Meshes() { return m_vMeshes; }

	 /**
	 * Gets a reference to the array of mesh instances.
	 *
	 * \return Returns a reference to the array of mesh instances.
	 */
	LSE_INLINE CVector<CMeshInstance> & LSE_CALL CSharedModel::MeshInstances() { return m_vMeshInstances; }

	/**
	 * Gets a mesh instance by its ID.  Returns NULL if no such mesh instance exists.
	 *
	 * \param _ui64Id ID of the mesh to get.
	 * \return Returns a pointer to the mesh instance with the given ID or NULL if no mesh instance
	 *	with the given ID exists.
	 */
	LSE_INLINE const CMeshInstance * LSE_CALL CSharedModel::FindMeshById( uint64_t _ui64Id ) const {
		for ( uint32_t I = m_vMeshInstances.Length(); I--; ) {
			if ( m_vMeshInstances[I].Data().ui64Id == _ui64Id ) { return &m_vMeshInstances[I]; }
		}
		return NULL;
	}

	/**
	 * Gets a constant pointer to the first hierarchical structure in the model.
	 *
	 * \return Returns a constant pointer to the first hierarchical structure in the model.
	 */
	LSE_INLINE const LSD_MESH_HIERARCHY * LSE_CALL CSharedModel::Hierarchy() const { return &m_vHierarchy[0]; }

	/**
	 * Gets the total hierarchy structures in the model.
	 *
	 * \return Returns the total hierarchy structures in the model.
	 */
	LSE_INLINE uint32_t LSE_CALL CSharedModel::TotalHierarchyObjects() const { return m_vHierarchy.Length(); }

	/**
	 * Gets the object count of the given object type within the hierarchy.
	 *
	 * \param _maType The object type to count.
	 * \return Returns the total for the given object type.
	 */
	LSE_INLINE uint32_t LSE_CALL CSharedModel::CountObject( LSD_MODEL_ATTRIBUTES _maType ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = TotalHierarchyObjects(); I--; ) {
			if ( Hierarchy()[I].ui16Type == _maType ) { ++ui32Total; }
		}
		return ui32Total;
	}

	/**
	 * Creates a standard shader key given a mesh and a material, excluding the input data.
	 *
	 * \param _skKey The key to create based off the inputs in the mesh and the data in the material.
	 * \param _mMesh A constant reference to the mesh whose input data will be used to create the shader key.
	 * \param _mmMaterial A constant reference to the material whose texture data will be used to create the shader key.
	 * \return Returns _skKey.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL CSharedModel::CreateStandardShaderKeyQuick( LSD_SHADERKEY &_skKey, const CMesh &_mMesh, const CModelMaterial &_mmMaterial ) {
		SetShaderKeyFlags( _skKey, _mmMaterial );
		SetShaderKeyTextures( _skKey, _mmMaterial, static_cast<uint8_t>(_mMesh.UvSets()) );
		SetShaderKeyRenderState( _skKey );
		return _skKey;
	}

	/**
	 * Fills only the flags of a shader key.
	 *
	 * \param _skKey The key whose flags are to be filled.
	 * \param _mmMaterial A constant reference to the material whose texture data will be used to fill the shader key's flags.
	 * \return Returns _skKey.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL CSharedModel::SetShaderKeyFlags( LSD_SHADERKEY &_skKey, const CModelMaterial &_mmMaterial ) {
		_skKey.ui64Flags = 0ULL;
		if ( _mmMaterial.IsAlpha() ) {
			_skKey.ui64Flags |= LSD_SF_TRANSLUCENCY;
		}
#ifdef LSG_OGLES
		//_skKey.ui64Flags |= LSD_SF_VERTEX_LIGHTING;
#endif	// #ifdef LSG_OGLES
		_skKey.ui64Flags |= (_mmMaterial.Type() == LSD_MA_LAMBERT) ? 0ULL : LSD_SF_SPECULAR;
		return _skKey;
	}

	/**
	 * Fills only the inputs of a shader key.
	 *
	 * \param _skKey The key whose inputs are to be filled.
	 * \param _mMesh A constant reference to the mesh that will be used to fill the shader key's inputs.
	 * \return Returns _skKey.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL CSharedModel::SetShaderKeyInputs( LSD_SHADERKEY &_skKey, const CMesh &_mMesh ) {
		if ( _mMesh.Flags() & LSD_MA_NORMALS ) {
			_skKey.ui8Inputs |= LSD_VI_NORMALS;
		}
		if ( _mMesh.Flags() & LSD_MA_COLORS ) {
			_skKey.ui8Inputs |= LSD_VI_COLORS;
		}
		if ( _mMesh.Flags() & LSD_MA_TANGENT ) {
			_skKey.ui8Inputs |= LSD_VI_TANGENT;
		}
		if ( _mMesh.Flags() & LSD_MA_BITANGENT ) {
			_skKey.ui8Inputs |= LSD_VI_BITANGENT;
		}
		
		/*for ( uint32_t K = 0; K < _mMesh.UvSets(); ++K ) {
			_skKey.ui8TextureUvs[K] = static_cast<uint8_t>(K);
		}*/

		if ( _mMesh.Flags() & LSD_MA_SKIN ) {
			_skKey.ui8Inputs |= LSD_VI_SKININDICES;
			_skKey.ui8Inputs |= LSD_VI_SKINWEIGHTS;
		}
		_skKey.ui8Uvs = static_cast<uint8_t>(_mMesh.UvSets());

		return _skKey;
	}

	/**
	 * Fills only the texture data of a shader key.
	 *
	 * \param _skKey The key whose texture data is to be filled.
	 * \param _mmMaterial A constant reference to the material that will be used to fill the shader key's texture data.
	 * \param _ui8TotalUvs Total UV's.
	 * \return Returns _skKey.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL CSharedModel::SetShaderKeyTextures( LSD_SHADERKEY &_skKey, const CModelMaterial &_mmMaterial, uint8_t _ui8TotalUvs ) {
		uint8_t ui8TexSlot = 0;

#define LSD_MAPTEX( NAME, SMALLNAME )									\
		if ( _mmMaterial.SMALLNAME##Texture().Valid() ) {				\
			_skKey.ui8Textures[LSD_TS_##NAME] = ui8TexSlot++;			\
			_skKey.ui8TextureUvs[LSD_TS_##NAME] = 0;					\
		}																\
		if ( _mmMaterial.SMALLNAME##FactorTexture().Valid() ) {			\
			_skKey.ui8Textures[LSD_TS_##NAME##_FACTOR] = ui8TexSlot++;	\
			_skKey.ui8TextureUvs[LSD_TS_##NAME##_FACTOR] = 0;			\
		}

		if ( _ui8TotalUvs ) {

			LSD_MAPTEX( DIFFUSE, Diffuse );
			LSD_MAPTEX( TRANSPARENCY, Transparency )
			LSD_MAPTEX( EMISSIVE, Emissive )
			LSD_MAPTEX( REFLECTION, Reflection )
			LSD_MAPTEX( SPECULAR, Specular )
			if ( _mmMaterial.ShininessTexture().Valid() ) {
				_skKey.ui8Textures[LSD_TS_SHININESS] = ui8TexSlot++;
				_skKey.ui8TextureUvs[LSD_TS_SHININESS] = 0;
			}
			if ( _mmMaterial.NormalMapTexture().Valid() ) {
				_skKey.ui8Textures[LSD_TS_NORMAL_MAP] = ui8TexSlot++;
				_skKey.ui8TextureUvs[LSD_TS_NORMAL_MAP] = 0;
			}

		}
		// Fix-ups.
		/*for ( uint32_t I = LSE_ELEMENTS( _skKey.ui8Textures ); I--; ) {
			if ( _skKey.ui8TextureUvs[I] >= _ui8TotalUvs ) {
				_skKey.ui8TextureUvs[I] = _skKey.ui8Textures[I] = 0xFF;
			}
		}*/


		return _skKey;
#undef LSD_MAPTEX
	}

	/**
	 * Fills only the render states of a shader key.
	 *
	 * \param _skKey The key whose render states are to be filled.
	 * \return Returns _skKey.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL CSharedModel::SetShaderKeyRenderState( LSD_SHADERKEY &_skKey ) {
		_skKey.ui8DirLights = static_cast<uint8_t>(CGfx::GetDirLightTotal());
		_skKey.ui8PointLights = static_cast<uint8_t>(CGfx::GetPointLightTotal());
		return _skKey;
	}

	/**
	 * Reads a string from an external byte stream.
	 *
	 * \param _ebsStream The byte stream from which to read the string.
	 * \param _sString The string read from the byte stream.
	 * \return Returns true if the string was fully read from the stream.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSharedModel::ReadString( const CExternalByteStream &_ebsStream, CString &_sString ) {
		_sString.Reset();
		uint32_t ui32Len = _ebsStream.ReadUInt32();
		if ( !_sString.AllocateAtLeast( ui32Len + 2 ) ) { return false; }
		for ( uint32_t I = ui32Len; I--; ) {
			if ( !_sString.Append( _ebsStream.ReadUInt8() ) ) { return false; }
		}
		/*CStd::DebugPrintA( _sString.CStr() );
		CStd::DebugPrintA( "\r\n" );*/
		return true;
	}

}	// namespace lsd

#endif	// __LSD_SHAREDMODEL_H__
