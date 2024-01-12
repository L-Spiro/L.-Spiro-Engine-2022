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
	LSBOOL LSE_CALL CPaletteDatabase::LoadPalette( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName ) {
		{
			CPalette pPal;
			if ( pPal.LoadPal( _pui8FileData, _ui32DataLen, _sFileName ) ) {
				return m_vPalettes.Push( pPal );
			}
		}

		// Size checks.
		if ( _ui32DataLen < sizeof( LSI_PPL_HEADER ) ) { return false; }
		const LSI_PPL_HEADER * pphHeader = reinterpret_cast<const LSI_PPL_HEADER *>(_pui8FileData);
		if ( pphHeader->ui32Pl98 != 0x38394C50 ) { return false; }														// RIFF.
		if ( (pphHeader->ui32NumPalettes * 256 + 2) * 4 + sizeof( LSI_PPL_HEADER ) > _ui32DataLen ) { return false; }	// There has to be enough data to store X palettes, each 256 entries and 4 bytes per entry, plus the header, plus 2 4-byte values after the table.
		_pui8FileData += sizeof( LSI_PPL_HEADER );
		_ui32DataLen -= sizeof( LSI_PPL_HEADER );
		for ( uint32_t I = 0; I < pphHeader->ui32NumPalettes; ++I ) {
			CString sTmpName = _sFileName + CString( "." ) + CString::CreateFromI64( I );
			CPalette pPal;
			if ( !pPal.LoadPpl( _pui8FileData, _ui32DataLen, sTmpName ) ) { return false; }
			if ( !m_vPalettes.Push( pPal ) ) { return false; }
		}
		return true;
	}

	/**
	 * Loads a palette file from a file path.
	 *
	 * \param _pcFile _pcFile UTF-8-formatted string representing the path to the file to load.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM, or that the given file does not exist.
	 */
	LSBOOL LSE_CALL CPaletteDatabase::LoadPalette( const char * _pcFile ) {
		uint8_t * ui8Buffer;
		uintptr_t uiptrSize;
		if ( !CFilesEx::LoadFileToMemory( _pcFile, ui8Buffer, &uiptrSize ) ) { return false; }
		char * pcTmp = new char[CStd::StrLen( _pcFile ) + 1];
		CStd::StrCpy( pcTmp, _pcFile );
		CFileLib::NoPath( pcTmp );
		LSBOOL bRet = LoadPalette( ui8Buffer, static_cast<uint32_t>(uiptrSize), pcTmp );
		CFilesEx::FreeFile( ui8Buffer );
		delete [] pcTmp;
		return bRet;
	}

	/**
	 * Gets a constant pointer to a palette by its ID, or NULL if no such palette could be found.  The database should not be modified while the point is being used.
	 * 
	 * \param _ui32Id The ID of the palette to find.
	 * \return Returns a pointer to the requested palette or NULL if no matching palette could be found.
	 **/
	const CPalette * CPaletteDatabase::PaletteById( uint32_t _ui32Id ) const {
		for ( uint32_t I = 0; I < m_vPalettes.Length(); ++I ) {
			if ( m_vPalettes[I].Id() == _ui32Id ) { return &m_vPalettes[I]; }
		}
		return NULL;
	}

}	// namespace lsi
