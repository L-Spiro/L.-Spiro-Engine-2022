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

#include "LSRAnimatedSprite.h"


namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CAnimatedSprite::CAnimatedSprite() :
		m_ui32FrameIndex( 0 ),
		m_ui32LastFrameIndex( LSE_MAXU32 ),
		m_ui32CurSegment( LSE_MAXU32 ),
		m_ui64Time( 0ULL ) {
		m_ui32EntityType |= LSR_ET_ANIMATEDSPRITE;
		m_saAnim.SetSprite( this );
	}
	LSE_CALLCTOR CAnimatedSprite::~CAnimatedSprite() {
	}

	// == Functions.
	/**
	 * Updates sprite data.  Call just before rendering.
	 *
	 * \param _ui64Time Microseconds by which to update the sprite.
	 */
	void LSE_CALL CAnimatedSprite::Update( uint64_t _ui64Time ) {
		if ( m_ui32CurSegment >= m_vSegments.Length() ) {
			m_saAnim.Advance( _ui64Time );
		}
		else {
			const LSR_ANIM_SEGMENT & asSegment = m_vSegments[m_ui32CurSegment];
			switch ( asSegment.saptMode ) {
				case LSR_SAPT_PLAY_ONCE : {
					m_ui64Time = CStd::Min( m_ui64Time + _ui64Time, asSegment.ui64Duration );
					break;
				}
				case LSR_SAPT_REPEAT : {
					if ( asSegment.ui64Duration ) {
						m_ui64Time = (m_ui64Time + _ui64Time) % asSegment.ui64Duration;
					}
					else {
						m_ui64Time = 0ULL;
					}
					break;
				}
				case LSR_SAPT_REVERSE : {
					if ( _ui64Time >= m_ui64Time ) {
						// Decreasing m_ui64Time by _ui64Time is less than or equal to 0.
						m_ui64Time = 0ULL;
					}
					else {
						m_ui64Time -= _ui64Time;
					}
					break;
				}
				case LSR_SAPT_REVERSE_REPEAT : {
					if ( _ui64Time >= m_ui64Time ) {
						// Decreasing m_ui64Time by _ui64Time is less than or equal to 0.
						if ( !asSegment.ui64Duration ) {
							m_ui64Time = 0ULL;
						}
						else {
							uint64_t ui64Dif = (_ui64Time - m_ui64Time) % asSegment.ui64Duration;
							m_ui64Time = asSegment.ui64Duration - ui64Dif;
							// Examples assuming asSegment.ui64Duration = 10:
							// m_ui64Time	_ui64Time	ui64Dif		(asSegment.ui64Duration - ui64Dif)
							// 3			4			1			9
							// 3			50			7			3
						}
					}
					else {
						m_ui64Time = m_ui64Time - _ui64Time;
					}
					break;
				}
			}
			m_saAnim.SetTime( m_ui64Time + asSegment.ui64Start );
		}
		m_ui32FrameIndex = CStd::Min<uint32_t>( m_ui32FrameIndex, m_faFrames.Length() );
		if ( m_ui32FrameIndex != m_ui32LastFrameIndex ) {
			const LSR_FRAME & fFrame = m_faFrames[m_ui32FrameIndex];
			TexRect().SetDimensions( fFrame.ui32Left, fFrame.ui32Top, fFrame.ui32Width, fFrame.ui32Height );
			Center().x = fFrame.fCenterX;
			Center().y = fFrame.fCenterY;
			m_ui32LastFrameIndex = m_ui32FrameIndex;
		}
	}

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
	LSBOOL LSE_CALL CAnimatedSprite::AddFrames( const LSR_SPRITE_ANIM_FRAME * _psafFrames, uint32_t _ui32Sprites,
		const LSR_SPRITE_ANIM_SINGLE_FRAME * _psasfFrameDefinitions, uint32_t _ui32Frames ) {
		// Add single frames.
		uint32_t ui32CurFrame = FrameArray().Length();
		for ( uint32_t I = 0; I < _ui32Sprites; ++I ) {
			const LSR_SPRITE_ANIM_FRAME &safFrame = _psafFrames[I];
			LSR_FRAME fFrame = {
				safFrame.ui32Left,
				safFrame.ui32Top,
				safFrame.ui32Width,
				safFrame.ui32Height,
				safFrame.fCenterX,
				safFrame.fCenterY,
			};
			if ( !FrameArray().Push( fFrame ) ) { return false; }
		}
		// Add tracks to play each frame.
		LSR_SPRITE_ANIM_UINT32::CTrackType ttFrameTrack;
		for ( uint32_t I = 0; I < _ui32Frames; ++I ) {
			ttFrameTrack.SetTrack( _psasfFrameDefinitions->ui32FrameIndex + ui32CurFrame,
				_psasfFrameDefinitions->ui32FrameIndex + ui32CurFrame,
				_psasfFrameDefinitions->ui64Duration );
			if ( !Anim().FrameIndex().AddTrack( ttFrameTrack ) ) { return false; }
			++_psasfFrameDefinitions;
		}
		return true;
	}

}	// namespace lsr
