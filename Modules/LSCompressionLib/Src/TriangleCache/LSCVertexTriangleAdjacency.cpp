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
 * Description: Builds a vertex-triangle adjacency list.  Each entry represents a vertex and has an array of
 *	pointers to the indices in an index buffer that make up the triangles adjacent to the respective vertex.
 *	The index buffer must be in triangle-list format.
 */

#include "LSCVertexTriangleAdjacency.h"


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CVertexTriangleAdjacency::CVertexTriangleAdjacency() {
	}

	// == Functions.
	/**
	 * Builds the vertex-triangle adjacency list.
	 *
	 * \param _pui32Indices The index buffer.
	 * \param _ui32Total Total indices in the index buffer.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CVertexTriangleAdjacency::BuildList( const uint32_t * _pui32Indices, uint32_t _ui32Total ) {
		// Determine how many vertices we need.
		if ( !m_vVerts.Resize( CountVertices( _pui32Indices, _ui32Total ) ) ) { return false; }
		if ( !m_vVerts.Length() ) { return true; }

		// Count how many total connections there are.
		uint32_t ui32Connections = _ui32Total;//CountConnections( _pui32Indices, _ui32Total );
		if ( !m_vPool.Resize( ui32Connections ) ) { return false; }

		uint32_t ui32AllocIndex = 0;
		// Make connections.
		for ( uint32_t I = 0; I < m_vVerts.Length(); ++I ) {
			ConnectTriangles( I, ui32AllocIndex, _pui32Indices, _ui32Total );
		}

		return true;
	}

	/**
	 * Counts the number of unique vertices in an index buffer.
	 *
	 * \param _pui32Indices The index buffer.
	 * \param _ui32Total Total indices in the index buffer.
	 * \return Returns the number of vertices in the index buffer.
	 */
	uint32_t LSE_CALL CVertexTriangleAdjacency::CountVertices( const uint32_t * _pui32Indices, uint32_t _ui32Total ) {
		// We map m_vVerts 1-for-1 to the vertices referenced by _pui32Indices, so the total unique vertices is
		//	actually the highest-referenced vertex + 1.
		uint32_t ui32Total = 0;
		for ( uint32_t I = _ui32Total; I--; ) {
			ui32Total = CStd::Max( ui32Total, _pui32Indices[I] );
		}
		return ui32Total + 1;
	}

	/**
	 * Counts the total triangles connected to a single vertex.
	 *
	 * \param _ui32Vert The vertex index.
	 * \param _pui32Indices The index buffer.
	 * \param _ui32Total Total indices in the index buffer.
	 * \return Returns the total number of triangles in the index buffer that are connected to the given vertex.
	 */
	uint32_t LSE_CALL CVertexTriangleAdjacency::CountConnections( uint32_t _ui32Vert, const uint32_t * _pui32Indices, uint32_t _ui32Total ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = 0; I < _ui32Total; I += 3 ) {
			// Important note: Count degenerates as multiple connections.  Later in the algorithm a single degenerate triangle
			//	will subtract the correct number of counts to make everything work correctly.
			if ( _pui32Indices[I] == _ui32Vert ) {
				++ui32Total;
			}
			if ( _pui32Indices[I+1] == _ui32Vert ) {
				++ui32Total;
			}
			if ( _pui32Indices[I+2] == _ui32Vert ) {
				++ui32Total;
			}
		}
		return ui32Total;
	}

	/**
	 * Counts the total connections to triangles for all vertices.
	 *
	 * \param _pui32Indices The index buffer.
	 * \param _ui32Total Total indices in the index buffer.
	 * \return Returns the total connections for all vertices.
	 */
	uint32_t LSE_CALL CVertexTriangleAdjacency::CountConnections( const uint32_t * _pui32Indices, uint32_t _ui32Total ) {
		uint32_t ui32Total = 0;
		for ( uint32_t I = m_vVerts.Length(); I--; ) {
			ui32Total += CountConnections( I, _pui32Indices, _ui32Total );
		}
		return ui32Total;
	}

	/**
	 * Connects triangles to a vertex.
	 *
	 * \param _ui32Vert The vertex to which to connect adjacent triangles.
	 * \param _ui32Alloc Allocation index.
	 * \param _pui32Indices The index buffer.
	 * \param _ui32Total Total indices in the index buffer.
	 */
	void LSE_CALL CVertexTriangleAdjacency::ConnectTriangles( uint32_t _ui32Vert, uint32_t &_ui32Alloc, const uint32_t * _pui32Indices, uint32_t _ui32Total ) {
		uint32_t * pui32Buffer = &m_vPool[_ui32Alloc];
		m_vVerts[_ui32Vert].pui32Tris = pui32Buffer;
		uint32_t ui32Total = 0;
		for ( uint32_t I = 0; I < _ui32Total; I += 3 ) {
			// Important note: Count degenerates as multiple connections.  Later in the algorithm a single degenerate triangle
			//	will subtract the correct number of counts to make everything work correctly.
			if ( _pui32Indices[I] == _ui32Vert ) {
				pui32Buffer[ui32Total++] = I / 3;
			}
			if ( _pui32Indices[I+1] == _ui32Vert ) {
				pui32Buffer[ui32Total++] = I / 3;
			}
			if ( _pui32Indices[I+2] == _ui32Vert ) {
				pui32Buffer[ui32Total++] = I / 3;
			}
		}
		m_vVerts[_ui32Vert].ui32Total = ui32Total;
		_ui32Alloc += ui32Total;
	}

}	// namespace lsc
