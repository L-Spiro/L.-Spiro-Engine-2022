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
 * Description: The navigation view.
 */

#include "LSSENaviOpenGlWidget.h"
#include "../Project/LSSEProject.h"
#include <QMouseEvent>
#include <QScrollBar>


// == Various constructors.
LSE_CALLCTOR CNaviOpenGlWidget::CNaviOpenGlWidget( QWidget * _pwParent ) :
	CSeOpenGlWidget( _pwParent ),
	m_ppProject( NULL ),
	m_hSrgb( NULL ),
	m_hBounds( NULL ) {

	static const LSCHAR * pcVs = //"uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"	
		"void Main( in vec2 _vInPosition : POSITION0,\r\n"
		"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
		"	out vec4 _vOutPosition : POSITION0,\r\n"
		"	out vec2 _vOutTexCoord0 : TEXCOORD0,\r\n"
		"	out vec2 _vOutViewPos : TEXCOORD1 ) {\r\n"
		//"	_vOutPosition = mul( g_mModelViewProjMatrix, vec4( _vInPosition.x, _vInPosition.y, 1.0, 1.0f ) );\r\n"
		"	_vOutViewPos = vec2( _vInPosition.x, -1.0 * _vInPosition.y );\r\n"
		"	_vOutPosition = vec4( _vInPosition.x, _vInPosition.y, 0.1, 1.0f );\r\n"
		"	_vOutTexCoord0 = _vInTexCoord0;\r\n"
		"}\r\n";
	CString sError;
	m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcVs), CStd::StrLen( pcVs ), NULL, LSG_ST_VERTEX, sError );
	static const LSCHAR * pcPs = "uniform Texture2d g_tTex2d0 : TEXUNIT0;\r\n"
		"uniform Sampler g_sSampler0 : TEXUNIT0;\r\n"
		"uniform bool g_bSelected;\r\n"
		"uniform bool g_bSrgb;\r\n"
		"uniform vec4 g_vBounds;\r\n"
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
		"	if ( _vInViewPos.x < g_vBounds.x || _vInViewPos.x > g_vBounds.z || _vInViewPos.y < g_vBounds.y || _vInViewPos.y > g_vBounds.w ) {\r\n"
		"		float fVal = saturate( _vOutColor.x * 0.212639005871510 + _vOutColor.y * 0.715168678767756 + _vOutColor.z * 0.072192315360734 );\r\n"
		"		_vOutColor.xyz = vec3( fVal * 0.75, fVal * 0.5, fVal * 0.5 );\r\n"
		"	}\r\n"
		"}\r\n";
	m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcPs), CStd::StrLen( pcPs ), NULL, LSG_ST_PIXEL, sError );
	m_sShader.Compile( sError );
	m_hSrgb = m_sShader.GetConstantByName( "g_bSrgb" );
	m_hBounds = m_sShader.GetConstantByName( "g_vBounds" );
}

// == Functions.
/**
 * Sets the project pointer.
 *
 * \param _ppProject The project pointer.
 */
LSVOID LSE_CALL CNaviOpenGlWidget::SetProject( CProject * _ppProject ) {
	m_ppProject = _ppProject;
}

/**
 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 */
void CNaviOpenGlWidget::paintGL() {
	setGeometry( QRect( 0, 0, 120, 120 ) );
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() && LSSpriteEditor::GetMainWindow() ) {
		LSUINT32 ui32Index = m_ppProject->SelectedIndex();
		//m_cCamera.SetWidthHeight( fWidth, fHeight );
		CSeOpenGlWidget::paintGL();

		QRegion rRegion = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->visibleRegion();
		LSREAL fWinX = rRegion.boundingRect().x();
		LSREAL fWinY = rRegion.boundingRect().y();
		LSREAL fWinR = rRegion.boundingRect().width() + fWinX;
		LSREAL fWinB = rRegion.boundingRect().height() + fWinY;

		CVector2 vXy = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->ScreenToTexture( fWinX, fWinY );
		CVector2 vRb = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->ScreenToTexture( fWinR, fWinB );
		CVector4 vBounds( vXy.x, vXy.y, vRb.x, vRb.y );
		// Convert to [-1..1].
		vBounds *= LSM_TWO;
		vBounds -= CVector4( LSM_ONE, LSM_ONE, LSM_ONE, LSM_ONE );
		
		LSG_VERTEX_BUFFER_MAP vbmMap = m_pnbvbVertices->Cur().Map( LSG_VBMA_WRITE_DISCARD );
		if ( vbmMap.pvData ) {
			LSREAL fLeft, fTop, fRight, fBottom;
			GetImageBoundsNdc( fLeft, fTop, fRight, fBottom );
			vBounds.x *= CMathLib::Abs( fLeft );
			vBounds.y *= CMathLib::Abs( fTop );
			vBounds.z *= CMathLib::Abs( fRight );
			vBounds.w *= CMathLib::Abs( fBottom );
			LSSE_SPRITE_VERTEX * psvVerts = reinterpret_cast<LSSE_SPRITE_VERTEX *>(vbmMap.pvData);
			CSeOpenGlWidget::MakeQuad( psvVerts, fLeft, fTop, fRight, fBottom,
				LSM_ZERO, LSM_ONE, LSM_ONE, LSM_ZERO );
			m_pnbvbVertices->Cur().Unmap( vbmMap );
			CGfx::SetShader( &m_sShader );
			//m_sShader.SetBool( m_hShaderSelected, ui32Index == ui32Index );
			m_sShader.SetBool( m_hSrgb, m_ppProject->Images()[ui32Index].iImage.IsSRgb() );
			m_sShader.SetVector4( m_hBounds, vBounds );
			m_pnbvbVertices->Cur().SetStream( 0 );
			CGfx::RSSetState( m_prsRasterState );
			CGfx::OMSetDepthStencilState( m_pdssDepthState );
			CGfx::OMSetBlendState( m_pbsAlphaBlendState );
			m_psSampler->Activate( 0 );
			m_ppProject->Images()[ui32Index].st2pTextureThumb->Activate( 0 );
			CGfx::Render();
		}
	}
	else {
		CSeOpenGlWidget::paintGL();
	}
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
 */
