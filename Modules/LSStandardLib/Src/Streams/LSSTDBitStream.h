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


#ifndef __LSSTD_BITSTREAM_H__
#define __LSSTD_BITSTREAM_H__

#include "../LSSTDStandardLib.h"
#include "LSSTDStream.h"

namespace lsstd {

	/**
	 * Class CBitStream
	 * \brief A standard bit stream
	 *
	 * Description: Provides a generic bit stream.
	 */
	class CBitStream : public CStream {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CBitStream();
		LSE_CALLCTOR							~CBitStream();

		
		// == Functions.
		/**
		 * Reset the stream completely.  Frees all resources it was using.
		 */
		void LSE_CALL							Reset();

		/**
		 * Read bits from the stream.  Stream position is advanced by the amount read.
		 *
		 * \param _pui8Buffer Buffer to be filled with the bits read from the stream.
		 * \param _ui32BitsToRead Number of bits to read.
		 * \return Returns true if all bits were successfully read from the bit stream.
		 */
		LSBOOL LSE_CALL							ReadBits( uint8_t * _pui8Buffer, uint32_t _ui32BitsToRead ) const;

		/**
		 * Read bytes from the stream.  Stream position is advanced by the amount read.
		 *
		 * \param _pui8Buffer Buffer to hold the bytes read from the stream.
		 * \param _ui32BytesToRead Number of bytes to read.
		 * \return Returns the actual number of bytes read or LSSTD_SE_ERROR on error.
		 */
		virtual uint32_t LSE_CALL				ReadBytes( uint8_t * _pui8Buffer, uint32_t _ui32BytesToRead ) const;

		/**
		 * Reads a 64-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 64-bit unsigned integer at the current position in the stream.
		 */
		virtual uint64_t LSE_CALL				ReadUInt64() const;
		
		/**
		 * Reads a 32-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 32-bit unsigned integer at the current position in the stream.
		 */
		virtual uint32_t LSE_CALL				ReadUInt32() const;

		/**
		 * Reads a 16-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 16-bit unsigned integer at the current position in the stream.
		 */
		virtual uint16_t LSE_CALL				ReadUInt16() const;

		/**
		 * Reads an 8-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 8-bit unsigned integer at the current position in the stream.
		 */
		virtual uint8_t LSE_CALL				ReadUInt8() const;

		/**
		 * Reads a 32-bit float from the stream and advances the stream position.
		 *
		 * \return Returns the 32-bit float at the current position in the stream.
		 */
		virtual float LSE_CALL				ReadFloat() const;

		/**
		 * Reads a 64-bit double from the stream and advances the stream position.
		 *
		 * \return Returns the 64-bit double at the current position in the stream.
		 */
		virtual double LSE_CALL				ReadDouble() const;

		/**
		 * Write bits to the current position in the stream.  Stream position is advanced by the amount written.
		 *
		 * \param _pui8Buffer Buffer containing the bits to write to the stream.
		 * \param _ui32BitsToWrite Number of bits to write from _pui8Buffer.
		 * \return Returns true if all bits were written to the stream.
		 */
		LSBOOL LSE_CALL							WriteBits( const uint8_t * _pui8Buffer, uint32_t _ui32BitsToWrite );

		/**
		 * Write bytes to the stream.  Stream position is advanced by the amount written.
		 *
		 * \param _pui8Buffer Buffer containing the bytes to write to the stream.
		 * \param _ui32BytesToWrite Number of bytes to write.
		 * \return Returns the actual number of bytes written or LSSTD_SE_ERROR on error.
		 */
		virtual uint32_t LSE_CALL				WriteBytes( const uint8_t * _pui8Buffer, uint32_t _ui32BytesToWrite );

		/**
		 * Writes a 64-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt64( const uint64_t &_ui64Value );
		
		/**
		 * Writes a 32-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt32( uint32_t _ui32Value );

		/**
		 * Writes a 16-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt16( uint16_t _ui16Value );

		/**
		 * Writes an 8-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt8( uint8_t _ui8Value );

		/**
		 * Writes a 32-bit float from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteFloat( float _fValue );

		/**
		 * Writes a 64-bit double from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteDouble( const double &_dValue );

		/**
		 * Gets the length of the stream in bytes.
		 *
		 * \return Returns the length of the stream in bytes.  All bits up to the last byte are used.  The last
		 *	byte may have some unused bits.
		 */
		uint32_t LSE_CALL						GetLengthInBytes() const;

		/**
		 * Gets a pointer to the beginning of the actual bytes in the stream.
		 *
		 * \return Returns the actual bytes in the stream.
		 */
		const uint8_t * LSE_CALL				GetRawBytes() const;

