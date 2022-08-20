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


#ifndef __LSF_FILELIB_H__
#define __LSF_FILELIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the threading library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the LSTL.
#include "LSTLib.h"
using namespace ::lstl;


#if defined( LSE_WINDOWS )
#include "Windows/LSSTDWindows.h"

#define LSF_MAX_PATH						MAX_PATH				/**< Max file path length. */
#define LSF_PATH_DELIMITER					"\\"					/**< Path delimiter. */
#define LSF_PATH_DELIMITER_CHAR				'\\'					/**< Path delimiter in character form. */

#elif defined( LSE_MAC )	// Macintosh uses standard C fopen() and etc.
#include <CoreFoundation/CoreFoundation.h>			

#define LSF_MAX_PATH						PATH_MAX				/**< Max file path length. */
#define LSF_PATH_DELIMITER					"/"						/**< Path delimiter. */
#define LSF_PATH_DELIMITER_CHAR				'/'						/**< Path delimiter in character form. */

#endif	// #if defined( LSE_WINDOWS )


// TODO: Learn and add the headers for other systems.


namespace lsf {

	// == Enumerations.
	/**
	 * Ways to open a file.
	 */
	enum LSF_OPEN_FILE {
#ifdef LSE_WINDOWS
		LSF_OF_READ							= FILE_READ_DATA,
		LSF_OF_WRITE						= FILE_WRITE_DATA,
		LSF_OF_APPEND						= FILE_APPEND_DATA,
#elif defined( LSE_MAC )
		LSF_OF_READ							= (1 << 0),
		LSF_OF_WRITE						= (1 << 1),
		LSF_OF_APPEND						= (1 << 2),
#endif	// LSE_WINDOWS
	};


	// == Types.
#ifdef LSE_WINDOWS
	// A few namespace-specific types.
	typedef HANDLE							LSF_FILE;

	/**
	 * Seek flags.
	 */
	enum LSF_SEEK {
		LSF_S_BEGIN							= FILE_BEGIN,
		LSF_S_CURRENT						= FILE_CURRENT,
		LSF_S_END							= FILE_END
	};

	/**
	 * Structure to keep track of data for mapping of files.
	 */
	typedef struct LSF_FILE_MAP {
		// Handle for the open file.
		HANDLE								hFile;

		// Handle for the file map.
		HANDLE								hFileMap;

		// Mapping mode.
		uint32_t							ui32Mode;

	} * LPLSF_FILE_MAP, * const LPCLSF_FILE_MAP;
#elif defined( LSE_MAC )
	typedef FILE *							LSF_FILE;

	/**
	 * Seek flags.
	 */
	enum LSF_SEEK {
		LSF_S_BEGIN							= SEEK_SET,
		LSF_S_CURRENT						= SEEK_CUR,
		LSF_S_END							= SEEK_END
	};

	/**
	 * Structure to keep track of data for mapping of files.
	 */
	typedef struct LSF_FILE_MAP {
		// Handle for the open file and later close.
		int									iFile;

		// Mapping mode.
		uint32_t							ui32Mode;
	} * LPLSF_FILE_MAP, * const LPCLSF_FILE_MAP;
#endif	// LSE_WINDOWS



	/**
	 * Class CFileLib
	 * \brief Cross-platform file functions.
	 *
	 * Description: Handles opening, closing, reading from, and writing to files.  Also allows getting
	 *	information on files such as size.
	 */
	class CFileLib {
	public :

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
		static LSF_FILE LSE_CALL			OpenFile( const char * _pcFile, LSF_OPEN_FILE _ofOpenType, LSBOOL _bOpenAlways );

		/**
		 * Close a file.  Sets the file pointer to NULL automatically.
		 *
		 * \param _fFile Handle to the open file to close.  This is set to an invalid handle after
		 *	being closed.
		 * \return Returns false if there was an error closing the handle.  If the handle is invalid,
		 *	returns true immediately.
		 */
		static LSBOOL LSE_CALL				CloseFile( LSF_FILE &_fFile );

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
		static LSBOOL LSE_CALL				ReadFile( LSF_FILE _fFile, void * _pvBuffer, uintptr_t _uiptrBytesToRead );

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
		static LSBOOL LSE_CALL				WriteFile( LSF_FILE _fFile, const void * _pvBuffer, uintptr_t _uiptrBytesToWrite );

		/**
		 * Moves the file pointer of the specified file.
		 *
		 * \param _fFile A handle to the file.
		 * \param _i64DistanceToMove The number of bytes to move the file pointer.  A positive value moves the pointer forward in the file and a negative value moves the file pointer backward.
		 * \param _sMoveMethod The starting point for the file pointer move.  Must be one of the LSF_SEEK enumerated values.
		 * \return Returns true if the supplied handle is valid.
		 */
		static LSBOOL LSE_CALL				SetFilePointer( LSF_FILE _fFile, int64_t _i64DistanceToMove, LSF_SEEK _sMoveMethod );

