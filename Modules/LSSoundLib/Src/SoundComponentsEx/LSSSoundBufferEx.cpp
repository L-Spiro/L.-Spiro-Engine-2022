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
 * Description: Adding the ability to load data from files to the sound engine.  We use the user parameter
 *	passed to the constructor (_uiptrUserData) as a char * indicating the file to load.  The name of the
 *	file is copied locally.
 */

#include "LSSSoundBufferEx.h"
#include "../Wave/LSSWaveLoader.h"


namespace lss {

	// == Various constructors.
	LSE_CALLCTOR CSoundBufferEx::CSoundBufferEx( const uint8_t * _pui8Data, uint32_t _ui32Length,
		uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, CAllocator * _paAllocator, LSBOOL _bRetain, uintptr_t _uiptrUserData ) :
		CSoundBuffer( _pui8Data, _ui32Length, _ui32Freq, _ui32Bits, _ui32Channels, _paAllocator, _bRetain, _uiptrUserData ),
		m_pcFile( NULL ),
		m_bSoftUnload( false ) {

		if ( _uiptrUserData && _paAllocator ) {
			m_pcFile = static_cast<char *>(_paAllocator->Alloc( CStd::StrLen( reinterpret_cast<char *>(_uiptrUserData) ) + sizeof( char ) ));
			CStd::StrCpy( m_pcFile, reinterpret_cast<char *>(_uiptrUserData) );
		}
	}
	LSE_CALLCTOR CSoundBufferEx::~CSoundBufferEx() {
		if ( m_pcFile && m_paAllocator ) {
			m_paAllocator->Free( m_pcFile );
		}
	}

	// == Functions.
	/**
	 * Sets the value indicating whether this buffer should resist soft unloads or not.  A soft unload is
	 *	an unload that is used to save RAM even though it is assumed the buffer may be needed (and will be
	 *	reloaded) later.  A hard unload is performed to force all buffers out of memory.  A hard unload
	 *	always destroys and releases the buffer, whereas a soft unload is optional and can only be done on
	 *	buffers with a zero reference count.  For a soft unload to work, there must be a way to restore the
	 *	buffer to its current state after being completely unloaded.  The core library adds the ability to
	 *	load sound data from files, which provides this ability.
	 *
	 * \return Returns true if this buffer is allowed to softly unload or false if not.
	 */
	LSBOOL LSH_CALL CSoundBufferEx::CanSoftUnload() const {
		return m_bSoftUnload;
	}

	/**
	 * Allow or disallow soft unloading.
	 *
	 * \param _bValue Value indicating whether soft unloading is allowed or not.
	 */
	void LSE_CALL CSoundBufferEx::SetSoftUnloading( LSBOOL _bValue ) {
		m_bSoftUnload = _bValue;
	}

	/**
	 * When needed, get the original bytes from the file passed to us
	 *	via _uiptrUserData in the constructor.
	 *
	 * \return Returns true if the original data was able to be retrieved.
	 */
	LSBOOL LSH_CALL CSoundBufferEx::GetOriginalData() {
		if ( !m_pcFile || !m_paAllocator ) { return false; }

		uint32_t ui32Len, ui32Freq, ui32Bits, ui32Channels;
		if ( !CWaveLoader::LoadWavFile( m_pcFile, m_pui8Data,
			ui32Len, ui32Freq, ui32Bits, ui32Channels,
			m_paAllocator ) ) { return false; }

		// Still have to verify it matches what we had before.
		if ( ui32Len != m_ui32Size || ui32Freq != m_ui32Freq ||
			ui32Bits != m_ui16BitDepth || ui32Bits != m_ui16BitDepth ) {
			m_paAllocator->Free( m_pui8Data );
		}

		return true;
	}

}	// namespace lss
