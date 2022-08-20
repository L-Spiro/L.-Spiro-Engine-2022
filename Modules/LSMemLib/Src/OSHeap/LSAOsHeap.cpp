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

#include "LSAOsHeap.h"

#ifdef LSE_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#include <fcntl.h>			// For ::open().
#include <sys/mman.h>		// For ::mmap().
#include <unistd.h>			// For ::close().
#endif	// #ifdef LSE_IPHONE


namespace lsa {

	// == Members.
	/**
	 * OS-specific handle to the heap.
	 */
	LSA_MEMHANDLE COsHeap::m_mhHeapHandle = NULL;
	
	/**
	 * Have we initialized?
	 */
	volatile LSBOOL COsHeap::m_bInit = false;
	
	/**
	 * OS-specific thread safety.
	 */
	CCriticalSection COsHeap::m_csCrit;
	
#ifdef LSE_IPHONE
	/**
	 * File handles and their respective addresses.
	 */
	COsHeap::LSA_FILE_MAP * COsHeap::m_pfmFileMaps = NULL;
	
	/**
	 * Total mapped files.
	 */
	uint32_t COsHeap::m_ui32TotalMaps = 0;
	
	/**
	 * Total allocated files.
	 */
	uint32_t COsHeap::m_ui32AllocMaps = 0;
	
	/**
	 * File ID numbers.
	 */
	uint32_t COsHeap::m_ui32FileId = 0;
#endif	// #ifdef LSE_IPHONE

	// == Functions.
	/**
	 * Initialize the allocator.  _sSize changes meaning per operating system.
	 * On Windows, it is ignored.
	 *
	 * \param _sSize Currently unused.
	 */
	void LSE_CALL COsHeap::Init(
#if defined( LSE_WINDOWS )
	LSA_SIZE _sSize
#else
	LSA_SIZE /*_sSize*/
#endif  // #if defined( LSE_WINDOWS )
	) {
		if ( m_bInit ) { return; }
		CCriticalSection::CLocker lLock( m_csCrit );

		m_bInit = true;
#if defined( LSE_WINDOWS )
		// Remove warning C4100.
		static_cast<LSA_SIZE>(_sSize);
		m_mhHeapHandle = ::HeapCreate( 0, 0, 0 );
#elif defined( LSE_IPHONE )
		m_pfmFileMaps = static_cast<COsHeap::LSA_FILE_MAP *>(::malloc( sizeof( (*m_pfmFileMaps) ) * 32 ));
		if ( m_pfmFileMaps ) {
			m_ui32AllocMaps = 32;
		}
#endif	// #if defined( LSE_WINDOWS )
	}

	/**
	 * Destroy the allocator.
	 */
	void LSE_CALL COsHeap::Destroy() {
		if ( !m_bInit ) { return; }

		{
			CCriticalSection::CLocker lLock( m_csCrit );
#if defined( LSE_WINDOWS )
			::HeapDestroy( m_mhHeapHandle );
#elif defined( LSE_IPHONE )
			::free( m_pfmFileMaps );
			m_pfmFileMaps = NULL;
			m_ui32AllocMaps = m_ui32TotalMaps = 0;
#endif	// #if defined( LSE_WINDOWS )

			m_bInit = false;
		}
	}

