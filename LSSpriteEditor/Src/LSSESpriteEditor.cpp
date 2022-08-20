#include "LSEEngine.h"
#include "LSSESpriteEditor.h"
#include "Project/LSSEProject.h"
#include <QScrollBar> 

// == Members.
/** A static copy of this object. */
LSSpriteEditor * LSSpriteEditor::m_pseEditor = NULL;

LSSpriteEditor::LSSpriteEditor( QWidget * _pwParent ) :
	QMainWindow( _pwParent ),
	m_bUpdating( false ),
	m_dmDrawMode( LSSE_DM_NORMAL ) {
	m_ppProject = LSENEW CProject( this );




	ui.setupUi( this );
	m_pseEditor = this;
	ui.mainOpenGLWidget->SetProject( m_ppProject );
	ui.naviOpenGLWidget->SetProject( m_ppProject );
	//setStyleSheet(QStringLiteral(""));
	ConnectSlots();
	UpdateAll();

	statusBar()->showMessage( tr( "Ready" ) );
}

LSSpriteEditor::~LSSpriteEditor() {
	for ( LSUINT32 I = m_vLoadedImages.Length(); I--; ) {
		LSEDELETE m_vLoadedImages[I];
	}
	m_vLoadedImages.Reset();
	ui.naviOpenGLWidget->SetProject( NULL );
	ui.mainOpenGLWidget->SetProject( NULL );

	LSEDELETE m_ppProject;
	m_ppProject = NULL;
	m_pseEditor = NULL;
}

// == Functions.
/**
 * Redraws all OpenGL windows.
 */
LSVOID LSE_CALL LSSpriteEditor::UpdateAll() {
	m_bUpdating = true;
	if ( m_ppProject->SelectedIndex() < m_ppProject->Images().Length() ) {
		CImage & iImage = m_ppProject->Images()[m_ppProject->SelectedIndex()].iImage;
		LSREAL fW = iImage.GetWidth() * m_ppProject->Images()[m_ppProject->SelectedIndex()].fZoom;
		LSREAL fH = iImage.GetHeight() * m_ppProject->Images()[m_ppProject->SelectedIndex()].fZoom;
		/*ui.mainOpenGLWidget->m_psbHorBar->setMaximum( fW );
		ui.mainOpenGLWidget->m_psbVertBar->setMaximum( fH );*/
		ui.mainOpenGLWidget->setMinimumSize( fW,
			fH );
	}
	else {
		ui.mainOpenGLWidget->setMinimumSize( 0, 0 );
	}
	ui.mainOpenGLWidget->updateGL();
	// Zoom buttons.
	if ( m_ppProject->SelectedIndex() >= m_ppProject->Images().Length() ) {
		ui.actionZoom_In->setEnabled( false );
		ui.actionZoom_Out->setEnabled( false );
		ui.naviSlider->setEnabled( false );
		ui.naviSlider->setRange( 1, 1 );
		ui.naviSpinBox->setEnabled( false );
	}
	else {
		ui.actionZoom_In->setEnabled( m_ppProject->GetZoom() != m_ppProject->MaxZoom( m_ppProject->SelectedIndex() ) );
		ui.actionZoom_Out->setEnabled( m_ppProject->GetZoom() != LSM_ONE );
		ui.naviSlider->setEnabled( true );
		ui.naviSlider->setRange( 100, m_ppProject->MaxZoom( m_ppProject->SelectedIndex() ) * 100 );
		ui.naviSlider->setSliderPosition( m_ppProject->GetZoom() * 100 );
		ui.naviSpinBox->setEnabled( true );
		ui.naviSpinBox->setRange( 1.0, m_ppProject->MaxZoom( m_ppProject->SelectedIndex() ) );
		ui.naviSpinBox->setValue( m_ppProject->GetZoom() );
	}
	m_bUpdating = false;
	RedrawSideImages();
	UpdateDrawModes();
}

/**
 * Redraws only the loaded-image OpenGL windows.
 */
LSVOID LSE_CALL LSSpriteEditor::RedrawSideImages() {
	if ( !m_bUpdating ) {
		for ( LSUINT32 I = m_vLoadedImages.Length(); I--; ) {
			if ( m_vLoadedImages[I] ) {
				m_vLoadedImages[I]->UpdateGl();
			}
		}
		ui.naviOpenGLWidget->updateGL();
	}
}

/**
 * Updates the draw mode checks.
 */
LSVOID LSE_CALL LSSpriteEditor::UpdateDrawModes() {
	ui.actionDraw_Normal->setChecked( m_dmDrawMode == LSSE_DM_NORMAL );
	ui.actionDraw_Fixed->setChecked( m_dmDrawMode == LSSE_DM_FIXED );
	ui.actionDraw_Wand->setChecked( m_dmDrawMode == LSSE_DM_WAND );
	ui.actionDraw_Normal->setEnabled( m_vLoadedImages.Length() != 0 );
	ui.actionDraw_Fixed->setEnabled( m_vLoadedImages.Length() != 0 );
	ui.actionDraw_Wand->setEnabled( m_vLoadedImages.Length() != 0 );
}

