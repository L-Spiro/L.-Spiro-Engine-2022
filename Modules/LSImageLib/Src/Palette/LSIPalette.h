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
 * Description: A palette which could have come from various file types, such as .PAL or .PPL.
 */


#ifndef __LSI_PALETTE_H__
#define __LSI_PALETTE_H__

#include "../LSIImageLib.h"
#include "String/LSTLString.h"

namespace lsi {

	/**
	 * Class CPalette
	 * \brief A palette which could have come from various file types, such as .PAL or .PPL.
	 *
	 * Description: A palette which could have come from various file types, such as .PAL or .PPL.
	 */
	class CPalette {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CPalette();
		LSE_CALLCTOR							~CPalette();


		// == Enumerations.
		/** Palette type. */
		enum LSI_PAL_TYPE : uint32_t {
			LSI_PT_NO_REMAP						= 0,
			LSI_PT_SHADE_HAZE					= 1,
			LSI_PT_TRANSLUCENT					= 2,
			LSI_PT_COLOR_QUANT					= 3,
			LSI_PT_ALPHA_QUANT					= 4,
			LSI_PT_ADDITIVE_QUANT				= 5,
			LSI_PT_ADDITIVE						= 6,
			LSI_PT_SUBTRACTIVE_QUANT			= 7,
			LSI_PT_SUBTRACTIVE					= 8,
		};


		// == Types.
		/** A palette color entry. */
		struct LSI_PAL_ENTRY_RGBA {
#pragma pack( push, 1 )
			uint8_t								ui8R;					/**< The red component. */
			uint8_t								ui8G;					/**< The green component. */
			uint8_t								ui8B;					/**< The blue component. */
			uint8_t								ui8A;					/**< The alpha component. */
#pragma pack( pop )
		};

		/** A palette entry. */
		union LSI_PALETTE_ENTRY {
			LSI_PAL_ENTRY_RGBA					perRgba;				/**< The RGBA color value. */
			uint8_t								ui8Vals[4];				/**< Array access into the RGBA values. 0 = red, 3 = alpha. */
		};

		/** The palette array. */
		typedef CVectorPoD<LSI_PALETTE_ENTRY, uint32_t, 256>
												CPalArray;

		/** A chunk header. */
		struct LSI_CHUNK {
#pragma pack( push, 1 )
			uint32_t							ui32Name;				/**< The chunk name. */
			uint32_t							ui32Size;				/**< The chunk size. */
#pragma pack( pop )
		};

		/** A PAL header. */
		struct LSI_PAL_HEADER {
#pragma pack( push, 1 )
			uint32_t							ui32Riff;				/**< "RIFF". */
			uint32_t							ui32FileSize;			/**< The file size - 8. */

			uint64_t							ui64PalData;			/**< "PAL data". */
			uint32_t							ui32DataSize;			/**< The file size - 20. */

			uint8_t								ui8Reserved[2];			/**< [0, 3]. */
			uint16_t							ui16PalEntries;			/**< The number of palette entries. */
#pragma pack( pop )
		};
		
		
		// == Functions.
		/**
		 * Loads a PAL file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _sFileName The name of the loaded file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL							LoadPal( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName );

		/**
		 * Loads a PPL file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _sFileName The name of the loaded file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		LSBOOL LSE_CALL							LoadPpl( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName );


	protected :
		// == Members.
		/** The palette RGBA data. */
		CPalArray								m_paPalette;

		/** The file name. */
		CString									m_sFileName;

		/** The ID of the palette. */
		uint32_t								m_ui32Id;

		/** The palette type. */
		LSI_PAL_TYPE							m_ptType;

	};

}	// namespace lsi

#endif	// __LSI_PALETTE_H__
