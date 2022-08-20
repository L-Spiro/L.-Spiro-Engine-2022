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
 * Description: Interface with the OpenGL ES * API.
 */

#include "LSGOpenGlEs.h"

#ifdef LSG_OGLES

#include "../Framebuffer/LSGFramebuffer.h"
#include "../OpenGLESShaders/LSGOpenGlEsCompiledShaderManager.h"
#include "../OpenGLESShaders/LSGOpenGlEsShaderManager.h"
#include "../OpenGLESShaders/LSGOpenGlEsShaderProgramManager.h"

namespace lsg {

	// == Various contructors.
	LSE_CALLCTOR COpenGlEs::CTempTexture2dBinder::CTempTexture2dBinder( GLuint _uiId ) {
		m_csBindTextureCrit.Lock();
		m_uiLastTexture = COpenGlEs::TextureBinding2d( 0 );
		COpenGlEs::BindTexture2d( _uiId, 0 );
	}
	LSE_CALLCTOR COpenGlEs::CTempTexture2dBinder::~CTempTexture2dBinder() {
		COpenGlEs::BindTexture2d( m_uiLastTexture, 0 );
		m_csBindTextureCrit.UnLock();
	}
	
	// == Various contructors.
	LSE_CALLCTOR COpenGlEs::CTempFramebufferBinder::CTempFramebufferBinder( GLuint _uiId ) {
		m_csFramebufferCrit.Lock();
		m_uiLastFramebuffer = m_rsCurRenderState.uiFramebuffer;
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGlEs::BindFramebuffer( _uiId );
		glWarnErrorAlways( "Invalid framebuffer ID" );
	}
	LSE_CALLCTOR COpenGlEs::CTempFramebufferBinder::~CTempFramebufferBinder() {
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGlEs::BindFramebuffer( m_uiLastFramebuffer );
		glWarnErrorAlways( "Invalid framebuffer ID" );
		m_csFramebufferCrit.UnLock();
	}

	// == Members.
	/** The ::glGetError() critical section. */
	CCriticalSection COpenGlEs::m_csCrit;
	
	/** The critical section for binding textures. */
	CCriticalSection COpenGlEs::m_csBindTextureCrit;

	/** The critical section for binding framebuffers. */
	CCriticalSection COpenGlEs::m_csFramebufferCrit;
	
	/** The main OpenGL ES framebuffer. */
	GLuint COpenGlEs::m_uiBackFramebuffer = 0;
	
	/** The main OpenGL ES renderbuffer. */
	GLuint COpenGlEs::m_uiBackRenderbuffer = 0;
		
	/** The shader manager. */
	CShaderManager * COpenGlEs::m_psmShaderMan = NULL;
	
#define LSG_CREATESTATESDEF_CLASS COpenGlEs
#include "../Gfx/LSGCreateStatesMembersDef.h"

	/** Default sampler state. */
	LSG_SAMPLER_STATE * COpenGlEs::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	LSG_RASTERIZER_STATE * COpenGlEs::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	LSG_BLEND_STATE * COpenGlEs::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE * COpenGlEs::m_pdssDefaultDepthStencilState = NULL;

	/** The last render state. */
	COpenGlEs::LSG_OPENGL_RENDER_STATE COpenGlEs::m_rsLastRenderState = {};

	/** The current render state. */
	COpenGlEs::LSG_OPENGL_RENDER_STATE COpenGlEs::m_rsCurRenderState = {};
	
