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
 * Description: A base class for compiled shaders, providing a generic interface for setting and getting data from shaders.
 *	A CShader object will have one of these per shader part (vertex, pixel, goemetry, etc.)  This is overridden by classes
 *	that specialize it for OpenGL GLSL, DirectX HLSL, etc.
 */


#ifndef __LSG_COMPILEDSHADERBASE_H__
#define __LSG_COMPILEDSHADERBASE_H__

#include "../LSGGraphicsLib.h"
#include "LSGShaderLanguageParser.h"
#include "String/LSTLString.h"

namespace lsg {

	class CShader;

	/**
	 * Class CCompiledShaderBase
	 * \brief A base class for compiled shaders, providing a generic interface for setting and getting data from shaders.
	 *
	 * Description: A base class for compiled shaders, providing a generic interface for setting and getting data from shaders.
	 *	A CShader object will have one of these per shader part (vertex, pixel, goemetry, etc.)  This is overridden by classes
	 *	that specialize it for OpenGL GLSL, DirectX HLSL, etc.
	 */
	class CCompiledShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CCompiledShaderBase();
		virtual LSE_CALLCTOR					~CCompiledShaderBase();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual void LSE_CALL					Reset();

		/**
		 * Gets the shader type.
		 *
		 * \return Returns the shader type.
		 */
		LSG_SHADER_TYPES						GetType() const;

		/**
		 * Gets the unique shader ID.
		 *
		 * \return Returns the shader ID.
		 */
		uint64_t LSE_CALL						GetId() const;

		/**
		 * Gets the reference count.
		 *
		 * \return Returns the reference count for this object.
		 */
		LSE_INLINE uint32_t LSE_CALL			GetRef() const;

		/**
		 * Increase the reference count on this object.
		 */
		void LSE_CALL							IncRef();

		/**
		 * Decrease the reference count on this object.
		 */
		void LSE_CALL							DecRef();

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
		virtual LSBOOL LSE_CALL					Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) = 0;


	protected :
		// == Members.
		/** The unique ID of this compiled shader. */
		uint32_t								m_ui32Id;

		/** The shader type. */
		LSG_SHADER_TYPES						m_stType;

		/** Reference counter. */
		uint32_t								m_ui32Ref;

		/** Unique ID counter. */
		static uint32_t							m_ui32IdCounter;


	private :
		friend class							CShader;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the reference count.
	 *
	 * \return Returns the reference count for this object.
	 */
	LSE_INLINE uint32_t LSE_CALL CCompiledShaderBase::GetRef() const {
		return m_ui32Ref;
	}

}	// namespace lsg

#endif	// __LSG_COMPILEDSHADERBASE_H__
