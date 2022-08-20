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
 * Description: In OpenGL, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
 *	exactly alike.
 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
 */

#include "LSGOpenGlShaderProgramManager.h"

#if defined( LSG_OGL )

#include "../Gfx/LSGGfx.h"
#include "../OpenGLVertexBuffer/LSGOpenGlVertexBuffer.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlShaderProgramManager::COpenGlShaderProgram::COpenGlShaderProgram() :
		m_uiProgram( 0 ),
		m_paulLocs( NULL ),
		m_ui32Attribs( 0 ),
		m_ui32Ref( 0 ),
		m_ui32NextUniformStart( 0 ) {
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			m_pogcsShader[I] = NULL;
		}
	}
	LSE_CALLCTOR COpenGlShaderProgramManager::COpenGlShaderProgram::~COpenGlShaderProgram() {
		assert( m_ui32Ref == 0 );
		if ( m_uiProgram ) {
			COpenGl::glDeleteShader( m_uiProgram );
			m_uiProgram = 0;
		}

		LSEDELETE [] m_paulLocs;
		m_paulLocs = NULL;
	}

	// == Functions.
	/**
	 * Adds a shader.
	 *
	 * \param _pogcsShader Pointer to the vertex shader to add to this program.
	 */
	void LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::AddShader( COpenGlCompiledShader * _pogcsShader ) {
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
	COpenGlCompiledShader * LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetShader( LSG_SHADER_TYPES _stShaderType ) {
		return m_pogcsShader[_stShaderType];
	}

	/**
	 * Gets a shader in a given slot.
	 *
	 * \param _stShaderType The type of shader to get from this object.
	 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
	 */
	const COpenGlCompiledShader * LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetShader( LSG_SHADER_TYPES _stShaderType ) const {
		return m_pogcsShader[_stShaderType];
	}

	/**
	 * Finalize the shader for use with OpenGL.
	 *
	 * \param _pulUniforms Uniforms in the shader.
	 * \param _palAttributes Attributes in the shader.
	 * \param _sError Filled with the error report on failure.
	 * \return Returns true if there are no compilation/linkage errors.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::Compile( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
		const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
		CString &_sError ) {
		// Must only be done once.
		assert( m_uiProgram == 0 );
		CCriticalSection::CLocker lGetError( COpenGl::m_csCrit );
		// Clear the error flag.
		if ( ::glGetError() != GL_NO_ERROR ) {
			CStd::DebugPrintA( "OpenGL threw an uncaught error before the creation of a shader program.\r\n" );
		}


		// Create a table of attribute locations.  There is a max of LSG_S_TOTAL attributes and each attribute can
		//	have up to LSG_MAX_VERTEX_SEMANTIC usages.  We could just make a LSG_S_TOTAL-by-LSG_MAX_VERTEX_SEMANTIC table but there are likely
		//	to be fewer attributes than LSG_S_TOTAL so count them and make a table as small as possible.
		// The purpose here is to provide fast hook-ups to vertex shaders, so we only care about the attributes in the vertex shader.
		// Here we just get the length of the array.  If there is an error, this is an easy place to error out.
		m_ui32Attribs = 0;
		for ( uint32_t I = _palAttributes[LSG_ST_VERTEX].Length(); I--; ) {
			if ( !_palAttributes[LSG_ST_VERTEX][I].bInput ) { continue; }
			uint32_t ui32This = ShaderSemanticToVertexAttribute( _palAttributes[LSG_ST_VERTEX][I].ui32SemanticType );
			if ( ui32This == LSE_MAXU32 ) {
				_sError = "Vertex shader input semantic invalid.";
				return false;
			}
			m_ui32Attribs = CStd::Max( m_ui32Attribs, ui32This );
		}
		++m_ui32Attribs;

		m_uiProgram = COpenGl::glCreateProgram();

		if ( !m_uiProgram ) {
			_sError = "Failed to create shader program: Out of memory.";
			return false;
		}


		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			if ( m_pogcsShader[I] ) {
				COpenGl::glAttachShader( m_uiProgram, m_pogcsShader[I]->GetHandle() );
				switch ( ::glGetError() ) {
					case GL_NO_ERROR : { break; }
					default : {
						_sError = "Failed to attach shader.";
						return false;
					}
				}
			}
		}
		


		// Linking the shader is the only part that actually has a risk of failure.
		COpenGl::glLinkProgram( m_uiProgram );

		// Linker errors have to be checked via the shader log.
		GLint iLinkStatus;
		COpenGl::glGetProgramiv( m_uiProgram, GL_LINK_STATUS, &iLinkStatus );
		if ( iLinkStatus == GL_FALSE ) {
			// Get the length of the error.
			GLint iLength;
			COpenGl::glGetProgramiv( m_uiProgram, GL_INFO_LOG_LENGTH, &iLength );
			if ( iLength > 1 ) {
				// There will be an error log.  Pass it up.
				GLchar * pcText = LSENEW GLchar[iLength+1];
				GLsizei siWritten;
				COpenGl::glGetProgramInfoLog( m_uiProgram, iLength, &siWritten, pcText );
				_sError = static_cast<const char *>(pcText);
				LSEDELETE [] pcText;
			}
			else {
				_sError = "Error creating OpenGL shader program: Unspecified linker error.";
			}
			return false;
		}

		// Print warnings to the debug console.
		GLint iLength;
		COpenGl::glGetProgramiv( m_uiProgram, GL_INFO_LOG_LENGTH, &iLength );
		if ( iLength > 1 ) {
			GLchar * pcText = LSENEW GLchar[iLength+1];
			GLsizei siWritten;
			COpenGl::glGetProgramInfoLog( m_uiProgram, iLength, &siWritten, pcText );
			CStd::DebugPrintA( pcText );
			CStd::DebugPrintA( "\r\n" );
			LSEDELETE [] pcText;
		}

		// Copy the semantics.
		for ( uint32_t I = LSG_ST_TOTAL; I--; ) {
			m_ulUniforms[I] = _pulUniforms[I];
		}

		// Add the data sizes.
		uint32_t ui32DataSize = 0;
		for ( uint32_t J = LSG_ST_TOTAL; J--; ) {
			for ( uint32_t I = m_ulUniforms[J].Length(); I--; ) {
				ui32DataSize += m_ulUniforms[J][I].ui32Size;
			}
		}
		m_ui32NextUniformStart = 0;
		if ( !m_vUniformBuffer.Resize( ui32DataSize ) ) {
			_sError = "Failed to create local uniform buffer: Not enough memory.";
			return false;
		}

		m_vUniformDataMap.ResetNoDealloc();



		// Create the attribute ID table.
		m_paulLocs = LSENEW LSG_ATTRIBUTE_USAGE_LOCS[m_ui32Attribs];
		CStd::MemSet( m_paulLocs, 0xFF, sizeof( LSG_ATTRIBUTE_USAGE_LOCS ) * m_ui32Attribs );
		for ( uint32_t I = _palAttributes[LSG_ST_VERTEX].Length(); I--; ) {
			if ( !_palAttributes[LSG_ST_VERTEX][I].bInput ) { continue; }
			uint32_t ui32This = ShaderSemanticToVertexAttribute( _palAttributes[LSG_ST_VERTEX][I].ui32SemanticType );
			uint32_t ui32Index = _palAttributes[LSG_ST_VERTEX][I].ui32SemanticIndex;

			m_paulLocs[ui32This].iLoc[ui32Index] = COpenGl::glGetAttribLocation( m_uiProgram, _palAttributes[LSG_ST_VERTEX][I].sName.CStr() );
			glWarnError( "Uncaught" );	// Clear error flag.
			// Failing to get attribute locations just means the attribute is not used.  No need to exit.
			if ( m_paulLocs[ui32This].iLoc[ui32Index] != -1 ) {
				/*if ( m_paulLocs[ui32This].iLoc[ui32Index] == -1 ) {
					_sError = CString( "Failed to get the location of attribute " ) + _palAttributes[LSG_ST_VERTEX][I].sName + CString( "." );
					return false;
				}*/
				/*if ( ui32This == LSG_S_POSITION ) {
					m_paulLocs[LSG_S_POSITIONW].iLoc[ui32Index] = m_paulLocs[LSG_S_POSITION].iLoc[ui32Index];
				}
				if ( ui32This == LSG_S_COLOR ) {
					m_paulLocs[LSG_S_ALPHA].iLoc[ui32Index] = m_paulLocs[LSG_S_COLOR].iLoc[ui32Index];
				}*/
			}
		}

		return true;
	}

	/**
	 * Activate this shader.
	 */
	void LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::Activate() {
#ifdef _DEBUG
		COpenGl::glValidateProgram( m_uiProgram );

		GLint iValidateStatus;
		COpenGl::glGetProgramiv( m_uiProgram, GL_VALIDATE_STATUS, &iValidateStatus );
		if ( iValidateStatus == GL_FALSE ) {
			// Get the length of the error.
			GLint iLength;
			COpenGl::glGetProgramiv( m_uiProgram, GL_INFO_LOG_LENGTH, &iLength );
			if ( iLength > 1 ) {
				// There will be an error log.  Pass it up.
				GLchar * pcText = LSENEW GLchar[iLength+1];
				GLsizei siWritten;
				COpenGl::glGetProgramInfoLog( m_uiProgram, iLength, &siWritten, pcText );
				CStd::DebugPrintA( pcText );
				CStd::DebugPrintA( "\r\n" );
				LSEDELETE [] pcText;
			}
		}
#endif	// #ifdef _DEBUG
	}

	/**
	 * Gets the reference count for this object.
	 *
	 * \return Returns the reference count for this object.
	 */
	uint32_t LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetRef() const {
		return m_ui32Ref;
	}

	/**
	 * Gets the unique ID of this object.  This matches the OpenGL shader program ID, which may be 0.
	 *
	 * \return Returns the unique ID for this object.
	 */
	uint32_t LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetId() const {
		return static_cast<uint32_t>(m_uiProgram);
	}

	/**
	 * Gets the attribute locations.
	 *
	 * \return Returns a pointer to the attribute locations for this shader.
	 */
	const COpenGlShaderProgramManager::LSG_ATTRIBUTE_USAGE_LOCS * LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetAttributeLocs() const {
		return m_paulLocs;
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CShaderBase::LSG_HANDLE LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::GetConstantByName( const char * _pcName ) {
		if ( m_uiProgram == 0 ) { return LSE_MAXU32; }
		// Have we already given this handle out?
		for ( uint32_t I = 0; I < m_vUniformDataMap.Length(); ++I ) {
			if ( CStd::StrCmp( m_ulUniforms[m_vUniformDataMap[I].stType][m_vUniformDataMap[I].ui32Name].sName.CStr(), _pcName ) == 0 ) {
				return I;
			}
		}
		// Have to make a new one.
		for ( uint32_t I = 0; I < LSE_ELEMENTS( m_ulUniforms ); ++I ) {
			for ( uint32_t J = m_ulUniforms[I].Length(); J--; ) {
				const char * pcThisName = m_ulUniforms[I][J].sName.CStr();
				if ( CStd::StrCmp( pcThisName, _pcName ) == 0 ) {
					// Make a new handle.
					uint32_t ui32Res = m_vUniformDataMap.Length();
					
					// Check that the uniform buffer is large enough (should always be the case).
					uint32_t ui32DataRemaining = m_vUniformBuffer.Length() - m_ui32NextUniformStart;
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
					umEntry.uiptrApiId = COpenGl::glGetUniformLocation( m_uiProgram, _pcName );
					if ( static_cast<GLint>(umEntry.uiptrApiId) == -1 ) {
						return LSE_MAXU32;
					}
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );

#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<float *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _fValue ) { return true; }
#endif	// #ifndef LSA_TOOLS
		m_vUniformDataMap[_hHandle].bDirty = false;
		(*reinterpret_cast<float *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _fValue;
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform1f( iApiHandle, static_cast<GLfloat>(_fValue) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		assert( sizeof( float ) == sizeof( GLfloat ) );	// Well just in case some strange system pops up in the future.
		glWarnError( "Uncaught" );

#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pfValues, sizeof( float ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		m_vUniformDataMap[_hHandle].bDirty = false;
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pfValues, sizeof( float ) * _ui32Total );

		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform1fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			static_cast<const GLfloat *>(_pfValues) );
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetInt( CShaderBase::LSG_HANDLE _hHandle, int32_t _iValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<int32_t *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _iValue ) { return true; }
#endif	// #ifndef LSA_TOOLS
		m_vUniformDataMap[_hHandle].bDirty = false;
		(*reinterpret_cast<int32_t *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _iValue;
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform1i( iApiHandle, static_cast<GLint>(_iValue) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		assert( sizeof( int32_t ) == sizeof( GLint ) );
		glWarnError( "Uncaught" );

#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( int32_t ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		m_vUniformDataMap[_hHandle].bDirty = false;
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( int32_t ) * _ui32Total );

		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform1iv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			static_cast<const GLint *>(_pi32Values) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		assert( sizeof( LSBOOL ) == sizeof( GLint ) );
		glWarnError( "Uncaught" );

#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pbValues, sizeof( LSBOOL ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pbValues, sizeof( LSBOOL ) * _ui32Total );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform1iv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			reinterpret_cast<const GLint *>(_pbValues) );
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
#endif	// #ifndef LSA_TOOLS
		(*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		float fTestMe[2] = {
			static_cast<float>(_vValue.x),
			static_cast<float>(_vValue.y)
		};
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform2f( iApiHandle, static_cast<GLfloat>(_vValue.x), static_cast<GLfloat>(_vValue.y) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector2 ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector2 ) * _ui32Total );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform2fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			reinterpret_cast<const GLfloat *>(_pvValues) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 2 * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform2fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
#ifndef LSA_TOOLS
		// Check for redundancy.
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
#endif	// #ifndef LSA_TOOLS
		(*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		float fTestMe[3] = {
			static_cast<float>(_vValue.x),
			static_cast<float>(_vValue.y),
			static_cast<float>(_vValue.z),
		};
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform3f( iApiHandle, static_cast<GLfloat>(_vValue.x), static_cast<GLfloat>(_vValue.y), static_cast<GLfloat>(_vValue.z) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector3 ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector3 ) * _ui32Total );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform3fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			reinterpret_cast<const GLfloat *>(_pvValues) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 3 * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform3fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && (*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
#endif	// #ifndef LSA_TOOLS
		(*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr])) = _vValue;
#else
		float fTestMe[4] = {
			static_cast<float>(_vValue.x),
			static_cast<float>(_vValue.y),
			static_cast<float>(_vValue.z),
			static_cast<float>(_vValue.w),
		};
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], fTestMe, sizeof( fTestMe ) );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform4f( iApiHandle, static_cast<GLfloat>(_vValue.x), static_cast<GLfloat>(_vValue.y), static_cast<GLfloat>(_vValue.z), static_cast<GLfloat>(_vValue.w) );
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform4fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			reinterpret_cast<const GLfloat *>(_pvValues) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 4 * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		GLfloat * pfBeginning = reinterpret_cast<GLfloat *>(&m_vTempBuffer[0]);
		GLfloat * pfUseMe = pfBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(&_pvValues);
		// Copy into the buffer.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
			(*pfBeginning++) = static_cast<GLfloat>(*pfSource++);
		}
		// Work based off pfUseMe.
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniform4fv( iApiHandle, static_cast<GLsizei>(_ui32Total),
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix2x2 ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix2x2 ) );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix2fv( iApiHandle, 1, GL_FALSE,
			reinterpret_cast<const GLfloat *>(&_mValue) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 2 * 2;
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
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix2fv( iApiHandle, 1, GL_FALSE,
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix3x3 ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix3x3 ) );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix3fv( iApiHandle, 1, GL_FALSE,
			reinterpret_cast<const GLfloat *>(&_mValue) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 3 * 3;
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
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix3fv( iApiHandle, 1, GL_FALSE,
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		glWarnError( "Uncaught" );
#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix4fv( iApiHandle, 1, GL_FALSE,
			reinterpret_cast<const GLfloat *>(&_mValue) );
#else
		uint32_t ui32NewSize = sizeof( GLfloat ) * 4 * 4;
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
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], pfUseMe, ui32NewSize );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix4fv( iApiHandle, 1, GL_FALSE,
			pfUseMe );
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) {
		if ( _hHandle == LSE_MAXU32 || _hHandle >= m_vUniformDataMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		glWarnError( "Uncaught" );

#if LSM_BASETYPE == LSM_FLOAT
#ifndef LSA_TOOLS
		if ( !m_vUniformDataMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total ) ) { return true; }
#endif	// #ifndef LSA_TOOLS
		CStd::MemCpy( &m_vUniformBuffer[m_vUniformDataMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total );
		
		GLint iApiHandle = static_cast<GLint>(m_vUniformDataMap[_hHandle].uiptrApiId);
		COpenGl::glUniformMatrix4fv( iApiHandle, _ui32Total, GL_FALSE,
			reinterpret_cast<const GLfloat *>(_pmValues) );
#else
#error "Add me."
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		m_vUniformDataMap[_hHandle].bDirty = false;
		glAssertError( "Failed to set uniform" );
		return true;
	}

	/**
	 * Increase the reference count on this object.
	 */
	void LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::IncRef() {
		++m_ui32Ref;
	}

	/**
	 * Decrease the reference count on this object.
	 */
	void LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::DecRef() {
		--m_ui32Ref;
	}

	/**
	 * Converts from the shader CShaderParser semantic type to a vertex buffer attribute type.
	 *
	 * \param _ui32Semantic The shader semantic type.
	 * \return Returns the converted attribute type.
	 */
	uint32_t LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::ShaderSemanticToVertexAttribute( uint32_t _ui32Semantic ) {
		switch ( _ui32Semantic ) {
			case CShaderParser::token::LSG_SLT_POSITION : { return LSG_S_POSITION; }
			case CShaderParser::token::LSG_SLT_POSITIONT : { return LSG_S_POSITIONT; }
			case CShaderParser::token::LSG_SLT_NORMAL : { return LSG_S_NORMAL; }
			case CShaderParser::token::LSG_SLT_COLOR : { return LSG_S_COLOR; }
			case CShaderParser::token::LSG_SLT_TEXCOORD : { return LSG_S_TEXCOORD; }
			case CShaderParser::token::LSG_SLT_BINORMAL : { return LSG_S_BINORMAL; }
			case CShaderParser::token::LSG_SLT_TANGENT : { return LSG_S_TANGENT; }
			case CShaderParser::token::LSG_SLT_BLENDWEIGHT : { return LSG_S_BLENDWEIGHT; }
			case CShaderParser::token::LSG_SLT_BLENDINDICES : { return LSG_S_BLENDINDICES; }
			case CShaderParser::token::LSG_SLT_PSIZE : { return LSG_S_PSIZE; }
		}
		return LSE_MAXU32;
	}

	// == Various constructors.
	LSE_CALLCTOR COpenGlShaderProgramManager::LSG_SHADER_PROGRAM_KEY::LSG_SHADER_PROGRAM_KEY() :
		ui32VertexId( 0 ),
		ui32PixelId( 0 ),
		ui32GeometryId( 0 ) {
	}
	
	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _spkOther The key against which to test.
	 * \return Returns true if this object is less than the given object.
	 */
		LSBOOL LSE_CALL COpenGlShaderProgramManager::LSG_SHADER_PROGRAM_KEY::operator < ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const {
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
	LSBOOL LSE_CALL COpenGlShaderProgramManager::LSG_SHADER_PROGRAM_KEY::operator == ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const {
		return (ui32VertexId == _spkOther.ui32VertexId) &&
			(ui32PixelId == _spkOther.ui32PixelId) &&
			(ui32GeometryId == _spkOther.ui32GeometryId);
	}

	// == Members.
	// Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
	LSBOOL COpenGlShaderProgramManager::m_bSharing = true;

	// Map of shader programs.
	CLinearMap<COpenGlShaderProgramManager::LSG_SHADER_PROGRAM_KEY, COpenGlShaderProgramManager::COpenGlShaderProgram *> COpenGlShaderProgramManager::m_mShaders;

	// == Functions.
	/**
	 * Sets sharing.
	 *
	 * \param _bVal If true, sharing is enabled, otherwise not.
	 */
	void LSE_CALL COpenGlShaderProgramManager::SetSharing( LSBOOL _bVal ) {
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
	COpenGlShaderProgramManager::COpenGlShaderProgram * LSE_CALL COpenGlShaderProgramManager::CreateShaderProgram( COpenGlCompiledShader * _pogcsVertexShader,
		COpenGlCompiledShader * _pogcsPixelShader, COpenGlCompiledShader * _pogcsGeometryShader,
		const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
		const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
		CString &_sError ) {
		if ( !m_bSharing ) {
			COpenGlShaderProgram * pogspRet = LSENEW COpenGlShaderProgram();
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
		COpenGlShaderProgram ogspTemp;
		ogspTemp.AddShader( _pogcsVertexShader );
		ogspTemp.AddShader( _pogcsPixelShader );
		ogspTemp.AddShader( _pogcsGeometryShader );
		uint32_t ui32Index;
		if ( m_mShaders.GetItemIndex( CreateKey( ogspTemp ), ui32Index ) ) {
			// Exists already.  Return it.
			m_mShaders.GetByIndex( ui32Index )->IncRef();
			return m_mShaders.GetByIndex( ui32Index );
		}

		// Does not exist, so add it.
		COpenGlShaderProgram * pogspNew = LSENEW COpenGlShaderProgram();
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
	void LSE_CALL COpenGlShaderProgramManager::DeleteShaderProgram( COpenGlShaderProgram * _pogspProgram ) {
		if ( !_pogspProgram ) { return; }
		if ( !m_bSharing ) {
			_pogspProgram->DecRef();
			LSEDELETE _pogspProgram;
			return;
		}

		for ( uint32_t I = m_mShaders.Length(); I--; ) {
			if ( m_mShaders.GetByIndex( I ) == _pogspProgram ) {
				_pogspProgram->DecRef();
				return;
			}
		}

		// Pointer not found.  This shader was not created by this class, which is an error.
		CStd::DebugPrintA( "COpenGlShaderProgramManager::DeleteShaderProgram(): Given shader program was not created by COpenGlShaderProgramManager::CreateShaderProgram().\r\n" );
	}

	/**
	 * Delete all shader programs that have a 0 reference count.  Returns the number of shader programs not deleted.
	 *
	 * \return Returns the number of shader programs not deleted. 
	 */
	uint32_t LSE_CALL COpenGlShaderProgramManager::DeleteUnreferencedShaderPrograms() {
		uint32_t ui32Ret = 0;
		for ( uint32_t I = m_mShaders.Length(); I--; ) {
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
	COpenGlShaderProgramManager::LSG_SHADER_PROGRAM_KEY LSE_CALL COpenGlShaderProgramManager::CreateKey( const COpenGlShaderProgram &_ogspProgram ) {
		LSG_SHADER_PROGRAM_KEY spkKey;
		spkKey.ui32VertexId = static_cast<uint32_t>(_ogspProgram.GetShader( LSG_ST_VERTEX ) ? _ogspProgram.GetShader( LSG_ST_VERTEX )->GetId() : 0);
		spkKey.ui32PixelId = static_cast<uint32_t>(_ogspProgram.GetShader( LSG_ST_PIXEL ) ? _ogspProgram.GetShader( LSG_ST_PIXEL )->GetId() : 0);
		spkKey.ui32GeometryId = 0;
		return spkKey;
	}

}	// namespace lsg

#endif	// #if defined( LSG_OGL )
