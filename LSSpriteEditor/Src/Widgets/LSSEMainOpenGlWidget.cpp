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
* Description: The main OpenGL view for editing sprites.
*/

#include "LSSEMainOpenGlWidget.h"
#include "../Project/LSSEProject.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent> 
#include <QScrollBar>



// == Various constructors.
LSE_CALLCTOR CMainOpenGlWidget::CMainOpenGlWidget( QWidget * _pwParent ) :
	CSeOpenGlWidget( _pwParent ),
	m_ppProject( NULL ),
	m_psbVertBar( Q_NULLPTR ),
	m_psbHorBar( Q_NULLPTR ) {
	m_vMousePos = CVector2( LSM_ZERO, LSM_ZERO );
	//m_psbVertBar = LSENEW QScrollBar( Qt::Vertical, this );
	//m_psbHorBar = LSENEW QScrollBar( Qt::Horizontal, this );
	{
		static const LSCHAR * pcVs = "uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"	
			"void Main( in vec2 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
			"	out vec4 _vOutPosition : POSITION0,\r\n"
			"	out vec2 _vOutTexCoord0 : TEXCOORD0,\r\n"
			"	out vec2 _vOutViewPos : TEXCOORD1 ) {\r\n"
			"	_vOutPosition = mul( g_mModelViewProjMatrix, vec4( _vInPosition.x, _vInPosition.y, 1.1, 1.0f ) );\r\n"
			"	_vOutViewPos = vec2( _vInPosition.x, -1.0 * _vInPosition.y );\r\n"
			"	_vOutTexCoord0 = _vInTexCoord0;\r\n"
			"}\r\n";
		CString sError;
		m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcVs), CStd::StrLen( pcVs ), NULL, LSG_ST_VERTEX, sError );
		static const LSCHAR * pcPs = "uniform Texture2d g_tTex2d0 : TEXUNIT0;\r\n"
			"uniform Sampler g_sSampler0 : TEXUNIT0;\r\n"
			"uniform bool g_bSrgb;\r\n"
			"#define LSE_LINEAR2SRGB( VAL )		(VAL) <= 0.0031308 ? (VAL) * 12.92 : 1.055 * pow( (VAL), 1.0f / 2.4 ) - 0.055\r\n"
			"void Main( out vec4 _vOutColor : COLOR,\r\n"
			"	in vec4 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
			"	in vec2 _vInViewPos : TEXCOORD1 ) {\r\n"
			"	_vOutColor = tex2d( g_tTex2d0, g_sSampler0, _vInTexCoord0 );\r\n"
			"	if ( g_bSrgb ) {\r\n"
			"		_vOutColor.x = LSE_LINEAR2SRGB( _vOutColor.x );\r\n"
			"		_vOutColor.y = LSE_LINEAR2SRGB( _vOutColor.y );\r\n"
			"		_vOutColor.z = LSE_LINEAR2SRGB( _vOutColor.z );\r\n"
			"	}\r\n"
			"}\r\n";
		m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcPs), CStd::StrLen( pcPs ), NULL, LSG_ST_PIXEL, sError );
		m_sShader.Compile( sError );
		m_hSrgb = m_sShader.GetConstantByName( "g_bSrgb" );
	}

	{
		static const LSCHAR * pcVs = "uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"	
			"void Main( in vec2 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
			"	out vec4 _vOutPosition : POSITION0,\r\n"
			"	out vec2 _vOutTexCoord0 : TEXCOORD0 ) {\r\n"
			"	_vOutPosition = mul( g_mModelViewProjMatrix, vec4( _vInPosition.x, _vInPosition.y, 1.1, 1.0f ) );\r\n"
			"	_vOutTexCoord0 = _vInTexCoord0;\r\n"
			"}\r\n";
		CString sError;
		m_sCrossHairShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcVs), CStd::StrLen( pcVs ), NULL, LSG_ST_VERTEX, sError );
		static const LSCHAR * pcPs = "uniform Texture2d g_tTex2d0 : TEXUNIT0;\r\n"
			"uniform Sampler g_sSampler0 : TEXUNIT0;\r\n"
			"uniform bool g_bSrgb;\r\n"
			"#define LSE_LINEAR2SRGB( VAL )		(VAL) <= 0.0031308 ? (VAL) * 12.92 : 1.055 * pow( (VAL), 1.0f / 2.4 ) - 0.055\r\n"
			"void Main( out vec4 _vOutColor : COLOR,\r\n"
			"	in vec4 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0 ) {\r\n"
			"	if ( _vInTexCoord0.x >= 0.0 && _vInTexCoord0.x <= 1.0 && _vInTexCoord0.y >= 0.0 && _vInTexCoord0.y <= 1.0 ) {\r\n"
			"		_vOutColor = tex2d( g_tTex2d0, g_sSampler0, _vInTexCoord0 );\r\n"
			"		if ( g_bSrgb ) {\r\n"
			"			_vOutColor.x = LSE_LINEAR2SRGB( _vOutColor.x );\r\n"
			"			_vOutColor.y = LSE_LINEAR2SRGB( _vOutColor.y );\r\n"
			"			_vOutColor.z = LSE_LINEAR2SRGB( _vOutColor.z );\r\n"
			"		}\r\n"
			"		_vOutColor.xyz = vec3( 1.0, 1.0, 1.0 ) - _vOutColor.xyz;\r\n"
			"		_vOutColor.w = 0.5;\r\n"
			"	}\r\n"
			"	else {\r\n"
			"		_vOutColor = vec4( 1.0, 0.0, 0.0, 1.0 );\r\n"
			"	}\r\n"
			"}\r\n";
		m_sCrossHairShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcPs), CStd::StrLen( pcPs ), NULL, LSG_ST_PIXEL, sError );
		m_sCrossHairShader.Compile( sError );
		m_hSrgbCrossHair = m_sCrossHairShader.GetConstantByName( "g_bSrgb" );
	}

	{
		static const LSCHAR * pcVs = "uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"	
			"void Main( in vec2 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
			"	out vec4 _vOutPosition : POSITION0,\r\n"
			"	out vec2 _vOutTexCoord0 : TEXCOORD0 ) {\r\n"
			"	_vOutPosition = mul( g_mModelViewProjMatrix, vec4( _vInPosition.x, _vInPosition.y, 1.1, 1.0f ) );\r\n"
			"	_vOutTexCoord0 = _vInTexCoord0;\r\n"
			"}\r\n";
		CString sError;
		m_sBoxShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcVs), CStd::StrLen( pcVs ), NULL, LSG_ST_VERTEX, sError );
		static const LSCHAR * pcPs = "uniform Texture2d g_tTex2d0 : TEXUNIT0;\r\n"
			"uniform Sampler g_sSampler0 : TEXUNIT0;\r\n"
			"uniform bool g_bSrgb;\r\n"
			"#define LSE_LINEAR2SRGB( VAL )		(VAL) <= 0.0031308 ? (VAL) * 12.92 : 1.055 * pow( (VAL), 1.0f / 2.4 ) - 0.055\r\n"
			"void Main( out vec4 _vOutColor : COLOR,\r\n"
			"	in vec4 _vInPosition : POSITION0,\r\n"
			"	in vec2 _vInTexCoord0 : TEXCOORD0 ) {\r\n"
			"	_vOutColor = vec4( 0.0, 1.0, 0.0, 0.5 );\r\n"
			"}\r\n";
		m_sBoxShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcPs), CStd::StrLen( pcPs ), NULL, LSG_ST_PIXEL, sError );
		m_sBoxShader.Compile( sError );
		//m_hSrgbCrossHair = m_sBoxShader.GetConstantByName( "g_bSrgb" );
	}


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
	for ( LSUINT32 I = m_nbvbCrossVertices.TotalBuffers(); I--; ) {
		m_nbvbCrossVertices.GetBuffer( I ).CreateVertexBuffer( NULL, vedVertDesc, 2, false,
			sizeof( LSSE_SPRITE_VERTEX ), 4 * 3, LSG_VBUT_DYNAMIC, LSG_PT_TRIANGLELIST );
	}
	for ( LSUINT32 I = m_nbvbBoxVertices.TotalBuffers(); I--; ) {
		m_nbvbBoxVertices.GetBuffer( I ).CreateVertexBuffer( NULL, vedVertDesc, 2, false,
			sizeof( LSSE_SPRITE_VERTEX ), 8 * 3, LSG_VBUT_DYNAMIC, LSG_PT_TRIANGLELIST );
	}
	
}
LSE_CALLCTOR CMainOpenGlWidget::~CMainOpenGlWidget() {
	LSEDELETE m_psbVertBar;
	LSEDELETE m_psbHorBar;
}

