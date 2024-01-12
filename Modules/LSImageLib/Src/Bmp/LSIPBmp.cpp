/**
 * Copyright L. Spiro 2024
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Loading a Starsiege: Tribes Phoenix bitmap file.
 */

#include "LSIPBmp.h"


namespace lsi {

	// == Functions.
	/**
	 * Load a PBMP file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _pdPalettes A database of palettes that can be referenced by ID to provide color information for the BMP result.
	 * \param _tbReturn The returned data.
	 * \param _vMipMaps The returned mipmap data, if any.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CPBmp::LoadPBmp( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, const CPaletteDatabase &_pdPalettes,
		CImageLib::CTexelBuffer &_tbReturn,
		CVector<CImageLib::CTexelBuffer> &_vMipMaps ) {

		if ( _ui32DataLen < sizeof( LSI_CHUNK ) ) { return false; }
		const LSI_CHUNK_EX * pcFile = reinterpret_cast<const LSI_CHUNK_EX *>(_pui8FileData);
		if ( pcFile->ui32Name != 0x504D4250 ) { return false; }														// PBMP.
		if ( pcFile->ui32Size > _ui32DataLen ) { return false; }

		_pui8FileData += sizeof( LSI_CHUNK );
		_ui32DataLen -= sizeof( LSI_CHUNK );

		const LSI_HEAD_CHUNK * pcHeader = NULL;
		const LSI_CHUNK_EX * pcData = NULL;
		const LSI_DETL_CHUNK * pcDetl = NULL;
		const LSI_PIDX_CHUNK * pcPidX = NULL;
		const LSI_CHUNK_EX * pcRiff = NULL;
		bool bContinue = true;
		while ( _ui32DataLen >= sizeof( LSI_CHUNK ) && bContinue ) {
			const LSI_CHUNK * pcThis = reinterpret_cast<const LSI_CHUNK *>(_pui8FileData);
			if ( (sizeof( LSI_CHUNK ) + pcThis->ui32Size) > _ui32DataLen ) { return false; }
			switch ( pcThis->ui32Name ) {
				case 0x64616568 : {																					// head.
					pcHeader = reinterpret_cast<const LSI_HEAD_CHUNK *>(_pui8FileData);
					break;
				}
				case 0x61746164 : {																					// data.
					pcData = reinterpret_cast<const LSI_CHUNK_EX *>(_pui8FileData);
					break;
				}
				case 0x4C544544 : {																					// DETL.
					pcDetl = reinterpret_cast<const LSI_DETL_CHUNK *>(_pui8FileData);
					break;
				}
				case 0x58446950 : {																					// PiDX.
					pcPidX = reinterpret_cast<const LSI_PIDX_CHUNK *>(_pui8FileData);
					break;
				}
				default : {
					bContinue = false;
					break;
				}
			}
			_pui8FileData += sizeof( LSI_CHUNK ) + pcThis->ui32Size;
			_ui32DataLen -= sizeof( LSI_CHUNK ) + pcThis->ui32Size;
		}

		if ( pcHeader == NULL ) { return false; }
		if ( pcDetl == NULL ) { return false; }
		if ( pcPidX == NULL ) { return false; }
		if ( pcData == NULL ) { return false; }

		const CPalette * ppPal = _pdPalettes.PaletteById( pcPidX->ui32PalIdx );
		if ( ppPal == NULL ) { return false; }

		
		switch ( pcHeader->ui32BitDepth ) {
			case 8 : {
				_pfFormat = LSI_PF_R8G8B8A8;
				break;
			}
			default : { return false; }
		}

		uint32_t ui32Mips = 1;
		if ( pcDetl->ui32MipLevels == 0 ) { return false; }
		if ( pcDetl->ui32MipLevels > 1 ) {
			uint32_t ui32MaxMipsW = static_cast<uint32_t>(std::round( std::log2( pcHeader->ui32Width ) + 1.0 ));
			uint32_t ui32MaxMipsH = static_cast<uint32_t>(std::round( std::log2( pcHeader->ui32Height ) + 1.0 ));
			ui32Mips = CStd::Min( CStd::Min( ui32MaxMipsW, ui32MaxMipsH ), pcDetl->ui32MipLevels );
			if ( !_vMipMaps.Resize( ui32Mips - 1 ) ) { return false; }
		}


		uint32_t ui32BytesPerPixelDst = CImageLib::GetFormatSize( _pfFormat );
		const uint8_t * pui8Src = pcData->ui8Data;
		uint32_t ui32SizeRem = pcData->ui32Size;
		for ( uint32_t I = 0; I < ui32Mips; ++I ) {
			uint32_t ui32W = pcHeader->ui32Width >> I;
			if ( !ui32W ) { return false; }
			uint32_t ui32H = pcHeader->ui32Height >> I;
			if ( !ui32H ) { return false; }
			uint32_t ui32ThisSize = ((ui32W * ui32H * pcHeader->ui32BitDepth >> 3) + 3) & ~3;

			CImageLib::CTexelBuffer & tbDest = (I == 0) ? _tbReturn : _vMipMaps[I-1];
			if ( ui32SizeRem < ui32ThisSize ) { return false; }

			if ( !tbDest.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, ui32W, ui32H ) ) ) { return false; }
			CStd::MemSet( &tbDest[0], 0, tbDest.Length() );
			uint32_t ui32DestRowWidth = CImageLib::GetRowSize( _pfFormat, ui32W );

			constexpr bool bReverse = true;
			switch ( pcHeader->ui32BitDepth ) {
				case 8 : {
					for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
						uint32_t ui32YOff = Y * ui32W;

						// This line is what handles reverse-encoded bitmaps.
						uint32_t ui32YOffDest = bReverse ? Y * ui32DestRowWidth :
							(ui32H - Y - 1) * ui32DestRowWidth;

						uint8_t * pui8Dest = &tbDest[ui32YOffDest];

						

						for ( uint32_t X = 0; X < ui32W; ++X ) {
							const CPalette::LSI_PALETTE_ENTRY peEntry = ppPal->Get( pui8Src[ui32YOff+X] );

							uint32_t ui32Color = (peEntry.perRgba.ui8R << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_R )) |
								(peEntry.perRgba.ui8G << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_G )) |
								(peEntry.perRgba.ui8B << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_B )) |
								(peEntry.perRgba.ui8A << CImageLib::GetComponentOffset( _pfFormat, LSI_PC_A ));

							uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8Dest[X*ui32BytesPerPixelDst]);
							(*pui32Dst) = ui32Color;
						}
					}
					break;
				}
				default : { return false; }
			}


			pui8Src += ui32ThisSize;
			ui32SizeRem -= ui32ThisSize;
		}
		_ui32Width = pcHeader->ui32Width;
		_ui32Height = pcHeader->ui32Height;
		return true;
	}


}	// namespace lsi
