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
 *	API-specific constants and enumerations.  For example, the enumerated values in LSG_BLEND_MODE will be
 *	different values for each API, though each enumerated literal (LSG_BM_ADD, etc.) will be the same.  Since
 *	these enumerated values change depending on the API, these structures cannot be defined until after all
 *	of the enumerated values have been defined, which means these structures would have to be either manually
 *	redefined for each API or just put into a header file and #included at the appropriate point in each API
 *	header.
 * These structures define the descriptions for getting render-state objects.
 */

// == Types.
/** Sampler description. */
typedef struct LSG_SAMPLER_DESC {
	LSG_FILTER									Filter;											/**< Filtering method to use when sampling a texture. */
	LSG_TEXTURE_ADDRESS_MODE					AddressU;										/**< Method to use for resolving a u texture coordinate that is outside the 0 to 1 range. */
	LSG_TEXTURE_ADDRESS_MODE					AddressV;										/**< Method to use for resolving a v texture coordinate that is outside the 0 to 1 range. */
	LSG_TEXTURE_ADDRESS_MODE					AddressW;										/**< Method to use for resolving a w texture coordinate that is outside the 0 to 1 range. */
	LSGREAL										MipLODBias;										/**< Offset from the calculated mipmap level. */
	uint32_t									MaxAnisotropy;									/**< Clamping value used if LSG_F_ANISOTROPIC is specified in Filter. */
	LSG_COMPARISON_FUNC							ComparisonFunc;									/**< A function that compares sampled data against existing sampled data. */
	LSGREAL										BorderColor[4];									/**< Border color to use if LSG_TAM_BORDER is specified for AddressU, AddressV, or AddressW. Range must be between 0.0 and 1.0 inclusive. */
	LSGREAL										MinLOD;											/**< Lower end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed. */
	LSGREAL										MaxLOD;											/**< Upper end of the mipmap range to clamp access to, where 0 is the largest and most detailed mipmap level and any level higher than that is less detailed. This value must be greater than or equal to MinLOD. To have no upper limit on LOD set this to a large value. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _sdOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_SAMPLER_DESC &_sdOther ) const {
		return CStd::MemCmp( this, &_sdOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _sdOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_SAMPLER_DESC &_sdOther ) const {
		return CStd::MemCmpF( this, &_sdOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_SAMPLER_DESC, * const LPCLSG_SAMPLER_DESC;

/** Rasterizer description. */
typedef struct LSG_RASTERIZER_DESC {
	LSG_FILL_MODE								FillMode;										/**< Determines the fill mode to use when rendering. */
	LSG_CULL_MODE								CullMode;										/**< Indicates triangles facing the specified direction are not drawn. */
	LSBOOL										FrontCounterClockwise;							/**< Determines if a triangle is front- or back-facing. If this parameter is TRUE, a triangle will be considered front-facing if its vertices are counter-clockwise on the render target and considered back-facing if they are clockwise. If this parameter is FALSE, the opposite is true. */
	int32_t										DepthBias;										/**< Depth value added to a given pixel. */
	LSGREAL										DepthBiasClamp;									/**< Maximum depth bias of a pixel. */
	LSGREAL										SlopeScaledDepthBias;							/**< Scalar on a given pixel's slope. */
	LSBOOL										DepthClipEnable;								/**< Enable clipping based on distance. */
	LSBOOL										ScissorEnable;									/**< Enable scissor-rectangle culling. All pixels outside an active scissor rectangle are culled. */
	LSBOOL										MultisampleEnable;								/**< Specifies whether to use the quadrilateral or alpha line anti-aliasing algorithm on multisample antialiasing (MSAA) render targets. Set to TRUE to use the quadrilateral line anti-aliasing algorithm and to FALSE to use the alpha line anti-aliasing algorithm. */
	LSBOOL										AntialiasedLineEnable;							/**< Specifies whether to enable line antialiasing; only applies if doing line drawing and MultisampleEnable is FALSE. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _rdOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_RASTERIZER_DESC &_rdOther ) const {
		return CStd::MemCmp( this, &_rdOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _rdOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_RASTERIZER_DESC &_rdOther ) const {
		return CStd::MemCmpF( this, &_rdOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_RASTERIZER_DESC, * const LPCLSG_RASTERIZER_DESC;

/** Render-target blend description. */
typedef struct LSG_RENDER_TARGET_BLEND_DESC {
	LSBOOL										BlendEnable;									/**< Enable (or disable) blending. */
	LSG_BLEND_MODE								SrcBlend;										/**< This blend option specifies the operation to perform on the RGB value that the pixel shader outputs. The BlendOp member defines how to combine the SrcBlend and DestBlend operations. */
	LSG_BLEND_MODE								DestBlend;										/**< This blend option specifies the operation to perform on the current RGB value in the render target. The BlendOp member defines how to combine the SrcBlend and DestBlend operations. */
	LSG_BLEND_OP								BlendOp;										/**< This blend operation defines how to combine the SrcBlend and DestBlend operations. */
	LSG_BLEND_MODE								SrcBlendAlpha;									/**< This blend option specifies the operation to perform on the alpha value that the pixel shader outputs. Blend options that end in _COLOR are not allowed. The BlendOpAlpha member defines how to combine the SrcBlendAlpha and DestBlendAlpha operations. */
	LSG_BLEND_MODE								DestBlendAlpha;									/**< This blend option specifies the operation to perform on the current alpha value in the render target. Blend options that end in _COLOR are not allowed. The BlendOpAlpha member defines how to combine the SrcBlendAlpha and DestBlendAlpha operations. */
	LSG_BLEND_OP								BlendOpAlpha;									/**< This blend operation defines how to combine the SrcBlendAlpha and DestBlendAlpha operations. */
	uint8_t										RenderTargetWriteMask;							/**< A write mask. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _rtbdOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_RENDER_TARGET_BLEND_DESC &_rtbdOther ) const {
		return CStd::MemCmp( this, &_rtbdOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _rtbdOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_RENDER_TARGET_BLEND_DESC &_rtbdOther ) const {
		return CStd::MemCmpF( this, &_rtbdOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_RENDER_TARGET_BLEND_DESC, * const LPCLSG_RENDER_TARGET_BLEND_DESC;

/** Blend description. */
typedef struct LSG_BLEND_DESC {
	LSBOOL										AlphaToCoverageEnable;							/**< Specifies whether to use alpha-to-coverage as a multisampling technique when setting a pixel to a render target. */
	LSBOOL										IndependentBlendEnable;							/**< Specifies whether to enable independent blending in simultaneous render targets. Set to TRUE to enable independent blending. */
	LSG_RENDER_TARGET_BLEND_DESC				RenderTarget[LSG_MAX_RENDER_TARGET_COUNT];		/**< An array of LSG_RENDER_TARGET_BLEND_DESC structures that describe the blend states for render targets; these correspond to the eight render targets that can be bound to the output-merger stage at one time. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _bdOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_BLEND_DESC &_bdOther ) const {
		return CStd::MemCmp( this, &_bdOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _bdOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_BLEND_DESC &_bdOther ) const {
		return CStd::MemCmpF( this, &_bdOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_BLEND_DESC, * const LPCLSG_BLEND_DESC;

/** Depth-stencil operation description. */
typedef struct LSG_DEPTH_STENCILOP_DESC {
	LSG_STENCIL_OP								StencilFailOp;									/**< The stencil operation to perform when stencil testing fails. */
	LSG_STENCIL_OP								StencilDepthFailOp;								/**< The stencil operation to perform when stencil testing passes and depth testing fails. */
	LSG_STENCIL_OP								StencilPassOp;									/**< The stencil operation to perform when stencil testing and depth testing both pass. */
	LSG_COMPARISON_FUNC							StencilFunc;									/**< A function that compares stencil data against existing stencil data. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _dsodOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_DEPTH_STENCILOP_DESC &_dsodOther ) const {
		return CStd::MemCmp( this, &_dsodOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _dsodOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_DEPTH_STENCILOP_DESC &_dsodOther ) const {
		return CStd::MemCmpF( this, &_dsodOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_DEPTH_STENCILOP_DESC, * const LPCLSG_DEPTH_STENCILOP_DESC;

/** Depth-stencil description. */
typedef struct LSG_DEPTH_STENCIL_DESC {
	LSBOOL										DepthEnable;									/**< Enable depth testing. */
	LSG_DEPTH_WRITE_MASK						DepthWriteMask;									/**< Identify a portion of the depth-stencil buffer that can be modified by depth data. */
	LSG_COMPARISON_FUNC							DepthFunc;										/**< A function that compares depth data against existing depth data. */
	LSBOOL										StencilEnable;									/**< Enable stencil testing. */
	uint8_t										StencilReadMask;								/**< Identify a portion of the depth-stencil buffer for reading stencil data. */
	uint8_t										StencilWriteMask;								/**< Identify a portion of the depth-stencil buffer for writing stencil data. */
	LSG_DEPTH_STENCILOP_DESC					FrontFace;										/**< Identify how to use the results of the depth test and the stencil test for pixels whose surface normal is facing towards the camera. */
	LSG_DEPTH_STENCILOP_DESC					BackFace;										/**< Identify how to use the results of the depth test and the stencil test for pixels whose surface normal is facing away from the camera. */


	// == Operators.
	/**
	 * Less-than comparison operator.
	 *
	 * \param _dsdOther Object against which to test for this object being less than.
	 * \return Returns true if this object is less than the given object by memory compare.
	 */
	bool LSE_FCALL								operator < ( const LSG_DEPTH_STENCIL_DESC &_dsdOther ) const {
		return CStd::MemCmp( this, &_dsdOther, sizeof( (*this) ) ) < 0;
	}

	/**
	 * Equality operator.
	 *
	 * \param _dsdOther Object against which to test for this object being equal.
	 * \return Returns true if this object is equal to the given object by memory compare.
	 */
	bool LSE_FCALL								operator == ( const LSG_DEPTH_STENCIL_DESC &_dsdOther ) const {
		return CStd::MemCmpF( this, &_dsdOther, sizeof( (*this) ) ) == static_cast<LSBOOL>(true);
	}
} * LPLSG_DEPTH_STENCIL_DESC, * const LPCLSG_DEPTH_STENCIL_DESC;
