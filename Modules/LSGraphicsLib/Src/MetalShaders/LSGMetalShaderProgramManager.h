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
 * Description: In Metal, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
 *	exactly alike.
 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
 */


#ifndef __LSG_METALSHADERPROGRAMMANAGER_H__
#define __LSG_METALSHADERPROGRAMMANAGER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_METAL )

#include "../ShaderLanguage/LSGShaderBase.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"
#include "LSGMetalCompiledShader.h"
#include "Map/LSTLLinearMap.h"

namespace lsg {

	/**
	 * Class CMetalShaderProgramManager
	 * \brief Allows sharing of shader programs in Metal.
	 *
	 * Description: In Metal, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
	 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
	 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
	 *	exactly alike.
	 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
	 */
	class CMetalShaderProgramManager {
	public :
		/**
		 * Class CMetalShaderProgram
		 * \brief A shader program in Metal.
		 *
		 * Description: A shader program wrapper for Metal.
		 */
		class CMetalShaderProgram {
		public :
			// == Various constructors.
			LSE_CALLCTOR												CMetalShaderProgram();
			LSE_CALLCTOR												~CMetalShaderProgram();


			// == Functions.
			/**
			 * Add a shader.
			 *
			 * \param _pogcsShader Pointer to the vertex shader to add to this program.
			 */
			LSVOID LSE_CALL												AddShader( CMetalCompiledShader * _pogcsShader );

			/**
			 * Gets a shader in a given slot.
			 *
			 * \param _stShaderType The type of shader to get from this object.
			 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
			 */
			CMetalCompiledShader * LSE_CALL								GetShader( LSG_SHADER_TYPES _stShaderType );

			/**
			 * Gets a shader in a given slot.
			 *
			 * \param _stShaderType The type of shader to get from this object.
			 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
			 */
			const CMetalCompiledShader * LSE_CALL						GetShader( LSG_SHADER_TYPES _stShaderType ) const;

			/**
			 * Finalize the shader for use with Metal.
			 *
			 * \param _pulUniforms Uniforms in the shader.
			 * \param _palAttributes Attributes in the shader.
			 * \param _sError Filled with the error report on failure.
			 * \return Returns true if there are no compilation/linkage errors.
			 */
			LSBOOL LSE_CALL												Compile( const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
				const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
				CString &_sError );

			/**
			 * Activate this shader.
			 */
			LSVOID LSE_CALL												Activate();

			/**
			 * Gets the reference count for this object.
			 *
			 * \return Returns the reference count for this object.
			 */
			LSUINT32 LSE_CALL											GetRef() const;

			/**
			 * Gets the unique ID of this object.  This matches the Metal shader program ID, which may be 0.
			 *
			 * \return Returns the unique ID for this object.
			 */
			LSUINT32 LSE_CALL											GetId() const;

			/**
			 * Gets a handle for a uniform variable in the shader.
			 *
			 * \param _pcName Name of the variable whose handle is to be obtained.
			 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
			 */
			CShaderBase::LSG_HANDLE LSE_CALL							GetConstantByName( const LSCHAR * _pcName );

			/**
			 * Sets a single scalar value in the shader of type float.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _fValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetFloat( CShaderBase::LSG_HANDLE _hHandle, LSFLOAT _fValue );

			/**
			 * Sets an array of scalar values in the shader of type float.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pfValues New values.
			 * \param _ui32Total Total floats to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const LSFLOAT * _pfValues, LSUINT32 _ui32Total );

			/**
			 * Sets a single scalar value in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _iValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetInt( CShaderBase::LSG_HANDLE _hHandle, LSINT32 _iValue );

			/**
			 * Sets an array of scalar values in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pi32Values New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const LSINT32 * _pi32Values, LSUINT32 _ui32Total );

			/**
			 * Sets an array of boolean values in the shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pi32Values New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, LSUINT32 _ui32Total );

			/**
			 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector2( CShaderBase::LSG_HANDLE _hHandle, const CVector2 &_vValue );

			/**
			 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, LSUINT32 _ui32Total );

			/**
			 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector3( CShaderBase::LSG_HANDLE _hHandle, const CVector3 &_vValue );

			/**
			 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, LSUINT32 _ui32Total );

			/**
			 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector4( CShaderBase::LSG_HANDLE _hHandle, const CVector4 &_vValue );

			/**
			 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _vValue New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, LSUINT32 _ui32Total );

			/**
			 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _mValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetMatrix2x2( CShaderBase::LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue );

			/**
			 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _mValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetMatrix3x3( CShaderBase::LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue );

			/**
			 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _mValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetMatrix4x4( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue );

			/**
			 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pmValues New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, LSUINT32 _ui32Total );


		protected :
			// == Types.
			/**
			 * Mapping between the uniform ID's we give out and the API and uniform buffer data.
			 */
			typedef struct LSG_UNIFORM_MAP {
				/** API ID value. */
				LSUINTPTR												uiptrApiId;

				/** Pointer to the start of the uniform buffer data. */
				LSUINT32												ui32DataPtr;

				/** Determines if the data is dirty.  If so, it is updated without
				 *	being checked. */
				LSBOOL													bDirty;

				/** Name of the uniform. */
				LSUINT32												ui32Name;

				/** Location of the uniform. */
				LSG_SHADER_TYPES					stType;
			} * LPLSG_UNIFORM_MAP, * const LPCLSG_UNIFORM_MAP;


			// == Members.
			/** The shaders to link. */
			CMetalCompiledShader *										m_pogcsShader[LSG_ST_TOTAL];

			/** The Metal shader program. */
			LSUINT32													m_uiProgram;

			/**
			 * Our reference count.
			 */
			LSUINT32													m_ui32Ref;

