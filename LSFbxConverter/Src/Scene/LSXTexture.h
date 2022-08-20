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
 * Description: A texture from the FBX file.
 */


#ifndef __LSX_TEXTURE_H__
#define __LSX_TEXTURE_H__

#include "../LSXFbxConverter.h"
#include "Atomic/LSSTDAtomic.h"
#include "Models/LSDModelAttributes.h"
#include "Set/LSTLSet.h"
#include "String/LSTLString.h"

namespace lsx {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// MACROS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** sRGB flag. */
#define LSX_TEX_sRGB							(1 << 0)
	/** RGB access. */
#define LSX_TEX_RGB_ACCESS						(1 << 1)
	/** Alpha access. */
#define LSX_TEX_ALPHA_ACCESS					(1 << 2)
	/** Normalize mipmaps. */
#define LSX_TEX_NORMALIZE_MIPS					(1 << 3)


	/**
	 * Class CTexture
	 * \brief A texture from the FBX file.
	 *
	 * Description: A texture from the FBX file.
	 */
	class CTexture {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CTexture();
		LSE_CALLCTOR							~CTexture();


		// == Enumerations.
		/** Component access. */
		enum LSX_COMPONENT {
			LSX_C_RGB,							/**< RGB component(s). */
			LSX_C_A,							/**< Alpha component. */
		};


		// == Types.
		/** Texture reference (file, component (RGB or A), and access type (sRGB or not)). */
		typedef struct LSX_TEXTURE_REF {
			FbxFileTexture *					pftTexture;			/**< Texture pointer. */
			LSUINT16							ui16Flags;			/**< sRGB, etc. */
			LSUINT16							ui16Component;		/**< RGB or A. */
			LSUINT32							ui32TextureId;		/**< Finalized texture ID. */


			// == Various constructors.
			LSE_CALLCTOR						LSX_TEXTURE_REF() :
				pftTexture( NULL ),
				ui16Flags( 0 ),
				ui16Component( 0 ),
				ui32TextureId( 0 ) {
			}

			
			// == Operators.
			/**
			 * Less-than operator.
			 *
			 * \param _trOther Right operand.
			 * \return Returns true if this object is less than _trOther.
			 */
			LSE_INLINE LSBOOL LSE_FCALL			operator < ( const LSX_TEXTURE_REF &_trOther ) const {
				if ( ui16Flags < _trOther.ui16Flags ) { return true; }
				if ( ui16Component < _trOther.ui16Component ) { return true; }
				return CStd::StrCmp( pftTexture->GetFileName(), _trOther.pftTexture->GetFileName() ) < 0;
			}

			/**
			 * Equality operator.
			 *
			 * \param _trOther Right operand.
			 * \return Returns true if this object is equal to _trOther.
			 */
			LSE_INLINE LSBOOL LSE_FCALL			operator == ( const LSX_TEXTURE_REF &_trOther ) const {
				return CStd::StrCmp( pftTexture->GetFileName(), _trOther.pftTexture->GetFileName() ) == 0 &&
					ui16Flags == _trOther.ui16Flags &&
					ui16Component == _trOther.ui16Component;
			}
		} * LPLSX_TEXTURE_REF, * const LPCLSX_TEXTURE_REF;

		/** A set of texture references. */
		typedef CSet<LSX_TEXTURE_REF>			LSX_TEXTURE_REF_SET;

		/** Texture transform data. */
		typedef struct LSX_TEXTURE_TRANSFORM {
			LSFLOAT								fScaleX;			/**< U scale. */
			LSFLOAT								fScaleY;			/**< V scale. */
			LSFLOAT								fTransX;			/**< U translate. */
			LSFLOAT								fTransY;			/**< V translate. */
			LSFLOAT								fRotX;				/**< U rotation. */
			LSFLOAT								fRotY;				/**< V rotation. */
			LSFLOAT								fRotZ;				/**< W rotation. */
			LSUINT32							ui32WrapU;			/**< U wrap mode */
			LSUINT32							ui32WrapV;			/**< V wrap mode */


			// == Various constructors.
			LSE_CALLCTOR						LSX_TEXTURE_TRANSFORM() :
				fScaleX( 1.0f ),
				fScaleY( 1.0f ),
				fTransX( 0.0f ),
				fTransY( 0.0f ),
				fRotX( 0.0f ),
				fRotY( 0.0f ),
				fRotZ( 0.0f ),
				ui32WrapU( LSD_MA_WRAP_REPEAT ),
				ui32WrapV( LSD_MA_WRAP_REPEAT ) {
			}
		} * LPLSX_TEXTURE_TRANSFORM, * const LPCLSX_TEXTURE_TRANSFORM;


