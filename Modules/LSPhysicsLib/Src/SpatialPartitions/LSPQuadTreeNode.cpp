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

#include "LSPQuadTreeNode.h"


namespace lsp {

	// == Various constructors.
	LSE_CALLCTOR CQuadTreeNode::CQuadTreeNode() {
		m_pqtnChildren[0] = m_pqtnChildren[1] = m_pqtnChildren[2] = m_pqtnChildren[3] = NULL;
	}
	LSE_CALLCTOR CQuadTreeNode::~CQuadTreeNode() {
		for ( CSingleLinkedList<CQuadTreeObject *>::LPLSTL_LINKED_LIST_NODE lpllnThis = m_sllObjects.Head();
			lpllnThis; lpllnThis = lpllnThis->pllnLinkedListNext ) {
			lpllnThis->tLinkedListObject->SetQuadTreeNode( NULL );
		}
	}

	// == Functions.
	/**
	 * Sets the center and radius of the node.
	 *
	 * \param _vPos Center of the node.
	 * \param _fRadius Radius of the node.
	 */
	void LSE_CALL CQuadTreeNode::SetCenterAndRadius( const CVector2 &_vPos, LSREAL _fRadius ) {
		m_vCenter = _vPos;
		m_fRadius = _fRadius;
	}

	/**
	 * Sets a child.
	 *
	 * \param _ui32Index Index of the child to set.
	 * \param _pqtnChild Pointer to the child node.
	 */
	void LSE_CALL CQuadTreeNode::SetChild( uint32_t _ui32Index, CQuadTreeNode * _pqtnChild ) {
		m_pqtnChildren[_ui32Index] = _pqtnChild;
	}

	/**
	 * Adds an object to the node.
	 *
	 * \param _pqtoObject The object to add to the node.
	 * \return Returns true if there were no memory allocation issues.
	 */
	LSBOOL LSE_CALL CQuadTreeNode::AddObject( CQuadTreeObject * _pqtoObject ) {
		if ( _pqtoObject->QuadTreeNode() == this ) { return true; }
		if ( m_sllObjects.PushFront( _pqtoObject ) ) {
			_pqtoObject->SetQuadTreeNode( this );
			return true;
		}
		return false;
	}

	/**
	 * Removes an object from the node.
	 *
	 * \param _pqtoObject The object to remove from the node.
	 */
	void LSE_CALL CQuadTreeNode::RemObject( CQuadTreeObject * _pqtoObject ) {
		m_sllObjects.RemoveItem( m_sllObjects.Find( _pqtoObject ) );
		_pqtoObject->SetQuadTreeNode( NULL );
	}

}	// namespace lsp
