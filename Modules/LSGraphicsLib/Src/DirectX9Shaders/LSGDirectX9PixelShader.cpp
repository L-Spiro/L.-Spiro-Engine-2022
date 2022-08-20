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
 * Description: A pixel shader in DirectX 9.  This is used by CDirectX9Shader as one of its components.  These
 *	objects are shared by default, managed by the CDirectX9CompiledShaderManager class.
 */

#include "LSGDirectX9PixelShader.h"

#ifdef LSG_DX9

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9PixelShader::CDirectX9PixelShader() :
		m_pps9PixelShader( NULL ) {
	}
	LSE_CALLCTOR CDirectX9PixelShader::~CDirectX9PixelShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CDirectX9PixelShader::Reset() {
		SafeRelease( m_pps9PixelShader );
		Parent::Reset();
	}

	/**
	 * Activate the shader.
	 *
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CDirectX9PixelShader::Activate() {
		for ( uint32_t I = m_vConstantMap.Length(); I--; ) {
			m_vConstantMap[I].bDirty = true;
		}
		return SUCCEEDED( CDirectX9::GetDirectX9Device()->SetPixelShader( m_pps9PixelShader ) );
	}

	/**
	 * Must perform some action when the device is lost.
	 */
	void LSE_CALL CDirectX9PixelShader::OnDeviceLost() {
		SafeRelease( m_pps9PixelShader );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	LSBOOL LSE_CALL CDirectX9PixelShader::OnDeviceReset() {
		if ( !m_pbShader ) { return true; }
		for ( uint32_t I = m_vConstantMap.Length(); I--; ) {
			m_vConstantMap[I].bDirty = true;
		}
		HRESULT hRes = CDirectX9::GetDirectX9Device()->CreatePixelShader( reinterpret_cast<DWORD *>(m_pbShader->GetBufferPointer()),
			&m_pps9PixelShader );
		if ( FAILED( hRes ) ) {
			return false;
		}
		m_pctConstantTable->SetDefaults( CDirectX9::GetDirectX9Device() );

		return true;
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
	LSBOOL LSE_CALL CDirectX9PixelShader::Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		// _stShaderType is always LSG_ST_PIXEL for us.
		static_cast<LSG_SHADER_TYPES>(_stShaderType);
		Reset();
		_sError.Reset();
		m_stType = LSG_ST_PIXEL;

		// Flags depend on the build type.
		DWORD dwShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
		dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_DEBUG;
#endif

		// Compile the code.
		ID3DXBuffer * pbErrors = NULL;
		HRESULT hRes = ::D3DXCompileShader( reinterpret_cast<LPCSTR>(_sShaderText.CStr()), static_cast<UINT>(_sShaderText.Length()), NULL, NULL, "Main",
			"ps_3_0", dwShaderFlags, &m_pbShader,
			&pbErrors, &m_pctConstantTable );

		if ( pbErrors ) {
			_sError = reinterpret_cast<const char *>(pbErrors->GetBufferPointer());
			SafeRelease( pbErrors );
		}
		if ( FAILED( hRes ) ) {
			switch ( hRes ) {
				case D3DERR_INVALIDCALL : {
					CStd::DebugPrintA( "CDirectX9PixelShader::Compile(): D3DERR_INVALIDCALL\r\n" );
					break;
				}
				case D3DXERR_INVALIDDATA : {
					CStd::DebugPrintA( "CDirectX9PixelShader::Compile(): D3DXERR_INVALIDDATA\r\n" );
					break;
				}
				case E_OUTOFMEMORY : {
					CStd::DebugPrintA( "CDirectX9PixelShader::Compile(): E_OUTOFMEMORY\r\n" );
					break;
				}
			}
			return false;
		}
		m_pctConstantTable->SetDefaults( CDirectX9::GetDirectX9Device() );

		// Create the shader.
		hRes = CDirectX9::GetDirectX9Device()->CreatePixelShader( reinterpret_cast<DWORD *>(m_pbShader->GetBufferPointer()),
			&m_pps9PixelShader );
		if ( FAILED( hRes ) ) {
			_sError += "\r\nFailed to create shader object due to lack of memory.\r\n";
			return false;
		}

		return true;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
