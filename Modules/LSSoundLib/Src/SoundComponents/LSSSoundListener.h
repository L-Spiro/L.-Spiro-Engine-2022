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
 * Description: The listener hears all the sounds in the world.  There is only one listener in the game at a
 *	time.  The gain property controls the master volume of all sound in the game.
 */


#ifndef __LSS_SOUNDLISTENER_H__
#define __LSS_SOUNDLISTENER_H__

#include "../LSSSoundLib.h"
#include "Vector/LSMVector3.h"

namespace lss {

	/**
	 * Class CSoundListener
	 * \brief The listener hears all the sounds in the world.
	 *
	 * Description: The listener hears all the sounds in the world.  There is only one listener in the game at a
	 *	time.  The gain property controls the master volume of all sound in the game.
	 */
	class CSoundListener {
	public :
		// == Various constructors.
		LSE_CALLCTOR									CSoundListener();
		LSE_CALLCTOR									~CSoundListener();


		// == Functions.
		/**
		 * Gets our position.
		 *
		 * \return Returns the position of the listener.
		 */
		const CVector3 & LSE_CALL						GetPos() const;

		/**
		 * Sets our position.
		 *
		 * \param _vPos Position to set.
		 */
		void LSE_CALL									SetPos( const CVector3 &_vPos );

		/**
		 * Gets our velocity.
		 *
		 * \return Returns the velocity of the listener.
		 */
		const CVector3 & LSE_CALL						GetVel() const;

		/**
		 * Sets our velocity.
		 *
		 * \param _vVel Velocity to set.
		 */
		void LSE_CALL									SetVel( const CVector3 &_vVel );

		/**
		 * Gets our forward orientation value.
		 *
		 * \return Returns the forward orientation value of the listener.
		 */
		const CVector3 & LSE_CALL						GetForward() const;

		/**
		 * Gets our up orientation value.
		 *
		 * \return Returns the up orientation value of the listener.
		 */
		const CVector3 & LSE_CALL						GetUp() const;

		/**
		 * Sets our orientation using forward/up vectors.
		 *
		 * \param _vFor Forward vector.
		 * \param _vUp Up vector.
		 */
		void LSE_CALL									SetOrientation( const CVector3 &_vFor, const CVector3 &_vUp );

		/**
		 * Gets our gain (volume).
		 *
		 * \return Returns the gain of the listener.
		 */
		LSSREAL LSE_CALL								GetGain() const;

		/**
		 * Sets our gain (volume).
		 *
		 * \param _fGain The new gain value.
		 */
		void LSE_CALL									SetGain( LSSREAL _fGain );

		/**
		 * Update the listener.  Just updates the hardware/sound system if dirty and clears the dirty flag.
		 */
		void LSE_CALL									Tick();


	protected :
		// == Members.
		/**
		 * Our position.
		 */
		CVector3										m_vPos;

		/**
		 * Our forward direction.  -Z = forward.
		 */
		CVector3										m_vForwardDir;

		/**
		 * Our up direction.  Y = up.
		 */
		CVector3										m_vUpDir;

		/**
		 * Our velocity.
		 */
		CVector3										m_vVel;

		/**
		 * Our gain (volume).  0 = nothing.  1 = full volume.
		 */
		LSSREAL											m_fGain;

		/**
		 * Dirty flag indicating we need to update on the next tick.
		 */
		LSBOOL											m_bDirty;


		// == Functions.
		/**
		 * Update our data.  Send it to the hardware or sound system.
		 *
		 * \return Returns true if there are no errors related to the sound system after the update.
		 */
		LSBOOL LSE_CALL									Update();

		/**
		 * Notifies us that the given device has become active and
		 *	we should send our buffer to it.  A return of false from this
		 *	function indicates the buffer could not be uploaded.  This is
		 *	considered a critical error by the engine.
		 *
		 * \return Returns true if the buffer could be reloaded.
		 */
		LSBOOL LSE_CALL									DeviceActivated();

		/**
		 * Notifies us that the device is about to be lost (changed).  We need
		 *	to sever our connections to it, although we may retain our data/
		 *	settings.
		 */
		void LSE_CALL									DeviceWillClose();

	private :
		friend class CSoundManager;
	};

}	// namespace lss

#endif	// __LSS_SOUNDLISTENER_H__
