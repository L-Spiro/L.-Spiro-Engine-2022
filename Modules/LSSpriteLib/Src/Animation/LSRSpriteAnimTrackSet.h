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
 * Description: A series of tracks that play in order.
 */


#ifndef __LSR_SPRITEANIMTRACKSET_H__
#define __LSR_SPRITEANIMTRACKSET_H__

#include "../LSRSpriteLib.h"
#include "LSRSpriteAnimTrack.h"
#include "Vector/LSTLVector.h"

namespace lsr {

	// == Enumerations.
	/** Playback type. */
	enum LSR_SPRITE_ANIM_PLAY_TYPE {
		LSR_SAPT_PLAY_ONCE,																		/**< Plays the track set forward once and stops. */
		LSR_SAPT_REPEAT,																		/**< The track set loops. */
		LSR_SAPT_REVERSE,																		/**< Plays the track set backwards until the time reaches 0. */
		LSR_SAPT_REVERSE_REPEAT,																/**< Plays the track set backwards and loops. */
	};

	/**
	 * Class CSpriteAnimTrackSet
	 * \brief A series of tracks that play in order.
	 *
	 * Description: A series of tracks that play in order.
	 */
	template <typename _tTargetType, typename _tDirtyFlagType, unsigned _uDirtyValue = 0>
	class CSpriteAnimTrackSet {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSpriteAnimTrackSet() :
			m_ui64Time( 0ULL ),
			m_ptTarget( NULL ),
			m_ptDirty( NULL ),
			m_ui32Index( 0 ),
			m_saptPlayMode( LSR_SAPT_PLAY_ONCE ),
			m_bPaused( false ) {
		}


		// == Types.
		/** Track type. */
		typedef CSpriteAnimTrack<_tTargetType, _tDirtyFlagType, _uDirtyValue>
												CTrackType;

		/** The internal structure used to keep track of the start times of each track. */
		typedef struct LSR_TRACK {
			uint64_t							ui64Start;										/**< Start time of the track. */
			CTrackType							ttTrack;										/**< The track. */
		} * LPLSR_TRACK, * const LPCLSR_TRACK;

		/** Track array type. */
		typedef CVector<LSR_TRACK, uint16_t, 16>CTrackVector;


		// == Functions.
		/**
		 * Sets the track target (the value to be updated by the track).
		 *
		 * \param _ptTarget The target value.
		 * \param _ptTargetDirtyFlag The target dirty flag.
		 */
		void LSE_CALL							SetTarget( _tTargetType * _ptTarget, _tDirtyFlagType * _ptTargetDirtyFlag = NULL ) {
			m_ptTarget = _ptTarget;
			m_ptDirty = _ptTargetDirtyFlag;
		}

		/**
		 * Clears the track, removing all tracks.
		 */
		void LSE_CALL							Clear() {
			m_tvTracks.Reset();
		}

		/**
		 * Adds a track to the end of the track array.
		 *
		 * \param _ttTrack The track to add to the end of the array of tracks.
		 * \return Returns false if there was a memory error while adding the track.
		 */
		LSBOOL LSE_CALL							AddTrack( const CTrackType &_ttTrack ) {
			LSR_TRACK tThis {
				TotalDuration(),
				_ttTrack
			};
			return m_tvTracks.Push( tThis );
		}

		/**
		 * Adds the given tracks to the end of the track array.
		 *
		 * \param _psatTracks Array of track data objects to add.
		 * \param _ui32Total Total objects to which _psatTracks points.
		 * \return Returns false if there are any memory errors.
		 */
		LSBOOL LSE_CALL							AddTracks( const LSR_SPRITE_ANIM_TRACK * _psatTracks, uint32_t _ui32Total ) {
			uint32_t ui32Len = m_tvTracks.Length();
			if ( !m_tvTracks.Resize( ui32Len + _ui32Total ) ) {
				return false;
			}
			for ( uint32_t I = ui32Len; I < m_tvTracks.Length(); ++I ) {
				m_tvTracks[I].SetTrack( (*_psatTracks++) );
			}
		}

		/**
		 * After all tracks have been added, call this to "finalize" the array.  This reduces memory consumption etc.
		 */
		void LSE_CALL							Finalize() {
			m_tvTracks.Snap();
		}

		/**
		 * Gets the total number of tracks.
		 *
		 * \return Returns the total number of tracks.
		 */
		uint32_t LSE_CALL						TotalTracks() {
			return m_tvTracks.Length();
		}

		/**
		 * Advances the set of tracks by the given number of microseconds.
		 *
		 * \param _ui64Time The number of microseconds by which to advance time.
		 */
		void LSE_CALL							Advance( uint64_t _ui64Time ) {
			if ( !m_bPaused ) {
				switch ( m_saptPlayMode ) {
					case LSR_SAPT_PLAY_ONCE : {
						SetTime( m_ui64Time + _ui64Time );
						break;
					}
					case LSR_SAPT_REPEAT : {
						uint64_t ui64Dur = TotalDuration();
						if ( ui64Dur ) {
							SetTime( (m_ui64Time + _ui64Time) % TotalDuration() );
						}
						break;
					}
					case LSR_SAPT_REVERSE : {
						if ( _ui64Time >= m_ui64Time ) {
							// Decreasing m_ui64Time by _ui64Time is less than or equal to 0.
							SetTime( 0ULL );
						}
						else {
							SetTime( m_ui64Time - _ui64Time );
						}
						break;
					}
					case LSR_SAPT_REVERSE_REPEAT : {
						if ( _ui64Time >= m_ui64Time ) {
							uint64_t ui64TotalDur = TotalDuration();
							// Decreasing m_ui64Time by _ui64Time is less than or equal to 0.
							if ( !ui64TotalDur ) {
								SetTime( 0 );
							}
							else {
								uint64_t ui64Dif = (_ui64Time - m_ui64Time) % ui64TotalDur;
								SetTime( ui64TotalDur - ui64Dif );
								// Examples assuming ui64TotalDur = 10:
								// m_ui64Time	_ui64Time	ui64Dif		(ui64TotalDur - ui64Dif)
								// 3			4			1			9
								// 3			50			7			3
							}
						}
						else {
							SetTime( m_ui64Time - _ui64Time );
						}
						break;
					}
				}
			}
		}

