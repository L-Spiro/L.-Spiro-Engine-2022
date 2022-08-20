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
 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for Metal.  After being compiled, the
 *	managing shader object will perform the linking.
 */


#ifndef __LSG_METALCOMPILEDSHADER_H__
#define __LSG_METALCOMPILEDSHADER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_METAL )

#include "../Metal/LSGMetal.h"
#include "../ShaderLanguage/LSGCompiledShaderBase.h"

namespace lsg {

	class CMetalCompiledShaderManager;

	/**
	 * Class CMetalCompiledShader
	 * \brief A compiled shader part (vertex, pixel, geometry, etc.) for Metal.
	 *
	 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for Metal.  After being compiled, the
	 *	managing shader object will perform the linking.
	 */
	class CMetalCompiledShader : public CCompiledShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMetalCompiledShader();
		LSE_CALLCTOR							~CMetalCompiledShader();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual LSVOID LSE_CALL					Reset();

		/**
		 * Gets our Metal handle.
		 *
		 * \return Returns the Metal handle for this object.
		 */
		const LSVOID * LSE_CALL					GetHandle() const;


	protected :
		// == Members.
		/** Our shader handle of type id <MTLLibrary>. */
		const LSVOID *							m_pvShaderHandle;
		
		/** Our function of type id <MTLFunction>. */
		const LSVOID *							m_pvFunc;


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
		friend class							CMetalCompiledShaderManager;
	};

}	// namespace lsg

#endif	// #if defined( LSG_METAL )

#endif	// __LSG_METALCOMPILEDSHADER_H__
