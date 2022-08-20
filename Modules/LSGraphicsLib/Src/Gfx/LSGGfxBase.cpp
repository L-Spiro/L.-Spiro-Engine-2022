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
 * Description: The base class for the primary interface to the underlying graphics API.
 */

#include "LSGGfxBase.h"


namespace lsg {

	// == Members.
	/**
	 * System metrics.
	 */
	CGfxBase::LSG_METRICS CGfxBase::m_mMetrics = { 0 };

	/** The world matrix. */
	CMatrix4x4 CGfxBase::m_mWorldMatrix;

	/** The view matrix. */
	CMatrix4x4 CGfxBase::m_mViewMatrix;

	/** The projection matrix. */
	CMatrix4x4 CGfxBase::m_mProjMatrix;

	/** The world-view matrix.  Updated only before rendering, and only if the world or view matrix have
	 *	changed. */
	CMatrix4x4 CGfxBase::m_mWorldViewMatrix;

	/** The world-view-projection matrix.  Updated only before rendering, and only if the world, view, or
	 *	projection matrices have changed. */
	CMatrix4x4 CGfxBase::m_mWorldViewProjMatrix;

	/** The normal matrix.  It transforms normals to screen space. */
	CMatrix3x3 CGfxBase::m_mNormalMatrix;

	/** The inverse world-view matrix.  Updated only before rendering, and only if the world or view matrix have
	 *	changed. */
	CMatrix4x4 CGfxBase::m_mInverseWorldViewMatrix;

	/** Has the world matrix been modified? */
	LSBOOL CGfxBase::m_bWorldDirty = false;

	/** Has the view matrix been modified? */
	LSBOOL CGfxBase::m_bViewDirty = false;

	/** Has the projection matrix been modified? */
	LSBOOL CGfxBase::m_bProjDirty = false;

	/** Camera position in world space. */
	CVector3 CGfxBase::m_vCamPos;

	/** Screen width. */
	uint32_t CGfxBase::m_ui32ScreenWidth = 0;

	/** Screen height. */
	uint32_t CGfxBase::m_ui32ScreenHeight = 0;

	/** Diffuse material color. */
	CVector4 CGfxBase::m_vDiffuseMaterial = CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO );

	/** Emissive material color. */
	CVector4 CGfxBase::m_vEmissiveMaterial = CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO );

	/** Specular material color. */
	CVector4 CGfxBase::m_vSpecularMaterial = CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO );

	/** Specular material power. */
	LSREAL CGfxBase::m_fPower = LSM_ZERO;

	/** Material roughness (x) and specular reflectance (y). */
	CVector4 CGfxBase::m_vRoughness = CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO );

	/** Reflectivity. */
	//LSREAL CGfxBase::m_fReflectivity = LSM_ZERO;

	/** Ashikhman-Shirley factors (Nu, Nv, Factor, Rs). */
	CVector4 CGfxBase::m_vAshikFactors = CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO );

	/** Directional light direction. */
	CVector4 CGfxBase::m_vDirLightDir[LSG_MAX_LIGHTS] = { CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ) };

	/** Directional light color. */
	CVector4 CGfxBase::m_vDirLightColor[LSG_MAX_LIGHTS] = { CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ) };

	/** Total directional lights set. */
	uint32_t CGfxBase::m_ui32TotalDirLights = 0;

	/** Point light positions. */
	CVector4 CGfxBase::m_vPointLightPos[LSG_MAX_LIGHTS] = { CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ) };

	/** Point light colors. */
	CVector4 CGfxBase::m_vPointLightColor[LSG_MAX_LIGHTS] = { CVector4( LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ZERO ) };

	/** Point light sizes. */
	float CGfxBase::m_fPointLightSize[LSG_MAX_LIGHTS] = { 0.0f };

	/** Total point lights set. */
	uint32_t CGfxBase::m_ui32TotalPointLights = 0;
	
	/** Are we active? */
	LSBOOL CGfxBase::m_bActive = true;

	// == Functions.
	/**
	 * Sets default values on all matrix-related members.
	 */
	void LSE_CALL CGfxBase::SetDefaultMatrices() {
		m_bWorldDirty = m_bViewDirty = m_bProjDirty = true;

		m_mWorldMatrix.Identity();
		m_mViewMatrix.Identity();
		m_mProjMatrix.Identity();
		m_mWorldViewMatrix.Identity();
		m_mWorldViewProjMatrix.Identity();
		m_mNormalMatrix.Identity();
		m_mInverseWorldViewMatrix.Identity();
		m_vCamPos.Clear();
	}

}	// namespace lsg
