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
 * Description: Provides a default allocator shared among all instances of CLinearMap classes.
 */


#ifndef __LSTL_MAPBASE_H__
#define __LSTL_MAPBASE_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"

namespace lstl {

	/**
	 * Class CMapBase
	 * \brief Provides a default allocator shared among all instances of CLinearMap classes.
	 *
	 * Description: Provides a default allocator shared among all instances of CLinearMap classes.
	 */
	class CMapBase {
	public :
		// == Members.
		/**
		 * Modifiable pointer to a default allocator used by all map objects that do not
		 *	explicitly allow using a specific allocator.  Must never be set to NULL.  Should only
		 *	be changed once, before any allocations are ever made.  If a map allocates from
		 *	the default allocator and then the default is changed, the map will not be able to
		 *	free its memory unless the default is changed back.  Normally you just create an
		 *	allocator at the beginning, set it as the default, and then let all the vectors work
		 *	off that.
		 */
		static CAllocator *			m_paDefaultAllocator;

	protected :
		/**
		 * The default allocator if none are supplied.
		 */
		static CAllocator			m_aAllocator;
	};

}	// namespace lstl

#endif	// __LSTL_MAPBASE_H__
