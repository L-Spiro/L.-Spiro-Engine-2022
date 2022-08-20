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
 * Description: Interface with the DirectX 11 API.
 */

#include "LSGDirectX11.h"

#ifdef LSG_DX11

#include "../DirectX11Shaders/LSGDirectX11CompiledShaderManager.h"
#include "../DirectX11Shaders/LSGDirectX11ShaderManager.h"
#include "../Framebuffer/LSGFramebuffer.h"

namespace lsg {

	// == Members.
	/** The current shader signature. */
	const void * CDirectX11::m_pvShaderSignature = NULL;

	/** The current shader size. */
	SIZE_T CDirectX11::m_stShaderSize = 0;

	/** The DirectX device. */
	ID3D11Device * CDirectX11::m_pdDevice = NULL;

	/** The Direct 3D context. */
	ID3D11DeviceContext * CDirectX11::m_pdcContext = NULL;

	/** Our swap chain. */
	IDXGISwapChain * CDirectX11::m_pscSwapChain = NULL;

	/** View to the back color buffer. */
	ID3D11RenderTargetView * CDirectX11::m_prtvRenderTargetView = NULL;

	/** The depth-stencil texture. */
	ID3D11Texture2D * CDirectX11::m_pt2dDepthStencilBuffer = NULL;

	/** View to the back depth/stencil buffer. */
	ID3D11DepthStencilView * CDirectX11::m_pdsvDepthStencilView = NULL;

	/** Default sampler state. */
	ID3D11SamplerState * CDirectX11::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	ID3D11RasterizerState * CDirectX11::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	ID3D11BlendState * CDirectX11::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	ID3D11DepthStencilState * CDirectX11::m_pdssDefaultDepthStencilState = NULL;

	/** The shader manager. */
	CShaderManager * CDirectX11::m_psmShaderMan = NULL;

	/** The driver type. */
	D3D_DRIVER_TYPE CDirectX11::m_dtDriver = D3D_DRIVER_TYPE_UNKNOWN;

	/** Initialization parameters. */
	CDirectX11::LSG_DIRECTX11_INIT CDirectX11::m_diInitParms = { NULL };

	/** The swap-chain creation parameters. */
	DXGI_SWAP_CHAIN_DESC CDirectX11::m_dscdSwapChainDesc = { { 0 }, };

	/** The last render state. */
	LSG_RENDER_STATE CDirectX11::m_rsLastRenderState = { NULL };

	/** The current render state. */
	LSG_RENDER_STATE CDirectX11::m_rsCurRenderState = { NULL };

	/** Are we taggling full-screen? */
	LSBOOL CDirectX11::m_bToggling = false;

