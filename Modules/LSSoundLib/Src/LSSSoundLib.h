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
 * Description: The primary include file for the sound library.  All files in the sound library include this
 *	one.
 */


#ifndef __LSS_SOUNDLIB_H__
#define __LSS_SOUNDLIB_H__

// Include the memory library.
#include "LSAMemLib.h"
using namespace ::lsa;

// Include the file library.
#include "LSFFileLib.h"
using namespace ::lsf;

// Include the thread library.
#include "LSHThreadLib.h"
using namespace ::lsh;

// Include the math library.
#include "LSMMathLib.h"
using namespace ::lsm;

// Include the standard library.
#include "LSSTDStandardLib.h"
using namespace ::lsstd;

// Include the LSTL.
#include "LSTLib.h"
using namespace ::lstl;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// OPTIONS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// Do we use OpenAL for this sound implementation?
#define LSS_USEOPENAL

#ifdef LSS_USEOPENAL
#if defined( LSE_IPHONE ) || defined( LSE_MAC )
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include "al.h"
#include "alc.h"
#endif	// #ifdef LSE_IPHONE
#endif	// #ifdef LSS_USEOPENAL


namespace lss {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// TYPES
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// Define the precision used in our library.
#ifdef LSS_USEOPENAL
	typedef ALfloat							LSSREAL;
#else	// #ifdef LSS_USEOPENAL
	typedef LSSFLOAT						LSSREAL;
#endif	// #ifdef LSS_USEOPENAL

	/**
	 * Class CSoundLib
	 *
	 * Description: The core of the sound library.
	 */
	class CSoundLib {
	public :
		// == Enumerations.
		/**
		 * Channel types and arrangements.
		 */
		enum LSS_CHANNELS {
			LSS_C_MONO8,
			LSS_C_MONO16,
			LSS_C_MONO_FLOAT32,
			LSS_C_STEREO8,
			LSS_C_STEREO16,
			LSS_C_STEREO_FLOAT32,
			LSS_C_MONO_IMA4,
			LSS_C_STEREO_IMA4,
			LSS_C_QUAD8_LOKI,
			LSS_C_QUAD16_LOKI,
			LSS_C_QUAD8,
			LSS_C_QUAD16,
			LSS_C_QUAD32,
			LSS_C_51CHN8,
			LSS_C_51CHN16,
			LSS_C_51CHN32,
			LSS_C_61CHN8,
			LSS_C_61CHN16,
			LSS_C_61CHN32,
			LSS_C_71CHN8,
			LSS_C_71CHN16,
			LSS_C_71CHN32,
			LSS_C_REAR8,
			LSS_C_REAR16,
			LSS_C_REAR32,
			LSS_C_TOTAL
		};


		// == Functions.
		/**
		 * Initialize the sound library.
		 */
		static void LSE_CALL				InitSoundLibrary();

		/**
		 * Post-initialization of the sound library, after the game object has been created.
		 */
		static void LSE_CALL				PostInitSoundLibrary();

		/**
		 * Destroy the sound library.
		 */
		static void LSE_CALL				ShutDownSoundLibrary();

		/**
		 * Gets the total number of sound devices.  Always 1 on consoles.
		 *
		 * \return Returns the total number of sound devices.
		 */
		static uint32_t LSE_CALL			GetTotalDevices();

		/**
		 * Gets the length of the longest name among the devices.  The length includes the
		 *	terminating NULL.  Allocating a string of this length guarantees it is long
		 *	enough to contain any device name along with its terminating NULL.
		 *
		 * \return Returns the length of the longest name among the devices.
		 */
		static uint32_t LSE_CALL			GetLongestDeviceName();

		/**
		 * Gets a device by index.  The returned value is the string name of the device.
		 * _pcNameReturn should be at least GetLongestDeviceName() characters long.
		 *	The actual length is given in _ui32MaxLen.  Writing to _pcNameReturn will
		 *	not exceed this length.  The returned string is always appended with a terminating
		 *	NULL unless it is of 0 length, in which case false is returned.
		 * False is also returned if the index is out of range.
		 *
		 * \param _ui32Index Index of the device to get.
		 * \param _pcNameReturn Returned device name.
		 * \param _ui32MaxLen Length of _pcNameReturn.
		 * \return Returns true if the index is valid.
		 */
		static LSBOOL LSE_CALL				GetDeviceNameByIndex( uint32_t _ui32Index,
			char * _pcNameReturn, uint32_t _ui32MaxLen );

		/**
		 * Is the given speaker arrangement supported?  Use a LSS_C_* to check.
		 *
		 * \param _cArrangement Channel arrangement to test.
		 * \return Returns true if the given channel arrangement is supported.
		 */
		static LSBOOL LSE_CALL				ChannelArrangementSupported( LSS_CHANNELS _cArrangement );

		/**
		 * Gets the OS-specific enumeration value for the given channel arrangement.
		 *
		 * \param _cArrangement Channel arrangement to convert to the library-specific value.
		 * \return Returns the library-specific channel enumeration.
		 */
		static uint32_t LSE_CALL			GetOsChannelEnum( LSS_CHANNELS _cArrangement );

		/**
		 * Gets the number of bits per channel given an LSS_CHANNELS value.
		 *
		 * \param _cArrangement Arrangement whose bit count is to be obtained.
		 * \return Returns the number of bits per channel on the given arrangement.
		 */
		static uint32_t LSE_CALL			GetBitsPerChannel( LSS_CHANNELS _cArrangement );

		/**
		 * Gets the number of channels given an LSS_CHANNELS value.
		 *
		 * \param _cArrangement Arrangement whose channel count is to be obtained.
		 * \return Returns the number of channels on the given arrangement.
		 */
		static uint32_t LSE_CALL			GetChannels( LSS_CHANNELS _cArrangement );

	protected :
		// == Types.
#ifdef LSS_USEOPENAL
		/**
		 * A type that holds channel/bit-depth/enum strings/etc. we will need.
		 */
		typedef struct LSS_CHANNEL_ARRANGEMENT {
			/**
			 * Name of the OpenAL enumeration.
			 */
			const char *					pcEnumName;

			/**
			 * Value of the enumeration.
			 */
			uint32_t						ui32EnumValue;

			/**
			 * Bits.
			 */
			uint32_t						ui32Bits;
			
			/**
			 * Channels.
			 */
			uint32_t						ui32Channels;
		} * LPLSS_CHANNEL_ARRANGEMENT, * const LPCLSS_CHANNEL_ARRANGEMENT;
#endif	// #ifdef LSS_USEOPENAL

		// == Members.

#ifdef LSS_USEOPENAL
		// Do we have the ability to enumerate devices?
		static LSBOOL						m_bEnumDevices;

		// String of device names.  Each name is terminated with a NULL.  The end of the list is
		//	terminated with 2 NULL's.
		static const ALCchar *				m_pcDevices;

		// Total number of devices.
		static uint32_t						m_ui32Devices;

		// Longest device name (including its terminating NULL).
		static uint32_t						m_ui32MaxDeviceNameLength;

		// Various enumerated values indicating the number and arrangement of channels.
		static LSS_CHANNEL_ARRANGEMENT		m_caChannels[LSS_C_TOTAL];

#endif	// #ifdef LSS_USEOPENAL

	};

}	// namespace lss

#endif	// __LSS_SOUNDLIB_H__
