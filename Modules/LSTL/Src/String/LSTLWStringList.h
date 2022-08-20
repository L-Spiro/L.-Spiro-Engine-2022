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
 * Description: A list of wchar_t-sized strings.
 */


#ifndef __LSTL_WSTRINGLIST_H__
#define __LSTL_WSTRINGLIST_H__

#include "../LSTLib.h"
#include "LSTLStringListBase.h"
#include "LSTLWString.h"

namespace lstl {

	/**
	 * Class CWStringList
	 * \brief A list of wchar_t-sized strings.
	 *
	 * Description: A list of wchar_t-sized strings.
	 */
	typedef CStringListBase<CWString>		CWStringList;

}	// namespace lstl

#endif	// __LSTL_WSTRINGLIST_H__
