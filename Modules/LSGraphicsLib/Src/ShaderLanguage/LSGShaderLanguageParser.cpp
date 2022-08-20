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
 * Description: Handles the meat of parsing a shader file.  Bison performs the actual parsing, but all nodes
 *	are sent to this class to be managed.  This is the go-to class for any information needed on a parsed shader.
 */

#include "LSGShaderLanguageParser.h"
#include "../Gfx/LSGGfx.h"
#include "LSGShaderLexer.h"
#include "LSGShaderParserWrapper.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )


namespace lsg {

	// == Operators.
	/**
	 * Assignment operator.
	 *
	 * \param _smSource The object to copy.
	 * \return Returns this object after the copy.
	 */
	CShaderLanguageParser::LSG_STRUCT_MEMBER & LSE_CALL CShaderLanguageParser::LSG_STRUCT_MEMBER::operator = ( const LSG_STRUCT_MEMBER &_smSource ) {
		sName = _smSource.sName;
		sName.Snap();
		ui64Type = _smSource.ui64Type;
		ui32Array = _smSource.ui32Array;
		return (*this);
	}

	// == Operators.
	/**
	 * Assignment operator.
	 *
	 * \param _sSource The object to copy.
	 * \return Returns this object after the copy.
	 */
	CShaderLanguageParser::LSG_STRUCT & LSE_CALL CShaderLanguageParser::LSG_STRUCT::operator = ( const LSG_STRUCT &_sSource ) {
		sName = _sSource.sName;
		sName.Snap();
		vMembers = _sSource.vMembers;
		return (*this);
	}

	/**
	 * Less-than operator.
	 *
	 * \param _sSource The object against which to compare.
	 * \return Returns the lexicographical relationship between the names of the two objects.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::LSG_STRUCT::operator < ( const LSG_STRUCT &_sSource ) {
		return sName < _sSource.sName;
	}

	// == Various constructors.
	LSE_CALLCTOR CShaderLanguageParser::CShaderLanguageParser( LSG_SHADER_TYPES _stType, CShaderLexer * _plLexer ) :
		m_stShaderType( _stType ),
		m_ui32CurFunc( LSE_MAXU32 ),
		m_plLexer( _plLexer ),
		m_ui32TypeId( LSG_SB_START ) {
	}

	// == Functions.
	/**
	 * Gets the shader type.
	 *
	 * \return Returns the shader type.
	 */
	LSG_SHADER_TYPES LSE_CALL CShaderLanguageParser::GetShaderType() const {
		return m_stShaderType;
	}

