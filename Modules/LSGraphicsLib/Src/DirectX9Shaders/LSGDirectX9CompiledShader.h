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
 * Description: A base class for CDirectX9PixelShader and CDirectX9VertexShader which handles the common
 *	functionality between them.  Both classes have compiled code and a constant table, and a way to convert
 *	handles from that constant table handle type to our custom handle type.
 */


#ifndef __LSG_DIRECTX9COMPILEDSHADER_H__
#define __LSG_DIRECTX9COMPILEDSHADER_H__

#include "../LSGGraphicsLib.h"

#ifdef LSG_DX9

#include "../DirectX9/LSGDirectX9.h"
#include "../DirectX9/LSGDirectX9LosableResource.h"
#include "../ShaderLanguage/LSGCompiledShaderBase.h"
#include "../ShaderLanguage/LSGShaderBase.h"

namespace lsg {

	/**
	 * Class CDirectX9CompiledShader
	 * \brief A base class for CDirectX9PixelShader and CDirectX9VertexShader which handles the common
	 *	functionality between them.
	 *
	 * Description: A base class for CDirectX9PixelShader and CDirectX9VertexShader which handles the common
	 *	functionality between them.  Both classes have compiled code and a constant table, and a way to convert
	 *	handles from that constant table handle type to our custom handle type.
	 */
	class CDirectX9CompiledShader : public CCompiledShaderBase, public CDirectX9LosableResource {
	public :
		// == Various constructors.
		LSE_CALLCTOR										CDirectX9CompiledShader();
		LSE_CALLCTOR										~CDirectX9CompiledShader();


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


	protected :
		// == Types.
		/**
		 * Mapping of constant handles from the ID3DXConstantTable to our custom type.
		 */
		typedef struct LSG_CONSTANT_MAP {
			/**
			 * The handle to the constant in the ID3DXConstantTable object.
			 */
			D3DXHANDLE										hDirectXHandle;

			/**
			 * The name of the constant.
			 */
			uint32_t										ui32Name;

			/**
			 * Pointer to the start of the uniform buffer data.
			 */
			uint32_t										ui32DataPtr;

			/**
			 * Determines if the data is dirty.  If so, it is updated without
			 *	being checked.
			 */
			LSBOOL											bDirty;
		} * LPLSG_CONSTANT_MAP, * const LPCLSG_CONSTANT_MAP;


		// == Members.
		/**
		 * The compiled shader.
		 */
		ID3DXBuffer *										m_pbShader;

		/**
		 * Constant table.
		 */
		ID3DXConstantTable *								m_pctConstantTable;

		/**
		 * Uniform globals in the shaders.
		 */
		CShaderTranslationProcessorBase::LSG_UNIFORM_LIST	m_ulUniforms;

		/**
		 * Pointer into the data for the next uniform handle.
		 */
		uint32_t											m_ui32NextUniformStart;

		/**
		 * Uniform local buffer.  Used to prevent redundant updating of values.
		 */
		CVectorPoD<uint8_t, uint32_t>						m_vUniformBuffer;

		/**
		 * Map of constants and their handles/other data related to them.
		 */
		CVectorPoD<LSG_CONSTANT_MAP>						m_vConstantMap;

		/**
		 * Temporary buffer.
		 */
		CVectorPoD<uint8_t, uint32_t>						m_vTempBuffer;


		// == Functions.
		/**
		 * Safely release an object.
		 *
		 * \param _tObj The object to release.
		 */
		template <typename tnType>
		static void LSE_CALL								SafeRelease( tnType &_tObj );


	private :
		typedef CCompiledShaderBase							Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Safely release an object.
	 *
	 * \param _tObj The object to release.
	 */
	template <typename tnType>
	LSE_INLINE void LSE_CALL CDirectX9CompiledShader::SafeRelease( tnType &_tObj ) {
		if ( _tObj ) {
			_tObj->Release();
			_tObj = NULL;
		}
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9

#endif	// __LSG_DIRECTX9COMPILEDSHADER_H__
