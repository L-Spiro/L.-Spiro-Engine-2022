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
 * Description: A template for a solid-leaf BSP.  The BSP class splits polygons as its sends them to either side
 *	of nodes.
 */


#ifndef __LSP_SOLIDLEAFBSPBASE_H__
#define __LSP_SOLIDLEAFBSPBASE_H__

#include "../LSPPhysicsLib.h"
#include "../Intersections/LSPClassify.h"
#include "../Intersections/LSPIntersect.h"
#include "../Misc/LSPPhysUtils.h"
#include "Vector/LSTLVector.h"

#ifdef LSP_TOOL
#include <iostream>
#endif	// #ifdef LSP_TOOL

namespace lsp {

	/**
	 * Class CSolidLeafBspBase
	 * \brief A template for a solid-leaf BSP.
	 *
	 * Description: A template for a solid-leaf BSP.  The BSP class splits polygons as its sends them to either side
	 *	of nodes.
	 */
	template <typename _tType, typename _tVecType, typename _tPlaneType, typename _tPolyType>
	class CSolidLeafBspBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CSolidLeafBspBase() {
		}


		// == Types.
		/**
		 * A node in the BSP tree.
		 */
		typedef struct LSP_NODE {
			/**
			 * The parent node.
			 */
			LSP_NODE *						pnParent;

			/**
			 * The front node.
			 */
			LSP_NODE *						pnFront;

			/**
			 * The back node.
			 */
			LSP_NODE *						pnBack;

			/**
			 * The array of polygons.
			 */
			CVector<_tPolyType, uint32_t, 4096UL>
											vPolies;

			/**
			 * The plane.
			 */
			_tPlaneType						tPlane;

			/**
			 * User data.
			 */
			uintptr_t						uiptrUser;


			// == Various constructors.
			LSE_CALLCTOR					LSP_NODE() :
				pnParent( NULL ),
				pnFront( NULL ),
				pnBack( NULL ),
				uiptrUser( 0 ) {
			}
			LSE_CALLCTOR					~LSP_NODE() {
				if ( pnFront ) {
					pnFront->~LSP_NODE();
					pnFront = NULL;
				}
				if ( pnBack ) {
					pnBack->~LSP_NODE();
					pnBack = NULL;
				}
			}
		} * LPLSP_NODE, * const LPCLSP_NODE;


		// == Functions.
		/**
		 * Gets a pointer to the root node to the BSP tree.  Never returns NULL.
		 *
		 * \return Returns a pointer to the root node to the BSP tree.  Never returns NULL.
		 */
		const LSP_NODE * LSE_CALL			Root() const {
			return &m_nRoot;
		}

		/**
		 * Builds the BSP given an array of polygons.
		 *
		 * \param _ppPolies The array of polygons.
		 * \param _ui32Total The total number of polygons.
		 * \param _psaAllocator The stack allocator to be used to allocate new nodes and polygon segments.
		 * \param _fThickness Plane thickness.
		 * \return Returns true if there was enough memory to create the BSP tree.
		 */
		LSBOOL LSE_CALL						CreateBsp( const _tPolyType * _ppPolies, uint32_t _ui32Total,
			CStackAllocator * _psaAllocator, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// Make a copy of the polygons.
			CVector<_tPolyType, uint32_t, 4096UL> vPolyCopy;
			vPolyCopy.Allocate( _ui32Total );
			for ( uint32_t I = 0; I < _ui32Total; ++I ) {
				if ( _ppPolies[I].TotalSegments() ) {
					if ( !vPolyCopy.Push( _tPolyType() ) ) { return false; }
					if ( !vPolyCopy[vPolyCopy.Length()-1].Copy( _ppPolies[I], _psaAllocator ) ) { return false; }
				}
			}
			vPolyCopy.Snap();

			return CreateBsp( m_nRoot, vPolyCopy, _psaAllocator, _fThickness );
		}

