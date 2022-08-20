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
 * Description: A template allowing to create polygons with a variable number of edges.
 */


#ifndef __LSP_POLYGON3BASE_H__
#define __LSP_POLYGON3BASE_H__

#include "../LSPPhysicsLib.h"
#include "Allocators/LSAStackAllocator.h"

#include <cassert>

namespace lsp {

	/**
	 * Class CPolygon3Base
	 * \brief A template allowing to create polygons with a variable number of edges.
	 *
	 * Description: A template allowing to create polygons with a variable number of edges.
	 */
	template <typename _tType, typename _tSeg3Type, typename _tPlane3Type>
	class CPolygon3Base {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CPolygon3Base() :
			m_ptSegs( NULL ),
			m_psaAllocator( NULL ),
			m_uiptrUser( 0 ),
			m_ui32Total( 0 ),
			m_bSplitter( false ) {
		}
		LSE_CALLCTOR							CPolygon3Base( const CPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bOther ) :
			m_ptSegs( NULL ),
			m_psaAllocator( NULL ),
			m_uiptrUser( 0 ),
			m_ui32Total( 0 ),
			m_bSplitter( false ) {
			Copy( _p3bOther, _p3bOther.m_psaAllocator );
		}
		LSE_CALLCTOR							~CPolygon3Base() {
			Reset();
		}


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index The index of the point to be returned.
		 * \return Returns the segment at the given index.
		 */
		_tSeg3Type * LSE_CALL					operator [] ( uint32_t _ui32Index ) {
			return &Segments()[_ui32Index];
		}

		/**
		 * Array access.
		 *
		 * \param _ui32Index The index of the point to be returned.
		 * \return Returns the segment at the given index.
		 */
		const _tSeg3Type * LSE_CALL				operator [] ( uint32_t _ui32Index ) const {
			return &Segments()[_ui32Index];
		}

		/**
		 * Copy operator.
		 *
		 * \param _pSrc The source polygon to copy.
		 * \return Returns a reference to the copied polygon.
		 */
		CPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> & LSE_CALL
												operator = ( const CPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_pSrc ) {
			Reset();
			Copy( _pSrc, _pSrc.m_psaAllocator );
			return (*this);
		}


		// == Functions.
		/**
		 * Resets the polygon.
		 */
		void LSE_CALL							Reset() {
			for ( ; m_ui32Total--; ) {
				m_ptSegs[m_ui32Total].~_tSeg3Type();	// Destroy, don't delete.
			}
			m_ptSegs = NULL;
			m_ui32Total = 0;
			m_psaAllocator = NULL;
			m_uiptrUser = 0;
			m_bSplitter = false;
		}

		/**
		 * Copies the source polygon into this one.
		 *
		 * \param _p3bSrc The polygon to copy into this one.
		 * \param _psaAllocator The stack allocator to be used for the line segments used by this polygon.
		 * \return Returns true if the segments could be allocated from the given source polygon.
		 */
		LSBOOL LSE_CALL							Copy( const CPolygon3Base<_tType, _tSeg3Type, _tPlane3Type> &_p3bSrc, CStackAllocator * _psaAllocator ) {
			Reset();
			if ( !SetTotalSides( _p3bSrc.TotalSegments(), _psaAllocator ) ) { return false; }
			for ( uint32_t I = _p3bSrc.TotalSegments(); I--; ) {
				m_ptSegs[I] = _p3bSrc.Segments()[I];
			}
			m_pPlane = _p3bSrc.m_pPlane;
			m_uiptrUser = _p3bSrc.m_uiptrUser;
			m_bSplitter = _p3bSrc.m_bSplitter;
			return true;
		}

