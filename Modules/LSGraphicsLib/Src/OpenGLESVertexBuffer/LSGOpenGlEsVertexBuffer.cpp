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
 * Description: An OpenGL ES vertex buffer.
 */

#include "LSGOpenGlEsVertexBuffer.h"

#ifdef LSG_OGLES

#include "../Gfx/LSGGfx.h"
#include "../OpenGLESShaders/LSGOpenGlEsShaderProgramManager.h"
#include "../ShaderLanguage/LSGShader.h"

namespace lsg {

	// == Members.
	/** Table of properties of vertex types. */
	COpenGlEsVertexBuffer::LSG_OPENGL_PREFINED_VERT_ATT COpenGlEsVertexBuffer::m_opvaVertProps[] = {
		// -- 32-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R32_F,					4,			1,	GL_FLOAT,							GL_FALSE },
		{ LSG_VE_R32_UI,				4,			1,	GL_UNSIGNED_INT,					GL_FALSE },
		{ LSG_VE_R32_SI,				4,			1,	GL_INT,								GL_FALSE },

		// * 2 components. * //
		{ LSG_VE_R32G32_F,				4,			2,	GL_FLOAT,							GL_FALSE },
		{ LSG_VE_R32G32_UI,				4,			2,	GL_UNSIGNED_INT,					GL_FALSE },
		{ LSG_VE_R32G32_SI,				4,			2,	GL_INT,								GL_FALSE },

		// * 3 components. * //
		{ LSG_VE_R32G32B32_F,			4,			3,	GL_FLOAT,							GL_FALSE },
		{ LSG_VE_R32G32B32_UI,			4,			3,	GL_UNSIGNED_INT,					GL_FALSE },
		{ LSG_VE_R32G32B32_SI,			4,			3,	GL_INT,								GL_FALSE },

		// * 4 components. * //
		{ LSG_VE_R32G32B32A32_F,		4,			4,	GL_FLOAT,							GL_FALSE },
		{ LSG_VE_R32G32B32A32_UI,		4,			4,	GL_UNSIGNED_INT,					GL_FALSE },
		{ LSG_VE_R32G32B32A32_SI,		4,			4,	GL_INT,								GL_FALSE },

		// -- 16-bit Elements. -- //
		// * 1 component. * //
		{ LSG_VE_R16_F,					2,			1,	GL_HALF_FLOAT_OES,					GL_FALSE },
		{ LSG_VE_R16_UI,				2,			1,	GL_UNSIGNED_SHORT,					GL_FALSE },
		{ LSG_VE_R16_SI,				2,			1,	GL_SHORT,							GL_FALSE },
		{ LSG_VE_R16_UI_N,				2,			1,	GL_UNSIGNED_SHORT,					GL_TRUE },
		{ LSG_VE_R16_SI_N,				2,			1,	GL_SHORT,							GL_TRUE },

		// * 2 components. * //
		{ LSG_VE_R16G16_F,				2,			2,	GL_HALF_FLOAT_OES,					GL_FALSE },
		{ LSG_VE_R16G16_UI,				2,			2,	GL_UNSIGNED_SHORT,					GL_FALSE },
		{ LSG_VE_R16G16_SI,				2,			2,	GL_SHORT,							GL_FALSE },
		{ LSG_VE_R16G16_UI_N,			2,			2,	GL_UNSIGNED_SHORT,					GL_TRUE },
		{ LSG_VE_R16G16_SI_N,			2,			2,	GL_SHORT,							GL_TRUE },

		// * 4 components. * //
		{ LSG_VE_R16G16B16A16_F,		2,			4,	GL_HALF_FLOAT_OES,					GL_FALSE },
		{ LSG_VE_R16G16B16A16_UI,		2,			4,	GL_UNSIGNED_SHORT,					GL_FALSE },
		{ LSG_VE_R16G16B16A16_SI,		2,			4,	GL_SHORT,							GL_FALSE },
		{ LSG_VE_R16G16B16A16_UI_N,		2,			4,	GL_UNSIGNED_SHORT,					GL_TRUE },
		{ LSG_VE_R16G16B16A16_SI_N,		2,			4,	GL_SHORT,							GL_TRUE },

		// -- 10-bit Elements. -- //
		// * 3 components. * //
		{ LSG_VE_R10G10B10A2_UI,		4,			1,	0,									GL_FALSE },
		{ LSG_VE_R10G10B10A2_UI_N,		4,			1,	0,									GL_TRUE },

		// -- 8-bit Elements. -- //
		// * 4 components. * //
		{ LSG_VE_R8G8B8A8_UI,			4,			GL_BGRA,	GL_UNSIGNED_INT,			GL_FALSE },
		{ LSG_VE_R8G8B8A8_UI_N,			4,			GL_BGRA,	GL_UNSIGNED_INT,			GL_TRUE },
	};

	/**< Current enabled vertex attributes. */
	uint8_t COpenGlEsVertexBuffer::m_ui8CurVertxAttribEnabled[LSG_MAX_VERTEX_ELEMENT_COUNT] = { false };

