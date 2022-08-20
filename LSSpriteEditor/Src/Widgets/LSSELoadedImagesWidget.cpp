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
* Description: Draws the loaded images with sprite-box overlays.
*/

#include "LSSELoadedImagesWidget.h"
#include "../Project/LSSEProject.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>


// == Various constructors.
LSE_CALLCTOR CLoadedImagesWidget::CLoadedImagesWidget( QWidget * _pwParent ) :
	CSeOpenGlWidget( _pwParent ),
	m_ppProject( NULL ),
	m_ui32Index( 0 ),
	m_hShaderSelected( NULL ),
	m_hSrgb( NULL ) {

	static const LSCHAR * pcVs = //"uniform float[4][4]	g_mModelViewProjMatrix : WORLDVIEWPROJ;\r\n"	
		"void Main( in vec2 _vInPosition : POSITION0,\r\n"
		"	in vec2 _vInTexCoord0 : TEXCOORD0,\r\n"
		"	out vec4 _vOutPosition : POSITION0,\r\n"
		"	out vec2 _vOutTexCoord0 : TEXCOORD0	) {\r\n"
		//"	_vOutPosition = mul( g_mModelViewProjMatrix, vec4( _vInPosition.x, _vInPosition.y, 1.0, 1.0f ) );\r\n"
		"	_vOutPosition = vec4( _vInPosition.x, _vInPosition.y, 0.1, 1.0f );\r\n"
		"	_vOutTexCoord0 = _vInTexCoord0;\r\n"
		"}\r\n";
	CString sError;
	m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcVs), CStd::StrLen( pcVs ), NULL, LSG_ST_VERTEX, sError );
	static const LSCHAR * pcPs = "uniform Texture2d g_tTex2d0 : TEXUNIT0;\r\n"
		"uniform Sampler g_sSampler0 : TEXUNIT0;\r\n"
		"uniform bool g_bSelected;\r\n"
		"uniform bool g_bSrgb;\r\n"
		"#define LSE_LINEAR2SRGB( VAL )		(VAL) <= 0.0031308 ? (VAL) * 12.92 : 1.055 * pow( (VAL), 1.0f / 2.4 ) - 0.055\r\n"
		"void Main( out vec4 _vOutColor : COLOR,\r\n"
		"	in vec4 _vInPosition : POSITION0,\r\n"
		"	in vec2 _vInTexCoord0 : TEXCOORD0 ) {\r\n"
		"	_vOutColor = tex2d( g_tTex2d0, g_sSampler0, _vInTexCoord0 );\r\n"
		"	if ( g_bSrgb ) {\r\n"
		"		_vOutColor.x = LSE_LINEAR2SRGB( _vOutColor.x );\r\n"
		"		_vOutColor.y = LSE_LINEAR2SRGB( _vOutColor.y );\r\n"
		"		_vOutColor.z = LSE_LINEAR2SRGB( _vOutColor.z );\r\n"
		"	}\r\n"
		"	if ( !g_bSelected ) {\r\n"
		"		float fVal = saturate( _vOutColor.x * 0.212639005871510 + _vOutColor.y * 0.715168678767756 + _vOutColor.z * 0.072192315360734 );\r\n"
		"		_vOutColor.xyz = vec3( fVal * 0.5, fVal * 0.5, fVal * 0.5 );\r\n"
		"	}\r\n"
		"}\r\n";
	m_sShader.SetShaderFromMemory( reinterpret_cast<const LSUINT8 *>(pcPs), CStd::StrLen( pcPs ), NULL, LSG_ST_PIXEL, sError );
	m_sShader.Compile( sError );
	m_hShaderSelected = m_sShader.GetConstantByName( "g_bSelected" );
	m_hSrgb = m_sShader.GetConstantByName( "g_bSrgb" );

	m_paDeletAction = LSENEW QAction( tr( "&Delete" ), this );
	m_paDeletAction->setStatusTip( tr( "Deletes this image from the project." ) );
	connect( m_paDeletAction, SIGNAL( triggered() ), LSSpriteEditor::GetMainWindow(), SLOT( DeleteImage() ) );
}
LSE_CALLCTOR CLoadedImagesWidget::~CLoadedImagesWidget() {
	LSEDELETE m_paDeletAction;
}

