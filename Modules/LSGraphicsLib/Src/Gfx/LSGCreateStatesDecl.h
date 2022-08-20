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
 * Description: This is meant to be included directly into the graphics API somewhere after it defines its
 *	API-specific constants and enumerations.
 * These are the declarations for methods that create and release various types of states.
 */


// == Functions.
/**
 * Creates a sampler-state object that encapsulates sampling information for a texture.
 *
 * \param _sdSamplerDesc Reference to a sampler state description.
 * \param _ppssSamplerState Address of a pointer to the sampler state object created.
 * \return Returns an error code indicating success or failure.
 */
static HRESULT LSE_CALL						CreateSamplerState( const LSG_SAMPLER_DESC &_sdSamplerDesc, LSG_SAMPLER_STATE ** _ppssSamplerState );

/**
 * Creates a rasterizer state object that tells the rasterizer stage how to behave.
 *
 * \param _rdRasterizerDesc Reference to a rasterizer state description.
 * \param _pprsRasterizerState Address of a pointer to the rasterizer state object created.
 * \return Returns an error code indicating success or failure.
 */
static HRESULT LSE_CALL						CreateRasterizerState( const LSG_RASTERIZER_DESC &_rdRasterizerDesc, LSG_RASTERIZER_STATE ** _pprsRasterizerState );

/**
 * Creates a blend-state object that encapsules blend state for the output-merger stage.
 *
 * \param _bdBlendDesc Reference to a blend state description.
 * \param _ppbsBlendState Address of a pointer to the blend state object created.
 * \return Returns an error code indicating success or failure.
 */
static HRESULT LSE_CALL						CreateBlendState( const LSG_BLEND_DESC &_bdBlendDesc, LSG_BLEND_STATE ** _ppbsBlendState );

/**
 * Creates a depth-stencil state object that encapsulates depth-stencil test information for the output-merger stage.
 *
 * \param _dsdDepthStencilDesc Reference to a depth/stencil state description.
 * \param _ppdssDepthStencilState Address of a pointer to the depth/stencil state object created.
 * \return Returns an error code indicating success or failure.
 */
static HRESULT LSE_CALL						CreateDepthStencilState( const LSG_DEPTH_STENCIL_DESC &_dsdDepthStencilDesc, LSG_DEPTH_STENCIL_STATE ** _ppdssDepthStencilState );

/**
 * Releases the given sampler state.
 *
 * \param _pssSamplerState The sampler state to release.
 */
static void LSE_CALL						ReleaseState( LSG_SAMPLER_STATE * &_pssSamplerState );

/**
 * Releases the given rasterizer state.
 *
 * \param _prsRasterizerState The rasterizer state to release.
 */
static void LSE_CALL						ReleaseState( LSG_RASTERIZER_STATE * &_prsRasterizerState );

/**
 * Releases the given blend state.
 *
 * \param _pbsBlendState The blend state to release.
 */
static void LSE_CALL						ReleaseState( LSG_BLEND_STATE * &_pbsBlendState );

/**
 * Releases the given depth-stencil state.
 *
 * \param _pdssDepthStencilState The depth-stencil state to release.
 */
static void LSE_CALL						ReleaseState( LSG_DEPTH_STENCIL_STATE * &_pdssDepthStencilState );

/**
 * Creates a state object.
 *
 * \param _tDesc State description.
 * \param _pptState Address of a pointer to the state object created.
 * \param _mMap Map to use.
 * \return Returns an error code indicating success or failure.
 */
template <typename _tDescType, typename _tStateType, typename _tStateRef>
static HRESULT LSE_CALL						CreateState( const _tDescType &_tDesc, _tStateType ** _pptState,
	CLinearMap<_tDescType, _tStateRef> &_mMap );

/**
 * Releases a state object.
 *
 * \param _ptState Address of a pointer to the state object to release.
 * \param _mMap Map to use.
 */
template <typename _tDescType, typename _tStateType, typename _tStateRef>
static void LSE_CALL						ReleaseState( _tStateType * _ptState, CLinearMap<_tDescType, _tStateRef> &_mMap );
