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
 * Description: Open Metal shader.  Specifically for Metal, after shaders are translated, they are linked
 *	into an Metal program object.  A shader program in Metal is a combination of a vertex shader, pixel
 *	shader, and geometry shader.  Each of these are called shader parts in this context, and shader parts
 *	can be shared.  For this reason, it may be common for multiple Metal shader programs to be comprised
 *	of the same shader parts.
 * As an optimization, this class redirects the creation of shader programs to another manager which has global
 *	scope and is capable of determining when shader programs are created from the same shader parts.  This
 *	reduces significantly the number of shader swaps that need to be made during rendering.
 */

#include "LSGMetalShader.h"
#include "../Gfx/LSGGfx.h"
#include "LSGMetalShaderManager.h"

#if defined( LSG_METAL )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalShader::CMetalShader() :
		m_pogsmShaderMan( NULL ),
		m_pospProgram( NULL ) {
		for ( LSUINT32 I = LSG_ST_TOTAL; I--; ) {
			m_pogcsShaders[I] = NULL;
		}
	}
	LSE_CALLCTOR CMetalShader::~CMetalShader() {
		DestroyTranslator();

		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.
	 */
	LSVOID LSE_CALL CMetalShader::Reset() {
		if ( m_pogsmShaderMan ) {
			// Unattach this shader from the manager.
			CMetalShaderManager * pogsmMan = m_pogsmShaderMan;
			m_pogsmShaderMan = NULL;
			pogsmMan->SetShader( NULL );
		}
		// Destroy the shader program.
		CMetalShaderProgramManager::DeleteShaderProgram( m_pospProgram );
		m_pospProgram = NULL;

		// Dereference shader parts.
		for ( LSUINT32 I = LSG_ST_TOTAL; I--; ) {
			CMetalCompiledShaderManager::DeleteShader( m_pogcsShaders[I] );
			m_pogcsShaders[I] = NULL;
		}
		Parent::Reset();
	}

	/**
	 * Gets the uniqe ID of this shader.  The ID is 0 if the shader is not actually ready
	 *	to be activated (that is, has not been compiled and registered with the graphics API).
	 * Activating a shader with a 0 ID causes the fixed-function pipeline to become activated
	 *	(if the fixed-function pipeline is supported).
	 *
	 * \return Returns the unique ID for this shader.
	 */
	LSUINT64 LSE_CALL CMetalShader::GetId() const {
		if ( m_pospProgram ) { return m_pospProgram->GetId(); }
		return 0ULL;
	}

	/**
	 * Activation callback.  A system value is passed that is to be used internally and changes meaning depending on the
	 *	graphics API.
	 *
	 * \param _pvSysValue A value with special system meaning.
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CMetalShader::Activate( LSVOID * _pvSysValue ) {


		return true;
	}

	/**
	 * Deactivation callback.  Calls by the system when the shader is deactivated.
	 */
	LSVOID LSE_CALL CMetalShader::Deactivate() {
		m_pogsmShaderMan = NULL;
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CMetalShader::LSG_HANDLE LSE_CALL CMetalShader::GetConstantByName( const LSCHAR * _pcName ) {
		if ( !m_pospProgram ) { return LSE_MAXU32; }
		return m_pospProgram->GetConstantByName( _pcName );
	}

	/**
	 * Sets a single scalar value in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _fValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetFloat( LSG_HANDLE _hHandle, LSFLOAT _fValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		return m_pospProgram->SetFloat( _hHandle, _fValue );
	}

	/**
	 * Sets an array of scalar values in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pfValues New values.
	 * \param _ui32Total Total floats to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetFloatArray( LSG_HANDLE _hHandle, const LSFLOAT * _pfValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetFloatArray( _hHandle, _pfValues, _ui32Total );
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetInt( LSG_HANDLE _hHandle, LSINT32 _iValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetInt( _hHandle, _iValue );
	}

	/**
	 * Sets an array of scalar values in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetIntArray( LSG_HANDLE _hHandle, const LSINT32 * _pi32Values, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetIntArray( _hHandle, _pi32Values, _ui32Total );
	}

	/**
	 * Sets a single scalar value in the shader of type bool.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _bValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetBool( LSG_HANDLE _hHandle, LSBOOL _bValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetInt( _hHandle, _bValue != false );
	}

	/**
	 * Sets an array of scalar values in the shader of type bool.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pbValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetBoolArray( LSG_HANDLE _hHandle, const LSBOOL * _pbValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetBoolArray( _hHandle, _pbValues, _ui32Total );
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector2( LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetVector2( _hHandle, _vValue );
	}

	/**
	 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector2Array( LSG_HANDLE _hHandle, const CVector2 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetVector2Array( _hHandle, _pvValues, _ui32Total );
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector3( LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetVector3( _hHandle, _vValue );
	}

	/**
	 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector3Array( LSG_HANDLE _hHandle, const CVector3 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetVector3Array( _hHandle, _pvValues, _ui32Total );
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector4( LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }

		return m_pospProgram->SetVector4( _hHandle, _vValue );
	}

	/**
	 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetVector4Array( LSG_HANDLE _hHandle, const CVector4 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetVector4Array( _hHandle, _pvValues, _ui32Total );
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetMatrix2x2( LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetMatrix2x2( _hHandle, _mValue );
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetMatrix3x3( LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetMatrix3x3( _hHandle, _mValue );
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetMatrix4x4( LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetMatrix4x4( _hHandle, _mValue );
	}

	/**
	 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pmValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShader::SetMatrix4x4Array( LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetMatrix4x4Array( _hHandle, _pmValues, _ui32Total );
	}

	/**
	 * Creates a new translator of type CMetalShaderTranslator.
	 */
	LSVOID LSE_CALL CMetalShader::CreateTranslator() {
		DestroyTranslator();
		CMetalShaderTranslator * pogstNew = LSENEW CMetalShaderTranslator();
		m_pstpbTranslator = pogstNew;
	}

	/**
	 * Destroys the CMetalShaderTranslator translator.
	 */
	LSVOID LSE_CALL CMetalShader::DestroyTranslator() {
		if ( m_pstpbTranslator ) {
			CMetalShaderTranslator * pogstDelete = static_cast<CMetalShaderTranslator *>(m_pstpbTranslator);
			m_pstpbTranslator = NULL;
			LSEDELETE pogstDelete;
		}
	}

	/**
	 * Post compile.  We use this to create Metal compiled shader parts and link them to
	 *	create a shader program.
	 *
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CMetalShader::PostCompile( CString &_sError ) {
		if ( m_sVertexShader.Length() ) {
			m_pogcsShaders[LSG_ST_VERTEX] = CMetalCompiledShaderManager::CreateShader( m_sFinalVertex, LSG_ST_VERTEX, _sError );
			if ( !m_pogcsShaders[LSG_ST_VERTEX] ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		if ( m_sPixelShader.Length() ) {
			m_pogcsShaders[LSG_ST_PIXEL] = CMetalCompiledShaderManager::CreateShader( m_sFinalPixel, LSG_ST_PIXEL, _sError );
			if ( !m_pogcsShaders[LSG_ST_PIXEL] ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		// Make a shader program from these parts.
		m_pospProgram = CMetalShaderProgramManager::CreateShaderProgram( m_pogcsShaders[LSG_ST_VERTEX],
			m_pogcsShaders[LSG_ST_PIXEL],
			NULL,
			m_ulUniforms,
			m_ulAttributes,
			_sError );
		if ( !m_pospProgram ) {
			return LSG_SE_GFXAPIERROR;
		}

		return LSG_SE_SUCCESS;
	}

}	// namespace lsg

#endif	// #if defined( LSG_METAL )
