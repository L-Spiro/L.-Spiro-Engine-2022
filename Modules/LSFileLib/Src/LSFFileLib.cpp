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
 * Description: Handles opening, closing, reading from, and writing to files.  Also allows getting
 *	information on files such as size.
 */

#include "LSFFileLib.h"
#ifdef LSE_WINDOWS
#include <cassert>
#include <cstdio>			// for EOF.
#include <shlwapi.h>

// Windows defines a few macros that cause us trouble.
#ifdef StrCat
#undef StrCat
#endif	// #ifdef StrCat

#ifdef StrCpy
#undef StrCpy
#endif	// #ifdef StrCpy

#ifdef StrNCpy
#undef StrNCpy
#endif	// #ifdef StrNCpy

#ifdef StrNCat
#undef StrNCat
#endif	// #ifdef StrNCat
#endif	// #ifdef LSE_WINDOWS
#ifdef LSE_MAC
#include <new>				// For std::nothrow.
#ifdef __OBJC__
#include <Foundation/Foundation.h>
#else	// #ifdef __OBJC__
#include "String/LSTLWStringList.h"
#include <CoreFoundation/CoreFoundation.h>
#endif	// #ifdef __OBJC__
#include <fcntl.h>			// For ::open().
#include <sys/mman.h>		// For ::mmap().
#include <sys/stat.h>		// For ::stat().
#include <unistd.h>			// For ::close().
#endif	// #ifdef LSE_MAC



namespace lsf {

	// == Members.
	/**
	 * The resource path.
	 */
	char CFileLib::m_szResourcePath[LSF_MAX_PATH] = { 0 };

	/**
	 * Length of the resource path in characters.
	 */
	uint32_t CFileLib::m_ui32ResPathLen = 0;
	
	// == Functions.
	/**
	 * Open a file.  Returns NULL if failed.
	 *
	 * \param _pcFile Path to the file to open.  Must be UTF-8.
	 * \param _ofOpenType Flag determining how to open the file.  Valid values are LSF_OF_READ,
	 *	LSF_OF_WRITE, and LSF_OF_APPEND, and some combinations thereof.
	 * \param _bOpenAlways If true, the file is opened even if it does not exist (it is created).
	 * \return The handle to the opened file or NULL.
	 */
	LSF_FILE LSE_CALL CFileLib::OpenFile( const char * _pcFile, LSF_OPEN_FILE _ofOpenType, LSBOOL _bOpenAlways ) {
#ifdef LSE_WINDOWS
		wchar_t szBuffer[LSF_MAX_PATH];
		if ( !::MultiByteToWideChar( CP_UTF8,
			0,
			_pcFile,
			-1,
			szBuffer,
			LSF_MAX_PATH ) ) {
			// Conversion somehow failed.  This is really impossible but we have another chance.
			LSF_FILE hFile = ::CreateFileA( _pcFile, _ofOpenType, FILE_SHARE_READ,
				NULL, _bOpenAlways ?
					(_ofOpenType == LSF_OF_READ ? OPEN_ALWAYS : CREATE_ALWAYS) :	// Always open.
					OPEN_EXISTING,													// Open only if it exists.  Same for both read and write.
				NULL, NULL );
			if ( hFile == INVALID_HANDLE_VALUE ) { return NULL; }
			return hFile;
		}
		LSF_FILE hFile = ::CreateFileW( szBuffer, _ofOpenType, FILE_SHARE_READ,
			NULL, _bOpenAlways ?
				(_ofOpenType == LSF_OF_READ ? OPEN_ALWAYS : CREATE_ALWAYS) :	// Always open.
				OPEN_EXISTING,													// Open only if it exists.  Same for both read and write.
			NULL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE ) { return NULL; }
		return hFile;
#elif defined( LSE_MAC )
		CFStringRef srStr = ::CFStringCreateWithCString( NULL, _pcFile, kCFStringEncodingUTF8 );
		if ( !srStr ) {
			return NULL;
		}
		char szFinal[LSF_MAX_PATH] = "";
		if ( !::CFStringGetFileSystemRepresentation( srStr, szFinal, LSF_MAX_PATH ) ) {
			::CFRelease( srStr );
			return NULL;
		}
		::CFRelease( srStr );
		
		
		char cString[5] = "";
		struct LSF_OPEN_MODE_TABLEA {
			// String to pass down.
			const char *			szString;

			// Mode required.
			uint32_t				ui32Mode;
			
			// Setting of _bOpenAlways.
			LSBOOL					bOpenAlways;
		};
		static const LSF_OPEN_MODE_TABLEA omtTable[] = {
			{ "r",	LSF_OF_READ,								false },
			{ "w",	LSF_OF_WRITE,								false },
			{ "a",	LSF_OF_APPEND,								false },
			{ "r",	LSF_OF_READ,								true },
			{ "w",	LSF_OF_WRITE,								true },
			{ "a",	LSF_OF_APPEND,								true },
			{ "r+",	LSF_OF_READ | LSF_OF_WRITE,					false },
			{ "w+",	LSF_OF_READ | LSF_OF_WRITE,					true },
			{ "a+",	LSF_OF_READ | LSF_OF_WRITE | LSF_OF_APPEND,	false },
			{ "a+",	LSF_OF_READ | LSF_OF_WRITE | LSF_OF_APPEND,	true },
		};
		LSBOOL bFound = false;
		for ( uint32_t I = 0; I < LSE_ELEMENTS( omtTable ); ++I ) {
			if ( _ofOpenType == omtTable[I].ui32Mode && _bOpenAlways == omtTable[I].bOpenAlways ) {
				// This is it, but if not set to open always then check for the existance of the file.
				if ( !_bOpenAlways ) {
					if ( !Exists( szFinal ) ) {
						continue;
					}
				}
				// Found it.
				CStd::StrCpy( cString, omtTable[I].szString );
				bFound = true;
				break;
			}
		}
		if ( !bFound ) {
			return NULL;
		}
		CStd::StrCat( cString, "b" );
		LSF_FILE fRet = ::fopen( szFinal, cString );
		
		return fRet;
#endif	// LSE_WINDOWS
	}

