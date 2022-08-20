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
 * Description: Like a CPolygon3Base, except that segments can be added and removed rather than having to all
 *	be added up-front.
 */


#ifndef __LSP_DYNAMICPOLYGONBASE_H__
#define __LSP_DYNAMICPOLYGONBASE_H__

#include "../LSPPhysicsLib.h"
#include "Allocators/LSADynamicStackAllocator.h"

namespace lsp {

	/**
	 * Class CDynamicPolygon3Base
	 * \brief Like a CPolygon3Base, except that segments can be added and removed rather than having to all
	 *	be added up-front.
	 *
	 * Description: Like a CPolygon3Base, except that segments can be added and removed rather than having to all
	 *	be added up-front.
	 */
	template <typename _tType, typename _tSeg3Type, typename _tPlane3Type>
	class CDynamicPolygon3Base {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDynamicPolygon3Base() :
			m_ptSegs( NULL ),
			m_pdsaAllocator( NULL ),
			m_ui32Total( 0 ),
			m_bOpen( false ),
			m_uiptrUser( 0 ) {
		}


		// == Functions.
		/**
		 * Begins the polygon.  Segments can be added later.  After calling this, the polygon will
		 *	have 0 segments.
		 *
		 * \param _pdaAllocator The allocator to use for adding segments later.
		 */
		void LSE_CALL							Begin( CDynamicStackAllocator * _pdaAllocator ) {
			Reset();
			m_pdsaAllocator = _pdaAllocator;
			m_bOpen = m_pdsaAllocator != NULL;
		}

		/**
		 * Adds a single line segment.
		 *
		 * \param _tSeg The segment to add.
		 * \return Returns true if the segment was added.
		 */
		LSBOOL LSE_CALL							AddSegment( const _tSeg3Type &_tSeg ) {
			if ( m_bOpen ) {
				_tSeg3Type * ptNew = static_cast<_tSeg3Type *>(m_pdsaAllocator->ReAlloc( sizeof( _tSeg3Type ) * (m_ui32Total + 1), __alignof( _tSeg3Type ) ));
				if ( ptNew ) {
					m_ptSegs = ptNew;
					//new( &m_ptSegs[m_ui32Total] ) _tSeg3Type();
					m_ptSegs[m_ui32Total] = _tSeg;
					++m_ui32Total;
					return true;
				}
			}
			return false;
		}

		/**
		 * Adds the given number of segments.
		 *
		 * \param _ptSegs The pointer to the segments to add.
		 * \param _ui32Total The number of segments to add.
		 * \return Returns true if all segments were added.
		 */
		LSBOOL LSE_CALL							AddSegments( const _tSeg3Type * _ptSegs, uint32_t _ui32Total ) {
			for ( uint32_t I = 0; I < _ui32Total; ++I ) {
				if ( !AddSegment( _ptSegs++ ) ) { return false; }
			}
			return true;
		}

		/**
		 * Removes a line segment at the given index.
		 *
		 * \param _ui32Index Index of the line segment to remove.
		 */
		void LSE_CALL							Remove( uint32_t _ui32Index ) {
			if ( m_bOpen ) {
				if ( _ui32Index < m_ui32Total ) {
					//m_ptSegs[_ui32Index].~_tSeg3Type();		// Destroy, don't delete.
					for ( uint32_t J = _ui32Index + 1; J < m_ui32Total; ++J ) {
						m_ptSegs[J-1] = m_ptSegs[J];
					}
					--m_ui32Total;
					// Allocating down always succeeds.
					m_ptSegs = static_cast<_tSeg3Type *>(m_pdsaAllocator->ReAlloc( sizeof( _tSeg3Type ) * m_ui32Total, __alignof( _tSeg3Type ) ));
				}
			}
		}

		/**
		 * Removes all segments.
		 */
		void LSE_CALL							RemoveAll() {
			if ( m_bOpen ) {
				/*for ( uint32_t I = 0; I < m_ui32Total; ++I ) {
					m_ptSegs[I].~_tSeg3Type();		// Destroy, don't delete.
				}*/
				m_ui32Total = 0;
				m_pdsaAllocator->Free();
				m_ptSegs = NULL;
			}
		}

		/**
		 * Inserts the given segment(s) into the polygon at the given index.
		 *
		 * \param _ptSegs The segments to insert.
		 * \param _ui32Total Total segments to insert.
		 * \param _ui32Index Index of the insertion.
		 * \return Returns true if all segments could be inserted.
		 */
		LSBOOL LSE_CALL							Insert( const _tSeg3Type * _ptSegs, uint32_t _ui32Total, uint32_t _ui32Index ) {
			if ( m_bOpen ) {
				_tSeg3Type * ptNew = static_cast<_tSeg3Type *>(m_pdsaAllocator->ReAlloc( sizeof( _tSeg3Type ) * (m_ui32Total + _ui32Total), __alignof( _tSeg3Type ) ));
				if ( ptNew ) {
					m_ptSegs = ptNew;
					m_ui32Total += _ui32Total;
					// Move them back.
					for ( uint32_t I = m_ui32Total - 1; I > _ui32Index; --I ) {
						m_ptSegs[I] = m_ptSegs[I-1];
					}
					// Insert.
					_ui32Total += _ui32Index;
					for ( uint32_t I = _ui32Index; I < _ui32Total; ++I ) {
						m_ptSegs[I] = (*_ptSegs++);
					}
					return true;
				}
			}
			return false;
		}