// == Functions.
/**
 * Sets the project pointer.
 *
 * \param _ppProject The project pointer.
 */
LSVOID LSE_CALL CMainOpenGlWidget::SetProject( CProject * _ppProject ) {
	m_ppProject = _ppProject;
}

/**
 * Converts a coordinate from the widget space into normalized texture coordinates.
 *
 * \param _fX The X coordinate to convert.
 * \param _fY The Y coordinate to convert.
 * \return Returns the converted [x,y] coordinates.
 */
CVector2 LSE_CALL CMainOpenGlWidget::ScreenToTexture( LSREAL _fX, LSREAL _fY ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();

	LSREAL fWinWidth = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->width();
	LSREAL fWinHeight = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->height();
	_fX -= fWinWidth / LSM_TWO;
	_fY -= fWinHeight / LSM_TWO;
	LSREAL fImageWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth() * m_ppProject->Images()[ui32Index].fZoom;
	LSREAL fImageHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight() * m_ppProject->Images()[ui32Index].fZoom;
	LSREAL fHalfWidth = fImageWidth / LSM_TWO;
	LSREAL fHalfHeight = fImageHeight / LSM_TWO;
	_fX += fHalfWidth;
	_fY += fHalfHeight;

	return CVector2( _fX / fImageWidth,
		_fY / fImageHeight );
}

