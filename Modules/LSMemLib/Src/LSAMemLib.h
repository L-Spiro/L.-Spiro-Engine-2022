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
 * Description: The base of the memory manager.
 */


#ifndef __LSA_MEMLIB_H__
#define __LSA_MEMLIB_H__

#include "OSHeap/LSAOsHeap.h"
#include "Allocators/LSAStdAllocator.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// It is better to call the full version of new, as:
//	new( __FILE__, __LINE__ ).
// Use the LSENEW macro to make this easier.
#ifndef LSA_TOOLS
#define LSENEW										new( static_cast<lsa::LSA_NEW>(lsa::LSA_N_NEW), static_cast<const char *>(__FILE__), static_cast<uint32_t>(__LINE__) )
#define LSEDELETE									delete
#else
#define LSENEW										new
#define LSEDELETE									delete
#endif	// #ifndef LSA_TOOLS

#if !defined( __CRTDECL )
#define __CRTDECL
#endif  // #if !defined( __CRTDECL )

namespace lsa {

	// == Types.
	/**
	 * For our own operator new.
	 */
	enum LSA_NEW {
		LSA_N_NEW
	};

	/**
	 * Class CMemLib
	 *
	 * Description: Initialize the memory manager, then allocate from it.
	 */
	class CMemLib {
	public :
		// == Functions.
		/**
		 * Initialize the memory manager.
		 *
		 * \param _sDefaultSize The default size of the heap.
		 * \param _bGrowable Whether the heap can grow.
		 * \return Returns true if the memory manager was initialized.
		 */
		static LSBOOL LSE_CALL						Init( LSA_SIZE _sDefaultSize, LSBOOL _bGrowable );

		/**
		 * Destroy the memory manager.  Called once when the game is shutting down.
		 */
		static void LSE_CALL						Destroy();

		/**
		 * Gets an allocator from the memory manager.  Having an extra allocator is like another pool of memory from which you
		 *	can allocate memory.  It is not necessary to use the memory manager.
		 *
		 * \param _sDefaultSize The default size of the heap.
		 * \param _bGrowable Whether the heap can grow.
		 * \return Returns a pointer to the new allocator or NULL if not enough memory exists to create an allocator.
		 */
		static CStdAllocator * LSE_CALL				GetAllocatorD( LSA_SIZE _sDefaultSize, LSBOOL _bGrowable LSA_DEBUGPARMSDECL );

		/**
		 * Destroy an allocator.
		 *
		 * \param _psaAllocator The allocator to destroy.
		 * \return Returns true if the given allocator exists and is destroyed.
		 */
		static LSBOOL LSE_CALL						DestroyAllocator( CStdAllocator * _psaAllocator );

		/**
		 * Allocates memory.
		 *
		 * \param _sSize Amount of memory to allocate.
		 * \param _ulAlign Alignment.  Can be 16 or 32.  Any other number is rounded down to one of these values.
		 * \return Returns the allocated memory or NULL if the allocation failed.
		 */
		static void * LSE_CALL					AllocD( LSA_SIZE _sSize, uint32_t _ulAlign = 0 LSA_DEBUGPARMSDECL );

		/**
		 * Allocates memory, setting the allocated memory to 0's.
		 *
		 * \param _sSize Amount of memory to allocate.
		 * \param _ulAlign Alignment.  Can be 16 or 32.  Any other number is rounded down to one of these values.
		 * \return Returns the allocated memory or NULL if the allocation failed.
		 */
		static void * LSE_CALL					CAllocD( LSA_SIZE _sSize, uint32_t _ulAlign = 0 LSA_DEBUGPARMSDECL );

		/**
		 * Free memory.
		 *
		 * \param _pvAddr Address to free.
		 * \return Returns true if the address was allocated by this memory manager.
		 */
		static LSBOOL LSE_CALL						Free( void * _pvAddr );

		/**
		 * Reallocate memory.  The data at the given address is moved to the new address if and only if a new address is returned.
		 *
		 * \param _pvAddr The address to reallocate.
		 * \param _sSize The new size of the address.
		 * \return Returns the new address of the allocated data or NULL.  If NULL is returned, _pvAddr still points to allocated
		 *	memory that must be freed.
		 */
		static void * LSE_CALL					ReAllocD( void * _pvAddr, LSA_SIZE _sSize LSA_DEBUGPARMSDECL );
		
