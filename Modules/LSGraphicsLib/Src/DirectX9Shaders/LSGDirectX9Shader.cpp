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
 * Description: A DirectX 9 shader.  Shaders in DirectX 9 are normally considered to be specifically a vertex shader
 *	or a pixel shader (etc.), but here a single vertex, pixel, and geometry shader are treated as one, matching
 *	shader behavior in OpenGL.  Vertex, pixel, and geometry shaders are called shader parts here, with the shader being
 *	the result of combining them.
 * Shader parts are optionally shared.  The DirectX shader part manager prevents duplicate shader parts and handles
 *	sharing.  Sharing can be disabled for debugging purposes.
 */

#include "LSGDirectX9Shader.h"
#include "LSGDirectX9CompiledShaderManager.h"
#include "LSGDirectX9ShaderManager.h"

#if defined( LSG_DX9 )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9Shader::CDirectX9Shader() :
		m_pdx9vsVertexShader( NULL ),
		m_pdx9psPixelShader( NULL ),
		m_pdx9smShaderManager( NULL ) {
	}
	LSE_CALLCTOR CDirectX9Shader::~CDirectX9Shader() {
		DestroyTranslator();

		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.
	 */
	void LSE_CALL CDirectX9Shader::Reset() {
		if ( m_pdx9smShaderManager ) {
			// Unattach this shader from the manager.
			CDirectX9ShaderManager * pdx9smMan = m_pdx9smShaderManager;
			m_pdx9smShaderManager = NULL;
			pdx9smMan->SetShader( NULL );
		}

		// Release the shaders.
		if ( m_pdx9vsVertexShader ) {
			CDirectX9CompiledShaderManager::DeleteShader( m_pdx9vsVertexShader );
			m_pdx9vsVertexShader = NULL;
		}
		if ( m_pdx9psPixelShader ) {
			CDirectX9CompiledShaderManager::DeleteShader( m_pdx9psPixelShader );
			m_pdx9psPixelShader = NULL;
		}


		// Delete handles.
		m_vHandles.Reset();

		Parent::Reset();
	}

	/**
	 * Gets the uniqe ID of this shader.  The ID is 0 if the shader is not actually ready
	 *	to be activated (that is, has not been compiled and registered with the graphics API).
	 * Activating a shader with a 0 ID is the same as deactivating shaders.
	 *
	 * \return Returns the unique ID for this shader.
	 */
	uint64_t LSE_CALL CDirectX9Shader::GetId() const {
		if ( m_pdx9vsVertexShader && m_pdx9psPixelShader ) {
			// Both must be set for this shader to be used.
			return m_pdx9vsVertexShader->GetId() | (m_pdx9psPixelShader->GetId() << 32ULL);
		}
		return 0ULL;
	}

	/**
	 * Activation callback.  A system value is passed that is to be used internally and changes meaning depending on the
	 *	graphics API.
	 *
	 * \param _pvSysValue A value with special system meaning.
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::Activate( void * _pvSysValue ) {
		m_pdx9smShaderManager = static_cast<CDirectX9ShaderManager *>(_pvSysValue);

		// Can't be called without both the vertex and pixel shaders being set, so don't waste time checking for NULL.
		if ( !m_pdx9psPixelShader->Activate() || !m_pdx9vsVertexShader->Activate() ) {
			CDirectX9::GetDirectX9Device()->SetPixelShader( NULL );
			CDirectX9::GetDirectX9Device()->SetVertexShader( NULL );
			return false;
		}
		return true;
	}

	/**
	 * Deactivation callback.  Calls by the system when the shader is deactivated.
	 */
	void LSE_CALL CDirectX9Shader::Deactivate() {
		m_pdx9smShaderManager = NULL;
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CShaderBase::LSG_HANDLE LSE_CALL CDirectX9Shader::GetConstantByName( const char * _pcName ) {
		if ( !m_pdx9vsVertexShader || !m_pdx9psPixelShader ) { return LSE_MAXU32; }

		// See if we already gave out this handle.
		for ( uint32_t I = 0; I < m_vHandles.Length(); ++I ) {
			if ( CStd::StrCmp( m_ulUniforms[m_vHandles[I].stType][m_vHandles[I].ui32Name].sName.CStr(), _pcName ) == 0 ) {
				return I;
			}
		}

		// Not found.  Make a new handle, if possible.
		uint32_t ui32Ret = m_vHandles.Length();
		LSG_DX9_SHADER_HANDLE dshNew;
		dshNew.hVertexHandle = m_pdx9vsVertexShader->GetConstantByName( _pcName );
		dshNew.hPixelHandle = m_pdx9psPixelShader->GetConstantByName( _pcName );
		dshNew.ui32Name = LSE_MAXU32;
		if ( dshNew.hVertexHandle != LSE_MAXU32 ) {
			// The name is known to exist in the LSG_ST_VERTEX list.  Find it.
			for ( uint32_t I = m_ulUniforms[LSG_ST_VERTEX].Length(); I--; ) {
				if ( CStd::StrCmp( m_ulUniforms[LSG_ST_VERTEX][I].sName.CStr(), _pcName ) == 0 ) {
					dshNew.ui32Name = I;
					dshNew.stType = LSG_ST_VERTEX;
					break;
				}
			}
		}
		else if ( dshNew.hPixelHandle != LSE_MAXU32 ) {
			// The name is known to exist in the LSG_ST_PIXEL list.  Find it.
			for ( uint32_t I = m_ulUniforms[LSG_ST_PIXEL].Length(); I--; ) {
				if ( CStd::StrCmp( m_ulUniforms[LSG_ST_PIXEL][I].sName.CStr(), _pcName ) == 0 ) {
					dshNew.ui32Name = I;
					dshNew.stType = LSG_ST_PIXEL;
					break;
				}
			}
		}
		if ( dshNew.ui32Name == LSE_MAXU32 ) { return LSE_MAXU32; }

		if ( !m_vHandles.Push( dshNew ) ) { return LSE_MAXU32; }

		return ui32Ret;
	}

	/**
	 * Sets a single scalar value in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _fValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetFloat( m_vHandles[_hHandle].hVertexHandle, _fValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetFloat( m_vHandles[_hHandle].hPixelHandle, _fValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets an array of scalar values in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pfValues New values.
	 * \param _ui32Total Total floats to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetFloatArray( LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetFloatArray( m_vHandles[_hHandle].hVertexHandle, _pfValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetFloatArray( m_vHandles[_hHandle].hPixelHandle, _pfValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetInt( LSG_HANDLE _hHandle, int32_t _iValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetInt( m_vHandles[_hHandle].hVertexHandle, _iValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetInt( m_vHandles[_hHandle].hPixelHandle, _iValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets an array of scalar values in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetIntArray( LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetIntArray( m_vHandles[_hHandle].hVertexHandle, _pi32Values, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetIntArray( m_vHandles[_hHandle].hPixelHandle, _pi32Values, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a single scalar value in the shader of type bool.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _bValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetBool( LSG_HANDLE _hHandle, LSBOOL _bValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetBool( m_vHandles[_hHandle].hVertexHandle, _bValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetBool( m_vHandles[_hHandle].hPixelHandle, _bValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets an array of scalar values in the shader of type bool.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pbValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetBoolArray( LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetBoolArray( m_vHandles[_hHandle].hVertexHandle, _pbValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetBoolArray( m_vHandles[_hHandle].hPixelHandle, _pbValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector2( LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector2( m_vHandles[_hHandle].hVertexHandle, _vValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector2( m_vHandles[_hHandle].hPixelHandle, _vValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector2Array( LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector2Array( m_vHandles[_hHandle].hVertexHandle, _pvValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector2Array( m_vHandles[_hHandle].hPixelHandle, _pvValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector3( LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector3( m_vHandles[_hHandle].hVertexHandle, _vValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector3( m_vHandles[_hHandle].hPixelHandle, _vValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector3Array( LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector3Array( m_vHandles[_hHandle].hVertexHandle, _pvValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector3Array( m_vHandles[_hHandle].hPixelHandle, _pvValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector4( LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector4( m_vHandles[_hHandle].hVertexHandle, _vValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector4( m_vHandles[_hHandle].hPixelHandle, _vValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetVector4Array( LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetVector4Array( m_vHandles[_hHandle].hVertexHandle, _pvValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetVector4Array( m_vHandles[_hHandle].hPixelHandle, _pvValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetMatrix2x2( LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetMatrix2x2( m_vHandles[_hHandle].hVertexHandle, _mValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetMatrix2x2( m_vHandles[_hHandle].hPixelHandle, _mValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetMatrix3x3( LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetMatrix3x3( m_vHandles[_hHandle].hVertexHandle, _mValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetMatrix3x3( m_vHandles[_hHandle].hPixelHandle, _mValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetMatrix4x4( LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetMatrix4x4( m_vHandles[_hHandle].hVertexHandle, _mValue ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetMatrix4x4( m_vHandles[_hHandle].hPixelHandle, _mValue ) ) { return false; }
		return true;
	}

	/**
	 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pmValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9Shader::SetMatrix4x4Array( LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) {
		if ( _hHandle > m_vHandles.Length() ) { return false; }
		if ( m_vHandles[_hHandle].hVertexHandle != LSE_MAXU32 && !m_pdx9vsVertexShader->SetMatrix4x4Array( m_vHandles[_hHandle].hVertexHandle, _pmValues, _ui32Total ) ) { return false; }
		if ( m_vHandles[_hHandle].hPixelHandle != LSE_MAXU32 && !m_pdx9psPixelShader->SetMatrix4x4Array( m_vHandles[_hHandle].hPixelHandle, _pmValues, _ui32Total ) ) { return false; }
		return true;
	}

	/**
	 * Creates a new translator of type CDirectX9ShaderTranslator.
	 */
	void LSE_CALL CDirectX9Shader::CreateTranslator() {
		DestroyTranslator();
		CDirectX9ShaderTranslator * pogstNew = LSENEW CDirectX9ShaderTranslator();
		m_pstpbTranslator = pogstNew;
	}

	/**
	 * Destroys the CDirectX9ShaderTranslator translator.
	 */
	void LSE_CALL CDirectX9Shader::DestroyTranslator() {
		if ( m_pstpbTranslator ) {
			CDirectX9ShaderTranslator * pogstDelete = static_cast<CDirectX9ShaderTranslator *>(m_pstpbTranslator);
			m_pstpbTranslator = NULL;
			LSEDELETE pogstDelete;
		}
	}

	/**
	 * Post compile.  We use this to create Direct3D 9 compiled shader parts.
	 *
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CDirectX9Shader::PostCompile( CString &_sError ) {
		if ( m_sFinalVertex.Length() ) {
			m_pdx9vsVertexShader = static_cast<CDirectX9VertexShader *>(CDirectX9CompiledShaderManager::CreateShader( m_sFinalVertex, LSG_ST_VERTEX, m_ulUniforms, _sError ));
			if ( !m_pdx9vsVertexShader ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		if ( m_sFinalPixel.Length() ) {
			m_pdx9psPixelShader = static_cast<CDirectX9PixelShader *>(CDirectX9CompiledShaderManager::CreateShader( m_sFinalPixel, LSG_ST_PIXEL, m_ulUniforms, _sError ));
			if ( !m_pdx9psPixelShader ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		return LSG_SE_SUCCESS;
	}

}	// namespace lsg

#endif	// #if defined( LSG_DX9 )
