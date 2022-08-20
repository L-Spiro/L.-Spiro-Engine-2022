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

#include "LSTTriangleTest.h"
#include "Image/LSIImage.h"


// == Various constructors.
LSE_CALLCTOR CTriangleTest::CTriangleTest() {
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
void LSE_CALL CTriangleTest::Init( CGame * _pgGame, int32_t /*_ui32State*/, int32_t /*_ui32PrevState*/, uintptr_t /*_uptrUser*/ ) {
	const static char szVertex[] = {
		"// == Uniforms.\n"
		"uniform float[4][4] g_mModelViewProjMatrix : WORLDVIEWPROJ;\n"

		"void Main( in vec3 _vInPos : POSITION,\r\n"
		"	out vec4 _vOutPos : POSITION ) {\r\n"
		"	_vOutPos = mul( g_mModelViewProjMatrix, vec4( _vInPos, 1.0f ) );\r\n"
		"}\r\n"
	};
	CString sError;
	if ( LSG_SE_SUCCESS != m_sShader.SetShaderFromMemory( reinterpret_cast<const uint8_t *>(szVertex), sizeof( szVertex ), "",
		LSG_ST_VERTEX, sError ) ) {
		CStd::DebugPrintA( "Error with vertex shader: " );
		CStd::DebugPrintA( sError.CStr() );
	}


	const static char szPixel[] = {
		"void Main( in vec4 _vInPos : POSITION,\r\n"
		"	out vec4 _vOutColor : COLOR ) {\r\n"
		"	_vOutColor = vec4( 1.0f, 0.0f, 0.0f, 1.0f );\r\n"
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




	const static CVector3 vVerts[] = {
		CVector3( -LSM_ONE, LSM_ZERO, LSM_ZERO ),
		CVector3( LSM_ZERO, LSM_ONE, LSM_ZERO ),
		CVector3( LSM_ONE, LSM_ZERO, LSM_ZERO ),
	};
	LSG_VERTEX_ELEMENT_DESC vedDesc[] = {
		{
			LSG_VE_R32G32B32_F,
			0,
			LSG_S_POSITION,
			0
		},
	};
	if ( !m_vbVertBuffer.CreateVertexBuffer( vVerts, vedDesc, LSE_ELEMENTS( vedDesc ), false,
		sizeof( CVector3 ),
		LSE_ELEMENTS( vVerts ),
		LSG_VBUT_SETONLY, LSG_PT_TRIANGLELIST ) ) {
		CStd::DebugPrintA( "Error creating vertex buffer." );
	}


	static const uint16_t ui16Indices[] = {
		0, 1, 2
	};
	if ( !m_ibIndexBuffer.CreateIndexBuffer( ui16Indices,
			LSE_ELEMENTS( ui16Indices ), sizeof( ui16Indices[0] ),
			false,
			LSG_IBUT_SETONLY,
			LSG_PT_TRIANGLELIST ) ) {
		CStd::DebugPrintA( "Error creating index buffer." );
	}


	m_ui32Frames = 0UL;
	m_ui64LastTime = _pgGame->RenderTime().GetCurMicros();
	CMemLib::ReleaseEmptyPools();
}

/**
 * Called once every frame the state has focus to allow the state to render itself.
 * If the state is in the background, this is not called.  Return false to exit the game.
 *
 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
 * \return Return false if the game should close.
 */
LSBOOL LSE_CALL CTriangleTest::Draw( CGame * _pgGame ) {
	CGfx::SetClearColor( CMathLib::FMod( _pgGame->Time().GetCurMills() / 1000.0f, LSM_ONE ), 1.0f, 0.0f, 1.0f );
	//CGfx::Clear();

	CGfx::SetShader( &m_sShader );

	m_vbVertBuffer.SetStream( 0 );

	CMatrix4x4 mWorld;
	mWorld.MatrixTranslation( CVector3::Lerp( CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ),
		CVector3( LSM_ZERO, LSM_ZERO, -300.0f ),
		CMathLib::Abs( CMathLib::Cos( _pgGame->Time().GetCurMills() / 1000.0f ) ) ) );
	CGfx::SetWorldMatrix( mWorld );

	CMatrix4x4 mProj;
	CGfx::PerspectiveFov( mProj, static_cast<LSREAL>(54.43) * LSM_DEG2RAD,
		_pgGame->GetWidthF() / _pgGame->GetHeightF(),
		0.1f, 300.0f );
	CGfx::SetProjMatrix( mProj );

	CGfx::Render( &m_ibIndexBuffer );



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