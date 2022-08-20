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
 * Description: Interface with the OpenGL * API.
 */

#include "LSGOpenGl.h"

#ifdef LSG_OGL

#include "../Framebuffer/LSGFramebuffer.h"
#include "../OpenGLShaders/LSGOpenGlCompiledShaderManager.h"
#include "../OpenGLShaders/LSGOpenGlShaderManager.h"
#include "../OpenGLShaders/LSGOpenGlShaderProgramManager.h"

namespace lsg {

	// == Types.
	// == Various contructors.
	LSE_CALLCTOR COpenGl::CTempTexture2dBinder::CTempTexture2dBinder( GLuint _uiId ) {
		m_csBindTextureCrit.Lock();
		m_uiLastTexture = COpenGl::TextureBinding2d( 0 );
		COpenGl::BindTexture2d( _uiId, 0 );
	}
	LSE_CALLCTOR COpenGl::CTempTexture2dBinder::~CTempTexture2dBinder() {
		COpenGl::BindTexture2d( m_uiLastTexture, 0 );
		m_csBindTextureCrit.UnLock();
	}

	// == Various contructors.
	LSE_CALLCTOR COpenGl::CTempTexture2dArrayBinder::CTempTexture2dArrayBinder( GLuint _uiId ) {
		m_csBindTextureCrit.Lock();
		m_uiLastTexture = COpenGl::TextureBinding2dArray( 0 );
		COpenGl::BindTexture2dArray( _uiId, 0 );
	}
	LSE_CALLCTOR COpenGl::CTempTexture2dArrayBinder::~CTempTexture2dArrayBinder() {
		COpenGl::BindTexture2dArray( m_uiLastTexture, 0 );
		m_csBindTextureCrit.UnLock();
	}

	// == Various contructors.
	LSE_CALLCTOR COpenGl::CTempFramebufferBinder::CTempFramebufferBinder( GLuint _uiId ) {
		m_csFramebufferCrit.Lock();
		m_uiLastFramebuffer = m_rsCurRenderState.uiFramebuffer;
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGl::BindFramebuffer( _uiId );
		glWarnErrorAlways( "Invalid framebuffer ID" );
	}
	LSE_CALLCTOR COpenGl::CTempFramebufferBinder::~CTempFramebufferBinder() {
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.
		COpenGl::BindFramebuffer( m_uiLastFramebuffer );
		glWarnErrorAlways( "Invalid framebuffer ID" );
		m_csFramebufferCrit.UnLock();
	}

	// == Members.
	/** An OpenGL API function. */
	PFNGLDRAWRANGEELEMENTSPROC COpenGl::glDrawRangeElements = nullptr;

	/** An OpenGL API function. */
	PFNGLDRAWELEMENTSBASEVERTEXPROC COpenGl::glDrawElementsBaseVertex = nullptr;

	/** An OpenGL API function. */
	PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC COpenGl::glDrawRangeElementsBaseVertex = nullptr;

	/** An OpenGL API function. */
	PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC COpenGl::glDrawElementsInstancedBaseVertex = nullptr;

	/** An OpenGL API function. */
	PFNGLCOMPRESSEDTEXIMAGE2DPROC COpenGl::glCompressedTexImage2D = nullptr;

	/** An OpenGL API function. */
	PFNGLGENERATEMIPMAPPROC COpenGl::glGenerateMipmap = nullptr;

	/** An OpenGL API function. */
	PFNGLDELETESHADERPROC COpenGl::glDeleteShader = nullptr;

	/** An OpenGL API function. */
	PFNGLCREATEPROGRAMPROC COpenGl::glCreateProgram = nullptr;

	/** An OpenGL API function. */
	PFNGLATTACHSHADERPROC COpenGl::glAttachShader = nullptr;

	/** An OpenGL API function. */
	PFNGLLINKPROGRAMPROC COpenGl::glLinkProgram = nullptr;

	/** An OpenGL API function. */
	PFNGLGETPROGRAMIVPROC COpenGl::glGetProgramiv = nullptr;

	/** An OpenGL API function. */
	PFNGLGETPROGRAMINFOLOGPROC COpenGl::glGetProgramInfoLog = nullptr;

