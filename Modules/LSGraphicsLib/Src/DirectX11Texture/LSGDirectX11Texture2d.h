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
 * Description: A 2D Direct3D 11 texture.
 */


#ifndef __LSG_DIRECTX11TEXTURE2D_H__
#define __LSG_DIRECTX11TEXTURE2D_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../DirectX11/LSGDirectX11.h"
#include "../Texture/LSGTexture2dBase.h"
#include "LSGDirectX11TextureConstants.h"

namespace lsg {

	/**
	 * Class CDirectX11Texture2d
	 * \brief A 2D Direct3D 11 texture.
	 *
	 * Description: A 2D Direct3D 11 texture.
	 */
	class CDirectX11Texture2d : public CTexture2dBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX11Texture2d();
		LSE_CALLCTOR							~CDirectX11Texture2d();


		// == Functions.
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
		/** The actual texure resource. */
		ID3D11Texture2D *						m_ptTexture;

		/** The render target views. */
		CVectorPoD<ID3D11RenderTargetView *>	m_vRenderTargetViews;

		/** The shader resource view. */
		ID3D11ShaderResourceView *				m_psrvShaderView;


		// == Functions.
		/**
		 * Creates a DirectX 11 texture and fills it with our texel data.  Mipmaps are generated if necessary.
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
		LSE_INLINE LSBOOL LSE_CALL				ActivateApi( uint32_t _ui32Slot );

		/**
		 * Gets this object's LSG_SHADER_RESOURCE ID.
		 *
		 * \return Returns this object's LSG_SHADER_RESOURCE ID.
		 */
		LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL
												ShaderResourceApi();

		/**
		 * Gets the Direct3D 11 texture format given an LSImageLib format.
		 *
		 * \param _pfFormat The format to translate to Direct3D 11's texture format.
		 * \param _bSrgb If true, the format will include an _sRGB extension.
		 * \return Returns the Direct3D 11 texture format given an LSImageLib format or DXGI_FORMAT_UNKNOWN if there is no conversion.
		 */
		static LSE_INLINE DXGI_FORMAT LSE_FCALL	Direct3DFormat( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bSrgb = false );

		/**
		 * Is the Direct3D 11 format inverted compared to ours?
		 *
		 * \param _pfFormat The format to check if byte-swapping is necessary.
		 * \return Returns true if the texels in CImage need to be byte-swapped for Direct3D 11.
		 */
		static LSE_INLINE LSBOOL LSE_FCALL		Direct3DInvertFormat( LSI_PIXEL_FORMAT _pfFormat );

		/**
		 * Constructs a full mipmap chain using the given image array.
		 *
		 * \param _ptTexture The texture for which to construct a full mipmap chain(s).
		 * \param _piTexels The images from which to copy the full mipmap chain(s).
		 * \param _ui32Total Total images to which _piTexels points.
		 * \return Returns true if the all mipmaps were copied.
		 */
		static LSBOOL LSE_CALL					CreateMips( ID3D11Texture2D * _ptTexture, const CImage * _piTexels, uint32_t _ui32Total );

		/**
		 * Constructs a full mipmap chain using the given image.
		 *
		 * \param _pmsSurface The texture for which to construct a full mipmap chain.
		 * \param _tbTexels The image from which to copy the full mipmap chain.
		 * \param _ui32Width The image width.
		 * \param _ui32Height The image height.
		 * \param _pFormat The format of the texels in _tbTexels.
		 */
		static void LSE_CALL					CreateMips( D3D11_MAPPED_SUBRESOURCE * _pmsSurface,
			const CImageLib::CTexelBuffer &_tbTexels, uint32_t _ui32Width, uint32_t _ui32Height, LSI_PIXEL_FORMAT _pFormat );

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
	LSE_INLINE LSBOOL LSE_CALL CDirectX11Texture2d::ActivateApi( uint32_t /*_ui32Slot*/ ) { return true; }

	/**
	 * Gets this object's LSG_SHADER_RESOURCE ID.
	 *
	 * \return Returns this object's LSG_SHADER_RESOURCE ID.
	 */
	LSE_INLINE LSG_SHADER_RESOURCE * LSE_FCALL CDirectX11Texture2d::ShaderResourceApi() {
		return m_psrvShaderView;
	}

