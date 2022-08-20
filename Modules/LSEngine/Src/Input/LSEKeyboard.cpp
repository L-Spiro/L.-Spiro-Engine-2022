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
 * Description: Keyboard input updated once per frame.  Requires a helper CKeyboardBuffer object to gather
 *	input notifications between frames.  Input data is buffered in the CKeyboardBuffer object and passed
 *	off to the keyboard.
 *
 * Key information includes if the key is down, just released, just pressed, and how long it has been down.
 */

#include "LSEKeyboard.h"


namespace lse {

	// == Various constructors.
	LSE_CALL CKeyboard::CKeyboard() {
		for ( uint32_t I = 0; I < LSE_ELEMENTS( m_kiKeys ); ++I ) {
			m_kiKeys[I].bIsDown = m_kiLastKeys[I].bIsDown = false;
			m_kiKeys[I].ui64TimePressed = m_kiLastKeys[I].ui64TimePressed = 0ULL;
		}
	}

	/**
	 * Determine if the given key is being held.
	 *
	 * \param _ui32Key The key to test for being held.
	 * \return Returns true if the given key is valid and is being held as of the last time the keyboard was updated.
	 */
	LSBOOL LSE_CALL CKeyboard::KeyIsDown( uint32_t _ui32Key ) const {
		if ( _ui32Key >= LSE_ELEMENTS( m_kiKeys ) ) { return false; }
		return m_kiKeys[_ui32Key].bIsDown;
	}

	/**
	 * Determine if a key was just pressed since the last time the keyboard was updated.
	 *
	 * \param _ui32Key The key to test for being just pressed.
	 * \return Returns true if the given key is valid and was just pressed as of the last time the keyboard was updated.
	 */
	LSBOOL LSE_CALL CKeyboard::KeyJustPressed( uint32_t _ui32Key ) const {
		if ( _ui32Key >= LSE_ELEMENTS( m_kiKeys ) ) { return false; }
		return m_kiKeys[_ui32Key].bIsDown && !m_kiLastKeys[_ui32Key].bIsDown;
	}

	/**
	 * Determine if a key was just released since the last time the keyboard was updated.
	 *
	 * \param _ui32Key The key to test for being just released.
	 * \return Returns true if the given key is valid and was just released as of the last time the keyboard was updated.
	 */
	LSBOOL LSE_CALL CKeyboard::KeyJustReleased( uint32_t _ui32Key ) const {
		if ( _ui32Key >= LSE_ELEMENTS( m_kiKeys ) ) { return false; }
		return !m_kiKeys[_ui32Key].bIsDown && m_kiLastKeys[_ui32Key].bIsDown;
	}

}	// namespace lse
