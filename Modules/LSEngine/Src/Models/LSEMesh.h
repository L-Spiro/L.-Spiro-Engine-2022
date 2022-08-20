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
 * Description: A mesh entity type.
 */


#ifndef __LSE_MESH_H__
#define __LSE_MESH_H__

#include "../LSEEngine.h"
#include "../Entity/LSEEntityTypes.h"
#include "LSEModelObjectBase.h"
#include "LSESubMesh.h"
#include "Models/LSDMeshInstance.h"
#include "Models/LSDSharedModel.h"

namespace lse {

	/**
	 * Class CMesh
	 * \brief A mesh entity type.
	 *
	 * Description: A mesh entity type.
	 */
	class CMesh : public CModelObjectBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CMesh();
		LSE_CALLCTOR							~CMesh();


		// == Functions.
		/**
		 * Loads the mesh instance from a shared mesh.
		 *
		 * \param _smModel The shared model owning the given mesh.
		 * \param _pmiMesh The shared mesh instance from which this mesh instance is to be created.
		 * \return Returns true if the mesh was successfully loaded.  False indicates a lack of memory.
		 */
		LSBOOL LSE_CALL							LoadMeshInstance( const CSharedModel &_smModel, const lsd::CMeshInstance * _pmiMesh );

		/**
		 * Renders a given part of the mesh.
		 *
		 * \param _ui32Index Index of the part of the mesh to render.
		 */
		void LSE_CALL							RenderPart( uint32_t _ui32Index );

		/**
		 * Gets a constant reference to the array of sub-meshes.
		 *
		 * \return Returns a constant reference to the array of sub-meshes.
		 */
		LSE_INLINE const CVector<lse::CSubMesh, uint32_t> & LSE_CALL
												SubMeshes() const;

		/**
		 * Inform child classes that the world transform has changed.
		 */
		virtual void LSE_CALL					WorldDidChange() const;


	protected :
		// == Members.
		/** Array of sub-meshes. */
		CVector<lse::CSubMesh, uint32_t>		m_vSubMeshes;

		/** The source mesh. */
		const lsd::CMeshInstance *				m_pmiSrcMesh;


		// == Functions.
		/**
		 * Activates a given material.
		 *
		 * \param _mmMaterial Material to activate.
		 * \param _skKey Shader key of the material to activate.
		 */
		static void LSE_CALL					ActivateMaterial( const CModelMaterial &_mmMaterial, const LSD_SHADERKEY &_skKey );

	private :
		typedef CModelObjectBase				Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Types.
	/** An array of pointers to meshes. */
	typedef CVectorPoD<lse::CMesh *, uint32_t>	CMeshInstanceArray;

	// == Functions.
	/**
	 * Gets a constant reference to the array of sub-meshes.
	 *
	 * \return Returns a constant reference to the array of sub-meshes.
	 */
	LSE_INLINE const CVector<lse::CSubMesh, uint32_t> & LSE_CALL CMesh::SubMeshes() const { return m_vSubMeshes; }

}	// namespace lse

#endif	// __LSE_MESH_H__
