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
* Description: The sprite-editor project class.  Handles a single sprite-editor project.
*/

#include "LSSEProject.h"
#include "../LSSESpriteEditor.h"
#include "../Widgets/LSSESeOpenGlWidget.h"
#include <QMessageBox>
#include <QObject>
#include <QtWidgets/QFileDialog>


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Converts m_pwMainWindow to a LSSpriteEditor pointer for convenience. */
#define MainWindow()		static_cast<LSSpriteEditor *>(m_pwMainWindow)

// == Various constructors.
LSE_CALLCTOR CProject::CProject( QWidget * _pwParent ) :
	m_pwMainWindow( _pwParent ),
	m_ui32Selected( 0 ) {
}
LSE_CALLCTOR CProject::~CProject() {
}

// == Functions.
/**
 * Saves the project to the project’s path.
 *
 * \return Returns true if the project was saved.
 */
LSBOOL LSE_CALL CProject::SaveProject() {
	if ( m_sPath.isNull() || m_sPath.isEmpty() ) {
		return SaveProjectAs();
	}
	MainWindow()->UpdateAll();
	return true;
}

/**
 * Asks for a path and saves the project there.
 *
 * \return Returns true if the project was saved.
 */
LSBOOL LSE_CALL CProject::SaveProjectAs() {
	QString strFile = QFileDialog::getSaveFileName( m_pwMainWindow, QObject::tr( "Save Project As" ),
		m_sCurFolder,
		QObject::tr( "L. Spiro Sprite Project File (*.lssproj)" ) );
	if ( strFile.isNull() ) { 
		return false;
	}
	m_sPath = strFile;

	QByteArray baPath = strFile.toUtf8();
	m_sCurFolder = CFileLib::NoFile( baPath.data() );

	return SaveProject();
}

/**
 * Adds an image to the project.
 */
LSVOID LSE_CALL CProject::AddImage() {
	QString strFile = QFileDialog::getOpenFileName( m_pwMainWindow, QObject::tr( "Please select a texture file to load..." ),
		m_sCurFolder,
		QObject::tr( "Image Data (*.bmp *.dds *.gif *.hdr *.jpeg *.jpg *.png *.tga *.tif *.tiff)" ) );
	if ( strFile.isNull() ) {
		return;
	}
	{
		LSSE_SPRITE_SHEET ssItem;
		if ( !m_liImages.Push( ssItem ) ) {
			QMessageBox::warning( NULL, QObject::tr( "Error" ), QObject::tr( "Failed to add image (out of memory)." ) );
			return;
		}
	}

	LSSE_SPRITE_SHEET & ssThisItem = m_liImages[m_liImages.Length()-1];
	ssThisItem.fZoom = LSM_ONE;
	ssThisItem.pfExportFormat = LSI_PF_ANY;	// No change.
	ssThisItem.sPath.Set( strFile.toUtf8().data(), strFile.toUtf8().size() );
	if ( !ssThisItem.iImage.LoadFile( strFile.toUtf8().data() ) ) {
		QMessageBox::warning( NULL, QObject::tr( "Error" ), QObject::tr( "Image failed to load (invalid format?)." ) );
		m_liImages.Pop();
		return;
	}
	ssThisItem.iImage.SetColorSpace( LSI_G_sRGB );
	if ( !ssThisItem.st2pTexture.New() ) {
		QMessageBox::warning( NULL, QObject::tr( "Error" ), QObject::tr( "Failed to create image texture (out of memory)." ) );
		m_liImages.Pop();
		return;
	}
	CSeOpenGlWidget::MakeCurrent();
	if ( !ssThisItem.st2pTexture->CreateTexture( &ssThisItem.iImage ) ) {
		ssThisItem.iImage.SetColorSpace( LSI_G_LINEAR );
		if ( !ssThisItem.st2pTexture->CreateTexture( &ssThisItem.iImage ) ) {
			QMessageBox::warning( NULL, QObject::tr( "Error" ), QObject::tr( "Failed to create image texture." ) );
			m_liImages.Pop();
			return;
		}
	}

	// Try to generate a thumbnail.
	ssThisItem.st2pTextureThumb = ssThisItem.st2pTexture;
	{
		CImage iThumb;
		LSREAL fSize = CStd::Max<LSREAL>( ssThisItem.iImage.GetWidth(), ssThisItem.iImage.GetHeight() );
		LSREAL fRatio = 120.0f / fSize;
		if ( fRatio < LSM_ONE ) {
			if ( ssThisItem.iImage.Resample( ssThisItem.iImage.GetWidth() * fRatio, ssThisItem.iImage.GetHeight() * fRatio, LSI_F_KAISER_FILTER,
				iThumb, ssThisItem.iImage.IsSRgb() ? -2.2f : 1.0f, CResampler::LSI_AM_CLAMP ) ) {
				CSharedTexture2dPtr st2pThumb;
				if ( st2pThumb.New() ) {
					iThumb.SetColorSpace( ssThisItem.iImage.IsSRgb() ? LSI_G_sRGB : LSI_G_LINEAR );
					if ( st2pThumb->CreateTexture( &iThumb ) ) {
						ssThisItem.st2pTextureThumb = st2pThumb;
					}
				}
			}
		}
	}

	if ( m_liImages.Length() ) {
		SetSelectionIndex( m_liImages.Length() - 1 );
	}
}