	/**
	 * Close a file.  Sets the file pointer to NULL automatically.
	 *
	 * \param _fFile Handle to the open file to close.  This is set to an invalid handle after
	 *	being closed.
	 * \return Returns false if there was an error closing the handle.  If the handle is invalid,
	 *	returns true immediately.
	 */
	LSBOOL LSE_CALL CFileLib::CloseFile( LSF_FILE &_fFile ) {
		if ( !_fFile ) { return true; }

#ifdef LSE_WINDOWS
		if ( ::CloseHandle( _fFile ) ) {
			_fFile = NULL;
			return true;
		}
		return false;
#elif defined( LSE_MAC )
		if ( ::fclose( _fFile ) == 0 ) {
			_fFile = NULL;
			return true;
		}
		return false;
#endif	// LSE_WINDOWS
	}

	/**
	 * Read from a file.  Returns false under any form of error.  If true is returned, the full amount of
	 *	data was read from the file to the buffer.
	 *
	 * \param _fFile Handle to the opened file from which to read.
	 * \param _pvBuffer Buffer to store the data read from the file.  This must be long enough to hold
	 *	_uiptrBytesToRead bytes.
	 * \param _uiptrBytesToRead Number of bytes to read from the file.
	 * \return Returns false if the read operation failed for any reason.
	 */
	LSBOOL LSE_CALL CFileLib::ReadFile( LSF_FILE _fFile, void * _pvBuffer, uintptr_t _uiptrBytesToRead ) {
		if ( !_fFile ) { return false; }

#ifdef LSE_WINDOWS
		DWORD dwBytesRead = 0;
		if ( !::ReadFile( _fFile, _pvBuffer, static_cast<DWORD>(_uiptrBytesToRead), &dwBytesRead, NULL ) ) { return false; }
		if ( dwBytesRead != _uiptrBytesToRead ) { return false; }
		return true;
#elif defined( LSE_MAC )
		return ::fread( _pvBuffer, _uiptrBytesToRead, 1, _fFile ) == 1;
#endif	// LSE_WINDOWS
	}

	/**
	 * Write to a file.  Returns false under any form of error.  Data is written starting at the
	 *	current position in the file.
	 *
	 * \param _fFile Handle to the opened file to which to write.  Must have been opened with either
	 *	LSF_OF_WRITE or LSF_OF_APPEND.
	 * \param _pvBuffer Data to write to the file.
	 * \param _uiptrBytesToWrite Number of bytes to copy from _pvBuffer to the file.
	 * \return Returns false if the write failed for any reason.
	 */
	LSBOOL LSE_CALL CFileLib::WriteFile( LSF_FILE _fFile, const void * _pvBuffer, uintptr_t _uiptrBytesToWrite ) {
		if ( !_fFile ) { return false; }

#ifdef LSE_WINDOWS
		DWORD dwBytesWritten = 0;
		if ( !::WriteFile( _fFile, _pvBuffer, static_cast<DWORD>(_uiptrBytesToWrite), &dwBytesWritten, NULL ) ) { return false; }
		if ( dwBytesWritten != _uiptrBytesToWrite ) { return false; }
		return true;
#elif defined( LSE_MAC )
		return ::fwrite( _pvBuffer, _uiptrBytesToWrite, 1, _fFile ) == 1;
#endif	// LSE_WINDOWS
	}

	/**
	 * Moves the file pointer of the specified file.
	 *
	 * \param _fFile A handle to the file.
	 * \param _i64DistanceToMove The number of bytes to move the file pointer.  A positive value moves the pointer forward in the file and a negative value moves the file pointer backward.
	 * \param _sMoveMethod The starting point for the file pointer move.  Must be one of the LSF_SEEK enumerated values.
	 * \return Returns true if the supplied handle is valid.
	 */
	LSBOOL LSE_CALL CFileLib::SetFilePointer( LSF_FILE _fFile, int64_t _i64DistanceToMove, LSF_SEEK _sMoveMethod ) {
		if ( !_fFile ) { return false; }

#ifdef LSE_WINDOWS
		LARGE_INTEGER liDist;
		liDist.QuadPart = _i64DistanceToMove;
		return ::SetFilePointerEx( _fFile, liDist, NULL, _sMoveMethod );
#elif defined( LSE_MAC )
		return ::fseeko( _fFile, _i64DistanceToMove, _sMoveMethod ) == 0;
#endif	// LSE_WINDOWS
	}

	/**
	 * Returns the position of the file pointer.
	 *
	 * \param _fFile A handle to the file.
	 * \return Returns the position of the file pointer. 
	 */
	uint64_t LSE_CALL CFileLib::GetFilePointer( LSF_FILE _fFile ) {
		if ( !_fFile ) { return false; }

#ifdef LSE_WINDOWS
		LARGE_INTEGER liDist;
		liDist.QuadPart = 0LL;
		LARGE_INTEGER liRet;
		if ( !::SetFilePointerEx( _fFile, liDist, &liRet, FILE_CURRENT ) ) { return LSE_MAXU64; }
		return liRet.QuadPart;
#elif defined( LSE_MAC )
		return static_cast<uint64_t>(::ftello( _fFile ));
#endif	// LSE_WINDOWS
	}

