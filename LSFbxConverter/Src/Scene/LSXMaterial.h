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


#ifndef __LSX_MATERIAL_H__
#define __LSX_MATERIAL_H__

#include "../LSXFbxConverter.h"
#include "LSXTexture.h"
#include "Models/LSDModelAttributes.h"
#include "String/LSTLString.h"

namespace lsx {

	// == Types.
	/** An array of textures. */
	typedef CVector<CTexture, LSUINT32>			CTextureArray;

	/**
	 * Class CMaterial
	 * \brief A surface material.
	 *
	 * Description: A surface material.
	 */
	class CMaterial {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMaterial();
		LSE_CALLCTOR							~CMaterial();


		// == Functions.
		/**
		 * Creates a material from an FbxSurfaceMaterial pointer.
		 *
		 * \return _psmMaterial The material from which to create the material.
		 * \return _trsStdTextures Referenced textures are added to _trsStdTextures as an output of the method.
		 * \return _trsNormalTextures Referenced normal- and bump- map textures are added to _trsNormalTextures as an output of the method.
		 * \return Returns false if there was not enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							LoadMaterial( FbxSurfaceMaterial * _psmMaterial,
			CTexture::LSX_TEXTURE_REF_SET &_trsStdTextures,
			CTexture::LSX_TEXTURE_REF_SET &_trsNormalTextures );

		/**
		 * Save the material to a binary buffer.
		 *
		 * \param _bBuffer The buffer where the data should be written.
		 * \param _trsTextures A set of texture references containing texture ID's.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							SaveMaterial( lsc::CMisc::LSC_BUFFER &_bBuffer,
			const CTexture::LSX_TEXTURE_REF_SET &_trsTextures );


	protected :
		// == Members.
		/** Name of the material. */
		CString									m_sName;

		/** ID. */
		LSUINT64								m_ui64Id;

		/** Material type. */
		LSD_MODEL_ATTRIBUTES					m_mdType;

		/** Diffuse color. */
		LSX_VECTOR								m_vDiffuse;

		/** Diffuse factor. */
		LSFLOAT									m_fDiffuseFactor;

		/** Specular color. */
		LSX_VECTOR								m_vSpecular;

		/** Specular factor. */
		LSFLOAT									m_fSpecularFactor;

		/** Emissive color. */
		LSX_VECTOR								m_vEmissive;

		/** Emissive factor. */
		LSFLOAT									m_fEmissiveFactor;

		/** Transparent color. */
		LSX_VECTOR								m_vTransparentColor;

		/** Transparent factor. */
		LSFLOAT									m_fTransparencyFactor;

		/** Reflection color. */
		LSX_VECTOR								m_vReflection;

		/** Reflection factor. */
		LSFLOAT									m_fReflectionFactor;

		/** Shininess factor. */
		LSFLOAT									m_fShininess;

#define LSX_TEX( NAME )							CTexture::LSX_TEXTURE_REF	m_tr ## NAME; CTexture::LSX_TEXTURE_TRANSFORM	m_tt ## NAME
		/** Diffuse texture. */
		LSX_TEX( Diffuse );

		/** Diffuse factor texture. */
		LSX_TEX( DiffuseFactor );

		/** Specular texture. */
		LSX_TEX( Specular );

		/** Specular factor texture. */
		LSX_TEX( SpecularFactor );

		/** Emissive texture. */
		LSX_TEX( Emissive );

		/** Emissive factor texture. */
		LSX_TEX( EmissiveFactor );

		/** Transparency texture. */
		LSX_TEX( Transparency );

		/** Transparency factor texture. */
		LSX_TEX( TransparencyFactor );

		/** Reflection texture. */
		LSX_TEX( Reflection );

		/** Reflection factor texture. */
		LSX_TEX( ReflectionFactor );

		/** Shininess texture. */
		LSX_TEX( Shininess );

		/** Normal-map texture. */
		LSX_TEX( NormalMap );

		/** Bump texture. */
		LSX_TEX( Bump );
#undef LSX_TEX

		// == Functions.
		/**
		 * Gets the texture ID associated with a given texture reference out of a set of texture references.
		 *
		 * \param _trRef The texture reference to cross-check against the set of references, which will contain
		 *	a texture ID.
		 * \param _trsSet The set of texture references that has been updated to include texture ID's.
		 * \return Returns the texture ID for the given input texture reference.
		 */
		static LSUINT32 LSE_CALL				RefToId( const CTexture::LSX_TEXTURE_REF &_trRef,
			const CTexture::LSX_TEXTURE_REF_SET &_trsSet );

		/**
		 * Fills out a CTexture::LSX_TEXTURE_TRANSFORM structure given a file texture.
		 *
		 * \param _pftTexture The file texture to use to create the CTexture::LSX_TEXTURE_TRANSFORM structure
		 *	to return.
		 * \return Returns a CTexture::LSX_TEXTURE_TRANSFORM containing information from the given file texture.
		 */
		static CTexture::LSX_TEXTURE_TRANSFORM LSE_CALL
												CreateTextureTransform( const FbxFileTexture * _pftTexture );
		
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsx

#endif	// __LSX_MATERIAL_H__
