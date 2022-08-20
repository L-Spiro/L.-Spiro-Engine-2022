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

#include "LSFFileMap.h"


namespace lsf {

	// == Various constructors.
	LSE_CALLCTOR CFileMap::CFileMap() :
		m_bMapped( false ),
		m_pvAddr( NULL ),
		m_uiptrSize( 0 ),
		m_uiptrCurSize( 0 ),
		m_ui64FileSize( 0ULL ),
		m_ui64Offset( 0 ) {
	}
	LSE_CALLCTOR CFileMap::~CFileMap() {
		Reset();
	}

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
	LSBOOL LSE_CALL CFileMap::CreateFileMap( const char * _pcFile, LSF_OPEN_FILE _ofOpenType, uint32_t _ui32BufferSize ) {
		wchar_t szBuffer[LSF_MAX_PATH];
		CStd::Utf8ToUtfX( szBuffer, reinterpret_cast<const LSUTF8 *>(_pcFile), LSF_MAX_PATH );
		return CreateFileMap( szBuffer, _ofOpenType, _ui32BufferSize );
	}

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
	LSBOOL LSE_CALL CFileMap::CreateFileMap( const wchar_t * _pwcFile, LSF_OPEN_FILE _ofOpenType, uint32_t _ui32BufferSize ) {
		Reset();

		if ( !CFileLib::CreateFileMap( _pwcFile, _ofOpenType, m_fmMap ) ) {
			return false;
		}

		// If the buffer size is 0, make it 1 so that the next line will round it up to a single page.
		if ( _ui32BufferSize == 0 ) { ++_ui32BufferSize; }
		m_uiptrSize = LSE_ROUND_UP( _ui32BufferSize, CFileLib::GetPageSize() << 5 );

		m_ui64FileSize = CFileLib::GetFileSize( _pwcFile );
		m_bMapped = true;
		return true;
	}

	/**
	 * Close the mapped file and discard all allocated resources.
	 */
	void LSE_CALL CFileMap::Reset() {
		if ( m_bMapped ) {
			CFileLib::UnMap( m_pvAddr, m_uiptrCurSize );
			CFileLib::DestroyFileMap( m_fmMap );
			m_bMapped = false;
			m_pvAddr = NULL;
			m_uiptrSize = m_uiptrCurSize = 0;
			m_ui64FileSize = 0;
			m_ui64Offset = 0;
		}
	}

	/**
	 * Read bytes from the mapped file.  The requested bytes are copied to the destination buffer.
	 *
	 * \param _pvBuffer Buffer where to store the read bytes.  Must be at least _ui32Total in length.
	 * \param _ui64Offset Offset in the file from which to read the bytes.
	 * \param _ui32Total Total bytes to read.
	 * \return Returns the actual number of bytes read.
	 */
	uint32_t LSE_CALL CFileMap::ReadBytes( void * _pvBuffer, uint64_t _ui64Offset, uint32_t _ui32Total ) const {
		if ( !m_bMapped ) { return 0; }
		// Get the bytes from the file in sections.
		// GetBuffer() is already optimized to keep buffers in memory and will read directly from the buffer
		//	if it is still loaded.  So the purpose of this function is to allow access to bytes of any range
		//	the user wants, using GetBuffer() to collect each segment of the file.
		uint8_t * pui8Buffer = reinterpret_cast<uint8_t *>(_pvBuffer);
		uint32_t ui32TotalCopied = 0;
		while ( _ui32Total != 0 && _ui64Offset < m_ui64FileSize ) {
			uint32_t ui32Max;
			uint8_t * pui8Source = GetBuffer( _ui64Offset, ui32Max );
			uint32_t ui32ThisLength = CStd::Min( _ui32Total, ui32Max );
			if ( _ui64Offset + ui32ThisLength > m_ui64FileSize ) {
				ui32ThisLength = static_cast<uint32_t>(m_ui64FileSize - _ui64Offset);
			}

			
			if ( pui8Source != NULL ) {
				/*
				char szBuffer[256];
				CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ),
					"CFileMap::ReadBytes(): Copying: %X, %X, %X\r\n",
					pui8Buffer, pui8Source, ui32ThisLength );
				CStd::DebugPrintA( szBuffer );*/
				ui32TotalCopied += ui32ThisLength;
				CStd::MemCpy( pui8Buffer, pui8Source, ui32ThisLength );
			}
			else {
				// Stop copying.
				return ui32TotalCopied;
			}

			_ui32Total -= ui32ThisLength;
			pui8Buffer += ui32ThisLength;
			_ui64Offset += ui32ThisLength;
		}
		
		
		return ui32TotalCopied;
	}