	/**
	 * Accepts a string from code in a shader, converts it to binary (for example, \r is
	 *	converted to character 0x0D), and returns the index of the string where it was stored
	 *	in the pool of strings.
	 *
	 * \param _pcText The string from the code to convert and store.
	 * \param _bHasQuotes If true, the first and last characters are assumed to be quotation marks and not part of the string itself.
	 * \return Returns the index of the stored string or LSE_MAXU32 on error.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::CreateString( const char * _pcText, LSBOOL _bHasQuotes ) {
		CString sString = _pcText;
		if ( sString.Length() == 0 ) { return LSE_MAXU32; }
		if ( _bHasQuotes ) {
			// Strip the quotes from the outsides.
			sString.RemLastChar();
			sString.RemChar( 0 );
		}


		// Convert the remaining string to binary and save.
		if ( !m_vStringPool.Push( sString.DecodeEscapeSequences() ) ) { return LSE_MAXU32; }
		return m_vStringPool.Length() - 1;
	}

	/**
	 * Accepts a string representation of a decimal constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateDecimal( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_NUMERICCONSTANT;

		CString sString = _pcText;
		if ( sString.Length() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = LSG_NC_INVALID;
			return false;
		}

		// If the last character is L, eat it.
		while ( CStd::ToLower( sString.GetLastChar() ) == 'l' ) {
			sString.RemLastChar();
		}

		// If the last character is a U, set to unsigned.
		if ( CStd::ToLower( sString.GetLastChar() ) == 'u' ) {
			sString.RemLastChar();
			_ndNode.nueNodeDataEx.ncConstantType = LSG_NC_UNSIGNED;
		}
		else {
			_ndNode.nueNodeDataEx.ncConstantType = LSG_NC_SIGNED;
		}

		_ndNode.nuNodeData.i64Const = CStd::AtoI64( sString.CStr() );

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a representation of a floating-point constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFloatingPoint( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_NUMERICCONSTANT;

		CString sString = _pcText;
		if ( sString.Length() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = LSG_NC_INVALID;
			return false;
		}

		_ndNode.nuNodeData.dConst = CStd::AtoF( _pcText );
		_ndNode.nueNodeDataEx.ncConstantType = LSG_NC_FLOATING;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a node that represents either boolean true or boolean false.
	 *
	 * \param _bValue The boolean value of the node.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateBool( LSBOOL _bValue, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_BOOLEAN;

		_ndNode.nuNodeData.ui32UnaryOp = _bValue;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a node pointing to an identifier (by index into the identifier stack).
	 *
	 * \param _pcText The string name of the identifier.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateIdentifier( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_IDENTIFIER;

		_ndNode.nuNodeData.ui32IdentifierIndex = AddIdentifier( _pcText );
		if ( _ndNode.nuNodeData.ui32IdentifierIndex == LSE_MAXU32 ) { return false; }

		return AddNode( _ndNode );
	}

	/**
	 * Creates an array-access node.
	 *
	 * \param _ndBase Base part of the expression.
	 * \param _ndIndex Array index part of the expression.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateArrayAccess( const YYSTYPE::LSG_NODE_DATA &_ndBase,
		const YYSTYPE::LSG_NODE_DATA &_ndIndex, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_ARRAYACCESS;
		_ndNode.nuNodeData.ui32NodeIndex = _ndBase.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndIndex.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a postfix or prefix ++ or -- node.
	 *
	 * \param _ndOp The left operand.
	 * \param _nType Type of operation.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreatePostfixOrPrefixIncOrDec( const YYSTYPE::LSG_NODE_DATA &_ndOp,
		LSG_NODE _nType, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = _nType;
		_ndNode.nuNodeData.ui32NodeIndex = _ndOp.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a unary !, ~, -, or + node.
	 *
	 * \param _ui32Op The operator.
	 * \param _ndBase The base expression modified by the operator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateUnaryOp( uint32_t _ui32Op, const YYSTYPE::LSG_NODE_DATA &_ndBase,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_UNARY;

		_ndNode.nuNodeData.ui32UnaryOp = _ui32Op;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBase.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a cast expression.
	 *
	 * \param _ui64Type The type to which to cast the expression.
	 * \param _ndBase The base expression.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateCast( uint64_t _ui64Type, const YYSTYPE::LSG_NODE_DATA &_ndBase,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_CAST;

		_ndNode.nuNodeData.ui64CastType = _ui64Type;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBase.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a node referencing two other nodes with an operator between them.  From X * Y to
	 *	X && Y.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ui32Op The operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateMath( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		uint32_t _ui32Op, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_MATH;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;
		_ndNode.nuoOp.ui32Op = _ui32Op;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a tertiary ?: expression.
	 *
	 * \param _ndCond The conditional operand.
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateTertiary( const YYSTYPE::LSG_NODE_DATA &_ndCond,
		const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_TERTIARY;
		_ndNode.nuNodeData.ui32NodeIndex = _ndCond.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndLeft.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an assignment node.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ui32Op The operator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateAssignment( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		uint32_t _ui32Op,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_ASSIGNMENT;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;
		_ndNode.nuoOp.ui32Op = _ui32Op;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a comma sequence.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateComma( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_COMMA;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an intrinsic function.
	 *
	 * \param _ui32Intrinsic The intrinsic to create.
	 * \param _ndExp The parameters for the intrinsic function.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateIntrinsic( uint32_t _ui32Intrinsic,
		const YYSTYPE::LSG_NODE_DATA &_ndExp,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_INTRINSIC;
		_ndNode.nuNodeData.ui32NodeIndex = _ui32Intrinsic;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndExp.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Make an argument expression list.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateArgExpList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_ARGLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a function-call node.
	 *
	 * \param _ndFunction The left operand.
	 * \param _ui32ArgList The index of the optional argument list.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFunctionCall( const YYSTYPE::LSG_NODE_DATA &_ndFunction,
		uint32_t _ui32ArgList,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FUNCTIONCALL;
		_ndNode.nuNodeData.ui32NodeIndex = _ndFunction.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ui32ArgList;

		if ( m_ui32CurFunc != LSE_MAXU32 ) {
			//CString sCurFunc = GetString( m_ui32CurFunc );
			const YYSTYPE::LSG_NODE_DATA & ndNode = GetNode( _ndNode.nuNodeData.ui32NodeIndex );
			if ( ndNode.nNodeType == LSG_N_IDENTIFIER ) {
				//CString sFuncName = GetString( GetIdentifier( ndNode.nuNodeData.ui32IdentifierIndex ).ui32Name );
				m_mmFuncRefs.Insert( m_ui32CurFunc, GetIdentifier( ndNode.nuNodeData.ui32IdentifierIndex ).ui32Name );
			}
		}

		return AddNode( _ndNode );
	}

	/**
	 * Creates a field-select node.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFieldSelect( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FIELDSELECT;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a constructor node.
	 *
	 * \param _ui64Type The type ID, including implicit arrays.
	 * \param _ui32Args Index of the argument-list node if any.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateConstructor( uint64_t _ui64Type,
		uint32_t _ui32Args,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_CONSTRUCTOR;
		_ndNode.nuNodeData.ui64Const = _ui64Type;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ui32Args;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an expression statement.
	 *
	 * \param _ui32Index Index of the expression or LSE_MAXU32
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateExpressionStatement( uint32_t _ui32Index,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_EXPRESSIONSTATEMENT;
		_ndNode.nuNodeData.ui32NodeIndex = _ui32Index;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an declaration statement.
	 *
	 * \param _ndDecl The declaration node.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateDeclarationStatement( const YYSTYPE::LSG_NODE_DATA &_ndDecl,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_DECLARATIONSTATEMENT;
		_ndNode.nuNodeData.ui32NodeIndex = _ndDecl.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a semantic.
	 *
	 * \param _pcText The semantic text.
	 * \param _ui32Type The semantic type.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateSemantic( const char * _pcText,
		uint32_t _ui32Type,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_SEMANTIC;
		// Sementic text.
		_ndNode.nuNodeData.ui32NodeIndex = CreateString( _pcText, false );
		if ( _ndNode.nuNodeData.ui32NodeIndex == LSE_MAXU32 ) {
			return false;
		}
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ui32Type;

		// Get the last characters on the string and convert them to a number if possible.
		const char * pcLast = _pcText;
		while ( (*pcLast) ) {
			if ( CStd::IsDigit( (*pcLast) ) ) {
				break;
			}
			++pcLast;
		}

		_ndNode.nuoOp.ui32NodeIndex3 = CStd::AtoI32( pcLast );
		
		return AddNode( _ndNode );
	}

	/**
	 * Creates a type-specifier node without a precision qualifier.  Precision qualifiers do not
	 *	currently exist but may in the future.
	 *
	 * \param _ui64Qualifiers The type qualifiers.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateTypeSpecifier( uint64_t _ui64Qualifiers,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_TYPESPECIFIERNOPREC;
		_ndNode.nuNodeData.ui64Const = _ui64Qualifiers;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a fully specified type.  Such as "IN void" or "float".
	 *
	 * \param _ui64Qualifiers The type qualifiers.
	 * \param _ndSpec Type specifier.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFullySpecifiedType( uint64_t _ui64Qualifiers,
		const YYSTYPE::LSG_NODE_DATA &_ndSpec,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FULLYSPECIFIEDTYPE;
		_ndNode.nuNodeData.ui64Const = _ui64Qualifiers;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndSpec.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a single declaration.  Such as "flat vector g_vUp = { 0.0f, 1.0f, 0.0 }".
	 *
	 * \param _ui32Name Name of the variable to create.
	 * \param _ndTypeNode Fully specified type data.
	 * \param _ui32ConstNodeIndex Index of the constant-expression node or LSE_MAXU32 if there is none.
	 * \param _ui32SemanticNodeIndex Index of the semantic node or LSE_MAXU32 if there is none.
	 * \param _ui32InitializerNodeIndex Index of the initializer node or LSE_MAXU32 if there is none.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateSingleDeclaration( uint32_t _ui32Name,
		const YYSTYPE::LSG_NODE_DATA &_ndTypeNode,
		uint32_t _ui32ConstNodeIndex,
		uint32_t _ui32SemanticNodeIndex,
		uint32_t _ui32InitializerNodeIndex,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_SINGLEDECL;
		_ndNode.nuNodeData.ui32NodeIndex = _ndTypeNode.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ui32ConstNodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ui32InitializerNodeIndex;
		_ndNode.ui32SingleDeclName = _ui32Name;
		_ndNode.ui32Semantic = _ui32SemanticNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an initializer list.
	 *
	 * \param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateInitializerList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_INITLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an initializer list, wrapped inside {}.
	 *
	 * \param _ndList The initialier list.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateInitializerListWrap( const YYSTYPE::LSG_NODE_DATA &_ndList,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_INITLISTWRAP;
		_ndNode.nuNodeData.ui32NodeIndex = _ndList.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a statement list.
	 *
	 * \param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStatementList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STATEMENTLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a statement with a raised scope.
	 *
	 * \param _ndStatement The statement with the increased scope.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateNewScopedStatement( const YYSTYPE::LSG_NODE_DATA &_ndStatement,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_NEWSCOPESTATEMENT;
		_ndNode.nuNodeData.ui32NodeIndex = _ndStatement.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates an empty statement.
	 *
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateBlankStatement( YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_BLANKSTATEMENT;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a structure-declarator node.  This is the declaration of a single member of a structure
	 *	excluding type qualifiers.
	 *
	 * \param _ndName Identifier node representing the name of the member.
	 * \param _ui32ArrayNodeIndex Index of the node representing the array constant or LSE_MAXU32.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStructDeclarator( const YYSTYPE::LSG_NODE_DATA &_ndName,
		uint32_t _ui32ArrayNodeIndex,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STRUCTDECLARATOR;
		_ndNode.nuNodeData.ui32NodeIndex = _ndName.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ui32ArrayNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a structure-declarator list node.
	 *
	 * \param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStructDeclaratorList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STRUCTDECLARATORLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a structure declarator.  A full member or list of members with the same type specifiers and qualifiers.
	 *
	 * \param _ui64Qualifiers The qualifiers.
	 * \param _ndType The type data.
	 * \param _ndDecl The declarator node, always of type LSG_N_STRUCTDECLARATORLIST.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStructDeclaration( uint64_t _ui64Qualifiers, 
		const YYSTYPE::LSG_NODE_DATA &_ndType,
		const YYSTYPE::LSG_NODE_DATA &_ndDecl,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STRUCTDECLARATION;
		_ndNode.nuNodeData.ui64Const = _ui64Qualifiers;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndDecl.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ndType.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a structure declaration list node.
	 *
	 * \param _ndLeft The left operand.
	 * \param _ndRight The right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStructDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STRUCTDECLARATIONLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a structure node.
	 *
	 * \param _ui32IdentifierIndex Index of the node representing the name of the structure, if any.  LSE_MAXU32 if
	 *	the structure is nameless.
	 * \param _ndDecl The structure members.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateStruct( uint32_t _ui32IdentifierIndex,
		const YYSTYPE::LSG_NODE_DATA &_ndDecl,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_STRUCT;

		// Give nameless structures names.
		if ( _ui32IdentifierIndex == LSE_MAXU32 ) {
			CString sName = "__unnamed_struct_";
			if ( !sName.Length() ) { return false; }
			CString sNumber;
			if ( !sNumber.FromUI64( _ndDecl.ui32NodeIndex ) ) { return false; }
			if ( !sName.Append( sNumber ) ) { return false; }
			YYSTYPE::LSG_NODE_DATA ndIdentifierNode;
			if ( !CreateIdentifier( sName.CStr(), ndIdentifierNode ) ) { return false; }
			_ui32IdentifierIndex = ndIdentifierNode.ui32NodeIndex;
		}

		_ndNode.nuNodeData.ui32IdentifierIndex = _ui32IdentifierIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndDecl.ui32NodeIndex;

		if ( !RegisterStructType( GetName( GetNode( _ndNode.nuNodeData.ui32IdentifierIndex ) ).CStr() ) ) {
			return false;
		}

		// Make a new structure entry with all of the structure members on it.
		CVectorPoD<uint32_t, uint32_t> vMembers;
		uint32_t ui32Total = CountStructMembers( _ndNode.nueNodeDataEx.ui32NodeIndexEx, &vMembers );
		if ( ui32Total == LSE_MAXU32 ) {
			return false;
		}
		LSG_STRUCT sNewStruct;
		sNewStruct.sName = GetName( GetNode( _ndNode.nuNodeData.ui32IdentifierIndex ) );
		sNewStruct.sName.Snap();
		CVectorPoD<LSG_STRUCT_DECLARATOR> vDeclarators;
		for ( uint32_t I = ui32Total; I--; ) {
			vDeclarators.ResetNoDealloc();
			LSG_STRUCT_MEMBER smThis;
			const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & ndThis = GetNode( GetNode( vMembers[I] ).nuoOp.ui32NodeIndex3 );
			smThis.ui64Type = ndThis.nuNodeData.ui64Const;

			if ( !GetStructDeclarators( GetNode( vMembers[I] ).nueNodeDataEx.ui32NodeIndexEx, vDeclarators ) ) {
				return false;
			}
			for ( uint32_t J = vDeclarators.Length(); J--; ) {
				smThis.sName = GetName( GetNode( vDeclarators[J].ui32Name ) );
				smThis.sName.Snap();
				smThis.ui32Array = vDeclarators[J].ui32Array;
				if ( smThis.ui32Array != LSE_MAXU32 ) {
					LSG_SCALAR sValue;
					LSG_NUM_CONSTANTS ncType = GetNodeValue( GetNode( smThis.ui32Array ), sValue );
					LSBOOL bRet;
					LSG_SCALAR sFinalValue = CastScalar( sValue, ConstantTypeToType( ncType ), ConstantTypeToType( LSG_NC_UNSIGNED ), bRet );
					if ( !bRet ) { return false; }
					smThis.ui32Array = static_cast<uint32_t>(sFinalValue.ui64Value);
				}

				if ( !sNewStruct.vMembers.Push( smThis ) ) {
					return false;
				}
			}


		}

		if ( !m_saStructures.Insert( sNewStruct.sName, sNewStruct ) ) {
			return false;
		}

		return AddNode( _ndNode );
	}

	/**
	 * Creates a parameter declarator node.
	 *
	 * \param _ndType The type information.
	 * \param _ndName Parameter name.
	 * \param _ui32ArrayNodeIndex Index of the node associated with the array part of the declarator if any.
	 * \param _ui32SemanticNodeIndex Index of the node associated with the semantic part of the declarator if any.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateParameterDeclarator( const YYSTYPE::LSG_NODE_DATA &_ndType,
		const YYSTYPE::LSG_NODE_DATA &_ndName,
		uint32_t _ui32ArrayNodeIndex,
		uint32_t _ui32SemanticNodeIndex,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_PARMDECLARATOR;
		_ndNode.nuNodeData.ui32IdentifierIndex = _ndName.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndType.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ui32ArrayNodeIndex;
		_ndNode.ui32SingleDeclName = _ui32SemanticNodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a parameter declaration node.
	 *
	 * \param _ui64TypeInfo Full type-qualifier information.
	 * \param _ndDecl Declaration node.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateParameterDeclaration( uint64_t _ui64TypeInfo,
		const YYSTYPE::LSG_NODE_DATA &_ndDecl,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_PARMDECLARATION;
		_ndNode.nuNodeData.ui64Const = _ui64TypeInfo;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndDecl.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a parameter declaration list.
	 *
	 * \param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateParameterDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_PARMDECLARATIONLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a function-declaration node.
	 *
	 * \param _ndType Type information.
	 * \param _ndName Function name.
	 * \param _ui32Parms Optional parameters.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFunctionDeclaration( const YYSTYPE::LSG_NODE_DATA &_ndType,
		const YYSTYPE::LSG_NODE_DATA &_ndName,
		uint32_t _ui32Parms,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FUNCDECL;
		_ndNode.nuNodeData.ui32NodeIndex = _ndType.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndName.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ui32Parms;

		// Set the current function accordingly.
		const YYSTYPE::LSG_NODE_DATA & ndIdentifier = GetNode( _ndName.ui32NodeIndex );
		if ( ndIdentifier.nNodeType == LSG_N_IDENTIFIER ) {
			m_ui32CurFunc = GetIdentifier( ndIdentifier.nuNodeData.ui32IdentifierIndex ).ui32Name;
		}

		return AddNode( _ndNode );
	}

	/**
	 * Creates a function definition node.
	 *
	 * \param _ndHeader Function header and parameters.
	 * \param _ndBody Function body.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateFunctionDefinition( const YYSTYPE::LSG_NODE_DATA &_ndHeader,
		const YYSTYPE::LSG_NODE_DATA &_ndBody,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FUNCDEFINITION;
		_ndNode.nuNodeData.ui32NodeIndex = _ndHeader.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBody.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a selection (if/else) statement node.
	 *
	 * \param _ndExp The expression to evaluate.
	 * \param _ndBody The statement body.
	 * \param _ui32ElseBody The index of the else body statement if any.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateSelectionStatement( const YYSTYPE::LSG_NODE_DATA &_ndExp,
		const YYSTYPE::LSG_NODE_DATA &_ndBody,
		uint32_t _ui32ElseBody,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_SELECTION;
		_ndNode.nuNodeData.ui32NodeIndex = _ndExp.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBody.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ui32ElseBody;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a for-initilializer node.
	 *
	 * \param _ndExp The initializer node.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateForInitDeclStatement( const YYSTYPE::LSG_NODE_DATA &_ndExp,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FORINIT;
		_ndNode.nuNodeData.ui32NodeIndex = _ndExp.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a while () node.
	 *
	 * \param _ndExp The conditional expression.
	 * \param _ndBody The body of the iterator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateWhileLoop( const YYSTYPE::LSG_NODE_DATA &_ndExp,
		const YYSTYPE::LSG_NODE_DATA &_ndBody,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_WHILE;
		_ndNode.nuNodeData.ui32NodeIndex = _ndExp.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBody.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a do () while node.
	 *
	 * \param _ndExp The conditional expression.
	 * \param _ndBody The body of the iterator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateDoWhileLoop( const YYSTYPE::LSG_NODE_DATA &_ndExp,
		const YYSTYPE::LSG_NODE_DATA &_ndBody,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_DOWHILE;
		_ndNode.nuNodeData.ui32NodeIndex = _ndExp.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndBody.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a for () node.
	 *
	 * \param _ndInitExp The initializer expression.
	 * \param _ndExp The conditional expression.
	 * \param _ui32ItIndex The iteartion expression index, of any.
	 * \param _ndBody The body of the iterator.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateForLoop( const YYSTYPE::LSG_NODE_DATA &_ndInitExp,
		const YYSTYPE::LSG_NODE_DATA &_ndExp,
		uint32_t _ui32ItIndex,
		const YYSTYPE::LSG_NODE_DATA &_ndBody,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_FOR;
		_ndNode.nuNodeData.ui32NodeIndex = _ndInitExp.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndExp.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ui32ItIndex;
		_ndNode.ui32SingleDeclName = _ndBody.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a jump node.
	 *
	 * \param _nType The jump node type.
	 * \param _ui32NodeIndex Index of the associated node, if there is one.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateJump( LSG_NODE _nType, uint32_t _ui32NodeIndex,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = _nType;
		_ndNode.nuNodeData.ui32NodeIndex = _ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a list of external declarations.
	 *
	 * \param param _ndLeft Left operand.
	 * \param _ndRight Right operand.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::CreateExternalDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.nNodeType = LSG_N_EXTERNALDECLARATIONLIST;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Find an identifier by name.
	 *
	 * \param _pcName Name of the identifier to find.
	 * \return Returns the index of the identifier with the given name in the stack of identifiers or LSE_MAXU32 on error.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::FindIdentifier( const char * _pcName ) const {
		for ( uint32_t I = m_vIdentifiers.Length(); I--; ) {
			if ( m_vStringPool[m_vIdentifiers[I].ui32Name] == _pcName ) { return I; }
		}
		return LSE_MAXU32;
	}

	/**
	 * Is the given string a registered token?
	 *
	 * \param _pcText Name to check for being a token as apposed to an identifier.
	 * \return Returns true if the given string is a token name.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsToken( const char * _pcText ) const {
		uint32_t ui32Index;
		return FindToken( _pcText, ui32Index );
	}

	/**
	 * Gets a node for read-only.
	 *
	 * \param _ui32Index Index of the node to get.
	 * \return Returns a constant reference to the requested node.
	 */
	const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & LSE_CALL CShaderLanguageParser::GetNode( uint32_t _ui32Index ) const {
		return m_vNodes[_ui32Index];
	}

