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
 * Description: A 2D Direct3D 9 texture.
 */


#ifndef __LSG_DIRECTX9TEXTURE2D_H__
#define __LSG_DIRECTX9TEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

#include "../DirectX9/LSGDirectX9.h"
#include "../DirectX9/LSGDirectX9LosableResource.h"
#include "../Texture/LSGTexture2dBase.h"
#include "LSGDirectX9TextureConstants.h"

namespace lsg {

	/**
	 * Class CDirectX9Texture2d
	 * \brief A 2D Direct3D 9 texture.
	 *
	 * Description: A 2D Direct3D 9 texture.
	 */
	class CDirectX9Texture2d : public CDirectX9LosableResource, public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX9Texture2d();
		LSE_CALLCTOR							~CDirectX9Texture2d();


		// == Functions.
		/**
		 * Must perform some action when the device is lost.
		 */
		virtual void LSE_CALL					OnDeviceLost();

		/**
		 * Must renew resources when the device is reset.
		 *
		 * \return Return true if the renewal is successful, false otherwise.
		 */
		virtual LSBOOL LSE_CALL					OnDeviceReset();

		/**
		 * Resets everything to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Activates the object as a color render target.
		 *
		 * \param _ui32Mipmap The mipmap to set.
		 * \param _ui32Slice The slice in a 3D texture to set.
		 * \param _ui32Slot The color slot into which to activate the render target.
		 * \return Returns true if it is ready to be used as a render target.
		 */
		virtual LSBOOL LSE_CALL					SetAsColorRenderTarget( uint32_t _ui32Mipmap, uint32_t _ui32Slice, uint32_t _ui32Slot ) const;


	protected :
		// == Members.
		/** The actual DirectX 9 texture. */
		IDirect3DTexture9 *						m_pd3dtTexture;

		/** For resources that can be lost on device-resets, we need a copy of the
		 *	original texel data. */
		CImage									m_iOrigTexels;


		// == Functions.
		/**
		 * Creates a DirectX 9 texture and fills it with our texel data.  Mipmaps are generated if necessary.
		 *
		 * \param _piImages Array of images to use to create the texture.
		 * \param _ui32Total Number of images to which _piImages points.
		 * \return Returns tue if the creation and filling of the texture succeeds.  False indicates a resource error.
		 */
		LSBOOL LSE_CALL							CreateApiTexture( const CImage * _piImages, uint32_t _ui32Total );

		/**
		 * Activates the texture in the given slot.
		 *
		 * \param _ui32Slot Slot into which to activate the texture.  Must be less than LSG_MAX_TEXTURE_UNITS.
		 * \return Returns true if the texture was activated in the given slot.
		 */
		LSBOOL LSE_CALL							ActivateApi( uint32_t _ui32Slot );

		/**
		 * Gets this object's LSG_SHADER_RESOURCE ID.
		 *
		 * \return Returns this object's LSG_SHADER_RESOURCE ID.
		 */
		LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL
												ShaderResourceApi();

		/**
		 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *
		 * \param _pd3dtTexture The texture for which to generate mipmaps.
		 * \param _iImage The image to use for the mipmapping process.
		 * \return Returns true if there were no failures.  Failure implies the resource was not lockable or _iImage does not
		 *	contain the proper number of mipmap levels.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( IDirect3DTexture9 * _pd3dtTexture, const CImage &_iImage );

		/**
		 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *
		 * \param _pd3dtTexture The texture for which to generate mipmaps.
		 * \param _iImage The image to use for the mipmapping process.
		 * \return Returns true if there were no failures.  Failure implies the resource was not lockable.
		 */
		LSBOOL LSE_CALL							GenerateDxtMipMaps( IDirect3DTexture9 * _pd3dtTexture, const CImage &_iImage );

		/**
		 * Handles mipmap generation for the given texture, which must be lockable.  Layer 0 (the non-scaled level) is also handled by this
		 *	function.  If mipmaps have not been specified for this texture, only the first mipmap level is created.
		 *
		 * \param _pd3dsSurface The surface to which to copy our texel data.
		 * \param _tbTexels The texel data to copy.
		 * \param _pfFormat Format of the data to copy.
		 * \param _ui32Width Width of the image data.
		 * \param _ui32Height Height of the image data.
		 * \return Returns true if there were no failures.  Failure implies the resource was not lockable.
		 */
		LSBOOL LSE_CALL							GenerateMipMaps( IDirect3DSurface9 * _pd3dsSurface,
			const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat, uint32_t _ui32Width, uint32_t _ui32Height );

		/**
		 * Gets the pool of the texture based on m_ui32Usage.
		 *
		 * \return Returns the pool of the texture based on m_ui32Usage.
		 */
		D3DPOOL LSE_CALL						Direct3DPool() const;

