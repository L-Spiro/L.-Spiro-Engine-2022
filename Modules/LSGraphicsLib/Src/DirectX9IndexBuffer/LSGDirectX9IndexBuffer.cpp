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
 * Description: A Direct3D 9 index buffer.
 */

#include "LSGDirectX9IndexBuffer.h"

#ifdef LSG_DX9

#include "../Gfx/LSGGfx.h"
#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9IndexBuffer::CDirectX9IndexBuffer() :
		m_pibIndexBuffer( NULL ) {
	}
	LSE_CALLCTOR CDirectX9IndexBuffer::~CDirectX9IndexBuffer() {
	}

	// == Functions.
	/**
	 * Must perform some action when the device is lost.
	 */
	void LSE_CALL CDirectX9IndexBuffer::OnDeviceLost() {
		if ( !m_bResourceCanBeLost || !m_pibIndexBuffer ) { return; }
		// Release the existing index buffer.
		CDirectX9::SafeRelease( m_pibIndexBuffer );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	LSBOOL LSE_CALL CDirectX9IndexBuffer::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		return CreateApiIndexBuffer();
	}

	/**
	 * Create an index buffer for the graphics API.  This should use the data in this object to create
	 *	the buffer.
	 *
	 * \return Returns false if there are any errors during the creation of the API index buffer.  This
	 *	always indicates a lack of resources.
	 */
	LSBOOL LSE_CALL CDirectX9IndexBuffer::CreateApiIndexBuffer() {
		ResetApi();

		assert( m_ui32ElementSize == 2 || m_ui32ElementSize == 4 );


		// Determine the type of buffer.
		// Which pool should we use?
		uint32_t ui32Size = m_oobpBuffer.Length();
		D3DPOOL pPool = (m_ui32Usage & LSG_IBUT_SETONLY)/* && (ui32Size >= 8 * 1024)*/ ? D3DPOOL_DEFAULT : D3DPOOL_SYSTEMMEM;
		m_bResourceCanBeLost = pPool == D3DPOOL_DEFAULT;

		// What format?
		D3DFORMAT fFormat = (m_ui32ElementSize == sizeof( uint16_t )) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;

		static LSBOOL bUseHardware = 3;
		if ( bUseHardware == 3 ) {
			const D3DDEVICE_CREATION_PARAMETERS & dcpParms = CDirectX9::GetCreationParms();
			bUseHardware = (dcpParms.BehaviorFlags & (D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING)) != 0;
		}

		// What usage?
		uint32_t ui32Usage = m_ui32Usage;	// Will be D3DUSAGE_WRITEONLY, 0, or D3DUSAGE_DYNAMIC.
		ui32Usage |= !bUseHardware ? D3DUSAGE_SOFTWAREPROCESSING : 0;

		// Attempt to create.
		if ( FAILED( CDirectX9::GetDirectX9Device()->CreateIndexBuffer( ui32Size,
			ui32Usage,
			fFormat,
			pPool,
			&m_pibIndexBuffer,
			NULL ) ) ) { return false; }

		if ( m_oobpBuffer.Length() ) {
			// Copy the indices into it.
			void * pvData;
			if ( FAILED( m_pibIndexBuffer->Lock( 0, 0, &pvData, (ui32Usage & D3DUSAGE_DYNAMIC) ? D3DLOCK_DISCARD : 0 ) ) ) {
				return false;
			}

			CStd::MemCpy( pvData, m_oobpBuffer.Buffer(), ui32Size );
			if ( FAILED( m_pibIndexBuffer->Unlock() ) ) {
				return false;
			}
		}

		if ( !m_bResourceCanBeLost && m_ui32Usage == LSG_IBUT_SETONLY && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}
		return true;
	}

	/**
	 * Destroy the index buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CDirectX9IndexBuffer::ResetApi() {
		CDirectX9::SafeRelease( m_pibIndexBuffer );
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CDirectX9IndexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		const CVertexBuffer & vbBase = (*CGfx::VertexBuffers()[0].pvbVertBuffer);	// Must be set.
		HRESULT hRes = CDirectX9::GetDirectX9Device()->SetIndices( m_pibIndexBuffer );
		hRes = CDirectX9::GetDirectX9Device()->DrawIndexedPrimitive( static_cast<D3DPRIMITIVETYPE>(m_ui32Topology),
			_ui32StartVertex,
			0,
			vbBase.Elements(),
			0,
			_ui32TotalPrimitives );
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
