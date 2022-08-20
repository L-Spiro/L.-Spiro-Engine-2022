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


#ifndef __LSI_TGA_H__
#define __LSI_TGA_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CTga
	 * \brief Loading a TGA file.
	 *
	 * Description: Loading a TGA file.
	 */
	class CTga {
	public :
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
		static LSBOOL LSE_CALL			LoadTga( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );

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
		static LSBOOL LSE_CALL			LoadTga( const char * _pcFile,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );


	protected :
		// == Types.
#pragma pack( push, 1 )
		/**
		 * The TGA file header.
		 */
		typedef struct LSI_TGAHEADER {
			uint8_t						ui8IdentSize;
			uint8_t						ui8ColorMapType;
			uint8_t						ui8ImageType;

			int16_t						i16ColorMapStart;
			int16_t						i16ColorMapLength;
			uint8_t						ui8ColorMapBits;

			int16_t						i16XStart,
										i16YStart,
										i16Width,
										i16Height;

			uint8_t						ui8Bits;
			uint8_t						ui8DescAlpha	: 4;
			uint8_t						ui8DescFlipX	: 1;
			uint8_t						ui8DescFlipY	: 1;
			uint8_t						ui8DescPad		: 2;
		} * LPLSI_TGAHEADER, * const LPCLSI_TGAHEADER;

		/**
		 * The TGA file footer.  Not used, but can be used for extra security.
		 */
		typedef struct LSI_TGAFOOTER {
			uint32_t					ui32ExtOffset;
			uint32_t					ui32DevDirOffset;
			uint8_t						ui8Id[16];
			uint8_t						ui8Pad1[2];
		} * LPLSI_TGAFOOTER, * const LPCLSI_TGAFOOTER;
#pragma pack( pop )


		// == Members.
	};

}	// namespace lsi

#endif	// __LSI_TGA_H__
