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

namespace lsi {

	/**
	 * Class CPBmp
	 * \brief Loading a bitmap file.
	 *
	 * Description: Loading a bitmap file.
	 */
	class CPBmp {
	public :
		// == Types.


		// == Functions.
		/**
		 * Load a PBMP file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _ui32Width The returned width of the image.
		 * \param _ui32Height The returned height of the image.
		 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
		 * \param _tbReturn The returned data.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		/*static LSBOOL LSE_CALL			LoadPBmp( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );*/
	};

}	// namespace lsi

#endif	// __LSI_PBMP_H__
