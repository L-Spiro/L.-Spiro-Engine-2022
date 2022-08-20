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
 * Description: The main OpenGL view for editing sprites.
 */


#ifndef __LSSE_MAINOPENGLWIDGET_H__
#define __LSSE_MAINOPENGLWIDGET_H__

#include "../LSSEMain.h"
#include "LSSESeOpenGlWidget.h"
#include "ShaderLanguage/LSGShader.h"
#include <QScrollBar>

class										CProject;

/**
 * Class CMainOpenGlWidget
 * \brief The main OpenGL view for editing sprites.
 *
 * Description: The main OpenGL view for editing sprites.
 */
class CMainOpenGlWidget : public CSeOpenGlWidget {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CMainOpenGlWidget( QWidget * _pwParent = Q_NULLPTR );
	LSE_CALLCTOR							~CMainOpenGlWidget();


	// == Functions.
	/**
	 * Sets the project pointer.
	 *
	 * \param _ppProject The project pointer.
	 */
	LSVOID LSE_CALL							SetProject( CProject * _ppProject );

	/**
	 * Converts a coordinate from the widget space into normalized texture coordinates.
	 *
	 * \param _fX The X coordinate to convert.
	 * \param _fY The Y coordinate to convert.
	 * \return Returns the converted [x,y] coordinates.
	 */
	CVector2 LSE_CALL						ScreenToTexture( LSREAL _fX, LSREAL _fY ) const;

	/**
	 * Converts a coordinate from normalized texture coordinates to widget space.
	 *
	 * \param _fX The X coordinate to convert.
	 * \param _fY The Y coordinate to convert.
	 * \return Returns the converted [x,y] coordinates.
	 */
	CVector2 LSE_CALL						TextureToScreen( LSREAL _fX, LSREAL _fY ) const;

	/**
	 * Gets the pixel under the given local coordinate.
	 *
	 * \param _fX X coordinate.
	 * \param _fY Y coordinate.
	 * \param _i32X Holds the returned X pixel coordinate.
	 * \param _i32Y Holds the returned Y pixel coordinate.
	 */
	LSVOID LSE_CALL							ClientToPixel( LSREAL _fX, LSREAL _fY, LSINT32 &_i32X, LSINT32 &_i32Y ) const;

	/**
	 * Gets the UV coordinate under the given local coordinate.
	 *
	 * \param _fX X coordinate.
	 * \param _fY Y coordinate.
	 * \param _fRetX Holds the returned X UV coordinate.
	 * \param _fRetY Holds the returned Y UV coordinate.
	 */
	LSVOID LSE_CALL							ClientToUv( LSREAL _fX, LSREAL _fY, LSREAL &_fRetX, LSREAL &_fRetY ) const;


	// == Members.
	/** Vertical scroll bar. */
	QScrollBar *							m_psbVertBar;

	/** Horizontal scroll bar. */
	QScrollBar *							m_psbHorBar;


protected :
	// == Members.
	/** The project pointer. */
	CProject *								m_ppProject;

	/** Our shader. */
	CShader									m_sShader;

	/** Crosshair shader. */
	CShader									m_sCrossHairShader;

	/** Box shader. */
	CShader									m_sBoxShader;

	/** Crosshair vertices. */
	CNBufferVertexBuffer<2> 				m_nbvbCrossVertices;

	/** Box vertices. */
	CNBufferVertexBuffer<10> 				m_nbvbBoxVertices;

	/** The handle to the shader uniform for sRGB. */
	CShader::LSG_HANDLE						m_hSrgb;

	/** The handle to the shader uniform for pixel size. */
	CShader::LSG_HANDLE						m_hSrgbCrossHair;

	/** Mouse position. */
	CVector2								m_vMousePos;

	/** Mouse-press location. */
	CVector2								m_vMousePress;

	/** Mouse-release location. */
	CVector2								m_vMouseRelease;

	/** Is the mouse down? */
	LSBOOL									m_bDragging;


	// == Functions.
	/**
	 * This virtual function is called whenever the widget needs to be painted. Reimplement it in a subclass.
	 *	There is no need to call makeCurrent() because this has already been done when this function is called.
	 */
	virtual void							paintGL();

	/**
	 * This virtual function is called whenever the widget has been resized. The new size is passed in _iWidth and _iHeight. Reimplement it in a subclass.
	 *	There is no need to call makeCurrent() because this has already been done when this function is called.
	 *
	 * \param _iWidth New width of the window.
	 * \param _iHeight New height of the window.
	 */
	virtual void							resizeGL( LSINT32 _iWidth, LSINT32 _iHeight );

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse move events for the widget.
	 *	If mouse tracking is switched off, mouse move events only occur if a mouse button is pressed while the mouse is being moved. If mouse tracking is switched on, mouse move events occur even if no mouse button is pressed.
	 *	QMouseEvent::pos() reports the position of the mouse cursor, relative to this widget. For press and release events, the position is usually the same as the position of the last mouse move event, but it might be different if the user's hand shakes. This is a feature of the underlying window system, not Qt.
	 *	If you want to show a tooltip immediately, while the mouse is moving (e.g., to get the mouse coordinates with QMouseEvent::pos() and show them as a tooltip), you must first enable mouse tracking as described above. Then, to ensure that the tooltip is updated immediately, you must call QToolTip::showText() instead of setToolTip() in your implementation of mouseMoveEvent().
	 */
	virtual void							mouseMoveEvent( QMouseEvent * _pmeEvent );

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse press events for the widget.
	 *	If you create new widgets in the mousePressEvent() the mouseReleaseEvent() may not end up where you expect, depending on the underlying window system (or X11 window manager), the widgets' location and maybe more.
	 *	The default implementation implements the closing of popup widgets when you click outside the window. For other widget types it does nothing.
	 */
	virtual void							mousePressEvent( QMouseEvent * _pmeEvent );

	/**
	 * This event handler, for event _pmeEvent, can be reimplemented in a subclass to receive mouse release events for the widget.
	 */
	virtual void							mouseReleaseEvent( QMouseEvent * _pmeEvent );

	/**
	 * Draws the crosshairs.
	 */
	LSVOID LSE_CALL							DrawCrosshairs();

	/**
	 * Draws a single box given the upper-left and bottom-right UV coordinates.
	 *
	 * \param _fLeft Left coordinate.
	 * \param _fTop Top coordinate.
	 * \param _fRight Right coordinate.
	 * \param _fBottom Bottom coordinate.
	 */
	LSVOID LSE_CALL							DrawBox( LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom );

	/**
	 * Gets the bounding box for the texture.
	 *
	 * \param _fLeft The returned left coordinate.
	 * \param _fRight The returned right coordinate.
	 * \param _fTop The returned top coordinate.
	 * \param _fBottom The returned bottom coordinate.
	 */
	LSVOID LSE_CALL							GetTextureBounds( LSREAL &_fLeft, LSREAL &_fRight, LSREAL &_fTop, LSREAL &_fBottom ) const;

	/**
	 * Converts a UV coordinate into a pixel location on the screen.
	 *
	 * \param _fX X coordinate.
	 * \param _fX Y coordinate.
	 * \return Returns the converted coordinate.
	 */
	CVector2 LSE_CALL						UvToClient( LSREAL _fX, LSREAL _fY ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#endif	// __LSSE_MAINOPENGLWIDGET_H__
