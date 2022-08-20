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

#include "LSGShader.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShader::CShader() :
		m_bBeenActivated( false ) {
	}
	LSE_CALLCTOR CShader::~CShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.
	 */
	void LSE_CALL CShader::Reset() {
		m_bBeenActivated = false;
		Parent::Reset();
	}

	/**
	 * Post compile.  If all shaders are compiled successfully, this is called.  We use it to get handles to
	 *	engine-specific uniforms.
	 *
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CShader::PostCompile( CString &_sError ) {
		LSG_SHADER_ERRORS seError = Parent::PostCompile( _sError );
		if ( seError != LSG_SE_SUCCESS ) { return seError; }
		CStd::MemSet( &m_euEngineHandles, 0xFF, sizeof( m_euEngineHandles ) );

		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			for ( uint32_t J = m_uslSemantics[I].Length(); J--; ) {
				switch ( m_uslSemantics[I][J].ui32SemanticType ) {
					case CShaderParser::token::LSG_SLT_WORLD : {
						m_euEngineHandles.hWorldMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_WORLDVIEW : {
						m_euEngineHandles.hWorldViewMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_WORLDVIEWPROJ : {
						m_euEngineHandles.hWorldViewProjMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_VIEW : {
						m_euEngineHandles.hViewMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_CAMPOS : {
						m_euEngineHandles.hCamPos = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_TIMESINCELASTFRAME : {
						m_euEngineHandles.hTimeSinceLastFrame = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_NORMALMATRIX : {
						m_euEngineHandles.hNormalMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_TEXUNIT : {
						uint64_t ui64Type = m_uslSemantics[I][J].ui64Type;
						if ( m_uslSemantics[I][J].ui32SemanticIndex < LSG_MAX_TEXTURE_UNITS ) {
							if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 ) ) {
								m_euEngineHandles.hTexUnits1D[m_uslSemantics[I][J].ui32SemanticIndex] = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
							}
							else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 ) ) {
								m_euEngineHandles.hTexUnits2D[m_uslSemantics[I][J].ui32SemanticIndex] = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
							}
							else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 ) ) {
								m_euEngineHandles.hTexUnits3D[m_uslSemantics[I][J].ui32SemanticIndex] = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
							}
							else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ) ) {
								m_euEngineHandles.hTexUnitsCube[m_uslSemantics[I][J].ui32SemanticIndex] = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
							}
							else if ( ui64Type == CShaderLanguageParser::CombineTypesAndArrays( CShaderParser::token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 ) ) {
								//m_euEngineHandles.hTexUnitsCube[m_uslSemantics[I][J].ui32SemanticIndex] = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
							}
							else {
								char szError[128];
								CStd::SPrintF( szError, LSE_ELEMENTS( szError ), "Texture sampler %s has unknown type.\r\n", m_uslSemantics[I][J].sName.CStr() );
								CStd::DebugPrintA( szError );
							}
						}
						break;
					}
					case CShaderParser::token::LSG_SLT_TEXMATRIX : {
						m_euEngineHandles.hTexMatrices = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_AMBIENTLIGHT : {
						m_euEngineHandles.hAmbientLight = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_LIGHTCOUNT : {
						m_euEngineHandles.hTotalLights = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_DIRLIGHTCOUNT : {
						m_euEngineHandles.hDirLights = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_POINTLIGHTCOUNT : {
						m_euEngineHandles.hPointLights = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SPOTLIGHTCOUNT : {
						m_euEngineHandles.hSpotLights = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_DIRLIGHTDIRS : {
						m_euEngineHandles.hDirLightDirs = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_DIRLIGHTDIFFUSES : {
						m_euEngineHandles.hDirLightColors = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_POINTLIGHTPOS : {
						m_euEngineHandles.hPointLightPos = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_POINTLIGHTDIFFUSES : {
						m_euEngineHandles.hPointLightColors = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_POINTLIGHTSIZES : {
						m_euEngineHandles.hPointLightSizes = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SPOTLIGHTDIRS : {
						m_euEngineHandles.hSpotLightVectors = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_LIGHTAMBIENT : {
						m_euEngineHandles.hLightAmbients = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_LIGHTDIFFUSE : {
						m_euEngineHandles.hLightDiffuses = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_LIGHTSPECULAR : {
						m_euEngineHandles.hLightSpeculars = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_LIGHTATT : {
						m_euEngineHandles.hAttenuations = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SPOTLIGHTRANGE : {
						m_euEngineHandles.hSpotLight = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_FOGSTART : { 
						m_euEngineHandles.hFogStart = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_FOGEND : { 
						m_euEngineHandles.hFogEnd = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_FOGCOLOR : { 
						m_euEngineHandles.hFogColor = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SHADOWMAPPING : {
						m_euEngineHandles.hShadowMapping = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SHADOWMAPUNIT : {
						m_euEngineHandles.hShadowMapUnit = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SHADOWMAPMATRIX : {
						m_euEngineHandles.hShadowMapMatrix = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SHADOWMAPUVDEPTHSCALES : {
						m_euEngineHandles.hShadowMapUVDepthScales = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SHADOWMAPTEXDIM : {
						m_euEngineHandles.hShadowMapTextureSize = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}															
					case CShaderParser::token::LSG_SLT_SHADOWMAPCASTERSIZE : {
						m_euEngineHandles.hShadowMapCasterSize = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_SKYCOLOR : {
						m_euEngineHandles.hSkyColor = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_GROUNDCOLOR : {
						m_euEngineHandles.hGroundColor = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_MATDIFFUSE : {
						m_euEngineHandles.hDiffuseMaterial = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_MATAMBIENT : {
						m_euEngineHandles.hAmbientMaterial = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_MATEMISSIVE : {
						m_euEngineHandles.hEmissiveMaterial = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_MATSPECULAR : {
						m_euEngineHandles.hSpecularMaterial = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_MATPOWER : {
						m_euEngineHandles.hPower = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_REFLECTIVITY : {
						m_euEngineHandles.hReflectivity = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_ROUGHNESS : {
						m_euEngineHandles.hRoughness = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
					case CShaderParser::token::LSG_SLT_ASHIKFACTORS : {
						m_euEngineHandles.hAshikFactor = GetConstantByName( m_uslSemantics[I][J].sName.CStr() );
						break;
					}
				}
			}

		}


		return LSG_SE_SUCCESS;
	}
	
	/**
	 * Activation callback.  This class allows its base class to activate the shader and then applies
	 *	engine-specific values to the shader once it is applied to the hardware.
	 *
	 * \param _pvSysValue A value with special system meaning.
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CShader::Activate( void * _pvSysValue ) {
		if ( !Parent::Activate( _pvSysValue ) ) { return false; }

		if ( !m_bBeenActivated ) {
			m_bBeenActivated = true;
			for ( uint32_t I = 0; I < LSG_MAX_TEXTURE_UNITS; ++I ) {
				SetInt( m_euEngineHandles.hTexUnits1D[I], I );
				SetInt( m_euEngineHandles.hTexUnits2D[I], I );
				SetInt( m_euEngineHandles.hTexUnits3D[I], I );
				SetInt( m_euEngineHandles.hTexUnitsCube[I], I );
			}
			if ( m_euEngineHandles.hShadowMapUnit != LSE_MAXU32 ) {
				SetInt( m_euEngineHandles.hShadowMapUnit, CGfx::GetMetrics().ui32MaxTexSlot - 1 );
			}
		}
		return true;
	}

}	// namespace lsg
