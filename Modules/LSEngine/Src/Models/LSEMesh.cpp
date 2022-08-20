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
 * Description: A mesh entity type.
 */

#include "LSEMesh.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CMesh::CMesh() :
		m_pmiSrcMesh( NULL ) {
		m_ui32EntityType |= LSE_ET_MESH;
	}
	LSE_CALLCTOR CMesh::~CMesh() {
	}

	// == Functions.
	/**
	 * Loads the mesh instance from a shared mesh.
	 *
	 * \param _smModel The shared model owning the given mesh.
	 * \param _pmiMesh The shared mesh instance from which this mesh instance is to be created.
	 * \return Returns true if the mesh was successfully loaded.  False indicates a lack of memory.
	 */
	LSBOOL LSE_CALL CMesh::LoadMeshInstance( const CSharedModel &/*_smModel*/, const lsd::CMeshInstance * _pmiMesh ) {
		assert( _pmiMesh );
		m_pmiSrcMesh = _pmiMesh;
		if ( !m_vSubMeshes.Resize( m_pmiSrcMesh->Mesh()->SubMeshes().Length() ) ) { return false; }
		for ( uint32_t I = m_pmiSrcMesh->Mesh()->SubMeshes().Length(); I--; ) {
			m_vSubMeshes[I].SetSubMesh( &m_pmiSrcMesh->Mesh()->SubMeshes()[I] );
		}
		SetName( m_pmiSrcMesh->Name() );
		return true;
	}

	/**
	 * Renders a given part of the mesh.
	 *
	 * \param _ui32Index Index of the part of the mesh to render.
	 */
	void LSE_CALL CMesh::RenderPart( uint32_t _ui32Index ) {
		assert( m_pmiSrcMesh );
		assert( _ui32Index < m_vSubMeshes.Length() );

		// NOTE: Performance check on "quick" compares and copies.

		lse::CSubMesh & smSubMesh = m_vSubMeshes[_ui32Index];
		const lsd::CMesh * pmMesh = m_pmiSrcMesh->Mesh();

		if ( !smSubMesh.Shader().Valid() ) {
			// Do everything.
			LSD_SHADERNAMEKEYPAIR snkpPair;
			snkpPair.skShaderKey = CSharedModel::CreateStandardShaderKey( smSubMesh.ShaderKey(), (*pmMesh), smSubMesh.Material() );
			snkpPair.nkNameKey = "default";
			smSubMesh.Shader() = CModelLib::ShaderManager().Translate( snkpPair );
		}
		else {
			LSD_SHADERKEY skNewKey;
			if ( !CSharedModel::CreateStandardShaderKeyQuick( skNewKey, (*pmMesh), smSubMesh.Material() ).QuickCmp( smSubMesh.ShaderKey() ) ) {
				LSD_SHADERNAMEKEYPAIR snkpPair;
				snkpPair.skShaderKey = smSubMesh.ShaderKey().QuickCpy( skNewKey );
				snkpPair.nkNameKey = "default";
				smSubMesh.Shader() = CModelLib::ShaderManager().Translate( snkpPair );
			}
		}

		CGfx::SetWorldMatrix( WorldTrans() );
		CGfx::SetShader( smSubMesh.Shader().Get() );
		ActivateMaterial( smSubMesh.Material(), smSubMesh.ShaderKey() );

		pmMesh->VertexBuffer().SetStream( 0 );
		CGfx::Render( const_cast<CIndexBuffer *>(&pmMesh->SubMeshes()[_ui32Index].IndexBuffer()),
			pmMesh->SubMeshes()[_ui32Index].BaseOffset() );
	}

	/**
	 * Inform child classes that the world transform has changed.
	 */
	void LSE_CALL CMesh::WorldDidChange() const {
		if ( m_pmiSrcMesh ) {
			m_aAabb.ComputeAabbFromAabbAndMatrix( m_pmiSrcMesh->Data().aAabb, m_mWorldTrans );
		}
		Parent::WorldDidChange();
	}

	// == Functions.
	/**
	 * Activates a given material.
	 *
	 * \param _mmMaterial Material to activate.
	 * \param _skKey Shader key of the material to activate.
	 */
	void LSE_CALL CMesh::ActivateMaterial( const CModelMaterial &_mmMaterial, const LSD_SHADERKEY &_skKey ) {
		CGfx::SetDiffuseMaterial( _mmMaterial.Diffuse() );
		CGfx::SetEmissiveMaterial( _mmMaterial.Emissive() );
		CGfx::SetSpecularMaterial( _mmMaterial.Specular() );
		

		CGfx::SetMaterialPower( _mmMaterial.Shininess() );
		CGfx::SetMaterialRoughness( _mmMaterial.Roughness(), _mmMaterial.SpecularReflectance() );
		//CGfx::SetReflectivity( _mmMaterial.Reflection() );
		CGfx::SetAshikhmanShirley( _mmMaterial.AshikminShirley().x, _mmMaterial.AshikminShirley().y );


#define LSE_ACTIVATE( Name, NAME )																							\
		if ( _mmMaterial.Name##Texture().Valid() ) {																		\
			const_cast<CSharedTexture2dPtr &>(_mmMaterial.Name##Texture())->Activate( _skKey.ui8Textures[LSD_TS_##NAME] );	\
		}

		LSE_ACTIVATE( Diffuse, DIFFUSE );
		LSE_ACTIVATE( DiffuseFactor, DIFFUSE_FACTOR );

		LSE_ACTIVATE( Transparency, TRANSPARENCY );
		LSE_ACTIVATE( TransparencyFactor, TRANSPARENCY_FACTOR );

		LSE_ACTIVATE( Emissive, EMISSIVE );
		LSE_ACTIVATE( EmissiveFactor, EMISSIVE_FACTOR );

		LSE_ACTIVATE( Reflection, REFLECTION );
		LSE_ACTIVATE( ReflectionFactor, REFLECTION_FACTOR );

		LSE_ACTIVATE( Specular, SPECULAR );
		LSE_ACTIVATE( SpecularFactor, SPECULAR_FACTOR );

		LSE_ACTIVATE( Shininess, SHININESS );

		LSE_ACTIVATE( NormalMap, NORMAL_MAP );

#undef LSE_ACTIVATE

		CGfx::OMSetBlendState( const_cast<LSG_BLEND_STATE *>(_mmMaterial.RenderStates().pbsBlendState), NULL, LSG_CWM_ALL );
		CGfx::RSSetState( const_cast<LSG_RASTERIZER_STATE *>(_mmMaterial.RenderStates().prsRasterState) );
		CGfx::OMSetDepthStencilState( const_cast<LSG_DEPTH_STENCIL_STATE *>(_mmMaterial.RenderStates().pdssDepthStencilState), 0 );
	}

}	// namespace lse
