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
 * Description: Building off the low-level file routines, this class provides higher-level file
 *	functionality such as loading files to RAM.
 */

#include "LSFFilesEx.h"
#include "String/LSTLWString.h"
#include <cassert>


namespace lsf {

	// == Members.
	/**
	 * Default allocator.
	 */
	CStdAllocator * CFilesEx::m_psaAllocator = NULL;

	// == Functions.
	/**
	 * Initialize the extended file functionality.  Required before LoadFileToMemory() and FreeFile() can be used with a NULL allocator.
	 */
	void LSE_CALL CFilesEx::InitFilesEx() {
		if ( m_psaAllocator ) { return; }
		m_psaAllocator = CMemLib::GetAllocatorD( 8 * 1024 * 1024, true );
	}

	/**
	 * Shut down the extended file functionality when the engine closes.
	 */
	void LSE_CALL CFilesEx::ShutDownFilesEx() {
		if ( !m_psaAllocator ) { return; }
		CMemLib::DestroyAllocator( m_psaAllocator );
		m_psaAllocator = NULL;
	}

	/**
	 * Load a file fully to RAM.
	 *
	 * \param _pcFile Path to the file to load.
	 * \param _pui8Buffer Holds the returned file data.  This buffer is allocated using _psaAllocator.
	 * \param _puiptrSize Optional value to hold the size of the loaded file.  If not needed, set to NULL.
	 * \param _psaAllocator Optional pointer to the allocator to use to allocate _pui8Buffer.
	 * \return Returns true on success, false otherwise.
	 */
	LSBOOL LSE_CALL CFilesEx::LoadFileToMemory( const char * _pcFile, uint8_t * &_pui8Buffer, uintptr_t * _puiptrSize,
		CStdAllocator * _psaAllocator ) {
		if ( !_psaAllocator ) {
			_psaAllocator = m_psaAllocator;
		}
		// Open the file.
		LSF_FILE fFile = CFileLib::OpenFile( _pcFile, LSF_OF_READ, false );
		if ( !fFile ) { return false; }

		// Get its size.
		uint64_t ui64Size = CFileLib::GetFileSize( fFile );
		// Copy.
		if ( _puiptrSize ) {
			(*_puiptrSize) = static_cast<uintptr_t>(ui64Size);
		}

		// Check.
		if ( !ui64Size ) {
			// Empty file; impossible to allocate anything.
			_pui8Buffer = NULL;
			CFileLib::CloseFile( fFile );
			return true;
		}

		// Allocate.
		_pui8Buffer = static_cast<uint8_t *>(_psaAllocator->Alloc( static_cast<uintptr_t>(ui64Size) ));
		if ( !_pui8Buffer ) {
			// Failed to allocate.
			CFileLib::CloseFile( fFile );
			return false;
		}

		// Read.
		if ( !CFileLib::ReadFile( fFile, _pui8Buffer, static_cast<uintptr_t>(ui64Size) ) ) {
			// Failed to read.
			_psaAllocator->Free( _pui8Buffer );
			CFileLib::CloseFile( fFile );
			return false;
		}

		// End happily.
		CFileLib::CloseFile( fFile );
		return true;
	}

	/**
	 * Removes from RAM a file that was loaded via LoadFileToMemory().
	 *
	 * \param _pui8Buffer The file data previously filled by a successful call to LoadFileToMemory().
	 * \param _psaAllocator Optional pointer to the allocator used to allocate _pui8Buffer.  This should be the same value that was passed
	 *	to LoadFileToMemory(), even (especially) if NULL was passed.
	 */
	void LSE_CALL CFilesEx::FreeFile( uint8_t * _pui8Buffer, CStdAllocator * _psaAllocator ) {
		if ( !_psaAllocator ) {
			_psaAllocator = m_psaAllocator;
		}
		_psaAllocator->Free( _pui8Buffer );
		if ( _psaAllocator == m_psaAllocator ) {
			// Large files would cause a permanent footprint if not for this.
			m_psaAllocator->ReleaseEmptyHeaps();
		}
	}

	/**
	 * Write a buffer to a file.  If the file exists its contents are erased.
	 *
	 * \param _pcPath Pointer to a string containing the path of the file to create.  Its contents will be overwritten.
	 * \param _pui8Buffer The buffer to write to the given file path.
	 * \param _uiptrSize Number of bytes to write.
	 * \return Returns true if the file was created and its contents written.
	 */
	LSBOOL LSE_CALL CFilesEx::SendBufferToFile( const char * _pcPath, const uint8_t * _pui8Buffer, uintptr_t _uiptrSize ) {
		LSF_FILE fFile = CFileLib::OpenFile( _pcPath, LSF_OF_WRITE, true );
		if ( !fFile ) { return false; }

		if ( !CFileLib::WriteFile( fFile, _pui8Buffer, _uiptrSize ) ) {
			CFileLib::CloseFile( fFile );
			return false;
		}
		CFileLib::CloseFile( fFile );
		return true;
	}

