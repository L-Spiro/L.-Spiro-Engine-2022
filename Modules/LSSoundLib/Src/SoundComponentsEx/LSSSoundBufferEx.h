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


#ifndef __LSS_SOUNDBUFFEREX_H__
#define __LSS_SOUNDBUFFEREX_H__

#include "../LSSSoundLib.h"
#include "../SoundComponents/LSSSoundBuffer.h"

namespace lss {

	/**
	 * Class CSoundBufferEx
	 * \brief Extending the sound buffer with the ability to load files.
	 *
	 * Description: Adding the ability to load data from files to the sound engine.  We use the user parameter
	 *	passed to the constructor (_uiptrUserData) as a char * indicating the file to load.  The name of the
	 *	file is copied locally.
	 */
	class CSoundBufferEx : public CSoundBuffer {
		// Only the sound manager can create us.
		friend class							CSoundManagerEx;

	public :
		// == Various constructors.
		virtual LSE_CALLCTOR					~CSoundBufferEx();


		// == Functions.
		/**
		 * Gets our path.
		 *
		 * \return Returns the path to the buffer.
		 */
		LSE_INLINE const char * LSE_CALL		GetPath() const;

		/**
		 * Sets the value indicating whether this buffer should resist soft unloads or not.  A soft unload is
		 *	an unload that is used to save RAM even though it is assumed the buffer may be needed (and will be
		 *	reloaded) later.  A hard unload is performed to force all buffers out of memory.  A hard unload
		 *	always destroys and releases the buffer, whereas a soft unload is optional and can only be done on
		 *	buffers with a zero reference count.  For a soft unload to work, there must be a way to restore the
		 *	buffer to its current state after being completely unloaded.  The core library adds the ability to
		 *	load sound data from files, which provides this ability.
		 * Soft unloading is used with registered sound effects that are set to load on demand.
		 *
		 * \return Returns true if this buffer is allowed to softly unload or false if not.
		 */
		virtual LSBOOL LSH_CALL					CanSoftUnload() const;

		/**
		 * Allow or disallow soft unloading.
		 *
		 * \param _bValue Value indicating whether soft unloading is allowed or not.
		 */
		void LSE_CALL							SetSoftUnloading( LSBOOL _bValue );


	protected :
		// == Various constructors.
		LSE_CALLCTOR							CSoundBufferEx( const uint8_t * _pui8Data, uint32_t _ui32Length,
			uint32_t _ui32Freq, uint32_t _ui32Bits, uint32_t _ui32Channels, CAllocator * _paAllocator, LSBOOL _bRetain, uintptr_t _uiptrUserData );


		// == Members.
		// The file we load.
		char *								m_pcFile;

		// Do we permit soft unloading?
		LSBOOL									m_bSoftUnload;


	private :
		// == Functions.
		/**
		 * When needed, get the original bytes from the file passed to us
		 *	via _uiptrUserData in the constructor.
		 *
		 * \return Returns true if the original data was able to be retrieved.
		 */
		virtual LSBOOL LSH_CALL					GetOriginalData();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets our path.
	 *
	 * \return Returns the path to the buffer.
	 */
	LSE_INLINE const char * LSE_CALL CSoundBufferEx::GetPath() const {
		return m_pcFile;
	}

}	// namespace lss

#endif	// __LSS_SOUNDBUFFEREX_H__

