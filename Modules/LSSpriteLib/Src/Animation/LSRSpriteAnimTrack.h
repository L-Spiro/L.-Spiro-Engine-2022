/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A single track linearly interpolates between 2 values.  This template allows specialization on
 *	the type of value being interpolated and the "dirty" flag if any.  Internally, a float is always used for
 *	interpolation, but is cast to the specialization type upon assignment to the target value.  The dirty flag
 *	is an optional value to update when the interpolated value is modified.  For example, orientations use a
 *	dirty flag to avoid generating matrices redundantly.  The matrix dirty flag would be passed here so that
 *	it can signal to the orientation class that something has changed.
 */


#ifndef __LSR_SPRITEANIMTRACK_H__
#define __LSR_SPRITEANIMTRACK_H__

#include "../LSRSpriteLib.h"
#include "../Animation/LSRSpriteAnimDataStructs.h"

namespace lsr {

	/**
	 * Class CSpriteAnimTrack
	 * \brief A single track linearly interpolates between 2 values.
	 *
	 * Description: A single track linearly interpolates between 2 values.  This template allows specialization on
	 *	the type of value being interpolated and the "dirty" flag if any.  Internally, a float is always used for
	 *	interpolation, but is cast to the specialization type upon assignment to the target value.  The dirty flag
	 *	is an optional value to update when the interpolated value is modified.  For example, orientations use a
	 *	dirty flag to avoid generating matrices redundantly.  The matrix dirty flag would be passed here so that
	 *	it can signal to the orientation class that something has changed.
	 */
	template <typename _tTargetType, typename _tDirtyFlagType, unsigned _uDirtyValue = 0>
	class CSpriteAnimTrack {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSpriteAnimTrack() :
			m_fStart( LSM_ZERO ),
			m_fEnd( LSM_ZERO ),
			m_fLastFrac( -LSM_ONE ),
			m_ui64Duration( 0ULL ),
			m_ui64Pad( 0ULL ),
			m_fInvDur( LSM_ZERO ),
			m_bActive( true ) {
		}


		// == Functions.
		/**
		 * Sets a track's from, to, and duration data.
		 *
		 * \param _tFrom The starting value of the track.
		 * \param _tTo The ending value of the track.
		 * \param _ui64Duration The duration of the track.
		 * \param _ui64PostWait How long after _ui64Duration the track stays alive (the track will write _tTo to the target value during this period).
		 */
		void LSE_CALL							SetTrack( const _tTargetType &_tFrom, const _tTargetType &_tTo, uint64_t _ui64Duration, uint64_t _ui64PostWait = 0ULL ) {
			m_fStart = static_cast<LSREAL>(_tFrom);
			m_fEnd = static_cast<LSREAL>(_tTo);
			m_fRange = m_fEnd - m_fStart;
			m_fLastFrac = -LSM_ONE;
			m_ui64Duration = _ui64Duration;
			m_ui64Pad = _ui64PostWait;
			if ( m_ui64Duration ) {
				m_fInvDur = LSREAL( 1.0 / m_ui64Duration );
			}
			else {
				m_fInvDur = LSM_ZERO;
			}
		}

		/**
		 * Sets a track's data.
		 *
		 * \param _satData The track data to set.
		 */
		void LSE_CALL							SetTrack( const LSR_SPRITE_ANIM_TRACK &_satData ) {
			SetTrack( static_cast<const _tTargetType &>(_satData.fFrom),
				static_cast<const _tTargetType &>(_satData.fTo),
				_satData.ui64Duration, _satData.ui64Padding );
		}

		/**
		 * Sets the track's post-wait time.
		 *
		 * \param _ui64PostWait How long after the duration the track stays alive.
		 */
		void LSE_CALL							SetPostDuration( uint64_t _ui64PostWait ) {
			m_ui64Pad = _ui64PostWait;
		}

		/**
		 * Advances the track by a given number of microseconds.
		 *
		 * \param _ui64Micros Number of microseconds by which to advance the track.
		 * \param _ptTarget The target value.
		 * \param _ptTargetDirtyFlag The target dirty flag.
		 */
		void LSE_CALL							SetTime( uint64_t _ui64Time, _tTargetType * _ptTarget, _tDirtyFlagType * _ptTargetDirtyFlag ) {
			if ( _ptTarget && m_bActive ) {
				LSREAL fFrac = CStd::Min( _ui64Time * m_fInvDur, LSM_ONE );
				if ( fFrac == m_fLastFrac ) { return; }
				m_fLastFrac = fFrac;
				LSREAL fVal = m_fRange * fFrac + m_fStart;
				(*_ptTarget) = static_cast<_tTargetType>(fVal);

				if ( _ptTargetDirtyFlag ) {
					(*_ptTargetDirtyFlag) |= static_cast<_tDirtyFlagType>(_uDirtyValue);
				}
			}
		}

		/**
		 * The total duration of the track.
		 *
		 * \return Returns the duration plus the post-duration delay.
		 */
		uint64_t LSE_CALL						TotalDuration() const {
			return m_ui64Duration + m_ui64Pad;
		}

		/**
		 * Gets the duration of the track.
		 *
		 * \return Returns the duration of the track.
		 */
		uint64_t LSE_CALL						Duration() const {
			return m_ui64Duration;
		}

		/**
		 * Sets whether or not the track is active.
		 *
		 * \param _bActive Whether the track is active or not.
		 */
		void LSE_CALL							SetActive( LSBOOL _bActive ) {
			m_bActive = _bActive;
		}


	protected :
		// == Members.
		/** Duration of the track in microseconds. */
		uint64_t								m_ui64Duration;

		/** Length of time after m_ui64Duration that the track stays active.  This can be used to pad between
		 *	tracks in a chain. */
		uint64_t								m_ui64Pad;
		
		/** The start value. */
		LSREAL									m_fStart;

		/** The end value. */
		LSREAL									m_fEnd;

		/** Distance between the start and end values (m_fEnd - m_fStart). */
		LSREAL									m_fRange;

		/** Last fraction set. */
		LSREAL									m_fLastFrac;

		/** Inverse of the duration. */
		mutable LSREAL							m_fInvDur;

		/** Is the track active? */
		LSBOOL									m_bActive;
	};

}	// namespace lsr

#endif	// __LSR_SPRITEANIMTRACK_H__
