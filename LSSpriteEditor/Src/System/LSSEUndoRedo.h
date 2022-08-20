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
 * Description: Undo and redo functionality.
 */


#ifndef __LSSE_UNDOREDO_H__
#define __LSSE_UNDOREDO_H__

#include "../LSSEMain.h"

// == Enumerations.


// == Types.
/** The union holding the data for each type. */
typedef union LSSE_UNDO_REDO {

} * LPLSSE_UNDO_REDO, * const LPCLSSE_UNDO_REDO;

/**
 * Class CUndoRedo
 * \brief Undo and redo functionality.
 *
 * Description: Undo and redo functionality.
 */
class CUndoRedo {
public :
	// == Various constructors.
	LSE_CALLCTOR							CUndoRedo();


	// == Functions.


protected :
	// == Members.
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// DEFINITIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#endif	// __LSSE_UNDOREDO_H__
