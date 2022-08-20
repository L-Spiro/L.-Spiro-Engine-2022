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
 * Description: Various types of specialized compression routines.  Data often gets compressed better when
 *	routines are specialized towards that type of data.  For example, if a normal is assumed to be unit-length,
 *	only 65 bits are needed to hold it, instead of 96.  An array of index data usually starts with low numbers
 *	and gradually builds into higher numbers.  Only a few bits of data are needed to hold the first several numbers.
 */


#ifndef __LSC_MISC_H__
#define __LSC_MISC_H__

#include "../LSCCompressionLib.h"
#include "../BitStream/LSCBitStream.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Vector/LSMF16Vector2.h"
#include "Vector/LSMVector2.h"
#include "Vector/LSMVector3.h"
#include "Vector/LSTLVectorPoD.h"

namespace lstl {
	class												CString;
}

namespace lsc {

	/**
	 * Class CMisc
	 * \brief Various types of specialized compression routines.
	 *
	 * Description: Various types of specialized compression routines.  Data often gets compressed better when
	 *	routines are specialized towards that type of data.  For example, if a normal is assumed to be unit-length,
	 *	only 65 bits are needed to hold it, instead of 96.  An array of index data usually starts with low numbers
	 *	and gradually builds into higher numbers.  Only a few bits of data are needed to hold the first several numbers.
	 */
	class CMisc {
	public :
		// == Enumerations.
		/**
		 * Compression levels.
		 */
		enum LSC_COMPRESSION_LEVELS {
			LSC_CL_BEST,								/**< Best compression. */
			LSC_CL_MEDIUM,								/**< Medium compression. */
			LSC_CL_NONE,								/**< No compression. */
		};


		// == Types.
		/**
		 * A buffer used to hold compressed data.  It is simply a vector of bytes.
		 */
		typedef CVectorPoD<uint8_t, uint32_t, 1048576UL>LSC_BUFFER;


		// == Functions
		/**
		 * Sets the compression mode.
		 *
		 * \param _clMode New compression mode. 
		 */
		static LSE_INLINE void LSE_CALL				SetCompressionMode( LSC_COMPRESSION_LEVELS _clMode );

		/**
		 * Compress an array of normals.  The normals are assumed to be of unit length.  Works by removing the entire Z
		 *	component except for its sign, which is returned separately.  Normals are reconstructed by determining the Z value
		 *	that makes the normal unit-length again.
		 *
		 * \param _pvIn The input 3D vector.
		 * \param _ui32Total The number of vectors to which _pvIn points.
		 * \param _vOut The output 2D vector containing the X and Y components of the input vector.
		 * \param _bsSigns The output sign array.  Each bit represents one Z sign.
		 * \return Returns true if there are no memory problems during the compression.
		 */
		static LSBOOL LSE_CALL							CompressNormals( const CVector3 * _pvIn, uint32_t _ui32Total,
			CVectorPoD<CF16Vector2, uint32_t> &_vOut,
			lsc::CBitStream &_bsSigns );

		/**
		 * Decompress normals to a buffer that is of the correct length to hold all of the decompressed normals.
		 *
		 * \param _pvNormal2d An array of 2D normals containing the X and Y components of the expanded 3D normal.
		 * \param _bsSigns The array of Z signs.  1 bit per sign.
		 * \param _ui32Total The number of normals to which _pvNormal2d points.
		 * \param _pvOut The output buffer.  Must be allocated to hold _ui32Total 3D normals.
		 */
		static void LSE_CALL							DecompressNormals( const CF16Vector2 * _pvNormal2d,
			const lsc::CBitStream &_bsSigns, uint32_t _ui32Total,
			CVector3 * _pvOut );

		/**
		 * Compress data at a given bit depth.
		 *
		 * \param _pui8Input The input data to compress.
		 * \param _ui32Length Length of the input data to compress.
		 * \param _ui32Bits The maximum bit depth.
		 * \param _bsStream The compressed data.
		 * \param _ui32MaxSize The maximum size of the compressed data.  If the compression size exceeds this, compression is stopped and false is returned.
		 * \return Returns true if there was enough memory to perform the encoding.
		 */
		static LSBOOL LSE_CALL							Compress( const uint8_t * _pui8Input, uint32_t _ui32Length, uint32_t _ui32Bits, lsc::CBitStream &_bsStream, uint32_t _ui32MaxSize = LSE_MAXU32 );

		/**
		 * Find the best compression size and return the compressed bytes and the bit-count.
		 *
		 * \param _pui8Input The input data to compress.
		 * \param _ui32Length Length of the input data to compress.
		 * \param _ui32Bits The returned bit depth.
		 * \param _bsStream The compressed data.
		 * \return Returns true if a suitable compression was found, false otherwise.
		 */
		static LSBOOL LSE_CALL							CompressBest( const uint8_t * _pui8Input, uint32_t _ui32Length, uint32_t &_ui32Bits, lsc::CBitStream &_bsStream );

		/**
		 * Compress bytes to a buffer.  If the compression is larger than the original data, the original data is written instead.
		 *
		 * \param _pui8Input The input data to compress.
		 * \param _ui32Length Length of the input data to compress.
		 * \param _bBuffer The buffer to which to write the bytes.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							CompressToBuffer( const uint8_t * _pui8Input, uint32_t _ui32Length, LSC_BUFFER &_bBuffer );

		/**
		 * Decompress from packed data of any kind to a buffer.  If the expanded size is known beforehand, call Allocate() on the buffer to improve
		 *	the performance of this call.
		 *
		 * \param _pvData Pointer to the data to expand.
		 * \param _uiptrSize Size of the data to expand in bytes.
		 * \param _bBuffer Holds the expanded data.
		 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
		 */
		static LSBOOL LSE_CALL							DecompressToBuffer( const void * _pvData, uintptr_t _uiptrSize, LSC_BUFFER &_bBuffer );