	/** An OpenGL API function. */
	PFNGLGETATTRIBLOCATIONPROC COpenGl::glGetAttribLocation = nullptr;

	/** An OpenGL API function. */
	PFNGLGETUNIFORMLOCATIONPROC COpenGl::glGetUniformLocation = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM1FPROC COpenGl::glUniform1f = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM1FVPROC COpenGl::glUniform1fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM1IPROC COpenGl::glUniform1i = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM1IVPROC COpenGl::glUniform1iv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM2FPROC COpenGl::glUniform2f = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM2FVPROC COpenGl::glUniform2fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM2IPROC COpenGl::glUniform2i = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM2IVPROC COpenGl::glUniform2iv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM3FPROC COpenGl::glUniform3f = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM3FVPROC COpenGl::glUniform3fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM3IPROC COpenGl::glUniform3i = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM3IVPROC COpenGl::glUniform3iv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM4FPROC COpenGl::glUniform4f = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM4FVPROC COpenGl::glUniform4fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM4IPROC COpenGl::glUniform4i = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORM4IVPROC COpenGl::glUniform4iv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORMMATRIX2FVPROC COpenGl::glUniformMatrix2fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORMMATRIX3FVPROC COpenGl::glUniformMatrix3fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUNIFORMMATRIX4FVPROC COpenGl::glUniformMatrix4fv = nullptr;

	/** An OpenGL API function. */
	PFNGLUSEPROGRAMPROC COpenGl::glUseProgram = nullptr;

	/** An OpenGL API function. */
	PFNGLVALIDATEPROGRAMPROC COpenGl::glValidateProgram = nullptr;

	/** An OpenGL API function. */
	PFNGLCREATESHADERPROC COpenGl::glCreateShader = nullptr;

	/** An OpenGL API function. */
	PFNGLSHADERSOURCEPROC COpenGl::glShaderSource = nullptr;

	/** An OpenGL API function. */
	PFNGLCOMPILESHADERPROC COpenGl::glCompileShader = nullptr;

	/** An OpenGL API function. */
	PFNGLGETSHADERIVPROC COpenGl::glGetShaderiv = nullptr;

	/** An OpenGL API function. */
	PFNGLGETSHADERINFOLOGPROC COpenGl::glGetShaderInfoLog = nullptr;

	/** An OpenGL API function. */
	PFNGLGENBUFFERSPROC COpenGl::glGenBuffers = nullptr;

	/** An OpenGL API function. */
	PFNGLBINDBUFFERPROC COpenGl::glBindBuffer = nullptr;

	/** An OpenGL API function. */
	PFNGLBUFFERDATAPROC COpenGl::glBufferData = nullptr;

	/** An OpenGL API function. */
	PFNGLDELETEBUFFERSPROC COpenGl::glDeleteBuffers = nullptr;

	/** An OpenGL API function. */
	PFNGLMAPBUFFERPROC COpenGl::glMapBuffer = nullptr;

	/** An OpenGL API function. */
	PFNGLUNMAPBUFFERPROC COpenGl::glUnmapBuffer = nullptr;

	/** An OpenGL API function. */
	PFNGLENABLEVERTEXATTRIBARRAYPROC COpenGl::glEnableVertexAttribArray = nullptr;

	/** An OpenGL API function. */
	PFNGLVERTEXATTRIBPOINTERPROC COpenGl::glVertexAttribPointer = nullptr;

	/** An OpenGL API function. */
	PFNGLDISABLEVERTEXATTRIBARRAYPROC COpenGl::glDisableVertexAttribArray = nullptr;

	/** An OpenGL API function. */
	PFNGLENABLEIPROC COpenGl::glEnablei = nullptr;

	/** An OpenGL API function. */
	PFNGLDISABLEIPROC COpenGl::glDisablei = nullptr;

	/** An OpenGL API function. */
	PFNGLBLENDFUNCSEPARATEPROC COpenGl::glBlendFuncSeparate = nullptr;

