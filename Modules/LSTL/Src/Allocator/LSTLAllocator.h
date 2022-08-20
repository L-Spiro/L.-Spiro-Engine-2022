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
 * Description: Provide a base interface for allocation.  All objects in this library use this when they
 *	want to allocate RAM.  Inherit from this and modify the Alloc, ReAlloc, and Free functions, then pass
 *	your class to the templates in this library to make them use your allocation methods instead.
 */


#ifndef __LSTL_ALLOCATOR_H__
#define __LSTL_ALLOCATOR_H__

#include "../LSTLib.h"
#include "Allocators/LSAStdAllocator.h"
#include "CriticalSection/LSHCriticalSection.h"

namespace lstl {

	/**
	 * Class CAllocator
	 * \brief Basic allocation methods.
	 *
	 * Description: Provide a base interface for allocation.  All objects in this library use this when they
	 *	want to allocate RAM.  Inherit from this and modify the Alloc, ReAlloc, and Free functions, then pass
	 *	your class to the templates in this library to make them use your allocation methods instead.
	 */
	class CAllocator {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CAllocator();
		virtual LSE_CALLCTOR				~CAllocator();


		// == Functions.
		/**
		 * Allocates.
		 *
		 * \param _uiptrSize Number of bytes to allocate.
		 * \return Returns the allocated bytes or NULL on failure.
		 */
		virtual void * LSE_CALL			Alloc( uintptr_t _uiptrSize );

		/**
		 * Reallocate memory.
		 *
		 * \param _pvAddr The address whose allocation size is to be changed.  If NULL, this behaves
		 *	exactly the same way as Alloc().
		 * \param _uiptrSize The new size of the allocated data.
		 * \return Returns the reallocated pointer or NULL on failure.  Upon failure, the original
		 *	data is not modified and _pvAddr remains valid.
		 */
		virtual void * LSE_CALL			ReAlloc( void * _pvAddr, uintptr_t _uiptrSize );

		/**
		 * Free memory previously allocated with Alloc() or ReAlloc().
		 *
		 * \param _pvAddr The address to free or NULL.
		 */
		virtual void LSE_CALL				Free( void * _pvAddr );

		/**
		 * Sets the allocator we use.
		 *
		 * \param _psaAllocator The allocator to be used by this object.
		 */
		void LSE_CALL						SetAllocator( CStdAllocator * _psaAllocator );

		/**
		 * Gets the allocator used by this object.
		 *
		 * \return Returns the allocator used by this object.
		 */
		CStdAllocator * LSE_CALL			GetAllocator();

		/**
		 * Gets the allocator used by this object.
		 *
		 * \return Returns the allocator used by this object.
		 */
		const CStdAllocator * LSE_CALL		GetAllocator() const;

		/**
		 * Trash this allocator.  Assumes a special heap has been assigned to this allocator
		 *	and that heap is about to be trashed.
		 */
		void LSE_CALL						Trash();


	protected :
		// == Members.
		/** The base allocator. */
		CStdAllocator *						m_psaAllocator;

		/** The number of allocations we have made. */
		uint32_t							m_ui32TotalAllocs;

		/** Allocation count.  Very simple counter that increases with each allocation and
		 *	decreases with each release.  If not 0 at the end of the game's life, something was leaked or
		 *	over-deallocated. */
		static uint32_t						m_ui32AllocCount;

		/** Class counter.  Keeps track of the number of this object in existance.  When this reaches 0,
		 *	m_ui32AllocCount is checked for being 0.  If not, debug information is printed. */
		static uint32_t						m_ui32ObjCount;

		/** Thread safety. */
		static CCriticalSection *			m_pcsCrit;

	};

}	// namespace lstl

#endif	// __LSTL_ALLOCATOR_H__
