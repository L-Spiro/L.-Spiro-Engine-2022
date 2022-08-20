/**
 * Copyright L. Spiro 2014
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 * If you wish to use this code in a commercial product contact yogurtemperor@hotmail.com.
 *
 * Description: Holds the joints in a skeleton.
 */


#ifndef __LSX_SKELETON_H__
#define __LSX_SKELETON_H__

#include "../LSXFbxConverter.h"
#include "../Misc/LSXMisc.h"
#include "Matrix/LSMMatrix4x4.h"
#include "String/LSTLString.h"
#include "Vector/LSTLVector.h"

namespace lsx {

	/**
	 * Class CSkeleton
	 * \brief Holds the joints in a skeleton
	 *
	 * Description: Holds the joints in a skeleton.
	 */
	class CSkeleton {
	public :
		// == Various constructors.
		LSE_CALL											CSkeleton();


		// == Types.
		class CJoint {
		public :
			// == Various constructors.
			LSE_CALL										CJoint();


			// == Functions.
			/**
			 * Extract the joint information from a node.
			 *
			 * \param _psNode The joint node whose information is to be extracted.
			 * \param _psScene The scene.
			 * \return Returns true if there was enough memory to extract all information.
			 */
			LSBOOL LSE_CALL									ExtractJoint( const FbxSkeleton * _psNode, FbxScene * _psScene );

			/**
			 * Save the data to a buffer.
			 *
			 * \param _bBuffer The buffer to which to save our data.
			 * \return Returns true only if all data is written to the buffer.
			 */
			LSBOOL LSE_CALL									SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer );

			/**
			 * Sets the index of the joint.  Children are set with increasing indices.
			 *
			 * \param _ui32Index The index to set.
			 */
			LSVOID LSE_CALL									SetIndex( LSUINT32 &_ui32Index );


			// == Members.
			/** World translation. */
			CMatrix4x4										m_mWorldTrans;

			/** Local translation. */
			CMatrix4x4										m_mLocalTrans;

			/** The name of this joint. */
			CString											m_sName;

			/** Our children. */
			CVector<CJoint, LSUINT16, 2>					m_vChildren;

			/** Index of this joint. */
			LSUINT32										m_ui32Index;

			/** Unique identifier of this joint. */
			LSUINT64										m_ui64Id;
		};


		// == Functions.
		/**
		 * Extract the skeleton from a given scene.
		 *
		 * \param _psScene The scene from which to extract the skeleton.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL										ExtractSkeleton( FbxScene * _psScene );

		/**
		 * Go to the bind pose.
		 *
		 * \param _psScene The scene.
		 */
		LSVOID LSE_CALL										GoToBindPose( FbxScene * _psScene );

		/**
		 * Save the data to a buffer.
		 *
		 * \param _bBuffer The buffer to which to save our data.
		 * \return Returns true only if all data is written to the buffer.
		 */
		LSBOOL LSE_CALL										SaveToBuffer( lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Gets a joint by name, if it exists.
		 *
		 * \param _pcName Name of the joint to get.
		 * \return Returns a constant pointer to the given joint.
		 */
		const CJoint * LSE_CALL								GetJoint( const LSCHAR * _pcName ) const;

		/**
		 * Gets the total number of joints in the skeleton.
		 *
		 * \return Returns the total number of joints in the skeleton.
		 */
		LSE_INLINE LSUINT32 LSE_CALL						TotalJoints() const;


	protected :
		// == Members.
		/**
		 * Our list of root nodes.
		 */
		CVector<CJoint>										m_vRoots;


		// == Functions.
		/**
		 * Get a joint by name.
		 *
		 * \param _sName Name of the joint to find.
		 * \return Returns the joint with the given name.
		 */
		CJoint * LSE_CALL									GetJointByName( const CString &_sName );

		/**
		 * Get a joint by name.
		 *
		 * \param _pjJoint The joint to search.
		 * \param _sName Name of the joint to find.
		 * \return Returns the joint with the given name.
		 */
		CJoint * LSE_CALL									GetJointByName( CJoint * _pjJoint, const CString &_sName );

		/**
		 * Gets the total number of joints in the skeleton.
		 *
		 * \param _pjJoint The joint to search.
		 * \return Returns the total number of joints in the skeleton.
		 */
		LSUINT32 LSE_CALL									TotalJoints( const CJoint * _pjJoint ) const;

		/**
		 * Convert joint world coordinates to local coordinates.
		 */
		LSVOID LSE_CALL										JointWorldToLocal();

		/**
		 * Convert a joint's and all of its children's world coordinates to local coordinates.
		 *
		 * \param _jJoint The joint to which to apply the changes.
		 * \param _pjParent Parent joint or NULL.
		 */
		LSVOID LSE_CALL										JointWorldToLocal( CJoint &_jJoint, CJoint * _pjParent );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total number of joints in the skeleton.
	 *
	 * \return Returns the total number of joints in the skeleton.
	 */
	 LSE_INLINE LSUINT32 LSE_CALL CSkeleton::TotalJoints() const {
		 LSUINT32 ui32Total = 0UL;
		 for ( LSUINT32 I = m_vRoots.Length(); I--; ) {
			 ui32Total += TotalJoints( &m_vRoots[I] );
		 }
		 return ui32Total;
	}

}	// namespace lsx

#endif	// __LSX_SKELETON_H__
