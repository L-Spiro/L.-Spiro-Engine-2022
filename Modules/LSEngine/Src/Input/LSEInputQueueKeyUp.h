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
 * Description: Key-up event for the input queue.
 */


#ifndef __LSE_INPUTQUEUEKEYUP_H__
#define __LSE_INPUTQUEUEKEYUP_H__

#include "../LSEEngine.h"
#include "LSEInputQueueItemBase.h"
#include "LSEKeyboardKeys.h"

namespace lse {

	/**
	 * Class LSE_INPUTQUEUEKEYUP
	 * \brief Key-up event for the input queue.
	 *
	 * Description: Key-up event for the input queue.
	 */
	typedef struct LSE_INPUTQUEUEKEYUP : public LSE_INPUTQUEUEITEMBASE {
		/** The key being released. */
		LSE_KEYCODES						kcKey;
	} * LPLSE_INPUTQUEUEKEYUP, * const LPCLSE_INPUTQUEUEKEYUP;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEKEYUP_H__
