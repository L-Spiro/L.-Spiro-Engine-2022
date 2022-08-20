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
 * Description: Manages the sharing of similar Metal shaders parts (a single vertex, pixel, or geometry shader).
 *	Since Metal does not give us back the compiled code (sigh) we can only share based on the input text.  Sadly
 *	this reduces the efficiency of the sharing, but it is not so bad thanks to the preprocessing the core engine
 *	does on each shader.  That is, two shaders may be exactly the same except for one having an extra whitespace
 *	character somewhere.  This would normally cause them to be considered as two different shaders even though they
 *	would end up compiled into exactly the same code.  But thanks to L. Spiro Shader Language (LSSL), whitespace
 *	will be made consistent and other formatting will be done so that the two shaders, by the time they reach this
 *	class, will match exactly in text form and proper sharing can take place.
 * Additionally, there is a setting that prevents any shaders from being shared.  This is mainly for tools because
 *	tools may have multiple Metal contexts and sharing will cause errors between them.
 */


#ifndef __LSG_METALCOMPILEDSHADERMANAGER_H__
#define __LSG_METALCOMPILEDSHADERMANAGER_H__

#include "../LSGGraphicsLib.h"
#include "LSGMetalCompiledShader.h"
#include "Map/LSTLLinearMap.h"
#include "String/LSTLStringList.h"

#if defined( LSG_METAL )

namespace lsg {

	/**
	 * Class CMetalCompiledShaderManager
	 * \brief Manages the sharing of similar Metal shaders parts (a single vertex, pixel, or geometry shader).
	 *
	 * Description: Manages the sharing of similar Metal shaders parts (a single vertex, pixel, or geometry shader).
	 *	Since Metal does not give us back the compiled code (sigh) we can only share based on the input text.  Sadly
	 *	this reduces the efficiency of the sharing, but it is not so bad thanks to the preprocessing the core engine
	 *	does on each shader.  That is, two shaders may be exactly the same except for one having an extra whitespace
	 *	character somewhere.  This would normally cause them to be considered as two different shaders even though they
	 *	would end up compiled into exactly the same code.  But thanks to L. Spiro Shader Language (LSSL), whitespace
	 *	will be made consistent and other formatting will be done so that the two shaders, by the time they reach this
	 *	class, will match exactly in text form and proper sharing can take place.
	 * Additionally, there is a setting that prevents any shaders from being shared.  This is mainly for tools because
	 *	tools may have multiple Metal contexts and sharing will cause errors between them.
	 */
	class CMetalCompiledShaderManager {
	public :
		// == Functions.
		/**
		 * Sets sharing.
		 *
		 * \param _bVal If true, sharing is enabled, otherwise not.
		 */
		static LSVOID LSE_CALL									SetSharing( LSBOOL _bVal );

		/**
		 * Creates a new shader of a given type.  The shader MUST be deleted using DeleteShader().
		 *
		 * \param _sShaderText The shader text to compile.  The syntax depends on the target graphics API.
		 * \param _stShaderType The shader type.
		 * \param _sError Holds returned errors.
		 * \return Returns a pointer to the new compiled shader on success, or NULL on failure.
		 */
		static CMetalCompiledShader * LSE_CALL				CreateShader( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError );

		/**
		 * Delete a shader.  If sharing, decreases its reference count.  Otherwise deletes it from memory.
		 *
		 * \param _pogcsShader The shader to delete.
		 */
		static LSVOID LSE_CALL									DeleteShader( CMetalCompiledShader * _pogcsShader );

		/**
		 * Deletes all unreferenced shaders from memory.  Returns the number of shaders NOT deleted.
		 *
		 * \return Returns the number of shaders NOT deleted.
		 */
		static LSUINT32 LSE_CALL								DeleteUnreferencedShaders();


	protected :
		// == Members.
		/**
		 * Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start.
		 */
		static LSBOOL											m_bSharing;

		/**
		 * Map of shaders.
		 */
		static CLinearMap<CString, CMetalCompiledShader *>	m_mShaders[LSG_ST_TOTAL];
	};

}	// namespace lsg

#endif	// #if defined( LSG_METAL )

#endif	// __LSG_METALCOMPILEDSHADERMANAGER_H__
