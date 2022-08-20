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
 * Description: DESCRIPTION.
 */

#include "LSTTextureTest.h"
#include "Image/LSIImage.h"

// == Various constructors.
LSE_CALLCTOR CTextureTest::CTextureTest() :
	m_pbsBlend( nullptr ) {
}

// == Functions.
/**
 * Called when the state is just created.  Use this to initialize resources.
 *
 * \param _pgGame Can be accessed for whatever game-specific information needs to be
 *	obtained.
 * \param _ui32State The current state identifier (the same class could
 *	then be used for multiple states if they are similar enough).
 * \param _ui32PrevState The identifier of the previous state.
 * \param _uptrUser User-defined data passed to the state.  How this is used is entirely
 *	defined by the state.  Examples of use:
 * #1: Which map to load in a game state.
 * #2: Which tutorial page to show in a tutorial state.
 * #3: Used as a pointer to data containing a lot of state-specific information such as
 *	map file to load, previous state of the player, etc.
 */
void LSE_CALL CTextureTest::Init( CGame * _pgGame, int32_t /*_ui32State*/, int32_t /*_ui32PrevState*/, uintptr_t /*_uptrUser*/ ) {
	const static char szVertex[] = {
		"// == Uniforms.\r\n"
		"uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"

		"void Main( in vec3 _vInPos : POSITION,\r\n"
		"	in vec2 _vInTex : TEXCOORD,\r\n"
		"	out vec4 _vOutPos : POSITION,\r\n"
		"	out vec2 _vOutTex : TEXCOORD ) {\r\n"
		"	_vOutPos = vec4( _vInPos, 1.0f );//mul( g_mModelViewProjMatrix, vec4( _vInPos, 1.0f ) );\r\n"
		"	_vOutTex = _vInTex;\r\n"
		"}\r\n"
	};
	CString sError;
	if ( LSG_SE_SUCCESS != m_sShader.SetShaderFromMemory( reinterpret_cast<const uint8_t *>(szVertex), sizeof( szVertex ), "",
		LSG_ST_VERTEX, sError ) ) {
		CStd::DebugPrintA( "Error with vertex shader: " );
		CStd::DebugPrintA( sError.CStr() );
	}


	const static char szPixel[] = {
		"uniform Sampler g_sSampler : TEXUNIT0;\r\n"
		"uniform Texture2D g_tTexure : TEXUNIT0;\r\n"
		"void Main( in vec4 _vInPos : POSITION,\r\n"
		"	in vec2 _vInTex : TEXCOORD,\r\n"
		"	out vec4 _vOutColor : COLOR ) {\r\n"
		//"	_vOutColor = vec4( _vInTex.x, _vInTex.y, (_vInTex.x + _vInTex.y) / 2.0f, 1.0f );\r\n"
		"	_vOutColor = tex2d( g_tTexure, g_sSampler, _vInTex );\r\n"
		//"	_vOutColor.a = 0.5;\r\n"
		//"	if ( _vOutColor.a != 1.0 ) { discard; }\r\n"
		"}\r\n"
	};
	if ( LSG_SE_SUCCESS != m_sShader.SetShaderFromMemory( reinterpret_cast<const uint8_t *>(szPixel), sizeof( szPixel ), "",
		LSG_ST_PIXEL, sError ) ) {
		CStd::DebugPrintA( "Error with pixel shader: " );
		CStd::DebugPrintA( sError.CStr() );
	}

	if ( LSG_SE_SUCCESS != m_sShader.Compile( sError ) ) {
		CStd::DebugPrintA( "Error compiling: " );
		CStd::DebugPrintA( sError.CStr() );
	}



	struct LST_TEX_VERT {
		CVector3			vPos;
		CVector2			vTex;
	};
	const static LST_TEX_VERT vVerts[] = {
		// 1
		{	// UL
			CVector3( -LSM_ONE, LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ZERO, LSM_ZERO ),
		},
		{	// UR
			CVector3( LSM_ONE, LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ONE, LSM_ZERO ),
		},
		{	// BL
			CVector3( -LSM_ONE, -LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ZERO, LSM_ONE ),
		},
		// 2
		{	// UR
			CVector3( LSM_ONE, LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ONE, LSM_ZERO ),
		},
		{	// BR
			CVector3( LSM_ONE, -LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ONE, LSM_ONE ),
		},
		{	// BL
			CVector3( -LSM_ONE, -LSM_ONE, LSM_ZERO ),
			CVector2( LSM_ZERO, LSM_ONE ),
		},
	};
	LSG_VERTEX_ELEMENT_DESC vedDesc[] = {
		{
			LSG_VE_R32G32B32_F,
			0,
			LSG_S_POSITION,
			0
		},
		{
			LSG_VE_R32G32_F,
			0,
			LSG_S_TEXCOORD,
			0
		},
	};
	if ( !m_vbVertBuffer.CreateVertexBuffer( /*vVerts*/NULL, vedDesc, LSE_ELEMENTS( vedDesc ), false,
		sizeof( LST_TEX_VERT ),
		LSE_ELEMENTS( vVerts ),
		LSG_VBUT_DYNAMIC, LSG_PT_TRIANGLELIST ) ) {
		CStd::DebugPrintA( "Error creating vertex buffer." );
	}
	LSG_VERTEX_BUFFER_MAP vbmMap = m_vbVertBuffer.Map( LSG_VBMA_WRITE_DISCARD );
	CStd::MemCpy( vbmMap.pvData, &vVerts, vbmMap.ui32Size );
	m_vbVertBuffer.Unmap( vbmMap );

	LSG_BLEND_DESC bdBlend;
	CGfx::DefaultBlendDesc( bdBlend );
	bdBlend.RenderTarget[0].BlendEnable = true;
	bdBlend.RenderTarget[0].SrcBlend = CStd::ImpCast( bdBlend.RenderTarget[0].SrcBlend, LSG_BM_SRC_ALPHA );
	bdBlend.RenderTarget[0].DestBlend = CStd::ImpCast( bdBlend.RenderTarget[0].DestBlend, LSG_BM_INV_SRC_ALPHA );
	bdBlend.RenderTarget[0].SrcBlendAlpha = CStd::ImpCast( bdBlend.RenderTarget[0].SrcBlendAlpha, LSG_BM_SRC_ALPHA );
	bdBlend.RenderTarget[0].DestBlendAlpha = CStd::ImpCast( bdBlend.RenderTarget[0].DestBlendAlpha, LSG_BM_ONE );
	bdBlend.RenderTarget[0].RenderTargetWriteMask = LSG_CWM_RED | LSG_CWM_GREEN | LSG_CWM_BLUE;
	CGfx::CreateBlendState( bdBlend, &m_pbsBlend );

	LSG_DEPTH_STENCIL_DESC dsdDepthStencil;
	CGfx::DefaultDepthStencilDesc( dsdDepthStencil );
	dsdDepthStencil.StencilReadMask = 0xFF;
	dsdDepthStencil.StencilWriteMask = 0xFF;
	dsdDepthStencil.StencilEnable = true;
	dsdDepthStencil.FrontFace.StencilFunc = CStd::ImpCast( dsdDepthStencil.FrontFace.StencilFunc, LSG_CF_EQUAL );
	dsdDepthStencil.FrontFace.StencilFailOp = CStd::ImpCast( dsdDepthStencil.FrontFace.StencilFailOp, LSG_SO_ZERO );
	dsdDepthStencil.FrontFace.StencilDepthFailOp = CStd::ImpCast( dsdDepthStencil.FrontFace.StencilDepthFailOp, LSG_SO_ZERO );
	dsdDepthStencil.FrontFace.StencilPassOp = CStd::ImpCast( dsdDepthStencil.FrontFace.StencilPassOp, LSG_SO_KEEP );
	CGfx::CreateDepthStencilState( dsdDepthStencil, &m_pdssDepthStencil );
	

	CImage iImage;
	char szBuffer[LSF_MAX_PATH];

	//iImage.LoadFile( CFileLib::ExpandResourcePath( "test-texture-RGB8.dds", szBuffer ) );
	iImage.LoadFile( CFileLib::ExpandResourcePath( "kodim10.qoi", szBuffer ) );
	//iImage.SetColorSpace( LSI_G_sRGB );
	//iImage.ResampleInPlace( iImage.GetWidth() >> 1, iImage.GetHeight() >> 1, LSI_F_KAISER_FILTER, 1.0/*-2.2*/ );
	//iImage.ResampleInPlace( 1920, 1080, LSI_F_MITCHELL_FILTER, 1.0/*-2.2*/ );
	//iImage.SetColorSpace( LSI_G_sRGB );
	iImage.ConvertToFormatInPlace( LSI_PF_R16G16B16A16F );
	//iImage.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
	//iImage.ConvertToFormatInPlace( LSI_PF_R8G8B8A8 );
	//iImage.ConvertToFormatInPlace( LSI_PF_R3G3B2 );
	//iImage.ConvertToFormatInPlace( LSI_PF_R16G16B16A16F );
	//iImage.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
	//iImage.ConvertToFormatInPlace( LSI_PF_R16G16B16A16 );
	//iImage.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
	iImage.ConvertToDxtInPlace( LSI_PF_DXT1 );
	iImage.ConvertToDxtInPlace( LSI_PF_DXT2 );
	iImage.ConvertToDxtInPlace( LSI_PF_DXT3 );
	iImage.ConvertToDxtInPlace( LSI_PF_DXT4 );
	iImage.ConvertToDxtInPlace( LSI_PF_DXT5 );
	iImage.ConvertToFormatInPlace( LSI_PF_R8G8B8A8 );
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_R16 );
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_RG_RGTC2 );
	
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_ATC_RGB_AMD );
	iImage.ConvertToKtx1InPlace( LSI_KIF_GL_RGB9_E5 );
	iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG );
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_SRGB_ALPHA );
	iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC );
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_ETC1_RGB8_OES );
	iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_RGBA_ASTC_4x4_KHR );
	//iImage.ConvertToKtx1InPlace( LSI_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM );
	//iImage.ConvertToFormatInPlace( LSI_PF_R16G16B16A16F );
	iImage.ConvertToFormatInPlace( LSI_PF_R32G32B32A32F );
	//iImage.ConvertToFormatInPlace( LSI_PF_R3G3B2 );
	//iImage.ResampleInPlace( iImage.GetWidth() >> 1, iImage.GetHeight() >> 1, LSI_F_BOX_FILTER, -2.2 );
	m_t2Texture.CreateTexture( &iImage );


	m_ui32Frames = 0UL;
	m_ui64LastTime = _pgGame->RenderTime().GetCurMicros();
	CMemLib::ReleaseEmptyPools();
}

