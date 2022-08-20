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

#include "LSDSharedModel.h"
#include "../Shaders/LSDDefaultShaderTranslator.h"
#include "Image/LSIImage.h"


namespace lsd {

	// == Members.
	/** Shared model reference counter. Counts how many shared models exist. */
	CAtomic::ATOM CSharedModel::m_aSharedModelCount = 0;

	// == Various constructors.
	LSE_CALLCTOR CSharedModel::CSharedModel() {
		if ( CAtomic::InterlockedIncrement( m_aSharedModelCount ) == 1 ) {
			CModelLib::ShaderManager().AddTranslator( CDefaultShaderTranslator::Translator() );
		}
	}
	LSE_CALLCTOR CSharedModel::~CSharedModel() {
		Reset();
		if ( CAtomic::InterlockedDecrement( m_aSharedModelCount ) == 0 ) {
			// Flush unused shared textures.
			CModelLib::TextureManager().ClearUnused();
			CModelLib::ShaderManager().Reset();
		}
	}

	// == Functions.
	/**
	 * Loads model data from a shared file pointer.
	 *
	 * \param _sfpFile Shared pointer to an in-memory file from which to load the model.
	 * \param _uiptrLoadEnd upon return, holds an offset within the file data at which a successful load ended.
	 * \return Returns an error code indicating the result of the load.
	 */
	LSSTD_ERRORS LSE_CALL CSharedModel::LoadFromMemory( CSharedFilePtr _sfpFile, uintptr_t &_uiptrLoadEnd ) {
		Reset();
		if ( !_sfpFile.Valid() ) { return LSSTD_E_FILENOTFOUND; }
#define LSD_FAIL( CODE )		Reset(); return CODE
		m_sfpFile = _sfpFile;

		CExternalByteStream ebsThis( &(*_sfpFile)[0], _sfpFile->Length() );
		uint32_t ui32Header = ebsThis.ReadUInt32();
		if ( ui32Header != LSD_MA_MODEL_HEADER ) { LSD_FAIL( LSSTD_E_INVALIDFILETYPE ); }

		uint32_t ui32Version = ebsThis.ReadUInt32();
		if ( ui32Version > LSD_MA_MODEL_VERSION ) { LSD_FAIL( LSSTD_E_BADVERSION ); }

		// Number of textures.
		uint32_t ui32Textures = ebsThis.ReadUInt32();
		m_stsTextures.AllocateAtLeast( ui32Textures );
		for ( uint32_t I = 0; I < ui32Textures; ++I ) {
			uint32_t ui32Len = ebsThis.ReadUInt32();

			LSD_SHARED_TEXTURE2D st2dTemp;
			st2dTemp.ui32Id = ebsThis.ReadUInt32();

			uint32_t ui32Crc1 = ebsThis.ReadUInt32();
			uint32_t ui32Crc2 = ebsThis.ReadUInt32();
			CImage iImage;
			if ( !iImage.LoadFileFromMemory( ebsThis.GetCurBufferPointer(), ui32Len ) ) {
				LSD_FAIL( LSSTD_E_INVALIDDATA );
			}

			st2dTemp.st2pTexture = CModelLib::TextureManager().GetTexture( ui32Crc1, ui32Crc2, 0, iImage );
			if ( !st2dTemp.st2pTexture.Valid() ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }

			if ( !m_stsTextures.Insert( st2dTemp ) ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }

			ebsThis.SkipBytes( ui32Len );
		}

		// Number of materials.
		if ( !m_vMaterials.Resize( ebsThis.ReadUInt32() ) ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }
		for ( uint32_t I = 0; I < m_vMaterials.Length(); ++I ) {
			LSSTD_ERRORS eError = m_vMaterials[I].LoadFromStream( ebsThis, m_stsTextures, ui32Version );
			if ( eError != LSSTD_E_SUCCESS ) {
				LSD_FAIL( eError );
			}
		}

		uint32_t ui32Meshes = ebsThis.ReadUInt32();
		if ( ui32Meshes ) {
			if ( !m_vMeshes.Resize( ui32Meshes ) ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }
		}
		//ebsThis.PadRead( 16 );

		for ( uint32_t I = 0; I < ui32Meshes; ++I ) {
			LSSTD_ERRORS eError = m_vMeshes[I].LoadFromStream( ebsThis,
				m_vMaterials.Length() ? &m_vMaterials[0] : NULL, m_vMaterials.Length(),
				ui32Version );
			if ( eError != LSSTD_E_SUCCESS ) {
				LSD_FAIL( eError );
			}
		}

		if ( !m_vMeshInstances.Resize( ebsThis.ReadUInt32() ) ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }
		for ( uint32_t I = 0; I < m_vMeshInstances.Length(); ++I ) {
			LSSTD_ERRORS eError = m_vMeshInstances[I].LoadFromStream( ebsThis, &m_vMeshes[0], m_vMeshes.Length(), ui32Version );
			if ( eError != LSSTD_E_SUCCESS ) { LSD_FAIL( eError ); }
		}
		//ebsThis.PadRead( 16 );

		// There is always at least one node in the hierarchy.
		const LSD_MESH_HIERARCHY * pmhHierarchy = reinterpret_cast<const LSD_MESH_HIERARCHY *>(ebsThis.GetCurBufferPointer());
		const LSD_MESH_HIERARCHY * pmhHierarchyEnd = pmhHierarchy;
		uint32_t ui32Total = 1;
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			ui32Total += pmhHierarchyEnd->ui16Children;
			++pmhHierarchyEnd;
		}
		uint32_t ui32HierarchySize = static_cast<uint32_t>((pmhHierarchyEnd - pmhHierarchy));
		// Copy the hierarchy all at once.
		if ( !m_vHierarchy.Resize( ui32HierarchySize ) ) { LSD_FAIL( LSSTD_E_OUTOFMEMORY ); }
		CStd::MemCpy( &m_vHierarchy[0], pmhHierarchy, ui32HierarchySize * sizeof( LSD_MESH_HIERARCHY ) );

