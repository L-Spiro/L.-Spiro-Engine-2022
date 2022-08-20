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
 * Description: A part of a texture, consisting of a texture pointer and a part of that texture (rectangular).
 *	Origin is the upper-left corner of the texture.
 */


#ifndef __LSR_TEXTURERECT_H__
#define __LSR_TEXTURERECT_H__

#include "../LSRSpriteLib.h"
#include "Texture/LSGTexture2d.h"
#include "Vector/LSMVector2.h"
#include "Vector/LSMVector4.h"

namespace lsr {

	/**
	 * Class CTextureRect
	 * \brief A part of a texture, consisting of a texture pointer and a part of that texture (rectangular).
	 *
	 * Description: A part of a texture, consisting of a texture pointer and a part of that texture (rectangular).
	 *	Origin is the upper-left corner of the texture.
	 */
	class CTextureRect {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CTextureRect();
		LSE_CALLCTOR							CTextureRect( const CSharedTexture2dPtr &_st2Texture,
			uint32_t _ui32Left, uint32_t _ui32Top, uint32_t _ui32Width, uint32_t _ui32Height );


		// == Functions.
		/**
		 * Sets the texture pointer.  Reference-counting on the texure must be done at a higher level for performance
		 *	reasons.
		 *
		 * \param _st2Texture The texture to set.
		 */
		LSE_INLINE void						SetTexture( const CSharedTexture2dPtr &_st2Texture );

		/**
		 * Gets the texture pointer, which may be NULL.
		 *
		 * \return Returns the current texture pointer, which may be NULL.
		 */
		LSE_INLINE CSharedTexture2dPtr			GetTexture() const;

		/**
		 * Sets the upper-left corner in texel units.
		 *
		 * \param _ui32Left Left coordinate inside the texture, in texels.
		 * \param _ui32Top Top coordinate inside the texture, in texels.
		 */
		LSE_INLINE void						SetOrigin( uint32_t _ui32Left, uint32_t _ui32Top );

