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
 * Description: A material for the model library.
 */

#include "LSDModelMaterial.h"
#include "LSDSharedModel.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CModelMaterial::CModelMaterial() :
		m_ui64Id( 0 ),
		m_ui16Type( LSD_MA_LAMBERT ),
		m_vDiffuse( CVector4( LSM_ONE, LSM_ONE, LSM_ONE, LSM_ONE ) ),
		m_vEmissive( CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE ) ),
		m_vTransparency( CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE ) ),
		m_vSpecular( CVector4( LSM_ONE, LSM_ONE, LSM_ONE, LSM_ONE ) ),
		m_vReflection( CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE ) ),
		m_fShininess( LSM_ONE ),
		m_vDiffuseOut( CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE ) ),
		m_bFlipCulling( false ) {
	}
	LSE_CALLCTOR CModelMaterial::~CModelMaterial() {
	}

	// == Functions.
	/**
	 * Loads a material from a byte stream.
	 *
	 * \param _ebsStream Byte stream from which to read.
	 * \param _stsTextures The set of shared 2D textures.
	 * \param _ui32Version File version number.
	 * \return Returns an error code indicating the result of the load.
	 */
	LSSTD_ERRORS LSE_CALL CModelMaterial::LoadFromStream( const CExternalByteStream &_ebsStream, const LSD_SHARED_TEXTURE2D_SET &_stsTextures,
		uint32_t /*_ui32Version*/ ) {
		if ( !CSharedModel::ReadString( _ebsStream, m_sName ) ) { return LSSTD_E_OUTOFMEMORY; }
		//_ebsStream.PadRead( 8 );

		m_ui64Id = _ebsStream.ReadUInt64();
		m_ui16Type = _ebsStream.ReadUInt16();
#define LSD_READ_VEC( NAME )	NAME.x = _ebsStream.ReadFloat();														\
	NAME.y = _ebsStream.ReadFloat();																					\
	NAME.z = _ebsStream.ReadFloat();																					\
	NAME.w = _ebsStream.ReadFloat()

		LSD_READ_VEC( m_vDiffuse );
		LSD_READ_VEC( m_vEmissive );
		LSD_READ_VEC( m_vTransparency );
		if ( m_ui16Type == LSD_MA_PHONG ) {
			LSD_READ_VEC( m_vSpecular );
			LSD_READ_VEC( m_vReflection );
			m_fShininess = _ebsStream.ReadFloat();
		}

		
		uint32_t ui32Diffuse = 0;
		uint32_t ui32DiffuseFactor = 0;
		uint32_t ui32Specular = 0;
		uint32_t ui32SpecularFactor = 0;
		uint32_t ui32Emissive = 0;
		uint32_t ui32EmissiveFactor = 0;
		uint32_t ui32Transparency = 0;
		uint32_t ui32TransparencyFactor = 0;
		uint32_t ui32Reflection = 0;
		uint32_t ui32ReflectionFactor = 0;
		uint32_t ui32Shininess = 0;
		uint32_t ui32NormalMap = 0;
#define LSD_READ_TEX( NAME, NAME_UPPER )		ui32 ## NAME = _ebsStream.ReadUInt32();																\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fScaleX = _ebsStream.ReadFloat();																\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fScaleY = _ebsStream.ReadFloat();																\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fTransX = _ebsStream.ReadFloat();																\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fTransY = _ebsStream.ReadFloat();																\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fRotX = _ebsStream.ReadFloat();																	\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fRotY = _ebsStream.ReadFloat();																	\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].fRotZ = _ebsStream.ReadFloat();																	\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].tamWrapU = GetGfxWrapMode( static_cast<LSD_MODEL_ATTRIBUTES>(_ebsStream.ReadUInt32() ));		\
		m_tsTextureSettings[LSD_MMT_ ## NAME_UPPER].tamWrapV = GetGfxWrapMode( static_cast<LSD_MODEL_ATTRIBUTES>(_ebsStream.ReadUInt32() ));

		LSD_READ_TEX( Diffuse, DIFFUSE );
		LSD_READ_TEX( DiffuseFactor, DIFFUSE_FACTOR );
		LSD_READ_TEX( Emissive, EMISSIVE );
		LSD_READ_TEX( EmissiveFactor, EMISSIVE_FACTOR );
		LSD_READ_TEX( Transparency, TRANSPARENCY );
		LSD_READ_TEX( TransparencyFactor, TRANSPARENCY_FACTOR );
		if ( m_ui16Type == LSD_MA_PHONG ) {
			LSD_READ_TEX( Specular, SPECULAR );
			LSD_READ_TEX( SpecularFactor, SPECULAR_FACTOR );
			LSD_READ_TEX( Reflection, REFLECTION );
			LSD_READ_TEX( ReflectionFactor, REFLECTION_FACTOR );
			LSD_READ_TEX( Shininess, SHININESS );
		}
		LSD_READ_TEX( NormalMap, NORMAL_MAP );

#undef LSD_READ_TEX

		// Link textures.
		uint32_t ui32Index;
#define LSD_LINK_TEX( NAME )																							\
		if ( ui32 ## NAME ) {																							\
			LSD_SHARED_TEXTURE2D st2dTemp = { ui32 ## NAME };															\
			if ( _stsTextures.GetItemIndex( st2dTemp, ui32Index ) ) {													\
				m_st2p ## NAME = _stsTextures.GetByIndex( ui32Index ).st2pTexture;										\
			}																											\
		}

		LSD_LINK_TEX( Diffuse );
		LSD_LINK_TEX( DiffuseFactor );
		LSD_LINK_TEX( Emissive );
		LSD_LINK_TEX( EmissiveFactor );
		LSD_LINK_TEX( Transparency );
		LSD_LINK_TEX( TransparencyFactor );
		LSD_LINK_TEX( Specular );
		LSD_LINK_TEX( SpecularFactor );
		LSD_LINK_TEX( Reflection );
		LSD_LINK_TEX( ReflectionFactor );
		LSD_LINK_TEX( Shininess );
		LSD_LINK_TEX( NormalMap );

		// Dirty all texture matrices and update sampler states.
		m_ui32MatrixDirty = m_ui32MatrixIdentity = 0;
		for ( uint32_t I = LSD_MMT_TOTAL; I--; ) {
			m_ui32MatrixDirty |= 1 << I;
			m_sSamplers[I].SetAddressMode( m_tsTextureSettings[I].tamWrapU, m_tsTextureSettings[I].tamWrapV );
		}

		UpdateDiffuse();
		UpdateEmissive();
		UpdateSpecular();
		UpdateReflection();
		UpdateMisc();
		UpdateRenderStates();
		return LSSTD_E_SUCCESS;
#undef LSD_LINK_TEX
#undef LSD_READ_VEC
	}

	/**
	 * Updates the rasterizer, depth/stencil, and blend states based on the material settings.
	 *
	 * \return Returns true if all states were updated.
	 */
	LSBOOL LSE_CALL CModelMaterial::UpdateRenderStates() const {
		{
			CGfx::ReleaseState( m_rsRenderStates.pbsBlendState );
			
			if ( IsAlpha() ) {
				LSG_BLEND_DESC bdDesc;
				CGfx::DefaultBlendDesc( bdDesc );
				bdDesc.RenderTarget[0].BlendEnable = true;
				bdDesc.RenderTarget[0].SrcBlend = CStd::ImpCast( bdDesc.RenderTarget[0].SrcBlend, LSG_BM_SRC_ALPHA );
				bdDesc.RenderTarget[0].DestBlend = CStd::ImpCast( bdDesc.RenderTarget[0].DestBlend, LSG_BM_INV_SRC_ALPHA );
				bdDesc.RenderTarget[0].SrcBlendAlpha = CStd::ImpCast( bdDesc.RenderTarget[0].SrcBlendAlpha, LSG_BM_SRC_ALPHA );
				bdDesc.RenderTarget[0].DestBlendAlpha = CStd::ImpCast( bdDesc.RenderTarget[0].DestBlendAlpha, LSG_BM_ONE );
				bdDesc.RenderTarget[0].RenderTargetWriteMask = LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE;
				if ( FAILED( CGfx::CreateBlendState( bdDesc, &m_rsRenderStates.pbsBlendState ) ) ) { return false; }
			}
		}
		{
			CGfx::ReleaseState( m_rsRenderStates.prsRasterState );

			LSG_RASTERIZER_DESC rdDesc;
			CGfx::DefaultRasterizerDesc( rdDesc );
			if ( m_bFlipCulling ) {
				rdDesc.FrontCounterClockwise = !rdDesc.FrontCounterClockwise;
			}
			if ( IsAlpha() ) {
				rdDesc.CullMode = CStd::ImpCast( rdDesc.CullMode, LSG_CM_NONE );
			}

			if ( FAILED( CGfx::CreateRasterizerState( rdDesc, &m_rsRenderStates.prsRasterState ) ) ) { return false; }
		}
		{
			CGfx::ReleaseState( m_rsRenderStates.pdssDepthStencilState );

			LSG_DEPTH_STENCIL_DESC dsdDesc;
			CGfx::DefaultDepthStencilDesc( dsdDesc );
			if ( FAILED( CGfx::CreateDepthStencilState( dsdDesc, &m_rsRenderStates.pdssDepthStencilState ) ) ) { return false; }
		}

		return true;
	}

	/**
	 * Updates all miscellanous factors.
	 */
	void LSE_CALL CModelMaterial::UpdateMisc() {
		m_fShininessOut = m_fShininess;

		if ( (m_fShininessOut + LSM_REAL_EPSILON * LSM_TWO) < LSM_TWO && m_fShininessOut >= LSM_ONE ) {
			// This was originally Blinn.  Get the original coefficient.
			LSREAL fCoef = LSM_ONE / CMathLib::Log( static_cast<LSREAL>(3.0), m_fShininessOut );
			LSREAL fTemp = (LSM_ONE - fCoef) / (LSM_ONE + fCoef);
			m_vRoughnessSpecReflect.y = fTemp * fTemp;
			fCoef *= static_cast<LSREAL>(1.0 / 0.045);	// The real number should be more exact, but this is damn close.

			m_fShininessOut = fCoef * fCoef;
			m_fShininessOut *= static_cast<LSREAL>(128.0 / 100.0);
			m_vRoughnessSpecReflect.x = CMathLib::Sqrt( LSM_TWO / CStd::Max( m_fShininessOut, LSM_REAL_EPSILON ) );
		}
		else {
			m_vRoughnessSpecReflect.x = CMathLib::Sqrt( LSM_TWO / CStd::Max( m_fShininessOut, LSM_REAL_EPSILON ) );
			m_vRoughnessSpecReflect.y = (LSM_ONE - m_vRoughnessSpecReflect.x) / (LSM_ONE + m_vRoughnessSpecReflect.x);
			m_vRoughnessSpecReflect.y *= m_vRoughnessSpecReflect.y;
		}
		if ( Reflection().w ) {
			m_vRoughnessSpecReflect.y = (Reflection().w + m_vRoughnessSpecReflect.y) / LSM_TWO;
			/*LSREAL fTemp = LSM_ONE - Reflection().w;
			m_vRoughnessSpecReflect.y = (LSM_ONE - fTemp) / (LSM_ONE + fTemp);
			m_vRoughnessSpecReflect.y *= m_vRoughnessSpecReflect.y;*/
		}

		m_vRoughnessSpecReflect.x = CStd::Min( m_vRoughnessSpecReflect.x, LSM_ONE );



		LSREAL fTemp = m_fShininessOut * static_cast<LSREAL>(1.0 / 128.0);
		if ( fTemp ) {
			fTemp = CMathLib::Sqrt( fTemp );
		}
		//fTemp *= fTemp;
		LSREAL fAshik = CStd::Max( fTemp * static_cast<LSREAL>(10000.0 / 16.0), LSM_REAL_EPSILON );
		m_vAshikhminShirley.x = m_vAshikhminShirley.y = fAshik;

	}

	/**
	 * Creates the texture matrix given texture settings.
	 *
	 * \param _tsSettings Texture settings to assemble into a texture matrix.
	 * \param _ui32BitFlag Bit for changing dirty and identity flags.
	 * \return Returns the created matrix.
	 */
	CMatrix4x4 LSE_CALL CModelMaterial::CreateTextureMatrix( const LSD_TEXTURE_SETTINGS &_tsSettings, uint32_t _ui32BitFlag ) const {
		_ui32BitFlag = (1 << _ui32BitFlag);
		m_ui32MatrixDirty &= ~_ui32BitFlag;

		CMatrix4x4 mScale;
		mScale.MatrixScaling( _tsSettings.fScaleX, _tsSettings.fScaleY, LSM_ONE );

		CMatrix4x4 mRot;
		mRot.MatrixRotationXYZLH( _tsSettings.fRotX, _tsSettings.fRotY, _tsSettings.fRotZ );

		CMatrix4x4 mFinal = mScale * mRot;
		mFinal._41 = _tsSettings.fTransX;
		mFinal._42 = _tsSettings.fTransY;

#define LSD_CMP( MEM, VAL )		CMathLib::Equals( MEM, VAL, LSM_FLT_EPSILON )
		if ( LSD_CMP( _tsSettings.fScaleX, LSM_ONE ) && LSD_CMP( _tsSettings.fScaleY, LSM_ONE ) &&
			LSD_CMP( _tsSettings.fTransX, LSM_ZERO ) && LSD_CMP( _tsSettings.fTransY, LSM_ZERO ) &&
			LSD_CMP( _tsSettings.fRotX, LSM_ZERO ) && LSD_CMP( _tsSettings.fRotY, LSM_ZERO ) && LSD_CMP( _tsSettings.fRotZ, LSM_ZERO ) ) {
			m_ui32MatrixIdentity |= _ui32BitFlag;
		}
		else {
			m_ui32MatrixIdentity &= ~_ui32BitFlag;
		}
#undef LSD_CMP
		return mFinal;
	}

}	// namespace lsd
