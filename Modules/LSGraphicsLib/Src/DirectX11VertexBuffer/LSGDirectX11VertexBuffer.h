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
 * Description: A Direct3D 11 vertex buffer.
 */


#ifndef __LSG_DIRECTX11VERTEXBUFFER_H__
#define __LSG_DIRECTX11VERTEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../DirectX11/LSGDirectX11.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Primitive topology. */
	enum LSG_PRIMITIVE_TOPOLOGY {
		LSG_PT_UNDEFINED						= D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED,			/**< Uninitialized topology. */
		LSG_PT_POINTLIST						= D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,			/**< Interpret the vertex data as a list of points. */
		LSG_PT_LINELIST							= D3D11_PRIMITIVE_TOPOLOGY_LINELIST,			/**< Interpret the vertex data as a list of lines. */
		LSG_PT_LINESTRIP						= D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,			/**< Interpret the vertex data as a line strip. */
		LSG_PT_TRIANGLELIST						= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,		/**< Interpret the vertex data as a list of triangles. */
		LSG_PT_TRIANGLESTRIP					= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,		/**< Interpret the vertex data as a triangle strip. */
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
		LSG_VE_R32_F							= DXGI_FORMAT_R32_FLOAT,						/**< A single-component, 32-bit floating-point format that supports 32 bits for the red channel. */
		LSG_VE_R32_UI							= DXGI_FORMAT_R32_UINT,							/**< A single-component, 32-bit unsigned-integer format that supports 32 bits for the red channel. */
		LSG_VE_R32_SI							= DXGI_FORMAT_R32_SINT,							/**< A single-component, 32-bit signed-integer format that supports 32 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R32G32_F							= DXGI_FORMAT_R32G32_FLOAT,						/**< A two-component, 64-bit floating-point format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_UI						= DXGI_FORMAT_R32G32_UINT,						/**< A two-component, 64-bit unsigned-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */
		LSG_VE_R32G32_SI						= DXGI_FORMAT_R32G32_SINT,						/**< A two-component, 64-bit signed-integer format that supports 32 bits for the red channel and 32 bits for the green channel. */

		// * 3 components. * //
		LSG_VE_R32G32B32_F						= DXGI_FORMAT_R32G32B32_FLOAT,					/**< A three-component, 96-bit floating-point format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_UI						= DXGI_FORMAT_R32G32B32_UINT,					/**< A three-component, 96-bit unsigned-integer format that supports 32 bits per color channel. */
		LSG_VE_R32G32B32_SI						= DXGI_FORMAT_R32G32B32_SINT,					/**< A three-component, 96-bit signed-integer format that supports 32 bits per color channel. */

		// * 4 components. * //
		LSG_VE_R32G32B32A32_F					= DXGI_FORMAT_R32G32B32A32_FLOAT,				/**< A four-component, 128-bit floating-point format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_UI					= DXGI_FORMAT_R32G32B32A32_UINT,				/**< A four-component, 128-bit unsigned-integer format that supports 32 bits per channel including alpha. */
		LSG_VE_R32G32B32A32_SI					= DXGI_FORMAT_R32G32B32A32_SINT,				/**< A four-component, 128-bit signed-integer format that supports 32 bits per channel including alpha. */

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		LSG_VE_R16_F							= DXGI_FORMAT_R16_FLOAT,						/**< A single-component, 16-bit floating-point format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI							= DXGI_FORMAT_R16_UINT,							/**< A single-component, 16-bit unsigned-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI							= DXGI_FORMAT_R16_SINT,							/**< A single-component, 16-bit signed-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_UI_N							= DXGI_FORMAT_R16_UNORM,						/**< A single-component, 16-bit unsigned-normalized-integer format that supports 16 bits for the red channel. */
		LSG_VE_R16_SI_N							= DXGI_FORMAT_R16_SNORM,						/**< A single-component, 16-bit signed-normalized-integer format that supports 16 bits for the red channel. */

		// * 2 components. * //
		LSG_VE_R16G16_F							= DXGI_FORMAT_R16G16_FLOAT,						/**< A two-component, 32-bit floating-point format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI						= DXGI_FORMAT_R16G16_UINT,						/**< A two-component, 32-bit unsigned-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_SI						= DXGI_FORMAT_R16G16_SINT,						/**< A two-component, 32-bit signed-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */
		LSG_VE_R16G16_UI_N						= DXGI_FORMAT_R16G16_UNORM,						/**< A two-component, 32-bit unsigned-normalized-integer format that supports 16 bits each for the green and red channels. */
		LSG_VE_R16G16_SI_N						= DXGI_FORMAT_R16G16_SNORM,						/**< A two-component, 32-bit signed-normalized-integer format that supports 16 bits for the red channel and 16 bits for the green channel. */

		// * 4 components. * //
		LSG_VE_R16G16B16A16_F					= DXGI_FORMAT_R16G16B16A16_FLOAT,				/**< A four-component, 64-bit floating-point format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI					= DXGI_FORMAT_R16G16B16A16_UINT,				/**< A four-component, 64-bit unsigned-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI					= DXGI_FORMAT_R16G16B16A16_SINT,				/**< A four-component, 64-bit signed-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_UI_N				= DXGI_FORMAT_R16G16B16A16_UNORM,				/**< A four-component, 64-bit unsigned-normalized-integer format that supports 16 bits per channel including alpha. */
		LSG_VE_R16G16B16A16_SI_N				= DXGI_FORMAT_R16G16B16A16_SNORM,				/**< A four-component, 64-bit signed-normalized-integer format that supports 16 bits per channel including alpha. */

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		LSG_VE_R10G10B10A2_UI					= DXGI_FORMAT_R10G10B10A2_UINT,					/**< A four-component, 32-bit unsigned-integer format that supports 10 bits for each color.  Alpha is ignored. */
		LSG_VE_R10G10B10A2_UI_N					= DXGI_FORMAT_R10G10B10A2_UNORM,				/**< A four-component, 32-bit unsigned-normalized-integer format that supports 10 bits for each color.  Alpha is ignored. */

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		LSG_VE_R8G8B8A8_UI						= DXGI_FORMAT_R8G8B8A8_UINT,					/**< A four-component, 32-bit unsigned-integer format that supports 8 bits per channel including alpha. */
		LSG_VE_R8G8B8A8_UI_N					= DXGI_FORMAT_R8G8B8A8_UNORM,					/**< A four-component, 32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha. */

	};

	/** Usage types. */
	enum LSG_VERTEX_BUFFER_USAGE_TYPES {
		LSG_VBUT_SETONLY						= D3D11_USAGE_IMMUTABLE,						/**< Vertex data is never read or written to. */
		LSG_VBUT_STANDARD						= D3D11_USAGE_DEFAULT,							/**< Vertex data is not read or written to often by the CPU. */
		LSG_VBUT_DYNAMIC						= D3D11_USAGE_DYNAMIC,							/**< Vertex data is updated frequently. */

		LSG_VBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/** Mapping types. */
	enum LSG_VERTEX_BUFFER_MAP_ACCESS {
		LSG_VBMA_READ							= D3D11_MAP_READ,								/**< Read-only access. */
		LSG_VBMA_WRITE							= D3D11_MAP_WRITE,								/**< Write-only access. */
		LSG_VBMA_READ_WRITE						= D3D11_MAP_READ_WRITE,							/**< Read and write access. */
		LSG_VBMA_WRITE_DISCARD					= D3D11_MAP_WRITE_DISCARD,						/**< Write access, discarding previous contents. */
	};


	/**
	 * Class CDirectX11VertexBuffer
	 * \brief A Direct3D 11 vertex buffer.
	 *
	 * Description: A Direct3D 11 vertex buffer.
	 */
	class CDirectX11VertexBuffer : public CVertexBufferBase {
		friend									class CGfx;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX11VertexBuffer();
		LSE_CALLCTOR							~CDirectX11VertexBuffer();


		// == Functions.
#include "../VertexBuffer/LSGVertexBufferCommonDecl.h"
		

	protected :
		// == Types.
		/** Last-set vertex declarations for a given stream combination. */
		typedef struct LSG_DIRECTX11_LAST_VERT_DECLS {
			/** The last stream combination. */
			uint32_t							ui32StreamIds[LSG_MAX_VERTEX_STREAMS];

			/** Input layout. */
			ID3D11InputLayout *					pilInputLayout;


			// == Various constructors.
			LSE_CALLCTOR						LSG_DIRECTX11_LAST_VERT_DECLS();
			LSE_CALLCTOR						~LSG_DIRECTX11_LAST_VERT_DECLS();
		} * LPLSG_DIRECTX11_LAST_VERT_DECLS, * const LPCLSG_DIRECTX11_LAST_VERT_DECLS;


		// == Members.
		/** A DirectX 11 vertex buffer. */
		ID3D11Buffer *							m_pbApiVertexBuffer;

		/** Vertex declaration. */
		D3D11_INPUT_ELEMENT_DESC				m_iedInputDesc[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** The element description array. */
		LSG_VERTEX_ELEMENT_DESC					m_veElements[LSG_MAX_VERTEX_ELEMENT_COUNT];

		/** An array of up to LSG_MAX_VERTEX_STREAMS entries to record the last-set vertex declarations for each slot combination. */
		mutable CVectorPoD<LSG_DIRECTX11_LAST_VERT_DECLS, uint8_t, 1>
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

		/**
		 * Update the entire vertex buffer.  The size of the vertex buffer will be the same.
		 *
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return Returns 
		 */
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
#define LSG_VBUFCOMMONDEF_CLASS CDirectX11VertexBuffer
#include "../VertexBuffer/LSGVertexBufferCommonImpl.h"

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11VERTEXBUFFER_H__