	// == Functions.
	/**
	 * Initialize DirectX 11.
	 *
	 * \param _lpdiInit Data required to initialize the device.
	 * \return Returns false if there are any problem during initialization.
	 */
	LSBOOL LSE_CALL CDirectX11::InitDirectX11( LPCLSG_DIRECTX11_INIT _lpdiInit ) {
		ResetInternal();

		m_diInitParms = (*_lpdiInit);
		RECT rClient;
		::GetClientRect( m_diInitParms.hWnd, &rClient );

		DXGI_SWAP_CHAIN_DESC dscdSwapChain = { 0 };
		dscdSwapChain.BufferCount = 2;
		dscdSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		dscdSwapChain.BufferDesc.Width = rClient.right - rClient.left;
		dscdSwapChain.BufferDesc.Height = rClient.bottom - rClient.top;
		dscdSwapChain.BufferDesc.RefreshRate.Numerator = 0;
		dscdSwapChain.BufferDesc.RefreshRate.Denominator = 0;
		dscdSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_DISCARD_ON_PRESENT | DXGI_USAGE_BACK_BUFFER;
		dscdSwapChain.OutputWindow = m_diInitParms.hWnd;
		dscdSwapChain.SampleDesc.Count = 1;
		dscdSwapChain.SampleDesc.Quality = 0;
		dscdSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		dscdSwapChain.Windowed = m_diInitParms.bWindowed;
		//dscdSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		m_dscdSwapChainDesc = dscdSwapChain;
		static const D3D_DRIVER_TYPE dtDrivers[] = {
			//D3D_DRIVER_TYPE_REFERENCE,	// Third attempt.
			D3D_DRIVER_TYPE_SOFTWARE,	// Second attempt.
			D3D_DRIVER_TYPE_HARDWARE,	// First attempt.
		};

		m_dtDriver = D3D_DRIVER_TYPE_NULL;
		UINT uiFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined( _DEBUG )
		//uiFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif	// #if defined( _DEBUG )
		for ( uint32_t I = LSE_ELEMENTS( dtDrivers ); I--; ) {
			if ( SUCCEEDED( ::D3D11CreateDevice( NULL, dtDrivers[I],
				NULL, uiFlags,//D3D11_CREATE_DEVICE_SINGLETHREADED,
				NULL, 0,
				D3D11_SDK_VERSION,
				&m_pdDevice,
				NULL,
				&m_pdcContext ) ) ) {
				m_dtDriver = dtDrivers[I];

				HRESULT hSwapRes = 0;
				IDXGIFactory1 * pfFactory = nullptr; 
				{ 
					IDXGIDevice * pdxgiDevice = nullptr; 
					HRESULT hRes = m_pdDevice->QueryInterface( __uuidof( IDXGIDevice ), reinterpret_cast<void **>(&pdxgiDevice) );
					if ( SUCCEEDED( hRes ) ) {
						IDXGIAdapter * paAdapter = nullptr;
						hRes = pdxgiDevice->GetAdapter( &paAdapter );
						if ( SUCCEEDED( hRes ) ) {
							hRes = paAdapter->GetParent( __uuidof( IDXGIFactory1 ), reinterpret_cast<void **>(&pfFactory) );
							SafeRelease( paAdapter );
						}
						SafeRelease( pdxgiDevice );
					}
				}
				if ( pfFactory ) {
					hSwapRes = pfFactory->CreateSwapChain( m_pdDevice, &m_dscdSwapChainDesc,
						&m_pscSwapChain );
					SafeRelease( pfFactory );
				}
				if ( !m_pscSwapChain ) {
					CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create swap chain.\r\n" );
					switch ( hSwapRes ) {
						case DXGI_ERROR_INVALID_CALL : {
							CStd::DebugPrintA( "	DXGI_ERROR_INVALID_CALL\r\n" );
							break;
						}
						case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE : {
							CStd::DebugPrintA( "	DXGI_ERROR_NOT_CURRENTLY_AVAILABLE\r\n" );
							break;
						}
						case E_OUTOFMEMORY : {
							CStd::DebugPrintA( "	E_OUTOFMEMORY\r\n" );
							break;
						}
					}
					ResetInternal();
					return false;
				}
				break;
			}
			/*if ( SUCCEEDED( ::D3D11CreateDeviceAndSwapChain( NULL, dtDrivers[I],
				NULL, 0,//D3D11_CREATE_DEVICE_SINGLETHREADED,
				NULL, 0,
				D3D11_SDK_VERSION,
				&dscdSwapChain,
				&m_pscSwapChain,
				&m_pdDevice,
				NULL,
				&m_pdcContext ) ) ) {
				m_dtDriver = dtDrivers[I];
				break;
			}*/
		}

		assert( m_dtDriver != D3D_DRIVER_TYPE_NULL );	// This is not a run-time check!  Use for development.

		// Create the default states.
		// Sampler.
		LSG_SAMPLER_DESC sdOurSamplerDesc;
		D3D11_SAMPLER_DESC sdSamplerDesc = ConvertDesc( DefaultSamplerDesc( sdOurSamplerDesc ) );
		if ( FAILED( m_pdDevice->CreateSamplerState( &sdSamplerDesc, &m_pssDefaultSamplerState ) ) ) {
			CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create default sampler state.\r\n" );
			ResetInternal();
			return false;
		}

		// Rasterizer.
		LSG_RASTERIZER_DESC rdOurRasterizerDesc;
		D3D11_RASTERIZER_DESC rdRasterizerDesc = ConvertDesc( DefaultRasterizerDesc( rdOurRasterizerDesc ) );
		if ( FAILED( m_pdDevice->CreateRasterizerState( &rdRasterizerDesc, &m_prsDefaultRasterizerState ) ) ) {
			CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create default rasterizer state.\r\n" );
			ResetInternal();
			return false;
		}

		// Blend.
		LSG_BLEND_DESC bdOurBlendDesc;
		D3D11_BLEND_DESC bdBlendDesc = ConvertDesc( DefaultBlendDesc( bdOurBlendDesc ) );
		if ( FAILED( m_pdDevice->CreateBlendState( &bdBlendDesc, &m_pbsDefaultBlendState ) ) ) {
			CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create default blend state.\r\n" );
			ResetInternal();
			return false;
		}

		// Depth/stencil.
		LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
		D3D11_DEPTH_STENCIL_DESC dsdDepthStencilDesc = ConvertDesc( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ) );
		if ( FAILED( m_pdDevice->CreateDepthStencilState( &dsdDepthStencilDesc, &m_pdssDefaultDepthStencilState ) ) ) {
			CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create default depth/stencil state.\r\n" );
			ResetInternal();
			return false;
		}