		/**
		 * Are we ready for allocation?
		 *
		 * \return Returns true if Init() has been called and Destroy() has not.
		 */
		static LSBOOL LSE_CALL						Ready();
		
		/**
		 * Release empty pools excluding the initial pool.
		 */
		static void LSE_CALL						ReleaseEmptyPools();

		/**
		 * Gets the allocation counter.  Used for debugging.  If LSA_DEBUG is not defined, this always returns 0.
		 *	This does not return the actual number of allocations.  It returns the unique allocation count to
		 *	be assigned to the next allocation.
		 *
		 * \return Returns the allocation counter, or 0 if LSA_DEBUG is not defined.
		 */
		static uint32_t LSE_CALL					GetAllocationCounter();

		/**
		 * Print allocations with an allocation counter above or equal to the given number.  If LSA_DEBUG is
		 *	not defined, this does nothing.  Prints only allocations on the default memory allocator.
		 *
		 * \param _ui32Count Allocations with an allocation number above or equal to this value will be printed.
		 *	Use 0 to print all allocations.
		 * \param _ui32End Ending allocation number up to which to print allocations.
		 * \return Returns the combined size of the allocations printed.
		 */
		static LSA_SIZE LSE_CALL					PrintAllocations( uint32_t _ui32Count = 0, uint32_t _ui32End = 0xFFFFFFFFUL );

		/**
		 * Gets the total amount of RAM allocated from the main heap.  This is the amount of RAM that the main heap has
		 *	allocated from the system for its own use, and includes the RAM that it gives to the application upon
		 *	allocation requests.
		 *
		 * \return Returns the total amount of RAM allocated by the main heap.
		 */
		static LSA_SIZE LSE_CALL					GetTotalAllocatedSize();

		/**
		 * Creates a new object using an allocator.
		 *
		 * \param _psaAllocator The allocator to use to create the new object of the given type.
		 * \return Returns the newly allocated and constructed object or NULL if there is not enough memory to
		 *	perform the operation.
		 */
		template <class _cType>
		static _cType * LSE_CALL					New( CStdAllocator * _psaAllocator LSA_DEBUGPARMSDECL );

		/**
		 * Delete an object using an allocator.
		 *
		 * \param _psaAllocator The allocator to use to delete the object.
		 * \param _pcObject Pointer to the object to delete.
		 */
		template <class _cType>
		static void LSE_CALL						Delete( CStdAllocator * _psaAllocator, _cType * _pcObject );

#ifdef LSA_DEBUG
		static CStdAllocator * LSE_CALL				GetThisAllocator()					{ return &m_saAllocator; }
#endif	// #ifdef LSA_DEBUG

#ifdef LSA_DEBUG
#define	MAllocAligned( _sSize, _ulAlign )			AllocD( _sSize, _ulAlign, __FILE__, __LINE__ )
#define	MAlloc( _sSize )							AllocD( _sSize, 0, __FILE__, __LINE__ )
#define	CMAllocAligned( _sSize, _ulAlign )			CAllocD( _sSize, _ulAlign, __FILE__, __LINE__ )
#define	CMAlloc( _sSize )							CAllocD( _sSize, 0, __FILE__, __LINE__ )
#define	MReAlloc( _pvAddr, _sSize )					ReAllocD( _pvAddr, _sSize, __FILE__, __LINE__ )
#define	MGetAllocator( _sDefaultSize, _bGrowable )	GetAllocatorD( _sDefaultSize, _bGrowable, __FILE__, __LINE__ )
#define	MNew( _psaAllocator, _cType )				New<_cType>( _psaAllocator, __FILE__, __LINE__ )
#define	MDelete( _psaAllocator, _cType, _pcObject )	Delete<_cType>( _psaAllocator, _pcObject )
#else	// #ifdef LSA_DEBUG
#define	MAllocAligned( _sSize, _ulAlign )			AllocD( _sSize, _ulAlign )
#define	MAlloc( _sSize )							AllocD( _sSize, 0 )
#define	CMAllocAligned( _sSize, _ulAlign )			CAllocD( _sSize, _ulAlign )
#define	CMAlloc( _sSize )							CAllocD( _sSize, 0 )
#define	MReAlloc( _pvAddr, _sSize )					ReAllocD( _pvAddr, _sSize )
#define	MGetAllocator( _sDefaultSize, _bGrowable )	GetAllocatorD( _sDefaultSize, _bGrowable )
#define	MNew( _psaAllocator, _cType )				New<_cType>( _psaAllocator )
#define	MDelete( _psaAllocator, _cType, _pcObject )	Delete<_cType>( _psaAllocator, _pcObject )
#endif	// #ifdef LSA_DEBUG

