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
 * Description: The base class for shaders.  This is overridden by classes specific to a given graphics API.
 */


#ifndef __LSG_SHADERBASE_H__
#define __LSG_SHADERBASE_H__

#include "../LSGGraphicsLib.h"
#include "../PreProc/LSGPreProc.h"
#include "Algorithm/LSTLAlgorithm.h"
#include "HighLevel/LSFFilesEx.h"
#include "LSGShaderLanguageParser.h"
#include "LSGShaderLexer.h"
#include "LSGShaderParserWrapper.h"
#include "LSGShaderTranslationProcessorBase.h"
#include "Map/LSTLLinearMap.h"
#include "Matrix/LSMMatrix2x2.h"
#include "Matrix/LSMMatrix3x3.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Set/LSTLSet.h"
#include "String/LSTLString.h"
#include "String/LSTLStringList.h"
#include "Vector/LSMVector2.h"
#include "Vector/LSMVector3.h"
#include "Vector/LSMVector4.h"

namespace lsg {

	// == Enumerations.
	/** Error codes returned by the parser. */
	enum LSG_SHADER_ERRORS {
		LSG_SE_SUCCESS,					/**< No error. */
		LSG_SE_INVALIDINPUT,			/**< Input shader string is NULL. */
		LSG_SE_STREAMERROR,				/**< Error creating the input stream. */
		LSG_SE_PARSERERROR,				/**< Error during parsing. */
		LSG_SE_TRANSLATIONRERROR,		/**< Error during translation. */
		LSG_SE_FILENOTFOUND,			/**< File does not exist. */
		LSG_SE_OUTOFMEMORY,				/**< Out of memory. */
		LSG_SE_SYNTAX,					/**< Syntax error. */
		LSG_SE_INCLUDERECURSE,			/**< Too many recursive includes. */
		LSG_SE_INVALIDSHADERTYPE,		/**< Invalid shader type. */
		LSG_SE_GFXAPIERROR,				/**< Error related to the graphics API. */
		LSG_SE_NOENDIF,					/**< #endif not found. */
		LSG_SE_PREPROC,					/**< Preprocessor error. */
	};

	/**
	 * Class CShaderBase
	 * \brief The base class for shaders.
	 *
	 * Description: The base class for shaders.  This is overridden by classes specific to a given graphics API.
	 */
	class CShaderBase {
	public :
		// == Various constructors.
		LSE_CALLCTOR						CShaderBase();
		virtual LSE_CALLCTOR				~CShaderBase();


		// == Types.
		/** A uniform handle. */
		typedef uint32_t					LSG_HANDLE;

		/** A preprocessor define. */
		typedef struct LSG_DEFINE {
			/** The macro. */
			const char *					pcMacro;
			/** The macro value. */
			const char *					pcDefinition;
		} * LPLSG_DEFINE, * const LPCLSG_DEFINE;


		// == Functions.
		/**
		 * Reset everything.
		 */
		void LSE_CALL						Reset();

		/**
		 * Gets the uniqe ID of this shader.  The ID is 0 if the shader is not actually ready
		 *	to be activated (that is, has not been compiled and registered with the graphics API).
		 * Activating a shader with a 0 ID causes the fixed-function pipeline to become activated
		 *	(if the fixed-function pipeline is supported).  Because this base class cannot perform
		 *	the hardware-specific operations, it always returns 0.  It is up to the specialization
		 *	classes to return a valid ID.
		 *
		 * \return Returns the unique ID for this shader.
		 */
		virtual uint64_t LSE_CALL			GetId() const;