		CGfxBase::m_mMetrics.bNonPo2 = true;
		CGfxBase::m_mMetrics.ui32MaxAniso = D3D11_REQ_MAXANISOTROPY;
		CGfxBase::m_mMetrics.ui32MaxRenderTargetSize = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		CGfxBase::m_mMetrics.ui32MaxRenderTargets = LSG_MAX_RENDER_TARGET_COUNT;
		CGfxBase::m_mMetrics.ui32MaxTextureWidth = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		CGfxBase::m_mMetrics.ui32MaxTextureHeight = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		CGfxBase::m_mMetrics.ui32MaxTextureArray = D3D11_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
		CGfxBase::m_mMetrics.ui32MaxTexSlot = LSG_MAX_TEXTURE_UNITS;
		CGfxBase::m_mMetrics.ui32MaxVertElements = 0xFFFFFFFF;
		CGfxBase::m_mMetrics.ui32MaxPrimitives = 0xFFFFFFFF;
		CGfxBase::m_mMetrics.bDdsSupport = true;
		CGfxBase::m_mMetrics.bPackedDepthStencil = true;
		CGfxBase::m_mMetrics.bDepthIsReadable = true;

		CreateSwapChainBackBuffers( _lpdiInit->hWnd );


		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "CDirectX11::InitDirectX11(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}

