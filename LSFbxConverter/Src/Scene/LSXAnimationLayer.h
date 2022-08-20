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
 * Description: A layer of animations is a collection of tracks.
 */


#ifndef __LSX_ANIMATIONLAYER_H__
#define __LSX_ANIMATIONLAYER_H__

#include "../LSXFbxConverter.h"
#include "../Misc/LSXMisc.h"
#include "LSXAnimationTrack.h"
#include "Vector/LSTLVector.h"

namespace lsx {

	/**
	 * Class CAnimationLayer
	 * \brief A layer of animations is a collection of tracks.
	 *
	 * Description: A layer of animations is a collection of tracks.
	 */
	class CAnimationLayer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CAnimationLayer();


		// == Functions.
		/**
		 * Loads an animation layer from an FBX animation layer and a node.  Internally called
		 *	recursively.
		 *
		 * \param _pfasStack The animation layer to load.
		 * \param _pfnNode The node from which to load animation data.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL							LoadLayer( FbxAnimLayer * _pfalLayer, FbxNode * _pfnNode );

		/**
		 * Save the data to a buffer.
		 *
		 * \param _bBuffer The buffer to which to save our data.
		 * \return Returns true only if all data is written to the buffer.
		 */
		LSBOOL LSE_CALL							SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer );


	protected :
		// == Members.
		/**
		 * Array of tracks for this animation layer.
		 */
		CVector<CAnimationTrack>				m_vTracks;

		/**
		 * Name of the layer.
		 */
		CString									m_sName;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsx

#endif	// __LSX_ANIMATIONLAYER_H__