	private :
		// == Members.
		/**
		 * The object we use for allocation.
		 */
		static CStdAllocator						m_saAllocator;
		
		/**
		 * If the allocator has not been constructed then we cannot use it (new/delete overloads cannot use it).
		 */
		static LSBOOL								m_bAllocatorReady;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Are we ready for allocation?
	 *
	 * \return Returns true if Init() has been called and Destroy() has not.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMemLib::Ready() {
		return m_bAllocatorReady;
	}

	/**
	 * Creates a new object using an allocator.
	 *
	 * \param _psaAllocator The allocator to use to create the new object of the given type.
	 * \return Returns the newly allocated and constructed object or NULL if there is not enough memory to
	 *	perform the operation.
	 */
	template <class _cType> _cType * LSE_CALL CMemLib::New( CStdAllocator * _psaAllocator LSA_DEBUGPARMSDEF ) {
		if ( !_psaAllocator ) { return LSENEW _cType(); }
		void * pvNew = _psaAllocator->Alloc( sizeof( _cType ), 16 LSA_DEBUGPARMSPASS );
		if ( !pvNew ) { return NULL; }
		return new( pvNew ) _cType();
	}

	/**
	 * Delete an object using an allocator.
	 *
	 * \param _psaAllocator The allocator to use to delete the object.
	 * \param _pcObject Pointer to the object to delete.
	 */
	template <class _cType> void LSE_CALL CMemLib::Delete( CStdAllocator * _psaAllocator, _cType * _pcObject ) {
		if ( !_pcObject ) { return; }
		if ( !_psaAllocator ) {
			LSEDELETE _pcObject;
			return;
		}
		_pcObject->~_cType();
		_psaAllocator->Free( _pcObject );
	}

}	// namespace lsa


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// OVERRIDES
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef LSA_TOOLS
#ifdef __GNUC__
#define LSA_THROW									throw()
#define LSA_THROW_BAD								throw( std::bad_alloc )
#else
#define LSA_THROW									
#define LSA_THROW_BAD								
#endif	// #ifdef __GNUC__
// Standard new.
void * __CRTDECL									operator new( size_t _stSize, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void * __CRTDECL									operator new( size_t _stSize, lsa::LSA_NEW ) LSA_THROW_BAD;
// Standard delete.
void __CRTDECL										operator delete( void * _pvAddr, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
//void __CRTDECL										operator delete( void * _pvAddr ) LSA_THROW;

// Array new.
void * __CRTDECL									operator new [] ( size_t _stSize, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void * __CRTDECL									operator new [] ( size_t _stSize, lsa::LSA_NEW ) LSA_THROW_BAD;
// Array delete.
void __CRTDECL										operator delete [] ( void * _pvAddr, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
//void __CRTDECL										operator delete [] ( void * _pvAddr ) LSA_THROW;

// No-throw new.
void * __CRTDECL									operator new( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void * __CRTDECL									operator new( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW ) LSA_THROW;
// No-throw delete.
void __CRTDECL										operator delete( void * _pvAddr, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void __CRTDECL										operator delete( void * _pvAddr, const std::nothrow_t &_tNoThrow ) LSA_THROW;

// Array no-throw new.
void * __CRTDECL									operator new [] ( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void * __CRTDECL									operator new [] ( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW ) LSA_THROW;
// Array no-throw delete.
void __CRTDECL										operator delete [] ( void * _pvAddr, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line );
void __CRTDECL										operator delete [] ( void * _pvAddr, const std::nothrow_t &_tNoThrow ) LSA_THROW;

#endif	// #ifndef LSA_TOOLS
#endif	// __LSA_MEMLIB_H__
