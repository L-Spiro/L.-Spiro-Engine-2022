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
 * Description: Manages the interface between shader objects and the Direct3D 9 API.  Its main function is to apply
 *	and unapply shaders, but it also applies engine-specific functionality to shaders as well, where they are
 *	not supported by HLSL.  For example, automatic application of light and fog values, etc.
 */


#ifndef __LSG_DIRECTX9SHADERMANAGER_H__
#define __LSG_DIRECTX9SHADERMANAGER_H__

#include "../LSGGraphicsLib.h"
#include "../Gfx/LSGGfx.h"
#include "../ShaderLanguage/LSGShader.h"

#if defined( LSG_DX9 )

namespace lsg {

	/**
	 * Class CDirectX9ShaderManager
	 * \brief Manages the interface between shader objects and the Direct3D 9 API.
	 *
	 * Description: Manages the interface between shader objects and the Direct3D 9 API.  Its main function is to apply
	 *	and unapply shaders, but it also applies engine-specific functionality to shaders as well, where they are
	 *	not supported by HLSL.  For example, automatic application of light and fog values, etc.
	 */
	class CDirectX9ShaderManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX9ShaderManager();
		LSE_CALLCTOR										~CDirectX9ShaderManager();


		/**
		 * Apply a shader or switch back to fixed-function support (or, if not available, emulated
		 *	fixed-function support via a system shader).  Pass NULL to deactivate shaders.  If the given
		 *	shader has a 0 ID, it is the same as passing a NULL shader.
		 *
		 * \param _psShader The shader to activate or NULL to deactivate all shaders.
		 */
		void LSE_CALL										SetShader( CShader * _psShader );

		/**
		 * Gets the currently set shader, or NULL if no shader is set.
		 *
		 * \return Returns the currently set shader, or NULL if no shader is set.
		 */
		CShader * LSE_CALL									GetShader();

		/**
		 * Prepare the current shader for rendering.
		 */
		void LSE_CALL										PrepareForRender();

	protected :
		// == Members.
		/** Currently active shader. */
		CShader *											m_psActiveShader;

		/** Light data. */
		//CShader::LSG_LIGHT_FULLDATA							m_lfdLightData;
	};

}	// namespace lsg

#endif	// #if defined( LSG_DX9 )

#endif	// __LSG_DIRECTX9SHADERMANAGER_H__
