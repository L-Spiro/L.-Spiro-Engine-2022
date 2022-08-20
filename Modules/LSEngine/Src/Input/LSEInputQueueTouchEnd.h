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
 * Description: A structure to hold touch-end event data for the input queue.
 */


#ifndef __LSE_INPUTQUEUETOUCHEND_H__
#define __LSE_INPUTQUEUETOUCHEND_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUETOUCHEND
	 * \brief A structure to hold touch-end event data for the input queue.
	 *
	 * Description: A structure to hold touch-end event data for the input queue.
	 */
	typedef struct LSE_INPUTQUEUETOUCHEND : public LSE_INPUTQUEUEITEMBASE {
		/** The X location of the touch. */
		LSREAL								fX;

		/** The Y location of the touch. */
		LSREAL								fY;
		
		/** The end X location of the touch. */
		LSREAL								fEndX;

		/** The end Y location of the touch. */
		LSREAL								fEndY;
	} * LPLSE_INPUTQUEUETOUCHEND, * const LPCLSE_INPUTQUEUETOUCHEND;

}	// namespace lse

#endif	// __LSE_INPUTQUEUETOUCHEND_H__