		/**
		 * Builds the BSP given an array of polygons in which the leafs store convex sets of polygons rather than
		 *	just polygons coplanar to the leaf plane.
		 *
		 * \param _ppPolies The array of polygons.
		 * \param _ui32Total The total number of polygons.
		 * \param _psaAllocator The stack allocator to be used to allocate new nodes and polygon segments.
		 * \param _fThickness Plane thickness.
		 * \return Returns true if there was enough memory to create the BSP tree.
		 */
		LSBOOL LSE_CALL						CreateBspConvex( const _tPolyType * _ppPolies, uint32_t _ui32Total,
			CStackAllocator * _psaAllocator, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// Make a copy of the polygons.
			CVector<_tPolyType, uint32_t, 4096UL> vPolyCopy;
			vPolyCopy.Allocate( _ui32Total );
			for ( uint32_t I = 0; I < _ui32Total; ++I ) {
				if ( _ppPolies[I].TotalSegments() ) {
					if ( !vPolyCopy.Push( _tPolyType() ) ) { return false; }
					if ( !vPolyCopy[vPolyCopy.Length()-1].Copy( _ppPolies[I], _psaAllocator ) ) { return false; }
				}
			}
			vPolyCopy.Snap();

			return CreateBspConvex( m_nRoot, vPolyCopy, _psaAllocator, _fThickness );
		}

		/**
		 * Fills the given array with pointers to the leafs on this tree.
		 *
		 * \param _vArray Holds the returned array of leafs.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSBOOL LSE_CALL						GetLeafs( CVectorPoD<LSP_NODE *, uint32_t> &_vArray ) {
			return GetLeafs( &m_nRoot, _vArray );
		}

		/**
		 * Fills the given array with pointers to the leafs on this tree.
		 *
		 * \param _vArray Holds the returned array of leafs.
		 * \param _vStack Explicit stack to use.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSBOOL LSE_CALL						GetLeafs( CVectorPoD<const LSP_NODE *, uint32_t> &_vArray, CVectorPoD<const LSP_NODE *, uint32_t> &_vStack ) const {
			return GetLeafs( &m_nRoot, _vArray, _vStack );
		}

		/**
		 * Fills the given array with pointers to the leafs on this tree.
		 *
		 * \param _pnNode The node.
		 * \param _vArray Holds the returned array of leafs.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSBOOL LSE_CALL						GetLeafs( LSP_NODE * _pnNode, CVectorPoD<LSP_NODE *, uint32_t> &_vArray ) const {
			// Explicit stacking for stability.
			CVector<LSP_NODE *, uint32_t, 4096UL> vStack;
			if ( !vStack.Push( _pnNode ) ) { return false; }
			while ( vStack.Length() ) {
				LSP_NODE * pnThis = vStack[vStack.Length()-1];
				vStack.PopNoDealloc();
				if ( !_vArray.Push( pnThis ) ) { return false; }

				if ( pnThis->pnFront ) {
					if ( !vStack.Push( pnThis->pnFront ) ) { return false; }
				}
				if ( pnThis->pnBack ) {
					if ( !vStack.Push( pnThis->pnBack ) ) { return false; }
				}
			}
			return true;
		}

		/**
		 * Fills the given array with pointers to the leafs on this tree.
		 *
		 * \param _pnNode The node.
		 * \param _vArray Holds the returned array of leafs.
		 * \param _vStack Explicit stack to use.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSBOOL LSE_CALL						GetLeafs( const LSP_NODE * _pnNode, CVectorPoD<const LSP_NODE *, uint32_t> &_vArray,
			CVectorPoD<const LSP_NODE *, uint32_t> &_vStack ) const {
			// Explicit stacking for stability.
			_vStack.ResetNoDealloc();
			if ( !_vStack.Push( _pnNode ) ) { return false; }
			while ( _vStack.Length() ) {
				const LSP_NODE * pnThis = _vStack[_vStack.Length()-1];
				_vStack.PopNoDealloc();
				if ( !_vArray.Push( pnThis ) ) { return false; }

				if ( pnThis->pnFront ) {
					if ( !_vStack.Push( pnThis->pnFront ) ) { return false; }
				}
				if ( pnThis->pnBack ) {
					if ( !_vStack.Push( pnThis->pnBack ) ) { return false; }
				}
			}
			return true;
		}

		/**
		 * Fills the given array with pointers to the parents of the given node.
		 *
		 * \param _pnNode The node whose parents are to be returned.
		 * \param _vArray Holds the returned array of parent nodes.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		static LSBOOL LSE_CALL				GetParents( const LSP_NODE * _pnNode, CVectorPoD<LSP_NODE *, uint32_t> &_vArray ) {
			_vArray.ResetNoDealloc();
			for ( LSP_NODE * pnParent = _pnNode->pnParent; pnParent; pnParent = pnParent->pnParent ) {
				if ( !_vArray.Push( pnParent ) ) { return false; }
			}
			return true;
		}


	protected :
		// == Types.
		/**
		 * The explicit stack used during BSP construction.
		 */
		typedef struct LSP_STACK {
			/**
			 * The node.
			 */
			LSP_NODE *						pnNode;

			/**
			 * Vector of polygons for this node.
			 */
			CVector<_tPolyType, uint32_t, 4096UL>
											vPolies;
		} * LPLSP_STACK, * const LPCLSP_STACK;


