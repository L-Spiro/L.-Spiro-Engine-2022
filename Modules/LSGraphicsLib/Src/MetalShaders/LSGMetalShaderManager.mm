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
 * Description: Manages the interface between shader objects and the Metal API.  Its main function is to apply
 *	and unapply shaders, but it also applies engine-specific functionality to shaders as well, where they are
 *	not supported by GLSL.  For example, automatic application of light and fog values, etc.
 * The DirectX HLSL version of this class has the same responsibilities but must also apply world* matrices etc.
 */

#include "LSGMetalShaderManager.h"

#if defined( LSG_METAL )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CMetalShaderManager::CMetalShaderManager() :
		m_psActiveShader( NULL ) {
		//m_lfdLightData.ui32DirtyCounter = 0UL;
	}
	LSE_CALLCTOR CMetalShaderManager::~CMetalShaderManager() {
		SetShader( NULL );
	}

	// == Functions.
	/**
	 * Apply a shader or switch back to fixed-function support (or, if not available, emulated
	 *	fixed-function support via a system shader).  Pass NULL to deactivate shaders.  If the given
	 *	shader has a 0 ID, it is the same as passing a NULL shader.
	 *
	 * \param _psShader The shader to activate or NULL to deactivate all shaders.
	 */
	LSVOID LSE_CALL CMetalShaderManager::SetShader( CShader * _psShader ) {
		if ( _psShader && _psShader->GetId() == 0UL ) {
			// A shader with a 0 ID is the same as no shader at all.
			_psShader = NULL;
		}
		if ( m_psActiveShader ) {
			if ( _psShader ) {
				if ( m_psActiveShader->GetId() == _psShader->GetId() ) { return; }
			}
			// Deactivate the previous shader.
			m_psActiveShader->Deactivate();
		}

		m_psActiveShader = _psShader;
		if ( m_psActiveShader ) {
			// Activate the new shader.  We pass our pointer to the shader so that it can tell us
			//	if it gets destroyed while in use.
			if ( !m_psActiveShader->Activate( this ) ) {
				m_psActiveShader = NULL;
			}
		}
		else {
			//::glUseProgram( 0 );
		}
	}

	/**
	 * Gets the currently set shader, or NULL if no shader is set.
	 *
	 * \return Returns the currently set shader, or NULL if no shader is set.
	 */
	CShader * LSE_CALL CMetalShaderManager::GetShader() {
		return m_psActiveShader;
	}

	/**
	 * Prepare the current shader for rendering.
	 */
	LSVOID LSE_CALL CMetalShaderManager::PrepareForRender() {
		if ( m_psActiveShader ) {
		}
	}

}	// namespace lsg

#endif	// #if defined( LSG_METAL )