		/**
		 * Sets the number of segments in the polygon.  This can't be changed after being set once.
		 *
		 * \param _ui32Total The number of segments in the polygon.
		 * \param _psaAllocator The stack allocator to be used for the line segments used by this polygon.
		 * \return Returns true if it was able to allocate the given number of segments from the stack allocator.
		 */
		LSBOOL LSE_CALL							SetTotalSides( uint32_t _ui32Total, CStackAllocator * _psaAllocator ) {
			Reset();
			m_psaAllocator = _psaAllocator;
			if ( _ui32Total < 3 ) { return false; }
			if ( (m_ptSegs = static_cast<_tSeg3Type *>(_psaAllocator->Alloc( sizeof( _tSeg3Type ) * _ui32Total, __alignof( _tSeg3Type ) ))) ) {
				m_ui32Total = _ui32Total;
				for ( ; _ui32Total--; ) {
					new( &m_ptSegs[_ui32Total] ) _tSeg3Type();
				}
				return true;
			}
			return false;
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
		 * Finalizes the polygon such that the end of each segment connects to the start of another segment.
		 *	If false is returned, at least one segment was unable to be connected.
		 *
		 * \param _tEpsilon The epsilon for snapping points together.
		 * \return Returns true if all end points of the polygon's edges connect with 1 starting point.
		 */
		LSBOOL LSE_CALL							Finalize( _tType _tEpsilon ) {
			assert( m_ui32Total >= 3 );
			// Remove any segments that have any missing connections.
			for ( uint32_t I = 0; I < m_ui32Total; ) {
				// Check P against Q.
				LSBOOL bHasP = false, bHasQ = false;
				_tType tLen = (m_ptSegs[I].q - m_ptSegs[I].p).LenSq();
				if ( tLen >= _tEpsilon * _tEpsilon ) {
					for ( uint32_t J = 0; J < m_ui32Total; ++J ) {
						if ( J == I ) { continue; }
						_tType tDist = (m_ptSegs[I].p - m_ptSegs[J].q).Len();
						if ( tDist <= _tEpsilon ) {
							bHasP = true;
							break;
						}
					}
					if ( bHasP ) {
						// Check its Q.
						for ( uint32_t J = 0; J < m_ui32Total; ++J ) {
							if ( J == I ) { continue; }
							_tType tDist = (m_ptSegs[I].q - m_ptSegs[J].p).Len();
							if ( tDist <= _tEpsilon ) {
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
			for ( uint32_t I = 0; I < m_ui32Total; ++I ) {
				// Find the starting point that connects to the end point of the Ith edge.
				for ( uint32_t J = I + 1; J < m_ui32Total; ++J ) {
					_tType tDist = (m_ptSegs[J].p - m_ptSegs[I].q).Len();
					if ( tDist <= _tEpsilon ) {
						if ( J != I + 1 ) {	// No need to swap here.
							CStd::Swap( m_ptSegs[I+1], m_ptSegs[J] );
						}
						break;
					}
				}
			}

			// Return true if we came full circle.
			return (m_ptSegs[0].p - m_ptSegs[m_ui32Total-1].q).Len() <= _tEpsilon;
		}

		/**
		 * Removes a line segment at the given index.
		 *
		 * \param _ui32Index Index of the line segment to remove.
		 */
		void LSE_CALL							Remove( uint32_t _ui32Index ) {
			if ( _ui32Index < m_ui32Total ) {
				m_ptSegs[_ui32Index].~_tSeg3Type();		// Destroy, don't delete.
				for ( uint32_t J = _ui32Index + 1; J < m_ui32Total; ++J ) {
					m_ptSegs[J-1] = m_ptSegs[J];
				}
				--m_ui32Total;
			}
		}

		/**
		 * Determines whether this ploygon is a splitter or not.
		 *
		 * \return Returns true if this polygon was used as a splitting plane in BSP compilation.
		 */
		LSBOOL LSE_CALL							IsSplitter() const {
			return m_bSplitter;
		}

		/**
		 * Sets this polygon as a splitter or not.
		 *
		 * \param _bVal If true, the polygon is marked as a splitter, otherwise it is not.
		 */
		void LSE_CALL							SetAsSplitter( LSBOOL _bVal ) {
			m_bSplitter = _bVal;
		}


	protected :
		// == Members.
		/** Pointer to the array of line segments. */
		_tSeg3Type *							m_ptSegs;

		/** The stack allocator used by this object. */
		CStackAllocator *						m_psaAllocator;

		/** Special meaning. */
		uintptr_t								m_uiptrUser;

		/** Total line segments. */
		uint32_t								m_ui32Total;

		/** The plane for this polygon. */
		_tPlane3Type							m_pPlane;

		/** Is this a splitter? */
		LSBOOL									m_bSplitter;

	};

}	// namespace lsp

#endif	// __LSP_POLYGON3BASE_H__
