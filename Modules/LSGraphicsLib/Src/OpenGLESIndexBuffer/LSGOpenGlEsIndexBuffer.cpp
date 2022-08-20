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
 * Description: An OpenGL ES index buffer.
 */

#include "LSGOpenGlEsIndexBuffer.h"

#ifdef LSG_OGLES

#include "../VertexBuffer/LSGVertexBuffer.h"

namespace lsg {

	// == Members.
	/** During the creation of index buffer objects, we must enforce single-threaded access to
	 *	some resources. */
	CCriticalSection COpenGlEsIndexBuffer::m_csCrit;

	// == Various constructors.
	LSE_CALLCTOR COpenGlEsIndexBuffer::COpenGlEsIndexBuffer() :
		m_uiVboId( 0 ),
		m_eIndexType( 0 ) {
	}
	LSE_CALLCTOR COpenGlEsIndexBuffer::~COpenGlEsIndexBuffer() {
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
	LSBOOL LSE_CALL COpenGlEsIndexBuffer::CreateApiIndexBuffer() {
		ResetApi();

		assert( m_ui32ElementSize == 1 || m_ui32ElementSize == 2 || m_ui32ElementSize == 4 );
		switch ( m_ui32ElementSize ) {
			case 1 : { m_eIndexType = GL_UNSIGNED_BYTE; break; }
			case 2 : { m_eIndexType = GL_UNSIGNED_SHORT; break; }
			case 4 : { m_eIndexType = GL_UNSIGNED_INT; break; }
		}

		// Try to use an index buffer object.
		// We need the error flag.
		CCriticalSection::CLocker lGetError( COpenGlEs::m_csCrit );
		glWarnErrorAlways( "Uncaught" );	// Clear error flag.

		::glGenBuffers( 1, &m_uiVboId );
		// Errors are actually non-critical here, since we can fall back on using regular index arrays.
		if ( glWarnErrorAlways( "Failed to generate IBO" ) ) {
			if ( m_uiVboId ) {
				::glDeleteBuffers( 1, &m_uiVboId );
				m_uiVboId = 0;
			}
			return true;
		}

		CCriticalSection::CLockerS lsLockBindBuffer( m_csCrit );
		COpenGlEs::BindIndexBuffer( m_uiVboId );

		// Success.  Upload.
		::glBufferData( GL_ELEMENT_ARRAY_BUFFER,
			m_oobpBuffer.Length(),
			m_oobpBuffer.Buffer(),
			m_ui32Usage );
		COpenGlEs::BindIndexBuffer( 0 );
		if ( ::glGetError() != GL_NO_ERROR ) {
			// Again, fall back on vertex arrays.
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
			return true;
		}

		if ( m_ui32Usage == GL_STATIC_DRAW && !m_oobpBuffer.IsBorrowed() ) {
			m_oobpBuffer.Reset();
		}

		return true;
	}

	/**
	 * Destroy the index buffer that is compliant with the current graphics API and all related
	 *	resources.
	 */
	void LSE_CALL COpenGlEsIndexBuffer::ResetApi() {
		if ( m_uiVboId ) {
			::glDeleteBuffers( 1, &m_uiVboId );
			m_uiVboId = 0;
		}
		m_eIndexType = 0;
	}

	/**
	 * Render.  This is performed on the first vertex buffer set.  All other vertex buffers must use the same primitive type
	 *	and have the same number of elements active.  If not, the system will throw an error.
	 *
	 * \param _ui32StartIndex Index of the first vertex to load.  Beginning at _ui32StartIndex the correct number of vertices will be read out of the vertex buffer.
	 * \param _ui32TotalPrimitives The total number of primitives to render.
	 */
	void LSE_CALL COpenGlEsIndexBuffer::RenderApi( uint32_t _ui32StartIndex, uint32_t _ui32TotalPrimitives ) const {
		if ( m_oobpBuffer.Length() || m_uiVboId ) {
			CCriticalSection::CLockerS lsLockBindBuffer( m_csCrit );
			glWarnError( "Uncaught" );	// Clear error flag.
			COpenGlEs::BindIndexBuffer( m_uiVboId );
			glWarnError( "Failed to bind index buffer" );

#ifdef _DEBUG
			if ( _ui32StartIndex != 0 ) {
				CStd::DebugPrintA( "_ui32StartIndex must be 0 on this platform.¥r¥n" );
			}
#endif	// #ifdef _DEBUG
			if ( _ui32StartIndex == 0 ) {
				COpenGlEs::FinalizeVertAttrib();
				::glDrawElements( m_ui32Topology,
					CVertexBuffer::VertexCount( static_cast<LSG_PRIMITIVE_TOPOLOGY>(Topology()), _ui32TotalPrimitives ),
					m_eIndexType, m_uiVboId ?
						0 :
						m_oobpBuffer.Buffer() );
				glWarnError( "Failed to draw with index buffer" );
			}
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_OGLES
