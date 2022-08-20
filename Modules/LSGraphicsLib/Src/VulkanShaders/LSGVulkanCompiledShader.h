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


#ifndef __LSG_VULKANCOMPILEDSHADER_H__
#define __LSG_VULKANCOMPILEDSHADER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_VKN )

#include "../Vulkan/LSGVulkan.h"
#include "../ShaderLanguage/LSGCompiledShaderBase.h"

namespace lsg {

	class CVulkanCompiledShaderManager;

	/**
	 * Class CVulkanCompiledShader
	 * \brief A compiled shader part (vertex, pixel, geometry, etc.) for Vulkan.
	 *
	 * Description: A compiled shader part (vertex, pixel, geometry, etc.) for Vulkan.  After being compiled, the
	 *	managing shader object will perform the linking.
	 */
	class CVulkanCompiledShader : public CCompiledShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVulkanCompiledShader();
		LSE_CALLCTOR							~CVulkanCompiledShader();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual void LSE_CALL					Reset();

		/**
		 * Gets our Vulkan handle.
		 *
		 * \return Returns the Vulkan handle for this object.
		 */
		VkShaderModule LSE_CALL					GetHandle() const;

		/**
		 * Converts a shader from our system type to the Vulkan type.
		 *
		 * \param _stShaderType The system shader type to convert.
		 * \return Returns the Vulkan equivilent of the shader type.
		 */
		static uint32_t LSE_CALL				ToVulkanType( LSG_SHADER_TYPES _stShaderType );


	protected :
		// == Members.
		/** Our handle. */
		VkShaderModule							m_hShaderHandle;


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
		friend class							CVulkanCompiledShaderManager;
	};

}	// namespace lsg

#endif	// #if defined( LSG_VKN )

#endif	// __LSG_VULKANCOMPILEDSHADER_H__
