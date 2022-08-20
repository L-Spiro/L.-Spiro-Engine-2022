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

#include "LSCTriangleCacheOptimizer.h"
#include "LSCVertexTriangleAdjacency.h"


namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CTriangleCacheOptimizer::CTriangleCacheOptimizer() {
	}

	// == Functions.
	/**
	 * Optimizes the input vertex buffer for best cache usage.
	 *
	 * \param _pui32Input Input vertex buffer.
	 * \param _ui32Total Number of indices in the input vertex.
	 * \param _pui32Output Output vertex buffer.  Must be able to hold _ui32Total indices.
	 * \return Returns true if there were no memory failures during the optimization.
	 */
	LSBOOL LSE_CALL CTriangleCacheOptimizer::Optimize( const uint32_t * _pui32Input, uint32_t _ui32Total, uint32_t * _pui32Output ) {
		const uint32_t ui32Cache = 12;						// k.
		CVertexTriangleAdjacency vtaAdjacency;				// A.
		if ( !vtaAdjacency.BuildList( _pui32Input, _ui32Total ) ) { return false; }
		CLiveTriangleList ltlLiveList;						// L.
		if ( !ltlLiveList.CreateLiveTriangleCountList( &vtaAdjacency ) ) { return false; }

		// Time stamp.
		uint32_t ui32TimeStamp = ui32Cache + 1;				// s.
		CVectorPoD<uint32_t, uint32_t> vTimeStamps;			// C.
		if ( !vTimeStamps.Resize( vtaAdjacency.List().Length() ) ) { return false; }
		for ( uint32_t I = vTimeStamps.Length(); I--; ) {
			vTimeStamps[I] = 0;
		}

		// Dead-end stack.
		CVectorPoD<uint32_t, uint32_t> vDeadEndStack;		// D.
		vDeadEndStack.Allocate( _ui32Total );

		// Emitted triangles.
		CVectorPoD<uint8_t, uint32_t> vEmittedTriangles;	// E.
		if ( !vEmittedTriangles.Resize( _ui32Total / 3 ) ) { return false; }
		for ( uint32_t I = vEmittedTriangles.Length(); I--; ) {
			vEmittedTriangles[I] = 0;
		}


		// Current index.
		uint32_t ui32Cursor = 1;							// i.

		// Fan.
		uint32_t ui32Fan = 0;								// f.

		uint32_t ui32Out = 0;

		CSet<uint32_t> sCandidates;							// N.

		sCandidates.AllocateAtLeast( _ui32Total >> 3 );
		while ( ui32Fan != LSE_MAXU32 ) {
			sCandidates.ResetNoDealloc();

			// For each adjacent triangle on this vertex.
			for ( uint32_t I = 0; I < vtaAdjacency.List()[ui32Fan].ui32Total; ++I ) {
				uint32_t ui32T = vtaAdjacency.List()[ui32Fan].pui32Tris[I];
				// If the triangle has not been emitted.
				if ( !vEmittedTriangles[ui32T] ) {
					// For each vertex on the triangle.
					for ( uint32_t J = 0; J < 3; ++J ) {
						uint32_t ui32V = _pui32Input[ui32T*3+J];
						// Output the vertex.
						_pui32Output[ui32Out++] = ui32V;
						// Add to the dead-end stack.
						if ( !vDeadEndStack.Push( ui32V ) ) { return false; }
						// Add to the candidiates stack.
						if ( !sCandidates.Insert( ui32V ) ) { return false; }
						// Decrease live triangle count.
						assert( ltlLiveList.List()[ui32V] != 0 );
						--ltlLiveList.List()[ui32V];

						if ( ui32TimeStamp - vTimeStamps[ui32V] > ui32Cache ) {
							vTimeStamps[ui32V] = ui32TimeStamp;
							++ui32TimeStamp;
						}
					}

					vEmittedTriangles[ui32T] = 1;
				}
			}

			ui32Fan = GetNextVertex( ui32Cursor, ui32Cache,
				sCandidates, vTimeStamps, ui32TimeStamp, ltlLiveList, vDeadEndStack );
		}

		assert( ui32Out == _ui32Total );

		return true;
	}

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
	uint32_t LSE_CALL CTriangleCacheOptimizer::GetNextVertex( uint32_t &_ui32Cursor, const uint32_t _ui32Cache,
		const CSet<uint32_t> &_sCandidates,
		const CVectorPoD<uint32_t, uint32_t> &_vTimeStamps,
		const uint32_t _ui32TimeStamp, const CLiveTriangleList &_ltlLiveList, CVectorPoD<uint32_t, uint32_t> &_vDeadEndStack ) {
		uint32_t ui32N = LSE_MAXU32, ui32P = LSE_MAXU32, ui32M = LSE_MAXU32;

		for ( uint32_t I = 0; I < _sCandidates.Length(); ++I ) {
			uint32_t ui32V = _sCandidates.GetByIndex( I );
			// Must have live triangles.
			if ( _ltlLiveList.List()[ui32V] ) {
				ui32P = 0;
				uint32_t ui32ThisTime = _ui32TimeStamp - _vTimeStamps[ui32V];
				if ( ui32ThisTime + (_ltlLiveList.List()[ui32V] << 1) <= _ui32Cache ) {
					ui32P = ui32ThisTime;
				}
				if ( ui32P > ui32M || ui32M == LSE_MAXU32 ) {
					ui32M = ui32P;
					ui32N = ui32V;
				}
			}
		}

		if ( ui32N == LSE_MAXU32 ) {
			ui32N = SkipDeadEnd( _ltlLiveList, _vDeadEndStack, _ui32Cursor );
		}
		return ui32N;
	}

	/**
	 * Finds a new non-local starting point after consuming all the vertices in cache.
	 *
	 * \param _ltlLiveList List of live triangles.
	 * \param _vDeadEndStack The dead-end stack.
	 * \param _ui32Cursor Cursor position.
	 * \return Returns the best new starting point or LSE_MAXU32.
	 */
	uint32_t LSE_CALL CTriangleCacheOptimizer::SkipDeadEnd( const CLiveTriangleList &_ltlLiveList, CVectorPoD<uint32_t, uint32_t> &_vDeadEndStack,
		uint32_t &_ui32Cursor ) {
		while ( _vDeadEndStack.Length() ) {
			uint32_t ui32D = _vDeadEndStack[_vDeadEndStack.Length()-1];
			_vDeadEndStack.PopNoDealloc();
			// If there are live triangles, just return.
			if ( _ltlLiveList.List()[ui32D] ) { return ui32D; }
		}

		while ( _ui32Cursor < _ltlLiveList.List().Length() ) {
			if ( _ltlLiveList.List()[_ui32Cursor] ) { return _ui32Cursor; }
			++_ui32Cursor;
		}
		return LSE_MAXU32;
	}

}	// namespace lsc