/**
 * Converts a coordinate from normalized texture coordinates to widget space.
 *
 * \param _fX The X coordinate to convert.
 * \param _fY The Y coordinate to convert.
 * \return Returns the converted [x,y] coordinates.
 */
CVector2 LSE_CALL CMainOpenGlWidget::TextureToScreen( LSREAL _fX, LSREAL _fY ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fImageWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth() * m_ppProject->Images()[ui32Index].fZoom;
	LSREAL fImageHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight() * m_ppProject->Images()[ui32Index].fZoom;
	LSREAL fHalfWidth = fImageWidth / LSM_TWO;
	LSREAL fHalfHeight = fImageHeight / LSM_TWO;
	_fX *= fImageWidth;
	_fY *= fImageHeight;
	_fX -= fHalfWidth;
	_fY -= fHalfHeight;
	LSREAL fWinWidth = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->width();
	LSREAL fWinHeight = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->height();
	_fX += fWinWidth / LSM_TWO;
	_fY += fWinHeight / LSM_TWO;
	return CVector2( _fX,
		_fY );
}

/**
 * Gets the pixel under the given local coordinate.
 *
 * \param _fX X coordinate.
 * \param _fY Y coordinate.
 * \param _i32X Holds the returned X pixel coordinate.
 * \param _i32Y Holds the returned Y pixel coordinate.
 */
LSVOID LSE_CALL CMainOpenGlWidget::ClientToPixel( LSREAL _fX, LSREAL _fY, LSINT32 &_i32X, LSINT32 &_i32Y ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight();
	LSREAL fUvX, fUvY;
	ClientToUv( _fX, _fY, fUvX, fUvY );

	_i32X = CMathLib::Floor( fUvX * fWidth );
	_i32Y = CMathLib::Floor( fUvY * fHeight );
}

/**
 * Gets the UV coordinate under the given local coordinate.
 *
 * \param _fX X coordinate.
 * \param _fY Y coordinate.
 * \param _fRetX Holds the returned X UV coordinate.
 * \param _fRetY Holds the returned Y UV coordinate.
 */
LSVOID LSE_CALL CMainOpenGlWidget::ClientToUv( LSREAL _fX, LSREAL _fY, LSREAL &_fRetX, LSREAL &_fRetY ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight();
	
	LSREAL fLeft, fRight, fTop, fBottom;
	GetTextureBounds( fLeft, fRight, fTop, fBottom );

	_fRetX = (_fX - fLeft) / (fRight - fLeft);
	_fRetY = (_fY - fBottom) / (fTop - fBottom);
}

/**
 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 */
