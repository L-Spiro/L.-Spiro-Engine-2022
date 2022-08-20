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

#include "LSIImagePack.h"
#include "FileStream/LSFFileStream.h"
#include "Streams/LSSTDExternalByteStream.h"


namespace lsi {

	// == Various constructors.
	LSE_CALLCTOR CImagePack::CImagePack() {
	}
	LSE_CALLCTOR CImagePack::~CImagePack() {
		Reset();
	}

	// == Functions.
	/**
	 * Loads an LSIP file from memory.  Returns an error code on failure.
	 *
	 * \param _pui8Data The data for the LSIP file loaded to memory.
	 * \param _uiptrLen The length of the data in bytes.
	 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
	 */
	LSSTD_ERRORS LSE_CALL CImagePack::LoadPackFromMemory( const uint8_t * _pui8Data, uintptr_t _uiptrLen ) {
		CExternalByteStream ebsStream( _pui8Data, _uiptrLen );
		return LoadPackFromSteam( ebsStream );
	}

	/**
	 * Loads an LSIP file from disk.  Returns an error code on failure.
	 *
	 * \param _pcPath The path to the LSIP file to load.
	 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
	 */
	LSSTD_ERRORS LSE_CALL CImagePack::LoadPackFromFile( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CFileStream fsStream( CFileLib::ExpandResourcePath( _pcPath, szBuffer ), LSF_OF_READ, false );
		return LoadPackFromSteam( fsStream );
	}

	/**
	 * Loads an LSIP from a stream.  Returns an error code on failure.
	 *
	 * \param _sStream The stream from which to read the LSIP data.
	 * \return Returns LSSTD_E_SUCCESS on success or another error code on failure.
	 */
	LSSTD_ERRORS LSE_CALL CImagePack::LoadPackFromSteam( const CStream &_sStream ) {
		Reset();
		uint32_t ui32Header;
		if ( !_sStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Header), sizeof( ui32Header ) ) ) {
			Reset();
			return LSSTD_E_INVALIDDATA;
		}
		if ( ui32Header != 0x5049534CUL ) {
			Reset();
			return LSSTD_E_INVALIDDATA;
		}
		uint32_t ui32Total;
		if ( !_sStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Total), sizeof( ui32Total ) ) ) {
			Reset();
			return LSSTD_E_INVALIDDATA;
		}
		if ( !m_mDictionary.Allocate( ui32Total ) ) {
			Reset();
			return LSSTD_E_OUTOFMEMORY;
		}
		if ( !m_vCompressedImages.Resize( ui32Total ) ) {
			Reset();
			return LSSTD_E_OUTOFMEMORY;
		}

		for ( uint32_t I = ui32Total; I--; ) {
			LSI_DIR_ENTRY deEntry;
			if ( !_sStream.ReadBytes( reinterpret_cast<uint8_t *>(&deEntry), sizeof( deEntry ) ) ) {
				Reset();
				return LSSTD_E_INVALIDDATA;
			}
			CString sName;
			if ( !sName.Append( deEntry.cMaterialName, 16 ) ) {
				Reset();
				return LSSTD_E_OUTOFMEMORY;
			}
			while ( sName.GetLastChar() == '\0' && sName.Length() ) { sName.RemLastChar(); }
			if ( !sName.Length() ) {
				Reset();
				return LSSTD_E_INVALIDDATA;
			}
			sName.Snap();
			if ( m_mDictionary.ItemExists( sName ) ) {
				Reset();
				return LSSTD_E_INVALIDDATA;
			}
			LSI_ENTRIES eThis = {
				deEntry.ui32Offset,
				deEntry.ui32Size,
			};
			if ( !m_mDictionary.Insert( sName, eThis ) ) {
				Reset();
				return LSSTD_E_OUTOFMEMORY;
			}
		}

		// Get the size of the big chunk.
		uint32_t ui32Chunk;
		if ( !_sStream.ReadBytes( reinterpret_cast<uint8_t *>(&ui32Chunk), sizeof( ui32Chunk ) ) ) {
			Reset();
			return LSSTD_E_INVALIDDATA;
		}
		if ( ui32Chunk ) {
			CVector<uint8_t, uint32_t> vDataArray;
			if ( !vDataArray.Resize( ui32Chunk ) ) {
				Reset();
				return LSSTD_E_OUTOFMEMORY;
			}
			if ( !_sStream.ReadBytes( &vDataArray[0], vDataArray.Length() ) ) {
				Reset();
				return LSSTD_E_INVALIDDATA;
			}

			for ( uint32_t I = ui32Total; I--; ) {
				const LSI_ENTRIES & eThis = m_mDictionary.GetByIndex( I );
				if ( !m_vCompressedImages[I].Resize( eThis.ui32Size ) ) {
					Reset();
					return LSSTD_E_OUTOFMEMORY;
				}
				if ( m_vCompressedImages[I].Length() ) {
					CStd::MemCpy( &m_vCompressedImages[I][0], &vDataArray[eThis.ui32Offset], eThis.ui32Size );
				}
			}
		}
		return LSSTD_E_SUCCESS;
	}

	/**
	 * Resets the pack completely, freeing all memory.
	 */
	void LSE_CALL CImagePack::Reset() {
		m_mDictionary.Reset();
		m_vCompressedImages.Reset();

		for ( uint32_t I = m_mUnpackedImages.Length(); I--; ) {
			LSEDELETE m_mUnpackedImages.GetByIndex( I );
		}
		m_mUnpackedImages.Reset();
	}

	/**
	 * Gets a pointer to a CImage by name.  If the given name does not represent a valid image
	 *	NULL is returned.  The first time an image is request from the pack it is decompressed
	 *	and its compressed data is released.
	 *
	 * \param _sName The name of the image to get from the packed image archive.
	 * \return Returns a pointer to the respective CImage or NULL if no image with the given name exists.
	 */
	CImage * LSE_CALL CImagePack::GetImageByName( const CString _sName ) {
		uint32_t ui32Index;
		if ( m_mUnpackedImages.ItemExists( _sName, ui32Index ) ) {
			return m_mUnpackedImages.GetByIndex( ui32Index );
		}
		// Not previously requested.  Does it exist at all?
		if ( !m_mDictionary.ItemExists( _sName, ui32Index ) ) {
			// Nope.
			return NULL;
		}
		// It exists.
		CImage * piNew = LSENEW CImage();
		if ( !piNew ) { return NULL; }
		if ( !piNew->LoadFileFromMemory( &m_vCompressedImages[ui32Index][0], m_vCompressedImages[ui32Index].Length() ) ) {
			LSEDELETE piNew;
			return NULL;
		}
		if ( !m_mUnpackedImages.Insert( _sName, piNew ) ) {
			LSEDELETE piNew;
			return NULL;
		}
		m_vCompressedImages[ui32Index].Reset();	// No longer need the compressed copy.
		return piNew;
	}

}	// namespace lsi
