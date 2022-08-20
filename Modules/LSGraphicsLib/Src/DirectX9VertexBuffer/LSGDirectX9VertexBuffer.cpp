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

#include "LSGDirectX9VertexBuffer.h"

#ifdef LSG_DX9

#include "../Gfx/LSGGfx.h"

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9VertexBuffer::LSG_DIRECTX9_LAST_VERT_DECLS::LSG_DIRECTX9_LAST_VERT_DECLS() :
		pvdVertexDeclaration( NULL ) {
		CStd::MemSet( ui32StreamIds, 0, sizeof( ui32StreamIds ) );
	}
	LSE_CALLCTOR CDirectX9VertexBuffer::LSG_DIRECTX9_LAST_VERT_DECLS::~LSG_DIRECTX9_LAST_VERT_DECLS() {
		if ( pvdVertexDeclaration ) {
			pvdVertexDeclaration->Release();
			pvdVertexDeclaration = NULL;
		}
	}

	// == Various constructors.
	LSE_CALLCTOR CDirectX9VertexBuffer::CDirectX9VertexBuffer() :
		m_pvbVertexBuffer( NULL ) {
		static const D3DVERTEXELEMENT9 veEnd = D3DDECL_END();
		for ( uint32_t I = LSE_ELEMENTS( m_vdInputDesc ); I--; ) {
			m_vdInputDesc[I] = veEnd;
		}
	}
	LSE_CALLCTOR CDirectX9VertexBuffer::~CDirectX9VertexBuffer() {
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
	LSG_VERTEX_BUFFER_MAP LSE_CALL CDirectX9VertexBuffer::Map( LSG_VERTEX_BUFFER_MAP_ACCESS _vbmaAccessType ) {
		uint32_t ui32Size = Stride() * Elements();
		void * pvData;
		if ( !m_pvbVertexBuffer || FAILED( m_pvbVertexBuffer->Lock( 0, ui32Size, &pvData, _vbmaAccessType ) ) ) {
			LSG_VERTEX_BUFFER_MAP vbmRet = {
				NULL
			};
			return vbmRet;
		}
		LSG_VERTEX_BUFFER_MAP vbmRet = {
			pvData,
			ui32Size
		};
		return vbmRet;
	}

	/**
	 * Unmaps the vertex buffer after an update.
	 *
	 * \param _vbmMappedData The returned mapped data from a previous call to Map().
	 */
	void LSE_CALL CDirectX9VertexBuffer::Unmap( LSG_VERTEX_BUFFER_MAP &_vbmMappedData ) {
		if ( m_pvbVertexBuffer ) {
			if ( FAILED( m_pvbVertexBuffer->Unlock() ) ) {
				CStd::DebugPrintA( "CDirectX9VertexBuffer::Unmap(): Failed to unmap vertex buffer.\r\n" );
			}
		}
		_vbmMappedData.pvData = NULL;
		_vbmMappedData.ui32Size = 0;
	}

	/**
	 * Must perform some action when the device is lost.
	 */
	void LSE_CALL CDirectX9VertexBuffer::OnDeviceLost() {
		if ( !m_bResourceCanBeLost ) { return; }

		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
				if ( reinterpret_cast<const CDirectX9VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->GetId() == GetId() ) {
					CGfx::VertexBuffers()[I].pvbVertBuffer = NULL;
					if ( CGfx::TotalActiveVertexBuffers() == I + 1 ) { CGfx::TotalActiveVertexBuffers()--; }
				}
			}
		}

		CDirectX9::SafeRelease( m_pvbVertexBuffer );
	}

	/**
	 * Must renew resources when the device is reset.
	 *
	 * \return Return true if the renewal is successful, false otherwise.
	 */
	LSBOOL LSE_CALL CDirectX9VertexBuffer::OnDeviceReset() {
		if ( !m_bResourceCanBeLost ) { return true; }
		return CreateApiVertexBuffer();
	}

	/**
	 * Creates a vertex buffer compliant with the current graphics API.
	 *
	 * \return Returns false if the creation of the buffer failed.
	 */
	LSBOOL LSE_CALL CDirectX9VertexBuffer::CreateApiVertexBuffer() {
		ResetApi();

		uint32_t ui32Size = m_ui32Stride * (m_ui32Elements > 1 ? m_ui32Elements : 1);
		D3DPOOL pPool = (m_ui32Usage == LSG_VBUT_SETONLY)/* && (ui32Size >= 8 * 1024)*/ ? D3DPOOL_MANAGED : D3DPOOL_SYSTEMMEM;
		m_bResourceCanBeLost = pPool == D3DPOOL_DEFAULT;
		uint32_t ui32Usage = m_ui32Usage;
		if ( m_ui32Topology == LSG_PT_POINTLIST ) {
			ui32Usage |= D3DUSAGE_POINTS;
		}
		LSBOOL bUseHardware = (CDirectX9::GetCreationParms().BehaviorFlags & (D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING)) != 0;
		ui32Usage |= !bUseHardware ? D3DUSAGE_SOFTWAREPROCESSING : 0;
		if ( pPool == D3DPOOL_DEFAULT ) {
			ui32Usage |= D3DUSAGE_WRITEONLY;
		}

		GenerateApiVertexDeclaration();


		HRESULT hError = CDirectX9::GetDirectX9Device()->CreateVertexBuffer( ui32Size,
			ui32Usage, 0, pPool,
			&m_pvbVertexBuffer, NULL );
		if ( FAILED( hError ) ) { return false; }


		// Copy the vertices into it.
		if ( m_oobpBuffer.Buffer() ) {
			void * pvData;
			if ( FAILED( m_pvbVertexBuffer->Lock( 0, 0, &pvData, 0 ) ) ) { return false; }
			CStd::MemCpy( pvData, m_oobpBuffer.Buffer(), m_ui32Stride * m_ui32Elements );
			if ( FAILED( m_pvbVertexBuffer->Unlock() ) ) { return false; }
		}

		if ( !m_bResourceCanBeLost && m_ui32Usage == LSG_VBUT_SETONLY && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}
		return true;
	}

	/**
	 * Destroy the vertex buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL CDirectX9VertexBuffer::ResetApi() {
		for ( uint32_t I = LSG_MAX_VERTEX_STREAMS; I--; ) {
			if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
				if ( reinterpret_cast<const CDirectX9VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->GetId() == GetId() ) {
					CGfx::VertexBuffers()[I].pvbVertBuffer = NULL;
					if ( CGfx::TotalActiveVertexBuffers() == I + 1 ) { CGfx::TotalActiveVertexBuffers()--; }
				}
			}
		}
		CDirectX9::SafeRelease( m_pvbVertexBuffer );

		static const D3DVERTEXELEMENT9 veEnd = D3DDECL_END();
		for ( uint32_t I = LSE_ELEMENTS( m_vdInputDesc ); I--; ) {
			m_vdInputDesc[I] = veEnd;
		}

		m_bResourceCanBeLost = false;
	}

	/**
	 * Given contents and a stream, generate a vertex declaration.
	 */
	void LSE_CALL CDirectX9VertexBuffer::GenerateApiVertexDeclaration() {
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


		uint32_t ui32Pos = 0;
		uint32_t ui32Index = 0;
		m_ui16ElementSize = 0;
		for ( uint32_t J = 0; J < m_ui16TotalDecl; ++J ) {
			assert( m_veElements[J].ui32Format < D3DDECLTYPE_UNUSED );
			for ( uint32_t I = 0; I < LSE_ELEMENTS( vdDecl ); ++I ) {
				if ( m_veElements[J].ui32Format == static_cast<uint32_t>(vdDecl[I].veType) ) {
					if ( m_veElements[J].ui32Offset ) {
						ui32Pos = m_veElements[J].ui32Offset;
					}
					m_vdInputDesc[ui32Index].Offset = static_cast<WORD>(ui32Pos);
					m_vdInputDesc[ui32Index].Type = static_cast<BYTE>(vdDecl[I].veType);
					m_vdInputDesc[ui32Index].Method = D3DDECLMETHOD_DEFAULT;
					m_vdInputDesc[ui32Index].Usage = static_cast<BYTE>(m_veElements[J].ui32Semantic);
					m_vdInputDesc[ui32Index].UsageIndex = static_cast<BYTE>(m_veElements[J].ui32SemanticIndex);
					m_vdInputDesc[ui32Index].Stream = 0;
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
	LSBOOL LSE_CALL CDirectX9VertexBuffer::SetStreamApi( uint32_t _ui32Index, uint32_t _ui32Offset ) const {
		return SUCCEEDED( CDirectX9::GetDirectX9Device()->SetStreamSource( _ui32Index, m_pvbVertexBuffer,
			_ui32Offset * m_ui32Stride, m_ui32Stride ) );
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL CDirectX9VertexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		// This is only called on the 0th vertex buffer active.  This is where we set buffer vertex declarations etc.
		// First check to see if this vertex buffer has been the base of this combination of vertex buffers before.
		if ( m_vVertDeclRecord.Length() < CGfx::TotalActiveVertexBuffers() ) {
			m_vVertDeclRecord.Resize( CGfx::TotalActiveVertexBuffers() );
		}
		LSG_DIRECTX9_LAST_VERT_DECLS * pdlvdLastDecl = &m_vVertDeclRecord[CGfx::TotalActiveVertexBuffers()-1];
		for ( uint32_t I = CGfx::TotalActiveVertexBuffers(); I--; ) {
			uint32_t ui32Id = CGfx::VertexBuffers()[I].pvbVertBuffer ? reinterpret_cast<const CDirectX9VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->GetId() : 0;
			
			if ( pdlvdLastDecl->ui32StreamIds[I] != ui32Id ) {
				// Not the same combination as last time.  Have to rebuild the vertex declaration.
				static D3DVERTEXELEMENT9 veElements[MAX_FVF_DECL_SIZE];
				uint32_t ui32Index = 0;
				for ( I = 0; I < CGfx::TotalActiveVertexBuffers(); ++I ) {
					if ( CGfx::VertexBuffers()[I].pvbVertBuffer ) {
						for ( uint32_t J = reinterpret_cast<const CDirectX9VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->m_ui16TotalDecl; J--; ) {
							veElements[ui32Index] = reinterpret_cast<const CDirectX9VertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer)->m_vdInputDesc[J];
							veElements[ui32Index++].Stream = static_cast<WORD>(I);
						}
					}
					pdlvdLastDecl->ui32StreamIds[I] = ui32Id;
				}

				static const D3DVERTEXELEMENT9 veEnd = D3DDECL_END();
				veElements[ui32Index] = veEnd;

				CDirectX9::SafeRelease( pdlvdLastDecl->pvdVertexDeclaration );
				if ( !SUCCEEDED( CDirectX9::GetDirectX9Device()->CreateVertexDeclaration( veElements, &pdlvdLastDecl->pvdVertexDeclaration ) ) ) {
					// Shit!
					return;
				}

				break;
			}
		}

		if ( !pdlvdLastDecl->pvdVertexDeclaration ) { return; }
		if ( !CDirectX9::SetInputLayout( pdlvdLastDecl->pvdVertexDeclaration ) ) { return; }


		if ( _ui32TotalPrimitives ) {
			CDirectX9::GetDirectX9Device()->DrawPrimitive( static_cast<D3DPRIMITIVETYPE>(m_ui32Topology),
				_ui32StartVertex,
				_ui32TotalPrimitives );
		}
	}

	/**
	 * Prepare for rendering.
	 */
	void LSE_CALL CDirectX9VertexBuffer::PrepareToRenderApi() const {
	}

	/**
	 * Finish rendering.
	 */
	void LSE_CALL CDirectX9VertexBuffer::EndRenderApi() const {
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