	/**
	 * Lists all the files and folders in a given directory.
	 * 
	 * \param _pcFolderPath The path to the directory to search.
	 * \param _pcSearchString A wildcard search string to find only certain files/folders.
	 * \param _bIncludeFolders If true, folders are included in the return.
	 * \param _slReturn Thereturn array.  Found files and folders are appended to the array.
	 * \return Returns true if at least 1 file/folder was found and there was enough memory to add all found files/folders to _slReturn.
	 **/
	LSBOOL LSE_CALL CFilesEx::GetFilesInDir( const char * _pcFolderPath, const char * _pcSearchString, bool _bIncludeFolders, CStringList &_slReturn ) {
#ifdef LSE_WINDOWS
		CString sPath = _pcFolderPath;
		sPath.FindAndReplaceChar( '\\', '/' );
		while ( sPath.GetLastChar() == '/' ) { sPath.RemLastChar(); }
		sPath.Append( '/' );
		CString sSearch;
		if ( _pcSearchString ) {
			sSearch = _pcSearchString;
			sSearch.FindAndReplaceChar( '\\', '/' );
			while ( sSearch.CStr()[0] == '/' ) {
				sSearch.RemChar( 0 );
			}
		}
		else {
			sSearch = "*";
		}


		CString sSearchPath = sPath + sSearch;
		if ( sSearchPath.FindString( 0, "\\\\?\\" ).ui32Start != 0 ) {
			if ( !sSearchPath.Insert( 0, "\\\\?\\" ) ) { return false; }
		}
		WIN32_FIND_DATAA wfdData;
		HANDLE hDir = ::FindFirstFileA( sSearchPath.CStr(), &wfdData );
		if ( INVALID_HANDLE_VALUE == hDir ) { return false; }
		
		do {
			if ( wfdData.cFileName[0] == '.' ) { continue; }
			bool bIsFolder = ((wfdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if ( !_bIncludeFolders && bIsFolder ) {
				continue;
			}
			if ( !_slReturn.Push( sPath + wfdData.cFileName ) ) { return false; }
		} while ( ::FindNextFileA( hDir, &wfdData ) );

		::FindClose( hDir );
		return true;
#else

		return false;
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Lists all the files and folders in a given directory.
	 * 
	 * \param _pwcFolderPath The path to the directory to search.
	 * \param _pwcSearchString A wildcard search string to find only certain files/folders.
	 * \param _bIncludeFolders If true, folders are included in the return.
	 * \param _slReturn Thereturn array.  Found files and folders are appended to the array.
	 * \return Returns true if at least 1 file/folder was found and there was enough memory to add all found files/folders to _slReturn.
	 **/
	LSBOOL LSE_CALL CFilesEx::GetFilesInDir( const wchar_t * _pwcFolderPath, const wchar_t * _pwcSearchString, bool _bIncludeFolders, CStringList &_slReturn ) {
#ifdef LSE_WINDOWS
		CWString sPath = _pwcFolderPath;
		sPath.FindAndReplaceChar( L'\\', L'/' );
		while ( sPath.GetLastChar() == L'/' ) { sPath.RemLastChar(); }
		sPath.Append( L'/' );
		CWString sSearch;
		if ( _pwcSearchString ) {
			sSearch = _pwcSearchString;
			sSearch.FindAndReplaceChar( L'\\', L'/' );
			while ( sSearch.CStr()[0] == L'/' ) {
				sSearch.RemChar( 0 );
			}
		}
		else {
			sSearch = "*";
		}


		CWString sSearchPath = sPath + sSearch;
		/*if ( sSearchPath.FindString( 0, L"\\\\?\\" ).ui32Start != 0 ) {
			if ( !sSearchPath.Insert( 0, L"\\\\?\\" ) ) { return false; }
		}*/
		WIN32_FIND_DATAW wfdData;
		HANDLE hDir = ::FindFirstFileW( sSearchPath.CStr(), &wfdData );
		if ( INVALID_HANDLE_VALUE == hDir ) { return false; }
		
		do {
			if ( wfdData.cFileName[0] == L'.' ) { continue; }
			bool bIsFolder = ((wfdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if ( !_bIncludeFolders && bIsFolder ) {
				continue;
			}
			if ( !_slReturn.Push( CString::CStringFromUtfX( (sPath + wfdData.cFileName).CStr() ) ) ) { return false; }
		} while ( ::FindNextFileW( hDir, &wfdData ) );

		::FindClose( hDir );
		return true;
#else

		return false;
#endif	// #ifdef LSE_WINDOWS
	}

}	// namespace lsf