	/**
	 * Creates a file-mapping object.  File must exist.
	 *
	 * \param _pcFile The file to map.
	 * \param _ofOpenType Flag determining how to open the file.  Valid values are LSF_OF_READ,
	 *	LSF_OF_WRITE, or both.
	 * \param _fmMap Holds the returned file map upon success.  If the function returns true, this
	 *	structure must be passed to a call to DestroyFileMap() when no longer needed.
	 * \return Returns true if the mapping succeeded.  False typically indicates that the file does not
	 *	exist or cannot be accessed.
	 */
	LSBOOL LSE_CALL CFileLib::CreateFileMap( const char * _pcFile, LSF_OPEN_FILE _ofOpenType,
		LSF_FILE_MAP &_fmMap ) {
#ifdef LSE_WINDOWS
		_fmMap.hFile = NULL;
		_fmMap.hFileMap = NULL;
		_fmMap.ui32Mode = 0;

		// Create the file.
		wchar_t szBuffer[LSF_MAX_PATH];
		HANDLE hFile = INVALID_HANDLE_VALUE;
		if ( !::MultiByteToWideChar( CP_UTF8,
			0,
			_pcFile,
			-1,
			szBuffer,
			LSF_MAX_PATH ) ) {
			hFile = ::CreateFileA( _pcFile, _ofOpenType, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, NULL, NULL );
		}
		else {
			hFile = ::CreateFileW( szBuffer, _ofOpenType, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, NULL, NULL );
		}
		if ( hFile == INVALID_HANDLE_VALUE ) { return false; }

		// Get the file size.  It must not be 0 bytes.
		uint64_t ui64Size = GetFileSize( hFile );
		if ( ui64Size == 0 ) {
			::CloseHandle( hFile );
			return false;
		}

		// Create a mapping object.
		static const uint32_t ui32Modes[] = {
			PAGE_READONLY,	LSF_OF_READ,
			PAGE_READWRITE,	LSF_OF_WRITE,
			PAGE_READWRITE,	LSF_OF_READ | LSF_OF_WRITE,
		};
		uint32_t ui32Mode = 0;
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Modes ) >> 1; I += 1 ) {
			if ( ui32Modes[I+1] == static_cast<uint32_t>(_ofOpenType) ) {
				ui32Mode = ui32Modes[I];
				break;
			}
		}
		if ( !ui32Mode ) {
			// Invalid mapping type.
			::CloseHandle( hFile );
			return false;
		}
		HANDLE hMap = ::CreateFileMapping( hFile, NULL, ui32Mode, 0, 0, NULL );
		if ( !hMap ) {
			::CloseHandle( hFile );
			return false;
		}


		// Done.  Save everything.
		_fmMap.hFile = hFile;
		_fmMap.hFileMap = hMap;
		_fmMap.ui32Mode = _ofOpenType;

		return true;
#elif defined( LSE_MAC )
		_fmMap.iFile = -1;
		// We only open the file on other platforms.
		int iFile = -1;
		if ( _ofOpenType == LSF_OF_READ ) {
			iFile = ::open( _pcFile, O_RDONLY );
		}
		else if ( _ofOpenType == LSF_OF_WRITE ) {
			iFile = ::open( _pcFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );
		}
		else if ( _ofOpenType == (LSF_OF_READ | LSF_OF_WRITE) ) {
			iFile = ::open( _pcFile, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
		}

		if ( iFile == -1 ) { return false; }

		// File opened.  Done.
		_fmMap.iFile = iFile;
		_fmMap.ui32Mode = _ofOpenType;
		return true;
#endif	// LSE_WINDOWS

	}

	/**
	 * Creates a file-mapping object.  File must exist.
	 *
	 * \param _pwcFile The file to map.
	 * \param _ofOpenType Flag determining how to open the file.  Valid values are LSF_OF_READ,
	 *	LSF_OF_WRITE, or both.
	 * \param _fmMap Holds the returned file map upon success.  If the function returns true, this
	 *	structure must be passed to a call to DestroyFileMap() when no longer needed.
	 * \return Returns true if the mapping succeeded.  False typically indicates that the file does not
	 *	exist or cannot be accessed.
	 */
	LSBOOL LSE_CALL CFileLib::CreateFileMap( const wchar_t * _pwcFile, LSF_OPEN_FILE _ofOpenType,
		LSF_FILE_MAP &_fmMap ) {
#ifdef LSE_WINDOWS
		_fmMap.hFile = NULL;
		_fmMap.hFileMap = NULL;
		_fmMap.ui32Mode = 0;

		// Create the file.
		HANDLE hFile = ::CreateFileW( _pwcFile, _ofOpenType, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, NULL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE ) { return false; }

		// Get the file size.  It must not be 0 bytes.
		uint64_t ui64Size = GetFileSize( hFile );
		if ( ui64Size == 0 ) {
			::CloseHandle( hFile );
			return false;
		}

		// Create a mapping object.
		static const uint32_t ui32Modes[] = {
			PAGE_READONLY,	LSF_OF_READ,
			PAGE_READWRITE,	LSF_OF_WRITE,
			PAGE_READWRITE,	LSF_OF_READ | LSF_OF_WRITE,
		};
		uint32_t ui32Mode = 0;
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui32Modes ) >> 1; I += 1 ) {
			if ( ui32Modes[I+1] == static_cast<uint32_t>(_ofOpenType) ) {
				ui32Mode = ui32Modes[I];
				break;
			}
		}
		if ( !ui32Mode ) {
			// Invalid mapping type.
			::CloseHandle( hFile );
			return false;
		}
		HANDLE hMap = ::CreateFileMapping( hFile, NULL, ui32Mode, 0, 0, NULL );
		if ( !hMap ) {
			::CloseHandle( hFile );
			return false;
		}


		// Done.  Save everything.
		_fmMap.hFile = hFile;
		_fmMap.hFileMap = hMap;
		_fmMap.ui32Mode = _ofOpenType;
		return true;
