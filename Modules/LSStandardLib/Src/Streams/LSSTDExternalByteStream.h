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


#ifndef __LSSTD_EXTERNALBYTESTREAM_H__
#define __LSSTD_EXTERNALBYTESTREAM_H__

#include "../LSSTDStandardLib.h"
#include "LSSTDStream.h"

namespace lsstd {

	/**
	 * Class CExternalByteStream
	 * \brief Allows treating an external array of bytes as a stream.
	 *
	 * Description: Allows treating an external array of bytes as a stream.  Allows modifying the array but not
	 *	beyond the actual length of the array.  This is useful for situations when you have a file image loaded to
	 *	memory and you wish to read from it in the fashion of a byte stream.
	 */
	class CExternalByteStream : public lsstd::CStream {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CExternalByteStream();
		LSE_CALLCTOR							CExternalByteStream( uint8_t * _pui8Buffer, uintptr_t _uiptrLen );
		LSE_CALLCTOR							CExternalByteStream( const uint8_t * _pui8Buffer, uintptr_t _uiptrLen );


		// == Functions.
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
		 * Pad-reads up to a multiple of the given value.  The values are discarded.
		 *
		 * \param _uiptrMultiple The multiple up to which to discard-read values.
		 */
		void LSE_CALL							PadRead( uintptr_t _uiptrMultiple ) const;

		/**
		 * Skips the given number of bytes.
		 *
		 * \param _uiptrSkip The number of bytes to skip.
		 */
		void LSE_CALL							SkipBytes( uintptr_t _uiptrSkip ) const;

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
		 * Sets the stream position.  Actual position set is clamped between 0 and the length of the stream.
		 *
		 * \param _uiptrPos New position of the stream.
		 */
		void LSE_CALL							SetPos( uintptr_t _uiptrPos ) const;

		/**
		 * Gets the current position of the stream pointer.
		 *
		 * \return Returns the position of the stream pointer.  The stream pointer indicates where the next
		 *	stream operation (reading or writing) will take place.
		 */
		uintptr_t LSE_CALL						GetPos() const;

		/**
		 * Gets the length of the stream.
		 *
		 * \return Returns the length, in bytes, of the stream.
		 */
		uintptr_t LSE_CALL						Length() const;

		/**
		 * Gets a pointer to the start of the buffer.
		 *
		 * \return Returns a pointer to the start of the buffer.
		 */
		const uint8_t * LSE_CALL				GetBuffer() const;

		/**
		 * Gets a pointer to the current location in the buffer.
		 *
		 * \return Returns a pointer to the current location in the buffer.
		 */
		const uint8_t * LSE_CALL				GetCurBufferPointer() const;


	protected :
		// == Members.
		/**
		 * Pointer to the stream of bytes.
		 */
		uint8_t *								m_pui8Stream;

		/**
		 * Current position in the stream.
		 */
		mutable uintptr_t						m_uiptrCurPos;

		/**
		 * Length of the stream.
		 */
		uintptr_t								m_uiptrLength;

		/**
		 * Are we read-only?
		 */
		LSBOOL									m_bReadOnly;

	};

}	// namespace lsstd

#endif	// __LSSTD_EXTERNALBYTESTREAM_H__
