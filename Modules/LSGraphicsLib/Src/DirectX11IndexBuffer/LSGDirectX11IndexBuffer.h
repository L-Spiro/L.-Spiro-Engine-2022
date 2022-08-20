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
 * Description: A Direct3D 11 index buffer.
 */


#ifndef __LSG_DIRECTX11INDEXBUFFER_H__
#define __LSG_DIRECTX11INDEXBUFFER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../DirectX11/LSGDirectX11.h"
#include "../IndexBuffer/LSGIndexBufferBase.h"

namespace lsg {

	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// CONSTANTS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	/** Usage types. */
	enum LSG_INDEX_BUFFER_USAGE_TYPES {
		LSG_IBUT_SETONLY						= D3D11_USAGE_IMMUTABLE,						/**< Index data is never read or written to. */
		LSG_IBUT_STANDARD						= D3D11_USAGE_DEFAULT,							/**< Index data is not read or written to often by the CPU. */
		LSG_IBUT_DYNAMIC						= D3D11_USAGE_DYNAMIC,							/**< Index data is updated frequently. */

		LSG_IBUT_FORCE_DWORD					= 0x7FFFFFFF
	};

	/**
	 * Class CDirectX11IndexBuffer
	 * \brief A Direct3D 11 index buffer.
	 *
	 * Description: A Direct3D 11 index buffer.
	 */
	class CDirectX11IndexBuffer : public CIndexBufferBase {
		friend									class CGfx;
	public :
		// == Functions.


	protected :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX11IndexBuffer();
		LSE_CALLCTOR							~CDirectX11IndexBuffer();


		// == Members.
		/** Our index buffer used with Direct3D 11. */
		ID3D11Buffer *							m_pbDirectX11IndexBuffer;

		/** The format of the index buffer. */
		DXGI_FORMAT								m_fFormat;


		// == Functions.
		/**
		 * Create an index buffer for the graphics API.  This should use the data in this object to create
		 *	the buffer.
		 *
		 * \return Returns false if there are any errors during the creation of the API index buffer.  This
		 *	always indicates a lack of resources.
		 */
		LSBOOL LSE_CALL							CreateApiIndexBuffer();

		/**
		 * Destroy the index buffer that is compliant with the current graphics API and all related
		 *	resources.
		 */
		void LSE_CALL							ResetApi();

		/**
		 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
		 *	and have the same number of elements active.  If not, the system will throw an error.
		 *
		 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
		 * \param _ui32TotalPrimitives The total number of primitives to render.
		 */
		void LSE_CALL							RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11INDEXBUFFER_H__