	/**
	 * Write bytes to the mapped file.  File must have been mapped with writing permissions.
	 *
	 * \param _pui8Buffer Source buffer.  Must be at least _ui32Total in length.
	 * \param _uiptrOffset Offset in the file at which to write the bytes.
	 * \param _ui32Total Total bytes to write.
	 * \return Returns the actual number of bytes written.
	 */
	uint32_t LSE_CALL CFileMap::WriteBytes( const uint8_t * _pui8Buffer, uint64_t _ui64Offset, uint32_t _ui32Total ) {
		if ( !m_bMapped ) { return 0; }
		// If not opened with write permissions exit before we crash due to access violations.
		if ( !CFileLib::CanWriteToFileMap( m_fmMap ) ) { return 0; }
		// Get the bytes from the file in sections.
		// GetBuffer() is already optimized to keep buffers in memory and will read directly from the buffer
		//	if it is still loaded.  So the purpose of this function is to allow access to bytes of any range
		//	the user wants, using GetBuffer() to collect each segment of the file.

		uint32_t ui32TotalCopied = 0;
		while ( _ui32Total != 0 && _ui64Offset < m_ui64FileSize ) {
			uint32_t ui32Max;
			uint8_t * pui8Source = GetBuffer( _ui64Offset, ui32Max );
			uint32_t ui32ThisLength = CStd::Min( _ui32Total, ui32Max );
			if ( _ui64Offset + ui32ThisLength > m_ui64FileSize ) {
				ui32ThisLength = static_cast<uint32_t>(m_ui64FileSize - _ui64Offset);
			}

			
			if ( pui8Source != NULL ) {
				CStd::MemCpy( pui8Source, _pui8Buffer, ui32ThisLength );
				ui32TotalCopied += ui32ThisLength;
			}
			else {
				// Stop copying.
				return ui32TotalCopied;
			}

			_ui32Total -= ui32ThisLength;
			_pui8Buffer += ui32ThisLength;
			_ui64Offset += ui32ThisLength;
		}
		
		
		return ui32TotalCopied;
	}

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
	uint8_t * LSE_CALL CFileMap::GetBuffer( uint64_t _ui64Offset, uint32_t &_ui32MaxLen ) const {
		uint64_t ui64NewOffset = GetBaseOffset( _ui64Offset );

		// Are we already mapping this region?
		if ( m_pvAddr != NULL && ui64NewOffset == m_ui64Offset ) {
			_ui64Offset -= ui64NewOffset;
			_ui32MaxLen = static_cast<uint32_t>(m_uiptrCurSize - _ui64Offset);
			/*
			char szBuffer[256];
			CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ),
				"CFileMap::GetBuffer(): Using previous data: %X, %X, %X %X %X\r\n",
				static_cast<uint32_t>(_ui64Offset), _ui32MaxLen, static_cast<uint32_t>(m_ui64Offset), m_uiptrCurSize, m_pvAddr );
			CStd::DebugPrintA( szBuffer );
			*/
			return &reinterpret_cast<uint8_t *>(m_pvAddr)[_ui64Offset];
		}

		// Map the new region.
		if ( m_pvAddr != NULL ) {
			CFileLib::UnMap( m_pvAddr, m_uiptrCurSize );
		}

		m_ui64Offset = ui64NewOffset;
		m_uiptrCurSize = CStd::Min( static_cast<uintptr_t>(m_ui64FileSize - ui64NewOffset), m_uiptrSize );
		if ( ui64NewOffset >= m_ui64FileSize ) {
			m_uiptrCurSize = 0;
		}
		m_pvAddr = CFileLib::Map( m_fmMap, m_ui64Offset, m_uiptrCurSize );
		if ( !m_pvAddr ) {
			_ui32MaxLen = 0;
			m_uiptrCurSize = 0;
			return NULL;
		}

		_ui64Offset -= ui64NewOffset;
		_ui32MaxLen = static_cast<uint32_t>(m_uiptrCurSize - _ui64Offset);

		/*
		char szBuffer[256];
		CStd::SPrintF( szBuffer, LSE_ELEMENTS( szBuffer ),
			"CFileMap::GetBuffer(): Using new data: %X, %X, %X, %X %X\r\n",
			static_cast<uint32_t>(_ui64Offset), _ui32MaxLen, static_cast<uint32_t>(m_ui64Offset), m_uiptrCurSize, m_pvAddr );
		CStd::DebugPrintA( szBuffer );
		*/
		return &reinterpret_cast<uint8_t *>(m_pvAddr)[_ui64Offset];
	}

}	// namespace lsf
