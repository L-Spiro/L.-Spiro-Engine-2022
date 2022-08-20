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


#ifndef __LSD_MODELMATERIAL_H__
#define __LSD_MODELMATERIAL_H__

#include "../LSDModelLib.h"
#include "../Textures/LSDSharedTexture2d.h"
#include "Gfx/LSGGfx.h"
#include "LSDModelAttributes.h"
#include "Sampler/LSGSampler.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "String/LSTLString.h"
#include "Vector/LSMVector4.h"

#pragma warning( push )

// warning C4324: 'lsd::CModelMaterial' : structure was padded due to __declspec(align())
#pragma warning( disable : 4324 )

namespace lsd {

	/**
	 * Class CModelMaterial
	 * \brief A material for the model library.
	 *
	 * Description: A material for the model library.
	 */
	class CModelMaterial {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CModelMaterial();
		LSE_CALLCTOR							~CModelMaterial();


		// == Enumerations.
		/** Types of textures. */
		enum LSD_MODEL_MATERIAL_TEXTURES {
			LSD_MMT_DIFFUSE						= 0,								/**< Diffuse texture. */
			LSD_MMT_DIFFUSE_FACTOR,													/**< Diffuse factor texture. */
			LSD_MMT_EMISSIVE,														/**< Emissive texture. */
			LSD_MMT_EMISSIVE_FACTOR,												/**< Emissive factor texture. */
			LSD_MMT_SPECULAR,														/**< Specular texture. */
			LSD_MMT_SPECULAR_FACTOR,												/**< Specular factor texture. */
			LSD_MMT_TRANSPARENCY,													/**< Specular texture. */
			LSD_MMT_TRANSPARENCY_FACTOR,											/**< Specular factor texture. */
			LSD_MMT_REFLECTION,														/**< Reflection texture. */
			LSD_MMT_REFLECTION_FACTOR,												/**< Reflection factor texture. */
			LSD_MMT_SHININESS,														/**< Shininess texture. */
			LSD_MMT_NORMAL_MAP,														/**< Normal-map texture. */

			LSD_MMT_TOTAL,															/**< Total textures. */
		};


		// == Types.
		/** Texture settings. */
		typedef struct LSD_TEXTURE_SETTINGS {
			LSREAL								fScaleX;							/**< U scale. */
			LSREAL								fScaleY;							/**< V scale. */
			LSREAL								fTransX;							/**< U translate. */
			LSREAL								fTransY;							/**< V translate. */
			LSREAL								fRotX;								/**< U rotation. */
			LSREAL								fRotY;								/**< V rotation. */
			LSREAL								fRotZ;								/**< W rotation. */
			LSG_TEXTURE_ADDRESS_MODE			tamWrapU;							/**< U wrap mode. */
			LSG_TEXTURE_ADDRESS_MODE			tamWrapV;							/**< V wrap mode. */


			// == Various constructors.
			LSE_CALLCTOR						LSD_TEXTURE_SETTINGS() :
				fScaleX( 1.0f ),
				fScaleY( 1.0f ),
				fTransX( 0.0f ),
				fTransY( 0.0f ),
				fRotX( 0.0f ),
				fRotY( 0.0f ),
				fRotZ( 0.0f ),
				tamWrapU( LSG_TAM_WRAP ),
				tamWrapV( LSG_TAM_WRAP ) {
			}
		} * LPLSD_TEXTURE_SETTINGS, * const LPCLSD_TEXTURE_SETTINGS;

