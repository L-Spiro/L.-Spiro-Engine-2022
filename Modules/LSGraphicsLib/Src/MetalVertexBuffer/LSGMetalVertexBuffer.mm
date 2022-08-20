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

#include "LSGMetalVertexBuffer.h"

#ifdef LSG_METAL

#include "../Gfx/LSGGfx.h"
#include "../Metal/LSGMetalObject.h"
#include "../MetalShaders/LSGMetalShaderProgramManager.h"
#include "../ShaderLanguage/LSGShader.h"

#include <Metal/MTLVertexDescriptor.h>

namespace lsg {

	// == Members.
	/** Table of properties of vertex types. */
	CMetalVertexBuffer::LSG_METAL_PREFINED_VERT_ATT CMetalVertexBuffer::m_mpvaVertProps[] = {
		// -- 32-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R32_F,					4,			1,	MTLVertexFormatFloat,					false },
		{ LSG_VE_R32_UI,				4,			1,	MTLVertexFormatUInt,					false },
		{ LSG_VE_R32_SI,				4,			1,	MTLVertexFormatInt,						false },

		// * 2 components. * //
		{ LSG_VE_R32G32_F,				4,			2,	MTLVertexFormatFloat2,					false },
		{ LSG_VE_R32G32_UI,				4,			2,	MTLVertexFormatUInt2,					false },
		{ LSG_VE_R32G32_SI,				4,			2,	MTLVertexFormatInt2,					false },

		// * 3 components. * //
		{ LSG_VE_R32G32B32_F,			4,			3,	MTLVertexFormatFloat3,					false },
		{ LSG_VE_R32G32B32_UI,			4,			3,	MTLVertexFormatUInt3,					false },
		{ LSG_VE_R32G32B32_SI,			4,			3,	MTLVertexFormatInt3,					false },

		// * 4 components. * //
		{ LSG_VE_R32G32B32A32_F,		4,			4,	MTLVertexFormatFloat4,					false },
		{ LSG_VE_R32G32B32A32_UI,		4,			4,	MTLVertexFormatUInt4,					false },
		{ LSG_VE_R32G32B32A32_SI,		4,			4,	MTLVertexFormatInt4,					false },

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R16_F,					2,			1,	MTLVertexFormatInvalid,					false },
		{ LSG_VE_R16_UI,				2,			1,	MTLVertexFormatInvalid,					false },
		{ LSG_VE_R16_SI,				2,			1,	MTLVertexFormatInvalid,					false },
		{ LSG_VE_R16_UI_N,				2,			1,	MTLVertexFormatInvalid,					true },
		{ LSG_VE_R16_SI_N,				2,			1,	MTLVertexFormatInvalid,					true },

		// * 2 components. * //
		{ LSG_VE_R16G16_F,				2,			2,	MTLVertexFormatHalf2,					false },
		{ LSG_VE_R16G16_UI,				2,			2,	MTLVertexFormatUShort2,					false },
		{ LSG_VE_R16G16_SI,				2,			2,	MTLVertexFormatShort2,					false },
		{ LSG_VE_R16G16_UI_N,			2,			2,	MTLVertexFormatUShort2Normalized,		true },
		{ LSG_VE_R16G16_SI_N,			2,			2,	MTLVertexFormatShort2Normalized,		true },

