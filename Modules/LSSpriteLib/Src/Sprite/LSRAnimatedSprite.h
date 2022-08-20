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
 * Description: An animated sprite.
 */


#ifndef __LSR_ANIMATEDSPRITE_H__
#define __LSR_ANIMATEDSPRITE_H__

#include "../LSRSpriteLib.h"
#include "../Animation/LSRSpriteAnimation.h"
#include "../Animation/LSRSpriteAnimDataStructs.h"
#include "LSRSprite.h"

namespace lsr {

	/**
	 * Class CAnimatedSprite
	 * \brief An animated sprite.
	 *
	 * Description: An animated sprite.
	 */
	class CAnimatedSprite : public CSprite {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CAnimatedSprite();
		LSE_CALLCTOR							~CAnimatedSprite();


		// == Types.
		/** A single frame. */
		typedef struct LSR_FRAME {
			/** Left part of the texture, in texels. */
			uint32_t							ui32Left;

			/** Top part of the texture, in texels. */
			uint32_t							ui32Top;

			/** Width of the texture, in texels. */
			uint32_t							ui32Width;

			/** Height of the texture, in texels. */
			uint32_t							ui32Height;

			/** Center X of the sprite in texels. */
			LSREAL								fCenterX;

			/** Center Y of the sprite in texels. */
			LSREAL								fCenterY;
		} * LPLSR_FRAME, * const LPCLSR_FRAME;

		/** Animation frame list. */
		typedef CVector<LSR_FRAME, uint32_t, 32>LSR_FRAME_ARRAY;

		/** A duration inside the whole animation.  This segment of the whole animation can be played separately
		 *	from the rest of the animation, and includes its own playback mode. */
		typedef struct LSR_ANIM_SEGMENT {
			/** Playback mode. */
			LSR_SPRITE_ANIM_PLAY_TYPE			saptMode;

			/** Start time of the segment in microseconds. */
			uint64_t							ui64Start;

			/** Duration of the segment. */
			uint64_t							ui64Duration;
		} * LPLSR_ANIM_SEGMENT, * const LPCLSR_ANIM_SEGMENT;


		// == Functions.
		/**
		 * Updates sprite data.  Call just before rendering.
		 *
		 * \param _ui64Time Microseconds by which to update the sprite.
		 */
		virtual void LSE_CALL					Update( uint64_t _ui64Time );

		/**
		 * Gets a constant reference to the animation object.
		 *
		 * \return Returns a constant reference to the animation object.
		 */
		LSE_INLINE const CSpriteAnimation & LSE_CALL
												Anim() const;

		/**
		 * Gets a reference to the animation object.
		 *
		 * \return Returns a reference to the animation object.
		 */
		LSE_INLINE CSpriteAnimation & LSE_CALL	Anim();

		/**
		 * Gets a constant reference to the animation frames.
		 *
		 * \return Returns a constant reference to the animation frames.
		 */
		LSE_INLINE const LSR_FRAME_ARRAY & LSE_CALL
												FrameArray() const;

		/**
		 * Gets a reference to the animation frames.
		 *
		 * \return Returns a reference to the animation frames.
		 */
		LSE_INLINE LSR_FRAME_ARRAY & LSE_CALL	FrameArray();

		/**
		 * Frees unnecessary memory.  Call when done modifying the animation.
		 */
		LSE_INLINE void LSE_CALL				Finalize();

		/**
		 * Gets the frame index.
		 *
		 * \return Returns the frame index.
		 */
		LSE_INLINE uint32_t LSE_CALL			FrameIndex() const;

		/**
		 * Sets the frame index.
		 *
		 * \param _ui32Index Index to set.
		 */
		LSE_INLINE void LSE_CALL				SetFrameIndex( uint32_t _ui32Index );

		/**
		 * Adds an animation segment.  A segment of a whole animation can be played by itself, allowing
		 *	multiple full animations to be strung together and played via segments.
		 *
		 * \param _asSegment The animation segment to add.
		 * \return Returns true if there was enough memory to add the segment.
		 */
		LSE_INLINE LSBOOL LSE_CALL				AddAnimSegment( const LSR_ANIM_SEGMENT &_asSegment );

		/**
		 * Sets the current animation segment.
		 *
		 * \param _ui32Index Index of the animation to set.  If the animation does not exist,
		 *	the whole animation set is played as per the default behavior.
		 */
		LSE_INLINE void LSE_CALL				SetAnimSegment( uint32_t _ui32Index );

