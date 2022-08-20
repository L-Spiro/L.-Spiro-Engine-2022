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
 *	LSG_DESCFUNCDEF_CLASS must be defined.
 */


#ifndef LSG_DESCFUNCDEF_CLASS
#error "LSG_DESCFUNCDEF_CLASS must be defined (for example as CDirectX9 or COpenGl)."
#endif	// #ifndef LSG_DESCFUNCDEF_CLASS

#define LSG_ENUM_ASSIGN( DEST, VAL )	DEST = CStd::ImpCast( DEST, VAL )

/**
 * Fills an LSG_SAMPLER_DESC structure with default values and returns the input structure.
 *
 * \param _sdDesc The sampler description whose default values are to be set.
 * \return Returns the input _sdDesc.
 */
LSG_SAMPLER_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultSamplerDesc( LSG_SAMPLER_DESC &_sdDesc ) {
	LSG_ENUM_ASSIGN( _sdDesc.Filter, LSG_F_ANISOTROPIC );
	LSG_ENUM_ASSIGN( _sdDesc.AddressU, LSG_TAM_WRAP );
	LSG_ENUM_ASSIGN( _sdDesc.AddressV, LSG_TAM_WRAP );
	LSG_ENUM_ASSIGN( _sdDesc.AddressW, LSG_TAM_WRAP );
	_sdDesc.MinLOD = static_cast<LSGREAL>(-LSM_FLT_MAX);
	_sdDesc.MaxLOD = static_cast<LSGREAL>(LSM_FLT_MAX);
	_sdDesc.MipLODBias = static_cast<LSGREAL>(0.0);
	_sdDesc.MaxAnisotropy = 16;
	LSG_ENUM_ASSIGN( _sdDesc.ComparisonFunc, LSG_CF_NEVER );
	_sdDesc.BorderColor[0] = _sdDesc.BorderColor[1] =
		_sdDesc.BorderColor[2] = _sdDesc.BorderColor[3] = static_cast<LSGREAL>(1.0);
	_sdDesc.MinLOD = static_cast<LSGREAL>(0.0);
	_sdDesc.MaxLOD = static_cast<LSGREAL>(1000.0);
	return _sdDesc;
}

/**
 * Fills an LSG_RASTERIZER_DESC structure with default values and returns the input structure.
 *
 * \param _rdDesc The rasterizer description whose default values are to be set.
 * \return Returns the input _rdDesc.
 */
LSG_RASTERIZER_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultRasterizerDesc( LSG_RASTERIZER_DESC &_rdDesc ) {
	LSG_ENUM_ASSIGN( _rdDesc.FillMode, LSG_FM_SOLID );
	LSG_ENUM_ASSIGN( _rdDesc.CullMode, LSG_CM_BACK );
	_rdDesc.FrontCounterClockwise = false;
	_rdDesc.DepthBias = 0;
	_rdDesc.SlopeScaledDepthBias = static_cast<LSGREAL>(0.0);
	_rdDesc.DepthBiasClamp = static_cast<LSGREAL>(0.0);
	_rdDesc.DepthClipEnable = true;
	_rdDesc.ScissorEnable = false;
	_rdDesc.MultisampleEnable = false;
	_rdDesc.AntialiasedLineEnable = false;
	return _rdDesc;
}

/**
 * Fills an LSG_RENDER_TARGET_BLEND_DESC structure with default values and returns the input structure.
 *
 * \param _rtbdDesc The render-target blend description whose default values are to be set.
 * \return Returns the input _rtbdDesc.
 */
LSG_RENDER_TARGET_BLEND_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultRenderTargetBlendDesc( LSG_RENDER_TARGET_BLEND_DESC &_rtbdDesc ) {
	_rtbdDesc.BlendEnable = false;
	LSG_ENUM_ASSIGN( _rtbdDesc.SrcBlend, LSG_BM_ONE );
	LSG_ENUM_ASSIGN( _rtbdDesc.SrcBlendAlpha, LSG_BM_ONE );
	LSG_ENUM_ASSIGN( _rtbdDesc.DestBlend, LSG_BM_ZERO );
	LSG_ENUM_ASSIGN( _rtbdDesc.DestBlendAlpha, LSG_BM_ZERO );
	LSG_ENUM_ASSIGN( _rtbdDesc.BlendOp, LSG_BO_ADD );
	LSG_ENUM_ASSIGN( _rtbdDesc.BlendOpAlpha, LSG_BO_ADD );
	LSG_ENUM_ASSIGN( _rtbdDesc.RenderTargetWriteMask, LSG_CWM_ALL );
	return _rtbdDesc;
}

/**
 * Fills an LSG_BLEND_DESC structure with default values and returns the input structure.
 *
 * \param _bdDesc The blend description whose default values are to be set.
 * \return Returns the input _bdDesc.
 */
LSG_BLEND_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultBlendDesc( LSG_BLEND_DESC &_bdDesc ) {
	_bdDesc.AlphaToCoverageEnable = false;
	_bdDesc.IndependentBlendEnable = false;
	for ( uint32_t I = LSE_ELEMENTS( _bdDesc.RenderTarget ); I--; ) {
		DefaultRenderTargetBlendDesc( _bdDesc.RenderTarget[I] );
	}
	return _bdDesc;
}

/**
 * Fills an LSG_DEPTH_STENCILOP_DESC structure with default values and returns the input structure.
 *
 * \param _dsodDesc The depth/stencil operation description whose default values are to be set.
 * \return Returns the input _dsodDesc.
 */
LSG_DEPTH_STENCILOP_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultDepthStencilOpDesc( LSG_DEPTH_STENCILOP_DESC &_dsodDesc ) {
	LSG_ENUM_ASSIGN( _dsodDesc.StencilFunc, LSG_CF_ALWAYS );
	LSG_ENUM_ASSIGN( _dsodDesc.StencilFailOp, LSG_SO_KEEP );
	LSG_ENUM_ASSIGN( _dsodDesc.StencilDepthFailOp, LSG_SO_KEEP );
	LSG_ENUM_ASSIGN( _dsodDesc.StencilPassOp, LSG_SO_KEEP );
	return _dsodDesc;
}

/**
 * Fills an LSG_DEPTH_STENCIL_DESC structure with default values and returns the input structure.
 *
 * \param _dsdDesc The depth/stencil description whose default values are to be set.
 * \return Returns the input _dsdDesc.
 */
LSG_DEPTH_STENCIL_DESC & LSE_FCALL LSG_DESCFUNCDEF_CLASS::DefaultDepthStencilDesc( LSG_DEPTH_STENCIL_DESC &_dsdDesc ) {
	_dsdDesc.DepthEnable = true;
	LSG_ENUM_ASSIGN( _dsdDesc.DepthWriteMask, LSG_DRM_ALL );
	LSG_ENUM_ASSIGN( _dsdDesc.DepthFunc, LSG_CF_LESS );
	_dsdDesc.StencilEnable = false;
	_dsdDesc.StencilReadMask = LSG_DEFAULT_STENCIL_READ_MASK;
	_dsdDesc.StencilWriteMask = LSG_DEFAULT_STENCIL_WRITE_MASK;
	_dsdDesc.BackFace = DefaultDepthStencilOpDesc( _dsdDesc.FrontFace );
	return _dsdDesc;
}

#undef LSG_ENUM_ASSIGN
#undef LSG_DESCFUNCDEF_CLASS
