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
 * Description: An 8-layer quad-tree with fast insertion times.  Uses a bit hack to increase insertion speed
 *	for high-performance use.
 */

#include "LSPQuadTree.h"
#include "../Intersections/LSPTest.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * The number of nodes in this tree.
 */
#define LSP_QUAD_NODES				(1+2*2+4*4+8*8+16*16+32*32+64*64+128*128)


namespace lsp {

	// == Various constructors.
	LSE_CALLCTOR CQuadTree::CQuadTree() :
		m_pui8Nodes( NULL ) {
	}
	LSE_CALLCTOR CQuadTree::~CQuadTree() {
		Reset();
	}

	// == Functions.
	/**
	 * Creates the quad-tree with the given size.  The created quad-tree has 8 nested layers.
	 *
	 * \param _fSize The size of the quad-tree to create.
	 * \return Returns true if there were no memory failures during the creation of the quad-tree.
	 */
	LSBOOL LSE_CALL CQuadTree::CreateQuadTree( LSREAL _fSize ) {
		Reset();
		// The number of nodes is fixed.
		//	We are "hacking" the array of nodes in order to save space.  Rather than allocating
		//	each node, which incurs a lot of wasted memory, we are storing them sequentially
		//	in a single buffer.
		m_pui8Nodes = LSENEW uint8_t[LSP_QUAD_NODES*sizeof( CQuadTreeNode )];
		if ( !m_pui8Nodes ) { return false; }
		CQuadTreeNode * pqtnNodes = Root();
		for ( uint32_t I = LSP_QUAD_NODES; I--; ) {
			new( &pqtnNodes[I] ) CQuadTreeNode();
		}

		m_fInvRadius = static_cast<LSREAL>(255.0) / _fSize;

		// Nodes are stored so that all nodes in a level are sequential in memory until
		//	the next level.  Within each level, the nodes are 2D arrays of equal width
		//	and height, double the previous layer.  There are 8 layers, with the first
		//	being 1-by-1 and the last being 128-by-128.
		//
		// This loop goes over each level
		CQuadTreeNode * pqtnThis = pqtnNodes, * pqtnLast = NULL;
		LSREAL fFull = _fSize;
		_fSize *= LSM_HALF;
		m_fRadius = _fSize;
		
		CVector2 vOffset( -_fSize, -_fSize );
		for ( uint32_t I = 0; I < 8; ++I ) {
			m_pqtnLevels[I] = pqtnThis;
			// For each node at this level.
			for ( uint32_t Y = (1 << I); Y--; ) {
				CQuadTreeNode * pqtnRow = &pqtnThis[Y*(1<<I)];
				LSREAL fYPos = LSREAL( Y ) / LSREAL( 1 << I ) * fFull + _fSize;
				for ( uint32_t X = (1 << I); X--; ) {
					pqtnRow[X].SetCenterAndRadius( CVector2( LSREAL( X ) / LSREAL( 1 << I ) * fFull + _fSize, fYPos ) + vOffset,
						_fSize );

					if ( pqtnLast ) {
						// There is a parent.  Get its index.
						uint32_t ui32ParentX = X >> 1;
						uint32_t ui32ParentY = Y >> 1;
						uint32_t ui32ParentIndex = (ui32ParentY * (1 << (I - 1))) + ui32ParentX;
						uint32_t ui32X = X & 1;
						uint32_t ui32Y = Y & 1;
						pqtnLast[ui32ParentIndex].SetChild( (ui32Y << 1) + ui32X, &pqtnRow[X] );
					}
				}
			}

			pqtnLast = pqtnThis;
			pqtnThis += (1 << I) * (1 << I);
			_fSize *= LSM_HALF;
		}

		return true;
	}

	/**
	 * Resets the quad-tree back to nothing.
	 */
	void LSE_CALL CQuadTree::Reset() {
		if ( m_pui8Nodes ) {
			CQuadTreeNode * pqtnNodes = Root();
			// The number of nodes is fixed.
			for ( uint32_t I = LSP_QUAD_NODES; I--; ) {
				pqtnNodes[I].~CQuadTreeNode();
			}
			LSEDELETE [] m_pui8Nodes;
			m_pui8Nodes = NULL;
		}
	}

