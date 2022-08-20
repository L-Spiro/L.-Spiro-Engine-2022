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


#ifndef __LSI_GIF_H__
#define __LSI_GIF_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CGif
	 * \brief Loading a GIF file.
	 *
	 * Description: Loading a GIF file.
	 */
	class CGif {
	public :
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
		static LSBOOL LSE_CALL			LoadGif( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );

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
		static LSBOOL LSE_CALL			LoadGif( const char * _pcFile,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );


	protected :
		// == Types.
#pragma pack( push, 1 )
		/**
		 * GIF file header.
		 */
		typedef struct LSI_GIFHEADER {
			uint8_t						ui8Header[3];
			uint8_t						ui8Version[3];
			uint16_t					ui16Width;
			uint16_t					ui16Height;
			uint8_t						ui8SizeOfGct	: 3;
			uint8_t						ui8SortFlag		: 1;
			uint8_t						ui8ColorBits	: 3;
			uint8_t						ui8GctFlag		: 1;
			uint8_t						ui8BackGroundIndex;
			uint8_t						ui8Aspect;
		} * LPLSI_GIFHEADER, * const LPCLSI_GIFHEADER;

		/**
		 * GIF colors.  Always 24 bits.
		 */
		typedef struct LSI_GIFCOLOR {
			uint8_t						ui8R;
			uint8_t						ui8G;
			uint8_t						ui8B;
		} * LPLSI_GIFCOLOR, * const LPCLSI_GIFCOLOR;

		/**
		 * GIF image descriptor.
		 */
		typedef struct LSI_GIFIMGDESCRIPTOR {
			uint8_t						ui8Separator;
			uint16_t					ui16X;
			uint16_t					ui16Y;
			uint16_t					ui16W;
			uint16_t					ui16H;
			uint8_t						ui8LocalCt		: 1;
			uint8_t						ui8Interlaced	: 1;
			uint8_t						ui8Sorted		: 1;
			uint8_t						ui8Reserved		: 2;
			uint8_t						ui8SizeOfLct	: 3;
		} * LPLSI_GIFIMGDESCRIPTOR, * const LPCLSI_GIFIMGDESCRIPTOR;

		/**
		 * GIF graphic control extension block.
		 */
		typedef struct LSI_GIFGRAPHICCONTROL {
			uint8_t						ui8Separator;
			uint8_t						ui8Label;
			uint8_t						ui8Size;
			uint8_t						ui8Reserved		: 3;
			uint8_t						ui8Disposal		: 3;
			uint8_t						ui8Input		: 1;
			uint8_t						ui8Trans		: 1;
			uint16_t					ui16Delay;
			uint8_t						ui8TransColor;
			uint8_t						ui8Terminator;
		} * LPLSI_GIFGRAPHICCONTROL, * const LPCLSI_GIFGRAPHICCONTROL;
#pragma pack( pop )
	};

}	// namespace lsi

#endif	// __LSI_GIF_H__
