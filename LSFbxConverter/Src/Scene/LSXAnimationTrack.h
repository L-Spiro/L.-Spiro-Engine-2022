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
 * Description: The FBX SDK calls these curves.  A track/curve is a collection of values at keyframes, nothing more.
 */


#ifndef __LSX_ANIMATIONTRACK_H__
#define __LSX_ANIMATIONTRACK_H__

#include "../LSXFbxConverter.h"
#include "../Misc/LSXMisc.h"
#include "Models/LSDAnimationAttributes.h"
#include "Set/LSTLSet.h"

namespace lsx {

	/**
	 * Class CAnimationTrack
	 * \brief The FBX SDK calls these curves.
	 *
	 * Description: The FBX SDK calls these curves.  A track/curve is a collection of values at keyframes, nothing more.
	 */
	class CAnimationTrack {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CAnimationTrack();
		LSE_CALLCTOR							~CAnimationTrack();


		// == Types.
		/**
		 * A key and value pair.
		 */
		typedef struct LSX_KEY_FRAME {
			/**
			 * The time of the keyframe.
			 */
			FbxTime								tTime;

			/**
			 * The value at the time of the keyframe.
			 */
			LSFLOAT								fValue;


			// == Operators.
			/**
			 * Less-than operator.
			 *
			 * \param _kfOther The keyframe against which to compare.
			 * \return Returns true if the time of this keyframe is less than the time of the given
			 *	keyframe.
			 */
			LSBOOL LSE_CALL						operator < ( const LSX_KEY_FRAME &_kfOther ) const;

			/**
			 * Equality operator.
			 *
			 * \param _kfOther The keyframe against which to compare.
			 * \return Returns true if the time of this keyframe is equal to the time of the given
			 *	keyframe.
			 */
			LSBOOL LSE_CALL						operator == ( const LSX_KEY_FRAME &_kfOther ) const;
		} * LPLSX_KEY_FRAME, * const LPCLSX_KEY_FRAME;


		// == Functions.
		/**
		 * Adds the given value at the given time to this track.
		 *
		 * \param _tTime The time of the keyframe to set.  If it already exists it will be overwritten.
		 * \param _fValue The value to set at the given time.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSE_INLINE LSBOOL LSE_CALL				SetKeyFrame( FbxTime _tTime, LSFLOAT _fValue );

		/**
		 * Gets the set of keyframes for reading only.
		 *
		 * \return Returns a read-only reference to the keyframes for this track.
		 */
		LSE_INLINE const CSet<LSX_KEY_FRAME> & LSE_CALL
												GetKeyFrames() const;

		/**
		 * Sets the node and attribute type for this curve.
		 *
		 * \param _pfnNode The node effected by this track.
		 * \param _ui32Attribute The attribute of that node affected by this track.
		 */
		LSE_INLINE LSVOID LSE_CALL				SetAttributes( FbxNode * _pfnNode, LSUINT32 _ui32Attribute );

		/**
		 * Loads data from an FBX animation curve.
		 *
		 * \param _pfacCurve The curve from which to load keyframes.
		 * \param _pfnNode The node effected by this track.
		 * \param _ui32Attribute The attribute of that node affected by this track.
		 * \return Returns true if there are no memory failures.
		 */
		LSBOOL LSE_CALL							Load( FbxAnimCurve * _pfacCurve, FbxNode * _pfnNode, LSUINT32 _ui32Attribute );

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
		 * Our set of keyframes.
		 */
		CSet<LSX_KEY_FRAME>					m_sKeyFrames;

		/**
		 * The node associated with this track.
		 */
		FbxNode *								m_pfnNode;

		/**
		 * Attribute.
		 */
		LSUINT32								m_ui32Attribute;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Adds the given value at the given time to this track.
	 *
	 * \param _tTime The time of the keyframe to set.  If it already exists it will be overwritten.
	 * \param _fValue The value to set at the given time.
	 * \return Returns true if there was enough memory to complete the operation.
	 */
	LSE_INLINE LSBOOL LSE_CALL CAnimationTrack::SetKeyFrame( FbxTime _tTime, LSFLOAT _fValue ) {
		LSX_KEY_FRAME kfInsertMe = {
			_tTime,
			_fValue
		};
		return m_sKeyFrames.Insert( kfInsertMe );
	}

	/**
	 * Gets the set of keyframes for reading only.
	 *
	 * \return Returns a read-only reference to the keyframes for this track.
	 */
	LSE_INLINE const CSet<CAnimationTrack::LSX_KEY_FRAME> & LSE_CALL CAnimationTrack::GetKeyFrames() const {
		return m_sKeyFrames;
	}

	/**
	 * Sets the node and attribute type for this curve.
	 *
	 * \param _pfnNode The node effected by this track.
	 * \param _ui32Attribute The attribute of that node affected by this track.
	 */
	LSE_INLINE LSVOID LSE_CALL CAnimationTrack::SetAttributes( FbxNode * _pfnNode, LSUINT32 _ui32Attribute ) {
		m_pfnNode = _pfnNode;
		m_ui32Attribute = _ui32Attribute;
	}

}	// namespace lsx

#endif	// __LSX_ANIMATIONTRACK_H__
