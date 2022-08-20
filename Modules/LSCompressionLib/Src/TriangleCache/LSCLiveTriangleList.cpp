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

#include "LSCLiveTriangleList.h"


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CLiveTriangleList::CLiveTriangleList() {
	}

	// == Functions.
	/**
	 * Creates the list of live triangles.
	 *
	 * \param _pvtaAdjacency Adjacency list from which to create the live triangle count list.
	 * \return Returns true if there was enough memory to create the list.
	 */
	LSBOOL LSE_CALL CLiveTriangleList::CreateLiveTriangleCountList( CVertexTriangleAdjacency * _pvtaAdjacency ) {
		if ( !m_vLiveTriangleCount.Resize( _pvtaAdjacency->List().Length() ) ) { return false; }
		for ( uint32_t I = m_vLiveTriangleCount.Length(); I--; ) {
			m_vLiveTriangleCount[I] = _pvtaAdjacency->List()[I].ui32Total;
		}
		return true;
	}

}	// namespace lsc
