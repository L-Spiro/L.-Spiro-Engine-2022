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

#include "LSAMemLib.h"


namespace lsa {

	// == Members.
	/**
	 * The object we use for allocation.
	 */
	CStdAllocator CMemLib::m_saAllocator;
	
	/**
	 * If the allocator has not been constructed then we cannot use it (new/delete overloads cannot use it).
	 */
	LSBOOL CMemLib::m_bAllocatorReady = false;

	// == Functions.
	/**
	 * Initialize the memory manager.
	 *
	 * \param _sDefaultSize The default size of the heap.
	 * \param _bGrowable Whether the heap can grow.
	 * \return Returns true if the memory manager was initialized.
	 */
	LSBOOL LSE_CALL CMemLib::Init( LSA_SIZE _sDefaultSize, LSBOOL _bGrowable ) {
		// Initialize the operating-system heap manager.
		COsHeap::Init( 0 );
		if ( m_saAllocator.SetSize( _sDefaultSize, _bGrowable ) ) {
			m_bAllocatorReady = true;
			return true;
		}
		return false;
	}

	/**
	 * Destroy the memory manager.  Called once when the game is shutting down.
	 */
	void LSE_CALL CMemLib::Destroy() {
		m_saAllocator.Clear();
		m_bAllocatorReady = false;
		COsHeap::Destroy();
	}

	/**
	 * Gets an allocator from the memory manager.  Having an extra allocator is like another pool of memory from which you
	 *	can allocate memory.  It is not necessary to use the memory manager.
	 *
	 * \param _sDefaultSize The default size of the heap.
	 * \param _bGrowable Whether the heap can grow.
	 * \return Returns a pointer to the new allocator or NULL if not enough memory exists to create an allocator.
	 */
	CStdAllocator * LSE_CALL CMemLib::GetAllocatorD( LSA_SIZE _sDefaultSize, LSBOOL _bGrowable LSA_DEBUGPARMSDEF ) {
		void * pvNew = m_saAllocator.Alloc( sizeof( CStdAllocator ), LSA_MIN_ALIGN LSA_DEBUGPARMSPASS );
		if ( !pvNew ) { return NULL; }
		CStdAllocator * psaRet = reinterpret_cast<CStdAllocator *>(pvNew);
		new( psaRet ) CStdAllocator();
		psaRet->SetSize( _sDefaultSize, _bGrowable );
		return psaRet;
	}

	/**
	 * Destroy an allocator.
	 *
	 * \param _psaAllocator The allocator to destroy.
	 * \return Returns true if the given allocator exists and is destroyed.
	 */
	LSBOOL LSE_CALL CMemLib::DestroyAllocator( CStdAllocator * _psaAllocator ) {
		if ( !_psaAllocator ) { return true; }
		LSBOOL bRet = _psaAllocator->Clear();
		_psaAllocator->~CStdAllocator();
		LSBOOL bFreed = m_saAllocator.Free( _psaAllocator );
		return bRet && bFreed;
	}

	/**
	 * Allocates memory.
	 *
	 * \param _sSize Amount of memory to allocate.
	 * \param _ulAlign Alignment.  Can be 16 or 32.  Any other number is rounded down to one of these values.
	 * \return Returns the allocated memory or NULL if the allocation failed.
	 */
	void * LSE_CALL CMemLib::AllocD( LSA_SIZE _sSize, uint32_t _ulAlign LSA_DEBUGPARMSDEF ) {
		return m_saAllocator.Alloc( _sSize, _ulAlign LSA_DEBUGPARMSPASS );
	}

	/**
	 * Allocates memory, setting the allocated memory to 0's.
	 *
	 * \param _sSize Amount of memory to allocate.
	 * \param _ulAlign Alignment.  Can be 16 or 32.  Any other number is rounded down to one of these values.
	 * \return Returns the allocated memory or NULL if the allocation failed.
	 */
	void * LSE_CALL CMemLib::CAllocD( LSA_SIZE _sSize, uint32_t _ulAlign LSA_DEBUGPARMSDEF ) {
		return m_saAllocator.CAlloc( _sSize, _ulAlign LSA_DEBUGPARMSPASS );
	}

