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

#include "LSSSoundLib.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lss {

	// == Members.
#ifdef LSS_USEOPENAL
	// Do we have the ability to enumerate devices?
	LSBOOL CSoundLib::m_bEnumDevices = false;

	// String of device names.  Each name is terminated with a NULL.  The end of the list is
	//	terminated with 2 NULL's.
	const ALCchar * CSoundLib::m_pcDevices = "\0\0";

	// Total number of devices.
	uint32_t CSoundLib::m_ui32Devices = 0;

	// Longest device name (including its terminating NULL).
	uint32_t CSoundLib::m_ui32MaxDeviceNameLength = 1;

	// Various enumerated values indicating the number and arrangement of channels.
	// Various enumerated values indicating the number and arrangement of channels.
	CSoundLib::LSS_CHANNEL_ARRANGEMENT CSoundLib::m_caChannels[LSS_C_TOTAL] = {
		{ "AL_FORMAT_MONO8",			0,	8,	1, },
		{ "AL_FORMAT_MONO16",			0,	16,	1, },
		{ "AL_FORMAT_MONO_FLOAT32",		0,	32,	1, },
		{ "AL_FORMAT_STEREO8",			0,	8,	2, },
		{ "AL_FORMAT_STEREO16",			0,	16,	2, },
		{ "AL_FORMAT_STEREO_FLOAT32",	0,	32,	2, },
		{ "AL_FORMAT_MONO_IMA4",		0,	4,	1, },
		{ "AL_FORMAT_STEREO_IMA4",		0,	4,	2, },
		{ "AL_FORMAT_QUAD8_LOKI",		0,	8,	4, },
		{ "AL_FORMAT_QUAD16_LOKI",		0,	16,	4, },
		{ "AL_FORMAT_QUAD8",			0,	8,	4, },
		{ "AL_FORMAT_QUAD16",			0,	16,	4, },
		{ "AL_FORMAT_QUAD32",			0,	32,	4, },
		{ "AL_FORMAT_51CHN8",			0,	8,	6, },
		{ "AL_FORMAT_51CHN16",			0,	16,	6, },
		{ "AL_FORMAT_51CHN32",			0,	32,	6, },
		{ "AL_FORMAT_61CHN8",			0,	8,	7, },
		{ "AL_FORMAT_61CHN16",			0,	16,	7, },
		{ "AL_FORMAT_61CHN32",			0,	32,	7, },
		{ "AL_FORMAT_71CHN8",			0,	8,	8, },
		{ "AL_FORMAT_71CHN16",			0,	16,	8, },
		{ "AL_FORMAT_71CHN32",			0,	32,	8, },
		{ "AL_FORMAT_REAR8",			0,	8,	2, },
		{ "AL_FORMAT_REAR16",			0,	16,	2, },
		{ "AL_FORMAT_REAR32",			0,	32,	2, },
	};