		/**
		 * Sets the width and height in texel units.
		 *
		 * \param _ui32Width Width of the rectangle, in texels.
		 * \param _ui32Height Height of the rectangle, in texels.
		 */
		LSE_INLINE void						SetSize( uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Sets the left of the texture rectangle.
		 *
		 * \param _ui32Left Left coordinate inside the texture, in texels.
		 */
		LSE_INLINE void						SetLeft( uint32_t _ui32Left );

		/**
		 * Sets the top of the texture rectangle.
		 *
		 * \param _ui32Top Top coordinate inside the texture, in texels.
		 */
		LSE_INLINE void						SetTop( uint32_t _ui32Top );

		/**
		 * Sets the right of the texture rectangle.
		 *
		 * \param _ui32Right Right coordinate inside the texture, in texels.
		 */
		LSE_INLINE void						SetRight( uint32_t _ui32Right );

		/**
		 * Sets the bottom of the texture rectangle.
		 *
		 * \param _ui32Bottom Bottom coordinate inside the texture, in texels.
		 */
		LSE_INLINE void						SetBottom( uint32_t _ui32Bottom );

		/**
		 * Sets the full dimensions of the texture rectangle.
		 *
		 * \param _ui32Left Left coordinate inside the texture, in texels.
		 * \param _ui32Top Top coordinate inside the texture, in texels.
		 * \param _ui32Width Width of the rectangle, in texels.
		 * \param _ui32Height Height of the rectangle, in texels.
		 */
		LSE_INLINE void						SetDimensions( uint32_t _ui32Left, uint32_t _ui32Top,
			uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Sets the full dimensions of the texture rectangle.
		 *
		 * \param _st2Texture The texture pointer to set.
		 * \param _ui32Left Left coordinate inside the texture, in texels.
		 * \param _ui32Top Top coordinate inside the texture, in texels.
		 * \param _ui32Width Width of the rectangle, in texels.
		 * \param _ui32Height Height of the rectangle, in texels.
		 */
		LSE_INLINE void						SetDimensions( const CSharedTexture2dPtr &_st2Texture,
			uint32_t _ui32Left, uint32_t _ui32Top,
			uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Sets the normalized dimensions of the texture rectangle (0-1 spans the full width/height
		 *	of the texture regardless of the texture size).
		 *
		 * \param _fLeft Left coordinate inside the texture, in normalized UV coordinates.
		 * \param _fTop Top coordinate inside the texture, in normalized UV coordinates.
		 * \param _fWidth Width of the rectangle, in normalized UV coordinates.
		 * \param _fHeight Height of the rectangle, in normalized UV coordinates.
		 */
		LSE_INLINE void						SetNormalizedDimensions( LSREAL _fLeft, LSREAL _fTop,
			LSREAL _fWidth, LSREAL _fHeight );

		/**
		 * Sets the normalized dimensions of the texture rectangle (0-1 spans the full width/height
		 *	of the texture regardless of the texture size).
		 *
		 * \param _st2Texture The texture pointer to set.
		 * \param _fLeft Left coordinate inside the texture, in normalized UV coordinates.
		 * \param _fTop Top coordinate inside the texture, in normalized UV coordinates.
		 * \param _fWidth Width of the rectangle, in normalized UV coordinates.
		 * \param _fHeight Height of the rectangle, in normalized UV coordinates.
		 */
		LSE_INLINE void						SetNormalizedDimensions( CSharedTexture2dPtr _st2Texture,
			LSREAL _fLeft, LSREAL _fTop,
			LSREAL _fWidth, LSREAL _fHeight );

		/**
		 * Gets the left of the texture rectangle.
		 *
		 * \return Returns the left of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetLeft() const;

		/**
		 * Gets the right of the texture rectangle.
		 *
		 * \return Returns the right of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetRight() const;

		/**
		 * Gets the width of the texture rectangle.
		 *
		 * \return Returns the width of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetWidth() const;

		/**
		 * Gets the height of the texture rectangle.
		 *
		 * \return Returns the height of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetHeight() const;

		/**
		 * Gets the top of the texture rectangle.
		 *
		 * \return Returns the top of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetTop() const;

		/**
		 * Gets the bottom of the texture rectangle.
		 *
		 * \return Returns the bottom of the texture rectangle.
		 */
		LSE_INLINE uint32_t						GetBottom() const;

		/**
		 * Gets the texture width (0 if no texture is set).
		 *
		 * \return Returns the texture width, if any.
		 */
		LSE_INLINE uint32_t						GetTextureWidth() const;

		/**
		 * Gets the texture height (0 if no texture is set).
		 *
		 * \return Returns the texture height, if any.
		 */
		LSE_INLINE uint32_t						GetTextureHeight() const;

		/**
		 * Gets the normalized UV coordinates.
		 *
		 * \return Returns the normalized UV coordinates.
		 */
		LSE_INLINE const CVector4 & LSE_CALL	Uvs() const;


	protected :
		// == Members.
		/** The upper-left and lower-right corners within the texture. */
		mutable CVector4						m_vDimensions;

		/** Texture pointer. */
		CSharedTexture2dPtr						m_st2SrcTexture;

		/** Left part of the texture, in texels. */
		uint32_t								m_ui32Left;

		/** Top part of the texture, in texels. */
		uint32_t								m_ui32Top;

		/** Width of the texture, in texels. */
		uint32_t								m_ui32Width;

		/** Height of the texture, in texels. */
		uint32_t								m_ui32Height;


		// == Functions.
		/**
		 * Updates the buffered origin and size values.
		 */
		LSE_INLINE void						UpdateInternal() const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Sets the texture pointer.  Reference-counting on the texure must be done at a higher level for performance
	 *	reasons.
	 *
	 * \param _st2Texture The texture to set.
	 */
	LSE_INLINE void CTextureRect::SetTexture( const CSharedTexture2dPtr &_st2Texture ) {
		m_st2SrcTexture = _st2Texture;
		UpdateInternal();
	}

	/**
	 * Gets the texture pointer, which may be NULL.
	 *
	 * \return Returns the current texture pointer, which may be NULL.
	 */
	LSE_INLINE CSharedTexture2dPtr CTextureRect::GetTexture() const {
		return m_st2SrcTexture;
	}

	/**
	 * Sets the upper-left corner in texel units.
	 *
	 * \param _ui32Left Left coordinate inside the texture, in texels.
	 * \param _ui32Top Top coordinate inside the texture, in texels.
	 */
	LSE_INLINE void CTextureRect::SetOrigin( uint32_t _ui32Left, uint32_t _ui32Top ) {
		m_ui32Left = _ui32Left;
		m_ui32Top = _ui32Top;
		UpdateInternal();
	}

	/**
	 * Sets the width and height in texel units.
	 *
	 * \param _ui32Width Width of the rectangle, in texels.
	 * \param _ui32Height Height of the rectangle, in texels.
	 */
	LSE_INLINE void CTextureRect::SetSize( uint32_t _ui32Width, uint32_t _ui32Height ) {
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		UpdateInternal();
	}

	/**
	 * Sets the left of the texture rectangle.
	 *
	 * \param _ui32Left Left coordinate inside the texture, in texels.
	 */
	LSE_INLINE void CTextureRect::SetLeft( uint32_t _ui32Left ) {
		m_ui32Left = _ui32Left;
		UpdateInternal();
	}

	/**
	 * Sets the top of the texture rectangle.
	 *
	 * \param _ui32Top Top coordinate inside the texture, in texels.
	 */
	LSE_INLINE void CTextureRect::SetTop( uint32_t _ui32Top ) {
		m_ui32Top = _ui32Top;
		UpdateInternal();
	}

	/**
	 * Sets the right of the texture rectangle.
	 *
	 * \param _ui32Right Right coordinate inside the texture, in texels.
	 */
	LSE_INLINE void CTextureRect::SetRight( uint32_t _ui32Right ) {
		m_ui32Width = (_ui32Right >= m_ui32Left) ? _ui32Right - m_ui32Left : 0;
		UpdateInternal();
	}

	/**
	 * Sets the bottom of the texture rectangle.
	 *
	 * \param _ui32Bottom Bottom coordinate inside the texture, in texels.
	 */
	LSE_INLINE void CTextureRect::SetBottom( uint32_t _ui32Bottom ) {
		m_ui32Height = (_ui32Bottom >= m_ui32Top) ? _ui32Bottom - m_ui32Top : 0;
		UpdateInternal();
	}

	/**
	 * Sets the full dimensions of the texture rectangle.
	 *
	 * \param _ui32Left Left coordinate inside the texture, in texels.
	 * \param _ui32Top Top coordinate inside the texture, in texels.
	 * \param _ui32Width Width of the rectangle, in texels.
	 * \param _ui32Height Height of the rectangle, in texels.
	 */
	LSE_INLINE void CTextureRect::SetDimensions( uint32_t _ui32Left, uint32_t _ui32Top,
		uint32_t _ui32Width, uint32_t _ui32Height ) {
		m_ui32Left = _ui32Left;
		m_ui32Top = _ui32Top;
		m_ui32Width = _ui32Width;
		m_ui32Height = _ui32Height;
		UpdateInternal();
	}

	/**
	 * Sets the full dimensions of the texture rectangle.
	 *
	 * \param _st2Texture The texture pointer to set.
	 * \param _ui32Left Left coordinate inside the texture, in texels.
	 * \param _ui32Top Top coordinate inside the texture, in texels.
	 * \param _ui32Width Width of the rectangle, in texels.
	 * \param _ui32Height Height of the rectangle, in texels.
	 */
	LSE_INLINE void CTextureRect::SetDimensions( const CSharedTexture2dPtr &_st2Texture,
		uint32_t _ui32Left, uint32_t _ui32Top,
		uint32_t _ui32Width, uint32_t _ui32Height ) {
		m_st2SrcTexture = _st2Texture;
		SetDimensions( _ui32Left, _ui32Top, _ui32Width, _ui32Height );		
	}

	/**
	 * Sets the normalized dimensions of the texture rectangle (0-1 spans the full width/height
	 *	of the texture regardless of the texture size).
	 *
	 * \param _fLeft Left coordinate inside the texture, in normalized UV coordinates.
	 * \param _fTop Top coordinate inside the texture, in normalized UV coordinates.
	 * \param _fWidth Width of the rectangle, in normalized UV coordinates.
	 * \param _fHeight Height of the rectangle, in normalized UV coordinates.
	 */
	LSE_INLINE void CTextureRect::SetNormalizedDimensions( LSREAL _fLeft, LSREAL _fTop,
		LSREAL _fWidth, LSREAL _fHeight ) {
		uint32_t ui32Width = GetTextureWidth();
		uint32_t ui32Height = GetTextureHeight();

		m_vDimensions = CVector4( _fLeft,
			_fTop,
			_fLeft + _fWidth,
			_fTop + _fHeight );
		m_ui32Left = uint32_t( _fLeft * ui32Width );
		m_ui32Top = uint32_t( _fTop * ui32Height );
		m_ui32Width = uint32_t( _fWidth * ui32Width );
		m_ui32Height = uint32_t( _fHeight * ui32Height );
	}

	/**
	 * Sets the normalized dimensions of the texture rectangle (0-1 spans the full width/height
	 *	of the texture regardless of the texture size).
	 *
	 * \param _st2Texture The texture pointer to set.
	 * \param _fLeft Left coordinate inside the texture, in normalized UV coordinates.
	 * \param _fTop Top coordinate inside the texture, in normalized UV coordinates.
	 * \param _fWidth Width of the rectangle, in normalized UV coordinates.
	 * \param _fHeight Height of the rectangle, in normalized UV coordinates.
	 */
	LSE_INLINE void CTextureRect::SetNormalizedDimensions( CSharedTexture2dPtr _st2Texture,
		LSREAL _fLeft, LSREAL _fTop,
		LSREAL _fWidth, LSREAL _fHeight ) {
		m_st2SrcTexture = _st2Texture;
		SetNormalizedDimensions( _fLeft, _fTop, _fWidth, _fHeight );
	}

	/**
	 * Gets the left of the texture rectangle.
	 *
	 * \return Returns the left of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetLeft() const {
		 return m_ui32Left;
	}

	/**
	 * Gets the right of the texture rectangle.
	 *
	 * \return Returns the right of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetRight() const {
		return m_ui32Left + m_ui32Width;
	}

	/**
	 * Gets the width of the texture rectangle.
	 *
	 * \return Returns the width of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetWidth() const {
		return m_ui32Width;
	}

	/**
	 * Gets the height of the texture rectangle.
	 *
	 * \return Returns the height of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetHeight() const {
		return m_ui32Height;
	}

	/**
	 * Gets the top of the texture rectangle.
	 *
	 * \return Returns the top of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetTop() const {
		return m_ui32Top;
	}

	/**
	 * Gets the bottom of the texture rectangle.
	 *
	 * \return Returns the bottom of the texture rectangle.
	 */
	LSE_INLINE uint32_t CTextureRect::GetBottom() const {
		return m_ui32Top + m_ui32Height;
	}

	/**
	 * Gets the texture width (0 if no texture is set).
	 *
	 * \return Returns the texture width, if any.
	 */
	LSE_INLINE uint32_t CTextureRect::GetTextureWidth() const {
		return m_st2SrcTexture.Valid() ? m_st2SrcTexture->GetWidth() : 0;
	}

	/**
	 * Gets the texture height (0 if no texture is set).
	 *
	 * \return Returns the texture height, if any.
	 */
	LSE_INLINE uint32_t CTextureRect::GetTextureHeight() const {
		return m_st2SrcTexture.Valid() ? m_st2SrcTexture->GetHeight() : 0;
	}

	/**
	 * Gets the normalized UV coordinates.
	 *
	 * \return Returns the normalized UV coordinates.
	 */
	LSE_INLINE const CVector4 & LSE_CALL CTextureRect::Uvs() const {
		return m_vDimensions;
	}

	/**
	 * Updates the buffered origin and size values.
	 */
	LSE_INLINE void CTextureRect::UpdateInternal() const {
		uint32_t ui32Width = GetTextureWidth();
		uint32_t ui32Height = GetTextureHeight();

		LSREAL fFactorX = ui32Width ? (LSM_ONE / ui32Width) : LSM_ZERO;
		LSREAL fFactorY = ui32Height ? (LSM_ONE / ui32Height) : LSM_ZERO;
		m_vDimensions = CVector4( GetLeft() * fFactorX,
			GetTop() * fFactorY,
			GetRight() * fFactorX,
			GetBottom() * fFactorY );
	}

}	// namespace lsr

#endif	// __LSR_TEXTURERECT_H__