	/**
	 * Gets the Direct3D 11 texture format given an LSImageLib format.
	 *
	 * \param _pfFormat The format to translate to Direct3D 11's texture format.
	 * \param _bSrgb If true, the format will include an _sRGB extension.
	 * \return Returns the Direct3D 11 texture format given an LSImageLib format or DXGI_FORMAT_UNKNOWN if there is no conversion.
	 */
	LSE_INLINE DXGI_FORMAT LSE_FCALL CDirectX11Texture2d::Direct3DFormat( LSI_PIXEL_FORMAT _pfFormat, LSBOOL _bSrgb ) {
		if ( _bSrgb ) {
			static const DXGI_FORMAT fBasicFormats[] = {
				DXGI_FORMAT_A8_UNORM,				// LSI_PF_A8
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R3G3B2
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R5G6B5
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R4G4B4A4 (DXGI_FORMAT_B4G4R4A4_UNORM supported from Windows 8)
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R5G5B5A1
				DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,	// LSI_PF_R8G8B8
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,	// LSI_PF_R8G8B8A8
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R16G16B16A16
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R16G16B16A16F
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return fBasicFormats[_pfFormat-LSI_PF_A8]; }

			static const DXGI_FORMAT fDxtFormats[] = {
				DXGI_FORMAT_BC1_UNORM_SRGB,			// LSI_PF_DXT1
				DXGI_FORMAT_BC2_UNORM_SRGB,			// LSI_PF_DXT2
				DXGI_FORMAT_BC2_UNORM_SRGB,			// LSI_PF_DXT3
				DXGI_FORMAT_BC3_UNORM_SRGB,			// LSI_PF_DXT4
				DXGI_FORMAT_BC3_UNORM_SRGB,			// LSI_PF_DXT5
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC4U
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC4S
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC5U
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC5S
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC6U
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC6S
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_BC7U
				DXGI_FORMAT_BC7_UNORM_SRGB,			// LSI_PF_BC7_SRGB
			};
			if ( CImageLib::IsCompressed( _pfFormat ) ) { return fDxtFormats[_pfFormat-LSI_PF_DXT1]; }
		}
		else {
			static const DXGI_FORMAT fBasicFormats[] = {
				DXGI_FORMAT_A8_UNORM,				// LSI_PF_A8
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R3G3B2
				DXGI_FORMAT_B5G6R5_UNORM,			// LSI_PF_R5G6B5
				DXGI_FORMAT_UNKNOWN,				// LSI_PF_R4G4B4A4 (DXGI_FORMAT_B4G4R4A4_UNORM supported from Windows 8)
				DXGI_FORMAT_B5G5R5A1_UNORM,			// LSI_PF_R5G5B5A1
				DXGI_FORMAT_B8G8R8X8_UNORM,			// LSI_PF_R8G8B8
				DXGI_FORMAT_R8G8B8A8_UNORM,			// LSI_PF_R8G8B8A8
				DXGI_FORMAT_R16G16B16A16_UNORM,		// LSI_PF_R16G16B16A16
				DXGI_FORMAT_R16G16B16A16_FLOAT,		// LSI_PF_R16G16B16A16F
				DXGI_FORMAT_R32G32B32A32_FLOAT,		// LSI_PF_R32G32B32A32F
			};
			if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return fBasicFormats[_pfFormat-LSI_PF_A8]; }

			static const DXGI_FORMAT fDxtFormats[] = {
				DXGI_FORMAT_BC1_UNORM,				// LSI_PF_DXT1
				DXGI_FORMAT_BC2_UNORM,				// LSI_PF_DXT2
				DXGI_FORMAT_BC2_UNORM,				// LSI_PF_DXT3
				DXGI_FORMAT_BC3_UNORM,				// LSI_PF_DXT4
				DXGI_FORMAT_BC3_UNORM,				// LSI_PF_DXT5
				DXGI_FORMAT_BC4_UNORM,				// LSI_PF_BC4U
				DXGI_FORMAT_BC4_SNORM,				// LSI_PF_BC4S
				DXGI_FORMAT_BC5_UNORM,				// LSI_PF_BC5U
				DXGI_FORMAT_BC5_SNORM,				// LSI_PF_BC5S
				DXGI_FORMAT_BC6H_UF16,				// LSI_PF_BC6U
				DXGI_FORMAT_BC6H_SF16,				// LSI_PF_BC6S
				DXGI_FORMAT_BC7_UNORM,				// LSI_PF_BC7U
				DXGI_FORMAT_BC7_UNORM_SRGB,			// LSI_PF_BC7_SRGB
			};
			if ( CImageLib::IsCompressed( _pfFormat ) ) { return fDxtFormats[_pfFormat-LSI_PF_DXT1]; }
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	/**
	 * Is the Direct3D 11 format inverted compared to ours?
	 *
	 * \param _pfFormat The format to check if byte-swapping is necessary.
	 * \return Returns true if the texels in CImage need to be byte-swapped for Direct3D 11.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CDirectX11Texture2d::Direct3DInvertFormat( LSI_PIXEL_FORMAT _pfFormat ) {
		static const LSBOOL bInvert[] = {
			false,				// LSI_PF_A8
			false,				// LSI_PF_R3G3B2
			true,				// LSI_PF_R5G6B5
			true,				// LSI_PF_R4G4B4A4 (DXGI_FORMAT_B4G4R4A4_UNORM supported from Windows 8)
			true,				// LSI_PF_R5G5B5A1
			false,				// LSI_PF_R8G8B8
			false,				// LSI_PF_R8G8B8A8
			false,				// LSI_PF_R16G16B16A16
			false,				// LSI_PF_R16G16B16A16F
			false,				// LSI_PF_R32G32B32A32F
		};
		if ( _pfFormat >= LSI_PF_A8 && _pfFormat <= LSI_PF_R32G32B32A32F ) { return bInvert[_pfFormat-LSI_PF_A8]; }
		return false;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11TEXTURE2D_H__