#endif	// #ifdef LSS_USEOPENAL


	// == Functions.
	/**
	 * Initialize the sound library.
	 */
	void LSE_CALL CSoundLib::InitSoundLibrary() {

		static LSBOOL bInitialized = false;
		if ( bInitialized ) { return; }

		bInitialized = true;


#ifdef LSS_USEOPENAL
		m_bEnumDevices = ::alcIsExtensionPresent( NULL, "ALC_ENUMERATION_EXT" ) == AL_TRUE;

		if ( m_bEnumDevices ) {
			m_pcDevices = ::alcGetString( NULL, ALC_DEVICE_SPECIFIER );

			// Count the number of devices.
			const ALCchar * pcStart = m_pcDevices;
			m_ui32Devices = 0;
			m_ui32MaxDeviceNameLength = 0;	// Will always be at least 1 by the end of the loop.
			while ( true ) {
				uint32_t ui32Len = CStd::StrLen( pcStart );
				if ( ui32Len + 1 > m_ui32MaxDeviceNameLength ) {
					m_ui32MaxDeviceNameLength = static_cast<uint32_t>(ui32Len + 1);
				}
				if ( !ui32Len ) { break; }

				m_ui32Devices++;
				pcStart += (ui32Len + 1);
			}
		}
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Post-initialization of the sound library, after the game object has been created.
	 */
	void LSE_CALL CSoundLib::PostInitSoundLibrary() {
#ifdef LSS_USEOPENAL
		// Get the supported channel arrangement.
		static const char * pcChannels[] = {
			"AL_FORMAT_MONO8",
			"AL_FORMAT_MONO16",
			"AL_FORMAT_MONO_FLOAT32",
			"AL_FORMAT_STEREO8",
			"AL_FORMAT_STEREO16",
			"AL_FORMAT_STEREO_FLOAT32",
			"AL_FORMAT_MONO_IMA4",
			"AL_FORMAT_STEREO_IMA4",
			"AL_FORMAT_QUAD8_LOKI",
			"AL_FORMAT_QUAD16_LOKI",
			"AL_FORMAT_QUAD8",
			"AL_FORMAT_QUAD16",
			"AL_FORMAT_QUAD32",
			"AL_FORMAT_51CHN8",
			"AL_FORMAT_51CHN16",
			"AL_FORMAT_51CHN32",
			"AL_FORMAT_61CHN8",
			"AL_FORMAT_61CHN16",
			"AL_FORMAT_61CHN32",
			"AL_FORMAT_71CHN8",
			"AL_FORMAT_71CHN16",
			"AL_FORMAT_71CHN32",
			"AL_FORMAT_REAR8",
			"AL_FORMAT_REAR16",
			"AL_FORMAT_REAR32",
		};
		for ( uint32_t I = 0; I < LSE_ELEMENTS( pcChannels ); ++I ) {
			m_caChannels[I].ui32EnumValue = ::alGetEnumValue( pcChannels[I] );
		}
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Destroy the sound library.
	 */
	void LSE_CALL CSoundLib::ShutDownSoundLibrary() {
	}

	/**
	 * Gets the total number of sound devices.  Always 1 on consoles.
	 *
	 * \return Returns the total number of sound devices.
	 */
	uint32_t LSE_CALL CSoundLib::GetTotalDevices() {
#ifdef LSS_USEOPENAL
		return m_ui32Devices;
#else
		return 1;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Gets the length of the longest name among the devices.  The length includes the
	 *	terminating NULL.  Allocating a string of this length guarantees it is long
	 *	enough to contain any device name along with its terminating NULL.
	 *
	 * \return Returns the length of the longest name among the devices.
	 */
	uint32_t LSE_CALL CSoundLib::GetLongestDeviceName() {
#ifdef LSS_USEOPENAL
		return m_ui32MaxDeviceNameLength;
#else
		return 1;
#endif	// #ifdef LSS_USEOPENAL
	}

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
	LSBOOL LSE_CALL CSoundLib::GetDeviceNameByIndex( uint32_t _ui32Index,
		char * _pcNameReturn, uint32_t _ui32MaxLen ) {
		if ( _ui32MaxLen == 0 ) { return false; }

		if ( _ui32Index >= GetTotalDevices() ) { return false; }

#ifdef LSS_USEOPENAL
		// Step through the names until we reach the desired index.
		// Not a function that can be called in performance-demanding situations.
		int32_t i32Count = static_cast<int32_t>(_ui32Index);
		const ALCchar * pcStart = m_pcDevices;
		while ( i32Count-- > 0 ) {
			uint32_t ui32Len = CStd::StrLen( pcStart );
			pcStart += (ui32Len + 1);
		}

		// Now points to the correct substring.  Just copy it over.
		CStd::StrNCpy( _pcNameReturn, pcStart, _ui32MaxLen );
		_pcNameReturn[_ui32MaxLen-1] = '\0';
		return true;
#else
		_pcNameReturn[0] = '\0';
		return true;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Is the given speaker arrangement supported?  Use a LSS_C_* to check.
	 *
	 * \param _cArrangement Channel arrangement to test.
	 * \return Returns true if the given channel arrangement is supported.
	 */
	LSBOOL LSE_CALL CSoundLib::ChannelArrangementSupported( LSS_CHANNELS _cArrangement ) {
#ifdef LSS_USEOPENAL
		if ( _cArrangement >= LSS_C_TOTAL ) { return false; }
		return m_caChannels[_cArrangement].ui32EnumValue != 0;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Gets the OS-specific enumeration value for the given channel arrangement.
	 *
	 * \param _cArrangement Channel arrangement to convert to the library-specific value.
	 * \return Returns the library-specific channel enumeration.
	 */
	uint32_t LSE_CALL CSoundLib::GetOsChannelEnum( LSS_CHANNELS _cArrangement ) {
#ifdef LSS_USEOPENAL
		if ( _cArrangement >= LSS_C_TOTAL ) { return 0; }
		return m_caChannels[_cArrangement].ui32EnumValue;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Gets the number of bits per channel given an LSS_CHANNELS value.
	 *
	 * \param _cArrangement Arrangement whose bit count is to be obtained.
	 * \return Returns the number of bits per channel on the given arrangement.
	 */
	uint32_t LSE_CALL CSoundLib::GetBitsPerChannel( LSS_CHANNELS _cArrangement ) {
#ifdef LSS_USEOPENAL
		if ( _cArrangement >= LSS_C_TOTAL ) { return 0; }
		return m_caChannels[_cArrangement].ui32Bits;
#endif	// #ifdef LSS_USEOPENAL
	}

	/**
	 * Gets the number of channels given an LSS_CHANNELS value.
	 *
	 * \param _cArrangement Arrangement whose channel count is to be obtained.
	 * \return Returns the number of channels on the given arrangement.
	 */
	uint32_t LSE_CALL CSoundLib::GetChannels( LSS_CHANNELS _cArrangement ) {
#ifdef LSS_USEOPENAL
		if ( _cArrangement >= LSS_C_TOTAL ) { return 0; }
		return m_caChannels[_cArrangement].ui32Channels;
#endif	// #ifdef LSS_USEOPENAL
	}

}	// namespace lss

#pragma warning( pop )
