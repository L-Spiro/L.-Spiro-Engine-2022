/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Manages the sharing of shaders between sprites.
 */

#include "LSRSpriteShaderManager.h"


namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CSpriteShaderManager::CSpriteShaderManager() {
	}
	LSE_CALLCTOR CSpriteShaderManager::~CSpriteShaderManager() {
		m_lmShaders.Reset();
	}

	// == Functions.
	/**
	 * Gets a shared shader pointer given a sprite property (LSR_SPRITE_PROPERTIES).
	 *
	 * \param _spProps Sprite properties.
	 * \param _pcVertexShader Vertex shader file name, or NULL to use the default.
	 * \param _pcPixelShader Pixel shader file name, or NULL to use the default.
	 * \return Returns a shared shader pointer, which will be invalid in the event of an error.
	 */
	CSharedShaderPtr LSE_CALL CSpriteShaderManager::GetShader( const LSR_SPRITE_PROPERTIES &_spProps, const char * _pcVertexShader, const char * _pcPixelShader ) {
		const LSR_SORTABLE_SPRITE_PROPERTIES * psspProps = reinterpret_cast<const LSR_SORTABLE_SPRITE_PROPERTIES *>(&_spProps);
		uint32_t ui32Index = 0;
		if ( !m_lmShaders.ItemExists( (*psspProps), ui32Index ) ) {
			// Create and add.
			CSharedShaderPtr sspShader;
			sspShader.New();
			if ( !sspShader.Valid() ) {
				CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): Failed to create shared sprite shader (out of memory).\r\n" );
				return sspShader;
			}
			_pcVertexShader = _pcVertexShader ? _pcVertexShader : "LSRSpriteDefaultVS.lssl";
			_pcPixelShader = _pcPixelShader ? _pcPixelShader : "LSRSpriteDefaultPS.lssl";
			CString sVsPath = FindFile( _pcVertexShader );
			CString sPsPath = FindFile( _pcPixelShader );
			CString sError;
			CVector<LDR_SHADER_DEFINE> vDefines;
			{
				GenerateMacros( _spProps, LSG_ST_VERTEX, vDefines );
				CVectorPoD<CShaderBase::LSG_DEFINE> vShaderDefines;
				vShaderDefines.Allocate( vDefines.Length() );
				for ( uint32_t I = 0; I < vDefines.Length(); ++I ) {
					CShaderBase::LSG_DEFINE dDef = {
						vDefines[I].sMacro.CStr(),
						vDefines[I].sDefinition.CStr(),
					};
					if ( !vShaderDefines.Push( dDef ) ) { return CSharedShaderPtr(); }
				}
				if ( !sspShader->SetMacros( &vShaderDefines[0], vShaderDefines.Length(), LSG_ST_VERTEX ) ) {
					CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): Failed to apply vertex-shader macros.\r\n" );
					return CSharedShaderPtr();
				}
				if ( sspShader->SetShaderFromFile( sVsPath.CStr(), LSG_ST_VERTEX, sError ) != LSG_SE_SUCCESS ) {
					CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): " );
					CStd::DebugPrintA( sError.CStr() );
					return CSharedShaderPtr();
				}
			}
			{
				GenerateMacros( _spProps, LSG_ST_PIXEL, vDefines );
				CVectorPoD<CShaderBase::LSG_DEFINE> vShaderDefines;
				vShaderDefines.Allocate( vDefines.Length() );
				for ( uint32_t I = 0; I < vDefines.Length(); ++I ) {
					CShaderBase::LSG_DEFINE dDef = {
						vDefines[I].sMacro.CStr(),
						vDefines[I].sDefinition.CStr(),
					};
					if ( !vShaderDefines.Push( dDef ) ) { return CSharedShaderPtr(); }
				}
				if ( !sspShader->SetMacros( &vShaderDefines[0], vShaderDefines.Length(), LSG_ST_PIXEL ) ) {
					CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): Failed to apply pixel-shader macros.\r\n" );
					return CSharedShaderPtr();
				}
				if ( sspShader->SetShaderFromFile( sPsPath.CStr(), LSG_ST_PIXEL, sError ) != LSG_SE_SUCCESS ) {
					CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): " );
					CStd::DebugPrintA( sError.CStr() );
					return CSharedShaderPtr();
				}
			}


			if ( LSG_SE_SUCCESS != sspShader->Compile( sError ) ) {
				CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): " );
				CStd::DebugPrintA( sError.CStr() );
				return CSharedShaderPtr();
			}

			if ( !m_lmShaders.Insert( (*psspProps), sspShader ) ) {
				CStd::DebugPrintA( "CSpriteShaderManager::GetShader(): Failed to add shared sprite shader (out of memory).\r\n" );
				return CSharedShaderPtr();
			}
			return sspShader;
		}
		return m_lmShaders.GetByIndex( ui32Index );
	}

	/**
	 * Adds a path to search for shaders.  If no paths are added manually only the default path is searched.
	 *
	 * \param _pcPath The path to add to the search list when looking for shaders.
	 */
	void LSE_CALL CSpriteShaderManager::AddSearchPath( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandPath( _pcPath, szBuffer );
		CString sThis = szBuffer;
		for ( uint32_t I = m_vShaderPaths.Length(); I--; ) {
			if ( m_vShaderPaths[I].Equals( sThis ) ) { return; }
		}

		m_vShaderPaths.Push( sThis );
	}

	/**
	 * Resets the object to scratch.
	 */
	void LSE_CALL CSpriteShaderManager::Reset() {
		m_vShaderPaths.Reset();
		m_lmShaders.Reset();
	}

	/**
	 * Finds a shader file by searching shader directories.
	 *
	 * \param _pcName Name of the shader file to find.
	 * \return Returns a string containing the full path to a valid shader file or an empty string if the file could be found.
	 */
	CString LSE_CALL CSpriteShaderManager::FindFile( const char * _pcPath ) const {
		CString sPath;
		for ( uint32_t I = m_vShaderPaths.Length(); I--; ) {
			sPath.ResetNoDealloc();
			if ( !sPath.Append( m_vShaderPaths[I] ) ) { return CString(); }
			if ( sPath.GetLastChar() != LSF_PATH_DELIMITER_CHAR ) {
				if ( !sPath.Append( LSF_PATH_DELIMITER_CHAR ) ) { return CString(); }
			}
			if ( !sPath.Append( _pcPath ) ) { return CString(); }
			if ( CFileLib::Exists( sPath.CStr() ) ) { return sPath; }
		}

		// Always also check the default data directory.
		sPath.ResetNoDealloc();
		char szBuffer[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcPath, szBuffer );

		if ( CFileLib::Exists( szBuffer ) ) {
			if ( !sPath.Set( szBuffer ) ) { return CString(); }
			return sPath;
		}
		return CString();
	}

	/**
	 * Loads a file to memory and stores it in the map containing loaded files.  If the file is already in the
	 *	map it is not loaded again.
	 *
	 * \param _sPath Path to find/load.
	 * \param _vpdResult Loaded file.
	 * \return Returns false if the file failed to load.
	 */
	LSBOOL LSE_CALL CSpriteShaderManager::LoadFile( const CString &_sPath, CVectorPoD<uint8_t, uint32_t> &_vpdResult ) const {
		// Have to load the file.
		uint64_t ui64Size = CFileLib::GetFileSize( _sPath.CStr() );
		if ( !ui64Size || ui64Size > LSE_MAXU32 ) { return false; }
		LSF_FILE fFile = CFileLib::OpenFile( _sPath.CStr(), LSF_OF_READ, false );
		if ( !fFile ) { return NULL; }
#define LSR_FAIL		CFileLib::CloseFile( fFile ); return false
		if ( !_vpdResult.Resize( static_cast<uint32_t>(ui64Size) ) ) { LSR_FAIL; }

		if ( !CFileLib::ReadFile( fFile, &_vpdResult[0], _vpdResult.Length() ) ) { LSR_FAIL; }

		CFileLib::CloseFile( fFile );
#undef LSR_FAIL

		return true;
	}

	/**
	 * Generates macros for a given shader type based on the sprite properties.
	 *
	 * \param _spProps Sprite properties.
	 * \param _stType Shader type.
	 * \param _vReturn Holds the returned array of defines.
	 * \return Returns true if there was enough memory to complete the operation.
	 */
	LSBOOL LSE_CALL CSpriteShaderManager::GenerateMacros( const LSR_SPRITE_PROPERTIES &_spProps, LSG_SHADER_TYPES _stType, CVector<LDR_SHADER_DEFINE> &_vReturn ) {
#define LSD_ADD_INTERM_MACRO_STRING( NAME )								\
		sdTempDefine.sMacro.ResetNoDealloc();							\
		if ( !sdTempDefine.sMacro.Append( NAME ) ) { return false; }	\
		if ( !_vReturn.Push( sdTempDefine ) ) { return false; }

		_vReturn.ResetNoDealloc();
		LDR_SHADER_DEFINE sdDefine;
		LDR_SHADER_DEFINE sdTempDefine;
		if ( _spProps.afAlpha == LSR_AF_ALPHA_ON ) {
			if ( !sdDefine.sMacro.Set( "LSE_ALPHA" ) ) { return false; }
			if ( !sdDefine.sDefinition.Set( "1" ) ) { return false; }
			if ( !_vReturn.Push( sdDefine ) ) { return false; }
		}
		sdDefine.sMacro.ResetNoDealloc();
		sdDefine.sDefinition.ResetNoDealloc();

		if ( _stType == LSG_ST_VERTEX ) {
			// ***********************
			// Vertex-shader inputs.
			// ***********************
			if ( !sdDefine.sMacro.Set( "lse_MainVsInputs" ) ) { return false; }
			// Always add position and UV.
			if ( !sdDefine.sDefinition.Set( "in vec3 _vInPosition : POSITION0" ) ) { return false; }
			if ( !sdDefine.sDefinition.Append( ", in vec2 _vInTexCoord0 : TEXCOORD0" ) ) { return false; }
			if ( !_vReturn.Push( sdDefine ) ) { return false; }
			LSD_ADD_INTERM_MACRO_STRING( "LSE_TEXCOORD0" );



			// ***********************
			// Vertex-shader outputs.
			// ***********************
			// Handle standard vertex-buffer outputs.
			if ( !sdDefine.sMacro.Set( "lse_MainVsOutputs" ) ) { return false; }

			// Always add position and UV.
			if ( !sdDefine.sDefinition.Set( "out vec4 _vOutPosition : POSITION0" ) ) { return false; }
			if ( !sdDefine.sDefinition.Append( ", out vec2 _vOutTexCoord0 : TEXCOORD0" ) ) { return false; }
			if ( !_vReturn.Push( sdDefine ) ) { return false; }



			// ***********************
			// Vertex-shader knick-knacks.
			// ***********************
			// Handle copying over of the variable number of texture coordinates.
			if ( !sdDefine.sMacro.Set( "LSE_TEXCOORD_COPY" ) ) { return false; }
			sdDefine.sDefinition.ResetNoDealloc();
			for ( uint32_t I = 0; I < 1; ++I ) {
				char szBuffer[64];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "_vOutTexCoord%u = _vInTexCoord%u;", I, I );
				if ( !sdDefine.sDefinition.Append( szBuffer ) ) { return false; }
			}

			if ( !_vReturn.Push( sdDefine ) ) { return false; }
		}
		else if ( _stType == LSG_ST_PIXEL ) {
			// ***********************
			// Pixel-shader inputs.
			// ***********************
			char szBuffer[128];
			// Handle standard vertex-buffer outputs.
			if ( !sdDefine.sMacro.Set( "lse_MainPsInputs" ) ) { return false; }
			// Always add position.
			if ( !sdDefine.sDefinition.Set( "in vec4 _vInPosition : POSITION0" ) ) { return false; }
			if ( !sdDefine.sDefinition.Append( ", in vec2 _vInTexCoord0 : TEXCOORD0" ) ) { return false; }
			if ( !_vReturn.Push( sdDefine ) ) { return false; }


			if ( !sdTempDefine.sMacro.Set( "LSE_TEX_UNIFORMS" ) ) { return false; }
			sdTempDefine.sDefinition.ResetNoDealloc();
			for ( uint32_t I = 0; I < 1; ++I ) {
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "uniform Texture2d g_tTex2d%u:TEXUNIT%u;",
					I, I );
				if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "uniform Sampler g_sSampler%u:TEXUNIT%u;",
					I, I );
				if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
			}
			if ( !_vReturn.Push( sdTempDefine ) ) { return false; }


			sdTempDefine.sDefinition.ResetNoDealloc();
			if ( !sdTempDefine.sMacro.Set( "LSE_TEX_STORES" ) ) { return false; }
			for ( uint32_t I = 0; I < 1; ++I ) {
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "vec4 vTex%u=tex2d(g_tTex2d%u,g_sSampler%u,_vInTexCoord%u);",
					I, I, I, I );
				if ( !sdTempDefine.sDefinition.Append( szBuffer ) ) { return false; }
			}
			if ( !_vReturn.Push( sdTempDefine ) ) { return false; }
		}

		return true;
#undef LSD_ADD_INTERM_MACRO_STRING
	}

}	// namespace lsr
