/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: The OpenGL widget used throughout the project.
 */

#include "LSSESeOpenGlWidget.h"


// == Members.
/** Tracks the first instance of an OpenGL context being made. */
CAtomic::ATOM CSeOpenGlWidget::m_aOpenGlContextMade = 0;

/** The shared OpenGL widget. */
QGLWidget * CSeOpenGlWidget::m_pqlwSharedWidget = Q_NULLPTR;

/** A basic vertex buffer for drawing quads. */
CNBufferVertexBuffer<4> * CSeOpenGlWidget::m_pnbvbVertices = NULL;

/** The default sprite sampler. */
CSampler * CSeOpenGlWidget::m_psSampler = NULL;

/** The sprite raster mode. */
LSG_RASTERIZER_STATE * CSeOpenGlWidget::m_prsRasterState = NULL;

/** The depth mode. */
LSG_DEPTH_STENCIL_STATE * CSeOpenGlWidget::m_pdssDepthState = NULL;

/** The blend state for alpha objects. */
LSG_BLEND_STATE * CSeOpenGlWidget::m_pbsAlphaBlendState = NULL;

 // == Various constructors.
LSE_CALLCTOR CSeOpenGlWidget::CSeOpenGlWidget( QWidget * _pwParent ) :
	QGLWidget( _pwParent, m_pqlwSharedWidget ) {
	QGLContext * pcContext = context();
	bool bIsSharing = pcContext->isSharing();

	if ( CAtomic::InterlockedIncrement( m_aOpenGlContextMade ) == 1 ) {
		m_pqlwSharedWidget = this;
		makeCurrent();
		CGraphicsLib::PostInitGraphicsLib();

		m_psSampler = LSENEW CSampler();
		m_psSampler->SetAddressMode( LSG_TAM_CLAMP, LSG_TAM_CLAMP, LSG_TAM_CLAMP );
		m_psSampler->SetMaxAnisotropy( 1 );
		m_psSampler->SetFilter( LSG_F_MIN_LINEAR_MAG_MIP_POINT );
		//m_psSampler->SetFilter( LSG_F_MIN_MAG_LINEAR_MIP_POINT );

		LSG_RASTERIZER_DESC rdRastrDesc;
		rdRastrDesc = CGfx::DefaultRasterizerDesc( rdRastrDesc );
		rdRastrDesc.CullMode = CStd::ImpCast( rdRastrDesc.CullMode, LSG_CM_NONE );
		rdRastrDesc.DepthClipEnable = false;
		CGfx::CreateRasterizerState( rdRastrDesc, &m_prsRasterState );

		LSG_DEPTH_STENCIL_DESC dsdDepdthDesc;
		dsdDepdthDesc = CGfx::DefaultDepthStencilDesc( dsdDepdthDesc );
		dsdDepdthDesc.StencilEnable = false;
		dsdDepdthDesc.DepthEnable = false;
		CGfx::CreateDepthStencilState( dsdDepdthDesc, &m_pdssDepthState );

		LSG_BLEND_DESC bdBlendDec;
		bdBlendDec = CGfx::DefaultBlendDesc( bdBlendDec );
		bdBlendDec.RenderTarget[0].BlendEnable = true;
		bdBlendDec.RenderTarget[0].SrcBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlend, LSG_BM_SRC_ALPHA );
		bdBlendDec.RenderTarget[0].DestBlend = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlend, LSG_BM_INV_SRC_ALPHA );
		bdBlendDec.RenderTarget[0].SrcBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].SrcBlendAlpha, LSG_BM_SRC_ALPHA );
		bdBlendDec.RenderTarget[0].DestBlendAlpha = CStd::ImpCast( bdBlendDec.RenderTarget[0].DestBlendAlpha, LSG_BM_ONE );
		CGfx::CreateBlendState( bdBlendDec, &m_pbsAlphaBlendState );

		static LSG_VERTEX_ELEMENT_DESC vedVertDesc[] = {
			{
				LSG_VE_R32G32_F,
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
		m_pnbvbVertices = LSENEW CNBufferVertexBuffer<4>();
		for ( LSUINT32 I = m_pnbvbVertices->TotalBuffers(); I--; ) {
			m_pnbvbVertices->GetBuffer( I ).CreateVertexBuffer( NULL, vedVertDesc, 2, false,
				sizeof( LSSE_SPRITE_VERTEX ), 6, LSG_VBUT_DYNAMIC, LSG_PT_TRIANGLELIST );
		}
	}
	m_cCamera.SetNearFar( LSREAL( 0.01 ), LSREAL( 100.0 ) );
	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setAttribute( Qt::WA_NoSystemBackground, true );
}
LSE_CALLCTOR CSeOpenGlWidget::~CSeOpenGlWidget() {
	if ( CAtomic::InterlockedDecrement( m_aOpenGlContextMade ) == 0 ) {
		LSEDELETE m_pnbvbVertices;
		m_pnbvbVertices = NULL;

		LSEDELETE m_psSampler;
		m_psSampler = NULL;

		CGfx::ReleaseState( m_prsRasterState );
		CGfx::ReleaseState( m_pdssDepthState );
		CGfx::ReleaseState( m_pbsAlphaBlendState );
	}
}