		/** Graphics states. */
		typedef struct LSD_RENDER_STATES {
			LSG_RASTERIZER_STATE *				prsRasterState;						/**< Rasterizer state. */
			LSG_BLEND_STATE *					pbsBlendState;						/**< Blend state. */
			LSG_DEPTH_STENCIL_STATE *			pdssDepthStencilState;				/**< Depth/stencil state. */


			// == Various constructors.
			LSE_CALLCTOR						LSD_RENDER_STATES() :
				prsRasterState( NULL ),
				pbsBlendState( NULL ),
				pdssDepthStencilState( NULL ) {
			}
			LSE_CALLCTOR						~LSD_RENDER_STATES() {
				CGfx::ReleaseState( prsRasterState );
				CGfx::ReleaseState( pbsBlendState );
				CGfx::ReleaseState( pdssDepthStencilState );
			}


			// == Operators.
			/**
			 * Assignment operator.
			 *
			 * \param _rsOther Object to copy.
			 * \return Returns a reference to this object.
			 */
			LSE_INLINE LSD_RENDER_STATES & LSE_FCALL
												operator = ( const LSD_RENDER_STATES &/*_rsOther*/ ) {
				// Delete resources we may have allocated.
				CGfx::ReleaseState( prsRasterState );
				CGfx::ReleaseState( pbsBlendState );
				CGfx::ReleaseState( pdssDepthStencilState );

				// New resources are meant to be created externally.  Don't create anything.
				return (*this);
			}

		} * LPLSD_RENDER_STATES, * const LPCLSD_RENDER_STATES;


		// == Functions.
		/**
		 * Loads a material from a byte stream.
		 *
		 * \param _ebsStream Byte stream from which to read.
		 * \param _stsTextures The set of shared 2D textures.
		 * \param _ui32Version File version number.
		 * \return Returns an error code indicating the result of the load.
		 */
		LSSTD_ERRORS LSE_CALL					LoadFromStream( const CExternalByteStream &_ebsStream, const LSD_SHARED_TEXTURE2D_SET &_stsTextures,
			uint32_t _ui32Version );

		/**
		 * Gets the material ID.
		 *
		 * \return Returns the material ID.
		 */
		LSE_INLINE uint64_t LSE_CALL			Id() const;

		/**
		 * Gets the material type.
		 *
		 * \return Returns the material type.
		 */
		LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL
												Type() const;

		/**
		 * Gets the diffuse component after scaling by its factor and applying a transparency modifier.
		 *
		 * \return Returns the diffuse component after scaling by its factor and applying a transparency modifier.
		 */
		LSE_INLINE const CVector4 & LSE_CALL	Diffuse() const;

		/**
		 * Gets the emissive component after scaling by its factor.
		 *
		 * \return Returns the emissive component after scaling by its factor.
		 */
		LSE_INLINE const CVector4 & LSE_CALL	Emissive() const;

		/**
		 * Gets the specular component after scaling by its factor.
		 *
		 * \return Returns the specular component after scaling by its factor.
		 */
		LSE_INLINE const CVector4 & LSE_CALL	Specular() const;

		/**
		 * Gets the reflection component after scaling by its factor.
		 *
		 * \return Returns the reflection component after scaling by its factor.
		 */
		LSE_INLINE const CVector4 & LSE_CALL	Reflection() const;

		/**
		 * Gets the specular power.
		 *
		 * \return Returns the specular power.
		 */
		LSE_INLINE LSREAL LSE_CALL				Shininess() const;

		/**
		 * Gets a constant reference to the diffuse texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the diffuse texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												DiffuseTexture() const;

		/**
		 * Gets a constant reference to the emissive texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the emissive texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												EmissiveTexture() const;

		/**
		 * Gets a constant reference to the transparency texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the transparency texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												TransparencyTexture() const;

		/**
		 * Gets a constant reference to the specular texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the specular texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												SpecularTexture() const;

		/**
		 * Gets a constant reference to the reflection texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the reflection texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												ReflectionTexture() const;

		/**
		 * Gets a constant reference to the shininess texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the shininess texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												ShininessTexture() const;

		/**
		 * Gets a constant reference to the normal-map.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the normal-map.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												NormalMapTexture() const;

		/**
		 * Gets a constant reference to the diffuse-factor texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the diffuse-factor texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												DiffuseFactorTexture() const;

		/**
		 * Gets a constant reference to the emissive-factor texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the emissive-factor texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												EmissiveFactorTexture() const;

		/**
		 * Gets a constant reference to the transparency-factor texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the transparency-factor texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												TransparencyFactorTexture() const;

		/**
		 * Gets a constant reference to the specular-factor texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the specular-factor texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												SpecularFactorTexture() const;

		/**
		 * Gets a constant reference to the reflection-factor texture.  If there is no texture, the shared pointer is invalid.
		 *
		 * \return Returns a constant reference to the reflection-factor texture.
		 */
		LSE_INLINE const CSharedTexture2dPtr & LSE_CALL
												ReflectionFactorTexture() const;

