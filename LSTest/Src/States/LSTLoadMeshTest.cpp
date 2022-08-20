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

#include "LSTLoadMeshTest.h"
#include "Map/LSTLMap.h"
#include "Models/LSEModelInstance.h"


// == Various constructors.
LSE_CALLCTOR CLoadMeshTest::CLoadMeshTest() {
}
LSE_CALLCTOR CLoadMeshTest::~CLoadMeshTest() {
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
void LSE_CALL CLoadMeshTest::Init( CGame * _pgGame, int32_t /*_ui32State*/, int32_t /*_ui32PrevState*/, uintptr_t /*_uptrUser*/ ) {
	/*CMap<uint32_t, uint16_t> mMap;
	mMap.Allocate( 5 );
	mMap.Insert( 32, 3 );
	mMap.Insert( 16, 2 );
	mMap.Insert( 8, 1 );
	mMap.Allocate( 5000 );
	mMap.Insert( 4, 0 );
	mMap.Insert( 64, 4 );
	mMap.Insert( 62, 4000 );
	mMap.Insert( 63, 4001 );*/
	
#if 1
#if !defined( LSG_DX11 )
	const static char szVertex[] = {
		"// == Uniforms.\r\n"
		"uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"

		"void Main( in vec3 _vInPos : POSITION,\r\n"
		"	in vec3 _vInNorm : NORMAL,\r\n"
		"	in vec3 _vInColor : COLOR,\r\n"
		"	out vec4 _vOutPos : POSITION,\r\n"
		"	out vec3 _vOutColor : COLOR,\r\n"
		"	out vec4 _vOutNorm : NORMAL ) {\r\n"
		"	_vOutPos = mul( g_mModelViewProjMatrix, vec4( _vInPos, 1.0f ) );\r\n"
		"	_vOutNorm = mul( g_mModelViewProjMatrix, vec4( _vInNorm, 0.0f ) );\r\n"
		"	_vOutColor = _vInColor;\r\n"
		"}\r\n"
	};
#else
	const static char szVertex[] = {
		"// == Uniforms.\r\n"
		"uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"

		"void Main( in vec3 _vInPos : POSITION,\r\n"
		"	in vec3 _vInNorm : NORMAL,\r\n"
		"	out vec4 _vOutPos : POSITION,\r\n"
		"	out vec4 _vOutNorm : NORMAL ) {\r\n"
		"	_vOutPos = mul( g_mModelViewProjMatrix, vec4( _vInPos, 1.0f ) );\r\n"
		"	_vOutNorm = mul( g_mModelViewProjMatrix, vec4( _vInNorm, 0.0f ) );\r\n"
		"}\r\n"
	};
#endif	// #if !defined( LSG_DX11 )
	CString sError;
	if ( LSG_SE_SUCCESS != m_sShader.SetShaderFromMemory( reinterpret_cast<const uint8_t *>(szVertex), sizeof( szVertex ), "",
		LSG_ST_VERTEX, sError ) ) {
		CStd::DebugPrintA( "Error with vertex shader: " );
		CStd::DebugPrintA( sError.CStr() );
	}

#if !defined( LSG_DX11 )
	const static char szPixel[] = {
		"uniform vec4		g_vDiffuseMaterial : MATDIFFUSE;\r\n"
		"uniform vec4		g_vEmissiveMaterial : MATEMISSIVE;\r\n"
		"uniform vec4		g_vSpecularMaterial : MATSPECULAR;\r\n"
		"uniform float		g_fReflectivity : REFLECTIVITY;\r\n"

		"void Main( in vec4 _vInPos : POSITION,\r\n"
		"	in vec4 _vInNorm : NORMAL,\r\n"
		"	in vec3 _vInColor : COLOR,\r\n"
		"	out vec4 _vOutColor : COLOR ) {\r\n"
		"	vec3 vInColor;"
		"	if ( length( _vInColor ) != 0.0 ) { vInColor = _vInColor * g_vDiffuseMaterial.xyz; }\r\n"
		"	else { vInColor = g_vDiffuseMaterial.xyz; }\r\n"
		//"	vInColor = _vInColor;\r\n"
		"	vec3 vNormal = normalize( _vInNorm.xyz );\r\n"
		"	vec3 vColor = dot( vec3( 0, 0, -1 ), vNormal ) * vInColor;\r\n"
		//"	vColor += saturate( dot( normalize( vec3( -0.7246, 1.0, -0.0 ) ), vNormal ) ) * vec3( 0.84, 0.12, 0.2 );\r\n"
		"	_vOutColor = vec4( vColor.x, vColor.y, vColor.z, g_vDiffuseMaterial.w );\r\n"
		"}\r\n"
	};
#else
	const static char szPixel[] = {
		"uniform vec4		g_vDiffuseMaterial : MATDIFFUSE;\r\n"
		"uniform vec4		g_vEmissiveMaterial : MATEMISSIVE;\r\n"
		"uniform vec4		g_vSpecularMaterial : MATSPECULAR;\r\n"
		"uniform float		g_fReflectivity : REFLECTIVITY;\r\n"

		"void Main( in vec4 _vInPos : POSITION,\r\n"
		"	in vec4 _vInNorm : NORMAL,\r\n"
		"	out vec4 _vOutColor : COLOR ) {\r\n"
		//"	vInColor = _vInColor;\r\n"
		"	vec3 vNormal = normalize( _vInNorm.xyz );\r\n"
		"	vec3 vColor = dot( vec3( 0, 0, -1 ), vNormal ) * g_vDiffuseMaterial.xyz;\r\n"
		//"	vColor += saturate( dot( normalize( vec3( -0.7246, 1.0, -0.0 ) ), vNormal ) ) * vec3( 0.84, 0.12, 0.2 );\r\n"
		"	_vOutColor = vec4( vColor.x, vColor.y, vColor.z, g_vDiffuseMaterial.w );\r\n"
		"}\r\n"
	};
#endif	// #if !defined( LSG_DX11 )
	if ( LSG_SE_SUCCESS != m_sShader.SetShaderFromMemory( reinterpret_cast<const uint8_t *>(szPixel), sizeof( szPixel ), "",
		LSG_ST_PIXEL, sError ) ) {
		CStd::DebugPrintA( "Error with pixel shader: " );
		CStd::DebugPrintA( sError.CStr() );
	}

	if ( LSG_SE_SUCCESS != m_sShader.Compile( sError ) ) {
		CStd::DebugPrintA( "Error compiling: " );
		CStd::DebugPrintA( sError.CStr() );
	}
#endif	// #if 0


	//uint64_t ui64Start = _pgGame->RenderTime().GetRealTime();
	//uintptr_t uiptrEnd;
	//CStd::PrintError( m_smModel.LoadFromMemory( CSharedModel::SharedFilePtr( "E:/My Projects/LSEngine/LSFbxConverter/Test/Dodge.lsm" ), uiptrEnd ) );
	/*CStd::PrintError( m_smModel.LoadFromMemory( CSharedModel::SharedFilePtr( "Samus.lsm" ), uiptrEnd ) );
	uint64_t ui64End = _pgGame->RenderTime().GetRealTime();
	char szBuffer[128];
	CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "Load time: %f seconds.\r\n", _pgGame->RenderTime().TicksToMicros( ui64End - ui64Start ) * (1.0f / 1000000.0f) );
	CStd::DebugPrintA( szBuffer );*/

	
	
	CSharedCameraPtr scpCam;
	scpCam.New();
	m_sm3dScene.SetMainCam( scpCam );
	
#if 0
	scpCam->SetPerspParms( static_cast<LSREAL>(54.43) * LSM_DEG2RAD,
		_pgGame->GetWidthF() / _pgGame->GetHeightF(),
		10.1f, 6000.0f );
#define LST_DIST	-155.0f
#define LST_DEPTH	-55.0f
	m_smipModel = m_sm3dScene.CreateModelInstance( "Amy.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( 0.0f, LST_DEPTH, LST_DIST );
	//m_smipModel->Orientation().SetScale( 0.025f );
	
	/*m_smipModel->Orientation().Pos() = CVector3( LSM_ZERO, LST_DEPTH, -24.0f );
	m_smipModel->Orientation().SetScale( 0.05f );*/
	m_smipModel = m_sm3dScene.CreateModelInstance( "Samus.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( -20.0f, LST_DEPTH, LST_DIST );

	m_smipModel = m_sm3dScene.CreateModelInstance( "Peach Blossom.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( 20.0f, LST_DEPTH, LST_DIST );

	m_smipModel = m_sm3dScene.CreateModelInstance( "Peach.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( 40.0f, LST_DEPTH, LST_DIST );

	//m_smipModel = m_sm3dScene.CreateModelInstance( "GigaBowser.lsm" );
	//m_smipModel->Orientation().Pos() = CVector3( 0.0f, LST_DEPTH, -55.0f );

	m_smipModel = m_sm3dScene.CreateModelInstance( "Mario Circuit.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( 0.0f, LST_DEPTH, LST_DIST );

	m_smipModel = m_sm3dScene.CreateModelInstance( "Petey Prianha.lsm" );
	m_smipModel->Orientation().Pos() = CVector3( 0.0f, 50.72f + LST_DEPTH, LST_DIST );
#undef LST_DIST
#else
	scpCam->SetPerspParms( static_cast<LSREAL>(54.43) * LSM_DEG2RAD,
		_pgGame->GetWidthF() / _pgGame->GetHeightF(),
		1.1f, 5000.0f );
	scpCam->Orientation().Pos() = CVector3( 0.0f, 6.0f, 30.0f );

	//m_smipModel = m_sm3dScene.CreateModelInstance( "Toad Harbor.lsm" );
	//m_smipModel->Orientation().Pos() = CVector3( 0.0f, 0.0f, 0.0f );
	//m_smipModel->Orientation().SetScale( 0.025f );

	m_smipModel = m_sm3dScene.CreateModelInstance( "Dodge.lsm" );


	m_smipModel = m_sm3dScene.CreateModelInstance( "Chevy.lsm" );
	if ( m_smipModel.Valid() ) {
		m_smipModel->Orientation().Pos() = CVector3( 20.0f, 0.0f, 0.0f );
	}


	m_smipModel = m_sm3dScene.CreateModelInstance( "Toad Harbor.lsm" );
	if ( m_smipModel.Valid() ) {
		m_smipModel->Orientation().Pos() = CVector3( -20.0f, -5.0f, 0.0f );
	}
#endif



	//m_vShaders;
	m_vShaders.Resize( m_smModel.TotalSubMeshes() );

	
	// Light data.
	CGfx::SetDirLight( 0, CVector4( -1.0f, -1.0f, -1.0f, 0.0f ), CVector4( 1.0f * LSM_PI, 1.0f * LSM_PI, 1.0f * LSM_PI, 0.0f ) );
	CGfx::SetDirLight( 1, CVector4( 1.0f, -1.0f, -1.0f, 0.0f ), CVector4( 0.25f * LSM_PI, 0.25f * LSM_PI, 0.25f * LSM_PI, 0.0f ) );
	CGfx::SetDirLight( 2, CVector4( -1.0f, 1.0f, 1.0f, 0.0f ), CVector4( 0.15f * LSM_PI, 0.15f * LSM_PI, 0.15f * LSM_PI, 0.0f ) );
	CGfx::SetDirLight( 3, CVector4( 0.0f, -2.5f, 1.0f, 0.0f ), CVector4( 0.35f * LSM_PI, 0.35f * LSM_PI, 0.35f * LSM_PI, 0.0f ) );
	/*CGfx::SetDirLight( 0, CVector4( -1.0f, -1.0f, -1.0f, 0.0f ), CVector4( 1.0f / LSM_PI, 1.0f / LSM_PI, 1.0f / LSM_PI, 0.0f ) );
	CGfx::SetDirLight( 1, CVector4( 1.0f, -1.0f, -1.0f, 0.0f ), CVector4( 0.15f / LSM_PI, 0.15f / LSM_PI, 0.14f / LSM_PI, 0.0f ) );*/
	CGfx::SetDirLightTotal( 3 );

	CGfx::SetPointLight( 0, CVector4( 0.0f, 0.0f, -5.0f, 1.0f ), CVector4( 1000.0f * 1.0f, 1000.0f * 1.0f, 1000.0f * 1.0f, 0.0f ), 500.0f );
	CGfx::SetPointLight( 2, CVector4( -1.0f, 2.0f, -5.0f, 1.0f ), CVector4( 1000.0f * LSM_PI, 1000.0f * 0.5f, 1000.0f * 0.5f, 0.0f ), 105.0f );
	CGfx::SetPointLight( 1, CVector4( 2.0f, 20.0f, -25.0f, 1.0f ), CVector4( 1000.0f * 0.5f, 1000.0f * LSM_PI, 1000.0f * LSM_PI, 0.0f ), 230.0f );
	CGfx::SetPointLightTotal( 3 );


	m_ui32Frames = 0UL;
	m_ui64LastTime = _pgGame->RenderTime().GetCurMicros();
	CMemLib::ReleaseEmptyPools();
	_pgGame->Mouse().Reset();

	//CMemLib::PrintAllocations();
}

/**
 * Called when it is time for the state to die.  Free resources here.
 *
 * \param _pgGame Pointer to the game class which allows access to game-specific resources.
 * \param _ui32NextState The identifier of the next state.
 */
void LSE_CALL CLoadMeshTest::Destroy( CGame * /*_pgGame*/, int32_t /*_ui32NextState*/ ) {
	m_psaAllocator->Free( m_pui8File );
	//m_smipModel.Reset();
}

/**
 * Called once every frame the state has focus to allow the state to render itself.
 * If the state is in the background, this is not called.  Return false to exit the game.
 *
 * \param _pgGame Pointer to the custom game class which can be used to access game-specific resources.
 * \return Return false if the game should close.
 */
LSBOOL LSE_CALL CLoadMeshTest::Draw( CGame * _pgGame ) {
	//CGfx::SetClearColor( 10.1f, 10.1f, 10.1f, 1.0f );
	CGfx::SetClearColor( CImageLib::LinearToSRgb( 0.1f ), CImageLib::LinearToSRgb( 0.01f ), CImageLib::LinearToSRgb( 0.01f ), 1.0f );
	//CGfx::SetClearColor( CImageLib::LinearToSRgb( 0.207843f ), CImageLib::LinearToSRgb( 0.596078f ), CImageLib::LinearToSRgb( 0.996078f ), 1.0f );
	//CGfx::SetShader( m_sspShader.Get() );

	/*CGfx::SetPointLight( 0, CVector4( CMathLib::Sin( _pgGame->Time().GetCurMills() / 530.0f ) * 100.0f,
			20.0f,
			CMathLib::Cos( _pgGame->Time().GetCurMills() / 530.0f ) * -100.0f - 24.0f,
			1.0f ),
		CVector4( 3000.0f * 1.0f, 3000.0f * 1.0f, 3000.0f * 1.0f, 0.0f ), 1500.0f );*/

	

#if 0
	CMatrix4x4 mWorld, mRotX, mRotY;
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -6.0f, -8.0f ) );
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -3.0f, -10.0f ) );
	mWorld.MatrixTranslation( CVector3( LSM_ZERO, -5.0f, -24.0f ) );
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -10.0f, -24.0f ) );
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -40.0f, -125.0f ) );
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -105.0f, -425.0f ) );
	//mWorld.MatrixTranslation( CVector3( LSM_ZERO, -105.0f, -10250.0f ) );

	/*CMatrix4x4 mScale;
	mScale.MatrixScaling( 5.0f, 5.0f, 5.0f );*/
	
	//mRotX.MatrixRotationXLH( _pgGame->RenderTime().GetCurMills() / 1000.0f * 0.0f );
	mRotY.MatrixRotationYLH( _pgGame->RenderTime().GetCurMills() / 3830.0f );
	//mRotY.Identity();
	//CGfx::SetWorldMatrix( mRotX * mRotY * mWorld );

	/*CMatrix4x4 mProj;
	mProj.PerspectiveFovRH( static_cast<LSREAL>(54.43) * LSM_DEG2RAD,
		_pgGame->GetWidthF() / _pgGame->GetHeightF(),
		0.1f, 1500.0f );
	CGfx::SetProjMatrix( mProj );*/

	/*for ( uint32_t I = 0; I < m_vMeshes.Length(); ++I ) {
		uint32_t ui32Total = m_vMeshes[I].m_vIndexBuffers.Length();
		for ( uint32_t J = 0; J < ui32Total; ++J ) {
			m_vMeshes[I].m_vbVertexBuffer.SetStream( 0 );
			CGfx::Render( &m_vMeshes[I].m_vIndexBuffers[J].m_ibIndexBuffer,
				m_vMeshes[I].m_vIndexBuffers[J].m_ui32Base );
		}
	}*/
	/*for ( uint32_t I = 0; I < m_vMeshInstances.Length(); ++I ) {
		lsd::CMesh & mMesh = m_vMeshes[m_vMeshInstances[I].m_miData.ui32Index];
		uint32_t ui32Total = mMesh.m_vIndexBuffers.Length();

		CGfx::SetWorldMatrix( m_vMeshInstances[I].m_miData.mLocalTrans * mRotX * mRotY * mWorld );
		CGfx::RSSetState( m_vMeshInstances[I].m_prsRasterState );
		for ( uint32_t J = 0; J < ui32Total; ++J ) {
			mMesh.m_vbVertexBuffer.SetStream( 0 );
			CGfx::Render( &mMesh.m_vIndexBuffers[J].m_ibIndexBuffer,
				mMesh.m_vIndexBuffers[J].m_ui32Base );
		}
	}*/
	uint32_t ui32ShaderIndex = 0;
	for ( uint32_t I = m_smModel.MeshInstances().Length(); I--; ) {
		lsd::CMeshInstance & miMeshInstance = m_smModel.MeshInstances()[I];
		lsd::CMesh * pmMesh = miMeshInstance.Mesh();

		CGfx::SetWorldMatrix( miMeshInstance.Transform() *
			//mScale *
			/*mRotX **/ mRotY * mWorld );
		CGfx::RSSetState( const_cast<LSG_RASTERIZER_STATE *>(miMeshInstance.RasterizerState()) );
		CGfx::OMSetBlendState( NULL, NULL, LSG_CWM_ALL );

		uint32_t ui32Total = pmMesh->SubMeshes().Length();
		for ( uint32_t J = 0; J < ui32Total; ++J ) {
			if ( pmMesh->SubMeshes()[J].Material() ) {
				if ( IsAlpha( pmMesh->SubMeshes()[J] ) ) { continue; }
			}
			else { continue; }

			CreateShader( ui32ShaderIndex, (*pmMesh), (*pmMesh->SubMeshes()[J].Material()) );
			CGfx::SetShader( m_vShaders[ui32ShaderIndex].sspShader.Get() );

			ActivateMaterial( (*pmMesh->SubMeshes()[J].Material()), m_vShaders[ui32ShaderIndex].skKey );

			++ui32ShaderIndex;
			pmMesh->VertexBuffer().SetStream( 0 );
			CGfx::Render( &pmMesh->SubMeshes()[J].IndexBuffer(),
				pmMesh->SubMeshes()[J].BaseOffset() );
		}
	}
	for ( uint32_t I = m_smModel.MeshInstances().Length(); I--; ) {
		lsd::CMeshInstance & miMeshInstance = m_smModel.MeshInstances()[I];
		lsd::CMesh * pmMesh = miMeshInstance.Mesh();

		CGfx::SetWorldMatrix( miMeshInstance.Transform() *
			//mScale *
			/*mRotX **/ mRotY * mWorld );
		CGfx::RSSetState( const_cast<LSG_RASTERIZER_STATE *>(miMeshInstance.RasterizerState()) );
		CGfx::OMSetBlendState( miMeshInstance.m_pbsBlendState, NULL, LSG_CWM_ALL );

		uint32_t ui32Total = pmMesh->SubMeshes().Length();
		for ( uint32_t J = 0; J < ui32Total; ++J ) {
			if ( pmMesh->SubMeshes()[J].Material() ) {
				if ( !IsAlpha( pmMesh->SubMeshes()[J] ) ) { continue; }
			}
			else { continue; }

			CreateShader( ui32ShaderIndex, (*pmMesh), (*pmMesh->SubMeshes()[J].Material()) );
			CGfx::SetShader( m_vShaders[ui32ShaderIndex].sspShader.Get() );

			ActivateMaterial( (*pmMesh->SubMeshes()[J].Material()), m_vShaders[ui32ShaderIndex].skKey );

			++ui32ShaderIndex;
			pmMesh->VertexBuffer().SetStream( 0 );
			CGfx::Render( &pmMesh->SubMeshes()[J].IndexBuffer(),
				pmMesh->SubMeshes()[J].BaseOffset() );
		}
	}