		return true;
	}

	/**
	 * Secondary initialization of DirectX 11, after a device has been created.
	 */
	void LSE_CALL CDirectX11::PostInitDirectX11() {
		ApplyDefaultStates();
		Clear();
		Present();
	}

	/**
	 * Destroy DirectX 11 one time at engine shutdown.
	 */
	void LSE_CALL CDirectX11::DestroyDirectX11() {
		if ( m_pscSwapChain ) {
			m_pscSwapChain->SetFullscreenState( FALSE, NULL );
		}
		CDirectX11CompiledShaderManager::DeleteUnreferencedShaders();

		LSEDELETE m_psmShaderMan;
		m_psmShaderMan = NULL;

		ResetInternal();
	}

	/**
	 * Sets the screen dimensions.
	 *
	 * \param _ui32Width Screen width, in pixels.
	 * \param _ui32Height Screen height, in pixels.
	 */
	void LSE_CALL CDirectX11::SetScreenSize( uint32_t _ui32Width, uint32_t _ui32Height ) {
		if ( m_bToggling ) {
			/*m_dscdSwapChainDesc.BufferDesc.Width = _ui32Width;
			m_dscdSwapChainDesc.BufferDesc.Height = _ui32Height;
			m_dscdSwapChainDesc.Windowed = !m_dscdSwapChainDesc.Windowed;
			if ( m_pscSwapChain ) {
				DXGI_MODE_DESC mdMode = {
					_ui32Width,									// Width
					_ui32Height,								// Height
					m_dscdSwapChainDesc.BufferDesc.RefreshRate,	// RefreshRate
					m_dscdSwapChainDesc.BufferDesc.Format,		// Format
					DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,		// ScanlineOrdering
					DXGI_MODE_SCALING_UNSPECIFIED,				// Scaling
				};
				if ( FAILED( m_pscSwapChain->ResizeTarget( &mdMode ) ) ) {
					CStd::DebugPrintA( "CDirectX11::SetScreenSize(): Failed to call ResizeTarget()\r\n" );
				}
				if ( FAILED( m_pscSwapChain->SetFullscreenState( m_dscdSwapChainDesc.Windowed, NULL ) ) ) {
					CStd::DebugPrintA( "CDirectX11::SetScreenSize(): Failed to call SetFullscreenState()\r\n" );
				}
			}*/
			return;
		}
		if ( m_pscSwapChain ) {
			ReleaseBackBuffers();
			m_dscdSwapChainDesc.BufferDesc.Width = _ui32Width;
			m_dscdSwapChainDesc.BufferDesc.Height = _ui32Height;
			if ( SUCCEEDED( m_pscSwapChain->ResizeBuffers( m_dscdSwapChainDesc.BufferCount,
				_ui32Width, _ui32Height,
				m_dscdSwapChainDesc.BufferDesc.Format,
				0 ) ) ) {
				CreateSwapChainBackBuffers( m_diInitParms.hWnd );
			}
		}
	}

	/**
	 * Sets and applies all default render states.
	 */
	void LSE_CALL CDirectX11::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		ApplyRenderStates( true );
	}

	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	void LSE_CALL CDirectX11::ApplyRenderStates( LSBOOL _bForce ) {
		if ( _bForce ) {
			SetDefaultMatrices();
		}

		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Sampler states.
		uint32_t ui32Index = 0, ui32Total = 0;
		uint32_t ui32Max = LSG_MAX_SHADER_SAMPLERS;
		for ( uint32_t I = 0; I <= ui32Max; ++I ) {
			if ( I < ui32Max && LSG_CHANGED( pssSamplers[I] ) ) {
				++ui32Total;
				LSG_UPDATE( pssSamplers[I] );
			}
			else {
				if ( ui32Total ) {
					// The context holds a reference to the samplers.  Keep that in mind.
					GetDirectX11Context()->CSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
					GetDirectX11Context()->DSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
					GetDirectX11Context()->HSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
					GetDirectX11Context()->GSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
					GetDirectX11Context()->PSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
					GetDirectX11Context()->VSSetSamplers( ui32Index, ui32Total, &rsCurState.pssSamplers[ui32Index] );
				}
				ui32Index = I + 1;
				ui32Total = 0;
			}
		}


		// Rasterizer state.
		if ( LSG_CHANGED( prsRasterState ) ) {
			GetDirectX11Context()->RSSetState( rsCurState.prsRasterState );
			LSG_UPDATE( prsRasterState );
		}


		// Blend state.
		if ( LSG_CHANGED( pbsBlendState ) ||
			LSG_CHANGED( fBlendFactors[0] ) || LSG_CHANGED( fBlendFactors[1] ) || LSG_CHANGED( fBlendFactors[2] ) || LSG_CHANGED( fBlendFactors[3] ) ||
			LSG_CHANGED( ui32SampleMask ) ) {
			GetDirectX11Context()->OMSetBlendState( rsCurState.pbsBlendState,
				rsCurState.fBlendFactors,
				rsCurState.ui32SampleMask );
			LSG_UPDATE( pbsBlendState );
			LSG_UPDATE( fBlendFactors[0] );
			LSG_UPDATE( fBlendFactors[1] );
			LSG_UPDATE( fBlendFactors[2] );
			LSG_UPDATE( fBlendFactors[3] );
			LSG_UPDATE( ui32SampleMask );
		}


		// Depth/stencil state.
		if ( LSG_CHANGED( pdssDepthStencilState ) || LSG_CHANGED( ui32StencilRef ) ) {
			GetDirectX11Context()->OMSetDepthStencilState( rsCurState.pdssDepthStencilState,
				rsCurState.ui32StencilRef );
			LSG_UPDATE( pdssDepthStencilState );
			LSG_UPDATE( ui32StencilRef );
		}


		// Render targets.
		ApplyFramebuffer( _bForce );


		// Viewports.
		LSBOOL bSetViewports = LSG_CHANGED( ui8TotalViewportsAndScissors );
		LSBOOL bSetScissors = bSetViewports;
		if ( !bSetViewports ) {
			// Check for changes in each viewport.
			for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
				if ( LSG_CHANGED( vViewports[I].TopLeftX ) || LSG_CHANGED( vViewports[I].TopLeftY ) ||
					LSG_CHANGED( vViewports[I].Width ) || LSG_CHANGED( vViewports[I].Height ) ||
					LSG_CHANGED( vViewports[I].MinDepth ) || LSG_CHANGED( vViewports[I].MaxDepth ) ) {
					bSetViewports = true;
					break;
				}
			}
		}
		if ( bSetViewports ) {
			GetDirectX11Context()->RSSetViewports( rsCurState.ui8TotalViewportsAndScissors,
				rsCurState.vViewports );
			// Update our copies of what we just sent to the hardware.
			for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
				LSG_UPDATE( vViewports[I] );
			}
		}


		// Scissors.
		if ( !bSetScissors ) {
			// Check for changes in each scissor rectangle.
			for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
				if ( LSG_CHANGED( rScissors[I].left ) || LSG_CHANGED( rScissors[I].right ) ||
					LSG_CHANGED( rScissors[I].top ) || LSG_CHANGED( rScissors[I].bottom ) ) {
					bSetScissors = true;
					break;
				}
			}
		}
		if ( bSetScissors ) {
			GetDirectX11Context()->RSSetScissorRects( rsCurState.ui8TotalViewportsAndScissors,
				rsCurState.rScissors );
			// Update our copies of what we just sent to the hardware.
			for ( uint32_t I = m_rsCurRenderState.ui8TotalViewportsAndScissors; I--; ) {
				LSG_UPDATE( rScissors[I] );
			}
		}
		LSG_UPDATE( ui8TotalViewportsAndScissors );


		// Textures.
		ui32Index = 0;
		ui32Total = 0;
		ui32Max = _bForce ? CGfxBase::m_mMetrics.ui32MaxTexSlot : CStd::Max( rsCurState.ui32MaxTextureSet, rsLastState.ui32MaxTextureSet );
		for ( uint32_t I = 0; I <= ui32Max; ++I ) {
			if ( I < ui32Max && LSG_CHANGED( psrShaderResources[I] ) ) {
				++ui32Total;
				LSG_UPDATE( psrShaderResources[I] );
			}
			else {
				if ( ui32Total ) {
					// The context holds a reference to the resources.  Keep that in mind.
					GetDirectX11Context()->CSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
					GetDirectX11Context()->DSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
					GetDirectX11Context()->HSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
					GetDirectX11Context()->GSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
					GetDirectX11Context()->PSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
					GetDirectX11Context()->VSSetShaderResources( ui32Index, ui32Total, &rsCurState.psrShaderResources[ui32Index] );
				}
				ui32Index = I + 1;
				ui32Total = 0;
			}
		}

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Applies the currently set framebuffer. The render targets in the framebuffer are not set until this
	 *	is called.
	 *
	 * \param _bForce If true, all render targets are applied, even if they are redundant.
	 * \return Returns true if any buffers were activated.
	 */
	LSBOOL LSE_CALL CDirectX11::ApplyFramebuffer( LSBOOL _bForce ) {
		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER
		
		if ( m_rsCurRenderState.pfFramebuffer ) {
			m_rsCurRenderState.pfFramebuffer->Activate();
		}
		else {
			ActivateBackBuffers();
		}

		// There must be at least 1 render target.
		m_rsCurRenderState.ui8TotalRenderTargets = CStd::Max<uint8_t>( m_rsCurRenderState.ui8TotalRenderTargets, 1 );
		// Render target 0 must not be NULL.
		if ( !m_rsCurRenderState.prtRenderTargets[0] ) {
			m_rsCurRenderState.prtRenderTargets[0] = GetBackBufferSurface();
		}
		LSBOOL bBindRender = LSG_CHANGED( pdsDepthStencil ) || LSG_CHANGED( ui8TotalRenderTargets );
		if ( !bBindRender ) {
			for ( uint32_t I = rsCurState.ui8TotalRenderTargets; I--; ) {
				if ( LSG_CHANGED( prtRenderTargets[I] ) ) {
					bBindRender = true;
					break;
				}
			}
		}
		if ( bBindRender ) {
			GetDirectX11Context()->OMSetRenderTargets( rsCurState.ui8TotalRenderTargets,
				rsCurState.prtRenderTargets,
				rsCurState.pdsDepthStencil );
			// Update our copies of all the render targets.
			for ( uint32_t I = LSE_ELEMENTS( rsCurState.prtRenderTargets ); I--; ) {
				// Those indexed above the value passed to OMSetRenderTargets() are set to NULL
				//	inside the device so do the same with our copies.
				if ( I >= m_rsCurRenderState.ui8TotalRenderTargets ) {
					m_rsCurRenderState.prtRenderTargets[I] = NULL;
				}
				LSG_UPDATE( prtRenderTargets[I] );
			}
			LSG_UPDATE( pdsDepthStencil );
			LSG_UPDATE( ui8TotalRenderTargets );
		}
		return bBindRender;
#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	void LSE_FCALL CDirectX11::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL CDirectX11::GetShader() {
		return m_psmShaderMan->GetShader();
	}

