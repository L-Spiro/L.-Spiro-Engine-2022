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
 * Description: A Metal vertex buffer.
 */


#ifndef __LSG_METALVERTEXBUFFER_H__
#define __LSG_METALVERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_METAL

#include "../Metal/LSGMetal.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Primitive topology. */
	enum LSG_PRIMITIVE_TOPOLOGY {
		LSG_PT_UNDEFINED						= -1,											/**< Uninitialized topology. */
		LSG_PT_POINTLIST						= 0 /*MTLPrimitiveTypePoint*/,					/**< Interpret the vertex data as a list of points. */
		LSG_PT_LINELIST							= 1 /*MTLPrimitiveTypeLine*/,					/**< Interpret the vertex data as a list of lines. */
		LSG_PT_LINESTRIP						= 2 /*MTLPrimitiveTypeLineStrip*/,				/**< Interpret the vertex data as a line strip. */
		LSG_PT_TRIANGLELIST						= 3 /*MTLPrimitiveTypeTriangle*/,				/**< Interpret the vertex data as a list of triangles. */
		LSG_PT_TRIANGLESTRIP					= 4 /*MTLPrimitiveTypeTriangleStrip*/,			/**< Interpret the vertex data as a triangle strip. */
	};

	/** Semantics. */
	enum LSG_SEMANTICS {
		LSG_S_BINORMAL,																			/**< Binormal. */
		LSG_S_BLENDINDICES,																		/**< Blend indices. */
		LSG_S_BLENDWEIGHT,																		/**< Blend weights. */
		LSG_S_COLOR,																			/**< Diffuse and specular color. */
		LSG_S_NORMAL,																			/**< Normal vector. */
		LSG_S_POSITION,																			/**< Vertex position in object space. */
		LSG_S_POSITIONT,																		/**< Transformed vertex position. */
		LSG_S_PSIZE,																			/**< Point size. */
		LSG_S_TANGENT,																			/**< Tangent. */
		LSG_S_TEXCOORD,																			/**< Texture coordinates. */

		LSG_S_TOTAL																				/**< Total semantics. */
	};

	/** Vertex element types. */
	enum LSG_VERTEX_ELEMENT {
		// -- 32-bit Elements. -- //
		// * 1 component. * //
		LSG_VE_R32_F,																			/**< A single-component, 32-bit floating-point format that supports 32 bits for the red channel. */
		LSG_VE_R32_UI,																			/**< A single-component, 32-bit unsigned-integer format that supports 32 bits for the red channel. */
		LSG_VE_R32_SI,																			/**< A single-component, 32-bit signed-integer format that supports 32 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R32G32_F,																		/**< A two-component, 64-bit floating-point format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_UI,																		/**< A two-component, 64-bit unsigned-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_SI,																		/**< A two-component, 64-bit signed-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */

		// * 3 components. * //
		LSG_VE_R32G32B32_F,																		/**< A three-component, 96-bit floating-point format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_UI,																	/**< A three-component, 96-bit unsigned-integer format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_SI,																	/**< A three-component, 96-bit signed-integer format that supports 32 bits per color channel. */

		// * 4 components. * //
		LSG_VE_R32G32B32A32_F,																	/**< A four-component, 128-bit floating-point format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_UI,																	/**< A four-component, 128-bit unsigned-integer format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_SI,																	/**< A four-component, 128-bit signed-integer format that supports 32 bits per channel including alpha. */

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		LSG_VE_R16_F,																			/**< A single-component, 16-bit floating-point format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI,																			/**< A single-component, 16-bit unsigned-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI,																			/**< A single-component, 16-bit signed-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI_N,																		/**< A single-component, 16-bit unsigned-normalized-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI_N,																		/**< A single-component, 16-bit signed-normalized-integer format that supports 16 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R16G16_F,																		/**< A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI,																		/**< A two-component, 32-bit unsigned-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_SI,																		/**< A two-component, 32-bit signed-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI_N,																		/**< A two-component, 32-bit unsigned-normalized-integer format that supports 16 bits each for the green and red channels. */
		LSG_VE_R16G16_SI_N,																		/**< A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */

		// * 4 components. * //
		LSG_VE_R16G16B16A16_F,																	/**< A four-component, 64-bit floating-point format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI,																	/**< A four-component, 64-bit unsigned-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI,																	/**< A four-component, 64-bit signed-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI_N,																/**< A four-component, 64-bit unsigned-normalized-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI_N,																/**< A four-component, 64-bit signed-normalized-integer format that supports 16 bits per channel including alpha. */

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		LSG_VE_R10G10B10_UI,																	/**< A four-component, 32-bit unsigned-integer format that supports 10 bits for each color.  Alpha is ignored. */
		LSG_VE_R10G10B10_UI_N,																	/**< A four-component, 32-bit unsigned-normalized-integer format that supports 10 bits for each color.  Alpha is ignored. */

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		LSG_VE_R8G8B8A8_UI,																		/**< A four-component, 32-bit unsigned-integer format that supports 8 bits per channel including alpha. */
		LSG_VE_R8G8B8A8_UI_N,																	/**< A four-component, 32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha. */
	};

	/** Usage types. */
	enum LSG_VERTEX_BUFFER_USAGE_TYPES {
		LSG_VBUT_SETONLY						= 1 << 0,										/**< Vertex data is never read or written to. */
		LSG_VBUT_STANDARD						= 1 << 1,										/**< Vertex data is not read or written to often by the CPU. */
		LSG_VBUT_DYNAMIC						= 1 << 2,										/**< Vertex data is updated frequently. */

		LSG_VBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/** Mapping types. */
	enum LSG_VERTEX_BUFFER_MAP_ACCESS {
		LSG_VBMA_READ							= 0,										/**< Read-only access. */
		LSG_VBMA_WRITE							= 1,										/**< Write-only access. */
		LSG_VBMA_READ_WRITE						= 2,										/**< Read and write access. */
		LSG_VBMA_WRITE_DISCARD					= 3,										/**< Write access, discarding previous contents. */
	};

	/**
	 * Class CMetalVertexBuffer
	 * \brief A Metal vertex buffer.
	 *
	 * Description: A Metal vertex buffer.
	 */
	class CMetalVertexBuffer : public CVertexBufferBase {
		friend									class CMetalIndexBuffer;
		friend									class CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMetalVertexBuffer();
		LSE_CALLCTOR							~CMetalVertexBuffer();


		// == Functions.
#include "../VertexBuffer/LSGVertexBufferCommonDecl.h"


	protected :
		// == Types.
		/**
		 * Vertex attribute properties.
		 */
		typedef struct LSG_METAL_PREFINED_VERT_ATT {
			/** The bit mask for the attribute. */
			LSUINT32							ui32Mask;

			/** Size of the attribute. */
			LSUINT32							ui32Size;

			/** Number of elements in the attribute. */
			LSINT32								iElements;

			/** Element type. */
			LSUINT32							eType;

			/** Normalized? */
			LSBOOL								bNormalized;
		} * LPLSG_METAL_PREFINED_VERT_ATT, * const LPCLSG_METAL_PREFINED_VERT_ATT;


		// == Members.
		/** The vertex buffer object ID as used by Metal. */
		const LSVOID *							m_pvBuffer;

		/** The element description array. */
		LSG_VERTEX_ELEMENT_DESC					m_veElements[LSG_MAX_VERTEX_ELEMENT_COUNT];
		
		/** The description array 1-to-1 to our internal LSG_METAL_PREFINED_VERT_ATT structure. */
		const LSG_METAL_PREFINED_VERT_ATT *		m_pmpvaMetalElements[LSG_MAX_VERTEX_ELEMENT_COUNT];
		
		/** Stream index in the last call to SetStreamApi(). */
		mutable LSUINT32						m_ui32StreamIndex;

		/** Stream offset in the last call to SetStreamApi(). */
		mutable LSUINT32						m_ui32StreamOffset;
		
		/** Last vertex-buffer configuration. */
		mutable const CMetalVertexBuffer *		m_pmvbDeclRecord[LSG_MAX_VERTEX_ELEMENT_COUNT];
		
		/** Last vertex declaration (MTLVertexDescriptor) */
		mutable const LSVOID *					m_pvVertexDescriptor;

		/** Table of properties of vertex types. */
		static LSG_METAL_PREFINED_VERT_ATT		m_mpvaVertProps[];


		// == Functions.
		/**
		 * Creates a vertex buffer compliant with the current graphics API.
		 *
		 * \return Returns false if the creation of the buffer failed.
		 */
		LSBOOL LSE_CALL							CreateApiVertexBuffer();

		/**
		 * Destroy the vertex buffer that is compliant with the current graphics API and all related
		 *	resources.
		 */
		LSVOID LSE_CALL							ResetApi();

		/**
		 * Sets the vertex buffer on a given stream.
		 *
		 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
		 * \param _ui32Offset Offset in elements of the stream source.
		 * \return Returns true if the vertex buffer was applied to the given stream index.
		 */
		LSBOOL LSE_CALL							SetStreamApi( LSUINT32 _ui32Index, LSUINT32 _ui32Offset = 0UL ) const;

		/**
		 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
		 *	and have the same number of elements active.  If not, the system will throw an error.
		 *
		 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
		 * \param _ui32TotalPrimitives The total number of primitives to render.
		 */
		LSVOID LSE_CALL							RenderApi( LSUINT32 _ui32StartVertex, LSUINT32 _ui32TotalPrimitives ) const;

		/**
		 * Prepare for rendering.
		 */
		LSVOID LSE_CALL							PrepareToRenderApi() const;

		/**
		 * Finish rendering.
		 */
		LSVOID LSE_CALL							EndRenderApi() const;
		
		/** 
		 * Creates the m_pvVertexDescriptor object based on the currently set vertex buffers.
		 */
		LSVOID LSE_CALL							CreateVertexDescriptor() const;

		/**
		 * Gets a pointer to a LSG_METAL_PREFINED_VERT_ATT structure from the m_opvaVertProps array by ID (a LSG_VERTEX_ELEMENT value).
		 *
		 * \param _veType The type whose information inside m_opvaVertProps is to be found.
		 * \return Returns a pointer to an item inside m_opvaVertProps or NULL if the item does not exist.
		 */
		static const LSG_METAL_PREFINED_VERT_ATT * LSE_CALL
												GetVertexElementAttr( LSG_VERTEX_ELEMENT _veType );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
#define LSG_VBUFCOMMONDEF_CLASS CMetalVertexBuffer
#include "../VertexBuffer/LSGVertexBufferCommonImpl.h"
	

}	// namespace lsg

#endif	// #ifdef LSG_METAL

#endif	// __LSG_METALVERTEXBUFFER_H__
