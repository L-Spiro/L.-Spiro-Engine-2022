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
 * Description: Stack allocators allocate from the top of a stack and from no where else.  They are designed to
 *	provide instant allocation but do not offer any way to free individual addresses.  This facilitates systems
 *	that never allocate more than they need and free all of their allocations at the end of their lives.
 *	Allocation time is reduced dramatically and release time is entirely eliminated, and there is no book-keeping
 *	for each allocation allowing for an extreme gain in efficiency.  With proper organization, this type of
 *	allocator can be the most useful of all allocators.
 */


#ifndef __LSA_STACKALLOCATOR_H__
#define __LSA_STACKALLOCATOR_H__

#include "../OSHeap/LSAOsHeap.h"

namespace lsa {

	/**
	 * Class CStackAllocator
	 * \brief Stack allocators allocate from the top of a stack and from no where else.
	 *
	 * Description: Stack allocators allocate from the top of a stack and from no where else.  They are designed to
	 *	provide instant allocation but do not offer any way to free individual addresses.  This facilitates systems
	 *	that never allocate more than they need and free all of their allocations at the end of their lives.
	 *	Allocation time is reduced dramatically and release time is entirely eliminated, and there is no book-keeping
	 *	for each allocation allowing for an extreme gain in efficiency.  With proper organization, this type of
	 *	allocator can be the most useful of all allocators.
	 */
	class CStackAllocator {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CStackAllocator();
		LSE_CALLCTOR							~CStackAllocator();


		// == Functions.
		/**
		 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
		 *
		 * \param _sSize Amount of RAM to allocate.
		 * \param _ui32Align Alignment of the RAM to allocate.  Can be any power of 2.
		 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL						Alloc( LSA_SIZE _sSize, uint32_t _ui32Align = 16 );

		/**
		 * Resets the stack allocator completely, releasing all resources back to the operating system.
		 */
		void LSE_CALL							Reset();


	protected :
		// == Types.
		/** Keeps track of a single stack of addresses. */
		typedef struct LSA_STACK {
			/** The current position on the stack. */
			uint8_t *							pui8CurPos;

			/** The next stack in the linked list. */
			LSA_STACK *							psNext;

			/** The size of the stack. */
			LSA_SIZE							sSize;
		} * LPLSA_STACK, * const LPCLSA_STACK;


		// == Members.
		/** The first stack in the linked list of stacks. */
		LSA_STACK *								m_psHead;

		/** The first stack in the linked list with any remaining bytes that can be allocated. */
		LSA_STACK *								m_psSearchStart;


		// == Functions.
		/**
		 * Adds a stack to the linked list of stacks with the given size.
		 *
		 * \param _sSize Size of the stack to add.
		 * \return Returns true if there was enough memory to add a stack of the given size.
		 */
		LSBOOL LSE_CALL							AddStack( LSA_SIZE _sSize );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsa

#endif	// __LSA_STACKALLOCATOR_H__
