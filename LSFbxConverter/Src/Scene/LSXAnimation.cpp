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
 * Description: The animations in the loaded scene.
 */

#include "LSXAnimation.h"
#include "LSXScene.h"


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR CAnimation::CAnimation() {
	}

	// == Functions.
	/**
	 * Loads the animation data from the given FBX scene.
	 *
	 * \param _pfsScene The scene from which to load the animation data.
	 * \return Returns true if there are no memory failures.
	 */
	LSBOOL LSE_CALL CAnimation::LoadAnimations( FbxScene * _pfsScene ) {
		LSUINT32 ui32Total = _pfsScene->GetSrcObjectCount<FbxAnimStack>();
		if ( !m_vStacks.Resize( ui32Total ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < ui32Total; ++I ) {
			if ( !m_vStacks[I].LoadStack( _pfsScene->GetSrcObject<FbxAnimStack>( I ), _pfsScene->GetRootNode() ) ) { return false; }
		}

		// Load the bind pose.
		if ( !m_sBindPose.ExtractSkeleton( _pfsScene ) ) { return false; }
		m_sBindPose.GoToBindPose( _pfsScene );
		return true;
	}

	/**
	 * Save the data to a buffer.
	 *
	 * \param _bBuffer The buffer to which to save our data.
	 * \return Returns true only if all data is written to the buffer.
	 */
	LSBOOL LSE_CALL CAnimation::SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		// Save the animation header.
		if ( !CMisc::WriteUInt( LSX_ANIM_HEADER, _bBuffer ) ) { return false; }
		// Export the bind pose.
		if ( !m_sBindPose.SaveToBuffer( _bBuffer ) ) { return false; }
		// Write the number of stacks.
		if ( !CMisc::WriteUInt( m_vStacks.Length(), _bBuffer ) ) { return false; }
		for ( LSUINT32 I = 0UL; I < m_vStacks.Length(); ++I ) {
			if ( !m_vStacks[I].SaveToBuffer( _bBuffer ) ) { return false; }
		}
		return true;
	}

}	// namespace lsx
