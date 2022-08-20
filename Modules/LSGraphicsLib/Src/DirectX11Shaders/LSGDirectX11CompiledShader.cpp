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

#include "LSGDirectX11CompiledShader.h"

#ifdef LSG_DX11

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/**
 * Gets the buffer location of a given handle.
 */
#define LSG_HANDLE2CBUF( HANDLE )		m_vConstBufferRawData[m_vConstantMap[HANDLE].ui32CBufferIndex].vRaw[m_vConstantMap[HANDLE].ui32DataPtr]

/**
 * Gets the dirty flag for a given handle.
 */
#define LSG_HANDLE2DIRTY( HANDLE )		m_vConstBufferRawData[m_vConstantMap[HANDLE].ui32CBufferIndex].bDirty

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX11CompiledShader::CDirectX11CompiledShader() :
		m_pbShader( NULL ) {
	}
	LSE_CALLCTOR CDirectX11CompiledShader::~CDirectX11CompiledShader() {
		Reset();
	}


	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CDirectX11CompiledShader::Reset() {
		for ( uint32_t I = m_vConstantBuffers.Length(); I--; ) {
			CDirectX11::SafeRelease( m_vConstantBuffers[I] );
		}
		CDirectX11::SafeRelease( m_pbShader );
		m_vConstantMap.Reset();
		m_ulUniforms.Reset();
		m_vConstBufferRawData.Reset();
		
		Parent::Reset();
	}

	/**
	 * Sets the uniforms associated with this shader.
	 *
	 * \param _ulUniforms The uniforms to copy to this shader.
	 * \return Returns true if there is enough memory to copy the list of uniforms.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetUniforms( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST &_ulUniforms ) {
		m_vConstantMap.Reset();
		m_ulUniforms = _ulUniforms;

		// Temporarily hardcode 4 for the number of cbuffers, but it is an array for ease in upgrading in the future.
		if ( !m_vConstBufferRawData.Resize( 4 ) ) { return false; }
		CVector<uint32_t> vSizes;
		if ( !vSizes.Resize( m_vConstBufferRawData.Length() ) ) { return false; }
		for ( uint32_t I = vSizes.Length(); I--; ) {
			vSizes[I] = 0;
		}
		for ( uint32_t I = m_ulUniforms.Length(); I--; ) {
			if ( static_cast<uint32_t>(m_ulUniforms[I].uiptrApiSpecific) == LSE_MAXU32 ) { continue; }
			uint32_t ui32CBuffer = static_cast<uint32_t>(m_ulUniforms[I].uiptrApiParam);
			uint32_t ui32Size = CStd::Max<uint32_t>( vSizes[ui32CBuffer], static_cast<uint32_t>(m_ulUniforms[I].uiptrApiSpecific) + m_ulUniforms[I].ui32Size );
			vSizes[ui32CBuffer] = ui32Size;
		}
		// Resize the arrays.
		for ( uint32_t I = m_vConstBufferRawData.Length(); I--; ) {
			m_vConstBufferRawData[I].bDirty = true;
			if ( vSizes[I] ) {
				if ( !m_vConstBufferRawData[I].vRaw.Resize( LSE_ROUND_UP( vSizes[I], 16 ) ) ) { return false; }
				for ( uint32_t J = m_vConstBufferRawData[I].vRaw.Length() / sizeof( float ); J--; ) {
					reinterpret_cast<float *>(&m_vConstBufferRawData[I].vRaw[0])[J] = 62.0f;
				}
			}
		}
		
		// Add the data sizes.
		for ( uint32_t I = m_ulUniforms.Length(); I--; ) {
			LSG_CONSTANT_MAP cmThis;
			cmThis.ui32CBufferIndex = static_cast<uint32_t>(m_ulUniforms[I].uiptrApiParam);
			cmThis.ui32DataPtr = static_cast<uint32_t>(m_ulUniforms[I].uiptrApiSpecific);
			cmThis.ui32Name = I;
			if ( !m_vConstantMap.Push( cmThis ) ) { return false; }
		}

		// Create the cbuffers.
		if ( !m_vConstantBuffers.Resize( m_vConstBufferRawData.Length() ) ) { return false; }
		for ( uint32_t I = m_vConstantBuffers.Length(); I--; ) {
			m_vConstantBuffers[I] = NULL;
		}
		D3D11_BUFFER_DESC bdDesc;
		bdDesc.Usage = D3D11_USAGE_DYNAMIC;
		bdDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bdDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bdDesc.MiscFlags = 0;
		for ( uint32_t I = m_vConstantBuffers.Length(); I--; ) {
			if ( !m_vConstBufferRawData[I].vRaw.Length() ) { continue; }
			bdDesc.ByteWidth = m_vConstBufferRawData[I].vRaw.Length();
			HRESULT hRes = CDirectX11::GetDirectX11Device()->CreateBuffer( &bdDesc, NULL, &m_vConstantBuffers[I] );
			if ( FAILED( hRes ) ) { return false; }
		}


		return m_ulUniforms.Length() == _ulUniforms.Length();
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CShaderBase::LSG_HANDLE LSE_CALL CDirectX11CompiledShader::GetConstantByName( const char * _pcName ) {
		if ( !m_vConstantBuffers.Length() ) { return LSE_MAXU32; }


		for ( uint32_t I = 0; I < m_vConstantMap.Length(); ++I ) {
			if ( CStd::StrCmp( m_ulUniforms[m_vConstantMap[I].ui32Name].sName.CStr(), _pcName ) == 0 ) {
				return I;
			}
		}
		return LSE_MAXU32;
	}

	/**
	 * Sets a single scalar value in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _fValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<float *>(&LSG_HANDLE2CBUF( _hHandle ))) != _fValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<float *>(&LSG_HANDLE2CBUF( _hHandle ))) = _fValue;
		}
		return true;
	}

	/**
	 * Sets an array of scalar values in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pfValues New values.
	 * \param _ui32Total Total floats to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		
		// Check for redundancy.
		LSBOOL bMatch = true;
		float * pfDest = reinterpret_cast<float *>(&LSG_HANDLE2CBUF( _hHandle ));
		for ( uint32_t I = _ui32Total; I--; ) {
			if ( pfDest[I<<2] != _pfValues[I] ) {
				pfDest[I<<2] = _pfValues[I];
				bMatch = false;
			}
		}
		if ( !bMatch ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
		}
		return true;
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetInt( CShaderBase::LSG_HANDLE _hHandle, int32_t _iValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( m_vConstantMap[_hHandle].ui32DataPtr == LSE_MAXU32 ) { return true; }	// Samplers.
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<int32_t *>(&LSG_HANDLE2CBUF( _hHandle ))) != _iValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<int32_t *>(&LSG_HANDLE2CBUF( _hHandle ))) = _iValue;
		}
		return true;
	}

	/**
	 * Sets an array of scalar values in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		
		// Check for redundancy.
		LSBOOL bMatch = true;
		int32_t * pui32Dest = reinterpret_cast<int32_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		for ( uint32_t I = _ui32Total; I--; ) {
			if ( pui32Dest[I<<2] != _pi32Values[I] ) {
				pui32Dest[I<<2] = _pi32Values[I];
				bMatch = false;
			}
		}
		if ( !bMatch ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
		}
		return true;
	}

	/**
	 * Sets a single boolean value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _bValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetBool( CShaderBase::LSG_HANDLE _hHandle, LSBOOL _bValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<LSBOOL *>(&LSG_HANDLE2CBUF( _hHandle ))) != _bValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<LSBOOL *>(&LSG_HANDLE2CBUF( _hHandle ))) = _bValue;
		}
		return true;
	}

	/**
	 * Sets an array of boolean values in the shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		assert( sizeof( BOOL ) == sizeof( LSBOOL ) );
		
		// Check for redundancy.
		LSBOOL bMatch = true;
		LSBOOL * pbDest = reinterpret_cast<LSBOOL *>(&LSG_HANDLE2CBUF( _hHandle ));
		for ( uint32_t I = _ui32Total; I--; ) {
			if ( pbDest[I<<2] != _pbValues[I] ) {
				pbDest[I<<2] = _pbValues[I];
				bMatch = false;
			}
		}
		if ( !bMatch ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
		}
		return true;
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<CVector2 *>(&LSG_HANDLE2CBUF( _hHandle ))) != _vValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<CVector2 *>(&LSG_HANDLE2CBUF( _hHandle ))) = _vValue;
		}

#else
		CDirectX11::LSG_VECTOR4 vVec = {
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			0.0f,
			0.0f
		};
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( FLOAT ) * 2 ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( FLOAT ) * 2 );
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return true;
	}

	/**
	 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

		// Whether or not CVector2 has elements equal to FLOAT (used by CDirectX11::LSG_VECTOR4), we still need to convert the whole
		//	array to an array of CDirectX11::LSG_VECTOR4 types.  So there is no need to branch based off the value of the LSM_BASETYPE macro.
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( CDirectX11::LSG_VECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		CDirectX11::LSG_VECTOR4 * pvBeginning = reinterpret_cast<CDirectX11::LSG_VECTOR4 *>(&m_vTempBuffer[0]);
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		// Copy and check at the same time.
		LSBOOL bDifferent = LSG_HANDLE2DIRTY( _hHandle );
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);
			(*pvBeginning).x = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).y = static_cast<FLOAT>(*pfSource++);
			if ( !bDifferent ) {
				bDifferent = (*pui64Start) != (*pui64TestMe++);
			}
			(*pvBeginning).z = (*pvBeginning).w = 0.0f;
			pvBeginning++;
		}
		if ( !LSG_HANDLE2DIRTY( _hHandle ) && !bDifferent ) { return true; }

		LSG_HANDLE2DIRTY( _hHandle ) = true;
		pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		pvBeginning = reinterpret_cast<CDirectX11::LSG_VECTOR4 *>(&m_vTempBuffer[0]);
		uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);
		while ( _ui32Total-- ) {
			(*pui64TestMe++) = (*pui64Start++);																		// Copies X and Y.
			(*pui64TestMe++) = (*pui64Start++);																		// Copies Z and W.
		}
		return true;
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.  This is just barely faster than the other version which calls CStd::MemCmpF(), and since this
		//	is the most common case, it is worth our time to make a special case to handle this.
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<CVector3 *>(&LSG_HANDLE2CBUF( _hHandle ))) == _vValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<CVector3 *>(&LSG_HANDLE2CBUF( _hHandle ))) = _vValue;
		}
#else
		CDirectX11::LSG_VECTOR4 vVec = {
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			static_cast<FLOAT>(_vValue.z),
			0.0f
		};
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( FLOAT ) * 3 ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( FLOAT ) * 3 );
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return true;
	}

	/**
	 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

		// Whether or not CVector3 has elements equal to FLOAT (used by CDirectX11::LSG_VECTOR4), we still need to convert the whole
		//	array to an array of CDirectX11::LSG_VECTOR4 types.  So there is no need to branch based off the value of the LSM_BASETYPE macro.
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( CDirectX11::LSG_VECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		CDirectX11::LSG_VECTOR4 * pvBeginning = reinterpret_cast<CDirectX11::LSG_VECTOR4 *>(&m_vTempBuffer[0]);
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));;

		// Copy and check at the same time.
		LSBOOL bDifferent = LSG_HANDLE2DIRTY( _hHandle );
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);

			(*pvBeginning).x = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).y = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).z = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).w = 0.0f;
			if ( !bDifferent ) {
				bDifferent = (*pui64Start) != (*pui64TestMe) ||														// Compares the X and Y together.
					reinterpret_cast<uint32_t *>(pui64TestMe)[2] != reinterpret_cast<uint32_t *>(pui64Start)[2];	// Checks the Z.	

				pui64TestMe += 2;
			}
			pvBeginning++;
		}
		if ( !LSG_HANDLE2DIRTY( _hHandle ) && !bDifferent ) { return true; }

		LSG_HANDLE2DIRTY( _hHandle ) = true;
		pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		pvBeginning = reinterpret_cast<CDirectX11::LSG_VECTOR4 *>(&m_vTempBuffer[0]);
		uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);
		while ( _ui32Total-- ) {
			(*pui64TestMe++) = (*pui64Start++);																		// Copies X and Y.
			(*pui64TestMe++) = (*pui64Start++);																		// Copies Z and W.
		}
		return true;
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		if ( LSG_HANDLE2DIRTY( _hHandle ) || (*reinterpret_cast<CVector4 *>(&LSG_HANDLE2CBUF( _hHandle ))) != _vValue ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<CVector4 *>(&LSG_HANDLE2CBUF( _hHandle ))) = _vValue;
		}
#else
		CDirectX11::LSG_VECTOR4 vVec = {
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			static_cast<FLOAT>(_vValue.z),
			static_cast<FLOAT>(_vValue.w)
		};
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( CDirectX11::LSG_VECTOR4 ) ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), &vVec, sizeof( CDirectX11::LSG_VECTOR4 ) );
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return true;
	}

	/**
	 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), _pvValues, sizeof( CVector4 ) * _ui32Total ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), _pvValues, sizeof( CVector4 ) * _ui32Total );
		}
#else
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( CDirectX11::LSG_VECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		CDirectX11::LSG_VECTOR4 * pvBeginning = reinterpret_cast<CDirectX11::LSG_VECTOR4 *>(&m_vTempBuffer[0]);
		CDirectX11::LSG_VECTOR4 * pvUseMe = pvBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));

		// Copy and check at the same time.
		LSBOOL bDifferent = LSG_HANDLE2DIRTY( _hHandle );
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);

			(*pvBeginning).x = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).y = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).z = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).w = static_cast<FLOAT>(*pfSource++);
			if ( !bDifferent ) {
				bDifferent = (*pui64Start++) != (*pui64TestMe++);	// X and Y.
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);	// Z and W.

			}
			pvBeginning++;
		}
		if ( !LSG_HANDLE2DIRTY( _hHandle ) && !bDifferent ) { return true; }

		LSG_HANDLE2DIRTY( _hHandle ) = true;
		pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvUseMe);
		while ( _ui32Total-- ) {
			(*pui64TestMe++) = (*pui64Start++);		// Copies X and Y.
			(*pui64TestMe++) = (*pui64Start++);		// Copies Z and W.
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return true;
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		// There is no way to set a 2-by-2 matrix in Direct3D so either way we have to construct a new matrix from the given data, regardless
		//	of whether or not the component types match.
		CDirectX11::LSG_MATRIX mThis;
		mThis._11 = static_cast<FLOAT>(_mValue._11);
		mThis._12 = static_cast<FLOAT>(_mValue._12);

		mThis._21 = static_cast<FLOAT>(_mValue._21);
		mThis._22 = static_cast<FLOAT>(_mValue._22);

		if ( LSG_HANDLE2DIRTY( _hHandle ) ||
			// _11 and _12.
			(reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0] != (*reinterpret_cast<uint64_t *>(&mThis._11)) ||
			// _21 and _22.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[3] != (*reinterpret_cast<uint64_t *>(&mThis._21))) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0] = (*reinterpret_cast<uint64_t *>(&mThis._11));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[3] = (*reinterpret_cast<uint64_t *>(&mThis._21));
		}
		return true;
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		// There is no way to set a 3-by-3 matrix in Direct3D so either way we have to construct a new matrix from the given data, regardless
		//	of whether or not the component types match.
		CDirectX11::LSG_MATRIX mThis;
		mThis._11 = static_cast<FLOAT>(_mValue._11);
		mThis._12 = static_cast<FLOAT>(_mValue._12);
		mThis._13 = static_cast<FLOAT>(_mValue._13);
		mThis._14 = 0.0f;

		mThis._21 = static_cast<FLOAT>(_mValue._21);
		mThis._22 = static_cast<FLOAT>(_mValue._22);
		mThis._23 = static_cast<FLOAT>(_mValue._23);
		mThis._24 = 0.0f;

		mThis._31 = static_cast<FLOAT>(_mValue._31);
		mThis._32 = static_cast<FLOAT>(_mValue._32);
		mThis._33 = static_cast<FLOAT>(_mValue._33);
		mThis._34 = 0.0f;

		if ( LSG_HANDLE2DIRTY( _hHandle ) ||
			// _11 and _12.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0])) != (*reinterpret_cast<uint64_t *>(&mThis._11)) ||
			// _21 and _22.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[4])) != (*reinterpret_cast<uint64_t *>(&mThis._21)) ||
			// _31 and _32.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[8])) != (*reinterpret_cast<uint64_t *>(&mThis._31)) ||
			// _13.
			reinterpret_cast<FLOAT *>(&LSG_HANDLE2CBUF( _hHandle ))[2] != mThis._13 ||
			// _23.
			reinterpret_cast<FLOAT *>(&LSG_HANDLE2CBUF( _hHandle ))[6] != mThis._23 ||
			// _33.
			reinterpret_cast<FLOAT *>(&LSG_HANDLE2CBUF( _hHandle ))[10] != mThis._33 ) {

			LSG_HANDLE2DIRTY( _hHandle ) = true;
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0])) = (*reinterpret_cast<uint64_t *>(&mThis._11));
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[2])) = (*reinterpret_cast<uint64_t *>(&mThis._13));

			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[4])) = (*reinterpret_cast<uint64_t *>(&mThis._21));
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[6])) = (*reinterpret_cast<uint64_t *>(&mThis._23));

			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[8])) = (*reinterpret_cast<uint64_t *>(&mThis._31));
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&LSG_HANDLE2CBUF( _hHandle ))[10])) = (*reinterpret_cast<uint64_t *>(&mThis._33));
		}
		return true;
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		assert( sizeof( CMatrix4x4 ) == sizeof( CDirectX11::LSG_MATRIX ) );
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), &_mValue, sizeof( CMatrix4x4 ) ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), &_mValue, sizeof( CMatrix4x4 ) );
		}
#else

		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		CDirectX11::LSG_MATRIX mThis;
		mThis._11 = static_cast<FLOAT>(_mValue._11);
		mThis._12 = static_cast<FLOAT>(_mValue._12);
		mThis._13 = static_cast<FLOAT>(_mValue._13);
		mThis._14 = static_cast<FLOAT>(_mValue._14);

		mThis._21 = static_cast<FLOAT>(_mValue._21);
		mThis._22 = static_cast<FLOAT>(_mValue._22);
		mThis._23 = static_cast<FLOAT>(_mValue._23);
		mThis._24 = static_cast<FLOAT>(_mValue._24);

		mThis._31 = static_cast<FLOAT>(_mValue._31);
		mThis._32 = static_cast<FLOAT>(_mValue._32);
		mThis._33 = static_cast<FLOAT>(_mValue._33);
		mThis._34 = static_cast<FLOAT>(_mValue._34);

		mThis._41 = static_cast<FLOAT>(_mValue._41);
		mThis._42 = static_cast<FLOAT>(_mValue._42);
		mThis._43 = static_cast<FLOAT>(_mValue._43);
		mThis._44 = static_cast<FLOAT>(_mValue._44);

		if ( LSG_HANDLE2DIRTY( _hHandle ) ||
			// _11 and _12.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0] != (*reinterpret_cast<uint64_t *>(&mThis._11)) ||
			// _13 and _14.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[1] != (*reinterpret_cast<uint64_t *>(&mThis._13)) ||
			// _21 and _22.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[2] != (*reinterpret_cast<uint64_t *>(&mThis._21)) ||
			// _23 and _24.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[3] != (*reinterpret_cast<uint64_t *>(&mThis._23)) ||
			// _31 and _32.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[4] != (*reinterpret_cast<uint64_t *>(&mThis._31)) ||
			// _33 and _34.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[5] != (*reinterpret_cast<uint64_t *>(&mThis._33)) ||
			// _41 and _42.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[6] != (*reinterpret_cast<uint64_t *>(&mThis._41)) ||
			// _43 and _44.
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[7] != (*reinterpret_cast<uint64_t *>(&mThis._43)) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[0] = (*reinterpret_cast<uint64_t *>(&mThis._11));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[1] = (*reinterpret_cast<uint64_t *>(&mThis._13));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[2] = (*reinterpret_cast<uint64_t *>(&mThis._21));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[3] = (*reinterpret_cast<uint64_t *>(&mThis._23));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[4] = (*reinterpret_cast<uint64_t *>(&mThis._31));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[5] = (*reinterpret_cast<uint64_t *>(&mThis._33));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[6] = (*reinterpret_cast<uint64_t *>(&mThis._41));
			reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ))[7] = (*reinterpret_cast<uint64_t *>(&mThis._43));
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return true;
	}

	/**
	 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pmValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX11CompiledShader::SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		assert( sizeof( CMatrix4x4 ) == sizeof( CDirectX11::LSG_MATRIX ) );
		// Check for redundancy.
		if ( LSG_HANDLE2DIRTY( _hHandle ) || !CStd::MemCmpF( &LSG_HANDLE2CBUF( _hHandle ), _pmValues, sizeof( CMatrix4x4 ) * _ui32Total ) ) {
			LSG_HANDLE2DIRTY( _hHandle ) = true;
			CStd::MemCpy( &LSG_HANDLE2CBUF( _hHandle ), _pmValues, sizeof( CMatrix4x4 ) * _ui32Total );
		}
		return true;
#else
		uint32_t ui32NewSize = sizeof( CMatrix4x4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}

		CDirectX11::LSG_MATRIX * pmBeginning = reinterpret_cast<CDirectX11::LSG_MATRIX *>(&m_vTempBuffer[0]);
		CDirectX11::LSG_MATRIX * pmUseMe = pmBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pmValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));

		// Copy and check at the same time.
		LSBOOL bDifferent = LSG_HANDLE2DIRTY( _hHandle );
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pmBeginning);

			// Row 0.
			(*pmBeginning)._11 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._12 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._13 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._14 = static_cast<FLOAT>(*pfSource++);

			// Row 1.
			(*pmBeginning)._21 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._22 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._23 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._24 = static_cast<FLOAT>(*pfSource++);

			// Row 2.
			(*pmBeginning)._31 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._32 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._33 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._34 = static_cast<FLOAT>(*pfSource++);

			// Row 3.
			(*pmBeginning)._41 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._42 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._43 = static_cast<FLOAT>(*pfSource++);
			(*pmBeginning)._44 = static_cast<FLOAT>(*pfSource++);
			if ( !bDifferent ) {
				// Row 0.
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);

				// Row 1.
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);

				// Row 2.
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);

				// Row 3.
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);
				bDifferent |= (*pui64Start++) != (*pui64TestMe++);

			}
			pmBeginning++;
		}
		if ( !LSG_HANDLE2DIRTY( _hHandle ) && !bDifferent ) { return true; }

		LSG_HANDLE2DIRTY( _hHandle ) = true;
		pui64TestMe = reinterpret_cast<uint64_t *>(&LSG_HANDLE2CBUF( _hHandle ));
		uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pmUseMe);
		while ( _ui32Total-- ) {
			// Row 0.
			(*pui64TestMe++) = (*pui64Start++);
			(*pui64TestMe++) = (*pui64Start++);

			// Row 1.
			(*pui64TestMe++) = (*pui64Start++);
			(*pui64TestMe++) = (*pui64Start++);

			// Row 2.
			(*pui64TestMe++) = (*pui64Start++);
			(*pui64TestMe++) = (*pui64Start++);

			// Row 3.
			(*pui64TestMe++) = (*pui64Start++);
			(*pui64TestMe++) = (*pui64Start++);
		}

		return true;
#endif	// #if LSM_BASETYPE == LSM_FLOAT
	}

}	// namespace lsg

#undef LSG_HANDLE2DIRTY
#undef LSG_HANDLE2CBUF

#endif	// #ifdef LSG_DX11
