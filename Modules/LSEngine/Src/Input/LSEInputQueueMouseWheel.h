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
 * Description: A structure to hold mouse-wheel event data for the input queue.
 */


#ifndef __LSE_INPUTQUEUEMOUSEWHEEL_H__
#define __LSE_INPUTQUEUEMOUSEWHEEL_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUEMOUSEWHEEL
	 * \brief A structure to hold mouse-wheel event data for the input queue.
	 *
	 * Description: A structure to hold mouse-wheel event data for the input queue.
	 */
	typedef struct LSE_INPUTQUEUEMOUSEWHEEL : public LSE_INPUTQUEUEITEMBASE {
		/** The amount the wheel has spun.  Negative numbers mean the wheel was rotated back, towards the user. */
		LSREAL								fDist;

		/** The X location of the event. */
		int32_t								i32X;

		/** The Y location of the event. */
		int32_t								i32Y;
	} * LPLSE_INPUTQUEUEMOUSEWHEEL, * const LPCLSE_INPUTQUEUEMOUSEWHEEL;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEMOUSEWHEEL_H__