		// == Functions.
		/**
		 * Creates the texture given a path and some flags.
		 *
		 * \param _pcPath Path to the file to load.
		 * \param _ui16Flags Flags.
		 * \return Returns an error code indicating whether there was a memory failure, path-not-found, or
		 *	success.
		 */
		LSSTD_ERRORS LSE_CALL					CreateFromFile( const LSCHAR * _pcPath, LSUINT16 _ui16Flags );

		/**
		 * Creates the texture from another texture with the specified flags.
		 *
		 * \param _tTexture The source texture to copy.
		 * \param _ui16Flags The flags to override in the new copy.
		 * \return Returns an error code indicating whether there was a memory failure or success.
		 */
		LSSTD_ERRORS LSE_CALL					CreateFromTexture( const CTexture &_tTexture, LSUINT16 _ui16Flags );

		/**
		 * Gets the texture ID.
		 *
		 * \return Returns the texture ID.
		 */
		LSE_INLINE LSUINT32 LSE_CALL			Id() const;

		/**
		 * Gets the texture flags.
		 *
		 * \return Returns the texture flags.
		 */
		LSE_INLINE LSUINT16 LSE_CALL			Flags() const;

		/**
		 * Adds flags.
		 *
		 * \param _ui16Flags Flags to add to the texture.
		 */
		LSE_INLINE LSVOID LSE_FCALL				AddFlags( LSUINT16 _ui16Flags );

		/**
		 * Adds a reference to the RGB components.
		 */
		LSE_INLINE LSVOID LSE_CALL				RefRgb();

		/**
		 * Adds a reference to the alpha component.
		 */
		LSE_INLINE LSVOID LSE_CALL				RefA();

		/**
		 * Gets the path to the image.
		 *
		 * \return Returns a NULL-terminated string indicating the path of the loaded image.
		 */
		LSE_INLINE const LSCHAR * LSE_CALL		Path() const;

		/**
		 * Gets a constant reference to the image.
		 *
		 * \return Returns a constant reference to the loaded image.
		 */
		LSE_INLINE const CImage & LSE_CALL		Image() const;

		/**
		 * Gets a reference to the image.
		 *
		 * \return Returns a reference to the loaded image.
		 */
		LSE_INLINE CImage & LSE_CALL			Image();

		/**
		 * Save the texture to a binary buffer.
		 *
		 * \param _bBuffer The buffer where the data should be written.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							SaveTexture( lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Converts from FBX's FbxLayerElement::EType texture type to LSD's.
		 *
		 * \param _tType FBX's texture usage type.
		 * \return Returns the LSD texture usage type corresponding to FBX's.
		 */
		static LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL
												TextureType( FbxLayerElement::EType _tType );

		/**
		 * Converts from FBX's FbxTexture::EWrapMode type to LSD's.
		 *
		 * \param _wmWrap FBX's wrap type.
		 * \return Returns the wrap mode corresponding to FBX's.
		 */
		static LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL
												WrapMode( FbxTexture::EWrapMode _wmWrap );

	protected :
		// == Members.
		/** Image data. */
		CImage									m_iImage;

		/** Texture ID. */
		LSUINT32								m_ui32Id;

		/** Flags. */
		LSUINT16								m_ui16Flags;

		/** Path to the file. */
		const LSCHAR *							m_pcPath;

		/** Texture ID counter. */
		static CAtomic::ATOM					m_aTexId;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the texture ID.
	 *
	 * \return Returns the texture ID.
	 */
	LSE_INLINE LSUINT32 LSE_CALL CTexture::Id() const { return m_ui32Id; }

	/**
	 * Gets the texture flags.
	 *
	 * \return Returns the texture flags.
	 */
	LSE_INLINE LSUINT16 LSE_CALL CTexture::Flags() const { return m_ui16Flags; }

