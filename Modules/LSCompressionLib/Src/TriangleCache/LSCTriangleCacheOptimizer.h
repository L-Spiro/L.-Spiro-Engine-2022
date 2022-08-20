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
 * Description: Optimizes a triangle-list index buffer for best cache usage.
 */


#ifndef __LSC_TRIANGLECACHEOPTIMIZER_H__
#define __LSC_TRIANGLECACHEOPTIMIZER_H__

#include "../LSCCompressionLib.h"
#include "LSCLiveTriangleList.h"
#include "Set/LSTLSet.h"

namespace lsc {

	/**
	 * Class CTriangleCacheOptimizer
	 * \brief Optimizes a triangle-list index buffer for best cache usage.
	 *
	 * Description: Optimizes a triangle-list index buffer for best cache usage.
	 */
	class CTriangleCacheOptimizer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CTriangleCacheOptimizer();


		// == Functions.
		/**
		 * Optimizes the input vertex buffer for best cache usage.
		 *
		 * \param _pui32Input Input vertex buffer.
		 * \param _ui32Total Number of indices in the input vertex.
		 * \param _pui32Output Output vertex buffer.  Must be able to hold _ui32Total indices.
		 * \return Returns true if there were no memory failures during the optimization.
		 */
		LSBOOL LSE_CALL							Optimize( const uint32_t * _pui32Input, uint32_t _ui32Total, uint32_t * _pui32Output );


	protected :
		// == Members.


		// == Functions.
		/**
		 * Gets the next vertex index from which to begin the next iteration of the main loop.
		 *
		 * \param _ui32Cursor Cursor position.
		 * \param _ui32Cache Cache size.
		 * \param _sCandidates Candidiates.
		 * \param _vTimeStamps Timestamps.
		 * \param _ui32TimeStamp Current timestamp counter.
		 * \param _ltlLiveList List of live triangles.
		 * \param _vDeadEndStack The dead-end stack.
		 * \return Returns the index of the next best starting point for cache usage or LSE_MAXU32.
		 */
		uint32_t LSE_CALL						GetNextVertex( uint32_t &_ui32Cursor, const uint32_t _ui32Cache,
			const CSet<uint32_t> &_sCandidates,
			const CVectorPoD<uint32_t, uint32_t> &_vTimeStamps,
			const uint32_t _ui32TimeStamp, const CLiveTriangleList &_ltlLiveList, CVectorPoD<uint32_t, uint32_t> &_vDeadEndStack );

		/**
		 * Finds a new non-local starting point after consuming all the vertices in cache.
		 *
		 * \param _ltlLiveList List of live triangles.
		 * \param _vDeadEndStack The dead-end stack.
		 * \param _ui32Cursor Cursor position.
		 * \return Returns the best new starting point or LSE_MAXU32.
		 */
		uint32_t LSE_CALL						SkipDeadEnd( const CLiveTriangleList &_ltlLiveList, CVectorPoD<uint32_t, uint32_t> &_vDeadEndStack,
			uint32_t &_ui32Cursor );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsc

#endif	// __LSC_TRIANGLECACHEOPTIMIZER_H__