	/** Last samplers actually set on the rendering API. */
	LSG_SAMPLER_STATE COpenGlEs::m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS] = { { static_cast<LSG_FILTER>(0) } };

	/** Last rasterizer states actually set on the rendering API. */
	LSG_RASTERIZER_STATE COpenGlEs::m_rsLastRaster = { static_cast<LSG_FILL_MODE>(0) };

	/** Last blend state actually set on the rendering API. */
	LSG_BLEND_STATE COpenGlEs::m_bsLastBlend = { 0 };

	/** Last depth/stencil state actually set on the rendering API. */
	LSG_DEPTH_STENCIL_STATE COpenGlEs::m_dssLastDepthStencil = { 0 };
	
	/** The main OpenGL ES context. */
	void * COpenGlEs::m_pvContext0 = NULL;

	// == Functions.
	/**
	 * Initialize OpenGL ES.
	 *
	 * \param _pvParm Varies by operating system.
	 */
	LSBOOL LSE_CALL COpenGlEs::InitOpenGlEs( void * _pvParm ) {
		ResetInternal( false );
		
		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "COpenGlEs::InitOpenGlEs(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}
		
		// Create the default states.
		// Sampler.
		LSG_SAMPLER_DESC sdOurSamplerDesc;
		if ( FAILED( CreateSamplerState( DefaultSamplerDesc( sdOurSamplerDesc ), &m_pssDefaultSamplerState ) ) ) {
			CStd::DebugPrintA( "COpenGlEs::InitOpenGlEs(): Failed to create default sampler state.\r\n" );
			ResetInternal();
			return false;
		}

		// Rasterizer.
		LSG_RASTERIZER_DESC rdOurRasterizerDesc;
		if ( FAILED( CreateRasterizerState( DefaultRasterizerDesc( rdOurRasterizerDesc ), &m_prsDefaultRasterizerState ) ) ) {
			CStd::DebugPrintA( "COpenGlEs::InitOpenGlEs(): Failed to create default rasterizer state.\r\n" );
			ResetInternal();
			return false;
		}

		// Blend.
		LSG_BLEND_DESC bdOurBlendDesc;
		if ( FAILED( CreateBlendState( DefaultBlendDesc( bdOurBlendDesc ), &m_pbsDefaultBlendState ) ) ) {
			CStd::DebugPrintA( "COpenGlEs::InitOpenGlEs(): Failed to create default blend state.\r\n" );
			ResetInternal();
			return false;
		}

		// DepthStencil.
		LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
		if ( FAILED( CreateDepthStencilState( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ), &m_pdssDefaultDepthStencilState ) ) ) {
			CStd::DebugPrintA( "COpenGlEs::InitOpenGlEs(): Failed to create default depth/stencil state.\r\n" );
			ResetInternal();
			return false;
		}

		return true;
	}
	
	/**
	 * Secondary initialization of OpenGL ES, after a context has been created.
	 */
	void LSE_CALL COpenGlEs::PostInitOpenGlEs() {
		// System metrics.
		GLint iTemp = 0;
		::glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &iTemp );
		m_mMetrics.ui32MaxRenderTargetSize = static_cast<uint32_t>(iTemp);

		iTemp = 1;
#ifdef GL_MAX_COLOR_ATTACHMENTS
		::glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &iTemp );
#endif // #ifdef GL_MAX_COLOR_ATTACHMENTS
		m_mMetrics.ui32MaxRenderTargets = CStd::Min<uint32_t>( iTemp, LSG_MAX_RENDER_TARGET_COUNT );

		iTemp = 128;
		::glGetIntegerv( GL_MAX_TEXTURE_SIZE, &iTemp );
		m_mMetrics.ui32MaxTextureHeight = m_mMetrics.ui32MaxTextureWidth = static_cast<uint32_t>(iTemp);

		m_mMetrics.ui32MaxTextureArray = 1;

		iTemp = 1;
		::glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &iTemp );
		m_mMetrics.ui32MaxAniso = static_cast<uint32_t>(iTemp);

		iTemp = 4;
		//::glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &iTemp );
		::glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iTemp );
		m_mMetrics.ui32MaxTexSlot = CStd::Min<uint32_t>( iTemp, LSG_MAX_TEXTURE_UNITS );
		
		iTemp = 0xFFFF;
#ifdef GL_MAX_ELEMENTS_VERTICES
		::glGetIntegerv( GL_MAX_ELEMENTS_VERTICES, &iTemp );
