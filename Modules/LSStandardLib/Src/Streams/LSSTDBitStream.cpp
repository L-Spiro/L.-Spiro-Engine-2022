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
 * Description: Provides a generic bit stream.
 */

#include "LSSTDBitStream.h"
#include <cassert>
#include <cstdlib>


namespace lsstd {

	// == Various constructors.
	LSE_CALLCTOR CBitStream::CBitStream() :
		m_pui32Buffer( NULL ),
		m_ui32Total( 0 ),
		m_ui32Alloc( 0 ),
		m_ui64Pos( 0ULL ),
		m_ui64TotalBits( 0ULL ),
		m_bBorrowed( false ) {
	}
	LSE_CALLCTOR CBitStream::~CBitStream() {
		Reset();
	}

	// == Functions.
	/**
	 * Allocates the given number of uint32_t's and store the result in m_pui32Buffer.  Note that this
	 *	function should not be overloaded when adding memory-manager functionality to this class.
	 *
	 * \param _ui32Total Total number of uint32_t's to allocate.
	 * \return Returns false on failure.
	 */
	LSBOOL LSE_CALL CBitStream::Alloc( uint32_t _ui32Total ) {
		// Has no function if the buffer is borrowed.
		if ( m_bBorrowed ) { return false; }

		if ( _ui32Total == 0 ) {
			Reset();
			return true;
		}

		_ui32Total *= sizeof( uint32_t );

		uint32_t * pui32Backup = reinterpret_cast<uint32_t *>(ReAlloc( m_pui32Buffer, _ui32Total ));
		if ( !pui32Backup ) { return false; }
		m_pui32Buffer = pui32Backup;

		
		if ( _ui32Total < m_ui32Total ) {
			m_ui32Total = _ui32Total;
		}
		m_ui32Alloc = _ui32Total;
		return true;
	}

	/**
	 * Reset the stream completely.  Frees all resources it was using.
	 */
	void LSE_CALL CBitStream::Reset() {
		if ( m_pui32Buffer && !m_bBorrowed ) {
			Free( m_pui32Buffer );
		}
		m_pui32Buffer = NULL;
		m_ui32Total = m_ui32Alloc = 0;
		m_ui64Pos = m_ui64TotalBits = 0ULL;
		m_bBorrowed = false;
	}

	/**
	 * Read bits from the stream.  Stream position is advanced by the amount read.
	 *
	 * \param _pui8Buffer Buffer to be filled with the bits read from the stream.
	 * \param _ui32BitsToRead Number of bits to read.
	 * \return Returns true if all bits were successfully read from the bit stream.
	 */
	LSBOOL LSE_CALL CBitStream::ReadBits( uint8_t * _pui8Buffer, uint32_t _ui32BitsToRead ) const {
		// Calculate where the stream pointer will be at the end of the read.
		uint64_t ui64FinalPos = m_ui64Pos + _ui32BitsToRead;
		if ( ui64FinalPos > m_ui64TotalBits ) { return false; }

		// There are enough bytes to fullfill the request.
		uint32_t ui32RoundUp = LSE_ROUND_UP( _ui32BitsToRead, 8 ) >> 3;
		if ( ui32RoundUp ) {
			// Set the last byte to 0.
			_pui8Buffer[ui32RoundUp-1] = 0;
		}

		// If we are aligned on a byte boundary, copy full bytes at a time.
		uint32_t I = 0;
		if ( (m_ui64Pos & 0x7ULL) == 0ULL ) {
			uint32_t ui32TotalBytes = _ui32BitsToRead >> 3;
			uint32_t ui32SrcByte = static_cast<uint32_t>(m_ui64Pos >> 3ULL);
			for ( ; I < ui32TotalBytes; ++I ) {
				_pui8Buffer[I] = reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32SrcByte++];
			}
			m_ui64Pos += ui32TotalBytes << 3ULL;
			I <<= 3;
		}

		// Finish trailing bits (which may be all of them).
		for ( ; I < _ui32BitsToRead; ++I, ++m_ui64Pos ) {
			uint32_t ui32SrcByte, ui32DstByte;
			uint32_t ui32SrcBit, ui32DstBit;
			GetByteAndBit( m_ui64Pos, ui32SrcByte, ui32SrcBit );
			GetByteAndBit( I, ui32DstByte, ui32DstBit );

			uint8_t ui8Value = static_cast<uint8_t>(reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32SrcByte] >> ui32SrcBit);
			
