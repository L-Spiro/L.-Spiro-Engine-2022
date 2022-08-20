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


#ifndef __LSC_STRIPADJACENCY_H__
#define __LSC_STRIPADJACENCY_H__

#include "../LSCCompressionLib.h"
#include "Map/LSTLLinearMap.h"
#include "Search/LSSTDIndexSorter.h"

namespace lsc {

	/**
	 * Class CStripAdjacency
	 * \brief Creates adjacency information for the preparation of the conversion from a triangle list to a
	 *	triangle strip.
	 *
	 * Description: Creates adjacency information for the preparation of the conversion from a triangle list to a
	 *	triangle strip.  Code based on Pierre Terdiman's original work.
	 */
	class CStripAdjacency {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CStripAdjacency();
		LSE_CALLCTOR							~CStripAdjacency();


		// == Types.
		/** Adjacency information.  Indices of triangles adjacent to each other. */
		typedef struct LSC_ADJ_TRI {
			/** Adjacent vertices. */
			uint32_t							ui32Verts[3];

			/** Adjacent triangles. */
			uint32_t							ui32Tris[3];
		} * LPLSC_ADJ_TRI, * const LPCLSC_ADJ_TRI;

		/** An adjacent edge. */
		typedef struct LSC_ADJ_EDGE {
			/** Vertex reference 0. */
			uint32_t							ui32Ref0;

			/** Vertex reference 1. */
			uint32_t							ui32Ref1;

			/** Face number. */
			uint32_t							ui32Face;
		} * LPLSC_ADJ_EDGE, * const LPCLSC_ADJ_EDGE;


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
		LSBOOL LSE_CALL							InitStripAdjacency( uint32_t _ui32Faces = 0UL, const uint32_t * _pui32Faces = NULL, const uint16_t * _pui16Faces = NULL );

		/**
		 * Create the adjency data.
		 *
		 * \return Returns false for non-manifold meshes.  If any edge is shared by more than 2 triangles, this returns false.
		 *	Most meshes are manifold, and non-manifold meshes are still easy to handle without triangle stripping.
		 */
		LSBOOL LSE_CALL							CreateDatabase();

		/**
		 * Reset the object back to default.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets the number of faces.
		 *
		 * \return Returns the number of faces.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalFaces() const;

		/**
		 * Gets the number of edges.
		 *
		 * \return Returns the number of edges.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalEdges() const;

		/**
		 * Gets the pointer to the start of the face array.
		 *
		 * \return Return the pointer to the start of the face array.
		 */
		LSE_INLINE const LSC_ADJ_TRI * LSE_CALL	Faces() const;

		/**
		 * Gets the pointer to the start of the edge array.
		 *
		 * \return Return the pointer to the start of the edge array.
		 */
		LSE_INLINE const LSC_ADJ_EDGE * LSE_CALL
												Edges() const;

		/**
		 * Find an edge on a triangle given two vertex references.
		 *
		 * \param _atAdjInfo Adjacency information.
		 * \param _ui32Ref0 First reference.
		 * \param _ui32Ref1 Second reference.
		 * \return Returns the edge number between 0 and 2 or 0xFF if the references are wrong.
		 */
		static LSE_INLINE uint32_t LSE_CALL		FindEdge( const LSC_ADJ_TRI &_atAdjInfo, uint32_t _ui32Ref0, uint32_t _ui32Ref1 );

		/**
		 * Gets the last vertex reference given the first two.
		 *
		 * \param _atAdjInfo Adjacency information.
		 * \param _ui32Ref0 First reference.
		 * \param _ui32Ref1 Second reference.
		 * \return Returns the last vertex reference given the first two or ~0UL on invalid input.
		 */
		static LSE_INLINE uint32_t LSE_CALL		OppositeVertex( const LSC_ADJ_TRI &_atAdjInfo, uint32_t _ui32Ref0, uint32_t _ui32Ref1 );

	protected :
		// == Members.
		/** Array of triangles. */
		LSC_ADJ_TRI *							m_patFaces;

		/** Array of edges. */
		LSC_ADJ_EDGE *							m_paeEdges;

		/** Total faces. */
		uint32_t								m_ui32Faces;

		/** Total edges. */
		uint32_t								m_ui32Edges;


		// == Functions.
		/**
		 * Adds a triangle to the database.
		 *
		 * \param _ui32Vert0 Vertex 0.
		 * \param _ui32Vert1 Vertex 1.
		 * \param _ui32Vert2 Vertex 2.
		 */
		void LSE_CALL							AddTriangle( uint32_t _ui32Vert0, uint32_t _ui32Vert1, uint32_t _ui32Vert2 );

		/**
		 * Adds an edge to the database.
		 *
		 * \param _ui32Edge0 Edge index 0.
		 * \param _ui32Edge1 Edge index 1.
		 * \param _ui32Face Face index.
		 */
		void LSE_CALL							AddEdge( uint32_t _ui32Edge0, uint32_t _ui32Edge1, uint32_t _ui32Face );

