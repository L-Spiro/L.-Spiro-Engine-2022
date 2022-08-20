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
 * Description: A shader manager holds all of the key translators, receives shader keys and translator names,
 *	selects the appropriate translator, and generates shared shaders.  Shaders that have already been created
 *	are returned immediately.  Shader keys are modified to contain meta-data which improves performance when
 *	checking to see if the shader has already been created.  This meta-data should not be modified anywhere
 *	else.
 */

#include "LSDShaderManager.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CShaderManager::CShaderManager() {
	}
	LSE_CALLCTOR CShaderManager::~CShaderManager() {
		Reset();
	}

	// == Functions.
	/**
	 * Translates a shader name and key pair and produces a shared pointer.  If the shader key has already been translated
	 *	into a shader, the existing shader is returned.
	 *
	 * \param _skpPair Shader name and key to translate into a shared shader.
	 * \return Returns a shared shader representing the shader key.
	 */
	CSharedShaderPtr LSE_CALL CShaderManager::Translate( const LSD_SHADERNAMEKEYPAIR &_skpPair ) {
		if ( !_skpPair.pvMeta ) {
			// Find a translator/map pair by name.
			uint32_t ui32Index;
			if ( !m_lmTranslators.ItemExists( _skpPair.nkNameKey, ui32Index ) ) {
				// Does not exist.
				return CSharedShaderPtr();
			}
			void * & pvMeta = const_cast<void * &>(_skpPair.pvMeta);
			pvMeta = reinterpret_cast<void *>(m_lmTranslators.GetByIndex( ui32Index ));
		}
		const LSD_SHADER_TRANSLATOR_AND_KEYS * pstakTrans = reinterpret_cast<const LSD_SHADER_TRANSLATOR_AND_KEYS *>(_skpPair.pvMeta);

		// Has it already been translated?
		uint32_t ui32Index;
		if ( pstakTrans->plmTranslated->ItemExists( _skpPair.skShaderKey, ui32Index ) ) {
			return pstakTrans->plmTranslated->GetByIndex( ui32Index );
		}

		// Not translated.
		CVector<CShaderKeyTranslator::LSD_SHADER_DEFINE> vDefines;
		if ( !pstakTrans->psktTranslator->Translate( _skpPair.skShaderKey, vDefines ) ) { return CSharedShaderPtr(); }
		CVectorPoD<CShaderBase::LSG_DEFINE> vShaderDefines;
		vShaderDefines.Allocate( vDefines.Length() );
		for ( uint32_t I = 0; I < vDefines.Length(); ++I ) {
			CShaderBase::LSG_DEFINE dDef = {
				vDefines[I].sMacro.CStr(),
				vDefines[I].sDefinition.CStr(),
			};
			if ( !vShaderDefines.Push( dDef ) ) { return CSharedShaderPtr(); }
		}

		CSharedShaderPtr sspPtr;
		if ( !sspPtr.New() ) { return CSharedShaderPtr(); }

		CString sError;
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			CString sPath = FindFile( pstakTrans->psktTranslator->GetShaderPath( static_cast<LSG_SHADER_TYPES>(I) ).CStr() );
			if ( !sPath.Length() ) {
				// Vertex and pixel shaders are required.
				if ( I == LSG_ST_VERTEX || I == LSG_ST_PIXEL ) { return CSharedShaderPtr(); }
			}
			else {
				const CVectorPoD<uint8_t, uint32_t> * pvShader = LoadFile( sPath );
				if ( !pvShader ) {
					CStd::DebugPrintA( "Shader not found: " );
					CStd::DebugPrintA( sPath.CStr() );
					CStd::DebugPrintA( "\r\n" );
					return CSharedShaderPtr();
				}
				if ( vShaderDefines.Length() ) {
					if ( !sspPtr->SetMacros( &vShaderDefines[0], vShaderDefines.Length(), static_cast<LSG_SHADER_TYPES>(I) ) ) {
						return CSharedShaderPtr();
					}
				}

				LSG_SHADER_ERRORS seError = sspPtr->SetShaderFromMemory( &(*pvShader)[0], pvShader->Length(), sPath.CStr(),
					static_cast<LSG_SHADER_TYPES>(I), sError );
				if ( seError != LSG_SE_SUCCESS ) {
					CStd::DebugPrintA( "Error setting shader " );
					CStd::DebugPrintA( sPath.CStr() );
					CStd::DebugPrintA( ": " );
					CStd::DebugPrintA( sError.CStr() );
					CStd::DebugPrintA( "\r\n" );
					return CSharedShaderPtr();
				}
			}
		}
		
		if ( LSG_SE_SUCCESS != sspPtr->Compile( sError ) ) {
			CStd::DebugPrintA( "Error compiling: " );
			CStd::DebugPrintA( sError.CStr() );
			CStd::DebugPrintA( "\r\n" );
			return CSharedShaderPtr();
		}

		// Map it.
		if ( !pstakTrans->plmTranslated->Insert( _skpPair.skShaderKey, sspPtr ) ) {
			return CSharedShaderPtr();
		}

		return sspPtr;
	}

	/**
	 * Creates a shader-key translator object based off a shader-key translator structure.  The created key must either be
	 *	passed successfully to AddTranslator() or manually deleted via LSEDELETE.
	 *
	 * \param _stTrans The shader-key translator structure off of which a shader-key translator object is to be
	 *	created.
	 * \return Returns the created shader-key translator or NULL if there was not enough memory to create one.
	 */
	CShaderKeyTranslator * LSE_CALL CShaderManager::CreateTranslator( const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR &_stTrans ) {
		CShaderKeyTranslator * psktTrans = LSENEW CShaderKeyTranslator();
		if ( !psktTrans ) { return NULL; }
		psktTrans->SetName( _stTrans.pcName );
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			psktTrans->SetShaderPath( static_cast<LSG_SHADER_TYPES>(I), _stTrans.pcShaderPaths[I] );
		}
		for ( uint32_t I = 0; I < _stTrans.ui32TotalCallbacks; ++I ) {
			if ( !psktTrans->RegisterCallback( _stTrans.ptfCallbacks[I] ) ) {
				LSEDELETE psktTrans;
				return NULL;
			}
		}
		return psktTrans;
	}

	/**
	 * Adds a translator to the manager.  If a translator with the same name already exists, nothing happens.
	 *	The translator is destroyed by the manager via LSEDELETE when Reset() is called.  The reference is set to NULL
	 *	if the deletion of the object will be handled by the manager.  If, upon return, _psktTranslator is not set to
	 *	NULL, the user is resposible for deleting the object.  The safest policy is therefor to always call LSEDELETE
	 *	on _psktTranslator after calling this method.
	 *
	 * \param _psktTranslator The translator, which must have been created via LSENEW, to add to the manager.
	 * \return Returns true if there was enough memory to store the translator.  Upon failure, _psktTranslator is not
		 *	added to the manager and therefor must be destroyed manually via LSEDELETE.
	 */
	LSBOOL LSE_CALL CShaderManager::AddTranslator( CShaderKeyTranslator * &_psktTranslator ) {
		if ( !_psktTranslator ) { return false; }
		// Does it already exist?
		uint32_t ui32Index;
		if ( m_lmTranslators.ItemExists( _psktTranslator->Name(), ui32Index ) ) {
			LSEDELETE _psktTranslator;
			_psktTranslator = NULL;
			return true;
		}

		LSD_SHADER_TRANSLATOR_AND_KEYS * pstakKey = LSENEW LSD_SHADER_TRANSLATOR_AND_KEYS;
		if ( !pstakKey ) {
			return false;
		}
		pstakKey->plmTranslated = LSENEW CLinearMap<LSD_SHADERKEY, CSharedShaderPtr>();
		pstakKey->psktTranslator = _psktTranslator;
		if ( !pstakKey->plmTranslated ) {
			LSEDELETE pstakKey;
			return false;
		}
		if ( !m_lmTranslators.Insert( _psktTranslator->Name(), pstakKey ) ) {
			LSEDELETE pstakKey->plmTranslated;
			LSEDELETE pstakKey;
			return false;
		}
		_psktTranslator = NULL;
		return true;
	}

	/**
	 * Adds a translator to the manager based off a descriptive structure.
	 *
	 * \param _stTrans The translator descriptor.
	 * \return Returns true if there was enough memory to create and store the translator.
	 */
	LSBOOL LSE_CALL CShaderManager::AddTranslator( const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR &_stTrans ) {
		CShaderKeyTranslator * psktTranslator = CreateTranslator( _stTrans );
		LSBOOL bAdded = AddTranslator( psktTranslator );
		LSEDELETE psktTranslator;
		return bAdded;
	}

	/**
	 * Resets the shader manager back to scratch, releasing all allocated memory.
	 */
	void LSE_CALL CShaderManager::Reset() {
		for ( uint32_t I = m_lmTranslators.Length(); I--; ) {
			LSEDELETE m_lmTranslators.GetByIndex( I )->plmTranslated;
			LSEDELETE m_lmTranslators.GetByIndex( I )->psktTranslator;
			LSEDELETE m_lmTranslators.GetByIndex( I );
			m_lmTranslators.RemoveByIndex( I );
		}
		m_lmTranslators.Reset();

		m_vShaderPaths.Reset();
		m_lmLoadedShaderFiles.Reset();
	}

	/**
	 * Finds a shader file by searching shader directories.
	 *
	 * \param _pcName Name of the shader file to find.
	 * \return Returns a string containing the full path to a valid shader file or an empty string if the file could be found.
	 */
	CString LSE_CALL CShaderManager::FindFile( const char * _pcPath ) const {
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
		
		/*if ( !sPath.Append( CFileLib::GetResPath() ) ) { return CString(); }
		if ( sPath.GetLastChar() != LSF_PATH_DELIMITER_CHAR ) {
			if ( !sPath.Append( LSF_PATH_DELIMITER_CHAR ) ) { return CString(); }
		}
		if ( !sPath.Append( _pcPath ) ) { return CString(); }*/
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
	 * \return Returns the loaded-to-memory file specified by the given path.
	 */
	const CVectorPoD<uint8_t, uint32_t> * LSE_CALL CShaderManager::LoadFile( const CString &_sPath ) const {
		uint32_t ui32Index;
		if ( m_lmLoadedShaderFiles.ItemExists( _sPath, ui32Index ) ) {
			return &m_lmLoadedShaderFiles.GetByIndex( ui32Index );
		}

		// Have to load the file.
		uint64_t ui64Size = CFileLib::GetFileSize( _sPath.CStr() );
		if ( !ui64Size || ui64Size > LSE_MAXU32 ) { return NULL; }
		LSF_FILE fFile = CFileLib::OpenFile( _sPath.CStr(), LSF_OF_READ, false );
		if ( !fFile ) { return NULL; }
#define LSD_FAIL		CFileLib::CloseFile( fFile ); return NULL
		CVectorPoD<uint8_t, uint32_t> vReturn;
		if ( !vReturn.Resize( static_cast<uint32_t>(ui64Size) ) ) { LSD_FAIL; }

		if ( !CFileLib::ReadFile( fFile, &vReturn[0], vReturn.Length() ) ) { LSD_FAIL; }

		CFileLib::CloseFile( fFile );
#undef LSD_FAIL

		CVectorPoD<uint8_t, uint32_t> * pvReturn;
		if ( !m_lmLoadedShaderFiles.Insert( _sPath, vReturn, &pvReturn ) ) { return NULL; }
		return pvReturn;
	}

}	// namespace lsd