			_pui8Buffer[ui32DstByte] |= (ui8Value & 1) << ui32DstBit;
		}
		return true;
	}

	/**
	 * Read bytes from the stream.  Stream position is advanced by the amount read.
	 *
	 * \param _pui8Buffer Buffer to hold the bytes read from the stream.
	 * \param _ui32BytesToRead Number of bytes to read.
	 * \return Returns the actual number of bytes read or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CBitStream::ReadBytes( uint8_t * _pui8Buffer, uint32_t _ui32BytesToRead ) const {
		if ( ReadBits( _pui8Buffer, _ui32BytesToRead << 3 ) ) { return _ui32BytesToRead; }
		return static_cast<uint32_t>(LSSTD_SE_ERROR);
	}

	/**
	 * Reads a 64-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit unsigned integer at the current position in the stream.
	 */
	uint64_t LSE_CALL CBitStream::ReadUInt64() const {
		uint64_t ui64Value;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&ui64Value), sizeof( ui64Value ) << 3 ) != LSSTD_SE_ERROR ) { return ui64Value; }
		return 0ULL;
	}
	
	/**
	 * Reads a 32-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit unsigned integer at the current position in the stream.
	 */
	uint32_t LSE_CALL CBitStream::ReadUInt32() const {
		uint32_t ui32Value;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&ui32Value), sizeof( ui32Value ) << 3 ) ) { return ui32Value; }
		return 0;
	}

	/**
	 * Reads a 16-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 16-bit unsigned integer at the current position in the stream.
	 */
	uint16_t LSE_CALL CBitStream::ReadUInt16() const {
		uint16_t ui16Value;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&ui16Value), sizeof( ui16Value ) << 3 ) ) { return ui16Value; }
		return 0;
	}

	/**
	 * Reads an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns the 8-bit unsigned integer at the current position in the stream.
	 */
	uint8_t LSE_CALL CBitStream::ReadUInt8() const {
		uint8_t ui8Value;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&ui8Value), sizeof( ui8Value ) << 3 ) ) { return ui8Value; }
		return 0;
	}

	/**
	 * Reads a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns the 32-bit float at the current position in the stream.
	 */
	float LSE_CALL CBitStream::ReadFloat() const {
		float fValue;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&fValue), sizeof( fValue ) << 3 ) ) { return fValue; }
		return 0.0f;
	}

	/**
	 * Reads a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns the 64-bit double at the current position in the stream.
	 */
	double LSE_CALL CBitStream::ReadDouble() const {
		double dValue;
		if ( ReadBits( reinterpret_cast<uint8_t *>(&dValue), sizeof( dValue ) << 3 ) ) { return dValue; }
		return 0.0;
	}

	/**
	 * Writes bits to the current position in the stream.  Stream position is advanced by the amount written.
	 *
	 * \param _pui8Buffer Buffer containing the bits to write to the stream.
	 * \param _ui32BitsToWrite Number of bits to write from _pui8Buffer.
	 * \return Returns true if all bits were written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteBits( const uint8_t * _pui8Buffer, uint32_t _ui32BitsToWrite ) {
		// Calculate where the stream pointer will be at the end of the write.
		uint64_t ui64FinalPos = m_ui64Pos + _ui32BitsToWrite;
		// Round up to the nearest 8 bits, convert to bytes, and check.
		ui64FinalPos = ((ui64FinalPos + ((8 - ui64FinalPos) & 0x7)) >> 3);
		if ( ui64FinalPos > m_ui32Alloc ) {
			// Allocate in even chunks of 2048.
			uint32_t ui32RoundedAlloc = static_cast<uint32_t>(ui64FinalPos + ((2048ULL - ui64FinalPos) & (2048ULL - 1ULL)));
			if ( !Alloc( ui32RoundedAlloc >> 2 ) ) { return false; }
		}

		// We are free to write the bits now.
		// There are enough bytes to fullfill the request.
		for ( uint32_t I = 0; I < _ui32BitsToWrite; ++I, ++m_ui64Pos ) {
			uint32_t ui32SrcByte, ui32DstByte;
			uint32_t ui32SrcBit, ui32DstBit;
			GetByteAndBit( m_ui64Pos, ui32DstByte, ui32DstBit );
			GetByteAndBit( I, ui32SrcByte, ui32SrcBit );

			uint8_t ui8Value = static_cast<uint8_t>(_pui8Buffer[ui32SrcByte] >> ui32SrcBit);
			
			// Unset the bit.
			reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32DstByte] &= ~(1 << ui32DstBit);
			// Set the bit as per its status in the source.
			reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32DstByte] |= (ui8Value & 1) << ui32DstBit;
		}
		m_ui32Total = static_cast<uint32_t>(ui64FinalPos);
		m_ui64TotalBits = CStd::Max( m_ui64TotalBits, m_ui64Pos );
		return true;
	}

	/**
	 * Writes bytes to the stream.  Stream position is advanced by the amount written.
	 *
	 * \param _pui8Buffer Buffer containing the bytes to write to the stream.
	 * \param _ui32BytesToWrite Number of bytes to write.
	 * \return Returns the actual number of bytes written or LSSTD_SE_ERROR on error.
	 */
	uint32_t LSE_CALL CBitStream::WriteBytes( const uint8_t * _pui8Buffer, uint32_t _ui32BytesToWrite ) {
		if ( m_ui64Pos & 0x07ULL ) {
			if ( WriteBits( _pui8Buffer, _ui32BytesToWrite << 3 ) ) { return _ui32BytesToWrite; }
			return static_cast<uint32_t>(LSSTD_SE_ERROR);
		}
		else {
			// We are aligned on a byte boundary so speed up the copy by doing the whole byte at once.
			// Calculate where the stream pointer will be at the end of the write.
			uint64_t ui64FinalPos = m_ui64Pos + (_ui32BytesToWrite << 3);
			ui64FinalPos >>= 3ULL;
			if ( ui64FinalPos > m_ui32Alloc ) {
				// Allocate in even chunks of 32.
				uint32_t ui32RoundedAlloc = static_cast<uint32_t>(ui64FinalPos + ((32 - ui64FinalPos) & (32 - 1)));
				if ( !Alloc( ui32RoundedAlloc >> 2 ) ) { return static_cast<uint32_t>(LSSTD_SE_ERROR); }
			}

			uint32_t ui32DstByte = static_cast<uint32_t>(m_ui64Pos >> 3);
			uint32_t ui32BytesToWrite = _ui32BytesToWrite;
			while ( ui32BytesToWrite-- ) {
				reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32DstByte++] = (*_pui8Buffer++);
			}
			m_ui64Pos = (ui64FinalPos << 3ULL);
			m_ui32Total = static_cast<uint32_t>(ui64FinalPos);
			m_ui64TotalBits = CStd::Max( m_ui64TotalBits, m_ui64Pos );
			return _ui32BytesToWrite;
		}
	}

	/**
	 * Writes a 64-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteUInt64( const uint64_t &_ui64Value ) {
		return WriteBits( reinterpret_cast<const uint8_t * >(&_ui64Value), sizeof( _ui64Value ) << 3 );
	}
	
	/**
	 * Writes a 32-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteUInt32( uint32_t _ui32Value ) {
		return WriteBits( reinterpret_cast<const uint8_t * >(&_ui32Value), sizeof( _ui32Value ) << 3 );
	}

	/**
	 * Writes a 16-bit unsigned integer to the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteUInt16( uint16_t _ui16Value ) {
		return WriteBits( reinterpret_cast<const uint8_t * >(&_ui16Value), sizeof( _ui16Value ) << 3 );
	}

	/**
	 * Writes an 8-bit unsigned integer from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteUInt8( uint8_t _ui8Value ) {
		if ( m_ui64Pos & 0x07ULL ) {
			return WriteBits( reinterpret_cast<const uint8_t * >(&_ui8Value), sizeof( _ui8Value ) << 3 );
		}
		// We are aligned on a byte boundary so speed up the copy by doing the whole byte at once.
		// Calculate where the stream pointer will be at the end of the write.
		uint64_t ui64FinalPos = m_ui64Pos + (8);
		ui64FinalPos >>= 3ULL;
		if ( ui64FinalPos > m_ui32Alloc ) {
			// Allocate in even chunks of 32.
			uint32_t ui32RoundedAlloc = static_cast<uint32_t>(ui64FinalPos + ((32 - ui64FinalPos) & (32 - 1)));
			if ( !Alloc( ui32RoundedAlloc >> 2 ) ) { return false; }
		}

		uint32_t ui32DstByte = static_cast<uint32_t>(m_ui64Pos >> 3);
		reinterpret_cast<uint8_t *>(m_pui32Buffer)[ui32DstByte] = _ui8Value;
		m_ui64Pos = (ui64FinalPos << 3ULL);
		m_ui32Total = static_cast<uint32_t>(ui64FinalPos);
		m_ui64TotalBits = CStd::Max( m_ui64TotalBits, m_ui64Pos );
		return true;
	}

	/**
	 * Writes a 32-bit float from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteFloat( float _fValue ) {
		return WriteBits( reinterpret_cast<const uint8_t * >(&_fValue), sizeof( _fValue ) << 3 );
	}

	/**
	 * Writes a 64-bit double from the stream and advances the stream position.
	 *
	 * \return Returns true if the value was fully written to the stream.
	 */
	LSBOOL LSE_CALL CBitStream::WriteDouble( const double &_dValue ) {
		return WriteBits( reinterpret_cast<const uint8_t * >(&_dValue), sizeof( _dValue ) << 3 );
	}

	/**
	 * Gets the length of the stream in bytes.
	 *
	 * \return Returns the length of the stream in bytes.  All bits up to the last byte are used.  The last
	 *	byte may have some unused bits.
	 */
	uint32_t LSE_CALL CBitStream::GetLengthInBytes() const {
		return m_ui32Total;
	}

	/**
	 * Gets a pointer to the beginning of the actual bytes in the stream.
	 *
	 * \return Returns the actual bytes in the stream.
	 */
	const uint8_t * LSE_CALL CBitStream::GetRawBytes() const {
		return m_pui32Buffer ? reinterpret_cast<const uint8_t *>(m_pui32Buffer) : reinterpret_cast<const uint8_t *>("");
	}

	/**
	 * Sets the stream position in bits.  If the position is set past the end of the stream length, it is clamped
	 *	to the end of the string.
	 *
	 * \param _ui64Pos The new position of the stream in bits.
	 * \return Returns the old position of the stream in bits.
	 */
	uint64_t LSE_CALL CBitStream::SetPos( uint64_t _ui64Pos ) const {
		uint64_t ui64Pos = m_ui64Pos;
		m_ui64Pos = CStd::Min( _ui64Pos, m_ui64TotalBits );
		return ui64Pos;
	}

	/**
	 * Gets the stream position in bits.
	 *
	 * \return Returns the stream position in bits.
	 */
	uint64_t LSE_CALL CBitStream::GetPos() const {
		return m_ui64Pos;
	}

	/**
	 * Pre-allocate a given amount in bits.  If the amount is less than or equal to the currently
	 *	allocated amount, true is returned with no action taking place.
	 *
	 * \param _ui64NewSize The new size to allocate.
	 * \return Returns true if allocation succeeds.  False is returned if allocation takes place but
	 *	fails.
	 */
	LSBOOL LSE_CALL CBitStream::PreAlloc( uint64_t _ui64NewSize ) {
		// Round up to the nearest 32 bits.
		_ui64NewSize = LSE_ROUND_UP( _ui64NewSize, 32 );
		if ( _ui64NewSize <= m_ui32Alloc ) { return true; }
		return Alloc( static_cast<uint32_t>(_ui64NewSize >> 2) );
	}

	/**
	 * Point the stream to an existing buffer.  The buffer cannot be resized from within this instance.
	 *	This can be a very dangerous function and must be used with caution.  This causes its existing
	 *	buffer to be freed.
	 *
	 * \param _pui8Buffer The buffer to be used by this stream directly.
	 * \param _uiptrSize The size of the buffer to which _pui8Buffer points.
	 */
	void LSE_CALL CBitStream::Borrow( const uint8_t * _pui8Buffer, uintptr_t _uiptrSize ) {
		Reset();
		m_pui32Buffer = reinterpret_cast<uint32_t *>(const_cast<uint8_t *>(_pui8Buffer));
		m_ui32Total = m_ui32Alloc = static_cast<uint32_t>(_uiptrSize);
		m_ui64TotalBits = m_ui32Total * 8ULL;
		m_bBorrowed = true;
	}

	/**
	 * Re-allocate bytes.  Override this to add functionality such as use of memory managers etc.
	 *
	 * \param _pvOriginal Address to re-alloate.  Pass NULL to allocate new data.
	 * \param _ui32Total Total number of bytes to allocate.
	 * \return Returns the re-allocated bytes or NULL upon failure.
	 */
	void * LSE_CALL CBitStream::ReAlloc( void * _pvOriginal, uint32_t _ui32Total ) const {
		assert( !m_bBorrowed );
		return ::realloc( _pvOriginal, _ui32Total );
	}

	/**
	 * Free bytes.  Override this to add functionality such as use of memory managers etc.
	 *
	 * \param _pvOriginal Pointer to the data, allocated with ReAlloc() to free.
	 */
	void LSE_CALL CBitStream::Free( void * _pvOriginal ) const {
		assert( !m_bBorrowed );
		::free( _pvOriginal );
	}

}	// namespace lsstd