		/**
		 * Returns the position of the file pointer.
		 *
		 * \param _fFile A handle to the file.
		 * \return Returns the position of the file pointer. 
		 */
		static uint64_t LSE_CALL			GetFilePointer( LSF_FILE _fFile );

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
		static LSBOOL LSE_CALL				CreateFileMap( const char * _pcFile, LSF_OPEN_FILE _ofOpenType,
			LSF_FILE_MAP &_fmMap );

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
		static LSBOOL LSE_CALL				CreateFileMap( const wchar_t * _pwcFile, LSF_OPEN_FILE _ofOpenType,
			LSF_FILE_MAP &_fmMap );

		/**
		 * Destroy a file map created with CreateFileMap().  Any regions not unmapped inside the file will
		 *	become invalid.  Use UnMap() on all mapped regions before calling this.
		 *
		 * \param _fmMap The file map to be destroyed.
		 */
		static void LSE_CALL				DestroyFileMap( LSF_FILE_MAP &_fmMap );

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
		static void * LSE_CALL				Map( const LSF_FILE_MAP &_fmMap, uint64_t _ui64Offset,
			uintptr_t _uiptrLen );

		/**
		 * Unmaps a region of memory previously mapped with a call to Map().
		 *
		 * \param _pvAddr Previous pointer returned by Map().
		 * \param _uiptrLen Length previously passed to Map() when creating the region.
		 */
		static void LSE_CALL				UnMap( const void * _pvAddr, uintptr_t _uiptrLen );

		/**
		 * Does the given file map provide write permissions?
		 *
		 * \param _fmMap The mapped file previously created with CreateFileMap();
		 * \return Returns true if the file was mapped with write permissions.
		 */
		static LSBOOL LSE_CALL				CanWriteToFileMap( const LSF_FILE_MAP &_fmMap );

		/**
		 * Gets the size of a page boundary for mapping files.
		 *
		 * \return Returns the size of page boundaries, required for file-mapping routines.
		 */
		static uint32_t LSE_CALL			GetPageSize();

		/**
		 * Gets the size of an open file.
		 *
		 * \param _fFile Handle to the opened file whose size is to be obtained.
		 * \return The size of the file.
		 */
		static uint64_t LSE_CALL			GetFileSize( LSF_FILE _fFile );

		/**
		 * Gets the size of a file.
		 *
		 * \param _pcFile UTF-8 path to the file whose size is to be obtained.
		 * \return The size of the file.
		 */
		static uint64_t LSE_CALL			GetFileSize( const char * _pcFile );

		/**
		 * Gets the size of a file.
		 *
		 * \param _pwcFile Path to the file whose size is to be obtained.
		 * \return The size of the file.
		 */
		static uint64_t LSE_CALL			GetFileSize( const wchar_t * _pwcFile );

		/**
		 * Does the given file exist?  Files only!
		 *
		 * \param _pcFile Path to the file.
		 * \return Returns true if the file exists.  If the path resolves to an existing folder or if the
		 *	path does not exist, false is returned.
		 */
		static LSBOOL LSE_CALL				Exists( const char * _pcFile );

		/**
		 * Does the given file exist?  Files only!
		 *
		 * \param _pcFile Path to the file.
		 * \return Returns true if the file exists.  If the path resolves to an existing folder or if the
		 *	path does not exist, false is returned.
		 */
		static LSBOOL LSE_CALL				Exists( const wchar_t * _pwcFile );

		/**
		 * Gets the path to the directory in which this executable resides.  Returns the string that was
		 *	put into it.  Must be at least LSF_MAX_PATH characters long.  Returns NULL in the event of error.
		 * The returned string always ends with a backslash (or forward slash depending on the operating
		 *	system).
		 *
		 * \param _pcRet Buffer to be filled with the path to this executable.
		 * \return Returns _pcRet.
		 */
		static char * LSE_CALL				GetSelfDir( char * _pcRet );

		/**
		 * Gets the path to the directory in which this executable resides.  Returns the string that was
		 *	put into it.  Must be at least LSF_MAX_PATH characters long.  Returns NULL in the event of error.
		 * The returned string always ends with a backslash (or forward slash depending on the operating
		 *	system).
		 *
		 * \param _pwcRet Buffer to be filled with the path to this executable.
		 * \return Returns _pwcRet.
		 */
		static wchar_t * LSE_CALL			GetSelfDir( wchar_t * _pwcRet );

		/**
		 * Fully expands the given path.  The result is stored in _pcRet, which must be at least LSF_MAX_PATH
		 *	characters long.  Returns _pcRet or NULL on error.
		 *
		 * \param _pcPath Path to expand, in UTF-8 format.
		 * \param _pcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
		 * \return Returns _pcRet or NULL on error.  On success, _pcRet will be filled with a
		 *	NULL-terminated UTF-8 string representing the expanded path.
		 */
		static char * LSE_CALL				ExpandPath( const char * _pcPath, char * _pcRet );

		/**
		 * Fully expands the given path.  The result is stored in _pwcRet, which must be at least LSF_MAX_PATH
		 *	characters long.  Returns _pwcRet or NULL on error.
		 *
		 * \param _pwcPath Path to expand.
		 * \param _pwcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
		 * \return _pwcRet or NULL on error.
		 */
		static wchar_t * LSE_CALL			ExpandPath( const wchar_t * _pwcPath, wchar_t * _pwcRet );

