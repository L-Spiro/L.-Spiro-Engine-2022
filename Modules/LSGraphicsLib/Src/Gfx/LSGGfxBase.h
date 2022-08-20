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
 * Description: The base class for the primary interface to the underlying graphics API.
 */


#ifndef __LSG_GFXBASE_H__
#define __LSG_GFXBASE_H__

#include "../LSGGraphicsLib.h"
#include "Frustum/LSMFrustum.h"
#include "Matrix/LSMMatrix3x3.h"
#include "Matrix/LSMMatrix4x4.h"
#include "MemOps/LSTLMemCmpF.h"
#include "Vector/LSMVector2.h"
#include "Vector/LSMVector3.h"
#include "Vector/LSMVector4.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// MACROS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define LSG_FILTER_POINT							1												/**< Point filter. */
#define LSG_FILTER_LINEAR							2												/**< Linear filter. */
#define	LSG_FILTER_TYPE_MASK						(0x3)											/**< Filter mask. */
#define	LSG_MIN_FILTER_SHIFT						(4)												/**< Min-filter shift. */
#define	LSG_MAG_FILTER_SHIFT						(2)												/**< Mag-fiter shift. */
#define	LSG_MIP_FILTER_SHIFT						(0)												/**< Min-filter shift. */
#define	LSG_COMPARISON_FILTERING_BIT				(0x80)											/**< Comparison bit. */
#define	LSG_ANISOTROPIC_FILTERING_BIT				(0x40)											/**< Anisotropic bit. */
#define LSG_ENC_BASIC_FILTER( MIN, MAG, MIP, COMPARISON )				\
	((COMPARISON) ? LSG_COMPARISON_FILTERING_BIT : 0) |					\
	(((MIN) & LSG_FILTER_TYPE_MASK) << LSG_MIN_FILTER_SHIFT) |			\
	(((MAG) & LSG_FILTER_TYPE_MASK) << LSG_MAG_FILTER_SHIFT) |			\
	(((MIP) & LSG_FILTER_TYPE_MASK) << LSG_MIP_FILTER_SHIFT)										/**< Encodes a basic (no anisotropy) filter. */
#define LSG_ENCODE_ANISOTROPIC_FILTER( COMPARISON )						\
	LSG_ANISOTROPIC_FILTERING_BIT |										\
	LSG_ENC_BASIC_FILTER( LSG_FILTER_LINEAR,							\
		LSG_FILTER_LINEAR,												\
		LSG_FILTER_LINEAR,												\
		COMPARISON )																				/**< Encodes an anisotropic filter. */
#define LSG_DECODE_MIN_FILTER( FILTER )									\
	(((FILTER) >> LSG_MIN_FILTER_SHIFT) & LSG_FILTER_TYPE_MASK)
#define LSG_DECODE_MAG_FILTER( FILTER )									\
	(((FILTER) >> LSG_MAG_FILTER_SHIFT) & LSG_FILTER_TYPE_MASK)
#define LSG_DECODE_MIP_FILTER( FILTER )									\
	(((FILTER) >> LSG_MIP_FILTER_SHIFT) & LSG_FILTER_TYPE_MASK) 
#define LSG_DECODE_IS_COMPARISON_FILTER( FILTER )						\
	((FILTER) & LSG_COMPARISON_FILTERING_BIT)							
