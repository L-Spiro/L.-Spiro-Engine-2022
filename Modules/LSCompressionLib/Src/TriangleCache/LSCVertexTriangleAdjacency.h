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


#ifndef __LSC_VERTEXTRIANGLEADJACENCY_H__
#define __LSC_VERTEXTRIANGLEADJACENCY_H__

#include "../LSCCompressionLib.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsc {

	/**
	 * Class CVertexTriangleAdjacency
	 * \brief Builds a vertex-triangle adjacency list.
	 *
	 * Description: Builds a vertex-triangle adjacency list.  Each entry represents a vertex and has an array of
	 *	pointers to the indices in an index buffer that make up the triangles adjacent to the respective vertex.
	 *	The index buffer must be in triangle-list format.
	 */
	class CVertexTriangleAdjacency {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVertexTriangleAdjacency();


		// == Types.
		/** A vertex. */
		typedef struct LSC_VERTEX_ADJACENCY {
			/** Array of indices to the adjacent triangles. */
			uint32_t *							pui32Tris;

			/** Total triangles to which this vertex is adjacent. */
			uint32_t							ui32Total;
		} * LPLSC_VERTEX_ADJACENCY, * const LPCLSC_VERTEX_ADJACENCY;


		// == Functions.
		/**
		 * Builds the vertex-triangle adjacency list.
		 *
		 * \param _pui32Indices The index buffer.
		 * \param _ui32Total Total indices in the index buffer.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL							BuildList( const uint32_t * _pui32Indices, uint32_t _ui32Total );

		/**
		 * Gets a reference to the list.
		 *
		 * \return Returns a reference to the list.
		 */
		LSE_INLINE CVectorPoD<LSC_VERTEX_ADJACENCY, uint32_t> & LSE_CALL
												List();


	protected :
		// == Members.
		/** The pool from which to "allocate" memory. */
		CVectorPoD<uint32_t, uint32_t>			m_vPool;

		/** The list of verteices. */
		CVectorPoD<LSC_VERTEX_ADJACENCY, uint32_t>
												m_vVerts;


		// == Functions.
		/**
		 * Counts the number of unique vertices in an index buffer.
		 *
		 * \param _pui32Indices The index buffer.
		 * \param _ui32Total Total indices in the index buffer.
		 * \return Returns the number of vertices in the index buffer.
		 */
		uint32_t LSE_CALL						CountVertices( const uint32_t * _pui32Indices, uint32_t _ui32Total );

		/**
		 * Counts the total triangles connected to a single vertex.
		 *
		 * \param _ui32Vert The vertex index.
		 * \param _pui32Indices The index buffer.
		 * \param _ui32Total Total indices in the index buffer.
		 * \return Returns the total number of triangles in the index buffer that are connected to the given vertex.
		 */
		uint32_t LSE_CALL						CountConnections( uint32_t _ui32Vert, const uint32_t * _pui32Indices, uint32_t _ui32Total );

		/**
		 * Counts the total connections to triangles for all vertices.
		 *
		 * \param _pui32Indices The index buffer.
		 * \param _ui32Total Total indices in the index buffer.
		 * \return Returns the total connections for all vertices.
		 */
		uint32_t LSE_CALL						CountConnections( const uint32_t * _pui32Indices, uint32_t _ui32Total );

		/**
		 * Connects triangles to a vertex.
		 *
		 * \param _ui32Vert The vertex to which to connect adjacent triangles.
		 * \param _ui32Alloc Allocation index.
		 * \param _pui32Indices The index buffer.
		 * \param _ui32Total Total indices in the index buffer.
		 */
		void LSE_CALL							ConnectTriangles( uint32_t _ui32Vert, uint32_t &_ui32Alloc, const uint32_t * _pui32Indices, uint32_t _ui32Total );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a reference to the list.
	 *
	 * \return Returns a reference to the list.
	 */
	LSE_INLINE CVectorPoD<CVertexTriangleAdjacency::LSC_VERTEX_ADJACENCY, uint32_t> & LSE_CALL CVertexTriangleAdjacency::List() {
		return m_vVerts;
	}

}	// namespace lsc

#endif	// __LSC_VERTEXTRIANGLEADJACENCY_H__
