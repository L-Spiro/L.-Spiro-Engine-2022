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
 * Description: An entity is simply the most abstract form of a "thing" in the game world.  This base class
 *	provides a parent/child system.  All entities can be parented and have children.
 */


#ifndef __LSR_ENTITY2D_H__
#define __LSR_ENTITY2D_H__

#include "../LSRSpriteLib.h"
#include "LSREntity2dTypes.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsr {

	/**
	 * Class CEntity2d
	 * \brief An entity is simply the most abstract form of a "thing" in the game world.
	 *
	 * Description: An entity is simply the most abstract form of a "thing" in the game world.  This base class
	 *	provides a parent/child system.  All entities can be parented and have children.
	 */
	class CEntity2d {
	public :
		// == Various constructors.
		virtual LSE_CALLCTOR					~CEntity2d();


		// == Functions.
		/**
		 * Gets the entity type.
		 *
		 * \return Returns the type mask for this entity.
		 */
		LSE_INLINE uint32_t LSE_CALL			EntityType() const;

		/**
		 * Sets our parent.
		 *
		 * \param _peParent A pointer to our new parent.
		 */
		virtual void LSE_CALL					SetEntityParent( CEntity2d * _peParent );

		/**
		 * Adds a child.
		 *
		 * \param _peChild The child to add.
		 * \return Returns true if there was enough RAM to add the child.
		 */
		virtual LSBOOL LSE_CALL					AddEntityChild( CEntity2d * _peChild );

		/**
		 * Removes a child.
		 *
		 * \param _peChild The child to add.
		 * \return Returns true if the given pointer was a child of this entity.
		 */
		virtual LSBOOL LSE_CALL					RemEntityChild( CEntity2d * _peChild );

		/**
		 * Gets the parent of this entity, which may be NULL.
		 *
		 * \return Returns the parent of this entity, if any.
		 */
		LSE_INLINE CEntity2d * LSE_CALL			EntityParent();

		/**
		 * Gets the parent of this entity, which may be NULL.
		 *
		 * \return Returns the parent of this entity, if any.
		 */
		LSE_INLINE const CEntity2d * LSE_CALL	EntityParent() const;

		/**
		 * Gets the upper-most parent of this entity or NULL if there are no parents.
		 *
		 * \return Returns the upper-most parent of this entity if any.
		 */
		LSE_INLINE CEntity2d * LSE_CALL			EntityGrandFather() const;

		/**
		 * Resets the entity.
		 */
		void LSE_CALL							ResetEntity();

		/**
		 * Allocates at least the given number of children.
		 *
		 * \param _ui32Total Total children to allocate.
		 */
		void LSE_CALL							AllocateChildren( uint32_t _ui32Total );


	protected :
		// == Various constructors.
		LSE_CALLCTOR							CEntity2d();


		// == Members.
		/** Our list of children. */
		CVectorPoD<CEntity2d *, uint32_t, 8UL>	m_vEntityChildren;

		/** Our parent. */
		CEntity2d *								m_peParent;

		/** Entity type. */
		uint32_t								m_ui32EntityType;


	private :
		// == Functions.
		/**
		 * Detach from its parent.  This can only be called by its parent.
		 */
		void LSE_CALL							Detach();

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the entity type.
	 *
	 * \return Returns the type mask for this entity.
	 */
	LSE_INLINE uint32_t LSE_CALL CEntity2d::EntityType() const {
		return m_ui32EntityType;
	}

	/**
	 * Gets the parent of this entity, which may be NULL.
	 *
	 * \return Returns the parent of this entity, if any.
	 */
	LSE_INLINE CEntity2d * LSE_CALL CEntity2d::EntityParent() {
		return m_peParent;
	}

	/**
	 * Gets the parent of this entity, which may be NULL.
	 *
	 * \return Returns the parent of this entity, if any.
	 */
	LSE_INLINE const CEntity2d * LSE_CALL CEntity2d::EntityParent() const {
		return m_peParent;
	}

	/**
	 * Gets the upper-most parent of this entity or NULL if there are no parents.
	 *
	 * \return Returns the upper-most parent of this entity if any.
	 */
	LSE_INLINE CEntity2d * LSE_CALL CEntity2d::EntityGrandFather() const {
		if ( !m_peParent ) { return NULL; }
		CEntity2d * peThis = m_peParent;
		while ( peThis->EntityParent() ) {
			peThis = peThis->EntityParent();
		}
		return peThis;
	}

}	// namespace lsr

#endif	// __LSR_ENTITY2D_H__