		// == Members.
		/**
		 * The root node.
		 */
		LSP_NODE							m_nRoot;


		// == Functions
		/**
		 * Builds the BSP given an array of polygons and the node in which to store the results.
		 *
		 * \param _nNode The node in which to store the results of the splitting.
		 * \param _vPolies The array of polygons.
		 * \param _psaAllocator The stack allocator to be used to allocate new nodes and polygon segments.
		 * \param _fThickness Plane thickness.
		 * \return Returns true if there was enough memory to create the BSP tree.
		 */
		LSBOOL LSE_CALL						CreateBsp( LSP_NODE &_nNode, const CVector<_tPolyType, uint32_t, 4096UL> &_vPolies,
			CStackAllocator * _psaAllocator, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// Explicit stacking is used for stability.  Stack overflow on large/complicated next-generation maps is not
			//	a problem for this algorithm.
			CVector<LSP_STACK, uint32_t, 4096UL> vStack;
			vStack.Allocate( _vPolies.Length() );	// The number of polygons should be sufficient stack size.

			// Start it off with the first node.
			if ( !vStack.Push( LSP_STACK() ) ) { return false; }
			vStack[0].pnNode = &_nNode;
			vStack[0].vPolies = _vPolies;

			CVector<_tPolyType, uint32_t, 4096UL> vFrontPolies, vBackPolies;

			while ( vStack.Length() ) {
				LSP_STACK sThis = vStack[vStack.Length()-1];
				vStack.PopNoDealloc();
				if ( sThis.vPolies.Length() == 0 ) { continue; }
				vFrontPolies.ResetNoDealloc();
				vBackPolies.ResetNoDealloc();
				uint32_t ui32Index;
				sThis.pnNode->tPlane = BestSplittingPlane( sThis.vPolies, ui32Index, _fThickness );
#ifdef LSP_TOOL
				::printf( "." );
#endif	// #ifdef LSP_TOOL

				for ( uint32_t I = 0; I < sThis.vPolies.Length(); ++I ) {
					if ( I == ui32Index ) {
						if ( !sThis.pnNode->vPolies.Push( sThis.vPolies[I] ) ) {
							return false;
						}
						continue;
					}
					switch ( CClassify::Polygon( sThis.vPolies[I], sThis.pnNode->tPlane, _fThickness ) ) {
						case LSM_PI_FRONT : {
							if ( !vFrontPolies.Push( sThis.vPolies[I] ) ) { return false; }
							break;
						}
						case LSM_PI_BACK : {
							if ( !vBackPolies.Push( sThis.vPolies[I] ) ) { return false; }
							break;
						}
						case LSM_PI_COPLANAR : {
							if ( !sThis.pnNode->vPolies.Push( sThis.vPolies[I] ) ) {
								return false;
							}
							break;
						}
						case LSM_PI_INTERSECT : {
							uint32_t ui32Left, ui32Right;
							CPhysUtils::Split<_tType, _tVecType, _tPolyType, _tPlaneType>( sThis.vPolies[I], sThis.pnNode->tPlane, &ui32Left, NULL, &ui32Right, NULL,
								_fThickness );
							_tPolyType tLeft, tRight;
							if ( !tLeft.SetTotalSides( ui32Left, _psaAllocator ) ) {
								return false;
							}
							if ( !tRight.SetTotalSides( ui32Right, _psaAllocator ) ) {
								return false;
							}
							CPhysUtils::Split<_tType, _tVecType, _tPolyType, _tPlaneType>( sThis.vPolies[I], sThis.pnNode->tPlane, NULL, &tLeft, NULL, &tRight,
								_fThickness );
							if ( !vFrontPolies.Push( tLeft ) ) { return false; }
							if ( !vBackPolies.Push( tRight ) ) { return false; }
							break;
						}
					}
				}

				sThis.pnNode->vPolies.Snap();
				if ( vFrontPolies.Length() ) {
					sThis.pnNode->pnFront = static_cast<LSP_NODE *>(_psaAllocator->Alloc( sizeof( LSP_NODE ), 4 ));
					if ( !sThis.pnNode->pnFront ) {
						return false;
					}
					new( sThis.pnNode->pnFront ) LSP_NODE();
					if ( !vStack.Push( LSP_STACK() ) ) {
						return false;
					}
					sThis.pnNode->pnFront->pnParent = sThis.pnNode;
					vStack[vStack.Length()-1].pnNode = sThis.pnNode->pnFront;
					vStack[vStack.Length()-1].vPolies = vFrontPolies;
				}
				if ( vBackPolies.Length() ) {
					sThis.pnNode->pnBack = static_cast<LSP_NODE *>(_psaAllocator->Alloc( sizeof( LSP_NODE ), 4 ));
					if ( !sThis.pnNode->pnBack ) {
						return false;
					}
					new( sThis.pnNode->pnBack ) LSP_NODE();
					if ( !vStack.Push( LSP_STACK() ) ) {
						return false;
					}
					sThis.pnNode->pnBack->pnParent = sThis.pnNode;
					vStack[vStack.Length()-1].pnNode = sThis.pnNode->pnBack;
					vStack[vStack.Length()-1].vPolies = vBackPolies;
				}
			}

			return true;
		}