#endif
	














	float fTime = _pgGame->RenderTime().GetSecondsSinceLastFrame();
	CVector3 vCamPos = m_sm3dScene.MainCam()->Orientation().Pos();
	//fTime = 1.0f / 20.0f;
	

	LSREAL fSpeed = _pgGame->Mouse().IsMouseButtonDown( 2 ) ? 5.0f : 1.0f;
#define LSE_SPEED_MOD		1.5f
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_W ) ) {
		vCamPos += m_sm3dScene.MainCam()->Orientation().GetForward() * fTime * -5.0f * LSE_SPEED_MOD * fSpeed;
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_S ) ) {
		vCamPos += m_sm3dScene.MainCam()->Orientation().GetForward() * fTime * 5.0f * LSE_SPEED_MOD * fSpeed;
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_A ) ) {
		vCamPos += m_sm3dScene.MainCam()->Orientation().GetRight() * fTime * -5.0f * LSE_SPEED_MOD * fSpeed;
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_D ) ) {
		vCamPos += m_sm3dScene.MainCam()->Orientation().GetRight() * fTime * 5.0f * LSE_SPEED_MOD * fSpeed;
	}

	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_UP ) ) {
		m_sm3dScene.MainCam()->Orientation().Pitch( 1.0f * fTime * -0.25f );
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_DOWN ) ) {
		m_sm3dScene.MainCam()->Orientation().Pitch( 1.0f * fTime * 0.25f );
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_LEFT ) ) {
		m_sm3dScene.MainCam()->Orientation().Yaw( 1.0f * fTime * 0.25f );
	}
	if ( _pgGame->Keyboard().KeyIsDown( LSE_KC_RIGHT ) ) {
		m_sm3dScene.MainCam()->Orientation().Yaw( 1.0f * fTime * -0.25f );
	}

	m_sm3dScene.MainCam()->Orientation().Pitch( _pgGame->Mouse().GetInterpYDelta() * fTime * 0.25f );
	m_sm3dScene.MainCam()->Orientation().Yaw( _pgGame->Mouse().GetInterpXDelta() * fTime * -0.25f );
	m_sm3dScene.MainCam()->Orientation().SetPosU( vCamPos );



	m_sm3dScene.Render();

	
	++m_ui32Frames;
	if ( _pgGame->RenderTime().GetCurMicros() - m_ui64LastTime >= 1000000ULL * 5ULL ) {
		char szBuffer[128];
		double dFps = m_ui32Frames / (static_cast<double>(_pgGame->RenderTime().GetCurMicros() - m_ui64LastTime) * (1.0 / 1000000.0));
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ), "%f FPS  %I64u Triangles per second.  %f ms per frame.\r\n", dFps,
			CGfx::GetTrianglesDrawn() / 5ULL,
			1000.0f / dFps );
		CStd::DebugPrintA( szBuffer );
		m_ui64LastTime = _pgGame->RenderTime().GetCurMicros();
		m_ui32Frames = 0UL;
		CGfx::ClearTriangles();
	}

	return true;
}

