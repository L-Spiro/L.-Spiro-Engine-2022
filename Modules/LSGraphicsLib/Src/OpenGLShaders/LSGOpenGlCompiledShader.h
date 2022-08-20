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


#ifndef __LSG_OPENGLCOMPILEDSHADER_H__
#define __LSG_OPENGLCOMPILEDSHADER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_OGL )

#include "../OpenGL/LSGOpenGl.h"
#include "../ShaderLanguage/LSGCompiledShaderBase.h"

namespace lsg {

	class COpenGlCompiledShaderManager;

	/**
	 * Class COpenGlCompiledShader
	 * \brief A compiled shader part (vertex, pixel, geometry, etc.) for OpenGL.
	 *
	 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for OpenGL.  After being compiled, the
	 *	managing shader object will perform the linking.
	 */
	class COpenGlCompiledShader : public CCompiledShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlCompiledShader();
		LSE_CALLCTOR							~COpenGlCompiledShader();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual void LSE_CALL					Reset();

		/**
		 * Gets our OpenGL handle.
		 *
		 * \return Returns the OpenGL handle for this object.
		 */
		GLuint LSE_CALL							GetHandle() const;

		/**
		 * Converts a shader from our system type to the OpenGL type.
		 *
		 * \param _stShaderType The system shader type to convert.
		 * \return Returns the OpenGL equivilent of the shader type.
		 */
		static uint32_t LSE_CALL				ToOpenGlType( LSG_SHADER_TYPES _stShaderType );


	protected :
		// == Members.
		/**
		 * Our handle.
		 */
		GLuint									m_hShaderHandle;


		// == Functions.
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
		virtual LSBOOL LSE_CALL					Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError );


	private :
		typedef CCompiledShaderBase				Parent;
		friend class							COpenGlCompiledShaderManager;
	};

}	// namespace lsg

#endif	// #if defined( LSG_OGL )

#endif	// __LSG_OPENGLCOMPILEDSHADER_H__