		/**
		 * Closes the polygon.  After calling this, segments cannot be added or removed.
		 */
		void LSE_CALL							Close() {
			if ( m_bOpen ) {
				if ( m_pdsaAllocator ) {
					m_pdsaAllocator->Done();
				}
				m_bOpen = false;
			}
		}

		/**
		 * Resets the polygon back to scratch.  If the polygon is open, segments are freed from its
		 *	allocator.
		 */
		void LSE_CALL							Reset() {
			for ( ; m_ui32Total--; ) {
				m_ptSegs[m_ui32Total].~_tSeg3Type();	// Destroy, don't delete.
			}
			m_ptSegs = NULL;
			m_ui32Total = 0;
			
			if ( m_bOpen ) {
				m_pdsaAllocator->Free();
			}
			m_bOpen = false;
			m_pdsaAllocator = NULL;
			m_uiptrUser = 0;
		}

		/**
		 * Copies the source polygon into this one.
		 *
		 * \param _p3bSrc The polygon to copy into this one.
		 * \param _psaAllocator The stack allocator to be used for the line segments used by this polygon.
		 * \return Returns true if the segments could be allocated from the given source polygon.
		 */
		LSBOOL LSE_CALL							Copy( const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, CDynamicStackAllocator * _pdaAllocator ) {
			Begin( _pdaAllocator );
			for ( uint32_t I = 0; I < _p3bSrc.TotalSegments(); ++I ) {
				if ( !AddSegment( _p3bSrc.Segments()[I] ) ) { return false; }
			}
			m_pPlane = _p3bSrc.Plane();
			m_uiptrUser = _p3bSrc.GetUser();
			if ( !_p3bSrc.m_bOpen ) {
				Close();
			}
			return true;
		}

		/**
		 * Copies a range of segments into the end of this polygon.
		 *
		 * \param _p3bSrc The polygon whose segments are to be copied.
		 * \param _ui32Start Index from where to begin the copy.
		 * \param _ui32Total Total segments to copy.
		 * \return Returns true if the segments could be copied.
		 */
		LSBOOL LSE_CALL							Copy( const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, uint32_t _ui32Start, uint32_t _ui32Total ) {
			_ui32Total += _ui32Start;
			for ( uint32_t I = _ui32Start; I < _ui32Total; ++I ) {
				if ( !AddSegment( _p3bSrc.Segments()[I] ) ) { return false; }
			}
			return true;
		}

		/**
		 * Gets the total segments on this polygon.
		 *
		 * \return Returns the total segments on this polygon.
		 */
		uint32_t LSE_CALL						TotalSegments() const {
			return m_ui32Total;
		}

		/**
		 * Gets a pointer to the array of segments on this polygon.
		 *
		 * \return Returns a pointer to the array of segments on this polygon, which may be NULL.
		 */
		_tSeg3Type * LSE_CALL					Segments() {
			return m_ptSegs;
		}

		/**
		 * Gets a pointer to the array of segments on this polygon.
		 *
		 * \return Returns a pointer to the array of segments on this polygon, which may be NULL.
		 */
		const _tSeg3Type * LSE_CALL				Segments() const {
			return m_ptSegs;
		}

		/**
		 * Gets the plane for read-only.
		 *
		 * \return Returns the plane for read-only.
		 */
		const _tPlane3Type & LSE_CALL			Plane() const {
			return m_pPlane;
		}

		/**
		 * Gets the plane for read and write.
		 *
		 * \return Returns the plane for read and write.
		 */
		_tPlane3Type & LSE_CALL					Plane() {
			return m_pPlane;
		}

		/**
		 * Sets the user value for the polygon.
		 *
		 * \param _uiptrValue The value to set.
		 */
		void LSE_CALL							SetUser( uintptr_t _uiptrValue ) {
			m_uiptrUser = _uiptrValue;
		}

		/**
		 * Gets the user value.
		 *
		 * \return Returns the user value.
		 */
		uintptr_t LSE_CALL						GetUser() const {
			return m_uiptrUser;
		}

		/**
		 * Removes any segments that are not connected to the base (first) segment.
		 *
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the polygon is still valid after removing unconnected segments.
		 */
		LSBOOL LSE_CALL							RemoveUnconnectedSegments( _tType _tEpsilon ) {
			for ( uint32_t I = 0; I < m_ui32Total; ) {
				// Check P against Q.
				LSBOOL bHasP = false, bHasQ = false;
				if ( !PointsTouch( m_ptSegs[I].q, m_ptSegs[I].p, _tEpsilon ) ) {
					for ( uint32_t J = 0; J < m_ui32Total; ++J ) {
						if ( J == I ) { continue; }
						if ( PointsTouch( m_ptSegs[I].p, m_ptSegs[J].q, _tEpsilon ) ) {
							bHasP = true;
							break;
						}
					}
					if ( bHasP ) {
						// Check its Q.
						for ( uint32_t J = 0; J < m_ui32Total; ++J ) {
							if ( J == I ) { continue; }
							if ( PointsTouch( m_ptSegs[I].q, m_ptSegs[J].p, _tEpsilon ) ) {
								bHasQ = true;
								break;
							}
						}
					}
				}

				if ( !bHasP || !bHasQ ) {
					Remove( I );
					if ( m_ui32Total <= 2 ) {
						return false;
					}
				}
				else {
					++I;
				}
			}
			return true;
		}

