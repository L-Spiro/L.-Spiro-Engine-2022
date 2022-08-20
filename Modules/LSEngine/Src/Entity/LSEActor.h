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
 * Description: An actor is an entity with an orientation.  It is the basis for all objects that can exist
 *	within the physical game world.  Most things inherit from this rather than from CEntity.
 */


#ifndef __LSE_ACTOR_H__
#define __LSE_ACTOR_H__

#include "../LSEEngine.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "LSEEntity.h"
#include "Orientation/LSMEulerOrientation.h"
#include "Orientation/LSMOrientation.h"
#include "SharedPtr/LSTLSharedPtr.h"

#pragma warning( push )

// warning C4324: 'lse::CActor' : structure was padded due to __declspec(align())
#pragma warning( disable : 4324 )

namespace lse {

	/**
	 * Class CActor
	 * \brief An actor is an entity with an orientation.
	 *
	 * Description: An actor is an entity with an orientation.  It is the basis for all objects that can exist
	 *	within the physical game world.  Most things inherit from this rather than from CEntity.
	 */
	class CActor : public CEntity {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CActor();
		virtual LSE_CALLCTOR					~CActor();


		// == Functions.
		/**
		 * Sets our parent.
		 *
		 * \param _peParent A pointer to our new parent.
		 */
		virtual void LSE_CALL					SetEntityParent( CEntity * _peParent );

		/**
		 * Gets the actor's unique ID.
		 *
		 * \return Returns the unique ID for this actor.
		 */
		LSE_INLINE uint32_t LSE_CALL			ActorId() const;

		/**
		 * Gets the local transform for this actor.
		 *
		 * \return Returns the local transform for this actor.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	LocalTrans() const;

		/**
		 * Gets the world transform for this actor.
		 *
		 * \return Returns the world transform for this actor.
		 */
		LSE_INLINE const CMatrix4x4 & LSE_CALL	WorldTrans() const;

		/**
		 * Gets the orientation of the object for read-only.
		 *
		 * \return Returns the orientation of this object.
		 */
		LSE_INLINE const COrientation & LSE_CALL
												GetOrientation() const;

		/**
		 * Gets the orientation of the object.
		 *
		 * \return Returns the orientation of this object.
		 */
		LSE_INLINE COrientation & LSE_CALL		Orientation();

		/**
		 * Dirty the children of this actor if necessary.
		 */
		void LSE_CALL							Propagate() const;

		/**
		 * Is the world transform dirty?
		 *
		 * \return Returns true if the world transform is in need of updating.
		 */
		LSE_INLINE LSBOOL LSE_CALL				IsWorldDirty() const;


	protected :
		// == Members.
		/** The local transform of this actor. */
		mutable CMatrix4x4						m_mLocalTrans;

		/** The world transform of this actor. */
		mutable CMatrix4x4						m_mWorldTrans;

		/** The local orientation of this actor. */
		COrientation							m_oOrientation;

		/** If true, the world transform is out-of-date. */
		mutable LSBOOL							m_bWorldDirty;

		/** If true, this actor inherits its transform from its parent.  This is almost
		 *	always the case.  If false, this actor's local transform is its world transform. */
		LSBOOL									m_bInheritsTrans;

		/** Unique ID for this actor. */
		uint32_t								m_ui32ActorId;

		/** Current ID counter. */
		static uint32_t							m_ui32ActorIdCounter;

		/** Actor count.  When this reaches 0, the ID counter can be set back to 0. */
		static uint32_t							m_ui32ActorCounter;

		/** Critical section to guard the ID counters. */
		static CCriticalSection					m_csCrit;


		// == Functions.
		/**
		 * Undirty the local transform.
		 */
		virtual void LSE_CALL					UndirtyLocal() const;

		/**
		 * Inform child classes that the local transform is about to change.
		 */
		virtual void LSE_CALL					LocalWillChange() const;

		/**
		 * Inform child classes that the local transform has changed.
		 */
		virtual void LSE_CALL					LocalDidChange() const;

		/**
		 * Undirty the world transform.
		 */
		void LSE_CALL							UndirtyWorld() const;

		/**
		 * Inform child classes that the world transform is about to change.
		 */
		virtual void LSE_CALL					WorldWillChange() const;

		/**
		 * Inform child classes that the world transform has changed.
		 */
		virtual void LSE_CALL					WorldDidChange() const;

		/**
		 * Causes all childrens' world dirty flag to be set to true.  Should be called any time our local rotation changes.
		 */
		void LSE_CALL							DirtyChildrenWorldTrans() const;


	private :
		typedef CEntity							Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to an actor.
	 */
	typedef CSharedPtr<CActor>					CActorPtr;

	// == Functions.
	/**
	 * Gets the actor's unique ID.
	 *
	 * \return Returns the unique ID for this actor.
	 */
	LSE_INLINE uint32_t LSE_CALL CActor::ActorId() const {
		return m_ui32ActorId;
	}

	/**
	 * Gets the local transform for this actor.
	 *
	 * \return Returns the local transform for this actor.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CActor::LocalTrans() const {
		UndirtyLocal();
		return m_mLocalTrans;
	}

	/**
	 * Gets the world transform for this actor.
	 *
	 * \return Returns the world transform for this actor.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CActor::WorldTrans() const {
		if ( m_bInheritsTrans ) {
			UndirtyWorld();
			return m_mWorldTrans;
		}
		return LocalTrans();
	}

	/**
	 * Gets the orientation of the object for read-only.
	 *
	 * \return Returns the orientation of this object.
	 */
	LSE_INLINE const COrientation & LSE_CALL CActor::GetOrientation() const {
		return m_oOrientation;
	}
	
	/**
	 * Gets the orientation of the object.
	 *
	 * \return Returns the orientation of this object.
	 */
	LSE_INLINE COrientation & LSE_CALL CActor::Orientation() {
		m_bWorldDirty = true;
		return m_oOrientation;
	}

	/**
	 * Is the world transform dirty?
	 *
	 * \return Returns true if the world transform is in need of updating.
	 */
	LSE_INLINE LSBOOL LSE_CALL CActor::IsWorldDirty() const {
		return m_bWorldDirty;
	}

}	// namespace lse

#pragma warning( pop )

#endif	// __LSE_ACTOR_H__
