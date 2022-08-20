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
 * Description: Structures used to create animation data for sprites.
 */


#ifndef __LSR_SPRITEANIMDATASTRUCTS_H__
#define __LSR_SPRITEANIMDATASTRUCTS_H__

#include "../LSRSpriteLib.h"

namespace lsr {

	/** A frame of a sprite animation.  Describes the location on a texture of a single sprite or frame of an animated sprite. */
	typedef struct LSR_SPRITE_ANIM_FRAME {
		uint32_t								ui32Left;										/**< Left of the sprite in texels. */
		uint32_t								ui32Top;										/**< Top of the sprite in texels. */
		uint32_t								ui32Width;										/**< Width of the sprite in texels. */
		uint32_t								ui32Height;										/**< Height of the sprite in texels. */
		LSREAL									fCenterX;										/**< X center of the sprite. */
		LSREAL									fCenterY;										/**< Y center of the sprite. */
	} * LPLSR_SPRITE_ANIM_FRAME, * const LPCLSR_SPRITE_ANIM_FRAME;

	/** A frame of animation (by index into an array of LSR_SPRITE_ANIM_FRAME objects) along with the duration of the frame in microseconds. */
	typedef struct LSR_SPRITE_ANIM_SINGLE_FRAME {
		uint32_t								ui32FrameIndex;									/**< Index into an array of LSR_SPRITE_ANIM_FRAME objects. */
		uint64_t								ui64Duration;									/**< Length of this frame in microseconds. */
	} * LPLSR_SPRITE_ANIM_SINGLE_FRAME, * const LPCLSR_SPRITE_ANIM_SINGLE_FRAME;

	/** A single track.  Values in floats are cast to the proper type during load. */
	typedef struct LSR_SPRITE_ANIM_TRACK {
		LSREAL									fFrom;											/**< The start value of the track. */
		LSREAL									fTo;											/**< The end value of the track. */
		uint64_t								ui64Duration;									/**< Length of the track in microseconds. */
		uint64_t								ui64Padding;									/**< How long to keep the track alive after its duration, in microseconds. */
	} * LPLSR_SPRITE_ANIM_TRACK, * const LPCLSR_SPRITE_ANIM_TRACK;

}	// namespace lsr

#endif	// __LSR_SPRITEANIMDATASTRUCTS_H__