		// * 4 components. * //
		{ LSG_VE_R16G16B16A16_F,		2,			4,	MTLVertexFormatHalf4,					false },
		{ LSG_VE_R16G16B16A16_UI,		2,			4,	MTLVertexFormatUShort4,					false },
		{ LSG_VE_R16G16B16A16_SI,		2,			4,	MTLVertexFormatShort4,					false },
		{ LSG_VE_R16G16B16A16_UI_N,		2,			4,	MTLVertexFormatUShort4Normalized,		true },
		{ LSG_VE_R16G16B16A16_SI_N,		2,			4,	MTLVertexFormatShort4Normalized,		true },

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		{ LSG_VE_R10G10B10_UI,			4,			1,	MTLVertexFormatInvalid,					false },
		{ LSG_VE_R10G10B10_UI_N,		4,			1,	MTLVertexFormatUInt1010102Normalized,	true },

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		{ LSG_VE_R8G8B8A8_UI,			4,			1,	MTLVertexFormatUChar4,					false },
		{ LSG_VE_R8G8B8A8_UI_N,			4,			1,	MTLVertexFormatUChar4Normalized,		true },
	};

	// == Various constructors.
	LSE_CALLCTOR CMetalVertexBuffer::CMetalVertexBuffer() :
		m_pvBuffer( NULL ),
		m_pvVertexDescriptor( NULL ),
		m_ui32StreamOffset( 0 ),
		m_ui32StreamIndex( 0 ) {
	}
	LSE_CALLCTOR CMetalVertexBuffer::~CMetalVertexBuffer() {
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
	LSG_VERTEX_BUFFER_MAP LSE_CALL CMetalVertexBuffer::Map( LSG_VERTEX_BUFFER_MAP_ACCESS _vbmaAccessType ) {
		/*if ( !m_uiVboId  ) {
			LSG_VERTEX_BUFFER_MAP vbmRet = {
				NULL
			};
			return vbmRet;
		}
		COpenGlEs::BindVertexBuffer( m_uiVboId );
		LSG_VERTEX_BUFFER_MAP vbmRet = {
			::glMapBufferRangeEXT( GL_ARRAY_BUFFER, 0, Stride() * Elements(), _vbmaAccessType & 0xFFFF ),
			Stride() * Elements()
		};
		return vbmRet;*/
	}

	/**
	 * Unmaps the vertex buffer after an update.
	 *
	 * \param _vbmMappedData The returned mapped data from a previous call to Map().
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::Unmap( LSG_VERTEX_BUFFER_MAP &_vbmMappedData ) {
		if ( 0 ) {

		}
		_vbmMappedData.pvData = NULL;
		_vbmMappedData.ui32Size = 0;
	}
	
	/**
	 * Creates a vertex buffer compliant with the current graphics API.
	 *
	 * \return Returns false if the creation of the buffer failed.
	 */
	LSBOOL LSE_CALL CMetalVertexBuffer::CreateApiVertexBuffer() {
		ResetApi();
		
		//MTLResourceStorageModeShared, MTLResourceStorageModePrivate;
		// MTLResourceOptionCPUCacheModeDefault is deprecated.
		MTLResourceOptions roOptions = (m_ui32Usage & LSG_VBUT_SETONLY) ? MTLResourceOptionCPUCacheModeDefault : MTLResourceOptionCPUCacheModeWriteCombined;
		
		id <MTLBuffer> bBuffer = [GetMetalDevice() newBufferWithBytes:m_oobpBuffer.Buffer() length:m_oobpBuffer.Length()
			options:roOptions];
		if ( bBuffer == nil ) { ResetApi(); return false; }
		m_pvBuffer = CFBridgingRetain( bBuffer );
		
		LSUINT32 ui32Pos = 0;
		m_ui16ElementSize = 0;
		for ( LSUINT32 J = 0; J < m_ui16TotalDecl; ++J ) {
			// Find the custom-format element.
			m_pmpvaMetalElements[J] = GetVertexElementAttr( static_cast<LSG_VERTEX_ELEMENT>(m_veElements[J].ui32Format) );
			if ( !m_pmpvaMetalElements[J] ) {
				ResetApi();
				return false;
			}
			if ( m_veElements[J].ui32Offset ) {
				ui32Pos = m_veElements[J].ui32Offset;
			}

			m_veElements[J].ui32Offset = ui32Pos;
			LSUINT32 ui32Size = m_pmpvaMetalElements[J]->ui32Size * m_pmpvaMetalElements[J]->iElements;
			ui32Pos += ui32Size;
			m_ui16ElementSize = CStd::Max<LSUINT16>( m_ui16ElementSize, static_cast<LSUINT16>(ui32Pos) );
		}


		return true;
	}

	/**
	 * Destroy the vertex buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::ResetApi() {
		if ( m_pvBuffer ) {
			CFBridgingRelease( m_pvBuffer );
			m_pvBuffer = NULL;
		}
		if ( m_pvVertexDescriptor ) {
			CFBridgingRelease( m_pvVertexDescriptor );
			m_pvVertexDescriptor = NULL;
		}
		m_ui32StreamOffset = m_ui32StreamIndex = 0;
	}

	/**
	 * Sets the vertex buffer on a given stream.
	 *
	 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
	 * \param _ui32Offset Offset in elements of the stream source.
	 * \return Returns true if the vertex buffer was applied to the given stream index.
	 */
	LSBOOL LSE_CALL CMetalVertexBuffer::SetStreamApi( LSUINT32 _ui32Index, LSUINT32 _ui32Offset ) const {
		m_ui32StreamIndex = _ui32Index;
		m_ui32StreamOffset = _ui32Offset;
		return true;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartVertex Index of the first vertex to load.  Beginning at _ui32StartVertex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::RenderApi( LSUINT32 _ui32StartVertex, LSUINT32 _ui32TotalPrimitives ) const {
		for ( LSUINT32 I = CGfx::TotalActiveVertexBuffers(); I--; ) {
			const CMetalVertexBuffer * pmvbThis = reinterpret_cast<const CMetalVertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer);
			if ( m_pmvbDeclRecord[I] != pmvbThis )  {
				CreateVertexDescriptor();
				break;
			}
		}
		if ( !m_pvVertexDescriptor ) { return; }
		GetRenderPipelineDescriptor().vertexDescriptor = (__bridge MTLVertexDescriptor *)m_pvVertexDescriptor;
		
		if ( _ui32TotalPrimitives ) {
			[GetCommandEncoder() drawPrimitives:static_cast<MTLPrimitiveType>(Topology())
				vertexStart:_ui32StartVertex
				vertexCount:VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives )];
		}
	}

	/**
	 * Prepare for rendering.
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::PrepareToRenderApi() const {
		assert( CGfx::GetShader() );
		id <MTLBuffer> bBuffer = (__bridge id <MTLBuffer>)m_pvBuffer;
		[GetCommandEncoder() setVertexBuffer:bBuffer offset:m_ui32StreamOffset atIndex:m_ui32StreamIndex];
	}

	/**
	 * Finish rendering.
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::EndRenderApi() const {

	}
	
	/** 
	 * Creates the m_pvVertexDescriptor object based on the currently set vertex buffers.
	 */
	LSVOID LSE_CALL CMetalVertexBuffer::CreateVertexDescriptor() const {
		if ( m_pvVertexDescriptor ) {
			CFBridgingRelease( m_pvVertexDescriptor );
			m_pvVertexDescriptor = NULL;
		}
		
		
		
		struct LSG_SEMANTIC {
			const CMetalVertexBuffer *		pmvbThis[LSG_MAX_VERTEX_SEMANTIC];
			LSUINT8							ui8Index[LSG_MAX_VERTEX_SEMANTIC];
			LSUINT8							ui8Stream[LSG_MAX_VERTEX_SEMANTIC];
			LSUINT8							ui8Low, ui8High;
			
			LSG_SEMANTIC() :
				ui8Low( LSG_MAX_VERTEX_SEMANTIC ),
				ui8High( 0 ) {}
		};
		LSG_SEMANTIC sSortedAttributes[LSG_S_TOTAL];
		
		
		LSUINT32 ui32Count = 0;
		MTLVertexDescriptor * pvdDesc = [[MTLVertexDescriptor alloc] init];
		// Put the attributes into the table for fast ordered access.
		for ( LSUINT32 I = CGfx::TotalActiveVertexBuffers(); I--; ) {
			const CMetalVertexBuffer * pmvbThis = reinterpret_cast<const CMetalVertexBuffer *>(CGfx::VertexBuffers()[I].pvbVertBuffer);
			m_pmvbDeclRecord[I] = pmvbThis;
			for ( LSUINT32 J = 0; J < pmvbThis->m_ui16TotalDecl; ++J ) {
				LSUINT32 ui32Semantic = pmvbThis->m_veElements[J].ui32Semantic;
				LSUINT32 ui32SemanticIndex = pmvbThis->m_veElements[J].ui32SemanticIndex;
				assert( ui32Semantic < LSG_S_TOTAL );
				assert( ui32SemanticIndex < LSG_MAX_VERTEX_SEMANTIC );
				
				sSortedAttributes[ui32Semantic].pmvbThis[ui32SemanticIndex] = pmvbThis;
				sSortedAttributes[ui32Semantic].ui8Index[ui32SemanticIndex] = static_cast<LSUINT8>(J);
				sSortedAttributes[ui32Semantic].ui8Stream[ui32SemanticIndex] = static_cast<LSUINT8>(I);
				sSortedAttributes[ui32Semantic].ui8Low = CStd::Min<LSUINT8>( sSortedAttributes[ui32Semantic].ui8Low, ui32SemanticIndex );
				sSortedAttributes[ui32Semantic].ui8High = CStd::Max<LSUINT8>( sSortedAttributes[ui32Semantic].ui8Low, ui32SemanticIndex + 1 );
			}
			pvdDesc.layouts[I].stride = pmvbThis->Stride();
			pvdDesc.layouts[I].stepFunction = MTLVertexStepFunctionPerVertex;
			ui32Count += pmvbThis->m_ui16TotalDecl;
		}
		
		// Go over the table in order.
		LSUINT32 ui32Index = 0;
		for ( LSUINT32 I = 0; I < LSG_S_TOTAL; ++I ) {
			for ( LSUINT32 J = sSortedAttributes[I].ui8Low; J < sSortedAttributes[I].ui8High; ++J ) {
				const CMetalVertexBuffer * pmvbThis = sSortedAttributes[I].pmvbThis[J];
				LSUINT8 ui8Index = sSortedAttributes[I].ui8Index[J];
				LSUINT8 ui8Stream = sSortedAttributes[I].ui8Stream[J];
				pvdDesc.attributes[ui32Index].format = static_cast<MTLVertexFormat>(pmvbThis->m_pmpvaMetalElements[ui8Index]->eType);
				pvdDesc.attributes[ui32Index].bufferIndex = ui8Stream;
				pvdDesc.attributes[ui32Index].offset = pmvbThis->m_veElements[ui8Index].ui32Offset;
				++ui32Index;
			}
		}
		for ( LSUINT32 I = CGfx::TotalActiveVertexBuffers(); I < LSG_MAX_VERTEX_ELEMENT_COUNT; ++I ) {
			m_pmvbDeclRecord[I] = NULL;
		}
		assert( ui32Index == ui32Count );
		
		m_pvVertexDescriptor = CFBridgingRetain( pvdDesc );
	}

	/**
	 * Gets a pointer to a LSG_METAL_PREFINED_VERT_ATT structure from the m_mpvaVertProps array by ID (a LSG_VERTEX_ELEMENT value).
	 *
	 * \param _veType The type whose information inside m_mpvaVertProps is to be found.
	 * \return Returns a pointer to an item inside m_mpvaVertProps or NULL if the item does not exist.
	 */
	const CMetalVertexBuffer::LSG_METAL_PREFINED_VERT_ATT * LSE_CALL CMetalVertexBuffer::GetVertexElementAttr( LSG_VERTEX_ELEMENT _veType ) {
		assert( _veType < LSE_ELEMENTS( m_mpvaVertProps ) );
		if ( m_mpvaVertProps[_veType].ui32Mask == static_cast<LSUINT32>(_veType) ) { return &m_mpvaVertProps[_veType]; }
		for ( LSUINT32 I = LSE_ELEMENTS( m_mpvaVertProps ); I--; ) {
			if ( m_mpvaVertProps[I].ui32Mask == static_cast<LSUINT32>(_veType) ) { return &m_mpvaVertProps[I]; }
		}
		return NULL;
	}
}

#endif	// #ifdef LSG_METAL
