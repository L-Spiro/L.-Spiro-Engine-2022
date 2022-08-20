/**
 * Copyright L. Spiro 2012
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A stack of animation layers.
 */

#include "LSXAnimationStack.h"


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CAnimationStack::CAnimationStack() {
	}

	// == Functions.
	/**
	 * Loads animation-stack information from an FBX animation stack.
	 *
	 * \param _pfasStack The stack from which to load information.
	 * \param _pfnNode The root node of the scene.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CAnimationStack::LoadStack( FbxAnimStack * _pfasStack, FbxNode * _pfnNode ) {
		m_sName = _pfasStack->GetName();
		LSUINT32 ui32Total = _pfasStack->GetMemberCount<FbxAnimLayer>();
		if ( !m_vLayers.Resize( ui32Total ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < ui32Total; ++I ) {
			if ( !m_vLayers[I].LoadLayer( _pfasStack->GetMember<FbxAnimLayer>( I ), _pfnNode ) ) { return false; }
		}
		return true;
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CAnimationStack::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// Save the name of the stack.
		if ( !CMisc::WriteString( m_sName, _bBuffer ) ) { return false; }
		// Write the number of layers.
		if ( !CMisc::WriteUInt( m_vLayers.Length(), _bBuffer ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < m_vLayers.Length(); ++I ) {
			if ( !m_vLayers[I].SaveToBuffer( _bBuffer ) ) { return false; }
		}
		return true;
	}

}	// namespace lsx
