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

#include "LSEModelInstance.h"
#include "Models/LSDModelManager.h"


namespace lse {

	// == Various constructors.
	LSE_CALLCTOR CModelInstance::CModelInstance() {
		m_ui32EntityType |= LSE_ET_MODEL;
	}
	LSE_CALLCTOR CModelInstance::~CModelInstance() {
		Reset();
	}

	// == Functions.
	/**
	 * Loads a model by its file name.
	 *
	 * \param _pcPath Path to the model to load.
	 * \return Returns true if the model was loaded.
	 */
	LSBOOL LSE_CALL CModelInstance::CreateModelInstance( const char * _pcPath ) {
		m_ssmpModel = CModelLib::SharedModelManager().LoadModel( _pcPath );
		if ( !m_ssmpModel.Valid() ) { return false; }
#define LSE_GETSIZE( TYPE, RET )	Construct( static_cast<LSD_MODEL_ATTRIBUTES>(TYPE), NULL, RET )
		// Get the total size of all objects in the model.
		uintptr_t uiptrSize = 0;
		uint32_t ui32Meshes = 0;
		for ( uint32_t I = m_ssmpModel->TotalHierarchyObjects(); I--; ) {
			uintptr_t uiptrThisSize;
			LSE_GETSIZE( m_ssmpModel->Hierarchy()[I].ui16Type, uiptrThisSize );
			uiptrSize += uiptrThisSize;
			if ( m_ssmpModel->Hierarchy()[I].ui16Type == LSD_MA_OBJECT_MESH ) {
				++ui32Meshes;
			}
		}
		if ( !m_vObjectList.Resize( static_cast<uint32_t>(uiptrSize) ) ) { Reset(); return false; }
		if ( !m_maMeshList.Resize( ui32Meshes ) ) { Reset(); return false; }
		// Construct all objects.
		uiptrSize = 0;
		ui32Meshes = 0;
		for ( uint32_t I = 0; I < m_ssmpModel->TotalHierarchyObjects(); ++I ) {
			uintptr_t uiptrThisSize;
			CModelObjectBase * pmobObj = Construct( static_cast<LSD_MODEL_ATTRIBUTES>(m_ssmpModel->Hierarchy()[I].ui16Type),
				&m_vObjectList[static_cast<uint32_t>(uiptrSize)],
				uiptrThisSize );
			pmobObj->CreateBaseData( m_ssmpModel->Hierarchy()[I] );
			uiptrSize += uiptrThisSize;
			if ( m_ssmpModel->Hierarchy()[I].ui16Type == LSD_MA_OBJECT_MESH ) {
				m_maMeshList[ui32Meshes++] = static_cast<lse::CMesh *>(pmobObj);
			}
		}


		for ( uint32_t I = 0; I < m_ssmpModel->TotalHierarchyObjects(); ) {
			SetParents( this, I );
		}
		/*for ( uint32_t I = 0; I < m_ssmpModel->TotalHierarchyObjects(); ++I ) {
			CModelObjectBase * pmobObj = ChildByIndex( I );
		}*/

		// Load meshes.
		uiptrSize = 0;
		for ( uint32_t I = 0; I < m_ssmpModel->TotalHierarchyObjects(); ++I ) {
			switch ( m_ssmpModel->Hierarchy()[I].ui16Type ) {
				case LSD_MA_OBJECT_MESH : {
					lse::CMesh * pmMesh = reinterpret_cast<lse::CMesh *>(&m_vObjectList[static_cast<uint32_t>(uiptrSize)]);
					const lsd::CMeshInstance * pmiMeshInstance = m_ssmpModel->FindMeshById( pmMesh->ObjectId() );
					if ( !pmiMeshInstance ) { Reset(); return false; }
					if ( !pmMesh->LoadMeshInstance( (*m_ssmpModel), pmiMeshInstance ) ) { Reset(); return false; }
					break;
				}
			}
			uintptr_t uiptrThisSize;
			LSE_GETSIZE( m_ssmpModel->Hierarchy()[I].ui16Type, uiptrThisSize );
			uiptrSize += uiptrThisSize;
		}

		DirtyChildrenWorldTrans();
		return true;
#undef LSE_GETSIZE
	}

	/**
	 * Resets the model to scratch.
	 */
	void LSE_CALL CModelInstance::Reset() {
#define LSE_GETSIZE( TYPE, RET )	Construct( static_cast<LSD_MODEL_ATTRIBUTES>(TYPE), NULL, RET )
		if ( m_ssmpModel.Valid() && m_vObjectList.Length() ) {
			uintptr_t uiptrSize = 0;
			for ( uint32_t I = 0; I < m_ssmpModel->TotalHierarchyObjects(); ++I ) {
				uintptr_t uiptrThisSize;
				// Get the size.
				LSE_GETSIZE( m_ssmpModel->Hierarchy()[I].ui16Type, uiptrThisSize );
				// Destructor.
				Destruct( static_cast<LSD_MODEL_ATTRIBUTES>(m_ssmpModel->Hierarchy()[I].ui16Type), &m_vObjectList[static_cast<uint32_t>(uiptrSize)] );
				uiptrSize += uiptrThisSize;
			}
		}
		m_vObjectList.Reset();
		m_ssmpModel.Reset();
		m_maMeshList.Reset();
		ResetEntity();
#undef LSE_GETSIZE
	}

