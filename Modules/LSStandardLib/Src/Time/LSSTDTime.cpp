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

#include "LSSTDTime.h"
#include "../LargeInteger/LSSTD128BitInteger.h"
#if defined( LSE_WINDOWS )
#include "../Windows/LSSTDWindows.h"
#include <time.h>				// ::clock() fallback.
#endif	// #if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )


namespace lsstd {

	// == Various constructors.
	LSE_CALLCTOR CTime::CTime() :
		m_ui64CurTime( 0ULL ),
		m_ui64LastTime( 0ULL ),
		m_ui64CurVirtTime( 0ULL ),
		m_ui64LastVirtTime( 0ULL ),
		m_ui32Frame( 0 ),
		m_ui32VirtFrame( 0 ),
		m_ui64CurMicros( 0ULL ),
		m_ui64LastMicros( 0ULL ),
		m_ui64CurVirtMicros( 0ULL ),
		m_ui64LastVirtMicros( 0ULL ) {
#if defined( LSE_WINDOWS )
		m_bHiRes = ::QueryPerformanceFrequency( reinterpret_cast<LARGE_INTEGER *>(&m_ui64Resolution) );
		if ( !m_bHiRes ) {
			m_ui64Resolution = CLOCKS_PER_SEC;
		}
#elif defined( LSSTD_TIME_MAC )
		::mach_timebase_info( &m_mtidTimeData );
		m_ui64Resolution = 1000ULL * m_mtidTimeData.numer / m_mtidTimeData.denom;
#endif	// #if defined( LSE_WINDOWS )
		
		m_ui64LastRealTime = GetRealTime();
	}

	// == Functions.
	/**
	 * Reset the time.  Causes it to begin ticking all values from 0.
	 */
	void LSE_CALL CTime::Reset() {
		m_ui64LastRealTime = GetRealTime();
		m_ui64CurTime = m_ui64LastTime = 0ULL;
		m_ui64CurVirtTime = m_ui64LastVirtTime = 0ULL;
		m_ui32Frame = 0;
		m_ui32VirtFrame = 0;

		m_ui64CurMicros = m_ui64LastMicros = 0ULL;
		m_ui64CurVirtMicros = m_ui64LastVirtMicros = 0ULL;
	}

	/**
	 * Update the time.  Internally calls UpdateFullyBy() with the correct amount of time.
	 *
	 * \param _bUpdateVirtuals If true, virtual values are updated as well.
	 */
	void LSE_CALL CTime::Update( LSBOOL _bUpdateVirtuals ) {
		uint64_t ui64TimeNow = GetRealTime();
		uint64_t ui64Dif = ui64TimeNow - m_ui64LastRealTime;
		
		UpdateFullyBy( ui64Dif, _bUpdateVirtuals );
	}

	/**
	 * Updates the time by a given amount.  The update includes internal place-holding values.
	 *
	 * \param _ui64Amnt Number of ticks by which to update the time.
	 * \param _bUpdateVirtuals If true, virtual values are updated as well.
	 */
	void LSE_CALL CTime::UpdateFullyBy( uint64_t _ui64Amnt, LSBOOL _bUpdateVirtuals ) {
		m_ui64LastRealTime += _ui64Amnt;
		UpdateBy( _ui64Amnt, _bUpdateVirtuals );
	}

