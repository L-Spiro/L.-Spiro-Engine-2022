/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A fast allocator for very specific types of allocations.  Allocations are made in a stack
 *	fashion and can't be freed until the allocator itself is freed, but unlike CStackAllocator these allow
 *	the last allocation to be reallocated.  A single object at a time can allocate from this allocator and
 *	resize the allocation up or down, then calls Done() to indicate that it is done allocating and another
 *	resizable allocation can be made.
 */


#ifndef __LSA_DYNAMICSTACKALLOCATOR_H__
#define __LSA_DYNAMICSTACKALLOCATOR_H__

#include "../OSHeap/LSAOsHeap.h"

namespace lsa {

	/**
	 * Class CDynamicStackAllocator
	 * \brief A fast allocator for very specific types of allocations.
	 *
	 * Description: A fast allocator for very specific types of allocations.  Allocations are made in a stack
	 *	fashion and can't be freed until the allocator itself is freed, but unlike CStackAllocator these allow
	 *	the last allocation to be reallocated.  A single object at a time can allocate from this allocator and
	 *	resize the allocation up or down, then calls Done() to indicate that it is done allocating and another
	 *	resizable allocation can be made.
	 */
	class CDynamicStackAllocator {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDynamicStackAllocator();
		LSE_CALLCTOR							~CDynamicStackAllocator();


		// == Functions.
		/**
		 * Allocates RAM.  If there is not enough and the heap can grow, it tries to grow the heap.
		 *
		 * \param _sSize Amount of RAM to allocate.
		 * \param _ui32Align Alignment of the RAM to allocate.
		 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL						Alloc( LSA_SIZE _sSize, uint32_t _ui32Align = 16 );

		/**
		 * Resizes the last allocation made to the new given size.  If no allocation was made last, this behaves the same
		 *	as Alloc().
		 *
		 * \param _sSize New size of the previous allocation.
		 * \param _ui32Align Alignment of the RAM to allocate.  Only used if no previous allocation has been made.
		 * \return Returns the allocated RAM or NULL if there is not enough RAM available to make the allocation.
		 */
		void * LSE_CALL						ReAlloc( LSA_SIZE _sSize, uint32_t _ui32Align = 16 );

		/**
		 * Finishes an allocation.  After this is called, if the next call is ReAlloc(), it will revert to calling Alloc().
		 *	Alloc() automatically calls this internally, so 2 patterns exist for using this allocator:
		 * #1: Alloc() ReAlloc() ReAlloc(), Alloc() ReAlloc() ReAlloc() [etc.]
		 * #2: ReAlloc() ReAlloc() ReAlloc() Done(), ReAlloc() ReAlloc() ReAlloc() Done() [etc.]
		 * These 2 patterns produce identical results.
		 */
		LSE_INLINE void LSE_CALL				Done();

		/**
		 * Frees the last allocation.
		 */
		LSE_INLINE void LSE_CALL				Free();

		/**
		 * Resets the stack allocator completely, releasing all resources back to the operating system.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets the size of the current allocation.
		 *
		 * \return Returns the size of the current allocation.
		 */
		LSE_INLINE LSA_SIZE LSE_CALL			CurSize() const;


	protected :
		// == Types.
		/** Keeps track of a single stack of addresses. */
		typedef struct LSA_STACK {
			/** Pointer to the next stack. */
			LSA_STACK *							psNext;

			/** Pointer to the start of the current allocation. */
			uint8_t *							pui8CurStart;

			/** Remaining size of the stack. */
			LSA_SIZE							sSize;

			/** Current allocation size. */
			LSA_SIZE							sCurSize;
		} * LPLSA_STACK, * const LPCLSA_STACK;


		// == Members.
		/** The first stack in the linked list of stacks. */
		LSA_STACK *								m_psHead;

		/** The first stack in the linked list with any remaining bytes that can be allocated. */
		LSA_STACK *								m_psSearchStart;

		/** The last stack from which a new allocation was made. */
		LSA_STACK *								m_psLast;

		/** The last alignment. */
		uint32_t								m_ui32Align;


		// == Functions.
		/**
		 * Allocates a new stack with the given size.
		 *
		 * \param _sSize Size of the stack to add.
		 * \return Returns true if there was enough memory to add a stack of the given size.
		 */
		LSBOOL LSE_CALL							AddStack( LSA_SIZE _sSize );

		/**
		 * Finds a stack with enough space for the requested allocation given an alignment.
		 *
		 * \param _sSize The requested size.
		 * \param _ui32Align The requested allignment.
		 * \return Returns an existing stack if it is large enough for the allocation or NULL if none are.
		 */
		LSA_STACK * LSE_CALL					FindStack( LSA_SIZE _sSize, uint32_t _ui32Align );

		/**
		 * Finalizes an allocation on a given stack.
		 *
		 * \param _psStack The stack to finalize.  The current allocation becomes final and a new one begins.
		 */
		static LSE_INLINE void LSE_CALL		FinalizeStack( LSA_STACK * _psStack );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Finishes an allocation.  After this is called, if the next call is ReAlloc(), it will revert to calling Alloc().
	 *	Alloc() automatically calls this internally, so 3 patterns exist for using this allocator:
	 * #1: Alloc() ReAlloc() ReAlloc(), Alloc() ReAlloc() ReAlloc() [etc.]
	 * #2: ReAlloc() ReAlloc() ReAlloc() Done(), ReAlloc() ReAlloc() ReAlloc() Done() [etc.]
	 * #1: Alloc() ReAlloc() ReAlloc() Done(), Alloc() ReAlloc() ReAlloc()  Done() [etc.]
	 * These patterns produce identical results.
	 */
	LSE_INLINE void LSE_CALL CDynamicStackAllocator::Done() {
		if ( m_psLast ) {
			FinalizeStack( m_psLast );
		}
	}

	/**
	 * Frees the last allocation.
	 */
	LSE_INLINE void LSE_CALL CDynamicStackAllocator::Free() {
		if ( m_psLast ) {
			m_psLast->sCurSize = 0;
		}
	}

	/**
	 * Gets the size of the current allocation.
	 *
	 * \return Returns the size of the current allocation.
	 */
	LSE_INLINE LSA_SIZE LSE_CALL CDynamicStackAllocator::CurSize() const {
		return m_psLast ? m_psLast->sCurSize : 0;
	}

	/**
	 * Finalizes an allocation on a given stack.
	 *
	 * \param _psStack The stack to finalize.  The current allocation becomes final and a new one begins.
	 */
	LSE_INLINE void LSE_CALL CDynamicStackAllocator::FinalizeStack( LSA_STACK * _psStack ) {
		assert( _psStack->sCurSize <= _psStack->sSize );
		_psStack->sSize -= _psStack->sCurSize;
		_psStack->pui8CurStart += _psStack->sCurSize;
		_psStack->sCurSize = 0;
	}

}	// namespace lsa

#endif	// __LSA_DYNAMICSTACKALLOCATOR_H__