		/**
		 * Is this material alpha-enabled?
		 *
		 * \return Returns true if this material should have alpha-blending enabled.
		 */
		LSE_INLINE LSBOOL LSE_CALL				IsAlpha() const;

		/**
		 * Ashikhmin-Shirley X and Y factors.
		 *
		 * \return Returns a constant reference to the Ashikhmin-Shirely factors.
		 */
		LSE_INLINE const CVector2 & LSE_CALL	AshikminShirley() const;

		/**
		 * Roughness.
		 *
		 * \return Returns the roughness.
		 */
		LSE_INLINE LSREAL LSE_CALL				Roughness() const;

		/**
		 * Specular reflectance.
		 *
		 * \return Returns the specular reflectance.
		 */
		LSE_INLINE LSREAL LSE_CALL				SpecularReflectance() const;

		/**
		 * Diffuse texture matrix.
		 *
		 * \return Returns a constant reference to the diffuse texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		DiffuseTextureMatrix() const;

		/**
		 * Diffuse factor texture matrix.
		 *
		 * \return Returns a constant reference to the diffuse factor texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		DiffuseFactorTextureMatrix() const;

		/**
		 * Emissive texture matrix.
		 *
		 * \return Returns a constant reference to the emissive texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		EmissiveTextureMatrix() const;

		/**
		 * Emissive factor texture matrix.
		 *
		 * \return Returns a constant reference to the emissive factor texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		EmissiveFactorTextureMatrix() const;

		/**
		 * Transparency texture matrix.
		 *
		 * \return Returns a constant reference to the transparency texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		TransparencyTextureMatrix() const;

		/**
		 * Transparency factor texture matrix.
		 *
		 * \return Returns a constant reference to the transparency factor texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		TransparencyFactorTextureMatrix() const;

		/**
		 * Specular texture matrix.
		 *
		 * \return Returns a constant reference to the specular texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		SpecularTextureMatrix() const;

		/**
		 * Specular factor texture matrix.
		 *
		 * \return Returns a constant reference to the specular factor texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		SpecularFactorTextureMatrix() const;

		/**
		 * Reflection texture matrix.
		 *
		 * \return Returns a constant reference to the reflection texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		ReflectionTextureMatrix() const;

		/**
		 * Reflection factor texture matrix.
		 *
		 * \return Returns a constant reference to the reflection factor texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		ReflectionFactorTextureMatrix() const;

		/**
		 * Shininess texture matrix.
		 *
		 * \return Returns a constant reference to the diffuse shininess matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		ShininessTextureMatrix() const;

		/**
		 * Normal-map texture matrix.
		 *
		 * \return Returns a constant reference to the normal-map texture matrix.
		 */
		LSE_INLINE CMatrix4x4 & LSE_CALL		NormalMapTextureMatrix() const;

		/**
		 * Sets the cull mode.
		 *
		 * \param _bReversed If true, the cull mode will be reversed.
		 */
		LSE_INLINE void LSE_CALL				SetReverseCulling( LSBOOL _bReversed );

