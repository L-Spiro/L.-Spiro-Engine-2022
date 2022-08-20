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
 * Description: A shader for the graphics engine.  This is the object into which the source code is fed and
 *	 a compiled runnable shader is output.
 */


#ifndef __LSG_SHADER_H__
#define __LSG_SHADER_H__

#include "../LSGGraphicsLib.h"
#ifdef LSG_DX9
#include "../DirectX9Shaders/LSGDirectX9Shader.h"
#elif defined( LSG_DX11 )
#include "../DirectX11Shaders/LSGDirectX11Shader.h"
#endif	// #ifdef LSG_DX9
#include "../Gfx/LSGGfx.h"
#ifdef LSG_OGLES
#include "../OpenGLEsShaders/LSGOpenGlEsShader.h"
#elif defined( LSG_OGL )
#include "../OpenGLShaders/LSGOpenGlShader.h"
#elif defined( LSG_METAL )
#include "../MetalShaders/LSGMetalShader.h"
#elif defined( LSG_VKN )
#include "../VulkanShaders/LSGVulkanShader.h"
#endif	// #ifdef LSG_GLES
#include "SharedPtr/LSTLSharedPtr.h"

namespace lsg {

	/**
	 * Class CShader
	 * \brief A shader for the graphics engine.
	 *
	 * Description: A shader for the graphics engine.  This is the object into which the source code is fed and
	 *	 a compiled runnable shader is output.
	 */
	class CShader : public 
#ifdef LSG_DX9
		CDirectX9Shader
#endif	// #ifdef LSG_DX9
#ifdef LSG_DX11
		CDirectX11Shader
#endif	// #ifdef LSG_DX11
#ifdef LSG_OGL
		COpenGlShader
#endif	// #ifdef LSG_OGL
#ifdef LSG_OGLES
		COpenGlEsShader
#endif	// #ifdef LSG_OGLES
#ifdef LSG_METAL
		CMetalShader
#endif	// #ifdef LSG_METAL
#ifdef LSG_VKN
		CVulkanShader
#endif	// #ifdef LSG_VKN
	{
		friend class							CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CShader();
		virtual LSE_CALLCTOR					~CShader();


		// == Functions.
		/**
		 * Reset everything.
		 */
		void LSE_CALL							Reset();
		
		/**
		 * Post compile.  If all shaders are compiled successfully, this is called.  We use it to get handles to
		 *	engine-specific uniforms.
		 *
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		virtual LSG_SHADER_ERRORS LSE_CALL		PostCompile( CString &_sError );

		/**
		 * Activation callback.  This class allows its base class to activate the shader and then applies
		 *	engine-specific values to the shader once it is applied to the hardware.
		 *
		 * \param _pvSysValue A value with special system meaning.
		 * \return Returns false if there is a failure to apply the shader via the current graphics API.
		 */
		virtual LSBOOL LSE_CALL					Activate( void * _pvSysValue );


	protected :
		// == Types.
		/** A structure that simply holds engine-specific uniform handles. */
		typedef struct LSG_ENGINE_UNIFORMS {
			/** World matrix. */
			LSG_HANDLE							hWorldMatrix;

			/** World-view matrix. */
			LSG_HANDLE							hWorldViewMatrix;

			/** World-view-projection matrix. */
			LSG_HANDLE							hWorldViewProjMatrix;

			/** View matrix. */
			LSG_HANDLE							hViewMatrix;

			/** Camera position. */
			LSG_HANDLE							hCamPos;

			/** Time since the last render. */
			LSG_HANDLE							hTimeSinceLastFrame;

			/** Normal matrix. */
			LSG_HANDLE							hNormalMatrix;

			/** Number of lights. */
			LSG_HANDLE							hTotalLights;

			/** Number of directional lights. */
			LSG_HANDLE							hDirLights;

			/** Number of point lights. */
			LSG_HANDLE							hPointLights;

			/** Number of spot lights. */
			LSG_HANDLE							hSpotLights;

			/** Directional light vectors. */
			LSG_HANDLE							hDirLightDirs;

			/** Directional light colors. */
			LSG_HANDLE							hDirLightColors;

			/** Point light positions. */
			LSG_HANDLE							hPointLightPos;

			/** Point light colors. */
			LSG_HANDLE							hPointLightColors;

			/** Point light sizes. */
			LSG_HANDLE							hPointLightSizes;

			/** Spot light directional vectors. */
			LSG_HANDLE							hSpotLightVectors;

			/** Point- and spot- light attenuations. */
			LSG_HANDLE							hAttenuations;

			/** Spot-light range and cut-off. */
			LSG_HANDLE							hSpotLight;

			/** Light ambient. */
			LSG_HANDLE							hLightAmbients;

			/** Light diffuse. */
			LSG_HANDLE							hLightDiffuses;

			/** Light speculars. */
			LSG_HANDLE							hLightSpeculars;

			/** Ambient light. */
			LSG_HANDLE							hAmbientLight;

			/** Sky color. */
			LSG_HANDLE							hSkyColor;

			/** Ground color. */
			LSG_HANDLE							hGroundColor;

			/** Diffuse material color. */
			LSG_HANDLE							hDiffuseMaterial;

			/** Ambient material color. */
			LSG_HANDLE							hAmbientMaterial;

			/** Emissive material color. */
			LSG_HANDLE							hEmissiveMaterial;

			/** Specular material color. */
			LSG_HANDLE							hSpecularMaterial;

			/** Specular power. */
			LSG_HANDLE							hPower;

			/** Reflectivity. */
			LSG_HANDLE							hReflectivity;

			/** Roughness. */
			LSG_HANDLE							hRoughness;

			/** Ashikhmin-Shirley factor. */
			LSG_HANDLE							hAshikFactor;

			/** 1D textures. */
			LSG_HANDLE							hTexUnits1D[LSG_MAX_TEXTURE_UNITS];
			
			/** 2D textures. */
			LSG_HANDLE							hTexUnits2D[LSG_MAX_TEXTURE_UNITS];
			
			/** 3D textures. */
			LSG_HANDLE							hTexUnits3D[LSG_MAX_TEXTURE_UNITS];
			
			/** Cube textures. */
			LSG_HANDLE							hTexUnitsCube[LSG_MAX_TEXTURE_UNITS];

			/** Texture matrices. */
			LSG_HANDLE							hTexMatrices;

			/** Fog start. */
			LSG_HANDLE							hFogStart;

			/** Fog end. */
			LSG_HANDLE							hFogEnd;

			/** Fog color. */
			LSG_HANDLE							hFogColor;

			/** Shadow-mapping enabled? */
			LSG_HANDLE							hShadowMapping;

			/** In which unit is the shadow map? */
			LSG_HANDLE							hShadowMapUnit;

			/** The shadow map matrix. */
			LSG_HANDLE							hShadowMapMatrix;

			/** The shadow map U, V, and depth scales. */
			LSG_HANDLE							hShadowMapUVDepthScales;

			/** The shadow map caster size. */
			LSG_HANDLE							hShadowMapCasterSize;

			/** The shadow map texture dimensions. */
			LSG_HANDLE							hShadowMapTextureSize;
		} * LPLSG_ENGINE_UNIFORMS, * const LPCLSG_ENGINE_UNIFORMS;


		// == Members.
		/** Handles. */
		LSG_ENGINE_UNIFORMS						m_euEngineHandles;

		/** The first time we are activated we need to set the samplers. */
		LSBOOL									m_bBeenActivated;


	private :
#ifdef LSG_OGL
		typedef COpenGlShader					Parent;
#endif	// #ifdef LSG_OGL
#ifdef LSG_OGLES
		typedef COpenGlEsShader					Parent;
#endif	// #ifdef LSG_OGLES
#ifdef LSG_DX9
		typedef CDirectX9Shader					Parent;
#endif	// #ifdef LSG_DX9
#ifdef LSG_DX11
		typedef CDirectX11Shader				Parent;
#endif	// #ifdef LSG_DX11
#ifdef LSG_METAL
		typedef CMetalShader					Parent;
#endif	// #ifdef LSG_METAL
#ifdef LSG_VKN
		typedef CVulkanShader					Parent;
#endif	// #ifdef LSG_VKN
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to a standard texture.
	 */
	typedef CSharedPtr<CShader>					CSharedShaderPtr;

}	// namespace lsg

#endif	// __LSG_SHADER_H__
