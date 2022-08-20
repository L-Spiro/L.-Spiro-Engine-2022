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


#ifndef __LSR_SPRITE_H__
#define __LSR_SPRITE_H__

#include "../LSRSpriteLib.h"
#include "../Entity/LSRActor2d.h"
#include "Gfx/LSGGfx.h"
#include "LSRTextureRect.h"
#include "ShaderLanguage/LSGShader.h"

namespace lsr {

	/**
	 * Class CSprite
	 * \brief A single 2D sprite.
	 *
	 * Description: A single 2D sprite.  A sprite contains a part of a texture (CTextureRect), rotation, scale, etc.
	 */
	class CSprite : public CActor2d {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSprite();


		// == Functions.
		/**
		 * Updates sprite data.  Call just before rendering.
		 *
		 * \param _ui64Time Microseconds by which to update the sprite.
		 */
		virtual void LSE_CALL					Update( uint64_t _ui64Time );

		/**
		 * Gets a constant reference to the texture rectangle.
		 *
		 * \return Returns a constant reference to the texture rectangle.
		 */
		LSE_INLINE const CTextureRect & LSE_CALL
												TexRect() const;

		/**
		 * Gets a reference to the texture rectangle.
		 *
		 * \return Returns a reference to the texture rectangle.
		 */
		LSE_INLINE CTextureRect & LSE_CALL		TexRect();

		/**
		 * Gets a constant reference to the shader used by this sprite.
		 *
		 * \return Returns a constant reference to the shader used by this sprite.
		 */
		LSE_INLINE const CSharedShaderPtr & LSE_CALL
												Shader() const;

		/**
		 * Gets a reference to the shader used by this sprite.
		 *
		 * \return Returns a reference to the shader used by this sprite.
		 */
		LSE_INLINE CSharedShaderPtr & LSE_CALL Shader();

		/**
		 * Renders the sprite.  Does not modify the view matrix.
		 */
		void LSE_CALL							Draw();

		/**
		 * Gets a constant reference to the center point.
		 *
		 * \return Returns a constant reference to the center point.
		 */
		LSE_INLINE const CVector2 & LSE_CALL	Center() const;

		/**
		 * Gets a reference to the center point.
		 *
		 * \return Returns a reference to the center point.
		 */
		LSE_INLINE CVector2 & LSE_CALL			Center();

		/**
		 * Sets the center of the sprite given normalized UV coordinates (0-1, with 0.5 being the center of the image).
		 *
		 * \param _fX Normalized X coordinate of the center.
		 * \param _fY Normalized Y coordinate of the center.
		 */
		LSE_INLINE void LSE_CALL				SetCenterNormalized( LSREAL _fX, LSREAL _fY );

		/**
		 * Gets a constant reference to the offset.  The offset is a post-transform displacement for rendering the sprite.
		 *
		 * \return Returns a constant reference to the offset.
		 */
		LSE_INLINE const CVector2 & LSE_CALL	Offset() const;

		/**
		 * Gets a reference to the offset.  The offset is a post-transform displacement for rendering the sprite.
		 *
		 * \return Returns a reference to the offset.
		 */
		LSE_INLINE CVector2 & LSE_CALL			Offset();


		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// INTERNAL USE ONLY
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


	protected :
		// == Members.
		/** The texture rectangle and center point. */
		CTextureRect							m_trTexRect;

		/** The shader to use for this sprite. */
		CSharedShaderPtr						m_sspShader;

		/** The center point of the sprite (its origin) in pixels. */
		CVector2								m_vCenter;

		/** An offset in pixels. */
		CVector2								m_vOffset;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to a standard texture.
	 */
	typedef CSharedPtr<CSprite>					CSharedSpritePtr;

	// == Functions.
	/**
	 * Gets a constant reference to the texture rectangle.
	 *
	 * \return Returns a constant reference to the texture rectangle.
	 */
	LSE_INLINE const CTextureRect & CSprite::TexRect() const {
		return m_trTexRect;
	}

	/**
	 * Gets a reference to the texture rectangle.
	 *
	 * \return Returns a reference to the texture rectangle.
	 */
	LSE_INLINE CTextureRect & CSprite::TexRect() {
		return m_trTexRect;
	}

	/**
	 * Gets a constant reference to the shader used by this sprite.
	 *
	 * \return Returns a constant reference to the shader used by this sprite.
	 */
	LSE_INLINE const CSharedShaderPtr & LSE_CALL CSprite::Shader() const {
		return m_sspShader;
	}

	/**
	 * Gets a reference to the shader used by this sprite.
	 *
	 * \return Returns a reference to the shader used by this sprite.
	 */
	LSE_INLINE CSharedShaderPtr & LSE_CALL CSprite::Shader() {
		return m_sspShader;
	}

	/**
	 * Gets a constant reference to the center point.
	 *
	 * \return Returns a constant reference to the center point.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CSprite::Center() const {
		return m_vCenter;
	}

	/**
	 * Gets a reference to the center point.
	 *
	 * \return Returns a reference to the center point.
	 */
	LSE_INLINE CVector2 & LSE_CALL CSprite::Center() {
		return m_vCenter;
	}

	/**
	 * Sets the center of the sprite given normalized UV coordinates (0-1, with 0.5 being the center of the image).
	 *
	 * \param _fX Normalized X coordinate of the center.
	 * \param _fY Normalized Y coordinate of the center.
	 */
	LSE_INLINE void LSE_CALL CSprite::SetCenterNormalized( LSREAL _fX, LSREAL _fY ) {
		m_vCenter.x = m_trTexRect.GetWidth() * _fX;
		m_vCenter.y = m_trTexRect.GetHeight() * _fY;
	}

	/**
	 * Gets a constant reference to the offset.  The offset is a post-transform displacement for rendering the sprite.
	 *
	 * \return Returns a constant reference to the offset.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CSprite::Offset() const {
		return m_vOffset;
	}

	/**
	 * Gets a reference to the offset.  The offset is a post-transform displacement for rendering the sprite.
	 *
	 * \return Returns a reference to the offset.
	 */
	LSE_INLINE CVector2 & LSE_CALL CSprite::Offset() {
		return m_vOffset;
	}

}	// namespace lsr

#endif	// __LSR_SPRITE_H__
