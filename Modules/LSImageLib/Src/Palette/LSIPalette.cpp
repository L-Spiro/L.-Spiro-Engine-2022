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

#include "LSIPalette.h"


namespace lsi {

	// == Various constructors.
	LSE_CALLCTOR CPalette::CPalette() :
		m_ui32Id( 0 ),
		m_ptType( LSI_PT_NO_REMAP ) {
	}
	LSE_CALLCTOR CPalette::~CPalette() {
		return;
	}

	// == Functions.
	/**
	 * Loads a PAL file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _sFileName The name of the loaded file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CPalette::LoadPal( const uint8_t * _pui8FileData, uint32_t _ui32DataLen, const CString &_sFileName ) {
		// Size checks.
		if ( _ui32DataLen < sizeof( LSI_PAL_HEADER ) ) { return false; }
		const LSI_PAL_HEADER * pphHeader = reinterpret_cast<const LSI_PAL_HEADER *>(_pui8FileData);
		if ( pphHeader->ui32Riff != 0x46464952 ) { return false; }	// RIFF.
		if ( pphHeader->ui64PalData != 0x61746164204C4150ULL ) { return false; }	// PAL data.
		if ( pphHeader->ui8Reserved[0] != 0 || pphHeader->ui8Reserved[1] != 3 ) { return false; }

		if ( pphHeader->ui32FileSize + 8 != _ui32DataLen ) { return false; }
		if ( pphHeader->ui32DataSize + 20 != _ui32DataLen ) { return false; }
		//if ( !m_paPalette.Resize( pphHeader->ui16PalEntries ) ) { return false; }
		uint32_t ui32Total = pphHeader->ui16PalEntries;
		_pui8FileData += sizeof( LSI_PAL_HEADER );
		_ui32DataLen -= sizeof( LSI_PAL_HEADER );
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			if ( _ui32DataLen < sizeof( LSI_PALETTE_ENTRY ) ) { return false; }
			const CPalette::LSI_PALETTE_ENTRY * ppeEntry = reinterpret_cast<const LSI_PALETTE_ENTRY *>(_pui8FileData);
			if ( !m_paPalette.Push( (*ppeEntry) ) ) { return false; }

			_pui8FileData += sizeof( LSI_PALETTE_ENTRY );
			_ui32DataLen -= sizeof( LSI_PALETTE_ENTRY );
		}

		if ( _ui32DataLen >= sizeof( uint32_t ) ) {
			m_ui32Id = (*reinterpret_cast<const uint32_t *>(_pui8FileData));

			_pui8FileData += sizeof( uint32_t );
			_ui32DataLen -= sizeof( uint32_t );
		}

		m_sFileName = _sFileName;
		return true;
	}

	/**
	 * Loads a PPL file from memory.
	 *
	 * \param _pui8FileData The in-memory image of the file.  This points only to the palette data and the ID/type that follow it.  On return,
	 *	this is adjusted to the end of the palette/ID/type data to the start of the next palette for the database to load.
	 * \param _ui32DataLen The length of the in-memory image of the file.
	 * \param _sFileName The name of the loaded file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	LSBOOL LSE_CALL CPalette::LoadPpl( const uint8_t * &_pui8FileData, uint32_t &_ui32DataLen, const CString &_sFileName ) {
		if ( _ui32DataLen < sizeof( LSI_PALETTE_ENTRY ) * 256 ) { return false; }

		for ( uint32_t I = 0; I < 256; ++I ) {
			if ( _ui32DataLen < sizeof( LSI_PALETTE_ENTRY ) ) { return false; }
			const CPalette::LSI_PALETTE_ENTRY * ppeEntry = reinterpret_cast<const LSI_PALETTE_ENTRY *>(_pui8FileData);
			if ( !m_paPalette.Push( (*ppeEntry) ) ) { return false; }

			_pui8FileData += sizeof( LSI_PALETTE_ENTRY );
			_ui32DataLen -= sizeof( LSI_PALETTE_ENTRY );
		}

		if ( _ui32DataLen >= sizeof( uint32_t ) ) {
			m_ui32Id = (*reinterpret_cast<const uint32_t *>(_pui8FileData));

			_pui8FileData += sizeof( uint32_t );
			_ui32DataLen -= sizeof( uint32_t );
		}
		if ( _ui32DataLen >= sizeof( uint32_t ) ) {
			m_ptType = static_cast<LSI_PAL_TYPE>((*reinterpret_cast<const uint32_t *>(_pui8FileData)));

			_pui8FileData += sizeof( uint32_t );
			_ui32DataLen -= sizeof( uint32_t );
		}


		m_sFileName = _sFileName;
		return true;
	}

}	// namespace lsi