/**
 * Delets an image by index.
 *
 * \param _ui32Index Index of the image to delete.
 */
LSVOID LSE_CALL CProject::DeleteImage( LSUINT32 _ui32Index ) {
	if ( _ui32Index < m_liImages.Length() ) {
		m_liImages.Remove( _ui32Index );
	}
	if ( m_liImages.Length() ) {
		m_ui32Selected = CStd::Clamp<LSUINT32>( m_ui32Selected, 0, m_liImages.Length() - 1 );
	}
}

/**
 * Gets the maximum zoom level for an image.  Based on maximum backbuffer size.
 *
 * \param _ui32Index The image for which to get the maximum zoom.
 * \return Returns the maximum zoom for the given image.
 */
LSREAL LSE_CALL CProject::MaxZoom( LSUINT32 _ui32Index ) const {
	if ( _ui32Index < m_liImages.Length() ) {
		return (CStd::Min<LSREAL>( CGfx::GetMetrics().ui32MaxRenderTargetSize * LSREAL( 0.5 ), LSG_VIEWPORT_MAX )) / CStd::Max<LSREAL>( m_liImages[_ui32Index].iImage.GetWidth(), m_liImages[_ui32Index].iImage.GetHeight() );
	}
	return LSM_ONE;
}

/**
 * Zooms in on a given image.
 *
 * \param _ui32Index Index of the image into which to zoom.
 */
LSVOID LSE_CALL CProject::ZoomIn( LSUINT32 _ui32Index ) {
	if ( _ui32Index < m_liImages.Length() ) {
		m_liImages[_ui32Index].fZoom = LSUINT32( m_liImages[_ui32Index].fZoom + LSM_ONE );
		m_liImages[_ui32Index].fZoom = CStd::Min( m_liImages[_ui32Index].fZoom, MaxZoom( _ui32Index ) );
		MainWindow()->UpdateAll();
	}
}

/**
 * Zooms out on a given image.
 *
 * \param _ui32Index Index of the image into which to zoom out.
 */
LSVOID LSE_CALL CProject::ZoomOut( LSUINT32 _ui32Index ) {
	if ( _ui32Index < m_liImages.Length() ) {
		if ( m_liImages[_ui32Index].fZoom == LSUINT32( m_liImages[_ui32Index].fZoom ) ) {
			m_liImages[_ui32Index].fZoom = LSUINT32( m_liImages[_ui32Index].fZoom - LSM_ONE );
		}
		else {
			m_liImages[_ui32Index].fZoom = LSUINT32( m_liImages[_ui32Index].fZoom );
		}
		m_liImages[_ui32Index].fZoom = CStd::Max( m_liImages[_ui32Index].fZoom, LSM_ONE );
		MainWindow()->UpdateAll();
	}
}

/**
 * Sets the current zoom.
 *
 * \param _ui32Index Index of the image into which to zoom.
 * \param _fZoom Zoom to set.  Zoom is automatically clamped.
 */
LSVOID LSE_CALL CProject::Zoom( LSUINT32 _ui32Index, LSREAL _fZoom ) {
	if ( _ui32Index < m_liImages.Length() ) {
		m_liImages[_ui32Index].fZoom = CStd::Clamp<LSREAL>( _fZoom, LSM_ONE, MaxZoom( _ui32Index ) );
		MainWindow()->UpdateAll();
	}
}

#undef MainWindow