/**
 * Gets the size of the heap to be allocated for this state, or 0 for none.
 *
 * \param _pgGame The game pointer.
 * \param _ui32State The ID of the current state.
 * \param _uptrUser User data passed to the state upon initialization.  Same value as what is passed in
 *	Init().
 * \return Return a non-zero value to indicate the size of the heap to be allocated for this class.
 */
LSA_SIZE LSE_CALL CLoadMeshTest::GetHeapSize( CGame * /*_pgGame*/, int32_t /*_ui32State*/, uintptr_t /*_uptrUser*/ ) const {
	return 1024 * 1024 * 32;
}

// Sets material information and activates textures.
void LSE_CALL CLoadMeshTest::ActivateMaterial( const CModelMaterial &_mmMaterial, const LSD_SHADERKEY &_skKey ) {
	CGfx::SetDiffuseMaterial( _mmMaterial.Diffuse() );
	CGfx::SetEmissiveMaterial( _mmMaterial.Emissive() );
	CGfx::SetSpecularMaterial( _mmMaterial.Specular() );
	
	CGfx::SetMaterialPower( _mmMaterial.Shininess() );
	CGfx::SetMaterialRoughness( _mmMaterial.Roughness(), _mmMaterial.SpecularReflectance() );
	//CGfx::SetReflectivity( _mmMaterial.Reflection() );
	CGfx::SetAshikhmanShirley( _mmMaterial.AshikminShirley().x, _mmMaterial.AshikminShirley().y );


#define LST_ACTIVATE( Name, NAME )																						\
	if ( _mmMaterial.Name##Texture().Valid() ) {																		\
		const_cast<CSharedTexture2dPtr &>(_mmMaterial.Name##Texture())->Activate( _skKey.ui8Textures[LSD_TS_##NAME] );	\
	}

	LST_ACTIVATE( Diffuse, DIFFUSE );
	LST_ACTIVATE( DiffuseFactor, DIFFUSE_FACTOR );

	LST_ACTIVATE( Transparency, TRANSPARENCY );
	LST_ACTIVATE( TransparencyFactor, TRANSPARENCY_FACTOR );

	LST_ACTIVATE( Emissive, EMISSIVE );
	LST_ACTIVATE( EmissiveFactor, EMISSIVE_FACTOR );

	LST_ACTIVATE( Reflection, REFLECTION );
	LST_ACTIVATE( ReflectionFactor, REFLECTION_FACTOR );

	LST_ACTIVATE( Specular, SPECULAR );
	LST_ACTIVATE( SpecularFactor, SPECULAR_FACTOR );

	LST_ACTIVATE( Shininess, SHININESS );

	LST_ACTIVATE( NormalMap, NORMAL_MAP );

#undef LST_ACTIVATE
}