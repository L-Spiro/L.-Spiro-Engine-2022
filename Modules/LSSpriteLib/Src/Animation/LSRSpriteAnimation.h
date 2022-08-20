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


#ifndef __LSR_SPRITEANIMATION_H__
#define __LSR_SPRITEANIMATION_H__

#include "../LSRSpriteLib.h"
#include "LSRSpriteAnimTrackSet.h"
#include "Orientation/LSMOrientation2d.h"

namespace lsr {

	// == Forward Declarations.
	class										CAnimatedSprite;


	// == Types.
	/** Position-vector members. */
	typedef CSpriteAnimTrackSet<LSREAL, uint32_t, LSM_2DODF_POSITION>
												LSR_SPRITE_ANIM_POS;

	/** Scale-vector members. */
	typedef CSpriteAnimTrackSet<LSREAL, uint32_t, LSM_2DODF_SCALE>
												LSR_SPRITE_ANIM_SCALE;

	/** Rotation members. */
	typedef CSpriteAnimTrackSet<LSREAL, uint32_t, LSM_2DODF_ROTATION>
												LSR_SPRITE_ANIM_ROT;

	/** Generic floats. */
	typedef CSpriteAnimTrackSet<LSREAL, uint32_t, 0>
												LSR_SPRITE_ANIM_FLOAT;

	/** Generic unsigned integers. */
	typedef CSpriteAnimTrackSet<uint32_t, uint32_t, 0>
												LSR_SPRITE_ANIM_UINT32;


	/**
	 * Class CSpriteAnimation
	 * \brief An animation for a single sprite.
	 *
	 * Description: An animation for a single sprite.
	 */
	class CSpriteAnimation {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSpriteAnimation();
		LSE_CALLCTOR							~CSpriteAnimation();


		// == Functions.
		/**
		 * Advances all track sets by the given amount of time.
		 *
		 * \param _ui64Time Number of microseconds by which to advance the animation time.
		 */
		void LSE_CALL							Advance( uint64_t _ui64Time );

		/**
		 * Sets the time for all tracks.
		 *
		 * \param _ui64Time Time to set on all tracks.
		 */
		void LSE_CALL							SetTime( uint64_t _ui64Time );

		/**
		 * Gets a reference to the X position track set.
		 *
		 * \return Returns a reference to the X position track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_POS & LSE_CALL
												PosX();

		/**
		 * Gets a reference to the Y position track set.
		 *
		 * \return Returns a reference to the Y position track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_POS & LSE_CALL
												PosY();

		/**
		 * Gets a reference to the X scale track set.
		 *
		 * \return Returns a reference to the X scale track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_SCALE & LSE_CALL
												ScaleX();

		/**
		 * Gets a reference to the Y scale track set.
		 *
		 * \return Returns a reference to the Y scale track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_SCALE & LSE_CALL
												ScaleY();

		/**
		 * Gets a reference to the rotation track set.
		 *
		 * \return Returns a reference to the rotation track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_ROT & LSE_CALL
												Rot();

		/**
		 * Gets a reference to the depth track set.
		 *
		 * \return Returns a reference to the depth track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL
												Depth();

		/**
		 * Gets a reference to the center X track set.
		 *
		 * \return Returns a reference to the center X track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL
												CenterX();

		/**
		 * Gets a reference to the center Y track set.
		 *
		 * \return Returns a reference to the center Y track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL
												CenterY();

		/**
		 * Gets a reference to the offset X track set.
		 *
		 * \return Returns a reference to the offset X track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL
												OffsetX();

		/**
		 * Gets a reference to the offset Y track set.
		 *
		 * \return Returns a reference to the offset Y track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL
												OffsetY();

		/**
		 * Gets a reference to the frame track set.
		 *
		 * \return Returns a reference to the frame track set.
		 */
		LSE_INLINE LSR_SPRITE_ANIM_UINT32 & LSE_CALL
												FrameIndex();

		/**
		 * Set paused.
		 *
		 * \param _bPaused Whether or not this animation is paused.
		 */
		LSE_INLINE void LSE_CALL				SetPaused( LSBOOL _bPaused );

		/**
		 * Sets the playback mode on all tracks.
		 *
		 * \param _saptMode The playback mode to set.
		 */
		void LSE_CALL							SetPlayMode( LSR_SPRITE_ANIM_PLAY_TYPE _saptMode );

		/**
		 * Sets all tracks to time 0;
		 */
		void LSE_CALL							Restart();

		/**
		 * Frees unnecessary memory.  Call when done modifying the tracks.
		 */
		void LSE_CALL							Finalize();

