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
 * Description: This is meant to be included directly into the graphics API immediately after it defines its
 *	API-specific constants and enumerations.
 * These are the definitions for methods that set default values for the structures in LSGDescStructs.h.
 */


#ifndef LSG_CREATESTATESDEF_CLASS
#error "LSG_CREATESTATESDEF_CLASS must be defined (for example as CDirectX9 or COpenGl)."
#endif	// #ifndef LSG_CREATESTATESDEF_CLASS

// == Functions.
/**
 * Creates a sampler-state object that encapsulates sampling information for a texture.
 *
 * \param _sdSamplerDesc Reference to a sampler state description.
 * \param _ppssSamplerState Address of a pointer to the sampler state object created.
 * \return Returns an error code indicating success or failure.
 */
HRESULT LSE_CALL LSG_CREATESTATESDEF_CLASS::CreateSamplerState( const LSG_SAMPLER_DESC &_sdSamplerDesc, LSG_SAMPLER_STATE ** _ppssSamplerState ) {
	CCriticalSection::CLocker lLockMe( m_csSamplerStateCrit );
	return CreateState<LSG_SAMPLER_DESC, LSG_SAMPLER_STATE, LSG_SAMPLER_STATE_REF>( _sdSamplerDesc, _ppssSamplerState, m_mSamplerStateMap );
}

/**
 * Creates a rasterizer state object that tells the rasterizer stage how to behave.
 *
 * \param _rdRasterizerDesc Reference to a rasterizer state description.
 * \param _pprsRasterizerState Address of a pointer to the rasterizer state object created.
 * \return Returns an error code indicating success or failure.
 */
HRESULT LSE_CALL LSG_CREATESTATESDEF_CLASS::CreateRasterizerState( const LSG_RASTERIZER_DESC &_rdRasterizerDesc, LSG_RASTERIZER_STATE ** _pprsRasterizerState ) {
	CCriticalSection::CLocker lLockMe( m_csRasterizerStateCrit );
	return CreateState<LSG_RASTERIZER_DESC, LSG_RASTERIZER_STATE, LSG_RASTERIZER_STATE_REF>( _rdRasterizerDesc, _pprsRasterizerState, m_mRasterizerStateMap );
}

/**
 * Creates a blend-state object that encapsules blend state for the output-merger stage.
 *
 * \param _bdBlendDesc Reference to a blend state description.
 * \param _ppbsBlendState Address of a pointer to the blend state object created.
 * \return Returns an error code indicating success or failure.
 */
HRESULT LSE_CALL LSG_CREATESTATESDEF_CLASS::CreateBlendState( const LSG_BLEND_DESC &_bdBlendDesc, LSG_BLEND_STATE ** _ppbsBlendState ) {
	CCriticalSection::CLocker lLockMe( m_csBlendStateCrit );
	return CreateState<LSG_BLEND_DESC, LSG_BLEND_STATE, LSG_BLEND_STATE_REF>( _bdBlendDesc, _ppbsBlendState, m_mBlendStateMap );
}

/**
 * Creates a depth-stencil state object that encapsulates depth-stencil test information for the output-merger stage.
 *
 * \param _dsdDepthStencilDesc Reference to a depth/stencil state description.
 * \param _ppdssDepthStencilState Address of a pointer to the depth/stencil state object created.
 * \return Returns an error code indicating success or failure.
 */
HRESULT LSE_CALL LSG_CREATESTATESDEF_CLASS::CreateDepthStencilState( const LSG_DEPTH_STENCIL_DESC &_dsdDepthStencilDesc, LSG_DEPTH_STENCIL_STATE ** _ppdssDepthStencilState ) {
	CCriticalSection::CLocker lLockMe( m_csDepthStencilStateCrit );
	return CreateState<LSG_DEPTH_STENCIL_DESC, LSG_DEPTH_STENCIL_STATE, LSG_DEPTH_STENCIL_STATE_REF>( _dsdDepthStencilDesc, _ppdssDepthStencilState, m_mDepthStencilStateMap );
}

/**
 * Releases the given sampler state.
 *
 * \param _pssSamplerState The sampler state to release.
 */
