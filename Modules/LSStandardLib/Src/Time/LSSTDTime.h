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
 * Description: Cross-platform time-management.  Tracks time in microseconds (1,000,000 per second).
 *	Also provides frame count and virtual time/frame values for pausing.  When paused, virtual
 *	values re not updated, causing any objects/systems using the virtual time values to stop
 *	(effectively pausing them).  Use the regular time values for systems that cannot be paused,
 *	such as the menus, which should flash and animate regardless of the pause status.
 */


#ifndef __LSSTD_TIME_H__
#define __LSSTD_TIME_H__

#include "../LSSTDStandardLib.h"

#if defined( MACOS_X ) || defined( __APPLE__ )
// Macintosh.
#include <mach/mach.h>
#include <mach/mach_time.h>
#define LSSTD_TIME_MAC

#endif	// #if defined( MACOS_X ) || defined( __APPLE__ )

namespace lsstd {

	/**
	 * Class CTime
	 * \brief High-resolution time class.
	 *
	 * Description: Cross-platform time-management.  Tracks time in microseconds (1,000,000 per second).
	 *	Also provides frame count and virtual time/frame values for pausing.  When paused, virtual
	 *	values re not updated, causing any objects/systems using the virtual time values to stop
	 *	(effectively pausing them).  Use the regular time values for systems that cannot be paused,
	 *	such as the menus, which should flash and animate regardless of the pause status.
	 */
	class CTime {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CTime();

		// == Functions.
		/**
		 * Gets the current microseconds.
		 *
		 * \return Returns an uint64_t value containing the current microseconds.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurMicros() const;

		/**
		 * Gets the current virtual microseconds.
		 *
		 * \return Returns an uint64_t value containing the current virtual microseconds
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurVirtMicros() const;
		
		/**
		 * Gets the current milliseconds.
		 *
		 * \return Returns an uint64_t value containing the current milliseconds.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurMills() const;
		
		/**
		 * Gets the current virtual milliseconds.
		 *
		 * \return Returns an uint64_t value containing the current virtual milliseconds.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurVirtMills() const;

		/**
		 * Gets the current frame.
		 *
		 * \return Returns the frame count.
		 */
		LSE_INLINE uint32_t LSE_CALL		GetCurFrame() const;
		
		/**
		 * Gets the current virtual frame.
		 *
		 * \return Returns the virtual frame count.
		 */
		LSE_INLINE uint32_t LSE_CALL		GetCurVirtFrame() const;

		/**
		 * Gets the microseconds since last frame.
		 *
		 * \return Returns the number of microseconds since the last frame.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetMicrosSinceLastFrame() const;
		
		/**
		 * Gets the virtual microseconds since last frame.
		 *
		 * \return Returns the number of virtual microseconds since the last frame.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetVirtMicrosSinceLastFrame() const;
		
		/**
		 * Gets the milliseconds since last frame.
		 *
		 * \return Returns the number of milliseconds since the last frame.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetMillsSinceLastFrame() const;
		
		/**
		 * Gets the virtual milliseconds since last frame.
		 *
		 * \return Returns the number of virtual milliseconds since the last frame.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetVirtMillsSinceLastFrame() const;

		/**
		 * Gets the seconds since last frame as a floating-point value.
		 *
		 * \return Returns the number of seconds since the last frame.
		 */
		LSE_INLINE float LSE_CALL			GetSecondsSinceLastFrame() const;
		
		/**
		 * Gets the virtual seconds since last frame as a floating-point value..
		 *
		 * \return Returns the number of virtual seconds since the last frame.
		 */
		LSE_INLINE float LSE_CALL			GetVirtSecondsSinceLastFrame() const;
		
		/**
		 * Gets the actual amount of time passed since last frame.  This value should never be used.
		 *	The amount of time it measures changes per device/hardware.  It is not useful for telling
		 *	time.
		 *
		 * \return Returns the number of ticks since the last frame.  Resolution depends on hardware.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetTicksSinceLastFrame() const;	

		/**
		 * Gets the current number of ticks.  Ticks are machine-dependent and should not be used for
		 *	tracking time.
		 *
		 * \return Returns the current tick count of this timer.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurTicks() const;

		/**
		 * Reset the time.  Causes it to begin ticking all values from 0.
		 */
		void LSE_CALL						Reset();

