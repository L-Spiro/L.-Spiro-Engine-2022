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
 * Description: Translates a shader key into macros to be passed to a shader.  A translator is named and is
 *	selected by the system for translation based on its name, which will be one of the parameters in a high-level
 *	effects-type language.
 */

#include "LSDShaderKeyTranslator.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CShaderKeyTranslator::CShaderKeyTranslator() :
		m_ptfInputCallback( DefaultInputTranslator ) {
	}
	LSE_CALLCTOR CShaderKeyTranslator::~CShaderKeyTranslator() {
	}

	// == Functions.
	/**
	 * Sets the name of the translator.
	 *
	 * \param _pcName Name of the translator.
	 */
	void LSE_CALL CShaderKeyTranslator::SetName( const char * _pcName ) {
		m_nkName = _pcName;
	}

	/**
	 * Translates a shader key, supplying macros based off the flags and values in the key.
	 *
	 * \param _skKey The shader key to translate.
	 * \param _vDefines The resulting array of macros from the translation process.
	 * \return Returns true if there was enough memory to supply all macros for the translation process.
	 */
	LSBOOL LSE_CALL CShaderKeyTranslator::Translate( const LSD_SHADERKEY &_skKey, CVector<LSD_SHADER_DEFINE> &_vDefines ) const {
		if ( !m_ptfInputCallback( _skKey, _vDefines ) ) { return false; }
		for ( uint32_t I = 0; I < m_vCallbacks.Length(); ++I ) {
			if ( !m_vCallbacks[I]( _skKey, _vDefines ) ) { return false; }
		}
		return true;
	}

	/**
	 * The default function for translating vertex-buffer inputs.
	 *
	 * \param _skKey The key to parse.
	 * \param _vDefines Holds generated macros.
	 * \return Returns true if there was enough memory to add all necessary macros to the dictionary.
	 */
	LSBOOL LSE_CALL CShaderKeyTranslator::DefaultInputTranslator( const LSD_SHADERKEY &_skKey, CVector<LSD_SHADER_DEFINE> &_vDefines ) {
#define LSD_ADD_INTERM_MACRO( NAME )									\
		sdTempDefine.sMacro.ResetNoDealloc();							\
		if ( !sdTempDefine.sMacro.Append( #NAME ) ) { return false; }	\
		if ( !_vDefines.Push( sdTempDefine ) ) { return false; }
#define LSD_ADD_INTERM_MACRO_STRING( NAME )								\
		sdTempDefine.sMacro.ResetNoDealloc();							\
		if ( !sdTempDefine.sMacro.Append( NAME ) ) { return false; }	\
		if ( !_vDefines.Push( sdTempDefine ) ) { return false; }


		LSBOOL bVertexLight = (_skKey.ui64Flags & LSD_SF_VERTEX_LIGHTING) &&
			(_skKey.ui8Textures[LSD_TS_NORMAL_MAP] == 0xFF) &&
			(_skKey.ui8Textures[LSD_TS_SPECULAR] == 0xFF) &&
			(_skKey.ui8Textures[LSD_TS_SHININESS] == 0xFF);
		LSD_SHADER_DEFINE sdTempDefine;
		LSD_SHADER_DEFINE sdDefine;

		// ***********************
		// Vertex-shader inputs.
		// ***********************
		if ( !sdDefine.sMacro.Append( "lse_MainVsInputs" ) ) { return false; }

		// Always add position.
		if ( !sdDefine.sDefinition.Append( "in vec3 _vInPosition : POSITION0" ) ) { return false; }
		// Normals?
		if ( _skKey.ui8Inputs & LSD_VI_NORMALS ) {
			if ( !sdDefine.sDefinition.Append( ", in vec3 _vInNormal : NORMAL0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_NORMALS );
		}
		// Colors?
		if ( _skKey.ui8Inputs & LSD_VI_COLORS ) {
			if ( !sdDefine.sDefinition.Append( ", in vec4 _vInColor : COLOR0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_COLORS );
		}

		// UV sets.
		for ( uint32_t I = 0; I < _skKey.ui8Uvs; ++I ) {
			char szBuffer[64];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ", in vec2 _vInTexCoord%u : TEXCOORD%u", I, I );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "LSE_TEXCOORD%u", I );
			LSD_ADD_INTERM_MACRO_STRING( szBuffer );
		}

		// Skin indices?
		if ( _skKey.ui8Inputs & LSD_VI_SKININDICES ) {
			if ( !sdDefine.sDefinition.Append( ", in vec4 _vInSkinIndices : BLENDINDICES0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_SKININDICES );
		}
		// Skin weights?
		if ( _skKey.ui8Inputs & LSD_VI_SKINWEIGHTS ) {
			if ( !sdDefine.sDefinition.Append( ", in vec4 _vInSkinWeights : BLENDWEIGHT0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_SKINWEIGHTS );
		}
		
		// Tangent?
		if ( _skKey.ui8Inputs & LSD_VI_TANGENT ) {
			if ( !sdDefine.sDefinition.Append( ", in vec3 _vInTangent : TANGENT0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_TANGENT );
		}
		// Bitangent?
		if ( _skKey.ui8Inputs & LSD_VI_BITANGENT ) {
			if ( !sdDefine.sDefinition.Append( ", in vec3 _vInBinormal : BINORMAL0" ) ) { return false; }
			LSD_ADD_INTERM_MACRO( LSE_BITANGENT );
			LSD_ADD_INTERM_MACRO( LSE_BINORMAL );
		}

		if ( !_vDefines.Push( sdDefine ) ) { return false; }

		sdDefine.sMacro.ResetNoDealloc();
		sdDefine.sDefinition.ResetNoDealloc();

		uint8_t uiTexCoorIndex = 0;

		// ***********************
		// Vertex-shader outputs.
		// ***********************
		// Handle standard vertex-buffer outputs.
		if ( !sdDefine.sMacro.Append( "lse_MainVsOutputs" ) ) { return false; }

		// Always add position.
		if ( !sdDefine.sDefinition.Append( "out vec4 _vOutPosition : POSITION0" ) ) { return false; }
		// Normals?
		if ( _skKey.ui8Inputs & LSD_VI_NORMALS && !bVertexLight ) {
			if ( !sdDefine.sDefinition.Append( ", out vec3 _vOutNormal : NORMAL0" ) ) { return false; }
		}
		// Colors?
		if ( _skKey.ui8Inputs & LSD_VI_COLORS ) {
			if ( !sdDefine.sDefinition.Append( ", out vec4 _vOutColor : COLOR0" ) ) { return false; }
		}
		// UV sets.
		for ( uint32_t I = 0; I < _skKey.ui8Uvs; ++I ) {
			char szBuffer[64];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ", out vec2 _vOutTexCoord%u : TEXCOORD%u", I, uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}
		// Tangent?
		if ( _skKey.ui8Inputs & LSD_VI_TANGENT && !bVertexLight  ) {
			if ( !sdDefine.sDefinition.Append( ", out vec3 _vOutTangent : TANGENT0" ) ) { return false; }
		}
		// Bitangent?
		if ( _skKey.ui8Inputs & LSD_VI_BITANGENT && !bVertexLight  ) {
			if ( !sdDefine.sDefinition.Append( ", out vec3 _vOutBinormal : BINORMAL0" ) ) { return false; }
		}
		// Vertex lighting?
		if ( bVertexLight ) {
			char szBuffer[64];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ", out vec4 _vOutDiffuseLight : TEXCOORD%u", uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), ", out vec4 _vOutSpecularLight : TEXCOORD%u", uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}

		if ( !_vDefines.Push( sdDefine ) ) { return false; }



		sdDefine.sMacro.ResetNoDealloc();
		sdDefine.sDefinition.ResetNoDealloc();
		uiTexCoorIndex = 0;


		// ***********************
		// Pixel-shader inputs.
		// ***********************
		// Handle standard vertex-buffer outputs.
		if ( !sdDefine.sMacro.Set( "lse_MainPsInputs" ) ) { return false; }
		LSBOOL bHasPrepended = false;
#define LSD_PREPEND																		\
	if ( bHasPrepended ) {																\
		if ( !sdDefine.sDefinition.Append( ", " ) ) { return false; }					\
	}
		// Always add position.
#ifdef LSG_DX11
		if ( !sdDefine.sDefinition.Set( "in vec4 _vInPosition : POSITION0" ) ) { return false; }
		bHasPrepended = true;
#endif	// #ifdef LSG_DX11
		// Normals?
		if ( _skKey.ui8Inputs & LSD_VI_NORMALS && !bVertexLight  ) {
			LSD_PREPEND
			if ( !sdDefine.sDefinition.Append( "in vec3 _vInNormal : NORMAL0" ) ) { return false; }
			bHasPrepended = true;
		}
		// Colors?
		if ( _skKey.ui8Inputs & LSD_VI_COLORS ) {
			LSD_PREPEND
			if ( !sdDefine.sDefinition.Append( "in vec4 _vInColor : COLOR0" ) ) { return false; }
			bHasPrepended = true;
		}
		// UV sets.
		for ( uint32_t I = 0; I < _skKey.ui8Uvs; ++I ) {
			char szBuffer[64];
			LSD_PREPEND
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "in vec2 _vInTexCoord%u : TEXCOORD%u", I, uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			bHasPrepended = true;
		}
		// Tangent?
		if ( _skKey.ui8Inputs & LSD_VI_TANGENT && !bVertexLight  ) {
			LSD_PREPEND
			if ( !sdDefine.sDefinition.Append( "in vec3 _vInTangent : TANGENT0" ) ) { return false; }
			bHasPrepended = true;
		}
		// Bitangent?
		if ( _skKey.ui8Inputs & LSD_VI_BITANGENT && !bVertexLight  ) {
			LSD_PREPEND
			if ( !sdDefine.sDefinition.Append( "in vec3 _vInBinormal : BINORMAL0" ) ) { return false; }
			bHasPrepended = true;
		}
		// Vertex lighting?
		if ( bVertexLight ) {
			char szBuffer[64];
			LSD_PREPEND
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "in vec4 _vInDiffuseLight : TEXCOORD%u", uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			bHasPrepended = true;
			LSD_PREPEND
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "in vec4 _vInSpecularLight : TEXCOORD%u", uiTexCoorIndex++ );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			bHasPrepended = true;
		}

		if ( !_vDefines.Push( sdDefine ) ) { return false; }



		sdDefine.sMacro.ResetNoDealloc();
		sdDefine.sDefinition.ResetNoDealloc();


		// ***********************
		// Shader flags.
		// ***********************
		struct LSD_FLAG_MAP {
			uint64_t				ui64Flag;
			const char *			pcMacro;
		};
		static const LSD_FLAG_MAP fmFlags[] = {
			{ LSD_SF_TRANSLUCENCY, "LSE_ALPHA" },
			{ LSD_SF_SPECULAR, "LSE_SPECULAR" },
		};
		sdDefine.sDefinition.Set( "1" );
		for ( uint32_t I = 0; I < LSE_ELEMENTS( fmFlags ); ++I ) {
			if ( _skKey.ui64Flags & fmFlags[I].ui64Flag ) {
				if ( !sdDefine.sMacro.Set( fmFlags[I].pcMacro ) ) { return false; }
				if ( !_vDefines.Push( sdDefine ) ) { return false; }
			}
		}
		if ( bVertexLight ) {
			if ( !sdDefine.sMacro.Set( "LSE_VERTEX_LIGHTING" ) ) { return false; }
			if ( !_vDefines.Push( sdDefine ) ) { return false; }
		}


		sdDefine.sMacro.ResetNoDealloc();
		sdDefine.sDefinition.ResetNoDealloc();


		// ***********************
		// Vertex-shader knick-knacks.
		// ***********************
		// Handle copying over of the variable number of texture coordinates.
		if ( !sdDefine.sMacro.Set( "LSE_TEXCOORD_COPY" ) ) { return false; }
		for ( uint32_t I = 0; I < _skKey.ui8Uvs; ++I ) {
			char szBuffer[64];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "_vOutTexCoord%u = _vInTexCoord%u;", I, I );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}

		if ( !_vDefines.Push( sdDefine ) ) { return false; }

		// Macros for the unused TEXCOORDn so users can append their own.
		uint32_t ui32Index = 0;
		for ( uint32_t I = uiTexCoorIndex; I < LSG_MAX_VERTEX_SEMANTIC; ++I ) {
			char szBuffer[64];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "LSE_TEXCOORD_USER%u", ui32Index++ );
			if ( !sdDefine.sMacro.Set( szBuffer ) ) { return false; }
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "TEXCOORD%u", I );
			if ( !sdDefine.sDefinition.Set( szBuffer ) ) { return false; }

			if ( !_vDefines.Push( sdDefine ) ) { return false; }
		}

		// Lights.
		if ( !sdDefine.sMacro.Set( "LSE_ACCUMULATE_DIR_LIGHTS( ARGSLIGHT, DIRS, COLORS, NORMAL, DIFFUSEGATHER, SPECULARGATHER, BRDF )" ) ) { return false; }
		sdDefine.sDefinition.ResetNoDealloc();
		for ( uint32_t I = 0; I < _skKey.ui8DirLights; ++I ) {
			char szBuffer[512];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "SetLightArgsDir( ARGSLIGHT, DIRS[%u].xyz, COLORS[%u], NORMAL.xyz );"
				"BRDF( ARGSLIGHT, DIFFUSEGATHER, SPECULARGATHER );",
				I, I );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}
		if ( !_vDefines.Push( sdDefine ) ) { return false; }


		if ( !sdDefine.sMacro.Set( "LSE_ACCUMULATE_POINT_LIGHTS( ARGSLIGHT, LIGHTPOS, COLORS, SIZE, POS, NORMAL, DIFFUSEGATHER, SPECULARGATHER, BRDF )" ) ) { return false; }
		sdDefine.sDefinition.ResetNoDealloc();
		for ( uint32_t I = 0; I < _skKey.ui8PointLights; ++I ) {
			char szBuffer[512];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "SetLightArgsPoint( ARGSLIGHT, LIGHTPOS[%u].xyz, COLORS[%u], SIZE[%u], POS.xyz, NORMAL.xyz );"
				"BRDF( ARGSLIGHT, DIFFUSEGATHER, SPECULARGATHER );",
				I, I, I );
			if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}
		if ( !_vDefines.Push( sdDefine ) ) { return false; }
		return true;
#undef LSD_ADD_INTERM_MACRO_STRING
#undef LSD_ADD_INTERM_MACRO
#undef LSD_PREPEND
	}

}	// namespace lsd