	/** An OpenGL API function. */
	PFNGLBLENDEQUATIONSEPARATEPROC COpenGl::glBlendEquationSeparate = nullptr;

	/** An OpenGL API function. */
	PFNGLSTENCILOPSEPARATEPROC COpenGl::glStencilOpSeparate = nullptr;

	/** An OpenGL API function. */
	PFNGLSTENCILFUNCSEPARATEPROC COpenGl::glStencilFuncSeparate = nullptr;

	/** An OpenGL API function. */
	PFNGLSTENCILMASKSEPARATEPROC COpenGl::glStencilMaskSeparate = nullptr;

	/** An OpenGL API function. */
	PFNGLTEXSTORAGE3DPROC COpenGl::glTexStorage3D = nullptr;

	/** An OpenGL API function. */
	PFNGLTEXSUBIMAGE3DPROC COpenGl::glTexSubImage3D = nullptr;

	/** An OpenGL API function. */
	PFNGLGENFRAMEBUFFERSPROC COpenGl::glGenFramebuffers = nullptr;

	/** An OpenGL API function. */
	PFNGLDELETEFRAMEBUFFERSPROC COpenGl::glDeleteFramebuffers = nullptr;

	/** An OpenGL API function. */
	PFNGLBINDFRAMEBUFFERPROC COpenGl::glBindFramebuffer = nullptr;

	/** An OpenGL API function. */
	PFNGLFRAMEBUFFERRENDERBUFFERPROC COpenGl::glFramebufferRenderbuffer = nullptr;

	/** An OpenGL API function. */
	PFNGLFRAMEBUFFERTEXTUREPROC COpenGl::glFramebufferTexture = nullptr;

	/** An OpenGL API function. */
	PFNGLFRAMEBUFFERTEXTURE1DPROC COpenGl::glFramebufferTexture1D = nullptr;

	/** An OpenGL API function. */
	PFNGLFRAMEBUFFERTEXTURE2DPROC COpenGl::glFramebufferTexture2D = nullptr;

	/** An OpenGL API function. */
	PFNGLFRAMEBUFFERTEXTURE3DPROC COpenGl::glFramebufferTexture3D = nullptr;

	/** An OpenGL API function. */
	PFNGLCHECKFRAMEBUFFERSTATUSPROC COpenGl::glCheckFramebufferStatus = nullptr;

	/** An OpenGL API function. */
	PFNGLGETINTERNALFORMATIVPROC COpenGl::glGetInternalformativ = nullptr;

	/** The ::glGetError() critical section. */
	CCriticalSection COpenGl::m_csCrit;

	/** The critical section for binding textures. */
	CCriticalSection COpenGl::m_csBindTextureCrit;

	/** The critical section for binding framebuffers. */
	CCriticalSection COpenGl::m_csFramebufferCrit;

#ifdef LSE_WINDOWS
	/** The window context onto which we render. */
	HDC COpenGl::m_hDc = NULL;

	/** Rendering context for OpenGL. */
	HGLRC COpenGl::m_hgrcContext = NULL;
#endif	// #ifdef LSE_WINDOWS

	/** The shader manager. */
	CShaderManager * COpenGl::m_psmShaderMan = NULL;

	/** Function pointer to the function used to get function addresses. */
	COpenGl::wglGetProcAddressLSG COpenGl::GetProcAddressLSG = NULL;

#define LSG_CREATESTATESDEF_CLASS COpenGl
#include "../Gfx/LSGCreateStatesMembersDef.h"

	/** Default sampler state. */
	LSG_SAMPLER_STATE * COpenGl::m_pssDefaultSamplerState = NULL;

	/** Default rasterizer state. */
	LSG_RASTERIZER_STATE * COpenGl::m_prsDefaultRasterizerState = NULL;

	/** Default blend state. */
	LSG_BLEND_STATE * COpenGl::m_pbsDefaultBlendState = NULL;

	/** Default depth/stencil state. */
	LSG_DEPTH_STENCIL_STATE * COpenGl::m_pdssDefaultDepthStencilState = NULL;

	/** The last render state. */
	COpenGl::LSG_OPENGL_RENDER_STATE COpenGl::m_rsLastRenderState;

