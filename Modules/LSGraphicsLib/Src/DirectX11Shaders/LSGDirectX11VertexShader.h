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
 * Description: A vertex shader in DirectX 11.  This is used by CDirectX11Shader as one of its components.  These
 *	objects are shared by default, managed by the CDirectX11CompiledShaderManager class.
 */


#ifndef __LSG_DIRECTX11VERTEXSHADER_H__
#define __LSG_DIRECTX11VERTEXSHADER_H__

#include "../LSGGraphicsLib.h"
#include "LSGDirectX11CompiledShader.h"
#include "String/LSTLString.h"

#ifdef LSG_DX11

namespace lsg {

	/**
	 * Class CDirectX11VertexShader
	 * \brief A vertex shader in DirectX 11.
	 *
	 * Description: A vertex shader in DirectX 11.  This is used by CDirectX11Shader as one of its components.  These
	 *	objects are shared by default, managed by the CDirectX11CompiledShaderManager class.
	 */
	class CDirectX11VertexShader : public CDirectX11CompiledShader {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX11VertexShader();
		virtual LSE_CALLCTOR								~CDirectX11VertexShader();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual void LSE_CALL								Reset();

		/**
		 * Activate the shader.
		 *
		 * \return Returns false if there is a failure to apply the shader via the current graphics API.
		 */
		virtual LSBOOL LSE_CALL								Activate();

		/**
		 * Called once prior to each render.
		 */
		virtual void LSE_CALL								PreRender();


	protected :
		// == Members.
		/**
		 * The Direct3D shader object.
		 */
		ID3D11VertexShader *								m_pvsVertexShader;

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
		virtual LSBOOL LSE_CALL								Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError );


	private :
		typedef CDirectX11CompiledShader					Parent;
		friend class										CDirectX11CompiledShaderManager;
	};

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11VERTEXSHADER_H__
