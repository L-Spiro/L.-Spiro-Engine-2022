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
 * Description: Provides a default allocator for the CLinkedList class.
 */


#ifndef __LSTL_LINKEDLISTBASE_H__
#define __LSTL_LINKEDLISTBASE_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"

namespace lstl {

	/**
	 * Class CLinkedListBase
	 * \brief Provides a default allocator for the CLinkedList class.
	 *
	 * Description: Provides a default allocator for the CLinkedList class.
	 */
	class CLinkedListBase {
	protected :
		// == Members.
		/**
		 * The default allocator if none are supplied.
		 */
		static CAllocator						m_aAllocator;
	};

}	// namespace lstl

#endif	// __LSTL_LINKEDLISTBASE_H__
