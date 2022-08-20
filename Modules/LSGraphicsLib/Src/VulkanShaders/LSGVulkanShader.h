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
 * Description: Open Vulkan shader.  Specifically for Vulkan, after shaders are translated, they are linked
 *	into an Vulkan program object.  A shader program in Vulkan is a combination of a vertex shader, pixel
 *	shader, and geometry shader.  Each of these are called shader parts in this context, and shader parts
 *	can be shared.  For this reason, it may be common for multiple Vulkan shader programs to be comprised
 *	of the same shader parts.
 * As an optimization, this class redirects the creation of shader programs to another manager which has global
 *	scope and is capable of determining when shader programs are created from the same shader parts.  This
 *	reduces significantly the number of shader swaps that need to be made during rendering.
 */


#ifndef __LSG_VULKANSHADER_H__
#define __LSG_VULKANSHADER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_VKN )

#include "../Vulkan/LSGVulkan.h"
#include "../ShaderLanguage/LSGShaderBase.h"
#include "LSGVulkanCompiledShader.h"
#include "LSGVulkanCompiledShaderManager.h"
#include "LSGVulkanShaderProgramManager.h"
#include "LSGVulkanShaderTranslator.h"

namespace lsg {

	class CVulkanShaderManager;

	/**
	 * Class CVulkanShader
	 * \brief Open Vulkan shader.
	 *
	 * Description: Open Vulkan shader.  Specifically for Vulkan, after shaders are translated, they are linked
	 *	into an Vulkan program object.  A shader program in Vulkan is a combination of a vertex shader, pixel
	 *	shader, and geometry shader.  Each of these are called shader parts in this context, and shader parts
	 *	can be shared.  For this reason, it may be common for multiple Vulkan shader programs to be comprised
	 *	of the same shader parts.
	 * As an optimization, this class redirects the creation of shader programs to another manager which has global
	 *	scope and is capable of determining when shader programs are created from the same shader parts.  This
	 *	reduces significantly the number of shader swaps that need to be made during rendering.
	 */
	class CVulkanShader : public CShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVulkanShader();
		virtual LSE_CALLCTOR					~CVulkanShader();


		// == Functions.
		/**
		 * Reset everything.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets the uniqe ID of this shader.  The ID is 0 if the shader is not actually ready
		 *	to be activated (that is, has not been compiled and registered with the graphics API).
		 * Activating a shader with a 0 ID causes the fixed-function pipeline to become activated
		 *	(if the fixed-function pipeline is supported).
		 *
		 * \return Returns the unique ID for this shader.
		 */
		virtual uint64_t LSE_CALL				GetId() const;

		/**
		 * Gets the attribute locations for this shader.
		 *
		 * \return Returns the attribute locations for this shader.
		 */
		const CVulkanShaderProgramManager::LSG_ATTRIBUTE_USAGE_LOCS * LSE_CALL
												GetAttributeLocs() const;

		/**
		 * Gets the total attribute locations for this shader.
		 *
		 * \return Returns the total attribute locations for this shader.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalAttributes() const;

		/**
		 * Activation callback.  A system value is passed that is to be used internally and changes meaning depending on the
		 *	graphics API.
		 *
		 * \param _pvSysValue A value with special system meaning.
		 * \return Returns false if there is a failure to apply the shader via the current graphics API.
		 */
		virtual LSBOOL LSE_CALL					Activate( void * _pvSysValue );

		/**
		 * Deactivation callback.  Calls by the system when the shader is deactivated.
		 */
		virtual void LSE_CALL					Deactivate();

		/**
		 * Gets a handle for a uniform variable in the shader.
		 *
		 * \param _pcName Name of the variable whose handle is to be obtained.
		 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
		 */
		virtual LSG_HANDLE LSE_CALL				GetConstantByName( const char * _pcName );

		/**
		 * Sets a single scalar value in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _fValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetFloat( LSG_HANDLE _hHandle, float _fValue );

		/**
		 * Sets an array of scalar values in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pfValues New values.
		 * \param _ui32Total Total floats to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetFloatArray( LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total );

		/**
		 * Sets a single scalar value in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _iValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetInt( LSG_HANDLE _hHandle, int32_t _iValue );

		/**
		 * Sets an array of scalar values in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pi32Values New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetIntArray( LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total );

		/**
		 * Sets a single scalar value in the shader of type bool.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _bValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetBool( LSG_HANDLE _hHandle, LSBOOL _bValue );

		/**
		 * Sets an array of scalar values in the shader of type bool.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pbValues New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetBoolArray( LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total );

		/**
		 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector2( LSG_HANDLE _hHandle, const CVector2 &_vValue );

		/**
		 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector2Array( LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector3( LSG_HANDLE _hHandle, const CVector3 &_vValue );

		/**
		 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector3Array( LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector4( LSG_HANDLE _hHandle, const CVector4 &_vValue );

		/**
		 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetVector4Array( LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetMatrix2x2( LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue );

		/**
		 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetMatrix3x3( LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue );

		/**
		 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL					SetMatrix4x4( LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue );

		/**
		 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pmValues New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL							SetMatrix4x4Array( LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total );


	protected :
		// == Members.
		/**
		 * One shader part for each shader type.  These may be shared.
		 */
		CVulkanCompiledShader *					m_pogcsShaders[LSG_ST_TOTAL];

		/**
		 * The shader program with all of the shader parts linked.
		 */
		CVulkanShaderProgramManager::CVulkanShaderProgram *
												m_pospProgram;

		/**
		 * The shader manager to which this shader is applied, if any.
		 */
		CVulkanShaderManager *					m_pogsmShaderMan;


		// == Functions.
		/**
		 * Creates a new translator of type CVulkanShaderTranslator.
		 */
		virtual void LSE_CALL					CreateTranslator();

		/**
		 * Destroys the CVulkanShaderTranslator translator.
		 */
		virtual void LSE_CALL					DestroyTranslator();

		/**
		 * Post compile.  We use this to create Vulkan compiled shader parts and link them to
		 *	create a shader program.
		 *
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		virtual LSG_SHADER_ERRORS LSE_CALL		PostCompile( CString &_sError );

	private :
		typedef CShaderBase						Parent;
		friend class							CVulkanShaderManager;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total attribute locations for this shader.
	 *
	 * \return Returns the total attribute locations for this shader.
	 */
	LSE_INLINE uint32_t LSE_CALL CVulkanShader::TotalAttributes() const {
		return m_pospProgram->TotalAttributes();
	}

}	// namespace lsg

#endif	// #if defined( LSG_VKN )

#endif	// __LSG_VULKANSHADER_H__
