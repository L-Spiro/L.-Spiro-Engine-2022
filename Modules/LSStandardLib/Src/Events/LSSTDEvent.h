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
 * Description: An event.  When triggered, a thread will activate from a waiting state.
 */


#ifndef __LSSTD_EVENT_H__
#define __LSSTD_EVENT_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_WINDOWS
#include "LSSTDWindowsEvent.h"
#else
#include "LSSTDPosixEvent.h"
#endif	// #ifdef LSE_WINDOWS

#endif	// __LSSTD_EVENT_H__