/**
 * Called when it is time for the state to die.  Free resources here.
 *
 * \param _pgGame Pointer to the game class which allows access to game-specific resources.
 * \param _ui32NextState The identifier of the next state.
 */
void LSE_CALL CTextureTest::Destroy( CGame * /*_pgGame*/, int32_t /*_ui32NextState*/ ) {
	CGfx::ReleaseState( m_pbsBlend );
	CGfx::ReleaseState( m_pdssDepthStencil );
}

/**
 * Called once every frame the state has focus to allow the state to render itself.
 * If the state is in the background, this is not called.  Return false to exit the game.
 *
 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
 * \return Return false if the game should close.
 */
LSBOOL LSE_CALL CTextureTest::Draw( CGame * _pgGame ) {
	CGfx::SetClearColor( CMathLib::FMod( _pgGame->Time().GetCurMills() / 1000.0f, LSM_ONE ), 1.0f, 0.0f, 1.0f );

	CGfx::SetShader( &m_sShader );

	m_vbVertBuffer.SetStream( 0 );
	CGfx::OMSetBlendState( m_pbsBlend, nullptr, LSG_CWM_ALL );
	CGfx::OMSetDepthStencilState( m_pdssDepthStencil, 0 );

	CMatrix4x4 mProj;
	mProj.Identity();
	CGfx::SetProjMatrix( mProj );
	m_t2Texture.Activate( 0 );

	CGfx::Render();



	++m_ui32Frames;
	if ( _pgGame->RenderTime().GetCurMicros() - m_ui64LastTime >= 1000000ULL * 5ULL ) {
		char szBuffer[128];
		double dFps = m_ui32Frames / (static_cast<double>(_pgGame->RenderTime().GetCurMicros() - m_ui64LastTime) * (1.0 / 1000000.0));
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%f FPS  %I64u Triangles per second.  %f ms per frame.\r\n", dFps,
			CGfx::GetTrianglesDrawn() / 5ULL,
			1000.0 / dFps );
		CStd::DebugPrintA( szBuffer );
		m_ui64LastTime = _pgGame->RenderTime().GetCurMicros();
		m_ui32Frames = 0UL;
		CGfx::ClearTriangles();
	}
	return true;
}