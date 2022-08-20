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


#ifndef __LSF_FILESEX_H__
#define __LSF_FILESEX_H__

#include "../LSFFileLib.h"
#include "Allocators/LSAStdAllocator.h"

namespace lsf {

	/**
	 * Class CFilesEx
	 * \brief High-level file functions.
	 *
	 * Description: Building off the low-level file routines, this class provides higher-level file
	 *	functionality such as loading files to RAM.
	 */
	class CFilesEx {
	public :
		// == Functions.
		/**
		 * Initialize the extended file functionality.  Required before LoadFileToMemory() and FreeFile() can be used with a NULL allocator.
		 */
		static void LSE_CALL				InitFilesEx();

		/**
		 * Shut down the extended file functionality when the engine closes.
		 */
		static void LSE_CALL				ShutDownFilesEx();

		/**
		 * Load a file fully to RAM.
		 *
		 * \param _pcFile Path to the file to load.  Must be in UTF-8 format.
		 * \param _pui8Buffer Holds the returned file data.  This buffer is allocated using _psaAllocator.
		 * \param _puiptrSize Optional value to hold the size of the loaded file.  If not needed, set to NULL.
		 * \param _psaAllocator Optional pointer to the allocator to use to allocate _pui8Buffer.
		 * \return Returns true on success, false otherwise.
		 */
		static LSBOOL LSE_CALL				LoadFileToMemory( const char * _pcFile, uint8_t * &_pui8Buffer, uintptr_t * _puiptrSize,
			CStdAllocator * _psaAllocator = NULL );

		/**
		 * Removes from RAM a file that was loaded via LoadFileToMemory().
		 *
		 * \param _pui8Buffer The file data previously filled by a successful call to LoadFileToMemory().
		 * \param _psaAllocator Optional pointer to the allocator used to allocate _pui8Buffer.  This should be the same value that was passed
		 *	to LoadFileToMemory(), even (especially) if NULL was passed.
		 */
		static void LSE_CALL				FreeFile( uint8_t * _pui8Buffer, CStdAllocator * _psaAllocator = NULL );

		/**
		 * Write a buffer to a file.  If the file exists its contents are erased.
		 *
		 * \param _pcPath Pointer to a string containing the path of the file to create.  Its contents will be overwritten.
		 * \param _pui8Buffer The buffer to write to the given file path.
		 * \param _uiptrSize Number of bytes to write.
		 * \return Returns true if the file was created and its contents written.
		 */
		static LSBOOL LSE_CALL				SendBufferToFile( const char * _pcPath, const uint8_t * _pui8Buffer, uintptr_t _uiptrSize );


	protected :
		// == Members.
		/**
		 * Default allocator.
		 */
		static CStdAllocator *				m_psaAllocator;
	};
}	// namespace lsf

#endif	// __LSF_FILESEX_H__