		/**
		 * Sets the time to the given value.
		 *
		 * \param _ui64Time New time of the track set.
		 */
		void LSE_CALL							SetTime( uint64_t _ui64Time ) {
			m_ui64Time = _ui64Time;
			if ( m_ptTarget && TotalTracks() ) {
				m_ui32Index = FindTrackByTime( m_ui64Time );
				m_tvTracks[m_ui32Index].ttTrack.SetTime( m_ui64Time - m_tvTracks[m_ui32Index].ui64Start,
					m_ptTarget, m_ptDirty );
			}
		}

		/**
		 * Gets the duration of the set of tracks.
		 *
		 * \return Returns the total duration in microseconds of all tracks in the set.
		 */
		uint64_t LSE_CALL						TotalDuration() {
			if ( !TotalTracks() ) { return 0ULL; }
			const LSR_TRACK & tThis = m_tvTracks[TotalTracks()-1];
			return tThis.ui64Start + tThis.ttTrack.TotalDuration();
		}

		/**
		 * Sets the playback mode.
		 *
		 * \param _saptMode The playback mode to set.
		 */
		void LSE_CALL							SetPlayMode( LSR_SPRITE_ANIM_PLAY_TYPE _saptMode ) {
			m_saptPlayMode = _saptMode;
		}

		/**
		 * Sets whether the track is paused or not.
		 *
		 * \param _bPaused Whether the track is paused or not.
		 */
		void LSE_CALL							SetPaused( LSBOOL _bPaused ) {
			m_bPaused = _bPaused;
		}

		/**
		 * Pads out the last track so that the total set duration equals the given time.
		 *	The given time must be larger than or equal to the track set duration, or else no
		 *	operation is performed.
		 *
		 * \param _ui64Time The last track is padded until the set's duration equals this value. 
		 */
		void LSE_CALL							SetDuration( uint64_t _ui64Time ) {
			if ( !TotalTracks() ) { return; }
			if ( _ui64Time > TotalDuration() ) {
				LSR_TRACK & tThis = m_tvTracks[TotalTracks()-1];
				tThis.ttTrack.SetPostDuration( (_ui64Time - tThis.ui64Start) - tThis.ttTrack.Duration() );
				assert( TotalDuration() == _ui64Time );
			}
		}

		/**
		 * Adds an inactive (do-nothing) track with a duration long enough to set the total track set time to the
		 *	given duration, effectively padding this track set with a track that does nothing.  This is useful
		 *	for aligning individual tracks, usually to facility animation segments.
		 *
		 * \param _ui64Time The time to which to pad this track set with a do-nothing track.
		 * \return Returns true if there was enough memory to add the track.
		 */
		LSBOOL LSE_CALL							PadUntilTime( uint64_t _ui64Time ) {
			uint64_t ui64Dur = TotalDuration();
			if ( _ui64Time > ui64Dur ) {
				LSR_SPRITE_ANIM_TRACK satTrack = {
					LSM_ZERO, LSM_ZERO,
					_ui64Time - ui64Dur,
					0ULL
				};
				if ( !AddTracks( &satTrack, 1 ) ) { return false; }
				m_tvTracks[TotalTracks()-1].SetActive( false );
				return true;
			}
			return true;
		}


	protected :
		// == Members.
		/** Time in microseconds. */
		uint64_t								m_ui64Time;

		/** The value to update. */
		_tTargetType *							m_ptTarget;

		/** The dirty flag. */
		_tDirtyFlagType *						m_ptDirty;

		/** The array of tracks, played in order. */
		CTrackVector							m_tvTracks;

		/** The current track index. */
		uint32_t								m_ui32Index;

		/** The playback mode. */
		LSR_SPRITE_ANIM_PLAY_TYPE				m_saptPlayMode;

		/** Paused? */
		LSBOOL									m_bPaused;


		// == Functions.
		/**
		 * Gets the current index based off time.
		 *
		 * \param _ui64Time The time.
		 * \return Returns the index of the track whose duration spans across the given time.
		 */
		uint32_t LSE_CALL						FindTrackByTime( uint64_t _ui64Time ) {
			for ( uint32_t I = m_ui32Index; I < TotalTracks(); ++I ) {
				if ( m_tvTracks[I].ui64Start > _ui64Time ) { break; }	// Passed it.
				if ( m_tvTracks[I].ui64Start <= _ui64Time && _ui64Time < m_tvTracks[I].ui64Start + m_tvTracks[I].ttTrack.TotalDuration() ) { return I; }
			}
			for ( uint32_t I = 0; I < m_ui32Index; ++I ) {
				if ( m_tvTracks[I].ui64Start > _ui64Time ) { break; }	// Passed it.
				if ( m_tvTracks[I].ui64Start <= _ui64Time && _ui64Time < m_tvTracks[I].ui64Start + m_tvTracks[I].ttTrack.TotalDuration() ) { return I; }
			}
			return TotalTracks() - 1;	// Return the last track.
		}

		
	};

}	// namespace lsr

#endif	// __LSR_SPRITEANIMTRACKSET_H__