#elif defined( LSE_MAC )
		_fmMap.iFile = -1;
		char * pcCopy = LSENEW char[LSF_MAX_PATH];
		if ( !pcCopy ) { return false; }
		if ( !CStd::UtfXToUtf8( _pwcFile, pcCopy, LSF_MAX_PATH ) ) {
			LSEDELETE [] pcCopy;
			return false;
		}
		
		// We only open the file on other platforms.
		int iFile = -1;
		if ( _ofOpenType == LSF_OF_READ ) {
			iFile = ::open( pcCopy, O_RDONLY );
		}
		else if ( _ofOpenType == LSF_OF_WRITE ) {
			iFile = ::open( pcCopy, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR );
		}
		else if ( _ofOpenType == (LSF_OF_READ | LSF_OF_WRITE) ) {
			iFile = ::open( pcCopy, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
		}

		LSEDELETE [] pcCopy;

		if ( iFile == -1 ) { return false; }

		// File opened.  Done.
		_fmMap.iFile = iFile;
		_fmMap.ui32Mode = _ofOpenType;
		return true;
#endif	// LSE_WINDOWS
	}

	/**
	 * Destroy a file map created with CreateFileMap().  Any regions not unmapped inside the file will
	 *	become invalid.  Use UnMap() on all mapped regions before calling this.
	 *
	 * \param _fmMap The file map to be destroyed.
	 */
	void LSE_CALL CFileLib::DestroyFileMap( LSF_FILE_MAP &_fmMap ) {
#ifdef LSE_WINDOWS

		// Close the handles.
		if ( _fmMap.hFileMap ) {
			::CloseHandle( _fmMap.hFileMap );
			_fmMap.hFileMap = NULL;
		}

		if ( _fmMap.hFile ) {
			::CloseHandle( _fmMap.hFile );
			_fmMap.hFile = NULL;
		}
		// Done.
#elif defined( LSE_MAC )
		if ( _fmMap.iFile != -1 ) {
			::close( _fmMap.iFile );
			_fmMap.iFile = -1;
		}
#endif	// LSE_WINDOWS
	}

	/**
	 * Map a region of a mapped file to memory.  The mapped region must be unmapped with UnMap() when no
	 *	longer needed.
	 *
	 * \param _fmMap The mapped file created with a previous call to CreateFileMap().
	 * \param _ui64Offset Offset into the file where to begin the mapping.  Must be aligned on a system
	 *	page boundary.
	 * \param _uiptrLen Number of bytes to map starting at the given offset.
	 * \return Returns NULL on failure, otherwise a pointer to the mapped data.
	 */
	void * LSE_CALL CFileLib::Map( const LSF_FILE_MAP &_fmMap, uint64_t _ui64Offset,
		uintptr_t _uiptrLen ) {
#ifdef LSE_WINDOWS
		if ( _fmMap.ui32Mode & LSF_OF_WRITE ) {
			void * pvRet = ::MapViewOfFile( _fmMap.hFileMap, FILE_MAP_WRITE,
				static_cast<DWORD>(_ui64Offset >> 32ULL), static_cast<DWORD>(_ui64Offset), _uiptrLen );
#ifdef _DEBUG
			if ( !pvRet ) {
				DWORD dwError = ::GetLastError();
				WCHAR * lpwcError;
				::FormatMessageW(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					dwError,
					MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					reinterpret_cast<LPWSTR>(&lpwcError),
					0, NULL );
				CStd::DebugPrintW( lpwcError );
				::LocalFree( lpwcError );
			}
#endif	// #ifdef _DEBUG
			return pvRet;
		}
		else {
			DWORD dwHigh = static_cast<DWORD>(_ui64Offset >> 32ULL);
			DWORD dwLow = static_cast<DWORD>(_ui64Offset);
			void * pvRet = ::MapViewOfFile( _fmMap.hFileMap, FILE_MAP_READ,
				dwHigh, dwLow, _uiptrLen );
#ifdef _DEBUG
			if ( !pvRet ) {
				DWORD dwError = ::GetLastError();
				WCHAR * lpwcError;
				::FormatMessageW(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM |
						FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					dwError,
					MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					reinterpret_cast<LPWSTR>(&lpwcError),
					0, NULL );
				CStd::DebugPrintW( lpwcError );
				::LocalFree( lpwcError );
			}
#endif	// #ifdef _DEBUG
			return pvRet;
		}
#elif defined( LSE_MAC )
		uint32_t ui32Prot = PROT_READ;
		if ( _fmMap.ui32Mode & LSF_OF_WRITE ) {
			ui32Prot |= PROT_WRITE;
		}
		void * pvData = ::mmap( NULL, _uiptrLen, static_cast<int>(ui32Prot), MAP_SHARED, _fmMap.iFile, static_cast<off_t>(_ui64Offset) );
		if ( pvData == MAP_FAILED ) { return NULL; }
		return pvData;
#endif	// LSE_WINDOWS
	}

	/**
	 * Unmaps a region of memory previously mapped with a call to Map().
	 *
	 * \param _pvAddr Previous pointer returned by Map().
	 * \param _uiptrLen Length previously passed to Map() when creating the region.
	 */
	void LSE_CALL CFileLib::UnMap( const void * _pvAddr, uintptr_t _uiptrLen ) {
		if ( !_pvAddr ) { return; }
#ifdef LSE_WINDOWS
		// Remove warning C4100.
		static_cast<uintptr_t>(_uiptrLen);
		::UnmapViewOfFile( _pvAddr );
#elif defined( LSE_MAC )
		::munmap( const_cast<void *>(_pvAddr), _uiptrLen );
#endif	// LSE_WINDOWS
	}

	/**
	 * Does the given file map provide write permissions?
	 *
	 * \param _fmMap The mapped file previously created with CreateFileMap();
	 * \return Returns true if the file was mapped with write permissions.
	 */
	LSBOOL LSE_CALL CFileLib::CanWriteToFileMap( const LSF_FILE_MAP &_fmMap ) {
		return (_fmMap.ui32Mode & LSF_OF_WRITE) != 0;
	}

	/**
	 * Gets the size of a page boundary for mapping files.
	 *
	 * \return Returns the size of page boundaries, required for file-mapping routines.
	 */
	uint32_t LSE_CALL CFileLib::GetPageSize() {
#ifdef LSE_WINDOWS
		SYSTEM_INFO siInfo;
		::GetSystemInfo( &siInfo );
		return siInfo.dwPageSize;
#elif defined( LSE_MAC )
		return static_cast<uint32_t>(::getpagesize());
#endif	// LSE_WINDOWS
	}

	/**
	 * Gets the size of an open file.
	 *
	 * \param _fFile Handle to the opened file whose size is to be obtained.
	 * \return The size of the file.
	 */
	uint64_t LSE_CALL CFileLib::GetFileSize( LSF_FILE _fFile ) {
		if ( !_fFile ) { return 0; }

#ifdef LSE_WINDOWS
		DWORD dwLow, dwHigh;
		dwLow = ::GetFileSize( _fFile, &dwHigh );
		return dwLow | (static_cast<uint64_t>(dwHigh) << 32ULL);
#elif defined( LSE_MAC )
		fpos_t pPos;
		if ( ::fgetpos( _fFile, &pPos ) != 0 ) { return 0xFFFFFFFFFFFFFFFFULL; }
		
		if ( ::fseek( _fFile, 0, SEEK_END ) != 0 ) { return 0xFFFFFFFFFFFFFFFFULL; }
		uint64_t ui64Pos = static_cast<uint64_t>(::ftello( _fFile ));
		::fsetpos( _fFile, &pPos );
		return ui64Pos;
#endif	// LSE_WINDOWS
	}

	/**
	 * Gets the size of a file.
	 *
	 * \param _pcFile UTF-8 path to the file whose size is to be obtained.
	 * \return The size of the file.
	 */
	uint64_t LSE_CALL CFileLib::GetFileSize( const char * _pcFile ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcFile), LSF_MAX_PATH );
		return GetFileSize( szBuffer );
	}
	
	/**
	 * Gets the size of a file.
	 *
	 * \param _pwcFile Path to the file whose size is to be obtained.
	 * \return The size of the file.
	 */
	uint64_t LSE_CALL CFileLib::GetFileSize( const wchar_t * _pwcFile ) {
#ifdef LSE_WINDOWS
		WIN32_FILE_ATTRIBUTE_DATA wfdData;
		if ( !::GetFileAttributesExW( _pwcFile, GetFileExInfoStandard, &wfdData ) ) { return 0ULL; }

		return wfdData.nFileSizeLow | static_cast<uint64_t>(wfdData.nFileSizeHigh) << 32ULL;
#elif defined( LSE_MAC )
		struct stat sStat;
		char * pcCopy = LSENEW char[LSF_MAX_PATH];
		if ( !pcCopy ) { return false; }
		if ( !CStd::UtfXToUtf8( _pwcFile, pcCopy, LSF_MAX_PATH ) ) {
			LSEDELETE [] pcCopy;
			return 0ULL;
		}
		// TODO: ::stat() returns only 32 bits.  Find something else for this.
		if ( ::stat( pcCopy, &sStat ) == -1 ) {
			LSEDELETE [] pcCopy;
			return 0ULL;
		}
		LSEDELETE [] pcCopy;
		return static_cast<uint64_t>(sStat.st_size);
#endif	// LSE_WINDOWS
	}

	/**
	 * Does the given file exist?  Files only!
	 *
	 * \param _pcFile Path to the file.
	 * \return Returns true if the file exists.  If the path resolves to an existing folder or if the
	 *	path does not exist, false is returned.
	 */
	LSBOOL LSE_CALL CFileLib::Exists( const char * _pcFile ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcFile), LSF_MAX_PATH );
		return Exists( szBuffer );
	}

	/**
	 * Does the given file exist?  Files only!
	 *
	 * \param _pcFile Path to the file.
	 * \return Returns true if the file exists.  If the path resolves to an existing folder or if the
	 *	path does not exist, false is returned.
	 */
	LSBOOL LSE_CALL CFileLib::Exists( const wchar_t * _pwcFile ) {
#ifdef LSE_WINDOWS
		DWORD dwAttributes = ::GetFileAttributesW( _pwcFile );
		return dwAttributes != INVALID_FILE_ATTRIBUTES && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#elif defined( LSE_MAC )
		// Get the length the hard way.
		struct stat sStat;
		char * pcCopy = LSENEW char[LSF_MAX_PATH];
		if ( !pcCopy ) { return false; }
		if ( !CStd::UtfXToUtf8( _pwcFile, pcCopy, LSF_MAX_PATH ) ) {
			LSEDELETE [] pcCopy;
			return 0ULL;
		}
		// TODO: ::stat() returns only 32 bits.  Find something else for this.
		if ( ::stat( pcCopy, &sStat ) == -1 ) {
			LSEDELETE [] pcCopy;
			return errno == ENOENT;
		}
		LSEDELETE [] pcCopy;
		return true;
		
		/*const wchar_t * pwcTemp = _pwcFile;
		while ( (*pwcTemp++) ) {}
		NSString * psStr = [[NSString alloc] initWithBytes:_pwcFile  length:sizeof( wchar_t ) * (pwcTemp - _pwcFile - 1) encoding:NSUTF32LittleEndianStringEncoding];
		LSBOOL bRet = [[NSFileManager defaultManager] fileExistsAtPath:psStr];
		[psStr release];
		return bRet;*/
#else
		return false;	// TODO.
#endif	// #ifdef LSE_WINDOWS
	}

	/**
	 * Gets the path to the directory in which this executable resides.  Returns the string that was
	 *	put into it.  Must be at least LSF_MAX_PATH characters long.  Returns NULL in the event of error.
	 * The returned string always ends with a backslash (or forward slash depending on the operating
	 *	system).
	 *
	 * \param _pcRet Buffer to be filled with the path to this executable.
	 * \return Returns _pcRet.
	 */
	char * LSE_CALL CFileLib::GetSelfDir( char * _pcRet ) {
		if ( !_pcRet ) { return NULL; }

		wchar_t szBuffer[LSF_MAX_PATH];
		if ( !GetSelfDir( szBuffer ) ) { return NULL; }
		CStd::UtfXToUtf8( szBuffer, _pcRet, LSF_MAX_PATH );
		return _pcRet;
	}

	/**
	 * Gets the path to the directory in which this executable resides.  Returns the string that was
	 *	put into it.  Must be at least LSF_MAX_PATH characters long.  Returns NULL in the event of error.
	 * The returned string always ends with a backslash (or forward slash depending on the operating
	 *	system).
	 *
	 * \param _pwcRet Buffer to be filled with the path to this executable.
	 * \return Returns _pwcRet.
	 */
	wchar_t * LSE_CALL CFileLib::GetSelfDir( wchar_t * _pwcRet ) {
		if ( !_pwcRet ) { return NULL; }
#ifdef LSE_WINDOWS
		try {	// Remind me again why I added this exception handler?
			if ( !::GetModuleFileNameW( NULL, _pwcRet, LSF_MAX_PATH ) ) { return NULL; }
			if ( !::PathRemoveFileSpecW( _pwcRet ) ) { return NULL; }
			if ( !::PathAddBackslashW( _pwcRet ) ) { return NULL; }
			return _pwcRet;
		}
		catch ( ... ) {
			return NULL;
		}
#elif defined( LSE_MAC )
		// Get the main bundle.
		CFBundleRef brBundle = ::CFBundleGetMainBundle();
		CFURLRef urlrUrl = ::CFBundleCopyBundleURL( brBundle );
		CFStringRef srString = ::CFURLCopyFileSystemPath( urlrUrl, kCFURLPOSIXPathStyle );
		::CFRelease( urlrUrl );
		
		// Seems we have to copy manually.
		CFIndex iLen = ::CFStringGetLength( srString );
		for ( CFIndex I = 0; I < iLen; ++I ) {
			_pwcRet[I] = static_cast<wchar_t>(::CFStringGetCharacterAtIndex( srString, I ));
		}
		::CFRelease( srString );
		
		// Append a slash.
		_pwcRet[iLen++] = L'/';
		_pwcRet[iLen] = L'\0';
		return _pwcRet;
#endif	// LSE_WINDOWS
	}

	/**
	 * Fully expands the given path.  The result is stored in _pcRet, which must be at least LSF_MAX_PATH
	 *	characters long.  Returns _pcRet or NULL on error.
	 *
	 * \param _pcPath Path to expand, in UTF-8 format.
	 * \param _pcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
	 * \return Returns _pcRet or NULL on error.  On success, _pcRet will be filled with a
	 *	NULL-terminated UTF-8 string representing the expanded path.
	 */
	char * LSE_CALL CFileLib::ExpandPath( const char * _pcPath, char * _pcRet ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		wchar_t szBufferRet[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcPath), LSF_MAX_PATH );
		if ( !ExpandPath( szBuffer, szBufferRet ) ) { return NULL; }
		CStd::UtfXToUtf8( szBufferRet, _pcRet, LSF_MAX_PATH );
		return _pcRet;
	}

	/**
	 * Fully expands the given path.  The result is stored in _pwcRet, which must be at least LSF_MAX_PATH
	 *	characters long.  Returns _pwcRet or NULL on error.
	 *
	 * \param _pwcPath Path to expand.
	 * \param _pwcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
	 * \return _pwcRet or NULL on error.
	 */
	wchar_t * LSE_CALL CFileLib::ExpandPath( const wchar_t * _pwcPath, wchar_t * _pwcRet ) {
#ifdef LSE_WINDOWS
		// The buffer that contains the current directory + the supplied path may be longer than
		//	LSF_MAX_PATH because of ..\ and .\ patterns in the string.
		// + 1 here accounts for a potential addition of a \ during combining.  This should not be
		//	necessary though.
		uintptr_t uiptrLen = CStd::WStrLen( _pwcPath ) + LSF_MAX_PATH + 1;
		wchar_t * pcBuffer = LSENEW wchar_t[uiptrLen];
		if ( !pcBuffer ) { return NULL; }	// Out of memory?

		if ( ::PathIsRelativeW( _pwcPath ) ) {
			// To hold the path to the curent directory.
			wchar_t szCurDirectory[LSF_MAX_PATH];

			// Buffer the current working directory.
			if ( !::GetCurrentDirectoryW( LSF_MAX_PATH, szCurDirectory ) ) {
				LSEDELETE [] pcBuffer;
				return NULL;
			}
			// Add the given path to it.
			if ( !::PathCombineW( pcBuffer, szCurDirectory, _pwcPath ) ) {
				LSEDELETE [] pcBuffer;
				return NULL;
			}
		}
		else {
			// Just copy the path over.
			CStd::WStrCpy( pcBuffer, _pwcPath );
		}
		// Fix all the ..\ and .\ patterns in it.
		// For this to work, / must be changed to \.
		for ( wchar_t * pcTemp = pcBuffer; (*pcTemp); ++pcTemp ) {
			if ( (*pcTemp) == L'/' ) {
				(*pcTemp) = L'\\';
			}
		}
		BOOL bRet = ::PathCanonicalizeW( _pwcRet, pcBuffer );
		LSEDELETE [] pcBuffer;
		return bRet ? _pwcRet : NULL;
#elif defined( LSE_MAC )
		// On Macintosh, if the first character is a /, the path is absolute.
		uintptr_t uiptrLen = CStd::WStrLen( _pwcPath ) + LSF_MAX_PATH + 1;
		wchar_t * pcBuffer = LSENEW wchar_t[uiptrLen];
		if ( !pcBuffer ) { return NULL; }	// Out of memory?

		if ( _pwcPath[0] == L'\\' || _pwcPath[0] == L'/' ) {
			// Just copy the path over.
			CStd::WStrCpy( pcBuffer, _pwcPath );
		}
		else {
			// To hold the path to the curent directory.
			wchar_t szCurDirectory[LSF_MAX_PATH];
			GetSelfDir( szCurDirectory );
			
			// Combine the current directory and the given path.
			CStd::WStrCpy( pcBuffer, szCurDirectory );
			CStd::WStrNCat( pcBuffer, _pwcPath, static_cast<uint32_t>(uiptrLen) );
		}
		
#ifdef __OBJC__

		// Copy into an NSString buffer.
		NSUInteger uiLen = CStd::WStrLen( pcBuffer );
		NSString * psPath = [[NSString alloc] initWithBytes:pcBuffer length:uiLen * sizeof( wchar_t ) encoding:NSUTF32LittleEndianStringEncoding];
		
		// Canonicalize.
		NSString * psResolved = [psPath stringByStandardizingPath];
		
		// Copy to the destination.
		uiLen = [psResolved length];
		for ( NSUInteger I = 0; I < uiLen; ++I ) {
			_pwcRet[I] = static_cast<wchar_t>([psResolved characterAtIndex:I]);
		}
		_pwcRet[uiLen] = L'\0';
		
		
		[psResolved release];
		[psPath release];
		
		LSEDELETE [] pcBuffer;
		return _pwcRet;
#else	// #ifdef __OBJC__
		// Replace \ with /.
		uint32_t ui32Len = CStd::WStrLen( pcBuffer );
		LSBOOL bLastIsSlash = false;
		for ( uintptr_t I = ui32Len; I--; ) {
			if ( pcBuffer[I] == L'\\' ||  pcBuffer[I] == L'/' ) {
				pcBuffer[I] = L'/';
				if ( I == ui32Len - 1 ) {
					bLastIsSlash = true;
				}
			}
		}
		CWStringList wlTokens;
		if ( !wlTokens.MakeByTokenizing( pcBuffer, ui32Len, L'/' ) ) {
			LSEDELETE [] pcBuffer;
			return NULL;
		}
		LSEDELETE [] pcBuffer;
		
		// Now remove excess path components.
		for ( uint32_t I = 0; I < wlTokens.Length(); ) {
			if ( wlTokens[I].Length() == 0 ) {
				// Empty path component.  Just remove it.
				wlTokens.Remove( I );
				continue;
			}
			if ( wlTokens[I] == L"." ) {
				// A ./ entry.  Just remove it.
				wlTokens.Remove( I );
				continue;
			}
			if ( wlTokens[I] == L".." ) {
				// A ../ entry.  If possible, go back.  Otherwise fail.
				if ( I == 0 ) { return NULL; }
				--I;					// Go back one path component.
				wlTokens.Remove( I );	// Remove that component.
				wlTokens.Remove( I );	// Remove the ../.
				continue;
			}
			// All other cases, keep the path component.
			++I;
		}
		
		// Copy the result into the return.
		wchar_t * pwcTemp = _pwcRet;
		uint32_t ui32Copied = 0;
		for ( uint32_t I = 0; I < wlTokens.Length(); ++I ) {
			if ( ui32Copied >= LSF_MAX_PATH ) { return NULL; }
			(*pwcTemp++) = L'/';
			++ui32Copied;
			if ( ui32Copied + wlTokens[I].Length() >= LSF_MAX_PATH ) { return NULL; }
			for ( uint32_t J = 0; J < wlTokens[I].Length(); ++J ) {
				(*pwcTemp++) = wlTokens[I].CStr()[J];
			}
			ui32Copied += wlTokens[I].Length();
		}
		if ( bLastIsSlash ) {
			if ( ui32Copied >= LSF_MAX_PATH ) { return NULL; }
			(*pwcTemp++) = L'/';
		}
		if ( ui32Copied >= LSF_MAX_PATH ) { return NULL; }
		(*pwcTemp++) = L'\0';
		
		return _pwcRet;
#endif	// #ifdef __OBJC__
#endif	// LSE_WINDOWS
	}

	/**
	 * Fully expands the given path, which is assumed to be relative to the resources directory.  If the given
	 *	path is not, an invalid path will be created.
	 *
	 * \param _pcPath Path to expand, in UTF-8 format.
	 * \param _pcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
	 * \return Returns _pcRet or NULL on error.  On success, _pcRet will be filled with a
	 *	NULL-terminated UTF-8 string representing the expanded path to the resource folder.
	 */
	char * LSE_CALL CFileLib::ExpandResourcePath( const char * _pcPath, char * _pcRet ) {
#ifdef LSE_WINDOWS
		wchar_t szTempBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szTempBuffer, reinterpret_cast<const LSUTF8 *>(_pcPath), LSF_MAX_PATH );
		if ( !::PathIsRelativeW( szTempBuffer ) ) {
			// Just copy.
			CStd::StrCpy( _pcRet, _pcPath );
			return _pcRet;
		}
