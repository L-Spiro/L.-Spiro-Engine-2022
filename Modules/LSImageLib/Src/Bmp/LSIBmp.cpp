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
 * Description: Loading a bitmap file.
 */

#include "LSIBmp.h"
#include "HighLevel/LSFFilesEx.h"


namespace lsi {

	// == Functions.
	/**
	 * Load a BMP file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CBmp::LoadBmp( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		// Size checks.
		if ( _ui32DataLen < sizeof( LSI_BITMAPFILEHEADER ) + sizeof( LSI_BITMAPINFOHEADER ) ) { return false; }
		const LSI_BITMAPFILEHEADER * lpbfhHeader = reinterpret_cast<const LSI_BITMAPFILEHEADER *>(_pui8FileData);
		// Check the header.
		if ( lpbfhHeader->ui16Header != 0x4D42 ) { return false; }

		// Verify the size of the file.
		if ( lpbfhHeader->ui32Size != _ui32DataLen ) { return false; }

		// Header checks are done.  Move on to the bitmap info.
		const LSI_BITMAPINFOHEADER * lpbfhInfo = reinterpret_cast<const LSI_BITMAPINFOHEADER *>(&_pui8FileData[sizeof( LSI_BITMAPFILEHEADER )]);
		
		// Check the size of the information header.
		if ( lpbfhInfo->ui32InfoSize < sizeof( LSI_BITMAPINFOHEADER ) ) { return false; }

		// Verify the compression.
		int32_t i32Compression = static_cast<int32_t>(lpbfhInfo->ui32Compression);
		if ( i32Compression < 0 || i32Compression > 3 ) { return false; }
		if ( i32Compression == 1 ) {
			if ( _ui32DataLen < sizeof( LSI_BITMAPFILEHEADER ) + sizeof( LSI_BITMAPINFOHEADER ) + sizeof( LSI_BITMAPPALETTE ) * lpbfhInfo->ui32ColorsInPalette ) { return false; }
		}
		if ( i32Compression == 3 ) {
			if ( _ui32DataLen < sizeof( LSI_BITMAPFILEHEADER ) + sizeof( LSI_BITMAPINFOHEADER ) + sizeof( LSI_BITMAPCOLORMASK ) ) { return false; }
		}

		// We only handle bits-per-pixels of 1, 4, 8, 16, 24, and 32.
		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 1 : {}
			case 4 : { return false; }	// Temporarily.
			case 8 : {
				if ( lpbfhInfo->ui32ColorsInPalette > 256 ) { return false; }
				if ( _ui32DataLen < sizeof( LSI_BITMAPFILEHEADER ) + sizeof( LSI_BITMAPINFOHEADER ) + sizeof( LSI_BITMAPPALETTE ) * lpbfhInfo->ui32ColorsInPalette ) { return false; }
				break;
			}
			case 16 : {}
			case 24 : {}
			case 32 : { break; }
			default : { return false; }
		}

		uint32_t ui32BytesPerPixel;
		uint32_t ui32BytesPerPixelDst;
		uint32_t ui32BitMask = 0x0;
		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 1 : {
				_pfFormat = LSI_PF_R3G3B2;
				ui32BytesPerPixel = 1;
				ui32BitMask = 0x1;
				break;
			}
			case 4 : {
				_pfFormat = LSI_PF_R3G3B2;
				ui32BytesPerPixel = 4;
				ui32BitMask = 0xF;
				break;
			}
			case 8 : {
				_pfFormat = LSI_PF_R8G8B8;
				ui32BytesPerPixel = 8;
				ui32BitMask = 0xFF;
				break;
			}
			case 16 : {
				_pfFormat = LSI_PF_R5G6B5;
				ui32BytesPerPixel = 2;
				break;
			}
			case 24 : { _pfFormat = LSI_PF_R8G8B8; ui32BytesPerPixel = 3; break; }
			case 32 : { _pfFormat = LSI_PF_R8G8B8A8; ui32BytesPerPixel = 4; break; }
			default : { return false; }
		}

		ui32BytesPerPixelDst = CImageLib::GetFormatSize( _pfFormat );

		// We are now ready to begin the extraction of the image data.
		// Start by allocating enough RAM.
		uint32_t ui32Height = lpbfhInfo->ui32Height;
		LSBOOL bReverse = false;
		if ( ui32Height & 0x80000000 ) {
			bReverse = true;
			// Invert the height.
			ui32Height = ~ui32Height + 1;
		}

		// Final check.
		if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, lpbfhInfo->ui32Width, ui32Height ) ) ) { return false; }
		CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );


		// Everything is set.  Start the extraction.
		_ui32Width = lpbfhInfo->ui32Width;
		_ui32Height = ui32Height;
		

		uint32_t ui32RowWidth = _ui32Width * ui32BytesPerPixel;
		if ( lpbfhInfo->ui16BitsPerPixel < 16 ) {
			ui32RowWidth = _ui32Width;
		}
		if ( ui32RowWidth & 0x3 ) {
			ui32RowWidth = (ui32RowWidth & ~3) + 4;
		}

		uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, lpbfhInfo->ui32Width );

		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 8 : {
				uint32_t ui32ActualOffset = lpbfhHeader->ui32Offset;

				// Pointer to the palette data.
				const LSI_BITMAPPALETTE * lpbpPalette = reinterpret_cast<const LSI_BITMAPPALETTE *>(&_pui8FileData[sizeof( LSI_BITMAPFILEHEADER )+sizeof( LSI_BITMAPINFOHEADER )]);

				// If it is RLE-compressed.
				if ( i32Compression == 1 ) {
				}
				else {
					uint32_t ui32EightOverBytes = 8 / ui32BytesPerPixel;
					// Otherwise not RLE compressed.
					for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
						uint32_t ui32BitIndexBase = Y * _ui32Width;
						uint32_t ui32YOff = ((Y * ui32RowWidth * ui32BytesPerPixel) >> 3) + ui32ActualOffset;

						// This line is what handles reverse-encoded bitmaps.
						uint32_t ui32YOffDest = bReverse ? Y * ui32DestRowWidth :
							(_ui32Height - Y - 1) * ui32DestRowWidth;

						uint8_t * pui8Dest = &_tbReturn[ui32YOffDest];

						

						for ( uint32_t X = 0; X < _ui32Width; ++X ) {
							// Get the byte from which we will read.
							const uint8_t * pui8TargetByte = &_pui8FileData[ui32YOff+((X*ui32BytesPerPixel)>>3)];
							// Get the bits within that byte we need.
							uint32_t ui32BitIndex = (ui32BitIndexBase + X) % (ui32EightOverBytes);
							uint32_t ui32Shift = ui32BitIndex * ui32BytesPerPixel;

							// Decode into an index.
							uint32_t ui32Index = ((*pui8TargetByte) & (ui32BitMask << (ui32Shift))) >> ui32Shift;

							// Sanity check.
							if ( ui32Index >= lpbfhInfo->ui32ColorsInPalette ) {
								_tbReturn.Reset();
								return false;
							}

							// Get the color from the palette.
							uint32_t ui32R = (lpbpPalette[ui32Index].ui32Color >> 0) & 0xFF;
							uint32_t ui32G = (lpbpPalette[ui32Index].ui32Color >> 8) & 0xFF;
							uint32_t ui32B = (lpbpPalette[ui32Index].ui32Color >> 16) & 0xFF;
							uint32_t ui32A = (lpbpPalette[ui32Index].ui32Color >> 24) & 0xFF;
							uint32_t ui32Color = (ui32R << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R )) |
								(ui32G << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G )) |
								(ui32B << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B )) |
								(ui32A << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_A ));

							// Convert to the destination format.
							uint32_t ui32Final = ui32Color;//static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( LSI_PF_R8G8B8A8, _pfFormat, ui32Color ));

							static const uint32_t ui32Mask[] = {
								0xFFFFFFFF,
								0xFFFFFF00,
								0xFFFF0000,
								0xFF000000,
								0x00000000
							};
							static const uint32_t ui32Sizes[] = {
								0x00000000,
								0x000000FF,
								0x0000FFFF,
								0x00FFFFFF,
								0xFFFFFFFF
							};

							uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8Dest[X*ui32BytesPerPixelDst]);
							(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32BytesPerPixelDst]) | (ui32Final & ui32Sizes[ui32BytesPerPixelDst]);
						}
					}
				}

				break;
			}
			/*case 2 : {
				for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
					uint32_t ui32YOff = Y * ui32RowWidth;
					for ( uint32_t X = 0; X < _ui32Width; ++X ) {
						uint8_t * pui8This = &_pui8FileData[ui32YOff+X*ui32BytesPerPixel];
					}
				}
				break;
			}*/
			case 16 : {}
			case 24 : {}
			case 32 : {
				uint32_t ui32ActualOffset = lpbfhHeader->ui32Offset;

				// The color masks tell us the order and sizes of the colors in the image.
				const LSI_BITMAPCOLORMASK * lpbcmMask;
				if ( i32Compression == 3 ) {
					lpbcmMask = reinterpret_cast<const LSI_BITMAPCOLORMASK *>(&_pui8FileData[sizeof( LSI_BITMAPFILEHEADER )+sizeof( LSI_BITMAPINFOHEADER )]);
				}
				else {
					static const LSI_BITMAPCOLORMASK bcmDefaultMask32 = {
						0x00FF0000,
						0x0000FF00,
						0x000000FF,
						0xFF000000,
					};
					static const LSI_BITMAPCOLORMASK bcmDefaultMask24 = {
						0x00FF0000,
						0x0000FF00,
						0x000000FF,
						0x00000000,
					};
					static const LSI_BITMAPCOLORMASK bcmDefaultMask16 = {
						0x00007C00,
						0x000003E0,
						0x0000001F,
						0x00000000,
					};
					if ( lpbfhInfo->ui16BitsPerPixel == 16 ) {
						lpbcmMask = &bcmDefaultMask16;
					}
					else if ( lpbfhInfo->ui16BitsPerPixel == 32 ) {
						lpbcmMask = &bcmDefaultMask32;
					}
					else {
						lpbcmMask = &bcmDefaultMask24;
					}
				}
				// Determine how much we need to shift each channel to get the values we expect.
				uint32_t ui32RShift = 0;
				uint32_t ui32GShift = 0;
				uint32_t ui32BShift = 0;
				uint32_t ui32AShift = 0;
				while ( ui32RShift < 32 && !(lpbcmMask->ui32Red & (1 << ui32RShift)) ) { ++ui32RShift; }
				while ( ui32GShift < 32 && !(lpbcmMask->ui32Green & (1 << ui32GShift)) ) { ++ui32GShift; }
				while ( ui32BShift < 32 && !(lpbcmMask->ui32Blue & (1 << ui32BShift)) ) { ++ui32BShift; }
				while ( ui32AShift < 32 && !(lpbcmMask->ui32Alpha & (1 << ui32AShift)) ) { ++ui32AShift; }

				// Also get the number of bits per component.
				uint32_t ui32RBits = 0;
				uint32_t ui32GBits = 0;
				uint32_t ui32BBits = 0;
				uint32_t ui32ABits = 0;
				while ( (lpbcmMask->ui32Red & (1 << (ui32RShift + ui32RBits)) ) ) { ++ui32RBits; }
				while ( (lpbcmMask->ui32Green & (1 << (ui32GShift + ui32GBits)) ) ) { ++ui32GBits; }
				while ( (lpbcmMask->ui32Blue & (1 << (ui32BShift + ui32BBits)) ) ) { ++ui32BBits; }
				while ( (lpbcmMask->ui32Alpha & (1 << (ui32AShift + ui32ABits)) ) ) { ++ui32ABits; }
				

				// It is not too late to change the format as long as it stays the same size.
				// Specifically, here we want to verify/modify the 16-bit format.
				if ( lpbfhInfo->ui16BitsPerPixel == 16 ) {
					// If there is an alpha channel, we need to modify the format to have one
					//	as well.
					if ( ui32ABits ) {
						// If more than one bit is needed for alpha.
						if ( ui32ABits > 1 ) {
							_pfFormat = LSI_PF_R4G4B4A4;
						}
						// Otherwise use one bit.
						else {
							_pfFormat = LSI_PF_R5G5B5A1;
						}
					}
				}


				// Convert to the format in which they can be used fastest.
				ui32RBits = 8 - ui32RBits;
				ui32GBits = 8 - ui32GBits;
				ui32BBits = 8 - ui32BBits;
				ui32ABits = 8 - ui32ABits;

				for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
					uint32_t ui32YOffSrc = Y * ui32RowWidth + ui32ActualOffset;
					// This line is what handles reverse-encoded bitmaps.
					uint32_t ui32YOffDest = bReverse ? Y * ui32DestRowWidth :
						(_ui32Height - Y - 1) * ui32DestRowWidth;

					// We can copy whole rows at a time if they are in the same format already.
					const uint8_t * pui8Src = &_pui8FileData[ui32YOffSrc];
					uint8_t * pui8Dest = &_tbReturn[ui32YOffDest];

					// Since bitmaps store the alpha on the reverse side, we cannot use this trick
					//	when there is an alpha channel.  In the end, it turns out that this only
					//	works on 24-bit bitmaps.  But that is not so bad since they are actually
					//	fairly common.
					if ( i32Compression == 0 &&
						ui32RShift == CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R ) &&
						ui32GShift == CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G ) &&
						ui32BShift == CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B ) &&
						(ui32AShift == 32 || ui32AShift == CImageLib::GetComponentOffset( _pfFormat, LSI_PC_A )) ) {
						CStd::MemCpy( pui8Dest, pui8Src, _ui32Width * ui32BytesPerPixel );
					}
					else {
						for ( uint32_t X = 0; X < _ui32Width; ++X ) {
							uint32_t ui32Src = (*reinterpret_cast<const uint32_t *>(&pui8Src[X*ui32BytesPerPixel]));

							uint32_t ui32R = (ui32Src & lpbcmMask->ui32Red) >> ui32RShift;
							ui32R <<= ui32RBits;
							uint32_t ui32G = (ui32Src & lpbcmMask->ui32Green) >> ui32GShift;
							ui32G <<= ui32GBits;
							uint32_t ui32B = (ui32Src & lpbcmMask->ui32Blue) >> ui32BShift;
							ui32B <<= ui32BBits;
							uint32_t ui32A = (ui32Src & lpbcmMask->ui32Alpha) >> ui32AShift;
							ui32A <<= ui32ABits;
							uint32_t ui32Final;
							// Correct the missing alpha channel.
							if ( ui32AShift == 32 ) { ui32A = 0xFF; }
							ui32Final = (ui32R << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_R )) |
								(ui32G << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_G )) |
								(ui32B << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_B )) |
								(ui32A << CImageLib::GetComponentOffset( LSI_PF_R8G8B8A8, LSI_PC_A ));

							// Convert to the destination format.
							ui32Final = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( LSI_PF_R8G8B8A8, _pfFormat, ui32Final, LSI_G_NONE ));

							static const uint32_t ui32Mask[] = {
								0xFFFFFFFF,
								0xFFFFFF00,
								0xFFFF0000,
								0xFF000000,
								0x00000000
							};
							static const uint32_t ui32Sizes[] = {
								0x00000000,
								0x000000FF,
								0x0000FFFF,
								0x00FFFFFF,
								0xFFFFFFFF
							};

							uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8Dest[X*ui32BytesPerPixelDst]);
							(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32BytesPerPixelDst]) | (ui32Final & ui32Sizes[ui32BytesPerPixelDst]);

						}
					}
				}
				break;
			}
		}

		return true;
	}

	/**
	 * Load a BMP file from a file.
	 *
	 * \param _pcFile File from which to load the bitmap data.  Must be in UTF-8 format.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CBmp::LoadBmp( const char * _pcFile,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadBmp( ui8Buffer, static_cast<uint32_t>(uiptrSize),
			_ui32Width, _ui32Height, _pfFormat,
			_tbReturn );
		CFilesEx::FreeFile( ui8Buffer );
		return bRet;
	}

}	// namespace lsi
