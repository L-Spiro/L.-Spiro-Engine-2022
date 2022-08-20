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


#include "LSGShaderBase.h"
#include "../Gfx/LSGGfx.h"
#include <sstream>


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShaderBase::LSG_INCLUDES::LSG_INCLUDES() {
		ui32RefCount = 0;
	}

	// == Operators.
	/**
	 * Less-than comparison.  Goes by path value.
	 *
	 * \param _iOther Include against which to compare.
	 * \return Returns true if this include path is less than the given include path.
	 */
	LSBOOL LSE_CALL CShaderBase::LSG_INCLUDES::operator < ( const LSG_INCLUDES &_iOther ) const {
		return sPath < _iOther.sPath;
	}

	/**
	 * Equality comparison.  Goes by path value.
	 *
	 * \param _iOther Include against which to compare.
	 * \return Returns true if this include path is equal to the given include path.
	 */
	LSBOOL LSE_CALL CShaderBase::LSG_INCLUDES::operator == ( const LSG_INCLUDES &_iOther ) const {
		return sPath == _iOther.sPath;
	}

	// == Operators.
	/**
	 * Copy operator.
	 *
	 * \param _pdOther The object to copy.
	 * \return Returns the copied object.
	 */
	CShaderBase::LSG_PREPROC_DEFINE & LSE_CALL CShaderBase::LSG_PREPROC_DEFINE::operator = ( const LSG_PREPROC_DEFINE &_pdOther ) {
		sName = _pdOther.sName;
		sDef = _pdOther.sDef;
		return (*this);
	}

	// == Members.
	// Paths to search for header files.
	CStringList CShaderBase::m_slHeaderPaths;

	// == Various constructors.
	LSE_CALLCTOR CShaderBase::CShaderBase() :
		m_pspwParser( NULL ),
		m_pslLexer( NULL ),
		m_pstpbTranslator( NULL ) {
	}
	LSE_CALLCTOR CShaderBase::~CShaderBase() {
		Reset();
	}

	// == Functions.
	/**
	 * Reset everything.
	 */
	void LSE_CALL CShaderBase::Reset() {
		ResetParsers();
		m_sVertexShader.Reset();
		m_sPixelShader.Reset();
		m_sFinalVertex.Reset();
		m_sFinalPixel.Reset();
	}

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
	uint64_t LSE_CALL CShaderBase::GetId() const {
		return 0ULL;
	}

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
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::Compile( CString &_sError ) {
		if ( m_sVertexShader.Length() ) {
			LSG_SHADER_ERRORS seRet = Compile( m_sVertexShader.CStr(), m_sVertexShader.Length(), LSG_ST_VERTEX, _sError );
			if ( seRet != LSG_SE_SUCCESS ) {
				_sError.Insert( 0, "Error compiling vertex shader: " );
				return seRet;
			}
		}

		if ( m_sPixelShader.Length() ) {
			LSG_SHADER_ERRORS seRet = Compile( m_sPixelShader.CStr(), m_sPixelShader.Length(), LSG_ST_PIXEL, _sError );
			if ( seRet != LSG_SE_SUCCESS ) {
				_sError.Insert( 0, "Error compiling pixel shader: " );
				return seRet;
			}
		}

		return PostCompile( _sError );
	}

	/**
	 * Sets and parses shaders from the provided files.
	 *
	 * \param _pcVertexFile The vertex shader file to compile.
	 * \param _pcPixelFile The pixel shader file to compile.
	 * \param _psError The returned error if any.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::CompileFromFiles( const char * _pcVertexFile, const char * _pcPixelFile,
		CString * _psError ) {
		CString sError;
		LSG_SHADER_ERRORS seErrors = SetShaderFromFile( _pcVertexFile, LSG_ST_VERTEX, sError );
		if ( seErrors != LSG_SE_SUCCESS ) {
			CStd::DebugPrintA( sError.CStr() );
			if ( _psError ) {
				(*_psError) = sError;
			}
			return seErrors;
		}

		seErrors = SetShaderFromFile( _pcPixelFile, LSG_ST_PIXEL, sError );
		if ( seErrors != LSG_SE_SUCCESS ) {
			CStd::DebugPrintA( sError.CStr() );
			if ( _psError ) {
				(*_psError) = sError;
			}
			return seErrors;
		}

		seErrors = Compile( sError );
		if ( seErrors != LSG_SE_SUCCESS ) {
			CStd::DebugPrintA( sError.CStr() );
			if ( _psError ) {
				(*_psError) = sError;
			}
			return seErrors;
		}
		return LSG_SE_SUCCESS;
	}

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
	LSBOOL LSE_CALL CShaderBase::SetVertexShader( const char * _pcShader, uint32_t _ui32Length ) {
		if ( !_pcShader ) {
			return false;
		}
		uint32_t ui32Len = _ui32Length ? _ui32Length : CStd::StrLen( _pcShader );
		CStringList slCode;
		CIncludeSet isIncludes;
		CPreProc::LSG_MACROS mMacros;
		CString sError;
		if ( CompileFromMemory( reinterpret_cast<const uint8_t *>(_pcShader), ui32Len,
			NULL, slCode, 0,
			LSG_ST_VERTEX,
			isIncludes, mMacros, sError ) != LSG_SE_SUCCESS ) {
			CStd::DebugPrintA( sError.CStr() );
			return false;
		}
		m_sVertexShader = slCode.ToString( true );
		m_sVertexShader.Snap();
		return true;
	}

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
	LSBOOL LSE_CALL CShaderBase::SetPixelShader( const char * _pcShader, uint32_t _ui32Length ) {
		if ( !_pcShader ) {
			return false;
		}
		uint32_t ui32Len = _ui32Length ? _ui32Length : CStd::StrLen( _pcShader );
		CStringList slCode;
		CIncludeSet isIncludes;
		CPreProc::LSG_MACROS mMacros;
		CString sError;
		if ( CompileFromMemory( reinterpret_cast<const uint8_t *>(_pcShader), ui32Len,
			NULL, slCode, 0,
			LSG_ST_PIXEL,
			isIncludes, mMacros, sError ) != LSG_SE_SUCCESS ) {
			CStd::DebugPrintA( sError.CStr() );
			return false;
		}
		m_sPixelShader = slCode.ToString( true );
		m_sPixelShader.Snap();
		return true;
	}

	/**
	 * Load a shader from a file.  Whether the shader is vertex or pixel must be specified.  Proper
	 *	workflow is to load both the vertex and pixel shaders, then compile, then use.
	 *
	 * \param _pcPath The path to the file to compile.
	 * \param _stShaderType Type of the shader.
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::SetShaderFromFile( const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
		CString &_sError ) {
		if ( !_pcPath ) {
			_sError = "Input string must not be NULL.";
			return LSG_SE_INVALIDINPUT;
		}
		uint8_t * pui8Data;
		uintptr_t uiptrSize;
		char szBuffer[LSF_MAX_PATH];
		if ( !CFilesEx::LoadFileToMemory( CFileLib::ExpandResourcePath( _pcPath, szBuffer ), pui8Data, &uiptrSize ) ) {
			_sError += "File \"";
			_sError += _pcPath;
			_sError += "\" not found.";
			return LSG_SE_FILENOTFOUND;
		}
		LSG_SHADER_ERRORS seRet = SetShaderFromMemory( pui8Data, static_cast<uint32_t>(uiptrSize),
			_pcPath, _stShaderType, _sError );
		CFilesEx::FreeFile( pui8Data );
		return seRet;
	}

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
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::SetShaderFromMemory( const uint8_t * _pui8Src, uint32_t _ui32Length,
		const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
		CString &_sError ) {
		switch ( _stShaderType ) {
			case LSG_ST_PIXEL : {}
			case LSG_ST_VERTEX : {
				break;
			}
			default : {
				return LSG_SE_INVALIDSHADERTYPE;
			}
		}
		// Put the file in line-by-line format, keeping the new-line characters.
		CStringList slFile;
		_sError.Reset();
		CPreProc::LSG_MACROS mMacros;
		CIncludeSet isIncludes;
		LSG_SHADER_ERRORS seRet = CompileFromMemory( _pui8Src, static_cast<uint32_t>(_ui32Length),
			_pcPath, slFile,
			0, _stShaderType, isIncludes, mMacros,
			_sError );
		if ( seRet != LSG_SE_SUCCESS ) { return seRet; }


		switch ( _stShaderType ) {
			case LSG_ST_PIXEL : {
				m_sPixelShader = slFile.ToString( true );
				m_sPixelShader.Snap();
				break;
			}
			case LSG_ST_VERTEX : {
				m_sVertexShader = slFile.ToString( true );
				m_sVertexShader.Snap();
				break;
			}
			default : {}
		}
		return LSG_SE_SUCCESS;
	}

	/**
	 * Sets preprocessor macros for a given shader type.
	 *
	 * \param _pdDefines The preprocessor macros.
	 * \param _ui32Total Total macros.
	 * \param _stShaderType The shader type to which to assign the macros.
	 * \return Returns true if there is enough memory to assign the macros.
	 */
	LSBOOL LSE_CALL CShaderBase::SetMacros( const LSG_DEFINE * _pdDefines, uint32_t _ui32Total, LSG_SHADER_TYPES _stShaderType ) {
		assert( _ui32Total <= 0xFFFF );
		m_vDefs[_stShaderType].Reset();
		if ( !m_vDefs[_stShaderType].Resize( _ui32Total ) ) { return false; }
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			m_vDefs[_stShaderType][I].sName = (*_pdDefines).pcMacro;
			m_vDefs[_stShaderType][I].sDef = (*_pdDefines++).pcDefinition;
			m_vDefs[_stShaderType][I].sName.Snap();
			m_vDefs[_stShaderType][I].sDef.Snap();
		}
		return true;
	}

	/**
	 * Called once prior to each render.
	 */
	void LSE_CALL CShaderBase::PreRender() {
	}

	/**
	 * Adds a folder to search for headers when #include and #import directives are reached in shader files.
	 *
	 * \param _pcPath The UTF-8 path of the folder to include in the search routine when seeking headers.
	 * \return Returns true if the path was added to the list of search paths.
	 */
	LSBOOL LSE_CALL CShaderBase::AddSearchPath( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CString sThis( CFileLib::ExpandResourcePath( _pcPath, szBuffer ) );
		sThis.FindAndReplaceChar( '\\', '/' );
		if ( sThis.GetLastChar() != '/' ) {
			if ( !sThis.Append( '/' ) ) {
				return false;
			}
		}

		if ( CAlgorithm::Find( m_slHeaderPaths, sThis ) == m_slHeaderPaths.Length() ) {
			return m_slHeaderPaths.Push( sThis );
		}
		return true;
	}

	/**
	 * Removes a folder from the paths to search for headers when #include and #import directives are
	 *	reached in shader files.
	 *
	 * \param _pcPath The UTF-8 path of the folder to remove from the search routine when seeking headers.
	 */
	void LSE_CALL CShaderBase::RemoveSearchPath( const char * _pcPath ) {
		char szBuffer[LSF_MAX_PATH];
		CString sThis( CFileLib::ExpandResourcePath( _pcPath, szBuffer ) );
		sThis.FindAndReplaceChar( '\\', '/' );
		if ( sThis.GetLastChar() != '/' ) {
			if ( !sThis.Append( '/' ) ) {
				return;
			}
		}

		uint32_t ui32Index = CAlgorithm::Find( m_slHeaderPaths, sThis );
		while ( ui32Index != m_slHeaderPaths.Length() ) {
			m_slHeaderPaths.Remove( ui32Index );
			ui32Index = CAlgorithm::Find( m_slHeaderPaths, sThis );
		}
	}

	/**
	 * Gets the total number of header search paths.
	 *
	 * \return Returns the number of header search paths.
	 */
	uint32_t LSE_CALL CShaderBase::TotalSearchPaths() {
		return m_slHeaderPaths.Length();
	}

	/**
	 * Gets a header search path by index.
	 *
	 * \param _ui32Index Index of the header search path to get.
	 * \return Returns a constant reference to the search path at the given index.
	 */
	const CString & LSE_CALL CShaderBase::GetSearchPath( uint32_t _ui32Index ) {
		return m_slHeaderPaths[_ui32Index];
	}

	/**
	 * Reset only the parser classes.  Retains the shader texts.
	 */
	void LSE_CALL CShaderBase::ResetParsers() {
		LSEDELETE m_pspwParser;
		m_pspwParser = NULL;

		LSEDELETE m_pslLexer;
		m_pslLexer = NULL;
	}

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
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::Compile( const char * _pcShader, uint32_t _ui32Length,
		LSG_SHADER_TYPES _stShaderType,
		CString &_sError ) {
		ResetParsers();

		CreateTranslator();
		_sError.Reset();
		if ( !_pcShader ) {
			_sError = "Input string must not be NULL.";
			return LSG_SE_INVALIDINPUT;
		}

		if ( !_ui32Length ) {
			_ui32Length = CStd::StrLen( _pcShader );
		}

		try {
			std::string strShaderText( _pcShader, _ui32Length );
			std::istringstream isStream( strShaderText );

			// Streams created.  Create the parsers.
			m_pslLexer = LSENEW CShaderLexer( &isStream, NULL );
			if ( !m_pslLexer ) {
				_sError = "Input string must not be NULL.";
				return LSG_SE_OUTOFMEMORY;
			}
			m_pspwParser = LSENEW CShaderParserWrapper( m_pslLexer, _stShaderType );
			if ( !m_pspwParser ) {
				_sError = "Input string must not be NULL.";
				return LSG_SE_OUTOFMEMORY;
			}
			m_pslLexer->SetContainer( m_pspwParser->GetContainer() );

			if ( m_pspwParser->parse() == 0 ) {
				CString * psFinal = NULL;
				if ( _stShaderType == LSG_ST_VERTEX ) {
					psFinal = &m_sFinalVertex;
				}
				else if ( _stShaderType == LSG_ST_PIXEL ) {
					psFinal = &m_sFinalPixel;
				}
				m_ecTranslationResult = m_pstpbTranslator->ProcessSyntaxTree( m_pspwParser->GetContainer(), (*psFinal), _sError );
				if ( m_ecTranslationResult == CShaderTranslationProcessorBase::LSG_EC_SUCCESS ) {
					if ( !m_pstpbTranslator->CopyGlobalSemantics( m_uslSemantics[_stShaderType] ) ) {
						_sError = "Not enough memory to copy global semantics.";
						return LSG_SE_OUTOFMEMORY;
					}
					if ( !m_pstpbTranslator->CopyGlobalUniforms( m_ulUniforms[_stShaderType] ) ) {
						_sError = "Not enough memory to copy global uniforms.";
						return LSG_SE_OUTOFMEMORY;
					}
					if ( !m_pstpbTranslator->CopyAttributes( m_ulAttributes[_stShaderType] ) ) {
						_sError = "Not enough memory to copy attributes.";
						return LSG_SE_OUTOFMEMORY;
					}
					return LSG_SE_SUCCESS;
				}
				else {
#ifdef _DEBUG
					CStd::DebugPrintA( _sError.CStr() );
					CStd::DebugPrintA( "\r\n" );
#endif	// #ifdef _DEBUG
					return LSG_SE_TRANSLATIONRERROR;
				}
			}
			else {
				_sError = "Syntax error.";
#ifdef _DEBUG
				CStd::DebugPrintA( _sError.CStr() );
				CStd::DebugPrintA( "\r\n" );

				CStd::DebugPrintA( "\r\n" );
				CStd::DebugPrintA( _pcShader );
				CStd::DebugPrintA( "\r\n" );
				CStd::DebugPrintA( "\r\n" );
#endif	// #ifdef _DEBUG
				return LSG_SE_PARSERERROR;
			}
		}
		catch ( ... ) {
			_sError = "Stream error.";
#ifdef _DEBUG
			CStd::DebugPrintA( _sError.CStr() );
			CStd::DebugPrintA( "\r\n" );
#endif	// #ifdef _DEBUG
			return LSG_SE_STREAMERROR;
		}
	}

	/**
	 * Compile a shader from a file.  The path must be in UTF-8 format.  The file data is treated
	 *	as UTF-8.
	 *
	 * \param _pcPath The path to the file to compile.
	 * \param _stShaderType Type of the shader.
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::CompileFromFile( const char * _pcPath, LSG_SHADER_TYPES _stShaderType,
		CString &_sError ) {
		// Put the file in line-by-line format, keeping the new-line characters.
		CStringList slFile;
		_sError.Reset();
		CIncludeSet isIncludes;
		CPreProc::LSG_MACROS mMacros;
		LSG_SHADER_ERRORS seRet = CompileFromFile( _pcPath, slFile, 0, _stShaderType, isIncludes, mMacros, _sError );
		if ( seRet != LSG_SE_SUCCESS ) { return seRet; }

		CString sFullFile = slFile.ToString( true );
		return Compile( sFullFile.CStr(), sFullFile.Length(), _stShaderType, _sError );
	}

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
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::CompileFromFile( const char * _pcPath, CStringList &_slData, uint32_t _ui32Line,
		LSG_SHADER_TYPES _stShaderType,
		CIncludeSet &_isIncludes,
		CPreProc::LSG_MACROS &_mMacros,
		CString &_sError ) {
		uint8_t * pui8Data;
		uintptr_t uiptrSize;
		char szBuffer[LSF_MAX_PATH];
		if ( !CFilesEx::LoadFileToMemory( CFileLib::ExpandResourcePath( _pcPath, szBuffer ), pui8Data, &uiptrSize ) ) {
			_sError += "File \"";
			_sError += _pcPath;
			_sError += "\" not found.";
			return LSG_SE_FILENOTFOUND;
		}
		LSG_SHADER_ERRORS seRet = CompileFromMemory( pui8Data, static_cast<uint32_t>(uiptrSize),
			_pcPath, _slData, _ui32Line,
			_stShaderType,
			_isIncludes,
			_mMacros,
			_sError );
		CFilesEx::FreeFile( pui8Data );
		return seRet;
	}

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
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::CompileFromMemory( const uint8_t * _pui8Src, uint32_t _ui32Length,
		const char * _pcPath, CStringList &_slData, uint32_t _ui32Line,
		LSG_SHADER_TYPES _stShaderType,
		CIncludeSet &_isIncludes,
		CPreProc::LSG_MACROS &_mMacros,
		CString &_sError ) {
		CStringList slThisFile;
		// There is a new-line constant approximately one in every 10 characters.  To avoid a divide we shift, dividing by
		//	16 instead.
		slThisFile.Allocate( _ui32Length );
		if ( !_pcPath ) { _pcPath = "<no source file>"; }

		for ( uint32_t I = 0; I < m_vDefs[_stShaderType].Length(); ++I ) {
			CString sThis = CString( "#define " ) + m_vDefs[_stShaderType][I].sName + CString( " " ) + m_vDefs[_stShaderType][I].sDef;
			if ( !slThisFile.Push( sThis ) ) {
				_sError += "Out of memory while loading file \"";
				_sError += _pcPath;
				_sError += "\".\r\n";
				return LSG_SE_OUTOFMEMORY;
			}
		}

		CString sFileName = _pcPath;
		sFileName.FindAndReplaceChar( '\\', '/' );
		sFileName.FindAndReplaceChar( '\"', '\'' );
#if defined( LSG_OGL )
		slThisFile.Push( CString( "#define LSE_OGL" ) );
#elif defined( LSG_OGLES )
		slThisFile.Push( CString( "#define LSE_OGLES" ) );
#elif defined( LSG_DX9 )
		slThisFile.Push( CString( "#define LSE_DX9" ) );
#elif defined( LSG_DX11 )
		slThisFile.Push( CString( "#define LSE_DX11" ) );
#endif	// #if defined( LSG_OPENGL )

#if defined( LSG_OGLES )
		slThisFile.Push( CString( "#define LSE_EPSILON 0.001" ) );
#else
		slThisFile.Push( CString( "#define LSE_EPSILON 0.00001" ) );
#endif	// #if defined( LSG_OGLES )

		slThisFile.Push( CString( "#define LSE_MAX_LIGHTS " ) + CString::CreateFromUI64( LSG_MAX_LIGHTS ) );
		slThisFile.Push( CString( "#define LSE_MAX_TEXTURE_UNITS " ) +
			CString::CreateFromUI64( CGfx::GetMetrics().ui32MaxTexSlot ) );
		if ( CGfx::GetMetrics().bDepthIsReadable ) {
			slThisFile.Push( CString( "#define LSE_DEPTH_IS_READABLE" ) );
		}



		slThisFile.Push( CString( "#line \"" ) + sFileName + CString( "\" 1" ) );
		uint32_t ui32HeaderLen = slThisFile.Length();
		if ( !slThisFile.MakeFromUtf8( reinterpret_cast<const char *>(_pui8Src), _ui32Length ) ) {
			_sError += "Out of memory while loading file \"";
			_sError += _pcPath;
			_sError += "\".";
			return LSG_SE_OUTOFMEMORY;
		}
		slThisFile.Push( "" );

		char szBuffer[16];

#define LSG_ERROR_ON_LINE			_sError += ": \""; _sError += _pcPath; _sError += "\":"; _sError += CStd::UI32toA( ui32Line - ui32HeaderLen + 1, szBuffer ); _sError += ".\r\n"


		CIncludeSet isTheseIncludes( _isIncludes );

		RemoveComments( slThisFile );
		uint32_t ui32Total = slThisFile.Length();

		// Scan the file and put multi-line directives in one line.
		// For example:
		// #include \
		//	"File.h"
		// Becomes:
		// #include		"File.h"
		for ( uint32_t I = 0; I < ui32Total; ++I ) {
			const CString & sThisLine = slThisFile[I];
			// Skip past opening whitespace.
			char cFirstChar = '\0';
			for ( uint32_t J = 0; J < sThisLine.Length(); ++J ) {
				if ( !CStd::IsWhitespace( sThisLine.CStr()[J] ) ) {
					cFirstChar = sThisLine.CStr()[J];
					break;
				}
			}
			
			if ( cFirstChar == '#' ) {
				// Line is a directive.  For each following line that ends with \, move that
				//	line to the end of this one.
				for ( uint32_t J = I + 1; J < ui32Total; ++J ) {
					char cLast = '\0';
					for ( uint32_t K = slThisFile[I].Length() - 1; K--; ) {
						cLast = slThisFile[I].CStr()[K];
						if ( cLast != '\r' && cLast != '\n' ) { break; }
					}
					if ( cLast == '\\' ) {
						// This line ends with \ so the next line moves to this one.
						while ( slThisFile[I].GetLastChar() != '\\' ) { slThisFile[I].RemLastChar(); }
						slThisFile[I].RemLastChar();	// Removes the \.
						if ( !slThisFile[I].Append( slThisFile[J] ) ) {
							_sError += "Out of memory while loading file \"";
							_sError += _pcPath;
							_sError += "\".\r\n";
							return LSG_SE_OUTOFMEMORY;
						}
						slThisFile[J].Reset();	// Simply clear the line.  Don't remove it from the array of lines.
						
					}
					else {
						break;
					}
				}
			}
		}

		// Scan each line looking for #include and #import directives.
		uint32_t ui32Line = 0;
		uint32_t ui32IfDefDepth = 0;
		
		CVectorPoD<LSG_CLEAR_STATE> vClearLines;

		CPreProc::LSG_PREPROC_DEFINE pdFile;
		pdFile.sName = "__FILE__";
		CPreProc::LSG_PREPROC_DEFINE pdLine;
		pdLine.sName = "__LINE__";
#if defined( LSG_OGLES )
		CPreProc::LSG_PREPROC_DEFINE pdOglEs;
		pdOglEs.sName = "GL_ES";
		_mMacros[pdOglEs] = "1";
#endif	// #if defined( LSG_OGLES )
		for ( uint32_t I = 0; I < ui32Total; ++I, ++ui32Line ) {
			uint32_t ui32Index = I + (slThisFile.Length() - ui32Total);
			CString sThisLine = slThisFile[ui32Index];
			sThisLine.RemoveWhitespace();
			if ( sThisLine.Length() ) {
				
				_mMacros[pdFile] = _pcPath;
				_mMacros[pdLine] = CString::CreateFromUI64( ui32Line - ui32HeaderLen + 1 );
				if ( sThisLine.CStr()[0] == '#' ) {
					sThisLine.RemChar( 0 );
					sThisLine.RemoveWhitespace();
					CString sDirective = sThisLine.GetToken( ' ', 0 );
					// Also get the length of the directive.
					uint32_t ui32AfterDir = 0;
					while ( ui32AfterDir < sThisLine.Length() && !CStd::IsWhitespace( sThisLine.CStr()[ui32AfterDir] ) ) { ui32AfterDir++; }
					while ( ui32AfterDir < sThisLine.Length() && CStd::IsWhitespace( sThisLine.CStr()[ui32AfterDir] ) ) { ui32AfterDir++; }

					if ( sDirective == "if" ) {
						vClearLines.Push( LSG_CS_NONE );
						CString sIf = &sThisLine.CStr()[ui32AfterDir];
						CPreProc ppPreProc;
						CPreProc::LSG_ERRORS eError = ppPreProc.Parse( sIf );
						if ( eError == CPreProc::LSG_E_ERROR ) {
							_sError += "Invalid #if directive";
							LSG_ERROR_ON_LINE;
							return LSG_SE_SYNTAX;
						}
						LSBOOL bRes;
						if ( !ppPreProc.GetResult( _mMacros, bRes ) ) {
							_sError += "Invalid #if directive";
							LSG_ERROR_ON_LINE;
							return LSG_SE_SYNTAX;
						}

						if ( !bRes ) {
							vClearLines[ui32IfDefDepth] = LSG_CS_CLEAR;
						}
						slThisFile[ui32Index].Reset();
						++ui32IfDefDepth;
					}
					else if ( sDirective == "ifdef" ) {
						vClearLines.Push( LSG_CS_NONE );
						CPreProc::LSG_PREPROC_DEFINE pdThis;
						pdThis.sName = &sThisLine.CStr()[ui32AfterDir];
						pdThis.sName.RemoveWhitespace();

						vClearLines[ui32IfDefDepth] = (!_mMacros.ItemExists( pdThis )) ? LSG_CS_CLEAR : LSG_CS_NONE;
						slThisFile[ui32Index].Reset();
						++ui32IfDefDepth;
					}
					else if ( sDirective == "ifndef" ) {
						vClearLines.Push( LSG_CS_NONE );
						CPreProc::LSG_PREPROC_DEFINE pdThis;
						pdThis.sName = &sThisLine.CStr()[ui32AfterDir];
						pdThis.sName.RemoveWhitespace();

						vClearLines[ui32IfDefDepth] = (_mMacros.ItemExists( pdThis )) ? LSG_CS_CLEAR : LSG_CS_NONE;
						slThisFile[ui32Index].Reset();
						++ui32IfDefDepth;
					}
					else if ( sDirective == "else" ) {
						slThisFile[ui32Index].Reset();
						if ( vClearLines[ui32IfDefDepth-1] == LSG_CS_NONE ) {
							// Previous #if was true, so scan to the matching #endif.
							vClearLines[ui32IfDefDepth-1] = LSG_CS_SEEKING_ENDIF;
						}
						else if ( vClearLines[ui32IfDefDepth-1] == LSG_CS_CLEAR ) {
							// Previous #if was false, so this area is true.
							vClearLines[ui32IfDefDepth-1] = LSG_CS_NONE;
						}
					}
					else if ( sDirective == "elif" ) {
						slThisFile[ui32Index].Reset();
						if ( vClearLines[ui32IfDefDepth-1] == LSG_CS_NONE ) {
							// Previous #if was true, so scan to the matching #endif.
							vClearLines[ui32IfDefDepth-1] = LSG_CS_SEEKING_ENDIF;
						}
						else if ( vClearLines[ui32IfDefDepth-1] == LSG_CS_CLEAR ) {
							CString sIf = &sThisLine.CStr()[ui32AfterDir];
							CPreProc ppPreProc;
							CPreProc::LSG_ERRORS eError = ppPreProc.Parse( sIf );
							if ( eError == CPreProc::LSG_E_ERROR ) {
								_sError += "Invalid #elif directive";
								LSG_ERROR_ON_LINE;
								return LSG_SE_SYNTAX;
							}
							LSBOOL bRes;
							if ( !ppPreProc.GetResult( _mMacros, bRes ) ) {
								_sError += "Invalid #elif directive";
								LSG_ERROR_ON_LINE;
								return LSG_SE_SYNTAX;
							}

							if ( !bRes ) {
								vClearLines[ui32IfDefDepth-1] = LSG_CS_CLEAR;
							}
							else {
								vClearLines[ui32IfDefDepth-1] = LSG_CS_NONE;
							}
							slThisFile[ui32Index].Reset();
						}
					}
					else if ( sDirective == "endif" ) {
						slThisFile[ui32Index].Reset();
						--ui32IfDefDepth;
						vClearLines.Pop();
					}
					if ( CAlgorithm::Find( vClearLines, LSG_CS_CLEAR ) != vClearLines.Length() || CAlgorithm::Find( vClearLines, LSG_CS_SEEKING_ENDIF ) != vClearLines.Length() ) {
						slThisFile[ui32Index].Reset();
					}
					else {
						if ( sDirective == "define" ) {
							CString sDefine = &sThisLine.CStr()[ui32AfterDir];
							if ( !ParseDefine( sDefine, _mMacros ) ) {
								_sError += "Invalid #define directive";
								LSG_ERROR_ON_LINE;
								return LSG_SE_SYNTAX;
							}
							slThisFile[ui32Index].Reset();
						}
						else if ( sDirective == "undef" ) {
							CPreProc::LSG_PREPROC_DEFINE pdThis;
							pdThis.sName = &sThisLine.CStr()[ui32AfterDir];
							pdThis.sName.RemoveWhitespace();
							if ( !CStd::IsIdentifier( pdThis.sName.CStr() ) ) {
								_sError += "Invalid #undef directive";
								LSG_ERROR_ON_LINE;
								return LSG_SE_SYNTAX;
							}
							_mMacros.Remove( pdThis );
							slThisFile[ui32Index].Reset();
						}
						else if ( sDirective == "error" ) {
							CString sMessage = &sThisLine.CStr()[ui32AfterDir];
							if ( !ExpandMacros( sMessage, _mMacros, _sError ) ) {
								LSG_ERROR_ON_LINE;
								return LSG_SE_PREPROC;
							}
							_sError += sMessage;
							LSG_ERROR_ON_LINE;
							return LSG_SE_PREPROC;
						}
						else if ( sDirective == "include" || sDirective == "import" ) {
							// Get the actual file to include.
							CString sFile = GetFilePart( sThisLine ).DecodeEscapeSequences();
							if ( !sFile.Length() ) {
								_sError += "Invalid #include directive";
								LSG_ERROR_ON_LINE;
								return LSG_SE_SYNTAX;
							}
							CString sActualFile = FindHeader( _pcPath, sFile );
							if ( sActualFile.Length() == 0 ) {
								_sError += "File \"";
								_sError += sFile;
								_sError += "\" not found";
								LSG_ERROR_ON_LINE;
								return LSG_SE_FILENOTFOUND;
							}

							// Remove this line and replace it with the contents of the file.
							slThisFile[ui32Index].Set( CString( "#line \"" ) + sFileName + CString( "\" " ) + CString::CreateFromUI64( ui32Line + 1 ) + CString( "\r\n" ));
							--I;
							--ui32Total;

							// Check if the file has been recursively included too much.
							uint32_t ui32IncCount = isTheseIncludes.ItemExists( sActualFile ) ? isTheseIncludes[sActualFile] : 0;
							isTheseIncludes[sActualFile] = ui32IncCount + 1;

							if ( sDirective == "import" && ui32IncCount > 0 ) {
								// No action taken.
							}
							else if ( sDirective == "include" && ui32IncCount > 5 ) {
								// Error.  Too many recursive includes.
								_sError += "File \"";
								_sError += sActualFile;
								_sError += "\" recursively included too many times";
								LSG_ERROR_ON_LINE;
								return LSG_SE_INCLUDERECURSE;
							}
							else {
								LSG_SHADER_ERRORS seError = CompileFromFile( sActualFile.CStr(), slThisFile, ui32Index, _stShaderType, isTheseIncludes, _mMacros, _sError );
								if ( seError != LSG_SE_SUCCESS ) {
									_sError += "\r\n\tFrom";
									LSG_ERROR_ON_LINE;
									return seError;
								}
							}
						}
					}
				}
				else if ( CAlgorithm::Find( vClearLines, LSG_CS_CLEAR ) != vClearLines.Length() || CAlgorithm::Find( vClearLines, LSG_CS_SEEKING_ENDIF ) != vClearLines.Length() ) {
					slThisFile[ui32Index].Reset();
				}
				else {
					if ( !ExpandMacros( slThisFile[ui32Index], _mMacros, _sError ) ) {
						LSG_ERROR_ON_LINE;
						return LSG_SE_PREPROC;
					}
				}
			}
		}

		if ( vClearLines.Length() != 0 ) {
			_sError += "End of file reached before finding a closing #endif in file \"";
			_sError += _pcPath;
			_sError += "\".\r\n";
			return LSG_SE_NOENDIF;
		}


		// Insert the lines into the given string list.
		if ( !_slData.Allocate( _slData.Length() + slThisFile.Length() ) ) {
			_sError += "Out of memory while loading file \"";
			_sError += _pcPath;
			_sError += "\".\r\n";
			return LSG_SE_OUTOFMEMORY;
		}
		for ( uint32_t I = slThisFile.Length(); I--; ) {
			_slData.Insert( slThisFile[I], _ui32Line );
		}

		/*CString sPrint = slThisFile.ToString( true );
		CStd::DebugPrintA( "*********************************\r\n*********************************\r\n*********************************\r\n" );
		CStd::DebugPrintA( sPrint.CStr() );
		CStd::DebugPrintA( "*********************************\r\n*********************************\r\n*********************************\r\n" );*/

		return LSG_SE_SUCCESS;

