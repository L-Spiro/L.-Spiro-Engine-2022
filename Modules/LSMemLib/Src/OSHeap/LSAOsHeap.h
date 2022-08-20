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
 * Description: Wrappers for the OS-dependent heap functions.
 */


#ifndef __LSA_OSHEAP_H__
#define __LSA_OSHEAP_H__

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the thread library.
#include "LSHThreadLib.h"
using namespace ::lsh;

#include "CriticalSection/LSHCriticalSection.h"

#include <cassert>

// Special definitions.
// For Windows.
#if defined( LSE_WINDOWS )
#include "Windows/LSSTDWindows.h"

#define LSA_SUPERSIZE						uint64_t
#define LSA_MEMHANDLE						HANDLE
#endif	// #if defined( LSE_WINDOWS )

// For Macintosh OS X and Linux.
#if defined( LSE_MAC ) || defined( LSE_LINUX )
// Define __LSA_64BIT__ if you want to support 64-bit machines.
//#define __LSA_64BIT__

#define LSA_SUPERSIZE						uint64_t
#define LSA_MEMHANDLE						void *
#endif	// #if defined( LSE_MAC )

// For iOS.
#ifdef LSE_IPHONE
#include <cstdio>
#endif	// #ifdef LSE_IPHONE



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// TUNING
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Number of hash buckets used to quickly find addresses for deleting.
// Higher number takes more RAM but runs faster.  Must be a power of 2.
#ifdef LSE_IPHONE
#define LSA_BUCKETS							32
#else
#define LSA_BUCKETS							64
#endif	// #ifdef LSE_IPHONE

// The minimum alignment.
#define LSA_MIN_ALIGN_BITS					4
#define LSA_MIN_ALIGN						(1 << LSA_MIN_ALIGN_BITS)

// On systems that allow efficient floating-point operations we can use some
//	trickier math to disperse addresses through a hash table less evenly.  By
//	default, addresses are dispersed linearly across the range of addresses
//	covered by a heap block.  This means that all of the first allocations go
//	to the low hash indices and slowly build up.  In most cases, however, the
//	indices higher into the hash table never even get used, and performance
//	is lost.
// With floating-point math we can change the hash-key generation to
//	distribute addresses very widely along the low range, widely along the
//	middle range, and tightly at the high range, where it is less likely for
//	any allocations ever to be made.
#define LSA_USE_FLOAT_OPTIMIZATIONS

// To enable debugging, uncomment the following macro.
// Defaults to disabled on iPhone because the iPhone OS starts
//	using our override new/delete and then does not free its memory, which
//	fills our output screen with unreleased allocations during shutdown,
//	making the debugging functionality of this library mostly useless.
#ifndef LSE_IPHONE
#ifdef _DEBUG
#define LSA_DEBUG
#endif	// #ifdef _DEBUG
#endif	// #ifndef LSE_IPHONE



#ifdef LSA_DEBUG
#define LSA_DEBUGPARMSDECL					, const char * _pcFile = NULL, uint32_t _ui32Line = 0
#define LSA_DEBUGPARMSDEF					, const char * _pcFile, uint32_t _ui32Line
#define LSA_DEBUGPARMSPASS					, _pcFile, _ui32Line
#define LSA_DEBUGPARMS						, __FILE__, __LINE__
#else	// #ifdef LSA_DEBUG
#define LSA_DEBUGPARMSDECL
#define LSA_DEBUGPARMSDEF
#define LSA_DEBUGPARMSPASS
#define LSA_DEBUGPARMS				
#endif	// #ifdef LSA_DEBUG


namespace lsa {

	typedef uintptr_t						LSA_SIZE;
	

	/**
	 * Class COsHeap
	 *
	 * Description: Use these functions instead of the operating-system functions.
	 *	They cannot be called except by friends of this class.
	 */
	class COsHeap {
		friend class						CDynamicStackAllocator;
		friend class						CMemLib;
		friend class						CStackAllocator;
		friend class						CStdAllocator;


		// Protect everything to deny unauthorized access.
	protected :
		// == Functions.
		/**
		 * Initialize the allocator.  _sSize changes meaning per operating system.
		 * On Windows, it is ignored.
		 *
		 * \param _sSize Currently unused.
		 */
		static void LSE_CALL				Init( LSA_SIZE _sSize );

		/**
		 * Destroy the allocator.
		 */
		static void LSE_CALL				Destroy();

		/**
		 * Allocates bytes.
		 *
		 * \param _sSize Number of bytes to allocate.
		 * \return Returns the allocated bytes or NULL.
		 */
		static void * LSE_CALL			Alloc( LSA_SIZE _sSize );

		/**
		 * Free bytes.
		 *
		 * \param _pvAddr Address of the bytes allocated by a previous call to Alloc() to be freed.
		 * \return Returns true if the address exists in this heap and was freed.
		 */
		static LSBOOL LSE_CALL				Free( void * _pvAddr );


	private :
#ifdef LSE_IPHONE
		// == Types.
		/**
		 * Maps file handles and memory addresses.
		 */
		typedef struct LSA_FILE_MAP {
			/**
			 * The handle to the file for this addresses.
			 */
			FILE *							pfFile;
			
			/**
			 * The address associated with the file handle.
			 */
			void *						pvAddr;
			
			/**
			 * The size of the mapping.
			 */
			LSA_SIZE						sSize;
			
			/**
			 * File number.
			 */
			uint32_t						ui32FileNumber;
		} * LPLSA_FILE_MAP, * const LPCLSA_FILE_MAP;
#endif	// #ifdef LSE_IPHONE


		// == Members.
		/**
		 * OS-specific handle to the heap.
		 */
		static LSA_MEMHANDLE				m_mhHeapHandle;

		/**
		 * Have we initialized?
		 */
		static volatile LSBOOL				m_bInit;

		/**
		 * OS-specific thread safety.
		 */
		static CCriticalSection				m_csCrit;
		
#ifdef LSE_IPHONE
		/**
		 * File handles and their respective addresses.
		 */
		static LSA_FILE_MAP *				m_pfmFileMaps;
		
		/**
		 * Total mapped files.
		 */
		static uint32_t						m_ui32TotalMaps;
		
		/**
		 * Total allocated files.
		 */
		static uint32_t						m_ui32AllocMaps;
		
		/**
		 * File ID numbers.
		 */
		static uint32_t						m_ui32FileId;
		
		
		// == Functions.
		/**
		 * Creates a path to a file that is to be used for memory mapping.
		 *
		 * \param _ui32Index Index of the file to be created.  Used as part of the file name.
		 * \param _pcReturn The character array to be filled with the returned path.
		 */
		static void LSE_CALL				MakeMemMapPath( uint32_t _ui32Index, char * _pcReturn );
#endif	// #ifdef LSE_IPHONE
	};

}	// namespace lsa

#endif	// __LSA_OSHEAP_H__
