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
 * Description: An animation for a single sprite.
 */

#include "LSRSpriteAnimation.h"
#include "../Sprite/LSRAnimatedSprite.h"

#define LSR_ALL_FUNC( FUNC )	\
	m_satsPosX.FUNC;			\
	m_satsPosY.FUNC;			\
	m_satsScaleX.FUNC;			\
	m_satsScaleY.FUNC;			\
	m_satsRot.FUNC;				\
	m_satsDepth.FUNC;			\
	m_satsCenterX.FUNC;			\
	m_satsCenterY.FUNC;			\
	m_satsOffsetX.FUNC;			\
	m_satsOffsetY.FUNC;			\
	m_saiFrame.FUNC

namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CSpriteAnimation::CSpriteAnimation() :
		m_pasSprite( NULL ),
		m_bPaused( false ) {
	}
	LSE_CALLCTOR CSpriteAnimation::~CSpriteAnimation() {
	}

	// == Functions.
	/**
	 * Advances all track sets by the given amount of time.
	 *
	 * \param _ui64Time Number of microseconds by which to advance the animation time.
	 */
	void LSE_CALL CSpriteAnimation::Advance( uint64_t _ui64Time ) {
		if ( !m_bPaused ) {
			LSR_ALL_FUNC( Advance( _ui64Time ) );
		}
	}

	/**
	 * Sets the time for all tracks.
	 *
	 * \param _ui64Time Time to set on all tracks.
	 */
	void LSE_CALL CSpriteAnimation::SetTime( uint64_t _ui64Time ) {
		LSR_ALL_FUNC( SetTime( _ui64Time ) );
	}

	/**
	 * Sets the playback mode on all tracks.
	 *
	 * \param _saptMode The playback mode to set.
	 */
	void LSE_CALL CSpriteAnimation::SetPlayMode( LSR_SPRITE_ANIM_PLAY_TYPE _saptMode ) {
		LSR_ALL_FUNC( SetPlayMode( _saptMode ) );
	}

	/**
	 * Sets all tracks to time 0;
	 */
	void LSE_CALL CSpriteAnimation::Restart() {
		LSR_ALL_FUNC( SetTime( 0 ) );
	}

	/**
	 * Frees unnecessary memory.  Call when done modifying the tracks.
	 */
	void LSE_CALL CSpriteAnimation::Finalize() {
		LSR_ALL_FUNC( Finalize() );
	}

	/**
	 * Sets all track sets to the duration of the longest track set.  In this way all tracks will end/loop
	 *	at the same time.
	 */
	void LSE_CALL CSpriteAnimation::MaximizeTrackDurations() {
		uint64_t ui64Time = CStd::Max( m_satsPosX.TotalDuration(), m_satsPosY.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsScaleX.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsScaleY.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsRot.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsDepth.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsCenterX.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsCenterY.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsOffsetX.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_satsOffsetY.TotalDuration() );
		ui64Time = CStd::Max( ui64Time, m_saiFrame.TotalDuration() );
		LSR_ALL_FUNC( SetDuration( ui64Time ) );
	}

	/**
	 * Sets the sprite pointer.
	 *
	 * \param _pasSprite The sprite pointer to set.
	 */
	void LSE_CALL CSpriteAnimation::SetSprite( CAnimatedSprite * _pasSprite ) {
		m_pasSprite = _pasSprite;

		if ( m_pasSprite ) {
			m_satsPosX.SetTarget( &m_pasSprite->Orientation().DirectPos().x, &m_pasSprite->Orientation().DirectDirty() );
			m_satsPosY.SetTarget( &m_pasSprite->Orientation().DirectPos().y, &m_pasSprite->Orientation().DirectDirty() );
			m_satsScaleX.SetTarget( &m_pasSprite->Orientation().DirectScale().x, &m_pasSprite->Orientation().DirectDirty() );
			m_satsScaleY.SetTarget( &m_pasSprite->Orientation().DirectScale().y, &m_pasSprite->Orientation().DirectDirty() );
			m_satsRot.SetTarget( &m_pasSprite->Orientation().DirectRot(), &m_pasSprite->Orientation().DirectDirty() );
			m_satsDepth.SetTarget( &m_pasSprite->Depth() );
			m_satsCenterX.SetTarget( &m_pasSprite->Center().x );
			m_satsCenterY.SetTarget( &m_pasSprite->Center().y );
			m_satsOffsetX.SetTarget( &m_pasSprite->Offset().x );
			m_satsOffsetY.SetTarget( &m_pasSprite->Offset().y );
			m_saiFrame.SetTarget( &m_pasSprite->DirectFrameIndex() );
		}
	}

}	// namespace lsr

#undef LSR_ALL_FUNC
