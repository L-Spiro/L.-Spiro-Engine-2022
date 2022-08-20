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
 * Description: Interface with the Metal API.
 */

#include "LSGMetal.h"

#ifdef LSG_METAL

#include "../MetalShaders/LSGMetalCompiledShaderManager.h"
#include "../MetalShaders/LSGMetalShaderManager.h"
#include "../MetalShaders/LSGMetalShaderProgramManager.h"
#include "LSGMetalObject.h"

#import <Metal/Metal.h>

namespace lsg {

	/** The shader manager. */
	CShaderManager * CMetal::m_psmShaderMan = NULL;
	
#define LSG_CREATESTATESDEF_CLASS CMetal
#include "../Gfx/LSGCreateStatesMembersDef.h"

	/** Default sampler state. */
	LSG_SAMPLER_STATE * CMetal::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	LSG_RASTERIZER_STATE * CMetal::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	LSG_BLEND_STATE * CMetal::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE * CMetal::m_pdssDefaultDepthStencilState = NULL;

	/** The last render state. */
	CMetal::LSG_METAL_RENDER_STATE CMetal::m_rsLastRenderState = {};

	/** The current render state. */
	CMetal::LSG_METAL_RENDER_STATE CMetal::m_rsCurRenderState = {};
	
	/** Last samplers actually set on the rendering API. */
	LSG_SAMPLER_STATE CMetal::m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS] = { { static_cast<LSG_FILTER>(0) } };

	/** Last rasterizer states actually set on the rendering API. */
	LSG_RASTERIZER_STATE CMetal::m_rsLastRaster = { static_cast<LSG_FILL_MODE>(0) };

	/** Last blend state actually set on the rendering API. */
	LSG_BLEND_STATE CMetal::m_bsLastBlend = { 0 };

	/** Last depth/stencil state actually set on the rendering API. */
	LSG_DEPTH_STENCIL_STATE CMetal::m_dssLastDepthStencil = { 0 };
	
	/** The main Metal device. */
	LSVOID * CMetal::m_pvMetalDevice = NULL;
	
	/** The descriptor for clearing render targets. */
	LSG_RENDER_PASS_DESCRIPTOR CMetal::m_rpdClearDesc;

	// == Functions.
	/**
	 * Initialize Metal.
	 *
	 * \param _pvParm Varies by operating system.
	 */
	LSBOOL LSE_CALL CMetal::InitMetal( LSVOID * _pvParm ) {
		ResetInternal( false );
		
		CGfxBase::m_mMetrics.bNonPo2 = true;
		CGfxBase::m_mMetrics.ui32MaxAniso = 8;
		CGfxBase::m_mMetrics.ui32MaxRenderTargetSize = 4096;
		CGfxBase::m_mMetrics.ui32MaxRenderTargets = LSG_MAX_RENDER_TARGET_COUNT;
		CGfxBase::m_mMetrics.ui32MaxTextureWidth = 4096;
		CGfxBase::m_mMetrics.ui32MaxTextureHeight = 4096;
		CGfxBase::m_mMetrics.ui32MaxTextureArray = 2048;
		CGfxBase::m_mMetrics.ui32MaxTexSlot = LSG_MAX_TEXTURE_UNITS;
		CGfxBase::m_mMetrics.ui32MaxVertElements = 0xFFFFFFFF;
		CGfxBase::m_mMetrics.ui32MaxPrimitives = 0xFFFFFFFF;
		CGfxBase::m_mMetrics.bDdsSupport = false;
		CGfxBase::m_mMetrics.bPackedDepthStencil = false;
		CGfxBase::m_mMetrics.bDepthIsReadable = true;
		
		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "CMetal::InitMetal(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}
		
		// Create the default states.
		// Sampler.
		LSG_SAMPLER_DESC sdOurSamplerDesc;
		if ( FAILED( CreateSamplerState( DefaultSamplerDesc( sdOurSamplerDesc ), &m_pssDefaultSamplerState ) ) ) {
			CStd::DebugPrintA( "CMetal::InitMetal(): Failed to create default sampler state.\r\n" );
			ResetInternal();
			return false;
		}

		// Rasterizer.
		LSG_RASTERIZER_DESC rdOurRasterizerDesc;
		if ( FAILED( CreateRasterizerState( DefaultRasterizerDesc( rdOurRasterizerDesc ), &m_prsDefaultRasterizerState ) ) ) {
			CStd::DebugPrintA( "CMetal::InitMetal(): Failed to create default rasterizer state.\r\n" );
			ResetInternal();
			return false;
		}

		// Blend.
		LSG_BLEND_DESC bdOurBlendDesc;
		if ( FAILED( CreateBlendState( DefaultBlendDesc( bdOurBlendDesc ), &m_pbsDefaultBlendState ) ) ) {
			CStd::DebugPrintA( "CMetal::InitMetal(): Failed to create default blend state.\r\n" );
			ResetInternal();
			return false;
		}

		// DepthStencil.
		LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
		if ( FAILED( CreateDepthStencilState( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ), &m_pdssDefaultDepthStencilState ) ) ) {
			CStd::DebugPrintA( "CMetal::InitMetal(): Failed to create default depth/stencil state.\r\n" );
			ResetInternal();
			return false;
		}

		return true;
	}

	/**
	 * Secondary initialization of Metal, after a context has been created.
	 */
	LSVOID LSE_CALL CMetal::PostInitMetal() {
		// Apply states.
		ApplyDefaultStates();
		BeginRender();
		Clear();
		Present();
		EndRender();
	}

	/**
	 * Destroy the Metal render window.
	 */
	LSVOID LSE_CALL CMetal::DestroyMetal() {
		CMetalCompiledShaderManager::DeleteUnreferencedShaders();
		CMetalShaderProgramManager::DeleteUnreferencedShaderPrograms();

		ResetInternal();
	}
	
	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	LSVOID LSE_FCALL CMetal::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL CMetal::GetShader() {
		return m_psmShaderMan->GetShader();
	}
	
	/**
	 * Begin rendering.  Must be called once per frame only, before any rendering takes place.
	 *
	 * \return Returns true if the rendering device is ready.
	 */
	LSBOOL LSE_FCALL CMetal::BeginRender() {
		if ( GetDeviceObject() ) {
			[GetDeviceObject() beginFrame];
			m_rsCurRenderState.prtRenderTargets[0] = (__bridge LSG_RENDER_TARGET *)GetBackBuffer().texture;
			for ( LSUINT32 I = 1; I < LSG_MAX_RENDER_TARGET_COUNT; ++I ) {
				m_rsCurRenderState.prtRenderTargets[1] = NULL;
			}
			m_rsCurRenderState.pdsDepthStencil = (__bridge LSG_DEPTH_STENCIL *)GetDepthBuffer();
			m_rsCurRenderState.pdsStencil = (__bridge LSG_DEPTH_STENCIL *)GetStencilBuffer();
		}
		return true;
	}

	/**
	 * End rendering.  Must be called once per frame only, after all rendering is done.
	 */
	LSVOID LSE_FCALL CMetal::EndRender() {
	}

	/**
	 * Sets and applies all default render states.
	 */
	LSVOID LSE_CALL CMetal::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		ApplyRenderStates( true );
	}
	
	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	LSVOID LSE_CALL CMetal::ApplyRenderStates( LSBOOL _bForce ) {
		if ( _bForce ) {
			SetDefaultMatrices();
		}

		/*const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;*/

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Basic states.
		SetSamplerStates( _bForce );		// Must be done after textures are set.
		SetRasterizerState( _bForce );
		SetBlendState( _bForce );
		SetDepthStencilState( _bForce );

#undef LSG_UPDATE
#undef LSG_CHANGED
	}
	
	/**
	 * Present the backbuffer to the screen.
	 */
	LSVOID LSE_FCALL CMetal::Present() {
		if ( m_pvMetalDevice && m_bActive ) {

			if ( GetBackBuffer() ) {
				[GetCommandBuffer() present:GetBackBuffer()];
			}
		}
	}
	
	/**
	 * Clear the active buffer of the given type(s).
	 *
	 * \param _ui32Mask The buffer(s) to clear.  Must be one or more LSG_BUFFERS enumerated values.
	 */
	LSVOID LSE_FCALL CMetal::Clear( LSUINT32 _ui32Mask ) {
		if ( !m_bActive ) { return; }
		
		for ( LSUINT32 I = LSG_MAX_RENDER_TARGET_COUNT; I--; ) {
			m_rpdClearDesc.dbColors[I].tTexture = m_rsCurRenderState.prtRenderTargets[I];
			if ( m_rpdClearDesc.dbColors[I].tTexture ) {
				m_rpdClearDesc.dbColors[I].fClearColor[0] = m_rsCurRenderState.fClearColor[0];
				m_rpdClearDesc.dbColors[I].fClearColor[1] = m_rsCurRenderState.fClearColor[1];
				m_rpdClearDesc.dbColors[I].fClearColor[2] = m_rsCurRenderState.fClearColor[2];
				m_rpdClearDesc.dbColors[I].fClearColor[3] = m_rsCurRenderState.fClearColor[3];
			}
			m_rpdClearDesc.dbColors[I].bClear = (_ui32Mask & LSG_B_COLOR_BUFFER) != 0;
		}
		
		m_rpdClearDesc.dbDepth.tTexture = m_rsCurRenderState.pdsDepthStencil;
		m_rpdClearDesc.dbDepth.fClearDepth = m_rsCurRenderState.fClearDepth;
		m_rpdClearDesc.dbDepth.bClear = (_ui32Mask & LSG_B_DEPTH_BUFFER) != 0;
		
		m_rpdClearDesc.dbStencil.tTexture = m_rsCurRenderState.pdsStencil;
		m_rpdClearDesc.dbStencil.ui8ClearStencil = m_rsCurRenderState.ui8ClearStencil;
		m_rpdClearDesc.dbStencil.bClear = (_ui32Mask & LSG_B_STENCIL_BUFFER) != 0;
		
		[GetCommandBuffer() startPass:m_rpdClearDesc];
	}
	
#define LSG_DESCFUNCDEF_CLASS CMetal
#include "../Gfx/LSGDescFuncDef.h"
	
	/**
	 * Release all resources and reset our internal state.
	 *
	 * \param _bResetContext If true, the context is also deleted.
	 */
	LSVOID LSE_CALL CMetal::ResetInternal( LSBOOL _bResetContext ) {
		ReleaseState( m_pssDefaultSamplerState );
		ReleaseState( m_prsDefaultRasterizerState );
		ReleaseState( m_pbsDefaultBlendState );
		ReleaseState( m_pdssDefaultDepthStencilState );

		if ( _bResetContext ) {
		}
	}

#define LSG_CREATESTATESDEF_CLASS CMetal
#include "../Gfx/LSGCreateStatesDef.h"

}	// namespace lsg

#endif	// #ifdef LSG_METAL
