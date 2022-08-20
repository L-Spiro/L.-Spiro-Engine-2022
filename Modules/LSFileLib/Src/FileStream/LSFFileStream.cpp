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
 * Description: File streaming using standard CFileLib calls.
 */

#include "LSFFileStream.h"


namespace lsf {

	// == Various constructors.
	LSE_CALLCTOR CFileStream::CFileStream( const char * _pcFile, LSF_OPEN_FILE _ofOpenType, LSBOOL _bOpenAlways ) {
		m_fFile = CFileLib::OpenFile( _pcFile, _ofOpenType, _bOpenAlways );
		if ( !m_fFile ) {
			CStd::DebugPrintA( "FAILED TO OPEN FILE " );
			CStd::DebugPrintA( _pcFile );
			CStd::DebugPrintA( ".\r\n" );
		}
	}
	LSE_CALLCTOR CFileStream::~CFileStream() {
		CFileLib::CloseFile( m_fFile );
	}

	// == Functions.
	/**
	 * Read bytes from the stream.  Stream position is advanced by the amount read.
	 *
	 * \param _pui8Buffer Buffer to hold the bytes read from the stream.
	 * \param _ui32BytesToRead Number of bytes to read.
	 * \return Returns the actual number of bytes read or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CFileStream::ReadBytes( uint8_t * _pui8Buffer, uint32_t _ui32BytesToRead ) const {
		if ( !CFileLib::ReadFile( m_fFile, _pui8Buffer, _ui32BytesToRead ) ) { return 0; }
		return _ui32BytesToRead;
	}

	/**
	 * Reads a 64-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit unsigned integer at the current position in the stream.
	 */
	uint64_t LSE_CALL CFileStream::ReadUInt64() const {
		uint64_t ui64Ret;
		if ( !CFileLib::ReadFile( m_fFile, &ui64Ret, sizeof( ui64Ret ) ) ) { return 0ULL; }
		return ui64Ret;
	}
	
	/**
	 * Reads a 32-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit unsigned integer at the current position in the stream.
	 */
	uint32_t LSE_CALL CFileStream::ReadUInt32() const {
		uint32_t ui32Ret;
		if ( !CFileLib::ReadFile( m_fFile, &ui32Ret, sizeof( ui32Ret ) ) ) { return 0; }
		return ui32Ret;
	}

	/**
	 * Reads a 16-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 16-bit unsigned integer at the current position in the stream.
	 */
	uint16_t LSE_CALL CFileStream::ReadUInt16() const {
		uint16_t ui16Ret;
		if ( !CFileLib::ReadFile( m_fFile, &ui16Ret, sizeof( ui16Ret ) ) ) { return 0; }
		return ui16Ret;
	}

	/**
	 * Reads an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 8-bit unsigned integer at the current position in the stream.
	 */
	uint8_t LSE_CALL CFileStream::ReadUInt8() const {
		uint8_t ui8Ret;
		if ( !CFileLib::ReadFile( m_fFile, &ui8Ret, sizeof( ui8Ret ) ) ) { return 0; }
		return ui8Ret;
	}

	/**
	 * Reads a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit float at the current position in the stream.
	 */
	float LSE_CALL CFileStream::ReadFloat() const {
		float fRet;
		if ( !CFileLib::ReadFile( m_fFile, &fRet, sizeof( fRet ) ) ) { return 0.0f; }
		return fRet;
	}

	/**
	 * Reads a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit double at the current position in the stream.
	 */
	double LSE_CALL CFileStream::ReadDouble() const {
		double dRet;
		if ( !CFileLib::ReadFile( m_fFile, &dRet, sizeof( dRet ) ) ) { return 0.0; }
		return dRet;
	}

	/**
	 * Write bytes to the stream.  Stream position is advanced by the amount written.
	 *
	 * \param _pui8Buffer Buffer containing the bytes to write to the stream.
	 * \param _ui32BytesToWrite Number of bytes to write.
	 * \return Returns the actual number of bytes written or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CFileStream::WriteBytes( const uint8_t * _pui8Buffer, uint32_t _ui32BytesToWrite ) {
		return CFileLib::WriteFile( m_fFile, _pui8Buffer, _ui32BytesToWrite ) ? _ui32BytesToWrite : LSSTD_SE_ERROR;
	}

	/**
	 * Writes a 64-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteUInt64( const uint64_t &_ui64Value ) {
		return CFileLib::WriteFile( m_fFile, &_ui64Value, sizeof( _ui64Value ) );
	}
	
	/**
	 * Writes a 32-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteUInt32( uint32_t _ui32Value ) {
		return CFileLib::WriteFile( m_fFile, &_ui32Value, sizeof( _ui32Value ) );
	}

	/**
	 * Writes a 16-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteUInt16( uint16_t _ui16Value ) {
		return CFileLib::WriteFile( m_fFile, &_ui16Value, sizeof( _ui16Value ) );
	}

	/**
	 * Writes an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteUInt8( uint8_t _ui8Value ) {
		return CFileLib::WriteFile( m_fFile, &_ui8Value, sizeof( _ui8Value ) );
	}

	/**
	 * Writes a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteFloat( float _fValue ) {
		return CFileLib::WriteFile( m_fFile, &_fValue, sizeof( _fValue ) );
	}

	/**
	 * Writes a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CFileStream::WriteDouble( const double &_dValue ) {
		return CFileLib::WriteFile( m_fFile, &_dValue, sizeof( _dValue ) );
	}

}	// namespace lsf
