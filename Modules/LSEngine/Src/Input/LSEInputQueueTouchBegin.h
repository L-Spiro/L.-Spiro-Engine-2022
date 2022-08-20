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
 * Description: A structure to hold touch-down event data for the input queue.
 */


#ifndef __LSE_INPUTQUEUETOUCHBEGIN_H__
#define __LSE_INPUTQUEUETOUCHBEGIN_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUETOUCHBEGIN
	 * \brief A structure to hold touch-down event data for the input queue.
	 *
	 * Description: A structure to hold touch-down event data for the input queue.
	 */
	typedef struct LSE_INPUTQUEUETOUCHBEGIN : public LSE_INPUTQUEUEITEMBASE {
		/** The X location of the touch. */
		LSREAL								fX;

		/** The Y location of the touch. */
		LSREAL								fY;
	} * LPLSE_INPUTQUEUETOUCHBEGIN, * const LPCLSE_INPUTQUEUETOUCHBEGIN;

}	// namespace lse

#endif	// __LSE_INPUTQUEUETOUCHBEGIN_H__
