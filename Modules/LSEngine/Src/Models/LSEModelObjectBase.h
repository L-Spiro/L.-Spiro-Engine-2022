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
 * Description: The base class for all model objects (any object loaded from a standard model file).
 */


#ifndef __LSE_MODELOBJECTBASE_H__
#define __LSE_MODELOBJECTBASE_H__

#include "../LSEEngine.h"
#include "../Entity/LSEActor.h"
#include "BoundingBoxes/LSPAabb.h"
#include "Models/LSDMeshHierarchy.h"

namespace lse {

	/**
	 * Class CModelObjectBase
	 * \brief The base class for all model objects (any object loaded from a standard model file).
	 *
	 * Description: The base class for all model objects (any object loaded from a standard model file).
	 */
	class CModelObjectBase : public CActor {
		friend class							CModelInstance;
	public :
		// == Various constructors.
		LSE_CALLCTOR							CModelObjectBase();
		virtual LSE_CALLCTOR					~CModelObjectBase();


		// == Functions.
		/**
		 * Gets the object ID.
		 *
		 * \return Returns the ID of the shared model object from which this object was created.
		 */
		LSE_INLINE uint64_t LSE_CALL			ObjectId() const;

		/**
		 * Gets the parent ID.
		 *
		 * \return Returns the parent ID.
		 */
		LSE_INLINE uint64_t LSE_CALL			ParentId() const;

		/**
		 * Sets the name of the object.  The input value is not copied and must remain a valid pointer
		 *	during the lifetime of this object.
		 *
		 * \param _pcName The name of the object.
		 */
		LSE_INLINE void LSE_CALL				SetName( const char * _pcName );

		/**
		 * Gets a constant reference to the object's bounding box.
		 *
		 * \return Returns a constant reference to the object's bounding box.
		 */
		LSE_INLINE const CAabb & LSE_CALL		Aabb() const;



	protected :
		// == Members.
		/** Post-multiply orientation. */
		mutable CMatrix4x4						m_mPostOrientation;

		/** ID (based on the shared model data). */
		uint64_t								m_ui64Id;

		/** Parent ID (based on the shared model data). */
		uint64_t								m_ui64ParentId;

		/** Scale pivot. */
		CVector3								m_vScalePivot;

		/** Scale. */
		CVector3								m_vScale;

		/** Scale offset. */
		CVector3								m_vScaleOffset;

		/** Rotation pivot point. */
		CVector3								m_vRotPivot;

		/** Post rotation. */
		CVector3								m_vPostRot;

		/** Rotation. */
		CVector3								m_vRot;

		/** Pre-rotation. */
		CVector3								m_vPreRot;

		/** Rotation offset. */
		CVector3								m_vRotOffset;

		/** Translation. */
		CVector3								m_vTranslation;

		/** Axis-aligned bounding box. */
		mutable CAabb							m_aAabb;

		/** Name. */
		const char *							m_pcName;


		// == Functions.
		/**
		 * Sets the data based off the given hierarchy structure from a shared model.
		 *
		 * \param _mhHier the hierarchy structure to clone.
		 */
		void LSE_CALL							CreateBaseData( const LSD_MESH_HIERARCHY &_mhHier );

		/**
		 * Undirty the local transform.
		 */
		virtual void LSE_CALL					UndirtyLocal() const;


	private :
		typedef CActor							Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the object ID.
	 *
	 * \return Returns the ID of the shared model object from which this object was created.
	 */
	LSE_INLINE uint64_t LSE_CALL CModelObjectBase::ObjectId() const { return m_ui64Id; }

	/**
	 * Gets the parent ID.
	 *
	 * \return Returns the parent ID.
	 */
	LSE_INLINE uint64_t LSE_CALL CModelObjectBase::ParentId() const { return m_ui64ParentId; }

	/**
	 * Sets the name of the object.  The input value is not copied and must remain a valid pointer
	 *	during the lifetime of this object.
	 *
	 * \param _pcName The name of the object.
	 */
	LSE_INLINE void LSE_CALL CModelObjectBase::SetName( const char * _pcName ) {
		m_pcName = _pcName;
	}

	/**
	 * Gets a constant reference to the object's bounding box.
	 *
	 * \return Returns a constant reference to the object's bounding box.
	 */
	LSE_INLINE const CAabb & LSE_CALL CModelObjectBase::Aabb() const {
		UndirtyWorld();
		return m_aAabb;
	}

}	// namespace lse

#endif	// __LSE_MODELOBJECTBASE_H__