#endif	// #ifdef GL_MAX_ELEMENTS_VERTICES
		m_mMetrics.ui32MaxVertElements = m_mMetrics.ui32MaxPrimitives = static_cast<uint32_t>(iTemp);

		m_mMetrics.bNonPo2 = false;	// Supported only for some wrap modes.
		m_mMetrics.bDdsSupport = HasExtension( "GL_ARB_texture_compression_s3tc" ) || HasExtension( "GL_EXT_texture_compression_s3tc" );
		m_mMetrics.bPackedDepthStencil = HasExtension( "GL_ARB_packed_depth_stencil" ) || HasExtension( "GL_EXT_packed_depth_stencil" );
		m_mMetrics.bDepthIsReadable = HasExtension( "GL_ARB_depth_texture" ) || HasExtension( "GL_EXT_depth_texture" );

		// Apply states.
		ApplyDefaultStates();
		Clear();
		Present();
	}
	
	/**
	 * Destroy the OpenGL ES render window.
	 */
	void LSE_CALL COpenGlEs::DestroyOpenGlEs() {
		//FreeTmpResources();

		MakeCurrent();
		COpenGlEsCompiledShaderManager::DeleteUnreferencedShaders();
		COpenGlEsShaderProgramManager::DeleteUnreferencedShaderPrograms();

		ResetInternal();

		//COpenGlStandardTexture::CloseOpenGlTextureContext( 0 );

		LSEDELETE m_psmShaderMan;
		m_psmShaderMan = NULL;


		if ( m_mSamplerStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGlEs::DestroyOpenGlEs(): Unreleased sampler states detected.\r\n" );
		}
		if ( m_mRasterizerStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGlEs::DestroyOpenGlEs(): Unreleased rasterizer states detected.\r\n" );
		}
		if ( m_mBlendStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGlEs::DestroyOpenGlEs(): Unreleased blend states detected.\r\n" );
		}
		if ( m_mDepthStencilStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGlEs::DestroyOpenGlEs(): Unreleased depth/stencil states detected.\r\n" );
		}

		ResetInternal();
	}
	
	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	void LSE_FCALL COpenGlEs::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL COpenGlEs::GetShader() {
		return m_psmShaderMan->GetShader();
	}
	
	/**
	 * In debug mode, this checks and prints any OpenGL errors.
	 *
	 * \param _pcHeader Header to be printed with any error messages returned by ::glGetError().
	 * \param _pcFile File in which the error was detected.
	 * \param _ui32Line Line in the file in which the error was detected.
	 * \return Returns true if there were any errors reported.
	 */
	LSBOOL LSE_CALL COpenGlEs::WarnErrorEx( const char * _pcHeader, const char * _pcFile, uint32_t _ui32Line ) {
		if ( !m_bActive ) {
			return false;
		}
		struct LSG_ERROR_DATA {
			/** String form of the enum. */
			const char *		pcEnum;
		};
		static const LSG_ERROR_DATA edBasicErrors[] = {
			//pcEnum
			{ "GL_INVALID_ENUM", },
			{ "GL_INVALID_VALUE", },
			{ "GL_INVALID_OPERATION", },
			{ "GL_STACK_OVERFLOW", },
			{ "GL_STACK_UNDERFLOW", },
			{ "GL_OUT_OF_MEMORY", },
			{ "GL_INVALID_FRAMEBUFFER_OPERATION" },
		};
		GLenum eError;
		LSBOOL bRet = false;
		do {
			{
				CCriticalSection::CLocker lGetError( m_csCrit );
				eError = ::glGetError();
			}
			if ( eError == GL_NO_ERROR ) { return bRet; }
			bRet = true;
			char szBuffer[64+LSF_MAX_PATH];
			if ( eError >= GL_INVALID_ENUM && eError <= GL_INVALID_FRAMEBUFFER_OPERATION ) {
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%s %u: %s (%s).\r\n",
					_pcFile, _ui32Line, _pcHeader, edBasicErrors[eError-GL_INVALID_ENUM] );
			}
			else {
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%s %u: %s (%.4X).\r\n",
					_pcFile, _ui32Line, _pcHeader, eError );
			}
			CStd::DebugPrintA( szBuffer );
		} while ( eError != GL_NO_ERROR );
		return true;
	}

	/**
	 * Sets and applies all default render states.
	 */
	void LSE_CALL COpenGlEs::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		ApplyRenderStates( true );
	}

	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	void LSE_CALL COpenGlEs::ApplyRenderStates( LSBOOL _bForce ) {
		if ( _bForce ) {
			SetDefaultMatrices();
		}
		
		if ( _bForce ) {
			::glEnable( GL_DITHER );
		}

		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Render targets.
		// There must be at least 1 render target.
		m_rsCurRenderState.ui8TotalRenderTargets = CStd::Max<uint8_t>( m_rsCurRenderState.ui8TotalRenderTargets, 1 );
		// Render target 0 must not be NULL.
		LSBOOL bForceViewport = false;
		if ( !m_rsCurRenderState.prtRenderTargets[0] ) {
			m_rsCurRenderState.prtRenderTargets[0] = &m_uiBackFramebuffer;
		}
		for ( uint32_t I = LSE_ELEMENTS( rsCurState.prtRenderTargets ); I--; ) {
			// Unset if higher than rsCurState.ui8TotalRenderTargets.
			if ( I >= rsCurState.ui8TotalRenderTargets ) {
				m_rsCurRenderState.prtRenderTargets[I] = NULL;
			}
			if ( m_rsCurRenderState.prtRenderTargets[I] ) {
				// Temporarily just set the framebuffer, but actually this should
				//	figure out which framebuffer to use based on size and attach
				//	things to that framebuffer as color attachments.
				BindFramebuffer( (*rsCurState.prtRenderTargets[I]) );
				// This causes the viewport to change.
				bForceViewport = true;
				//LSG_UPDATE( prtRenderTargets[I] );
			}
		}

		// Viewports.  Must be done AFTER setting render targets, as render targets change the viewport.
		for ( uint32_t I = LSG_MAX_VIEWPORTS_SCISSORS; I--; ) {
			LSBOOL bViewportSizeChanged = bForceViewport ||
				LSG_CHANGED( vViewports[I].iX ) || LSG_CHANGED( vViewports[I].iY ) ||
				LSG_CHANGED( vViewports[I].sWidth ) || LSG_CHANGED( vViewports[I].sHeight );
			LSBOOL bViewportDepthChanged = bForceViewport ||
				LSG_CHANGED( vViewports[I].fMinZ ) || LSG_CHANGED( vViewports[I].fMaxZ );
			if ( bViewportSizeChanged ) {
				::glViewport( rsCurState.vViewports[I].iX, rsCurState.vViewports[I].iY,
					rsCurState.vViewports[I].sWidth, rsCurState.vViewports[I].sHeight );
				glWarnError( "glViewport" );
				LSG_UPDATE( vViewports[I].iX );
				LSG_UPDATE( vViewports[I].iY );
				LSG_UPDATE( vViewports[I].sWidth );
				LSG_UPDATE( vViewports[I].sHeight );
			}
			if ( bViewportDepthChanged ) {
				::glDepthRangef( rsCurState.vViewports[I].fMinZ, rsCurState.vViewports[I].fMaxZ );
				glWarnError( "glDepthRangef" );
				LSG_UPDATE( vViewports[I].fMinZ );
				LSG_UPDATE( vViewports[I].fMaxZ );
			}
		}

		// Scissor rectangles.
		for ( uint32_t I = LSG_MAX_VIEWPORTS_SCISSORS; I--; ) {
			if ( LSG_CHANGED( rScissors[I].left ) || LSG_CHANGED( rScissors[I].top ) ||
				LSG_CHANGED( rScissors[I].right ) || LSG_CHANGED( rScissors[I].bottom ) ) {
				GLsizei sWidth = rsCurState.rScissors[I].right - rsCurState.rScissors[I].left;
				GLsizei sHeight = rsCurState.rScissors[I].bottom - rsCurState.rScissors[I].top;
				::glScissor( rsCurState.rScissors[I].left, rsCurState.rScissors[I].top,
					sWidth, sHeight );
				glWarnError( "glScissor" );
				LSG_UPDATE( rScissors[I] );
			}
		}

		// Textures.
		uint32_t ui32Max = _bForce ? CGfxBase::m_mMetrics.ui32MaxTexSlot : CStd::Max( rsCurState.ui32MaxTextureSet, rsLastState.ui32MaxTextureSet );
		for ( uint32_t I = ui32Max; I--; ) {
			GLuint uiCur = rsCurState.psrShaderResources[I] ? (*rsCurState.psrShaderResources[I]) : 0;
			GLuint uiLast = rsLastState.psrShaderResources[I] ? (*rsLastState.psrShaderResources[I]) : 0;
			if ( uiCur != uiLast || _bForce ) {
				BindTexture2d( uiCur, I );
				LSG_UPDATE( psrShaderResources[I] );
			}
		}



		// Basic states.
		SetSamplerStates( _bForce );		// Must be done after textures are set.
		SetRasterizerState( _bForce );
		SetBlendState( _bForce );
		SetDepthStencilState( _bForce );

#undef LSG_UPDATE
#undef LSG_CHANGED
	}

