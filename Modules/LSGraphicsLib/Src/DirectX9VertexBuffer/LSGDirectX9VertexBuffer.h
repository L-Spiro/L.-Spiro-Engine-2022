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
 * Description: A Direct3D 9 vertex buffer.
 */


#ifndef __LSG_DIRECTX9VERTEXBUFFER_H__
#define __LSG_DIRECTX9VERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

#include "../DirectX9/LSGDirectX9.h"
#include "../DirectX9/LSGDirectX9LosableResource.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Primitive topology. */
	enum LSG_PRIMITIVE_TOPOLOGY {
		LSG_PT_UNDEFINED						= -1,											/**< Uninitialized topology. */
		LSG_PT_POINTLIST						= D3DPT_POINTLIST,								/**< Interpret the vertex data as a list of points. */
		LSG_PT_LINELIST							= D3DPT_LINELIST,								/**< Interpret the vertex data as a list of lines. */
		LSG_PT_LINESTRIP						= D3DPT_LINESTRIP,								/**< Interpret the vertex data as a line strip. */
		LSG_PT_TRIANGLELIST						= D3DPT_TRIANGLELIST,							/**< Interpret the vertex data as a list of triangles. */
		LSG_PT_TRIANGLESTRIP					= D3DPT_TRIANGLESTRIP,							/**< Interpret the vertex data as a triangle strip. */
	};

	/** Semantics. */
	enum LSG_SEMANTICS {
		LSG_S_BINORMAL							= D3DDECLUSAGE_BINORMAL,						/**< Binormal. */
		LSG_S_BLENDINDICES						= D3DDECLUSAGE_BLENDINDICES,					/**< Blend indices. */
		LSG_S_BLENDWEIGHT						= D3DDECLUSAGE_BLENDWEIGHT,						/**< Blend weights. */
		LSG_S_COLOR								= D3DDECLUSAGE_COLOR,							/**< Diffuse and specular color. */
		LSG_S_NORMAL							= D3DDECLUSAGE_NORMAL,							/**< Normal vector. */
		LSG_S_POSITION							= D3DDECLUSAGE_POSITION,						/**< Vertex position in object space. */
		LSG_S_POSITIONT							= D3DDECLUSAGE_POSITIONT,						/**< Transformed vertex position. */
		LSG_S_PSIZE								= D3DDECLUSAGE_PSIZE,							/**< Point size. */
		LSG_S_TANGENT							= D3DDECLUSAGE_TANGENT,							/**< Tangent. */
		LSG_S_TEXCOORD							= D3DDECLUSAGE_TEXCOORD,						/**< Texture coordinates. */
		
		LSG_S_TOTAL								= 10,											/**< Total semantics. */
	};

	/** Vertex element types. */
	enum LSG_VERTEX_ELEMENT {
		// -- 32-bit Elements. -- //
		// * 1 component. * //
		LSG_VE_R32_F							= D3DDECLTYPE_FLOAT1,							/**< A single-component, 32-bit floating-point format that supports 32 bits for the red channel. */
		LSG_VE_R32_UI							= D3DDECLTYPE_UNUSED + 1,						/**< A single-component, 32-bit unsigned-integer format that supports 32 bits for the red channel. */
		LSG_VE_R32_SI							= D3DDECLTYPE_UNUSED + 2,						/**< A single-component, 32-bit signed-integer format that supports 32 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R32G32_F							= D3DDECLTYPE_FLOAT2,							/**< A two-component, 64-bit floating-point format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_UI						= D3DDECLTYPE_UNUSED + 3,						/**< A two-component, 64-bit unsigned-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_SI						= D3DDECLTYPE_UNUSED + 4,						/**< A two-component, 64-bit signed-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */

		// * 3 components. * //
		LSG_VE_R32G32B32_F						= D3DDECLTYPE_FLOAT3,							/**< A three-component, 96-bit floating-point format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_UI						= D3DDECLTYPE_UNUSED + 5,						/**< A three-component, 96-bit unsigned-integer format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_SI						= D3DDECLTYPE_UNUSED + 6,						/**< A three-component, 96-bit signed-integer format that supports 32 bits per color channel. */

		// * 4 components. * //
		LSG_VE_R32G32B32A32_F					= D3DDECLTYPE_FLOAT4,							/**< A four-component, 128-bit floating-point format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_UI					= D3DDECLTYPE_UNUSED + 7,						/**< A four-component, 128-bit unsigned-integer format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_SI					= D3DDECLTYPE_UNUSED + 8,						/**< A four-component, 128-bit signed-integer format that supports 32 bits per channel including alpha. */

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		LSG_VE_R16_F							= D3DDECLTYPE_UNUSED + 9,						/**< A single-component, 16-bit floating-point format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI							= D3DDECLTYPE_UNUSED + 10,						/**< A single-component, 16-bit unsigned-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI							= D3DDECLTYPE_UNUSED + 11,						/**< A single-component, 16-bit signed-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI_N							= D3DDECLTYPE_UNUSED + 12,						/**< A single-component, 16-bit unsigned-normalized-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI_N							= D3DDECLTYPE_UNUSED + 13,						/**< A single-component, 16-bit signed-normalized-integer format that supports 16 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R16G16_F							= D3DDECLTYPE_FLOAT16_2,						/**< A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI						= D3DDECLTYPE_UNUSED + 14,						/**< A two-component, 32-bit unsigned-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_SI						= D3DDECLTYPE_SHORT2,							/**< A two-component, 32-bit signed-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI_N						= D3DDECLTYPE_USHORT2N,							/**< A two-component, 32-bit unsigned-normalized-integer format that supports 16 bits each for the green and red channels. */
		LSG_VE_R16G16_SI_N						= D3DDECLTYPE_SHORT2N,							/**< A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */

		// * 4 components. * //
		LSG_VE_R16G16B16A16_F					= D3DDECLTYPE_FLOAT16_4,						/**< A four-component, 64-bit floating-point format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI					= D3DDECLTYPE_UNUSED + 15,						/**< A four-component, 64-bit unsigned-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI					= D3DDECLTYPE_SHORT4,							/**< A four-component, 64-bit signed-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI_N				= D3DDECLTYPE_USHORT4N,							/**< A four-component, 64-bit unsigned-normalized-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI_N				= D3DDECLTYPE_SHORT4N,							/**< A four-component, 64-bit signed-normalized-integer format that supports 16 bits per channel including alpha. */

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		LSG_VE_R10G10B10A2_UI					= D3DDECLTYPE_UDEC3,							/**< A four-component, 32-bit unsigned-integer format that supports 10 bits for each color.  Alpha is ignored. */
		LSG_VE_R10G10B10A2_UI_N					= D3DDECLTYPE_UNUSED + 16,						/**< A four-component, 32-bit unsigned-normalized-integer format that supports 10 bits for each color.  Alpha is ignored. */

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		LSG_VE_R8G8B8A8_UI						= D3DDECLTYPE_UBYTE4,							/**< A four-component, 32-bit unsigned-integer format that supports 8 bits per channel including alpha. */
		LSG_VE_R8G8B8A8_UI_N					= D3DDECLTYPE_UBYTE4N,							/**< A four-component, 32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha. */

	};

	/** Usage types. */
	enum LSG_VERTEX_BUFFER_USAGE_TYPES {
		LSG_VBUT_SETONLY						= D3DUSAGE_WRITEONLY,							/**< Vertex data is never read or written to. */
		LSG_VBUT_STANDARD						= 0,											/**< Vertex data is not read or written to often by the CPU. */
		LSG_VBUT_DYNAMIC						= D3DUSAGE_DYNAMIC,								/**< Vertex data is updated frequently. */

		LSG_VBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/** Mapping types. */
	enum LSG_VERTEX_BUFFER_MAP_ACCESS {
		LSG_VBMA_READ							= D3DLOCK_READONLY,								/**< Read-only access. */
		LSG_VBMA_WRITE							= 0,											/**< Write-only access. */
		LSG_VBMA_READ_WRITE						= 0,											/**< Read and write access. */
		LSG_VBMA_WRITE_DISCARD					= D3DLOCK_DISCARD,								/**< Write access, discarding previous contents. */
	};

	/**
	 * Class CDirectX9VertexBuffer
	 * \brief A Direct3D 9 vertex buffer.
	 *
	 * Description: A Direct3D 9 vertex buffer.
	 */
	class CDirectX9VertexBuffer : public CVertexBufferBase, public CDirectX9LosableResource {
		friend									class CDirectX9IndexBuffer;
		friend									class CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX9VertexBuffer();
		LSE_CALLCTOR							~CDirectX9VertexBuffer();


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

#include "../VertexBuffer/LSGVertexBufferCommonDecl.h"


	protected :
		// == Types.
		/**
		 * Last-set vertex declarations for a given stream combination.
		 */
		typedef struct LSG_DIRECTX9_LAST_VERT_DECLS {
			/** The last stream combination. */
			uint32_t							ui32StreamIds[LSG_MAX_VERTEX_STREAMS];

			/** Vertex declaration. */
			IDirect3DVertexDeclaration9 *		pvdVertexDeclaration;


			// == Various constructors.
			LSE_CALLCTOR						LSG_DIRECTX9_LAST_VERT_DECLS();
			LSE_CALLCTOR						~LSG_DIRECTX9_LAST_VERT_DECLS();
		} * LPLSG_DIRECTX9_LAST_VERT_DECLS, * const LPCLSG_DIRECTX9_LAST_VERT_DECLS;


		// == Members.
		/** A DirectX 9 vertex buffer. */
		IDirect3DVertexBuffer9 *				m_pvbVertexBuffer;

		/** Vertex declaration. */
		D3DVERTEXELEMENT9						m_vdInputDesc[LSG_MAX_VERTEX_ELEMENT_COUNT+1];

		/** The element description array. */
		LSG_VERTEX_ELEMENT_DESC					m_veElements[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** An array of up to LSG_MAX_VERTEX_STREAMS entries to record the last-set vertex declarations for each slot combination. */
		mutable CVectorPoD<LSG_DIRECTX9_LAST_VERT_DECLS, uint8_t, 1>
												m_vVertDeclRecord;


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
		 * Given contents and a stream, generate a vertex declaration.
		 */
		void LSE_CALL							GenerateApiVertexDeclaration();

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
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
#define LSG_VBUFCOMMONDEF_CLASS CDirectX9VertexBuffer
#include "../VertexBuffer/LSGVertexBufferCommonImpl.h"

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9VERTEXBUFFER_H__
