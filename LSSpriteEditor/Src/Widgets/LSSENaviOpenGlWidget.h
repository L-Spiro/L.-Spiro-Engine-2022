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


#ifndef __LSSE_NAVIOPENGLWIDGET_H__
#define __LSSE_NAVIOPENGLWIDGET_H__

#include "../LSSEMain.h"
#include "LSSESeOpenGlWidget.h"
#include "ShaderLanguage/LSGShader.h"

class										CProject;

/**
* Class CNaviOpenGlWidget
* \brief The navigation view.
*
* Description: The navigation view.
*/
class CNaviOpenGlWidget : public CSeOpenGlWidget {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CNaviOpenGlWidget( QWidget * _pwParent = Q_NULLPTR );


	// == Functions.
	/**
	 * Sets the project pointer.
	 *
	 * \param _ppProject The project pointer.
	 */
	LSVOID LSE_CALL							SetProject( CProject * _ppProject );


protected :
	// == Members.
	/** The project pointer. */
	CProject *								m_ppProject;

	/** Our shader. */
	CShader									m_sShader;

	/** The handle to the shader uniform for sRGB. */
	CShader::LSG_HANDLE						m_hSrgb;

	/** The handle to the shader uniform for the view bounds. */
	CShader::LSG_HANDLE						m_hBounds;


	// == Functions.
	/**
	 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
	 *	There is no need to call makeCurrent() because this has already been done when this function is called.
	 */
	virtual void							paintGL();

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
	 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
	 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
	 */
	virtual void							mousePressEvent( QMouseEvent * _pmeEvent );

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse move events for the widget.
	 *	If mouse tracking is switched off, mouse move events only occur if a mouse button is pressed while the mouse is being moved. If mouse tracking is switched on, mouse move events occur even if no mouse button is pressed.
	 *	QMouseEvent::pos() reports the position of the mouse cursor, relative to this widget. For press and release events, the position is usually the same as the position of the last mouse move event, but it might be different if the user's hand shakes. This is a feature of the underlying window system, not Qt.
	 *	If you want to show a tooltip immediately, while the mouse is moving (e.g., to get the mouse coordinates with QMouseEvent::pos() and show them as a tooltip), you must first enable mouse tracking as described above. Then, to ensure that the tooltip is updated immediately, you must call QToolTip::showText() instead of setToolTip() in your implementation of mouseMoveEvent().
	 */
	virtual void							mouseMoveEvent( QMouseEvent * _pmeEvent );

	/**
	 * Gets the box around the texture for drawing.
	 *
	 * \param _fLeft The returned left coordinate of the box in NDC.
	 * \param _fTop The returned left coordinate of the box in NDC.
	 * \param _fRight The returned left coordinate of the box in NDC.
	 * \param _fBottom The returned left coordinate of the box in NDC.
	 */
	LSVOID LSE_CALL							GetImageBoundsNdc( LSREAL &_fLeft, LSREAL &_fTop, LSREAL &_fRight, LSREAL &_fBottom ) const;

	/**
	 * Puts the given coordinates in view by scrolling the main view.
	 *
	 * \param _iX The X coordinate that should be put into the center of the view on the main widget.
	 * \param _iY The Y coordinate that should be put into the center of the view on the main widget.
	 */
	LSVOID LSE_CALL							PutIntoView( int _iX, int _iY ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#endif	// __LSSE_NAVIOPENGLWIDGET_H__