	/**
	 * Free memory.
	 *
	 * \param _pvAddr Address to free.
	 * \return Returns true if the address was allocated by this memory manager.
	 */
	LSBOOL LSE_CALL CMemLib::Free( void * _pvAddr ) {
		return m_saAllocator.Free( _pvAddr );
	}

	/**
	 * Reallocate memory.  The data at the given address is moved to the new address if and only if a new address is returned.
	 *
	 * \param _pvAddr The address to reallocate.
	 * \param _sSize The new size of the address.
	 * \return Returns the new address of the allocated data or NULL.  If NULL is returned, _pvAddr still points to allocated
	 *	memory that must be freed.
	 */
	void * LSE_CALL CMemLib::ReAllocD( void * _pvAddr, LSA_SIZE _sSize LSA_DEBUGPARMSDEF ) {
		return m_saAllocator.ReAlloc( _pvAddr, _sSize LSA_DEBUGPARMSPASS );
	}
	
	/**
	 * Release empty pools excluding the initial pool.
	 */
	void LSE_CALL CMemLib::ReleaseEmptyPools() {
		m_saAllocator.ReleaseEmptyHeaps();
	}

	/**
	 * Gets the allocation counter.  Used for debugging.  If LSA_DEBUG is not defined, this always returns 0.
	 *	This does not return the actual number of allocations.  It returns the unique allocation count to
	 *	be assigned to the next allocation.
	 *
	 * \return Returns the allocation counter, or 0 if LSA_DEBUG is not defined.
	 */
	uint32_t LSE_CALL CMemLib::GetAllocationCounter() {
		return CStdAllocatorHeap::GetAllocationCounter();
	}

	/**
	 * Print allocations with an allocation counter above or equal to the given number.  If LSA_DEBUG is
	 *	not defined, this does nothing.  Prints only allocations on the default memory allocator.
	 *
	 * \param _ui32Count Allocations with an allocation number above or equal to this value will be printed.
	 *	Use 0 to print all allocations.
	 * \param _ui32End Ending allocation number up to which to print allocations.
	 * \return Returns the combined size of the allocations printed.
	 */
	LSA_SIZE LSE_CALL CMemLib::PrintAllocations( uint32_t _ui32Count, uint32_t _ui32End ) {
		return m_saAllocator.PrintAllocations( _ui32Count, _ui32End );
	}

	/**
	 * Gets the total amount of RAM allocated from the main heap.  This is the amount of RAM that the main heap has
	 *	allocated from the system for its own use, and includes the RAM that it gives to the application upon
	 *	allocation requests.
	 *
	 * \return Returns the total amount of RAM allocated by the main heap.
	 */
	LSA_SIZE LSE_CALL CMemLib::GetTotalAllocatedSize() {
		return m_saAllocator.GetTotalAllocatedSize();
	}
	
}	// namespace lsa

// == Operators.
#if !defined( LSA_DEBUG ) && defined( LSE_IPHONE )
#define LSA_DEFER_TO_MALLOC
#endif	// #if !defined( LSA_DEBUG ) && defined( LSE_IPHONE )

#ifndef LSA_TOOLS

