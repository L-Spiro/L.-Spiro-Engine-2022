/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A 2D Metal texture.
 */


#ifndef __LSG_METALTEXTURE2D_H__
#define __LSG_METALTEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Metal/LSGMetal.h"
#include "../Texture/LSGTexture2dBase.h"
#include "LSGMetalTextureConstants.h"

namespace lsg {

	/**
	 * Class CMetalTexture2d
	 * \brief A 2D Metal texture.
	 *
	 * Description: A 2D Metal texture.
	 */
	class CMetalTexture2d : public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMetalTexture2d();
		LSE_CALLCTOR							~CMetalTexture2d();


		// == Functions.
		/**
		 * Resets everything to scratch.
		 */
		LSVOID LSE_CALL							Reset();


	protected :
		// == Members.
		/** ID of the texture as recognized by Metal. */
		const LSVOID *							m_pvTexture;


		// == Functions.
		/**
		 * Creates a Metal texture and fills it with our texel data.  Mipmaps are generated if necessary.
		 *
		 * \param _piImages Array of images to use to create the texture.
		 * \param _ui32Total Number of images to which _piImages points.
		 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
		 */
		LSBOOL LSE_CALL							CreateApiTexture( const CImage * _piImages, LSUINT32 _ui32Total );
		
		/**
		 * Activates the texture in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
		 * \return Returns true if the texture was activated in the given slot.
		 */
		LSE_INLINE LSBOOL LSE_CALL				ActivateApi( LSUINT32 _ui32Slot );
		
		/**
		 * Gets this object's LSG_SHADER_RESOURCE ID.
		 *
		 * \return Returns this object's LSG_SHADER_RESOURCE ID.
		 */
		LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL
												ShaderResourceApi();

		/**
		 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *	It acts on the currently bound texture.
		 *
		 * \param _piImages The source image(s).
		 * \param _ui32Total Total images to which _piImages points.
		 * \return Returns true if there were no failures.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( const CImage * _piImages, LSUINT32 _ui32Total );

		/**
		 * Handles mipmap generation for the given texture.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *	It acts on the currently bound texture.
		 *
		 * \param _iMipLevel The surface level to which to copy our texel data.
		 * \param _tbTexels The texel data to copy.
		 * \param _pfFormat Format of the data to copy.
		 * \param _ui32Width Width of the image data.
		 * \param _ui32Height Height of the image data.
		 * \param _ui32Slice Array slice to which to copy our texel data.
		 * \return Returns true if there were no failures.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( LSUINT32 _iMipLevel,
			const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, LSUINT32 _ui32Width, LSUINT32 _ui32Height,
			LSUINT32 _ui32Slice );
		
		/**
		 * Gets a texel from a buffer given the row width, X, Y, format, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _pfFormat Format of the texels in _tbTexels.
		 * \param _ui32Mip Mipmap level.
		 * \param _ui32Slice Target array slice.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 */
		LSVOID LSE_CALL							CopyTexels( const CImageLib::CTexelBuffer &_tbTexels,
			LSI_PIXEL_FORMAT _pfFormat,
			LSUINT32 _ui32Mip, LSUINT32 _ui32Slice,
			LSUINT32 _ui32Width, LSUINT32 _ui32Height );
		
		/**
		 * Copies the given texel buffer into the given slice of this object's Metal texture.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _pfFormat Format of the data.
		 * \param _pfReqFormat Format of the output data.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \return Returns a texel as an LSUINT64 value.
		 */
		static LSE_INLINE LSUINT64 LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
			LSI_PIXEL_FORMAT _pfReqFormat,
			LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
			LSUINT32 _ui32X, LSUINT32 _ui32Y );
			
		/**
		 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \param _f16Return Holds the returned values.
		 */
		static LSE_INLINE LSVOID LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
			LSUINT32 _ui32X, LSUINT32 _ui32Y,
			CFloat16 * _pf16Return );

		/**
		 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
		 *
		 * \param _tbTexels The buffer from which to extract the texel.
		 * \param _ui32RowWidth Row width of the image data.
		 * \param _ui32TexelSize Texel size.
		 * \param _ui32X The X coordinate of the texel to get.
		 * \param _ui32Y The Y coordinate of the texel to get.
		 * \param _pfReturn Holds the returned values.
		 */
		static LSE_INLINE LSVOID LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
			LSUINT32 _ui32X, LSUINT32 _ui32Y,
			LSFLOAT * _pfReturn );

	private :
		// == Types.
		typedef CTexture2dBase					Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Activates the texture in the given slot.
	 *
	 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
	 * \return Returns true if the texture was activated in the given slot.
	 */
	LSE_INLINE LSBOOL LSE_CALL CMetalTexture2d::ActivateApi( LSUINT32 _ui32Slot ) {
		return true;
	}
	
	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL CMetalTexture2d::ShaderResourceApi() {
		if ( !m_pvTexture ) { return NULL; }
		return const_cast<LSG_SHADER_RESOURCE *>(m_pvTexture);
	}
	
	/**
	 * Gets a texel from a buffer given the row width, X, Y, format, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _pfFormat Format of the data.
	 * \param _pfReqFormat Format of the output data.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \return Returns a texel as an LSUINT64 value.
	 */
	LSE_INLINE LSUINT64 LSE_CALL CMetalTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
		LSI_PIXEL_FORMAT _pfReqFormat,
		LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
		LSUINT32 _ui32X, LSUINT32 _ui32Y ) {
		return CImageLib::IntegerTexelToIntegerTexel( _pfFormat, _pfReqFormat, (*reinterpret_cast<const LSUINT64 *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize])) );
	}

	/**
	 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \param _f16Return Holds the returned values.
	 */
	LSE_INLINE LSVOID LSE_CALL CMetalTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
		LSUINT32 _ui32X, LSUINT32 _ui32Y,
		CFloat16 * _pf16Return ) {
		// Can be copied via a single 64-bit bitwise copy.
		LSUINT64 * pui64Ret = reinterpret_cast<LSUINT64 *>(_pf16Return);
		(*pui64Ret) = (*reinterpret_cast<const LSUINT64 *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]));
	}

	/**
	 * Gets a texel from a buffer given the row width, X, Y, and texel buffer.
	 *
	 * \param _tbTexels The buffer from which to extract the texel.
	 * \param _ui32RowWidth Row width of the image data.
	 * \param _ui32TexelSize Texel size.
	 * \param _ui32X The X coordinate of the texel to get.
	 * \param _ui32Y The Y coordinate of the texel to get.
	 * \param _pfReturn Holds the returned values.
	 */
	LSE_INLINE LSVOID LSE_CALL CMetalTexture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		LSUINT32 _ui32RowWidth, LSUINT32 _ui32TexelSize,
		LSUINT32 _ui32X, LSUINT32 _ui32Y,
		LSFLOAT * _pfReturn ) {
		// Can be copied via 2 64-bit bitwise copies.
		LSUINT64 * pui64Ret = reinterpret_cast<LSUINT64 *>(_pfReturn);
		const LSUINT64 * pui64Src = reinterpret_cast<const LSUINT64 *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]);
		(*pui64Ret++) = (*pui64Src++);
		(*pui64Ret) = (*pui64Src);
	}

}	// namespace lsg

#endif	// #ifdef LSG_METAL

#endif	// __LSG_METALTEXTURE2D_H__