		/**
		 * Builds the BSP given an array of polygons and the node in which to store the results
		 *
		 * \param _nNode The node in which to store the results of the splitting.
		 * \param _vPolies The array of polygons.
		 * \param _psaAllocator The stack allocator to be used to allocate new nodes and polygon segments.
		 * \param _fThickness Plane thickness.
		 * \return Returns true if there was enough memory to create the BSP tree.
		 */
		LSBOOL LSE_CALL						CreateBspConvex( LSP_NODE &_nNode, const CVector<_tPolyType, uint32_t, 4096UL> &_vPolies,
			CStackAllocator * _psaAllocator, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// Explicit stacking is used for stability.  Stack overflow on large/complicated next-generation maps is not
			//	a problem for this algorithm.
			CVector<LSP_STACK, uint32_t, 4096UL> vStack;
			vStack.Allocate( _vPolies.Length() );	// The number of polygons should be sufficient stack size.

			// Start it off with the first node.
			if ( !vStack.Push( LSP_STACK() ) ) { return false; }
			vStack[0].pnNode = &_nNode;
			vStack[0].vPolies = _vPolies;

			CVector<_tPolyType, uint32_t, 4096UL> vFrontPolies, vBackPolies;
#ifdef LSP_TOOL
			uint32_t ui32Max = 0;
#endif	// #ifdef LSP_TOOL
			while ( vStack.Length() ) {
				LSP_STACK sThis = vStack[vStack.Length()-1];
				vStack.PopNoDealloc();
				if ( sThis.vPolies.Length() == 0 ) { continue; }
				if ( sThis.vPolies.Length() == 1 ) {
					// Only one polygon.  This is a leaf.
					sThis.pnNode->tPlane = sThis.vPolies[0].Plane();
					if ( !sThis.pnNode->vPolies.Push( sThis.vPolies[0] ) ) {
						return false;
					}
					sThis.pnNode->vPolies.Snap();
					continue;
				}
				vFrontPolies.ResetNoDealloc();
				vBackPolies.ResetNoDealloc();
				uint32_t ui32Index;
				sThis.pnNode->tPlane = BestSplittingPlane( sThis.vPolies, ui32Index, _fThickness );
#ifdef LSP_TOOL
				::printf( "." );
#endif	// #ifdef LSP_TOOL

				for ( uint32_t I = 0; I < sThis.vPolies.Length(); ++I ) {
					if ( I == ui32Index ) {
						//if ( !sThis.pnNode->vPolies.Push( sThis.vPolies[I] ) ) {
						sThis.vPolies[I].SetAsSplitter( true );
						if ( !vFrontPolies.Push( sThis.vPolies[I] ) ) {
							return false;
						}
						continue;
					}
					switch ( CClassify::Polygon( sThis.vPolies[I], sThis.pnNode->tPlane, _fThickness ) ) {
						case LSM_PI_FRONT : {
							if ( !vFrontPolies.Push( sThis.vPolies[I] ) ) { return false; }
							break;
						}
						case LSM_PI_BACK : {
							sThis.vPolies[I].SetAsSplitter( false );
							if ( !vBackPolies.Push( sThis.vPolies[I] ) ) { return false; }
							break;
						}
						case LSM_PI_COPLANAR : {
							if ( sThis.vPolies[I].Plane().n * sThis.pnNode->tPlane.n > _tType( 0 ) ) {
								//if ( !sThis.pnNode->vPolies.Push( sThis.vPolies[I] ) ) {
								sThis.vPolies[I].SetAsSplitter( true );
								if ( !vFrontPolies.Push( sThis.vPolies[I] ) ) {
									return false;
								}
							}
							else {
								sThis.vPolies[I].SetAsSplitter( false );
								if ( !vBackPolies.Push( sThis.vPolies[I] ) ) {
									return false;
								}
							}

							break;
						}
						case LSM_PI_INTERSECT : {
							uint32_t ui32Left, ui32Right;
							CPhysUtils::Split<_tType, _tVecType, _tPolyType, _tPlaneType>( sThis.vPolies[I], sThis.pnNode->tPlane, &ui32Left, NULL, &ui32Right, NULL,
								_fThickness );
							_tPolyType tLeft, tRight;
							if ( !tLeft.SetTotalSides( ui32Left, _psaAllocator ) ) {
								return false;
							}
							if ( !tRight.SetTotalSides( ui32Right, _psaAllocator ) ) {
								return false;
							}
							CPhysUtils::Split<_tType, _tVecType, _tPolyType, _tPlaneType>( sThis.vPolies[I], sThis.pnNode->tPlane, NULL, &tLeft, NULL, &tRight,
								_fThickness );
							if ( sThis.vPolies[I].IsSplitter() ) {
								tLeft.SetAsSplitter( true );
								//tRight.SetAsSplitter( true );
							}
							if ( !vFrontPolies.Push( tLeft ) ) { return false; }
							if ( !vBackPolies.Push( tRight ) ) { return false; }
							break;
						}
					}
				}
				if ( vFrontPolies.Length() && (AllAreSplitters( vFrontPolies ) || AllAreConvex( sThis.pnNode->vPolies, vFrontPolies, _fThickness )) ) {
					// Add them to the front node.
					if ( !sThis.pnNode->vPolies.Append( &vFrontPolies[0], vFrontPolies.Length() ) ) {
						return false;
					}

					/*sThis.pnNode->pnFront = static_cast<LSP_NODE *>(_psaAllocator->Alloc( sizeof( LSP_NODE ), 4 ));
					if ( !sThis.pnNode->pnFront ) {
						return false;
					}
					new( sThis.pnNode->pnFront ) LSP_NODE();
					sThis.pnNode->pnFront->pnParent = sThis.pnNode;
					if ( !sThis.pnNode->pnFront->vPolies.Append( &vFrontPolies[0], vFrontPolies.Length() ) ) {
						sThis.pnNode->pnFront->vPolies.Append( &vFrontPolies[0], vFrontPolies.Length() );
						return false;
					}
					sThis.pnNode->pnFront->vPolies.Snap();*/

#ifdef LSP_TOOL
					::printf( "%u ", sThis.pnNode->vPolies.Length() );
					ui32Max = CStd::Max( ui32Max, sThis.pnNode->vPolies.Length() );
					/*::printf( "%u ", sThis.pnNode->pnFront->vPolies.Length() );
					ui32Max = CStd::Max( ui32Max, sThis.pnNode->pnFront->vPolies.Length() );*/
#endif	// #ifdef LSP_TOOL
					vFrontPolies.ResetNoDealloc();
				}

				sThis.pnNode->vPolies.Snap();
				if ( vFrontPolies.Length() ) {
					sThis.pnNode->pnFront = static_cast<LSP_NODE *>(_psaAllocator->Alloc( sizeof( LSP_NODE ), 4 ));
					if ( !sThis.pnNode->pnFront ) {
						return false;
					}
					new( sThis.pnNode->pnFront ) LSP_NODE();
					if ( !vStack.Push( LSP_STACK() ) ) {
						return false;
					}
					sThis.pnNode->pnFront->pnParent = sThis.pnNode;
					vStack[vStack.Length()-1].pnNode = sThis.pnNode->pnFront;
					vStack[vStack.Length()-1].vPolies = vFrontPolies;
				}
				if ( vBackPolies.Length() ) {
					sThis.pnNode->pnBack = static_cast<LSP_NODE *>(_psaAllocator->Alloc( sizeof( LSP_NODE ), 4 ));
					if ( !sThis.pnNode->pnBack ) {
						return false;
					}
					new( sThis.pnNode->pnBack ) LSP_NODE();
					if ( !vStack.Push( LSP_STACK() ) ) {
						return false;
					}
					sThis.pnNode->pnBack->pnParent = sThis.pnNode;
					vStack[vStack.Length()-1].pnNode = sThis.pnNode->pnBack;
					vStack[vStack.Length()-1].vPolies = vBackPolies;
				}
			}
#ifdef LSP_TOOL
			::printf( "\r\nMax: %u.\r\n", ui32Max );
#endif	// #ifdef LSP_TOOL
			return true;
		}