	/** The current render state. */
	COpenGl::LSG_OPENGL_RENDER_STATE COpenGl::m_rsCurRenderState;

	/** Last samplers actually set on the rendering API. */
	LSG_SAMPLER_STATE COpenGl::m_ssLastSamplers[LSG_MAX_SHADER_SAMPLERS] = { { static_cast<LSG_FILTER>(0) } };

	/** Last rasterizer states actually set on the rendering API. */
	LSG_RASTERIZER_STATE COpenGl::m_rsLastRaster = { static_cast<LSG_FILL_MODE>(0) };

	/** Last blend state actually set on the rendering API. */
	LSG_BLEND_STATE COpenGl::m_bsLastBlend = { 0 };

	/** Last depth/stencil state actually set on the rendering API. */
	LSG_DEPTH_STENCIL_STATE COpenGl::m_dssLastDepthStencil = { 0 };

	/** An OpenGL API function. */
	PFNGLACTIVETEXTUREPROC COpenGl::glActiveTexture = NULL;

	// == Functions.
	/**
	 * Initialize OpenGL.
	 *
	 * \param _lpdiInit Data required to initialize the device.
	 * \return Returns false if there are any problem during initialization.
	 */
	LSBOOL LSE_CALL COpenGl::InitOpenGl( void * _pvInit ) {
		//ResetInternal( false );

#ifndef LSA_TOOLS
#ifdef LSE_WINDOWS
		m_hDc = static_cast<HDC>(_pvInit);
		if ( !m_hgrcContext ) {
			m_hgrcContext = ::wglCreateContext( m_hDc );
		}
#endif	// #ifdef LSE_WINDOWS
#endif	// #ifndef LSA_TOOLS

		if ( !m_psmShaderMan ) {
			m_psmShaderMan = LSENEW CShaderManager();
			if ( !m_psmShaderMan ) {
				CStd::DebugPrintA( "COpenGl::InitOpenGl(): Failed to create shader manager.\r\n" );
				ResetInternal();
				return false;
			}
		}
		

		// Create the default states.
		// Sampler.
		if ( !m_pssDefaultSamplerState ) {
			LSG_SAMPLER_DESC sdOurSamplerDesc;
			if ( FAILED( CreateSamplerState( DefaultSamplerDesc( sdOurSamplerDesc ), &m_pssDefaultSamplerState ) ) ) {
				CStd::DebugPrintA( "COpenGl::InitOpenGl(): Failed to create default sampler state.\r\n" );
				ResetInternal();
				return false;
			}
		}

		// Rasterizer.
		if ( !m_prsDefaultRasterizerState ) {
			LSG_RASTERIZER_DESC rdOurRasterizerDesc;
			if ( FAILED( CreateRasterizerState( DefaultRasterizerDesc( rdOurRasterizerDesc ), &m_prsDefaultRasterizerState ) ) ) {
				CStd::DebugPrintA( "COpenGl::InitOpenGl(): Failed to create default rasterizer state.\r\n" );
				ResetInternal();
				return false;
			}
		}

		// Blend.
		if ( !m_pbsDefaultBlendState ) {
			LSG_BLEND_DESC bdOurBlendDesc;
			if ( FAILED( CreateBlendState( DefaultBlendDesc( bdOurBlendDesc ), &m_pbsDefaultBlendState ) ) ) {
				CStd::DebugPrintA( "COpenGl::InitOpenGl(): Failed to create default blend state.\r\n" );
				ResetInternal();
				return false;
			}
		}

		// DepthStencil.
		if ( !m_pdssDefaultDepthStencilState ) {
			LSG_DEPTH_STENCIL_DESC dsdOurDepthStencilDesc;
			if ( FAILED( CreateDepthStencilState( DefaultDepthStencilDesc( dsdOurDepthStencilDesc ), &m_pdssDefaultDepthStencilState ) ) ) {
				CStd::DebugPrintA( "COpenGl::InitOpenGl(): Failed to create default depth/stencil state.\r\n" );
				ResetInternal();
				return false;
			}
		}

		return true;
	}

