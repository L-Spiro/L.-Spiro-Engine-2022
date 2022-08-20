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
 * Description: A surface material.
 */

#include "LSXMaterial.h"
#include "../Misc/LSXMisc.h"


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CMaterial::CMaterial() :
		m_ui64Id( 0 ),
		m_mdType( LSD_MA_LAMBERT ),
		m_fDiffuseFactor( 1.0f ),
		m_fSpecularFactor( 1.0f ),
		m_fEmissiveFactor( 0.0f ),
		m_fTransparencyFactor( 0.0f ),
		m_fReflectionFactor( 1.0f ),
		m_fShininess( 100.0f ) {
	}
	LSE_CALLCTOR CMaterial::~CMaterial() {
	}

	// == Functions.
	/**
	 * Creates a material from an FbxSurfaceMaterial pointer.
	 *
	 * \return _psmMaterial The material from which to create the material.
	 * \return _trsStdTextures Referenced textures are added to _trsStdTextures as an output of the method.
	 * \return _trsNormalTextures Referenced normal- and bump- map textures are added to _trsNormalTextures as an output of the method.
	 * \return Returns false if there was not enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CMaterial::LoadMaterial( FbxSurfaceMaterial * _psmMaterial,
		CTexture::LSX_TEXTURE_REF_SET &_trsStdTextures,
		CTexture::LSX_TEXTURE_REF_SET &_trsNormalTextures ) {
		m_sName = _psmMaterial->GetName();
		m_ui64Id = (*reinterpret_cast<LSUINTPTR *>(&_psmMaterial));
		m_mdType = LSD_MA_LAMBERT;

		FbxSurfaceLambert * pspLambert = reinterpret_cast<FbxSurfaceLambert *>(_psmMaterial);
		// Copy the common colors.
		FbxPropertyT<FbxDouble3> pdDouble3;
#define LSX_COPYCOLOR( PROP )		try { pdDouble3 = pspLambert->PROP;							\
	m_v ## PROP = LSX_VECTOR( static_cast<LSFLOAT>(pdDouble3.Get()[0]),							\
		static_cast<LSFLOAT>(pdDouble3.Get()[1]),												\
		static_cast<LSFLOAT>(pdDouble3.Get()[2]) ); } catch ( ... ) {}
#define LSX_COPYFACTOR( PROP )		try { m_f ## PROP = static_cast<LSFLOAT>(pspLambert->PROP.Get()); } catch ( ... ) { m_f ## PROP = 1.0f; }

		LSX_COPYCOLOR( Diffuse );
		LSX_COPYFACTOR( DiffuseFactor );
		LSX_COPYCOLOR( Emissive );
		LSX_COPYFACTOR( EmissiveFactor );
		LSX_COPYCOLOR( TransparentColor );
		LSX_COPYFACTOR( TransparencyFactor );

		if ( _psmMaterial->GetClassId().Is( FbxSurfacePhong::ClassId ) ) {
#undef LSX_COPYFACTOR
#undef LSX_COPYCOLOR

#define LSX_COPYCOLOR( PROP )		try { pdDouble3 = pspPhong->PROP;							\
	m_v ## PROP = LSX_VECTOR( static_cast<LSFLOAT>(pdDouble3.Get()[0]),							\
		static_cast<LSFLOAT>(pdDouble3.Get()[1]),												\
		static_cast<LSFLOAT>(pdDouble3.Get()[2]) ); } catch ( ... ) {}
#define LSX_COPYFACTOR( PROP )		try { m_f ## PROP = static_cast<LSFLOAT>(pspPhong->PROP.Get()); } catch ( ... ) { m_f ## PROP = 1.0f; }

			FbxSurfacePhong * pspPhong = reinterpret_cast<FbxSurfacePhong *>(_psmMaterial);
			m_mdType = LSD_MA_PHONG;

			LSX_COPYCOLOR( Specular );
			LSX_COPYFACTOR( SpecularFactor );

			LSX_COPYCOLOR( Reflection );
			LSX_COPYFACTOR( ReflectionFactor );

			LSX_COPYFACTOR( Shininess );	// Specular power.
		}
		
#define LSX_TEX( TYPE ) _psmMaterial->FindProperty( FbxLayerElement::sTextureChannelNames[FbxLayerElement::TYPE-FbxLayerElement::eTextureDiffuse] )
		FbxProperty pDiffuse = LSX_TEX( eTextureDiffuse );
		FbxProperty pDiffuseFactor = LSX_TEX( eTextureDiffuseFactor );
		FbxProperty pTransparency = LSX_TEX( eTextureTransparency );
		FbxProperty pTransparencyFactor = LSX_TEX( eTextureTransparencyFactor );
		FbxProperty pEmissive = LSX_TEX( eTextureEmissive );
		FbxProperty pEmissiveFactor = LSX_TEX( eTextureEmissiveFactor );
		FbxProperty pSpecular;
		FbxProperty pSpecularFactor;
		FbxProperty pReflection;
		FbxProperty pReflectionFactor;
		FbxProperty pShininess;
		FbxProperty pNormalMap = LSX_TEX( eTextureNormalMap );
		FbxProperty pBump = LSX_TEX( eTextureBump );

#define LSX_TEX_2_FILE_TEX( PROP, FILETEX )		if ( PROP.IsValid() ) {								\
			if ( PROP.GetSrcObjectCount<FbxFileTexture>() ) {										\
				FILETEX = FbxCast<FbxFileTexture>(PROP.GetSrcObject<FbxFileTexture>( 0 ));			\
			}																						\
		}

		FbxFileTexture * pftDiffuse = NULL;
		LSX_TEX_2_FILE_TEX( pDiffuse, pftDiffuse );
		FbxFileTexture * pftDiffuseFactor = NULL;
		LSX_TEX_2_FILE_TEX( pDiffuseFactor, pftDiffuseFactor );
		FbxFileTexture * pftTransparency = NULL;
		LSX_TEX_2_FILE_TEX( pTransparency, pftTransparency );
		FbxFileTexture * pftTransparencyFactor = NULL;
		LSX_TEX_2_FILE_TEX( pTransparencyFactor, pftTransparencyFactor );
		FbxFileTexture * pftEmissive = NULL;
		LSX_TEX_2_FILE_TEX( pEmissive, pftEmissive );
		FbxFileTexture * pftEmissiveFactor = NULL;
		LSX_TEX_2_FILE_TEX( pEmissiveFactor, pftEmissiveFactor );
		FbxFileTexture * pftSpecular = NULL;
		FbxFileTexture * pftSpecularFactor = NULL;
		FbxFileTexture * pftReflection = NULL;
		FbxFileTexture * pftReflectionFactor = NULL;
		FbxFileTexture * pftShininess = NULL;
		FbxFileTexture * pftNormalMap = NULL;
		LSX_TEX_2_FILE_TEX( pNormalMap, pftNormalMap );
		FbxFileTexture * pftBump = NULL;
		LSX_TEX_2_FILE_TEX( pBump, pftBump );
		
		if ( m_mdType == LSD_MA_PHONG ) {
			pSpecular = LSX_TEX( eTextureSpecular );
			pSpecularFactor = LSX_TEX( eTextureSpecularFactor );
			pReflection = LSX_TEX( eTextureReflection );
			pReflectionFactor = LSX_TEX( eTextureReflectionFactor );
			pShininess = LSX_TEX( eTextureShininess );

			LSX_TEX_2_FILE_TEX( pSpecular, pftSpecular );
			LSX_TEX_2_FILE_TEX( pSpecularFactor, pftSpecularFactor );
			LSX_TEX_2_FILE_TEX( pReflection, pftReflection );
			LSX_TEX_2_FILE_TEX( pReflectionFactor, pftReflectionFactor );
			LSX_TEX_2_FILE_TEX( pShininess, pftShininess );
		}

		// Standard RGB textures first.
#define LSX_ADD_TEX_REF( LOCAL, FILETEX, COMPONENT, FLAG )											\
		if ( FILETEX && FILETEX->GetFileName() && CStd::StrLen( FILETEX->GetFileName() ) ) {		\
			m_tr ## LOCAL.pftTexture = FILETEX;														\
			m_tr ## LOCAL.ui16Component = COMPONENT;												\
			m_tr ## LOCAL.ui16Flags = FLAG;															\
			if ( !_trsStdTextures.Insert( m_tr ## LOCAL ) ) { return false; }						\
			m_tt ## LOCAL = CreateTextureTransform( FILETEX );										\
		}
		
		LSX_ADD_TEX_REF( Diffuse, pftDiffuse, CTexture::LSX_C_RGB, LSX_TEX_sRGB );
		LSX_ADD_TEX_REF( Specular, pftSpecular, CTexture::LSX_C_RGB, LSX_TEX_sRGB );
		LSX_ADD_TEX_REF( Emissive, pftEmissive, CTexture::LSX_C_RGB, LSX_TEX_sRGB );
		LSX_ADD_TEX_REF( Reflection, pftReflection, CTexture::LSX_C_RGB, LSX_TEX_sRGB );

		// RGB-modifier textures.
		LSX_ADD_TEX_REF( Transparency, pftTransparency, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( DiffuseFactor, pftDiffuseFactor, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( SpecularFactor, pftSpecularFactor, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( EmissiveFactor, pftEmissiveFactor, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( TransparencyFactor, pftTransparencyFactor, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( ReflectionFactor, pftReflectionFactor, CTexture::LSX_C_A, 0 );
		LSX_ADD_TEX_REF( Shininess, pftShininess, CTexture::LSX_C_A, 0 );

#undef LSX_ADD_TEX_REF
#define LSX_ADD_TEX_REF( LOCAL, FILETEX )															\
		if ( FILETEX && FILETEX->GetFileName() && CStd::StrLen( FILETEX->GetFileName() ) ) {		\
			m_tr ## LOCAL.pftTexture = FILETEX;														\
			m_tr ## LOCAL.ui16Component = 0;														\
			m_tr ## LOCAL.ui16Flags = 0;															\
			if ( !_trsNormalTextures.Insert( m_tr ## LOCAL ) ) { return false; }					\
			m_tt ## LOCAL = CreateTextureTransform( FILETEX );										\
		}
		LSX_ADD_TEX_REF( NormalMap, pftNormalMap );
		LSX_ADD_TEX_REF( Bump, pftBump );

		return true;
#undef LSX_ADD_TEX_REF
#undef LSX_TEX_2_FILE_TEX
#undef LSX_TEX
#undef LSX_COPYFACTOR
#undef LSX_COPYCOLOR
	}

	/**
	 * Save the material to a binary buffer.
	 *
	 * \param _bBuffer The buffer where the data should be written.
	 * \param _trsTextures A set of texture references containing texture ID's.
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CMaterial::SaveMaterial( lsc::CMisc::LSC_BUFFER &_bBuffer,
		const CTexture::LSX_TEXTURE_REF_SET &_trsTextures ) {
		lsc::CMisc::LSC_BUFFER bTemp;
		if ( !lsx::CMisc::WriteString( m_sName, bTemp ) ) { return false; }
		//if ( !CMisc::Pad( bTemp, 8 ) ) { return false; }
		if ( !lsx::CMisc::WriteUInt64( m_ui64Id, bTemp ) ) { return false; }
		if ( !lsx::CMisc::WriteUInt16( static_cast<LSUINT16>(m_mdType), bTemp ) ) { return false; }
#define LSX_SAVE_VEC( NAME )	if ( !lsx::CMisc::WriteFloat( NAME.x, bTemp ) ) { return false; }	\
		if ( !lsx::CMisc::WriteFloat( NAME.y, bTemp ) ) { return false; }							\
		if ( !lsx::CMisc::WriteFloat( NAME.z, bTemp ) ) { return false; }

		LSX_SAVE_VEC( m_vDiffuse );
		if ( !lsx::CMisc::WriteFloat( m_fDiffuseFactor, bTemp ) ) { return false; }

		LSX_SAVE_VEC( m_vEmissive );
		if ( !lsx::CMisc::WriteFloat( m_fEmissiveFactor, bTemp ) ) { return false; }

		LSX_SAVE_VEC( m_vTransparentColor );
		if ( !lsx::CMisc::WriteFloat( m_fTransparencyFactor, bTemp ) ) { return false; }

		if ( m_mdType == LSD_MA_PHONG ) {
			LSX_SAVE_VEC( m_vSpecular );
			if ( !lsx::CMisc::WriteFloat( m_fSpecularFactor, bTemp ) ) { return false; }

			LSX_SAVE_VEC( m_vReflection );
			if ( !lsx::CMisc::WriteFloat( m_fReflectionFactor, bTemp ) ) { return false; }

			if ( !lsx::CMisc::WriteFloat( m_fShininess, bTemp ) ) { return false; }
		}
		// Texture references.
#define LSX_EXP_TEX( NAME )	if ( !lsx::CMisc::WriteUInt( RefToId( m_tr ## NAME, _trsTextures ), bTemp ) ) { return false; }		\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fScaleX, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fScaleY, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fTransX, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fTransY, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fRotX, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fRotY, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteFloat( m_tt ## NAME.fRotZ, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteUInt( m_tt ## NAME.ui32WrapU, bTemp ) ) { return false; }											\
		if ( !lsx::CMisc::WriteUInt( m_tt ## NAME.ui32WrapV, bTemp ) ) { return false; }

		LSX_EXP_TEX( Diffuse )
		LSX_EXP_TEX( DiffuseFactor )
		LSX_EXP_TEX( Emissive )
		LSX_EXP_TEX( EmissiveFactor )
		LSX_EXP_TEX( Transparency )
		LSX_EXP_TEX( TransparencyFactor )
		if ( m_mdType == LSD_MA_PHONG ) {
			LSX_EXP_TEX( Specular )
			LSX_EXP_TEX( SpecularFactor )
			LSX_EXP_TEX( Reflection )
			LSX_EXP_TEX( ReflectionFactor )
			LSX_EXP_TEX( Shininess )
		}
		if ( m_trNormalMap.pftTexture ) {
			LSX_EXP_TEX( NormalMap )
		}
		else {
			LSX_EXP_TEX( Bump )
		}

		if ( bTemp.Length() ) {
			if ( !CMisc::WriteBytes( &bTemp[0], bTemp.Length(), _bBuffer ) ) { return false; }
		}

		return true;
#undef LSX_EXP_TEX
#undef LSX_SAVE_VEC
	}

	/**
	 * Gets the texture ID associated with a given texture reference out of a set of texture references.
	 *
	 * \param _trRef The texture reference to cross-check against the set of references, which will contain
	 *	a texture ID.
	 * \param _trsSet The set of texture references that has been updated to include texture ID's.
	 * \return Returns the texture ID for the given input texture reference.
	 */
	LSUINT32 LSE_CALL CMaterial::RefToId( const CTexture::LSX_TEXTURE_REF &_trRef,
		const CTexture::LSX_TEXTURE_REF_SET &_trsSet ) {
		if ( !_trRef.pftTexture ) { return 0; }
		for ( LSUINT32 I = _trsSet.Length(); I--; ) {
			if ( _trsSet.GetByIndex( I ) == _trRef ) {
				register LSUINT32 ui32Ret = _trsSet.GetByIndex( I ).ui32TextureId;
				return ui32Ret;
			}
		}
		return 0;
	}

	/**
	 * Fills out a CTexture::LSX_TEXTURE_TRANSFORM structure given a file texture.
	 *
	 * \param _pftTexture The file texture to use to create the CTexture::LSX_TEXTURE_TRANSFORM structure
	 *	to return.
	 * \return Returns a CTexture::LSX_TEXTURE_TRANSFORM containing information from the given file texture.
	 */
	CTexture::LSX_TEXTURE_TRANSFORM LSE_CALL CMaterial::CreateTextureTransform( const FbxFileTexture * _pftTexture ) {
		CTexture::LSX_TEXTURE_TRANSFORM ttTrans;
		ttTrans.fScaleX = static_cast<LSFLOAT>(_pftTexture->GetScaleU());
		ttTrans.fScaleY = static_cast<LSFLOAT>(_pftTexture->GetScaleV());
		ttTrans.fTransX = static_cast<LSFLOAT>(_pftTexture->GetTranslationU());
		ttTrans.fTransY = static_cast<LSFLOAT>(_pftTexture->GetTranslationV());
		ttTrans.fRotX = static_cast<LSFLOAT>(_pftTexture->GetRotationU());
		ttTrans.fRotY = static_cast<LSFLOAT>(_pftTexture->GetRotationV());
		ttTrans.fRotZ = static_cast<LSFLOAT>(_pftTexture->GetRotationW());
		ttTrans.ui32WrapU = CTexture::WrapMode( _pftTexture->GetWrapModeU() );
		ttTrans.ui32WrapV = CTexture::WrapMode( _pftTexture->GetWrapModeV() );
		return ttTrans;
	}

}	// namespace lsx