void CMainOpenGlWidget::paintGL() {
	glWarnError( "paintGL Uncaught" );
	CSeOpenGlWidget::paintGL();

	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		LSUINT32 ui32Index = m_ppProject->SelectedIndex();
		LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth();
		LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight();
		m_cCamera.SetOrigin( CVector2( LSM_ZERO, LSM_ZERO ) );
		m_cCamera.Update();
		CGfx::SetProjMatrix( m_cCamera.ProjMatrix() );


		
		LSG_VERTEX_BUFFER_MAP vbmMap = m_pnbvbVertices->Cur().Map( LSG_VBMA_WRITE_DISCARD );
		if ( vbmMap.pvData ) {
			LSREAL fLeft, fRight, fTop, fBottom;
			GetTextureBounds( fLeft, fRight, fTop, fBottom );

			LSSE_SPRITE_VERTEX * psvVerts = reinterpret_cast<LSSE_SPRITE_VERTEX *>(vbmMap.pvData);
			CSeOpenGlWidget::MakeQuad( psvVerts, fLeft, fTop, fRight, fBottom,
				LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
			m_pnbvbVertices->Cur().Unmap( vbmMap );

			CGfx::SetShader( &m_sShader );
			m_sShader.SetBool( m_hSrgb, m_ppProject->Images()[ui32Index].iImage.IsSRgb() );
			m_pnbvbVertices->Cur().SetStream( 0 );
			CGfx::RSSetState( m_prsRasterState );
			CGfx::OMSetDepthStencilState( m_pdssDepthState );
			CGfx::OMSetBlendState( m_pbsAlphaBlendState );
			m_psSampler->Activate( 0 );
			m_ppProject->Images()[ui32Index].st2pTexture->Activate( 0 );
			CGfx::Render();

			DrawCrosshairs();
		}
	}
}

/**
 * This virtual function is called whenever the widget has been resized. The new size is passed in _iWidth and _iHeight. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 *
 * \param _iWidth New width of the window.
 * \param _iHeight New height of the window.
 */
void CMainOpenGlWidget::resizeGL( LSINT32 _iWidth, LSINT32 _iHeight ) {
	CSeOpenGlWidget::resizeGL( _iWidth, _iHeight );
	//m_psbVertBar->setRange( 0,  );
	if ( m_psbVertBar ) {
		m_psbVertBar->setFixedHeight( _iHeight );
	}
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse move events for the widget.
 *	If mouse tracking is switched off, mouse move events only occur if a mouse button is pressed while the mouse is being moved. If mouse tracking is switched on, mouse move events occur even if no mouse button is pressed.
 *	QMouseEvent::pos() reports the position of the mouse cursor, relative to this widget. For press and release events, the position is usually the same as the position of the last mouse move event, but it might be different if the user's hand shakes. This is a feature of the underlying window system, not Qt.
 *	If you want to show a tooltip immediately, while the mouse is moving (e.g., to get the mouse coordinates with QMouseEvent::pos() and show them as a tooltip), you must first enable mouse tracking as described above. Then, to ensure that the tooltip is updated immediately, you must call QToolTip::showText() instead of setToolTip() in your implementation of mouseMoveEvent().
 */
void CMainOpenGlWidget::mouseMoveEvent( QMouseEvent * _pmeEvent ) {
	m_bDragging = false;
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		m_vMousePos.x = _pmeEvent->pos().x();
		m_vMousePos.y = _pmeEvent->pos().y();
		if ( _pmeEvent->buttons() & Qt::LeftButton ) {
			// Dragging.
			m_bDragging = true;
		}
		updateGL();
	}
	CSeOpenGlWidget::mouseMoveEvent( _pmeEvent );
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
 */
void CMainOpenGlWidget::mousePressEvent( QMouseEvent * _pmeEvent ) {
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		m_vMousePress.x = _pmeEvent->pos().x();
		m_vMousePress.y = _pmeEvent->pos().y();
		m_bDragging = (_pmeEvent->button() == Qt::LeftButton) ? true : m_bDragging;
		updateGL();
	}
	CSeOpenGlWidget::mousePressEvent( _pmeEvent );
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse release events for the widget.
 */
void CMainOpenGlWidget::mouseReleaseEvent( QMouseEvent * _pmeEvent ) {
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		m_vMouseRelease.x = _pmeEvent->pos().x();
		m_vMouseRelease.y = _pmeEvent->pos().y();
		m_bDragging = (_pmeEvent->button() == Qt::LeftButton) ? false : m_bDragging;
		updateGL();
	}
	CSeOpenGlWidget::mouseReleaseEvent( _pmeEvent );
}

/**
 * Draws the crosshairs.
 */