#undef LSG_ERROR_ON_LINE
	}

	/**
	 * Post compile.  If all shaders are compiled successfully, this is called, meant to be overridden by the
	 *	classes that specialize towards specific graphics API's.
	 *
	 * \param _sError The returned error string if there is an error.
	 * \return Returns an error code or LSG_SE_SUCCESS.
	 */
	LSG_SHADER_ERRORS LSE_CALL CShaderBase::PostCompile( CString &/*_sError*/ ) {
		return LSG_SE_SUCCESS;
	}

	/**
	 * Gets the file part of an #include or #import directive.
	 *
	 * \param _sDirective The line to scan for the file name.
	 * \return Returns the file path part of the line or an empty string indicating a syntax error.
	 */
	CString LSE_CALL CShaderBase::GetFilePart( const CString &_sDirective ) {
		CString sRet;
		// Scan for the first ", but skip /**/ and // comments.
		LSBOOL bInString = false;
		LSBOOL bInComment = false;
		uint32_t ui32Len;
		uint32_t ui32LastChar = '\0';
		for ( uint32_t I = 0; I < _sDirective.Length(); I += ui32Len ) {
			uint32_t ui32Char = CStd::NextUtf8Char( reinterpret_cast<const LSUTF8 *>(&_sDirective.CStr()[I]), _sDirective.Length() - I, &ui32Len );
			if ( bInComment ) {
				// Inside a /**/-style comment.  Override all other paths until a */ is found.
				if ( ui32LastChar == '*' && ui32Char == '/' ) {
					bInComment = false;
				}
			}
			else if ( !bInString ) {
				if ( ui32Char == '/' ) {
					// Not in a string.  This could be a comment.
					if ( ui32LastChar == '/' ) {
						// //-style comment.  Nothing lies beyond this in the string so error.
						return sRet;
					}
				}
				else if ( ui32Char == '*' ) {
					// Not in a string.  This could be a comment.
					if ( ui32LastChar == '/' ) {
						// /**/-style comment.
						// Skip over it.
						bInComment = true;
					}
				}
				// Not in a comment and not in a string.  The first " character begins a string.
				else if ( ui32Char == '\"' ) {
					bInString = true;
				}
			}
			else if ( bInString ) {
				// In a string.
				//	Add characters until the closing " is found.
				if ( ui32Char == '\\' && ui32LastChar == '\\' ) {
					// A little trick to avoid a pit-hole when trying to detect escaped \" in our simple fashion.
					//	Without this, \\" would incorrectly continue the string.
					ui32LastChar = '\0';
				}
				else if ( ui32Char == '\"' ) {
					if ( ui32LastChar == '\\' ) {
						// An escaped quote.  Add it.
						if ( !sRet.Append( '\\' ) ) {
							sRet.Reset();
							return sRet;
						}
						if ( !sRet.Append( '\"' ) ) {
							sRet.Reset();
							return sRet;
						}
					}
					else {
						// The ending quote.  Return.
						return sRet;
					}
				}
				else {
					// Any other character.
					if ( !sRet.Append( &_sDirective.CStr()[I], ui32Len ) ) {
						sRet.Reset();
						return sRet;
					}
				}
			}
			ui32LastChar = ui32Char;
		}

		// If we got here it means the directive was improperly formatted.
		sRet.Reset();
		return sRet;
	}

	/**
	 * Find a header file.  Searches first the relative path from the main includer, then each of the search paths
	 *	set by the engine user.
	 *
	 * \param _pcIncluder Path to the file that is including the 
	 * \param _sFileToInclude The file to include.
	 * \return Returns the path to the file to include or an empty string if the file could not be found.
	 */
	CString LSE_CALL CShaderBase::FindHeader( const char * _pcIncluder, const CString &_sFileToInclude ) {
		char szBuffer[LSF_MAX_PATH];
		CStd::StrCpy( szBuffer, _pcIncluder );
		CString sPath = CFileLib::NoFile( szBuffer );
		if ( !sPath.Append( _sFileToInclude ) ) { return CString(); }
		CFileLib::ExpandResourcePath( sPath.CStr(), szBuffer );
		if ( CFileLib::Exists( szBuffer ) ) { return szBuffer; }

		for ( uint32_t I = 0; I < TotalSearchPaths(); ++I ) {
			sPath = GetSearchPath( I );
			if ( !sPath.Append( _sFileToInclude ) ) { return CString(); }

			CFileLib::ExpandResourcePath( sPath.CStr(), szBuffer );
			if ( CFileLib::Exists( szBuffer ) ) { return szBuffer; }
		}
		return CString();
	}

	/**
	 * Handles a #define.
	 *
	 * \param _sLine The text after "#define".  Parsed to determin the name, parameters, and definition of a macro.
	 * \param _mMacros Set of macros to which to add the new macro.
	 * \return Returns true if the text is valid and the macro was added.
	 */
	LSBOOL LSE_CALL CShaderBase::ParseDefine( const CString &_sLine, CPreProc::LSG_MACROS &_mMacros ) {
		CPreProc::LSG_PREPROC_DEFINE pdDefine;
		uint32_t ui32Pos = 0;
		for ( ; ui32Pos < _sLine.Length(); ++ui32Pos ) {
			if ( CStd::IsWhitespace( _sLine.CStr()[ui32Pos] ) || _sLine.CStr()[ui32Pos] == '(' ) { break; }
			if ( !pdDefine.sName.Append( _sLine.CStr()[ui32Pos] ) ) { return false; }
		}
		if ( !pdDefine.sName.Length() ) { return false; }

		// Must be an identifier.
		if ( !CStd::IsIdentifier( pdDefine.sName.CStr() ) ) { return false; }

		// Optionally add parameters.
		if ( _sLine.CStr()[ui32Pos] == '(' ) {
			// Get parameters.
			CString sParm;
			for ( ++ui32Pos; ui32Pos < _sLine.Length(); ++ui32Pos ) {
				if ( CStd::IsWhitespace( _sLine.CStr()[ui32Pos] ) ) { continue; }
				if ( _sLine.CStr()[ui32Pos] == ',' || _sLine.CStr()[ui32Pos] == ')' ) {
					if ( !CStd::IsIdentifier( sParm.CStr() ) ) { return false; }
					if ( !pdDefine.sParms.Push( sParm ) ) { return false; }
					sParm.ResetNoDealloc();
				}
				else {
					if ( !sParm.Append( _sLine.CStr()[ui32Pos] ) ) { return false; }
				}
				if ( _sLine.CStr()[ui32Pos] == ')' ) { ++ui32Pos; break; }
			}
		}

		// Skip to the definition.
		while ( CStd::IsWhitespace( _sLine.CStr()[ui32Pos] ) ) { ++ui32Pos; }

		_mMacros[pdDefine] = &_sLine.CStr()[ui32Pos];
		return true;
	}

	/**
	 * Removes comments from code.
	 *
	 * \param _slText The text from which to remove comments.
	 */
	void LSE_CALL CShaderBase::RemoveComments( CStringList &_slText ) {
		for ( uint32_t I = 0; I < _slText.Length(); ++I ) {
			uint32_t ui32Len = _slText[I].Length();
			if ( ui32Len < 2 ) { continue; }
			--ui32Len;
			for ( uint32_t J = 0; J < ui32Len; ++J ) {
				if ( I == _slText.Length() ) {
					return;
				}
				SkipString( _slText[I], J );
				if ( J == _slText[I].Length() ) { continue; }

				// Check for //-style comments.
				char cThis = _slText[I].CStr()[J];
				char cNext = _slText[I].CStr()[J+1];
				if ( cThis == '/' && cNext == '/' ) {
					// Chop the line here.
					_slText[I].RemChars( J, _slText[I].Length() - J );
					break;
				}
				else if ( cThis == '/' && cNext == '*' ) {
					const_cast<char *>(_slText[I].CStr())[J++] = ' ';	// Remove /.
					const_cast<char *>(_slText[I].CStr())[J++] = ' ';	// Remove *.
					LSBOOL bFound = false;
					for ( ; I < _slText.Length() && !bFound; ++I ) {
						for ( ; J < _slText[I].Length(); ++J ) {
							if ( J < _slText[I].Length() - 1 ) {
								cThis = _slText[I].CStr()[J];
								cNext = _slText[I].CStr()[J+1];
								if ( cThis == '*' && cNext == '/' ) {
									const_cast<char *>(_slText[I].CStr())[J++] = ' ';	// Remove *.
									const_cast<char *>(_slText[I].CStr())[J++] = ' ';	// Remove /.
									bFound = true;
									break;
								}
							}
							const_cast<char *>(_slText[I].CStr())[J] = ' ';
						}
						if ( bFound ) {
							ui32Len = _slText[I].Length() - 1;
							break;
						}
						J = 0;
					}
				}
			}
		}
	}

	/**
	 * Skip strings in code.
	 *
	 * \param _sString The string containing the string to skip.
	 * \param _ui32Pos The position of the string to skip.  Upon returning, this holds the new position from
	 *	where parsing can continue.
	 */
	void LSE_CALL CShaderBase::SkipString( const CString &_sLine, uint32_t &_ui32Pos ) {
		char cStringType = _sLine.CStr()[_ui32Pos];
		if ( cStringType != '"' && cStringType != '\'' ) { return; }

		for ( _ui32Pos++; _ui32Pos < _sLine.Length(); ++_ui32Pos ) {
			char cThis = _sLine.CStr()[_ui32Pos];
			if ( cThis == '\\' ) {
				// Skip the next character.				
				++_ui32Pos;
				continue;
			}
			// In every other case, check for the terminator.
			if ( cThis == cStringType ) {
				++_ui32Pos;
				return;
			}
		}
	}

	/**
	 * Replace all macros in a given string.
	 *
	 * \param _sString The string in which to expand macros.
	 * \param _mMacros The macro dictionary.
	 * \param _sError The error string if the return is false.
	 * \return Returns true if the expansion succeeds.  Otherwise filles _sError with the error string and returns false.
	 */
	LSBOOL LSE_CALL CShaderBase::ExpandMacros( CString &_sString, const CPreProc::LSG_MACROS &_mMacros, CString &_sError ) {
		CSet<CString> sUsedStrings;
		return ExpandMacros( _sString, _mMacros, sUsedStrings, _sError );
	}

	/**
	 * Replace all macros in a given string.
	 *
	 * \param _sString The string in which to expand macros.
	 * \param _mMacros The macro dictionary.
	 * \param _sUsedValues Values that have already been expanded.
	 * \param _sError The error string if the return is false.
	 * \return Returns true if the expansion succeeds.  Otherwise filles _sError with the error string and returns false.
	 */
	LSBOOL LSE_CALL CShaderBase::ExpandMacros( CString &_sString, const CPreProc::LSG_MACROS &_mMacros, const CSet<CString> &_sUsedValues, CString &_sError ) {
		CString sTemp;
		CString::LSSTD_RANGE rRange = {
			0,
			0
		};
		uint32_t ui32OriginalLen = _sString.Length();
		for ( uint32_t I = 0; I <= ui32OriginalLen; ++I ) {
			// Adjust for changes in length in the source string.
			uint32_t ui32Index = _sString.Length() - ui32OriginalLen + I;
			ui32Index = CStd::Min<uint32_t>( ui32Index, _sString.Length() - 1 );
			if ( rRange.ui32Start == 0xFFFFFFFFUL ) {
				rRange.ui32Start = rRange.ui32End = ui32Index;
			}

			char cThis = _sString.CStr()[ui32Index];
			if ( I != ui32OriginalLen && (CStd::IsAlNum( cThis ) || cThis == '_') ) {
				sTemp.Append( cThis );
				++rRange.ui32End;
			}
			else {
				if ( sTemp.Length() ) {
					if ( CStd::IsIdentifier( sTemp.CStr() ) ) {
						// Found an identifier.
						// Has it been replaced yet?
						if ( !_sUsedValues.ItemExists( sTemp ) ) {
							// Have not replaced this value yet.
							// Is it a macro?
							CPreProc::LSG_PREPROC_DEFINE pdTestMe;
							pdTestMe.sName = sTemp;
							uint32_t ui32MacroIndex;
							if ( _mMacros.ItemExists( pdTestMe, ui32MacroIndex ) ) {
								// It is a macro and needs to be replaced.
								// Is it a function-style macro?
								if ( _mMacros.GetKeyByIndex( ui32MacroIndex ).sParms.Length() ) {
									// Need to gather parameters.
									CVector<CString> vParms;
									uint32_t ui32PosAfter;
									if ( GetMacroParms( _sString, ui32Index, ui32PosAfter, vParms ) ) {
										CString sReplaced;
										if ( !GetMacroReplacementString( ui32MacroIndex, vParms, _mMacros, _sUsedValues, sReplaced, _sError ) ) {
										}
										else {
											rRange.ui32End += ui32PosAfter - ui32Index;	// Include the parameters in what is replaced.
											I += ui32PosAfter - ui32Index - 1;
											_sString.Replace( rRange, sReplaced.CStr(), sReplaced.Length() );
											sTemp.ResetNoDealloc();
											rRange.ui32Start = 0xFFFFFFFFUL;
											continue;
										}
									}
								}
								else {
									// Not a function-style macro.
									CString sReplaced;
									CVector<CString> vParms;
									if ( !GetMacroReplacementString( ui32MacroIndex, vParms, _mMacros, _sUsedValues, sReplaced, _sError ) ) {
									}
									else {
										_sString.Replace( rRange, sReplaced.CStr(), sReplaced.Length() );
										sTemp.ResetNoDealloc();
										rRange.ui32Start = 0xFFFFFFFFUL;
										continue;
									}
								}
							}
						}
					}
				}
				// Skip strings.
				uint32_t ui32IndexBefore = ui32Index;
				SkipString( _sString, ui32Index );

				// Account for changes that may have been made to ui32Index.
				I += ui32Index - ui32IndexBefore;

				sTemp.ResetNoDealloc();
				rRange.ui32Start = 0xFFFFFFFFUL;
			}
		}

		return true;
	}

	/**
	 * Gets the parameters for a function-style macro.  If no parameters are given, returns false.
	 *
	 * \param _psString The string from which to extract function-style macro parameters.
	 * \param _ui32Pos The position at which to begin the extraction.
	 * \param _ui32NewPos The position after successful extraction.
	 * \param _vRet The returned parameters, as strings.
	 * \return Returns true if the parameters were extracted.  False indicates no parameters present or an invalid parameter string.
	 */
	LSBOOL LSE_CALL CShaderBase::GetMacroParms( const CString &_sString, uint32_t _ui32Pos, uint32_t &_ui32NewPos, CVector<CString> &_vRet ) {
		_ui32NewPos = _ui32Pos;
		// Skip whitespace.
		while ( _ui32NewPos < _sString.Length() && CStd::IsWhitespace( _sString.CStr()[_ui32NewPos] ) ) { ++_ui32NewPos; }
		// Next character must be an opening brace.
		if ( _ui32NewPos == _sString.Length() ) { return false; }
		if ( _sString.CStr()[_ui32NewPos] != '(' ) { return false; }


		uint32_t ui32ParDepth = 1;
		CString sCur;
		for ( ++_ui32NewPos; _ui32NewPos < _sString.Length(); ++_ui32NewPos ) {
			char cThis = _sString.CStr()[_ui32NewPos];
			if ( cThis == ',' && ui32ParDepth == 1 ) {
				if ( !sCur.Length() ) { return false; }
				sCur.RemoveWhitespace();
				_vRet.Push( sCur );
				sCur.ResetNoDealloc();
			}
			else {
				
				sCur.Append( cThis );
				if ( cThis == '(' ) {
					++ui32ParDepth;
				}
				else if ( cThis == ')' ) {
					--ui32ParDepth;
					if ( ui32ParDepth == 0 ) {
						sCur.RemLastChar();
						sCur.RemoveWhitespace();
						_vRet.Push( sCur );
						sCur.ResetNoDealloc();
						++_ui32NewPos;
						return true;
					}
				}
			}
		}
		// No closing ) found.
		return false;
	}

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
	LSBOOL LSE_CALL CShaderBase::GetMacroReplacementString( uint32_t _ui32MacroIndex, CVector<CString> &_vParms,
		const CPreProc::LSG_MACROS &_mMacros, const CSet<CString> &_sUsedValues,
		CString &_sRet,
		CString &_sError ) {

		if ( _mMacros.GetKeyByIndex( _ui32MacroIndex ).sParms.Length() != _vParms.Length() ) {
			_sError += CString( "Macro " ) + _mMacros.GetKeyByIndex( _ui32MacroIndex ).sName +
				CString( " takes " ) + CString::CreateFromUI64( _mMacros.GetKeyByIndex( _ui32MacroIndex ).sParms.Length() ) + CString( " arguments (was given " ) +
				CString::CreateFromUI64( _vParms.Length() ) + CString( ")" );
			return false;
		}

		// Do the primary translation.
		_sRet = _mMacros.GetByIndex( _ui32MacroIndex );

		// Within the created string, replace parameters.
		CString sTemp;
		CString::LSSTD_RANGE rRange = {
			0,
			0
		};
		uint32_t ui32OriginalLen = _sRet.Length();
		for ( uint32_t I = 0; I <= ui32OriginalLen; ++I ) {
			// Adjust for changes in length in the source string.
			uint32_t ui32Index = _sRet.Length() - ui32OriginalLen + I;
			ui32Index = CStd::Min<uint32_t>( ui32Index, _sRet.Length() - 1 );
			if ( rRange.ui32Start == 0xFFFFFFFFUL ) {
				rRange.ui32Start = rRange.ui32End = ui32Index;
			}

			char cThis = _sRet.CStr()[ui32Index];
			if ( I != ui32OriginalLen && (CStd::IsAlNum( cThis ) || cThis == '_') ) {
				sTemp.Append( cThis );
				++rRange.ui32End;
			}
			else {
				if ( sTemp.Length() ) {
					if ( CStd::IsIdentifier( sTemp.CStr() ) ) {
						// It is an identifier.  Is it a parameter?
						uint32_t ui32IndexOfParm = CAlgorithm::Find( _mMacros.GetKeyByIndex( _ui32MacroIndex ).sParms, sTemp );
						if ( ui32IndexOfParm != _vParms.Length() ) {
							// It is a parameter.
							//	Replace it within the string.
							_sRet.Replace( rRange, _vParms[ui32IndexOfParm].CStr(), _vParms[ui32IndexOfParm].Length() );
						}
					}
				}
				sTemp.ResetNoDealloc();
				rRange.ui32Start = 0xFFFFFFFFUL;
			}
		}

		// Parameters have been replaced.
		// Now continue expanding recursively.
		CSet<CString> sStringsReplaced;
		sStringsReplaced = _sUsedValues;
		sStringsReplaced.Insert( _mMacros.GetKeyByIndex( _ui32MacroIndex ).sName );
		ExpandMacros( _sRet, _mMacros, sStringsReplaced, _sError );
		return true;
	}

}	// namespace lsg