#elif defined( LSE_MAC )
		if ( _pcPath[0] == '\\' || _pcPath[0] == '/' ) {
			// Just copy the path over.
			CStd::StrCpy( _pcRet, _pcPath );
			return _pcRet;
		}
#endif	// LSE_WINDOWS
		char szBuffer[LSF_MAX_PATH];
		CStd::StrNCpy( szBuffer, m_szResourcePath, LSF_MAX_PATH );
		CStd::StrNCpy( szBuffer + m_ui32ResPathLen, _pcPath, LSF_MAX_PATH - m_ui32ResPathLen );
		return ExpandPath( szBuffer, _pcRet );
	}

	/**
	 * Removes the extension from a path.  Returns the input string, which is modified in-place.
	 *	The returned string is in UTF-8 format.
	 *
	 * \param _pcPath On input, it holds the path whose extension is to be removed.  On return, it
	 *	holds the path without the removed extension.
	 * \return Returns _pcPath.
	 */
	char * LSE_CALL CFileLib::NoExtension( char * _pcPath ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcPath), LSF_MAX_PATH );
		if ( !NoExtension( szBuffer ) ) { return NULL; }
		CStd::UtfXToUtf8( szBuffer, _pcPath, LSF_MAX_PATH );
		return _pcPath;
	}

	/**
	 * Removes the extension from a path.  Returns the input string, which is modified in-place.
	 *
	 * \param _pwcPath On input, it holds the path whose extension is to be removed.  On return, it
	 *	holds the path without the removed extension.
	 * \return Returns _pwcPath.
	 */
	wchar_t * LSE_CALL CFileLib::NoExtension( wchar_t * _pwcPath ) {
#ifdef LSE_WINDOWS
		::PathRemoveExtensionW( _pwcPath );
		return _pwcPath;
#elif defined( LSE_MAC )
		if ( !_pwcPath ) { return NULL; }
		wchar_t * pwcEnd = _pwcPath;
		// Go to the end.
		// Keep track of the last period found along the way.
		wchar_t * pwcPeriod = NULL;
		while ( (*pwcEnd) ) {
			if ( (*pwcEnd) == L'\\' || (*pwcEnd) == L'/' ) {
				// Entering a new directory; reset the period pointer.
				pwcPeriod = NULL;
			}
			else if ( (*pwcEnd) == L'.' ) {
				pwcPeriod = pwcEnd;
			}
			++pwcEnd;
		}
		
		// If there was a final period, NULL it.
		if ( pwcPeriod ) {
			(*pwcPeriod) = L'\0';
		}
		return _pwcPath;
#endif	// LSE_WINDOWS
	}

	/**
	 * Removes the path part from a path (leaves only the file part).  Returns the input string,
	 *	which is modified in-place.
	 *	The returned string is in UTF-8 format.
	 *
	 * \param _pcPath On input, it holds the string whose path part is to be removed.  On return,
	 *	it holds the string removed of its path.
	 * \return Returns _pcPath.
	 */
	char * LSE_CALL CFileLib::NoPath( char * _pcPath ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcPath), LSF_MAX_PATH );
		if ( !NoPath( szBuffer ) ) { return NULL; }
		CStd::UtfXToUtf8( szBuffer, _pcPath, LSF_MAX_PATH );
		return _pcPath;
	}

	/**
	 * Removes the path part from a path (leaves only the file part).  Returns the input string,
	 *	which is modified in-place.
	 *
	 * \param _pwcPath On input, it holds the string whose path part is to be removed.  On return,
	 *	it holds the string removed of its path.
	 * \return Returns _pwcPath.
	 */
	wchar_t * LSE_CALL CFileLib::NoPath( wchar_t * _pwcPath ) {
		if ( !_pwcPath ) { return NULL; }
#ifdef LSE_WINDOWS
		::PathStripPathW( _pwcPath );
		return _pwcPath;
#elif defined( LSE_MAC )
		wchar_t * pwcEnd = _pwcPath;
		// Find the last directory delimiter.
		wchar_t * pwcDirDel = NULL;
		while ( (*pwcEnd) ) {
			if ( (*pwcEnd) == L'\\' || (*pwcEnd) == L'/' ) {
				pwcDirDel = pwcEnd;
			}
			++pwcEnd;
		}
		// If no slashes found, there is no path.
		if ( !pwcDirDel ) { return _pwcPath; }
		
		// Otherwise we need to copy from the next character beyond the delimiter.
		pwcEnd = _pwcPath;
		while ( (*++pwcDirDel) ) {
			(*pwcEnd++) = (*pwcDirDel);
		}
		(*pwcEnd) = L'\0';
		return _pwcPath;
#endif	// LSE_WINDOWS
	}

	/**
	 * Removes the file part of a path (leaves only the folder part).  The return includes the terminating
	 *	/ or \.  If there is no folder part to return, ./ is returned.  Returns the input string,
	 *	which is modified in-place.
	 *
	 * \param _pcPath On input, it holds the string whose file part is to be removed.  On return,
	 *	it holds the string removed of its file.
	 * \return Returns _pcPath.
	 */
	char * LSE_CALL CFileLib::NoFile( char * _pcPath ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcPath), LSF_MAX_PATH );
		if ( !NoFile( szBuffer ) ) { return NULL; }
		CStd::UtfXToUtf8( szBuffer, _pcPath, LSF_MAX_PATH );
		return _pcPath;
	}

	/**
	 * Removes the file part of a path (leaves only the folder part).  The return includes the terminating
	 *	/ or \.  If there is no folder part to return, ./ is returned.  Returns the input string,
	 *	which is modified in-place.
	 *
	 * \param _pwcPath On input, it holds the string whose file part is to be removed.  On return,
	 *	it holds the string removed of its file.
	 * \return Returns _pwcPath.
	 */
	wchar_t * LSE_CALL CFileLib::NoFile( wchar_t * _pwcPath ) {
		if ( !_pwcPath ) { return NULL; }
		wchar_t * pwcEnd = _pwcPath;
		// Find the last directory delimiter.
		wchar_t * pwcDirDel = NULL;
		while ( (*pwcEnd) ) {
			if ( (*pwcEnd) == L'\\' || (*pwcEnd) == L'/' ) {
				pwcDirDel = pwcEnd;
			}
			++pwcEnd;
		}
		// If no slashes found, there is no path.
		if ( !pwcDirDel ) {
			CStd::WStrCpy( _pwcPath, L"./" );
			return _pwcPath;
		}

		// A slash was found.  A case such as "///File.exe" would erroneously return
		//	"///" if we do not perform the extra step of backstepping over repeat slashes.
		// There is always at least one iteration here.
		while ( ((*pwcDirDel) == L'\\' || (*pwcDirDel) == L'/') && pwcDirDel >= _pwcPath ) { --pwcDirDel; }

		// Step back over one slash.
		++pwcDirDel;

		// If we have "", "\", or "/", change to "./".
		if ( (pwcDirDel - _pwcPath) <= 1 ) {
			CStd::WStrCpy( _pwcPath, L"./" );
			return _pwcPath;
		}

		// Skip over the slash.
		++pwcDirDel;
		
		// NULL-terminate here.
		(*pwcDirDel) = L'\0';

		// Return.
		return _pwcPath;
	}

	/**
	 * Gets a pointer to the extension, not including the . (period).
	 *
	 * \param _pcPath Holds the file name (including the path or not) whose extension is to be found.
	 * \return Returns a pointer to the position within the given string where the extension begins.
	 */
	const char * LSE_CALL CFileLib::GetExtension( const char * _pcPath ) {
		size_t sLen = CStd::StrLen( _pcPath );
		const char * pcDef = &_pcPath[sLen];	// Points to the end-string NULL.
		while ( sLen ) {
			if ( _pcPath[--sLen] == '.' ) { return &_pcPath[sLen+1]; }
			if ( _pcPath[sLen] == '\\' || _pcPath[sLen] == '/' ) { break; }
		}
		return pcDef;
	}

	/**
	 * Gets a pointer to the extension, not including the . (period).
	 *
	 * \param _pwcPath Holds the file name (including the path or not) whose extension is to be found.
	 * \return Returns a pointer to the position within the given string where the extension begins.
	 */
	const wchar_t * LSE_CALL CFileLib::GetExtension( const wchar_t * _pwcPath ) {
		size_t sLen = CStd::WStrLen( _pwcPath );
		const wchar_t * pwcDef = &_pwcPath[sLen];	// Points to the end-string NULL.
		while ( sLen ) {
			if ( _pwcPath[--sLen] == L'.' ) { return &_pwcPath[sLen+1]; }
			if ( _pwcPath[sLen] == L'\\' || _pwcPath[sLen] == L'/' ) { break; }
		}
		return pwcDef;
	}

	/**
	 * Returns the system EOF (end-of-file) value.  This is normally -1 but is not guaranteed to be so on every platform/compiler.
	 *
	 * \return Returns the system EOF value.
	 */
	int32_t LSE_CALL CFileLib::Eof() {
		return EOF;
	}

	/**
	 * Sets the resource path.
	 *
	 * \param _pcPath The resource path to set.
	 */
	void LSE_CALL CFileLib::SetResPath( const char * _pcPath ) {
		//CStd::StrNCpy( m_szResourcePath, _pcPath, LSE_ELEMENTS( m_szResourcePath ) );
		ExpandPath( _pcPath, m_szResourcePath );
		m_ui32ResPathLen = CStd::StrLen( m_szResourcePath );
		if ( m_ui32ResPathLen && m_szResourcePath[m_ui32ResPathLen-1] != '/' && m_szResourcePath[m_ui32ResPathLen-1] != '\\' ) {
			m_szResourcePath[m_ui32ResPathLen++] = '/';
			assert( m_ui32ResPathLen < LSF_MAX_PATH );
		}
	}

}	// namespace lsf
