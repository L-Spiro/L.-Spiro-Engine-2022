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
 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for Vulkan.  After being compiled, the
 *	managing shader object will perform the linking.
 */

#include "LSGVulkanCompiledShader.h"

#if defined( LSG_VKN )

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CVulkanCompiledShader::CVulkanCompiledShader() :
		m_hShaderHandle( VK_NULL_HANDLE ) {
	}
	LSE_CALLCTOR CVulkanCompiledShader::~CVulkanCompiledShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CVulkanCompiledShader::Reset() {
		if ( m_hShaderHandle != VK_NULL_HANDLE ) {
			::vkDestroyShaderModule( CVulkan::GetVulkanDevice(), m_hShaderHandle, NULL );
			m_hShaderHandle = VK_NULL_HANDLE;
		}
		Parent::Reset();
	}

	/**
	 * Gets our Vulkan handle.
	 *
	 * \return Returns the Vulkan handle for this object.
	 */
	VkShaderModule LSE_CALL CVulkanCompiledShader::GetHandle() const {
		return m_hShaderHandle;
	}

	/**
	 * Converts a shader from our system type to the Vulkan type.
	 *
	 * \param _stShaderType The system shader type to convert.
	 * \return Returns the Vulkan equivilent of the shader type.
	 */
	uint32_t LSE_CALL CVulkanCompiledShader::ToVulkanType( LSG_SHADER_TYPES _stShaderType ) {
		switch ( _stShaderType ) {
			case LSG_ST_VERTEX : { return VK_SHADER_STAGE_VERTEX_BIT; }
			case LSG_ST_PIXEL : { return VK_SHADER_STAGE_FRAGMENT_BIT; }
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
	LSBOOL LSE_CALL CVulkanCompiledShader::Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		Reset();

		m_stType = _stShaderType;
		// We are compiled and read to go.
		return true;
	}

}	// namespace lsg

#endif	// #if defined( LSG_VKN )
