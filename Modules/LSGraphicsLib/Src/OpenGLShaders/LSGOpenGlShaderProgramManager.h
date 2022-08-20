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
 * Description: In OpenGL, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
 *	exactly alike.
 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
 */


#ifndef __LSG_OPENGLSHADERPROGRAMMANAGER_H__
#define __LSG_OPENGLSHADERPROGRAMMANAGER_H__

#include "../LSGGraphicsLib.h"

#if defined( LSG_OGL )

#include "../OpenGL/LSGOpenGl.h"
#include "../OpenGLVertexBuffer/LSGOpenGlVertexBuffer.h"
#include "../ShaderLanguage/LSGShaderBase.h"
#include "../ShaderLanguage/LSGShaderTranslationProcessorBase.h"
#include "../VertexBuffer/LSGVertexBufferBase.h"
#include "LSGOpenGlCompiledShader.h"
#include "Map/LSTLLinearMap.h"

namespace lsg {

	/**
	 * Class COpenGlShaderProgramManager
	 * \brief Allows sharing of shader programs in OpenGL.
	 *
	 * Description: In OpenGL, shader programs are made by linking shader parts together.  In L. Spiro Engine, shader
	 *	parts are automatically shared (optionally disablable for multi-context situations), reducing the total possible
	 *	unique combinations between shader parts, meaning that it is very possible for entire shader programs to be
	 *	exactly alike.
	 * A second sharing system for shader programs is necessary, reducing the risk of the redundant application of shaders.
	 */
	class COpenGlShaderProgramManager {
	public :
		// == Types.
		/** A set of locations for a single attribute.  One for every possible usage index. */
		typedef struct LSG_ATTRIBUTE_USAGE_LOCS {
			/** The locations. */
			GLint														iLoc[LSG_MAX_VERTEX_SEMANTIC];
		} * LPLSG_ATTRIBUTE_USAGE_LOCS, * const LPCLSG_ATTRIBUTE_USAGE_LOCS;

		/**
		 * Class COpenGlShaderProgram
		 * \brief A shader program in OpenGL.
		 *
		 * Description: A shader program wrapper for OpenGL.
		 */
		class COpenGlShaderProgram {
		public :
			// == Various constructors.
			LSE_CALLCTOR												COpenGlShaderProgram();
			LSE_CALLCTOR												~COpenGlShaderProgram();


			// == Functions.
			/**
			 * Adds a shader.
			 *
			 * \param _pogcsShader Pointer to the vertex shader to add to this program.
			 */
			void LSE_CALL												AddShader( COpenGlCompiledShader * _pogcsShader );

			/**
			 * Gets a shader in a given slot.
			 *
			 * \param _stShaderType The type of shader to get from this object.
			 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
			 */
			COpenGlCompiledShader * LSE_CALL							GetShader( LSG_SHADER_TYPES _stShaderType );

			/**
			 * Gets a shader in a given slot.
			 *
			 * \param _stShaderType The type of shader to get from this object.
			 * \return Returns a pointer to the shader set in the given slot.  Returns NULL for empty slots.
			 */
			const COpenGlCompiledShader * LSE_CALL						GetShader( LSG_SHADER_TYPES _stShaderType ) const;

			/**
			 * Finalize the shader for use with OpenGL.
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
			void LSE_CALL												Activate();

			/**
			 * Gets the reference count for this object.
			 *
			 * \return Returns the reference count for this object.
			 */
			uint32_t LSE_CALL											GetRef() const;

			/**
			 * Gets the unique ID of this object.  This matches the OpenGL shader program ID, which may be 0.
			 *
			 * \return Returns the unique ID for this object.
			 */
			uint32_t LSE_CALL											GetId() const;

			/**
			 * Gets the attribute locations.
			 *
			 * \return Returns a pointer to the attribute locations for this shader.
			 */
			const LSG_ATTRIBUTE_USAGE_LOCS * LSE_CALL					GetAttributeLocs() const;

			/**
			 * Gets the total attributes.
			 *
			 * \return Returns the total attributes in GetAttributeLocs().
			 */
			LSE_INLINE uint32_t LSE_CALL								TotalAttributes() const;

			/**
			 * Gets a handle for a uniform variable in the shader.
			 *
			 * \param _pcName Name of the variable whose handle is to be obtained.
			 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
			 */
			CShaderBase::LSG_HANDLE LSE_CALL							GetConstantByName( const char * _pcName );

			/**
			 * Sets a single scalar value in the shader of type float.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _fValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetFloat( CShaderBase::LSG_HANDLE _hHandle, float _fValue );

			/**
			 * Sets an array of scalar values in the shader of type float.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pfValues New values.
			 * \param _ui32Total Total floats to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetFloatArray( CShaderBase::LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total );

			/**
			 * Sets a single scalar value in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _iValue New value.
			 * \return Returns true if the value was set in the shader.
			 */
			LSBOOL LSE_CALL												SetInt( CShaderBase::LSG_HANDLE _hHandle, int32_t _iValue );

			/**
			 * Sets an array of scalar values in the shader of type int.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pi32Values New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetIntArray( CShaderBase::LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total );

			/**
			 * Sets an array of boolean values in the shader.  Shader must be active.
			 *
			 * \param _hHandle Handle to the value to set in the shader.
			 * \param _pi32Values New values.
			 * \param _ui32Total Total ints to set.
			 * \return Returns true if the values were set in the shader.
			 */
			LSBOOL LSE_CALL												SetBoolArray( CShaderBase::LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total );

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
			LSBOOL LSE_CALL												SetVector2Array( CShaderBase::LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total );

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
			LSBOOL LSE_CALL												SetVector3Array( CShaderBase::LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total );

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
			LSBOOL LSE_CALL												SetVector4Array( CShaderBase::LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total );

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
			LSBOOL LSE_CALL												SetMatrix4x4Array( CShaderBase::LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total );


