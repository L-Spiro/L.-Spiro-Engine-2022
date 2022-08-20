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
 * Description: Loading a GIF file.
 */

#include "LSIGif.h"
#include "../LZW/LSILzwDictionary.h"
#include "HighLevel/LSFFilesEx.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lsi {

	// == Functions.
	/**
	 * Load a GIF file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CGif::LoadGif( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		// Verify the size of the buffer.
		if ( _ui32DataLen < sizeof( LSI_GIFHEADER ) ) { return false; }
		const LSI_GIFHEADER * lpghHeader = reinterpret_cast<const LSI_GIFHEADER *>(_pui8FileData);

		if ( !CStd::MemCmpF( "GIF", lpghHeader->ui8Header, sizeof( lpghHeader->ui8Header ) ) ) { return false; }

		uint8_t ui8New;
		if ( CStd::MemCmpF( "87a", lpghHeader->ui8Version, sizeof( lpghHeader->ui8Version ) ) ) { ui8New = false; }
		else if ( CStd::MemCmpF( "89a", lpghHeader->ui8Version, sizeof( lpghHeader->ui8Version ) ) ) { ui8New = true; }
		else { return false; }			// No version match.

		// Get the total global colors.
		uint32_t ui32GlobalColors = 1 << (lpghHeader->ui8SizeOfGct + 1);
		uint32_t ui32SizeOfGlobalColors = ui32GlobalColors * sizeof( LSI_GIFCOLOR );
		if ( _ui32DataLen <= sizeof( LSI_GIFHEADER ) + ui32SizeOfGlobalColors + sizeof( LSI_GIFIMGDESCRIPTOR ) ) { return false; }

		// Keep a pointer to the global color table.
		const LSI_GIFCOLOR * pgcGlobalTable = NULL;
		uint32_t ui32TranspIndex;
		if ( lpghHeader->ui8GctFlag ) {
			pgcGlobalTable = reinterpret_cast<const LSI_GIFCOLOR *>(&_pui8FileData[sizeof( LSI_GIFHEADER )]);
			ui32TranspIndex = LSE_MAXU32;//ui32TranspIndex = lpghHeader->ui8BackGroundIndex;
		}
		else {
			ui32SizeOfGlobalColors = 0;
			ui32TranspIndex = LSE_MAXU32;
		}


		const LSI_GIFIMGDESCRIPTOR * pgidImage;
		if ( !ui8New ) {
			pgidImage = reinterpret_cast<const LSI_GIFIMGDESCRIPTOR *>(&_pui8FileData[sizeof( LSI_GIFHEADER )+ui32SizeOfGlobalColors]);
		}
		else {
			// Otherwise we need to step over some things to get there.
			const uint8_t * pui8Pointer = &_pui8FileData[sizeof( LSI_GIFHEADER )+ui32SizeOfGlobalColors];
			while ( true ) {
				if ( (*pui8Pointer) == 0x2C ) {
					// The descriptor is here.
					pgidImage = reinterpret_cast<const LSI_GIFIMGDESCRIPTOR *>(pui8Pointer);

					// Verify the size again.
					uintptr_t ui32Index = static_cast<uintptr_t>(pui8Pointer - _pui8FileData);
					if ( _ui32DataLen <= ui32Index + sizeof( LSI_GIFIMGDESCRIPTOR ) ) { return false; }
					break;
				}
				else {
					if ( pui8Pointer[0] != 0x21 ) {
						// Not an extension block.  We cannot get derailed, so this is really not a GIF
						//	file.
						return false;
					}
					if ( pui8Pointer[1] == 0xF9 ) {
						// Graphic control extension block.  This is where we find transparency.
						const LSI_GIFGRAPHICCONTROL * pggcControl = reinterpret_cast<const LSI_GIFGRAPHICCONTROL *>(pui8Pointer);
						if ( pggcControl->ui8Trans == 0 ) {
							ui32TranspIndex = pggcControl->ui8TransColor;
						}
					}

					// Otherwise the value after the next tells us how large the section is.
					uint32_t ui32Size = pui8Pointer[1] == 0xFE ?
						4 + 1 :						// Comment label.
						pui8Pointer[2] + 3 + 1;		// Any other label.

					// Verify the size again.
					uintptr_t ui32Index = static_cast<uintptr_t>(pui8Pointer - _pui8FileData);
					if ( _ui32DataLen <= ui32Index + ui32Size ) { return false; }
					// Every section ends with 0x00.
					if ( pui8Pointer[ui32Size-1] != 0x00 ) { return false; }

					pui8Pointer += ui32Size;
				}
			}
		}

		// Get a pointer to the local color table, if any.
		const LSI_GIFCOLOR * pgcLocalTable = NULL;
		uint32_t ui32LocalColors = 1 << (pgidImage->ui8SizeOfLct + 1);
		uint32_t ui32SizeOfLocalColors = ui32LocalColors * sizeof( LSI_GIFCOLOR );
		const uint8_t * pui8CompressedData = reinterpret_cast<const uint8_t *>(pgidImage);
		if ( pgidImage->ui8LocalCt ) {
			const uint8_t * pui8Data = reinterpret_cast<const uint8_t *>(pgidImage);
			pui8Data += sizeof( LSI_GIFIMGDESCRIPTOR );
			pgcLocalTable = reinterpret_cast<const LSI_GIFCOLOR *>(pui8Data);

			if ( _ui32DataLen <= (static_cast<uint32_t>(pui8Data -_pui8FileData)) + ui32SizeOfLocalColors ) { return false; }

			pui8CompressedData += sizeof( LSI_GIFIMGDESCRIPTOR ) + ui32SizeOfLocalColors;
		}
		else {
			ui32SizeOfLocalColors = 0;
			pui8CompressedData += sizeof( LSI_GIFIMGDESCRIPTOR );
		}


		// Which color table do we use?
		// Use local if available.
		const LSI_GIFCOLOR * pgcUseTable = pgcLocalTable ? pgcLocalTable : pgcGlobalTable;
		uint32_t ui32UseColors = pgcLocalTable ? ui32LocalColors : ui32GlobalColors;
		// Temporarily, require that there be one table or the other.
		if ( !pgcUseTable ) { return false; }


		++pui8CompressedData;


		// Ready to become decompressed!
		CLzwDictionary ldDict;
		ldDict.InitializeDictionary( ui32UseColors, 12 );

		uint32_t ui32FullSize = pgidImage->ui16W * pgidImage->ui16H;
		uint8_t * pui8DecompressBuffer = static_cast<uint8_t *>(CMemLib::MAlloc( sizeof( uint8_t ) * (ui32FullSize) ));
		uint32_t ui32DecompressIndex = 0;
		if ( !pui8DecompressBuffer ) {
			return false;
		}

		// Buffer for decompressed strings.
		// Increase the size if the assert() below ever triggers.
		uint8_t ui8DecompressedStrings[1024];


		uint32_t ui32Old = LSE_MAXU32;
		
		uint64_t ui64Bit = 0ULL;

		// Check that there is enough of the file loaded.
		if ( static_cast<uint32_t>(pui8CompressedData - _pui8FileData) + 1 >= _ui32DataLen ) {
			CMemLib::Free( pui8DecompressBuffer );
			return false;
		}
		uint32_t ui32BlockSize = (*pui8CompressedData++);
		if ( ui32BlockSize == 0 ) {
			// First block should never be 0-sized.
			// If it is, there is no image data anyway!
			CMemLib::Free( pui8DecompressBuffer );
			return false;
		}
		

		if ( static_cast<uint32_t>(pui8CompressedData - _pui8FileData) + ui32BlockSize >= _ui32DataLen ) {
			CMemLib::Free( pui8DecompressBuffer );
			return false;
		}

		

		uint32_t ui32Char = ui32Old;
		while ( true ) {
			uint32_t ui32BitsToread = ldDict.GetBitsNeeded();
			uint32_t ui32Value;
			if ( (ui64Bit + ui32BitsToread) > (ui32BlockSize << 3) ) {
				// We will cross a sub-block boundary by reading these bits.
				// We need to read part of the current block and part of the next block.
				uint8_t ui8Temp[4];
				ui8Temp[0] = pui8CompressedData[ui32BlockSize-2];	// Second-to-last byte of this block.
				ui8Temp[1] = pui8CompressedData[ui32BlockSize-1];	// Last byte of this block.
				pui8CompressedData += ui32BlockSize;				// Go to the start of the next block.
				// Verify the next block.
				// Can we read the next byte?
				if ( static_cast<uint32_t>(pui8CompressedData - _pui8FileData) + 1 >= _ui32DataLen ) {
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}

				uint32_t ui32OldBlockSize = ui32BlockSize;
				ui32BlockSize = (*pui8CompressedData++);
				if ( ui32BlockSize == 0 ) { break; }				// End of all blocks.
				

				// Not the end of the blocks.  Can we read the entire next block?
				if ( static_cast<uint32_t>(pui8CompressedData - _pui8FileData) + ui32BlockSize >= _ui32DataLen ) {
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}

				// Get the next byte.
				ui8Temp[2] = pui8CompressedData[0];
				ui8Temp[3] = pui8CompressedData[1];

				// Read from this buffer.
				ui64Bit = (ui64Bit - ((ui32OldBlockSize - 2) << 3));
				ui32Value = CLzwDictionary::GetBits( ui8Temp, ui64Bit, ui32BitsToread );
				ui64Bit -= 16ULL;
			}
			else {
				// Get the next code.
				ui32Value = CLzwDictionary::GetBits( pui8CompressedData, ui64Bit, ui32BitsToread );
			}

			if ( ldDict.IsEndOfInputCode( static_cast<CLzwDictionary::LSI_LZW_CODE>(ui32Value) ) ) {
				break;
			}
			else if ( ldDict.IsClearCode( static_cast<CLzwDictionary::LSI_LZW_CODE>(ui32Value) ) ) {
				ldDict.InitializeDictionary( ui32UseColors, 12 );
				ui32Old = LSE_MAXU32;
				continue;
			}

			uint32_t ui32Size;
			if ( ui32Value >= ldDict.GetTotal() ) {
				if ( ui32Old == LSE_MAXU32 ) {
					// Unable to handle this case.  The old code is invalid.
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				if ( ui32Value != ldDict.GetTotal() ) {
					// The unknown code must be the next code we will add.
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				// Code not found in the dictionary.
				ui32Size = ldDict.GetString( static_cast<CLzwDictionary::LSI_LZW_CODE>(ui32Old), ui8DecompressedStrings );
				if ( ui32Size == LSE_MAXU32 ) {
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				assert( ui32Size < sizeof( ui8DecompressedStrings ) );

				// Output the string followed by the current character.
				if ( ui32DecompressIndex + ui32Size >= ui32FullSize ) {
					// Overrunning our buffer.  Decoded too much.
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				for ( uint32_t I = ui32Size; I--; ) {
					pui8DecompressBuffer[ui32DecompressIndex++] = ui8DecompressedStrings[I];
				}
				pui8DecompressBuffer[ui32DecompressIndex++] = static_cast<uint8_t>(ui32Char);
			}
			else {
				// It was in the table.
				// Output its string normally.
				ui32Size = ldDict.GetString( static_cast<CLzwDictionary::LSI_LZW_CODE>(ui32Value), ui8DecompressedStrings );
				if ( ui32Size == LSE_MAXU32 ) {
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				assert( ui32Size < sizeof( ui8DecompressedStrings ) );

				// Output the string.
				if ( ui32DecompressIndex + ui32Size > ui32FullSize ) {
					// Overrunning our buffer.  Decoded too much.
					CMemLib::Free( pui8DecompressBuffer );
					return false;
				}
				for ( uint32_t I = ui32Size; I--; ) {
					if ( ui32DecompressIndex == ui32FullSize ) {
						// Overrunning our buffer.  Decoded too much.
						CMemLib::Free( pui8DecompressBuffer );
						return false;
					}
					pui8DecompressBuffer[ui32DecompressIndex++] = ui8DecompressedStrings[I];
				}
			}

			ui32Char = ui8DecompressedStrings[ui32Size-1];
			if ( ui32Old != LSE_MAXU32 ) {
				ldDict.AddCode( static_cast<CLzwDictionary::LSI_LZW_CODE>(ui32Old), static_cast<uint8_t>(ui32Char) );
			}
			ui32Old = ui32Value;

			if ( ui32DecompressIndex == ui32FullSize ) { break; }
		}

		// Verify that the exact amount of data was decompressed.
		if ( ui32DecompressIndex != ui32FullSize ) {
			CMemLib::Free( pui8DecompressBuffer );
			return false;
		}

		// By this point we are almost guaranteed success.  Failure beyond this point is only theoretical.

		// We can give the data back in exactly the right format.
		LSI_PIXEL_FORMAT pfOurFormat = _pfFormat;
		if ( pfOurFormat == LSI_PF_ANY ) {
			pfOurFormat = _pfFormat = LSI_PF_R8G8B8A8;
		}
		_ui32Width = pgidImage->ui16W;
		_ui32Height = pgidImage->ui16H;

		// Convert the palette.
		uint32_t ui32Colors[256];
		for ( uint32_t I = ui32UseColors; I--; ) {
			ui32Colors[I] = (I == ui32TranspIndex) ? LSI_MAKE_RGBA( pgcUseTable[I].ui8R, pgcUseTable[I].ui8G, pgcUseTable[I].ui8B,
				0x00 ) :
				LSI_MAKE_RGBX( pgcUseTable[I].ui8R, pgcUseTable[I].ui8G, pgcUseTable[I].ui8B );
			// Convert to the desired format.
			ui32Colors[I] = static_cast<uint32_t>(CImageLib::IntegerTexelToIntegerTexel( LSI_PF_R8G8B8A8, pfOurFormat, ui32Colors[I], LSI_G_NONE ));
		}


		if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( pfOurFormat, _ui32Width, _ui32Height ) ) ) {
			// Oops!  The image loaded but we ran out of RAM!
			CMemLib::Free( pui8DecompressBuffer );
			return false;
		}
		CStd::MemSet( &_tbReturn[0], 0, _tbReturn.Length() );

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

		uint32_t ui32DstPixelSize = CImageLib::GetFormatSize( _pfFormat );
		for ( uint32_t Y = 0; Y < pgidImage->ui16H; ++Y ) {
			uint32_t ui32YOff = Y * _ui32Width;
			uint32_t ui32YDstOff = Y * CImageLib::GetRowSize( _pfFormat, _ui32Width );
			for ( uint32_t X = 0; X < pgidImage->ui16W; ++X ) {
				// Only way to fail is if the color index is out of range, which is highly unlikely by this point.
				uint32_t ui32Index = pui8DecompressBuffer[ui32YOff+X];
				if ( ui32Index >= ui32UseColors ) {
					// Wow!  But this will never actually happen.
					CMemLib::Free( pui8DecompressBuffer );
					_tbReturn.Reset();
					return false;
				}
				uint32_t ui32Final = ui32Colors[pui8DecompressBuffer[ui32YOff+X]];
				uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&_tbReturn[ui32YDstOff+X*ui32DstPixelSize]);
				(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32DstPixelSize]) | (ui32Final & ui32Sizes[ui32DstPixelSize]);
			}
		}


		CMemLib::Free( pui8DecompressBuffer );
		return true;
	}

	/**
	 * Load a GIF file from a file.
	 *
	 * \param _pcFile File from which to load the bitmap data.  Must be in UTF-8 format.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CGif::LoadGif( const char * _pcFile,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
		CImageLib::CTexelBuffer &_tbReturn ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadGif( ui8Buffer, static_cast<uint32_t>(uiptrSize),
			_ui32Width, _ui32Height, _pfFormat,
			_tbReturn );
		CFilesEx::FreeFile( ui8Buffer );
		return bRet;
	}

}	// namespace lsi

#pragma warning( pop )
