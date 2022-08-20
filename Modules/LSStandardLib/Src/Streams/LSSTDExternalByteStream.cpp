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
 * Description: Allows treating an external array of bytes as a stream.  Allows modifying the array but not
 *	beyond the actual length of the array.  This is useful for situations when you have a file image loaded to
 *	memory and you wish to read from it in the fashion of a byte stream.
 */

#include "LSSTDExternalByteStream.h"
#include <cassert>


namespace lsstd {

	// == Various constructors.
	LSE_CALLCTOR CExternalByteStream::CExternalByteStream() :
		m_pui8Stream( NULL ),
		m_uiptrCurPos( 0 ),
		m_uiptrLength( 0 ),
		m_bReadOnly( false ) {
	}
	LSE_CALLCTOR CExternalByteStream::CExternalByteStream( uint8_t * _pui8Buffer, uintptr_t _uiptrLen ) :
		m_pui8Stream( _pui8Buffer ),
		m_uiptrCurPos( 0 ),
		m_uiptrLength( _uiptrLen ),
		m_bReadOnly( false ) {
		if ( !m_pui8Stream ) {
			m_uiptrCurPos = m_uiptrLength = 0;
		}
	}
	LSE_CALLCTOR CExternalByteStream::CExternalByteStream( const uint8_t * _pui8Buffer, uintptr_t _uiptrLen ) :
		m_pui8Stream( const_cast<uint8_t *>(_pui8Buffer) ),
		m_uiptrCurPos( 0 ),
		m_uiptrLength( _uiptrLen ),
		m_bReadOnly( true ) {
		if ( !m_pui8Stream ) {
			m_uiptrCurPos = m_uiptrLength = 0;
		}
	}

	// == Functions.
	/**
	 * Read bytes from the stream.  Stream position is advanced by the amount read.
	 *
	 * \param _pui8Buffer Buffer to hold the bytes read from the stream.
	 * \param _ui32BytesToRead Number of bytes to read.
	 * \return Returns the actual number of bytes read or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CExternalByteStream::ReadBytes( uint8_t * _pui8Buffer, uint32_t _ui32BytesToRead ) const {
		// If reading beyond the end of the buffer.
		if ( m_uiptrCurPos + _ui32BytesToRead > m_uiptrLength ) {
			uintptr_t uiptrSize = m_uiptrLength - m_uiptrCurPos;
			CStd::MemCpy( _pui8Buffer, &m_pui8Stream[m_uiptrCurPos], uiptrSize );
			m_uiptrCurPos += uiptrSize;
			return static_cast<uint32_t>(uiptrSize);
		}

		CStd::MemCpy( _pui8Buffer, &m_pui8Stream[m_uiptrCurPos], _ui32BytesToRead );
		m_uiptrCurPos += _ui32BytesToRead;
		return _ui32BytesToRead;
	}

	/**
	 * Reads a 64-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit unsigned integer at the current position in the stream.
	 */
	uint64_t LSE_CALL CExternalByteStream::ReadUInt64() const {
		uint64_t ui64Ret = 0ULL;
		ReadBytes( reinterpret_cast<uint8_t *>(&ui64Ret), sizeof( ui64Ret ) );
		return ui64Ret;
	}
	
	/**
	 * Reads a 32-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit unsigned integer at the current position in the stream.
	 */
	uint32_t LSE_CALL CExternalByteStream::ReadUInt32() const {
		uint32_t ui32Ret = 0;
		ReadBytes( reinterpret_cast<uint8_t *>(&ui32Ret), sizeof( ui32Ret ) );
		return ui32Ret;
	}

	/**
	 * Reads a 16-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 16-bit unsigned integer at the current position in the stream.
	 */
	uint16_t LSE_CALL CExternalByteStream::ReadUInt16() const {
		uint16_t ui16Ret = 0;
		ReadBytes( reinterpret_cast<uint8_t *>(&ui16Ret), sizeof( ui16Ret ) );
		return ui16Ret;
	}

	/**
	 * Reads an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 8-bit unsigned integer at the current position in the stream.
	 */
	uint8_t LSE_CALL CExternalByteStream::ReadUInt8() const {
		uint8_t ui8Ret = 0;
		ReadBytes( reinterpret_cast<uint8_t *>(&ui8Ret), sizeof( ui8Ret ) );
		return ui8Ret;
	}

	/**
	 * Reads a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit float at the current position in the stream.
	 */
	float LSE_CALL CExternalByteStream::ReadFloat() const {
		float fRet = 0.0f;
		ReadBytes( reinterpret_cast<uint8_t *>(&fRet), sizeof( fRet ) );
		return fRet;
	}

	/**
	 * Reads a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit double at the current position in the stream.
	 */
	double LSE_CALL CExternalByteStream::ReadDouble() const {
		double dRet = 0.0;
		ReadBytes( reinterpret_cast<uint8_t *>(&dRet), sizeof( dRet ) );
		return dRet;
	}

