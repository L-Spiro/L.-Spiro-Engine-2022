/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A structure to hold mouse-down event data for the input queue.
 */


#ifndef __LSE_INPUTQUEUEMOUSEDOWN_H__
#define __LSE_INPUTQUEUEMOUSEDOWN_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"
#include "LSEMouseButtons.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUEMOUSEDOWN
	 * \brief A structure to hold mouse-down event data for the input queue.
	 *
	 * Description: A structure to hold mouse-down event data for the input queue.
	 */
	typedef struct LSE_INPUTQUEUEMOUSEDOWN : public LSE_INPUTQUEUEITEMBASE {
		/** The mouse button being pressed. */
		LSE_MOUSEBUTTONCODES				mbcButton;
	} * LPLSE_INPUTQUEUEMOUSEDOWN, * const LPCLSE_INPUTQUEUEMOUSEDOWN;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEMOUSEDOWN_H__
