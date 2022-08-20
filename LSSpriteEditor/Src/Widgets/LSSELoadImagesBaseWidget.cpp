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
* Description: A widget for displaying a loaded texture, the texture path, etc.
*/

#include "LSSELoadImagesBaseWidget.h"
#include "../Project/LSSEProject.h"


// == Various constructors.
LSE_CALLCTOR CLoadImagesBaseWidget::CLoadImagesBaseWidget( QWidget * _pwParent ) :
	QWidget( _pwParent ),
	m_ppProject( NULL ),
	m_ui32Index( 0 ) {
	ui.setupUi( this );
	
}
LSE_CALLCTOR CLoadImagesBaseWidget::~CLoadImagesBaseWidget() {
}

// == Functions.
/**
 * Sets the project pointer.
 *
 * \param _ppProject The project pointer.
 */
LSVOID LSE_CALL CLoadImagesBaseWidget::SetProject( CProject * _ppProject ) {
	m_ppProject = _ppProject;
	ui.openGLWidget->SetProject( _ppProject );
}

/**
 * Sets the image index inside the array of loaded images.
 *
 * \param _ui32Index Index into the array of loaded images.
 */
LSVOID LSE_CALL CLoadImagesBaseWidget::SetIndex( LSUINT32 _ui32Index ) {
	static const int iOrigHeight = height();
	static const int iOrigImageFileHeight = ui.imageFile->height();
	static const int iOrigExportY = ui.groupBox->y();
	static const int iOrigComboY = ui.comboBox->y();
	m_ui32Index = _ui32Index;
	ui.openGLWidget->SetIndex( _ui32Index );
	ui.openGLWidget->setVisible( true );
	ui.openGLWidget->updateGL();
	// Set the name etc. of the label.
	LSCHAR szBuffer[LSF_MAX_PATH];
	CStd::StrCpy( szBuffer, m_ppProject->Images()[_ui32Index].sPath.CStr() );
	ui.imageFile->setText( CFileLib::NoPath( szBuffer ) );
	QSize sSize = ui.imageFile->sizeHint();
	int iHeightChange = iOrigImageFileHeight - sSize.height();
	// Decrease the height by this much.
	ui.groupBox->setGeometry( ui.groupBox->x(), iOrigExportY - iHeightChange, ui.groupBox->width(), ui.groupBox->height() );
	ui.comboBox->setGeometry( ui.comboBox->x(), iOrigComboY - iHeightChange, ui.comboBox->width(), ui.comboBox->height() );
	setMinimumHeight( iOrigHeight - iHeightChange );
	setMaximumHeight( iOrigHeight - iHeightChange );
	setGeometry( x(), y(), width(), iOrigHeight - iHeightChange );

	ui.imageFile->setToolTip( m_ppProject->Images()[_ui32Index].sPath.CStr() );
	ui.imageFile->setStatusTip( m_ppProject->Images()[_ui32Index].sPath.CStr() );

	ui.openGLWidget->setToolTip( m_ppProject->Images()[_ui32Index].sPath.CStr() );
	ui.openGLWidget->setStatusTip( m_ppProject->Images()[_ui32Index].sPath.CStr() );	
}

/**
 * Repaints the OpenGL area.
 */
LSVOID LSE_CALL CLoadImagesBaseWidget::UpdateGl() {
	if ( ui.openGLWidget ) {
		ui.openGLWidget->updateGL();
	}
}