#define LSG_DECODE_IS_ANISOTROPIC_FILTER( FILTER )						\
	(((FILTER) & LSG_ANISOTROPIC_FILTERING_BIT) &&						\
	(LSG_FILTER_LINEAR == LSG_DECODE_MIN_FILTER( FILTER )) &&			\
	(LSG_FILTER_LINEAR == LSG_DECODE_MAG_FILTER( FILTER )) &&			\
	(LSG_FILTER_LINEAR == LSG_DECODE_MIP_FILTER( FILTER )))

	// == Enumerations.
	/** Type of fog.  Linear, exponential, or exponential squared. */
	enum LSG_FOG_TYPE {
		LSG_FT_LINEAR,																				/**< Linear fall-off. */
		LSG_FT_EXP,																					/**< Exponential fall-off. */
		LSG_FT_EXP2,																				/**< Squared exponential fall-off. */
	};

	/** Base metrics. */
	enum LSG_BASE_METRICS {
		LSG_MAX_LIGHTS											= 8,								/**< Maximum number of simultaneous lights in a shader. */
	};

	/**
	 * Class CGfxBase
	 * \brief The base class for the primary interface to the underlying graphics API.
	 *
	 * Description: The base class for the primary interface to the underlying graphics API.
	 */
	class CGfxBase {
	public :
		/**
		 * Sets the world matrix.
		 *
		 * \param _mMat the world matrix to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetWorldMatrix( const CMatrix4x4 &_mMat );

		/**
		 * Gets the world matrix.
		 *
		 * \return Returns a constant reference to the world matrix.
		 */
		static LSE_INLINE const CMatrix4x4 & LSE_FCALL			GetWorldMatrix();

		/**
		 * Sets the view matrix.
		 *
		 * \param _mMat the view matrix to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetViewMatrix( const CMatrix4x4 &_mMat );

		/**
		 * Gets the view matrix.
		 *
		 * \return Returns a constant reference to the view matrix.
		 */
		static LSE_INLINE const CMatrix4x4 & LSE_FCALL			GetViewMatrix();

		/**
		 * Sets the projection matrix.
		 *
		 * \param _mMat the projection matrix to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetProjMatrix( const CMatrix4x4 &_mMat );

		/**
		 * Gets the projection matrix.
		 *
		 * \return Returns a constant reference to the projection matrix.
		 */
		static LSE_INLINE const CMatrix4x4 & LSE_FCALL			GetProjMatrix();

		/**
		 * Sets the camera position.
		 *
		 * \param _vPos The camera position in world space.
		 */
		static LSE_INLINE void LSE_CALL							SetCamPos( const CVector3 &_vPos );

		/**
		 * Sets the diffuse material value.
		 *
		 * \param _vColor The diffuse color to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetDiffuseMaterial( const CVector4 &_vColor );

		/**
		 * Sets the emissive material value.
		 *
		 * \param _vColor The emissive color to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetEmissiveMaterial( const CVector4 &_vColor );

		/**
		 * Sets the specular material value.
		 *
		 * \param _vColor The specular color to set.
		 */
		static LSE_INLINE void LSE_FCALL						SetSpecularMaterial( const CVector4 &_vColor );

		/**
		 * Sets the material power value.  Must be between 0 and 128.
		 *
		 * \param _fPower The new material specular power.
		 */
		static LSE_INLINE void LSE_FCALL						SetMaterialPower( LSREAL _fPower );

		/**
		 * Sets the material roughness and specular reflectivity.
		 *
		 * \param _fRoughness The new material roughness.
		 * \param _fSpecularReflectivity The new specular reflectivity.
		 */
		static LSE_INLINE void LSE_FCALL						SetMaterialRoughness( LSREAL _fRoughness, LSREAL _fSpecularReflectivity );

		/**
		 * Sets the material reflectivity value.
		 *
		 * \param _fPower The new reflectivity value.
		 */
		//static LSE_INLINE void LSE_FCALL						SetReflectivity( LSREAL _fReflectivity );

		/**
		 * Sets the Ashikhman-Shirely Nu and Nv values (0-10,000).
		 *
		 * \param _fNu The Nu parameter for the Ashikhman-Shirley shading model.
		 * \param _fNv The Nv parameter for the Ashikhman-Shirley shading model.
		 */
		static LSE_INLINE void LSE_FCALL						SetAshikhmanShirley( LSREAL _fNu, LSREAL _fNv );

		/**
		 * Sets a directional light's direction and color.
		 *
		 * \param _ui32Slot Index of the light data to set.  Must be smaller than LSG_MAX_LIGHTS.
		 * \param _vDir Direction of the light.
		 * \param _vColor Color of the light.
		 */
		static LSE_INLINE void LSE_FCALL						SetDirLight( uint32_t _ui32Slot, const CVector4 &_vDir, const CVector4 &_vColor );

		/**
		 * Sets the total directional lights active.  Must be less than or equal to LSG_MAX_LIGHTS.
		 *
		 * \param _ui32Total The total active directional lights.
		 */
		static LSE_INLINE void LSE_FCALL						SetDirLightTotal( uint32_t _ui32Total );

		/**
		 * Gets the total directional lights active.
		 *
		 * \return Returns the total directional lights active.
		 */
		static LSE_INLINE uint32_t LSE_CALL						GetDirLightTotal();

		/**
		 * Sets a point light's position, color, and size.
		 *
		 * \param _ui32Slot Index of the light data to set.  Must be smaller than LSG_MAX_LIGHTS.
		 * \param _vPos Position of the point light.
		 * \param _vColor Color of the light.
		 * \param _fSize Size of the point light.
		 */
		static LSE_INLINE void LSE_FCALL						SetPointLight( uint32_t _ui32Slot, const CVector4 &_vPos, const CVector4 &_vColor,
			LSREAL _fSize );

		/**
		 * Sets the total point lights active.  Must be less than or equal to LSG_MAX_LIGHTS.
		 *
		 * \param _ui32Total The total active point lights.
		 */
		static LSE_INLINE void LSE_FCALL						SetPointLightTotal( uint32_t _ui32Total );

		/**
		 * Gets the total point lights active.
		 *
		 * \return Returns the total point lights active.
		 */
		static LSE_INLINE uint32_t LSE_CALL						GetPointLightTotal();

		/**
		 * Converts a color from sRGB to linear space.
		 *
		 * \param _vColor Color to convert.
		 * \return Returns the converted color.
		 */
		static LSE_INLINE CVector4 LSE_FCALL					ColorToLinear( const CVector4 &_vColor );
		
		/**
		 * Sets whether we are active or not.  If not, no graphics commands can be
		 *	issued.
		 *
		 * \param _bActive Sets the graphics library to active or not.
		 */
		static LSE_INLINE void LSE_CALL							SetActive( LSBOOL _bActive );


	protected :
		// == Types.
		/**
		 * System metrics.
		 */
		typedef struct LSG_METRICS {
			/** Maximum render target size. */
			uint32_t											ui32MaxRenderTargetSize;

			/** Maximum render targets. */
			uint32_t											ui32MaxRenderTargets;

			/** Maximum texture width. */
			uint32_t											ui32MaxTextureWidth;

			/** Maximum texture height. */
			uint32_t											ui32MaxTextureHeight;

			/** Maximum number of texture layers. */
			uint32_t											ui32MaxTextureArray;

			/** Maximum anisotropic value. */
			uint32_t											ui32MaxAniso;

			/** Maximum texture slots. */
			uint32_t											ui32MaxTexSlot;

			/** Maximum elements in a vertex buffer. */
			uint32_t											ui32MaxVertElements;

			/** Maximum number of primitives in a draw call. */
			uint32_t											ui32MaxPrimitives;

			/** Support for non-power-of-2 textures. */
			LSBOOL												bNonPo2;

			/** Support for DDS files. */
			LSBOOL												bDdsSupport;

			/** Support for compact depth/stencil buffers. */
			LSBOOL												bPackedDepthStencil;

			/** Support for directly reading a depth buffer as a texture. */
			LSBOOL												bDepthIsReadable;

		} * LPLSG_METRICS, * const LPCLSG_METRICS;


		// == Members.
		/** System metrics. */
		static LSG_METRICS										m_mMetrics;

		/** The world matrix. */
		static CMatrix4x4										m_mWorldMatrix;

		/** The view matrix. */
		static CMatrix4x4										m_mViewMatrix;

		/** The projection matrix. */
		static CMatrix4x4										m_mProjMatrix;

		/** The world-view matrix.  Updated only before rendering, and only if the world or view matrix have
		 *	changed. */
		static CMatrix4x4										m_mWorldViewMatrix;

		/** The world-view-projection matrix.  Updated only before rendering, and only if the world, view, or
		 *	projection matrices have changed. */
		static CMatrix4x4										m_mWorldViewProjMatrix;

		/** The normal matrix.  It transforms normals to screen space. */
		static CMatrix3x3										m_mNormalMatrix;

		/** The inverse world-view matrix.  Updated only before rendering, and only if the world or view matrix have
		 *	changed. */
		static CMatrix4x4										m_mInverseWorldViewMatrix;

		/** Has the world matrix been modified? */
		static LSBOOL											m_bWorldDirty;

		/** Has the view matrix been modified? */
		static LSBOOL											m_bViewDirty;

		/** Has the projection matrix been modified? */
		static LSBOOL											m_bProjDirty;

		/** Camera position in world space. */
		static CVector3											m_vCamPos;

		/** Screen width. */
		static uint32_t											m_ui32ScreenWidth;

		/** Screen height. */
		static uint32_t											m_ui32ScreenHeight;

		/** Diffuse material color. */
		static CVector4											m_vDiffuseMaterial;

		/** Emissive material color. */
		static CVector4											m_vEmissiveMaterial;

		/** Specular material color. */
		static CVector4											m_vSpecularMaterial;

		/** Specular material power. */
		static LSREAL											m_fPower;

		/** Material roughness (x) and specular reflectance (y). */
		static CVector4											m_vRoughness;

		/** Reflectivity. */
		//static LSREAL											m_fReflectivity;

		/** Ashikhman-Shirley factors (Nu, Nv, Factor, Rs). */
		static CVector4											m_vAshikFactors;

		/** Directional light direction. */
		static CVector4											m_vDirLightDir[LSG_MAX_LIGHTS];

		/** Directional light color. */
		static CVector4											m_vDirLightColor[LSG_MAX_LIGHTS];

		/** Total directional lights set. */
		static uint32_t											m_ui32TotalDirLights;

		/** Point light positions. */
		static CVector4											m_vPointLightPos[LSG_MAX_LIGHTS];

		/** Point light colors. */
		static CVector4											m_vPointLightColor[LSG_MAX_LIGHTS];

		/** Point light sizes. */
		static float											m_fPointLightSize[LSG_MAX_LIGHTS];

		/** Total point lights set. */
		static uint32_t											m_ui32TotalPointLights;
		
		/** Are we active? */
		static LSBOOL											m_bActive;


		// == Functions.
		/**
		 * Sets default values on all matrix-related members.
		 */
		static void LSE_CALL									SetDefaultMatrices();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets the world matrix.
	 *
	 * \param _mMat the world matrix to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetWorldMatrix( const CMatrix4x4 &_mMat ) {
		m_mWorldMatrix = _mMat;
		m_bWorldDirty = true;
	}

	/**
	 * Gets the world matrix.
	 *
	 * \return Returns a constant reference to the world matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_FCALL CGfxBase::GetWorldMatrix() {
		return m_mWorldMatrix;
	}

	/**
	 * Sets the view matrix.
	 *
	 * \param _mMat the view matrix to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetViewMatrix( const CMatrix4x4 &_mMat ) {
		m_mViewMatrix = _mMat;
		m_bViewDirty = true;
	}

	/**
	 * Gets the view matrix.
	 *
	 * \return Returns a constant reference to the view matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_FCALL CGfxBase::GetViewMatrix() {
		return m_mViewMatrix;
	}

	/**
	 * Sets the projection matrix.
	 *
	 * \param _mMat the projection matrix to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetProjMatrix( const CMatrix4x4 &_mMat ) {
		m_mProjMatrix = _mMat;
		m_bProjDirty = true;
	}

	/**
	 * Gets the projection matrix.
	 *
	 * \return Returns a constant reference to the projection matrix.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_FCALL CGfxBase::GetProjMatrix() {
		return m_mProjMatrix;
	}

	/**
	 * Sets the camera position.
	 *
	 * \param _vPos The camera position in world space.
	 */
	LSE_INLINE void LSE_CALL CGfxBase::SetCamPos( const CVector3 &_vPos ) {
		m_vCamPos = _vPos;
	}

	/**
	 * Sets the diffuse material value.
	 *
	 * \param _vColor The diffuse color to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetDiffuseMaterial( const CVector4 &_vColor ) {
		m_vDiffuseMaterial = _vColor;
	}

	/**
	 * Sets the emissive material value.
	 *
	 * \param _vColor The emissive color to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetEmissiveMaterial( const CVector4 &_vColor ) {
		m_vEmissiveMaterial = _vColor;
	}

	/**
	 * Sets the specular material value.
	 *
	 * \param _vColor The specular color to set.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetSpecularMaterial( const CVector4 &_vColor ) {
		m_vSpecularMaterial = _vColor;
	}

	/**
	 * Sets the material power value.  Must be between 0 and 128.
	 *
	 * \param _fPower The new material specular power.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetMaterialPower( LSREAL _fPower ) {
		m_fPower = _fPower;
	}

	/**
	 * Sets the material roughness and specular reflectivity.
	 *
	 * \param _fRoughness The new material roughness.
	 * \param _fSpecularReflectivity The new specular reflectivity.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetMaterialRoughness( LSREAL _fRoughness, LSREAL _fSpecularReflectivity ) {
		m_vRoughness.x = _fRoughness;
		m_vRoughness.y = _fSpecularReflectivity;
		m_vRoughness.z = m_vRoughness.x * m_vRoughness.x;
		m_vRoughness.w = LSM_ZERO;
	}

	/**
	 * Sets the Ashikhman-Shirely Nu and Nv values (0-10,000).
	 *
	 * \param _fNu The Nu parameter for the Ashikhman-Shirley shading model.
	 * \param _fNv The Nv parameter for the Ashikhman-Shirley shading model.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetAshikhmanShirley( LSREAL _fNu, LSREAL _fNv ) {
		m_vAshikFactors.x = _fNu;
		m_vAshikFactors.y = _fNv;
		m_vAshikFactors.z = CMathLib::Sqrt( (_fNu + LSM_ONE) * (_fNv + LSM_ONE) );
		m_vAshikFactors.w = static_cast<LSREAL>(0.2);
	}

	/**
	 * Sets a directional light's direction and color.
	 *
	 * \param _ui32Slot Index of the light data to set.  Must be smaller than LSG_MAX_LIGHTS.
	 * \param _vDir Direction of the light.
	 * \param _vColor Color of the light.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetDirLight( uint32_t _ui32Slot, const CVector4 &_vDir, const CVector4 &_vColor ) {
		assert( _ui32Slot < LSG_MAX_LIGHTS );
		m_vDirLightDir[_ui32Slot] = -_vDir;
		m_vDirLightDir[_ui32Slot].w = LSM_ZERO;
		m_vDirLightDir[_ui32Slot].Normalize();
		m_vDirLightColor[_ui32Slot] = _vColor;
		m_vDirLightColor[_ui32Slot].w = LSM_ZERO;
	}

	/**
	 * Sets the total directional lights active.  Must be less than or equal to LSG_MAX_LIGHTS.
	 *
	 * \param _ui32Total The total active directional lights.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetDirLightTotal( uint32_t _ui32Total ) {
		assert( _ui32Total <= LSG_MAX_LIGHTS );
		m_ui32TotalDirLights = _ui32Total;
	}

	/**
	 * Gets the total directional lights active.
	 *
	 * \return Returns the total directional lights active.
	 */
	LSE_INLINE uint32_t LSE_CALL CGfxBase::GetDirLightTotal() {
		return m_ui32TotalDirLights;
	}

	/**
	 * Sets a point light's position, color, and size.
	 *
	 * \param _ui32Slot Index of the light data to set.  Must be smaller than LSG_MAX_LIGHTS.
	 * \param _vPos Position of the point light.
	 * \param _vColor Color of the light.
	 * \param _fSize Size of the point light.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetPointLight( uint32_t _ui32Slot, const CVector4 &_vPos, const CVector4 &_vColor,
		LSREAL _fSize ) {
		assert( _ui32Slot <= LSG_MAX_LIGHTS );
		m_vPointLightPos[_ui32Slot] = _vPos;
		m_vPointLightColor[_ui32Slot] = _vColor;
		m_fPointLightSize[_ui32Slot] = CStd::Max( float( _fSize ), LSM_FLT_EPSILON * 2.0f );
	}

	/**
	 * Sets the total point lights active.  Must be less than or equal to LSG_MAX_LIGHTS.
	 *
	 * \param _ui32Total The total active point lights.
	 */
	LSE_INLINE void LSE_FCALL CGfxBase::SetPointLightTotal( uint32_t _ui32Total ) {
		assert( _ui32Total <= LSG_MAX_LIGHTS );
		m_ui32TotalPointLights = _ui32Total;
	}

	/**
	 * Gets the total point lights active.
	 *
	 * \return Returns the total point lights active.
	 */
	LSE_INLINE uint32_t LSE_CALL CGfxBase::GetPointLightTotal() {
		return m_ui32TotalPointLights;
	}

	/**
	 * Sets the material reflectivity value.
	 *
	 * \param _fPower The new reflectivity value.
	 */
	/*LSE_INLINE void LSE_FCALL CGfxBase::SetReflectivity( LSREAL _fReflectivity ) {
		m_fReflectivity = _fReflectivity;
	}*/

	/**
	 * Converts a color from sRGB to linear space.
	 *
	 * \param _vColor Color to convert.
	 * \return Returns the converted color.
	 */
	LSE_INLINE CVector4 LSE_FCALL CGfxBase::ColorToLinear( const CVector4 &_vColor ) {
		CVector4 vTemp;
		vTemp.x = _vColor.x <= LSREAL( 0.04045 ) ?
			_vColor.x * LSREAL( 1.0 / 12.92 ) :
			CMathLib::Pow( (_vColor.x + LSREAL( 0.055 )) * LSREAL( 1.0 / 1.055 ), LSREAL( 2.4 ) );
		vTemp.y = _vColor.y <= LSREAL( 0.04045 ) ?
			_vColor.y * LSREAL( 1.0 / 12.92 ) :
			CMathLib::Pow( (_vColor.y + LSREAL( 0.055 )) * LSREAL( 1.0 / 1.055 ), LSREAL( 2.4 ) );
		vTemp.z = _vColor.z <= LSREAL( 0.04045 ) ?
			_vColor.z * LSREAL( 1.0 / 12.92 ) :
			CMathLib::Pow( (_vColor.z + LSREAL( 0.055 )) * LSREAL( 1.0 / 1.055 ), LSREAL( 2.4 ) );
		vTemp.w = _vColor.w;
		return vTemp;
	}
	
	/**
	 * Sets whether we are active or not.  If not, no graphics commands can be
	 *	issued.
	 *
	 * \param _bActive Sets the graphics library to active or not.
	 */
	LSE_INLINE void LSE_CALL CGfxBase::SetActive( LSBOOL _bActive ) {
		m_bActive = _bActive;
	}

}	// namespace lsg

#endif	// __LSG_GFXBASE_H__
