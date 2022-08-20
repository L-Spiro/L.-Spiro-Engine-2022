/**
 * Copyright L. Spiro 2016
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
 *	within the physical game world.  Most things inherit from this rather than from CEntity2d.
 */


#ifndef __LSR_ACTOR2D_H__
#define __LSR_ACTOR2D_H__

#include "../LSRSpriteLib.h"
#include "CriticalSection/LSHCriticalSection.h"
#include "LSREntity2d.h"
#include "Orientation/LSMOrientation2d.h"
#include "SharedPtr/LSTLSharedPtr.h"

#pragma warning( push )

// warning C4324: 'lsr::CActor2d' : structure was padded due to __declspec(align())
#pragma warning( disable : 4324 )

namespace lsr {

	/**
	 * Class CActor2d
	 * \brief An actor is an entity with an orientation.
	 *
	 * Description: An actor is an entity with an orientation.  It is the basis for all objects that can exist
	 *	within the physical game world.  Most things inherit from this rather than from CEntity2d.
	 */
	class CActor2d : public CEntity2d {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CActor2d();
		virtual LSE_CALLCTOR					~CActor2d();


		// == Functions.
		/**
		 * Sets our parent.
		 *
		 * \param _peParent A pointer to our new parent.
		 */
		virtual void LSE_CALL					SetEntityParent( CEntity2d * _peParent );

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
		LSE_INLINE const COrientation2d & LSE_CALL
												GetOrientation() const;

		/**
		 * Gets the orientation of the object.
		 *
		 * \return Returns the orientation of this object.
		 */
		LSE_INLINE COrientation2d & LSE_CALL	Orientation();

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

		/**
		 * Gets the depth of the sprite.
		 *
		 * \param Returns the depth of the sprite.
		 */
		LSE_INLINE LSREAL LSE_FCALL				GetDepth() const;

		/**
		 * Gets the depth of the sprite.
		 *
		 * \return Returns the depth of the sprite.
		 */
		LSE_INLINE LSREAL & LSE_CALL			Depth();


	protected :
		// == Members.
		/** The local transform of this actor. */
		mutable CMatrix4x4						m_mLocalTrans;

		/** The world transform of this actor. */
		mutable CMatrix4x4						m_mWorldTrans;

		/** The local orientation of this actor. */
		COrientation2d							m_oOrientation2d;

		/** If true, the world transform is out-of-date. */
		mutable LSBOOL							m_bWorldDirty;

		/** If true, this actor inherits its transform from its parent.  This is almost
		 *	always the case.  If false, this actor's local transform is its world transform. */
		LSBOOL									m_bInheritsTrans;

		/** Unique ID for this actor. */
		uint32_t								m_ui32ActorId;

		/** The Z (depth) value of the sprite. */
		LSREAL									m_fZ;

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
		typedef CEntity2d						Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/**
	 * A shared pointer to an actor.
	 */
	typedef CSharedPtr<CActor2d>				CActor2dPtr;

	// == Functions.
	/**
	 * Gets the actor's unique ID.
	 *
	 * \return Returns the unique ID for this actor.
	 */
	LSE_INLINE uint32_t LSE_CALL CActor2d::ActorId() const {
		return m_ui32ActorId;
	}

	/**
	 * Gets the local transform for this actor.
	 *
	 * \return Returns the local transform for this actor.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CActor2d::LocalTrans() const {
		UndirtyLocal();
		return m_mLocalTrans;
	}

	/**
	 * Gets the world transform for this actor.
	 *
	 * \return Returns the world transform for this actor.
	 */
	LSE_INLINE const CMatrix4x4 & LSE_CALL CActor2d::WorldTrans() const {
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
	LSE_INLINE const COrientation2d & LSE_CALL CActor2d::GetOrientation() const {
		return m_oOrientation2d;
	}

	/**
	 * Gets the orientation of the object.
	 *
	 * \return Returns the orientation of this object.
	 */
	LSE_INLINE COrientation2d & LSE_CALL CActor2d::Orientation() {
		m_bWorldDirty = true;
		return m_oOrientation2d;
	}

	/**
	 * Is the world transform dirty?
	 *
	 * \return Returns true if the world transform is in need of updating.
	 */
	LSE_INLINE LSBOOL LSE_CALL CActor2d::IsWorldDirty() const {
		return m_bWorldDirty;
	}

	/**
	* Gets the depth of the sprite.
	*
	* \param Returns the depth of the sprite.
	*/
	LSE_INLINE LSREAL LSE_FCALL CActor2d::GetDepth() const {
		return m_fZ;
	}

	/**
	 * Gets the depth of the sprite.
	 *
	 * \return Returns the depth of the sprite.
	 */
	LSE_INLINE LSREAL & LSE_CALL CActor2d::Depth() {
		return m_fZ;
	}

}	// namespace lsr

#pragma warning( pop )

#endif	// __LSR_ACTOR2D_H__
