/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A single 2D sprite.  A sprite contains a part of a texture (CTextureRect), rotation, scale, etc.
 */

#include "LSRSprite.h"


namespace lsr {

	// == Various constructors.
	LSE_CALLCTOR CSprite::CSprite() :
		m_vCenter( CVector2( LSM_ZERO, LSM_ZERO ) ),
		m_vOffset( CVector2( LSM_ZERO, LSM_ZERO ) ) {
		m_ui32EntityType |= LSR_ET_SPRITE;
		m_mLocalTrans.Identity();
		m_mWorldTrans.Identity();
		m_oOrientation2d.Undirty();	// m_mLocalTrans is identity, the same matrix this would create at the start.
	}

	// == Functions.
	/**
	 * Updates sprite data.  Call just before rendering.
	 *
	 * \param _ui64Time Microseconds by which to update the sprite.
	 */
	void LSE_CALL CSprite::Update( uint64_t /*_ui64Time*/ ) {
	}

	/**
	 * Renders the sprite.  Does not modify the view matrix.
	 */
	void LSE_CALL CSprite::Draw() {
		if ( !m_sspShader.Valid() ) { return; }
		CGfx::SetShader( m_sspShader.Get() );
		CGfx::SetWorldMatrix( m_mWorldTrans );
	}

}	// namespace lsr
