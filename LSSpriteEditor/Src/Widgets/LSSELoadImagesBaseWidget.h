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


#ifndef __LSSE_LOADEDIMAGESBASEWIDGET_H__
#define __LSSE_LOADEDIMAGESBASEWIDGET_H__

#include "../LSSEMain.h"
#include "../ui_LSSELoadedImage.h"
#include <QtWidgets/QWidget.h>

/**
 * Class CLoadImagesBaseWidget
 * \brief A widget for displaying a loaded texture, the texture path, etc.
 *
 * Description: A widget for displaying a loaded texture, the texture path, etc.
 */
class CLoadImagesBaseWidget : public QWidget {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CLoadImagesBaseWidget( QWidget * _pwParent = Q_NULLPTR );
	LSE_CALLCTOR							~CLoadImagesBaseWidget();


	// == Functions.
	/**
	 * Sets the project pointer.
	 *
	 * \param _ppProject The project pointer.
	 */
	LSVOID LSE_CALL							SetProject( CProject * _ppProject );

	/**
	 * Sets the image index inside the array of loaded images.
	 *
	 * \param _ui32Index Index into the array of loaded images.
	 */
	LSVOID LSE_CALL							SetIndex( LSUINT32 _ui32Index );

	/**
	 * Repaints the OpenGL area.
	 */
	LSVOID LSE_CALL							UpdateGl();


protected :
	// == Members.
	/** The project pointer. */
	CProject *								m_ppProject;

	/** Index. */
	LSUINT32								m_ui32Index;

private :
	Ui::LoadedImagesForm					ui;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#endif	// __LSSE_LOADEDIMAGESBASEWIDGET_H__
