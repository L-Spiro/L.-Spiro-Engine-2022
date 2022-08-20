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
 * Description: The base structure for all forms of input that can go into an input queue.
 */


#ifndef __LSE_INPUTQUEUEITEMBASE_H__
#define __LSE_INPUTQUEUEITEMBASE_H__

#include "../LSEEngine.h"

namespace lse {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Input types. */
	enum LSE_INPUT_TYPES {
		LSE_IT_KEYBOARDBTNDWN,							/**< Keyboard button down type. */
		LSE_IT_KEYBOARDBTNUP,							/**< Keyboard button up type. */
		LSE_IT_MOUSEBTNDWN,								/**< Mouse button down type. */
		LSE_IT_MOUSEBTNUP,								/**< Mouse button up type. */
		LSE_IT_MOUSEMOVE,								/**< Mouse move type. */
		LSE_IT_MOUSEWHEEL,								/**< Mouse wheel type. */
		LSE_IT_TOUCHBEGIN,								/**< Touch-begin type. */
		LSE_IT_TOUCHEND,								/**< Touch-end type. */
		LSE_IT_TOUCHMOVE,								/**< Touch-move type. */
		LSE_IT_TOUCHCANCEL,								/**< Touch-cancel type. */
	};

	/**
	 * Class CInputQueueItemBase
	 * \brief The base structure for all forms of input that can go into an input queue.
	 *
	 * Description: The base structure for all forms of input that can go into an input queue.
	 */
	typedef struct LSE_INPUTQUEUEITEMBASE {
		/** Type of input. */
		LSE_INPUT_TYPES									itType;

		/** Time of event in microseconds. */
		uint64_t										ui64Time;
	} * LPLSE_INPUTQUEUEITEMBASE, * const LPCLSE_INPUTQUEUEITEMBASE;

}	// namespace lse

#endif	// __LSE_INPUTQUEUEITEMBASE_H__
