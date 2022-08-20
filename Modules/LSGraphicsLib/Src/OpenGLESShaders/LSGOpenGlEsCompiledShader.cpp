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
 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for OpenGL ES.  After being compiled, the
 *	managing shader object will perform the linking.
 */

#include "LSGOpenGlEsCompiledShader.h"

#if defined( LSG_OGLES )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsCompiledShader::COpenGlEsCompiledShader() :
		m_hShaderHandle( 0 ) {
	}
	LSE_CALLCTOR COpenGlEsCompiledShader::~COpenGlEsCompiledShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL COpenGlEsCompiledShader::Reset() {
		if ( m_hShaderHandle ) {
			::glDeleteShader( m_hShaderHandle );
			m_hShaderHandle = 0;
		}
		Parent::Reset();
	}

	/**
	 * Gets our OpenGL ES handle.
	 *
	 * \return Returns the OpenGL ES handle for this object.
	 */
	GLuint LSE_CALL COpenGlEsCompiledShader::GetHandle() const {
		return m_hShaderHandle;
	}

	/**
	 * Converts a shader from our system type to the OpenGL ES type.
	 *
	 * \param _stShaderType The system shader type to convert.
	 * \return Returns the OpenGL ES equivilent of the shader type.
	 */
	uint32_t LSE_CALL COpenGlEsCompiledShader::ToOpenGlType( LSG_SHADER_TYPES _stShaderType ) {
		switch ( _stShaderType ) {
			case LSG_ST_VERTEX : { return GL_VERTEX_SHADER; }
			case LSG_ST_PIXEL : { return GL_FRAGMENT_SHADER; }
			default : {}
		}
		assert( false );
		return 0UL;
	}
	
	/**
	 * Prints any shader compiler errors.
	 *
	 * \param _uiShader The shader to check.
	 * \param _sError Holds the returned error string.
	 * \return Returns true if there were no shader errors to report.
	 */
	LSBOOL LSE_CALL COpenGlEsCompiledShader::PrintShaderError( GLuint _uiShader, CString &_sError ) {
		// Compiler errors have to be checked via the shader log.
		GLint iStatus;
		::glGetShaderiv( _uiShader, GL_COMPILE_STATUS, &iStatus);
		if ( iStatus == GL_FALSE ) {
			GLint iLength;
			::glGetShaderiv( _uiShader, GL_INFO_LOG_LENGTH, &iLength );
			if ( iLength > 1 ) {
				GLchar * pcText = LSENEW GLchar[iLength+1];
				GLsizei siWritten;
				::glGetShaderInfoLog( _uiShader, iLength, &siWritten, pcText );
				_sError = static_cast<const char *>(pcText);
				LSEDELETE [] pcText;
			}
			else {
				_sError = "Error creating OpenGL ES shader: Unspecified compiler error.";
			}
			
			return false;
		}
		return true;
	}

	/**
	 * Compile to the given platform.  Must be overridden.  The shader text must be syntactically correct for whatever
	 *	the target graphics API is.  GLSL, HLSL, Cg, etc.  This function will register the shader with the hardware after
	 *	compilation.
	 *
	 * \param _sShaderText The shader text to compile.  The syntax depends on the target graphics API.
	 * \param _stShaderType The shader type.
	 * \param _sError Holds returned errors.
	 * \return Returns true if the text compiles on the current graphics API.
	 */
	LSBOOL LSE_CALL COpenGlEsCompiledShader::Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		Reset();

		CCriticalSection::CLocker lGetError( COpenGlEs::m_csCrit );
		// Need to reset the error flag, but in case there was an error, report that it was ignored.
		glWarnError( "OpenGL ES threw an error before shader was compiled and the error has been ignored" );

		m_stType = _stShaderType;

		const GLchar * pcString = static_cast<const GLchar *>(_sShaderText.CStr());

		// Create the shader object.
		m_hShaderHandle = ::glCreateShader( ToOpenGlType( _stShaderType ) );
		if ( m_hShaderHandle ) {
			glWarnError( "glCreateShader() threw a warning but did not return 0" );
		}
		else {
			if ( glWarnErrorAlways( "Error creating OpenGL ES shader object" ) ) {
				Reset();
				return false;
			}
		}

		// Set the shader source code.
		::glShaderSource( m_hShaderHandle,
			static_cast<GLsizei>(1),
			&pcString,
			NULL );
			
		if ( glWarnErrorAlways( "Error setting OpenGL ES shader source" ) ) {
			/*PrintShaderError( m_hShaderHandle, _sError );
			Reset();
			return false;*/
		}

		// Compile the shader.
		::glCompileShader( m_hShaderHandle );
		if ( glWarnErrorAlways( "Error compiling OpenGL ES shader" ) ) {
			/*PrintShaderError( m_hShaderHandle, _sError );
			Reset();
			return false;*/
		}

		if ( !PrintShaderError( m_hShaderHandle, _sError ) ) {
			return false;
		}

		// We are compiled and read to go.
		return true;
	}

}	// namespace lsg

#endif	// #if defined( LSG_OGLES )
