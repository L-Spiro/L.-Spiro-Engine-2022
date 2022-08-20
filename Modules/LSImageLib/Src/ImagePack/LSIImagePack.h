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
 * Description: Handles loading of an L. Spiro Image Pack and distributing shared pointers to images within
 *	the pack.  Memory is conserved as much as possible, and images from the pack are only expanded when
 *	first requested.
 */


#ifndef __LSE_IMAGEPACK_H__
#define __LSE_IMAGEPACK_H__

#include "../LSIImageLib.h"
#include "../Image/LSIImage.h"
#include "Map/LSTLLinearMap.h"
#include "Streams/LSSTDStream.h"
#include "String/LSTLString.h"

namespace lsi {

	/**
	 * Class CImagePack
	 * \brief Handles loading of an L. Spiro Image Pack and distributing shared pointers to images within
	 *	the pack.
	 *
	 * Description: Handles loading of an L. Spiro Image Pack and distributing shared pointers to images within
	 *	the pack.  Memory is conserved as much as possible, and images from the pack are only expanded when
	 *	first requested.
	 */
	class CImagePack {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CImagePack();
		LSE_CALLCTOR							~CImagePack();


		// == Functions.
		/**
		 * Loads an LSIP file from memory.  Returns an error code on failure.
		 *
		 * \param _pui8Data The data for the LSIP file loaded to memory.
		 * \param _uiptrLen The length of the data in bytes.
		 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
		 */
		LSSTD_ERRORS LSE_CALL					LoadPackFromMemory( const uint8_t * _pui8Data, uintptr_t _uiptrLen );

		/**
		 * Loads an LSIP file from disk.  Returns an error code on failure.
		 *
		 * \param _pcPath The path to the LSIP file to load.
		 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
		 */
		LSSTD_ERRORS LSE_CALL					LoadPackFromFile( const char * _pcPath );

		/**
		 * Loads an LSIP from a stream.  Returns an error code on failure.
		 *
		 * \param _sStream The stream from which to read the LSIP data.
		 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
		 */
		LSSTD_ERRORS LSE_CALL					LoadPackFromSteam( const lsstd::CStream &_sStream );

		/**
		 * Resets the pack completely, freeing all memory.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets a pointer to a CImage by name.  If the given name does not represent a valid image
		 *	NULL is returned.  The first time an image is request from the pack it is decompressed
		 *	and its compressed data is released.
		 *
		 * \param _sName The name of the image to get from the packed image archive.
		 * \return Returns a pointer to the respective CImage or NULL if no image with the given name exists.
		 */
		CImage * LSE_CALL						GetImageByName( const CString _sName );


	protected :
		// == Types.
		/**
		 * The pack dictionary of entries.
		 */
		typedef struct LSI_ENTRIES {
			/**
			 * Offset into the data pile of this image.
			 */
			uint32_t							ui32Offset;

			/**
			 * Compressed size of the image.
			 */
			uint32_t							ui32Size;
		} * LPLSI_ENTRIES, * const LPCLSI_ENTRIES;

		/**
		 * Directory entry.
		 */
#pragma pack( push, 1 )
		typedef struct LSI_DIR_ENTRY {
			/**
			 * Material name.
			 */
			char								cMaterialName[16];

			/**
			 * Offset into the data from the start of the data.
			 */
			uint32_t							ui32Offset;

			/**
			 * Size of the data.
			 */
			uint32_t							ui32Size;
		} * LPLSI_DIR_ENTRY, * const LPCLSI_DIR_ENTRY;
#pragma pack( pop )

		// == Members.
		/**
		 * The entries sorted by name.
		 */
		CLinearMap<CString, LSI_ENTRIES>		m_mDictionary;

		/**
		 * The compressed images.  Images are deleted when decompressed.
		 */
		CVector<CVector<uint8_t, uint32_t>, uint32_t>
												m_vCompressedImages;

		/**
		 * Entries that have been requested to be unpacked.
		 */
		CLinearMap<CString, CImage *>			m_mUnpackedImages;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsi

#endif	// __LSE_IMAGEPACK_H__
