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
 * Description: Key-down event for the input queue.
 */


#ifndef __LSE_INPUTQUEUEKEYDOWN_H__
#define __LSE_INPUTQUEUEKEYDOWN_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"
#include "LSEKeyboardKeys.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUEKEYDOWN
	 * \brief Key-down event for the input queue.
	 *
	 * Description: Key-down event for the input queue.
	 */
	typedef struct LSE_INPUTQUEUEKEYDOWN : public LSE_INPUTQUEUEITEMBASE {
		/** The key being pressed. */
		LSE_KEYCODES						kcKey;
	} * LPLSE_INPUTQUEUEKEYDOWN, * const LPCLSE_INPUTQUEUEKEYDOWN;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEKEYDOWN_H__