		/**
		 * Update links between two triangles that share the same edge.
		 *
		 * \param _ui32Tri0 First triangle sharing the edge.
		 * \param _ui32Tri1 Second triangle sharing the edge.
		 * \param _ui32Ref0 Common edge index 0.
		 * \param _ui32Ref1 Common edge index 1.
		 * \return Returns false if the input is invalid.
		 */
		LSBOOL LSE_CALL							UpdateLink( uint32_t _ui32Tri0, uint32_t _ui32Tri1, uint32_t _ui32Ref0, uint32_t _ui32Ref1 );

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the number of faces.
	 *
	 * \return Returns the number of faces.
	 */
	 LSE_INLINE uint32_t LSE_CALL CStripAdjacency::TotalFaces() const {
		 return m_ui32Faces;
	 }

	 /**
	 * Gets the number of edges.
	 *
	 * \return Returns the number of edges.
	 */
	 LSE_INLINE uint32_t LSE_CALL CStripAdjacency::TotalEdges() const {
		return m_ui32Edges;
	 }

	 /**
	 * Gets the pointer to the start of the face array.
	 *
	 * \return Return the pointer to the start of the face array.
	 */
	 LSE_INLINE const CStripAdjacency::LSC_ADJ_TRI * LSE_CALL CStripAdjacency::Faces() const {
		return m_patFaces;
	 }

	 /**
	 * Gets the pointer to the start of the edge array.
	 *
	 * \return Return the pointer to the start of the edge array.
	 */
	LSE_INLINE const CStripAdjacency::LSC_ADJ_EDGE * LSE_CALL CStripAdjacency::Edges() const {
		return m_paeEdges;
	}

	/**
	 * Find an edge on a triangle given two vertex references.
	 *
	 * \param _atAdjInfo Adjacency information.
	 * \param _ui32Ref0 First reference.
	 * \param _ui32Ref1 Second reference.
	 * \return Returns the edge number between 0 and 2 or 0xFF if the references are wrong.
	 */
	LSE_INLINE uint32_t LSE_CALL CStripAdjacency::FindEdge( const LSC_ADJ_TRI &_atAdjInfo, uint32_t _ui32Ref0, uint32_t _ui32Ref1 ) {
		uint32_t ui32Edge = 0xFF;
		if ( _atAdjInfo.ui32Verts[0] == _ui32Ref0 && _atAdjInfo.ui32Verts[1] == _ui32Ref1 ) { ui32Edge = 0; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref1 && _atAdjInfo.ui32Verts[1] == _ui32Ref0 ) { ui32Edge = 0; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref0 && _atAdjInfo.ui32Verts[2] == _ui32Ref1 ) { ui32Edge = 1; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref1 && _atAdjInfo.ui32Verts[2] == _ui32Ref0 ) { ui32Edge = 1; }
		else if ( _atAdjInfo.ui32Verts[1] == _ui32Ref0 && _atAdjInfo.ui32Verts[2] == _ui32Ref1 ) { ui32Edge = 2; }
		else if ( _atAdjInfo.ui32Verts[1] == _ui32Ref1 && _atAdjInfo.ui32Verts[2] == _ui32Ref0 ) { ui32Edge = 2; }
		return ui32Edge;
	}

	/**
	 * Gets the last vertex reference given the first two.
	 *
	 * \param _atAdjInfo Adjacency information.
	 * \param _ui32Ref0 First reference.
	 * \param _ui32Ref1 Second reference.
	 * \return Returns the last vertex reference given the first two or ~0UL on invalid input.
	 */
	LSE_INLINE uint32_t LSE_CALL CStripAdjacency::OppositeVertex( const LSC_ADJ_TRI &_atAdjInfo, uint32_t _ui32Ref0, uint32_t _ui32Ref1 ) {
		uint32_t ui32Ref = ~0UL;
		if ( _atAdjInfo.ui32Verts[0] == _ui32Ref0 && _atAdjInfo.ui32Verts[1] == _ui32Ref1 ) { ui32Ref = _atAdjInfo.ui32Verts[2]; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref1 && _atAdjInfo.ui32Verts[1] == _ui32Ref0 ) { ui32Ref = _atAdjInfo.ui32Verts[2]; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref0 && _atAdjInfo.ui32Verts[2] == _ui32Ref1 ) { ui32Ref = _atAdjInfo.ui32Verts[1]; }
		else if ( _atAdjInfo.ui32Verts[0] == _ui32Ref1 && _atAdjInfo.ui32Verts[2] == _ui32Ref0 ) { ui32Ref = _atAdjInfo.ui32Verts[1]; }
		else if ( _atAdjInfo.ui32Verts[1] == _ui32Ref0 && _atAdjInfo.ui32Verts[2] == _ui32Ref1 ) { ui32Ref = _atAdjInfo.ui32Verts[0]; }
		else if ( _atAdjInfo.ui32Verts[1] == _ui32Ref1 && _atAdjInfo.ui32Verts[2] == _ui32Ref0 ) { ui32Ref = _atAdjInfo.ui32Verts[0]; }
		return ui32Ref;
	}

}	// namespace lsc

#endif	// __LSC_STRIPADJACENCY_H__
