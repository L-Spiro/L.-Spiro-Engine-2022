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


#ifndef __LSG_DIRECX11CBUFFER_H__
#define __LSG_DIRECX11CBUFFER_H__

#include "../LSGGraphicsLib.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"

#ifdef LSG_DX11

namespace lsg {

	/**
	 * Class CDirectX11CBuffer
	 * \brief A software-side cbuffer for Direct3D 11.
	 *
	 * Description: A software-side cbuffer for Direct3D 11.
	 */
	class CDirectX11CBuffer {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CDirectX11CBuffer();
		LSE_CALLCTOR							~CDirectX11CBuffer();


		// == Types.
		/**
		 * An entry in the buffer.
		 */
		typedef struct LSG_CBUFFER_UNIFORM : public CShaderTranslationProcessorBase::LSG_UNIFORM {
			/**
			 * Its offset into the cbuffer.
			 */
			uint32_t							ui32BufferOffset;
		} * LPLSG_CBUFFER_UNIFORM, * const LPCLSG_CBUFFER_UNIFORM;


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
		uint32_t LSE_CALL						AddUniform( const CShaderTranslationProcessorBase::LSG_UNIFORM &_uUniform, uint32_t _ui32Offset );

		/**
		 * Gets a constant reference to the array of uniforms in the buffer.
		 *
		 * \return Returns a constant reference to the array of uniforms in the buffer.
		 */
		const CVector<LSG_CBUFFER_UNIFORM> & LSE_CALL
												Uniforms() const;


	protected :
		// == Members.
		/**
		 * An array of uniforms inside this buffer.
		 */
		CVector<LSG_CBUFFER_UNIFORM>			m_vUniforms;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECX11CBUFFER_H__
