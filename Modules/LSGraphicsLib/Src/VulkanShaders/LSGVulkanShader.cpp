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
 * Description: Open Vulkan shader.  Specifically for Vulkan, after shaders are translated, they are linked
 *	into an Vulkan program object.  A shader program in Vulkan is a combination of a vertex shader, pixel
 *	shader, and geometry shader.  Each of these are called shader parts in this context, and shader parts
 *	can be shared.  For this reason, it may be common for multiple Vulkan shader programs to be comprised
 *	of the same shader parts.
 * As an optimization, this class redirects the creation of shader programs to another manager which has global
 *	scope and is capable of determining when shader programs are created from the same shader parts.  This
 *	reduces significantly the number of shader swaps that need to be made during rendering.
 */

#include "LSGVulkanShader.h"

#if defined( LSG_VKN )

#include "../Gfx/LSGGfx.h"
#include "LSGVulkanShaderManager.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanShader::CVulkanShader() :
		m_pogsmShaderMan( NULL ),
		m_pospProgram( NULL ) {
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			m_pogcsShaders[I] = NULL;
		}
	}
	LSE_CALLCTOR CVulkanShader::~CVulkanShader() {
		DestroyTranslator();

		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.
	 */
	void LSE_CALL CVulkanShader::Reset() {
		if ( m_pogsmShaderMan ) {
			// Unattach this shader from the manager.
			CVulkanShaderManager * pogsmMan = m_pogsmShaderMan;
			m_pogsmShaderMan = NULL;
			pogsmMan->SetShader( NULL );
		}
		// Destroy the shader program.
		CVulkanShaderProgramManager::DeleteShaderProgram( m_pospProgram );
		m_pospProgram = NULL;

		// Dereference shader parts.
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			CVulkanCompiledShaderManager::DeleteShader( m_pogcsShaders[I] );
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
	uint64_t LSE_CALL CVulkanShader::GetId() const {
		if ( m_pospProgram ) { return m_pospProgram->GetId(); }
		return 0ULL;
	}

	/**
	 * Gets the attribute locations for this shader.
	 *
	 * \return Returns the attribute locations for this shader.
	 */
	const CVulkanShaderProgramManager::LSG_ATTRIBUTE_USAGE_LOCS * LSE_CALL CVulkanShader::GetAttributeLocs() const {
		return m_pospProgram->GetAttributeLocs();
	}

	/**
	 * Activation callback.  A system value is passed that is to be used internally and changes meaning depending on the
	 *	graphics API.
	 *
	 * \param _pvSysValue A value with special system meaning.
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CVulkanShader::Activate( void * _pvSysValue ) {
		m_pogsmShaderMan = static_cast<CVulkanShaderManager *>(_pvSysValue);

		m_pospProgram->Activate();

		return true;
	}

	/**
	 * Deactivation callback.  Calls by the system when the shader is deactivated.
	 */
	void LSE_CALL CVulkanShader::Deactivate() {
		m_pogsmShaderMan = NULL;
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CVulkanShader::LSG_HANDLE LSE_CALL CVulkanShader::GetConstantByName( const char * _pcName ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetFloat( LSG_HANDLE _hHandle, float _fValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetFloatArray( LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetInt( LSG_HANDLE _hHandle, int32_t _iValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetIntArray( LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetBool( LSG_HANDLE _hHandle, LSBOOL _bValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetBoolArray( LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector2( LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector2Array( LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector3( LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector3Array( LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector4( LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetVector4Array( LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetMatrix2x2( LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetMatrix3x3( LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetMatrix4x4( LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
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
	LSBOOL LSE_CALL CVulkanShader::SetMatrix4x4Array( LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || !m_pospProgram ) { return false; }
		
		return m_pospProgram->SetMatrix4x4Array( _hHandle, _pmValues, _ui32Total );
	}

	/**
	 * Creates a new translator of type CVulkanShaderTranslator.
	 */
	void LSE_CALL CVulkanShader::CreateTranslator() {
		DestroyTranslator();
		CVulkanShaderTranslator * pogstNew = LSENEW CVulkanShaderTranslator();
		m_pstpbTranslator = pogstNew;
	}

	/**
	 * Destroys the CVulkanShaderTranslator translator.
	 */
	void LSE_CALL CVulkanShader::DestroyTranslator() {
		if ( m_pstpbTranslator ) {
			CVulkanShaderTranslator * pogstDelete = static_cast<CVulkanShaderTranslator *>(m_pstpbTranslator);
			m_pstpbTranslator = NULL;
			LSEDELETE pogstDelete;
		}
	}

	/**
	 * Post compile.  We use this to create Vulkan compiled shader parts and link them to
	 *	create a shader program.
	 *
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CVulkanShader::PostCompile( CString &_sError ) {
		if ( m_sVertexShader.Length() ) {
			m_pogcsShaders[LSG_ST_VERTEX] = CVulkanCompiledShaderManager::CreateShader( m_sFinalVertex, LSG_ST_VERTEX, _sError );
			if ( !m_pogcsShaders[LSG_ST_VERTEX] ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		if ( m_sPixelShader.Length() ) {
			m_pogcsShaders[LSG_ST_PIXEL] = CVulkanCompiledShaderManager::CreateShader( m_sFinalPixel, LSG_ST_PIXEL, _sError );
			if ( !m_pogcsShaders[LSG_ST_PIXEL] ) {
				return LSG_SE_GFXAPIERROR;
			}
		}

		// Make a shader program from these parts.
		m_pospProgram = CVulkanShaderProgramManager::CreateShaderProgram( m_pogcsShaders[LSG_ST_VERTEX],
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

#endif	// #if defined( LSG_VKN )