// Standard new.
void * __CRTDECL operator new( size_t _stSize, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line ) {
#ifdef LSE_WINDOWS
	// Remove warning C4100.
	static_cast<const char *>(_pcFile);
	static_cast<uint32_t>(_ui32Line);
#endif	// #ifdef LSE_WINDOWS
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, _pcFile, _ui32Line
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void * __CRTDECL operator new( size_t _stSize, lsa::LSA_NEW ) LSA_THROW_BAD {
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	void * pvAddr = !lsa::CMemLib::Ready() ? ::malloc( _stSize ) : lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, __FILE__, __LINE__
#endif	// #ifdef LSA_DEBUG
		);
	return pvAddr;
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
// Standard delete.
void __CRTDECL operator delete( void * _pvAddr, lsa::LSA_NEW, const char * /*_pcFile*/, uint32_t /*_ui32Line*/ ) {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void __CRTDECL operator delete( void * _pvAddr ) LSA_THROW {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC

}

// Array new.
void * __CRTDECL operator new [] ( size_t _stSize, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line ) {
#ifdef LSE_WINDOWS
	// Remove warning C4100.
	static_cast<const char *>(_pcFile);
	static_cast<uint32_t>(_ui32Line);
#endif	// #ifdef LSE_WINDOWS
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, _pcFile, _ui32Line
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void * __CRTDECL operator new [] ( size_t _stSize, lsa::LSA_NEW ) LSA_THROW_BAD {
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, __FILE__, __LINE__
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
// Array delete.
void __CRTDECL operator delete [] ( void * _pvAddr, lsa::LSA_NEW, const char * /*_pcFile*/, uint32_t /*_ui32Line*/ ) {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void __CRTDECL operator delete [] ( void * _pvAddr ) LSA_THROW {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}

// No-throw new.
void * __CRTDECL operator new( size_t _stSize, const std::nothrow_t &/*_tNoThrow*/, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line ) {
#ifdef LSE_WINDOWS
	// Remove warning C4100.
	static_cast<const char *>(_pcFile);
	static_cast<uint32_t>(_ui32Line);
#endif	// #ifdef LSE_WINDOWS
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, _pcFile, _ui32Line
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void * __CRTDECL operator new( size_t _stSize, const std::nothrow_t &/*_tNoThrow*/, lsa::LSA_NEW ) LSA_THROW {
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, __FILE__, __LINE__
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
// No-throw delete.
void __CRTDECL operator delete( void * _pvAddr, const std::nothrow_t &/*_tNoThrow*/, lsa::LSA_NEW, const char * /*_pcFile*/, uint32_t /*_ui32Line*/ ) {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void __CRTDECL operator delete( void * _pvAddr, const std::nothrow_t &/*_tNoThrow*/ ) LSA_THROW {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else

	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}

// Array no-throw new.
void * __CRTDECL operator new [] ( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW, const char * _pcFile, uint32_t _ui32Line ) {
#ifdef LSE_WINDOWS
	// Remove warning C4100.
	static_cast<const char *>(_pcFile);
	static_cast<uint32_t>(_ui32Line);
	static_cast<const std::nothrow_t &>(_tNoThrow);
#endif	// #ifdef LSE_WINDOWS
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, _pcFile, _ui32Line
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void * __CRTDECL operator new [] ( size_t _stSize, const std::nothrow_t &_tNoThrow, lsa::LSA_NEW ) LSA_THROW {
#ifdef LSE_WINDOWS
	// Remove warning C4100.
	static_cast<const std::nothrow_t &>(_tNoThrow);
#endif	// #ifdef LSE_WINDOWS
#ifdef LSA_DEFER_TO_MALLOC
	return ::malloc( _stSize );
#else
	if ( !lsa::CMemLib::Ready() ) {
		return ::malloc( _stSize );
	}
	return lsa::CMemLib::AllocD( _stSize, 0
#ifdef LSA_DEBUG
		, __FILE__, __LINE__
#endif	// #ifdef LSA_DEBUG
		);
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
// Array no-throw delete.
void __CRTDECL operator delete [] ( void * _pvAddr, const std::nothrow_t &/*_tNoThrow*/, lsa::LSA_NEW, const char * /*_pcFile*/, uint32_t /*_ui32Line*/ ) {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}
void __CRTDECL operator delete [] ( void * _pvAddr, const std::nothrow_t &/*_tNoThrow*/ ) LSA_THROW {
#ifdef LSA_DEFER_TO_MALLOC
	::free( _pvAddr );
#else
	if ( !lsa::CMemLib::Ready() ) {
		::free( _pvAddr );
		return;
	}
	
	if ( !lsa::CMemLib::Free( _pvAddr ) ) {
		::free( _pvAddr );
	}
#endif	// #ifdef LSA_DEFER_TO_MALLOC
}

#endif	// #ifndef LSA_TOOLS
