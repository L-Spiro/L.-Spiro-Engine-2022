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


#ifndef __LSF_FILESTREAM_H__
#define __LSF_FILESTREAM_H__

#include "../LSFFileLib.h"
#include "Streams/LSSTDStream.h"

namespace lsf {

	/**
	 * Class CFileStream
	 * \brief File streaming using standard CFileLib calls.
	 *
	 * Description: File streaming using standard CFileLib calls.
	 */
	class CFileStream : public lsstd::CStream {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CFileStream( const char * _pcFile, LSF_OPEN_FILE _ofOpenType, LSBOOL _bOpenAlways );
		LSE_CALLCTOR							~CFileStream();


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


	protected :
		// == Members.
		/**
		 * The currently opened file.
		 */
		LSF_FILE								m_fFile;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsf

#endif	// __LSF_FILESTREAM_H__
