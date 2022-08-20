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
 * Description: A wrapper class for file-mapping routines.  This class makes it easy to map files and use the
 *	maps in an efficient and safe way.
 */


#ifndef __LSF_FILEMAP_H__
#define __LSF_FILEMAP_H__

#include "../LSFFileLib.h"

namespace lsf {

	/**
	 * Class CFileMap
	 * \brief A wrapper class for file-mapping routines.
	 *
	 * Description: A wrapper class for file-mapping routines.  This class makes it easy to map files and use the
	 *	maps in an efficient and safe way.
	 */
	class CFileMap {
	public :
		// == Various constructors.
		LSE_CALLCTOR			CFileMap();
		LSE_CALLCTOR			~CFileMap();


		// == Functions.
		/**
		 * Creates a file map.
		 *
		 * \param _pcFile The file to map.  The entire file is mapped.
		 * \param _ofOpenType Flag determining how to open the file.  Valid values are LSF_OF_READ,
		 *	LSF_OF_WRITE, or both.
		 * \param _ui32BufferSize Buffer size.  Reading operations buffer this much of the file at a time.  This
		 *	allows high RAM conservation.
		 * \return Returns true if mapping was successful.
		 */
		LSBOOL LSE_CALL			CreateFileMap( const char * _pcFile, LSF_OPEN_FILE _ofOpenType = LSF_OF_READ, uint32_t _ui32BufferSize = 0 );

		/**
		 * Creates a file map.
		 *
		 * \param _pwcFile The file to map.  The entire file is mapped.
		 * \param _ofOpenType Flag determining how to open the file.  Valid values are LSF_OF_READ,
		 *	LSF_OF_WRITE, or both.
		 * \param _ui32BufferSize Buffer size.  Reading operations buffer this much of the file at a time.  This
		 *	allows high RAM conservation.
		 * \return Returns true if mapping was successful.
		 */
		LSBOOL LSE_CALL			CreateFileMap( const wchar_t * _pwcFile, LSF_OPEN_FILE _ofOpenType = LSF_OF_READ, uint32_t _ui32BufferSize = 0 );

		/**
		 * Close the mapped file and discard all allocated resources.
		 */
		void LSE_CALL			Reset();

		/**
		 * Read bytes from the mapped file.  The requested bytes are copied to the destination buffer.
		 *
		 * \param _pvBuffer Buffer where to store the read bytes.  Must be at least _ui32Total in length.
		 * \param _ui64Offset Offset in the file from which to read the bytes.
		 * \param _ui32Total Total bytes to read.
		 * \return Returns the actual number of bytes read.
		 */
		uint32_t LSE_CALL		ReadBytes( void * _pvBuffer, uint64_t _ui64Offset, uint32_t _ui32Total ) const;

		/**
		 * Write bytes to the mapped file.  File must have been mapped with writing permissions.
		 *
		 * \param _pui8Buffer Source buffer.  Must be at least _ui32Total in length.
		 * \param _ui64Offset Offset in the file at which to write the bytes.
		 * \param _ui32Total Total bytes to write.
		 * \return Returns the actual number of bytes written.
		 */
		uint32_t LSE_CALL		WriteBytes( const uint8_t * _pui8Buffer, uint64_t _ui64Offset, uint32_t _ui32Total );

		/**
		 * Gets the size of the mapped file.
		 *
		 * \return Returns the size of the mapped file, in bytes.
		 */
		uint64_t LSE_CALL		GetFileSize() const;

	protected :
		// == Members.
		// Our file map.
		LSF_FILE_MAP			m_fmMap;

		// Have we mapped a file?
		LSBOOL					m_bMapped;

		// Address we have mapped.
		mutable void *		m_pvAddr;

		// Size of the mapping.
		uintptr_t				m_uiptrSize;

		// Current size of the mapping.
		mutable uintptr_t		m_uiptrCurSize;

		// Size of the file.
		uint64_t				m_ui64FileSize;

		// Currently buffered offset.
		mutable uint64_t		m_ui64Offset;


		// == Functions.
		/**
		 * Gets the base of the pointer (offset) for the current chunk buffered.
		 *	Does not actually set the offset value, but instead derives it to be used as a reference
		 *	by the other functions.
		 *
		 * \param _ui64Offset The desired offset into the file to be used as a base for the calculation of
		 *	the chunk base.
		 * \return Returns the base offset of the chunk that matches the alignment settings and covers the
		 *	given offset.
		 */
		uint64_t LSE_CALL		GetBaseOffset( uint64_t _ui64Offset ) const;

		/**
		 * Gets a pointer to the requested data from the mapped file.  Returns a pointer inside the current buffer
		 *	and a value indicating the valid length of the data within the buffer.
		 *
		 * \param _ui64Offset File offset to be translated to a virtual address containing the file data.
		 * \param _ui32MaxLen Upon return, this is filled with the number of bytes that are valid from the
		 *	returned pointer to the end of the current buffer.
		 * \return Returns a pointer within the current buffer that represents the requested offset within the
		 *	file, along with the amount of bytes that are valid from that pointer to the end of the buffer.
		 */
		uint8_t * LSE_CALL		GetBuffer( uint64_t _ui64Offset, uint32_t &_ui32MaxLen ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Gets the size of the mapped file.
	 *
	 * \return Returns the size of the mapped file, in bytes.
	 */
	LSE_INLINE uint64_t LSE_CALL CFileMap::GetFileSize() const {
		return m_ui64FileSize;
	}

	/**
	 * Gets the base of the pointer (offset) for the current chunk buffered.
	 *	Does not actually set the offset value, but instead derives it to be used as a reference
	 *	by the other functions.
	 *
	 * \param _ui64Offset The desired offset into the file to be used as a base for the calculation of
	 *	the chunk base.
	 * \return Returns the base offset of the chunk that matches the alignment settings and covers the
	 *	given offset.
	 */
	LSE_INLINE uint64_t LSE_CALL CFileMap::GetBaseOffset( uint64_t _ui64Offset ) const {
		if ( _ui64Offset < (m_uiptrSize >> 1) ) { return 0; }
		if ( m_ui64FileSize <= m_uiptrSize ) { return 0; }
		return ((_ui64Offset + (m_uiptrSize >> 2)) & (~((m_uiptrSize >> 1) - 1))) - (m_uiptrSize >> 1);
	}

}	// namespace lsf

#endif	// __LSF_FILEMAP_H__