		/**
		 * Parse the set shaders.  
		 * In the case of OpenGL and DirectX, the shaders are converted to either GLSL or HLSL,
		 *	respectively, and sent to the respective sub-compiler, which may throw its own errors.
		 *	These errors are sent back through _sError, and may contain code not present in the
		 *	original shaders.  This is due to the conversion.  The engine compiler does its best to
		 *	maintain synchrony, but this is not always possible.  The primary guarantee of the
		 *	compiler is that any shader recognized as valid by this compiler will be valid and run
		 *	in the same manner on all supported platforms.
		 *
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			Compile( CString &_sError );

		/**
		 * Sets and parses shaders from the provided files.
		 *
		 * \param _pcVertexFile The vertex shader file to compile.
		 * \param _pcPixelFile The pixel shader file to compile.
		 * \param _psError The returned error if any.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			CompileFromFiles( const char * _pcVertexFile, const char * _pcPixelFile,
			CString * _psError = NULL );

		/**
		 * Sets the vertex shader.  The shader must be in finalized format.  That is, #include
		 *	directives must have been fully expanded and all the parts for the shader
		 *	included in the given string.  The string must be NULL-terminated if _ui32Length
		 *	is 0.  New-lines can consist of either \r\n pairs or just \n.  Specifically, \r
		 *	is ignored entirely.
		 *
		 * \param _pcShader The shader to set.
		 * \param _ui32Length The length of the string to which _pcShader points or 0 if _pcShader is NULL-terminated.
		 * \return Returns false if there is not enough memory to store the shader text.
		 */
		LSBOOL LSE_CALL						SetVertexShader( const char * _pcShader, uint32_t _ui32Length = 0 );

		/**
		 * Sets the pixel shader.  The shader must be in finalized format.  That is, #include
		 *	directives must have been fully expanded and all the parts for the shader
		 *	included in the given string.  The string must be NULL-terminated if _ui32Length
		 *	is 0.  New-lines can consist of either \r\n pairs or just \n.  Specifically, \r
		 *	is ignored entirely.
		 *
		 * \param _pcShader The shader to set.
		 * \param _ui32Length The length of the string to which _pcShader points or 0 if _pcShader is NULL-terminated.
		 * \return Returns false if there is not enough memory to store the shader text.
		 */
		LSBOOL LSE_CALL						SetPixelShader( const char * _pcShader, uint32_t _ui32Length = 0 );

		/**
		 * Load a shader from a file.  Whether the shader is vertex or pixel must be specified.  Proper
		 *	workflow is to load both the vertex and pixel shaders, then compile, then use.
		 *
		 * \param _pcPath The path to the file to compile.
		 * \param _stShaderType Type of the shader.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			SetShaderFromFile( const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
			CString &_sError );

		/**
		 * Load a shader from from memory.  Whether the shader is vertex or pixel must be specified.  Proper
		 *	workflow is to load both the vertex and pixel shaders, then compile, then use.
		 *
		 * \param _pui8Src The code to compile.
		 * \param _ui32Length Length of the buffered code to compile.
		 * \param _pcPath The path to the code being compiled.  May be NULL if there is no source file.
		 * \param _stShaderType Type of shader being compiled.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			SetShaderFromMemory( const uint8_t * _pui8Src, uint32_t _ui32Length,
			const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
			CString &_sError );

		/**
		 * Sets preprocessor macros for a given shader type.
		 *
		 * \param _pdDefines The preprocessor macros.
		 * \param _ui32Total Total macros.
		 * \param _stShaderType The shader type to which to assign the macros.
		 * \return Returns true if there is enough memory to assign the macros.
		 */
		LSBOOL LSE_CALL						SetMacros( const LSG_DEFINE * _pdDefines, uint32_t _ui32Total, LSG_SHADER_TYPES _stShaderType );

		/**
		 * Activation callback.  A system value is passed that is to be used internally and changes meaning depending on the
		 *	graphics API.
		 *
		 * \param _pvSysValue A value with special system meaning.
		 * \return Returns false if there is a failure to apply the shader via the current graphics API.
		 */
		virtual LSBOOL LSE_CALL				Activate( void * _pvSysValue ) = 0;

		/**
		 * Deactivation callback.  Calls by the system when the shader is deactivated.
		 */
		virtual void LSE_CALL				Deactivate() = 0;

		/**
		 * Called once prior to each render.
		 */
		virtual void LSE_CALL				PreRender();

		/**
		 * Gets a handle for a uniform variable in the shader.
		 *
		 * \param _pcName Name of the variable whose handle is to be obtained.
		 * \return Returns the handle to the given variable or LSE_MAXU32 if the given name does not exist.
		 */
		virtual LSG_HANDLE LSE_CALL			GetConstantByName( const char * _pcName ) = 0;

