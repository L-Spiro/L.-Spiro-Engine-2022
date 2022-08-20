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
 * Description: Handles the loading of 8-bit PNG files.
 */

#include "LSIPng.h"
#include "ZLib/LSCZLib.h"


namespace lsi {

	// == Members.
	/**
	 * Translates from a standard filter to a first-row filter.
	 */
	CPng::LSI_PNG_FILTER CPng::m_pfFirstRowFilter[] = {
		LSI_PF_NONE, LSI_PF_SUB, LSI_PF_NONE, LSI_PF_AVG_FIRST, LSI_PF_PAETH_FIRST
	};

	// == Functions.
	/**
	 * Load a PNG file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CPng::LoadPng( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		CExternalByteStream ebsStream( _pui8FileData, _ui32DataLen );

		if ( !CheckHeader( ebsStream ) ) { return false; }

		uint8_t ui8Palette[1024];
		uint8_t ui8PalImgN = 0;
		uint32_t ui32ImgN = 0, ui32PalLen = 0;
		CVectorPoD<uint8_t, uint32_t, 4096UL> vDataI;
		uint8_t ui8Components = 0, ui8Interlace = 0;
		LSBOOL bHasTrans = false;
		uint8_t ui8Tc[3];

		LSBOOL bIPhone = false, bInterlaced = false, bFirst = true;
#define LSI_PNG_TYPE( A, B, C, D )  (((A) << 24UL) + ((B) << 16) + ((C) << 8) + (D))
		for ( ; ; bFirst = false ) {
			LSI_PNG_CHUNK pcChunk;
			if ( !NextChunk( ebsStream, pcChunk ) ) { return false; }
			switch ( pcChunk.ui32Type ) {
				case LSI_PNG_TYPE( 'C', 'g', 'B', 'I' ) : {
					bIPhone = true;
					Skip( ebsStream, pcChunk.ui32Length );
					break;
				}
				case LSI_PNG_TYPE( 'I', 'H', 'D', 'R' ) : {
					if ( !bFirst ) { return false; }
					bFirst = false;
					if ( pcChunk.ui32Length != 13 ) { return false; }	// Bad header.
					if ( !ReadUi32( ebsStream, _ui32Width ) ) { return false; }
					if ( !ReadUi32( ebsStream, _ui32Height ) ) { return false; }
					if ( !_ui32Width || !_ui32Height ) { return false; }// 0-sized image.

					uint8_t ui8Depth;
					if ( !ReadUi8( ebsStream, ui8Depth ) ) { return false; }
					if ( ui8Depth != 8 ) { return false; }				// We only handle 8-bit images.

					uint8_t ui8Color;
					if ( !ReadUi8( ebsStream, ui8Color ) ) { return false; }
					if ( ui8Color > 6 ) { return false; }				// Bad ctype.

					if ( ui8Color == 3 ) {
						ui8PalImgN = 3;
					}
					else if ( ui8Color & 1 ) {
						return false;									// Bad ctype.
					}


					if ( !ReadUi8( ebsStream, ui8Components ) ) { return false; }
					if ( ui8Components ) { return false; }				// Bad component method.
					if ( ui8Components > 4 ) { return false; }

					uint8_t ui8Filter;
					if ( !ReadUi8( ebsStream, ui8Filter ) ) { return false; }
					if ( ui8Filter ) { return false; }					// Bad filter method.

					
					if ( !ReadUi8( ebsStream, ui8Interlace ) ) { return false; }
					if ( ui8Interlace > 1 ) { return false; }			// Bad interlace setting.
					bInterlaced = ui8Interlace;

					if ( !ui8PalImgN ) {
						// No palette.
						ui32ImgN = (ui8Color & 2 ? 3 : 1) + (ui8Color & 4 ? 1 : 0);
					}
					else {
						// Paletted.
						ui32ImgN = 1;
					}
					break;
				}
				case LSI_PNG_TYPE( 'P', 'L', 'T', 'E' ) : {
					if ( bFirst ) { return false; }
					if ( pcChunk.ui32Length > 256 * 3 ) { return false; }
					ui32PalLen = pcChunk.ui32Length / 3;
					if ( ui32PalLen * 3 != pcChunk.ui32Length ) { return false; }	// Make sure the palette length is a multiple of 3.
					for ( uint32_t I = 0; I < ui32PalLen; ++I ) {
						if ( !ReadUi8( ebsStream, ui8Palette[(I<<2)+0] ) ) { return false; }
						if ( !ReadUi8( ebsStream, ui8Palette[(I<<2)+1] ) ) { return false; }
						if ( !ReadUi8( ebsStream, ui8Palette[(I<<2)+2] ) ) { return false; }
						ui8Palette[(I<<2)+3] = 0xFF;
						
					}
					break;
				}
				case LSI_PNG_TYPE( 't', 'R', 'N', 'S' ) : {
					if ( bFirst ) { return false; }
					if ( vDataI.Length() ) { return false; }
					if ( ui8PalImgN ) {
						if ( ui32PalLen == 0 ) { return false; }
						if ( pcChunk.ui32Length > ui32PalLen ) { return false; }
						ui8PalImgN = 4;
						for ( uint32_t I = 0; I < pcChunk.ui32Length; ++I ) {
							if ( !ReadUi8( ebsStream, ui8Palette[(I<<2)+3] ) ) { return false; }
						}
					}
					else {
						if ( !(ui32ImgN & 1) ) { return false; }
						if ( pcChunk.ui32Length != (ui32ImgN << 1) ) { return false; }
						bHasTrans = true;
						for ( uint32_t I = 0; I < ui32ImgN; ++I ) {
							uint16_t ui16Temp;
							if ( !ReadUi16( ebsStream, ui16Temp ) ) { return false; }
							ui8Tc[I] = static_cast<uint8_t>(ui16Temp);
						}
					}
					break;
				}
				case LSI_PNG_TYPE( 'I', 'D', 'A', 'T' ) : {
					if ( bFirst ) { return false; }
					if ( ui8PalImgN && !ui32PalLen ) { return false; }
					uint32_t ui32Len = vDataI.Length();
					if ( !vDataI.Resize( ui32Len + pcChunk.ui32Length ) ) { return false; }	// Out of memory.

					if ( !ebsStream.ReadBytes( &vDataI[ui32Len], pcChunk.ui32Length ) ) { return false; }
					break;
				}
				case LSI_PNG_TYPE( 'I', 'E', 'N', 'D' ) : {
					if ( bFirst ) { return false; }
					if ( !vDataI.Length() ) { return false; }
					CZLib::LSC_ZLIB_BUFFER zbRet;
					CZLib zLib;
					if ( !zLib.DecodeZLib( &vDataI[0], vDataI.Length(), bIPhone, zbRet ) ) { return false; }
					if ( !zbRet.Length() ) { return false; }
					vDataI.Reset();		// Save some memory.

					LSI_PNG_FINAL pfFinal;
					pfFinal.ui32X = pfFinal.ui32Width = _ui32Width;
					pfFinal.ui32Y = pfFinal.ui32Height = _ui32Height;
					pfFinal.ui32Components = ui32ImgN;
					pfFinal.puiRawData = &zbRet[0];
					pfFinal.ui32RawLen = zbRet.Length();
					if ( !CreatePng( pfFinal, _tbReturn, ui8Interlace ) ) { return false; }

					static const LSI_PIXEL_FORMAT pfFormats[] = {
						LSI_PF_R3G3B2,
						LSI_PF_R4G4B4A4,
						LSI_PF_R8G8B8,
						LSI_PF_R8G8B8A8
					};
					if ( ui8PalImgN ) {
						_pfFormat = pfFormats[ui8PalImgN-1];
						if ( !ExpandPalette( _tbReturn, ui8Palette, ui32PalLen, _ui32Width, _ui32Height, ui8PalImgN, !bIPhone ) ) { return false; }
					}
					else {
						_pfFormat = pfFormats[ui32ImgN-1];

						if ( !bIPhone ) {
							FixColors( _tbReturn, ui32ImgN );
						}
					}
					return true;
				}
				default : {
					if ( bFirst ) { return false; }
					if ( !(pcChunk.ui32Type & (1 << 29UL)) ) { return false; }
					Skip( ebsStream, pcChunk.ui32Length );
				}
			}

			// End of the chunk.  Skip the CRC.
			uint32_t ui32Crc;
			if ( !ReadUi32( ebsStream, ui32Crc ) ) { return false; }
		}
#undef LSI_PNG_TYPE
	}

	/**
	 * Check the header of a PNG file loaded to memory.  Validates that the header data matches a PNG
	 *	header.
	 *
	 * \param _ebsStream The stream containing the loaded PNG file.  The position must be at the start of
	 *	the stream.
	 * \return Returns true if the bytes in the stream form a valid PNG header.
	 */
	LSBOOL LSE_CALL CPng::CheckHeader( const CExternalByteStream &_ebsStream ) {
		static uint8_t ui8Sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

		uint8_t ui8This;
		for ( uint32_t I = 0; I < LSE_ELEMENTS( ui8Sig ); ++I ) {
			if ( !_ebsStream.ReadBytes( &ui8This, 1 ) ) { return false; }
			if ( ui8This != ui8Sig[I] ) { return false; }
		}
		return true;
	}

