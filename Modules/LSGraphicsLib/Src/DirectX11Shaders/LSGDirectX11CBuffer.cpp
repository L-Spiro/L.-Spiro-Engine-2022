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
 * Description: A software-side cbuffer for Direct3D 11.
 */

#include "LSGDirectX11CBuffer.h"

#ifdef LSG_DX11

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11CBuffer::CDirectX11CBuffer() {
	}
	LSE_CALLCTOR CDirectX11CBuffer::~CDirectX11CBuffer() {
	}

	// == Functions.
	/**
	 * Adds the given uniform to this cbuffer representative class.
	 *
	 * \param _uUniform The uniform to add.
	 * \param _ui32Offset The optional offset at which to add it.  Pass LSE_MAXU32 for automatic
	 *	offset determination.
	 * \return Returns the actual offset where the uniform was placed into the constant buffer or LSE_MAXU32 upon
	 *	a memory failure.
	 */
	uint32_t LSE_CALL CDirectX11CBuffer::AddUniform( const CShaderTranslationProcessorBase::LSG_UNIFORM &_uUniform, uint32_t _ui32Offset ) {
		if ( _ui32Offset == LSE_MAXU32 ) {
			// Get the next available offset for the given uniform.
			if ( !m_vUniforms.Length() ) {
				// Easy.
				_ui32Offset = 0;
			}
			else {
				const LSG_CBUFFER_UNIFORM & uThis = m_vUniforms[m_vUniforms.Length()-1];
				_ui32Offset = uThis.ui32BufferOffset + uThis.ui32Size;
			}
		}
		// First, everything starts on 4-byte boundaries.
		_ui32Offset = LSE_ROUND_UP( _ui32Offset, 4 );
		// Next, they can't straddle 16-byte boundaries.
		if ( _ui32Offset + _uUniform.ui32Size > LSE_ROUND_UP( _ui32Offset, 16 ) ) {
			_ui32Offset = LSE_ROUND_UP( _ui32Offset, 16 );
		}

		if ( !m_vUniforms.Push( LSG_CBUFFER_UNIFORM() ) ) { return LSE_MAXU32; }
		LSG_CBUFFER_UNIFORM & cuThis = m_vUniforms[m_vUniforms.Length()-1];
		static_cast<CShaderTranslationProcessorBase::LSG_UNIFORM &>(cuThis) = _uUniform;
		cuThis.ui32BufferOffset = _ui32Offset;
		m_vUniforms.Snap();

		return _ui32Offset;
	}

	/**
	 * Gets a constant reference to the array of uniforms in the buffer.
	 *
	 * \return Returns a constant reference to the array of uniforms in the buffer.
	 */
	const CVector<CDirectX11CBuffer::LSG_CBUFFER_UNIFORM> & LSE_CALL CDirectX11CBuffer::Uniforms() const {
		return m_vUniforms;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11