	/**
	 * Secondary initialization of OpenGL, after a context has been created.
	 */
	void LSE_CALL COpenGl::PostInitOpenGl() {
#ifdef LSE_WINDOWS
#ifndef LSA_TOOLS
		assert( m_hDc );
		assert( m_hgrcContext );
#endif	// #ifndef LSA_TOOLS
		GetProcAddressLSG = reinterpret_cast<wglGetProcAddressLSG>(::wglGetProcAddress);

		// Disable V-sync.
		//::wglGetExtensionsStringARB();
		//LSBOOL bSwapInterval = HasExtension( "WGL_EXT_swap_control" );

		typedef BOOL (APIENTRYP SwapIntervalLSG)( int interval );
		SwapIntervalLSG pfSwapIntervalLSP = reinterpret_cast<SwapIntervalLSG>(GetProcAddressLSG( "wglSwapIntervalEXT" ));
		if ( pfSwapIntervalLSP ) {
			pfSwapIntervalLSP( 0 );
		}
		//::wglSwapIntervalEXT();
#endif	// #ifdef LSE_WINDOWS

#ifdef LSE_WINDOWS
#define LSG_PROADDR( TYPE, PROC )	PROC = static_cast<TYPE>(GetProcAddressLSG( #PROC ))
#elif defined( LSE_OSX )
#define LSG_PROADDR( TYPE, PROC )	COpenGl::PROC = ::PROC
#endif	// #ifdef LSE_WINDOWS

#ifdef _DEBUG
		GLint iMajor, iMinor;
		::glGetIntegerv( GL_MAJOR_VERSION, &iMajor );
		::glGetIntegerv( GL_MINOR_VERSION, &iMinor );
		char szBuffer[64];
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "OpenGL Version: %d.%d.\r\n", iMajor, iMinor );
		CStd::DebugPrintA( szBuffer );
#endif	// #ifdef _DEBUG


		// Assign function pointers.
		// Drawing.
		LSG_PROADDR( PFNGLDRAWRANGEELEMENTSPROC, glDrawRangeElements );
		LSG_PROADDR( PFNGLDRAWELEMENTSBASEVERTEXPROC, glDrawElementsBaseVertex );
		LSG_PROADDR( PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC, glDrawRangeElementsBaseVertex );
		LSG_PROADDR( PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC, glDrawElementsInstancedBaseVertex );

		// Textures.
		LSG_PROADDR( PFNGLACTIVETEXTUREPROC, glActiveTexture );
		LSG_PROADDR( PFNGLCOMPRESSEDTEXIMAGE2DPROC, glCompressedTexImage2D );
		LSG_PROADDR( PFNGLGENERATEMIPMAPPROC, glGenerateMipmap );
		LSG_PROADDR( PFNGLTEXSTORAGE3DPROC, glTexStorage3D );
		LSG_PROADDR( PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D );

		LSG_PROADDR( PFNGLGETINTERNALFORMATIVPROC, glGetInternalformativ );

		// Framebuffers.
		LSG_PROADDR( PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers );
		LSG_PROADDR( PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers );
		LSG_PROADDR( PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer );
		LSG_PROADDR( PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer );
		LSG_PROADDR( PFNGLFRAMEBUFFERTEXTUREPROC, glFramebufferTexture );
		LSG_PROADDR( PFNGLFRAMEBUFFERTEXTURE1DPROC, glFramebufferTexture1D );
		LSG_PROADDR( PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D );
		LSG_PROADDR( PFNGLFRAMEBUFFERTEXTURE3DPROC, glFramebufferTexture3D );
		LSG_PROADDR( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, glCheckFramebufferStatus );

