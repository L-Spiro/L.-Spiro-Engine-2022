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
 * Description: Manages the sharing of a single Direct3D 9 shader (vertex or pixel shader).  Sharing is based
 *	off the (adjusted) shader text for the sake of speed.  The alternative--sharing based off compiled code--
 *	makes loading times too long.
 * For debug purposes, sharing can be disabled.
 */


#ifndef __LSG_DIRECTX9COMPILERDSHADERMANAGER_H__
#define __LSG_DIRECTX9COMPILERDSHADERMANAGER_H__

#include "../LSGGraphicsLib.h"
#include "LSGDirectX9PixelShader.h"
#include "LSGDirectX9VertexShader.h"
#include "String/LSTLString.h"

#if defined( LSG_DX9 )

namespace lsg {

	/**
	 * Class CDirectX9CompiledShaderManager
	 * \brief Manages the sharing of a single Direct3D 9 shader (vertex or pixel shader).
	 *
	 * Description: Manages the sharing of a single Direct3D 9 shader (vertex or pixel shader).  Sharing is based
	 *	off the (adjusted) shader text for the sake of speed.  The alternative--sharing based off compiled code--
	 *	makes loading times too long.
	 * For debug purposes, sharing can be disabled.
	 */
	class CDirectX9CompiledShaderManager {
	public :
		// == Functions.
		/**
		 * Sets sharing.
		 *
		 * \param _bVal If true, sharing is enabled, otherwise not.
		 */
		static void LSE_CALL									SetSharing( LSBOOL _bVal );

		/**
		 * Creates a new shader of a given type.  The shader MUST be deleted using DeleteShader().
		 *
		 * \param _sShaderText The shader text to compile.  The syntax depends on the target graphics API.
		 * \param _stShaderType The shader type.
		 * \param _pulUniforms Shader uniforms.
		 * \param _sError Holds returned errors.
		 * \return Returns a pointer to the new compiled shader on success, or NULL on failure.
		 */
		static CCompiledShaderBase * LSE_CALL					CreateShader( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, 
			CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms, CString &_sError );

		/**
		 * Delete a shader.  If sharing, decreases its reference count.  Otherwise deletes it from memory.
		 *
		 * \param _pogcsShader The shader to delete.
		 */
		static void LSE_CALL									DeleteShader( CCompiledShaderBase * _pogcsShader );

		/**
		 * Deletes all unreferenced shaders from memory.  Returns the number of shaders NOT deleted.
		 *
		 * \return Returns the number of shaders NOT deleted.
		 */
		static uint32_t LSE_CALL								DeleteUnreferencedShaders();

	protected :
		// == Members.
		/**
		 * Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
		 */
		static LSBOOL											m_bSharing;

		/**
		 * Map of shaders.
		 */
		static CLinearMap<CString, CCompiledShaderBase *>		m_mShaders[LSG_ST_TOTAL];
	};

}	// namespace lsg

#endif	// #if defined( LSG_DX9 )

#endif	// __LSG_DIRECTX9COMPILERDSHADERMANAGER_H__