		/**
		 * Update the time.  Internally calls UpdateFullyBy() with the correct amount of time.
		 *
		 * \param _bUpdateVirtuals If true, virtual values are updated as well.
		 */
		void LSE_CALL						Update( LSBOOL _bUpdateVirtuals );

		/**
		 * Updates the time by a given amount.  The update includes internal place-holding values.
		 *
		 * \param _ui64Amnt Number of ticks by which to update the time.
		 * \param _bUpdateVirtuals If true, virtual values are updated as well.
		 */
		void LSE_CALL						UpdateFullyBy( uint64_t _ui64Amnt, LSBOOL _bUpdateVirtuals );

		/**
		 * Update the time by a certain amount.
		 *
		 * \param _ui64Amnt Number of ticks by which to update the time.
		 * \param _bUpdateVirtuals If true, virtual values are updated as well.
		 */
		void LSE_CALL						UpdateBy( uint64_t _ui64Amnt, LSBOOL _bUpdateVirtuals );

		/**
		 * Converts microseconds to tick values used by this timer internally.
		 *
		 * \param _ui64Amnt The value to convert.
		 * \return Returns the number of ticks that roughly equal the given amount of microseconds.
		 */
		uint64_t LSE_CALL					MicrosToTicks( uint64_t _ui64Amnt ) const;

		/**
		 * Converts a tick value to microseconds.
		 *
		 * \param _ui64Amnt The value to convert.
		 * \return Returns the number of microseconds that roughly equal the given amount of ticks.
		 */
		uint64_t LSE_CALL					TicksToMicros( uint64_t _ui64Amnt ) const;

		/**
		 * Gets the real system time.  Not to be used for any other purpose besides random-number
		 *	seeding.
		 *
		 * \return Returns the real system time in system tick units.
		 */
		uint64_t LSE_CALL					GetRealTime() const;

		/**
		 * Gets the internal current real time value.  The value is used internally for keeping track
		 *	of how long to make updates inside Update().
		 *
		 * \return Returns the internal current real time value.
		 */
		LSE_INLINE uint64_t LSE_CALL		GetCurrentRealTime() const;

		/**
		 * Gets the number of cycles and amount of tick updates needed to advance the time by the constant time step
		 *	given.
		 *
		 * \param _ui64StartRealTime The starting real time value (taken by calling GetCurrentRealTime() on the timer
		 *	that is to be updated by the given constant time step).
		 * \param _ui64CurRealTime the current real time (taken by calling GetCurrentRealTime() on a master timer up
		 *	to which the first timer is to catch by stepping in a fixed-step fashion).
		 * \param _ui64Ticks Number of ticks by which to update the time.  Must be a constant number,
		 *	used for every call to this function for a given instance.  The time is not updated by this amount, but
		 *	instead returns the number of updates needed to make the timer approximately current by this value.
		 * \param _ui32UnitsToNextUpdate Holds a returned value indicating the number of 1,000th units that would need
		 *	to pass for another update to happen at the given tick rate.
		 * \return Returns the number of iterations needed to advance the time so that, if advanced by the given
		 *	tick amount by the returned number of times, the timer will be approximately current.
		 */
		static uint32_t LSE_CALL			GetConstantStepUpdateTimesFromTicks( uint64_t _ui64StartRealTime, uint64_t _ui64CurRealTime,
			uint64_t _ui64Ticks, uint32_t &_ui32UnitsToNextUpdate );


	protected :
		// == Members.
		/**
		 * Timer resolution.
		 */
		uint64_t							m_ui64Resolution;

		/**
		 * Current time (starts at 0).
		 */
		uint64_t							m_ui64CurTime;

		/**
		 * Time last update.
		 */
		uint64_t							m_ui64LastTime;

		/**
		 * Virtual ticks.
		 */
		uint64_t							m_ui64CurVirtTime;
		uint64_t							m_ui64LastVirtTime;

		/**
		 * Record of the last real time (to handle integer overflow).
		 */
		uint64_t							m_ui64LastRealTime;

		/**
		 * Current microseconds.
		 */
		uint64_t							m_ui64CurMicros;

		/**
		 * Last microseconds.
		 */
		uint64_t							m_ui64LastMicros;

		/**
		 * Current virtual microseconds.
		 */
		uint64_t							m_ui64CurVirtMicros;

		/**
		 * Last virtual microseconds.
		 */
		uint64_t							m_ui64LastVirtMicros;

		/**
		 * Frame count.
		 */
		uint32_t							m_ui32Frame;
		uint32_t							m_ui32VirtFrame;

