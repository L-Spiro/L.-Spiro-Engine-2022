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
 * Description: A model-library shader key.
 */


#ifndef __LSD_SHADERKEY_H__
#define __LSD_SHADERKEY_H__

#include "../LSDModelLib.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/** Shader flags. */


namespace lsd {

	// == Enumerations.
	/** Vertex-buffer inputs. */
	enum LSD_VERTEX_INPUTS {
		LSD_VI_NORMALS									= (1 << 0),				/**< Contains normals. */
		LSD_VI_COLORS									= (1 << 1),				/**< Contains vertex colors. */
		LSD_VI_TANGENT									= (1 << 2),				/**< Contains tangents. */
		LSD_VI_BITANGENT								= (1 << 3),				/**< Contains bitangents. */
		LSD_VI_SKININDICES								= (1 << 4),				/**< Contains skin indices. */
		LSD_VI_SKINWEIGHTS								= (1 << 5),				/**< Contains skin weights. */
	};

	/** Texture slots. */
	enum LSD_TEXTURE_SLOTS {
		LSD_TS_DIFFUSE									= 0,					/**< Slot for the diffuse texture, if any. */
		LSD_TS_DIFFUSE_FACTOR							= 1,					/**< Slot for the diffuse-factor texture, if any. */

		LSD_TS_TRANSPARENCY								= 2,					/**< Slot for the transparency texture, if any. */
		LSD_TS_TRANSPARENCY_FACTOR						= 3,					/**< Slot for the transparency-factor texture, if any. */

		LSD_TS_EMISSIVE									= 4,					/**< Slot for the emissive texture, if any. */
		LSD_TS_EMISSIVE_FACTOR							= 5,					/**< Slot for the emissive-factor texture, if any. */

		LSD_TS_REFLECTION								= 6,					/**< Slot for the reflection texture, if any. */
		LSD_TS_REFLECTION_FACTOR						= 7,					/**< Slot for the reflection-factor texture, if any. */

		LSD_TS_SPECULAR									= 8,					/**< Slot for the specular texture, if any. */
		LSD_TS_SPECULAR_FACTOR							= 9,					/**< Slot for the specular-factor texture, if any. */

		LSD_TS_SHININESS								= 10,					/**< Slot for the shininess texture, if any. */

		LSD_TS_NORMAL_MAP								= 11,					/**< Slot for the normal-map texture, if any. */

		LSD_TS_TOTAL,															/**< Total slots. */
	};

	/** Shader flags. */
#define LSD_SF_TRANSLUCENCY								(1ULL << 0ULL)			/**< Is there translucency? */
#define LSD_SF_VERTEX_LIGHTING							(1ULL << 1ULL)			/**< Vertex lighting. */
#define LSD_SF_SPECULAR									(1ULL << 2ULL)			/**< Calculate and apply specular.  Not to be used with Lambert materials. */

	/** A model-library shader key. */
#pragma pack( push, 1 )
	typedef struct LSD_SHADERKEY {
		uint64_t										ui64Flags;				/**< First set of shader flags. */
		uint8_t											ui8Textures[16];		/**< Texture mapping. */
		uint8_t											ui8TextureUvs[16];		/**< Texture UV mapping. */
		uint8_t											ui8Inputs;				/**< Input flags. */
		uint8_t											ui8Uvs;					/**< Total texture UV's. */
		uint8_t											ui8DirLights;			/**< Total directional lights. */
		uint8_t											ui8PointLights;			/**< Total point lights. */


		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							LSD_SHADERKEY();
		

		// == Operators.
		/**
		 * Less-than operator.
		 *
		 * \param _skOther The shader key against which to compare.
		 * \return Returns true if this shader key compares less than the given shader key by
		 *	predefined sorting criteria.
		 */
		LSE_INLINE bool LSE_FCALL						operator < ( const LSD_SHADERKEY &_skOther ) const;

		/**
		 * Equality operator.
		 *
		 * \param _skOther The shader key against which to compare.
		 * \return Returns true if this shader key compares equal to the given shader key.
		 */
		LSE_INLINE bool LSE_FCALL						operator == ( const LSD_SHADERKEY &_skOther ) const;


		// == Functions.
		/**
		 * Performes a quick comparison between 2 shader keys.  Inputs are not checked.
		 *
		 * \param _skOther The key against which to test this key.
		 * \return Returns true if the keys match except for their inputs.
		 */
		LSE_INLINE bool LSE_FCALL						QuickCmp( const LSD_SHADERKEY &_skOther ) const;

		/**
		 * Performs a quick copy of the shader keys.
		 *
		 * \param _skOther The key to copy into this one.
		 * \return Returns this key after the copy.
		 */
		LSE_INLINE LSD_SHADERKEY & LSE_FCALL			QuickCpy( const LSD_SHADERKEY &_skOther );
	} * LPLSD_SHADERKEY, * const LPCLSD_SHADERKEY;
#pragma pack( pop )


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR LSD_SHADERKEY::LSD_SHADERKEY() :
		ui64Flags( 0ULL ),
		ui8Inputs( 0 ),
		ui8Uvs( 0 ),
		ui8DirLights( 0 ),
		ui8PointLights( 0 ) {
		assert( (sizeof( ui8Textures ) + sizeof( ui8TextureUvs )) % sizeof( uint64_t ) == 0 );
		register uint64_t * pui64This = reinterpret_cast<uint64_t *>(&ui8Textures[0]);
		for ( uint32_t I = (sizeof( ui8Textures ) + sizeof( ui8TextureUvs )) / sizeof( uint64_t ); I--; ) {
			(*pui64This++) = LSE_MAXU64;
		}
	}

	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _skOther The shader key against which to compare.
	 * \return Returns true if this shader key compares less than the given shader key by
	 *	predefined sorting criteria.
	 */
	LSE_INLINE bool LSE_FCALL LSD_SHADERKEY::operator < ( const LSD_SHADERKEY &_skOther ) const {
#if 1
		return CStd::MemCmp( this, &_skOther, sizeof( LSD_SHADERKEY ) ) < 0;
#else
		register const uint64_t * pui64This = reinterpret_cast<const uint64_t *>(this);
		register const uint64_t * pui64That = reinterpret_cast<const uint64_t *>(&_skOther);
		return pui64This[0] < pui64That[0] ||			// ui64Flags
			pui64This[1] < pui64That[1] ||				// ui8Textures[0] - ui8Textures[7]
			pui64This[2] < pui64That[2] ||				// ui8Textures[8] - ui8Textures[15]
			pui64This[3] < pui64That[3] ||				// ui8TextureUvs[0] - ui8TextureUvs[7]
			pui64This[4] < pui64That[4] ||				// ui8TextureUvs[8] - ui8TextureUvs[15]
			ui8Inputs < _skOther.ui8Inputs ||			// ui8Inputs
			ui8Uvs < _skOther.ui8Uvs ||					// ui8Uvs
			ui8DirLights < _skOther.ui8DirLights ||		// ui8DirLights
			ui8PointLights < _skOther.ui8PointLights;	// ui8PointLights
#endif	// #if 1
	}

