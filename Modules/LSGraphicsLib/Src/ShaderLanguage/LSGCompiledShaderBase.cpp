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


#include "LSGCompiledShaderBase.h"


namespace lsg {

	// Unique ID counter.
	uint32_t CCompiledShaderBase::m_ui32IdCounter = 0;

	// == Various constructors.
	LSE_CALLCTOR CCompiledShaderBase::CCompiledShaderBase() :
		m_ui32Id( ++m_ui32IdCounter ),
		m_stType( LSG_ST_INVALID ),
		m_ui32Ref( 0 ) {
	}
	LSE_CALLCTOR CCompiledShaderBase::~CCompiledShaderBase() {
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CCompiledShaderBase::Reset() {
		// Must not be in use.
		assert( m_ui32Ref == 0 );

		// Need a new ID.
		m_ui32Id = ++m_ui32IdCounter;
		m_stType = LSG_ST_INVALID;
		m_ui32Ref = 0;
	}

	/**
	 * Gets the shader type.
	 *
	 * \return Returns the shader type.
	 */
	LSG_SHADER_TYPES CCompiledShaderBase::GetType() const {
		return m_stType;
	}

	/**
	 * Gets the unique shader ID.
	 *
	 * \return Returns the shader ID.
	 */
	uint64_t LSE_CALL CCompiledShaderBase::GetId() const {
		return m_ui32Id;
	}

	/**
	 * Increase the reference count on this object.
	 */
	void LSE_CALL CCompiledShaderBase::IncRef() {
		++m_ui32Ref;
	}

	/**
	 * Decrease the reference count on this object.
	 */
	void LSE_CALL CCompiledShaderBase::DecRef() {
		--m_ui32Ref;
	}

}	// namespace lsg