		/**
		 * Are we high-resolution enabled?
		 */
		LSBOOL								m_bHiRes;
		
#ifdef LSSTD_TIME_MAC
		mach_timebase_info_data_t			m_mtidTimeData;
#endif	// #ifdef LSSTD_TIME_MAC
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the current microseconds.
	 *
	 * \return Returns an uint64_t value containing the current microseconds.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurMicros() const {
		return m_ui64CurMicros;
	}
	
	/**
	 * Gets the current virtual microseconds.
	 *
	 * \return Returns an uint64_t value containing the current virtual microseconds
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurVirtMicros() const {
		return m_ui64CurVirtMicros;
	}
	
	/**
	 * Gets the current milliseconds.
	 *
	 * \return Returns an uint64_t value containing the current milliseconds.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurMills() const {
		return m_ui64CurMicros / 1000ULL;
	}
	
	/**
	 * Gets the current virtual milliseconds.
	 *
	 * \return Returns an uint64_t value containing the current virtual milliseconds.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurVirtMills() const {
		return m_ui64CurVirtMicros / 1000ULL;
	}

	/**
	 * Gets the current frame.
	 *
	 * \return Returns the frame count.
	 */
	LSE_INLINE uint32_t LSE_CALL CTime::GetCurFrame() const {
		return m_ui32Frame;
	}
	
	/**
	 * Gets the current virtual frame.
	 *
	 * \return Returns the virtual frame count.
	 */
	LSE_INLINE uint32_t LSE_CALL CTime::GetCurVirtFrame() const {
		return m_ui32VirtFrame;
	}

	/**
	 * Gets the microseconds since last frame.
	 *
	 * \return Returns the number of microseconds since the last frame.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetMicrosSinceLastFrame() const {
		return GetCurMicros() - m_ui64LastMicros;
	}
	
	/**
	 * Gets the virtual microseconds since last frame.
	 *
	 * \return Returns the number of virtual microseconds since the last frame.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetVirtMicrosSinceLastFrame() const {
		return GetCurVirtMicros() - m_ui64LastVirtMicros;
	}
	
	/**
	 * Gets the milliseconds since last frame.
	 *
	 * \return Returns the number of milliseconds since the last frame.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetMillsSinceLastFrame() const {
		return GetCurMills() - m_ui64LastMicros / 1000ULL;
	}
	
	/**
	 * Gets the virtual milliseconds since last frame.
	 *
	 * \return Returns the number of virtual milliseconds since the last frame.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetVirtMillsSinceLastFrame() const {
		return GetCurVirtMills() - m_ui64CurVirtMicros / 1000ULL;
	}

	/**
	 * Gets the seconds since last frame as a floating-point value.
	 *
	 * \return Returns the number of seconds since the last frame.
	 */
	LSE_INLINE float LSE_CALL CTime::GetSecondsSinceLastFrame() const {
		return GetMicrosSinceLastFrame() * (1.0f / 1000000.0f);
	}
	
	/**
	 * Gets the virtual seconds since last frame as a floating-point value..
	 *
	 * \return Returns the number of virtual seconds since the last frame.
	 */
	LSE_INLINE float LSE_CALL CTime::GetVirtSecondsSinceLastFrame() const {
		return GetVirtMicrosSinceLastFrame() * (1.0f / 1000000.0f);
	}
	
	/**
	 * Gets the actual amount of time passed since last frame.  This value should never be used.
	 *	The amount of time it measures changes per device/hardware.  It is not useful for telling
	 *	time.
	 *
	 * \return Returns the number of ticks since the last frame.  Resolution depends on hardware.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetTicksSinceLastFrame() const {
		return m_ui64CurTime - m_ui64LastTime;
	}

	/**
	 * Gets the current number of ticks.  Ticks are machine-dependent and should not be used for
	 *	tracking time.
	 *
	 * \return Returns the current tick count of this timer.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurTicks() const {
		return m_ui64CurTime;
	}

	/**
	 * Gets the internal current real time value.  The value is used internally for keeping track
	 *	of how long to make updates inside Update().
	 *
	 * \return Returns the internal current real time value.
	 */
	LSE_INLINE uint64_t LSE_CALL CTime::GetCurrentRealTime() const {
		return m_ui64LastRealTime;
	}

}	// namespace lsstd

#endif	// __LSSTD_TIME_H__
