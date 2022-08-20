/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A template allowing to create polygons with a variable number of edges and holes.  Unlike with
 *	CPolygon3Base, this can have a growable number of edges for a limited amount of time.  Once a different
 *	object has made an allocation on the dynamic stack allocator, this polygon must not increase its edge count.
 */


#ifndef __LSP_COMPLEXPOLYGON3BASE_H__
#define __LSP_COMPLEXPOLYGON3BASE_H__

#include "../LSPPhysicsLib.h"
#include "Allocators/LSADynamicStackAllocator.h"

#include <cassert>

namespace lsp {

	/**
	 * Class CComplexPolygon3Base
	 * \brief A template allowing to create polygons with a variable number of edges and holes.
	 *
	 * Description: A template allowing to create polygons with a variable number of edges and holes.  Unlike with
	 *	CPolygon3Base, this can have a growable number of edges for a limited amount of time.  Once a different
	 *	object has made an allocation on the dynamic stack allocator, this polygon must not increase its edge count.
	 */
	template <typename _tType, typename _tSeg3Type, typename _tPlane3Type>
	class CComplexPolygon3Base {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CComplexPolygon3Base() {
		}
		LSE_CALLCTOR							~CComplexPolygon3Base() {
		}


		// == Functions.


	protected :
		// == Members.
	};
	

}	// namespace lsp

#endif	// __LSP_COMPLEXPOLYGON3BASE_H__