LSVOID LSE_CALL CMainOpenGlWidget::DrawCrosshairs() {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight();
	CGfx::SetViewMatrix( m_cCamera.ViewMatrix() );
	CGfx::SetProjMatrix( m_cCamera.ProjMatrix() );
	m_nbvbCrossVertices.Swap();

	LSG_VERTEX_BUFFER_MAP vbmMap = m_nbvbCrossVertices.Cur().Map( LSG_VBMA_WRITE_DISCARD );
	if ( vbmMap.pvData ) {
		// Convert to UV coordinates.
		LSREAL fX = m_vMousePos.x;
		LSREAL fY = m_vMousePos.y;
		LSINT32 i32PixX, i32PixY;
		ClientToPixel( fX, fY, i32PixX, i32PixY );
		LSREAL fUvX = i32PixX / fWidth;
		LSREAL fUvY = i32PixY / fHeight;

		LSREAL fLeft, fRight, fTop, fBottom;
		GetTextureBounds( fLeft, fRight, fTop, fBottom );
		LSREAL fTexLeft = fLeft, fTexRight = fRight, fTexTop = fTop, fTexBottom = fBottom;

		CVector2 vScreenPos = UvToClient( fUvX, fUvY );
		CVector2 vScreenPosTemp = UvToClient( fUvX + LSM_ONE / fWidth, fUvY + LSM_ONE / fHeight );
		LSSE_SPRITE_VERTEX * psvVerts = reinterpret_cast<LSSE_SPRITE_VERTEX *>(vbmMap.pvData);
		// Horizontal bar.
		fLeft = LSM_ZERO;
		fRight = width();
		fBottom = vScreenPos.y;
		fTop = vScreenPosTemp.y;
		ClientToUv( fLeft, fBottom, fTexLeft, fTexTop );
		ClientToUv( fRight, fTop, fTexRight, fTexBottom );
		CSeOpenGlWidget::MakeQuad( psvVerts, fLeft, fTop, fRight, fBottom,
			fTexLeft,
			fTexTop,
			fTexRight,
			fTexBottom );

		psvVerts += 6;
		// Vertical bar.
		fLeft = vScreenPos.x;
		fRight = vScreenPosTemp.x;
		fBottom = LSM_ZERO;
		fTop = height();
		ClientToUv( fLeft, fBottom, fTexLeft, fTexTop );
		ClientToUv( fRight, fTop, fTexRight, fTexBottom );
		CSeOpenGlWidget::MakeQuad( psvVerts, fLeft, fTop, fRight, fBottom,
			fTexLeft,
			fTexTop,
			fTexRight,
			fTexBottom );
		m_nbvbCrossVertices.Cur().Unmap( vbmMap );
		
		CGfx::SetShader( &m_sCrossHairShader );
		m_sCrossHairShader.SetBool( m_hSrgbCrossHair, m_ppProject->Images()[ui32Index].iImage.IsSRgb() );
		m_nbvbCrossVertices.Cur().SetStream( 0 );
		CGfx::RSSetState( m_prsRasterState );
		CGfx::OMSetDepthStencilState( m_pdssDepthState );
		CGfx::OMSetBlendState( m_pbsAlphaBlendState );
		m_psSampler->Activate( 0 );
		m_ppProject->Images()[ui32Index].st2pTexture->Activate( 0 );
		CGfx::Render();


		if ( m_bDragging ) {
			LSREAL fX, fY;
			ClientToUv( m_vMousePress.x, m_vMousePress.y, fX, fY );
			LSREAL fEndX, fEndY;
			ClientToUv( m_vMousePos.x, m_vMousePos.y, fEndX, fEndY );
			DrawBox( fX, fY, fEndX, fEndY );
		}
	}
}

/**
 * Draws a single box given the upper-left and bottom-right UV coordinates.
 *
 * \param _fLeft Left coordinate.
 * \param _fTop Top coordinate.
 * \param _fRight Right coordinate.
 * \param _fBottom Bottom coordinate.
 */