		/**
		 * Gets the Direct3D 9 texture format given an LSImageLib format.
		 *
		 * \param _pfFormat The format to translate to Direct3D 9's texture format.
		 * \return Returns the Direct3D 9 texture format given an LSImageLib format or D3DFMT_UNKNOWN if there is no conversion.
		 */
		static LSE_INLINE D3DFORMAT LSE_CALL	Direct3DFormat( LSI_PIXEL_FORMAT _pfFormat );

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
		 * \return Returns a texel as an uint64_t value.
		 */
		static LSE_INLINE uint64_t LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
			LSI_PIXEL_FORMAT _pfReqFormat,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y );

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
		static LSE_INLINE void LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y,
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
		static LSE_INLINE void LSE_CALL		GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
			uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
			uint32_t _ui32X, uint32_t _ui32Y,
			float * _pfReturn );


	private :
		// == Types.
		typedef CTexture2dBase					Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL CDirectX9Texture2d::ShaderResourceApi() {
		return m_pd3dtTexture;
	}

	/**
	 * Gets the Direct3D 9 texture format given an LSImageLib format.
	 *
	 * \param _pfFormat The format to translate to Direct3D 9's texture format.
	 * \return Returns the Direct3D 9 texture format given an LSImageLib format or D3DFMT_UNKNOWN if there is no conversion.
	 */
	LSE_INLINE D3DFORMAT LSE_CALL CDirectX9Texture2d::Direct3DFormat( LSI_PIXEL_FORMAT _pfFormat ) {
		static const D3DFORMAT fBasicFormats[] = {
			D3DFMT_A8,						// LSI_PF_A8
			D3DFMT_UNKNOWN,					// LSI_PF_R3G3B2
			D3DFMT_R5G6B5,					// LSI_PF_R5G6B5
			D3DFMT_A4R4G4B4,				// LSI_PF_R4G4B4A4
			D3DFMT_A1R5G5B5,				// LSI_PF_R5G5B5A1
			D3DFMT_X8R8G8B8,				// LSI_PF_R8G8B8
			D3DFMT_A8R8G8B8,				// LSI_PF_R8G8B8A8
			D3DFMT_A16B16G16R16,			// LSI_PF_R16G16B16A16
			D3DFMT_A16B16G16R16F,			// LSI_PF_R16G16B16A16F
			D3DFMT_A32B32G32R32F,			// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return fBasicFormats[_pfFormat-LSI_PF_A8]; }

		static const D3DFORMAT fDxtFormats[] = {
			D3DFMT_DXT1,					// LSI_PF_DXT1
			D3DFMT_DXT2,					// LSI_PF_DXT2
			D3DFMT_DXT3,					// LSI_PF_DXT3
			D3DFMT_DXT4,					// LSI_PF_DXT4
			D3DFMT_DXT5,					// LSI_PF_DXT5
		};
		if ( CImageLib::IsCompressed( _pfFormat ) ) { return fDxtFormats[_pfFormat-LSI_PF_DXT1]; }
		return D3DFMT_UNKNOWN;
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
	 * \return Returns a texel as an uint64_t value.
	 */
	LSE_INLINE uint64_t LSE_CALL CDirectX9Texture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels, LSI_PIXEL_FORMAT _pfFormat,
		LSI_PIXEL_FORMAT _pfReqFormat,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y ) {
		return CImageLib::IntegerTexelToIntegerTexel( _pfFormat, _pfReqFormat, (*reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize])), LSI_G_NONE );
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
	LSE_INLINE void LSE_CALL CDirectX9Texture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y,
		CFloat16 * _pf16Return ) {
		// Can be copied via a single 64-bit bitwise copy.
		uint64_t * pui64Ret = reinterpret_cast<uint64_t *>(_pf16Return);
		(*pui64Ret) = (*reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]));
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
	LSE_INLINE void LSE_CALL CDirectX9Texture2d::GetTexelAt( const CImageLib::CTexelBuffer &_tbTexels,
		uint32_t _ui32RowWidth, uint32_t _ui32TexelSize,
		uint32_t _ui32X, uint32_t _ui32Y,
		float * _pfReturn ) {
		// Can be copied via 2 64-bit bitwise copies.
		uint64_t * pui64Ret = reinterpret_cast<uint64_t *>(_pfReturn);
		const uint64_t * pui64Src = reinterpret_cast<const uint64_t *>(&_tbTexels[_ui32Y*_ui32RowWidth+_ui32X*_ui32TexelSize]);
		(*pui64Ret++) = (*pui64Src++);
		(*pui64Ret) = (*pui64Src);
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9TEXTURE2D_H__