		/**
		 * Sets a single scalar value in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _fValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetFloat( LSG_HANDLE _hHandle, float _fValue ) = 0;

		/**
		 * Sets an array of scalar values in the shader of type float.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pfValues New values.
		 * \param _ui32Total Total floats to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetFloatArray( LSG_HANDLE _hHandle, const float * _pfValues, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a single scalar value in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _iValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetInt( LSG_HANDLE _hHandle, int32_t _iValue ) = 0;

		/**
		 * Sets an array of scalar values in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pi32Values New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetIntArray( LSG_HANDLE _hHandle, const int32_t * _pi32Values, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a single scalar value in the shader of type bool.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _bValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetBool( LSG_HANDLE _hHandle, LSBOOL _bValue ) = 0;

		/**
		 * Sets an array of scalar values in the shader of type bool.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pbValues New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetBoolArray( LSG_HANDLE _hHandle, const LSBOOL * _pbValues, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a 2-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector2( LSG_HANDLE _hHandle, const CVector2 &_vValue ) = 0;

		/**
		 * Sets a 2-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector2Array( LSG_HANDLE _hHandle, const CVector2 * _pvValues, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a 3-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector3( LSG_HANDLE _hHandle, const CVector3 &_vValue ) = 0;

		/**
		 * Sets a 3-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector3Array( LSG_HANDLE _hHandle, const CVector3 * _pvValues, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a 4-dimensional vector in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector4( LSG_HANDLE _hHandle, const CVector4 &_vValue ) = 0;

		/**
		 * Sets a 4-dimensional vector array in the shader of type int.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _vValue New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetVector4Array( LSG_HANDLE _hHandle, const CVector4 * _pvValues, uint32_t _ui32Total ) = 0;

		/**
		 * Sets a 2-by-2 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetMatrix2x2( LSG_HANDLE _hHandle, const CMatrix2x2 &_mValue ) = 0;
		
		/**
		 * Sets a 3-by-3 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetMatrix3x3( LSG_HANDLE _hHandle, const CMatrix3x3 &_mValue ) = 0;
		
		/**
		 * Sets a 4-by-4 matrix in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _mValue New value.
		 * \return Returns true if the value was set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetMatrix4x4( LSG_HANDLE _hHandle, const CMatrix4x4 &_mValue ) = 0;

		/**
		 * Sets an array of 4-by-4 matrices in a shader.  Shader must be active.
		 *
		 * \param _hHandle Handle to the value to set in the shader.
		 * \param _pmValues New values.
		 * \param _ui32Total Total ints to set.
		 * \return Returns true if the values were set in the shader.
		 */
		virtual LSBOOL LSE_CALL				SetMatrix4x4Array( LSG_HANDLE _hHandle, const CMatrix4x4 * _pmValues, uint32_t _ui32Total ) = 0;

		/**
		 * Adds a folder to search for headers when #include and #import directives are reached in shader files.
		 *
		 * \param _pcPath The UTF-8 path of the folder to include in the search routine when seeking headers.
		 * \return Returns true if the path was added to the list of search paths.
		 */
		static LSBOOL LSE_CALL				AddSearchPath( const char * _pcPath );

		/**
		 * Removes a folder from the paths to search for headers when #include and #import directives are
		 *	reached in shader files.
		 *
		 * \param _pcPath The UTF-8 path of the folder to remove from the search routine when seeking headers.
		 */
		static void LSE_CALL				RemoveSearchPath( const char * _pcPath );

		/**
		 * Gets the total number of header search paths.
		 *
		 * \return Returns the number of header search paths.
		 */
		static uint32_t LSE_CALL			TotalSearchPaths();

		/**
		 * Gets a header search path by index.
		 *
		 * \param _ui32Index Index of the header search path to get.
		 * \return Returns a constant reference to the search path at the given index.
		 */
		static const CString & LSE_CALL		GetSearchPath( uint32_t _ui32Index );