		/**
		 * Updates the rasterizer, depth/stencil, and blend states based on the material settings.
		 *
		 * \return Returns true if all states were updated.
		 */
		LSBOOL LSE_CALL							UpdateRenderStates() const;

		/**
		 * Gets a constant reference to the render states.
		 *
		 * \return Returns a constant reference to the render states.
		 */
		LSE_INLINE const LSD_RENDER_STATES & LSE_CALL
												RenderStates() const;

		/**
		 * Converts from a model-attribute texture wrap mode to a graphics-library texture-wrap mode.
		 *
		 * \param _maAttrib The model-library wrap mode to convert.
		 * \return Returns the graphics-library texture wrap mode associated with the input model-library
		 *	wrap mode.  In debug mode, an assertion on validity is made.
		 */
		static LSE_INLINE LSG_TEXTURE_ADDRESS_MODE LSE_FCALL
												GetGfxWrapMode( LSD_MODEL_ATTRIBUTES _maAttrib );


	protected :
		// == Members.
		/** Diffuse color and factor (.w). */
		CVector4								m_vDiffuse;

		/** Emissive color and factor (.w). */
		CVector4								m_vEmissive;

		/** Transparency color and factor (.w). */
		CVector4								m_vTransparency;

		/** Specular color and factor (.w). */
		CVector4								m_vSpecular;

		/** Reflection color and factor (.w). */
		CVector4								m_vReflection;

		/** Output diffuse color. */
		mutable CVector4						m_vDiffuseOut;

		/** Output emissive color. */
		mutable CVector4						m_vEmissiveOut;

		/** Output specular color. */
		mutable CVector4						m_vSpecularOut;

		/** Output relfection color. */
		mutable CVector4						m_vReflectionOut;

		/** Material name. */
		CString									m_sName;

		/** Material ID. */
		uint64_t								m_ui64Id;

		/** Specular power. */
		LSREAL									m_fShininess;

		/** Specular power output. */
		mutable LSREAL							m_fShininessOut;

		// Historical reasons why these are not just multiple arrays indexed by enumerations.
#define LSD_TEX_DECL( NAME )																\
		CSharedTexture2dPtr						m_st2p ## NAME;

		/** Diffuse texture. */
		LSD_TEX_DECL( Diffuse );

		/** Diffuse factor texture. */
		LSD_TEX_DECL( DiffuseFactor );

		/** Emissive texture. */
		LSD_TEX_DECL( Emissive );

		/** Emissive factor texture. */
		LSD_TEX_DECL( EmissiveFactor );

		/** Transparency texture. */
		LSD_TEX_DECL( Transparency );

		/** Transparency factor texture. */
		LSD_TEX_DECL( TransparencyFactor );

		/** Specular texture. */
		LSD_TEX_DECL( Specular );

		/** Specular factor texture. */
		LSD_TEX_DECL( SpecularFactor );

		/** Reflection texture. */
		LSD_TEX_DECL( Reflection );

		/** Reflection factor texture. */
		LSD_TEX_DECL( ReflectionFactor );

		/** Shininess texture. */
		LSD_TEX_DECL( Shininess );

		/** Normal-map. */
		LSD_TEX_DECL( NormalMap );

#undef LSD_TEX_DECL

		/** Material type. */
		uint16_t								m_ui16Type;

		/** Ashikhmin-Shirley factors. */
		CVector2								m_vAshikhminShirley;

		/** Roughness and specular reflectance. */
		CVector2								m_vRoughnessSpecReflect;

		/** Texture-matrix dirty flags. */
		mutable uint32_t						m_ui32MatrixDirty;

		/** Texture-matrix non-identify flags. */
		mutable uint32_t						m_ui32MatrixIdentity;

		/** Major blend states for opaque renders. */
		mutable LSD_RENDER_STATES				m_rsRenderStates;

		/** Flip culling or not. */
		LSBOOL									m_bFlipCulling;

		/** Texture settings. */
		LSD_TEXTURE_SETTINGS					m_tsTextureSettings[LSD_MMT_TOTAL];

