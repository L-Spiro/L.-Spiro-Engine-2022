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
 * Description: An instance of a shared 3D model.  All of its children are allocated linerally for best caching.
 */


#ifndef __LSE_MODELINSTANCE_H__
#define __LSE_MODELINSTANCE_H__

#include "../LSEEngine.h"
#include "LSEMesh.h"
#include "LSEModelObjectBase.h"
#include "Models/LSDSharedModel.h"

namespace lse {

	/**
	 * Class CModelInstance
	 * \brief An instance of a shared 3D model.
	 *
	 * Description: An instance of a shared 3D model.  All of its children are allocated linerally for best caching.
	 */
	class CModelInstance : public CModelObjectBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CModelInstance();
		LSE_CALLCTOR							~CModelInstance();


		// == Types.
		/** Array of meshes. */
		typedef CVectorPoD<lse::CMesh *, uint32_t>
												CMeshArray;


		// == Functions.
		/**
		 * Loads a model by its file name.
		 *
		 * \param _pcPath Path to the model to load.
		 * \return Returns true if the model was loaded.
		 */
		LSBOOL LSE_CALL							CreateModelInstance( const char * _pcPath );

		/**
		 * Resets the model to scratch.
		 */
		void LSE_CALL							Reset();

		/**
		 * Gets a constant reference to the array of meshes on this model instance.
		 *
		 * \return Returns a constant reference to the array of meshes on this model instance.
		 */
		LSE_INLINE const CMeshArray & LSE_CALL	MeshList() const;


	protected :
		// == Members.
		/** The shared pointer to the shared model we reference. */
		CSharedSharedModelPtr					m_ssmpModel;

		/** All loaded objects. */
		CVectorPoD<uint8_t, uint32_t>			m_vObjectList;

		/** Loaded meshes. */
		CMeshArray								m_maMeshList;


		// == Functions.
		/**
		 * Constructs a model object based on type.  Size of the created type is also returned.  If _pvAddress, no object is
		 *	constructed and only the size of the object is returned.
		 *
		 * \param _maType The type to construct.
		 * \param _pvAddress The address where to construct the object, or NULL to only get the size.
		 * \param _uiptrSize The size of the constructed type, in bytes.
		 * \return Returns a pointer to the constructed cast to a CModelObjectBase *.
		 */
		CModelObjectBase * LSE_CALL				Construct( LSD_MODEL_ATTRIBUTES _maType, void * _pvAddress, uintptr_t &_uiptrSize );

		/**
		 * Destructs a model object based on type and address.
		 *
		 * \param _maType The type to destruct.
		 * \param _pvAddress The address where to destruct the object.
		 */
		void LSE_CALL							Destruct( LSD_MODEL_ATTRIBUTES _maType, void * _pvAddress );

		/**
		 * Sets the parent of the given object recursively.
		 *
		 * \param _pmobParent The parent object.
		 * \param _ui32Index Index of the current hierachy object and the child object whose parent is to be set.
		 */
		void LSE_CALL							SetParents( CModelObjectBase * _pmobParent, uint32_t &_ui32Index );

		/**
		 * Gets a child by index.
		 *
		 * \param _ui32Index Index of the child to get.
		 * \return Returns a pointer to the child at the given index.
		 */
		CModelObjectBase * LSE_CALL				ChildByIndex( uint32_t _ui32Index );

	private :
		typedef CModelObjectBase				Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** A shared model instance pointer. */
	typedef CSharedPtr<CModelInstance>			CSharedModelInstancePtr;

	// == Functions.
	/**
	 * Gets a constant reference to the array of meshes on this model instance.
	 *
	 * \return Returns a constant reference to the array of meshes on this model instance.
	 */
	LSE_INLINE const CModelInstance::CMeshArray & LSE_CALL CModelInstance::MeshList() const { return m_maMeshList; }

}	// namespace lse

#endif	// __LSE_MODELINSTANCE_H__