	/**
	 * Inserts an object into the quad-tree.  Any items not fully inside the bounds of the quad-tree
	 *	are inserted at the root level.  The insertion process is extremely efficient.
	 *
	 * \param _pqtoObject The quad-tree-object to insert.
	 * \return Returns true if there are no memory failures during the insert.
	 */
	LSBOOL LSE_CALL CQuadTree::InsertObject( CQuadTreeObject * _pqtoObject ) {
		const CAabb2d & a2Box = _pqtoObject->QuadTreeAabb();
		if ( a2Box.m_vMin.x < -m_fRadius || a2Box.m_vMax.x > m_fRadius ||
			a2Box.m_vMin.y < -m_fRadius || a2Box.m_vMax.y > m_fRadius ) {
			return Root()->AddObject( _pqtoObject );
		}

		// The quad-tree exists in the range from -m_fRadius to m_fRadius.
		//	Convert to the range 0 to (m_fRadius * 2).
		CAabb2d a2Shifted;
		a2Shifted.m_vMin = a2Box.m_vMin + CVector2( m_fRadius, m_fRadius );
		a2Shifted.m_vMax = a2Box.m_vMax + CVector2( m_fRadius, m_fRadius );

		// Now to the range from [0..255].
		a2Shifted.m_vMin.x *= m_fInvRadius;
		a2Shifted.m_vMin.y *= m_fInvRadius;
		a2Shifted.m_vMax.x *= m_fInvRadius;
		a2Shifted.m_vMax.y *= m_fInvRadius;

		// Convert to integers and clamp.
		uint32_t ui32MinX = static_cast<uint32_t>(CStd::Clamp( CMathLib::Floor( a2Shifted.m_vMin.x ), LSM_ZERO, static_cast<LSREAL>(255.0) ));
		uint32_t ui32MaxX = static_cast<uint32_t>(CStd::Clamp( CMathLib::Ceil( a2Shifted.m_vMax.x ), LSM_ZERO, static_cast<LSREAL>(255.0) ));

		uint32_t ui32MinY = static_cast<uint32_t>(CStd::Clamp( CMathLib::Floor( a2Shifted.m_vMin.y ), LSM_ZERO, static_cast<LSREAL>(255.0) ));
		uint32_t ui32MaxY = static_cast<uint32_t>(CStd::Clamp( CMathLib::Ceil( a2Shifted.m_vMax.y ), LSM_ZERO, static_cast<LSREAL>(255.0) ));

		// Get the level at which the object will be inserted.
		uint32_t ui32X = ui32MinX ^ ui32MaxX;
		if ( !ui32X ) { ui32X = 7UL; }	// 100% flat objects go to the highest (smallest) level.
		else { ui32X = 7UL - CStd::HighestBit( ui32X ); }
		uint32_t ui32Y = ui32MinY ^ ui32MaxY;
		if ( !ui32Y ) { ui32Y = 7UL; }	// 100% flat objects go to the highest (smallest) level.
		else { ui32Y = 7UL - CStd::HighestBit( ui32Y ); }

		uint32_t ui32Level = CStd::Min( ui32X, ui32Y );

		// Now we know which level in the tree it is.
		// Find out which node on that level owns it.
		ui32X = ui32MinX >> (8 - ui32Level);
		ui32Y = ui32MinY >> (8 - ui32Level);

		// Add it to the node.
#ifdef _DEBUG
		CQuadTreeNode * pqtnNode = &m_pqtnLevels[ui32Level][ui32Y*(1<<ui32Level)+ui32X];
		assert( pqtnNode->GetCenter().x - pqtnNode->GetRadius() <= a2Box.m_vMin.x );
		assert( pqtnNode->GetCenter().x + pqtnNode->GetRadius() >= a2Box.m_vMax.x );
		assert( pqtnNode->GetCenter().y - pqtnNode->GetRadius() <= a2Box.m_vMin.y );
		assert( pqtnNode->GetCenter().y + pqtnNode->GetRadius() >= a2Box.m_vMax.y );
#endif	// #ifdef _DEBUG
		return m_pqtnLevels[ui32Level][ui32Y*(1<<ui32Level)+ui32X].AddObject( _pqtoObject );
	}

	/**
	 * Gets all objects in the quad-tree whose 2D AABB's overlap or are contained by the given
	 *	2D AABB.
	 *
	 * \param _a2Box The 2D AABB within which objects are tested.
	 * \param _vReturn The returned array of objects.
	 * \return Returns true if the full list of objects could be returned without encountering memory failures.
	 */
	LSBOOL LSE_CALL CQuadTree::FindObjects( const CAabb2d &_a2Box, CVector<CQuadTreeObject *, uint32_t> &_vReturn ) {
		// The root level is handled differently, since objects are added to the root node when they are completely
		//	outside of the quad-tree.  We basically just omit the check to see if the root node itself is within
		//	the given bounds, and each object in the root node will then be checked.
		return GatherObjectsInNode( Root(), _a2Box, _vReturn );
	}

	/**
	 * Gathers all objects in the node within the given bounds recursively.
	 *
	 * \param _pqtnNode The node in which to gather objects inside the given box.
	 * \param _a2Box The box.
	 * \param _vReturn The returned array of objects.
	 * \return Returns true if there were no memory failures.
	 */
	LSBOOL LSE_CALL CQuadTree::GatherObjectsInNode( const CQuadTreeNode * _pqtnNode, const CAabb2d &_a2Box, CVector<CQuadTreeObject *, uint32_t> &_vReturn ) {
		for ( const CSingleLinkedList<CQuadTreeObject *>::LSTL_LINKED_LIST_NODE * pllnThis = _pqtnNode->m_sllObjects.Head(); pllnThis; pllnThis = pllnThis->pllnLinkedListNext ) {
			if ( CTest::Aabb2dAabb2d( pllnThis->tLinkedListObject->QuadTreeAabb(), _a2Box ) ) {
				if ( !_vReturn.Push( pllnThis->tLinkedListObject ) ) {
					return false;
				}
			}
		}
		// Now check the child nodes of this node.
		if ( _pqtnNode->m_pqtnChildren[0] ) {
			for ( uint32_t I = 4; I--; ) {
				if ( NodeIsInBounds( _pqtnNode->m_pqtnChildren[I], _a2Box ) ) {
					if ( !GatherObjectsInNode( _pqtnNode->m_pqtnChildren[I], _a2Box, _vReturn ) ) { return false; }
				}
			}
		}
		return true;
	}

}	// namespace lsp
