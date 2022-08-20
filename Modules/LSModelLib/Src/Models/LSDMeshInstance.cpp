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
 * Description: An instance of a mesh in a shared model.  The model holds an array of unique meshes into which
 *	a mesh indexes.  A mesh instance also includes a transform, name, parent/children, etc.
 */

#include "LSDMeshInstance.h"
#include "LSDSharedModel.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CMeshInstance::CMeshInstance() {
	}
	LSE_CALLCTOR CMeshInstance::~CMeshInstance() {
	}

	// == Functions.
	/**
	 * Loads a sub-mesh from a byte stream.
	 *
	 * \param _ebsStream Byte stream from which to read.
	 * \param _pmMeshes Array of meshes.
	 * \param _ui32Meshes Total meshes.
	 * \param _ui32Version File version number.
	 * \return Returns an error code indicating the result of the load.
	 */
	LSSTD_ERRORS LSE_CALL CMeshInstance::LoadFromStream( const CExternalByteStream &_ebsStream,
		CMesh * _pmMeshes, uint32_t _ui32Meshes, uint32_t /*_ui32Version*/ ) {
		if ( !CSharedModel::ReadString( _ebsStream, m_sName ) ) { return LSSTD_E_OUTOFMEMORY; }

		//if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&m_miData), sizeof( m_miData ) ) != sizeof( m_miData ) ) { return LSSTD_E_INVALIDDATA; }
		float fLocalTrans[16];
		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&fLocalTrans), sizeof( fLocalTrans ) ) != sizeof( fLocalTrans ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 16; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.mLocalTrans)[I] = fLocalTrans[I];
		}
		//if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&m_miData.mLocalTrans), sizeof( m_miData.mLocalTrans ) ) != sizeof( m_miData.mLocalTrans ) ) { return LSSTD_E_INVALIDDATA; }
		m_miData.ui32Index = _ebsStream.ReadUInt32();
		if ( m_miData.ui32Index >= _ui32Meshes ) { return LSSTD_E_INVALIDDATA; }
		m_miData.ui64Id = _ebsStream.ReadUInt64();
		m_miData.ui64ParentId = _ebsStream.ReadUInt64();
		float fVec[3];
		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.vEulerRot)[I] = fVec[I];
		}

		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.vEulerScale)[I] = fVec[I];
		}

		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.vEulerPos)[I] = fVec[I];
		}

		m_miData.ui32RotType = _ebsStream.ReadUInt32();
		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.aAabb.m_vMin)[I] = fVec[I];
		}
		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.aAabb.m_vMax)[I] = fVec[I];
		}

		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(fVec), sizeof( fVec ) ) != sizeof( fVec ) ) { return LSSTD_E_INVALIDDATA; }
		for ( uint32_t I = 3; I--; ) {
			reinterpret_cast<LSREAL *>(&m_miData.sSphere.c)[I] = fVec[I];
		}

		if ( _ebsStream.ReadBytes( reinterpret_cast<uint8_t *>(&fVec[0]), sizeof( fVec[0] ) ) != sizeof( fVec[0] ) ) { return LSSTD_E_INVALIDDATA; }
		m_miData.sSphere.r = static_cast<LSREAL>(fVec[0]);


		m_pmMesh = &_pmMeshes[m_miData.ui32Index];
		return LSSTD_E_SUCCESS;
	}

}	// namespace lsd
