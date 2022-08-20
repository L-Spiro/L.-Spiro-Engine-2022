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
 * Description: Manages the interface between shader objects and the Vulkan API.  Its main function is to apply
 *	and unapply shaders, but it also applies engine-specific functionality to shaders as well, where they are
 *	not supported by GLSL.  For example, automatic application of light and fog values, etc.
 * The DirectX HLSL version of this class has the same responsibilities but must also apply world* matrices etc.
 */

#include "LSGVulkanShaderManager.h"

#if defined( LSG_VKN )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanShaderManager::CVulkanShaderManager() :
		m_psActiveShader( NULL ) {
		//m_lfdLightData.ui32DirtyCounter = 0;
	}
	LSE_CALLCTOR CVulkanShaderManager::~CVulkanShaderManager() {
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
	void LSE_CALL CVulkanShaderManager::SetShader( CShader * _psShader ) {
		if ( _psShader && _psShader->GetId() == 0 ) {
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
			//CVulkan::glUseProgram( 0 );
		}
	}

	/**
	 * Gets the currently set shader, or NULL if no shader is set.
	 *
	 * \return Returns the currently set shader, or NULL if no shader is set.
	 */
	CShader * LSE_CALL CVulkanShaderManager::GetShader() {
		return m_psActiveShader;
	}

	/**
	 * Prepare the current shader for rendering.
	 */
	void LSE_CALL CVulkanShaderManager::PrepareForRender() {
		if ( m_psActiveShader ) {
			// Update the engine values in the shader.
			// The CGfx optimizes the setting of this data, and the shader optimizes the application of the light values,
			//	so leave them to do their work without trying to check here whether things have changed etc.
			//CGfx::GetLightData( m_lfdLightData );
			//LSG_FOG fFog;
			//CGfx::GetFog( fFog );
		}
	}

}	// namespace lsg

#endif	// #if defined( LSG_VKN )