		/**
		 * Sets the stream position in bits.  If the position is set past the end of the stream length, it is clamped
		 *	to the end of the string.
		 *
		 * \param _ui64Pos The new position of the stream in bits.
		 * \return Returns the old position of the stream in bits.
		 */
		uint64_t LSE_CALL						SetPos( uint64_t _ui64Pos ) const;

		/**
		 * Gets the stream position in bits.
		 *
		 * \return Returns the stream position in bits.
		 */
		uint64_t LSE_CALL						GetPos() const;

		/**
		 * Pre-allocate a given amount in bits.  If the amount is less than or equal to the currently
		 *	allocated amount, true is returned with no action taking place.
		 *
		 * \param _ui64NewSize The new size to allocate.
		 * \return Returns true if allocation succeeds.  False is returned if allocation takes place but
		 *	fails.
		 */
		LSBOOL LSE_CALL							PreAlloc( uint64_t _ui64NewSize );

		/**
		 * Point the stream to an existing buffer.  The buffer cannot be resized from within this instance.
		 *	This can be a very dangerous function and must be used with caution.  This causes its existing
		 *	buffer to be freed.
		 *
		 * \param _pui8Buffer The buffer to be used by this stream directly.
		 * \param _uiptrSize The size of the buffer to which _pui8Buffer points.
		 */
		void LSE_CALL							Borrow( const uint8_t * _pui8Buffer, uintptr_t _uiptrSize );

	protected :
		// == Members.
		/**
		 * Length of the stream in bits.
		 */
		uint64_t								m_ui64TotalBits;

		/**
		 * We buffer 32 bits worth at a time.
		 */
		uint32_t *								m_pui32Buffer;

		/**
		 * Total length of the allocated list in bytes.
		 */
		uint32_t								m_ui32Total;

		/**
		 * Allocated length of the list in bytes.
		 */
		uint32_t								m_ui32Alloc;

		/**
		 * Is the buffer borrowed?
		 */
		LSBOOL									m_bBorrowed;


		/**
		 * Stream position in bits.  Why mutable?  Because reading from a stream requires const functions!
		 *	This makes sense in terms of logical const-correctness.  If this stream were to be compared against
		 *	another stream, m_ui64Pos would not be a factor in determining if they were equal, thus modifying
		 *	m_ui64Pos has no influence on the logical const-ness of this object.  Mutable it is.
		 */
		mutable uint64_t						m_ui64Pos;


		// == Functions.
		/**
		 * Allocates the given number of uint32_t's and store the result in m_pui32Buffer.  Note that this
		 *	function should not be overloaded when adding memory-manager functionality to this class.
		 *
		 * \param _ui32Total Total number of uint32_t's to allocate.
		 * \return Returns false on failure.
		 */
		LSBOOL LSE_CALL							Alloc( uint32_t _ui32Total );

		/**
		 * Re-allocate bytes.  Override this to add functionality such as use of memory managers etc.
		 *
		 * \param _pvOriginal Address to re-alloate.  Pass NULL to allocate new data.
		 * \param _ui32Total Total number of bytes to allocate.
		 * \return Returns the re-allocated bytes or NULL upon failure.
		 */
		virtual void * LSE_CALL				ReAlloc( void * _pvOriginal, uint32_t _ui32Total ) const;

		/**
		 * Free bytes.  Override this to add functionality such as use of memory managers etc.
		 *
		 * \param _pvOriginal Pointer to the data, allocated with ReAlloc() to free.
		 */
		virtual void LSE_CALL					Free( void * _pvOriginal ) const;

		/**
		 * Gets the byte and number of bits into that byte given a bit position.
		 *
		 * \param _ui64BitPos The bit position for which the byte index and bit index are to be obtained.
		 * \param _ui32ByteIndex Holds the returned byte index.
		 * \param _ui32BitIndex Holds the returned bit index.
		 */
		static void LSE_CALL					GetByteAndBit( const uint64_t &_ui64BitPos,
			uint32_t &_ui32ByteIndex, uint32_t &_ui32BitIndex );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the byte and number of bits into that byte given a bit position.
	 *
	 * \param _ui64BitPos The bit position for which the byte index and bit index are to be obtained.
	 * \param _ui32ByteIndex Holds the returned byte index.
	 * \param _ui32BitIndex Holds the returned bit index.
	 */
	LSE_INLINE void LSE_CALL CBitStream::GetByteAndBit( const uint64_t &_ui64BitPos,
		uint32_t &_ui32ByteIndex, uint32_t &_ui32BitIndex ) {
		_ui32ByteIndex = static_cast<uint32_t>(_ui64BitPos >> 3ULL);
		_ui32BitIndex = static_cast<uint32_t>(_ui64BitPos & 7ULL);
	}

}	// namespace lsstd

#endif	// __LSSTD_BITSTREAM_H__
