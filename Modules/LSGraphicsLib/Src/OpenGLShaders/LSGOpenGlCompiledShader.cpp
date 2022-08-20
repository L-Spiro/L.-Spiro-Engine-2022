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
 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for OpenGL.  After being compiled, the
 *	managing shader object will perform the linking.
 */

#include "LSGOpenGlCompiledShader.h"

#if defined( LSG_OGL )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR COpenGlCompiledShader::COpenGlCompiledShader() :
		m_hShaderHandle( 0 ) {
	}
	LSE_CALLCTOR COpenGlCompiledShader::~COpenGlCompiledShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL COpenGlCompiledShader::Reset() {
		if ( m_hShaderHandle ) {
			COpenGl::glDeleteShader( m_hShaderHandle );
			m_hShaderHandle = 0;
		}
		Parent::Reset();
	}

	/**
	 * Gets our OpenGL handle.
	 *
	 * \return Returns the OpenGL handle for this object.
	 */
	GLuint LSE_CALL COpenGlCompiledShader::GetHandle() const {
		return m_hShaderHandle;
	}

	/**
	 * Converts a shader from our system type to the OpenGL type.
	 *
	 * \param _stShaderType The system shader type to convert.
	 * \return Returns the OpenGL equivilent of the shader type.
	 */
	uint32_t LSE_CALL COpenGlCompiledShader::ToOpenGlType( LSG_SHADER_TYPES _stShaderType ) {
		switch ( _stShaderType ) {
			case LSG_ST_VERTEX : { return GL_VERTEX_SHADER; }
			case LSG_ST_PIXEL : { return GL_FRAGMENT_SHADER; }
			default : {}
		}
		assert( false );
		return 0;
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
	LSBOOL LSE_CALL COpenGlCompiledShader::Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		Reset();

		CCriticalSection::CLocker lGetError( COpenGl::m_csCrit );
		// Need to reset the error flag, but in case there was an error, report that it was ignored.
		glWarnErrorAlways( "OpenGL threw an error before shader was compiled and the error has been ignored" );

		m_stType = _stShaderType;

		const GLchar * pcString = static_cast<const GLchar *>(_sShaderText.CStr());

		// Create the shader object.
		m_hShaderHandle = COpenGl::glCreateShader( ToOpenGlType( _stShaderType ) );
		if ( glWarnErrorAlways( "Error creating OpenGL shader object" ) ) {
			Reset();
			return false;
		}

		// Set the shader source code.
		COpenGl::glShaderSource( m_hShaderHandle,
			static_cast<GLsizei>(1),
			&pcString,
			NULL );

		if ( glWarnErrorAlways( "Error setting OpenGL shader source" ) ) {
			Reset();
			return false;
		}

		// Compile the shader.
		COpenGl::glCompileShader( m_hShaderHandle );
		if ( glWarnErrorAlways( "Error compiling OpenGL shader" ) ) {
			Reset();
			return false;
		}


		// Compiler errors have to be checked via the shader log.
		GLint iStatus;
		COpenGl::glGetShaderiv( m_hShaderHandle, GL_COMPILE_STATUS, &iStatus );
		if ( iStatus == GL_FALSE ) {
			// Get the length of the error.
			GLint iLength;
			COpenGl::glGetShaderiv( m_hShaderHandle, GL_INFO_LOG_LENGTH, &iLength );
			if ( iLength > 1 ) {
				// There will be an error log.  Pass it up.
				GLchar * pcText = LSENEW GLchar[iLength+1];
				GLsizei siWritten;
				COpenGl::glGetShaderInfoLog( m_hShaderHandle, iLength, &siWritten, pcText );
				_sError = static_cast<const char *>(pcText);
				LSEDELETE [] pcText;
			}
			else {
				_sError = "Error creating OpenGL shader: Unspecified compiler error.";
			}
			return false;
		}

		// We are compiled and read to go.
		return true;
	}

}	// namespace lsg

#endif	// #if defined( LSG_OGL )
