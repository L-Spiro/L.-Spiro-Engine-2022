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
 * Description: Common attributes among models, including file headers, etc.
 */


#ifndef __LSD_MODELATTRIBUTES_H__
#define __LSD_MODELATTRIBUTES_H__

#include "../LSDModelLib.h"
#include "SharedPtr/LSTLSharedPtr.h"

namespace lsd {

	// == Enumerations.
	/** Model attributes. */
	enum LSD_MODEL_ATTRIBUTES {
		LSD_MA_MODEL_HEADER								= 0x004C534D,				/**< Model file header. */
		LSD_MA_ANIM_HEADER								= 0x0041534C,				/**< Animation file header. */
		LSD_MA_MAX_SKINS								= 4,						/**< Maximum number of skin weights to load during run-time. */
		LSD_MA_MODEL_VERSION							= 0,						/**< Model file version. */

		// Vertex attributes.
		LSD_MA_NORMALS									= (1 << 0),					/**< Normal data is included. */
		LSD_MA_COLORS									= (1 << 1),					/**< Color data is included. */
		LSD_MA_SKIN										= (1 << 2),					/**< Skin data is included. */
		LSD_MA_TANGENT									= (1 << 3),					/**< Tangent data is included. */
		LSD_MA_BITANGENT								= (1 << 4),					/**< Bitangent data is included. */

		// Materials.
		LSD_MA_LAMBERT									= 0x1000,					/**< Lambert material type. */
		LSD_MA_PHONG									= 0x1100,					/**< Phong material type. */

		// Wrap modes.
		LSD_MA_WRAP_CLAMP								= 0xF000,					/**< Clamp wrap mode. */
		LSD_MA_WRAP_MIRROR								= 0xF001,					/**< Mirror wrap mode. */
		LSD_MA_WRAP_REPEAT								= 0xF002,					/**< Repeat wrap mode. */

		// Index buffers.
		LSD_MA_TRIANGLE_LIST							= 0x20,						/**< Triangle list. */
		LSD_MA_TRIANGLE_STRIP							= 0x21,						/**< Triangle strip. */

		// Texture usages.
		LSD_MA_TEXTURE_DIFFUSE							= 0x2000,					/**< Diffuse texture.  Can also contain an alpha channel. */
		LSD_MA_TEXTURE_DIFFUSE_FACTOR,												/**< Each texel modifies the intensity of the diffuse color on a model. */
		LSD_MA_TEXTURE_EMISSIVE,													/**< Emissive texture.  Alpha channel unused. */
		LSD_MA_TEXTURE_EMISSIVE_FACTOR,												/**< Each texel modifies the intensity of the emissive color on a model. */
		LSD_MA_TEXTURE_SPECULAR,													/**< Specular texture.  Alpha channel unused. */
		LSD_MA_TEXTURE_SPECULAR_FACTOR,												/**< Each texel modifies the intensity of the specular color on a model. */
		LSD_MA_TEXTURE_TRANSPARENCY,												/**< Transparency texture.  Alpha channel unused. */
		LSD_MA_TEXTURE_TRANSPARENCY_FACTOR,											/**< Each texel modifies the intensity of the transparency color on a model. */
		LSD_MA_TEXTURE_REFLECTION,													/**< Reflection texture.  Alpha channel unused. */
		LSD_MA_TEXTURE_REFLECTION_FACTOR,											/**< Each texel modifies the intensity of the reflection color on a model. */
		LSD_MA_TEXTURE_SHININESS,													/**< Specular-power texture (greyscale). */
		LSD_MA_TEXTURE_NORMAL,														/**< Normal map. */

		// Object types.
		LSD_MA_OBJECT_NODE								= 0x3000,					/**< A blank node with only a transform and parent/child information. */
		LSD_MA_OBJECT_JOINT,														/**< A joint in a skeleton. */
		LSD_MA_OBJECT_MESH,															/**< A mesh. */
		LSD_MA_OBJECT_CAMERA,														/**< A camera. */
		LSD_MA_OBJECT_STEREOCAMERA,													/**< A stereo camera. */
		LSD_MA_OBJECT_LIGHT,														/**< A light. */


		// Unused.
		LSD_MA_FORCE_DWORD								= 0x7FFFFFFF,
	};


	// == Types.
	typedef CSharedPtr<CVectorPoD<uint8_t, uint32_t> >	CSharedFilePtr;				/**< Shared model file. */

}	// namespace lsd

#endif	// __LSD_MODELATTRIBUTES_H__
