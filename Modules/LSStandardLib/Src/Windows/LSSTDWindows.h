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
 * Description: Includes Windows.h (only on the Windows OS).
 */


#ifndef __LSSTD_WINDOWS_H__
#define __LSSTD_WINDOWS_H__

#include "../LSSTDStandardLib.h"

#ifdef LSE_WINDOWS
// Windows.
// Set the Windows version.
#ifndef WINVER
#define WINVER         0x0501
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501 
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0501
#endif

#define NOMINMAX				// We do not want min/max as macros.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>			// ::QueryPerformanceCounter()/::QueryPerformanceFrequency().

#endif	// #ifdef LSE_WINDOWS

#endif	// __LSSTD_WINDOWS_H__

