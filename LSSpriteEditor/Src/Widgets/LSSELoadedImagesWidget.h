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


#ifndef __LSSE_LOADEDIMAGESWIDGET_H__
#define __LSSE_LOADEDIMAGESWIDGET_H__

#include "../LSSEMain.h"
#include "LSSESeOpenGlWidget.h"
#include "ShaderLanguage/LSGShader.h"

class										CProject;

/**
 * Class CLoadedImagesWidget
 * \brief Draws the loaded images with sprite-box overlays.
 *
 * Description: Draws the loaded images with sprite-box overlays.
 */
class CLoadedImagesWidget : public CSeOpenGlWidget {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CLoadedImagesWidget( QWidget * _pwParent = Q_NULLPTR );
	LSE_CALLCTOR							~CLoadedImagesWidget();


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

protected :
	// == Members.
	/** The project pointer. */
	CProject *								m_ppProject;

	/** Index. */
	LSUINT32								m_ui32Index;

	/** Our shader. */
	CShader									m_sShader;

	/** The delete action. */
	QAction *								m_paDeletAction;

	/** The handle to the shader uniform for masking. */
	CShader::LSG_HANDLE						m_hShaderSelected;

	/** The handle to the shader uniform for sRGB. */
	CShader::LSG_HANDLE						m_hSrgb;


protected slots :
	// == Functions.


protected :
	// == Functions.
	/**
	 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
	 *	There is no need to call makeCurrent() because this has already been done when this function is called.
	 */
	virtual void							paintGL();

	/**
	 * This event handler, for event _pcmeEvent, can be reimplemented in a subclass to receive widget context menu events.
	 *	The handler is called when the widget's contextMenuPolicy is Qt::DefaultContextMenu.
	 *
	 * \param _pcmeEvent The context menu event data.
	 */
	virtual void							contextMenuEvent( QContextMenuEvent * _pcmeEvent );

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
	 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
	 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
	 */
	virtual void							mousePressEvent( QMouseEvent * _pmeEvent );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#endif	// __LSSE_LOADEDIMAGESWIDGET_H__
