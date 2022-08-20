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
 * Description: The OpenGL widget used throughout the project.
 */


#ifndef __LSSE_SEOPENGLWIDGET_H__
#define __LSSE_SEOPENGLWIDGET_H__

#include "../LSSEMain.h"
#include "Atomic/LSSTDAtomic.h"
#include "Camera/LSRCamera2d.h"
#include "Gfx/LSGGfx.h"
#include "Sampler/LSGSampler.h"
#include "VertexBuffer/LSGNBufferVertexBuffer.h"
#include <QGLWidget>

/**
 * Class CSeOpenGlWidget
 * \brief The OpenGL widget used throughout the project.
 *
 * Description: The OpenGL widget used throughout the project.
 */
class CSeOpenGlWidget : public QGLWidget {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CSeOpenGlWidget( QWidget * _pwParent = Q_NULLPTR );
	LSE_CALLCTOR							~CSeOpenGlWidget();


	// == Types.
	/** The system-provided vertex type, which consists of just a position and UV coordinates. */
#pragma pack( push, 1 )
	typedef struct LSSE_SPRITE_VERTEX {
		LSFLOAT								fPos[2];								/**< Vertex position. */
		LSFLOAT								fUv[2];									/**< UV coordinates. */
	} * LPLSSE_SPRITE_VERTEX, * const LPCLSSE_SPRITE_VERTEX;
#pragma pack( pop )


	// == Functions.
	/**
	 * Makes the original OpenGL context current.
	 */
	static LSVOID LSE_CALL					MakeCurrent();

	/**
	 * Arranges the given vertices into a quad.
	 *
	 * \param _psvVertices Vertices to arrange (must be 6 of them).
	 * \param _fLeft Left of the quad.
	 * \param _fTop Top of the quad.
	 * \param _fRight Right of the quad.
	 * \param _fBottom Bottom of the quad.
	 * \param _fUvLeft Left UV of the quad.
	 * \param _fUvTop Top UV of the quad.
	 * \param _fUvRight Right UV of the quad.
	 * \param _fUvBottom Bottom UV of the quad.
	 */
	static LSVOID LSE_CALL					MakeQuad( LSSE_SPRITE_VERTEX * _psvVertices,
		LSREAL _fLeft, LSREAL _fTop, LSREAL _fRight, LSREAL _fBottom,
		LSREAL _fUvLeft, LSREAL _fUvTop, LSREAL _fUvRight, LSREAL _fUvBottom );


protected :
	// == Members.
	/**
	 * This virtual function is called once before the first call to paintGL() or resizeGL(), and then once whenever the widget has been assigned a new QGLContext. Reimplement it in a subclass.
	 *	This function should set up any required OpenGL context rendering flags, defining display lists, etc.
	 *	There is no need to call makeCurrent() because this has already been done when this function is called.
	 */
	virtual void							initializeGL();

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


protected :
	// == Members.
	/** The 2D camera for the view. */
	CCamera2d								m_cCamera;

	/** Tracks the first instance of an OpenGL context being made. */
	static CAtomic::ATOM					m_aOpenGlContextMade;

	/** The shared OpenGL widget. */
	static QGLWidget *						m_pqlwSharedWidget;

	/** A basic vertex buffer for drawing quads. */
	static CNBufferVertexBuffer<4> *		m_pnbvbVertices;

	/** The default sprite sampler. */
	static CSampler *						m_psSampler;

	/** The sprite raster mode. */
	static LSG_RASTERIZER_STATE *			m_prsRasterState;

	/** The depth mode. */
	static LSG_DEPTH_STENCIL_STATE *		m_pdssDepthState;

	/** The blend state for alpha objects. */
	static LSG_BLEND_STATE *				m_pbsAlphaBlendState;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#endif	// __LSSE_SEOPENGLWIDGET_H__
