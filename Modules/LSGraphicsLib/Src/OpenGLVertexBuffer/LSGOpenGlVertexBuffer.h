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
 * Description: An OpenGL vertex buffer.
 */


#ifndef __LSG_OPENGLVERTEXBUFFER_H__
#define __LSG_OPENGLVERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_OGL

#include "../OpenGL/LSGOpenGl.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Primitive topology. */
	enum LSG_PRIMITIVE_TOPOLOGY {
		LSG_PT_UNDEFINED						= -1,											/**< Uninitialized topology. */
		LSG_PT_POINTLIST						= GL_POINTS,									/**< Interpret the vertex data as a list of points. */
		LSG_PT_LINELIST							= GL_LINES,										/**< Interpret the vertex data as a list of lines. */
		LSG_PT_LINESTRIP						= GL_LINE_STRIP,								/**< Interpret the vertex data as a line strip. */
		LSG_PT_TRIANGLELIST						= GL_TRIANGLES,									/**< Interpret the vertex data as a list of triangles. */
		LSG_PT_TRIANGLESTRIP					= GL_TRIANGLE_STRIP,							/**< Interpret the vertex data as a triangle strip. */
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
		LSG_VE_R10G10B10A2_UI,																	/**< A four-component, 32-bit unsigned-integer format that supports 10 bits for each color.  Alpha is ignored. */
		LSG_VE_R10G10B10A2_UI_N,																/**< A four-component, 32-bit unsigned-normalized-integer format that supports 10 bits for each color.  Alpha is ignored. */

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		LSG_VE_R8G8B8A8_UI,																		/**< A four-component, 32-bit unsigned-integer format that supports 8 bits per channel including alpha. */
		LSG_VE_R8G8B8A8_UI_N,																	/**< A four-component, 32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha. */
	};

	/** Usage types. */
	enum LSG_VERTEX_BUFFER_USAGE_TYPES {
		LSG_VBUT_SETONLY						= GL_STATIC_DRAW,								/**< Vertex data is never read or written to. */
		LSG_VBUT_STANDARD						= GL_STREAM_DRAW,								/**< Vertex data is not read or written to often by the CPU. */
		LSG_VBUT_DYNAMIC						= GL_DYNAMIC_DRAW,								/**< Vertex data is updated frequently. */

		LSG_VBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/** Mapping types. */
	enum LSG_VERTEX_BUFFER_MAP_ACCESS {
		LSG_VBMA_READ							= GL_READ_ONLY,									/**< Read-only access. */
		LSG_VBMA_WRITE							= GL_WRITE_ONLY,								/**< Write-only access. */
		LSG_VBMA_READ_WRITE						= GL_READ_WRITE,								/**< Read and write access. */
		LSG_VBMA_WRITE_DISCARD					= GL_WRITE_ONLY | (1 << 16),					/**< Write access, discarding previous contents. */

		LSG_VBMA_FORCE_DWORD					= 0x7FFFFFFF
	};

	/**
	 * Class COpenGlVertexBuffer
	 * \brief An OpenGL vertex buffer.
	 *
	 * Description: An OpenGL vertex buffer.
	 */
	class COpenGlVertexBuffer : public CVertexBufferBase {
		friend									class COpenGlIndexBuffer;
		friend									class CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							COpenGlVertexBuffer();
		LSE_CALLCTOR							~COpenGlVertexBuffer();


		// == Functions.
#include "../VertexBuffer/LSGVertexBufferCommonDecl.h"


	protected :
		// == Types.
		/**
		 * Vertex attribute properties.
		 */
		typedef struct LSG_OPENGL_PREFINED_VERT_ATT {
			/** The bit mask for the attribute. */
			uint32_t							ui32Mask;

			/** Size of the attribute. */
			uint32_t							ui32Size;

			/** Number of elements in the attribute. */
			GLint								iElements;

			/** Element type. */
			GLenum								eType;

			/** Normalized? */
			GLboolean							bNormalized;
		} * LPLSG_OPENGL_PREFINED_VERT_ATT, * const LPCLSG_OPENGL_PREFINED_VERT_ATT;

		/**
		 * Preparing for activation.  Stores the data needed for calls to glVertexAttribPointer().
		 */
		typedef struct LSG_VERTEX_ATTRIB_POINTER {
			/** Offset/pointer. */
			void *							pvPointer;

			/** Element type. */
			uint32_t							ui32Type;

			/** The location class.  One of the LSG_S_* enumerated values. */
			uint16_t							ui16LocClass;

			/** The location usage index.  From 0 to (LSG_MAX_VERTEX_SEMANTIC - 1). */
			uint16_t							ui16LocUsage;

			/** Size of the vertex element.  The stride. */
			uint16_t							ui16Stride;

			/** Elements.  1, 2, 3, or 4. */
			uint8_t								ui8Elements;

			/** Normalized? */
			uint8_t								ui8Normalized;
		} * LPLSG_VERTEX_ATTRIB_POINTER, * const LPCLSG_VERTEX_ATTRIB_POINTER;


		// == Members.
		/** The vertex buffer object ID as used by OpenGL. */
		GLuint									m_uiVboId;

		/** The element description array. */
		LSG_VERTEX_ELEMENT_DESC					m_veElements[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** Array of vertex attributes for fast activation of the vertex buffer. */
		LSG_VERTEX_ATTRIB_POINTER				m_vapVertexAttribPointer[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** Stream offset in the last call to SetStreamApi(). */
		mutable uint32_t						m_ui32StreamOffset;

		/** Table of properties of vertex types. */
		static LSG_OPENGL_PREFINED_VERT_ATT		m_opvaVertProps[];

		/**< Current enabled vertex attributes. */
		static uint8_t							m_ui8CurVertxAttribEnabled[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/**< Last enabled vertex attributes. */
		static uint8_t							m_ui8LastVertxAttribEnabled[LSG_MAX_VERTEX_ELEMENT_COUNT];


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
		void LSE_CALL							ResetApi();

		/**
		 * Sets the vertex buffer on a given stream.
		 *
		 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
		 * \param _ui32Offset Offset in elements of the stream source.
		 * \return Returns true if the vertex buffer was applied to the given stream index.
		 */
		LSBOOL LSE_CALL							SetStreamApi( uint32_t _ui32Index, uint32_t _ui32Offset = 0 ) const;

		/**
		 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
		 *	and have the same number of elements active.  If not, the system will throw an error.
		 *
		 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
		 * \param _ui32TotalPrimitives The total number of primitives to render.
		 */
		void LSE_CALL							RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const;

		/**
		 * Prepare for rendering.
		 */
		void LSE_CALL							PrepareToRenderApi() const;

		/**
		 * Finish rendering.
		 */
		void LSE_CALL							EndRenderApi() const;

		/**
		 * Gets a pointer to a LSG_OPENGL_PREFINED_VERT_ATT structure from the m_opvaVertProps array by ID (a LSG_VERTEX_ELEMENT value).
		 *
		 * \param _veType The type whose information inside m_opvaVertProps is to be found.
		 * \return Returns a pointer to an item inside m_opvaVertProps or NULL if the item does not exist.
		 */
		static const LSG_OPENGL_PREFINED_VERT_ATT * LSE_CALL
												GetVertexElementAttr( LSG_VERTEX_ELEMENT _veType );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
#define LSG_VBUFCOMMONDEF_CLASS COpenGlVertexBuffer
#include "../VertexBuffer/LSGVertexBufferCommonImpl.h"

}	// namespace lsg

#endif	// #ifdef LSG_OGL

#endif	// __LSG_OPENGLVERTEXBUFFER_H__