	/**
	 * Pad-reads up to a multiple of the given value.  The values are discarded.
	 *
	 * \param _uiptrMultiple The multiple up to which to discard-read values.
	 */
	void LSE_CALL CExternalByteStream::PadRead( uintptr_t _uiptrMultiple ) const {
		m_uiptrCurPos = ((m_uiptrCurPos + (_uiptrMultiple - 1)) / _uiptrMultiple) * _uiptrMultiple;
		m_uiptrCurPos = CStd::Min<uintptr_t>( m_uiptrCurPos, m_uiptrLength );
	}

	/**
	 * Skips the given number of bytes.
	 *
	 * \param _uiptrSkip The number of bytes to skip.
	 */
	void LSE_CALL CExternalByteStream::SkipBytes( uintptr_t _uiptrSkip ) const {
		m_uiptrCurPos += _uiptrSkip;
		m_uiptrCurPos = CStd::Min<uintptr_t>( m_uiptrCurPos, m_uiptrLength );
	}

	/**
	 * Write bytes to the stream.  Stream position is advanced by the amount written.
	 *
	 * \param _pui8Buffer Buffer containing the bytes to write to the stream.
	 * \param _ui32BytesToWrite Number of bytes to write.
	 * \return Returns the actual number of bytes written or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CExternalByteStream::WriteBytes( const uint8_t * _pui8Buffer, uint32_t _ui32BytesToWrite ) {
		assert( !m_bReadOnly );
		// If writing beyond the end of the buffer.
		if ( m_uiptrCurPos + _ui32BytesToWrite > m_uiptrLength ) {
			uintptr_t uiptrSize = m_uiptrLength - m_uiptrCurPos;
			CStd::MemCpy( &m_pui8Stream[m_uiptrCurPos], _pui8Buffer, uiptrSize );
			m_uiptrCurPos += uiptrSize;
			return static_cast<uint32_t>(uiptrSize);
		}

		CStd::MemCpy( &m_pui8Stream[m_uiptrCurPos], _pui8Buffer, _ui32BytesToWrite );
		m_uiptrCurPos += _ui32BytesToWrite;
		return _ui32BytesToWrite;
	}

	/**
	 * Writes a 64-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteUInt64( const uint64_t &_ui64Value ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _ui64Value ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_ui64Value), sizeof( _ui64Value ) );
		return true;
	}
	
	/**
	 * Writes a 32-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteUInt32( uint32_t _ui32Value ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _ui32Value ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_ui32Value), sizeof( _ui32Value ) );
		return true;
	}

	/**
	 * Writes a 16-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteUInt16( uint16_t _ui16Value ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _ui16Value ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_ui16Value), sizeof( _ui16Value ) );
		return true;
	}

	/**
	 * Writes an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteUInt8( uint8_t _ui8Value ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _ui8Value ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_ui8Value), sizeof( _ui8Value ) );
		return true;
	}

	/**
	 * Writes a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteFloat( float _fValue ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _fValue ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_fValue), sizeof( _fValue ) );
		return true;
	}

	/**
	 * Writes a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CExternalByteStream::WriteDouble( const double &_dValue ) {
		assert( !m_bReadOnly );
		if ( m_uiptrCurPos + sizeof( _dValue ) > m_uiptrLength ) {
			return false;
		}
		WriteBytes( reinterpret_cast<const uint8_t *>(&_dValue), sizeof( _dValue ) );
		return true;
	}

	/**
	 * Sets the stream position.  Actual position set is clamped between 0 and the length of the stream.
	 *
	 * \param _uiptrPos New position of the stream.
	 */
	void LSE_CALL CExternalByteStream::SetPos( uintptr_t _uiptrPos ) const {
		m_uiptrCurPos = CStd::Min( m_uiptrLength, _uiptrPos );
	}

	/**
	 * Gets the current position of the stream pointer.
	 *
	 * \return Returns the position of the stream pointer.  The stream pointer indicates where the next
	 *	stream operation (reading or writing) will take place.
	 */
	uintptr_t LSE_CALL CExternalByteStream::GetPos() const {
		return m_uiptrCurPos;
	}

	/**
	 * Gets the length of the stream.
	 *
	 * \return Returns the length, in bytes, of the stream.
	 */
	uintptr_t LSE_CALL CExternalByteStream::Length() const {
		return m_uiptrLength;
	}

	/**
	 * Gets a pointer to the start of the buffer.
	 *
	 * \return Returns a pointer to the start of the buffer.
	 */
	const uint8_t * LSE_CALL CExternalByteStream::GetBuffer() const{ 
		return m_pui8Stream;
	}

	/**
	 * Gets a pointer to the current location in the buffer.
	 *
	 * \return Returns a pointer to the current location in the buffer.
	 */
	const uint8_t * LSE_CALL CExternalByteStream::GetCurBufferPointer() const {
		return &GetBuffer()[GetPos()];
	}

}	// namespace lsstd