	/**
	 * Gets the total number of nodes.
	 *
	 * \return Returns the total number of nodes in the syntax tree.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::TotalNodes() const {
		return m_vNodes.Length();
	}

	/**
	 * Gets the total identifiers.
	 *
	 * \return Returns the total number of identifiers.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::TotalIdentifiers() const {
		return m_vIdentifiers.Length();
	}

	/**
	 * Gets an identifier by index.
	 *
	 * \param _ui32Index Index of the identifier to get.
	 * \return Returns a constant reference to the requested identifier.
	 */
	const CShaderLanguageParser::LSG_IDENTIFIER & LSE_CALL CShaderLanguageParser::GetIdentifier( uint32_t _ui32Index ) const {
		return m_vIdentifiers[_ui32Index];
	}

	/**
	 * Gets the total strings.
	 *
	 * \return Returns the total number of strings.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::TotalStrings() const {
		return m_vStringPool.Length();
	}

	/**
	 * Gets a string by index.
	 *
	 * \param _ui32Index Index of the string to get.
	 * \return Returns a constant reference to the requested string.
	 */
	const CString & LSE_CALL CShaderLanguageParser::GetString( uint32_t _ui32Index ) const {
		return m_vStringPool[_ui32Index];
	}

	/**
	 * Gets the type and value of a node.  The node must represent a constant numeric type.
	 *
	 * \param _ndNode The node to test.
	 * \param _sValue The value of the node, which should be reinterpreted based on the type of the node.
	 * \return Returns the type of the node.
	 */
	LSG_NUM_CONSTANTS LSE_CALL CShaderLanguageParser::GetNodeValue( const YYSTYPE::LSG_NODE_DATA &_ndNode, LSG_SCALAR &_sValue ) const {
		switch ( _ndNode.nNodeType ) {
			case LSG_N_NUMERICCONSTANT : {
				_sValue.ui64Value = _ndNode.nuNodeData.ui64Const;
				return _ndNode.nueNodeDataEx.ncConstantType;
			}
			case LSG_N_BOOLEAN : {
				_sValue.ui64Value = _ndNode.nuNodeData.ui32UnaryOp;
				return LSG_NC_SIGNED;
			}
			case LSG_N_IDENTIFIER : {
				// Determine the type of the identifier.  TODO.
				return LSG_NC_INVALID;
			}
			case LSG_N_ARRAYACCESS : {
				// Determine the type of the identifier.  TODO.
				return LSG_NC_INVALID;
			}
			case LSG_N_POSTFIXINC : {}
			case LSG_N_POSTFIXDEC : {
				return GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], _sValue );
			}
			case LSG_N_PREFIXINC : {
				LSG_NUM_CONSTANTS ncRet = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], _sValue );
				switch ( ncRet ) {
					case LSG_NC_SIGNED : {}
					case LSG_NC_UNSIGNED : {
						++_sValue.ui64Value;
						break;
					}
					case LSG_NC_FLOATING : {
						_sValue.dValue += 1.0;
					}
					default : {}
				}
				return ncRet;
			}
			case LSG_N_PREFIXDEC : {
				LSG_NUM_CONSTANTS ncRet = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], _sValue );
				switch ( ncRet ) {
					case LSG_NC_SIGNED : {}
					case LSG_NC_UNSIGNED : {
						--_sValue.ui64Value;
						break;
					}
					case LSG_NC_FLOATING : {
						_sValue.dValue -= 1.0;
					}
					default : {}
				}
				return ncRet;
			}
			case LSG_N_UNARY : {
				LSG_NUM_CONSTANTS ncRet = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], _sValue );
				switch ( _ndNode.nuNodeData.ui32UnaryOp ) {
					case '!' : {
						switch ( ncRet ) {
							case LSG_NC_SIGNED : {}
							case LSG_NC_UNSIGNED : {
								_sValue.ui64Value = !_sValue.ui64Value;
								break;
							}
							case LSG_NC_FLOATING : {
								ncRet = LSG_NC_SIGNED;
								_sValue.i64Value = !_sValue.dValue;
							}
							default : {}
						}
						break;
					}
					case '~' : {
						switch ( ncRet ) {
							case LSG_NC_SIGNED : {}
							case LSG_NC_UNSIGNED : {
								_sValue.ui64Value = ~_sValue.ui64Value;
								break;
							}
							case LSG_NC_FLOATING : {
								ncRet = LSG_NC_INVALID;
							}
							default : {}
						}
						break;
					}
					case '-' : {
						switch ( ncRet ) {
							case LSG_NC_SIGNED : {}
							case LSG_NC_UNSIGNED : {
								_sValue.i64Value = -_sValue.i64Value;
								break;
							}
							case LSG_NC_FLOATING : {
								_sValue.dValue = -_sValue.dValue;
							}
							default : {}
						}
						break;
					}
				}
				return ncRet;
			}
			case LSG_N_CAST : {
				LSG_NUM_CONSTANTS ncRet = GetNodeValue( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx], _sValue );
				LSBOOL bSuccess;
				_sValue = CastScalar( _sValue, ConstantTypeToType( ncRet ), _ndNode.nuNodeData.ui64CastType, bSuccess );
				if ( !bSuccess ) { return LSG_NC_INVALID; }

				return TypeToConstantType( _ndNode.nuNodeData.ui64CastType );
			}
			case LSG_N_MATH : {
				LSG_SCALAR sLeft, sRight;
				LSG_NUM_CONSTANTS ncLeft = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], sLeft );
				LSG_NUM_CONSTANTS ncRight = GetNodeValue( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx], sRight );
				LSG_NUM_CONSTANTS ncRet = LSG_NC_INVALID;
				if ( ncLeft == LSG_NC_INVALID || ncRight == LSG_NC_INVALID ) { return ncRet; }

				// Cast to the common type.
				LSBOOL bSuccess;
				LSG_NUM_CONSTANTS ncType = GetHighType( ncLeft, ncRight );
				sLeft = CastScalar( sLeft, ConstantTypeToType( ncLeft ), ConstantTypeToType( ncType ), bSuccess );
				if ( !bSuccess ) { return ncRet; }
				sRight = CastScalar( sRight, ConstantTypeToType( ncRight ), ConstantTypeToType( ncType ), bSuccess );
				if ( !bSuccess ) { return ncRet; }

				ncRet = ncType;
				switch ( _ndNode.nuoOp.ui32Op ) {
#define LSG_CMP( CASE, CHECK )																\
					case CASE : {															\
						ncRet = LSG_NC_SIGNED;												\
						switch ( ncType ) {													\
							case LSG_NC_SIGNED : {											\
								_sValue.i64Value = sLeft.i64Value CHECK sRight.i64Value;	\
								break;														\
							}																\
							case LSG_NC_UNSIGNED : {										\
								_sValue.i64Value = sLeft.ui64Value CHECK sRight.ui64Value;	\
								break;														\
							}																\
							case LSG_NC_FLOATING : {										\
								_sValue.i64Value = sLeft.dValue CHECK sRight.dValue;		\
								break;														\
							}																\
							default : { return LSG_NC_INVALID; }							\
						}																	\
					}
					LSG_CMP( '>', > )
					LSG_CMP( '<', < )
					LSG_CMP( CShaderParser::token::LSG_SLT_GE, >= )
					LSG_CMP( CShaderParser::token::LSG_SLT_LE, <= )
					LSG_CMP( CShaderParser::token::LSG_SLT_EQUALS, == )
					LSG_CMP( CShaderParser::token::LSG_SLT_NOT_EQUALS, != )
					LSG_CMP( CShaderParser::token::LSG_SLT_AND, && )
					LSG_CMP( CShaderParser::token::LSG_SLT_OR, || )
#undef LSG_CMP
#define LSG_CMP( CHECK )																	\
						switch ( ncType ) {													\
							case LSG_NC_SIGNED : {											\
								_sValue.i64Value = sLeft.i64Value CHECK sRight.i64Value;	\
								break;														\
							}																\
							case LSG_NC_UNSIGNED : {										\
								_sValue.ui64Value = sLeft.ui64Value CHECK sRight.ui64Value;	\
								break;														\
							}																\
							case LSG_NC_FLOATING : {										\
								_sValue.dValue = sLeft.dValue CHECK sRight.dValue;			\
								break;														\
							}																\
							default : { return LSG_NC_INVALID; }							\
						}
#define LSG_CMPNEST( CASE, CHECK )															\
						case CASE : {														\
							LSG_CMP( CHECK )												\
							break;															\
						}
					LSG_CMPNEST( '*', * )
					LSG_CMPNEST( '/', / )
					LSG_CMPNEST( '+', + )
					LSG_CMPNEST( '-', - )

#undef LSG_CMP
#define LSG_CMP( CHECK )																	\
						switch ( ncType ) {													\
							case LSG_NC_SIGNED : {											\
								_sValue.i64Value = sLeft.i64Value CHECK sRight.i64Value;	\
								break;														\
							}																\
							case LSG_NC_UNSIGNED : {										\
								_sValue.ui64Value = sLeft.ui64Value CHECK sRight.ui64Value;	\
								break;														\
							}																\
							default : { return LSG_NC_INVALID; }							\
						}
					LSG_CMPNEST( '%', % )
					LSG_CMPNEST( CShaderParser::token::LSG_SLT_SHIFT_LEFT, << )
					LSG_CMPNEST( CShaderParser::token::LSG_SLT_SHIFT_RIGHT, >> )
					LSG_CMPNEST( '&', & )
					LSG_CMPNEST( '^', ^ )
					LSG_CMPNEST( '|', | )

#undef LSG_CMPNEST
#undef LSG_CMP
				}
				return ncRet;
			}
			case LSG_N_TERTIARY : {
				LSG_NUM_CONSTANTS ncRet = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], _sValue );
				if ( (ncRet == LSG_NC_SIGNED && _sValue.i64Value) ||
					(ncRet == LSG_NC_UNSIGNED && _sValue.ui64Value) ||
					(ncRet == LSG_NC_FLOATING && _sValue.dValue) ) {
					// Use the left.
					return GetNodeValue( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx], _sValue );
				}
				return GetNodeValue( m_vNodes[_ndNode.nuoOp.ui32NodeIndex3], _sValue );
			}
			case LSG_N_ASSIGNMENT : {
				LSG_SCALAR sLeft, sRight;
				LSG_NUM_CONSTANTS ncRight = GetNodeValue( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx], sLeft );
				LSG_NUM_CONSTANTS ncLeft = GetNodeValue( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex], sRight );

				LSG_NUM_CONSTANTS ncRet = LSG_NC_INVALID;
				if ( ncLeft == LSG_NC_INVALID || ncRight == LSG_NC_INVALID ) { return ncRet; }

				// Cast to the left type.
				LSBOOL bSuccess;
				LSG_NUM_CONSTANTS ncType = ncLeft;
				sLeft = CastScalar( sLeft, ncLeft, ncType, bSuccess );
				if ( !bSuccess ) { return ncRet; }

				switch ( _ndNode.nuoOp.ui32Op ) {
#define LSG_CMP( CASE, CHECK )																\
					case CASE : {															\
						switch ( ncType ) {													\
							case LSG_NC_SIGNED : {											\
								_sValue.i64Value = sLeft.i64Value CHECK sRight.i64Value;	\
								break;														\
							}																\
							case LSG_NC_UNSIGNED : {										\
								_sValue.ui64Value = sLeft.ui64Value CHECK sRight.ui64Value;	\
								break;														\
							}																\
							case LSG_NC_FLOATING : {										\
								_sValue.dValue = sLeft.dValue CHECK sRight.dValue;			\
								break;														\
							}																\
							default : { return LSG_NC_INVALID; }							\
						}																	\
						break;																\
					}

					LSG_CMP( '=', = )
					LSG_CMP( CShaderParser::token::LSG_SLT_PLUS_EQUALS, + )
					LSG_CMP( CShaderParser::token::LSG_SLT_MINUS_EQUALS, - )
					LSG_CMP( CShaderParser::token::LSG_SLT_TIMES_EQUALS, * )
					LSG_CMP( CShaderParser::token::LSG_SLT_DIV_EQUALS, / )
#undef LSG_CMP
#define LSG_CMP( CASE, CHECK )																\
					case CASE : {															\
						switch ( ncType ) {													\
							case LSG_NC_SIGNED : {											\
								_sValue.i64Value = sLeft.i64Value CHECK sRight.i64Value;	\
								break;														\
							}																\
							case LSG_NC_UNSIGNED : {										\
								_sValue.ui64Value = sLeft.ui64Value CHECK sRight.ui64Value;	\
								break;														\
							}																\
							default : { return LSG_NC_INVALID; }							\
						}																	\
						break;																\
					}
					LSG_CMP( CShaderParser::token::LSG_SLT_MOD_EQUALS, % )
					LSG_CMP( CShaderParser::token::LSG_SLT_SHIFT_LEFT_EQUALS, << )
					LSG_CMP( CShaderParser::token::LSG_SLT_SHIFT_RIGHT_EQUALS, >> )
					LSG_CMP( CShaderParser::token::LSG_SLT_AND_EQUALS, & )
					LSG_CMP( CShaderParser::token::LSG_SLT_XOR_EQUALS, ^ )
					LSG_CMP( CShaderParser::token::LSG_SLT_OR_EQUALS, | )
#undef LSG_CMP
				}

				return ncRet;
			}
			case LSG_N_COMMA : {
				return GetNodeValue( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx], _sValue );
			}
			case LSG_N_FUNCTIONCALL : {
				// Can't predict the value of a function call.
				return LSG_NC_INVALID;
			}
			case LSG_N_INTRINSIC : {
				// Can't predict the value of a function call.
				return LSG_NC_INVALID;
			}
			default : {}
		}
		return LSG_NC_INVALID;
	}

	/**
	 * Determine if a given expression resolves to an integer value.
	 *
	 * \param _ndNode The node to check.
	 * \return Returns true if the expression resolves to an integer value.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsInteger( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		switch ( _ndNode.nNodeType ) {
			case LSG_N_NUMERICCONSTANT : {
				return _ndNode.nueNodeDataEx.ncConstantType == LSG_NC_SIGNED ||
					_ndNode.nueNodeDataEx.ncConstantType == LSG_NC_UNSIGNED;
			}
			case LSG_N_BOOLEAN : {
				return true;
			}
			case LSG_N_IDENTIFIER : {
				CString sName = GetName( _ndNode );
				// Determine the type of the identifier.
				uint32_t ui32Index = FindIdentifier( sName.CStr() );
				if ( LSE_MAXU32 == ui32Index ) { return false; }	// Identifier not found.
				//return m_vIdentifiers[ui32Index].ui8Type == LSG_IDENTIFIER::LSG_IT_INTEGRAL;
				return true;
			}
			case LSG_N_ARRAYACCESS : {
				return IsInteger( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex] );
			}
			case LSG_N_POSTFIXINC : {}
			case LSG_N_POSTFIXDEC : {}
			case LSG_N_PREFIXINC : {}
			case LSG_N_PREFIXDEC : {
				return IsInteger( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex] );
			}
			case LSG_N_UNARY : {
				return IsInteger( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx] );
			}
			case LSG_N_CAST : {
				return IsInteger( _ndNode.nuNodeData.ui64CastType );
			}
			case LSG_N_MATH : {
				switch ( _ndNode.nuoOp.ui32Op ) {
					case '>' : {}
					case '<' : {}
					case CShaderParser::token::LSG_SLT_GE : {}
					case CShaderParser::token::LSG_SLT_LE : {}
					case CShaderParser::token::LSG_SLT_EQUALS : {}
					case CShaderParser::token::LSG_SLT_NOT_EQUALS : {}
					case CShaderParser::token::LSG_SLT_AND : {}
					case CShaderParser::token::LSG_SLT_XOR : {}
					case CShaderParser::token::LSG_SLT_OR : { return true; }
					default : {
						return IsInteger( m_vNodes[_ndNode.nuNodeData.ui32NodeIndex] ) && IsInteger( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx] );
					}
				}
			}
			case LSG_N_TERTIARY : {
				return IsInteger( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx] ) && IsInteger( m_vNodes[_ndNode.nuoOp.ui32NodeIndex3] );
			}
			case LSG_N_ASSIGNMENT : {
				// Determine the type of the identifier.  TODO.
				return false;
			}
			case LSG_N_COMMA : {
				return IsInteger( m_vNodes[_ndNode.nueNodeDataEx.ui32NodeIndexEx] );
			}
			case LSG_N_FUNCTIONCALL : {
				// Determine the type of the function.  TODO.
				return false;
			}
			case LSG_N_INTRINSIC : {
				// No intrinsic functions return integers.
				return false;
			}
			default : {}
		}
		return false;
	}

	/**
	 * Count the number of arguments in the given argument list up to the maximum specified.  Also stores the argument
	 *	node indices in the given array in reverse order.  If the number of arguments is less than or equal to the given
	 *	max, the node count is returned and the nodes are stored in _pui32Nodes.  Otherwise LSE_MAXU32 is returned and the
	 *	contents of _pui32Nodes are undefined.  This is used by the engine for intrinsic functions only.
	 *
	 * \param _ui32ArgListIndex Index of the argument list whose nodes are to be counted.
	 * \param _ui32MaxArgs Maximum number of arguments that can be stored in _pui32Nodes.
	 * \param _pui32Nodes Filled with the arguments in reverse order upon return.
	 * \return Returns the number of arguments found or LSE_MAXU32 if there are more than _ui32MaxArgs.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::CountArgs( uint32_t _ui32ArgListIndex, uint32_t _ui32MaxArgs, uint32_t * _pui32Nodes ) const {
		if ( _ui32ArgListIndex == LSE_MAXU32 ) { return 0; }

		switch ( m_vNodes[_ui32ArgListIndex].nNodeType ) {
			case LSG_N_ARGLIST : {
				// Go into the right.
				uint32_t ui32Right = CountArgs( m_vNodes[_ui32ArgListIndex].nueNodeDataEx.ui32NodeIndexEx, _ui32MaxArgs, _pui32Nodes );
				if ( ui32Right == LSE_MAXU32 ) { return LSE_MAXU32; }

				// Go into the left.
				_ui32MaxArgs -= ui32Right;
				_pui32Nodes += ui32Right;

				uint32_t ui32Left = CountArgs( m_vNodes[_ui32ArgListIndex].nuNodeData.ui32NodeIndex, _ui32MaxArgs, _pui32Nodes );
				if ( ui32Left == LSE_MAXU32 ) { return LSE_MAXU32; }
				return ui32Right + ui32Left;
			}
			default : {
				// Everything else is a argument.
				if ( _ui32MaxArgs == 0 ) {
					// Not enough room for another argument.
					return LSE_MAXU32;
				}

				// Push the argument.
				(*_pui32Nodes) = _ui32ArgListIndex;
				return 1;
			}
		}
	}

	/**
	 * Count the number of parameters in the given argument list.  Optionally returns the argument node indices in the
	 *	optionally supplied vector in reverse order.
	 *
	 * \param _ui32ArgListIndex Index of the argument list whose nodes are to be counted.
	 * \param _pvRet If not NULL, filled with the arguments in reverse order upon return.
	 * \return return Returns the number of arguments found or LSE_MAXU32 if there is not enough memory to add the indices to the given vector.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::CountParms( uint32_t _ui32ArgListIndex, CVectorPoD<uint32_t, uint32_t> * _pvRet ) {
		if ( _ui32ArgListIndex == LSE_MAXU32 ) { return 0; }
		// Use an explicit stack because there could theoretically be thousands of arguments which would cause stack overflow if we were
		//	to try to parse recursively.  Of course, such a function is ridiculous, and would never happen in the real world (and would
		//	not run on any graphics card even if it did), but this engine is built to be solid.  There is nothing that can crash this
		//	engine because even the most ridiculous cases are handled correctly.
		CVectorPoD<uint32_t, uint32_t> vStack;
		if ( !vStack.Push( _ui32ArgListIndex ) ) {
			return LSE_MAXU32;
		}
		uint32_t ui32Return = 0;
		while ( vStack.Length() ) {
			uint32_t ui32ThisIndex = vStack[vStack.Length()-1];
			vStack.PopNoDealloc();
			LSG_NODE nType = m_vNodes[ui32ThisIndex].nNodeType;
			switch ( nType ) {
				case LSG_N_PARMDECLARATIONLIST : {
					// An argument list.  Go into the right first, then left.  Push in opposite order to get these results.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nuNodeData.ui32NodeIndex ) ) { return LSE_MAXU32; }		// Left node index.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nueNodeDataEx.ui32NodeIndexEx ) ) { return LSE_MAXU32; }	// Right node index.
					break;
				}
				default : {
					// Parameter.
					if ( _pvRet ) {
						if ( !_pvRet->Push( ui32ThisIndex ) ) { return LSE_MAXU32; }
					}
					++ui32Return;
				}
			}
		}
		return ui32Return;
	}

	/**
	 * Count the number of members on a structure.  Optionally returns the member declarations in reverse order.
	 *
	 * \param _ui32MemberListIndex Index of the structure declaration list whose nodes are to be counted.
	 * \param _pvRet If not NULL, filled with the arguments in reverse order upon return.
	 * \return return Returns the number of members found or LSE_MAXU32 if there is not enough memory to add the indices to the given vector.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::CountStructMembers( uint32_t _ui32MemberListIndex, CVectorPoD<uint32_t, uint32_t> * _pvRet ) {
		if ( _ui32MemberListIndex == LSE_MAXU32 ) { return 0; }
		// Use an explicit stack because there could theoretically be thousands of arguments which would cause stack overflow if we were
		//	to try to parse recursively.  Of course, such a function is ridiculous, and would never happen in the real world (and would
		//	not run on any graphics card even if it did), but this engine is built to be solid.  There is nothing that can crash this
		//	engine because even the most ridiculous cases are handled correctly.
		CVectorPoD<uint32_t, uint32_t> vStack;
		if ( !vStack.Push( _ui32MemberListIndex ) ) {
			return LSE_MAXU32;
		}
		uint32_t ui32Return = 0;
		while ( vStack.Length() ) {
			uint32_t ui32ThisIndex = vStack[vStack.Length()-1];
			vStack.PopNoDealloc();
			LSG_NODE nType = m_vNodes[ui32ThisIndex].nNodeType;
			switch ( nType ) {
				case LSG_N_STRUCTDECLARATIONLIST : {
					// A structure declaration list.  Go into the right first, then left.  Push in opposite order to get these results.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nuNodeData.ui32NodeIndex ) ) { return LSE_MAXU32; }		// Left node index.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nueNodeDataEx.ui32NodeIndexEx ) ) { return LSE_MAXU32; }	// Right node index.
					break;
				}
				default : {
					// Parameter.
					if ( _pvRet ) {
						if ( !_pvRet->Push( ui32ThisIndex ) ) { return LSE_MAXU32; }
					}
					++ui32Return;
				}
			}
		}
		return ui32Return;
	}

	/**
	 * Gets the declarators on a structure declaration.
	 *
	 * \param _ui32Index Index of the structure declaration.
	 * \param _vRet Returned array of structure declarators.
	 * \return Returns true if there was enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::GetStructDeclarators( uint32_t _ui32Index, CVectorPoD<LSG_STRUCT_DECLARATOR> &_vRet ) const {
		if ( _ui32Index == LSE_MAXU32 ) { return true; }
		// Use an explicit stack because there could theoretically be thousands of arguments which would cause stack overflow if we were
		//	to try to parse recursively.  Of course, such a function is ridiculous, and would never happen in the real world (and would
		//	not run on any graphics card even if it did), but this engine is built to be solid.  There is nothing that can crash this
		//	engine because even the most ridiculous cases are handled correctly.
		CVectorPoD<uint32_t, uint32_t> vStack;
		if ( !vStack.Push( _ui32Index ) ) {
			return false;
		}
		while ( vStack.Length() ) {
			uint32_t ui32ThisIndex = vStack[vStack.Length()-1];
			vStack.PopNoDealloc();
			LSG_NODE nType = m_vNodes[ui32ThisIndex].nNodeType;
			switch ( nType ) {
				case LSG_N_STRUCTDECLARATORLIST : {
					// A structure declaration list.  Go into the right first, then left.  Push in opposite order to get these results.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nuNodeData.ui32NodeIndex ) ) { return false; }		// Left node index.
					if ( !vStack.Push( m_vNodes[ui32ThisIndex].nueNodeDataEx.ui32NodeIndexEx ) ) { return false; }	// Right node index.
					break;
				}
				case LSG_N_STRUCTDECLARATOR : {
					// Parameter.
					LSG_STRUCT_DECLARATOR sdDecl = {
						m_vNodes[ui32ThisIndex].nuNodeData.ui32NodeIndex,
						m_vNodes[ui32ThisIndex].nueNodeDataEx.ui32NodeIndexEx
					};
					if ( !_vRet.Push( sdDecl ) ) {
						return false;
					}
					break;
				}
				default : {
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * Gets the type qualifiers from a fully specified type node.
	 *
	 * \param _ndNode The node from which to extract the type qualifiers.
	 * \return Returns the type qualifiers from the given node.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::GetTypeQualifier( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_FULLYSPECIFIEDTYPE );
		return _ndNode.nuNodeData.ui64Const;
	}

	/**
	 * Gets the type from a fully specified type node.
	 *
	 * \param _ndNode The node from which to extract the type.
	 * \return Returns the type from the given node.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::GetType( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_FULLYSPECIFIEDTYPE );
		uint32_t ui32Index = _ndNode.nueNodeDataEx.ui32NodeIndexEx;
		assert( GetNode( ui32Index ).nNodeType == LSG_N_TYPESPECIFIERNOPREC );
		return GetNode( ui32Index ).nuNodeData.ui64Const;
	}

	/**
	 * Gets a name from an identifier node.
	 *
	 * \param _ndNode The node from which to extract the name.
	 * \return Returns the name of the given node.
	 */
	const CString & LSE_CALL CShaderLanguageParser::GetName( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		return GetString( GetNameIndex( _ndNode ) );
	}

	/**
	 * Gets the index of a name from an identifier node.
	 *
	 * \param _ndNode The node from which to extract the name index.
	 * \return Returns the index of the name of the given node.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetNameIndex( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_IDENTIFIER );
		uint32_t ui32Name = _ndNode.nuNodeData.ui32IdentifierIndex;
		// Found the index of it in the identifier list.  Now get the index of the string.
		ui32Name = GetIdentifier( ui32Name ).ui32Name;
		return ui32Name;
	}

	/**
	 * Gets the semantics node from a parameter declaration (LSG_N_PARMDECLARATION) node.
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the semantic type and the semantic numeric, with the numeric being stored in the upper 32 bits of the return value.
	 *	If there is no semantic, LSE_MAXU64 is returned.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::GetSemantic( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );
		if ( GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).ui32SingleDeclName == LSE_MAXU32 ) {
			// No semantic.
			return LSE_MAXU64;
		}

		const YYSTYPE::LSG_NODE_DATA &ndSemantic = GetNode( GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).ui32SingleDeclName );
		return ndSemantic.nueNodeDataEx.ui32NodeIndexEx | (static_cast<uint64_t>(ndSemantic.nuoOp.ui32NodeIndex3) << 32ULL);
	}

	/**
	 * Gets the type qualifiers from a parameter declaration (LSG_N_PARMDECLARATION) node. 
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the type qualifiers on the given node.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::GetTypeQualifiersOnParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );
		return _ndNode.nuNodeData.ui64Const;
	}

	/**
	 * Gets the type from a parameter declaration (LSG_N_PARMDECLARATION) node. 
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the type on the given node.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::GetTypeOnParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );
		uint32_t ui32Index = GetNode( GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).nueNodeDataEx.ui32NodeIndexEx ).ui32NodeIndex;
		return GetNode( ui32Index ).nuNodeData.ui64Const;
	}

	/**
	 * Gets the name of a parameter declaration (LSG_N_PARMDECLARATION) node. 
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the name of the given node.
	 */
	const CString & LSE_CALL CShaderLanguageParser::GetNameOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );
		return GetName( GetNode( GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).nuNodeData.ui32IdentifierIndex ) );
	}

	/**
	 * Gets the name index of a parameter declaration (LSG_N_PARMDECLARATION) node. 
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the name index of the given node.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetNameIndexOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );
		return GetNameIndex( GetNode( GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).nuNodeData.ui32IdentifierIndex ) );
	}

	/**
	 * Gets the array index of a parameter declaration (LSG_N_PARMDECLARATION) node. 
	 *
	 * \param _ndNode Node to search.
	 * \return Returns the array index of the given node.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetArrayIndexOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const {
		assert( _ndNode.nNodeType == LSG_N_PARMDECLARATION );

		return GetNode( _ndNode.nueNodeDataEx.ui32NodeIndexEx ).nuoOp.ui32NodeIndex3;
	}

	/**
	 * Register a user type.  This is always a structure.
	 *
	 * \param _pcText The type name.
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::RegisterStructType( const char * _pcText ) {
		CString sThis = _pcText;
		sThis.Snap();
		if ( !m_mUserTypes.Insert( sThis, m_ui32TypeId++ ) ) { return false; }
		return true;
	}

	/**
	 * Gets the type ID for a given user-defined type.  Returns 0 if no such type exists.
	 *
	 * \param _pcText The type name.
	 * \return Returns the type ID if the given type name exists or 0 if it does not.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetStructTypeId( const char * _pcText ) const {
		CString sThis( _pcText );
		uint32_t ui32Index;
		if ( !m_mUserTypes.GetItemIndex( sThis, ui32Index ) ) { return 0; }
		return m_mUserTypes.GetByIndex( ui32Index );
	}

	/**
	 * Gets a structure name by its type ID.
	 *
	 * \param _ui32Id ID of the user-defined structure to get.
	 * \return Returns the name of the structure if found, or an empty string if not.
	 */
	CString LSE_CALL CShaderLanguageParser::GetStructById( uint32_t _ui32Id ) const {
		for ( uint32_t I = m_mUserTypes.Length(); I--; ) {
			if ( m_mUserTypes.GetByIndex( I ) == _ui32Id ) { return m_mUserTypes.GetKeyByIndex( I ); }
		}
		return CString();
	}

	/**
	 * Gets the size of a type.  For structures, elements are always packed on 16-byte boundaries, and the
	 *	size returned will be rounded up to the next 16-byte boundary.
	 *
	 * \param _ui64Type The type whose size is to be obtained.
	 * \return Returns the size of the given type or LSE_MAXU32 if the type is not defined.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetSize( uint64_t _ui64Type ) const {
		uint32_t ui32BasicId = static_cast<uint32_t>(_ui64Type & 0xFFFFFFFFUL);
		uint32_t ui32Array0 = GetArrayUnit( _ui64Type, 0 );
		uint32_t ui32Array1 = GetArrayUnit( _ui64Type, 1 );
		if ( ui32Array0 == LSE_MAXU32 ) {
			ui32Array0 = 1;
		}
		if ( ui32Array1 == LSE_MAXU32 ) {
			ui32Array1 = 1;
		}

		switch ( ui32BasicId ) {
			case CShaderParser::token::LSG_SLT_VOID : {
				return LSE_MAXU32;
			}
			case CShaderParser::token::LSG_SLT_SAMPLER : {}
			case CShaderParser::token::LSG_SLT_TEXTURE1D : {}
			case CShaderParser::token::LSG_SLT_TEXTURE2D : {}
			case CShaderParser::token::LSG_SLT_TEXTURE3D : {}
			case CShaderParser::token::LSG_SLT_TEXTURECUBE : {
				return sizeof( uint32_t ) * ui32Array0 * ui32Array1;
			}
			case CShaderParser::token::LSG_SLT_FLOAT : {
				return sizeof( float ) * ui32Array0 * ui32Array1;
			}
			case CShaderParser::token::LSG_SLT_INT : {}
			case CShaderParser::token::LSG_SLT_UINT : {
				return sizeof( uint32_t ) * ui32Array0 * ui32Array1;
			}
			case CShaderParser::token::LSG_SLT_BOOL : {
				return sizeof( uint8_t ) * ui32Array0 * ui32Array1;
			}
			default : {
				// It is a structure.  Run through every member on it and accumulate sizes, but keep each
				//	member on a 16-byte boundary.
				CString sName = GetStructById( ui32BasicId );
				if ( sName.Length() == 0 ) { return LSE_MAXU32; }

				uint32_t ui32Index;
				if ( !m_saStructures.GetItemIndex( sName, ui32Index ) ) {
					return LSE_MAXU32;
				}
				const LSG_STRUCT & sThis = m_saStructures.GetByIndex( ui32Index );

				uint32_t ui32Total = 0;
				for ( uint32_t I = 0; I < sThis.vMembers.Length(); ++I ) {
					uint32_t ui32ThisSize = GetSize( sThis.vMembers[I].ui64Type );
					if ( GetArrayUnit( sThis.vMembers[I].ui64Type, 0 ) != LSE_MAXU32 ) {
						// Vectors and matrixes must start on 16-byte boundaries.
						ui32Total = LSE_ROUND_UP( ui32Total, 16 );
						// And arrays of these as well.
						if ( sThis.vMembers[I].ui32Array != LSE_MAXU32 ) {
							ui32ThisSize = LSE_ROUND_UP( ui32ThisSize, 16 );
							ui32ThisSize *= sThis.vMembers[I].ui32Array;
						}
					}
					else if ( sThis.vMembers[I].ui32Array != LSE_MAXU32 ) {
						ui32ThisSize *= sThis.vMembers[I].ui32Array;
					}
					ui32Total += ui32ThisSize;
				}
				return LSE_ROUND_UP( ui32Total, 16 );
			}
		}
	}

	/**
	 * Gets a constant reference to the multi-map of referenced functions.
	 *
	 * \return Returns a constant reference to the multi-map of referenced functions.
	 */
	const CMultiMap<uint32_t, uint32_t> & LSE_CALL CShaderLanguageParser::GetFuncRefs() const {
		return m_mmFuncRefs;
	}

	/**
	 * Is the given type an integral type?
	 *
	 * \param _ui64Type The type to check for being an integer.
	 * \return Returns true if the given type is an integral type that is not an array.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsInteger( uint64_t _ui64Type ) {
		if ( GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 || GetArrayUnit( _ui64Type, 1 ) != LSE_MAXU32 ) { return false; }

		// It is not an array.  Check the type.
		switch ( GetType( _ui64Type ) ) {
			case LSG_NC_SIGNED : {}
			case LSG_NC_UNSIGNED : { return true; }
			default : { return false; }
		}
	}

	/**
	 * Gets the length of an array unit of a type.
	 *
	 * \param _ui64Type The type whose array unit is to be obtained.
	 * \param _ui32Unit The array unit to be obtained.
	 * \return Returns the array unit of a given type.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetArrayUnit( uint64_t _ui64Type, uint32_t _ui32Unit ) {
		uint32_t ui32Ret = static_cast<uint32_t>(_ui64Type >> (_ui32Unit * 16ULL + 32) & 0xFFFFULL);
		if ( ui32Ret == 0xFFFFULL ) { return LSE_MAXU32; }
		return ui32Ret;
	}

	/**
	 * Gets the type part of a type + array-unit combination.
	 *
	 * \param _ui64Type The type whose type data is to be obtained.
	 * \return Returns the type part of the combination.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::GetType( uint64_t _ui64Type ) {
		return static_cast<uint32_t>(_ui64Type & 0xFFFFFFFFULL);
	}

	/**
	 * Combine a type specifier with array specifiers to create a value that contains both.
	 *
	 * \param _ui64Type Type specifier.
	 * \param _ui32Array0 Array value 1.
	 * \param _ui32Array1 Array value 2.  Must be LSE_MAXU32 if not used.
	 * \return Returns the value created by combining the input values into a single uint64_t.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::CombineTypesAndArrays( uint64_t _ui64Type, uint32_t _ui32Array0, uint32_t _ui32Array1 ) {
		_ui64Type |= (static_cast<uint64_t>(_ui32Array0) & 0xFFFFULL) << 32ULL;
		_ui64Type |= (static_cast<uint64_t>(_ui32Array1) & 0xFFFFULL) << (32ULL + 16ULL);
		return _ui64Type;
	}

	/**
	 * Converts a type to a numeric-constant type.  Returns LSG_NC_INVALID if the type is not a basic type (arrays).
	 *
	 * \param _ui64Type The type to convert.
	 * \return Returns the LSG_NUM_CONSTANTS type of the given type.
	 */
	LSG_NUM_CONSTANTS LSE_CALL CShaderLanguageParser::TypeToConstantType( uint64_t _ui64Type ) {
		// If there is an array value, error out.
		if ( GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 || GetArrayUnit( _ui64Type, 1 ) != LSE_MAXU32 ) { return LSG_NC_INVALID; }
		switch ( GetType( _ui64Type ) ) {
			case CShaderParser::token::LSG_SLT_VOID : { return LSG_NC_INVALID; }
			case CShaderParser::token::LSG_SLT_FLOAT : { return LSG_NC_FLOATING; }
			case CShaderParser::token::LSG_SLT_INT : { return LSG_NC_SIGNED; }
			case CShaderParser::token::LSG_SLT_UINT : { return LSG_NC_UNSIGNED; }
			case CShaderParser::token::LSG_SLT_BOOL : { return LSG_NC_SIGNED; }
		}
		return LSG_NC_INVALID;
	}

	/**
	 * Converts a numeric-constant type to a full type that most closely matches the numeric-constant type.
	 *
	 * \param _ncType The type to convert.
	 * \return Returns the full type form that matches most closely the given numeric-constant type.
	 */
	uint64_t LSE_CALL CShaderLanguageParser::ConstantTypeToType( LSG_NUM_CONSTANTS _ncType ) {
		switch ( _ncType ) {
			case LSG_NC_INVALID : { return CombineTypesAndArrays( CShaderParser::token::LSG_SLT_VOID, LSE_MAXU32, LSE_MAXU32 ); }
			case LSG_NC_FLOATING : { return CombineTypesAndArrays( CShaderParser::token::LSG_SLT_FLOAT, LSE_MAXU32, LSE_MAXU32 ); }
			case LSG_NC_SIGNED : { return CombineTypesAndArrays( CShaderParser::token::LSG_SLT_INT, LSE_MAXU32, LSE_MAXU32 ); }
			case LSG_NC_UNSIGNED : { return CombineTypesAndArrays( CShaderParser::token::LSG_SLT_UINT, LSE_MAXU32, LSE_MAXU32 ); }
		}
		return CombineTypesAndArrays( CShaderParser::token::LSG_SLT_VOID, LSE_MAXU32, LSE_MAXU32 );
	}

	/**
	 * Gets the name of a basic type.
	 *
	 * \param _ui64Type The type whose name is to be obtained.
	 * \return Returns the type in string form.
	 */
	CString LSE_CALL CShaderLanguageParser::BasicTypeToString( uint64_t _ui64Type ) {
		CString sRet;
		switch ( GetType( _ui64Type ) ) {
			case CShaderParser::token::LSG_SLT_VOID : {
				sRet = "void";
				break;
			}
			case CShaderParser::token::LSG_SLT_FLOAT : {
				sRet = "float";
				break;
			}
			case CShaderParser::token::LSG_SLT_INT : {
				sRet = "int";
				break;
			}
			case CShaderParser::token::LSG_SLT_UINT : {
				sRet = "uint";
				break;
			}
			case CShaderParser::token::LSG_SLT_BOOL : {
				sRet = "bool";
				break;
			}
			default : { return sRet; }
		}

		if ( GetArrayUnit( _ui64Type, 0 ) != LSE_MAXU32 ) {
			sRet += CString( "[" ) + CString::CreateFromUI64( GetArrayUnit( _ui64Type, 0 ) ) + CString( "]" );
		}
		if ( GetArrayUnit( _ui64Type, 1 ) != LSE_MAXU32 ) {
			sRet += CString( "[" ) + CString::CreateFromUI64( GetArrayUnit( _ui64Type, 1 ) ) + CString( "]" );
		}
		return sRet;
	}

	/**
	 * Converts from one scalar type to another.
	 *
	 * \param _sScalar The value to cast.
	 * \param _ui64OriginalType The type of value in _sScalar.
	 * \param _ui64NewType The type to which to cast the given scalar value.
	 * \param _bRes If set to true, the function succeeded in converting the values.
	 * \return Returns the scalar cast to the given type.
	 */
	CShaderLanguageParser::LSG_SCALAR LSE_CALL CShaderLanguageParser::CastScalar( const LSG_SCALAR &_sScalar, uint64_t _ui64OriginalType, uint64_t _ui64NewType, LSBOOL &_bRes ) {
		LSG_NUM_CONSTANTS ncFrom = TypeToConstantType( _ui64OriginalType );
		LSG_NUM_CONSTANTS ncTo = TypeToConstantType( _ui64NewType );
		LSG_SCALAR sRet;
		sRet.ui64Value = 0ULL;

		_bRes = false;
		if ( ncFrom == LSG_NC_INVALID || ncTo == LSG_NC_INVALID ) {
			sRet.ui64Value = 0ULL;
			return sRet;
		}
		switch ( GetType( _ui64NewType ) ) {
			case CShaderParser::token::LSG_SLT_BOOL : {
				switch ( GetType( _ui64OriginalType ) ) {
					case CShaderParser::token::LSG_SLT_BOOL : {
						sRet.i64Value = _sScalar.i64Value ? 1LL : 0LL;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_FLOAT : {
						sRet.i64Value = _sScalar.dValue ? 1LL : 0LL;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_INT : {
						sRet.i64Value = _sScalar.i64Value ? 1LL : 0LL;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_UINT : {
						sRet.i64Value = _sScalar.ui64Value ? 1LL : 0LL;
						_bRes = true;
						break;
					}
				}
			}

			case CShaderParser::token::LSG_SLT_FLOAT : {
				switch ( GetType( _ui64OriginalType ) ) {
					case CShaderParser::token::LSG_SLT_BOOL : {
						sRet.dValue = _sScalar.i64Value ? 1.0 : 0.0;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_FLOAT : {
						sRet.dValue = _sScalar.dValue;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_INT : {
						sRet.dValue = static_cast<double>(_sScalar.i64Value);
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_UINT : {
						sRet.dValue = static_cast<double>(_sScalar.ui64Value);
						_bRes = true;
						break;
					}
				}
			}

			case CShaderParser::token::LSG_SLT_INT : {
				switch ( GetType( _ui64OriginalType ) ) {
					case CShaderParser::token::LSG_SLT_BOOL : {
						sRet.i64Value = _sScalar.i64Value ? 1LL : 0LL;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_FLOAT : {
						sRet.i64Value = static_cast<int64_t>(_sScalar.dValue);
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_INT : {
						sRet.i64Value = _sScalar.i64Value;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_UINT : {
						sRet.i64Value = _sScalar.ui64Value;
						_bRes = true;
						break;
					}
				}
			}
			
			case CShaderParser::token::LSG_SLT_UINT : {
				switch ( GetType( _ui64OriginalType ) ) {
					case CShaderParser::token::LSG_SLT_BOOL : {
						sRet.ui64Value = _sScalar.i64Value ? 1LL : 0LL;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_FLOAT : {
						sRet.ui64Value = static_cast<uint64_t>(_sScalar.dValue);
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_INT : {
						sRet.ui64Value = _sScalar.i64Value;
						_bRes = true;
						break;
					}
					case CShaderParser::token::LSG_SLT_UINT : {
						sRet.ui64Value = _sScalar.ui64Value;
						_bRes = true;
						break;
					}
				}
			}
		}

		return sRet;
	}

	/**
	 * Gets the best type between the two given types.  In order: bool, int, uint, float.
	 *
	 * \param _ncLeft The left type.
	 * \param _ncRight The right type.
	 * \return Returns the highest precision type between the two given types.
	 */
	LSG_NUM_CONSTANTS LSE_CALL CShaderLanguageParser::GetHighType( LSG_NUM_CONSTANTS _ncLeft, LSG_NUM_CONSTANTS _ncRight ) {
		if ( _ncLeft == LSG_NC_FLOATING || _ncRight == LSG_NC_FLOATING ) { return LSG_NC_FLOATING; }
		if ( _ncLeft == LSG_NC_UNSIGNED || _ncRight == LSG_NC_UNSIGNED ) { return LSG_NC_UNSIGNED; }
		return LSG_NC_SIGNED;
	}

	/**
	 * Is one type or the other a matrix?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if either type is a matrix type.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsEitherMatrix( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) != LSE_MAXU32) ||
			(GetArrayUnit( _ui64Right, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) != LSE_MAXU32);
	}

	/**
	 * Are both types matrices?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if both types are matrices.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::BothAreMatrices( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) != LSE_MAXU32) &&
			(GetArrayUnit( _ui64Right, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) != LSE_MAXU32);
	}

	/**
	 * Is one type or the other a vector?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if either type is a vector type.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsEitherVector( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) == LSE_MAXU32) ||
			(GetArrayUnit( _ui64Right, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) == LSE_MAXU32);
	}

	/**
	 * Are both types vectors?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if both types are vectors.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::BothAreVectors( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) == LSE_MAXU32) &&
			(GetArrayUnit( _ui64Right, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) == LSE_MAXU32);
	}

	/**
	 * Is one type or the other a scalar?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if either type is a scalar type.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsEitherScalar( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) == LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) == LSE_MAXU32) ||
			(GetArrayUnit( _ui64Right, 0 ) == LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) == LSE_MAXU32);
	}

	/**
	 * Are both types scalars?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if both types are scalars.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::BothAreScalars( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return (GetArrayUnit( _ui64Left, 0 ) == LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) == LSE_MAXU32) &&
			(GetArrayUnit( _ui64Right, 0 ) == LSE_MAXU32 && GetArrayUnit( _ui64Right, 1 ) == LSE_MAXU32);
	}

	/**
	 * Is one type or the other a structure?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if either type is a structure type.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::IsEitherStruct( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return GetType( _ui64Left ) >= LSG_SB_START || GetType( _ui64Right ) >= LSG_SB_START;
	}

	/**
	 * Are both types structures?
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \return Returns true if both types are structures.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::BothAreStructs( uint64_t _ui64Left, uint64_t _ui64Right ) {
		return GetType( _ui64Left ) >= LSG_SB_START && GetType( _ui64Right ) >= LSG_SB_START;
	}

	/**
	 * Is the given type a sampler?
	 *
	 * \param _ui64Type Type to check for being a sampler.
	 * \return Returns true if the given type is a sampler.
	 */
	LSBOOL LSE_FCALL CShaderLanguageParser::IsSampler( uint64_t _ui64Type ) {
		return _ui64Type == CombineTypesAndArrays( CShaderParser::token::LSG_SLT_SAMPLER, LSE_MAXU32, LSE_MAXU32 );
	}

	/**
	 * Is the given type a texture?
	 *
	 * \param _ui64Type Type to check for being a texture.
	 * \return Returns true if the given type is a texture.
	 */
	LSBOOL LSE_FCALL CShaderLanguageParser::IsTexture( uint64_t _ui64Type ) {
		return (_ui64Type == CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE1D, LSE_MAXU32, LSE_MAXU32 )) ||
			(_ui64Type == CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE2D, LSE_MAXU32, LSE_MAXU32 )) ||
			(_ui64Type == CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURE3D, LSE_MAXU32, LSE_MAXU32 )) ||
			(_ui64Type == CombineTypesAndArrays( CShaderParser::token::LSG_SLT_TEXTURECUBE, LSE_MAXU32, LSE_MAXU32 ));
	}

	/**
	 * Gets the matrix dimensions from the type that is a matrix.  Exactly one of the types must be a matrix.
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \param _ui32Columns Number of columns.
	 * \param _ui32Rows Number of rows.
	 */
	void LSE_CALL CShaderLanguageParser::GetMatrixSize( uint64_t _ui64Left, uint64_t _ui64Right,
		uint32_t &_ui32Columns, uint32_t &_ui32Rows ) {
		if ( GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) != LSE_MAXU32 ) {
			// The left side is the matrix.
			_ui32Columns = GetArrayUnit( _ui64Left, 0 );
			_ui32Rows = GetArrayUnit( _ui64Left, 1 );
		}
		else {
			// The right side is the matrix.
			_ui32Columns = GetArrayUnit( _ui64Right, 0 );
			_ui32Rows = GetArrayUnit( _ui64Right, 1 );
		}
	}

	/**
	 * Gets the vector dimension from the type that is a vector.  Exactly one of the types must be a vector.
	 *
	 * \param _ui64Left Left type.
	 * \param _ui64Right Right type.
	 * \param _ui32Ret Number of scalars in the vector type.
	 */
	void LSE_CALL CShaderLanguageParser::GetVectorSize( uint64_t _ui64Left, uint64_t _ui64Right,
		uint32_t &_ui32Ret ) {
		if ( GetArrayUnit( _ui64Left, 0 ) != LSE_MAXU32 && GetArrayUnit( _ui64Left, 1 ) == LSE_MAXU32 ) {
			// The left side is the vector.
			_ui32Ret = GetArrayUnit( _ui64Left, 0 );
		}
		else {
			// The right side is the vector.
			_ui32Ret = GetArrayUnit( _ui64Right, 0 );
		}
	}

	/**
	 * Adds a node to the stack.  Set the node index too.
	 *
	 * \param _ndNode The node to add to the stack.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::AddNode( LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ui32NodeIndex = m_vNodes.Length();
		_ndNode.ui32NodeLine = m_plLexer->lineno();
		if ( m_plLexer->get_file().size() ) {
			_ndNode.ui32NodeFile = CAlgorithm::Find( m_vStringPool, CString( m_plLexer->get_file().c_str() ) );
			if ( _ndNode.ui32NodeFile == m_vStringPool.Length() ) {
				// Not there so add it.
				if ( !m_vStringPool.Push( CString( m_plLexer->get_file().c_str() ) ) ) { return LSE_MAXU32; }
			}
		}
		else {
			_ndNode.ui32NodeFile = LSE_MAXU32;
		}
		return m_vNodes.Push( _ndNode );
	}

	/**
	 * Adds an identifier by name.  Returns the index of the created identifier in the identifier stack.
	 *	The type of the created identifier is undefined.
	 *
	 * \param _pcText The string name of the identifier.
	 * \return Returns the index of the created identifier.
	 */
	uint32_t LSE_CALL CShaderLanguageParser::AddIdentifier( const char * _pcText ) {
		uint32_t ui32Ret = m_vIdentifiers.Length();
		LSG_IDENTIFIER iIdentifier;
		// Find the name in the existing stack of strings if it is there.
		iIdentifier.ui32Name = CAlgorithm::Find( m_vStringPool, CString( _pcText ) );
		if ( iIdentifier.ui32Name == m_vStringPool.Length() ) {
			// Not there so add it.
			if ( !m_vStringPool.Push( _pcText ) ) { return LSE_MAXU32; }
		}



		iIdentifier.ui8Type = LSG_IDENTIFIER::LSG_IT_UNDEFINED;
		if ( !m_vIdentifiers.Push( iIdentifier ) ) { return LSE_MAXU32; }
		return ui32Ret;
	}

	/**
	 * Gets the index of a token or the index where the token should be added if it is not in the list of tokens.
	 *
	 * \param _pcName Name of the token to find.
	 * \param _ui32Index Holds the returned index.
	 * \return Returns true if the token is in the list, false otherwise.
	 */
	LSBOOL LSE_CALL CShaderLanguageParser::FindToken( const char * _pcName, uint32_t &_ui32Index ) const {
		uint32_t ui32Hi = m_vSortedTokens.Length();
		if ( !ui32Hi ) {
			_ui32Index = 0;
			return false;
		}
		// Handle the worst case also.
		/*if ( m_vSortedTokens[ui32Hi-1] < _tVal ) {
			// It goes at the end of the list.
			_ui32Index = ui32Hi;
			return false;
		}*/

		uint32_t ui32Low = 0;
		

		while ( true ) {
			uint32_t ui32Index = ((ui32Hi - ui32Low) >> 1) + ui32Low;
			int32_t i32Cmp = CStd::StrCmp( m_vStringPool[m_vSortedTokens[ui32Index]].CStr(), _pcName );
			if ( i32Cmp == 0 ) {
				_ui32Index = ui32Index;
				return true;
			}
			if ( i32Cmp < 0 ) {
				// Move up.
				if ( ui32Index == ui32Low ) {
					_ui32Index = ui32Index + 1;
					return false;
				}
				ui32Low = ui32Index;
			}
			else {
				// Move down.
				if ( ui32Index == ui32Hi ) {
					_ui32Index = ui32Index;
					return false;
				}
				ui32Hi = ui32Index;
			}
		}

		/*if ( !m_vSortedTokens.Length() ) {
			_ui32Index = 0;
			return false;
		}

		// Do a binary search for the key.
		uint32_t ui32Jmp = m_vSortedTokens.Length() >> 1;
		uint32_t ui32IndexToCheck = -static_cast<int32_t>(ui32Jmp);
		do {
			ui32IndexToCheck += ui32Jmp;
			if ( ui32IndexToCheck >= m_vSortedTokens.Length() ) {
				// Passed the end of the list.  Go back.
				ui32IndexToCheck -= ui32Jmp;
				ui32Jmp >>= 1;
				ui32Jmp = CStd::Min<uint32_t>( ui32Jmp, m_vSortedTokens.Length() - ui32IndexToCheck - 1 );
				continue;
			}
			int32_t i32Cmp = CStd::StrCmp( m_vStringPool[m_vSortedTokens[ui32IndexToCheck]].CStr(), _pcName );
			if ( i32Cmp == 0 ) {
				_ui32Index = ui32IndexToCheck;
				return true;
			}
			// Check if we passed the item we want to find.
			if ( i32Cmp > 0 ) {
				// If the jump is already 0, we can leave here with this being the final index.
				if ( ui32Jmp == 0 || ui32IndexToCheck == 0 ) {
					_ui32Index = ui32IndexToCheck;
					return false;
				}
				// Go back and jump by half as much.
				ui32IndexToCheck -= ui32Jmp;
				ui32Jmp >>= 1;
				ui32Jmp = CStd::Min<uint32_t>( ui32Jmp, m_vSortedTokens.Length() - ui32IndexToCheck - 1 );
				continue;
			}
			if ( ui32Jmp == 0 ) {
				// The given key is greater than the current index.  It should be inserted after.
				_ui32Index = ui32IndexToCheck + 1;
				return false;
			}
		} while ( true );*/
	}

}	// namespace lsg

#pragma warning( pop )
