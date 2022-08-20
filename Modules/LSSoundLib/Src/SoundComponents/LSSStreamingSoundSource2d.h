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
 * Description: A 2D streaming sound source.  Typically used for background music.  All streaming sounds
 *	default to resisting soft resets, so this stream is prepared to continue playing across state changes,
 *	until manually stopped.
 */


#ifndef __LSS_STREAMINGSOUNDSOURCE2D_H__
#define __LSS_STREAMINGSOUNDSOURCE2D_H__

#include "../LSSSoundLib.h"
#include "LSSStreamingSoundSource.h"

namespace lss {

	/**
	 * Class CStreamingSoundSource2d
	 * \brief A 2D streaming sound source.
	 *
	 * Description: A 2D streaming sound source.  Typically used for background music.  All streaming sounds
	 *	default to resisting soft resets, so this stream is prepared to continue playing across state changes,
	 *	until manually stopped.
	 */
	class CStreamingSoundSource2d : public CStreamingSoundSource {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR										CStreamingSoundSource2d( uint32_t _ui32Id );
		LSE_CALLCTOR										~CStreamingSoundSource2d();


		// == Functions.
		/**
		 * Gets the type of this source.
		 *
		 * \return Returns one of the LSS_SOUND_SOURCE values.
		 */
		virtual LSS_SOUND_SOURCE LSE_CALL					GetType() const;

	protected :
		// == Members.


		// == Functions.
		/**
		 * Apply states to the hardware or sound system (OpenAL for example).
		 * Does not clear the dirty flag.
		 */
		virtual void LSE_CALL								ApplyStates();

		/**
		 * A device has been created.  We can register with the hardware.
		 * The sound manager notifies us of this event after the device has
		 *	been changed.  If there is a device already when we are created,
		 *	we are notified via this function immediately after being created.
		 *
		 * \return Returns false if there is an error uploading our sound data to the hardware.
		 */
		virtual LSBOOL LSE_CALL								DeviceActivated();


	private :
		typedef CStreamingSoundSource						Parent;
	};

}	// namespace lss

#endif	// __LSS_STREAMINGSOUNDSOURCE2D_H__