		/**
		 * Finds the best splitting plane for the given set of input polygons.
		 *
		 * \param _vPolies The polygons for which to find the best splitting plane.
		 * \param _ui32Index On return, holds the index of the polygon whose plane was used for the return.
		 * \param _fThickness Plane thickness.
		 * \return Returns the best plane for splitting the given set of polygons.
		 */
		_tPlaneType LSE_CALL				BestSplittingPlane( const CVector<_tPolyType, uint32_t, 4096UL> &_vPolies, uint32_t &_ui32Index, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// Blend factor for optimizing for balancing splits (to be tweaked).
			_tType fK = static_cast<_tType>(0.95);

			// Variables for tracking the best plane seen so far.
			_tPlaneType pBest;
			_tType fBestScore = static_cast<_tType>(LSM_INFINITY);	// Lower score = better.
			LSBOOL bFoundOneBest = false;

			// Compare every plane against every other plane.
			for ( uint32_t I = 0; I < _vPolies.Length(); ++I ) {
				if ( _vPolies[I].IsSplitter() ) { continue; }
				uint32_t ui32Front = 0, ui32Behind = 0, ui32Straddling = 0;
				const _tPlaneType & pPlane = _vPolies[I].Plane();
				if ( bFoundOneBest && pBest == pPlane ) { continue; }

				// Check against all other polygons.  See how many are in front,
				//	behind, or straddling.
				for ( uint32_t J = 0; J < _vPolies.Length(); ++J ) {
					if ( I == J ) { continue; }

					switch ( CClassify::Polygon( _vPolies[J], pPlane, _fThickness ) ) {
						case LSM_PI_COPLANAR : {
							// Send these down the front by falling through.
						}
						case LSM_PI_FRONT : {
							++ui32Front;
							break;
						}
						case LSM_PI_BACK : {
							++ui32Behind;
							break;
						}
						case LSM_PI_INTERSECT : {
							++ui32Straddling;
							break;
						}
					}
				}

				// Get the score for this plane.
				_tType fScore = fK * ui32Straddling + (static_cast<_tType>(1.0) - fK) * CMathLib::AbsT<_tType>( static_cast<_tType>(ui32Front - ui32Behind) );
				if ( fScore < fBestScore ) {
					fBestScore = fScore;
					pBest = pPlane;
					bFoundOneBest = true;
					_ui32Index = I;
				}
			}
			if ( !bFoundOneBest ) {
				// Just find one that is not a splitter.
				for ( uint32_t I = 0; I < _vPolies.Length(); ++I ) {
					if ( !_vPolies[I].IsSplitter() ) {
						pBest = _vPolies[I].Plane();
						_ui32Index = I;
						break;
					}
				}
			}
			return pBest;
		}