#define LSG_DESCFUNCDEF_CLASS COpenGlEs
#include "../Gfx/LSGDescFuncDef.h"
	
	/**
	 * Determine if an extension is present.
	 *
	 * \param _pcExt The name of the extension for which to check.
	 * \return Returns true if the given extension is supported.
	 */
	LSBOOL LSE_CALL COpenGlEs::HasExtension( const char * _pcExt ) {
		static const char * pcExtensions = reinterpret_cast<const char *>(::glGetString( GL_EXTENSIONS ));
		if ( !pcExtensions ) { return false; }

		assert( _pcExt );

		uint32_t ui32Len = CStd::StrLen( _pcExt );
		while ( (*pcExtensions) ) {
			uint32_t ui32ThisLen = CStd::StrCSpn( pcExtensions, " " );
			if ( ui32ThisLen == ui32Len ) {
				if ( CStd::StrNCmp( pcExtensions, _pcExt, ui32Len ) == 0 ) {
					return true;
				}
			}
			pcExtensions += ui32ThisLen + 1;
		}
		return false;
	}

	/**
	 * Release all resources and reset our internal state.
	 *
	 * \param _bResetContext If true, the context is also deleted.
	 */
	void LSE_CALL COpenGlEs::ResetInternal( LSBOOL _bResetContext ) {
		ReleaseState( m_pssDefaultSamplerState );
		ReleaseState( m_prsDefaultRasterizerState );
		ReleaseState( m_pbsDefaultBlendState );
		ReleaseState( m_pdssDefaultDepthStencilState );

		if ( _bResetContext ) {
#ifdef LSE_WINDOWS
			if ( m_hgrcContext ) {
				::wglMakeCurrent( m_hDc, NULL );
				::wglDeleteContext( m_hgrcContext );
				m_hgrcContext = NULL;
				m_hDc = NULL;
			}
#endif	// #ifdef LSE_WINDOWS
		}
	}
	
#define LSG_CREATESTATESDEF_CLASS COpenGlEs
#include "../Gfx/LSGCreateStatesDef.h"

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