			/**
			 * Uniform globals in the shaders.
			 */
			CShaderTranslationProcessorBase::LSG_UNIFORM_LIST			m_ulUniforms[LSG_ST_TOTAL];

			/**
			 * Pointer into the data for the next uniform handle.
			 */
			LSUINT32													m_ui32NextUniformStart;

			/**
			 * Uniform local buffer.  Used to prevent redundant updating of values.
			 */
			CVectorPoD<LSUINT8, LSUINT32>								m_vUniformBuffer;

			/**
			 * Our array of uniform ID's.  When we give out a uniform ID, we actually give out an index
			 *	into this array.  This array holds the native API-specific uniform ID as well as an index
			 *	into our uniform buffer.  When the user sets uniform data, we use the index to get the API
			 *	ID and use the buffer data to check to see if an update is really necessary.  This helps
			 *	avoid redundant updating of the shader.
			 */
			CVectorPoD<LSG_UNIFORM_MAP>									m_vUniformDataMap;

			/**
			 * Temporary buffer.
			 */
			CVectorPoD<LSUINT8, LSUINT32>								m_vTempBuffer;


			// == Functions.
			/**
			 * Increase the reference count on this object.
			 */
			LSVOID LSE_CALL												IncRef();

			/**
			 * Decrease the reference count on this object.
			 */
			LSVOID LSE_CALL												DecRef();

			/**
			 * Converts from the shader parser semantic type to a vertex buffer attribute type.
			 *
			 * \param _ui32Semantic The shader semantic type.
			 * \return Returns the converted attribute type.
			 */
			static LSUINT32 LSE_CALL									ShaderSemanticToVertexAttribute( LSUINT32 _ui32Semantic );

		private :
			friend class												CMetalShaderProgramManager;
		};


		// == Functions.
		/**
		 * Sets sharing.
		 *
		 * \param _bVal If true, sharing is enabled, otherwise not.
		 */
		static LSVOID LSE_CALL											SetSharing( LSBOOL _bVal );

		/**
		 * Creates a shader program given various shader parts, any of which may be NULL (but not all of which).  If
		 *	sharing is enabled, the shader may be shared, otherwise each returned shader program will be unique.
		 *
		 * \param _pogcsVertexShader The vertex shader.
		 * \param _pogcsPixelShader The pixel shader.
		 * \param _pogcsGeometryShader The geometry shader.
		 * \param _pulUniforms Uniforms in the shader.
		 * \param _palAttributes Attributes in the shader.
		 * \param _sError Filled with an error report in the event of an error.
		 * \return Returns a shader program created from the given shader parts.
		 */
		static CMetalShaderProgram * LSE_CALL							CreateShaderProgram( CMetalCompiledShader * _pogcsVertexShader,
			CMetalCompiledShader * _pogcsPixelShader, CMetalCompiledShader * _pogcsGeometryShader,
			const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
			const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
			CString &_sError );

		/**
		 * Delete a shader program.  If sharing, its reference count is decreased only.  Otherwise it is fully deleted from memory.
		 *	Must have been created with a previous call to CreateShaderProgram();
		 *
		 * \param _pogspProgram The shader program to delete.
		 */
		static LSVOID LSE_CALL											DeleteShaderProgram( CMetalShaderProgram * _pogspProgram );

		/**
		 * Delete all shader programs that have a 0 reference count.  Returns the number of shader programs not deleted.
		 *
		 * \return Returns the number of shader programs not deleted. 
		 */
		static LSUINT32 LSE_CALL										DeleteUnreferencedShaderPrograms();


	protected :
		// == Types.
		/**
		 * The key for the shader maps.  The shader part ID's are used to determine
		 *	if a shader program is the same as another shader program.
		 */
		typedef struct LSG_SHADER_PROGRAM_KEY {
			/**
			 * The unique ID of the vertex shader.
			 */
			LSUINT32													ui32VertexId;

			/**
			 * The unique ID of the pixel shader.
			 */
			LSUINT32													ui32PixelId;

			/**
			 * The unique ID of the geometry shader.
			 */
			LSUINT32													ui32GeometryId;


			// == Various constructors.
			LSE_CALLCTOR												LSG_SHADER_PROGRAM_KEY();
			
			// == Operators.
			/**
			 * Less-than operator.
			 *
			 * \param _spkOther The key against which to test.
			 * \return Returns true if this object is less than the given object.
			 */
			LSBOOL LSE_CALL												operator < ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const;

			/**
			 * Equality operator.
			 *
			 * \param _spkOther The key against which to test.
			 * \return Returns true if this object is equal to the given object.
			 */
			LSBOOL LSE_CALL												operator == ( const LSG_SHADER_PROGRAM_KEY &_spkOther ) const;
		} * LPLSG_SHADER_PROGRAM_KEY, * LPCLSG_SHADER_PROGRAM_KEY;


		// == Members.
		/** Is sharing enabled?  Defaults to true.  Tools/editors should set this to false at program start. */
		static LSBOOL													m_bSharing;

		/** Map of shader programs. */
		static CLinearMap<LSG_SHADER_PROGRAM_KEY, CMetalShaderProgram *>
																		m_mShaders;


		// == Functions.
		/**
		 * Creates a key from a shader program.
		 *
		 * \param _ogspProgram The program from which to create a key after its shaders have been set.
		 * \return Returns the key describing the shader program.
		 */
		static LSG_SHADER_PROGRAM_KEY LSE_CALL							CreateKey( const CMetalShaderProgram &_ogspProgram );
	};

}	// namespace lsg

#endif	// #if defined( LSG_METAL )

#endif	// __LSG_METALSHADERPROGRAMMANAGER_H__