void LSE_CALL LSG_CREATESTATESDEF_CLASS::ReleaseState( LSG_SAMPLER_STATE * &_pssSamplerState ) {
	CCriticalSection::CLocker lLockMe( m_csSamplerStateCrit );
	ReleaseState<LSG_SAMPLER_DESC, LSG_SAMPLER_STATE, LSG_SAMPLER_STATE_REF>( _pssSamplerState, m_mSamplerStateMap );
	_pssSamplerState = NULL;
}

/**
 * Releases the given rasterizer state.
 *
 * \param _prsRasterizerState The rasterizer state to release.
 */
void LSE_CALL LSG_CREATESTATESDEF_CLASS::ReleaseState( LSG_RASTERIZER_STATE * &_prsRasterizerState ) {
	CCriticalSection::CLocker lLockMe( m_csRasterizerStateCrit );
	ReleaseState( _prsRasterizerState, m_mRasterizerStateMap );
	_prsRasterizerState = NULL;
}

/**
 * Releases the given blend state.
 *
 * \param _pbsBlendState The blend state to release.
 */
void LSE_CALL LSG_CREATESTATESDEF_CLASS::ReleaseState( LSG_BLEND_STATE * &_pbsBlendState ) {
	CCriticalSection::CLocker lLockMe( m_csBlendStateCrit );
	ReleaseState( _pbsBlendState, m_mBlendStateMap );
	_pbsBlendState = NULL;
}

/**
 * Releases the given depth-stencil state.
 *
 * \param _pdssDepthStencilState The depth-stencil state to release.
 */
void LSE_CALL LSG_CREATESTATESDEF_CLASS::ReleaseState( LSG_DEPTH_STENCIL_STATE * &_pdssDepthStencilState ) {
	CCriticalSection::CLocker lLockMe( m_csDepthStencilStateCrit );
	ReleaseState( _pdssDepthStencilState, m_mDepthStencilStateMap );
	_pdssDepthStencilState = NULL;
}

/**
 * Creates a state object.
 *
 * \param _tDesc State description.
 * \param _pptState Address of a pointer to the state object created.
 * \param _mMap Map to use.
 * \return Returns an error code indicating success or failure.
 */
template <typename _tDescType, typename _tStateType, typename _tStateRef>
HRESULT LSE_CALL LSG_CREATESTATESDEF_CLASS::CreateState( const _tDescType &_tDesc, _tStateType ** _pptState,
	CLinearMap<_tDescType, _tStateRef> &_mMap ) {
	uint32_t ui32Index;
	if ( _mMap.ItemExists( _tDesc, ui32Index ) ) {
		_tStateRef & srRef = _mMap.GetByIndex( ui32Index );
		++srRef.ui32Ref;
		(*_pptState) = srRef.psObj;
		return S_OK;
	}
	// Does not exist.  Add it.
	_tStateRef srRef = {
		1,
		LSENEW _tDescType
	};
	if ( !srRef.psObj ) { return E_OUTOFMEMORY; }
	(*srRef.psObj) = _tDesc;
	if ( !_mMap.Insert( _tDesc, srRef ) ) {
		LSEDELETE srRef.psObj;
		return E_OUTOFMEMORY;
	}
	(*_pptState) = srRef.psObj;
	return S_OK;
}

/**
 * Releases a state object.
 *
 * \param _ptState Address of a pointer to the state object to release.
 * \param _mMap Map to use.
 */
template <typename _tDescType, typename _tStateType, typename _tStateRef>
void LSE_CALL LSG_CREATESTATESDEF_CLASS::ReleaseState( _tStateType * _ptState, CLinearMap<_tDescType, _tStateRef> &_mMap ) {
	if ( !_ptState ) { return; }
	uint32_t ui32Index;
	if ( _mMap.ItemExists( static_cast<const _tDescType &>(*_ptState), ui32Index ) ) {
		_tStateRef & srRef = _mMap.GetByIndex( ui32Index );
		if ( !srRef.ui32Ref ) { return; }
		if ( !--srRef.ui32Ref ) {
			// Delete the object.
			LSEDELETE srRef.psObj;
			_mMap.RemoveByIndex( ui32Index );
			if ( _mMap.Length() == 0 ) {
				_mMap.Reset();	// Releases all memory.  Prevents leak messages at program shut-down.
			}
		}
	}
}

#undef LSG_CREATESTATESDEF_CLASS
