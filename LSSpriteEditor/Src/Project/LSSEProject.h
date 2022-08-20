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


#ifndef __LSSE_PROJECT_H__
#define __LSSE_PROJECT_H__

#include "../LSSESpriteEditor.h"
#include "../Game/LSSESpriteGame.h"
#include "Manager/LSRSpriteManager.h"

/**
 * Class CProject
 * \brief The sprite-editor project class.
 *
 * Description: The sprite-editor project class.  Handles a single sprite-editor project.
 */
class CProject {
public :
	// == Various constructors.
	LSE_CALLCTOR							CProject( QWidget * _pwParent );
	LSE_CALLCTOR							~CProject();


	// == Types.
	/** An image and texture pair. */
	typedef struct LSSE_SPRITE_SHEET {
		/** The actual image. */
		CImage								iImage;

		/** The texture. */
		CSharedTexture2dPtr					st2pTexture;

		/** The texture thumbnail. */
		CSharedTexture2dPtr					st2pTextureThumb;

		/** Image path. */
		CString								sPath;

		/** Export format. */
		LSI_PIXEL_FORMAT					pfExportFormat;

		/** The zoom for this image. */
		LSREAL								fZoom;
	} * LPLSSE_SPRITE_SHEET, * const LPCLSSE_SPRITE_SHEET;

	/** The loaded image data. */
	typedef CVector<LSSE_SPRITE_SHEET, LSUINT32, 2>
											LSSE_LOADED_IMAGE;


	// == Functions.
	/**
	 * Gets a reference to the game class.
	 *
	 * \return Returns a reference to the game class.
	 */
	LSE_INLINE CSpriteGame & LSE_CALL		Game();

	/**
	 * Saves the project to the project’s path.
	 *
	 * \return Returns true if the project was saved.
	 */
	LSBOOL LSE_CALL							SaveProject();

	/**
	 * Asks for a path and saves the project there.
	 *
	 * \return Returns true if the project was saved.
	 */
	LSBOOL LSE_CALL							SaveProjectAs();

	/**
	 * Adds an image to the project.
	 */
	LSVOID LSE_CALL							AddImage();

	/**
	 * Gets the loaded images.
	 *
	 * \return Returns the loaded images.
	 */
	LSE_INLINE LSSE_LOADED_IMAGE & LSE_CALL	Images();

	/**
	 * Delets an image by index.
	 *
	 * \param _ui32Index Index of the image to delete.
	 */
	LSVOID LSE_CALL							DeleteImage( LSUINT32 _ui32Index );

	/**
	 * Gets the index of the selected image.
	 *
	 * \return Returns the index of the selected image.
	 */
	LSE_INLINE LSUINT32 LSE_CALL			SelectedIndex() const;

	/**
	 * Sets the selection index.
	 *
	 * \param _ui32Index Selection index to set.
	 */
	LSE_INLINE LSVOID LSE_CALL				SetSelectionIndex( LSUINT32 _ui32Index );

	/**
	 * Gets the maximum zoom level for an image.  Based on maximum backbuffer size.
	 *
	 * \param _ui32Index The image for which to get the maximum zoom.
	 * \return Returns the maximum zoom for the given image.
	 */
	LSREAL LSE_CALL							MaxZoom( LSUINT32 _ui32Index ) const;

	/**
	 * Zooms in on a given image.
	 *
	 * \param _ui32Index Index of the image into which to zoom.
	 */
	LSVOID LSE_CALL							ZoomIn( LSUINT32 _ui32Index );

	/**
	 * Zooms out on a given image.
	 *
	 * \param _ui32Index Index of the image into which to zoom out.
	 */
	LSVOID LSE_CALL							ZoomOut( LSUINT32 _ui32Index );

	/**
	 * Sets the current zoom.
	 *
	 * \param _ui32Index Index of the image into which to zoom.
	 * \param _fZoom Zoom to set.  Zoom is automatically clamped.
	 */
	LSVOID LSE_CALL							Zoom( LSUINT32 _ui32Index, LSREAL _fZoom );

	/**
	 * Gets the current zoom level.
	 *
	 * \return Returns the zoom level on the currently zelected item.
	 */
	LSE_INLINE LSREAL LSE_CALL				GetZoom() const;


protected :
	// == Members.
	/** The game class. */
	CSpriteGame								m_sgGame;

	/** Project path. */
	QString									m_sPath;

	/** Current folder. */
	QString									m_sCurFolder;

	/** The main window. */
	QWidget *								m_pwMainWindow;

	/** The sprite manager. */
	CSpriteManager							m_smManager;

	/** Array of loaded textures. */
	LSSE_LOADED_IMAGE						m_liImages;

	/** Currently selected image. */
	LSUINT32								m_ui32Selected;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// == Functions.
/**
 * Gets a reference to the game class.
 *
 * \return Returns a reference to the game class.
 */
LSE_INLINE CSpriteGame & LSE_CALL CProject::Game() {
	return m_sgGame;
}

/**
 * Gets the loaded images.
 *
 * \return Returns the loaded images.
 */
LSE_INLINE CProject::LSSE_LOADED_IMAGE & LSE_CALL CProject::Images() {
	return m_liImages;
}

/**
 * Gets the index of the selected image.
 *
 * \return Returns the index of the selected image.
 */
LSE_INLINE LSUINT32 LSE_CALL CProject::SelectedIndex() const {
	return m_ui32Selected;
}

/**
 * Sets the selection index.
 *
 * \param _ui32Index Selection index to set.
 */
LSE_INLINE LSVOID LSE_CALL CProject::SetSelectionIndex( LSUINT32 _ui32Index ) {
	m_ui32Selected = _ui32Index;
}

/**
 * Gets the current zoom level.
 *
 * \return Returns the zoom level on the currently zelected item.
 */
LSE_INLINE LSREAL LSE_CALL CProject::GetZoom() const {
	if ( SelectedIndex() < m_liImages.Length() ) {
		return m_liImages[SelectedIndex()].fZoom;
	}
	return LSM_ONE;
}

#endif	// __LSSE_PROJECT_H__