	/**< Last enabled vertex attributes. */
	uint8_t COpenGlEsVertexBuffer::m_ui8LastVertxAttribEnabled[LSG_MAX_VERTEX_ELEMENT_COUNT] = { false };

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsVertexBuffer::COpenGlEsVertexBuffer() :
		m_uiVboId( 0 ),
		m_ui32StreamOffset( 0UL ) {
	}
	LSE_CALLCTOR COpenGlEsVertexBuffer::~COpenGlEsVertexBuffer() {
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
	LSG_VERTEX_BUFFER_MAP LSE_CALL COpenGlEsVertexBuffer::Map( LSG_VERTEX_BUFFER_MAP_ACCESS _vbmaAccessType ) {
		if ( !m_uiVboId  ) {
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
		return vbmRet;
	}

	/**
	 * Unmaps the vertex buffer after an update.
	 *
	 * \param _vbmMappedData The returned mapped data from a previous call to Map().
	 */
	void LSE_CALL COpenGlEsVertexBuffer::Unmap( LSG_VERTEX_BUFFER_MAP &_vbmMappedData ) {
		if ( m_uiVboId ) {
			COpenGlEs::BindVertexBuffer( m_uiVboId );
			::glUnmapBufferOES( GL_ARRAY_BUFFER );
		}
		_vbmMappedData.pvData = NULL;
		_vbmMappedData.ui32Size = 0;
	}

	/**
	 * Creates a vertex buffer compliant with the current graphics API.
	 *
	 * \return Returns GL_FALSE if the creation of the buffer failed.
	 */
	LSBOOL LSE_CALL COpenGlEsVertexBuffer::CreateApiVertexBuffer() {
		ResetApi();

		uint32_t ui32Offset = 0UL;
		for ( uint32_t I = 0UL; I < m_ui16TotalDecl; ++I ) {
			if ( m_veElements[I].ui32Offset ) {
				ui32Offset = m_veElements[I].ui32Offset;
			}
			LSG_VERTEX_ATTRIB_POINTER vapPointer;
			const LSG_OPENGL_PREFINED_VERT_ATT * popvaAttr = GetVertexElementAttr( static_cast<LSG_VERTEX_ELEMENT>(m_veElements[I].ui32Format) );
			if ( !popvaAttr ) {
				CStd::DebugPrintA( "COpenGlEsVertexBuffer::CreateApiVertexBuffer(): Unrecognized format in vertex buffer." );
				ResetApi();
				return false;
			}

			vapPointer.pvPointer = m_oobpBuffer.Buffer() + ui32Offset;
			vapPointer.ui16LocClass = static_cast<uint16_t>(m_veElements[I].ui32Semantic);
			vapPointer.ui16LocUsage = static_cast<uint16_t>(m_veElements[I].ui32SemanticIndex);
			vapPointer.ui16Stride = static_cast<uint16_t>(m_ui32Stride);
			vapPointer.ui8Normalized = popvaAttr->bNormalized;
			vapPointer.ui32Type = popvaAttr->eType;
			vapPointer.ui8Elements = static_cast<uint8_t>(popvaAttr->iElements);
			uint32_t ui32ThisSize = popvaAttr->ui32Size;
			if ( popvaAttr->iElements != GL_BGRA ) {
				ui32ThisSize *= popvaAttr->iElements;
			}
			m_vapVertexAttribPointer[I] = vapPointer;
			ui32Offset += ui32ThisSize;
		}

		// Try to use a vertex buffer object.
		// We need the error flag.
		CCriticalSection::CLocker lGetError( COpenGlEs::m_csCrit );
		glWarnErrorAlways( "Uncaught" );

		// Create a new vertex buffer object.
		::glGenBuffers( 1, &m_uiVboId );

		// Errors are actually non-critical here since we can fall back on using regular vertex arrays.
		if ( ::glGetError() != GL_NO_ERROR || m_uiVboId == 0 ) {
			m_uiVboId = 0;
			return true;
		}
		
		// From this point on, single-thread.  Ensure that the binded buffer stays binded while we
		//	are working on it.
		CCriticalSection::CLockerS lsLockBindBuffer( m_csCrit );
		COpenGlEs::BindVertexBuffer( m_uiVboId );
		if ( glWarnErrorAlways( "glBindBuffer() failed" ) ) {
			// Again, fall back on vertex arrays.
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
			return true;
		}

		// Success.  Upload.
		::glBufferData( GL_ARRAY_BUFFER,
			m_ui32Elements * m_ui32Stride,
			m_oobpBuffer.Buffer(),
			m_ui32Usage );
		if ( glWarnErrorAlways( "glBufferData() failed" ) ) {
			// Again, fall back on vertex arrays.
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
			COpenGlEs::BindVertexBuffer( 0 );
			::glGetError();
			return true;
		}

		COpenGlEs::BindVertexBuffer( 0 );
		if ( glWarnErrorAlways( "glBindBuffer() failed (on unbind)" ) ) {
			// Again, fall back on vertex arrays.
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
			return true;
		}

		// Everything went to plan so we need to adjust the offsets in our vertex attribute list.
		for ( uint32_t I = m_ui16TotalDecl; I--; ) {
			m_vapVertexAttribPointer[I].pvPointer = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(m_vapVertexAttribPointer[I].pvPointer) -
				reinterpret_cast<uintptr_t>(m_oobpBuffer.Buffer()));
		}

		if ( m_ui32Usage == GL_STATIC_DRAW && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}
		return true;
	}

	/**
	 * Destroy the vertex buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL COpenGlEsVertexBuffer::ResetApi() {
		if ( m_uiVboId ) {
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
		}
		m_ui32StreamOffset = 0UL;
	}

	/**
	 * Sets the vertex buffer on a given stream.
	 *
	 * \param _ui32Index Index of the stream to which to apply this vertex buffer.
	 * \param _ui32Offset Offset in elements of the stream source.
	 * \return Returns true if the vertex buffer was applied to the given stream index.
	 */
	LSBOOL LSE_CALL COpenGlEsVertexBuffer::SetStreamApi( uint32_t /*_ui32Index*/, uint32_t _ui32Offset ) const {
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
	void LSE_CALL COpenGlEsVertexBuffer::RenderApi( uint32_t _ui32StartVertex, uint32_t _ui32TotalPrimitives ) const {
		if ( _ui32TotalPrimitives ) {
			COpenGlEs::FinalizeVertAttrib();
			
			::glDrawArrays( m_ui32Topology,
				_ui32StartVertex,
				VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives ) );
		}
	}

	/**
	 * Prepare for rendering.
	 */
	void LSE_CALL COpenGlEsVertexBuffer::PrepareToRenderApi() const {
		assert( CGfx::GetShader() );
		glWarnError( "Uncaught" );
		CCriticalSection::CLockerS lsLockBindBuffer( m_csCrit );
		COpenGlEs::BindVertexBuffer( m_uiVboId );	// Even if m_uiVboId is 0, that is fine.

		// Set vertex attribute locations.
		for ( uint32_t I = m_ui16TotalDecl; I--; ) {
			const LSG_VERTEX_ATTRIB_POINTER * pvapThis = &m_vapVertexAttribPointer[I];
			const COpenGlEsShaderProgramManager::LSG_ATTRIBUTE_USAGE_LOCS * paulLocs = CGfx::GetShader()->GetAttributeLocs();
			GLint iAttrib = paulLocs[pvapThis->ui16LocClass].iLoc[pvapThis->ui16LocUsage];
			if ( iAttrib != -1 ) {
				COpenGlEs::SetVertexAttribArray( iAttrib, GL_TRUE );
				::glVertexAttribPointer( iAttrib,
					pvapThis->ui8Elements,
					pvapThis->ui32Type,
					pvapThis->ui8Normalized,
					m_ui32Stride,
					reinterpret_cast<uint8_t *>(pvapThis->pvPointer) + m_ui32StreamOffset * m_ui32Stride );
				glWarnError( "Error setting attribute pointer" );
			}
		}
	}

	/**
	 * Finish rendering.
	 */
	void LSE_CALL COpenGlEsVertexBuffer::EndRenderApi() const {
		for ( uint32_t I = m_ui16TotalDecl; I--; ) {
			GLint iAttrib = CGfx::GetShader()->GetAttributeLocs()[m_vapVertexAttribPointer[I].ui16LocClass].iLoc[m_vapVertexAttribPointer[I].ui16LocUsage];
			if ( iAttrib != -1 ) {
				COpenGlEs::SetVertexAttribArray( iAttrib, GL_FALSE );
			}
		}
	}

	/**
	 * Gets a pointer to a LSG_OPENGL_PREFINED_VERT_ATT structure from the m_opvaVertProps array by ID (a LSG_VERTEX_ELEMENT value).
	 *
	 * \param _veType The type whose information inside m_opvaVertProps is to be found.
	 * \return Returns a pointer to an item inside m_opvaVertProps or NULL if the item does not exist.
	 */
	const COpenGlEsVertexBuffer::LSG_OPENGL_PREFINED_VERT_ATT * LSE_CALL COpenGlEsVertexBuffer::GetVertexElementAttr( LSG_VERTEX_ELEMENT _veType ) {
		assert( _veType < LSE_ELEMENTS( m_opvaVertProps ) );
		if ( m_opvaVertProps[_veType].ui32Mask == static_cast<uint32_t>(_veType) ) { return &m_opvaVertProps[_veType]; }
		for ( uint32_t I = LSE_ELEMENTS( m_opvaVertProps ); I--; ) {
			if ( m_opvaVertProps[I].ui32Mask == static_cast<uint32_t>(_veType) ) { return &m_opvaVertProps[I]; }
		}
		return NULL;
	}
}

#endif	// #ifdef LSG_OGLES
