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
 * Description: This interface serves as the base class for all streams.  File streams, bit streams,
 *	network streams, etc., are built on top of this.
 */


#ifndef __LSSTD_STREAM_H__
#define __LSSTD_STREAM_H__

#include "../LSSTDStandardLib.h"

namespace lsstd {

	/**
	 * Class CStream
	 * \brief The base interface for all streams.
	 *
	 * Description: This interface serves as the base class for all streams.  File streams, bit streams,
	 *	network streams, etc., are built on top of this.
	 */
	class CStream {
		// All is public.  This class has no secrets.
	public :
		// == Enumerations.
		/** Error codes. */
		enum LSSTD_STREAM_ERRORS {
			LSSTD_SE_ERROR						= LSE_MAXU32,							/**< Error. */
			LSSTD_SE_SUCCESS					= 0,									/**< No error. */
			LSSTD_SE_MEMORYERROR				= 1,									/**< Memory failure. */
			LSSTD_SE_ENDOFSTREAM				= 2,									/**< End of stream. */

			LSSTD_SE_FORCEDWORD					= 0x7FFFFFFFUL,							/**< Do not use. */
		};


		// == Functions.
		/**
		 * Read bytes from the stream.  Stream position is advanced by the amount read.
		 *
		 * \param _pui8Buffer Buffer to hold the bytes read from the stream.
		 * \param _ui32BytesToRead Number of bytes to read.
		 * \return Returns the actual number of bytes read or LSSTD_SE_ERROR on error.
		 */
		virtual uint32_t LSE_CALL				ReadBytes( uint8_t * _pui8Buffer, uint32_t _ui32BytesToRead ) const = 0;

		/**
		 * Reads a 64-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 64-bit unsigned integer at the current position in the stream.
		 */
		virtual uint64_t LSE_CALL				ReadUInt64() const = 0;
		
		/**
		 * Reads a 32-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 32-bit unsigned integer at the current position in the stream.
		 */
		virtual uint32_t LSE_CALL				ReadUInt32() const = 0;

		/**
		 * Reads a 16-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 16-bit unsigned integer at the current position in the stream.
		 */
		virtual uint16_t LSE_CALL				ReadUInt16() const = 0;

		/**
		 * Reads an 8-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns the 8-bit unsigned integer at the current position in the stream.
		 */
		virtual uint8_t LSE_CALL				ReadUInt8() const = 0;

		/**
		 * Reads a 32-bit float from the stream and advances the stream position.
		 *
		 * \return Returns the 32-bit float at the current position in the stream.
		 */
		virtual float LSE_CALL				ReadFloat() const = 0;

		/**
		 * Reads a 64-bit double from the stream and advances the stream position.
		 *
		 * \return Returns the 64-bit double at the current position in the stream.
		 */
		virtual double LSE_CALL				ReadDouble() const = 0;

		/**
		 * Write bytes to the stream.  Stream position is advanced by the amount written.
		 *
		 * \param _pui8Buffer Buffer containing the bytes to write to the stream.
		 * \param _ui32BytesToWrite Number of bytes to write.
		 * \return Returns the actual number of bytes written or LSSTD_SE_ERROR on error.
		 */
		virtual uint32_t LSE_CALL				WriteBytes( const uint8_t * _pui8Buffer, uint32_t _ui32BytesToWrite ) = 0;

		/**
		 * Writes a 64-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt64( const uint64_t &_ui64Value ) = 0;
		
		/**
		 * Writes a 32-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt32( uint32_t _ui32Value ) = 0;

		/**
		 * Writes a 16-bit unsigned integer to the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt16( uint16_t _ui16Value ) = 0;

		/**
		 * Writes an 8-bit unsigned integer from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteUInt8( uint8_t _ui8Value ) = 0;

		/**
		 * Writes a 32-bit float from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteFloat( float _fValue ) = 0;

		/**
		 * Writes a 64-bit double from the stream and advances the stream position.
		 *
		 * \return Returns true if the value was fully written to the stream.
		 */
		virtual LSBOOL LSE_CALL					WriteDouble( const double &_dValue ) = 0;
	};

}	// namespace lsstd

#endif	// __LSSTD_STREAM_H__
