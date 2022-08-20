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

#include "LSGDirectX9CompiledShader.h"

#ifdef LSG_DX9

namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CDirectX9CompiledShader::CDirectX9CompiledShader() :
		m_pctConstantTable( NULL ),
		m_pbShader( NULL ) {
	}
	LSE_CALLCTOR CDirectX9CompiledShader::~CDirectX9CompiledShader() {
		Reset();
	}


	// == Functions.
	/**
	 * Reset everything to scratch.
	 */
	void LSE_CALL CDirectX9CompiledShader::Reset() {
		SafeRelease( m_pctConstantTable );
		SafeRelease( m_pbShader );
		Parent::Reset();
	}

	/**
	 * Sets the uniforms associated with this shader.
	 *
	 * \param _ulUniforms The uniforms to copy to this shader.
	 * \return Returns true if there is enough memory to copy the list of uniforms.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetUniforms( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST &_ulUniforms ) {
		m_ulUniforms = _ulUniforms;
		// Add the data sizes.
		uint32_t ui32DataSize = 0;
		for ( uint32_t I = m_ulUniforms.Length(); I--; ) {
			ui32DataSize += m_ulUniforms[I].ui32Size;
		}
		m_ui32NextUniformStart = 0;
		if ( !m_vUniformBuffer.Resize( ui32DataSize ) ) {
			CStd::DebugPrintA( "Failed to create local uniform buffer: Not enough memory.\r\n" );
			return false;
		}

		return m_ulUniforms.Length() == _ulUniforms.Length();
	}

	/**
	 * Gets a handle for a uniform variable in the shader.
	 *
	 * \param _pcName Name of the variable whose handle is to be obtained.
	 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
	 */
	CShaderBase::LSG_HANDLE LSE_CALL CDirectX9CompiledShader::GetConstantByName( const char * _pcName ) {
		if ( !m_pctConstantTable ) { return LSE_MAXU32; }


		// Have we already given this handle out?
		for ( uint32_t I = 0; I < m_vConstantMap.Length(); ++I ) {
			if ( CStd::StrCmp( m_ulUniforms[m_vConstantMap[I].ui32Name].sName.CStr(), _pcName ) == 0 ) {
				return I;
			}
		}

		// Not found.  Create a new handle.
		for ( uint32_t J = m_ulUniforms.Length(); J--; ) {
			const char * pcThisName = m_ulUniforms[J].sName.CStr();
			if ( CStd::StrCmp( pcThisName, _pcName ) == 0 ) {
				// Make a new handle.
				uint32_t ui32Res = m_vConstantMap.Length();

				// Check that the uniform buffer is large enough (should always be the case).
				uint32_t ui32DataRemaining = m_vUniformBuffer.Length() - m_ui32NextUniformStart;
				if ( m_ulUniforms[J].ui32Size > ui32DataRemaining ) {
					// An impossible case, but luckily we can resize the buffer.
					if ( !m_vUniformBuffer.Resize( m_ui32NextUniformStart + m_ulUniforms[J].ui32Size ) ) {
						return LSE_MAXU32;
					}
				}

				// Prepare an entry.
				LSG_CONSTANT_MAP cmEntry;
				cmEntry.bDirty = true;
				cmEntry.ui32DataPtr = m_ui32NextUniformStart;
				cmEntry.ui32Name = J;
				cmEntry.hDirectXHandle = m_pctConstantTable->GetConstantByName( NULL, _pcName );
				if ( cmEntry.hDirectXHandle == NULL ) {
					return LSE_MAXU32;
				}
				if ( !m_vConstantMap.Push( cmEntry ) ) {
					return LSE_MAXU32;
				}
				m_ui32NextUniformStart += m_ulUniforms[J].ui32Size;
				// The index of that entry is the handle we give back.
				return ui32Res;
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
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<float *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _fValue ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetFloat( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _fValue ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			(*reinterpret_cast<float *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _fValue;
			return true;
		}
		return false;
	}

	/**
	 * Sets an array of scalar values in the shader of type float.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pfValues New values.
	 * \param _ui32Total Total floats to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pfValues, sizeof( float ) * _ui32Total ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetFloatArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _pfValues, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pfValues, sizeof( float ) * _ui32Total );
			return true;
		}
		return false;
	}

	/**
	 * Sets a single scalar value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _iValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetInt( CShaderBase::LSG_HANDLE _hHandle, int32_t _iValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<int32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _iValue ) { return true; }
		HRESULT hRes = m_pctConstantTable->SetInt( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _iValue );
		if ( SUCCEEDED( hRes ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			(*reinterpret_cast<int32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _iValue;
			return true;
		}
		return false;
	}

	/**
	 * Sets an array of scalar values in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( int32_t ) * _ui32Total ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetIntArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _pi32Values, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pi32Values, sizeof( int32_t ) * _ui32Total );
			return true;
		}
		return false;
	}

	/**
	 * Sets a single boolean value in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _bValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetBool( CShaderBase::LSG_HANDLE _hHandle, LSBOOL _bValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<LSBOOL *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _bValue ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetBool( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _bValue ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			(*reinterpret_cast<LSBOOL *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _bValue;
			return true;
		}
		return false;
	}

	/**
	 * Sets an array of boolean values in the shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pi32Values New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }
		assert( sizeof( BOOL ) == sizeof( LSBOOL ) );
		
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pbValues, sizeof( int32_t ) * _ui32Total ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetBoolArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, _pbValues, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pbValues, sizeof( int32_t ) * _ui32Total );
			return true;
		}
		return false;
	}

	/**
	 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		D3DXVECTOR4 vVec(
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			0.0f,
			0.0f
		);
#else
		D3DXVECTOR4 vVec(
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			0.0f,
			0.0f
		);
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( FLOAT ) * 2 ) ) { return true; }
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		if ( SUCCEEDED( m_pctConstantTable->SetVector( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &vVec ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
#if LSM_BASETYPE == LSM_FLOAT
			(*reinterpret_cast<CVector2 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _vValue;
#else
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( FLOAT ) * 2 );
#endif	// #if LSM_BASETYPE == LSM_FLOAT
			return true;
		}
		return false;
	}

	/**
	 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

		// Whether or not CVector2 has elements equal to FLOAT (used by D3DXVECTOR4), we still need to convert the whole
		//	array to an array of D3DXVECTOR4 types.  So there is no need to branch based off the value of the LSM_BASETYPE macro.
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( D3DXVECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		D3DXVECTOR4 * pvBeginning = reinterpret_cast<D3DXVECTOR4 *>(&m_vTempBuffer[0]);
		D3DXVECTOR4 * pvUseMe = pvBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);
		// Copy and check at the same time.
		LSBOOL bDifferent = m_vConstantMap[_hHandle].bDirty;
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
		if ( !bDifferent ) { return true; }

		if ( SUCCEEDED( m_pctConstantTable->SetVectorArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, pvUseMe, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);
			pvBeginning = reinterpret_cast<D3DXVECTOR4 *>(&m_vTempBuffer[0]);
			while ( _ui32Total-- ) {
				uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);
				(*pui64TestMe++) = (*pui64Start);
				++pvBeginning;
			}
			return true;
		}
		return false;
	}

	/**
	 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }

#if LSM_BASETYPE == LSM_FLOAT
		// Can work directly on the vector.  This is just barely faster than the other version which calls CStd::MemCmpF(), and since this
		//	is the most common case, it is worth our time to make a special case to handle this.
		// Check for redundancy.
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		D3DXVECTOR4 vVec(
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			static_cast<FLOAT>(_vValue.z),
			0.0f
		);
#else
		D3DXVECTOR4 vVec(
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			static_cast<FLOAT>(_vValue.z),
			0.0f
		);
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( FLOAT ) * 3 ) ) { return true; }
#endif	// #if LSM_BASETYPE == LSM_FLOAT

		if ( SUCCEEDED( m_pctConstantTable->SetVector( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &vVec ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
#if LSM_BASETYPE == LSM_FLOAT
			(*reinterpret_cast<CVector3 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _vValue;
#else
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( FLOAT ) * 3 );
#endif	// #if LSM_BASETYPE == LSM_FLOAT
			return true;
		}
		return false;
	}

	/**
	 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

		// Whether or not CVector3 has elements equal to FLOAT (used by D3DXVECTOR4), we still need to convert the whole
		//	array to an array of D3DXVECTOR4 types.  So there is no need to branch based off the value of the LSM_BASETYPE macro.
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( D3DXVECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		D3DXVECTOR4 * pvBeginning = reinterpret_cast<D3DXVECTOR4 *>(&m_vTempBuffer[0]);
		D3DXVECTOR4 * pvUseMe = pvBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);;

		// Copy and check at the same time.
		LSBOOL bDifferent = m_vConstantMap[_hHandle].bDirty;
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);

			(*pvBeginning).x = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).y = static_cast<FLOAT>(*pfSource++);
			(*pvBeginning).z = static_cast<FLOAT>(*pfSource++);
			if ( !bDifferent ) {
				bDifferent = (*pui64Start) != (*pui64TestMe) ||														// Compares the X and Y together.
					reinterpret_cast<uint32_t *>(pui64TestMe)[2] != reinterpret_cast<uint32_t *>(pui64Start)[2];	// Checks the Z.	

				pui64TestMe = reinterpret_cast<uint64_t *>(reinterpret_cast<uint8_t *>(pui64TestMe) + (sizeof( FLOAT ) * 3));
			}
			(*pvBeginning).w = 0.0f;
			pvBeginning++;
		}
		if ( !bDifferent ) { return true; }

		if ( SUCCEEDED( m_pctConstantTable->SetVectorArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, pvUseMe, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);
			pvBeginning = reinterpret_cast<D3DXVECTOR4 *>(&m_vTempBuffer[0]);
			while ( _ui32Total-- ) {
				uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvBeginning);
				(*pui64TestMe++) = (*pui64Start);																	// Copies X and Y.
				reinterpret_cast<uint32_t *>(pui64TestMe)[2] = reinterpret_cast<uint32_t *>(pui64Start)[2];			// Copies Z.
				++pvBeginning;
			}
			return true;
		}
		return false;
	}

	/**
	 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vConstantMap[_hHandle].bDirty && (*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) == _vValue ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetVector( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, reinterpret_cast<const D3DXVECTOR4 *>(&_vValue) ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			(*reinterpret_cast<CVector4 *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])) = _vValue;
			return true;
		}
#else
		D3DXVECTOR4 vVec(
			static_cast<FLOAT>(_vValue.x),
			static_cast<FLOAT>(_vValue.y),
			static_cast<FLOAT>(_vValue.z),
			static_cast<FLOAT>(_vValue.w)
		);
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( D3DXVECTOR4 ) ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetVector( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &vVec ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &vVec, sizeof( D3DXVECTOR4 ) );
			return true;
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return false;
	}

	/**
	 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _vValue New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		if ( !_ui32Total ) { return true; }

#if LSM_BASETYPE == LSM_FLOAT
		if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetVectorArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, reinterpret_cast<const D3DXVECTOR4 *>(_pvValues), _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pvValues, sizeof( CVector4 ) * _ui32Total );
			return true;
		}
#else
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );	// We use uint64_t to compare two floats at a time.
		uint32_t ui32NewSize = sizeof( D3DXVECTOR4 ) * _ui32Total;
		if ( m_vTempBuffer.Length() < ui32NewSize ) {
			m_vTempBuffer.Resize( ui32NewSize );
		}
		D3DXVECTOR4 * pvBeginning = reinterpret_cast<D3DXVECTOR4 *>(&m_vTempBuffer[0]);
		D3DXVECTOR4 * pvUseMe = pvBeginning;
		const LSREAL * pfSource = reinterpret_cast<const LSREAL *>(_pvValues);
		uint64_t * pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);;

		// Copy and check at the same time.
		LSBOOL bDifferent = m_vConstantMap[_hHandle].bDirty;
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
		if ( !bDifferent ) { return true; }

		if ( SUCCEEDED( m_pctConstantTable->SetVectorArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, pvUseMe, _ui32Total ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			pui64TestMe = reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr]);
			uint64_t * pui64Start = reinterpret_cast<uint64_t *>(pvUseMe);
			while ( _ui32Total-- ) {
				(*pui64TestMe++) = (*pui64Start++);		// Copies X and Y.
				(*pui64TestMe++) = (*pui64Start++);		// Copies Z and W.
			}
			return true;
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return false;
	}

	/**
	 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		// There is no way to set a 2-by-2 matrix in Direct3D so either way we have to construct a new matrix from the given data, regardless
		//	of whether or not the component types match.
		D3DXMATRIX mThis;
		mThis._11 = static_cast<FLOAT>(_mValue._11);
		mThis._12 = static_cast<FLOAT>(_mValue._21);
		mThis._13 = 0.0f;
		mThis._14 = 0.0f;

		mThis._21 = static_cast<FLOAT>(_mValue._12);
		mThis._22 = static_cast<FLOAT>(_mValue._22);
		mThis._23 = 0.0f;
		mThis._24 = 0.0f;

		mThis._31 = 0.0f;
		mThis._32 = 0.0f;
		mThis._33 = 0.0f;
		mThis._34 = 0.0f;

		mThis._41 = 0.0f;
		mThis._42 = 0.0f;
		mThis._43 = 0.0f;
		mThis._44 = 1.0f;

		if ( !m_vConstantMap[_hHandle].bDirty &&
			// _11 and _12.
			(reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0] == (*reinterpret_cast<uint64_t *>(&mThis._11)) &&
			// _21 and _22.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[1] == (*reinterpret_cast<uint64_t *>(&mThis._21))) ) {
			return true;
		}

		if ( SUCCEEDED( m_pctConstantTable->SetMatrix( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &mThis ) ) ) {
			m_vConstantMap[_hHandle].bDirty = false;
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0] = (*reinterpret_cast<uint64_t *>(&mThis._11));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[1] = (*reinterpret_cast<uint64_t *>(&mThis._21));
			return true;
		}
		return false;
	}

	/**
	 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		// There is no way to set a 3-by-3 matrix in Direct3D so either way we have to construct a new matrix from the given data, regardless
		//	of whether or not the component types match.
		D3DXMATRIX mThis;
		mThis._11 = static_cast<FLOAT>(_mValue._11);
		mThis._12 = static_cast<FLOAT>(_mValue._21);
		mThis._13 = static_cast<FLOAT>(_mValue._31);
		mThis._14 = 0.0f;

		mThis._21 = static_cast<FLOAT>(_mValue._12);
		mThis._22 = static_cast<FLOAT>(_mValue._22);
		mThis._23 = static_cast<FLOAT>(_mValue._32);
		mThis._24 = 0.0f;

		mThis._31 = static_cast<FLOAT>(_mValue._13);
		mThis._32 = static_cast<FLOAT>(_mValue._23);
		mThis._33 = static_cast<FLOAT>(_mValue._33);
		mThis._34 = 0.0f;

		mThis._41 = 0.0f;
		mThis._42 = 0.0f;
		mThis._43 = 0.0f;
		mThis._44 = 1.0f;
/*
		if ( !m_vConstantMap[_hHandle].bDirty &&
			// _11 and _12.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0])) == (*reinterpret_cast<uint64_t *>(&mThis._11)) &&
			// _21 and _22.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[3])) == (*reinterpret_cast<uint64_t *>(&mThis._21)) &&
			// _31 and _32.
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[6])) == (*reinterpret_cast<uint64_t *>(&mThis._31)) &&
			// _13.
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[2] == mThis._13 &&
			// _23.
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[5] == mThis._23 &&
			// _33.
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[8] == mThis._33 ) {
			return true;
		}
*/
		if ( SUCCEEDED( m_pctConstantTable->SetMatrix( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &mThis ) ) ) {
			/*m_vConstantMap[_hHandle].bDirty = false;
			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0])) = (*reinterpret_cast<uint64_t *>(&mThis._11));
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[2] = mThis._13;

			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[3])) = (*reinterpret_cast<uint64_t *>(&mThis._21));
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[5] = mThis._23;

			(*reinterpret_cast<uint64_t *>(&reinterpret_cast<uint32_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[6])) = (*reinterpret_cast<uint64_t *>(&mThis._31));
			reinterpret_cast<FLOAT *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[8] = mThis._33;*/
			return true;
		}
		return false;
	}

	/**
	 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _mValue New value.
	 * \return Returns true if the value was set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		assert( sizeof( CMatrix4x4 ) == sizeof( D3DXMATRIX ) );
		// Check for redundancy.
		//if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetMatrixTranspose( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, reinterpret_cast<const D3DXMATRIX *>(&_mValue) ) ) ) {
			//m_vConstantMap[_hHandle].bDirty = false;
			//CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], &_mValue, sizeof( CMatrix4x4 ) );
			return true;
		}
#else

		assert( sizeof( FLOAT ) * 2 == sizeof( uint64_t ) );
		D3DXMATRIX mThis;
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

		/*if ( !m_vConstantMap[_hHandle].bDirty &&
			// _11 and _12.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0] == (*reinterpret_cast<uint64_t *>(&mThis._11)) &&
			// _13 and _14.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[1] == (*reinterpret_cast<uint64_t *>(&mThis._13)) &&
			// _21 and _22.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[2] == (*reinterpret_cast<uint64_t *>(&mThis._21)) &&
			// _23 and _24.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[3] == (*reinterpret_cast<uint64_t *>(&mThis._23)) &&
			// _31 and _32.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[4] == (*reinterpret_cast<uint64_t *>(&mThis._31)) &&
			// _33 and _34.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[5] == (*reinterpret_cast<uint64_t *>(&mThis._33)) &&
			// _41 and _42.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[6] == (*reinterpret_cast<uint64_t *>(&mThis._41)) &&
			// _43 and _44.
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[7] == (*reinterpret_cast<uint64_t *>(&mThis._43)) ) {
			return true;
		}*/
		if ( SUCCEEDED( m_pctConstantTable->SetMatrixTranspose( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, &mThis ) ) ) {
			/*m_vConstantMap[_hHandle].bDirty = false;
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[0] = (*reinterpret_cast<uint64_t *>(&mThis._11));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[1] = (*reinterpret_cast<uint64_t *>(&mThis._13));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[2] = (*reinterpret_cast<uint64_t *>(&mThis._21));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[3] = (*reinterpret_cast<uint64_t *>(&mThis._23));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[4] = (*reinterpret_cast<uint64_t *>(&mThis._31));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[5] = (*reinterpret_cast<uint64_t *>(&mThis._33));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[6] = (*reinterpret_cast<uint64_t *>(&mThis._41));
			reinterpret_cast<uint64_t *>(&m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr])[7] = (*reinterpret_cast<uint64_t *>(&mThis._43));*/
			return true;
		}
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return false;
	}

	/**
	 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
	 *
	 * \param _hHandle Handle to the value to set in the shader.
	 * \param _pmValues New values.
	 * \param _ui32Total Total ints to set.
	 * \return Returns true if the values were set in the shader.
	 */
	LSBOOL LSE_CALL CDirectX9CompiledShader::SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) {
		if ( _hHandle >= m_vConstantMap.Length() ) { return false; }
#if LSM_BASETYPE == LSM_FLOAT
		assert( sizeof( CMatrix4x4 ) == sizeof( D3DXMATRIX ) );
		// Check for redundancy.
		//if ( !m_vConstantMap[_hHandle].bDirty && CStd::MemCmpF( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total ) ) { return true; }
		if ( SUCCEEDED( m_pctConstantTable->SetMatrixTransposeArray( CDirectX9::GetDirectX9Device(), m_vConstantMap[_hHandle].hDirectXHandle, reinterpret_cast<const D3DXMATRIX *>(_pmValues), _ui32Total ) ) ) {
			/*m_vConstantMap[_hHandle].bDirty = false;
			CStd::MemCpy( &m_vUniformBuffer[m_vConstantMap[_hHandle].ui32DataPtr], _pmValues, sizeof( CMatrix4x4 ) * _ui32Total );*/
			return true;
		}
#else
#error "Add me."
#endif	// #if LSM_BASETYPE == LSM_FLOAT
		return false;
	}

}	// namespace lsg

#endif	// #ifdef LSG_DX9
