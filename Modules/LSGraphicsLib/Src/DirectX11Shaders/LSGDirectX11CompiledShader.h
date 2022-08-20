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
 * Description: A base class for CDirectX11PixelShader and CDirectX11VertexShader which handles the common
 *	functionality between them.  Both classes have compiled code and a few constant buffers, and a way to convert
 *	handles from that constant table handle type to our custom handle type.
 */


#ifndef __LSG_DIRECTX11COMPILEDSHADER_H__
#define __LSG_DIRECTX11COMPILEDSHADER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX11

#include "../DirectX11/LSGDirectX11.h"
#include "../ShaderLanguage/LSGCompiledShaderBase.h"
#include "../ShaderLanguage/LSGShaderBase.h"

namespace lsg {

	/**
	 * Class CDirectX11CompiledShader
	 * \brief A base class for CDirectX11PixelShader and CDirectX11VertexShader which handles the common
	 *	functionality between them.
	 *
	 * Description: A base class for CDirectX11PixelShader and CDirectX11VertexShader which handles the common
	 *	functionality between them.  Both classes have compiled code and a few constant buffers, and a way to convert
	 *	handles from that constant table handle type to our custom handle type.
	 */
	class CDirectX11CompiledShader : public CCompiledShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX11CompiledShader();
		LSE_CALLCTOR										~CDirectX11CompiledShader();


		// == Functions.
		/**
		 * Reset everything to scratch.
		 */
		virtual void LSE_CALL								Reset();

		/**
		 * Sets the uniforms associated with this shader.
		 *
		 * \param _ulUniforms The uniforms to copy to this shader.
		 * \return Returns true if there is enough memory to copy the list of uniforms.
		 */
		LSBOOL LSE_CALL										SetUniforms( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST &_ulUniforms );

		/**
		 * Gets a handle for a uniform variable in the shader.
		 *
		 * \param _pcName Name of the variable whose handle is to be obtained.
		 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
		 */
		CShaderBase::LSG_HANDLE LSE_CALL					GetConstantByName( const char * _pcName );

		/**
		 * Sets a single scalar value in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _fValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue );

		/**
		 * Sets an array of scalar values in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pfValues New values.
		 * \param _ui32Total Total floats to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total );

		/**
		 * Sets a single scalar value in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _iValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetInt( CShaderBase::LSG_HANDLE _hHandle, int32_t _iValue );

		/**
		 * Sets an array of scalar values in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pi32Values New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total );

		/**
		 * Sets a single boolean value in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _bValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetBool( CShaderBase::LSG_HANDLE _hHandle, LSBOOL _bValue );

		/**
		 * Sets an array of boolean values in the shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pi32Values New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total );

		/**
		 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue );

		/**
		 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue );

		/**
		 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue );

		/**
		 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total );

		/**
		 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue );

		/**
		 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue );

		/**
		 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		LSBOOL LSE_CALL										SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue );

		/**
		 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pmValues New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		LSBOOL LSE_CALL										SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total );

		/**
		 * Gets the blob's signature.
		 *
		 * \return Returns a constant pointer to the blob data.
		 */
		LSE_INLINE const void * LSE_CALL					GetBufferPointer() const;

		/**
		 * Gets the blob's size.
		 *
		 * \return Returns the size of the blob data.
		 */
		LSE_INLINE SIZE_T LSE_CALL							GetBufferSize() const;


	protected :
		// == Types.
		/**
		 * Mapping of constant handles from the ID3DXConstantTable to our custom type.
		 */
		typedef struct LSG_CONSTANT_MAP {
			/**
			 * The name of the constant.
			 */
			uint32_t										ui32Name;

			/**
			 * Index into the array of cbuffers.
			 */
			uint32_t										ui32CBufferIndex;

			/**
			 * Pointer to the start of the uniform buffer data.
			 */
			uint32_t										ui32DataPtr;
		} * LPLSG_CONSTANT_MAP, * const LPCLSG_CONSTANT_MAP;

		/**
		 * A cbuffer.  It is just an array of bytes and a dirty flag.
		 */
		typedef struct LSG_CS_CBUFFER {
			/**
			 * The dirty flag.  If dirty, the buffer has to be remapped.
			 */
			LSBOOL											bDirty;

			/**
			 * The raw copy of the buffer data.
			 */
			CVector<uint8_t, uint32_t>						vRaw;
		} * LPLSG_CS_CBUFFER, * const LPCLSG_CS_CBUFFER;


		// == Members.
		/**
		 * The compiled shader.
		 */
		ID3DBlob *											m_pbShader;

		/**
		 * Constant tables, one for each cbuffer.
		 */
		CVectorPoD<ID3D11Buffer *>							m_vConstantBuffers;

		/**
		 * Array of cbuffers.
		 */
		CVector<LSG_CS_CBUFFER>								m_vConstBufferRawData;

		/**
		 * Uniform globals in the shaders.
		 */
		CShaderTranslationProcessorBase::LSG_UNIFORM_LIST	m_ulUniforms;

		/**
		 * Map of constants and their handles/other data related to them.
		 */
		CVectorPoD<LSG_CONSTANT_MAP>						m_vConstantMap;

		/**
		 * Temporary buffer.
		 */
		CVectorPoD<uint8_t, uint32_t>						m_vTempBuffer;


		// == Functions.


	private :
		typedef CCompiledShaderBase							Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the blob's signature.
	 *
	 * \return Returns a constant pointer to the blob data.
	 */
	LSE_INLINE const void * LSE_CALL CDirectX11CompiledShader::GetBufferPointer() const {
		return m_pbShader ? m_pbShader->GetBufferPointer() : NULL;
	}

	/**
	 * Gets the blob's size.
	 *
	 * \return Returns the size of the blob data.
	 */
	LSE_INLINE SIZE_T LSE_CALL CDirectX11CompiledShader::GetBufferSize() const {
		return m_pbShader ? m_pbShader->GetBufferSize() : 0;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX11

#endif	// __LSG_DIRECTX11COMPILEDSHADER_H__
