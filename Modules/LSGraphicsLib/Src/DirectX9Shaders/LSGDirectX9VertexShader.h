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
 * Description: A vertex shader in DirectX 9.  This is used by CDirectX9Shader as one of its components.  These
 *	objects are shared by default, managed by the CDirectX9CompiledShaderManager class.
 */


#ifndef __LSG_DIRECTX9VERTEXSHADER_H__
#define __LSG_DIRECTX9VERTEXSHADER_H__

#include "../LSGGraphicsLib.h"
#include "LSGDirectX9CompiledShader.h"
#include "String/LSTLString.h"

#ifdef LSG_DX9

namespace lsg {

	/**
	 * Class CDirectX9VertexShader
	 * \brief A vertex shader in DirectX 9.
	 *
	 * Description: A vertex shader in DirectX 9.  This is used by CDirectX9Shader as one of its components.  These
	 *	objects are shared by default, managed by the CDirectX9CompiledShaderManager class.
	 */
	class CDirectX9VertexShader : public CDirectX9CompiledShader {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX9VertexShader();
		virtual LSE_CALLCTOR								~CDirectX9VertexShader();


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
		 * Must perform some action when the device is lost.
		 */
		virtual void LSE_CALL								OnDeviceLost();

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual LSBOOL LSE_CALL								OnDeviceReset();


	protected :
		// == Members.
		/**
		 * The Direct3D shader object.
		 */
		IDirect3DVertexShader9 *							m_pvs9VertexShader;

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
		typedef CDirectX9CompiledShader						Parent;
		friend class										CDirectX9CompiledShaderManager;
	};

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9VERTEXSHADER_H__
