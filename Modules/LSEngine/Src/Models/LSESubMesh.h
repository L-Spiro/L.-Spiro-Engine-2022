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
 * Description: An instance of a sub-mesh.
 */


#ifndef __LSE_SUBMESH_H__
#define __LSE_SUBMESH_H__

#include "../LSEEngine.h"
#include "Models/LSDSubMesh.h"
#include "ShaderLanguage/LSGShader.h"
#include "Shaders/LSDShaderKey.h"

namespace lse {

	/**
	 * Class CSubMesh
	 * \brief An instance of a sub-mesh.
	 *
	 * Description: An instance of a sub-mesh.
	 */
	class CSubMesh {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSubMesh();
		LSE_CALLCTOR							~CSubMesh();


		// == Functions.
		/**
		 * Sets the sub-mesh reference.
		 *
		 * \param _psmSubMesh The sub-mesh to set.
		 */
		void LSE_CALL							SetSubMesh( const lsd::CSubMesh * _psmSubMesh );

		/**
		 * Gets a constant reference to the material for this sub-mesh.
		 *
		 * \return Returns a constant reference to the material for this sub-mesh.
		 */
		LSE_INLINE const CModelMaterial & LSE_CALL
												Material() const;

		/**
		 * Gets the last shader key for read and write.
		 *
		 * \return Returns a reference to the last shader key.
		 */
		LSE_INLINE LSD_SHADERKEY & LSE_CALL		ShaderKey();

		/**
		 * Gets the last shader key for read only.
		 *
		 * \return Returns a constant reference to the last shader key.
		 */
		LSE_INLINE const LSD_SHADERKEY & LSE_CALL
												ShaderKey() const;

		/**
		 * Gets a read/write reference to the shader for this sub-mesh.
		 *
		 * \return Returns a read/write reference to the shader for this sub-mesh.
		 */
		LSE_INLINE CSharedShaderPtr & LSE_CALL	Shader();

		/**
		 * Gets a read-only reference to the shader for this sub-mesh.
		 *
		 * \return Returns a read-only reference to the shader for this sub-mesh.
		 */
		LSE_INLINE const CSharedShaderPtr & LSE_CALL
												Shader() const;

	protected :
		// == Members.
		/** Pointer to the original sub-mesh. */
		const lsd::CSubMesh *					m_psmSubMesh;

		/** Material. */
		CModelMaterial							m_mmMaterial;

		/** Shader. */
		CSharedShaderPtr						m_sspShader;

		/** The last shader key used. */
		LSD_SHADERKEY							m_skLastKey;

	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets a constant reference to the material for this sub-mesh.
	 *
	 * \return Returns a constant reference to the material for this sub-mesh.
	 */
	LSE_INLINE const CModelMaterial & LSE_CALL CSubMesh::Material() const { return m_mmMaterial; }

	/**
	 * Gets the last shader key for read and write.
	 *
	 * \return Returns a reference to the last shader key.
	 */
	LSE_INLINE LSD_SHADERKEY & LSE_CALL CSubMesh::ShaderKey() { return m_skLastKey; }

	/**
	 * Gets the last shader key for read only.
	 *
	 * \return Returns a constant reference to the last shader key.
	 */
	LSE_INLINE const LSD_SHADERKEY & LSE_CALL CSubMesh::ShaderKey() const { return m_skLastKey; }

	/**
	 * Gets a read/write reference to the shader for this sub-mesh.
	 *
	 * \return Returns a read/write reference to the shader for this sub-mesh.
	 */
	LSE_INLINE CSharedShaderPtr & LSE_CALL CSubMesh::Shader() { return m_sspShader; }

	/**
	 * Gets a read-only reference to the shader for this sub-mesh.
	 *
	 * \return Returns a read-only reference to the shader for this sub-mesh.
	 */
	LSE_INLINE const CSharedShaderPtr & LSE_CALL CSubMesh::Shader() const { return m_sspShader; }

}	// namespace lse

#endif	// __LSE_SUBMESH_H__
