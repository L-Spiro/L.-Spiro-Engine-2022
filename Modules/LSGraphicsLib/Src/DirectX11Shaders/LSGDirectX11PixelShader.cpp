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
 * Description: A pixel shader in DirectX 11.  This is used by CDirectX11Shader as one of its components.  These
 *	objects are shared by default, managed by the CDirectX11CompiledShaderManager class.
 */

#include "LSGDirectX11PixelShader.h"

#ifdef LSG_DX11

#include <D3Dcompiler.h>

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11PixelShader::CDirectX11PixelShader() :
		m_ppsPixelShader( NULL ) {
	}
	LSE_CALLCTOR CDirectX11PixelShader::~CDirectX11PixelShader() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CDirectX11PixelShader::Reset() {
		CDirectX11::SafeRelease( m_ppsPixelShader );
		Parent::Reset();
	}

	/**
	 * Activate the shader.
	 *
	 * \return Returns false if there is a failure to apply the shader via the current graphics API.
	 */
	LSBOOL LSE_CALL CDirectX11PixelShader::Activate() {
		if ( !m_ppsPixelShader ) { return false; }
		CDirectX11::GetDirectX11Context()->PSSetShader( m_ppsPixelShader, NULL, 0 );
		return true;
	}

	/**
	 * Called once prior to each render.
	 */
	void LSE_CALL CDirectX11PixelShader::PreRender() {
		// Remap any dirty constant buffers.
		ID3D11Buffer * pbBuffers[14] = {
			NULL
		};
		for ( uint32_t I = m_vConstantBuffers.Length(); I--; ) {
			if ( m_vConstantBuffers[I] ) {
				if ( m_vConstBufferRawData[I].bDirty && m_vConstBufferRawData[I].vRaw.Length() ) {
					void * pvConstData;
					D3D11_MAPPED_SUBRESOURCE msMapped;
					CDirectX11::GetDirectX11Context()->Map( m_vConstantBuffers[I], 0, D3D11_MAP_WRITE_DISCARD, 0, &msMapped );
					pvConstData = msMapped.pData;
					CStd::MemCpy( pvConstData, &m_vConstBufferRawData[I].vRaw[0], m_vConstBufferRawData[I].vRaw.Length() );
					CDirectX11::GetDirectX11Context()->Unmap( m_vConstantBuffers[I], 0 );
					m_vConstBufferRawData[I].bDirty = false;
				}
				pbBuffers[I] = m_vConstantBuffers[I];
			}
		}
		CDirectX11::GetDirectX11Context()->PSSetConstantBuffers( 0, m_vConstantBuffers.Length(), pbBuffers );
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
	LSBOOL LSE_CALL CDirectX11PixelShader::Compile( const CString &_sShaderText, LSG_SHADER_TYPES _stShaderType, CString &_sError ) {
		// _stShaderType is always LSG_ST_PIXEL for us.
		static_cast<LSG_SHADER_TYPES>(_stShaderType);
		Reset();
		_sError.Reset();
		m_stType = LSG_ST_PIXEL;

		// Flags depend on the build type.
		DWORD dwShaderFlags = 0;

#if defined( DEBUG ) || defined( _DEBUG )
		//dwShaderFlags |= D3DXSHADER_SKIPOPTIMIZATION | D3DXSHADER_DEBUG;
#endif

		// Compile the code.
		ID3DBlob * pbErrors = NULL;
		HRESULT hRes = ::D3DCompile( reinterpret_cast<LPCVOID>(_sShaderText.CStr()),
			static_cast<SIZE_T>(_sShaderText.Length()),
			NULL, NULL, NULL,
			"Main",
			"ps_5_0",
			dwShaderFlags,
			0,
			&m_pbShader,
			&pbErrors );

		if ( pbErrors ) {
			_sError = reinterpret_cast<const char *>(pbErrors->GetBufferPointer());
			CDirectX11::SafeRelease( pbErrors );
		}
		if ( FAILED( hRes ) ) {
			switch ( hRes ) {
				case D3DERR_INVALIDCALL : {
					CStd::DebugPrintA( "CDirectX11PixelShader::Compile(): D3DERR_INVALIDCALL\r\n" );
					break;
				}
				case E_OUTOFMEMORY : {
					CStd::DebugPrintA( "CDirectX11PixelShader::Compile(): E_OUTOFMEMORY\r\n" );
					break;
				}
				default : {
					CStd::DebugPrintA( "CDirectX11PixelShader::Compile(): Unrecognized error.\r\n" );
				}
			}
			CStd::DebugPrintA( _sError.CStr() );
			CStd::DebugPrintA( "\r\n" );
			return false;
		}

		// Create the shader.
		hRes = CDirectX11::GetDirectX11Device()->CreatePixelShader( m_pbShader->GetBufferPointer(), m_pbShader->GetBufferSize(),
			NULL,
			&m_ppsPixelShader );
		if ( FAILED( hRes ) ) {
			_sError += "\r\nFailed to create shader object due to lack of memory.\r\n";
			return false;
		}

		return true;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
