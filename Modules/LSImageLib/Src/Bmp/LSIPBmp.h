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


#ifndef __LSI_PBMP_H__
#define __LSI_PBMP_H__

#include "../LSIImageLib.h"
#include "../Palette/LSIPaletteDatabase.h"

namespace lsi {

	/**
	 * Class CPBmp
	 * \brief Loading a Starsiege: Tribes Phoenix bitmap file.
	 *
	 * Description: Loading a Starsiege: Tribes Phoenix bitmap file.
	 */
	class CPBmp {
	public :
		// == Types.
		/** A chunk header. */
		struct LSI_CHUNK {
#pragma pack( push, 1 )
			uint32_t							ui32Name;				/**< The chunk name. */
			uint32_t							ui32Size;				/**< The chunk size. */
#pragma pack( pop )
		};

		/** A chunk and its data. */
		struct LSI_CHUNK_EX : public LSI_CHUNK {
#pragma pack( push, 1 )
			uint8_t								ui8Data[1];				/**< The beginning of the chunk data. */
#pragma pack( pop )
		};

		/** A "head" chunk. */
		struct LSI_HEAD_CHUNK : public LSI_CHUNK {
#pragma pack( push, 1 )
			uint32_t							ui32Version;			/**< The file version. */
			uint32_t							ui32Width;				/**< The image (mip 0) width. */
			uint32_t							ui32Height;				/**< The image height (mip 0). */
			uint32_t							ui32BitDepth;			/**< The image bit depth (should always be 8). */
			uint32_t							ui32Attribute;			/**< Image attributes. */
#pragma pack( pop )
		};

		/** A "DETL" chunk. */
		struct LSI_DETL_CHUNK : public LSI_CHUNK {
#pragma pack( push, 1 )
			uint32_t							ui32MipLevels;			/**< The number of mip levels, including the base level mip 0. */
#pragma pack( pop )
		};

		/** A "PiDX" chunk. */
		struct LSI_PIDX_CHUNK : public LSI_CHUNK {
#pragma pack( push, 1 )
			uint32_t							ui32PalIdx;				/**< If there is no RIFF chunk, this must exist to locate a palette within the palette database. */
#pragma pack( pop )
		};


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
		static LSBOOL LSE_CALL			LoadPBmp( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat, const CPaletteDatabase &_pdPalettes,
			CImageLib::CTexelBuffer &_tbReturn,
			CVector<CImageLib::CTexelBuffer> &_vMipMaps );
	};

}	// namespace lsi

#endif	// __LSI_PBMP_H__
