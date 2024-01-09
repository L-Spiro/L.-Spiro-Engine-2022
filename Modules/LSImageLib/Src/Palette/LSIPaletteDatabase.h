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
 * Description: A palette database.  Palettes can be accessed either by index, ID, or file name.
 */


#ifndef __LSI_PALETTE_DATABASE_H__
#define __LSI_PALETTE_DATABASE_H__

#include "../LSIImageLib.h"
#include "LSIPalette.h"
#include "Vector/LSTLVector.h"

namespace lsi {

	/**
	 * Class CPalette
	 * \brief A palette database.
	 *
	 * Description: A palette database.  Palettes can be accessed either by index, ID, or file name.
	 */
	class CPaletteDatabase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CPaletteDatabase();
		LSE_CALLCTOR							~CPaletteDatabase();


		// == Types.
		/** A PAL header. */
		struct LSI_PPL_HEADER {
#pragma pack( push, 1 )
			uint32_t							ui32Pl98;				/**< "PL98". */
			uint32_t							ui32NumPalettes;		/**< The number of palettes. */

			int32_t								i32ShadeShift;			/**< Shade shift. */
			int32_t								i32HazeLevels;			/**< Haze levels. */
			int32_t								i32HazeColors;			/**< Haze colors. */
			uint8_t								ui8ValidColors[32];		/**< 256 bit indicating colors that can be matched against. */
#pragma pack( pop )
		};


		// == Functions.
		/**
		 * Loads a palette file from memory.  The loaded palette is added to the database.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _sFileName The name of the loaded file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL							LoadPalatte( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName );

		/**
		 * Loads a palette file from a file path.
		 *
		 * \param _pcFile _pcFile UTF-8-formatted string representing the path to the file to load.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM, or that the given file does not exist.
		 */
		LSBOOL LSE_CALL							LoadPalatte( const char * _pcFile );


	protected :
		// == Members.
		/** The array of palettes. */
		CVector<CPalette, uint32_t, 16>			m_vPalettes;


	};

}	// namespace lsi

#endif	// #ifndef __LSI_PALETTE_DATABASE_H__
