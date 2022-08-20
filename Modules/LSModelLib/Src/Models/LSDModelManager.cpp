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
 * Description: Ensures that no model file is loaded redundantly.
 */

#include "LSDModelManager.h"


namespace lsd {

	// == Various constructors.
	LSE_CALLCTOR CModelManager::CModelManager() {
	}
	LSE_CALLCTOR CModelManager::~CModelManager() {
		Reset();
	}

	// == Functions.
	/**
	 * Loads (if necessary) the given model and returns a shared pointer.
	 *
	 * \param _pcPath Path to the model file to load.
	 * \return Returns a shared pointer to the file designated by its path.
	 */
	CSharedSharedModelPtr LSE_CALL CModelManager::LoadModel( const char * _pcPath ) {
		char szPath[LSF_MAX_PATH];
		CFileLib::ExpandResourcePath( _pcPath, szPath );
		CString sPath = szPath;
		uint32_t ui32Index;
		if ( m_lmModels.ItemExists( sPath, ui32Index ) ) {
			return m_lmModels.GetByIndex( ui32Index );
		}

		
		CSharedSharedModelPtr ssmpRet;
		if ( !ssmpRet.New() ) {
			CStd::PrintError( LSSTD_E_OUTOFMEMORY );
			return CSharedSharedModelPtr();
		}
		CSharedFilePtr sfpFile = CSharedModel::SharedFilePtr( sPath.CStr() );
		if ( !sfpFile.Valid() ) {
			CStd::PrintError( LSSTD_E_FILENOTFOUND );
			return CSharedSharedModelPtr();
		}

		uintptr_t uiptrEnd;
		LSSTD_ERRORS eError = ssmpRet->LoadFromMemory( sfpFile, uiptrEnd );
		if ( eError != LSSTD_E_SUCCESS ) {
			CStd::PrintError( eError );
			return CSharedSharedModelPtr();
		}

		if ( !m_lmModels.Insert( sPath, ssmpRet ) ) {
			CStd::PrintError( LSSTD_E_OUTOFMEMORY );
			return CSharedSharedModelPtr();
		}
		return ssmpRet;
	}

	/**
	 * Resets the manager back to scratch.
	 */
	void LSE_CALL CModelManager::Reset() {
		m_lmModels.Reset();
	}

	/**
	 * Clears any model resources that are not currently in use.
	 */
	void LSE_CALL CModelManager::SoftReset() {
		for ( uint32_t I = m_lmModels.Length(); I--; ) {
			if ( m_lmModels.GetByIndex( I ).RefCount() == 1 ) {
				// Referenced only by us.
				m_lmModels.RemoveByIndex( I );
			}
		}
		if ( m_lmModels.Length() == 0 ) { m_lmModels.Reset(); }
	}

}	// namespace lsd
