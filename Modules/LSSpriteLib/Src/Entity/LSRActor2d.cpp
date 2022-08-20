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

#include "LSRActor2d.h"


namespace lsr {

	// == Members.
	/** Current ID counter. */
	uint32_t CActor2d::m_ui32ActorIdCounter = 0UL;

	/** Actor count.  When this reaches 0, the ID counter can be set back to 0. */
	uint32_t CActor2d::m_ui32ActorCounter = 0UL;

	/** Critical section to guard the ID counters. */
	CCriticalSection CActor2d::m_csCrit;

	// == Various constructors.
	LSE_CALLCTOR CActor2d::CActor2d() :
		m_bWorldDirty( true ),
		m_bInheritsTrans( true ),
		m_fZ( LSM_ONE ) {
		m_ui32EntityType |= LSR_ET_ACTOR2D;
		m_mLocalTrans.Identity();
		m_mWorldTrans.Identity();

		m_csCrit.Lock();
		m_ui32ActorId = ++m_ui32ActorIdCounter;
		++m_ui32ActorCounter;
		m_csCrit.UnLock();
	}
	LSE_CALLCTOR CActor2d::~CActor2d() {
		DirtyChildrenWorldTrans();

		m_csCrit.Lock();
		assert( m_ui32ActorCounter );	// A 0 actor count?  Not possible.
		if ( --m_ui32ActorCounter == 0UL ) {
			// No actors alive.  Reset the counter.
			m_ui32ActorIdCounter = 0UL;
		}
		m_csCrit.UnLock();
	}

	// == Functions.
	/**
	 * Sets our parent.
	 *
	 * \param _peParent A pointer to our new parent.
	 */
	void LSE_CALL CActor2d::SetEntityParent( CEntity2d * _peParent ) {
		Parent::SetEntityParent( _peParent );
		DirtyChildrenWorldTrans();
	}

	/**
	 * Dirty the children of this actor if necessary.
	 */
	void LSE_CALL CActor2d::Propagate() const {
		if ( m_bWorldDirty ) {
			DirtyChildrenWorldTrans();
		}
	}

	/**
	 * Undirty the local transform.
	 */
	void LSE_CALL CActor2d::UndirtyLocal() const {
		if ( m_oOrientation2d.IsDirty() ) {
			LocalWillChange();

			m_oOrientation2d.UpdateMatrix( m_mLocalTrans, true, m_fZ );
			DirtyChildrenWorldTrans();

			LocalDidChange();
		}
	}

	/**
	 * Inform child classes that the local transform is about to change.
	 */
	void LSE_CALL CActor2d::LocalWillChange() const {
	}

	/**
	 * Inform child classes that the local transform has changed.
	 */
	void LSE_CALL CActor2d::LocalDidChange() const {
	}

	/**
	 * Undirty the world transform.
	 */
	void LSE_CALL CActor2d::UndirtyWorld() const {
		if ( m_bWorldDirty || GetOrientation().IsDirty() ) {
			WorldWillChange();

			if ( !m_peParent ) {
				// Optimize this case because it is very common.
				m_mWorldTrans = LocalTrans();
			}
			else {
				// Call LocalTrans() to undirty the local translation if necessary.
				//if ( m_peParent->EntityType() & LSE_ET_ACTOR ) {
				m_mWorldTrans = LocalTrans() * reinterpret_cast<const CActor2d *>(m_peParent)->WorldTrans();
				/*}
				else {
				// Chain is broken.
				m_mWorldTrans = LocalTrans();
				}*/
			}
			m_bWorldDirty = false;

			WorldDidChange();
		}
	}

	/**
	 * Inform child classes that the world transform is about to change.
	 */
	void LSE_CALL CActor2d::WorldWillChange() const {
	}

	/**
	 * Inform child classes that the world transform has changed.
	 */
	void LSE_CALL CActor2d::WorldDidChange() const {
	}

	/**
	 * Causes all childrens' world dirty flag to be set to true.  Should be called any time our local rotation changes.
	 */
	void LSE_CALL CActor2d::DirtyChildrenWorldTrans() const {
		m_bWorldDirty = true;
		for ( uint32_t I = m_vEntityChildren.Length(); I--; ) {
			const CActor2d * paChild = reinterpret_cast<const CActor2d *>(m_vEntityChildren[I]);
			paChild->DirtyChildrenWorldTrans();
		}
	}

}	// namespace lsr
