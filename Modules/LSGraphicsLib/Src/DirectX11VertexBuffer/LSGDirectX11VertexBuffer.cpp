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

#include "LSGDirectX11VertexBuffer.h"

#ifdef LSG_DX11

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11VertexBuffer::LSG_DIRECTX11_LAST_VERT_DECLS::LSG_DIRECTX11_LAST_VERT_DECLS() :
		pilInputLayout( NULL ) {
		CStd::MemSet( ui32StreamIds, 0, sizeof( ui32StreamIds ) );
	}
	LSE_CALLCTOR CDirectX11VertexBuffer::LSG_DIRECTX11_LAST_VERT_DECLS::~LSG_DIRECTX11_LAST_VERT_DECLS() {
		if ( pilInputLayout ) {
			pilInputLayout->Release();
			pilInputLayout = NULL;
		}
	}

	// == Various constructors.
	LSE_CALLCTOR CDirectX11VertexBuffer::CDirectX11VertexBuffer() :
		m_pbApiVertexBuffer( NULL ) {
	}
	LSE_CALLCTOR CDirectX11VertexBuffer::~CDirectX11VertexBuffer() {
		ResetApi();
	}

	// == Functions.
	/**
	 * Maps the vertex buffer for updating.  The returned pointer can be used for directly writing to or reading from
	 *	the vertex buffer.
	 *
	 * \param _vbmaAccessType The type of access needed for the mapping.
	 * \return Returns a pointer to which the new vertex-buffer data can be directly written or read.
	 */
	LSG_VERTEX_BUFFER_MAP LSE_CALL CDirectX11VertexBuffer::Map( LSG_VERTEX_BUFFER_MAP_ACCESS _vbmaAccessType ) {
		D3D11_MAPPED_SUBRESOURCE msRet = { NULL };
		if ( !m_pbApiVertexBuffer || FAILED( CDirectX11::GetDirectX11Context()->Map( m_pbApiVertexBuffer, 0, static_cast<D3D11_MAP>(_vbmaAccessType), 0, &msRet ) ) ) {
			LSG_VERTEX_BUFFER_MAP vbmRet = {
				NULL
			};
			return vbmRet;
		}
		LSG_VERTEX_BUFFER_MAP vbmRet = {
			msRet.pData,
			Stride() * Elements()
		};
		return vbmRet;
	}

	/**
	 * Unmaps the vertex buffer after an update.
	 *
	 * \param _vbmMappedData The returned mapped data from a previous call to Map().
	 */
	void LSE_CALL CDirectX11VertexBuffer::Unmap( LSG_VERTEX_BUFFER_MAP &_vbmMappedData ) {
		if ( m_pbApiVertexBuffer ) {
			CDirectX11::GetDirectX11Context()->Unmap( m_pbApiVertexBuffer, 0 );
		}
		_vbmMappedData.pvData = NULL;
		_vbmMappedData.ui32Size = 0;
	}

	/**
	 * Creates a vertex buffer compliant with the current graphics API.
	 *
	 * \return Returns false if the creation of the buffer failed.
	 */
	LSBOOL LSE_CALL CDirectX11VertexBuffer::CreateApiVertexBuffer() {
		ResetApi();

		uint32_t ui32Size = m_ui32Stride * (m_ui32Elements > 1 ? m_ui32Elements : 1);
		D3D11_BUFFER_DESC bdDesc = { 0 };
		bdDesc.ByteWidth = ui32Size;
		bdDesc.Usage = static_cast<D3D11_USAGE>(m_ui32Usage);
		bdDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bdDesc.CPUAccessFlags = (bdDesc.Usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;

		GenerateApiVertexDeclaration();

		const D3D11_SUBRESOURCE_DATA sdSubData = { m_oobpBuffer.Buffer() };

		if ( FAILED( CDirectX11::GetDirectX11Device()->CreateBuffer( &bdDesc,
			sdSubData.pSysMem ? &sdSubData : NULL,
			&m_pbApiVertexBuffer ) ) ) {
			ResetApi();
			return false;
		}

		if ( m_ui32Usage == LSG_VBUT_SETONLY && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}
		return true;
	}

	/**
	 * Destroy the vertex buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CDirectX11VertexBuffer::ResetApi() {
		CDirectX11::SafeRelease( m_pbApiVertexBuffer );
	}

	/**
	 * Given contents and a stream, generate a vertex declaration.
	 */
	void LSE_CALL CDirectX11VertexBuffer::GenerateApiVertexDeclaration() {
		struct LSG_VERT_DECL {
			LSG_VERTEX_ELEMENT				veType;
			uint32_t						ui32Size;
		} static const vdDecl[] = {
			// -- 32-bit Elements. -- //
			// * 1 component. * //
			{ LSG_VE_R32_F,					sizeof( LSGREAL ) * 1 },
			{ LSG_VE_R32_UI,				sizeof( uint32_t ) * 1 },
			{ LSG_VE_R32_SI,				sizeof( int32_t ) * 1 },
			
			// * 2 components. * //
			{ LSG_VE_R32G32_F,				sizeof( LSGREAL ) * 2 },
			{ LSG_VE_R32G32_UI,				sizeof( uint32_t ) * 2 },
			{ LSG_VE_R32G32_SI,				sizeof( int32_t ) * 2 },

			// * 3 components. * //
			{ LSG_VE_R32G32B32_F,			sizeof( LSGREAL ) * 3 },
			{ LSG_VE_R32G32B32_UI,			sizeof( uint32_t ) * 3 },
			{ LSG_VE_R32G32B32_SI,			sizeof( int32_t ) * 3 },

			// * 4 components. * //
			{ LSG_VE_R32G32B32A32_F,		sizeof( LSGREAL ) * 4 },
			{ LSG_VE_R32G32B32A32_UI,		sizeof( uint32_t ) * 4 },
			{ LSG_VE_R32G32B32A32_SI,		sizeof( int32_t ) * 4 },

			// -- 16-bit Elements. -- //
			// * 1 component. * //
			{ LSG_VE_R16_F,					sizeof( CFloat16 ) * 1 },
			{ LSG_VE_R16_UI,				sizeof( uint16_t ) * 1 },
			{ LSG_VE_R16_SI,				sizeof( int16_t ) * 1 },
			{ LSG_VE_R16_UI_N,				sizeof( uint16_t ) * 1 },
			{ LSG_VE_R16_SI_N,				sizeof( int16_t ) * 1 },

			// * 2 components. * //
			{ LSG_VE_R16G16_F,				sizeof( CFloat16 ) * 2 },
			{ LSG_VE_R16G16_UI,				sizeof( uint16_t ) * 2 },
			{ LSG_VE_R16G16_SI,				sizeof( int16_t ) * 2 },
			{ LSG_VE_R16G16_UI_N,			sizeof( uint16_t ) * 2 },
			{ LSG_VE_R16G16_SI_N,			sizeof( int16_t ) * 2 },

			// * 4 components. * //
			{ LSG_VE_R16G16B16A16_F,		sizeof( CFloat16 ) * 4 },
			{ LSG_VE_R16G16B16A16_UI,		sizeof( uint16_t ) * 4 },
			{ LSG_VE_R16G16B16A16_SI,		sizeof( int16_t ) * 4 },
			{ LSG_VE_R16G16B16A16_UI_N,		sizeof( uint16_t ) * 4 },
			{ LSG_VE_R16G16B16A16_SI_N,		sizeof( int16_t ) * 4 },

			// -- 10-bit Elements. -- //
			// * 3 components. * //
			{ LSG_VE_R10G10B10A2_UI,		sizeof( uint32_t ) * 1 },
			{ LSG_VE_R10G10B10A2_UI_N,		sizeof( uint32_t ) * 1 },

			// -- 8-bit Elements. -- //
			// * 4 components. * //
			{ LSG_VE_R8G8B8A8_UI,			sizeof( uint32_t ) * 1 },
			{ LSG_VE_R8G8B8A8_UI_N,			sizeof( uint32_t ) * 1 },
		};

		// Semantic names.
		static LPCSTR lpcsSemanticNames[] = {
			"BINORMAL",						// LSG_S_BINORMAL.
			"BLENDINDICES",					// LSG_S_BLENDINDICES.
			"BLENDWEIGHT",					// LSG_S_BLENDWEIGHT.
			"COLOR",						// LSG_S_COLOR.
			"NORMAL",						// LSG_S_NORMAL.
			"SV_POSITION",					// LSG_S_POSITION.
			"POSITIONT",					// LSG_S_POSITIONT.
			"PSIZE",						// LSG_S_PSIZE.
			"TANGENT",						// LSG_S_TANGENT.
			"TEXCOORD",						// LSG_S_TEXCOORD.
		};


		// Create the Direct3D vertex declaration from our own.
		uint32_t ui32Pos = 0;
		uint32_t ui32Index = 0;
		m_ui16ElementSize = 0;
		for ( uint32_t J = 0; J < m_ui16TotalDecl; ++J ) {
			for ( uint32_t I = 0; I < LSE_ELEMENTS( vdDecl ); ++I ) {
				if ( m_veElements[J].ui32Format == static_cast<uint32_t>(vdDecl[I].veType) ) {
					if ( m_veElements[J].ui32Offset ) {
						ui32Pos = m_veElements[J].ui32Offset;
					}
					assert( m_veElements[J].ui32Semantic < LSE_ELEMENTS( lpcsSemanticNames ) );
					m_iedInputDesc[ui32Index].SemanticName = lpcsSemanticNames[m_veElements[J].ui32Semantic];
					m_iedInputDesc[ui32Index].SemanticIndex = m_veElements[J].ui32SemanticIndex;
					m_iedInputDesc[ui32Index].Format = static_cast<DXGI_FORMAT>(m_veElements[J].ui32Format);
					m_iedInputDesc[ui32Index].InputSlot = 0;
					m_iedInputDesc[ui32Index].AlignedByteOffset = ui32Pos;
					m_iedInputDesc[ui32Index].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					m_iedInputDesc[ui32Index].InstanceDataStepRate = 0;
					++ui32Index;
					ui32Pos += vdDecl[I].ui32Size;
					m_ui16ElementSize = CStd::Max<uint16_t>( m_ui16ElementSize, static_cast<uint16_t>(ui32Pos) );
					break;
				}
			}
		}

		assert( ui32Index == m_ui16TotalDecl );
	}

	/**
	 * Sets the vertex buffer on a given stream.
	 *
	 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
	 * \param _ui32Offset Offset in elements of the stream source.
	 * \return Returns true if the vertex buffer was applied to the given stream index.
	 */
	LSBOOL LSE_CALL CDirectX11VertexBuffer::SetStreamApi( uint32_t /*_ui32Index*/, uint32_t /*_ui32Offset*/ ) const {
		return true;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CDirectX11VertexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		// This is only called on the 0th vertex buffer active.  This is where we set buffer vertex declarations etc.
		// First check to see if this vertex buffer has been the base of this combination of vertex buffers before.
		if ( m_vVertDeclRecord.Length() < CGfx::TotalActiveVertexBuffers() ) {
			m_vVertDeclRecord.Resize( CGfx::TotalActiveVertexBuffers() );
		}



		LSG_DIRECTX11_LAST_VERT_DECLS * pdlvdLastDecl = &m_vVertDeclRecord[CGfx::TotalActiveVertexBuffers()-1];
		for ( uint32_t I = CGfx::TotalActiveVertexBuffers(); I--; ) {
			uint32_t ui32Id = CGfx::VertexBuffers()[I].pvbVertBuffer ? reinterpret_cast<const CDirectX11VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->GetId() : 0;

			if ( pdlvdLastDecl->ui32StreamIds[I] != ui32Id ) {
				// Not the same combination as last time.  Have to rebuild the vertex declaration.
				static D3D11_INPUT_ELEMENT_DESC iedElements[64];	// Find out what value should be here instead of 64.
				uint32_t ui32Index = 0;
				for ( I = 0; I < CGfx::TotalActiveVertexBuffers(); ++I ) {
					if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
						const CDirectX11VertexBuffer * pdx11vbThisBuffer = reinterpret_cast<const CDirectX11VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer);
						for ( uint32_t J = pdx11vbThisBuffer->m_ui16TotalDecl; J--; ) {
							iedElements[ui32Index] = pdx11vbThisBuffer->m_iedInputDesc[J];
							iedElements[ui32Index++].InputSlot = static_cast<UINT>(I);
						}
					}
					pdlvdLastDecl->ui32StreamIds[I] = ui32Id;
				}

				CDirectX11::SafeRelease( pdlvdLastDecl->pilInputLayout );
				if ( FAILED( CDirectX11::GetDirectX11Device()->CreateInputLayout( iedElements, ui32Index, CDirectX11::m_pvShaderSignature,
					CDirectX11::m_stShaderSize, &pdlvdLastDecl->pilInputLayout ) ) ) {
					// Should never happen.
					return;
				}

				break;
			}
		}
		if ( !pdlvdLastDecl->pilInputLayout ) { return; }
		CDirectX11::SetInputLayout( pdlvdLastDecl->pilInputLayout );

		static ID3D11Buffer * pbBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { NULL };
		static UINT uiStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		static UINT uiOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { 0 };
		/*uint32_t ui32Index = 0;
		uint32_t ui32Total = 0;*/
		for ( uint32_t I = CGfx::TotalActiveVertexBuffers(); I--; ) {
			if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
				const CDirectX11VertexBuffer * pdx11vbThisBuffer = reinterpret_cast<const CDirectX11VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer);
				pbBuffers[I] = pdx11vbThisBuffer->m_pbApiVertexBuffer;
				uiStrides[I] = pdx11vbThisBuffer->Stride();
			}
			else {
				pbBuffers[I] = NULL;
				uiStrides[I] = 0;
			}
		}
		CDirectX11::GetDirectX11Context()->IASetVertexBuffers( 0, CGfx::TotalActiveVertexBuffers(),
			pbBuffers, uiStrides, uiOffsets );
		if ( _ui32TotalPrimitives ) {
			CDirectX11::GetDirectX11Context()->IASetPrimitiveTopology( static_cast<D3D11_PRIMITIVE_TOPOLOGY>(Topology()) );
			CDirectX11::GetDirectX11Context()->Draw( VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives ), _ui32StartVertex );
		}
	}

	/**
	 * Prepare for rendering.
	 */
	void LSE_CALL CDirectX11VertexBuffer::PrepareToRenderApi() const {
	}

	/**
	 * Finish rendering.
	 */
	void LSE_CALL CDirectX11VertexBuffer::EndRenderApi() const {
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
