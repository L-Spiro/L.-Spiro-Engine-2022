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
 * Description: A static sound emenating from somewhere in the world.  The world positions are not
 *	defined here.  Does not use streaming.
 */


#ifndef __LSS_STATICSOUNDSOURCE_H__
#define __LSS_STATICSOUNDSOURCE_H__

#include "../LSSSoundLib.h"
#include "LSSSoundBuffer.h"
#include "LSSSoundSource.h"

namespace lss {

	/**
	 * Class CStaticSoundSource
	 *
	 * Description: A static sound emenating from somewhere in the world.  The world positions are not
	 *	defined here.  Does not use streaming.
	 */
	class CStaticSoundSource : public CSoundSource {
	public :
		// == Various constructors.
		LSE_CALLCTOR									CStaticSoundSource( uint32_t _ui32Id );
		virtual LSE_CALLCTOR							~CStaticSoundSource();


		// == Functions.
		/**
		 * Gets the type of this source.
		 *
		 * \return Returns one of the LSS_SOUND_SOURCE values.
		 */
		virtual LSS_SOUND_SOURCE LSE_CALL				GetType() const;

		/**
		 * Gets the ID of the buffer we reference, if any.
		 *
		 * \return Returns the unique ID of the buffer we reference, or CSoundSource::LSS_E_INVALID_BUFFER if there is none.
		 */
		uint32_t LSE_CALL								GetBufferId() const;

		/**
		 * Begin playing the sound.  If there is no buffer, returns false.  The sound plays regardless
		 *	of whether or not there is a sound device.  If there is no sound device, the sound will begin
		 *	playing at the correct position when a device becomes active.  The sound begins playing on the
		 *	next tick event.
		 *
		 * \param _ui32LoopCount Number of times to loop the sound.
		 * \return Returns true if the play was successful.  A return of false indicates that no buffer was
		 *	applied to this source.
		 */
		virtual LSBOOL LSE_CALL							Play( uint32_t _ui32LoopCount = 1 );

		/**
		 * Pause the sound.  If there is no buffer, returns false.
		 *
		 * \return Returns true if a buffer has been assigned to this source.
		 */
		virtual LSBOOL LSE_CALL							Pause();

		/**
		 * Stop the sound.  Sets the position back to 0 and waits for a play command.
		 *
		 * \return Returns true if a buffer has been assigned to this source.
		 */
		virtual LSBOOL LSE_CALL							Stop();

		/**
		 * Is this source static (non-streaming)?
		 *
		 * \return Returns true if the source is static, false if streaming.
		 */
		virtual LSBOOL LSE_CALL							Static() const;

		/**
		 * Sets the buffer.  Does not cause playing.  NULL is valid.
		 *
		 * \param _psbBuffer The possibly shared buffer to be used by this source.
		 */
		virtual void LSE_CALL							SetBuffer( const CSoundBuffer * _psbBuffer );

		/**
		 * Update the source.  This usually entails checking the dirty flag and updating the hardware accordingly.
		 * Time is given in microseconds.
		 *
		 * \param _ui64TimeSinceLastUpdate Microseconds by which to update.
		 */
		virtual void LSE_CALL							Tick( uint64_t _ui64TimeSinceLastUpdate );

	protected :
		// == Members.
		/**
		 * The buffer to which we are attached.
		 */
		const CSoundBuffer *							m_psbBuffer;

		/**
		 * Is our play state dirty?
		 */
		LSBOOL											m_bPlayStateDirty;


		// == Functions.
		/**
		 * Notifies us that the given device has become active and
		 *	we should send our buffer to it.  A return of false from this
		 *	function indicates the buffer could not be uploaded.  This is
		 *	considered a critical error by the engine.
		 *
		 * \return Returns true if the buffer could be reloaded.
		 */
		virtual LSBOOL LSE_CALL							DeviceActivated();

		/**
		 * Update the play state.
		 */
		virtual void LSE_CALL							UpdatePlayState();

	private :
		typedef CSoundSource							Parent;
	};

}	// namespace lss

#endif	// __LSS_STATICSOUNDSOURCE_H__
