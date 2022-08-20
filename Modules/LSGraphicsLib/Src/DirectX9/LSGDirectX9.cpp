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
 * Description: Interface with the DirectX 9 API.
 */

#include "LSGDirectX9.h"

#ifdef LSG_DX9

#include "../DirectX9Shaders/LSGDirectX9CompiledShaderManager.h"
#include "../DirectX9Shaders/LSGDirectX9ShaderManager.h"
#include "../Framebuffer/LSGFramebuffer.h"
#include "LSGDirectX9LosableResourceManager.h"

namespace lsg {

	// == Members.
	/** The DirectX 9 device. */
	IDirect3DDevice9 * CDirectX9::m_pd3dDevice = NULL;

	/** The DirectX 9 object. */
	IDirect3D9 * CDirectX9::m_d3dObject = NULL;

	/** The main/default window with which the device was created. */
	HWND CDirectX9::m_hMainWindow = NULL;

	/** The currently active window. */
	HWND CDirectX9::m_hCurWindow = NULL;

	/** The back buffer surface. */
	IDirect3DSurface9 * CDirectX9::m_pd3s9Back = NULL;

	/** The main depth/stencil buffer. */
	IDirect3DSurface9 * CDirectX9::m_pd3s9DepthStencil = NULL;

	/** Default sampler state. */
	LSG_SAMPLER_STATE * CDirectX9::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	LSG_RASTERIZER_STATE * CDirectX9::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	LSG_BLEND_STATE * CDirectX9::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE * CDirectX9::m_pdssDefaultDepthStencilState = NULL;

	/** The device capabilities. */
	D3DCAPS9 CDirectX9::m_cCaps = { static_cast<D3DDEVTYPE>(0) };

	/** Device creation parameters. */
	D3DDEVICE_CREATION_PARAMETERS CDirectX9::m_dcpCreationParms = { 0 };

	/** The last presentation parameters we used. */
	D3DPRESENT_PARAMETERS CDirectX9::m_ppPresentParms = { 0 };

	/** The shader manager. */
	CShaderManager * CDirectX9::m_psmShaderMan = NULL;

	/** D3DCREATE_HARDWARE_VERTEXPROCESSING or D3DCREATE_SOFTWARE_VERTEXPROCESSING. */
	int32_t CDirectX9::m_i32VertProcType = 0L;

	/** In the event in which there are multiple windows, this tracks the information
	 *	for each window. */
	CLinearMap<HWND, CDirectX9::LSG_WINDOW> CDirectX9::m_mWindows;

#define LSG_CREATESTATESDEF_CLASS CDirectX9
#include "../Gfx/LSGCreateStatesMembersDef.h"

	/** Is our device lost? */
	LSBOOL CDirectX9::m_bDeviceLost = false;

	/** Are we toggling full-screen? */
	LSBOOL CDirectX9::m_bToggling = false;

	/** The last render state. */
	CDirectX9::LSG_DX9_RENDER_STATE CDirectX9::m_rsLastRenderState;

	/** The current render state. */
	CDirectX9::LSG_DX9_RENDER_STATE CDirectX9::m_rsCurRenderState;