	/**
	 * Update the time by a certain amount.
	 *
	 * \param _ui64Amnt Number of ticks by which to update the time.
	 * \param _bUpdateVirtuals If true, virtual values are updated as well.
	 */
	void LSE_CALL CTime::UpdateBy( uint64_t _ui64Amnt, LSBOOL _bUpdateVirtuals ) {
		m_ui64LastTime = m_ui64CurTime;
		m_ui64CurTime += _ui64Amnt;
		
		// Always update the counters for the last frame.
		m_ui64LastVirtTime = m_ui64CurVirtTime;
		m_ui64LastVirtMicros = m_ui64CurVirtMicros;
		++m_ui32Frame;

		// Optionally update virtuals.
		if ( _bUpdateVirtuals ) {
			m_ui64CurVirtTime += _ui64Amnt;
			++m_ui32VirtFrame;


			//CLargeInteger<3> liValue( m_ui64CurVirtTime );
			C128BitInteger biValue( m_ui64CurVirtTime );
#if defined( LSSTD_TIME_MAC )
			biValue *= m_ui64Resolution;
			biValue /= 1000000ULL;
#else
			biValue *= 1000000ULL;
			biValue /= m_ui64Resolution;
#endif	// #if defined( LSSTD_TIME_MAC );
			m_ui64CurVirtMicros = (*reinterpret_cast<uint64_t *>(&biValue));//biValue.GetAs64bitUInt();
		}

		//CLargeInteger<3> liValue( m_ui64CurTime );
		C128BitInteger biValue( m_ui64CurTime );
		m_ui64LastMicros = m_ui64CurMicros;
#if defined( LSSTD_TIME_MAC )
		biValue *= m_ui64Resolution;
		biValue /= 1000000ULL;
#else
		biValue *= 1000000ULL;
		biValue /= m_ui64Resolution;
#endif	// #if defined( LSSTD_TIME_MAC );
		m_ui64CurMicros = (*reinterpret_cast<uint64_t *>(&biValue));//biValue.GetAs64bitUInt();
	}

	/**
	 * Converts microseconds to tick values used by this timer internally.
	 *
	 * \param _ui64Amnt The value to convert.
	 * \return Returns the number of ticks that roughly equal the given amount of microseconds.
	 */
	uint64_t LSE_CALL CTime::MicrosToTicks( uint64_t _ui64Amnt ) const {
#if defined( LSSTD_TIME_MAC )
		return _ui64Amnt * 1000000ULL / m_ui64Resolution;
#else
		return _ui64Amnt * m_ui64Resolution / 1000000ULL;
#endif	// #if defined( LSSTD_TIME_MAC )
	}

	/**
	 * Converts a tick value to microseconds.
	 *
	 * \param _ui64Amnt The value to convert.
	 * \return Returns the number of microseconds that roughly equal the given amount of ticks.
	 */
	uint64_t LSE_CALL CTime::TicksToMicros( uint64_t _ui64Amnt ) const {
#if defined( LSSTD_TIME_MAC )
		return _ui64Amnt * m_ui64Resolution / 1000000ULL;
#else
		return _ui64Amnt * 1000000ULL / m_ui64Resolution;
#endif	// #if defined( LSSTD_TIME_MAC )
	}

	/**
	 * Gets the real system time.  Not to be used for any other purpose besides random-number
	 *	seeding.
	 *
	 * \return Returns the real system time in system tick units.
	 */
	uint64_t LSE_CALL CTime::GetRealTime() const {
#if defined( LSE_WINDOWS )
		if ( m_bHiRes ) {
			uint64_t ui64Ret;
			if ( ::QueryPerformanceCounter( reinterpret_cast<LARGE_INTEGER *>(&ui64Ret) ) ) { return ui64Ret; }
		}
		return ::clock();
#elif defined( LSSTD_TIME_MAC )
		return ::mach_absolute_time();
#else
#error "No implementation for CTime on this platform."
#endif	// #if defined( LSE_WINDOWS )
	}

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
	uint32_t LSE_CALL CTime::GetConstantStepUpdateTimesFromTicks( uint64_t _ui64StartRealTime, uint64_t _ui64CurRealTime,
		uint64_t _ui64Ticks, uint32_t &_ui32UnitsToNextUpdate ) {
		uint64_t ui64Dif = _ui64CurRealTime - _ui64StartRealTime;

		uint32_t ui32Count = static_cast<uint32_t>(ui64Dif / _ui64Ticks);

		// Determine how far this update lands between two other updates at the same fixed interval,
		//	in units of 1 1,000th.
		_ui32UnitsToNextUpdate = static_cast<uint32_t>(((ui64Dif % _ui64Ticks) * 1000ULL) / _ui64Ticks);
		return ui32Count;
	}

}	// namespace lsstd
