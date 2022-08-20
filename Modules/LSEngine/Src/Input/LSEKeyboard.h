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


#ifndef __LSE_KEYBOARD_H__
#define __LSE_KEYBOARD_H__

#include "../LSEEngine.h"
#include "LSEKeyboardKeys.h"

namespace lse {

	// == Enumerations.
	/** Keyboard metrics. */
	enum LSE_KEYBOARD_METRICS {
		LSE_KM_TOTALKEYS					= 300,
		LSE_KM_MAXEVENTS					= 250
	};

	/**
	 * Class CKeyboard
	 * \brief Manages keyboard input.
	 *
	 * Description: Keyboard input updated once per frame.  Requires a helper CKeyboardBuffer object to gather
	 *	input notifications between frames.  Input data is buffered in the CKeyboardBuffer object and passed
	 *	off to the keyboard.
	 *
	 * Key information includes if the key is down, just released, just pressed, and how long it has been down.
	 */
	class CKeyboard {
		// The CKeyboardBuffer class will update us with its data.
		friend class					CGame;

	public :
		// == Various constructors.
		LSE_CALL						CKeyboard();


		// == Functions.
		/**
		 * Determine if the given key is being held.
		 *
		 * \param _ui32Key The key to test for being held.
		 * \return Returns true if the given key is valid and is being held as of the last time the keyboard was updated.
		 */
		LSBOOL LSE_CALL					KeyIsDown( uint32_t _ui32Key ) const;

		/**
		 * Determine if a key was just pressed since the last time the keyboard was updated.
		 *
		 * \param _ui32Key The key to test for being just pressed.
		 * \return Returns true if the given key is valid and was just pressed as of the last time the keyboard was updated.
		 */
		LSBOOL LSE_CALL					KeyJustPressed( uint32_t _ui32Key ) const;

		/**
		 * Determine if a key was just released since the last time the keyboard was updated.
		 *
		 * \param _ui32Key The key to test for being just released.
		 * \return Returns true if the given key is valid and was just released as of the last time the keyboard was updated.
		 */
		LSBOOL LSE_CALL					KeyJustReleased( uint32_t _ui32Key ) const;


	protected :
		// == Types.
		/** A key. */
		typedef struct LSE_KEY_INFO {
			/** Is the key down? */
			LSBOOL						bIsDown;

			/** Time when the key was pressed. */
			uint64_t					ui64TimePressed;

			/** Time when the key was released. Only applicable if the key is not down. */
			uint64_t					ui64TimeReleased;
		} * LPLSE_KEY_INFO, * const LPCLSE_KEY_INFO;


		// == Members.
		/** Key information. */
		LSE_KEY_INFO					m_kiKeys[LSE_KM_TOTALKEYS];

		/** Copy of the keys from last frame. */
		LSE_KEY_INFO					m_kiLastKeys[LSE_KM_TOTALKEYS];
	};

}	// namespace lse

#endif	// __LSE_KEYBOARD_H__
