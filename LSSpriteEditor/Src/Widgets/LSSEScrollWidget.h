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
 * Description: Includes a notification of scrolling events.
 */


#ifndef __LSSE_SCROLLWIDGET_H__
#define __LSSE_SCROLLWIDGET_H__

#include "../LSSEMain.h"
#include <QScrollArea>

/**
 * Class CScrollWidget
 * \brief Includes a notification of scrolling events.
 *
 * Description: Includes a notification of scrolling events.
 */
class CScrollWidget : public QScrollArea {
	Q_OBJECT
public :
	// == Various constructors.
	LSE_CALLCTOR							CScrollWidget( QWidget * _pwParent = Q_NULLPTR );


	// == Functions.


protected :
	// == Members.


	// == Functions.
	/**
	 * This virtual handler is called when the scroll bars are moved by _iDx, _iDy, and consequently the viewport's contents should be scrolled accordingly.
	 *	The default implementation simply calls update() on the entire viewport(), subclasses can reimplement this handler for optimization purposes, or - like QScrollArea - to move a contents widget. The parameters _iDx and _iDy are there for convenience, so that the class knows how much should be scrolled (useful e.g. when doing pixel-shifts). You may just as well ignore these values and scroll directly to the position the scroll bars indicate.
	 *	Calling this function in order to scroll programmatically is an error, use the scroll bars instead (e.g. by calling QScrollBar::setValue() directly).
	 */
	virtual void							scrollContentsBy( int _iDx, int _iDy );

	/**
	 * The main event handler for the scrolling area (the viewport() widget). It handles the event specified, and can be called by subclasses to provide reasonable default behavior.
	 *	Returns true to indicate to the event system that the event has been handled, and needs no further processing; otherwise returns false to indicate that the event should be propagated further.
	 *	You can reimplement this function in a subclass, but we recommend using one of the specialized event handlers instead.
	 *	Specialized handlers for viewport events are: paintEvent(), mousePressEvent(), mouseReleaseEvent(), mouseDoubleClickEvent(), mouseMoveEvent(), wheelEvent(), dragEnterEvent(), dragMoveEvent(), dragLeaveEvent(), dropEvent(), contextMenuEvent(), and resizeEvent().
	 */
	virtual bool							viewportEvent( QEvent * _peEvent );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#endif	// __LSSE_SCROLLWIDGET_H__
