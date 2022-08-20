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
 * Description: In Metal, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
 *	exactly alike.
 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
 */

#include "LSGMetalShaderProgramManager.h"

#if defined( LSG_METAL )

#include "../Gfx/LSGGfx.h"
#include "../MetalVertexBuffer/LSGMetalVertexBuffer.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalShaderProgramManager::CMetalShaderProgram::CMetalShaderProgram() :
		m_uiProgram( 0 ),
		m_ui32Ref( 0UL ),
		m_ui32NextUniformStart( 0UL ) {
		for ( LSUINT32 I = LSG_ST_TOTAL; I--; ) {
			m_pogcsShader[I] = NULL;
		}
	}
	LSE_CALLCTOR CMetalShaderProgramManager::CMetalShaderProgram::~CMetalShaderProgram() {
		assert( m_ui32Ref == 0UL );

	}

	// == Functions.
	/**
	 * Add a shader.
	 *
	 * \param _pogcsShader Pointer to the vertex shader to add to this program.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::AddShader( CMetalCompiledShader * _pogcsShader ) {
		if ( !_pogcsShader ) { return; }
		// The slot is determined by the shader type.  One shader per type.
		m_pogcsShader[_pogcsShader->GetType()] = _pogcsShader;
	}

	/**
	 * Gets a shader in a given slot.
	 *
	 * \param _stShaderType The type of shader to get from this object.
	 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
	 */
	CMetalCompiledShader * LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::GetShader( LSG_SHADER_TYPES _stShaderType ) {
		return m_pogcsShader[_stShaderType];
	}

	/**
	 * Gets a shader in a given slot.
	 *
	 * \param _stShaderType The type of shader to get from this object.
	 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
	 */
	const CMetalCompiledShader * LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::GetShader( LSG_SHADER_TYPES _stShaderType ) const {
		return m_pogcsShader[_stShaderType];
	}

	/**
	 * Finalize the shader for use with Metal.
	 *
	 * \param _pulUniforms Uniforms in the shader.
	 * \param _palAttributes Attributes in the shader.
	 * \param _sError Filled with the error report on failure.
	 * \return Returns true if there are no compilation/linkage errors.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::Compile( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
		const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
		CString &_sError ) {
		// Must only be done once.
		assert( m_uiProgram == 0 );
		return true;
	}

	/**
	 * Activate this shader.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::Activate() {
#ifdef _DEBUG
		
#endif	// #ifdef _DEBUG
	}

	/**
	 * Gets the reference count for this object.
	 *
	 * \return Returns the reference count for this object.
	 */
	LSUINT32 LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::GetRef() const {
		return m_ui32Ref;
	}

	/**
	 * Gets the unique ID of this object.  This matches the Metal shader program ID, which may be 0.
	 *
	 * \return Returns the unique ID for this object.
	 */
	LSUINT32 LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::GetId() const {
		return static_cast<LSUINT32>(m_uiProgram);
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CShaderBase::LSG_HANDLE LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::GetConstantByName( const LSCHAR * _pcName ) {
		if ( m_uiProgram == 0UL ) { return LSE_MAXU32; }
		// Have we already given this handle out?
		for ( LSUINT32 I = 0UL; I < m_vUniformDataMap.Length(); ++I ) {
			if ( CStd::StrCmp( m_ulUniforms[m_vUniformDataMap[I].stType][m_vUniformDataMap[I].ui32Name].sName.CStr(), _pcName ) == 0 ) {
				return I;
			}
		}
		// Have to make a new one.
		for ( LSUINT32 I = 0UL; I < LSE_ELEMENTS( m_ulUniforms ); ++I ) {
			for ( LSUINT32 J = m_ulUniforms[I].Length(); J--; ) {
				const LSCHAR * pcThisName = m_ulUniforms[I][J].sName.CStr();
				if ( CStd::StrCmp( pcThisName, _pcName ) == 0 ) {
					// Make a new handle.
					LSUINT32 ui32Res = m_vUniformDataMap.Length();
					
					// Check that the uniform buffer is large enough (should always be the case).
					LSUINT32 ui32DataRemaining = m_vUniformBuffer.Length() - m_ui32NextUniformStart;
					if ( m_ulUniforms[I][J].ui32Size > ui32DataRemaining ) {
						// An impossible case, but luckily we can resize the buffer.
						if ( !m_vUniformBuffer.Resize( m_ui32NextUniformStart + m_ulUniforms[I][J].ui32Size ) ) {
							return LSE_MAXU32;
						}
					}

					// Prepare an entry.
					LSG_UNIFORM_MAP umEntry;
					umEntry.bDirty = true;
					umEntry.stType = static_cast<LSG_SHADER_TYPES>(I);
					umEntry.ui32DataPtr = m_ui32NextUniformStart;
					umEntry.ui32Name = J;
					/*umEntry.uiptrApiId = ::glGetUniformLocation( m_uiProgram, _pcName );
					if ( static_cast<GLint>(umEntry.uiptrApiId) == -1 ) {
						return LSE_MAXU32;
					}*/
					if ( !m_vUniformDataMap.Push( umEntry ) ) {
						return LSE_MAXU32;
					}
					m_ui32NextUniformStart += m_ulUniforms[I][J].ui32Size;
					// The index of that entry is the handle we give back.
					return ui32Res;
				}
			}
		}
		return LSE_MAXU32;
	}

	/**
	 * Sets a single scalar value in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _fValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetFloat( CShaderBase::LSG_HANDLE _hHandle, LSFLOAT _fValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<LSFLOAT *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _fValue ) { return true; }
		m_vUniformDataMap[_hHandle].bDirty = false;
		(*reinterpret_cast<LSFLOAT *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _fValue;
		
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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const LSFLOAT * _pfValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pfValues, sizeof( LSFLOAT ) * _ui32Total ) ) { return true; }
		m_vUniformDataMap[_hHandle].bDirty = false;
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pfValues, sizeof( LSFLOAT ) * _ui32Total );

		return true;
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetInt( CShaderBase::LSG_HANDLE _hHandle, LSINT32 _iValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<LSINT32 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _iValue ) { return true; }
		m_vUniformDataMap[_hHandle].bDirty = false;
		(*reinterpret_cast<LSINT32 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _iValue;

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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const LSINT32 * _pi32Values, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( LSINT32 ) * _ui32Total ) ) { return true; }
		m_vUniformDataMap[_hHandle].bDirty = false;
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( LSINT32 ) * _ui32Total );

		return true;
	}

	/**
	 * Sets an array of boolean values in the shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pbValues, sizeof( LSBOOL ) * _ui32Total ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pbValues, sizeof( LSBOOL ) * _ui32Total );

		return true;
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		(*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		LSFLOAT fTestMe[2] = {
			static_cast<LSFLOAT>(_vValue.x),
			static_cast<LSFLOAT>(_vValue.y)
		};
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector2 ) * _ui32Total ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector2 ) * _ui32Total );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 2UL * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( LSUINT32 I = 0UL; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT


		return true;
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		(*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		LSFLOAT fTestMe[3] = {
			static_cast<LSFLOAT>(_vValue.x),
			static_cast<LSFLOAT>(_vValue.y),
			static_cast<LSFLOAT>(_vValue.z),
		};
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector3 ) * _ui32Total ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector3 ) * _ui32Total );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 3UL * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( LSUINT32 I = 0UL; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

		return true;
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		(*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		LSFLOAT fTestMe[4] = {
			static_cast<LSFLOAT>(_vValue.x),
			static_cast<LSFLOAT>(_vValue.y),
			static_cast<LSFLOAT>(_vValue.z),
			static_cast<LSFLOAT>(_vValue.w),
		};
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 4UL * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( LSUINT32 I = 0UL; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

		return true;
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix2x2 ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix2x2 ) );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 2UL * 2UL;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_mValue);
		// Copy into the buffer.
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning) = static_cast<GLfloat>(*pfSource);

		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

		return true;
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix3x3 ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix3x3 ) );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 3UL * 3UL;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_mValue);
		// Copy into the buffer.
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);

		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

		return true;
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) );
		

#else
		LSUINT32 ui32NewSize = sizeof( GLfloat ) * 4UL * 4UL;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_mValue);
		// Copy into the buffer.
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		(*pfBeginning) = static_cast<GLfloat>(*pfSource);

		// Work based off pfUseMe.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		

#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

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
	LSBOOL LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, LSUINT32 _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total ) ) { return true; }
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total );
		

#else
#error "Add me."
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;

		return true;
	}

	/**
	 * Increase the reference count on this object.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::IncRef() {
		++m_ui32Ref;
	}

	/**
	 * Decrease the reference count on this object.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::DecRef() {
		--m_ui32Ref;
	}

	/**
	 * Converts from the shader CShaderParser semantic type to a vertex buffer attribute type.
	 *
	 * \param _ui32Semantic The shader semantic type.
	 * \return Returns the converted attribute type.
	 */
	LSUINT32 LSE_CALL CMetalShaderProgramManager::CMetalShaderProgram::ShaderSemanticToVertexAttribute( LSUINT32 _ui32Semantic ) {
		switch ( _ui32Semantic ) {
			case CShaderParser::token::LSG_SLT_POSITION : { return LSG_S_POSITION; }
			case CShaderParser::token::LSG_SLT_NORMAL : { return LSG_S_NORMAL; }
			case CShaderParser::token::LSG_SLT_COLOR : { return LSG_S_COLOR; }
			case CShaderParser::token::LSG_SLT_TEXCOORD : { return LSG_S_TEXCOORD; }
			case CShaderParser::token::LSG_SLT_BINORMAL : { return LSG_S_BINORMAL; }
			case CShaderParser::token::LSG_SLT_TANGENT : { return LSG_S_TANGENT; }
			case CShaderParser::token::LSG_SLT_BLENDWEIGHT : { return LSG_S_BLENDWEIGHT; }
			case CShaderParser::token::LSG_SLT_BLENDINDICES : { return LSG_S_BLENDINDICES; }
		}
		return LSE_MAXU32;
	}

	// == Various constructors.
	LSE_CALLCTOR CMetalShaderProgramManager::LSG_SHADER_PROGRAM_KEY::LSG_SHADER_PROGRAM_KEY() :
		ui32VertexId( 0UL ),
		ui32PixelId( 0UL ),
		ui32GeometryId( 0UL ) {
	}
	
	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _spkOther The key against which to test.
	 * \return Returns true if this object is less than the given object.
	 */
		LSBOOL LSE_CALL CMetalShaderProgramManager::LSG_SHADER_PROGRAM_KEY::operator < ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const {
		if ( ui32VertexId < _spkOther.ui32VertexId ) {
			return true;
		}
		else if ( ui32VertexId > _spkOther.ui32VertexId ) {
			return false;
		}

		if ( ui32PixelId < _spkOther.ui32PixelId ) {
			return true;
		}
		else if ( ui32PixelId > _spkOther.ui32PixelId ) {
			return false;
		}

		if ( ui32GeometryId < _spkOther.ui32GeometryId ) {
			return true;
		}
		else if ( ui32GeometryId > _spkOther.ui32GeometryId ) {
			return false;
		}
		// They are the same.
		return false;
	}

	/**
	 * Equality operator.
	 *
	 * \param _spkOther The key against which to test.
	 * \return Returns true if this object is equal to the given object.
	 */
	LSBOOL LSE_CALL CMetalShaderProgramManager::LSG_SHADER_PROGRAM_KEY::operator == ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const {
		return (ui32VertexId == _spkOther.ui32VertexId) &&
			(ui32PixelId == _spkOther.ui32PixelId) &&
			(ui32GeometryId == _spkOther.ui32GeometryId);
	}

	// == Members.
	// Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
	LSBOOL CMetalShaderProgramManager::m_bSharing = true;

	// Map of shader programs.
	CLinearMap<CMetalShaderProgramManager::LSG_SHADER_PROGRAM_KEY, CMetalShaderProgramManager::CMetalShaderProgram *> CMetalShaderProgramManager::m_mShaders;

	// == Functions.
	/**
	 * Sets sharing.
	 *
	 * \param _bVal If true, sharing is enabled, otherwise not.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::SetSharing( LSBOOL _bVal ) {
		m_bSharing = _bVal;
	}

	/**
	 * Creates a shader program given various shader parts, any of which may be NULL (but not all of which).  If
	 *	sharing is enabled, the shader may be shared, otherwise each returned shader program will be unique.
	 *
	 * \param _pogcsVertexShader The vertex shader.
	 * \param _pogcsPixelShader The pixel shader.
	 * \param _pogcsGeometryShader The geometry shader.
	 * \param _pulUniforms Uniforms in the shader.
	 * \param _palAttributes Attributes in the shader.
	 * \param _sError Filled with an error report in the event of an error.
	 * \return Returns a shader program created from the given shader parts.
	 */
	CMetalShaderProgramManager::CMetalShaderProgram * LSE_CALL CMetalShaderProgramManager::CreateShaderProgram( CMetalCompiledShader * _pogcsVertexShader,
		CMetalCompiledShader * _pogcsPixelShader, CMetalCompiledShader * _pogcsGeometryShader,
		const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
		const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
		CString &_sError ) {
		if ( !m_bSharing ) {
			CMetalShaderProgram * pogspRet = LSENEW CMetalShaderProgram();
			if ( !pogspRet ) {
				_sError = "Failed to create shader program: Not enough memory.";
				return NULL;
			}

			pogspRet->AddShader( _pogcsVertexShader );
			pogspRet->AddShader( _pogcsPixelShader );
			pogspRet->AddShader( _pogcsGeometryShader );
			if ( !pogspRet->Compile( _pulUniforms, _palAttributes, _sError ) ) {
				LSEDELETE pogspRet;
				pogspRet = NULL;
			}
			else {
				pogspRet->IncRef();
			}

			return pogspRet;
		}

		// See if it exists.
		CMetalShaderProgram ogspTemp;
		ogspTemp.AddShader( _pogcsVertexShader );
		ogspTemp.AddShader( _pogcsPixelShader );
		ogspTemp.AddShader( _pogcsGeometryShader );
		LSUINT32 ui32Index;
		if ( m_mShaders.GetItemIndex( CreateKey( ogspTemp ), ui32Index ) ) {
			// Exists already.  Return it.
			m_mShaders.GetByIndex( ui32Index )->IncRef();
			return m_mShaders.GetByIndex( ui32Index );
		}

		// Does not exist, so add it.
		CMetalShaderProgram * pogspNew = LSENEW CMetalShaderProgram();
		if ( !pogspNew ) {
			_sError = "Failed to create shader program: Not enough memory.";
			return NULL;
		}
		pogspNew->AddShader( _pogcsVertexShader );
		pogspNew->AddShader( _pogcsPixelShader );
		pogspNew->AddShader( _pogcsGeometryShader );
		if ( !pogspNew->Compile( _pulUniforms, _palAttributes, _sError ) ) {
			LSEDELETE pogspNew;
			return NULL;
		}
		if ( !m_mShaders.Insert( CreateKey( (*pogspNew) ), pogspNew ) ) {
			LSEDELETE pogspNew;
			_sError = "Failed to create shader program: Not enough memory.";
			return NULL;
		}

		pogspNew->IncRef();
		return pogspNew;
	}

	/**
	 * Delete a shader program.  If sharing, its reference count is decreased only.  Otherwise it is fully deleted from memory.
	 *	Must have been created with a previous call to CreateShaderProgram();
	 *
	 * \param _pogspProgram The shader program to delete.
	 */
	LSVOID LSE_CALL CMetalShaderProgramManager::DeleteShaderProgram( CMetalShaderProgram * _pogspProgram ) {
		if ( !_pogspProgram ) { return; }
		if ( !m_bSharing ) {
			_pogspProgram->DecRef();
			LSEDELETE _pogspProgram;
			return;
		}

		for ( LSUINT32 I = m_mShaders.Length(); I--; ) {
			if ( m_mShaders.GetByIndex( I ) == _pogspProgram ) {
				_pogspProgram->DecRef();
				return;
			}
		}

		// Pointer not found.  This shader was not created by this class, which is an error.
		CStd::DebugPrintA( "CMetalShaderProgramManager::DeleteShaderProgram(): Given shader program was not created by CMetalShaderProgramManager::CreateShaderProgram().\r\n" );
	}

	/**
	 * Delete all shader programs that have a 0 reference count.  Returns the number of shader programs not deleted.
	 *
	 * \return Returns the number of shader programs not deleted. 
	 */
	LSUINT32 LSE_CALL CMetalShaderProgramManager::DeleteUnreferencedShaderPrograms() {
		LSUINT32 ui32Ret = 0UL;
		for ( LSUINT32 I = m_mShaders.Length(); I--; ) {
			if ( m_mShaders.GetByIndex( I )->GetRef() ) {
				++ui32Ret;
			}
			else {
				LSEDELETE m_mShaders.GetByIndex( I );
				m_mShaders.RemoveByIndexNoDealloc( I );
			}
		}

		if ( !m_mShaders.Length() ) { m_mShaders.Reset(); }
		return ui32Ret;
	}

	/**
	 * Creates a key from a shader program.
	 *
	 * \param _ogspProgram The program from which to create a key after its shaders have been set.
	 * \return Returns the key describing the shader program.
	 */
	CMetalShaderProgramManager::LSG_SHADER_PROGRAM_KEY LSE_CALL CMetalShaderProgramManager::CreateKey( const CMetalShaderProgram &_ogspProgram ) {
		LSG_SHADER_PROGRAM_KEY spkKey;
		spkKey.ui32VertexId = static_cast<LSUINT32>(_ogspProgram.GetShader( LSG_ST_VERTEX ) ? _ogspProgram.GetShader( LSG_ST_VERTEX )->GetId() : 0UL);
		spkKey.ui32PixelId = static_cast<LSUINT32>(_ogspProgram.GetShader( LSG_ST_PIXEL ) ? _ogspProgram.GetShader( LSG_ST_PIXEL )->GetId() : 0UL);
		spkKey.ui32GeometryId = 0UL;
		return spkKey;
	}

}	// namespace lsg

#endif	// #if defined( LSG_METAL )