		/**
		 * Decompress from packed data of any kind to a buffer.  If the expanded size is known beforehand, call Allocate() on the buffer to improve
		 *	the performance of this call.
		 *
		 * \param _isStream Stream containing the bytes to expand.
		 * \param _bBuffer Holds the expanded data.
		 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
		 */
		static LSBOOL LSE_CALL							DecompressToBuffer( const lsstd::CStream &_isStream, LSC_BUFFER &_bBuffer );

		/**
		 * Compress index data to a given bitsream.
		 *
		 * \param _pui32Input The input data to compress.
		 * \param _ui32Length Number of indices to to which _pui32Input points.
		 * \param _bsStream The compressed data.
		 * \return Returns true if there was enough memory to perform the encoding.
		 */
		static LSBOOL LSE_CALL							CompressIndices( const uint32_t * _pui32Input, uint32_t _ui32Length, lsc::CBitStream &_bsStream );

		/**
		 * Decompress indices that are not further compressed to a buffer.  Input data must have been created with a call to CompressIndices()
		 *
		 * \param _bsStream The data to decompress.
		 * \param _bBuffer Holds the expanded data.
		 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
		 */
		static LSBOOL LSE_CALL							DecompressIndices( const lsc::CBitStream &_bsStream, LSC_BUFFER &_bBuffer );

		/**
		 * Compress index data to a buffer.  If the compression is larger than the original data, the original data is written instead.
		 *
		 * \param _pui32Input The input data to compress.
		 * \param _ui32Length Number of indices to to which _pui32Input points.
		 * \param _bBuffer The buffer to which to write the bytes.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							CompressIndicesToBuffer( const uint32_t * _pui32Input, uint32_t _ui32Length, LSC_BUFFER &_bBuffer );

		/**
		 * Decompress indices from packed data to a buffer.
		 *
		 * \param _pvData Pointer to the data to expand.
		 * \param _uiptrSize Size of the data to expand in bytes.
		 * \param _bBuffer Holds the expanded data in which every index is 32 bits.
		 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
		 */
		static LSBOOL LSE_CALL							DecompressIndicesToBuffer( const void * _pvData, uintptr_t _uiptrSize, LSC_BUFFER &_bBuffer );

		/**
		 * Decompress indices from packed data to a buffer.
		 *
		 * \param _isStream The data to decompress.
		 * \param _bBuffer Holds the expanded data in which every index is 32 bits.
		 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
		 */
		static LSBOOL LSE_CALL							DecompressIndicesToBuffer( const lsstd::CStream &_isStream, LSC_BUFFER &_bBuffer );

		/**
		 * Read a string from a stream.
		 *
		 * \param _sStream The stream from which to read a string.
		 * \param _sRet The returned string.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		static LSBOOL LSE_CALL							ReadString( const lsstd::CStream &_sStream, CString &_sRet );

		/**
		 * Reads only the 3-by-4 part of a matrix to a buffer.
		 *
		 * \param _sStream The stream from which to read the matrix the value.
		 * \param _mMat The matrix to hold the returned value.
		 * \param Returns true only if all bytes are read from the given stream.
		 */
		static LSBOOL LSE_CALL							ReadMatrixSimple( const lsstd::CStream &_sStream, CMatrix4x4 &_mMat );

		/**
		 * Write bytes to a buffer.
		 *
		 * \param _pui8Bytes The values to write to the buffer.
		 * \param _ui32Length Number of bytes to write to the buffer.
		 * \param _bBuffer The buffer to which to write the bytes.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteBytes( const uint8_t * _pui8Bytes, uint32_t _ui32Length, LSC_BUFFER &_bBuffer );

		/**
		 * Write a byte to a buffer.
		 *
		 * \param _ui8Val The value to write to the buffer.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUChar( uint8_t _ui8Val, LSC_BUFFER &_bBuffer );

		/**
		 * Write an unsigned integer to a buffer.
		 *
		 * \param _ui32Val The value to write to the file.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUInt( uint32_t _ui32Val, LSC_BUFFER &_bBuffer );


	protected :
		// == Members.
		/**
		 * Compression level.
		 */
		static uint32_t									m_ui32CompressLevel;


		// == Functions.
		/**
		 * Gets the number of bits needed to store a number assumed to be signed.
		 *
		 * \param _i32Value The value to count how many bits are needed to store assuming the top bit is reserved for signs.
		 * \return Returns the number of bits needed to store a number assumed to be signed.
		 */
		static uint32_t LSE_CALL						GetBitsNeededForSignedValue( int32_t _i32Value );

		/**
		 * Gets a signed number from a stream.
		 *
		 * \param _bsStream Stream from which to get the signed value.
		 * \param _ui32Bits Number of bits used to store the value.
		 * \return Returns the signed value.
		 */
		static int32_t LSE_CALL							GetSignedValue( const lsc::CBitStream &_bsStream, uint32_t _ui32Bits );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions
	/**
	 * Sets the compression mode.
	 *
	 * \param _clMode New compression mode. 
	 */
	LSE_INLINE void LSE_CALL CMisc::SetCompressionMode( LSC_COMPRESSION_LEVELS _clMode ) {
		m_ui32CompressLevel = _clMode;
	}

}	// namespace lsc

#endif	// __LSC_MISC_H__
