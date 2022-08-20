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
 * Description: The default system shader-key translator.
 */

#include "LSDDefaultShaderTranslator.h"


namespace lsd {

	// == Members.
	/** The array of default functions. */
	CShaderKeyTranslator::PfTransFunc CDefaultShaderTranslator::m_tfFuncs[] = {
		DefaultTextureTranslator,
		NULL
	};

	/** The structure for creating actual shader-key translators. */
	CShaderKeyTranslator::LSD_SHADER_TRANSLATOR CDefaultShaderTranslator::m_stTranslator = {
		"default",													// pcName
		{
			"LSDForwardDefaultVS.lssl",								// pcShaderPaths[LSE_ST_VERTEX]
			"LSDForwardDefaultPS.lssl",								// pcShaderPaths[LSE_ST_PIXEL]
		},
		m_tfFuncs,													// ptfCallbacks
		LSE_ELEMENTS( m_tfFuncs ) - 1,								// ui32TotalCallbacks
	};

	// == Functions.
	/**
	 * Gets a constant reference to the default shader-key translator structure.
	 *
	 * \return Returns a constant reference to the default shader-key translator structure.
	 */
	const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR & LSE_CALL CDefaultShaderTranslator::Translator() {
		return m_stTranslator;
	}

	/**
	 * The default function for translating texture inputs (connecting input textures to UV's).
	 *
	 * \param _skKey The key to parse.
	 * \param _vDefines Holds generated macros.
	 * \return Returns true if there was enough memory to add all necessary macros to the dictionary.
	 */
	LSBOOL LSE_CALL CDefaultShaderTranslator::DefaultTextureTranslator( const LSD_SHADERKEY &_skKey, CVector<CShaderKeyTranslator::LSD_SHADER_DEFINE> &_vDefines ) {
#define LSD_ADD_MACRO( NAME )															\
		if ( !sdTempDefine.sMacro.Set( NAME ) ) { return false; }

#define LSD_ADD_INTERM_MACRO( NAME )													\
		LSD_ADD_MACRO( "LSE_TEXCOORD_" #NAME )											\
		if ( !sdTempDefine.sDefinition.Set( szBuffer ) ) { return false; }				\
		if ( !_vDefines.Push( sdTempDefine ) ) { return false; }

#define LSD_ADD_TEX( NAME )																														\
		if ( _skKey.ui8Textures[LSD_TS_##NAME] != 0xFF ) {																						\
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "_vInTexCoord%u", _skKey.ui8TextureUvs[LSD_TS_##NAME] );							\
			LSD_ADD_INTERM_MACRO( NAME );																										\
			if ( !sReads.Insert( _skKey.ui8Textures[LSD_TS_##NAME] | (_skKey.ui8TextureUvs[LSD_TS_##NAME] << 16) ) ) { return false; }			\
			if ( !sTexUnits.Insert( _skKey.ui8Textures[LSD_TS_##NAME] ) ) { return false; }														\
		}

		CSet<uint32_t> sReads;
		CSet<uint32_t> sTexUnits;
		CShaderKeyTranslator::LSD_SHADER_DEFINE sdTempDefine;
		char szBuffer[128];
		LSD_ADD_TEX( DIFFUSE );
		LSD_ADD_TEX( DIFFUSE_FACTOR );
		LSD_ADD_TEX( TRANSPARENCY );
		LSD_ADD_TEX( TRANSPARENCY_FACTOR );
		LSD_ADD_TEX( EMISSIVE );
		LSD_ADD_TEX( EMISSIVE_FACTOR );
		LSD_ADD_TEX( REFLECTION );
		LSD_ADD_TEX( REFLECTION_FACTOR );
		LSD_ADD_TEX( SPECULAR );
		LSD_ADD_TEX( SPECULAR_FACTOR );
		LSD_ADD_TEX( SHININESS );
		LSD_ADD_TEX( NORMAL_MAP );

		

		sdTempDefine.sMacro.ResetNoDealloc();
		sdTempDefine.sDefinition.ResetNoDealloc();
		if ( !sdTempDefine.sMacro.Set( "LSE_TEX_STORES" ) ) { return false; }
		// Ensure that textures are only read once.  Make a set of macros that store reads from textures only once.
		for ( uint32_t I = 0; I < sReads.Length(); ++I ) {
			uint32_t ui32Index = sReads.GetByIndex( I );
			uint8_t ui8TexUnit = static_cast<uint8_t>(ui32Index);
			uint8_t ui8TexUv = static_cast<uint8_t>(ui32Index >> 16);
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "vec4 vTex%u=tex2d(g_tTex2d%u,g_sSampler%u,_vInTexCoord%u);",
				ui32Index, ui8TexUnit, ui8TexUnit, ui8TexUv );
			if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}
		if ( !_vDefines.Push( sdTempDefine ) ) { return false; }
		



		sdTempDefine.sMacro.ResetNoDealloc();
		sdTempDefine.sDefinition.ResetNoDealloc();
		// Texture uniforms (Texture2D and Sampler).
		if ( !sdTempDefine.sMacro.Set( "LSE_TEX_UNIFORMS" ) ) { return false; }
		for ( uint32_t I = sTexUnits.Length(); I--; ) {
			uint32_t ui32TexUnit = sTexUnits.GetByIndex( I );
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "uniform Texture2d g_tTex2d%u:TEXUNIT%u;",
				ui32TexUnit, ui32TexUnit );
			if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "uniform Sampler g_sSampler%u:TEXUNIT%u;",
				ui32TexUnit, ui32TexUnit );
			if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
		}
		if ( !_vDefines.Push( sdTempDefine ) ) { return false; }



		// Names of properties.
		static const char * pcNames[] = {
			"DIFFUSE",
			"DIFFUSE_FACTOR",
			"TRANSPARENCY",
			"TRANSPARENCY_FACTOR",
			"EMISSIVE",
			"EMISSIVE_FACTOR",
			"REFLECTION",
			"REFLECTION_FACTOR",
			"SPECULAR",
			"SPECULAR_FACTOR",
			"SHININESS",
			"NORMAL_MAP",
		};
		assert( LSE_ELEMENTS( pcNames ) <= LSD_TS_TOTAL );

		// Macros for single-read textures using vTexX.
		for ( uint32_t I = LSE_ELEMENTS( pcNames ); I--; ) {
			if ( _skKey.ui8Textures[I] != 0xFF ) {
				uint32_t ui32TexUnit = _skKey.ui8Textures[I];
				uint32_t ui32TexUv = _skKey.ui8TextureUvs[I];
				uint32_t ui32Index;
				if ( sReads.GetItemIndex( ui32TexUnit | (ui32TexUv << 16), ui32Index ) ) {
					// Name the macro.
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "LSE_%s_TEX_LOCAL",
						pcNames[I] );
					if ( !sdTempDefine.sMacro.Set( szBuffer ) ) { return false; }
					// Define it based off which texture slot + UV combination was used.
					CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "vTex%u",
						ui32Index );
					if ( !sdTempDefine.sDefinition.Set( szBuffer ) ) { return false; }
					if ( !_vDefines.Push( sdTempDefine ) ) { return false; }
				}
			}
		}
		return true;
#undef LSD_ADD_TEX
#undef LSD_ADD_INTERM_MACRO_STRING
#undef LSD_ADD_MACRO
	}

}	// namespace lsd
