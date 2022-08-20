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


#ifndef __LSI_QOI_H__
#define __LSI_QOI_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Color-space values.
	 */
	enum LSI_QOI_COLOR_SPACE : uint8_t {
		LSI_QCS_SRGB						= 0,
		LSI_QCS_LINEAR						= 1,
	};

	/**
	 * Masks.
	 */
	enum LSI_QOI_MASK : uint8_t {
		LSI_QM_OP_INDEX						= 0x00, /* 00xxxxxx */
		LSI_QM_OP_DIFF						= 0x40, /* 01xxxxxx */
		LSI_QM_OP_LUMA						= 0x80, /* 10xxxxxx */
		LSI_QM_OP_RUN						= 0xC0, /* 11xxxxxx */
		LSI_QM_OP_RGB						= 0xFE, /* 11111110 */
		LSI_QM_OP_RGBA						= 0xFF, /* 11111111 */

		LSI_QM_MASK_2						= 0xC0
	};

	/**
	 * Class CQoi
	 * \brief Loading a QOI file.
	 *
	 * Description: Loading a QOI file.
	 */
	class CQoi {
	public :
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
		static LSBOOL LSE_CALL			LoadQoi( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, bool _bSrgb,
			CImageLib::CTexelBuffer &_tbReturn );

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
		static LSBOOL LSE_CALL			LoadQoi( const char * _pcFile,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, bool _bSrgb,
			CImageLib::CTexelBuffer &_tbReturn );


	protected :
		// == Types.
#pragma pack( push, 1 )
		/**
		 * The QOI file header.
		 */
		typedef struct LSI_QOIHEADER {
			uint8_t						ui8Magic[4];
			uint32_t					ui32Width;
			uint32_t					ui32Height;
			uint8_t						ui8Channels;
			uint8_t						ui8ColorSpace;
		} * LPLSI_QOIHEADER, * const LPCLSI_QOIHEADER;
#pragma pack( pop )


		// == Members.
	};

}	// namespace lsi

#endif	// __LSI_QOI_H__
