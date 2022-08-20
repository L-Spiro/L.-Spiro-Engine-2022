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
 * Description: Loading a TGA file.
 */

#include "LSITga.h"
#include "HighLevel/LSFFilesEx.h"


namespace lsi {

	// == Functions.
	/**
	 * Load a TGA file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CTga::LoadTga( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		// Verify the size of the buffer.
		if ( _ui32DataLen < sizeof( LSI_TGAHEADER ) ) { return false; }
		const LSI_TGAHEADER * lpthHeader = reinterpret_cast<const LSI_TGAHEADER *>(_pui8FileData);

		// Check the footer.
		if ( _ui32DataLen < sizeof( LSI_TGAFOOTER ) ) { return false; }
		/*	// Comment in if headers should be checked for verification.
		const LSI_TGAFOOTER * lptfFooter = reinterpret_cast<const LSI_TGAFOOTER *>(&_pui8FileData[_ui32DataLen-sizeof( LSI_TGAFOOTER )]);
		// Some valid files (for some reason) omit the tailing 0x00.  Try both.
		if ( lptfFooter->ui8Pad1[0] != 0x2E ) {
			lptfFooter = reinterpret_cast<const LSI_TGAFOOTER *>(reinterpret_cast<uintptr_t>(lptfFooter) + 1);
			if ( lptfFooter->ui8Pad1[0] != 0x2E ) {
				return false;
			}
		}
		else if ( lptfFooter->ui8Pad1[1] != 0x00 ) { return false; }
		// For security reasons, we only allow the new TGA format.
		if ( !CStd::MemCmpF( "TRUEVISION-XFILE", lptfFooter->ui8Id, sizeof( lptfFooter->ui8Id ) ) ) { return false; }
		*/

		// Verify image type.
		if ( lpthHeader->ui8ColorMapType > 1 ) { return false; }
		if ( lpthHeader->ui8ImageType == 0 || lpthHeader->ui8ImageType > 11 ) { return false; }

		if ( lpthHeader->ui8ColorMapType == 0 ) {
			/*if ( lpthHeader->i16ColorMapStart != 0 ) { return false; }
			if ( lpthHeader->i16ColorMapLength != 0 ) { return false; }*/
		}
		else {
			if ( lpthHeader->i16ColorMapLength == 0 ) { return false; }
			switch ( lpthHeader->ui8ColorMapBits ) {
				case 8 : {}
				case 15 : {}
				case 16 : {}
				case 24 : {}
				case 32 : { break; }
				default : { return false; }
			}
		}

		switch ( lpthHeader->ui8Bits ) {
			case 8 : {}
			case 16 : {}
			case 24 : {}
			case 32 : { break; }
			default : { return false; }
		}

		if ( lpthHeader->ui8DescPad != 0 ) { return false; }


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

		// Parse the data.
		if ( lpthHeader->ui8ImageType == 1 ) {
			// Uncompressed indexed data.
			uint32_t ui32TableSize = static_cast<uint32_t>((lpthHeader->i16ColorMapLength * lpthHeader->ui8ColorMapBits) >> 3);
			if ( _ui32DataLen < static_cast<uint32_t>(lpthHeader->i16Width * lpthHeader->i16Height *
				(lpthHeader->ui8Bits >> 3)) +
				ui32TableSize ) { return false; }

			uint32_t ui32ActualOffset = sizeof( LSI_TGAHEADER ) + lpthHeader->ui8IdentSize;

			// Make a pointer to the color map.
			const uint8_t * pui8ColorMap = reinterpret_cast<const uint8_t *>(&_pui8FileData[ui32ActualOffset]);

			


			_ui32Width = static_cast<uint16_t>(lpthHeader->i16Width);
			_ui32Height = static_cast<uint16_t>(lpthHeader->i16Height);
			LSI_PIXEL_FORMAT pfSrcFormat;
			uint32_t ui32AlphaBits = lpthHeader->ui8DescAlpha;
			switch ( lpthHeader->ui8ColorMapBits ) {
				case 8 : {
					_pfFormat = pfSrcFormat = LSI_PF_R3G3B2;
					ui32AlphaBits = 0;
					break;
				}
				case 16 : {
					_pfFormat = LSI_PF_R5G6B5;
					pfSrcFormat = LSI_PF_R5G5B5A1;
					ui32AlphaBits = 1;
					break;
				}
				case 24 : {
					_pfFormat = LSI_PF_R8G8B8;
					pfSrcFormat = LSI_PF_R8G8B8;
					ui32AlphaBits = 0;
					break;
				}
				case 32 : {
					_pfFormat = LSI_PF_R8G8B8A8;
					pfSrcFormat = LSI_PF_R8G8B8A8;
					ui32AlphaBits = 8;
					break;
				}
				default : { return false; }
			}

			uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );
			uint32_t ui32SrcPixelSize = CImageLib::GetFormatSize( pfSrcFormat );
			uint32_t ui32Shift = ui32SrcPixelSize * 8 - ui32AlphaBits;
			uint32_t ui32AlphaMask = ~(~((1 << ui32AlphaBits) - 1));
			uint32_t ui32SrcIndexSize = (lpthHeader->ui8Bits >> 3);