	/**
	 * Equality operator.
	 *
	 * \param _skOther The shader key against which to compare.
	 * \return Returns true if this shader key compares equal to the given shader key.
	 */
	LSE_INLINE bool LSE_FCALL LSD_SHADERKEY::operator == ( const LSD_SHADERKEY &_skOther ) const {
#if 1
		return CStd::MemCmpF( this, &_skOther, sizeof( LSD_SHADERKEY ) ) == static_cast<LSBOOL>(true);
#else
		register const uint64_t * pui64This = reinterpret_cast<const uint64_t *>(this);
		register const uint64_t * pui64That = reinterpret_cast<const uint64_t *>(&_skOther);
		return pui64This[0] == pui64That[0] &&			// ui64Flags
			pui64This[1] == pui64That[1] &&				// ui8Textures[0] - ui8Textures[7]
			pui64This[2] == pui64That[2] &&				// ui8Textures[8] - ui8Textures[15]
			pui64This[3] == pui64That[3] &&				// ui8TextureUvs[0] - ui8TextureUvs[7]
			pui64This[4] == pui64That[4] &&				// ui8TextureUvs[8] - ui8TextureUvs[15]
			ui8Inputs == _skOther.ui8Inputs &&			// ui8Inputs
			ui8Uvs == _skOther.ui8Uvs &&				// ui8Uvs
			ui8DirLights == _skOther.ui8DirLights &&	// ui8DirLights
			ui8PointLights == _skOther.ui8PointLights;	// ui8PointLights
#endif	// #if 1
	}

	// == Functions.
	/**
	 * Performes a quick comparison between 2 shader keys.  Inputs are not checked.
	 *
	 * \param _skOther The key against which to test this key.
	 * \return Returns true if the keys match except for their inputs.
	 */
	LSE_INLINE bool LSE_FCALL LSD_SHADERKEY::QuickCmp( const LSD_SHADERKEY &_skOther ) const {
		register const uint64_t * pui64This = reinterpret_cast<const uint64_t *>(&ui8Textures[0]);
		register const uint64_t * pui64That = reinterpret_cast<const uint64_t *>(&_skOther.ui8Textures[0]);
		return ui64Flags == _skOther.ui64Flags &&		// ui64Flags
			pui64This[0] == pui64That[0] &&				// ui8Textures[0] - ui8Textures[7]
			pui64This[1] == pui64That[1] &&				// ui8Textures[8] - ui8Textures[15]
			pui64This[2] == pui64That[2] &&				// ui8TextureUvs[0] - ui8TextureUvs[7]
			pui64This[3] == pui64That[3] &&				// ui8TextureUvs[8] - ui8TextureUvs[15]
			ui8DirLights == _skOther.ui8DirLights &&	// ui8DirLights
			ui8PointLights == _skOther.ui8PointLights;	// ui8PointLights
	}

	/**
	 * Performs a quick copy of the shader keys.
	 *
	 * \param _skOther The key to copy into this one.
	 * \return Returns this key after the copy.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_FCALL LSD_SHADERKEY::QuickCpy( const LSD_SHADERKEY &_skOther ) {
		ui64Flags = _skOther.ui64Flags;
		register uint64_t * pui64This = reinterpret_cast<uint64_t *>(&ui8Textures[0]);
		register const uint64_t * pui64That = reinterpret_cast<const uint64_t *>(&_skOther.ui8Textures[0]);
		(*pui64This++) = (*pui64That++);				// ui8Textures[0] - ui8Textures[7]
		(*pui64This++) = (*pui64That++);				// ui8Textures[8] - ui8Textures[15]
		(*pui64This++) = (*pui64That++);				// ui8TextureUvs[0] - ui8TextureUvs[7]
		(*pui64This) = (*pui64That);					// ui8TextureUvs[8] - ui8TextureUvs[15]
		ui8DirLights = _skOther.ui8DirLights;
		ui8PointLights = _skOther.ui8PointLights;
		return (*this);
	}

}	// namespace lsd

#endif	// __LSD_SHADERKEY_H__
