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


#ifndef __LSP_QUADTREE_H__
#define __LSP_QUADTREE_H__

#include "../LSPPhysicsLib.h"
#include "LSPQuadTreeNode.h"
#include "Vector/LSTLVector.h"

namespace lsp {

	/**
	 * Class CQuadTree
	 * \brief An 8-layer quad-tree with fast insertion times.
	 *
	 * Description: An 8-layer quad-tree with fast insertion times.  Uses a bit hack to increase insertion speed
	 *	for high-performance use.
	 */
	class CQuadTree {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CQuadTree();
		LSE_CALLCTOR							~CQuadTree();


		// == Functions.
		/**
		 * Creates the quad-tree with the given size.  The created quad-tree has 8 nested layers.
		 *
		 * \param _fSize The size of the quad-tree to create.
		 * \return Returns true if there were no memory failures during the creation of the quad-tree.
		 */
		LSBOOL LSE_CALL							CreateQuadTree( LSREAL _fSize );

		/**
		 * Resets the quad-tree back to nothing.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets the root node of the tree, which can be NULL.
		 *
		 * \return Returns the root node of the tree, which can be NULL.
		 */
		const CQuadTreeNode * LSE_CALL			Root() const;

		/**
		 * Inserts an object into the quad-tree.  Any items not fully inside the bounds of the quad-tree
		 *	are inserted at the root level.  The insertion process is extremely efficient.
		 *
		 * \param _pqtoObject The quad-tree-object to insert.
		 * \return Returns true if there are no memory failures during the insert.
		 */
		LSBOOL LSE_CALL							InsertObject( CQuadTreeObject * _pqtoObject );

		/**
		 * Gets all objects in the quad-tree whose 2D AABB's overlap or are contained by the given
		 *	2D AABB.
		 *
		 * \param _a2Box The 2D AABB within which objects are tested.
		 * \param _vReturn The returned array of objects.
		 * \return Returns true if the full list of objects could be returned without encountering memory failures.
		 */
		LSBOOL LSE_CALL							FindObjects( const CAabb2d &_a2Box, CVector<CQuadTreeObject *, uint32_t> &_vReturn );


	protected :
		// == Members.
		/**
		 * Pointer to the nodes of the quad-tree.  The first node encompasses the
		 *	entire bounding box of this tree.
		 */
		uint8_t *								m_pui8Nodes;

		/**
		 * Pointers to each level of the tree, with index 0 being the root node.
		 */
		CQuadTreeNode *							m_pqtnLevels[8];

		/**
		 * The radius of this quad-tree.
		 */
		LSREAL									m_fRadius;

		/**
		 * The inverse radius of the quad-tree.
		 */
		LSREAL									m_fInvRadius;


		// == Functions.
		/**
		 * Gets the root node of the tree, which can be NULL.
		 *
		 * \return Returns the root node of the tree, which can be NULL.
		 */
		LSE_INLINE CQuadTreeNode * LSE_CALL		Root();

		/**
		 * Is the given node within the bounds specified by the given 2D AABB?
		 *
		 * \param _pqtnNode The node to check for being within the given bounds.
		 * \param _a2Box The 2D AABB specifying the bounds.
		 * \return Returns true if the node is contained by or intersecting the given 2D AABB.
		 */
		static LSE_INLINE LSBOOL LSE_CALL		NodeIsInBounds( const CQuadTreeNode * _pqtnNode, const CAabb2d &_a2Box );

		/**
		 * Gathers all objects in the node within the given bounds recursively.
		 *
		 * \param _pqtnNode The node in which to gather objects inside the given box.
		 * \param _a2Box The box.
		 * \param _vReturn The returned array of objects.
		 * \return Returns true if there were no memory failures.
		 */
		static LSBOOL LSE_CALL					GatherObjectsInNode( const CQuadTreeNode * _pqtnNode, const CAabb2d &_a2Box, CVector<CQuadTreeObject *, uint32_t> &_vReturn );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the root node of the tree, which can be NULL.
	 *
	 * \return Returns the root node of the tree, which can be NULL.
	 */
	LSE_INLINE const CQuadTreeNode * LSE_CALL CQuadTree::Root() const { return reinterpret_cast<CQuadTreeNode *>(m_pui8Nodes); }

	/**
	 * Gets the root node of the tree, which can be NULL.
	 *
	 * \return Returns the root node of the tree, which can be NULL.
	 */
	LSE_INLINE CQuadTreeNode * LSE_CALL CQuadTree::Root() { return reinterpret_cast<CQuadTreeNode *>(m_pui8Nodes); }

	/**
	 * Is the given node within the bounds specified by the given 2D AABB?
	 *
	 * \param _pqtnNode The node to check for being within the given bounds.
	 * \param _a2Box The 2D AABB specifying the bounds.
	 * \return Returns true if the node is contained by or intersecting the given 2D AABB.
	 */
	LSE_INLINE LSBOOL LSE_CALL CQuadTree::NodeIsInBounds( const CQuadTreeNode * _pqtnNode, const CAabb2d &_a2Box ) {
		LSREAL fRad = _pqtnNode->GetRadius();
		// TODO: Try to vectorize this.
		return (_pqtnNode->GetCenter().x - fRad <= _a2Box.m_vMax.x) && (_pqtnNode->GetCenter().y - fRad <= _a2Box.m_vMax.y) &&
			(_pqtnNode->GetCenter().x + fRad >= _a2Box.m_vMin.x) && (_pqtnNode->GetCenter().y + fRad >= _a2Box.m_vMin.y);
	}

}	// namespace lsp

#endif	// __LSP_QUADTREE_H__