		/**
		 * Removes any duplicate segments.
		 *
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the polygon is still valid after removal.
		 */
		LSBOOL LSE_CALL							RemoveDuplicateSegments( _tType _tEpsilon ) {
			for ( uint32_t I = 0; I < m_ui32Total; ++I ) {
				for ( uint32_t J = I + 1; J < m_ui32Total; ++J ) {
					if ( PointsTouch( m_ptSegs[I].p, m_ptSegs[J].p, _tEpsilon ) &&
						PointsTouch( m_ptSegs[I].q, m_ptSegs[J].q, _tEpsilon ) ) {
						Remove( J );
					}
				}
			}
			return m_ui32Total >= 3;
		}

		/**
		 * Removes any segments that share the same points but in opposite directions.
		 *
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the polygon is still valid after removal.
		 */
		LSBOOL LSE_CALL							RemoveRedundantSegments( _tType _tEpsilon ) {
			LSBOOL bRemoveMe;
			for ( uint32_t I = m_ui32Total; I--; ) {
				bRemoveMe = false;
				if ( PointsTouch( m_ptSegs[I].p, m_ptSegs[I].q, _tEpsilon ) ) {
					// Segment is 0-sized.
					bRemoveMe = true;
				}
				else {
					for ( uint32_t J = I + 1; J < m_ui32Total; ++J ) {
						if ( PointsTouch( m_ptSegs[I].p, m_ptSegs[J].q, _tEpsilon ) &&
							PointsTouch( m_ptSegs[I].q, m_ptSegs[J].p, _tEpsilon ) ) {
							Remove( J );
							bRemoveMe = true;
						}
					}
				}
				if ( bRemoveMe ) {
					Remove( I );
				}
			}
			return m_ui32Total >= 3;
		}

		/**
		 * Determines if the polygon has any overlaps.
		 *
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the polygon overlaps itself anywhere.
		 */
		LSBOOL LSE_CALL							HasOverlaps( _tType _tEpsilon ) const {
			// For each segment.
			for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
				// For each other segment 2 ahead of this one.
				for ( uint32_t J = 2; J < TotalSegments(); ++J ) {
					uint32_t ui32Index = (I + J) % TotalSegments();
					_tType tS, tT, tRealS, tRealT;
					auto v0 = Segments()[I].p;
					auto v1 = Segments()[I].p;
					_tType tDist = ClosestPointOnLineSegToLineSeg( Segments()[I], Segments()[ui32Index], tS, tT, tRealS, tRealT, v0, v1, _tEpsilon );
					if ( tS < _tType( 1.0 ) && tS > _tType( 0.0 ) &&
						tT < _tType( 1.0 ) && tT > _tType( 0.0 ) ) {
						// They are overlapping.
						return true;
					}
				}
			}
			return false;
		}

