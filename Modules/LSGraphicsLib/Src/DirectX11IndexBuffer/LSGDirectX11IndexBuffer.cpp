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

#include "LSGDirectX11IndexBuffer.h"

#ifdef LSG_DX11

#include "../Gfx/LSGGfx.h"
#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11IndexBuffer::CDirectX11IndexBuffer() :
		m_pbDirectX11IndexBuffer( NULL ),
		m_fFormat( DXGI_FORMAT_UNKNOWN ) {
	}
	LSE_CALLCTOR CDirectX11IndexBuffer::~CDirectX11IndexBuffer() {
		ResetApi();
	}

	// == Functions.
	/**
	 * Create an index buffer for the graphics API.  This should use the data in this object to create
	 *	the buffer.
	 *
	 * \return Returns false if there are any errors during the creation of the API index buffer.  This
	 *	always indicates a lack of resources.
	 */
	LSBOOL LSE_CALL CDirectX11IndexBuffer::CreateApiIndexBuffer() {
		ResetApi();

		assert( m_ui32ElementSize == 2 || m_ui32ElementSize == 4 );

		D3D11_BUFFER_DESC bdDesc = { 0 };
		bdDesc.Usage = CStd::ImpCast( bdDesc.Usage, m_ui32Usage );
		bdDesc.ByteWidth = m_oobpBuffer.Length();
		bdDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bdDesc.CPUAccessFlags = 0;
		bdDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA sdInitData;
		sdInitData.pSysMem = m_oobpBuffer.Buffer();
		sdInitData.SysMemPitch = 0;
		sdInitData.SysMemSlicePitch = 0;
		if ( FAILED( CDirectX11::GetDirectX11Device()->CreateBuffer( &bdDesc, &sdInitData, &m_pbDirectX11IndexBuffer ) ) ) { ResetApi(); return false; }

		m_fFormat = m_ui32ElementSize == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		return true;
	}

	/**
	 * Destroy the index buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CDirectX11IndexBuffer::ResetApi() {
		CDirectX11::SafeRelease( m_pbDirectX11IndexBuffer );
		m_fFormat = DXGI_FORMAT_UNKNOWN;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CDirectX11IndexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		CDirectX11::GetDirectX11Context()->IASetPrimitiveTopology( static_cast<D3D11_PRIMITIVE_TOPOLOGY>(Topology()) );
		CDirectX11::GetDirectX11Context()->IASetIndexBuffer( m_pbDirectX11IndexBuffer, m_fFormat, 0 );
		CDirectX11::GetDirectX11Context()->DrawIndexed( CVertexBuffer::VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives ),
			0,
			_ui32StartVertex );
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
