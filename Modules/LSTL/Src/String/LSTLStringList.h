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
 * Description: A list of regular char-sized strings.
 */


#ifndef __LSTL_STRINGLIST_H__
#define __LSTL_STRINGLIST_H__

#include "../LSTLib.h"
#include "LSTLString.h"
#include "LSTLStringListBase.h"

namespace lstl {

	/**
	 * Class CStringList
	 * \brief A list of regular char-sized strings.
	 *
	 * Description: A list of regular char-sized strings.
	 */
	typedef CStringListBase<CString>		CStringList;

}	// namespace lstl

#endif	// __LSTL_STRINGLIST_H__
