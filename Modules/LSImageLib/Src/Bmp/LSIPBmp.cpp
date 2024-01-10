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

		if ( pcDetl->ui32MipLevels == 0 ) { return false; }
		if ( pcData->ui32Size < (pcHeader->ui32Width * pcHeader->ui32Height * pcHeader->ui32BitDepth) / 8 ) { return false; }

		return false;
	}


}	// namespace lsi
