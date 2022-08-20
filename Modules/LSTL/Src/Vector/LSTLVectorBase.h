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
 * Description: A common base class used for all types of vectors.  This does not add any size to the vector;
 *	it simply adds a static default allocator.  If this were added to the template classes then each type of
 *	vector would get its own copy of the default allocators, but really there should be only this one.
 * This organization is done specifically so that CSVectorPoD types can remain small, since their focus is directly
 *	on size.  In exchange for the small size of the CSVectorPoD template, flexibility is lost in that all CSVectorPoD
 *	types use the same allocator.  You cannot have some CSVectorPoD types use one pool of RAM while another set
 *	uses another.
 */


#ifndef __LSTL_VECTORBASE_H__
#define __LSTL_VECTORBASE_H__

#include "../LSTLib.h"
#include "../Allocator/LSTLAllocator.h"

namespace lstl {

	/**
	 * Class CVectorBase
	 * \brief Default allocator for all vector instances.
	 *
	 * Description: A common base class used for all types of vectors.  This does not add any size to the vector;
	 *	it simply adds a static default allocator.  If this were added to the template classes then each type of
	 *	vector would get its own copy of the default allocators, but really there should be only this one.
	 * This organization is done specifically so that CSVectorPoD types can remain small, since their focus is directly
	 *	on size.  In exchange for the small size of the CSVectorPoD template, flexibility is lost in that all CSVectorPoD
	 *	types use the same allocator.  You cannot have some CSVectorPoD types use one pool of RAM while another set
	 *	uses another.
	 */
	class CVectorBase {
	public :
		// == Members.
		/**
		 * Modifiable pointer to a default allocator used by all vector objects that do not
		 *	explicitly allow using a specific allocator.  Must never be set to NULL.  Should only
		 *	be changed once, before any allocations are ever made.  If a vector allocates from
		 *	the default allocator and then the default is changed, the vector will not be able to
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

#endif	// __LSTL_VECTORBASE_H__