	/**
	 * Allocates bytes.
	 *
	 * \param _sSize Number of bytes to allocate.
	 * \return Returns the allocated bytes or NULL.
	 */
	void * LSE_CALL COsHeap::Alloc( LSA_SIZE _sSize ) {
		CCriticalSection::CLocker lLock( m_csCrit );
#if defined( LSE_WINDOWS )
		assert( m_mhHeapHandle != NULL );
		return ::HeapAlloc( m_mhHeapHandle, 0, _sSize );
#elif defined( LSE_IPHONE )
		void * pvAddr;
		if ( _sSize < 8 * 1024 * 1024 ) {
			pvAddr = ::malloc( _sSize );
			if ( pvAddr ) { return pvAddr; }
			CStd::DebugPrintA( "Warning: Memory exhausted; using memory mapping.\r\n" );
		}

		if ( m_ui32TotalMaps == m_ui32AllocMaps ) {
			COsHeap::LSA_FILE_MAP * pfmNew = static_cast<COsHeap::LSA_FILE_MAP *>(::realloc( m_pfmFileMaps,
				sizeof( (*m_pfmFileMaps) ) * (m_ui32AllocMaps + 32) ));
			if ( !pfmNew ) { return NULL; }
			m_pfmFileMaps = pfmNew;
			m_ui32AllocMaps += 32;
		}
		m_pfmFileMaps[m_ui32TotalMaps].ui32FileNumber = m_ui32FileId;
		char szPath[PATH_MAX];
		MakeMemMapPath( m_ui32FileId++, szPath );
		
		m_pfmFileMaps[m_ui32TotalMaps].sSize = _sSize;
		
		// Create a scratch file.
		m_pfmFileMaps[m_ui32TotalMaps].pfFile = ::fopen( szPath, "wb+" );
		if ( !m_pfmFileMaps[m_ui32TotalMaps].pfFile ) {
			::remove( szPath );
			return NULL;
		}
		
		// Fill it to the proper size.  Has to be done by writing bytes to it.
		void * pvBuffer = ::malloc( 1024 * 1024 );
		LSA_SIZE sBufferSize;
		for ( LSA_SIZE I = _sSize; I; I -= sBufferSize ) {
			sBufferSize = CStd::Min<LSA_SIZE>( I, 1024 * 1024 );
			if ( !::fwrite( pvBuffer, 1, sBufferSize, m_pfmFileMaps[m_ui32TotalMaps].pfFile ) ) {
				::fclose( m_pfmFileMaps[m_ui32TotalMaps].pfFile );
				::free( pvBuffer );
				::remove( szPath );
				return NULL;
			}
		}
		::free( pvBuffer );
		
		// Go back to the start and map it.
		::fseek( m_pfmFileMaps[m_ui32TotalMaps].pfFile, 0, SEEK_SET );
		pvAddr = ::mmap( NULL, _sSize, PROT_READ | PROT_WRITE, MAP_SHARED,
			::fileno( m_pfmFileMaps[m_ui32TotalMaps].pfFile ), 0 );
			
			
		if ( pvAddr == MAP_FAILED ) {
			::fclose( m_pfmFileMaps[m_ui32TotalMaps].pfFile );
			::remove( szPath );
			return NULL;
		}
		m_pfmFileMaps[m_ui32TotalMaps++].pvAddr = pvAddr;
		return pvAddr;
#elif defined( LSE_MAC )
		return ::malloc( _sSize );
#elif defined( LSE_LINUX )
#error "No implementation available for COsHeap::Alloc() on Linux."
#endif
	}

	/**
	 * Free bytes.
	 *
	 * \param _pvAddr Address of the bytes allocated by a previous call to Alloc() to be freed.
	 * \return Returns true if the address exists in this heap and was freed.
	 */
	LSBOOL LSE_CALL COsHeap::Free( void * _pvAddr ) {
		CCriticalSection::CLocker lLock( m_csCrit );
#if defined( LSE_WINDOWS )
		assert( m_mhHeapHandle != NULL );
		return ::HeapFree( m_mhHeapHandle, 0, _pvAddr ) != 0;
#elif defined( LSE_IPHONE )
		// Just search for the memory map.
		for ( uint32_t I = m_ui32TotalMaps; I--; ) {
			if ( m_pfmFileMaps[I].pvAddr == _pvAddr ) {
				::munmap( _pvAddr, m_pfmFileMaps[I].sSize );
				::fclose( m_pfmFileMaps[I].pfFile );

				char szPath[PATH_MAX];
				MakeMemMapPath( m_pfmFileMaps[I].ui32FileNumber, szPath );
				::remove( szPath );
				CStd::MemMove( &m_pfmFileMaps[I], &m_pfmFileMaps[I+1], (m_ui32TotalMaps - I - 1) * sizeof( (*m_pfmFileMaps) ) );
				--m_ui32TotalMaps;
				if ( m_ui32TotalMaps == 0 ) {
					m_ui32AllocMaps = 0;
					::free( m_pfmFileMaps );
					m_pfmFileMaps = NULL;
				}
				return true;
			}
		}
		::free( _pvAddr );
		return true;	// No way to detect otherwise.
#elif defined( LSE_MAC )
		::free( _pvAddr );
		return true;	// No way to detect otherwise.
#elif defined( LSE_LINUX )
#error "No implementation available for COsHeap::Free() on Linux."
#endif
	}

}	// namespace lsa