			// Final check.
			if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
			CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );
			

			// Recompile the color map.
			uint8_t * pui8NewColorMap = static_cast<uint8_t *>(CMemLib::MAlloc( (ui32DstPixelSize * static_cast<uint16_t>(lpthHeader->i16ColorMapLength) * lpthHeader->ui8ColorMapBits) >> 3 ));
			if ( !pui8ColorMap ) {
				_tbReturn.Reset();
				return false;
			}

			for ( uint32_t I = 0; I < static_cast<uint32_t>(lpthHeader->i16ColorMapLength); ++I ) {
				uint32_t ui32ThisColor = (*reinterpret_cast<const uint32_t *>(&pui8ColorMap[I*ui32SrcPixelSize]));
				// If there are any alpha bits, move them to the bottom from the top.
				if ( ui32AlphaBits ) {
					uint32_t ui32Alpha = ui32ThisColor >> ui32Shift;
					ui32Alpha &= ui32AlphaMask;

					ui32ThisColor <<= ui32AlphaBits;
					ui32ThisColor = (ui32ThisColor & ~ui32AlphaMask) | ui32Alpha;
				}

				uint32_t ui32Final = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( pfSrcFormat, _pfFormat, ui32ThisColor, LSI_G_NONE ));

				uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8NewColorMap[I*ui32DstPixelSize]);
				(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);
			}

			// After this the source and destination sizes are the same.
			ui32SrcPixelSize = ui32DstPixelSize;

			ui32ActualOffset += ui32TableSize;
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				uint32_t ui32YOffSrc = Y * _ui32Width * ui32SrcIndexSize + ui32ActualOffset;

				uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
					(_ui32Height - Y - 1) * ui32DestRowWidth :
					Y * ui32DestRowWidth;
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
						(_ui32Width - X - 1) * ui32DstPixelSize :
						X * ui32DstPixelSize;

					uint32_t ui32Index = (*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32YOffSrc+X*ui32SrcIndexSize]));
					ui32Index &= (1 << lpthHeader->ui8Bits) - 1;

					ui32Index -= static_cast<uint16_t>(lpthHeader->i16ColorMapStart);
					if ( ui32Index >= static_cast<uint32_t>(lpthHeader->i16ColorMapLength) ) {
						_tbReturn.Reset();
						CMemLib::Free( pui8NewColorMap );
						return false;
					}

					uint32_t ui32Final = (*reinterpret_cast<const uint32_t *>(&pui8NewColorMap[ui32Index*ui32SrcPixelSize]));

					uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
					(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);
				}
			}
			CMemLib::Free( pui8NewColorMap );
		}
		else if ( lpthHeader->ui8ImageType == 2 ) {
			// Uncompressed raw data.
			if ( _ui32DataLen < static_cast<uint32_t>(lpthHeader->i16Width * lpthHeader->i16Height *
				(lpthHeader->ui8Bits >> 3)) ) { return false; }

			_ui32Width = static_cast<uint16_t>(lpthHeader->i16Width);
			_ui32Height = static_cast<uint16_t>(lpthHeader->i16Height);
			LSI_PIXEL_FORMAT pfSrcFormat;
			uint32_t ui32AlphaBits = lpthHeader->ui8DescAlpha;
			switch ( lpthHeader->ui8Bits ) {
				case 8 : {
					_pfFormat = pfSrcFormat = LSI_PF_R3G3B2;
					ui32AlphaBits = 0;
					break;
				}
				case 16 : {
					_pfFormat = LSI_PF_R5G6B5;
					pfSrcFormat = LSI_PF_R5G5B5A1;
					ui32AlphaBits = 1;
					break;
				}
				case 24 : {
					_pfFormat = LSI_PF_R8G8B8;
					pfSrcFormat = LSI_PF_R8G8B8;
					ui32AlphaBits = 0;
					break;
				}
				case 32 : {
					_pfFormat = LSI_PF_R8G8B8A8;
					pfSrcFormat = LSI_PF_R8G8B8A8;
					ui32AlphaBits = 8;
					break;
				}
				default : { return false; }
			}

			uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );
			uint32_t ui32SrcPixelSize = CImageLib::GetFormatSize( pfSrcFormat );
			uint32_t ui32Shift = ui32SrcPixelSize * 8 - ui32AlphaBits;
			uint32_t ui32AlphaMask = ~(~((1 << ui32AlphaBits) - 1));

			// Final check.
			if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
			CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );

			uint32_t ui32ActualOffset = sizeof( LSI_TGAHEADER ) + lpthHeader->ui8IdentSize;
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				uint32_t ui32YOffSrc = Y * _ui32Width * ui32SrcPixelSize + ui32ActualOffset;

				uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
					(_ui32Height - Y - 1) * ui32DestRowWidth :
					Y * ui32DestRowWidth;
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
						(_ui32Width - X - 1) * ui32DstPixelSize :
						X * ui32DstPixelSize;

					uint32_t ui32ThisColor = (*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32YOffSrc+X*ui32SrcPixelSize]));
					// If there are any alpha bits, move them to the bottom from the top.
					if ( ui32AlphaBits ) {
						uint32_t ui32Alpha = ui32ThisColor >> ui32Shift;
						ui32Alpha &= ui32AlphaMask;

						ui32ThisColor <<= ui32AlphaBits;
						ui32ThisColor = (ui32ThisColor & ~ui32AlphaMask) | ui32Alpha;
					}

					uint32_t ui32Final = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( pfSrcFormat, _pfFormat, ui32ThisColor, LSI_G_NONE ));

					uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
					(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);
				}
			}
		}
		else if ( lpthHeader->ui8ImageType == 3 ) {
			// Uncompressed black-and-white data.
			if ( _ui32DataLen < static_cast<uint32_t>(lpthHeader->i16Width * lpthHeader->i16Height *
				(lpthHeader->ui8Bits >> 3)) ) { return false; }

			_ui32Width = static_cast<uint16_t>(lpthHeader->i16Width);
			_ui32Height = static_cast<uint16_t>(lpthHeader->i16Height);
			_pfFormat = LSI_PF_R8G8B8;
			uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );
			// Final check.
			if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
			CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );

			uint32_t ui32ActualOffset = sizeof( LSI_TGAHEADER ) + lpthHeader->ui8IdentSize;
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				uint32_t ui32YOffSrc = Y * _ui32Width + ui32ActualOffset;

				uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
					(_ui32Height - Y - 1) * ui32DestRowWidth :
					Y * ui32DestRowWidth;
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
						(_ui32Width - X - 1) * ui32DstPixelSize :
						X * ui32DstPixelSize;

					uint32_t ui32ThisColor = _pui8FileData[ui32YOffSrc+X];
					

					uint32_t ui32Final = (ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R )) |
						(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G )) |
						(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B ));

					uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
					(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);
				}
			}
		}
		else if ( lpthHeader->ui8ImageType == 10 ) {
			// Run-length encoding.

			_ui32Width = static_cast<uint16_t>(lpthHeader->i16Width);
			_ui32Height = static_cast<uint16_t>(lpthHeader->i16Height);
			LSI_PIXEL_FORMAT pfSrcFormat;
			uint32_t ui32AlphaBits = lpthHeader->ui8DescAlpha;
			switch ( lpthHeader->ui8Bits ) {
				case 8 : {
					_pfFormat = pfSrcFormat = LSI_PF_R3G3B2;
					ui32AlphaBits = 0;
					break;
				}
				case 16 : {
					_pfFormat = LSI_PF_R5G6B5;
					pfSrcFormat = LSI_PF_R5G5B5A1;
					ui32AlphaBits = 1;
					break;
				}
				case 24 : {
					_pfFormat = LSI_PF_R8G8B8;
					pfSrcFormat = LSI_PF_R8G8B8;
					ui32AlphaBits = 0;
					break;
				}
				case 32 : {
					_pfFormat = LSI_PF_R8G8B8A8;
					pfSrcFormat = LSI_PF_R8G8B8A8;
					ui32AlphaBits = 8;
					break;
				}
				default : { return false; }
			}

			uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );
			uint32_t ui32SrcPixelSize = CImageLib::GetFormatSize( pfSrcFormat );
			uint32_t ui32Shift = ui32SrcPixelSize * 8 - ui32AlphaBits;
			uint32_t ui32AlphaMask = ~(~((1 << ui32AlphaBits) - 1));

			// Final check.
			if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
			CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );

			uint32_t ui32ActualOffset = sizeof( LSI_TGAHEADER ) + lpthHeader->ui8IdentSize;

			uint32_t ui32Total = _ui32Width * _ui32Height;
			uint32_t ui32OffsetIntoData = 0;
			uint32_t ui32RawTotalSums = 0;
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t I = 0; I < ui32Total; ) {
				uint8_t ui8RowType = static_cast<uint8_t>((*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32ActualOffset+ui32OffsetIntoData++])));
				uint32_t ui32RawTotal = (ui8RowType & 0x7F) + 1;

				if ( ui32RawTotalSums + ui32RawTotal > ui32Total ) {
					_tbReturn.Reset();
					return false;
				}

				if ( (ui8RowType & 0x80) == 0 ) {
					// Raw pixels.

					for ( uint32_t J = 0; J < ui32RawTotal; ++J ) {
						uint32_t ui32X = I % _ui32Width;
						uint32_t ui32Y = I / _ui32Width;
						uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
							(_ui32Height - ui32Y - 1) * ui32DestRowWidth :
							ui32Y * ui32DestRowWidth;
						uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
							(_ui32Width - ui32X - 1) * ui32DstPixelSize :
							ui32X * ui32DstPixelSize;

						uint32_t ui32ThisColor = (*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32ActualOffset+ui32OffsetIntoData]));
						// If there are any alpha bits, move them to the bottom from the top.
						if ( ui32AlphaBits ) {
							uint32_t ui32Alpha = ui32ThisColor >> ui32Shift;
							ui32Alpha &= ui32AlphaMask;

							ui32ThisColor <<= ui32AlphaBits;
							ui32ThisColor = (ui32ThisColor & ~ui32AlphaMask) | ui32Alpha;
						}


						uint32_t ui32Final = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( pfSrcFormat, _pfFormat, ui32ThisColor, LSI_G_NONE ));

						uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
						(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);

						++I;
						ui32OffsetIntoData += ui32SrcPixelSize;
					}
				}
				else {
					// Use the next color repeatedly.

					// Get the next color.
					uint32_t ui32ThisColor = (*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32ActualOffset+ui32OffsetIntoData]));
					// If there are any alpha bits, move them to the bottom from the top.
					if ( ui32AlphaBits ) {
						uint32_t ui32Alpha = ui32ThisColor >> ui32Shift;
						ui32Alpha &= ui32AlphaMask;

						ui32ThisColor <<= ui32AlphaBits;
						ui32ThisColor = (ui32ThisColor & ~ui32AlphaMask) | ui32Alpha;
					}

					uint32_t ui32Final = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( pfSrcFormat, _pfFormat, ui32ThisColor, LSI_G_NONE ));

					ui32OffsetIntoData += ui32SrcPixelSize;

					// Add it over and over.
					for ( uint32_t J = 0; J < ui32RawTotal; ++J ) {
						uint32_t ui32X = I % _ui32Width;
						uint32_t ui32Y = I / _ui32Width;
						uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
							(_ui32Height - ui32Y - 1) * ui32DestRowWidth :
							ui32Y * ui32DestRowWidth;
						uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
							(_ui32Width - ui32X - 1) * ui32DstPixelSize :
							ui32X * ui32DstPixelSize;

						uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
						(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);

						++I;
					}
				}
				ui32RawTotalSums += ui32RawTotal;
			}
		}
		else if ( lpthHeader->ui8ImageType == 11 ) {
			// Run-length encoding, black and white.
			_ui32Width = static_cast<uint16_t>(lpthHeader->i16Width);
			_ui32Height = static_cast<uint16_t>(lpthHeader->i16Height);
			_pfFormat = LSI_PF_R8G8B8;
			uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );

			// Final check.
			if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
			CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );

			uint32_t ui32ActualOffset = sizeof( LSI_TGAHEADER ) + lpthHeader->ui8IdentSize;

			uint32_t ui32Total = _ui32Width * _ui32Height;
			uint32_t ui32OffsetIntoData = 0;
			uint32_t ui32RawTotalSums = 0;
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t I = 0; I < ui32Total; ) {
				uint8_t ui8RowType = static_cast<uint8_t>((*reinterpret_cast<const uint32_t *>(&_pui8FileData[ui32ActualOffset+ui32OffsetIntoData++])));
				uint32_t ui32RawTotal = (ui8RowType & 0x7F) + 1;

				if ( ui32RawTotalSums + ui32RawTotal > ui32Total ) {
					_tbReturn.Reset();
					return false;
				}

				if ( (ui8RowType & 0x80) == 0 ) {
					// Raw pixels.
					for ( uint32_t J = 0; J < ui32RawTotal; ++J ) {
						uint32_t ui32X = I % _ui32Width;
						uint32_t ui32Y = I / _ui32Width;
						uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
							(_ui32Height - ui32Y - 1) * ui32DestRowWidth :
							ui32Y * ui32DestRowWidth;
						uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
							(_ui32Width - ui32X - 1) * ui32DstPixelSize :
							ui32X * ui32DstPixelSize;

						uint32_t ui32ThisColor = _pui8FileData[ui32ActualOffset+ui32OffsetIntoData];

						uint32_t ui32Final = (ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R )) |
							(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G )) |
							(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B ));

						uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
						(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);

						++I;
						++ui32OffsetIntoData;
					}
				}
				else {
					// Use the next color repeatedly.

					// Get the next color.
					uint32_t ui32ThisColor = _pui8FileData[ui32ActualOffset+ui32OffsetIntoData];

					uint32_t ui32Final = (ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R )) |
						(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G )) |
						(ui32ThisColor << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B ));

					++ui32OffsetIntoData;

					// Add it over and over.
					for ( uint32_t J = 0; J < ui32RawTotal; ++J ) {
						uint32_t ui32X = I % _ui32Width;
						uint32_t ui32Y = I / _ui32Width;
						uint32_t ui32YOffDst = !lpthHeader->ui8DescFlipY ?
							(_ui32Height - ui32Y - 1) * ui32DestRowWidth :
							ui32Y * ui32DestRowWidth;
						uint32_t ui32DstX = lpthHeader->ui8DescFlipX ?
							(_ui32Width - ui32X - 1) * ui32DstPixelSize :
							ui32X * ui32DstPixelSize;

						uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YOffDst+ui32DstX]);
						(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);

						++I;
					}
				}
				ui32RawTotalSums += ui32RawTotal;
			}
		}
		else {
			// Unknown format.
			return false;
		}
		return true;
	}

	/**
	 * Load a TGA file from a file.
	 *
	 * \param _pcFile File from which to load the bitmap data.  Must be in UTF-8 format.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CTga::LoadTga( const char * _pcFile,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadTga( ui8Buffer, static_cast<uint32_t>(uiptrSize),
			_ui32Width, _ui32Height, _pfFormat,
			_tbReturn );
		CFilesEx::FreeFile( ui8Buffer );
		return bRet;
	}

}	// namespace lsi