	protected :
		// == Enumerations.
		/**
		 * Clear states for internal use.
		 */
		enum LSG_CLEAR_STATE {
			LSG_CS_NONE,
			LSG_CS_CLEAR,
			LSG_CS_SEEKING_ENDIF,
		};
		
		// == Types.
		/**
		 * Included files.
		 */
		typedef struct LSG_INCLUDES {
			/**
			 * The path to the included file.
			 */
			CString							sPath;

			/**
			 * Number of times the path has been included.
			 */
			uint32_t						ui32RefCount;

			// == Various constructors.
			LSE_CALLCTOR					LSG_INCLUDES();

			// == Operators.
			/**
			 * Less-than comparison.  Goes by path value.
			 *
			 * \param _iOther Include against which to compare.
			 * \return Returns true if this include path is less than the given include path.
			 */
			LSBOOL LSE_CALL					operator < ( const LSG_INCLUDES &_iOther ) const;

			/**
			 * Equality comparison.  Goes by path value.
			 *
			 * \param _iOther Include against which to compare.
			 * \return Returns true if this include path is equal to the given include path.
			 */
			LSBOOL LSE_CALL					operator == ( const LSG_INCLUDES &_iOther ) const;
		} * LPLSG_INCLUDES, * const LPCLSG_INCLUDES;

		/**
		 * Preprocessor defines.
		 */
		typedef struct LSG_PREPROC_DEFINE {
			/**
			 * Macro name.
			 */
			CString							sName;

			/**
			 * Macro definition.
			 */
			CString							sDef;


			// == Operators.
			/**
			 * Copy operator.
			 *
			 * \param _pdOther The object to copy.
			 * \return Returns the copied object.
			 */
			LSG_PREPROC_DEFINE & LSE_CALL	operator = ( const LSG_PREPROC_DEFINE &_pdOther );
		} * LPLSG_PREPROC_DEFINE, * const LPCLSG_PREPROC_DEFINE;

		/**
		 * Included files.
		 */
		typedef CLinearMap<CString, uint32_t>		CIncludeSet;


		// == Members.
		/**
		 * The syntax parser.  Can be deleted when done compiling.
		 */
		CShaderParserWrapper *				m_pspwParser;

		/**
		 * The lexer.  Can be deleted when done compiling.
		 */
		CShaderLexer *						m_pslLexer;

		/**
		 * Definitions for the shaders.
		 */
		CVector<LSG_PREPROC_DEFINE>			m_vDefs[LSG_ST_TOTAL];

		/**
		 * The vertex shader text (in final form).  Length() is 0 if there is none.
		 */
		CString								m_sVertexShader;

		/**
		 * The pixel shader text (in final form).  Length() is 0 if there is none.
		 */
		CString								m_sPixelShader;

		/**
		 * The final form of the vertex shader.
		 */
		CString								m_sFinalVertex;

		/**
		 * The final form of the pixel shader.
		 */
		CString								m_sFinalPixel;

		/**
		 * The result of translation.
		 */
		CShaderTranslationProcessorBase::LSG_ERROR_CODES
											m_ecTranslationResult;

		/**
		 * The translator.
		 */
		CShaderTranslationProcessorBase *	m_pstpbTranslator;

		/**
		 * Global semantics in the shaders.
		 */
		CShaderTranslationProcessorBase::LSG_UNIFORM_SEMANTIC_LIST
											m_uslSemantics[LSG_ST_TOTAL];

		/**
		 * Uniform globals in the shaders.
		 */
		CShaderTranslationProcessorBase::LSG_UNIFORM_LIST
											m_ulUniforms[LSG_ST_TOTAL];

		/**
		 * Uniform globals in the shaders.
		 */
		CShaderTranslationProcessorBase::LSG_ATTRIBUTE_LIST
											m_ulAttributes[LSG_ST_TOTAL];

		/**
		 * Paths to search for header files.
		 */
		static CStringList					m_slHeaderPaths;


		// == Functions.
		/**
		 * Reset only the parser classes.  Retains the shader texts.
		 */
		void LSE_CALL						ResetParsers();