		// Shaders.
		LSG_PROADDR( PFNGLDELETESHADERPROC, glDeleteShader );
		LSG_PROADDR( PFNGLCREATEPROGRAMPROC, glCreateProgram );
		LSG_PROADDR( PFNGLATTACHSHADERPROC, glAttachShader );
		LSG_PROADDR( PFNGLLINKPROGRAMPROC, glLinkProgram );
		LSG_PROADDR( PFNGLGETPROGRAMIVPROC, glGetProgramiv );
		LSG_PROADDR( PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog );
		LSG_PROADDR( PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation );
		LSG_PROADDR( PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation );
		LSG_PROADDR( PFNGLUNIFORM1FPROC, glUniform1f );
		LSG_PROADDR( PFNGLUNIFORM1FVPROC, glUniform1fv );
		LSG_PROADDR( PFNGLUNIFORM1IPROC, glUniform1i );
		LSG_PROADDR( PFNGLUNIFORM1IVPROC, glUniform1iv );
		LSG_PROADDR( PFNGLUNIFORM2FPROC, glUniform2f );
		LSG_PROADDR( PFNGLUNIFORM2FVPROC, glUniform2fv );
		LSG_PROADDR( PFNGLUNIFORM2IPROC, glUniform2i );
		LSG_PROADDR( PFNGLUNIFORM2IVPROC, glUniform2iv );
		LSG_PROADDR( PFNGLUNIFORM3FPROC, glUniform3f );
		LSG_PROADDR( PFNGLUNIFORM3FVPROC, glUniform3fv );
		LSG_PROADDR( PFNGLUNIFORM3IPROC, glUniform3i );
		LSG_PROADDR( PFNGLUNIFORM3IVPROC, glUniform3iv );
		LSG_PROADDR( PFNGLUNIFORM4FPROC, glUniform4f );
		LSG_PROADDR( PFNGLUNIFORM4FVPROC, glUniform4fv );
		LSG_PROADDR( PFNGLUNIFORM4IPROC, glUniform4i );
		LSG_PROADDR( PFNGLUNIFORM4IVPROC, glUniform4iv );
		LSG_PROADDR( PFNGLUNIFORMMATRIX2FVPROC, glUniformMatrix2fv );
		LSG_PROADDR( PFNGLUNIFORMMATRIX3FVPROC, glUniformMatrix3fv );
		LSG_PROADDR( PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv );
		LSG_PROADDR( PFNGLUSEPROGRAMPROC, glUseProgram );
		LSG_PROADDR( PFNGLVALIDATEPROGRAMPROC, glValidateProgram );
		LSG_PROADDR( PFNGLCREATESHADERPROC, glCreateShader );
		LSG_PROADDR( PFNGLSHADERSOURCEPROC, glShaderSource );
		LSG_PROADDR( PFNGLCOMPILESHADERPROC, glCompileShader );
		LSG_PROADDR( PFNGLGETSHADERIVPROC, glGetShaderiv );
		LSG_PROADDR( PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog );

		// Buffers.
		LSG_PROADDR( PFNGLGENBUFFERSPROC, glGenBuffers );
		LSG_PROADDR( PFNGLBINDBUFFERPROC, glBindBuffer );
		LSG_PROADDR( PFNGLBUFFERDATAPROC, glBufferData );
		LSG_PROADDR( PFNGLDELETEBUFFERSPROC, glDeleteBuffers );
		LSG_PROADDR( PFNGLMAPBUFFERPROC, glMapBuffer );
		LSG_PROADDR( PFNGLUNMAPBUFFERPROC, glUnmapBuffer );
		LSG_PROADDR( PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray );
		LSG_PROADDR( PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer );
		LSG_PROADDR( PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray );

		// Blending.
		LSG_PROADDR( PFNGLENABLEIPROC, glEnablei );
		LSG_PROADDR( PFNGLDISABLEIPROC, glDisablei );
		LSG_PROADDR( PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate );
		LSG_PROADDR( PFNGLBLENDEQUATIONSEPARATEPROC, glBlendEquationSeparate );

		// Stencil buffer.
		LSG_PROADDR( PFNGLSTENCILOPSEPARATEPROC, glStencilOpSeparate );
		LSG_PROADDR( PFNGLSTENCILFUNCSEPARATEPROC, glStencilFuncSeparate );
		LSG_PROADDR( PFNGLSTENCILMASKSEPARATEPROC, glStencilMaskSeparate );