	/** Last samplers actually set on the rendering API. */
	LSG_SAMPLER_STATE CDirectX9::m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS] = { { static_cast<LSG_FILTER>(0) } };

	/** Last rasterizer states actually set on the rendering API. */
	LSG_RASTERIZER_STATE CDirectX9::m_rsLastRaster = { static_cast<LSG_FILL_MODE>(0) };

	/** Last blend state actually set on the rendering API. */
	LSG_BLEND_STATE CDirectX9::m_bsLastBlend = { 0 };

	/** Last depth/stencil state actually set on the rendering API. */
	LSG_DEPTH_STENCIL_STATE CDirectX9::m_dssLastDepthStencil = { 0 };

	// == Functions.
	/**
	 * Initialize DirectX 9.
	 *
	 * \param _lpdiInit Data required to initialize the device.
	 * \return Returns false if there are any problem during initialization.
	 */
	LSBOOL LSE_CALL CDirectX9::InitDirectX9( LPCLSG_DIRECTX9_INIT _lpdiInit ) {
		ResetInternal();
		CStd::MemSet( &m_rsCurRenderState, 0, sizeof( m_rsCurRenderState ) );
		CStd::MemSet( &m_rsLastRenderState, 0, sizeof( m_rsLastRenderState ) );


		// Create a DirectX device.
		m_d3dObject = ::Direct3DCreate9( D3D_SDK_VERSION );
		if ( !m_d3dObject ) { return false; }

		// Get the device capabilities.
		if ( FAILED( m_d3dObject->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_cCaps ) ) ) { return false; }

		// The type of vertex processing we intend to use.  Try for hardware if possible.
		m_i32VertProcType = (m_cCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;


		// Get the width and height of the window.
		RECT rClient;
		::GetClientRect( _lpdiInit->hWnd, &rClient );
		m_ppPresentParms.BackBufferWidth			= rClient.right - rClient.left;
		m_ppPresentParms.BackBufferHeight			= rClient.bottom - rClient.top;
		m_ppPresentParms.BackBufferFormat			= D3DFMT_A8R8G8B8;
		m_ppPresentParms.BackBufferCount			= 1;
		m_ppPresentParms.MultiSampleType			= D3DMULTISAMPLE_NONE;
		m_ppPresentParms.MultiSampleQuality			= 0;
		m_ppPresentParms.SwapEffect					= D3DSWAPEFFECT_DISCARD;
		m_ppPresentParms.hDeviceWindow				= _lpdiInit->hWnd;
		m_ppPresentParms.Windowed					= _lpdiInit->bWindowed;
		m_ppPresentParms.EnableAutoDepthStencil		= TRUE;
		m_ppPresentParms.AutoDepthStencilFormat		= D3DFMT_D24S8;
		m_ppPresentParms.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
		m_ppPresentParms.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
		m_ppPresentParms.PresentationInterval		= D3DPRESENT_INTERVAL_IMMEDIATE;


		/*DWORD dwMs;
		m_d3dObject->CheckDeviceMultiSampleType( m_cCaps.AdapterOrdinal, 
			m_cCaps.DeviceType, m_ppPresentParms.BackBufferFormat, 
			FALSE, m_ppPresentParms.MultiSampleType, &dwMs );

		DWORD dwMsD;
		m_d3dObject->CheckDeviceMultiSampleType( m_cCaps.AdapterOrdinal, 
			m_cCaps.DeviceType, m_ppPresentParms.AutoDepthStencilFormat, 
			FALSE, m_ppPresentParms.MultiSampleType, &dwMsD );*/


		if ( FAILED( m_d3dObject->CreateDevice( D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			_lpdiInit->hWnd,
			m_i32VertProcType,
			&m_ppPresentParms,
			&m_pd3dDevice ) ) ) { return false; }

		

		m_hMainWindow = m_hCurWindow = _lpdiInit->hWnd;
		// Get the creation parameters.
		if ( FAILED( m_pd3dDevice->GetCreationParameters( &m_dcpCreationParms ) ) ) { return false; }

		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "CDirectX9::InitDirectX9(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}

		// Get the back buffer surface.
		if ( FAILED( m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pd3s9Back ) ) ) { return false; }
		if ( FAILED( m_pd3dDevice->GetDepthStencilSurface( &m_pd3s9DepthStencil ) ) ) { return false; }

		CGfxBase::m_mMetrics.bNonPo2 = !(m_cCaps.TextureCaps & D3DPTEXTURECAPS_POW2) && !(m_cCaps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
		CGfxBase::m_mMetrics.ui32MaxAniso = m_cCaps.MaxAnisotropy;
		CGfxBase::m_mMetrics.ui32MaxRenderTargetSize = CStd::Max( m_cCaps.MaxTextureHeight, m_cCaps.MaxTextureWidth );
		CGfxBase::m_mMetrics.ui32MaxRenderTargets = m_cCaps.NumSimultaneousRTs;
		CGfxBase::m_mMetrics.ui32MaxTextureWidth = m_cCaps.MaxTextureWidth;
		CGfxBase::m_mMetrics.ui32MaxTextureHeight = m_cCaps.MaxTextureHeight;
		CGfxBase::m_mMetrics.ui32MaxTextureArray = 1;
		CGfxBase::m_mMetrics.ui32MaxTexSlot = CStd::Min<uint32_t>( LSG_MAX_TEXTURE_UNITS, 16 );	// MaxTextureBlendStages and MaxSimultaneousTextures are for fixed-function only.  We use ps_3_0 which has 16 sampler units.
		CGfxBase::m_mMetrics.ui32MaxVertElements = m_cCaps.MaxPrimitiveCount == 0xFFFF ? m_cCaps.MaxPrimitiveCount : 0xFFFFFFFF;
		CGfxBase::m_mMetrics.ui32MaxPrimitives = m_cCaps.MaxPrimitiveCount;
		CGfxBase::m_mMetrics.bDdsSupport = true;
		CGfxBase::m_mMetrics.bPackedDepthStencil = true;
		CGfxBase::m_mMetrics.bDepthIsReadable = false;

		LSG_WINDOW wWindow = { NULL };
		wWindow.hWnd = _lpdiInit->hWnd;
		wWindow.pd3ds9BackBuffer = m_pd3s9Back;
		wWindow.pd3ds9DepthStencilBuffer = m_pd3s9DepthStencil;
		wWindow.ppPresent = m_ppPresentParms;
		if ( FAILED( m_pd3dDevice->GetSwapChain( 0, &wWindow.pd3dsc9SwapChain ) ) ) {
			DeleteWindow( wWindow );
			return false;
		}
		if ( !m_mWindows.Insert( wWindow.hWnd, wWindow ) ) {
			DeleteWindow( wWindow );
			return false;
		}

		// Create the default states.
		// Sampler.
		LSG_SAMPLER_DESC sdOurSamplerDesc;
		if ( FAILED( CreateSamplerState( DefaultSamplerDesc( sdOurSamplerDesc ), &m_pssDefaultSamplerState ) ) ) {
			CStd::DebugPrintA( "CDirectX9::InitDirectX9(): Failed to create default sampler state.\r\n" );
			ResetInternal();
			return false;
		}

		// Rasterizer.
		LSG_RASTERIZER_DESC rdOurRasterizerDesc;
		if ( FAILED( CreateRasterizerState( DefaultRasterizerDesc( rdOurRasterizerDesc ), &m_prsDefaultRasterizerState ) ) ) {
			CStd::DebugPrintA( "CDirectX9::InitDirectX9(): Failed to create default rasterizer state.\r\n" );
			ResetInternal();
			return false;
		}

		// Blend.
		LSG_BLEND_DESC bdOurBlendDesc;
		if ( FAILED( CreateBlendState( DefaultBlendDesc( bdOurBlendDesc ), &m_pbsDefaultBlendState ) ) ) {
			CStd::DebugPrintA( "CDirectX9::InitDirectX9(): Failed to create default blend state.\r\n" );
			ResetInternal();
			return false;
		}

		// DepthStencil.
		LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
		if ( FAILED( CreateDepthStencilState( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ), &m_pdssDefaultDepthStencilState ) ) ) {
			CStd::DebugPrintA( "CDirectX9::InitDirectX9(): Failed to create default depth/stencil state.\r\n" );
			ResetInternal();
			return false;
		}

		return true;
	}

	/**
	 * Secondary initialization of DirectX 9, after a device has been created.
	 */
	void LSE_CALL CDirectX9::PostInitDirectX9() {
		ApplyDefaultStates();
		Clear();
		Present();
	}

	/**
	 * Destroy DirectX 9 one time at engine shutdown.
	 */
	void LSE_CALL CDirectX9::DestroyDirectX9() {
		CDirectX9CompiledShaderManager::DeleteUnreferencedShaders();

		LSEDELETE m_psmShaderMan;
		m_psmShaderMan = NULL;

		CDirectX9LosableResourceManager::Destroy();
		ResetInternal();

		if ( m_mSamplerStateMap.Length() ) {
			CStd::DebugPrintA( "CDirectX9::DestroyDirectX9(): Unreleased sampler states detected.\r\n" );
		}
		if ( m_mRasterizerStateMap.Length() ) {
			CStd::DebugPrintA( "CDirectX9::DestroyDirectX9(): Unreleased rasterizer states detected.\r\n" );
		}
		if ( m_mBlendStateMap.Length() ) {
			CStd::DebugPrintA( "CDirectX9::DestroyDirectX9(): Unreleased blend states detected.\r\n" );
		}
		if ( m_mDepthStencilStateMap.Length() ) {
			CStd::DebugPrintA( "CDirectX9::DestroyDirectX9(): Unreleased depth/stencil states detected.\r\n" );
		}
	}

	/**
	 * Called when a device is lost.
	 */
	void LSE_CALL CDirectX9::LoseDevice() {
		if ( !m_bDeviceLost ) {
			if ( GetShaderManager() ) {
				GetShaderManager()->SetShader( NULL );
			}
			// Reset all swap chains.
			for ( uint32_t I = m_mWindows.Length(); I--; ) {
				DeleteWindow( m_mWindows.GetByIndex( I ) );
			}
			CDirectX9LosableResourceManager::OnLostDevice();
			ApplyDefaultStates();
			m_bDeviceLost = true;
		}
	}

	/**
	 * Reset the device.
	 */
	LSBOOL LSE_CALL CDirectX9::ResetDevice() {
		if ( m_bDeviceLost || m_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET ) {
			m_bDeviceLost = true;
			HRESULT hRes = m_pd3dDevice->Reset( &m_ppPresentParms );
			switch ( hRes ) {
				case D3D_OK : {
					// Recreate all windows.
					for ( uint32_t I = m_mWindows.Length(); I--; ) {
						LSG_WINDOW & wWindow = m_mWindows.GetByIndex( I );
						if ( wWindow.hWnd == m_hMainWindow ) {
							// Have to handle the device's implicit buffers differently.
							if ( FAILED( m_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &wWindow.pd3ds9BackBuffer ) ) ) { return false; }
							if ( FAILED( m_pd3dDevice->GetDepthStencilSurface( &wWindow.pd3ds9DepthStencilBuffer ) ) ) { return false; }
							if ( FAILED( m_pd3dDevice->GetSwapChain( 0, &wWindow.pd3dsc9SwapChain ) ) ) { return false; }
							wWindow.ppPresent = m_ppPresentParms;
							MakeWindowCurrent( m_hMainWindow );
						}
						else {
							// It is an extra registered window.
							CreateWindowSwapChain( wWindow );
						}
					}
					ApplyDefaultStates();
					CDirectX9LosableResourceManager::OnResetDevice();
					m_bDeviceLost = false;
					return true;
				}
				case D3DERR_DEVICELOST : {
					return false;
				}
				case D3DERR_DEVICEREMOVED : {
					return false;
				}
				case D3DERR_DRIVERINTERNALERROR : {
					return false;
				}
				case D3DERR_OUTOFVIDEOMEMORY : {
					return false;
				}
				case D3DERR_INVALIDCALL : {
					return false;
				}
				default : {
					return false;
				}
			}
		}
		return false;
	}

	/**
	 * Adds a window as a possible render destination.
	 *
	 * \param _hWnd The handle to the window.
	 * \return Returns true if there were enough resources available to register the window with
	 *	the system.
	 */
	LSBOOL LSE_CALL CDirectX9::AddWindow( HWND _hWnd ) {
		if ( m_mWindows.ItemExists( _hWnd ) ) {
			// Already registered.
			return true;
		}
		LSG_WINDOW wWindow = { NULL };
		wWindow.hWnd = _hWnd;
		wWindow.ppPresent = m_ppPresentParms;

		// Get the width and height of the window.
		RECT rClient;
		::GetClientRect( _hWnd, &rClient );
		wWindow.ppPresent.BackBufferWidth = rClient.right - rClient.left;
		wWindow.ppPresent.BackBufferHeight = rClient.bottom - rClient.top;
		wWindow.ppPresent.hDeviceWindow = _hWnd;

		if ( !CreateWindowSwapChain( wWindow ) ) { return false; }

		if ( !m_mWindows.Insert( wWindow.hWnd, wWindow ) ) {
			DeleteWindow( wWindow );
			return false;
		}
		return true;
	}

	/**
	 * Unregisters a window from the system.  Does not close the window.
	 *
	 * \param _hWnd The window to remove from the engine registry.
	 */
	void LSE_CALL CDirectX9::RemoveWindow( HWND _hWnd ) {
		uint32_t ui32Index;
		if ( m_mWindows.ItemExists( _hWnd, ui32Index ) ) {
			LSG_WINDOW & wWindow = m_mWindows.GetByIndex( ui32Index );
			uint32_t ui32NewIndex = LSE_MAXU32;
			if ( m_mWindows.Length() >= 2 ) {
				// Pick either index 0 or 1, whichever is not the one being deleted.
				ui32NewIndex = ui32Index == 0 ? 1 : 0;
			}
			// Set the new window data.
			if ( ui32NewIndex != LSE_MAXU32 ) {
				MakeWindowCurrent( m_mWindows.GetKeyByIndex( ui32NewIndex ) );
			}
			else {
				// Last window is being deleted.  Should only happen at shut-down.
				m_hCurWindow = NULL;
				m_pd3s9Back = m_pd3s9DepthStencil = NULL;
			}

			// Delete the window being deleted.
			DeleteWindow( wWindow );
			m_mWindows.RemoveByIndex( ui32Index );
			if ( m_mWindows.Length() == 0 ) {
				m_mWindows.Reset();
			}
		}
		else {
			CStd::DebugPrintA( "CDirectX9::RemoveWindow(): Provided window is not registered.\r\n" );
		}
	}

	/**
	 * Makes the given window, which must have previously been registered via a call to AddWindow(), the
	 *	current window.
	 *
	 * \param _hWnd The window to make current.
	 */
	void LSE_CALL CDirectX9::MakeWindowCurrent( HWND _hWnd ) {
		uint32_t ui32Index;
		if ( m_mWindows.ItemExists( _hWnd, ui32Index ) ) {
			LSG_WINDOW & wWindow = m_mWindows.GetByIndex( ui32Index );
			m_hCurWindow = _hWnd;
			m_pd3s9Back = wWindow.pd3ds9BackBuffer;
			m_pd3s9DepthStencil = wWindow.pd3ds9DepthStencilBuffer;

			GetDirectX9Device()->SetRenderTarget( 0, GetBackBufferSurface() );
			GetDirectX9Device()->SetDepthStencilSurface( GetDepthStencilSurface() );
			m_rsLastRenderState.prtRenderTargets[0] = GetBackBufferSurface();
			m_rsLastRenderState.pdsDepthStencil = GetDepthStencilSurface();
		}
	}

	/**
	 * Sets the screen dimensions.
	 *
	 * \param _ui32Width Screen width, in pixels.
	 * \param _ui32Height Screen height, in pixels.
	 */
	void LSE_CALL CDirectX9::SetScreenSize( uint32_t _ui32Width, uint32_t _ui32Height ) {
		if ( m_hCurWindow == m_hMainWindow ) {
			if ( m_ppPresentParms.BackBufferWidth == 0 && m_ppPresentParms.BackBufferHeight == 0 ) { return; }
			if ( m_ppPresentParms.BackBufferWidth == _ui32Width && m_ppPresentParms.BackBufferHeight == _ui32Height ) { return; }
			LoseDevice();

			m_ppPresentParms.BackBufferWidth = _ui32Width;
			m_ppPresentParms.BackBufferHeight = _ui32Height;
			if ( !m_bToggling ) {
				ResetDevice();
			}
			ApplyDefaultStates();
		}
		else {
			// Not necessary to reset the device.  Just resize the swap chain buffers.
			uint32_t ui32Index;
			if ( m_mWindows.ItemExists( m_hCurWindow, ui32Index ) ) {
				LSG_WINDOW & wWindow = m_mWindows.GetByIndex( ui32Index );
				if ( wWindow.ppPresent.BackBufferWidth == _ui32Width && wWindow.ppPresent.BackBufferHeight == _ui32Height ) { return; }
				DeleteWindow( wWindow );
				wWindow.ppPresent.BackBufferWidth = _ui32Width;
				wWindow.ppPresent.BackBufferHeight = _ui32Height;
				CreateWindowSwapChain( wWindow );
			}

			// Have to reset the back buffers etc.
			MakeWindowCurrent( m_hCurWindow );
		}
	}

	/**
	 * Sets and applies all default render states.
	 */
	void LSE_CALL CDirectX9::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		// States that are always set and non-configurable.
		GetDirectX9Device()->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE, TRUE );
		GetDirectX9Device()->SetRenderState( D3DRS_TWOSIDEDSTENCILMODE, TRUE );
		GetDirectX9Device()->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );

		// Other states.
		ApplyRenderStates( true );

		// Vertex buffers.
		GetDirectX9Device()->SetVertexDeclaration( static_cast<IDirect3DVertexDeclaration9 *>(const_cast<LSG_INPUT_LAYOUT *>(m_rsCurRenderState.pilInputLayout)) );
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			m_rsLastRenderState.pvbVertexBuffers[I].pvbVertBuffer = NULL;
		}
	}

	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	void LSE_CALL CDirectX9::ApplyRenderStates( LSBOOL _bForce ) {
		if ( _bForce ) {
			SetDefaultMatrices();
		}

		// Basic states.
		SetSamplerStates( _bForce );
		SetRasterizerState( _bForce );
		SetBlendState( _bForce );
		SetDepthStencilState( _bForce );

		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Render targets.
		LSBOOL bForceViewport = ApplyFramebuffer( _bForce );
		
		// Viewports.  Must be done AFTER setting render targets, as render targets change the viewport.
		for ( uint32_t I = LSG_MAX_VIEWPORTS_SCISSORS; I--; ) {
			if ( bForceViewport ||
				LSG_CHANGED( vViewports[I].X ) || LSG_CHANGED( vViewports[I].Y ) ||
				LSG_CHANGED( vViewports[I].Width ) || LSG_CHANGED( vViewports[I].Height ) ||
				LSG_CHANGED( vViewports[I].MinZ ) || LSG_CHANGED( vViewports[I].MaxZ ) ) {
				GetDirectX9Device()->SetViewport( &rsCurState.vViewports[I] );
				LSG_UPDATE( vViewports[I] );
			}
		}

		// Scissor rectangles.
		for ( uint32_t I = LSG_MAX_VIEWPORTS_SCISSORS; I--; ) {
			if ( LSG_CHANGED( rScissors[I].left ) || LSG_CHANGED( rScissors[I].top ) ||
				LSG_CHANGED( rScissors[I].right ) || LSG_CHANGED( rScissors[I].bottom ) ) {
				GetDirectX9Device()->SetScissorRect( &rsCurState.rScissors[I] );
				LSG_UPDATE( rScissors[I] );
			}
		}

		// Textures.
		uint32_t ui32Max = _bForce ? CGfxBase::m_mMetrics.ui32MaxTexSlot : CStd::Max( rsCurState.ui32MaxTextureSet, rsLastState.ui32MaxTextureSet );
		for ( uint32_t I = ui32Max; I--; ) {
			if ( LSG_CHANGED( psrShaderResources[I] ) ) {
				GetDirectX9Device()->SetTexture( I, rsCurState.psrShaderResources[I] );
				LSG_UPDATE( psrShaderResources[I] );
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
	LSBOOL LSE_CALL CDirectX9::ApplyFramebuffer( LSBOOL _bForce ) {
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
		LSBOOL bForceViewport = false;
		for ( uint32_t I = LSE_ELEMENTS( rsCurState.prtRenderTargets ); I--; ) {
			// Unset if higher than rsCurState.ui8TotalRenderTargets.
			if ( I >= rsCurState.ui8TotalRenderTargets ) {
				m_rsCurRenderState.prtRenderTargets[I] = NULL;
			}
			if ( LSG_CHANGED( prtRenderTargets[I] ) ) {
				GetDirectX9Device()->SetRenderTarget( I, rsCurState.prtRenderTargets[I] );
				// This causes the viewport to change.
				bForceViewport = true;
				LSG_UPDATE( prtRenderTargets[I] );
			}
		}

		// Depth/stencil buffer.
		if ( LSG_CHANGED( pdsDepthStencil ) ) {
			GetDirectX9Device()->SetDepthStencilSurface( rsCurState.pdsDepthStencil );
			LSG_UPDATE( pdsDepthStencil );
		}
		return bForceViewport;
#undef LSG_UPDATE
#undef LSG_CHANGED
	}

	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	void LSE_FCALL CDirectX9::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL CDirectX9::GetShader() {
		return m_psmShaderMan->GetShader();
	}

#define LSG_DESCFUNCDEF_CLASS CDirectX9
#include "../Gfx/LSGDescFuncDef.h"

#define LSG_CREATESTATESDEF_CLASS CDirectX9
#include "../Gfx/LSGCreateStatesDef.h"

	/**
	 * Release all resources and reset our internal state.
	 */
	void LSE_CALL CDirectX9::ResetInternal() {
		// Delete all windows.
		for ( uint32_t I = m_mWindows.Length(); I--; ) {
			RemoveWindow( m_mWindows.GetKeyByIndex( I ) );
		}

		ReleaseState( m_pssDefaultSamplerState );
		ReleaseState( m_prsDefaultRasterizerState );
		ReleaseState( m_pbsDefaultBlendState );
		ReleaseState( m_pdssDefaultDepthStencilState );

		SafeRelease( m_pd3dDevice );
		SafeRelease( m_d3dObject );
		m_i32VertProcType = D3DSPD_IUNKNOWN;
	}

	/**
	 * Safely deletes an LSG_WINDOW object.
	 *
	 * \param _wWindow The object to delete.
	 */
	void LSE_CALL CDirectX9::DeleteWindow( LSG_WINDOW &_wWindow ) {
		for ( uint32_t I = LSE_ELEMENTS( m_rsCurRenderState.prtRenderTargets ); I--; ) {
			if ( m_rsCurRenderState.prtRenderTargets[I] == _wWindow.pd3ds9BackBuffer ) {
				m_rsCurRenderState.prtRenderTargets[I] = NULL;
			}
		}
		if ( m_rsCurRenderState.pdsDepthStencil == _wWindow.pd3ds9DepthStencilBuffer ) {
			m_rsCurRenderState.pdsDepthStencil = NULL;
		}

		if ( _wWindow.pd3ds9BackBuffer == m_pd3s9Back ) { m_pd3s9Back = NULL; }
		if ( _wWindow.pd3ds9DepthStencilBuffer == m_pd3s9DepthStencil ) { m_pd3s9DepthStencil = NULL; }

		SafeRelease( _wWindow.pd3ds9BackBuffer );
		SafeRelease( _wWindow.pd3ds9DepthStencilBuffer );
		SafeRelease( _wWindow.pd3dsc9SwapChain );
	}

	/**
	 * Creates a swap chain, back buffer, and depth/stencil buffer for a window object.  The ppPresent
	 *	member must be fully filled.
	 *
	 * \param _wWindow The object whose swap-chain features are to be created.
	 * \return Returns false if the creation of any of the resources fail.  Upon failure, the any created
	 *	resources will be freed.
	 */
	LSBOOL LSE_CALL CDirectX9::CreateWindowSwapChain( LSG_WINDOW &_wWindow ) {
		if ( FAILED( m_pd3dDevice->CreateAdditionalSwapChain( &_wWindow.ppPresent, &_wWindow.pd3dsc9SwapChain ) ) ) {
			DeleteWindow( _wWindow );
			return false;
		}
		if ( FAILED( _wWindow.pd3dsc9SwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &_wWindow.pd3ds9BackBuffer ) ) ) {
			DeleteWindow( _wWindow );
			return false;
		}
		if ( FAILED( m_pd3dDevice->CreateDepthStencilSurface( _wWindow.ppPresent.BackBufferWidth,
			_wWindow.ppPresent.BackBufferHeight,
			_wWindow.ppPresent.AutoDepthStencilFormat,
			D3DMULTISAMPLE_NONE,
			0,
			TRUE,
			&_wWindow.pd3ds9DepthStencilBuffer,
			NULL ) ) ) {
			DeleteWindow( _wWindow );
			return false;
		}
		return true;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