		/**
		 * Parse a shader.  The shader must be in finalized format.  That is, #include
		 *	directives must have been fully expanded and all the parts for the shader
		 *	included in the given string.  The string must be NULL-terminated if _ui32Length
		 *	is 0.  New-lines can consist of either \r\n pairs or just \n.  Specifically, \r
		 *	is ignored entirely.
		 * In the case of OpenGL and DirectX, the shader is converted to either GLSL or HLSL,
		 *	respectively, and sent to the respective sub-compiler, which may throw its own errors.
		 *	These errors are sent back through _sError, and may contain code not present in the
		 *	original shader.  This is due to the conversion.  The engine compiler does its best to
		 *	maintain synchrony, but this is not always possible.  The primary guarantee of the
		 *	compiler is that any shader recognized as valid by this compiler will be valid and run
		 *	in the same manner on all supported platforms.
		 *
		 * \param _pcShader The shader to compile.
		 * \param _ui32Length The length of the string to which _pcShader points or 0 if _pcShader is NULL-terminated.
		 * \param _stShaderType Type of the shader.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			Compile( const char * _pcShader, uint32_t _ui32Length,
			LSG_SHADER_TYPES _stShaderType,
			CString &_sError );

		/**
		 * Compile a shader from a file.  The path must be in UTF-8 format.  The file data is treated
		 *	as UTF-8.
		 *
		 * \param _pcPath The path to the file to compile.
		 * \param _stShaderType Type of the shader.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			CompileFromFile( const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
			CString &_sError );

		/**
		 * Compile a shader from a file.  The path must be in UTF-8 format.  The file data is treated
		 *	as UTF-8.
		 *
		 * \param _pcPath The path to the file to compile.
		 * \param _slData String list to which the file data is added.
		 * \param _ui32Line Line at which the file data is added.
		 * \param _stShaderType Type of the shader.
		 * \param _isIncludes Set of included files.
		 * \param _mMacros Defines macros.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			CompileFromFile( const char * _pcPath, CStringList &_slData, uint32_t _ui32Line,
			LSG_SHADER_TYPES _stShaderType,
			CIncludeSet &_isIncludes,
			CPreProc::LSG_MACROS &_mMacros,
			CString &_sError );

		/**
		 * Compile a shader from memory.  Performs preprocessing.
		 *
		 * \param _pui8Src The code to compile.
		 * \param _ui32Length Length of the buffered code to compile.
		 * \param _pcPath The path to the code being compiled.  May be NULL if there is no source file.
		 * \param _slData String list to which the file data is added.
		 * \param _ui32Line Line at which the file data is added.
		 * \param _stShaderType Type of shader being compiled.
		 * \param _isIncludes Set of included files.
		 * \param _mMacros Defines macros.
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		LSG_SHADER_ERRORS LSE_CALL			CompileFromMemory( const uint8_t * _pui8Src, uint32_t _ui32Length,
			const char * _pcPath, CStringList &_slData, uint32_t _ui32Line,
			LSG_SHADER_TYPES _stShaderType,
			CIncludeSet &_isIncludes,
			CPreProc::LSG_MACROS &_mMacros,
			CString &_sError );

		/**
		 * Post compile.  If all shaders are compiled successfully, this is called, meant to be overridden by the
		 *	classes that specialize towards specific graphics API's.
		 *
		 * \param _sError The returned error string if there is an error.
		 * \return Returns an error code or LSG_SE_SUCCESS.
		 */
		virtual LSG_SHADER_ERRORS LSE_CALL	PostCompile( CString &_sError );

		/**
		 * Creates a new translator.  Must be overridden.
		 */
		virtual void LSE_CALL				CreateTranslator() = 0;

		/**
		 * Destroys the translator.  Must be overridden.
		 */
		virtual void LSE_CALL				DestroyTranslator() = 0;

		/**
		 * Gets the file part of an #include or #import directive.
		 *
		 * \param _sDirective The line to scan for the file name.
		 * \return Returns the file path part of the line or an empty string indicating a syntax error.
		 */
		static CString LSE_CALL				GetFilePart( const CString &_sDirective );