		/**
		 * Determines if any of the given segments overlap any segments in this polygon.
		 *
		 * \param _ptSeg The segments to check.
		 * \param _ui32Total Total segments to which _ptSeg points.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if any of the given segments overlap any segments on this polygon.
		 */
		LSBOOL LSE_CALL							AnyOverlap( const _tSeg3Type * _ptSeg, uint32_t _ui32Total, _tType _tEpsilon ) const {
			// For each segment.
			for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
				for ( uint32_t J = 0; J < _ui32Total; ++J ) {
					_tType tS, tT;
					_tType tDist = ClosestPointOnLineSegToLineSeg( Segments()[I], _ptSeg[J], tS, tT, _tEpsilon );
					if ( tS < _tType( 1.0 ) && tS > _tType( 0.0 ) &&
						tT < _tType( 1.0 ) && tT > _tType( 0.0 ) ) {
						// They are overlapping.
						return true;
					}
				}
			}
			return false;
		}

		/**
		 * Finalizes the polygon such that the end of each segment connects to the start of another segment.
		 *	If false is returned, at least one segment was unable to be connected.
		 *
		 * \param _tEpsilon The epsilon for snapping points together.
		 * \param _dsaScratchAllocator A scratch allocator used for handling any temporary work this polygon needs to perform
		 *	during finalization.  If there are holes in the polygon, a copy will be made on the scratch heap etc.
		 * \return Returns true if finalization was successful.  This means that all segments are connected, there is only
		 *	one path through the segments, and the path ends where it begins.
		 */
		LSBOOL LSE_CALL							Finalize( _tType _tEpsilon, CDynamicStackAllocator &_dsaScratchAllocator ) {
			assert( m_ui32Total >= 3 );
			// Remove any segments that have any missing connections.
			if ( !RemoveUnconnectedSegments( _tEpsilon ) ) { return false; }
			if ( !RemoveDuplicateSegments( _tEpsilon ) ) { return false; }
			if ( !RemoveRedundantSegments( _tEpsilon ) ) { return false; }
			uint32_t ui32Start = 0;
			uint32_t ui32Paths = 0;
			while ( ui32Start < m_ui32Total ) {
				uint32_t ui32TotalConnected = ConnectEdges( ui32Start, _tEpsilon );
				ui32Start += ui32TotalConnected;
				++ui32Paths;
			}
			if ( ui32Paths > 1 ) {
				// There are holes.
				LSP_HOLE_TABLE * phtTable = reinterpret_cast<LSP_HOLE_TABLE *>(_dsaScratchAllocator.Alloc( sizeof( LSP_HOLE_TABLE ) * ui32Paths, __alignof( LSP_HOLE_TABLE ) ));
				LSP_HOLE_TABLE * phtTemp = phtTable;
				ui32Start = 0;
				while ( ui32Start < m_ui32Total ) {
					uint32_t ui32TotalConnected = CountEdges( ui32Start, _tEpsilon );
					phtTemp->ui32Start = ui32Start;
					phtTemp->ui32Total = ui32TotalConnected;
					phtTemp->bCopied = false;
					ui32Start += ui32TotalConnected;
					++phtTemp;
				}
				// Make sure the temorary polygon does not overwrite the table.
				_dsaScratchAllocator.Done();
				
				CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> dpbTemp;
				if ( !dpbTemp.Copy( (*this), &_dsaScratchAllocator ) ) {
					_dsaScratchAllocator.Free();
					return false;
				}

				// Copy successfully made.  Remove all of our segments and get ready to copy them back.
				RemoveAll();

				// Automatically copy the first segments back.
				if ( !Copy( dpbTemp, &phtTable[0] ) ) {
					_dsaScratchAllocator.Free();
					return false;
				}

				uint32_t ui32Local, ui32Other;
				uint32_t ui32Best = FindBestSegmentToMerge( dpbTemp, phtTable, ui32Paths, ui32Local, ui32Other, _tEpsilon );
				while ( ui32Best != LSE_MAXU32 ) {
					// Add the connectors.
					_tSeg3Type tSeg0 = _tSeg3Type( Segments()[ui32Local], dpbTemp.Segments()[ui32Other] );
					_tSeg3Type tSeg1 = _tSeg3Type( dpbTemp.Segments()[ui32Other], Segments()[ui32Local] );
					if ( !PointsTouch( tSeg0.p, tSeg0.q, _tEpsilon ) ) {
						if ( !Insert( &tSeg0, 1, ui32Local ) ) {
							_dsaScratchAllocator.Free();
							return false;
						}
						// Add everything else after this.
						++ui32Local;

						if ( !Insert( &tSeg1, 1, ui32Local ) ) {
							_dsaScratchAllocator.Free();
							return false;
						}
					}
					if ( !InsertSectionWithOffset( dpbTemp, &phtTable[ui32Best], ui32Local, ui32Other ) ) {
						_dsaScratchAllocator.Free();
						return false;
					}
					

					ui32Best = FindBestSegmentToMerge( dpbTemp, phtTable, ui32Paths, ui32Local, ui32Other, _tEpsilon );
				}
			}
			_dsaScratchAllocator.Free();
			return PointsTouch( m_ptSegs[0].p, m_ptSegs[m_ui32Total-1].q, _tEpsilon );
		}

		/**
		 * Gets the list of points as an array.
		 *
		 * \param _pdsaAllocator Used to allocate the returned array of points.  The number of points will be equal to TotalSegments().
		 */
		template <typename _tVec3Type>
		_tVec3Type * LSE_CALL					GetPoints( CDynamicStackAllocator * _pdsaAllocator ) {
			_tVec3Type * ptRet = reinterpret_cast<_tVec3Type *>(_pdsaAllocator->Alloc( TotalSegments() * sizeof( _tVec3Type ), __alignof( _tVec3Type ) ));
			if ( ptRet ) {
				_tVec3Type * ptDst = ptRet;
				for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
					(*ptDst++) = _tVec3Type( Segments()[I].p.x, Segments()[I].p.y, Segments()[I].p.z );
				}
			}
			return ptRet;
		}

		/**
		 * Determines if the given segment already exists in the polygon.
		 *
		 * \param _tSeg The segment to check for its existance in the polygon.
		 * \return Returns the index of the given segment on the polygon or LSE_MAXU32 if it does not exist.
		 */
		uint32_t LSE_CALL						HasSegment( const _tSeg3Type &_tSeg ) const {
			for ( uint32_t I = TotalSegments(); I--; ) {
				if ( Segments()[I].p == _tSeg.p && Segments()[I].q == _tSeg.q ) { return I; }
			}
			return LSE_MAXU32;
		}

		/**
		 * Determines if a given point is in the polygon.
		 *
		 * \param _tV0 The point to check for being in the polygon.
		 * \return Returns true if _tV0 is in the polygon.
		 */
		template <typename _tVec3Type>
		LSBOOL LSE_CALL							HasPoint( const _tVec3Type &_tV0 ) const {
			for ( uint32_t I = TotalSegments(); I--; ) {
				if ( Segments()[I].p == _tV0 || Segments()[I].q == _tV0 ) { return true; }
			}
			return false;
		}

		/**
		 * Determines how many of the given points exist in the polygon
		 *
		 * \param _ptV0 Pointer to the points to check.
		 * \param _ui32Total Total points to which _ptV0 points.
		 * \return Returns the number of points in _ptV0 that exist in the polygon.
		 */
		template <typename _tVec3Type>
		uint32_t LSE_CALL						HasPoints( const _tVec3Type * _ptV0, uint32_t _ui32Total ) const {
			uint32_t ui32Ret = 0;
			for ( uint32_t I = _ui32Total; I--; ) {
				if ( HasPoint( _ptV0[I] ) ) {
					++ui32Ret;
				}
			}
			return ui32Ret;
		}

		/**
		 * Counts how many line segments touch the given point.
		 *
		 * \param _tV0 The point to check.
		 * \return Returns the number of segments that touch the given point.
		 */
		template <typename _tVec3Type>
		uint32_t LSE_CALL						CountTouches( const _tVec3Type &_tV0 ) const {
			uint32_t ui32Ret = 0;
			for ( uint32_t I = TotalSegments(); I--; ) {
				if ( Segments()[I].p == _tV0 ) { ++ui32Ret; }
				if ( Segments()[I].q == _tV0 ) { ++ui32Ret; }
			}
			return ui32Ret;
		}

		/**
		 * Gets the index of the first segment in the polygon whose starting point matches the given point.
		 *
		 * \param _tV0 The point to check
		 * \return Returns the index of the first segment in the polygon whose starting point matches the given point.
		 *	Returns LSE_MAXU32 if no such point exists.
		 */
		template <typename _tVec3Type>
		uint32_t LSE_CALL						HasStartingPoint( const _tVec3Type &_tV0 ) const {
			for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
				if ( Segments()[I].p == _tV0 ) { return I; }
			}
			return LSE_MAXU32;
		}

		/**
		 * Gets the index of the segment in the polygon that has the given starting point and most closely follows the given
		 *	direction.
		 *
		 * \param _tV0 The point to check.
		 * \param _tDir The preferred direction.
		 * \return Returns the index of the first segment in the polygon whose starting point matches the given point.
		 *	Returns LSE_MAXU32 if no such point exists.
		 */
		template <typename _tVec3Type>
		uint32_t LSE_CALL						HasStartingPointWithDirection( const _tVec3Type &_tV0, const _tVec3Type &_tDir ) const {
			_tType tDir = _tType( -20.0 );
			uint32_t ui32Winner = LSE_MAXU32;
			for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
				if ( Segments()[I].p == _tV0 ) {
					_tVec3Type vThis = Segments()[I].q - Segments()[I].p;
					vThis.Normalize();
					_tType tThisDir = _tDir.Dot( vThis );
					if ( tThisDir > tDir ) {
						tDir = tThisDir;
						ui32Winner = I;
					}
				}
			}
			return ui32Winner;
		}


	protected :
		// == Types.
		/** A temporary table of paths in a polygon. */
		typedef struct LSP_HOLE_TABLE {
			/** Start of the segments. */
			uint32_t							ui32Start;

			/** Total segments in this set. */
			uint32_t							ui32Total;

			/** If it has been copied back into the original or not. */
			LSBOOL								bCopied;
		} * LPLSP_HOLE_TABLE, * const LPCLSP_HOLE_TABLE;


		// == Members.
		/** Pointer to the array of line segments. */
		_tSeg3Type *							m_ptSegs;

		/** The stack allocator used by this object. */
		CDynamicStackAllocator *				m_pdsaAllocator;

		/** Special meaning. */
		uintptr_t								m_uiptrUser;

		/** The plane for this polygon. */
		_tPlane3Type							m_pPlane;

		/** Total line segments. */
		uint32_t								m_ui32Total;		

		/** If the polygon is still open or not. */
		LSBOOL									m_bOpen;


		// == Functions.
		/**
		 * Moves edges so that each edge connects to the one before and after it.  Works until the series of edges has made
		 *	a full circle.
		 *
		 * \param _ui32StartEdge The edge from where to start connecting.
		 * \param _tEpsilon The epsilon for snapping points together.
		 * \return Returns the number of edges connected to make a fully enclosed set of edges such that the last edge connects back
		 *	to the first edge.
		 */
		uint32_t LSE_CALL						ConnectEdges( uint32_t _ui32StartEdge, _tType _tEpsilon ) {
			uint32_t I;
			for ( I = _ui32StartEdge; I < m_ui32Total; ++I ) {
				// Find the starting point that connects to the end point of the Ith edge.
				for ( uint32_t J = I + 1; J < m_ui32Total; ++J ) {
					if ( PointsTouch( m_ptSegs[J].p, m_ptSegs[I].q, _tEpsilon ) ) {
						if ( J != I + 1 ) {	// No need to swap here.
							CStd::Swap( m_ptSegs[I+1], m_ptSegs[J] );
						}
						break;
					}
				}
				// Have we gone full-circle?
				if ( PointsTouch( m_ptSegs[_ui32StartEdge].p, m_ptSegs[I].q, _tEpsilon ) ) {
					break;
				}
			}
			return (I + 1) - _ui32StartEdge;
		}

		/**
		 * Counts the connected edges sequentially until a full loop is found.
		 *
		 * \param _ui32StartEdge The edge from where to start counting.
		 * \param _tEpsilon The epsilon for considering points connected.
		 * \return Returns the number of edges connected to make a fully enclosed set of edges such that the last edge connects back
		 *	to the first edge.
		 */
		uint32_t LSE_CALL						CountEdges( uint32_t _ui32StartEdge, _tType _tEpsilon ) {
			uint32_t I;
			for ( I = _ui32StartEdge; I < m_ui32Total; ++I ) {
				// Have we gone full-circle?
				if ( PointsTouch( m_ptSegs[_ui32StartEdge].p, m_ptSegs[I].q, _tEpsilon ) ) {
					break;
				}
			}
			return (I + 1) - _ui32StartEdge;
		}

		/**
		 * Copies a section from the given polygon into this one.
		 *
		 * \param _p3bSrc The polygon whose segments are to be copied.
		 * \param _phtTable Pointer to a single table entry indicating the section that should be copied.
		 * \return Returns true if the segments could be copied.
		 */
		LSBOOL LSE_CALL							Copy( const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, LSP_HOLE_TABLE * _phtTable ) {
			if ( Copy( _p3bSrc, _phtTable->ui32Start, _phtTable->ui32Total ) ) {
				_phtTable->bCopied = true;
				return true;
			}
			return false;
		}

		/**
		 * Finds the best set of segments to copy as well as the start and end points between which to connect them.  The search is based on the segments
		 *	in this polygon vs. the segments in each section of the given polygon, with sections noted by _phtTable.
		 *
		 * \param _p3bSrc The polygon whose segments are to be scanned.
		 * \param _phtTable The table of sections in the source polygon.
		 * \param _ui32TotalSections Number of sections to which _phtTable points.
		 * \param _ui32Localindex Index on this polygon into which to insert the 2nd polygon.
		 * \param _ui32Otherindex Index on the 2nd polygon to connect to this polygon.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns the index of the section to copy.
		 */
		uint32_t LSE_CALL						FindBestSegmentToMerge( const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, LSP_HOLE_TABLE * _phtTable, uint32_t _ui32TotalSections,
			uint32_t &_ui32Localindex, uint32_t &_ui32Otherindex, _tType _tEpsilon ) const {
			uint32_t ui32Best = LSE_MAXU32;
			_tType tScore = _tType( 0.0 );

			for ( uint32_t I = 0; I < _ui32TotalSections; ++I ) {
				if ( _phtTable[I].bCopied ) { continue; }

				// For each point in this polygon.
				for ( uint32_t Y = 0; Y < TotalSegments(); ++Y ) {
					// For each point in the current segment.
					uint32_t ui32ThisTotal = _phtTable[I].ui32Total + _phtTable[I].ui32Start;
					for ( uint32_t K = _phtTable[I].ui32Start; K < ui32ThisTotal; ++K ) {
						// If they touch then they are automatically the best choice.
						if ( PointsTouch( Segments()[Y].p, _p3bSrc.Segments()[K].p, _tEpsilon ) ) {
							_ui32Localindex = Y;
							_ui32Otherindex = K;
							return I;
						}
						// Create a segment between them.
						_tSeg3Type sTemp( Segments()[Y], _p3bSrc.Segments()[K] );
						if ( !SegmentOverlaps( sTemp, _p3bSrc, _tEpsilon ) ) {
							// Determine its score.  Shorter distances are better, but also being more perpendicular
							//	to the neighboring faces.
							auto tOrigin = sTemp.q - sTemp.p;
							_tType tThisDist = tOrigin.LenSq();
							if ( tThisDist ) { tThisDist = _tType( ::sqrt( tThisDist ) ); }
							/*tOrigin.Normalize();
							auto tThisSeg = Segments()[Y].q - Segments()[Y].p;
							tThisSeg.Normalize();
							uint32_t ui32NextIdx = (Y + 1) % TotalSegments();
							auto tThisSegNext = Segments()[ui32NextIdx].q - Segments()[ui32NextIdx].p;
							tThisSegNext.Normalize();

							auto tThatSeg = _p3bSrc.Segments()[K].q - _p3bSrc.Segments()[K].p;
							tThatSeg.Normalize();
							ui32NextIdx = ((K + 1) - _phtTable[I].ui32Start) % _phtTable[I].ui32Total + _phtTable[I].ui32Start;
							auto tThatSegNext = _p3bSrc.Segments()[ui32NextIdx].q - _p3bSrc.Segments()[ui32NextIdx].p;
							tThatSegNext.Normalize();
							_tType tCross0 = (tOrigin % tThisSeg).LenSq();
							_tType tCross1 = (tOrigin % tThatSeg).LenSq();
							_tType tCross2 = (tOrigin % tThisSegNext).LenSq();
							_tType tCross3 = (tOrigin % tThatSegNext).LenSq();*/

							// Also show a preference for segments that are not parallel to any other segments.
							_tType tParScore = RateParallelism( sTemp, _p3bSrc );
							
							// Distance cannot be 0.
							//_tType tThisScore = ((tCross0 * tCross1 * tCross2 * tCross3) + tParScore) / tThisDist;
							//_tType tThisScore = tCross0 * tCross1 * tCross2 * tCross3 + tParScore;
							_tType tThisScore = _tType( 1.0 ) / tThisDist;// + (tParScore * _tType( 0.0 ));
							if ( ui32Best == LSE_MAXU32 || tThisScore > tScore ) {
								tScore = tThisScore;
								ui32Best = I;
								_ui32Localindex = Y;
								_ui32Otherindex = K;
							}
						}
					}
				}
			}

			return ui32Best;
		}

		/**
		 * Determines if the given line segment overlaps any line segments in either this polygon or the given polygon.
		 *
		 * \param _tSeg The segment to test for overlap.
		 * \param _p3bOther The secondary polygon to test.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the segment overlaps any segments in either polygon.
		 */
		LSBOOL LSE_CALL							SegmentOverlaps( const _tSeg3Type &_tSeg, const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bOther, _tType _tEpsilon ) const {
			// Test this polygon first.
			for ( uint32_t I = TotalSegments(); I--; ) {
				// Only check if they are not touching.
				if ( !PointsTouch( Segments()[I].p, _tSeg.p, _tEpsilon ) && !PointsTouch( Segments()[I].q, _tSeg.p, _tEpsilon ) &&
					!PointsTouch( Segments()[I].p, _tSeg.q, _tEpsilon ) && !PointsTouch( Segments()[I].q, _tSeg.q, _tEpsilon ) ) {
					_tType fS, fT;
					_tType fDist = ClosestPointOnLineSegToLineSeg( _tSeg, Segments()[I], fS, fT, _tEpsilon );
					if ( fS < _tType( 1.0 ) && fS > _tType( 0.0 ) &&
						fT < _tType( 1.0 ) && fT > _tType( 0.0 ) ) { return true; }
					//if ( !fDist ) { return true; }
				}
			}

			// Test the other polygon.
			for ( uint32_t I = _p3bOther.TotalSegments(); I--; ) {
				// Only check if they are not touching.
				if ( !PointsTouch( _p3bOther.Segments()[I].p, _tSeg.p, _tEpsilon ) && !PointsTouch( _p3bOther.Segments()[I].q, _tSeg.p, _tEpsilon ) &&
					!PointsTouch( _p3bOther.Segments()[I].p, _tSeg.q, _tEpsilon ) && !PointsTouch( _p3bOther.Segments()[I].q, _tSeg.q, _tEpsilon ) ) {
					_tType fS, fT;
					_tType fDist = ClosestPointOnLineSegToLineSeg( _tSeg, _p3bOther.Segments()[I], fS, fT, _tEpsilon );
					if ( fS < _tType( 1.0 ) && fS > _tType( 0.0 ) &&
						fT < _tType( 1.0 ) && fT > _tType( 0.0 ) ) { return true; }
					//if ( !fDist ) { return true; }
				}
			}
			return false;
		}

		/**
		 * Determines how parallel a given segment is with any segments in either the current polygon or the given polygon.
		 *	Lower values are more parallel.
		 *
		 * \param _tSeg The segment to test.
		 * \param _p3bOther The secondary polygon.
		 * \return Returns a score rating how parallel the given segment is with all segments in this and the given polygon.
		 */
		_tType LSE_CALL							RateParallelism( const _tSeg3Type &_tSeg, const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bOther ) const {
			_tType tRes = _tType( LSM_FLT_MAX );
			auto vSeg = _tSeg.q - _tSeg.p;
			vSeg.Normalize();
			for ( uint32_t I = 0; I < TotalSegments(); ++I ) {
				auto vThis = Segments()[I].q - Segments()[I].p;
				vThis.Normalize();
				_tType tTemp = (vSeg % vThis).LenSq();
				tRes = CStd::Min( tRes, tTemp );
			}
			for ( uint32_t I = 0; I < _p3bOther.TotalSegments(); ++I ) {
				auto vThis = _p3bOther.Segments()[I].q - _p3bOther.Segments()[I].p;
				vThis.Normalize();
				_tType tTemp = (vSeg % vThis).LenSq();
				tRes = CStd::Min( tRes, tTemp );
			}
			return tRes;
		}

		/**
		 * Copies a section of a polygon into this one with an offset.  The section is effectively rotated by the given amount during the copy.
		 *
		 * \param _p3bSrc The polygon whose segments are to be copied.
		 * \param _phtSection The single section in the source polygon to be copied.
		 * \param _ui32Localindex Index on this polygon into which to insert the 2nd polygon.
		 * \param _ui32Otherindex Index on the 2nd polygon to connect to this polygon.
		 * \return Returns true if all segments could be copied.
		 */
		LSBOOL LSE_CALL							InsertSectionWithOffset( const CDynamicPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, LSP_HOLE_TABLE * _phtSection,
			uint32_t _ui32Localindex, uint32_t _ui32Otherindex ) {
			assert( _ui32Otherindex >= _phtSection->ui32Start );
			uint32_t ui32Offset = _ui32Otherindex - _phtSection->ui32Start;
			for ( uint32_t I = 0; I < _phtSection->ui32Total; ++I ) {
				uint32_t ui32This = ((I + ui32Offset) % _phtSection->ui32Total) + _phtSection->ui32Start;
				if ( !Insert( &_p3bSrc.Segments()[ui32This], 1, _ui32Localindex++ ) ) { return false; }
			}
			_phtSection->bCopied = true;
			return true;
		}

		/**
		 * Gets the squared distance between the given segments.  Also returns t for
		 *	the position of that point along both segments.
		 *
		 * \param _tSeg0 Line segment to test.
		 * \param _tSeg1 Line segment to test.
		 * \param _tS Holds the returned time value of the point along _tSeg0.
		 * \param _tT Holds the returned time value of the point along _tSeg1.
		 * \param _tRealS Holds the unclamped time of intersection on _tSeg0.
		 * \param _tRealT Holds the unclamped time of intersection on _tSeg1.
		 * \param _v0 Holds the closest point on _tSeg0 to _tSeg1.
		 * \param _v1 Holds the closest point on the _tSeg1 to _tSeg0.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns the squared distance between the line segments and the times t and s along each segment that represents
		 *	the closest points between them.
		 */
		template <typename _tVec3Type>
		_tType LSE_FCALL						ClosestPointOnLineSegToLineSeg( const _tSeg3Type &_tSeg0, const _tSeg3Type &_tSeg1, _tType &_tS, _tType &_tT,
			_tType &_tRealS, _tType &_tRealT, _tVec3Type &_v0, _tVec3Type &_v1, _tType _tEpsilon ) const {
			auto vD1 = _tSeg0.q - _tSeg0.p;
			auto vD2 = _tSeg1.q - _tSeg1.p;
			auto vR = _tSeg0.p - _tSeg1.p;
			_tType fA = vD1.LenSq();
			_tType fE = vD2.LenSq();
			_tType fF = vD2.Dot( vR );

			// If both segments degenerate to a point.
			if ( fA <= _tEpsilon && fE <= _tEpsilon ) {
				_tT = _tS = static_cast<_tType>(0.0);
				// No valid value for _tRealS and _tRealT.
				_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
				_v0 = _tSeg0.p;
				_v1 = _tSeg1.p;
				return vR.LenSq();
			}
			// If this segment degenerates into a point.
			if ( fA <= _tEpsilon ) {
				_tS = static_cast<_tType>(0.0);
				// No valid value for _tRealS and _tRealT.
				_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
				_tT = CMathLib::Clamp( fF / fE, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
			}
			else {
				_tType fC = vD1.Dot( vR );
				// If the other segment degenerates into a point.
				if ( fE <= _tEpsilon ) {
					_tT = static_cast<_tType>(0.0);
					// No valid value for _tRealS and _tRealT.
					_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
					_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
				}
				else {
					// Neither are degenerate.  This is most cases.
					_tType fB = vD1.Dot( vD2 );
					_tType fDenom = fA * fE - fB * fB;

					// If not parallel.
					if ( fDenom != static_cast<_tType>(0.0) ) {
						_tRealS = (fB * fF - fC * fE) / fDenom;
						_tS = CMathLib::Clamp( _tRealS, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					// Otherwise pick an arbitrary point (0 here).
					else {
						_tRealS = _tS = static_cast<_tType>(0.0);
					}

					_tT = _tRealT = (fB * _tS + fF) / fE;

					// If _tT is in the range [0,1] then we are done.  Otherwise
					//	adjusting T requires updating S also.
					if ( _tT < static_cast<_tType>(0.0) ) {
						_tT = static_cast<_tType>(0.0);
						_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					else if ( _tT > static_cast<_tType>(1.0) ) {
						_tT = static_cast<_tType>(1.0);
						_tS = CMathLib::Clamp( (fB - fC) / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}

				}
			}

			_v0 = _tSeg0.p + vD1 * _tS;
			_v1 = _tSeg1.p + vD2 * _tT;
			_tType fDist = (_v1 - _v0).LenSq();
			// If there was a degenerate situation, _tRealS/_tRealT will only be valid in the case where the line segments actually
			//	intersect.  In a degenerate situation, one or both of the line segments is treated as a point, so they cannot
			//	be calculated except if the degenerate point is on the valid line segment, or both degenerate points are the same.
			if ( _tRealS == static_cast<_tType>(LSM_INFINITY) && fDist <= _tEpsilon ) {
				_tRealS = _tS;
				_tRealT = _tT;
			}
			return fDist;
		}

		/**
		 * Gets the squared distance between between the given segments.  Also returns t for
		 *	the position of that point along both segments.
		 *
		 * \param _tSeg0 Line segment to test.
		 * \param _tSeg1 Line segment to test.
		 * \param _tS Holds the returned time value of the point along _tSeg0.
		 * \param _tT Holds the returned time value of the point along _tSeg1.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns the squared distance between the line segments and the times t and s along each segment that represents
		 *	the closest points between them.
		 */
		_tType LSE_FCALL						ClosestPointOnLineSegToLineSeg( const _tSeg3Type &_tSeg0, const _tSeg3Type &_tSeg1, _tType &_tS, _tType &_tT, _tType _tEpsilon ) const {
			auto vD1 = _tSeg0.p, vD2 = _tSeg0.p;
			_tType tRealS, tRealT;
			return ClosestPointOnLineSegToLineSeg( _tSeg0, _tSeg1, _tS, _tT, tRealS, tRealT, vD1, vD2, _tEpsilon );
		}

		/**
		 * Gets the sum of the lengths of each segment between the given indices.
		 *
		 * \param _ui32Index0 The starting index.
		 * \param _ui32Index1 The starting index.
		 * \return Returns the sum of the lengths of each segment between the given indices.
		 */
		_tType LSE_FCALL						SumLengthsBetween( uint32_t _ui32Index0, uint32_t _ui32Index1 ) const {
			_tType tSum = _tType( 0.0 );
			while ( _ui32Index0 != _ui32Index1 ) {
				_tType tTemp = (Segments()[_ui32Index0].q - Segments()[_ui32Index0].p).LenSq();
				if ( tTemp ) {
					tSum += _tType( ::sqrt( tTemp ) );
				}
				_ui32Index0 = (_ui32Index0 + 1) % TotalSegments();
			}
			return tSum;
		}

		/**
		 * Determines if 2 points touch given an epsilon.
		 *
		 * \param _tLeft The left point.
		 * \param _tRight The right point.
		 * \param _tEpsilon The epsilon for determining if points are considered touching.
		 * \return Returns true if the points are within _tEpsilon distance of each other.
		 */
		template <typename _tVec3Type>
		static LSBOOL LSE_FCALL					PointsTouch( const _tVec3Type &_tLeft, const _tVec3Type &_tRight, _tType _tEpsilon ) {
			return (_tLeft - _tRight).LenSq() <= (_tEpsilon * _tEpsilon);
		}
	};

}	// namespace lsp

#endif	// __LSP_DYNAMICPOLYGONBASE_H__