// == Functions.
/**
 * Sets the project pointer.
 *
 * \param _ppProject The project pointer.
 */
LSVOID LSE_CALL CLoadedImagesWidget::SetProject( CProject * _ppProject ) {
	m_ppProject = _ppProject;
}

/**
 * Sets the image index inside the array of loaded images.
 *
 * \param _ui32Index Index into the array of loaded images.
 */
LSVOID LSE_CALL CLoadedImagesWidget::SetIndex( LSUINT32 _ui32Index ) {
	m_ui32Index = _ui32Index;
}

/**
 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
 *	There is no need to call makeCurrent() because this has already been done when this function is called.
 */
void CLoadedImagesWidget::paintGL() {
	setGeometry( QRect( 0, 0, 120, 120 ) );
	if ( m_ppProject && m_ui32Index < m_ppProject->Images().Length() ) {
		LSREAL fWidth = m_ppProject->Images()[m_ui32Index].st2pTextureThumb->GetWidth();
		LSREAL fHeight = m_ppProject->Images()[m_ui32Index].st2pTextureThumb->GetHeight();
		//m_cCamera.SetWidthHeight( fWidth, fHeight );
		CSeOpenGlWidget::paintGL();
		
		LSG_VERTEX_BUFFER_MAP vbmMap = m_pnbvbVertices->Cur().Map( LSG_VBMA_WRITE_DISCARD );
		if ( vbmMap.pvData ) {
			LSREAL fSize = CStd::Max( fWidth, fHeight );
			/*LSREAL fLeft = fWidth / -LSM_TWO;
			LSREAL fRight = fWidth / LSM_TWO;
			LSREAL fTop = fHeight / -LSM_TWO;
			LSREAL fBottom = fHeight / LSM_TWO;*/
			LSREAL fLeft = fWidth / -fSize;
			LSREAL fRight = fWidth / fSize;
			LSREAL fTop = fHeight / -fSize;
			LSREAL fBottom = fHeight / fSize;
			LSSE_SPRITE_VERTEX * psvVerts = reinterpret_cast<LSSE_SPRITE_VERTEX *>(vbmMap.pvData);
			CSeOpenGlWidget::MakeQuad( psvVerts, fLeft, fTop, fRight, fBottom,
				LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ONE );
			m_pnbvbVertices->Cur().Unmap( vbmMap );
			CGfx::SetShader( &m_sShader );
			m_sShader.SetBool( m_hShaderSelected, m_ppProject->SelectedIndex() == m_ui32Index );
			m_sShader.SetBool( m_hSrgb, m_ppProject->Images()[m_ui32Index].iImage.IsSRgb() );
			m_pnbvbVertices->Cur().SetStream( 0 );
			CGfx::RSSetState( m_prsRasterState );
			CGfx::OMSetDepthStencilState( m_pdssDepthState );
			CGfx::OMSetBlendState( m_pbsAlphaBlendState );
			m_psSampler->Activate( 0 );
			m_ppProject->Images()[m_ui32Index].st2pTextureThumb->Activate( 0 );
			CGfx::Render();
		}
	}
	else {
		CSeOpenGlWidget::paintGL();
	}
}

/**
 * This event handler, for event _pcmeEvent, can be reimplemented in a subclass to receive widget context menu events.
 *	The handler is called when the widget's contextMenuPolicy is Qt::DefaultContextMenu.
 *
 * \param _pcmeEvent The context menu event data.
 */
void CLoadedImagesWidget::contextMenuEvent( QContextMenuEvent * _pcmeEvent ) {
	LSSpriteEditor::GetMainWindow()->SetDeleteIndex( m_ui32Index );
	QMenu mMenu( LSSpriteEditor::GetMainWindow() );
	mMenu.addAction( m_paDeletAction );
	mMenu.exec( _pcmeEvent->globalPos() );
}

/**
 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
 */
void CLoadedImagesWidget::mousePressEvent( QMouseEvent * _pmeEvent ) {
	if ( m_ppProject ) {
		m_ppProject->SetSelectionIndex( m_ui32Index );
		LSSpriteEditor::GetMainWindow()->UpdateAll();
	}
	CSeOpenGlWidget::mousePressEvent( _pmeEvent );
}
