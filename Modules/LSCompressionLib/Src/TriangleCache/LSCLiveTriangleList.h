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
 * Description: Maintains a per-vertex list of live triangle counts.  Live triangles are those that have not
 *	yet been output during the cache reordering.
 */


#ifndef __LSC_LIVETRIANGLELIST_H__
#define __LSC_LIVETRIANGLELIST_H__

#include "../LSCCompressionLib.h"
#include "LSCVertexTriangleAdjacency.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsc {

	/**
	 * Class CLiveTriangleList
	 * \brief Maintains a per-vertex list of live triangle counts.
	 *
	 * Description: Maintains a per-vertex list of live triangle counts.  Live triangles are those that have not
	 *	yet been output during the cache reordering.
	 */
	class CLiveTriangleList {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CLiveTriangleList();


		// == Functions.
		/**
		 * Creates the list of live triangles.
		 *
		 * \param _pvtaAdjacency Adjacency list from which to create the live triangle count list.
		 * \return Returns true if there was enough memory to create the list.
		 */
		LSBOOL LSE_CALL							CreateLiveTriangleCountList( CVertexTriangleAdjacency * _pvtaAdjacency );

		/**
		 * Gets the list of live triangles.
		 *
		 * \return Returns the list of live triangles.
		 */
		LSE_INLINE CVectorPoD<uint32_t, uint32_t> & LSE_CALL
												List();

		/**
		 * Gets the list of live triangles.
		 *
		 * \return Returns the list of live triangles.
		 */
		LSE_INLINE const CVectorPoD<uint32_t, uint32_t> & LSE_CALL
												List() const;


	protected :
		// == Members.
		/** The per-vertex count of live triangles. */
		CVectorPoD<uint32_t, uint32_t>			m_vLiveTriangleCount;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the list of live triangles.
	 *
	 * \return Returns the list of live triangles.
	 */
	LSE_INLINE CVectorPoD<uint32_t, uint32_t> & LSE_CALL CLiveTriangleList::List() {
		return m_vLiveTriangleCount;
	}

	/**
	 * Gets the list of live triangles.
	 *
	 * \return Returns the list of live triangles.
	 */
	LSE_INLINE const CVectorPoD<uint32_t, uint32_t> & LSE_CALL CLiveTriangleList::List() const {
		return m_vLiveTriangleCount;
	}

}	// namespace lsc

#endif	// __LSC_LIVETRIANGLELIST_H__