		/**
		 * Find a header file.  Searches first the relative path from the main includer, then each of the search paths
		 *	set by the engine user.
		 *
		 * \param _pcIncluder Path to the file that is including the 
		 * \param _sFileToInclude The file to include.
		 * \return Returns the path to the file to include or an empty string if the file could not be found.
		 */
		static CString LSE_CALL				FindHeader( const char * _pcIncluder, const CString &_sFileToInclude );

		/**
		 * Handles a #define.
		 *
		 * \param _sLine The text after "#define".  Parsed to determin the name, parameters, and definition of a macro.
		 * \param _mMacros Set of macros to which to add the new macro.
		 * \return Returns true if the text is valid and the macro was added.
		 */
		static LSBOOL LSE_CALL				ParseDefine( const CString &_sLine, CPreProc::LSG_MACROS &_mMacros );

		/**
		 * Removes comments from code.
		 *
		 * \param _slText The text from which to remove comments.
		 */
		static void LSE_CALL				RemoveComments( CStringList &_slText );

		/**
		 * Skip strings in code.
		 *
		 * \param _sString The string containing the string to skip.
		 * \param _ui32Pos The position of the string to skip.  Upon returning, this holds the new position from
		 *	where parsing can continue.
		 */
		static void LSE_CALL				SkipString( const CString &_sLine, uint32_t &_ui32Pos );

		/**
		 * Replace all macros in a given string.
		 *
		 * \param _sString The string in which to expand macros.
		 * \param _mMacros The macro dictionary.
		 * \param _sError The error string if the return is false.
		 * \return Returns true if the expansion succeeds.  Otherwise filles _sError with the error string and returns false.
		 */
		static LSBOOL LSE_CALL				ExpandMacros( CString &_sString, const CPreProc::LSG_MACROS &_mMacros, CString &_sError );

		/**
		 * Replace all macros in a given string.
		 *
		 * \param _sString The string in which to expand macros.
		 * \param _mMacros The macro dictionary.
		 * \param _sUsedValues Values that have already been expanded.
		 * \param _sError The error string if the return is false.
		 * \return Returns true if the expansion succeeds.  Otherwise filles _sError with the error string and returns false.
		 */
		static LSBOOL LSE_CALL				ExpandMacros( CString &_sString, const CPreProc::LSG_MACROS &_mMacros, const CSet<CString> &_sUsedValues, CString &_sError );

		/**
		 * Gets the parameters for a function-style macro.  If no parameters are given, returns false.
		 *
		 * \param _psString The string from which to extract function-style macro parameters.
		 * \param _ui32Pos The position at which to begin the extraction.
		 * \param _ui32NewPos The position after successful extraction.
		 * \param _vRet The returned parameters, as strings.
		 * \return Returns true if the parameters were extracted.  False indicates no parameters present or an invalid parameter string.
		 */
		static LSBOOL LSE_CALL				GetMacroParms( const CString &_sString, uint32_t _ui32Pos, uint32_t &_ui32NewPos, CVector<CString> &_vRet );

		/**
		 * Gets the replacement string for a given macro and optional parameters.  Returns false if there is an error expanding the macro.
		 *
		 * \param _ui32MacroIndex Index of the macro to expand.
		 * \param _vParms Optional macro parameters.  If this does not match the number of parameters the macro receives, false is returned.
		 * \param _mMacros The macro dictionary.
		 * \param _sUsedValues Values that have already been expanded.  Allows recursion to be avoided.
		 * \param _sRet The returned fully expanded string.
		 * \param _sError The error report string if there is an error during macro expansion.
		 * \return Returns true if there were no errors during macro expansion.
		 */
		static LSBOOL LSE_CALL				GetMacroReplacementString( uint32_t _ui32MacroIndex, CVector<CString> &_vParms,
			const CPreProc::LSG_MACROS &_mMacros, const CSet<CString> &_sUsedValues,
			CString &_sRet,
			CString &_sError );

	};

}	// namespace lsg

#endif	// __LSG_SHADERBASE_H__