		/**
		 * Determines if the polygons in the given array form a convex region of space.
		 *
		 * \param _vPlanePolies The array of polygons on the splitting plane.
		 * \param _vPolies The array of polygons to check for forming a convex region of space.
		 * \param _fThickness Plane thickness.
		 * \return Returns true if the polygons form a convex region of space.
		 */
		LSBOOL LSE_CALL						AllAreConvex( const CVector<_tPolyType, uint32_t, 4096UL> &_vPlanePolies, const CVector<_tPolyType, uint32_t, 4096UL> &_vPolies, _tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			for ( uint32_t I = 0; I < _vPolies.Length(); ++I ) {
				// If any other polygons are behind this one's plane, it is not convex.
				// Check here first since it is more likely to exit early.
				for ( uint32_t J = I + 1; J < _vPolies.Length(); ++J ) {
					if ( CClassify::Polygon( _vPolies[J], _vPolies[I].Plane(), _fThickness ) == LSM_PI_BACK ) {
						return false;
					}
					// Check against the plane polygons too.
					for ( uint32_t J = _vPlanePolies.Length(); J--; ) {
						if ( CClassify::Polygon( _vPlanePolies[J], _vPolies[I].Plane(), _fThickness ) == LSM_PI_BACK ) { return false; }
					}
				}
			}
			return true;
		}

		/**
		 * Determines if all given polygons are splitters.
		 *
		 * \param _vPolies The polygons to check.
		 * \return Returns true if all polygons are marked as being splitters.
		 */
		LSBOOL LSE_CALL						AllAreSplitters( const CVector<_tPolyType, uint32_t, 4096UL> &_vPolies ) {
			for ( uint32_t I = _vPolies.Length(); I--; ) {
				if ( !_vPolies[I].IsSplitter() ) { return false; }
			}
			return true;
		}

	};

}	// namespace lsp

#endif	// __LSP_SOLIDLEAFBSPBASE_H__
