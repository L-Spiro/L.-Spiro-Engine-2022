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
 * Description: An instance of a sub-mesh.
 */

#include "LSESubMesh.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CSubMesh::CSubMesh() :
		m_psmSubMesh( NULL ) {
	}
	LSE_CALLCTOR CSubMesh::~CSubMesh() {
	}

	// == Functions.
	/**
	 * Sets the sub-mesh reference.
	 *
	 * \param _psmSubMesh The sub-mesh to set.
	 */
	void LSE_CALL CSubMesh::SetSubMesh( const lsd::CSubMesh * _psmSubMesh ) {
		m_psmSubMesh = _psmSubMesh;
		if ( m_psmSubMesh && m_psmSubMesh->Material() ) {
			m_mmMaterial = (*m_psmSubMesh->Material());
		}
		else {
			// Reset the material.
			m_mmMaterial.~CModelMaterial();
			new( &m_mmMaterial ) CModelMaterial();
		}
		m_mmMaterial.UpdateRenderStates();
	}

}	// namespace lse
