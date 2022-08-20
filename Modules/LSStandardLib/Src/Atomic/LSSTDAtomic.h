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
 * Description: Atomic operations.
 */


#ifndef __LSSTD_ATOMIC_H__
#define __LSSTD_ATOMIC_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_WINDOWS
#include "LSSTDWindowsAtomic.h"
#else
#include "LSSTDGccAtomic.h"
#endif	// #ifdef LSE_WINDOWS

#endif	// __LSSTD_ATOMIC_H__