/**
 * Sets the image to be deleted on the next call to DeleteImage().
 *
 * \param _ui32Index The index of the image to be deleted.
 */
LSVOID LSE_CALL LSSpriteEditor::SetDeleteIndex( LSUINT32 _ui32Index ) {
	m_ui32DeleteIndex = _ui32Index;
}

/**
 * Gets the draw mode.
 *
 * \return Returns the draw mode.
 */
LSSpriteEditor::LSSE_DRAW_MODE LSE_CALL LSSpriteEditor::DrawMode() const {
	return m_dmDrawMode;
}

/**
 * Connects actions and slots.
 */
LSVOID LSE_CALL LSSpriteEditor::ConnectSlots() {
}

/**
 * Synchronizes the loaded-images page with the actual loaded images.
 */
LSVOID LSE_CALL LSSpriteEditor::SyncLoadedImages() {
	m_bUpdating = true;
	// Remove any excess.
	for ( LSUINT32 I = m_ppProject->Images().Length(); I < m_vLoadedImages.Length(); ++I ) {
		LSEDELETE m_vLoadedImages[I];
	}
	LSUINT32 ui32Total = m_vLoadedImages.Length();
	m_vLoadedImages.Resize( m_ppProject->Images().Length() );
	// Add new.
	for ( LSUINT32 I = ui32Total; I < m_vLoadedImages.Length(); ++I ) {
		m_vLoadedImages[I] = LSENEW CLoadImagesBaseWidget( ui.imagesScroll );
		m_vLoadedImages[I]->setEnabled( true );
		m_vLoadedImages[I]->setVisible( true );
	}
	int iMin = 0;
	for ( LSUINT32 I = 0; I < m_vLoadedImages.Length(); ++I ) {
		m_vLoadedImages[I]->SetProject( m_ppProject );
		m_vLoadedImages[I]->SetIndex( I );
		m_vLoadedImages[I]->setGeometry( 0, iMin, m_vLoadedImages[I]->width(), m_vLoadedImages[I]->height() );
		iMin += m_vLoadedImages[I]->height();
	}
	ui.imagePage_2->setMinimumHeight( iMin );
	
	m_bUpdating = false;
	UpdateAll();
}

/**
 * Creates a new project.
 */
void LSSpriteEditor::NewProject() {
}

/**
 * Opens a project.
 */
void LSSpriteEditor::Open() {
}

/**
 * Saves the project.
 */
void LSSpriteEditor::Save() {
	m_ppProject->SaveProject();
}

/**
 * Saves the project as.
 */
void LSSpriteEditor::SaveAs() {
	m_ppProject->SaveProjectAs();
}

/**
 * Adds an image file to the project.
 */
void LSSpriteEditor::AddImage() {
	m_ppProject->AddImage();
	SyncLoadedImages();
}

/**
 * Deletes an image based on m_ui32DeleteIndex;
 */
void LSSpriteEditor::DeleteImage() {
	m_ppProject->DeleteImage( m_ui32DeleteIndex );
	SyncLoadedImages();
}

/**
 * Zooms in on an image.
 */
void LSSpriteEditor::ZoomIn() {
	if ( !m_bUpdating ) {
		m_ppProject->ZoomIn( m_ppProject->SelectedIndex() );
	}
}

/**
 * Zooms out on an image.
 */
void LSSpriteEditor::ZoomOut() {
	if ( !m_bUpdating ) {
		m_ppProject->ZoomOut( m_ppProject->SelectedIndex() );
	}
}

/**
 * Zooms based on the zoom slider.
 */
void LSSpriteEditor::SlideZoom() {
	if ( !m_bUpdating ) {
		m_ppProject->Zoom( m_ppProject->SelectedIndex(), ui.naviSlider->value() / LSREAL( 100.0 ) );
	}
}

/**
 * Zooms based on the spin box.
 */
void LSSpriteEditor::SpinZoom() {
	if ( !m_bUpdating ) {
		m_ppProject->Zoom( m_ppProject->SelectedIndex(), ui.naviSpinBox->value() );
	}
}

/**
 * Sets the normal draw mode.
 */
void LSSpriteEditor::DrawModeNormal() {
	m_dmDrawMode = LSSE_DM_NORMAL;
	UpdateDrawModes();
}

/**
 * Sets the fixed draw mode.
 */
void LSSpriteEditor::DrawModeFixed() {
	m_dmDrawMode = LSSE_DM_FIXED;
	UpdateDrawModes();
}

/**
 * Sets the wand draw mode.
 */
void LSSpriteEditor::DrawModeWand() {
	m_dmDrawMode = LSSE_DM_WAND;
	UpdateDrawModes();
}