	/**
	 * Read the next chunk.
	 *
	 * \param _ebsStream The stream from which to read the next chunk.
	 * \param _pcChunk Holds the returned chunk data, which does not need to be freed.
	 * \return Returns true if the next chunk was read.
	 */
	LSBOOL LSE_CALL CPng::NextChunk( const CExternalByteStream &_ebsStream, LSI_PNG_CHUNK &_pcChunk ) {
		if ( !ReadUi32( _ebsStream, _pcChunk.ui32Length ) ) { return false; }
		if ( !ReadUi32( _ebsStream, _pcChunk.ui32Type ) ) { return false; }
		return true;
	}

	/**
	 * Create the final image data from decompressed PNG data.  Image data must not be interlaced.
	 *
	 * \param _pfFinal Parameters.
	 * \param _tbReturn The output of the processing.
	 * \return Returns true if there is enough memory to allocate the final buffer.
	 */
	LSBOOL LSE_CALL CPng::CreatePng( LSI_PNG_FINAL &_pfFinal, CImageLib::CTexelBuffer &_tbReturn ) {
		uint32_t ui32Stride = _pfFinal.ui32X * _pfFinal.ui32Components;
		if ( !_tbReturn.Resize( ui32Stride * _pfFinal.ui32Y ) ) { return false; }


		if ( _pfFinal.ui32Width == _pfFinal.ui32X && _pfFinal.ui32Height == _pfFinal.ui32Y ) {
			if ( _pfFinal.ui32RawLen != (_pfFinal.ui32Components * _pfFinal.ui32X + 1) * _pfFinal.ui32Y ) { return false; }
		}
		else {
			// Interlaced.
			if ( _pfFinal.ui32RawLen < (_pfFinal.ui32Components * _pfFinal.ui32X + 1) * _pfFinal.ui32Y ) { return false; }
		}


		uint8_t * pui8Raw = _pfFinal.puiRawData;
		for ( uint32_t J = 0; J < _pfFinal.ui32Y; ++J ) {
			uint8_t * pui8Cur = &_tbReturn[0] + ui32Stride * J;
			uint8_t * pui8Prev = pui8Cur - ui32Stride;
			uint32_t ui32Filter = (*pui8Raw++);
			if ( ui32Filter > 4 ) { return false; }

			if ( J == 0 ) {
				// First row must not sample previous row.
				ui32Filter = m_pfFirstRowFilter[ui32Filter];
			}

			uint32_t K;
			for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
				switch ( ui32Filter ) {
					case LSI_PF_NONE : {
						pui8Cur[K] = pui8Raw[K];
						break;
					}
					case LSI_PF_SUB : {
						pui8Cur[K] = pui8Raw[K];
						break;
					}
					case LSI_PF_UP : {
						pui8Cur[K] = pui8Raw[K] + pui8Prev[K];
						break;
					}
					case LSI_PF_AVG : {
						pui8Cur[K] = pui8Raw[K] + (pui8Prev[K] >> 1);
						break;
					}
					case LSI_PF_PAETH : {
						pui8Cur[K] = static_cast<uint8_t>(pui8Raw[K] + Paeth( 0, pui8Prev[K], 0 ));
						break;
					}
				}
			}
			pui8Raw += _pfFinal.ui32Components;
			pui8Cur += _pfFinal.ui32Components;
			pui8Prev += _pfFinal.ui32Components;

			uint32_t I;
			switch ( ui32Filter ) {
				case LSI_PF_NONE : {
					for ( I = _pfFinal.ui32X - 1; I >= 1; --I,
						pui8Raw += _pfFinal.ui32Components,
						pui8Cur += _pfFinal.ui32Components,
						pui8Prev += _pfFinal.ui32Components ) {
						for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
							pui8Cur[K] = pui8Raw[K];
						}
					}
					break;
				}
				case LSI_PF_SUB : {
					for ( I = _pfFinal.ui32X - 1; I >= 1; --I,
						pui8Raw += _pfFinal.ui32Components,
						pui8Cur += _pfFinal.ui32Components,
						pui8Prev += _pfFinal.ui32Components ) {
						for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
							pui8Cur[K] = pui8Raw[K] + pui8Cur[static_cast<uintptr_t>(K)-static_cast<uintptr_t>(_pfFinal.ui32Components)];
						}
					}
					break;
				}
				case LSI_PF_UP : {
					for ( I = _pfFinal.ui32X - 1; I >= 1; --I,
						pui8Raw += _pfFinal.ui32Components,
						pui8Cur += _pfFinal.ui32Components,
						pui8Prev += _pfFinal.ui32Components ) {
						for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
							pui8Cur[K] = pui8Raw[K] + pui8Prev[K];
						}
					}
					break;
				}
				case LSI_PF_AVG : {
					for ( I = _pfFinal.ui32X - 1; I >= 1; --I,
						pui8Raw += _pfFinal.ui32Components,
						pui8Cur += _pfFinal.ui32Components,
						pui8Prev += _pfFinal.ui32Components ) {
						for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
							pui8Cur[K] = pui8Raw[K] + ((pui8Prev[K] + pui8Cur[static_cast<uintptr_t>(K)-static_cast<uintptr_t>(_pfFinal.ui32Components)]) >> 1);
						}
					}
					break;
				}
				case LSI_PF_PAETH : {
					for ( I = _pfFinal.ui32X - 1; I >= 1; --I,
						pui8Raw += _pfFinal.ui32Components,
						pui8Cur += _pfFinal.ui32Components,
						pui8Prev += _pfFinal.ui32Components ) {
						for ( K = 0; K < _pfFinal.ui32Components; ++K ) {
							pui8Cur[K] = static_cast<uint8_t>(pui8Raw[K] + Paeth( pui8Cur[static_cast<uintptr_t>(K)-static_cast<uintptr_t>(_pfFinal.ui32Components)], pui8Prev[K], pui8Prev[static_cast<uintptr_t>(K)-static_cast<uintptr_t>(_pfFinal.ui32Components)] ));
						}
					}
					break;
				}
			}
		}


		return true;
	}

	/**
	 * Create the final image data from decompressed PNG data.
	 *
	 * \param _pfFinal Parameters.
	 * \param _tbReturn The output of the processing.
	 * \param _bInterlaced If true, the interlace decoder is used.
	 * \return Returns true if there is enough memory to allocate the final buffer.
	 */
	LSBOOL LSE_CALL CPng::CreatePng( LSI_PNG_FINAL &_pfFinal, CImageLib::CTexelBuffer &_tbReturn, LSBOOL _bInterlaced ) {
		if ( !_bInterlaced ) { return CreatePng( _pfFinal, _tbReturn ); }

		CImageLib::CTexelBuffer tbTemp;
		if ( !_tbReturn.Resize( _pfFinal.ui32Width * _pfFinal.ui32Height * _pfFinal.ui32Components ) ) { return false; }

		static const uint32_t ui32OrigX[] = { 0, 4, 0, 2, 0, 1, 0 };
		static const uint32_t ui32OrigY[] = { 0, 0, 4, 0, 2, 0, 1 };
		static const uint32_t ui32SpaceX[]  = { 8, 8, 4, 4, 2, 2, 1 };
		static const uint32_t ui32SpaceY[]  = { 8, 8, 8, 4, 4, 2, 2 };

		for ( uint32_t I = 0; I < 7UL; ++I ) {
			_pfFinal.ui32X = (_pfFinal.ui32Width - ui32OrigX[I] + ui32SpaceX[I] - 1) / ui32SpaceX[I];
			_pfFinal.ui32Y = (_pfFinal.ui32Height - ui32OrigY[I] + ui32SpaceY[I] - 1) / ui32SpaceY[I];

			if ( _pfFinal.ui32X && _pfFinal.ui32Y ) {
				if ( !CreatePng( _pfFinal, tbTemp ) ) { return false; }
				for ( uint32_t Y = 0; Y < _pfFinal.ui32Y; ++Y ) {
					for ( uint32_t X = 0; X < _pfFinal.ui32X; ++X ) {
						CStd::MemCpy( &_tbReturn[0] + (Y * ui32SpaceY[I] + ui32OrigY[I]) * _pfFinal.ui32Width * _pfFinal.ui32Components +
							(X * ui32SpaceX[I] + ui32OrigX[I]) * _pfFinal.ui32Components,
							&tbTemp[0] + (Y * _pfFinal.ui32X + X) * _pfFinal.ui32Components,
							_pfFinal.ui32Components );
					}
				}

				_pfFinal.puiRawData += (_pfFinal.ui32X * _pfFinal.ui32Components + 1) * _pfFinal.ui32Y;
				_pfFinal.ui32RawLen -= (_pfFinal.ui32X * _pfFinal.ui32Components + 1) * _pfFinal.ui32Y;
			}
		}

		return true;
	}

	/**
	 * Expand the color data with the data in the given palette.
	 *
	 * \param _tbReturn The buffer to expand in-place.
	 * \param _pui8Palette The palette.
	 * \param _ui32Len Length of the palette.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Components Components in the image.
	 * \param _bSwapRB If true, the R and B components of the palette are swapped as they are placed into the image buffer.
	 * \return Returns true if there is enough memory to expand the buffer and if none of the indices refence above
	 *	or equal to _ui32Len.
	 */
	LSBOOL LSE_CALL CPng::ExpandPalette( CImageLib::CTexelBuffer &_tbReturn, const uint8_t * _pui8Palette, uint32_t _ui32Len,
		uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Components, LSBOOL _bSwapRB ) {
		uint32_t ui32Orig = _tbReturn.Length();
		if ( !_tbReturn.Resize( _ui32Width * _ui32Height * _ui32Components ) ) { return false; }
		_ui32Len *= 4;
		uint8_t * pui8Dest = &_tbReturn[0] + _tbReturn.Length();

		uint32_t ui32R = _bSwapRB ? 2 : 0;
		uint32_t ui32B = _bSwapRB ? 0 : 2;
		if ( _ui32Components == 3 ) {
			for ( uint32_t I = ui32Orig; I--; ) {
				pui8Dest -= 3;
				uint32_t ui32Index = static_cast<uint32_t>(_tbReturn[I]) << 2;
				if ( ui32Index >= _ui32Len ) { return false; }
				pui8Dest[0] = _pui8Palette[ui32Index+ui32R];
				pui8Dest[1] = _pui8Palette[ui32Index+1];
				pui8Dest[2] = _pui8Palette[ui32Index+ui32B];

			}
		}
		else {
			for ( uint32_t I = ui32Orig; I--; ) {
				pui8Dest -= 4;
				uint32_t ui32Index = static_cast<uint32_t>(_tbReturn[I]) << 2;
				if ( ui32Index >= _ui32Len ) { return false; }
				pui8Dest[0] = _pui8Palette[ui32Index+3];
				pui8Dest[1] = _pui8Palette[ui32Index+ui32R];
				pui8Dest[2] = _pui8Palette[ui32Index+1];
				pui8Dest[3] = _pui8Palette[ui32Index+ui32B];

			}
		}
		return true;
	}

	/**
	 * Swap the RGB colors in the given buffer.
	 *
	 * \param _tbBuffer Buffer of colors to fix.
	 * \param _ui32Components Number of components.
	 */
	void LSE_CALL CPng::FixColors( CImageLib::CTexelBuffer &_tbBuffer, uint32_t _ui32Components ) {
		if ( _ui32Components == 1 ) { return; }
		if ( _ui32Components == 4 ) {
			for ( uint32_t I = 0; I < _tbBuffer.Length(); I += 4 ) {
				(*reinterpret_cast<uint32_t *>(&_tbBuffer[I])) = CStd::ByteSwap32( (*reinterpret_cast<uint32_t *>(&_tbBuffer[I])) );
			}
		}
		else {
			for ( uint32_t I = 0; I < _tbBuffer.Length(); I += _ui32Components ) {
				uint8_t ui8This = _tbBuffer[I+0];
				_tbBuffer[I+0] = _tbBuffer[I+2];
				_tbBuffer[I+2] = ui8This;
			}
		}
	}

	/**
	 * Reads a single 8-bit value from the given stream.
	 *
	 * \param _ebsStream The stream from which to read an 8-bit value.
	 * \param _ui8Dst Destination for the read value.
	 * \return Returns true if the value was read from the stream.
	 */
	LSBOOL LSE_CALL CPng::ReadUi8( const CExternalByteStream &_ebsStream, uint8_t &_ui8Dst ) {
		return _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&_ui8Dst), sizeof( _ui8Dst ) ) != 0;
	}

	/**
	 * Reads a single 16-bit value from the given stream.
	 *
	 * \param _ebsStream The stream from which to read an 16-bit value.
	 * \param _ui16Dst Destination for the read value.
	 * \return Returns true if the value was read from the stream.
	 */
	LSBOOL LSE_CALL CPng::ReadUi16( const CExternalByteStream &_ebsStream, uint16_t &_ui16Dst ) {
		if ( !_ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&_ui16Dst), sizeof( _ui16Dst ) ) ) { return false; }
		_ui16Dst = CStd::ByteSwap16( _ui16Dst );
		return true;
	}

	/**
	 * Reads a single 32-bit value from the given stream.
	 *
	 * \param _ebsStream The stream from which to read an 32-bit value.
	 * \param _ui32Dst Destination for the read value.
	 * \return Returns true if the value was read from the stream.
	 */
	LSBOOL LSE_CALL CPng::ReadUi32( const CExternalByteStream &_ebsStream, uint32_t &_ui32Dst ) {
		if ( !_ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&_ui32Dst), sizeof( _ui32Dst ) ) ) { return false; }
		_ui32Dst = CStd::ByteSwap32( _ui32Dst );
		return true;
	}

	/**
	 * Skip a section of the stream.
	 *
	 * \param _ebsStream The stream.
	 * \param _ui32Length Number of bytes to skip.
	 */
	void LSE_CALL CPng::Skip( const CExternalByteStream &_ebsStream, uint32_t _ui32Length ) {
		_ebsStream.SetPos( _ebsStream.GetPos() + _ui32Length );
	}

	/**
	 * Perform a PAETH filter.
	 *
	 * \param _i32A A.
	 * \param _i32B B.
	 * \param _i32C C.
	 * \return Returns happiness.
	 */
	int32_t LSE_CALL CPng::Paeth( int32_t _i32A, int32_t _i32B, int32_t _i32C ) {
		int32_t i32P = _i32A + _i32B - _i32C;
#define LSI_ABS( VAL )		(VAL) > 0 ? (VAL) : -(VAL)
		int32_t i32Pa = LSI_ABS( i32P - _i32A );
		int32_t i32Pb = LSI_ABS( i32P - _i32B );
		int32_t i32Pc = LSI_ABS( i32P - _i32C );
		if ( i32Pa < i32Pb && i32Pa <= i32Pc ) { return _i32A; }
		if ( i32Pb <= i32Pc ) { return _i32B; }

		return _i32C;
#undef LSI_ABS
	}

}	// namespace lsi
