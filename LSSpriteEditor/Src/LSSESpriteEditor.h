#ifndef LSSESPRITEEDITOR_H
#define LSSESPRITEEDITOR_H

#include "LSSEMain.h"
#include "Widgets/LSSELoadImagesBaseWidget.h"
#include "ui_LSSESpriteEditor.h"
#include <QtWidgets/QMainWindow>


class										CProject;

class LSSpriteEditor : public QMainWindow
{
	Q_OBJECT

public:
	LSSpriteEditor( QWidget * parent = 0 );
	~LSSpriteEditor();


	// == Types.
	/** Drawing modes. */
	enum LSSE_DRAW_MODE {
		LSSE_DM_NORMAL,																		/**< Normal drawing mode. */
		LSSE_DM_FIXED,																		/**< Fixed-size drawing mode. */
		LSSE_DM_WAND,																		/**< Wand drawing mode. */
	};


	// == Functions.
	/**
	 * Redraws all OpenGL windows.
	 */
	LSVOID LSE_CALL							UpdateAll();

	/**
	 * Redraws only the loaded-image OpenGL windows.
	 */
	LSVOID LSE_CALL							RedrawSideImages();

	/**
	 * Updates the draw mode checks.
	 */
	LSVOID LSE_CALL							UpdateDrawModes();

	/**
	 * Sets the image to be deleted on the next call to DeleteImage().
	 *
	 * \param _ui32Index The index of the image to be deleted.
	 */
	LSVOID LSE_CALL							SetDeleteIndex( LSUINT32 _ui32Index );

	/**
	 * Gets the draw mode.
	 *
	 * \return Returns the draw mode.
	 */
	LSSE_DRAW_MODE LSE_CALL					DrawMode() const;

	/**
	 * Gets the static pointer to the main window.
	 *
	 * \return Returns the static pointer to the main window.
	 */
	static LSE_INLINE LSSpriteEditor * LSE_CALL
											GetMainWindow();


	// == Members.
	Ui::LSSpriteEditorClass					ui;

protected :
	// == Members.
	/** The project. */
	CProject *								m_ppProject;

	/** The array of loaded images. */
	CVector<CLoadImagesBaseWidget *, LSUINT32, 4>
											m_vLoadedImages;

	/** The image to delete. */
	LSUINT32								m_ui32DeleteIndex;

	/** Are we updating controls ourselves? */
	LSBOOL									m_bUpdating;

	/** Draw mode. */
	LSSE_DRAW_MODE							m_dmDrawMode;

	/** A static copy of this object. */
	static LSSpriteEditor *					m_pseEditor;


	// == Functions.
	/**
	 * Connects actions and slots.
	 */
	LSVOID LSE_CALL							ConnectSlots();

	/**
	 * Synchronizes the loaded-images page with the actual loaded images.
	 */
	LSVOID LSE_CALL							SyncLoadedImages();


private slots :
	// == Functions.
	/**
	 * Creates a new project.
	 */
	void									NewProject();

	/**
	 * Opens a project.
	 */
	void									Open();

	/**
	 * Saves the project.
	 */
	void									Save();

	/**
	 * Saves the project as.
	 */
	void									SaveAs();

	/**
	 * Adds an image file to the project.
	 */
	void									AddImage();

	/**
	 * Deletes an image based on m_ui32DeleteIndex;
	 */
	void									DeleteImage();

	/**
	 * Zooms in on an image.
	 */
	void									ZoomIn();

	/**
	 * Zooms out on an image.
	 */
	void									ZoomOut();

	/**
	 * Zooms based on the zoom slider.
	 */
	void									SlideZoom();

	/**
	 * Zooms based on the spin box.
	 */
	void									SpinZoom();

	/**
	 * Sets the normal draw mode.
	 */
	void									DrawModeNormal();

	/**
	 * Sets the fixed draw mode.
	 */
	void									DrawModeFixed();

	/**
	 * Sets the wand draw mode.
	 */
	void									DrawModeWand();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// == Functions.
/**
* Gets the static pointer to the main window.
*
* \return Returns the static pointer to the main window.
*/
LSE_INLINE LSSpriteEditor * LSE_CALL LSSpriteEditor::GetMainWindow() {
	return m_pseEditor;
}

#endif // LSSESPRITEEDITOR_H