LSVOID LSE_CALL CMainOpenGlWidget::DrawBox( LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom ) {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight();
	CGfx::SetViewMatrix( m_cCamera.ViewMatrix() );
	CGfx::SetProjMatrix( m_cCamera.ProjMatrix() );
	m_nbvbBoxVertices.Swap();

	LSREAL fLeft = CStd::Min( _fLeft, _fRight );
	LSREAL fRight = CStd::Max( _fLeft, _fRight );
	LSREAL fTop = CStd::Min( _fTop, _fBottom );
	LSREAL fBottom = CStd::Max( _fTop, _fBottom );

	// Include the right and bottom pixels.
	fRight += LSM_ONE / fWidth;
	fBottom += LSM_ONE / fHeight;

	LSG_VERTEX_BUFFER_MAP vbmMap = m_nbvbBoxVertices.Cur().Map( LSG_VBMA_WRITE_DISCARD );
	if ( vbmMap.pvData ) {
		LSINT32 i32PixX, i32PixY;
		CVector2 vOuter0 = UvToClient( fLeft, fTop );
		ClientToPixel( vOuter0.x, vOuter0.y, i32PixX, i32PixY );
		LSREAL fUvX = i32PixX / fWidth;
		LSREAL fUvY = i32PixY / fHeight;
		vOuter0 = UvToClient( fUvX, fUvY );
		CVector2 vInner0 = UvToClient( fUvX + LSM_ONE / fWidth, fUvY + LSM_ONE / fHeight );


		CVector2 vOuter1 = UvToClient( fRight, fBottom );
		ClientToPixel( vOuter1.x, vOuter1.y, i32PixX, i32PixY );
		fUvX = i32PixX / fWidth;
		fUvY = i32PixY / fHeight;
		vOuter1 = UvToClient( fUvX, fUvY );
		CVector2 vInner1 = UvToClient( fUvX - LSM_ONE / fWidth, fUvY - LSM_ONE / fHeight );

		// Top box.
		LSSE_SPRITE_VERTEX * psvVerts = reinterpret_cast<LSSE_SPRITE_VERTEX *>(vbmMap.pvData);
		CSeOpenGlWidget::MakeQuad( psvVerts, vOuter0.x, vOuter0.y, vOuter1.x, vInner0.y,
			LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
		psvVerts += 6;

		// Bottom box.
		CSeOpenGlWidget::MakeQuad( psvVerts, vOuter0.x, vInner1.y, vOuter1.x, vOuter1.y,
			LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
		psvVerts += 6;

		// Left box.
		CSeOpenGlWidget::MakeQuad( psvVerts, vOuter0.x, vOuter0.y, vInner0.x, vOuter1.y,
			LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
		psvVerts += 6;

		// Right box.
		CSeOpenGlWidget::MakeQuad( psvVerts, vInner1.x, vOuter0.y, vOuter1.x, vOuter1.y,
			LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
		psvVerts += 6;

		m_nbvbBoxVertices.Cur().Unmap( vbmMap );

		CGfx::SetShader( &m_sBoxShader );
		//m_sBoxShader.SetBool( m_hSrgbCrossHair, m_ppProject->Images()[ui32Index].iImage.IsSRgb() );
		m_nbvbBoxVertices.Cur().SetStream( 0 );
		CGfx::RSSetState( m_prsRasterState );
		CGfx::OMSetDepthStencilState( m_pdssDepthState );
		CGfx::OMSetBlendState( m_pbsAlphaBlendState );
		m_psSampler->Activate( 0 );
		m_ppProject->Images()[ui32Index].st2pTexture->Activate( 0 );
		CGfx::Render();
	}
}

/**
 * Gets the bounding box for the texture.
 *
 * \param _fLeft The returned left coordinate.
 * \param _fRight The returned right coordinate.
 * \param _fTop The returned top coordinate.
 * \param _fBottom The returned bottom coordinate.
 */
LSVOID LSE_CALL CMainOpenGlWidget::GetTextureBounds( LSREAL &_fLeft, LSREAL &_fRight, LSREAL &_fTop, LSREAL &_fBottom ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].iImage.GetWidth() * m_ppProject->GetZoom();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].iImage.GetHeight() * m_ppProject->GetZoom();
	_fLeft = CMathLib::Floor( fWidth / -LSM_TWO + (width() / LSM_TWO) );
	_fRight = CMathLib::Floor( fWidth / LSM_TWO + (width() / LSM_TWO) );
	_fTop = CMathLib::Floor( fHeight / LSM_TWO + (height() / LSM_TWO) );
	_fBottom = CMathLib::Floor( fHeight / -LSM_TWO + (height() / LSM_TWO) );
}

/**
 * Converts a UV coordinate into a pixel location on the screen.
 *
 * \param _fX X coordinate.
 * \param _fX Y coordinate.
 * \return Returns the converted coordinate.
 */
CVector2 LSE_CALL CMainOpenGlWidget::UvToClient( LSREAL _fX, LSREAL _fY ) const {
	LSREAL fLeft, fRight, fTop, fBottom;
	GetTextureBounds( fLeft, fRight, fTop, fBottom );
	_fX *= CMathLib::Abs( fLeft - fRight );
	_fY *= CMathLib::Abs( fTop - fBottom );
	return CVector2( _fX + fLeft, _fY + fBottom );
}
