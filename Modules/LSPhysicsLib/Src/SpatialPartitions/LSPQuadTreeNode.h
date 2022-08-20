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
 * Description: A node in a quad-tree.
 */


#ifndef __LSP_QUADTREENODE_H__
#define __LSP_QUADTREENODE_H__

#include "../LSPPhysicsLib.h"
#include "LinkedList/LSTLSingleLinkedList.h"
#include "LSPQuadTreeObject.h"
#include "Vector/LSMVector2.h"

namespace lsp {

	/**
	 * Class CQuadTreeNode
	 * \brief A node in a quad-tree.
	 *
	 * Description: A node in a quad-tree.
	 */
	class CQuadTreeNode {
		friend class							CQuadTree;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CQuadTreeNode();
		LSE_CALLCTOR							~CQuadTreeNode();


		// == Functions.
		/**
		 * Sets the center and radius of the node.
		 *
		 * \param _vPos Center of the node.
		 * \param _fRadius Radius of the node.
		 */
		void LSE_CALL							SetCenterAndRadius( const CVector2 &_vPos, LSREAL _fRadius );

		/**
		 * Gets the center of the node.
		 *
		 * \return Returns the center of the node.
		 */
		LSE_INLINE const CVector2 & LSE_CALL	GetCenter() const;

		/**
		 * Gets the radius of the node.
		 *
		 * \return Returns the radius of the node.
		 */
		LSE_INLINE LSREAL LSE_CALL				GetRadius() const;

		/**
		 * Sets a child.
		 *
		 * \param _ui32Index Index of the child to set.
		 * \param _pqtnChild Pointer to the child node.
		 */
		void LSE_CALL							SetChild( uint32_t _ui32Index, CQuadTreeNode * _pqtnChild );

		/**
		 * Adds an object to the node.
		 *
		 * \param _pqtoObject The object to add to the node.
		 * \return Returns true if there were no memory allocation issues.
		 */
		LSBOOL LSE_CALL							AddObject( CQuadTreeObject * _pqtoObject );

		/**
		 * Removes an object from the node.
		 *
		 * \param _pqtoObject The object to remove from the node.
		 */
		void LSE_CALL							RemObject( CQuadTreeObject * _pqtoObject );

	protected :
		// == Members.
		/**
		 * Linked list of objects in this node.
		 */
		CSingleLinkedList<CQuadTreeObject *>	m_sllObjects;

		/**
		 * The 4 children of this node.
		 */
		CQuadTreeNode *							m_pqtnChildren[4];

		/**
		 * The center of the node.
		 */
		CVector2								m_vCenter;

		/**
		 * The radius of the node.
		 */
		LSREAL									m_fRadius;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the center of the node.
	 *
	 * \return Returns the center of the node.
	 */
	LSE_INLINE const CVector2 & LSE_CALL CQuadTreeNode::GetCenter() const {
		return m_vCenter;
	}

	/**
	 * Gets the radius of the node.
	 *
	 * \return Returns the radius of the node.
	 */
	LSE_INLINE LSREAL LSE_CALL CQuadTreeNode::GetRadius() const {
		return m_fRadius;
	}

}	// namespace lsp

#endif	// __LSP_QUADTREENODE_H__
