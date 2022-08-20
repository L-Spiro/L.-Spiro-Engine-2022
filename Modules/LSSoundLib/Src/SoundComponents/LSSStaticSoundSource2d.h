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
 * Description: A static sound emenating from somewhere in a 3D world.  Does not use streaming.  The source
 *	is not relative to the listener.
 */


#ifndef __LSS_STATICSOUNDSOURCE2D_H__
#define __LSS_STATICSOUNDSOURCE2D_H__

#include "../LSSSoundLib.h"
#include "LSSSoundBuffer.h"
#include "LSSStaticSoundSource.h"

namespace lss {

	/**
	 * Class CStaticSoundSource2d
	 *
	 * Description: A static sound emenating from somewhere in a 2D world.
	 */
	class CStaticSoundSource2d : public CStaticSoundSource {
	public :
		// == Various constructors.
		LSE_CALLCTOR									CStaticSoundSource2d( uint32_t _ui32Id );
		virtual LSE_CALLCTOR							~CStaticSoundSource2d();


		// == Functions.
		/**
		 * Gets the type of this source.
		 *
		 * \return Returns one of the LSS_SOUND_SOURCE values.
		 */
		virtual LSS_SOUND_SOURCE LSE_CALL				GetType() const;

	protected :
		// == Members.


		// == Functions.
		/**
		 * Apply states to the hardware or sound system (OpenAL for example).
		 * Does not clear the dirty flag.
		 */
		virtual void LSE_CALL							ApplyStates();

		/**
		 * A device has been created.  We can register with the hardware.
		 * The sound manager notifies us of this event after the device has
		 *	been changed.  If there is a device already when we are created,
		 *	we are notified via this function immediately after being created.
		 *
		 * \return Returns false if there is an error uploading our sound data to the hardware.
		 */
		virtual LSBOOL LSE_CALL							DeviceActivated();

	private :
		typedef CStaticSoundSource						Parent;
	};

}	// namespace lss

#endif	// __LSS_STATICSOUNDSOURCE2D_H__
