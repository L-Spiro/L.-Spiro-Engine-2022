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
 * Description: Creates adjacency information for the preparation of the conversion from a triangle list to a
 *	triangle strip.  Code based on Pierre Terdiman's original work.
 */

#include "LSCStripAdjacency.h"
#include "Vector/LSTLVectorPoD.h"
#include <new>
#include "String/LSTLString.h"


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CStripAdjacency::CStripAdjacency() :
		m_patFaces( NULL ),
		m_ui32Faces( 0UL ),
		m_paeEdges( NULL ),
		m_ui32Edges( 0UL ) {
	}
	LSE_CALLCTOR CStripAdjacency::~CStripAdjacency() {
		LSEDELETE [] m_patFaces;
		LSEDELETE [] m_paeEdges;
	}

	// == Functions.
	/**
	 * Initialize adjacency information.  _pui32Faces and _pui16Faces cannot both be valid.  One or the other
	 *	must be NULL, but not both.
	 *
	 * \param _ui32Faces Number of triangles.
	 * \param _pui32Faces Pointer to the triangles with 32-bit indices or NULL.
	 * \param _pui16Faces Pointer to the triangles with 16-bit indices or NULL.
	 * \return Returns false if there are not enough resources to complete the setup.
	 */
	LSBOOL LSE_CALL CStripAdjacency::InitStripAdjacency( uint32_t _ui32Faces, const uint32_t * _pui32Faces, const uint16_t * _pui16Faces ) {
		m_patFaces = LSENEW LSC_ADJ_TRI[_ui32Faces];
		if ( !m_patFaces ) { return false; }

		m_paeEdges = LSENEW LSC_ADJ_EDGE[_ui32Faces*3UL];
		if ( !m_paeEdges ) { return false; }


		for ( uint32_t I = 0UL; I < _ui32Faces; ++I ) {
			uint32_t ui32Ref0 = _pui32Faces ? _pui32Faces[I*3+0] : _pui16Faces[I*3+0];
			uint32_t ui32Ref1 = _pui32Faces ? _pui32Faces[I*3+1] : _pui16Faces[I*3+1];
			uint32_t ui32Ref2 = _pui32Faces ? _pui32Faces[I*3+2] : _pui16Faces[I*3+2];

			AddTriangle( ui32Ref0, ui32Ref1, ui32Ref2 );
		}
		

		return true;
	}

	/**
	 * Create the adjency data.
	 *
	 * \return Returns false for non-manifold meshes.  If any edge is shared by more than 2 triangles, this returns false.
	 *	Most meshes are manifold, and non-manifold meshes are still easy to handle without triangle stripping.
	 */
	LSBOOL LSE_CALL CStripAdjacency::CreateDatabase() {
		if ( m_ui32Edges < 3UL ) { return true; }	// Nothing to do.  Minimum of 3 edges neeed to make a single triangle.
		if ( !m_paeEdges ) { return true; }			// Already successfully called and ready to be used.


		CVectorPoD<uint32_t, uint32_t> vFaces( m_ui32Edges );
		if ( !vFaces.Resize( m_ui32Edges ) ) { return false; }

		CVectorPoD<uint32_t, uint32_t> vRefs0( m_ui32Edges );
		if ( !vRefs0.Resize( m_ui32Edges ) ) { return false; }

		CVectorPoD<uint32_t, uint32_t> vRefs1( m_ui32Edges );
		if ( !vRefs1.Resize( m_ui32Edges ) ) { return false; }

		for ( uint32_t I = m_ui32Edges; I--; ) {
			vFaces[I] = m_paeEdges[I].ui32Face;
			vRefs0[I] = m_paeEdges[I].ui32Ref0;
			vRefs1[I] = m_paeEdges[I].ui32Ref1;
		}

		CIndexSorter<uint32_t> isSorter;
		// Sort by refs 1, then by refs 0, then by faces.
		const uint32_t * pui32Sorted = isSorter.Sort( &vFaces[0], m_ui32Edges ).Sort( &vRefs0[0], m_ui32Edges ).Sort( &vRefs1[0], m_ui32Edges ).GetIndices();


		uint32_t ui32LastRef0 = vRefs0[pui32Sorted[0]];
		uint32_t ui32LastRef1 = vRefs1[pui32Sorted[0]];
		uint32_t ui32Count = 0UL;
		uint32_t ui32TempBuffer[3];

		uint32_t ui32TotalLinks = 0UL;
		for ( uint32_t I = 0UL; I < m_ui32Edges; ++I ) {
			uint32_t ui32Face = vFaces[pui32Sorted[I]];
			uint32_t ui32Ref0 = vRefs0[pui32Sorted[I]];
			uint32_t ui32Ref1 = vRefs1[pui32Sorted[I]];

			if ( ui32Ref0 == ui32LastRef0 && ui32Ref1 == ui32LastRef1 ) {
				// Current edge is the same as the last one.
				ui32TempBuffer[ui32Count++] = ui32Face;
				if ( ui32Count == 3UL ) {
					// Not manifold.
					return false;
				}
			}
			else {
				// New edge shared by ui32Count stored in ui32TempBuffer.
				if ( ui32Count == 2UL ) {
					// Shared by exactly two triangles (we support no more than this).  Update edge links.
					++ui32TotalLinks;
					if ( !UpdateLink( ui32TempBuffer[0], ui32TempBuffer[1], ui32LastRef0, ui32LastRef1 ) ) { return false; }
				}

				ui32Count = 0UL;
				ui32TempBuffer[ui32Count++] = ui32Face;
				ui32LastRef0 = ui32Ref0;
				ui32LastRef1 = ui32Ref1;
			}
		}

		if ( ui32Count == 2UL ) {
			++ui32TotalLinks;
			if ( !UpdateLink( ui32TempBuffer[0], ui32TempBuffer[1], ui32LastRef0, ui32LastRef1 ) ) { return false; }
		}

		// No longer need the edges.
		LSEDELETE [] m_paeEdges;
		m_paeEdges = NULL;

		return true;
	}

	/**
	 * Reset the object back to default.
	 */
	void LSE_CALL CStripAdjacency::Reset() {
		LSEDELETE [] m_patFaces;
		LSEDELETE [] m_paeEdges;
		m_ui32Faces = m_ui32Edges = 0UL;
	}

	/**
	 * Adds a triangle to the database.
	 *
	 * \param _ui32Vert0 Vertex 0.
	 * \param _ui32Vert1 Vertex 1.
	 * \param _ui32Vert2 Vertex 2.
	 */
	void LSE_CALL CStripAdjacency::AddTriangle( uint32_t _ui32Vert0, uint32_t _ui32Vert1, uint32_t _ui32Vert2 ) {
		m_patFaces[m_ui32Faces].ui32Verts[0] = _ui32Vert0;
		m_patFaces[m_ui32Faces].ui32Verts[1] = _ui32Vert1;
		m_patFaces[m_ui32Faces].ui32Verts[2] = _ui32Vert2;

		m_patFaces[m_ui32Faces].ui32Tris[0]	= ~0UL;
		m_patFaces[m_ui32Faces].ui32Tris[1]	= ~0UL;
		m_patFaces[m_ui32Faces].ui32Tris[2]	= ~0UL;

		// Add edge 01 to database.
		if ( _ui32Vert0 < _ui32Vert1 ) { AddEdge( _ui32Vert0, _ui32Vert1, m_ui32Faces ); }
		else { AddEdge( _ui32Vert1, _ui32Vert0, m_ui32Faces ); }

		// Add edge 02 to database.
		if ( _ui32Vert0 < _ui32Vert2 ) { AddEdge( _ui32Vert0, _ui32Vert2, m_ui32Faces ); }
		else { AddEdge( _ui32Vert2, _ui32Vert0, m_ui32Faces ); }

		// Add edge 12 to database.
		if ( _ui32Vert1 < _ui32Vert2 ) { AddEdge( _ui32Vert1, _ui32Vert2, m_ui32Faces ); }
		else { AddEdge( _ui32Vert2, _ui32Vert1, m_ui32Faces ); }

		m_ui32Faces++;
	}

	/**
	 * Adds an edge to the database.
	 *
	 * \param _ui32Edge0 Edge index 0.
	 * \param _ui32Edge1 Edge index 1.
	 * \param _ui32Face Face index.
	 */
	void LSE_CALL CStripAdjacency::AddEdge( uint32_t _ui32Edge0, uint32_t _ui32Edge1, uint32_t _ui32Face ) {
		m_paeEdges[m_ui32Edges].ui32Ref0	= _ui32Edge0;
		m_paeEdges[m_ui32Edges].ui32Ref1	= _ui32Edge1;
		m_paeEdges[m_ui32Edges].ui32Face	= _ui32Face;
		m_ui32Edges++;
	}

	/**
	 * Update links between two triangles that share the same edge.
	 *
	 * \param _ui32Tri0 First triangle sharing the edge.
	 * \param _ui32Tri1 Second triangle sharing the edge.
	 * \param _ui32Ref0 Common edge index 0.
	 * \param _ui32Ref1 Common edge index 1.
	 * \return Returns false if the input is invalid.
	 */
	LSBOOL LSE_CALL CStripAdjacency::UpdateLink( uint32_t _ui32Tri0, uint32_t _ui32Tri1, uint32_t _ui32Ref0, uint32_t _ui32Ref1 ) {
		//return true;
		LSC_ADJ_TRI * patTri0 = &m_patFaces[_ui32Tri0];
		LSC_ADJ_TRI * patTri1 = &m_patFaces[_ui32Tri1];

		// Edge ID where 0xFF means invalid.
		uint8_t ui8Edge0 = static_cast<uint8_t>(FindEdge( (*patTri0), _ui32Ref0, _ui32Ref1 ));
		uint8_t ui8Edge1 = static_cast<uint8_t>(FindEdge( (*patTri1), _ui32Ref0, _ui32Ref1 ));
		if ( ui8Edge0 == 0xFF || ui8Edge1 == 0xFF ) { return false; }

		patTri0->ui32Tris[ui8Edge0] = _ui32Tri1 | (static_cast<uint32_t>(ui8Edge1) << 30UL);
		patTri1->ui32Tris[ui8Edge1] = _ui32Tri0 | (static_cast<uint32_t>(ui8Edge0) << 30UL);
		return true;
	}

}	// namespace lsc