		/**
		 * Fully expands the given path, which is assumed to be relative to the resources directory.  If the given
		 *	path is not, an invalid path will be created.
		 *
		 * \param _pcPath Path to expand, in UTF-8 format.
		 * \param _pcRet Buffer to receive the expanded path.  Must be at least LSF_MAX_PATH characters long.
		 * \return Returns _pcRet or NULL on error.  On success, _pcRet will be filled with a
		 *	NULL-terminated UTF-8 string representing the expanded path to the resource folder.
		 */
		static char * LSE_CALL				ExpandResourcePath( const char * _pcPath, char * _pcRet );

		/**
		 * Removes the extension from a path.  Returns the input string, which is modified in-place.
		 *	The returned string is in UTF-8 format.
		 *
		 * \param _pcPath On input, it holds the path whose extension is to be removed.  On return, it
		 *	holds the path without the removed extension.
		 * \return Returns _pcPath.
		 */
		static char * LSE_CALL				NoExtension( char * _pcPath );

		/**
		 * Removes the extension from a path.  Returns the input string, which is modified in-place.
		 *
		 * \param _pwcPath On input, it holds the path whose extension is to be removed.  On return, it
		 *	holds the path without the removed extension.
		 * \return Returns _pwcPath.
		 */
		static wchar_t * LSE_CALL			NoExtension( wchar_t * _pwcPath );

		/**
		 * Removes the path part from a path (leaves only the file part).  Returns the input string,
		 *	which is modified in-place.
		 *	The returned string is in UTF-8 format.
		 *
		 * \param _pcPath On input, it holds the string whose path part is to be removed.  On return,
		 *	it holds the string removed of its path.
		 * \return Returns _pcPath.
		 */
		static char * LSE_CALL				NoPath( char * _pcPath );

		/**
		 * Removes the path part from a path (leaves only the file part).  Returns the input string,
		 *	which is modified in-place.
		 *
		 * \param _pwcPath On input, it holds the string whose path part is to be removed.  On return,
		 *	it holds the string removed of its path.
		 * \return Returns _pwcPath.
		 */
		static wchar_t * LSE_CALL			NoPath( wchar_t * _pwcPath );

		/**
		 * Removes the file part of a path (leaves only the folder part).  The return includes the terminating
		 *	/ or \.  If there is no folder part to return, ./ is returned.  Returns the input string,
		 *	which is modified in-place.
		 *
		 * \param _pcPath On input, it holds the string whose file part is to be removed.  On return,
		 *	it holds the string removed of its file.
		 * \return Returns _pcPath.
		 */
		static char * LSE_CALL				NoFile( char * _pcPath );

		/**
		 * Removes the file part of a path (leaves only the folder part).  The return includes the terminating
		 *	/ or \.  If there is no folder part to return, ./ is returned.  Returns the input string,
		 *	which is modified in-place.
		 *
		 * \param _pwcPath On input, it holds the string whose file part is to be removed.  On return,
		 *	it holds the string removed of its file.
		 * \return Returns _pwcPath.
		 */
		static wchar_t * LSE_CALL			NoFile( wchar_t * _pwcPath );

		/**
		 * Gets a pointer to the extension, not including the . (period).
		 *
		 * \param _pcPath Holds the file name (including the path or not) whose extension is to be found.
		 * \return Returns a pointer to the position within the given string where the extension begins.
		 */
		static const char * LSE_CALL		GetExtension( const char * _pcPath );

		/**
		 * Gets a pointer to the extension, not including the . (period).
		 *
		 * \param _pwcPath Holds the file name (including the path or not) whose extension is to be found.
		 * \return Returns a pointer to the position within the given string where the extension begins.
		 */
		static const wchar_t * LSE_CALL		GetExtension( const wchar_t * _pwcPath );

		/**
		 * Returns the system EOF (end-of-file) value.  This is normally -1 but is not guaranteed to be so on every platform/compiler.
		 *
		 * \return Returns the system EOF value.
		 */
		static int32_t LSE_CALL				Eof();

		/**
		 * Sets the resource path.
		 *
		 * \param _pcPath The resource path to set.
		 */
		static void LSE_CALL				SetResPath( const char * _pcPath );

		/**
		 * Gets the resource path.
		 *
		 * \return Returns a pointer to the string containing the path to the resources.
		 */
		static LSE_INLINE const char * LSE_CALL
											GetResPath();


	protected :
		/**
		 * The resource path.
		 */
		static char							m_szResourcePath[LSF_MAX_PATH];

		/**
		 * Length of the resource path in characters.
		 */
		static uint32_t						m_ui32ResPathLen;
	};

	// == Functions.
	/**
	 * Gets the resource path.
	 *
	 * \return Returns a pointer to the string containing the path to the resources.
	 */
	LSE_INLINE const char * LSE_CALL CFileLib::GetResPath() {
		return m_szResourcePath;
	}

}	// namespace lsf

#endif	// __LSF_FILELIB_H__
