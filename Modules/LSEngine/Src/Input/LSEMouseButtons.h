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
 * Description: Mouse button codes.
 */


#ifndef __LSE_MOUSEBUTTONS_H__
#define __LSE_MOUSEBUTTONS_H__

#include "../LSEEngine.h"

namespace lse {

	/** Mouse button codes. */
	enum LSE_MOUSEBUTTONCODES {
		LSE_MBC_INVALID			= ~0,
		LSE_MBC_LBUTTON			= 0,
		LSE_MBC_MBUTTON,
		LSE_MBC_RBUTTON,
		LSE_MBC_XBUTTON1,
		LSE_MBC_XBUTTON2,
	};

}	// namespace lse

#endif	// __LSE_MOUSEBUTTONS_H__
