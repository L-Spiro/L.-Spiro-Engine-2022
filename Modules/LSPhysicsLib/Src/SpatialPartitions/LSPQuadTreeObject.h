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
 * Description: An object that can be fit into a quad-tree must inherit from this.
 */


#ifndef __LSP_QUADTREEOBJECT_H__
#define __LSP_QUADTREEOBJECT_H__

#include "../LSPPhysicsLib.h"
#include "../BoundingBoxes/LSPAabb2d.h"

namespace lsp {

	class CQuadTreeNode;

	/**
	 * Class CQuadTreeObject
	 * \brief An object that can be fit into a quad-tree must inherit from this.
	 *
	 * Description: An object that can be fit into a quad-tree must inherit from this.
	 */
	class CQuadTreeObject {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CQuadTreeObject();
		LSE_CALLCTOR						~CQuadTreeObject();


		// == Functions.
		/**
		 * Sets the quad-tree node into which this object has been inserted.
		 *
		 * \param _pqtnNode The node into which this object has been inserted.
		 */
		void LSE_CALL						SetQuadTreeNode( CQuadTreeNode * _pqtnNode );

		/**
		 * Must be overloaded to return a constant reference to a 2D AABB.
		 *
		 * \return Returns a constant reference to a 2D AABB.
		 */
		virtual const CAabb2d & LSE_CALL	QuadTreeAabb() const = 0;

		/**
		 * Gets the quad-tree node into which this object is inserted.
		 *
		 * \return Returns the quad-tree node into which this object is inserted, if any.
		 */
		LSE_INLINE const CQuadTreeNode * LSE_CALL
											QuadTreeNode() const;

	protected :
		// == Members.
		/**
		 * The node to which this object has been added.
		 */
		CQuadTreeNode *						m_pqtnNode;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the quad-tree node into which this object is inserted.
	 *
	 * \return Returns the quad-tree node into which this object is inserted, if any.
	 */
	LSE_INLINE const CQuadTreeNode * LSE_CALL CQuadTreeObject::QuadTreeNode() const {
		return m_pqtnNode;
	}

}	// namespace lsp

#endif	// __LSP_QUADTREEOBJECT_H__
