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
 * Description: Manages the sharing of a single Direct3D 11 shader (vertex or pixel shader).  Sharing is based
 *	off the (adjusted) shader text for the sake of speed.  The alternative--sharing based off compiled code--
 *	makes loading times too long.
 * For debug purposes, sharing can be disabled.
 */

#include "LSGDirectX11CompiledShaderManager.h"
#include "String/LSTLStringList.h"

#if defined( LSG_DX11 )

namespace lsg {

	// == Members.
	/**
	 * Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
	 */
	LSBOOL CDirectX11CompiledShaderManager::m_bSharing = true;

	/**
	 * Map of shaders.
	 */
	CLinearMap<CString, CCompiledShaderBase *> CDirectX11CompiledShaderManager::m_mShaders[LSG_ST_TOTAL];

	// == Functions.
	/**
	 * Sets sharing.
	 *
	 * \param _bVal If true, sharing is enabled, otherwise not.
	 */
	void LSE_CALL CDirectX11CompiledShaderManager::SetSharing( LSBOOL _bVal ) {
		m_bSharing = _bVal;
	}

	/**
	 * Creates a new shader of a given type.  The shader MUST be deleted using DeleteShader().
	 *
	 * \param _sShaderText The shader text to compile.  The syntax depends on the target graphics API.
	 * \param _stShaderType The shader type.
	 * \param _pulUniforms Shader uniforms.
	 * \param _sError Holds returned errors.
	 * \return Returns a pointer to the new compiled shader on success, or NULL on failure.
	 */
	CCompiledShaderBase * LSE_CALL CDirectX11CompiledShaderManager::CreateShader( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType,
		CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms, CString &_sError ) {
		assert( _stShaderType < LSG_ST_TOTAL );
		// If not sharing, this is simple; just make a new object and return it.
		if ( !m_bSharing ) {
			CCompiledShaderBase * pogcsRet = NULL;
			switch ( _stShaderType ) {
				case LSG_ST_PIXEL : {
					pogcsRet = LSENEW CDirectX11PixelShader();
					break;
				}
				case LSG_ST_VERTEX : {
					pogcsRet = LSENEW CDirectX11VertexShader();
					break;
				}
			}
			if ( !pogcsRet ) {
				_sError = "Failed to create shader: Not enough memory.";
				return NULL;
			}

			if ( !pogcsRet->Compile( _sShaderText, _stShaderType, _sError ) ) {
				LSEDELETE pogcsRet;
				pogcsRet = NULL;
			}
			else {
				if ( !static_cast<CDirectX11CompiledShader *>(pogcsRet)->SetUniforms( _pulUniforms[_stShaderType] ) ) {
					LSEDELETE pogcsRet;
					pogcsRet = NULL;
				}
				else {
					// Win!
					pogcsRet->IncRef();
				}
			}
			return pogcsRet;
		}

		// Sharing, so fix up the text so that #line directives are removed.
		CStringList slList;
		if ( !slList.MakeFromUtf8( _sShaderText.CStr(), _sShaderText.Length() ) ) {
			_sError = "Failed to create shader: Not enough memory.";
			return NULL;
		}
		for ( uint32_t I = slList.Length(); I--; ) {
			CString sStart = slList[I].GetToken( ' ', 0 );
			if ( sStart == "#line" ) {
				slList.RemoveNoDealloc( I );
			}
		}

		CString sFinal = slList.ToString( false );

		// Use this to find the existing shader.
		uint32_t ui32Index;
		if ( m_mShaders[_stShaderType].GetItemIndex( sFinal, ui32Index ) ) {
			m_mShaders[_stShaderType].GetByIndex( ui32Index )->IncRef();
			return m_mShaders[_stShaderType].GetByIndex( ui32Index );
		}
		// Not in the map already; add it.
		CCompiledShaderBase * pogcsRet = NULL;
		switch ( _stShaderType ) {
			case LSG_ST_PIXEL : {
				pogcsRet = LSENEW CDirectX11PixelShader();
				break;
			}
			case LSG_ST_VERTEX : {
				pogcsRet = LSENEW CDirectX11VertexShader();
				break;
			}
		}
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
		if ( !static_cast<CDirectX11CompiledShader *>(pogcsRet)->SetUniforms( _pulUniforms[_stShaderType] ) ) {
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
	void LSE_CALL CDirectX11CompiledShaderManager::DeleteShader( CCompiledShaderBase * _pogcsShader ) {
		if ( !_pogcsShader ) { return; }



		if ( !m_bSharing ) {
			switch ( _pogcsShader->GetType() ) {
				case LSG_ST_PIXEL : {
					static_cast<CDirectX11PixelShader *>(_pogcsShader)->DecRef();
					break;
				}
				case LSG_ST_VERTEX : {
					static_cast<CDirectX11VertexShader *>(_pogcsShader)->DecRef();
					break;
				}
			}
			LSEDELETE _pogcsShader;
		}
		else {
			// Just do a linear search for the pointer.
			for ( uint32_t I = m_mShaders[_pogcsShader->GetType()].Length(); I--; ) {
				if ( m_mShaders[_pogcsShader->GetType()].GetByIndex( I ) == _pogcsShader ) {
					_pogcsShader->DecRef();
					return;
				}
			}
			// Pointer not found.  This shader was not created by this class, which is an error.
			CStd::DebugPrintA( "CDirectX11CompiledShaderManager::DeleteShader(): Given shader was not created by CDirectX11CompiledShaderManager::CreateShader().\r\n" );
		}
	}

	/**
	 * Deletes all unreferenced shaders from memory.  Returns the number of shaders NOT deleted.
	 *
	 * \return Returns the number of shaders NOT deleted.
	 */
	uint32_t LSE_CALL CDirectX11CompiledShaderManager::DeleteUnreferencedShaders() {
		uint32_t ui32Total = 0;

		for ( uint32_t J = LSG_ST_TOTAL; J--; ) {
			for ( uint32_t I = m_mShaders[J].Length(); I--; ) {
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

#endif	// #if defined( LSG_DX11 )
