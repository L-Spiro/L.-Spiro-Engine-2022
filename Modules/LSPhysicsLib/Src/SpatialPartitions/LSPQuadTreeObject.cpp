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

#include "LSPQuadTreeObject.h"
#include "LSPQuadTreeNode.h"


namespace lsp {

	// == Various constructors.
	LSE_CALLCTOR CQuadTreeObject::CQuadTreeObject() :
		m_pqtnNode( NULL ) {
	}
	LSE_CALLCTOR CQuadTreeObject::~CQuadTreeObject() {
		SetQuadTreeNode( NULL );
	}

	// == Functions.
	/**
	 * Sets the quad-tree node into which this object has been inserted.
	 *
	 * \param _pqtnNode The node into which this object has been inserted.
	 */
	void LSE_CALL CQuadTreeObject::SetQuadTreeNode( CQuadTreeNode * _pqtnNode ) {
		if ( m_pqtnNode == _pqtnNode ) { return; }
		if ( m_pqtnNode ) {
			CQuadTreeNode * pqtnParent = m_pqtnNode;
			m_pqtnNode = NULL;
			pqtnParent->RemObject( this );
		}
		m_pqtnNode = _pqtnNode;
		if ( m_pqtnNode ) {
			m_pqtnNode->AddObject( this );
		}
	}

}	// namespace lsp
