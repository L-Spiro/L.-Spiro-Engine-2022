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

#include "LSIPaletteDatabase.h"
#include "HighLevel/LSFFilesEx.h"


namespace lsi {

	// == Various constructors.
	LSE_CALLCTOR CPaletteDatabase::CPaletteDatabase() {
	}
	LSE_CALLCTOR CPaletteDatabase::~CPaletteDatabase() {
	}

	// == Functions.
	/**
	 * Loads a palette file from memory.  The loaded palette is added to the database.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _sFileName The name of the loaded file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CPaletteDatabase::LoadPalatte( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName ) {
		CPalette pPal;
		if ( !pPal.LoadPal( _pui8FileData, _ui32DataLen, _sFileName ) ) {
			if ( !pPal.LoadPpl( _pui8FileData, _ui32DataLen, _sFileName ) ) {
				return false;
			}
		}
		return m_vPalettes.Push( pPal );
	}

	/**
	 * Loads a palette file from a file path.
	 *
	 * \param _pcFile _pcFile UTF-8-formatted string representing the path to the file to load.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM, or that the given file does not exist.
	 */
	LSBOOL LSE_CALL CPaletteDatabase::LoadPalatte( const char * _pcFile ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		char * pcTmp = new char[CStd::StrLen( _pcFile ) + 1];
		CStd::StrCpy( pcTmp, _pcFile );
		CFileLib::NoPath( pcTmp );
		LSBOOL bRet = LoadPalatte( ui8Buffer, static_cast<uint32_t>(uiptrSize), pcTmp );
		CFilesEx::FreeFile( ui8Buffer );
		delete [] pcTmp;
		return bRet;
	}

}	// namespace lsi
