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
 * Description: A structure to hold mouse-up event data for the input queue.
 */


#ifndef __LSE_INPUTQUEUEMOUSEUP_H__
#define __LSE_INPUTQUEUEMOUSEUP_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"
#include "LSEMouseButtons.h"

namespace lse {

	/**
	 * Class INPUTQUEUEMOUSEUP
	 * \brief A structure to hold mouse-up event data for the input queue.
	 *
	 * Description: A structure to hold mouse-up event data for the input queue.
	 */
	typedef struct LSE_INPUTQUEUEMOUSEUP : public LSE_INPUTQUEUEITEMBASE {
		/** The mouse button being release. */
		LSE_MOUSEBUTTONCODES				mbcButton;
	} * LPLSE_INPUTQUEUEMOUSEUP, * const LPCLSE_INPUTQUEUEMOUSEUP;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEMOUSEUP_H__