#define LSG_DESCFUNCDEF_CLASS CDirectX11
#include "../Gfx/LSGDescFuncDef.h"

	/**
	 * Release all resources and reset our internal state.
	 */
	void LSE_CALL CDirectX11::ResetInternal() {
		ReleaseBackBuffers();

		// Default states.
		SafeRelease( m_pssDefaultSamplerState );
		SafeRelease( m_prsDefaultRasterizerState );
		SafeRelease( m_pbsDefaultBlendState );
		SafeRelease( m_pdssDefaultDepthStencilState );

		// Device.
		SafeRelease( m_pscSwapChain );
		SafeRelease( m_pdcContext );
		SafeRelease( m_pdDevice );
		m_dtDriver = D3D_DRIVER_TYPE_NULL;
	}

	/**
	 * Release only resources related to the back buffers (but not the swap chain).
	 */
	void LSE_CALL CDirectX11::ReleaseBackBuffers() {
		SafeRelease( m_pdsvDepthStencilView );
		SafeRelease( m_pt2dDepthStencilBuffer );
		SafeRelease( m_prtvRenderTargetView );
	}

	/**
	 * Creates a swap chain and render targets.
	 *
	 * \param _hWnd The window for which to create the swap chain.
	 * \return Returns false if it is unable to acquire enough resources.
	 */
	LSBOOL LSE_CALL CDirectX11::CreateSwapChainBackBuffers( HWND /*_hWnd*/ ) {
		ReleaseBackBuffers();


		// Create the color buffer.
		ID3D11Texture2D * ptBuffer = NULL;
		if ( FAILED( m_pscSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ),
			reinterpret_cast<void **>(&ptBuffer) ) ) ) {
			CStd::DebugPrintA( "Failed to access front buffer.\r\n" );
			ReleaseBackBuffers();
			return false;
		}
		if ( FAILED( m_pdDevice->CreateRenderTargetView( ptBuffer, NULL, &m_prtvRenderTargetView ) ) ) {
			CStd::DebugPrintA( "Failed to create the main render target.\r\n" );
			ReleaseBackBuffers();
			SafeRelease( ptBuffer );
			return false;
		}
		SafeRelease( ptBuffer );
		

		// Make the depth/stencil buffer.
		D3D11_TEXTURE2D_DESC tdDepthStencilDesc = { 0 };
		tdDepthStencilDesc.Width = m_dscdSwapChainDesc.BufferDesc.Width;
		tdDepthStencilDesc.Height = m_dscdSwapChainDesc.BufferDesc.Height;
		tdDepthStencilDesc.MipLevels = 1;
		tdDepthStencilDesc.ArraySize = 1;
		tdDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		tdDepthStencilDesc.SampleDesc.Count = 1;
		tdDepthStencilDesc.SampleDesc.Quality = 0;
		tdDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		tdDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		tdDepthStencilDesc.CPUAccessFlags = 0;
		tdDepthStencilDesc.MiscFlags = 0;
		if ( FAILED( m_pdDevice->CreateTexture2D( &tdDepthStencilDesc, NULL, &m_pt2dDepthStencilBuffer ) ) ) {
			CStd::DebugPrintA( "Failed to create the main depth/stencil buffer.\r\n" );
			ReleaseBackBuffers();
			return false;
		}

		// And now the view.
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvdDesc = { tdDepthStencilDesc.Format };
		dsvdDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvdDesc.Texture2D.MipSlice = 0;
		if ( FAILED( m_pdDevice->CreateDepthStencilView( m_pt2dDepthStencilBuffer, &dsvdDesc, &m_pdsvDepthStencilView ) ) ) {
			CStd::DebugPrintA( "Failed to create the depth/stencil view.\r\n" );
			ReleaseBackBuffers();
			return false;
		}

		ApplyDefaultStates();
		return true;
	}

	/**
	 * Converts a LSG_SAMPLER_DESC structure to a D3D11_SAMPLER_DESC structure.
	 *
	 * \param _sdIn The input LSG_SAMPLER_DESC structure.
	 * \return Returns a D3D11_SAMPLER_DESC representing the input _sdIn.
	 */
	D3D11_SAMPLER_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_SAMPLER_DESC &_sdIn ) {
		D3D11_SAMPLER_DESC sdOut = { D3D11_FILTER_MIN_MAG_MIP_LINEAR };
#define LSG_COPYMEMBER( MEMBER )	sdOut.MEMBER = CStd::ImpCast( sdOut.MEMBER, _sdIn.MEMBER )
		LSG_COPYMEMBER( Filter );
		LSG_COPYMEMBER( AddressU );
		LSG_COPYMEMBER( AddressV );
		LSG_COPYMEMBER( AddressW );
		LSG_COPYMEMBER( MipLODBias );
		LSG_COPYMEMBER( MaxAnisotropy );
		LSG_COPYMEMBER( ComparisonFunc );
		LSG_COPYMEMBER( BorderColor[0] );
		LSG_COPYMEMBER( BorderColor[1] );
		LSG_COPYMEMBER( BorderColor[2] );
		LSG_COPYMEMBER( BorderColor[3] );
		LSG_COPYMEMBER( MinLOD );
		LSG_COPYMEMBER( MaxLOD );
#undef LSG_COPYMEMBER

		return sdOut;
	}

	/**
	 * Converts a LSG_RASTERIZER_DESC structure to a D3D11_RASTERIZER_DESC structure.
	 *
	 * \param _rdIn The input LSG_RASTERIZER_DESC structure.
	 * \return Returns a D3D11_RASTERIZER_DESC representing the input _rdIn.
	 */
	D3D11_RASTERIZER_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_RASTERIZER_DESC &_rdIn ) {
		D3D11_RASTERIZER_DESC rdOut = { D3D11_FILL_SOLID };
#define LSG_COPYMEMBER( MEMBER )	rdOut.MEMBER = CStd::ImpCast( rdOut.MEMBER, _rdIn.MEMBER )
		LSG_COPYMEMBER( FillMode );
		LSG_COPYMEMBER( CullMode );
		LSG_COPYMEMBER( FrontCounterClockwise );
		LSG_COPYMEMBER( DepthBias );
		LSG_COPYMEMBER( DepthBiasClamp );
		LSG_COPYMEMBER( SlopeScaledDepthBias );
		LSG_COPYMEMBER( DepthClipEnable );
		LSG_COPYMEMBER( ScissorEnable );
		LSG_COPYMEMBER( MultisampleEnable );
	    LSG_COPYMEMBER( AntialiasedLineEnable );
#undef LSG_COPYMEMBER

		return rdOut;
	}

	/**
	 * Converts a LSG_RENDER_TARGET_BLEND_DESC structure to a D3D11_RENDER_TARGET_BLEND_DESC structure.
	 *
	 * \param _rtbdIn The input LSG_RENDER_TARGET_BLEND_DESC structure.
	 * \return Returns a D3D11_RENDER_TARGET_BLEND_DESC representing the input _rtbdIn.
	 */
	D3D11_RENDER_TARGET_BLEND_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_RENDER_TARGET_BLEND_DESC &_rtbdIn ) {
		D3D11_RENDER_TARGET_BLEND_DESC rtbdOut = { false };
#define LSG_COPYMEMBER( MEMBER )	rtbdOut.MEMBER = CStd::ImpCast( rtbdOut.MEMBER, _rtbdIn.MEMBER )
		LSG_COPYMEMBER( BlendEnable );
		LSG_COPYMEMBER( SrcBlend );
		LSG_COPYMEMBER( DestBlend );
		LSG_COPYMEMBER( BlendOp );
		LSG_COPYMEMBER( SrcBlendAlpha );
		LSG_COPYMEMBER( DestBlendAlpha );
		LSG_COPYMEMBER( BlendOpAlpha );
		LSG_COPYMEMBER( RenderTargetWriteMask );
#undef LSG_COPYMEMBER

		return rtbdOut;
	}

	/**
	 * Converts a LSG_BLEND_DESC structure to a D3D11_BLEND_DESC structure.
	 *
	 * \param _bdIn The input LSG_BLEND_DESC structure.
	 * \return Returns a D3D11_BLEND_DESC representing the input _bdIn.
	 */
	D3D11_BLEND_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_BLEND_DESC &_bdIn ) {
		D3D11_BLEND_DESC bdOut = { false };
#define LSG_COPYMEMBER( MEMBER )	bdOut.MEMBER = CStd::ImpCast( bdOut.MEMBER, _bdIn.MEMBER )
		LSG_COPYMEMBER( AlphaToCoverageEnable );
		LSG_COPYMEMBER( IndependentBlendEnable );
		assert( LSE_ELEMENTS( _bdIn.RenderTarget ) == LSE_ELEMENTS( bdOut.RenderTarget ) );
		for ( uint32_t I = LSE_ELEMENTS( bdOut.RenderTarget ); I--; ) {
			bdOut.RenderTarget[I] = ConvertDesc( _bdIn.RenderTarget[I] );
		}
#undef LSG_COPYMEMBER

		return bdOut;
	}

	/**
	 * Converts a LSG_DEPTH_STENCILOP_DESC structure to a D3D11_DEPTH_STENCILOP_DESC structure.
	 *
	 * \param _dsodIn The input LSG_DEPTH_STENCILOP_DESC structure.
	 * \return Returns a D3D11_DEPTH_STENCILOP_DESC representing the input _dsodIn.
	 */
	D3D11_DEPTH_STENCILOP_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_DEPTH_STENCILOP_DESC &_dsodIn ) {
		D3D11_DEPTH_STENCILOP_DESC dsodOut = { D3D11_STENCIL_OP_KEEP };
#define LSG_COPYMEMBER( MEMBER )	dsodOut.MEMBER = CStd::ImpCast( dsodOut.MEMBER, _dsodIn.MEMBER )
		LSG_COPYMEMBER( StencilFailOp );
		LSG_COPYMEMBER( StencilDepthFailOp );
		LSG_COPYMEMBER( StencilPassOp );
		LSG_COPYMEMBER( StencilFunc );
#undef LSG_COPYMEMBER

		return dsodOut;
	}

	/**
	 * Converts a LSG_DEPTH_STENCIL_DESC structure to a D3D11_DEPTH_STENCIL_DESC structure.
	 *
	 * \param _dsdIn The input LSG_DEPTH_STENCIL_DESC structure.
	 * \return Returns a D3D11_DEPTH_STENCIL_DESC representing the input _dsdIn.
	 */
	D3D11_DEPTH_STENCIL_DESC LSE_FCALL CDirectX11::ConvertDesc( const LSG_DEPTH_STENCIL_DESC &_dsdIn ) {
		D3D11_DEPTH_STENCIL_DESC dsdOut = { true };

#define LSG_COPYMEMBER( MEMBER )	dsdOut.MEMBER = CStd::ImpCast( dsdOut.MEMBER, _dsdIn.MEMBER )
		LSG_COPYMEMBER( DepthEnable );
		LSG_COPYMEMBER( DepthWriteMask );
		LSG_COPYMEMBER( DepthFunc );
		LSG_COPYMEMBER( StencilEnable );
		LSG_COPYMEMBER( StencilReadMask );
		LSG_COPYMEMBER( StencilWriteMask );
		dsdOut.FrontFace = ConvertDesc( _dsdIn.FrontFace );
		dsdOut.BackFace = ConvertDesc( _dsdIn.BackFace );
#undef LSG_COPYMEMBER

		return dsdOut;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