		/**
		 * Sets all track sets to the duration of the longest track set.  In this way all tracks will end/loop
		 *	at the same time.
		 */
		void LSE_CALL							MaximizeTrackDurations();


	protected :
		// == Members.
		/** The sprite we are animating. */
		CAnimatedSprite *						m_pasSprite;

		/** Position.X tracks. */
		LSR_SPRITE_ANIM_POS						m_satsPosX;

		/** Position.Y tracks. */
		LSR_SPRITE_ANIM_POS						m_satsPosY;

		/** Scale.X tracks. */
		LSR_SPRITE_ANIM_SCALE					m_satsScaleX;

		/** Scale.Y tracks. */
		LSR_SPRITE_ANIM_SCALE					m_satsScaleY;

		/** Rotation tracks. */
		LSR_SPRITE_ANIM_ROT						m_satsRot;

		/** Depth tracks. */
		LSR_SPRITE_ANIM_FLOAT					m_satsDepth;

		/** Center.X tracks. */
		LSR_SPRITE_ANIM_FLOAT					m_satsCenterX;

		/** Center.Y tracks. */
		LSR_SPRITE_ANIM_FLOAT					m_satsCenterY;

		/** Offset.X tracks. */
		LSR_SPRITE_ANIM_FLOAT					m_satsOffsetX;
		
		/** Offset.Y tracks. */
		LSR_SPRITE_ANIM_FLOAT					m_satsOffsetY;
		
		/** Animation frame. */
		LSR_SPRITE_ANIM_UINT32					m_saiFrame;

		/** Are we paused? */
		LSBOOL									m_bPaused;


		// == Functions.
		/**
		 * Sets the sprite pointer.
		 *
		 * \param _pasSprite The sprite pointer to set.
		 */
		void LSE_CALL							SetSprite( CAnimatedSprite * _pasSprite );

	private :
		friend class							CAnimatedSprite;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a reference to the X position track set.
	 *
	 * \return Returns a reference to the X position track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_POS & LSE_CALL CSpriteAnimation::PosX() {
		return m_satsPosX;
	}

	/**
	 * Gets a reference to the Y position track set.
	 *
	 * \return Returns a reference to the Y position track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_POS & LSE_CALL CSpriteAnimation::PosY() {
		return m_satsPosY;
	}

	/**
	 * Gets a reference to the X scale track set.
	 *
	 * \return Returns a reference to the X scale track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_SCALE & LSE_CALL CSpriteAnimation::ScaleX() {
		return m_satsScaleX;
	}

	/**
	 * Gets a reference to the Y scale track set.
	 *
	 * \return Returns a reference to the Y scale track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_SCALE & LSE_CALL CSpriteAnimation::ScaleY() {
		return m_satsScaleY;
	}

	/**
	 * Gets a reference to the rotation track set.
	 *
	 * \return Returns a reference to the rotation track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_ROT & LSE_CALL CSpriteAnimation::Rot() {
		return m_satsRot;
	}

	/**
	 * Gets a reference to the depth track set.
	 *
	 * \return Returns a reference to the depth track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL CSpriteAnimation::Depth() {
		return m_satsDepth;
	}

	/**
	 * Gets a reference to the center X track set.
	 *
	 * \return Returns a reference to the center X track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL CSpriteAnimation::CenterX() {
		return m_satsCenterX;
	}

	/**
	 * Gets a reference to the center Y track set.
	 *
	 * \return Returns a reference to the center Y track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL CSpriteAnimation::CenterY() {
		return m_satsCenterY;
	}

	/**
	 * Gets a reference to the offset X track set.
	 *
	 * \return Returns a reference to the offset X track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL CSpriteAnimation::OffsetX() {
		return m_satsOffsetX;
	}

	/**
	 * Gets a reference to the offset Y track set.
	 *
	 * \return Returns a reference to the offset Y track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_FLOAT & LSE_CALL CSpriteAnimation::OffsetY() {
		return m_satsOffsetY;
	}

	/**
	 * Gets a reference to the frame track set.
	 *
	 * \return Returns a reference to the frame track set.
	 */
	LSE_INLINE LSR_SPRITE_ANIM_UINT32 & LSE_CALL CSpriteAnimation::FrameIndex() {
		return m_saiFrame;
	}

	/**
	 * Set paused.
	 *
	 * \param _bPaused Whether or not this animation is paused.
	 */
	LSE_INLINE void LSE_CALL CSpriteAnimation::SetPaused( LSBOOL _bPaused ) {
		m_bPaused = _bPaused;
	}

}	// namespace lsr

#endif	// __LSR_SPRITEANIMATION_H__
