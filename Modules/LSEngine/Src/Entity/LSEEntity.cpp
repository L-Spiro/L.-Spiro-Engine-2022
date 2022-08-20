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
 * Description: An entity is simply the most abstract form of a "thing" in the game world.  This base class
 *	provides a parent/child system.  All entities can be parented and have children.
 */

#include "LSEEntity.h"
#include "Algorithm/LSTLAlgorithm.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CEntity::CEntity() :
		m_peParent( NULL ),
		m_ui32EntityType( LSE_ET_ENTITY ) {
	}
	LSE_CALLCTOR CEntity::~CEntity() {
		// Inform our parent that we are dead.
		if ( m_peParent ) {
			m_peParent->RemEntityChild( this );
		}

		for ( uint32_t I = m_vEntityChildren.Length(); I--; ) {
			m_vEntityChildren[I]->Detach();
		}
	}

	// == Functions.
	/**
	 * Sets our parent.
	 *
	 * \param _peParent A pointer to our new parent.
	 */
	void LSE_CALL CEntity::SetEntityParent( CEntity * _peParent ) {
		if ( m_peParent == _peParent ) { return; }
		if ( m_peParent ) {
			m_peParent->RemEntityChild( this );
		}
		m_peParent = NULL;
		if ( _peParent ) {
			_peParent->AddEntityChild( this );
		}
	}

	/**
	 * Adds a child.
	 *
	 * \param _peChild The child to add.
	 * \return Returns true if there was enough RAM to add the child.
	 */
	LSBOOL LSE_CALL CEntity::AddEntityChild( CEntity * _peChild ) {
		if ( _peChild ) {
			if ( _peChild->m_peParent == this ) { return true; }
			if ( !m_vEntityChildren.Push( _peChild ) ) { return false; }
			// It has no parent or a different parent.
			if ( _peChild->m_peParent ) {
				_peChild->m_peParent->RemEntityChild( this );
			}

			_peChild->m_peParent = this;
			return true;
		}
		return true;
	}

	/**
	 * Removes a child.
	 *
	 * \param _peChild The child to add.
	 * \return Returns true if the given pointer was a child of this entity.
	 */
	LSBOOL LSE_CALL CEntity::RemEntityChild( CEntity * _peChild ) {
		LSBOOL bFound = false;
		for ( uint32_t I = m_vEntityChildren.Length(); I--; ) {
			if ( m_vEntityChildren[I] == _peChild ) {
				m_vEntityChildren[I]->Detach();
				m_vEntityChildren.RemoveNoDealloc( I );
				bFound = true;
				break;
			}
		}
		if ( m_vEntityChildren.Length() == 0 ) { m_vEntityChildren.Reset(); }
		return bFound;
	}

	/**
	 * Resets the entity.
	 */
	void LSE_CALL CEntity::ResetEntity() {
		for ( uint32_t I = m_vEntityChildren.Length(); I--; ) {
			m_vEntityChildren[I]->Detach();
		}
		m_vEntityChildren.Reset();
		SetEntityParent( NULL );
	}

	/**
	 * Allocates at least the given number of children.
	 *
	 * \param _ui32Total Total children to allocate.
	 */
	void LSE_CALL CEntity::AllocateChildren( uint32_t _ui32Total ) {
		if ( _ui32Total > m_vEntityChildren.Allocated() ) {
			m_vEntityChildren.Allocate( _ui32Total );
		}
	}

	/**
	 * Detach from its parent.  This can only be called by its parent.
	 */
	void LSE_CALL CEntity::Detach() {
		// The parent knows we are leaving it.  Don't send any callbacks to it.
		m_peParent = NULL;
	}

}	// namespace lse