void CNaviOpenGlWidget::mousePressEvent( QMouseEvent * _pmeEvent ) {
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		PutIntoView( _pmeEvent->x(), _pmeEvent->y() );

		LSSpriteEditor::GetMainWindow()->UpdateAll();
	}
	CSeOpenGlWidget::mousePressEvent( _pmeEvent );
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse move events for the widget.
 *	If mouse tracking is switched off, mouse move events only occur if a mouse button is pressed while the mouse is being moved. If mouse tracking is switched on, mouse move events occur even if no mouse button is pressed.
 *	QMouseEvent::pos() reports the position of the mouse cursor, relative to this widget. For press and release events, the position is usually the same as the position of the last mouse move event, but it might be different if the user's hand shakes. This is a feature of the underlying window system, not Qt.
 *	If you want to show a tooltip immediately, while the mouse is moving (e.g., to get the mouse coordinates with QMouseEvent::pos() and show them as a tooltip), you must first enable mouse tracking as described above. Then, to ensure that the tooltip is updated immediately, you must call QToolTip::showText() instead of setToolTip() in your implementation of mouseMoveEvent().
 */
void CNaviOpenGlWidget::mouseMoveEvent( QMouseEvent * _pmeEvent ) {
	if ( m_ppProject && m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		PutIntoView( _pmeEvent->x(), _pmeEvent->y() );

		LSSpriteEditor::GetMainWindow()->UpdateAll();
	}
	CSeOpenGlWidget::mouseMoveEvent( _pmeEvent );
}

/**
 * Gets the box around the texture for drawing.
 *
 * \param _fLeft The returned left coordinate of the box in NDC.
 * \param _fTop The returned left coordinate of the box in NDC.
 * \param _fRight The returned left coordinate of the box in NDC.
 * \param _fBottom The returned left coordinate of the box in NDC.
 */
LSVOID LSE_CALL CNaviOpenGlWidget::GetImageBoundsNdc( LSREAL &_fLeft, LSREAL &_fTop, LSREAL &_fRight, LSREAL &_fBottom ) const {
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	// It is actually based off the thumb dimensions since that is what we display.
	LSREAL fWidth = m_ppProject->Images()[ui32Index].st2pTextureThumb->GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].st2pTextureThumb->GetHeight();

	LSREAL fSize = CStd::Max( fWidth, fHeight );
	_fLeft = fWidth / -fSize;
	_fRight = fWidth / fSize;
	_fTop = fHeight / fSize;
	_fBottom = fHeight / -fSize;
}

/**
 * Puts the given coordinates in view by scrolling the main view.
 *
 * \param _iX The X coordinate that should be put into the center of the view on the main widget.
 * \param _iY The Y coordinate that should be put into the center of the view on the main widget.
 */
LSVOID LSE_CALL CNaviOpenGlWidget::PutIntoView( int _iX, int _iY ) const {
	// Convert to texture coordinates.
	LSUINT32 ui32Index = m_ppProject->SelectedIndex();
	LSREAL fWidth = m_ppProject->Images()[ui32Index].st2pTextureThumb->GetWidth();
	LSREAL fHeight = m_ppProject->Images()[ui32Index].st2pTextureThumb->GetHeight();

	LSREAL fLeft, fTop, fRight, fBottom;
	GetImageBoundsNdc( fLeft, fBottom, fRight, fTop );
	// NDC to [0..1];
	fLeft = (fLeft + LSM_ONE) / LSM_TWO;
	fTop = (fTop + LSM_ONE) / LSM_TWO;
	fRight = (fRight + LSM_ONE) / LSM_TWO;
	fBottom = (fBottom + LSM_ONE) / LSM_TWO;
	// [0..1] to widget size.
	fLeft *= width();
	fTop *= height();
	fRight *= width();
	fBottom *= height();

	// UV coordinates.
	LSREAL fFinalX = (_iX - fLeft) / fWidth;
	LSREAL fFinalY = (_iY - fTop) / fHeight;
	// Widget coordinates (if scrolling is enabled).
	fFinalX *= LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->width();
	fFinalY *= LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->height();
	// The click represents where the center should be.  Offset the scroll amount by half the visible area.
	QRegion rReg = LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->visibleRegion();
	int iRegW = rReg.boundingRect().width();
	int iRegH = rReg.boundingRect().height();
	fFinalX -= iRegW / LSM_TWO;
	fFinalY -= iRegH / LSM_TWO;
	LSSpriteEditor::GetMainWindow()->ui.scrollArea->horizontalScrollBar()->setSliderPosition( fFinalX );
	LSSpriteEditor::GetMainWindow()->ui.scrollArea->verticalScrollBar()->setSliderPosition( fFinalY );
	/*LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->m_psbHorBar->setSliderPosition( fFinalX );
	LSSpriteEditor::GetMainWindow()->ui.mainOpenGLWidget->m_psbVertBar->setSliderPosition( fFinalY );*/
}