		/** Texture matrices. */
		mutable CMatrix4x4						m_mMatrices[LSD_MMT_TOTAL];

		/** Sampler states. */
		CSampler								m_sSamplers[LSD_MMT_TOTAL];


		// == Functions.
		/**
		 * Updates the output diffuse.
		 */
		LSE_INLINE void LSE_CALL				UpdateDiffuse() const;

		/**
		 * Updates the output emissive.
		 */
		LSE_INLINE void LSE_CALL				UpdateEmissive() const;

		/**
		 * Updates the output specular.
		 */
		LSE_INLINE void LSE_CALL				UpdateSpecular() const;

		/**
		 * Updates the output reflection.
		 */
		LSE_INLINE void LSE_CALL				UpdateReflection() const;

		/**
		 * Updates all miscellanous factors.
		 */
		void LSE_CALL							UpdateMisc();

		/**
		 * Creates the texture matrix given texture settings.
		 *
		 * \param _tsSettings Texture settings to assemble into a texture matrix.
		 * \param _ui32BitFlag Bit for changing dirty and identity flags.
		 * \return Returns the created matrix.
		 */
		CMatrix4x4 LSE_CALL						CreateTextureMatrix( const LSD_TEXTURE_SETTINGS &_tsSettings, uint32_t _ui32BitFlag ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the material ID.
	 *
	 * \return Returns the material ID.
	 */
	LSE_INLINE uint64_t LSE_CALL CModelMaterial::Id() const {
		return m_ui64Id;
	}

	/**
	 * Gets the material type.
	 *
	 * \return Returns the material type.
	 */
	LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL CModelMaterial::Type() const {
		return static_cast<LSD_MODEL_ATTRIBUTES>(m_ui16Type);
	}

	/**
	 * Gets the diffuse component after scaling by its factor and applying a transparency modifier.
	 *
	 * \return Returns the diffuse component after scaling by its factor and applying a transparency modifier.
	 */
	LSE_INLINE const CVector4 & LSE_CALL CModelMaterial::Diffuse() const {
		return m_vDiffuseOut;
	}

	/**
	 * Gets the emissive component after scaling by its factor.
	 *
	 * \return Returns the emissive component after scaling by its factor.
	 */
	LSE_INLINE const CVector4 & LSE_CALL CModelMaterial::Emissive() const {
		return m_vEmissiveOut;
	}

	/**
	 * Gets the specular component after scaling by its factor.
	 *
	 * \return Returns the specular component after scaling by its factor.
	 */
	LSE_INLINE const CVector4 & LSE_CALL CModelMaterial::Specular() const {
		return m_vSpecularOut;
	}

	/**
	 * Gets the reflection component after scaling by its factor.
	 *
	 * \return Returns the reflection component after scaling by its factor.
	 */
	LSE_INLINE const CVector4 & LSE_CALL CModelMaterial::Reflection() const {
		return m_vReflectionOut;
	}

	/**
	 * Gets the specular power.
	 *
	 * \return Returns the specular power.
	 */
	LSE_INLINE LSREAL LSE_CALL CModelMaterial::Shininess() const {
		return m_fShininessOut;
	}

	/**
	 * Gets a constant reference to the diffuse texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the diffuse texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::DiffuseTexture() const {
		return m_st2pDiffuse;
	}

	/**
	 * Gets a constant reference to the emissive texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the emissive texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::EmissiveTexture() const {
		return m_st2pEmissive;
	}

	/**
	 * Gets a constant reference to the transparency texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the transparency texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::TransparencyTexture() const {
		return m_st2pTransparency;
	}

	/**
	 * Gets a constant reference to the specular texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the specular texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::SpecularTexture() const {
		return m_st2pSpecular;
	}

	/**
	 * Gets a constant reference to the reflection texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the reflection texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::ReflectionTexture() const {
		return m_st2pReflection;
	}

	/**
	 * Gets a constant reference to the shininess texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the shininess texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::ShininessTexture() const {
		return m_st2pShininess;
	}

	/**
	 * Gets a constant reference to the normal-map.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the normal-map.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::NormalMapTexture() const {
		return m_st2pNormalMap;
	}

	/**
	 * Gets a constant reference to the diffuse-factor texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the diffuse-factor texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::DiffuseFactorTexture() const {
		return m_st2pDiffuseFactor;
	}

	/**
	 * Gets a constant reference to the emissive-factor texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the emissive-factor texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::EmissiveFactorTexture() const {
		return m_st2pEmissiveFactor;
	}

	/**
	 * Gets a constant reference to the transparency-factor texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the transparency-factor texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::TransparencyFactorTexture() const {
		return m_st2pTransparencyFactor;
	}

	/**
	 * Gets a constant reference to the specular-factor texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the specular-factor texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::SpecularFactorTexture() const {
		return m_st2pSpecularFactor;
	}

	/**
	 * Gets a constant reference to the reflection-factor texture.  If there is no texture, the shared pointer is invalid.
	 *
	 * \return Returns a constant reference to the reflection-factor texture.
	 */
	LSE_INLINE const CSharedTexture2dPtr & LSE_CALL CModelMaterial::ReflectionFactorTexture() const {
		return m_st2pReflection;
	}

	/**
	 * Is this material alpha-enabled?
	 *
	 * \return Returns true if this material should have alpha-blending enabled.
	 */
	LSE_INLINE LSBOOL LSE_CALL CModelMaterial::IsAlpha() const {
		return Diffuse().w != 1.0f || TransparencyTexture().Valid();
	}

	/**
	 * Ashikhmin-Shirley X and Y factors.
	 *
	 * \return Returns a constant reference to the Ashikhmin-Shirely factors.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CModelMaterial::AshikminShirley() const {
		return m_vAshikhminShirley;
	}

	/**
	 * Roughness.
	 *
	 * \return Returns the roughness.
	 */
	LSE_INLINE LSREAL LSE_CALL CModelMaterial::Roughness() const {
		return m_vRoughnessSpecReflect.x;
	}

	/**
	 * Specular reflectance.
	 *
	 * \return Returns the specular reflectance.
	 */
	LSE_INLINE LSREAL LSE_CALL CModelMaterial::SpecularReflectance() const {
		return m_vRoughnessSpecReflect.y;
	}

	/**
	 * Diffuse texture matrix.
	 *
	 * \return Returns a constant reference to the diffuse texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::DiffuseTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_DIFFUSE) ) {
			m_mMatrices[LSD_MMT_DIFFUSE] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_DIFFUSE], LSD_MMT_DIFFUSE );
		}
		return m_mMatrices[LSD_MMT_DIFFUSE];
	}

	/**
	 * Diffuse factor texture matrix.
	 *
	 * \return Returns a constant reference to the diffuse factor texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::DiffuseFactorTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_DIFFUSE_FACTOR) ) {
			m_mMatrices[LSD_MMT_DIFFUSE_FACTOR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_DIFFUSE_FACTOR], LSD_MMT_DIFFUSE_FACTOR );
		}
		return m_mMatrices[LSD_MMT_DIFFUSE_FACTOR];
	}

	/**
	 * Emissive texture matrix.
	 *
	 * \return Returns a constant reference to the emissive texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::EmissiveTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_EMISSIVE) ) {
			m_mMatrices[LSD_MMT_EMISSIVE] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_EMISSIVE], LSD_MMT_EMISSIVE );
		}
		return m_mMatrices[LSD_MMT_EMISSIVE];
	}

	/**
	 * Emissive factor texture matrix.
	 *
	 * \return Returns a constant reference to the emissive factor texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::EmissiveFactorTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_EMISSIVE_FACTOR) ) {
			m_mMatrices[LSD_MMT_EMISSIVE_FACTOR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_EMISSIVE_FACTOR], LSD_MMT_EMISSIVE_FACTOR );
		}
		return m_mMatrices[LSD_MMT_EMISSIVE_FACTOR];
	}

	/**
	 * Transparency texture matrix.
	 *
	 * \return Returns a constant reference to the transparency texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::TransparencyTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_TRANSPARENCY) ) {
			m_mMatrices[LSD_MMT_TRANSPARENCY] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_TRANSPARENCY], LSD_MMT_TRANSPARENCY );
		}
		return m_mMatrices[LSD_MMT_TRANSPARENCY];
	}

	/**
	 * Transparency factor texture matrix.
	 *
	 * \return Returns a constant reference to the transparency factor texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::TransparencyFactorTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_TRANSPARENCY_FACTOR) ) {
			m_mMatrices[LSD_MMT_TRANSPARENCY_FACTOR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_TRANSPARENCY_FACTOR], LSD_MMT_TRANSPARENCY_FACTOR );
		}
		return m_mMatrices[LSD_MMT_TRANSPARENCY_FACTOR];
	}

	/**
	 * Specular texture matrix.
	 *
	 * \return Returns a constant reference to the specular texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::SpecularTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_SPECULAR) ) {
			m_mMatrices[LSD_MMT_SPECULAR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_SPECULAR], LSD_MMT_SPECULAR );
		}
		return m_mMatrices[LSD_MMT_SPECULAR];
	}

	/**
	 * Specular factor texture matrix.
	 *
	 * \return Returns a constant reference to the specular factor texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::SpecularFactorTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_SPECULAR_FACTOR) ) {
			m_mMatrices[LSD_MMT_SPECULAR_FACTOR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_SPECULAR_FACTOR], LSD_MMT_SPECULAR_FACTOR );
		}
		return m_mMatrices[LSD_MMT_SPECULAR_FACTOR];
	}

	/**
	 * Reflection texture matrix.
	 *
	 * \return Returns a constant reference to the reflection texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::ReflectionTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_REFLECTION) ) {
			m_mMatrices[LSD_MMT_REFLECTION] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_REFLECTION], LSD_MMT_REFLECTION );
		}
		return m_mMatrices[LSD_MMT_REFLECTION];
	}

	/**
	 * Reflection factor texture matrix.
	 *
	 * \return Returns a constant reference to the reflection factor texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::ReflectionFactorTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_REFLECTION_FACTOR) ) {
			m_mMatrices[LSD_MMT_REFLECTION_FACTOR] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_REFLECTION_FACTOR], LSD_MMT_REFLECTION_FACTOR );
		}
		return m_mMatrices[LSD_MMT_REFLECTION_FACTOR];
	}

	/**
	 * Shininess texture matrix.
	 *
	 * \return Returns a constant reference to the diffuse shininess matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::ShininessTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_SHININESS) ) {
			m_mMatrices[LSD_MMT_SHININESS] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_SHININESS], LSD_MMT_SHININESS );
		}
		return m_mMatrices[LSD_MMT_SHININESS];
	}

	/**
	 * Normal-map texture matrix.
	 *
	 * \return Returns a constant reference to the normal-map texture matrix.
	 */
	LSE_INLINE CMatrix4x4 & LSE_CALL CModelMaterial::NormalMapTextureMatrix() const {
		if ( m_ui32MatrixDirty & (1 << LSD_MMT_NORMAL_MAP) ) {
			m_mMatrices[LSD_MMT_NORMAL_MAP] = CreateTextureMatrix( m_tsTextureSettings[LSD_MMT_NORMAL_MAP], LSD_MMT_NORMAL_MAP );
		}
		return m_mMatrices[LSD_MMT_NORMAL_MAP];
	}

	/**
	 * Sets the cull mode.
	 *
	 * \param _bReversed If true, the cull mode will be reversed.
	 */
	LSE_INLINE void LSE_CALL CModelMaterial::SetReverseCulling( LSBOOL _bReversed ) {
		m_bFlipCulling = _bReversed;
	}

	/**
	 * Gets a constant reference to the render states.
	 *
	 * \return Returns a constant reference to the render states.
	 */
	LSE_INLINE const CModelMaterial::LSD_RENDER_STATES & LSE_CALL CModelMaterial::RenderStates() const {
		 return m_rsRenderStates;
	}

	/**
	 * Converts from a model-attribute texture wrap mode to a graphics-library texture-wrap mode.
	 *
	 * \param _maAttrib The model-library wrap mode to convert.
	 * \return Returns the graphics-library texture wrap mode associated with the input model-library
	 *	wrap mode.  In debug mode, an assertion on validity is made.
	 */
	LSE_INLINE LSG_TEXTURE_ADDRESS_MODE LSE_FCALL CModelMaterial::GetGfxWrapMode( LSD_MODEL_ATTRIBUTES _maAttrib ) {
		switch ( _maAttrib ) {
			case LSD_MA_WRAP_CLAMP : {
				return LSG_TAM_CLAMP;
			}
			case LSD_MA_WRAP_MIRROR : {
				return LSG_TAM_MIRROR;
			}
			case LSD_MA_WRAP_REPEAT : {
				return LSG_TAM_WRAP;
			}
			default : {
				assert( 0 );
				return LSG_TAM_WRAP;
			}
		}
	}

	/**
	 * Updates the output diffuse.
	 */
	LSE_INLINE void LSE_CALL CModelMaterial::UpdateDiffuse() const {
		LSREAL fAlpha = ((m_vTransparency.x * LSI_R_WEIGHT) +
			(m_vTransparency.y * LSI_G_WEIGHT) +
			(m_vTransparency.z * LSI_B_WEIGHT)) * m_vTransparency.w;
		m_vDiffuseOut = CGfx::ColorToLinear( CVector4( m_vDiffuse.x * m_vDiffuse.w,
			m_vDiffuse.y * m_vDiffuse.w,
			m_vDiffuse.z * m_vDiffuse.w,
			CStd::Clamp( LSM_ONE - fAlpha, LSM_ZERO, LSM_ONE ) ) );
		// Premultiply alpha.
		m_vDiffuseOut.x *= m_vDiffuseOut.w;
		m_vDiffuseOut.y *= m_vDiffuseOut.w;
		m_vDiffuseOut.z *= m_vDiffuseOut.w;
	}

	/**
	 * Updates the output emissive.
	 */
	LSE_INLINE void LSE_CALL CModelMaterial::UpdateEmissive() const {
		m_vEmissiveOut = CGfx::ColorToLinear( CVector4( m_vEmissive.x * m_vEmissive.w,
			m_vEmissive.y * m_vEmissive.w,
			m_vEmissive.z * m_vEmissive.w,
			LSM_ONE ) );
	}

	/**
	 * Updates the output specular.
	 */
	LSE_INLINE void LSE_CALL CModelMaterial::UpdateSpecular() const {
		m_vSpecularOut = CGfx::ColorToLinear( CVector4( m_vSpecular.x * m_vSpecular.w,
			m_vSpecular.y * m_vSpecular.w,
			m_vSpecular.z * m_vSpecular.w,
			LSM_ONE ) );
	}

	/**
	 * Updates the output reflection.
	 */
	LSE_INLINE void LSE_CALL CModelMaterial::UpdateReflection() const {
		m_vReflectionOut = CVector4( m_vReflection.x * m_vReflection.w,
			m_vReflection.y * m_vReflection.w,
			m_vReflection.z * m_vReflection.w,
			m_vReflection.w );
	}

}	// namespace lsd

#pragma warning( pop )

#endif	// __LSD_MODELMATERIAL_H__
