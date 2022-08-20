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


#ifndef __LSI_BMP_H__
#define __LSI_BMP_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CBmp
	 * \brief Loading a bitmap file.
	 *
	 * Description: Loading a bitmap file.
	 */
	class CBmp {
	public :
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
		static LSBOOL LSE_CALL			LoadBmp( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );

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
		static LSBOOL LSE_CALL			LoadBmp( const char * _pcFile,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );
		

	protected :
		// == Types.
#pragma pack( push, 1 )
		/** The bitmap file header. */
		typedef struct LSI_BITMAPFILEHEADER {
			uint16_t					ui16Header;
			uint32_t					ui32Size;
			uint16_t					ui16Reserved1;
			uint16_t					ui16Reserved2;
			uint32_t					ui32Offset;
		} * LPLSI_BITMAPFILEHEADER, * const LPCLSI_BITMAPFILEHEADER;

		/** The bitmap info header. */
		typedef struct LSI_BITMAPINFOHEADER {
			uint32_t					ui32InfoSize;
			uint32_t					ui32Width;
			uint32_t					ui32Height;
			uint16_t					ui16Planes;
			uint16_t					ui16BitsPerPixel;
			uint32_t					ui32Compression;
			uint32_t					ui32ImageSize;
			uint32_t					ui32PixelsPerMeterX;
			uint32_t					ui32PixelsPerMeterY;
			uint32_t					ui32ColorsInPalette;
			uint32_t					ui32ImportantColors;
		} * LPLSI_BITMAPINFOHEADER, * const LPCLSI_BITMAPINFOHEADER;

		/** Bitmap color mask. */
		typedef struct LSI_BITMAPCOLORMASK {
			uint32_t					ui32Red,
										ui32Green,
										ui32Blue,
										ui32Alpha;
		} * LPLSI_BITMAPCOLORMASK, * const LPCLSI_BITMAPCOLORMASK;

		/** Bitmap palette data. */
		typedef union LSI_BITMAPPALETTE {
			struct LSI_BM_COLOR {
				uint8_t					ui8Red,
										ui8Green,
										ui8Blue,
										ui8Alpha;
			};
			uint32_t					ui32Color;
		} * LPLSI_BITMAPPALETTE, * const LPCLSI_BITMAPPALETTE;
#pragma pack( pop )
	};

}	// namespace lsi

#endif	// __LSI_BMP_H__
