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
 * Description: Manages the sharing of similar Metal shaders parts (a single vertex, pixel, or geometry shader).
 *	Since Metal does not give us back the compiled code (sigh) we can only share based on the input text.  Sadly
 *	this reduces the efficiency of the sharing, but it is not so bad thanks to the preprocessing the core engine
 *	does on each shader.  That is, two shaders may be exactly the same except for one having an extra whitespace
 *	character somewhere.  This would normally cause them to be considered as two different shaders even though they
 *	would end up compiled into exactly the same code.  But thanks to L. Spiro Shader Language (LSSL), whitespace
 *	will be made consistent and other formatting will be done so that the two shaders, by the time they reach this
 *	class, will match exactly in text form and proper sharing can take place.
 * Additionally, there is a setting that prevents any shaders from being shared.  This is mainly for tools because
 *	tools may have multiple Metal contexts and sharing will cause errors between them.
 */


#include "LSGMetalCompiledShaderManager.h"

#if defined( LSG_METAL )

namespace lsg {

	// == Members.
	/**
	 * Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
	 */
	LSBOOL CMetalCompiledShaderManager::m_bSharing = true;

	/**
	 * Map of shaders.
	 */
	CLinearMap<CString, CMetalCompiledShader *> CMetalCompiledShaderManager::m_mShaders[LSG_ST_TOTAL];

	// == Functions.
	/**
	 * Sets sharing.
	 *
	 * \param _bVal If true, sharing is enabled, otherwise not.
	 */
	LSVOID LSE_CALL CMetalCompiledShaderManager::SetSharing( LSBOOL _bVal ) {
		m_bSharing = _bVal;
	}

	/**
	 * Creates a new shader of a given type.  The shader MUST be deleted using DeleteShader().
	 *
	 * \param _sShaderText The shader text to compile.  The syntax depends on the target graphics API.
	 * \param _stShaderType The shader type.
	 * \param _sError Holds returned errors.
	 * \return Returns a pointer to the new compiled shader on success, or NULL on failure.
	 */
	CMetalCompiledShader * LSE_CALL CMetalCompiledShaderManager::CreateShader( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		assert( _stShaderType < LSG_ST_TOTAL );
		// If not sharing, this is simple; just make a new object and return it.
		if ( !m_bSharing ) {
			CMetalCompiledShader * pogcsRet = LSENEW CMetalCompiledShader();
			if ( !pogcsRet ) {
				_sError = "Failed to create shader: Not enough memory.";
				return NULL;
			}

			if ( !pogcsRet->Compile( _sShaderText, _stShaderType, _sError ) ) {
				LSEDELETE pogcsRet;
				pogcsRet = NULL;
			}
			else {
				pogcsRet->IncRef();
			}
			return pogcsRet;
		}

		// Sharing, so fix up the text so that #line directives are removed.
		CStringList slList;
		if ( !slList.MakeFromUtf8( _sShaderText.CStr(), _sShaderText.Length() ) ) {
			_sError = "Failed to create shader: Not enough memory.";
			return NULL;
		}
		for ( LSUINT32 I = slList.Length(); I--; ) {
			CString sStart = slList[I].GetToken( ' ', 0UL );
			if ( sStart == "#line" ) {
				slList.RemoveNoDealloc( I );
			}
		}

		CString sFinal = slList.ToString( false );

		// Use this to find the existing shader.
		LSUINT32 ui32Index;
		if ( m_mShaders[_stShaderType].GetItemIndex( sFinal, ui32Index ) ) {
			m_mShaders[_stShaderType].GetByIndex( ui32Index )->IncRef();
			return m_mShaders[_stShaderType].GetByIndex( ui32Index );
		}
		// Not in the map already; add it.
		CMetalCompiledShader * pogcsRet = LSENEW CMetalCompiledShader();
		if ( !pogcsRet ) {
			sFinal.Reset();	// Try to free memory for the error message.
			_sError = "Failed to create shader: Not enough memory.";
			return NULL;
		}

		if ( !pogcsRet->Compile( _sShaderText, _stShaderType, _sError ) ) {
			// Error message will have been filled by the compiled object.
			LSEDELETE pogcsRet;
			return NULL;
		}
		if ( !m_mShaders[_stShaderType].Insert( sFinal, pogcsRet ) ) {
			sFinal.Reset();	// Try to free memory for the error message.
			LSEDELETE pogcsRet;
			_sError = "Failed to create shader: Not enough memory.";
			return NULL;
		}

		// Done.
		pogcsRet->IncRef();

		return pogcsRet;
	}

	/**
	 * Delete a shader.  If sharing, decreases its reference count.  Otherwise deletes it from memory.
	 *
	 * \param _pogcsShader The shader to delete.
	 */
	LSVOID LSE_CALL CMetalCompiledShaderManager::DeleteShader( CMetalCompiledShader * _pogcsShader ) {
		if ( !_pogcsShader ) { return; }

		if ( !m_bSharing ) {
			_pogcsShader->DecRef();
			LSEDELETE _pogcsShader;
		}
		else {
			// Just do a linear search for the pointer.
			for ( LSUINT32 I = m_mShaders[_pogcsShader->GetType()].Length(); I--; ) {
				if ( m_mShaders[_pogcsShader->GetType()].GetByIndex( I ) == _pogcsShader ) {
					_pogcsShader->DecRef();
					return;
				}
			}
			// Pointer not found.  This shader was not created by this class, which is an error.
			CStd::DebugPrintA( "CMetalCompiledShaderManager::DeleteShader(): Given shader was not created by CMetalCompiledShaderManager::CreateShader().\r\n" );
		}
	}

	/**
	 * Deletes all unreferenced shaders from memory.  Returns the number of shaders NOT deleted.
	 *
	 * \return Returns the number of shaders NOT deleted.
	 */
	LSUINT32 LSE_CALL CMetalCompiledShaderManager::DeleteUnreferencedShaders() {
		LSUINT32 ui32Total = 0UL;

		for ( LSUINT32 J = LSG_ST_TOTAL; J--; ) {
			for ( LSUINT32 I = m_mShaders[J].Length(); I--; ) {
				if ( m_mShaders[J].GetByIndex( I )->GetRef() ) {
					++ui32Total;
				}
				else {
					LSEDELETE m_mShaders[J].GetByIndex( I );
					m_mShaders[J].RemoveByIndexNoDealloc( I );
				}
			}
			if ( !m_mShaders[J].Length() ) {
				m_mShaders[J].Reset();
			}
		}
		return ui32Total;
	}

}	// namespace lsg

#endif	// #if defined( LSG_METAL )