		ebsThis.SkipBytes( ui32HierarchySize );
		//ebsThis.PadRead( 16 );

		_uiptrLoadEnd = ebsThis.GetPos();
		m_sfpFile.Reset();
		return LSSTD_E_SUCCESS;
#undef LSD_FAIL
	}

	/**
	 * Resets everything back to scratch.
	 */
	void LSE_CALL CSharedModel::Reset() {
		m_vMaterials.Reset();
		m_vMeshInstances.Reset();
		m_vMeshes.Reset();
		m_sfpFile.Reset();
		m_stsTextures.Reset();
		m_vHierarchy.Reset();
	}

	/**
	 * Gets the total number of sub-meshes in the model.
	 *
	 * \return Returns the total number of sub-meshes in the model.
	 */
	uint32_t LSE_CALL CSharedModel::TotalSubMeshes() const {
		uint32_t ui32Total = 0;
		for ( uint32_t I = m_vMeshInstances.Length(); I--; ) {
			ui32Total += m_vMeshInstances[I].Mesh()->SubMeshes().Length();
		}
		return ui32Total;
	}

	/**
	 * Gets a constant pointer to a hierarchy entry for the given ID.
	 *
	 * \param _ui64Id The ID of the hierarchy entry to find.
	 * \return Returns a pointer to the hierarchy entry to find or NULL if it does not exist.
	 */
	const LSD_MESH_HIERARCHY * LSE_CALL CSharedModel::FindHierarchyById( uint64_t _ui64Id ) const {
		for ( uint32_t I = TotalHierarchyObjects(); I--; ) {
			if ( Hierarchy()[I].ui64Id == _ui64Id ) { return &Hierarchy()[I]; }
		}
		return NULL;
	}

	/**
	 * Creates a standard shader key given a mesh and a material.
	 *
	 * \param _skKey The key to create based off the inputs in the mesh and the data in the material.
	 * \param _mMesh A constant reference to the mesh whose input data will be used to create the shader key.
	 * \param _mmMaterial A constant reference to the material whose texture data will be used to create the shader key.
	 * \return Returns _skKey.
	 */
	LSD_SHADERKEY & LSE_FCALL CSharedModel::CreateStandardShaderKey( LSD_SHADERKEY &_skKey, const CMesh &_mMesh, const CModelMaterial &_mmMaterial ) {
		SetShaderKeyInputs( _skKey, _mMesh );
		SetShaderKeyFlags( _skKey, _mmMaterial );
		SetShaderKeyTextures( _skKey, _mmMaterial, _skKey.ui8Uvs );
		SetShaderKeyRenderState( _skKey );
		return _skKey;
	}

	/**
	 * Creates a shared file pointer given a file path.
	 *
	 * \param _pcPath UTF-8 path to the file to load and store in a CSharedFilePtr object.
	 * \return Returns the created CSharedFilePtr object, which will be invalid on failure.
	 */
	CSharedFilePtr LSE_CALL CSharedModel::SharedFilePtr( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcPath, szBuffer );
		uint64_t ui64Size = CFileLib::GetFileSize( szBuffer );
		if ( !ui64Size || ui64Size > LSE_MAXU32 ) { return CSharedFilePtr(); }
		LSF_FILE fFile = CFileLib::OpenFile( szBuffer, LSF_OF_READ, false );
		if ( !fFile ) { return CSharedFilePtr(); }
#define LSD_FAIL		CFileLib::CloseFile( fFile ); return CSharedFilePtr()
		CSharedFilePtr sfpReturn;
		if ( !sfpReturn.New() ) { LSD_FAIL; }
		if ( !sfpReturn->Resize( static_cast<uint32_t>(ui64Size) ) ) { LSD_FAIL; }

		if ( !CFileLib::ReadFile( fFile, &(*sfpReturn)[0], sfpReturn->Length() ) ) { LSD_FAIL; }

		CFileLib::CloseFile( fFile );
		return sfpReturn;
#undef LSD_FAIL
	}

}	// namespace lsd
