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


#include "LSCMisc.h"
#include "../LZW/LSCLzwDecoder.h"
#include "../LZW/LSCLzwEncoder.h"
#include "Helpers/LSMHelpers.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Streams/LSSTDExternalByteStream.h"
#include "String/LSTLString.h"


namespace lsc {

	// == Members.
	/**
	 * Compression level.
	 */
	uint32_t CMisc::m_ui32CompressLevel = CMisc::LSC_CL_BEST;

	// == Functions.
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
	LSBOOL LSE_CALL CMisc::CompressNormals( const CVector3 * _pvIn, uint32_t _ui32Total,
		CVectorPoD<CF16Vector2, uint32_t> &_vOut,
		lsc::CBitStream &_bsSigns ) {
		if ( !_vOut.Allocate( _ui32Total ) ) { return false; }
		if ( !_bsSigns.PreAlloc( _ui32Total ) ) { return false; }
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			// We do normalization manually to guarantee accuracy.  Remember, the engine libraries can
			//	be compiled to a base type other than float or double, and has optional optimization switches to
			//	reduce the accuracy of square roots.
			double dVal[3] = {
				static_cast<double>(_pvIn[I].x),
				static_cast<double>(_pvIn[I].y),
				static_cast<double>(_pvIn[I].z)
			};
			double dLen = dVal[0] * dVal[0] + dVal[1] * dVal[1] + dVal[2] * dVal[2];
			if ( dLen ) {
				dLen = 1.0 / ::sqrt( dLen );
				dVal[0] *= dLen;
				dVal[1] *= dLen;
				// Don't need to normalize the last value since it is not stored.
			}
			_vOut.Push( CF16Vector2( static_cast<LSREAL>(dVal[0]), static_cast<LSREAL>(dVal[1]) ) );

			static const uint8_t ui8One = 1;
			static const uint8_t ui8Zero = 0;
			if ( dVal[2] >= 0.0 ) { _bsSigns.WriteBits( &ui8Zero, 1 ); }
			else { _bsSigns.WriteBits( &ui8One, 1 ); }
		}
		return true;
	}

	/**
	 * Decompress normals to a buffer that is of the correct length to hold all of the decompressed normals.
	 *
	 * \param _pvNormal2d An array of 2D normals containing the X and Y components of the expanded 3D normal.
	 * \param _bsSigns The array of Z signs.  1 bit per sign.
	 * \param _ui32Total The number of normals to which _pvNormal2d points.
	 * \param _pvOut The output buffer.  Must be allocated to hold _ui32Total 3D normals.
	 */
	void LSE_CALL CMisc::DecompressNormals( const CF16Vector2 * _pvNormal2d,
		const lsc::CBitStream &_bsSigns, uint32_t _ui32Total,
		CVector3 * _pvOut ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			LSREAL fZ = CHelpers::GetNormalizedZ( static_cast<float>(_pvNormal2d[I].x),
				static_cast<float>(_pvNormal2d[I].y) );
			_pvOut[I] = CVector3( static_cast<float>(_pvNormal2d[I].x), static_cast<float>(_pvNormal2d[I].y), static_cast<float>(fZ) );
			uint8_t ui8Sign = 0;
			_bsSigns.ReadBits( &ui8Sign, 1 );
			if ( ui8Sign ) { _pvOut[I].z = -_pvOut[I].z; }
		}
	}

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
	LSBOOL LSE_CALL CMisc::Compress( const uint8_t * _pui8Input, uint32_t _ui32Length, uint32_t _ui32Bits, lsc::CBitStream &_bsStream, uint32_t _ui32MaxSize ) {
		CLzwEncoder leEncoder;
		if ( !leEncoder.Encode( _pui8Input, _ui32Length,
			256, _ui32Bits, _bsStream, false, 0, _ui32MaxSize ) ) {
			return false;
		}
		if ( !leEncoder.EndStream( _bsStream ) ) { return false; }
		return true;
	}

	/**
	 * Find the best compression size and return the compressed bytes and the bit-count.
	 *
	 * \param _pui8Input The input data to compress.
	 * \param _ui32Length Length of the input data to compress.
	 * \param _ui32Bits The returned bit depth.
	 * \param _bsStream The compressed data.
	 * \return Returns true if a suitable compression was found, false otherwise.
	 */
	LSBOOL LSE_CALL CMisc::CompressBest( const uint8_t * _pui8Input, uint32_t _ui32Length, uint32_t &_ui32Bits, lsc::CBitStream &_bsStream ) {
		// Find the best compression level.
		_ui32Bits = 0;
		uint32_t ui32LowSize = LSE_MAXU32;
		for ( uint32_t I = 9; I <= 14; ++I ) {
			if ( m_ui32CompressLevel == CMisc::LSC_CL_MEDIUM && I != 12 ) { continue; }
			lsc::CBitStream bsTemp;
			bsTemp.PreAlloc( _ui32Length );
			if ( !Compress( _pui8Input, _ui32Length, I, bsTemp, ui32LowSize ) ) { return false; }

			if ( _ui32Bits == 0 || bsTemp.GetLengthInBytes() < _bsStream.GetLengthInBytes() ) {
				_bsStream.Reset();
				if ( _bsStream.WriteBytes( bsTemp.GetRawBytes(), bsTemp.GetLengthInBytes() ) != bsTemp.GetLengthInBytes() ) {
					return false;
				}
				_ui32Bits = I;
				ui32LowSize = bsTemp.GetLengthInBytes();
			}
		}

		// If the smallest compression is larger than the original data, use no compression.
		if ( _bsStream.GetLengthInBytes() >= _ui32Length ) {
			_ui32Bits = 0;
			_bsStream.Reset();
		}
		return true;
	}

	/**
	 * Compress bytes to a buffer.  If the compression is larger than the original data, the original data is written instead.
	 *
	 * \param _pui8Input The input data to compress.
	 * \param _ui32Length Length of the input data to compress.
	 * \param _bBuffer The buffer to which to write the bytes.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::CompressToBuffer( const uint8_t * _pui8Input, uint32_t _ui32Length, LSC_BUFFER &_bBuffer ) {
		uint32_t ui32Bits = 0;
		lsc::CBitStream bsTemp;
		if ( m_ui32CompressLevel != CMisc::LSC_CL_NONE ) {
			if ( !CompressBest( _pui8Input, _ui32Length, ui32Bits, bsTemp ) ) { return false; }
		}
		// Write the bit count.  0 = no compression.
		if ( !WriteUChar( static_cast<uint8_t>(ui32Bits), _bBuffer ) ) { return false; }

		// Append the data.
		if ( !ui32Bits ) {
			if ( !WriteUInt( _ui32Length, _bBuffer ) ) { return false; }
			if ( !WriteBytes( _pui8Input,
				_ui32Length, _bBuffer ) ) { return false; }
		}
		else {
			if ( !WriteUInt( bsTemp.GetLengthInBytes(), _bBuffer ) ) { return false; }
			if ( !WriteBytes( bsTemp.GetRawBytes(),
				bsTemp.GetLengthInBytes(), _bBuffer ) ) { return false; }
		}
		return true;
	}

	/**
	 * Decompress from packed data of any kind to a buffer.  If the expanded size is known beforehand, call Allocate() on the buffer to improve
	 *	the performance of this call.
	 *
	 * \param _pvData Pointer to the data to expand.
	 * \param _uiptrSize Size of the data to expand in bytes.
	 * \param _bBuffer Holds the expanded data.
	 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
	 */
	LSBOOL LSE_CALL CMisc::DecompressToBuffer( const void * _pvData, uintptr_t _uiptrSize, LSC_BUFFER &_bBuffer ) {
		CExternalByteStream ebsStream( reinterpret_cast<const uint8_t *>(_pvData), _uiptrSize );
		
		return DecompressToBuffer( ebsStream, _bBuffer );
	}

	/**
	 * Decompress from packed data of any kind to a buffer.  If the expanded size is known beforehand, call Allocate() on the buffer to improve
	 *	the performance of this call.
	 *
	 * \param _isStream Stream containing the bytes to expand.
	 * \param _bBuffer Holds the expanded data.
	 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
	 */
	LSBOOL LSE_CALL CMisc::DecompressToBuffer( const lsstd::CStream &_isStream, LSC_BUFFER &_bBuffer ) {
		uint8_t ui8CompressionBits;
		if ( _isStream.ReadBytes( &ui8CompressionBits, sizeof( ui8CompressionBits ) ) != sizeof( ui8CompressionBits ) ) { return false; }

		uint32_t ui32Size;
		if ( _isStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Size), sizeof( ui32Size ) ) != sizeof( ui32Size ) ) { return false; }

		uint8_t * pui8Buffer = LSENEW uint8_t[ui32Size];
		if ( !pui8Buffer ) { return false; }

		if ( _isStream.ReadBytes( pui8Buffer, ui32Size ) != ui32Size ) {
			LSEDELETE [] pui8Buffer;
			return false;
		}


		if ( ui8CompressionBits == 0 ) {
			// No compression.
			LSBOOL bRet = _bBuffer.Append( pui8Buffer,
				ui32Size );
			LSEDELETE [] pui8Buffer;
			return bRet;
		}
		// Must be decompressed.
		CLzwDecoder ldDecoder;
		lsc::CBitStream bsDest;
		bsDest.PreAlloc( ui32Size << 1 );	// Average compression amount is 40%, so using a buffer double the needed size
											//	should be good.  And if this fails, just continue.
		lsc::CBitStream bsBits;
		bsBits.Borrow( pui8Buffer, ui32Size );
		if ( ldDecoder.Decode( bsBits, 256, ui8CompressionBits, bsDest ) == 0 ) {
			LSEDELETE [] pui8Buffer;
			return false;
		}

		LSEDELETE [] pui8Buffer;
		return _bBuffer.Append( bsDest.GetRawBytes(),
			bsDest.GetLengthInBytes() );
	}

	/**
	 * Compress index data to a given bitsream.
	 *
	 * \param _pui32Input The input data to compress.
	 * \param _ui32Length Number of indices to to which _pui32Input points.
	 * \param _bsStream The compressed data.
	 * \return Returns true if there was enough memory to perform the encoding.
	 */
	LSBOOL LSE_CALL CMisc::CompressIndices( const uint32_t * _pui32Input, uint32_t _ui32Length, lsc::CBitStream &_bsStream ) {
		if ( !_bsStream.WriteBytes( reinterpret_cast<const uint8_t *>(&_ui32Length), sizeof( _ui32Length ) ) ) { return false; }


		if ( !_ui32Length ) { return true; }
		// Write the first index manually.
		if ( !_bsStream.WriteBits( reinterpret_cast<const uint8_t *>(&_pui32Input[0]), 32 ) ) { return false; }

		uint32_t ui32Last = _pui32Input[0];
		uint32_t ui32Index = 1;
		for ( uint32_t I = 3; I < 32 && ui32Index < _ui32Length; ++I ) {
			// The current bit size is I.
			// How many indices can be accurately stored with the current number of bits (I)?
			uint32_t J;
			uint32_t ui32LastCopy = ui32Last;
			for ( J = ui32Index; J < _ui32Length; ++J ) {
				uint32_t ui32This = _pui32Input[J] - ui32LastCopy;
				if ( GetBitsNeededForSignedValue( static_cast<int32_t>(ui32This) ) > I ) { break; }
				ui32LastCopy = _pui32Input[J];
			}
			// 24-bit counters are enough for any data.  This gives us room for 33,554,431 indices
			//	for each bit level.
			uint32_t ui32Total = J - ui32Index;
			assert( ui32Total < (1 << 25UL) );
			if ( !_bsStream.WriteBits( reinterpret_cast<const uint8_t *>(&ui32Total), 24 ) ) { return false; }

			// Write each index using I as the number of bits per index.
			while ( ui32Index < J ) {
				uint32_t ui32This = _pui32Input[ui32Index] - ui32Last;
				if ( !_bsStream.WriteBits( reinterpret_cast<const uint8_t *>(&ui32This), I ) ) { return false; }
				ui32Last = _pui32Input[ui32Index++];
			}

		}
		return true;
	}

	/**
	 * Decompress indices that are not further compressed to a buffer.  Input data must have been created with a call to CompressIndices()
	 *
	 * \param _bsStream The data to decompress.
	 * \param _bBuffer Holds the expanded data.
	 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
	 */
	LSBOOL LSE_CALL CMisc::DecompressIndices( const lsc::CBitStream &_bsStream, LSC_BUFFER &_bBuffer ) {
		uint32_t ui32Length;
		if ( !_bsStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Length), sizeof( ui32Length ) ) ) { return false; }
		if ( !ui32Length ) { return true; }

		uint32_t ui32LastValue = 0;
		if ( !_bsStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32LastValue), sizeof( ui32LastValue ) ) ) { return false; }
		if ( !_bBuffer.Append( reinterpret_cast<uint8_t *>(&ui32LastValue), sizeof( ui32LastValue ) ) ) { return false; }

		uint32_t ui32TotalRead = 1;
		for ( uint32_t I = 3; I < 32 && ui32TotalRead < ui32Length; ++I ) {
			uint32_t ui32Size = 0;
			if ( !_bsStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Size), 3 ) ) { return false; }
			ui32TotalRead += ui32Size;
			for ( ; ui32Size--; ) {
				uint32_t ui32Index = static_cast<uint32_t>(GetSignedValue( _bsStream, I ));
				//if ( !_bsStream.ReadBits( reinterpret_cast<uint8_t *>(&ui32Index), I ) ) { return false; }

				ui32Index += ui32LastValue;
				if ( !_bBuffer.Append( reinterpret_cast<uint8_t *>(&ui32Index), sizeof( ui32Index ) ) ) { return false; }
				ui32LastValue = ui32Index;
			}
		}
		return true;
	}

	/**
	 * Compress index data to a buffer.  If the compression is larger than the original data, the original data is written instead.
	 *
	 * \param _pui32Input The input data to compress.
	 * \param _ui32Length Number of indices to to which _pui32Input points.
	 * \param _bBuffer The buffer to which to write the bytes.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::CompressIndicesToBuffer( const uint32_t * _pui32Input, uint32_t _ui32Length, LSC_BUFFER &_bBuffer ) {
		lsc::CBitStream bsStream;
		if ( !CompressIndices( _pui32Input, _ui32Length, bsStream ) ) { return false; }

		return CompressToBuffer( bsStream.GetRawBytes(), bsStream.GetLengthInBytes(), _bBuffer );
	}

	/**
	 * Decompress indices from packed data to a buffer.
	 *
	 * \param _pvData Pointer to the data to expand.
	 * \param _uiptrSize Size of the data to expand in bytes.
	 * \param _bBuffer Holds the expanded data in which every index is 32 bits.
	 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
	 */
	LSBOOL LSE_CALL CMisc::DecompressIndicesToBuffer( const void * _pvData, uintptr_t _uiptrSize, LSC_BUFFER &_bBuffer ) {
		LSC_BUFFER bTemp;
		if ( !DecompressToBuffer( _pvData, _uiptrSize, bTemp ) ) { return false; }
		if ( bTemp.Length() == 0 ) { return true; }

		lsc::CBitStream bsStream;
		bsStream.Borrow( &bTemp[0], bTemp.Length() );
		return DecompressIndices( bsStream, _bBuffer );
	}

	/**
	 * Decompress indices from packed data to a buffer.
	 *
	 * \param _isStream The data to decompress.
	 * \param _bBuffer Holds the expanded data in which every index is 32 bits.
	 * \return Returns true if the input is valid and if there is enough memory to perform the expansion.  False is returned otherwise.
	 */
	LSBOOL LSE_CALL CMisc::DecompressIndicesToBuffer( const lsstd::CStream &_isStream, LSC_BUFFER &_bBuffer ) {
		LSC_BUFFER bTemp;
		if ( !DecompressToBuffer( _isStream, bTemp ) ) { return false; }
		if ( bTemp.Length() == 0 ) { return true; }

		lsc::CBitStream bsStream;
		bsStream.Borrow( &bTemp[0], bTemp.Length() );
		return DecompressIndices( bsStream, _bBuffer );
	}

	/**
	 * Read a string from a stream.
	 *
	 * \param _sStream The stream from which to read a string.
	 * \param _sRet The returned string.
	 * \return Returns true if there was enough memory to complete the operation.
	 */
	LSBOOL LSE_CALL CMisc::ReadString( const lsstd::CStream &_sStream, CString &_sRet ) {
		uint32_t ui32Len;
		if ( !_sStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Len), sizeof( ui32Len ) ) ) { return false; }
		_sRet.AllocateAtLeast( ui32Len + 2 );

		for ( ; ui32Len--; ) {
			uint8_t ui8Byte;
			if ( !_sStream.ReadBytes( &ui8Byte, sizeof( ui8Byte ) ) ) { return false; }
			if ( !_sRet.Append( static_cast<char>(ui8Byte) ) ) { return false; }
		}
		return true;
	}

	/**
	 * Reads only the 3-by-4 part of a matrix to a buffer.
	 *
	 * \param _sStream The stream from which to read the matrix the value.
	 * \param _mMat The matrix to hold the returned value.
	 * \param Returns true only if all bytes are read from the given stream.
	 */
	LSBOOL LSE_CALL CMisc::ReadMatrixSimple( const lsstd::CStream &_sStream, CMatrix4x4 &_mMat ) {
		_mMat.Identity();
		for ( uint32_t Y = 0; Y < 4; ++Y ) {
			for ( uint32_t X = 0; X < 3; ++X ) {
				LSREAL fVal;
				if ( _sStream.ReadBytes( reinterpret_cast<uint8_t *>(&fVal), sizeof( fVal ) ) != sizeof( fVal ) ) { return false; }
				_mMat( Y, X ) = fVal;
			}
		}
		return true;
	}

	/**
	 * Write bytes to a buffer.
	 *
	 * \param _pui8Bytes The values to write to the buffer.
	 * \param _ui32Length Number of bytes to write to the buffer.
	 * \param _bBuffer The buffer to which to write the bytes.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteBytes( const uint8_t * _pui8Bytes, uint32_t _ui32Length, LSC_BUFFER &_bBuffer ) {
		return _bBuffer.Append( _pui8Bytes, _ui32Length );
	}

	/**
	 * Write a byte to a buffer.
	 *
	 * \param _ui8Val The value to write to the buffer.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUChar( uint8_t _ui8Val, LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const uint8_t *>(&_ui8Val), sizeof( _ui8Val ), _bBuffer );
	}

	/**
	 * Write an unsigned integer to a buffer.
	 *
	 * \param _ui32Val The value to write to the file.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt( uint32_t _ui32Val, LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const uint8_t *>(&_ui32Val), sizeof( _ui32Val ), _bBuffer );
	}

	/**
	 * Gets the number of bits needed to store a number assumed to be signed.
	 *
	 * \param _i32Value The value to count how many bits are needed to store assuming the top bit is reserved for signs.
	 * \return Returns the number of bits needed to store a number assumed to be signed.
	 */
	uint32_t LSE_CALL CMisc::GetBitsNeededForSignedValue( int32_t _i32Value ) {
		if ( _i32Value < 0 ) {
			_i32Value = ~_i32Value;
		}
		return CStd::HighestBit( static_cast<uint32_t>(_i32Value) ) + 2;
	}

	/**
	 * Gets a signed number from a stream.
	 *
	 * \param _bsStream Stream from which to get the signed value.
	 * \param _ui32Bits Number of bits used to store the value.
	 * \return Returns the signed value.
	 */
	int32_t LSE_CALL CMisc::GetSignedValue( const lsc::CBitStream &_bsStream, uint32_t _ui32Bits ) {
		int32_t i32Value = 0;
		if ( !_bsStream.ReadBits( reinterpret_cast<uint8_t *>(&i32Value), _ui32Bits ) ) { return 0; }
		return static_cast<int32_t>(i32Value << (32 - static_cast<int32_t>(_ui32Bits))) >> (32 - static_cast<int32_t>(_ui32Bits));
	}

}	// namespace lsc