		/**
		 * Adds a series of sprite frames.  Each frame is added one after the other to the existing array
		 *	of sprite frames.
		 *
		 * \param _psafFrames Array of individual sprite boxes and center points.
		 * \param _ui32Sprites Total sprites to which _psafFrames points.
		 * \param _psasfFrameDefinitions Pointer to an aray of frame definitions.  A single frame is defined by its index into
		 *	_psafFrames (for the sprite to display) and the time to display that image.
		 * \param _ui32Frames Total objects to which _psasfFrameDefinitions points.
		 * \return Returns true if the whole series of frames was added.
		 */
		LSBOOL LSE_CALL							AddFrames( const LSR_SPRITE_ANIM_FRAME * _psafFrames, uint32_t _ui32Sprites,
			const LSR_SPRITE_ANIM_SINGLE_FRAME * _psasfFrameDefinitions, uint32_t _ui32Frames );


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// INTERNAL USE ONLY
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		/**
		 * Gets a direct reference to the frame index.  Do not use.
		 *
		 * \return Returns a direct reference to the frame index.
		 */
		LSE_INLINE uint32_t & LSE_CALL			DirectFrameIndex();


	protected :
		// == Members.
		/** The sprite's animation. */
		CSpriteAnimation						m_saAnim;

		/** Animation frames. */
		LSR_FRAME_ARRAY							m_faFrames;

		/** Segment array. */
		CVectorPoD<LSR_ANIM_SEGMENT>			m_vSegments;

		/** Frame index. */
		uint32_t								m_ui32FrameIndex;

		/** Last frame index. */
		uint32_t								m_ui32LastFrameIndex;

		/** Current segment. */
		uint32_t								m_ui32CurSegment;

		/** Segment time. */
		uint64_t								m_ui64Time;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to a standard texture.
	 */
	typedef CSharedPtr<CAnimatedSprite>			CSharedAnimatedSpritePtr;

	// == Functions.
	/**
	 * Gets a constant reference to the animation object.
	 *
	 * \return Returns a constant reference to the animation object.
	 */
	LSE_INLINE const CSpriteAnimation & LSE_CALL CAnimatedSprite::Anim() const {
		return m_saAnim;
	}

	/**
	 * Gets a reference to the animation object.
	 *
	 * \return Returns a reference to the animation object.
	 */
	LSE_INLINE CSpriteAnimation & LSE_CALL CAnimatedSprite::Anim() {
		return m_saAnim;
	}

	/**
	 * Gets a constant reference to the animation frames.
	 *
	 * \return Returns a constant reference to the animation frames.
	 */
	LSE_INLINE const CAnimatedSprite::LSR_FRAME_ARRAY & LSE_CALL CAnimatedSprite::FrameArray() const {
		return m_faFrames;
	}

	/**
	 * Gets a reference to the animation frames.
	 *
	 * \return Returns a reference to the animation frames.
	 */
	LSE_INLINE CAnimatedSprite::LSR_FRAME_ARRAY & LSE_CALL CAnimatedSprite::FrameArray() {
		return m_faFrames;
	}

	/**
	 * Frees unnecessary memory.  Call when done modifying the animation.
	 */
	LSE_INLINE void LSE_CALL CAnimatedSprite::Finalize() {
		m_faFrames.Snap();
		m_saAnim.Finalize();
		m_vSegments.Snap();
	}

	/**
	 * Gets the frame index.
	 *
	 * \return Returns the frame index.
	 */
	LSE_INLINE uint32_t LSE_CALL CAnimatedSprite::FrameIndex() const {
		return m_ui32FrameIndex;
	}

	/**
	 * Sets the frame index.
	 *
	 * \param _ui32Index Index to set.
	 */
	LSE_INLINE void LSE_CALL CAnimatedSprite::SetFrameIndex( uint32_t _ui32Index ) {
		m_ui32FrameIndex = _ui32Index;
	}

	/**
	 * Adds an animation segment.  A segment of a whole animation can be played by itself, allowing
	 *	multiple full animations to be strung together and played via segments.
	 *
	 * \param _asSegment The animation segment to add.
	 * \return Returns true if there was enough memory to add the segment.
	 */
	LSE_INLINE LSBOOL LSE_CALL CAnimatedSprite::AddAnimSegment( const LSR_ANIM_SEGMENT &_asSegment ) {
		return m_vSegments.Push( _asSegment );
	}

	/**
	 * Sets the current animation segment.
	 *
	 * \param _ui32Index Index of the animation to set.  If the animation does not exist,
	 *	the whole animation set is played as per the default behavior.
	 */
	LSE_INLINE void LSE_CALL CAnimatedSprite::SetAnimSegment( uint32_t _ui32Index ) {
		m_ui32CurSegment = _ui32Index;
	}

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// INTERNAL USE ONLY
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/**
	 * Gets a direct reference to the frame index.  Do not use.
	 *
	 * \return Returns a direct reference to the frame index.
	 */
	LSE_INLINE uint32_t & LSE_CALL CAnimatedSprite::DirectFrameIndex() {
		return m_ui32FrameIndex;
	}

}	// namespace lsr

#endif	// __LSR_ANIMATEDSPRITE_H__