		protected :
			// == Types.
			/**
			 * Mapping between the uniform ID's we give out and the API and uniform buffer data.
			 */
			typedef struct LSG_UNIFORM_MAP {
				/** API ID value. */
				uintptr_t												uiptrApiId;

				/** Pointer to the start of the uniform buffer data. */
				uint32_t												ui32DataPtr;

				/** Determines if the data is dirty.  If so, it is updated without being checked. */
				LSBOOL													bDirty;

				/** Name of the uniform. */
				uint32_t												ui32Name;

				/** Location of the uniform. */
				LSG_SHADER_TYPES										stType;
			} * LPLSG_UNIFORM_MAP, * const LPCLSG_UNIFORM_MAP;


			// == Members.
			/** The shaders to link. */
			COpenGlCompiledShader *										m_pogcsShader[LSG_ST_TOTAL];

			/** Array of attribute locations. */
			LSG_ATTRIBUTE_USAGE_LOCS *									m_paulLocs;

			/** Total attribute use locations. */
			uint32_t													m_ui32Attribs;

			/** The OpenGL shader program. */
			GLuint														m_uiProgram;

			/** Our reference count. */
			uint32_t													m_ui32Ref;

			/** Uniform globals in the shaders. */
			CShaderTranslationProcessorBase::LSG_UNIFORM_LIST			m_ulUniforms[LSG_ST_TOTAL];

			/** Pointer into the data for the next uniform handle. */
			uint32_t													m_ui32NextUniformStart;

			/** Uniform local buffer.  Used to prevent redundant updating of values. */
			CVectorPoD<uint8_t, uint32_t>								m_vUniformBuffer;

			/** Our array of uniform ID's.  When we give out a uniform ID, we actually give out an index
			 *	into this array.  This array holds the native API-specific uniform ID as well as an index
			 *	into our uniform buffer.  When the user sets uniform data, we use the index to get the API
			 *	ID and use the buffer data to check to see if an update is really necessary.  This helps
			 *	avoid redundant updating of the shader. */
			CVectorPoD<LSG_UNIFORM_MAP>									m_vUniformDataMap;

			/** Temporary buffer. */
			CVectorPoD<uint8_t, uint32_t>								m_vTempBuffer;


			// == Functions.
			/**
			 * Increase the reference count on this object.
			 */
			void LSE_CALL												IncRef();

			/**
			 * Decrease the reference count on this object.
			 */
			void LSE_CALL												DecRef();

			/**
			 * Converts from the shader parser semantic type to a vertex buffer attribute type.
			 *
			 * \param _ui32Semantic The shader semantic type.
			 * \return Returns the converted attribute type.
			 */
			static uint32_t LSE_CALL									ShaderSemanticToVertexAttribute( uint32_t _ui32Semantic );

		private :
			friend class												COpenGlShaderProgramManager;
		};


		// == Functions.
		/**
		 * Sets sharing.
		 *
		 * \param _bVal If true, sharing is enabled, otherwise not.
		 */
		static void LSE_CALL											SetSharing( LSBOOL _bVal );

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
		static COpenGlShaderProgram * LSE_CALL							CreateShaderProgram( COpenGlCompiledShader * _pogcsVertexShader,
			COpenGlCompiledShader * _pogcsPixelShader, COpenGlCompiledShader * _pogcsGeometryShader,
			const CShaderTranslationProcessorBase::LSG_UNIFORM_LIST * _pulUniforms,
			const CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST * _palAttributes,
			CString &_sError );

		/**
		 * Delete a shader program.  If sharing, its reference count is decreased only.  Otherwise it is fully deleted from memory.
		 *	Must have been created with a previous call to CreateShaderProgram();
		 *
		 * \param _pogspProgram The shader program to delete.
		 */
		static void LSE_CALL											DeleteShaderProgram( COpenGlShaderProgram * _pogspProgram );

		/**
		 * Delete all shader programs that have a 0 reference count.  Returns the number of shader programs not deleted.
		 *
		 * \return Returns the number of shader programs not deleted. 
		 */
		static uint32_t LSE_CALL										DeleteUnreferencedShaderPrograms();


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
			uint32_t													ui32VertexId;

			/**
			 * The unique ID of the pixel shader.
			 */
			uint32_t													ui32PixelId;

			/**
			 * The unique ID of the geometry shader.
			 */
			uint32_t													ui32GeometryId;


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
		static CLinearMap<LSG_SHADER_PROGRAM_KEY, COpenGlShaderProgram *>
																		m_mShaders;


		// == Functions.
		/**
		 * Creates a key from a shader program.
		 *
		 * \param _ogspProgram The program from which to create a key after its shaders have been set.
		 * \return Returns the key describing the shader program.
		 */
		static LSG_SHADER_PROGRAM_KEY LSE_CALL							CreateKey( const COpenGlShaderProgram &_ogspProgram );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total attributes.
	 *
	 * \return Returns the total attributes in GetAttributeLocs().
	 */
	LSE_INLINE uint32_t LSE_CALL COpenGlShaderProgramManager::COpenGlShaderProgram::TotalAttributes() const {
		return m_ui32Attribs;
	}

}	// namespace lsg

#endif	// #if defined( LSG_OGL )

#endif	// __LSG_OPENGLSHADERPROGRAMMANAGER_H__