		// System metrics.
		GLint iTemp = 0;
		::glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &iTemp );
		m_mMetrics.ui32MaxRenderTargetSize = static_cast<uint32_t>(iTemp);

		iTemp = 1;
		::glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &iTemp );
		m_mMetrics.ui32MaxRenderTargets = CStd::Min<uint32_t>( iTemp, LSG_MAX_RENDER_TARGET_COUNT );

		iTemp = 128;
		::glGetIntegerv( GL_MAX_TEXTURE_SIZE, &iTemp );
		m_mMetrics.ui32MaxTextureHeight = m_mMetrics.ui32MaxTextureWidth = static_cast<uint32_t>(iTemp);

		iTemp = 256;
		::glGetIntegerv( GL_MAX_ARRAY_TEXTURE_LAYERS, &iTemp );
		m_mMetrics.ui32MaxTextureArray = iTemp;

		iTemp = 1;
		::glGetIntegerv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &iTemp );
		m_mMetrics.ui32MaxAniso = static_cast<uint32_t>(iTemp);

		iTemp = 4;
		//::glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &iTemp );
		::glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &iTemp );
		m_mMetrics.ui32MaxTexSlot = CStd::Min<uint32_t>( iTemp, LSG_MAX_TEXTURE_UNITS );
		
		iTemp = 0;
		::glGetIntegerv( GL_MAX_ELEMENTS_VERTICES, &iTemp );
		m_mMetrics.ui32MaxVertElements = m_mMetrics.ui32MaxPrimitives = static_cast<uint32_t>(iTemp);

		m_mMetrics.bNonPo2 = true;	// Required after OpenGL 2.0 (and GL_ARB_texture_non_power_of_two/GL_EXT_texture_non_power_of_two need not be defined).
		m_mMetrics.bDdsSupport = HasExtension( "GL_ARB_texture_compression_s3tc" ) || HasExtension( "GL_EXT_texture_compression_s3tc" );
		m_mMetrics.bPackedDepthStencil = HasExtension( "GL_ARB_packed_depth_stencil" ) || HasExtension( "GL_EXT_packed_depth_stencil" );
		m_mMetrics.bDepthIsReadable = HasExtension( "GL_ARB_depth_texture" ) || HasExtension( "GL_EXT_depth_texture" );

		// Apply states.
		ApplyDefaultStates();
		Clear();
		Present();
	}

	/**
	 * Destroy the OpenGL render window.
	 */
	void LSE_CALL COpenGl::DestroyOpenGl() {
		//FreeTmpResources();

		MakeCurrent();
		COpenGlCompiledShaderManager::DeleteUnreferencedShaders();
		COpenGlShaderProgramManager::DeleteUnreferencedShaderPrograms();

		ResetInternal();

		//COpenGlStandardTexture::CloseOpenGlTextureContext( 0 );

		LSEDELETE m_psmShaderMan;
		m_psmShaderMan = NULL;


		if ( m_mSamplerStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGl::DestroyOpenGl(): Unreleased sampler states detected.\r\n" );
		}
		if ( m_mRasterizerStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGl::DestroyOpenGl(): Unreleased rasterizer states detected.\r\n" );
		}
		if ( m_mBlendStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGl::DestroyOpenGl(): Unreleased blend states detected.\r\n" );
		}
		if ( m_mDepthStencilStateMap.Length() ) {
			CStd::DebugPrintA( "COpenGl::DestroyOpenGl(): Unreleased depth/stencil states detected.\r\n" );
		}

		ResetInternal();
	}

	/**
	 * Sets the current shader.  A shader must be set in order to render.
	 *
	 * \param _psShader The shader to use for the current render.  NULL is a valid value, but rendering
	 *	without a shader is not permitted.
	 */
	void LSE_FCALL COpenGl::SetShader( CShader * _psShader ) {
		m_psmShaderMan->SetShader( _psShader );
	}

	/**
	 * Gets the current shader.  If no shader is set, NULL is returned.
	 *
	 * \return Returns the current shader or NULL.
	 */
	CShader * LSE_FCALL COpenGl::GetShader() {
		return m_psmShaderMan->GetShader();
	}

	/**
	 * Make this OpenGL context current.
	 */
	void LSE_CALL COpenGl::MakeCurrent() {
#ifdef LSE_WINDOWS
		if ( m_hgrcContext ) {
			::wglMakeCurrent( m_hDc, m_hgrcContext );
		}
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * In debug mode, this checks and prints any OpenGL errors.
	 *
	 * \param _pcHeader Header to be printed with any error messages returned by ::glGetError().
	 * \param _pcFile File in which the error was detected.
	 * \param _ui32Line Line in the file in which the error was detected.
	 * \return Returns true if there were any errors reported.
	 */
	LSBOOL LSE_CALL COpenGl::WarnErrorEx( const char * _pcHeader, const char * _pcFile, uint32_t _ui32Line ) {
		struct LSG_ERROR_DATA {
			/**
			 * String form of the enum.
			 */
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
#ifdef LSE_WINDOWS
			if ( eError == GL_INVALID_OPERATION ) {
				if ( !::wglGetCurrentContext() ) {
					CStd::DebugPrintA( "No OpenGL context.\r\n" );
					return true;
				}
			}
#endif	// #ifdef LSE_WINDOWS
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
	void LSE_CALL COpenGl::ApplyDefaultStates() {
		SetAllDefaultRenderStates();

		ApplyRenderStates( true );
	}

	/**
	 * Applies all current render states.
	 *
	 * \param _bForce If true, all states are applied, even if they are redundant.
	 */
	void LSE_CALL COpenGl::ApplyRenderStates( LSBOOL _bForce ) {
		glWarnError( "Uncaught" );
		if ( _bForce ) {
			SetDefaultMatrices();
		}

		const LSG_RENDER_STATE & rsCurState = m_rsCurRenderState;
		LSG_RENDER_STATE & rsLastState = m_rsLastRenderState;

#define LSG_CHANGED( MEMBER )	((rsCurState.MEMBER != rsLastState.MEMBER) || _bForce)
#define LSG_UPDATE( MEMBER )	rsLastState.MEMBER = rsCurState.MEMBER

		// Render targets.
		// There must be at least 1 render target.
		m_rsCurRenderState.ui8TotalRenderTargets = CStd::Max<uint8_t>( m_rsCurRenderState.ui8TotalRenderTargets, 1 );
		// Render target 0 must not be NULL.
		LSBOOL bForceViewport = ApplyFramebuffer( _bForce );

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
				::glDepthRange( rsCurState.vViewports[I].fMinZ, rsCurState.vViewports[I].fMaxZ );
				glWarnError( "glDepthRange" );
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

	/**
	 * Applies the currently set framebuffer. The render targets in the framebuffer are not set until this
	 *	is called.
	 *
	 * \param _bForce If true, all render targets are applied, even if they are redundant.
	 * \return Returns true if any buffers were activated.
	 */
	LSBOOL LSE_CALL COpenGl::ApplyFramebuffer( LSBOOL /*_bForce*/ ) {
		GLuint uiPrevFramebuffer = m_rsCurRenderState.uiFramebuffer;
		if ( m_rsCurRenderState.pfFramebuffer ) {
			m_rsCurRenderState.pfFramebuffer->Activate();
		}
		else {
			ActivateBackBuffers();
		}
		return uiPrevFramebuffer != m_rsCurRenderState.uiFramebuffer;
	}

#define LSG_DESCFUNCDEF_CLASS COpenGl
#include "../Gfx/LSGDescFuncDef.h"

	/**
	 * Determine if an extension is present.
	 *
	 * \param _pcExt The name of the extension for which to check.
	 * \return Returns true if the given extension is supported.
	 */
	LSBOOL LSE_CALL COpenGl::HasExtension( const char * _pcExt ) {
		const char * pcExtensions = reinterpret_cast<const char *>(::glGetString( GL_EXTENSIONS ));
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
	void LSE_CALL COpenGl::ResetInternal( LSBOOL _bResetContext ) {
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

#define LSG_CREATESTATESDEF_CLASS COpenGl
#include "../Gfx/LSGCreateStatesDef.h"

}	// namespace lsg

#endif	// #ifdef LSG_OGL