// == Functions.
/**
 * Makes the original OpenGL context current.
 */
LSVOID LSE_CALL CSeOpenGlWidget::MakeCurrent() {
	if ( m_pqlwSharedWidget ) {
		m_pqlwSharedWidget->context()->makeCurrent();
	}
}

/**
 * Arranges the given vertices into a quad.
 *
 * \param _psvVertices Vertices to arrange (must be 6 of them).
 * \param _fLeft Left of the quad.
 * \param _fTop Top of the quad.
 * \param _fRight Right of the quad.
 * \param _fBottom Bottom of the quad.
 * \param _fUvLeft Left UV of the quad.
 * \param _fUvTop Top UV of the quad.
 * \param _fUvRight Right UV of the quad.
 * \param _fUvBottom Bottom UV of the quad.
 */
LSVOID LSE_CALL CSeOpenGlWidget::MakeQuad( LSSE_SPRITE_VERTEX * _psvVertices,
	LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom,
	LSREAL _fUvLeft, LSREAL _fUvTop, LSREAL _fUvRight, LSREAL _fUvBottom ) {

	// ============
	// 1st Triangle
	// ============
	// Upper-left.
	_psvVertices->fPos[0] = _fLeft;
	_psvVertices->fPos[1] = _fTop;
	_psvVertices->fUv[0] = _fUvLeft;
	_psvVertices->fUv[1] = _fUvBottom;
	++_psvVertices;
	// Upper-right.
	_psvVertices->fPos[0] = _fRight;
	_psvVertices->fPos[1] = _fTop;
	_psvVertices->fUv[0] = _fUvRight;
	_psvVertices->fUv[1] = _fUvBottom;
	++_psvVertices;
	// Bottom-left.
	_psvVertices->fPos[0] = _fLeft;
	_psvVertices->fPos[1] = _fBottom;
	_psvVertices->fUv[0] = _fUvLeft;
	_psvVertices->fUv[1] = _fUvTop;
	++_psvVertices;
	// ============
	// 2nd Triangle
	// ============
	// Upper-right.
	_psvVertices->fPos[0] = _fRight;
	_psvVertices->fPos[1] = _fTop;
	_psvVertices->fUv[0] = _fUvRight;
	_psvVertices->fUv[1] = _fUvBottom;
	++_psvVertices;
	// Bottom-right.
	_psvVertices->fPos[0] = _fRight;
	_psvVertices->fPos[1] = _fBottom;
	_psvVertices->fUv[0] = _fUvRight;
	_psvVertices->fUv[1] = _fUvTop;
	++_psvVertices;
	// Bottom-left.
	_psvVertices->fPos[0] = _fLeft;
	_psvVertices->fPos[1] = _fBottom;
	_psvVertices->fUv[0] = _fUvLeft;
	_psvVertices->fUv[1] = _fUvTop;
}

/**
 * This virtual function is called once before the first call to paintGL() or resizeGL(), and then once whenever the widget has been assigned a new QGLContext. Reimplement it in a subclass.
 *	This function should set up any required OpenGL context rendering flags, defining display lists, etc.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 */
void CSeOpenGlWidget::initializeGL() {

}

/**
 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 */
void CSeOpenGlWidget::paintGL() {
	m_pnbvbVertices->Swap();
	m_cCamera.Update();
	/*LSCHAR szPrint[256];
	CStd::SPrintF( szPrint, 256, "OpenGl %f %f\r\n", m_cCamera.Width(), m_cCamera.Height() );
	CStd::DebugPrintA( szPrint );*/
	CGfx::SetScreenSize( width(), height() );
	CGfx::ApplyDefaultStates();
	CGfx::SetViewport( LSM_ZERO, LSM_ZERO, width(), height() );
	CGfx::SetClearColor( LSREAL( 171.0 / 255.0 ), LSREAL( 171.0 / 255.0 ), LSREAL( 171.0 / 255.0 ), LSM_ONE );
	CGfx::Clear();
	CGfx::SetWorldMatrix( CMatrix4x4::m_mIdentity );
	CGfx::SetViewMatrix( m_cCamera.ViewMatrix() );
	CGfx::SetProjMatrix( m_cCamera.ProjMatrix() );
}

/**
 * This virtual function is called whenever the widget has been resized. The new size is passed in _iWidth and _iHeight. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 *
 * \param _iWidth New width of the window.
 * \param _iHeight New height of the window.
 */
void CSeOpenGlWidget::resizeGL( LSINT32 _iWidth, LSINT32 _iHeight ) {
	m_cCamera.SetWidthHeight( _iWidth, _iHeight );
	m_cCamera.SetOrigin( CVector2( _iWidth / LSM_TWO, _iHeight / LSM_TWO ) );
}
