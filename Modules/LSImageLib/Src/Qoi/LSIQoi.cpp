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
 * Description: Loading a QOI file.
 */

#include "LSIQoi.h"
#include "HighLevel/LSFFilesEx.h"


namespace lsi {

	// == Functions.
	/**
	 * Load a QOI file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _bSrgb Indicates whether the image is sRGB (if true) or linear (if false).
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CQoi::LoadQoi( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, bool _bSrgb,
		CImageLib::CTexelBuffer &_tbReturn ) {
		// Verify the size of the buffer.
		if ( _ui32DataLen < sizeof( LSI_QOIHEADER ) ) { return false; }
		const LSI_QOIHEADER * lpthHeader = reinterpret_cast<const LSI_QOIHEADER *>(_pui8FileData);
		if ( !CStd::MemCmpF( lpthHeader->ui8Magic, "qoif", 4 ) ) { return false; }
		switch ( lpthHeader->ui8Channels ) {
			case 3 : {
				_pfFormat = LSI_PF_R8G8B8;
				break;
			}
			case 4 : {
				_pfFormat = LSI_PF_R8G8B8A8;
				break;
			}
			default : { return false; }
		}
		switch ( lpthHeader->ui8ColorSpace ) {
			case LSI_QCS_SRGB : {
				_bSrgb = true;
				break;
			}
			case LSI_QCS_LINEAR : {
				_bSrgb = false;
				break;
			}
			default : { return false; }
		}
		_ui32Width = CStd::ByteSwap32( lpthHeader->ui32Width );
		_ui32Height = CStd::ByteSwap32( lpthHeader->ui32Height );
		struct LSI_QOI_RGBA {
			uint8_t ui8R;
			uint8_t ui8G;
			uint8_t ui8B;
			uint8_t ui8A;
		} qrgbaLastRgba = {
				0, 0, 0, 255
		};
		LSI_QOI_RGBA qrgbaIndex[64];
		CStd::MemSet( qrgbaIndex, 0, sizeof( qrgbaIndex ) );

		if ( !_tbReturn.Resize( CImageLib::GetBufferSizeWithPadding( _pfFormat, _ui32Width, _ui32Height ) ) ) { return false; }
		const uint8_t ui8Ending[] = { 0, 0, 0, 0, 0, 0, 0, 1 };
#define LSI_QOI_COLOR_HASH( C )			(C.ui8R * 3 + C.ui8G * 5 + C.ui8B * 7 + C.ui8A * 11)
		uint32_t ui32Pos = sizeof( LSI_QOIHEADER );
		uint32_t ui32MaxTexel = _ui32Width * _ui32Height * lpthHeader->ui8Channels;
		int32_t i32Run = 0;
		for ( uint32_t I = 0; I < ui32MaxTexel; I += lpthHeader->ui8Channels ) {
			if ( i32Run > 0 ) {
				--i32Run;
			}
			else if ( _ui32DataLen - ui32Pos >= sizeof( ui8Ending ) ) {
				uint8_t ui8Mask = _pui8FileData[ui32Pos++];
				if ( ui8Mask == LSI_QM_OP_RGB ) {
					qrgbaLastRgba.ui8R = _pui8FileData[ui32Pos++];
					qrgbaLastRgba.ui8G = _pui8FileData[ui32Pos++];
					qrgbaLastRgba.ui8B = _pui8FileData[ui32Pos++];
				}
				else if ( ui8Mask == LSI_QM_OP_RGBA ) {
					qrgbaLastRgba.ui8R = _pui8FileData[ui32Pos++];
					qrgbaLastRgba.ui8G = _pui8FileData[ui32Pos++];
					qrgbaLastRgba.ui8B = _pui8FileData[ui32Pos++];
					qrgbaLastRgba.ui8A = _pui8FileData[ui32Pos++];
				}
				else if ( (ui8Mask & LSI_QM_MASK_2) == LSI_QM_OP_INDEX ) {
					qrgbaLastRgba = qrgbaIndex[ui8Mask];
				}
				else if ( (ui8Mask & LSI_QM_MASK_2) == LSI_QM_OP_DIFF ) {
					qrgbaLastRgba.ui8R += ((ui8Mask >> 4) & 0x03) - 2;
					qrgbaLastRgba.ui8G += ((ui8Mask >> 2) & 0x03) - 2;
					qrgbaLastRgba.ui8B += ( ui8Mask       & 0x03) - 2;
				}
				else if ( (ui8Mask & LSI_QM_MASK_2) == LSI_QM_OP_LUMA ) {
					int32_t i32B2 = _pui8FileData[ui32Pos++];
					int32_t i32Vg = (ui8Mask & 0x3F) - 32;
					qrgbaLastRgba.ui8R += uint8_t( i32Vg - 8 + ((i32B2 >> 4) & 0x0F) );
					qrgbaLastRgba.ui8G += uint8_t( i32Vg );
					qrgbaLastRgba.ui8B += uint8_t( i32Vg - 8 +  (i32B2       & 0x0F) );
				}
				else if ((ui8Mask & LSI_QM_MASK_2) == LSI_QM_OP_RUN) {
					i32Run = (ui8Mask & 0x3F);
				}
				qrgbaIndex[LSI_QOI_COLOR_HASH(qrgbaLastRgba)%64] = qrgbaLastRgba;
			}
			if ( lpthHeader->ui8Channels == 4 ) {
				_tbReturn[I+3] = qrgbaLastRgba.ui8R;
				_tbReturn[I+2] = qrgbaLastRgba.ui8G;
				_tbReturn[I+1] = qrgbaLastRgba.ui8B;
				_tbReturn[I+0] = qrgbaLastRgba.ui8A;
			}
			else {
				_tbReturn[I+2] = qrgbaLastRgba.ui8R;
				_tbReturn[I+1] = qrgbaLastRgba.ui8G;
				_tbReturn[I+0] = qrgbaLastRgba.ui8B;
			}
		}

		/*while ( ui32Pos < _ui32DataLen ) {
			if ( _ui32DataLen - ui32Pos < 8 ) { return false; }
			uint8_t ui8Byte = _pui8FileData[ui32Pos++];


		}*/

#undef LSI_QOI_COLOR_HASH

		return true;
	}

	/**
	 * Load a QOI file from a file.
	 *
	 * \param _pcFile File from which to load the bitmap data.  Must be in UTF-8 format.
	 * \param _ui32Width The returned width of the image.
	 * \param _ui32Height The returned height of the image.
	 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
	 * \param _bSrgb Indicates whether the image is sRGB (if true) or linear (if false).
	 * \param _tbReturn The returned data.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CQoi::LoadQoi( const char * _pcFile,
		uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, bool _bSrgb,
		CImageLib::CTexelBuffer &_tbReturn ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		LSBOOL bRet = LoadQoi( ui8Buffer, static_cast<uint32_t>(uiptrSize),
			_ui32Width, _ui32Height, _pfFormat, _bSrgb,
			_tbReturn );
		CFilesEx::FreeFile( ui8Buffer );
		return bRet;
	}

}	// namespace lsi