	/**
	 * Adds flags.
	 *
	 * \param _ui16Flags Flags to add to the texture.
	 */
	LSE_INLINE LSVOID LSE_FCALL CTexture::AddFlags( LSUINT16 _ui16Flags ) { m_ui16Flags |= _ui16Flags; }

	/**
	 * Adds a reference to the RGB components.
	 */
	LSE_INLINE LSVOID LSE_CALL CTexture::RefRgb() { m_ui16Flags |= LSX_TEX_RGB_ACCESS; }

	/**
	 * Adds a reference to the alpha component.
	 */
	LSE_INLINE LSVOID LSE_CALL CTexture::RefA() { m_ui16Flags |= LSX_TEX_ALPHA_ACCESS; }

	/**
	 * Gets the path to the image.
	 *
	 * \return Returns a NULL-terminated string indicating the path of the loaded image.
	 */
	LSE_INLINE const LSCHAR * LSE_CALL CTexture::Path() const { return m_pcPath; }

	/**
	 * Gets a constant reference to the image.
	 *
	 * \return Returns a constant reference to the loaded image.
	 */
	LSE_INLINE const CImage & LSE_CALL CTexture::Image() const { return m_iImage; }

	/**
	 * Gets a reference to the image.
	 *
	 * \return Returns a reference to the loaded image.
	 */
	LSE_INLINE CImage & LSE_CALL CTexture::Image() { return m_iImage; }

	/**
	 * Converts from FBX's FbxLayerElement::EType texture type to LSD's.
	 *
	 * \param _tType FBX's texture usage type.
	 * \return Returns the LSD texture usage type corresponding to FBX's.
	 */
	LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL CTexture::TextureType( FbxLayerElement::EType _tType ) {
		switch ( _tType ) {
			case FbxLayerElement::eTextureDiffuseFactor : {
				return LSD_MA_TEXTURE_DIFFUSE_FACTOR;
			}
			case FbxLayerElement::eTextureEmissive : {
				return LSD_MA_TEXTURE_EMISSIVE;
			}
			case FbxLayerElement::eTextureEmissiveFactor : {
				return LSD_MA_TEXTURE_EMISSIVE_FACTOR;
			}
			case FbxLayerElement::eTextureSpecular : {
				return LSD_MA_TEXTURE_SPECULAR;
			}
			case FbxLayerElement::eTextureSpecularFactor : {
				return LSD_MA_TEXTURE_SPECULAR_FACTOR;
			}
			case FbxLayerElement::eTextureShininess : {
				return LSD_MA_TEXTURE_SHININESS;
			}
			case FbxLayerElement::eTextureNormalMap : {
				return LSD_MA_TEXTURE_NORMAL;
			}
			case FbxLayerElement::eTextureTransparency : {
				return LSD_MA_TEXTURE_TRANSPARENCY;
			}
			case FbxLayerElement::eTextureTransparencyFactor : {
				return LSD_MA_TEXTURE_TRANSPARENCY_FACTOR;
			}
			case FbxLayerElement::eTextureReflection : {
				return LSD_MA_TEXTURE_REFLECTION;
			}
			case FbxLayerElement::eTextureReflectionFactor : {
				return LSD_MA_TEXTURE_REFLECTION_FACTOR;
			}
			default : {
				return LSD_MA_TEXTURE_DIFFUSE;
			}
		}
	}

	/**
	 * Converts from FBX's FbxTexture::EWrapMode type to LSD's.
	 *
	 * \param _wmWrap FBX's wrap type.
	 * \return Returns the wrap mode corresponding to FBX's.
	 */
	LSE_INLINE LSD_MODEL_ATTRIBUTES LSE_CALL CTexture::WrapMode( FbxTexture::EWrapMode _wmWrap ) {
		switch ( _wmWrap ) {
			case FbxTexture::eRepeat : {
				return LSD_MA_WRAP_REPEAT;
			}
			case FbxTexture::eClamp : {
				return LSD_MA_WRAP_CLAMP;
			}
		}
		return LSD_MA_WRAP_MIRROR;
	}

}	// namespace lsx

#endif	// __LSX_TEXTURE_H__