	/**
	 * Constructs a model object based on type.  Size of the created type is also returned.  If _pvAddress, no object is
	 *	constructed and only the size of the object is returned.
	 *
	 * \param _maType The type to construct.
	 * \param _pvAddress The address where to construct the object, or NULL to only get the size.
	 * \param _uiptrSize The size of the constructed type, in bytes.
	 * \return Returns a pointer to the constructed cast to a CModelObjectBase *.
	 */
	CModelObjectBase * LSE_CALL CModelInstance::Construct( LSD_MODEL_ATTRIBUTES _maType, void * _pvAddress, uintptr_t &_uiptrSize ) {
#define LSE_CREATE( CASE, CLASS )												\
	case CASE : {																\
		_uiptrSize = sizeof( lse::CLASS );										\
		return _pvAddress ? new( _pvAddress ) lse::CLASS() : NULL;				\
	}
		switch ( _maType ) {
			LSE_CREATE( LSD_MA_OBJECT_NODE, CModelObjectBase )
			LSE_CREATE( LSD_MA_OBJECT_JOINT, CModelObjectBase )
			LSE_CREATE( LSD_MA_OBJECT_MESH, CMesh )
			LSE_CREATE( LSD_MA_OBJECT_CAMERA, CModelObjectBase )
			LSE_CREATE( LSD_MA_OBJECT_STEREOCAMERA, CModelObjectBase )
			LSE_CREATE( LSD_MA_OBJECT_LIGHT, CModelObjectBase )
			default : {}
		}
		_uiptrSize = 0;
		return NULL;
#undef LSE_CREATE
	}

	/**
	 * Destructs a model object based on type and address.
	 *
	 * \param _maType The type to destruct.
	 * \param _pvAddress The address where to destruct the object.
	 */
	void LSE_CALL CModelInstance::Destruct( LSD_MODEL_ATTRIBUTES _maType, void * _pvAddress ) {
#define LSE_DESTROY( CASE, CLASS )												\
	case CASE : {																\
		reinterpret_cast<lse::CLASS *>(_pvAddress)->~CLASS();					\
		break;																	\
	}
		switch ( _maType ) {
			LSE_DESTROY( LSD_MA_OBJECT_NODE, CModelObjectBase )
			LSE_DESTROY( LSD_MA_OBJECT_JOINT, CModelObjectBase )
			LSE_DESTROY( LSD_MA_OBJECT_MESH, CMesh )
			LSE_DESTROY( LSD_MA_OBJECT_CAMERA, CModelObjectBase )
			LSE_DESTROY( LSD_MA_OBJECT_STEREOCAMERA, CModelObjectBase )
			LSE_DESTROY( LSD_MA_OBJECT_LIGHT, CModelObjectBase )
			default : {}
		}
#undef LSE_DESTROY
	}

	/**
	 * Sets the parent of the given object recursively.
	 *
	 * \param _pmobParent The parent object.
	 * \param _ui32Index Index of the current hierachy object and the child object whose parent is to be set.
	 */
	void LSE_CALL CModelInstance::SetParents( CModelObjectBase * _pmobParent, uint32_t &_ui32Index ) {
		CModelObjectBase * pmobObj = ChildByIndex( _ui32Index );
		pmobObj->SetEntityParent( _pmobParent );
		assert( pmobObj->ParentId() == _pmobParent->ObjectId() );
		uint16_t ui16Children = m_ssmpModel->Hierarchy()[_ui32Index++].ui16Children;
		pmobObj->AllocateChildren( ui16Children );
		for ( uint32_t I = ui16Children; I--; ) {
			SetParents( pmobObj, _ui32Index );
		}
	}

	/**
	 * Gets a child by index.
	 *
	 * \param _ui32Index Index of the child to get.
	 * \return Returns a pointer to the child at the given index.
	 */
	CModelObjectBase * LSE_CALL CModelInstance::ChildByIndex( uint32_t _ui32Index ) {
		assert( _ui32Index < m_ssmpModel->TotalHierarchyObjects() );
#define LSE_GETSIZE( TYPE, RET )	Construct( static_cast<LSD_MODEL_ATTRIBUTES>(TYPE), NULL, RET )
		uintptr_t uiptrSize = 0;
		for ( uint32_t I = _ui32Index; I--; ) {
			uintptr_t uiptrThisSize;
			LSE_GETSIZE( m_ssmpModel->Hierarchy()[I].ui16Type, uiptrThisSize );
			uiptrSize += uiptrThisSize;
		}

		return reinterpret_cast<CModelObjectBase *>(&m_vObjectList[static_cast<uint32_t>(uiptrSize)]);
#undef LSE_GETSIZE
	}

}	// namespace lse
