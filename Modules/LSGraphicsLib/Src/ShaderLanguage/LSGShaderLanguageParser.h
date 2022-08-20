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


#ifndef __LSG_SHADERLANGUAGEPARSER_H__
#define __LSG_SHADERLANGUAGEPARSER_H__

#include "../LSGGraphicsLib.h"
#include "Algorithm/LSTLAlgorithm.h"
#include "LSGShaderScopeStack.h"
#include "LSGShaderSyntaxNodes.h"
#include "String/LSTLString.h"
#include "Vector/LSTLVector.h"

namespace lsg {

	class CShaderLexer;

	// == Enumerations.
	/**
	 * Shader types.
	 */
	enum LSG_SHADER_TYPES {
		LSG_ST_INVALID							= -1,
		LSG_ST_VERTEX,							/**< Vertex shader. */
		LSG_ST_PIXEL,							/**< Pixel shader. */

		LSG_ST_TOTAL,							// Shader types must be from 0 to X, as the enumerated values are
												//	used as indices in some places.
	};

	/**
	 * Class CShaderLanguageParser
	 * \brief Manages all information related to a parsed shader file.
	 *
	 * Description: Handles the meat of parsing a shader file.  Bison performs the actual parsing, but all nodes
	 *	are sent to this class to be managed.  This is the go-to class for any information needed on a parsed shader.
	 */
	class CShaderLanguageParser {
	public :
		// == Enumerations.
		/**
		 * Structure base ID.
		 */
		enum LSG_STRUCT_BASE {
			LSG_SB_START							= 10000
		};


		// == Various constructors.
		LSE_CALLCTOR								CShaderLanguageParser( LSG_SHADER_TYPES _stType, CShaderLexer * _plLexer );
		
		
		// == Types.
		/**
		 * Identifier data.
		 */
		typedef struct LSG_IDENTIFIER {
			// == Enumerations.
			enum LSG_IDENTIFIER_TYPE {
				LSG_IT_UNDEFINED,
				LSG_IT_INTEGRAL,
				LSG_IT_FLOATING,
				LSG_IT_VECTOR,
				LSG_IT_MATRIX,
				LSG_IT_STRING,
			};

			
			// == Members.
			/**
			 * The name of the identifier.  Indexed into the string stack.
			 */
			uint32_t								ui32Name;

			/**
			 * The identifier type.
			 */
			uint8_t									ui8Type;
		} * LPLSG_IDENTIFIER, * const LPCLSG_IDENTIFIER;

		/**
		 * A scalar type.
		 */
		typedef union LSG_SCALAR {
			/**
			 * In the form of an unsigned integer.
			 */
			uint64_t								ui64Value;

			/**
			 * In the form of a signed integer.
			 */
			int64_t									i64Value;

			/**
			 * In the form of a floating-point number.
			 */
			double								dValue;
		} * LPLSG_SCALAR, * const LPCLSG_SCALAR;

		/**
		 * A structure member.
		 */
		typedef struct LSG_STRUCT_MEMBER {
			/**
			 * Name of the member.
			 */
			CString									sName;

			/**
			 * Type.
			 */
			uint64_t								ui64Type;

			/**
			 * Array size.
			 */
			uint32_t								ui32Array;


			// == Operators.
			/**
			 * Assignment operator.
			 *
			 * \param _smSource The object to copy.
			 * \return Returns this object after the copy.
			 */
			LSG_STRUCT_MEMBER & LSE_CALL			operator = ( const LSG_STRUCT_MEMBER &_smSource );
		} * LPLSG_STRUCT_MEMBER, * const LPCLSG_STRUCT_MEMBER;

		/**
		 * A structure.
		 */
		typedef struct LSG_STRUCT {
			/**
			 * Name.
			 */
			CString									sName;

			/**
			 * Array of members.
			 */
			CVector<LSG_STRUCT_MEMBER>				vMembers;


			// == Operators.
			/**
			 * Assignment operator.
			 *
			 * \param _sSource The object to copy.
			 * \return Returns this object after the copy.
			 */
			LSG_STRUCT & LSE_CALL					operator = ( const LSG_STRUCT &_sSource );

			/**
			 * Less-than operator.
			 *
			 * \param _sSource The object against which to compare.
			 * \return Returns the lexicographical relationship between the names of the two objects.
			 */
			LSBOOL LSE_CALL							operator < ( const LSG_STRUCT &_sSource );
		} * LPLSG_STRUCT, * const LPCLSG_STRUCT;

		/**
		 * An array of structures.
		 */
		typedef CLinearMap<CString, LSG_STRUCT>			LSG_STRUCT_ARRAY, * LPLSG_STRUCT_ARRAY, * const LPCLSG_STRUCT_ARRAY;

		/**
		 * Structure declarator.
		 */
		typedef struct LSG_STRUCT_DECLARATOR {
			/**
			 * Name.
			 */
			uint32_t								ui32Name;

			/**
			 * Array.
			 */
			uint32_t								ui32Array;
		} * LPLSG_STRUCT_DECLARATOR, * const LPCLSG_STRUCT_DECLARATOR;

		/**
		 * An engine-defined macro.
		 */
		typedef struct LSG_ENGINE_MACRO {
			/**
			 * Macro name.
			 */
			const char *							pcName;

			/**
			 * Macro value.
			 */
			uint32_t								ui32Value;
		} * LPLSG_ENGINE_MACRO, * const LPCLSG_ENGINE_MACRO;


		// == Functions.
		/**
		 * Gets the shader type.
		 *
		 * \return Returns the shader type.
		 */
		LSG_SHADER_TYPES LSE_CALL					GetShaderType() const;

		/**
		 * Accepts a string from code in a shader, converts it to binary (for example, \r is
		 *	converted to character 0x0D), and returns the index of the string where it was stored
		 *	in the pool of strings.
		 *
		 * \param _pcText The string from the code to convert and store.
		 * \param _bHasQuotes If true, the first and last characters are assumed to be quotation marks and not part of the string itself.
		 * \return Returns the index of the stored string or LSE_MAXU32 on error.
		 */
		uint32_t LSE_CALL							CreateString( const char * _pcText, LSBOOL _bHasQuotes = true );

		/**
		 * Accepts a string representation of a decimal constant and stores the result in the
		 *	given node.
		 *
		 * \param _pcText String constant containing the numeric.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateDecimal( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Accepts a representation of a floating-point constant and stores the result in the
		 *	given node.
		 *
		 * \param _pcText String constant containing the numeric.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFloatingPoint( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a node that represents either boolean true or boolean false.
		 *
		 * \param _bValue The boolean value of the node.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateBool( LSBOOL _bValue, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a node pointing to an identifier (by index into the identifier stack).
		 *
		 * \param _pcText The string name of the identifier.
		 * \param _ndNode Node to create based off the string constant.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateIdentifier( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an array-access node.
		 *
		 * \param _ndBase Base part of the expression.
		 * \param _ndIndex Array index part of the expression.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateArrayAccess( const YYSTYPE::LSG_NODE_DATA &_ndBase,
			const YYSTYPE::LSG_NODE_DATA &_ndIndex, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a postfix or prefix ++ or -- node.
		 *
		 * \param _ndOp The left operand.
		 * \param _nType Type of operation.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreatePostfixOrPrefixIncOrDec( const YYSTYPE::LSG_NODE_DATA &_ndOp,
			LSG_NODE _nType, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a unary !, ~, -, or + node.
		 *
		 * \param _ui32Op The operator.
		 * \param _ndBase The base expression modified by the operator.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateUnaryOp( uint32_t _ui32Op, const YYSTYPE::LSG_NODE_DATA &_ndBase,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a cast expression.
		 *
		 * \param _ui64Type The type to which to cast the expression.
		 * \param _ndBase The base expression.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateCast( uint64_t _ui64Type, const YYSTYPE::LSG_NODE_DATA &_ndBase,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

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
		LSBOOL LSE_CALL								CreateMath( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			uint32_t _ui32Op, YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a tertiary ?: expression.
		 *
		 * \param _ndCond The conditional operand.
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateTertiary( const YYSTYPE::LSG_NODE_DATA &_ndCond,
			const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an assignment node.
		 *
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ui32Op The operator.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateAssignment( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			uint32_t _ui32Op,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a comma sequence.
		 *
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateComma( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an intrinsic function.
		 *
		 * \param _ui32Intrinsic The intrinsic to create.
		 * \param _ndExp The parameters for the intrinsic function.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateIntrinsic( uint32_t _ui32Intrinsic,
			const YYSTYPE::LSG_NODE_DATA &_ndExp,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Make an argument expression list.
		 *
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateArgExpList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a function-call node.
		 *
		 * \param _ndFunction The left operand.
		 * \param _ui32ArgList The index of the optional argument list.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFunctionCall( const YYSTYPE::LSG_NODE_DATA &_ndFunction,
			uint32_t _ui32ArgList,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a field-select node.
		 *
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFieldSelect( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a constructor node.
		 *
		 * \param _ui64Type The type ID, including implicit arrays.
		 * \param _ui32Args Index of the argument-list node if any.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateConstructor( uint64_t _ui64Type,
			uint32_t _ui32Args,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an expression statement.
		 *
		 * \param _ui32Index Index of the expression or LSE_MAXU32
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateExpressionStatement( uint32_t _ui32Index,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an declaration statement.
		 *
		 * \param _ndDecl The declaration node.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateDeclarationStatement( const YYSTYPE::LSG_NODE_DATA &_ndDecl,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a semantic.
		 *
		 * \param _pcText The semantic text.
		 * \param _ui32Type The semantic type.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateSemantic( const char * _pcText,
			uint32_t _ui32Type,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a type-specifier node without a precision qualifier.  Precision qualifiers do not
		 *	currently exist but may in the future.
		 *
		 * \param _ui64Qualifiers The type qualifiers.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateTypeSpecifier( uint64_t _ui64Qualifiers,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a fully specified type.  Such as "IN void" or "float".
		 *
		 * \param _ui64Qualifiers The type qualifiers.
		 * \param _ndSpec Type specifier.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFullySpecifiedType( uint64_t _ui64Qualifiers,
			const YYSTYPE::LSG_NODE_DATA &_ndSpec,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

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
		LSBOOL LSE_CALL								CreateSingleDeclaration( uint32_t _ui32Name,
			const YYSTYPE::LSG_NODE_DATA &_ndTypeNode,
			uint32_t _ui32ConstNodeIndex,
			uint32_t _ui32SemanticNodeIndex,
			uint32_t _ui32InitializerNodeIndex,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an initializer list.
		 *
		 * \param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateInitializerList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an initializer list, wrapped inside {}.
		 *
		 * \param _ndList The initialier list.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateInitializerListWrap( const YYSTYPE::LSG_NODE_DATA &_ndList,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a statement list.
		 *
		 * \param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStatementList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a statement with a raised scope.
		 *
		 * \param _ndStatement The statement with the increased scope.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateNewScopedStatement( const YYSTYPE::LSG_NODE_DATA &_ndStatement,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates an empty statement.
		 *
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateBlankStatement( YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a structure-declarator node.  This is the declaration of a single member of a structure
		 *	excluding type qualifiers.
		 *
		 * \param _ndName Identifier node representing the name of the member.
		 * \param _ui32ArrayNodeIndex Index of the node representing the array constant or LSE_MAXU32.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStructDeclarator( const YYSTYPE::LSG_NODE_DATA &_ndName,
			uint32_t _ui32ArrayNodeIndex,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a structure-declarator list node.
		 *
		 * \param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStructDeclaratorList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a structure declarator.  A full member or list of members with the same type specifiers and qualifiers.
		 *
		 * \param _ui64Qualifiers The qualifiers.
		 * \param _ndType The type data.
		 * \param _ndDecl The declarator node, always of type LSG_N_STRUCTDECLARATORLIST.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStructDeclaration( uint64_t _ui64Qualifiers, 
			const YYSTYPE::LSG_NODE_DATA &_ndType,
			const YYSTYPE::LSG_NODE_DATA &_ndDecl,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a structure declaration list node.
		 *
		 * \param _ndLeft The left operand.
		 * \param _ndRight The right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStructDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a structure node.
		 *
		 * \param _ui32IdentifierIndex Index of the node representing the name of the structure, if any.  LSE_MAXU32 if
		 *	the structure is nameless.
		 * \param _ndDecl The structure members.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateStruct( uint32_t _ui32IdentifierIndex,
			const YYSTYPE::LSG_NODE_DATA &_ndDecl,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

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
		LSBOOL LSE_CALL								CreateParameterDeclarator( const YYSTYPE::LSG_NODE_DATA &_ndType,
			const YYSTYPE::LSG_NODE_DATA &_ndName,
			uint32_t _ui32ArrayNodeIndex,
			uint32_t _ui32SemanticNodeIndex,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a parameter declaration node.
		 *
		 * \param _ui64TypeInfo Full type-qualifier information.
		 * \param _ndDecl Declaration node.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateParameterDeclaration( uint64_t _ui64TypeInfo,
			const YYSTYPE::LSG_NODE_DATA &_ndDecl,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a parameter declaration list.
		 *
		 * \param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateParameterDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a function-declaration node.
		 *
		 * \param _ndType Type information.
		 * \param _ndName Function name.
		 * \param _ui32Parms Optional parameters.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFunctionDeclaration( const YYSTYPE::LSG_NODE_DATA &_ndType,
			const YYSTYPE::LSG_NODE_DATA &_ndName,
			uint32_t _ui32Parms,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a function definition node.
		 *
		 * \param _ndHeader Function header and parameters.
		 * \param _ndBody Function body.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateFunctionDefinition( const YYSTYPE::LSG_NODE_DATA &_ndHeader,
			const YYSTYPE::LSG_NODE_DATA &_ndBody,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a selection (if/else) statement node.
		 *
		 * \param _ndExp The expression to evaluate.
		 * \param _ndBody The statement body.
		 * \param _ui32ElseBody The index of the else body statement if any.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateSelectionStatement( const YYSTYPE::LSG_NODE_DATA &_ndExp,
			const YYSTYPE::LSG_NODE_DATA &_ndBody,
			uint32_t _ui32ElseBody,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a for-initilializer node.
		 *
		 * \param _ndExp The initializer node.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateForInitDeclStatement( const YYSTYPE::LSG_NODE_DATA &_ndExp,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a while () node.
		 *
		 * \param _ndExp The conditional expression.
		 * \param _ndBody The body of the iterator.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateWhileLoop( const YYSTYPE::LSG_NODE_DATA &_ndExp,
			const YYSTYPE::LSG_NODE_DATA &_ndBody,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a do () while node.
		 *
		 * \param _ndExp The conditional expression.
		 * \param _ndBody The body of the iterator.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateDoWhileLoop( const YYSTYPE::LSG_NODE_DATA &_ndExp,
			const YYSTYPE::LSG_NODE_DATA &_ndBody,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

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
		LSBOOL LSE_CALL								CreateForLoop( const YYSTYPE::LSG_NODE_DATA &_ndInitExp,
			const YYSTYPE::LSG_NODE_DATA &_ndExp,
			uint32_t _ui32ItIndex,
			const YYSTYPE::LSG_NODE_DATA &_ndBody,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a jump node.
		 *
		 * \param _nType The jump node type.
		 * \param _ui32NodeIndex Index of the associated node, if there is one.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateJump( LSG_NODE _nType, uint32_t _ui32NodeIndex,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Creates a list of external declarations.
		 *
		 * \param param _ndLeft Left operand.
		 * \param _ndRight Right operand.
		 * \param _ndNode Node to create.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								CreateExternalDeclarationList( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
			const YYSTYPE::LSG_NODE_DATA &_ndRight,
			YYSTYPE::LSG_NODE_DATA &_ndNode );

		/**
		 * Find an identifier by name.
		 *
		 * \param _pcName Name of the identifier to find.
		 * \return Returns the index of the identifier with the given name in the stack of identifiers or LSE_MAXU32 on error.
		 */
		uint32_t LSE_CALL							FindIdentifier( const char * _pcName ) const;

		/**
		 * Is the given string a registered token?
		 *
		 * \param _pcText Name to check for being a token as apposed to an identifier.
		 * \return Returns true if the given string is a token name.
		 */
		LSBOOL LSE_CALL								IsToken( const char * _pcText ) const;

		/**
		 * Gets a node for read-only.
		 *
		 * \param _ui32Index Index of the node to get.
		 * \return Returns a constant reference to the requested node.
		 */
		const LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA & LSE_CALL
													GetNode( uint32_t _ui32Index ) const;

		/**
		 * Gets the total number of nodes.
		 *
		 * \return Returns the total number of nodes in the syntax tree.
		 */
		uint32_t LSE_CALL							TotalNodes() const;

		/**
		 * Gets the total identifiers.
		 *
		 * \return Returns the total number of identifiers.
		 */
		uint32_t LSE_CALL							TotalIdentifiers() const;

		/**
		 * Gets an identifier by index.
		 *
		 * \param _ui32Index Index of the identifier to get.
		 * \return Returns a constant reference to the requested identifier.
		 */
		const LSG_IDENTIFIER & LSE_CALL				GetIdentifier( uint32_t _ui32Index ) const;

		/**
		 * Gets the total strings.
		 *
		 * \return Returns the total number of strings.
		 */
		uint32_t LSE_CALL							TotalStrings() const;

		/**
		 * Gets a string by index.
		 *
		 * \param _ui32Index Index of the string to get.
		 * \return Returns a constant reference to the requested string.
		 */
		const CString & LSE_CALL					GetString( uint32_t _ui32Index ) const;

		/**
		 * Gets the type and value of a node.  The node must represent a constant numeric type.
		 *
		 * \param _ndNode The node to test.
		 * \param _sValue The value of the node, which should be reinterpreted based on the type of the node.
		 * \return Returns the type of the node.
		 */
		LSG_NUM_CONSTANTS LSE_CALL					GetNodeValue( const YYSTYPE::LSG_NODE_DATA &_ndNode, LSG_SCALAR &_sValue ) const;

		/**
		 * Determine if a given expression resolves to an integer value.
		 *
		 * \param _ndNode The node to check.
		 * \return Returns true if the expression resolves to an integer value.
		 */
		LSBOOL LSE_CALL								IsInteger( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

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
		uint32_t LSE_CALL							CountArgs( uint32_t _ui32ArgListIndex, uint32_t _ui32MaxArgs, uint32_t * _pui32Nodes ) const;

		/**
		 * Count the number of parameters in the given argument list.  Optionally returns the argument node indices in the
		 *	optionally supplied vector in reverse order.
		 *
		 * \param _ui32ArgListIndex Index of the argument list whose nodes are to be counted.
		 * \param _pvRet If not NULL, filled with the arguments in reverse order upon return.
		 * \return return Returns the number of arguments found or LSE_MAXU32 if there is not enough memory to add the indices to the given vector.
		 */
		uint32_t LSE_CALL							CountParms( uint32_t _ui32ArgListIndex, CVectorPoD<uint32_t, uint32_t> * _pvRet = NULL );

		/**
		 * Count the number of members on a structure.  Optionally returns the member declarations in reverse order.
		 *
		 * \param _ui32MemberListIndex Index of the structure declaration list whose nodes are to be counted.
		 * \param _pvRet If not NULL, filled with the arguments in reverse order upon return.
		 * \return return Returns the number of members found or LSE_MAXU32 if there is not enough memory to add the indices to the given vector.
		 */
		uint32_t LSE_CALL							CountStructMembers( uint32_t _ui32MemberListIndex, CVectorPoD<uint32_t, uint32_t> * _pvRet = NULL );

		/**
		 * Gets the declarators on a structure declaration.
		 *
		 * \param _ui32Index Index of the structure declaration.
		 * \param _vRet Returned array of structure declarators.
		 * \return Returns true if there was enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								GetStructDeclarators( uint32_t _ui32Index, CVectorPoD<LSG_STRUCT_DECLARATOR> &_vRet ) const;

		/**
		 * Gets the type qualifiers from a fully specified type node.
		 *
		 * \param _ndNode The node from which to extract the type qualifiers.
		 * \return Returns the type qualifiers from the given node.
		 */
		uint64_t LSE_CALL							GetTypeQualifier( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the type from a fully specified type node.
		 *
		 * \param _ndNode The node from which to extract the type.
		 * \return Returns the type from the given node.
		 */
		uint64_t LSE_CALL							GetType( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets a name from an identifier node.
		 *
		 * \param _ndNode The node from which to extract the name.
		 * \return Returns the name of the given node.
		 */
		const CString & LSE_CALL					GetName( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the index of a name from an identifier node.
		 *
		 * \param _ndNode The node from which to extract the name index.
		 * \return Returns the index of the name of the given node.
		 */
		uint32_t LSE_CALL							GetNameIndex( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the semantics node from a parameter declaration (LSG_N_PARMDECLARATION) node.
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the semantic type and the semantic numeric, with the numeric being stored in the upper 32 bits of the return value.
		 *	If there is no semantic, LSE_MAXU64 is returned.
		 */
		uint64_t LSE_CALL							GetSemantic( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the type qualifiers from a parameter declaration (LSG_N_PARMDECLARATION) node. 
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the type qualifiers on the given node.
		 */
		uint64_t LSE_CALL							GetTypeQualifiersOnParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the type from a parameter declaration (LSG_N_PARMDECLARATION) node. 
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the type on the given node.
		 */
		uint64_t LSE_CALL							GetTypeOnParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the name of a parameter declaration (LSG_N_PARMDECLARATION) node. 
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the name of the given node.
		 */
		const CString & LSE_CALL					GetNameOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the name index of a parameter declaration (LSG_N_PARMDECLARATION) node. 
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the name index of the given node.
		 */
		uint32_t LSE_CALL							GetNameIndexOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Gets the array index of a parameter declaration (LSG_N_PARMDECLARATION) node. 
		 *
		 * \param _ndNode Node to search.
		 * \return Returns the array index of the given node.
		 */
		uint32_t LSE_CALL							GetArrayIndexOfParm( const YYSTYPE::LSG_NODE_DATA &_ndNode ) const;

		/**
		 * Register a user type.  This is always a structure.
		 *
		 * \param _pcText The type name.
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								RegisterStructType( const char * _pcText );

		/**
		 * Gets the type ID for a given user-defined type.  Returns 0 if no such type exists.
		 *
		 * \param _pcText The type name.
		 * \return Returns the type ID if the given type name exists or 0 if it does not.
		 */
		uint32_t LSE_CALL							GetStructTypeId( const char * _pcText ) const;

		/**
		 * Gets a structure name by its type ID.
		 *
		 * \param _ui32Id ID of the user-defined structure to get.
		 * \return Returns the name of the structure if found, or an empty string if not.
		 */
		CString LSE_CALL							GetStructById( uint32_t _ui32Id ) const;

		/**
		 * Gets the size of a type.  For structures, elements are always packed on 16-byte boundaries, and the
		 *	size returned will be rounded up to the next 16-byte boundary.
		 *
		 * \param _ui64Type The type whose size is to be obtained.
		 * \return Returns the size of the given type or LSE_MAXU32 if the type is not defined.
		 */
		uint32_t LSE_CALL							GetSize( uint64_t _ui64Type ) const;

		/**
		 * Gets a constant reference to the multi-map of referenced functions.
		 *
		 * \return Returns a constant reference to the multi-map of referenced functions.
		 */
		const CMultiMap<uint32_t, uint32_t> & LSE_CALL
													GetFuncRefs() const;


		/**
		 * Is the given type an integral type?
		 *
		 * \param _ui64Type The type to check for being an integer.
		 * \return Returns true if the given type is an integral type that is not an array.
		 */
		static LSBOOL LSE_CALL						IsInteger( uint64_t _ui64Type );

		/**
		 * Gets the length of an array unit of a type.
		 *
		 * \param _ui64Type The type whose array unit is to be obtained.
		 * \param _ui32Unit The array unit to be obtained.
		 * \return Returns the array unit of a given type.
		 */
		static uint32_t LSE_CALL					GetArrayUnit( uint64_t _ui64Type, uint32_t _ui32Unit );

		/**
		 * Gets the type part of a type + array-unit combination.
		 *
		 * \param _ui64Type The type whose type data is to be obtained.
		 * \return Returns the type part of the combination.
		 */
		static uint32_t LSE_CALL					GetType( uint64_t _ui64Type );

		/**
		 * Combine a type specifier with array specifiers to create a value that contains both.
		 *
		 * \param _ui64Type Type specifier.
		 * \param _ui32Array0 Array value 1.
		 * \param _ui32Array1 Array value 2.  Must be LSE_MAXU32 if not used.
		 * \return Returns the value created by combining the input values into a single uint64_t.
		 */
		static uint64_t LSE_CALL					CombineTypesAndArrays( uint64_t _ui64Type, uint32_t _ui32Array0, uint32_t _ui32Array1 );

		/**
		 * Converts a type to a numeric-constant type.  Returns LSG_NC_INVALID if the type is not a basic type (arrays).
		 *
		 * \param _ui64Type The type to convert.
		 * \return Returns the LSG_NUM_CONSTANTS type of the given type.
		 */
		static LSG_NUM_CONSTANTS LSE_CALL			TypeToConstantType( uint64_t _ui64Type );

		/**
		 * Converts a numeric-constant type to a full type that most closely matches the numeric-constant type.
		 *
		 * \param _ncType The type to convert.
		 * \return Returns the full type form that matches most closely the given numeric-constant type.
		 */
		static uint64_t LSE_CALL					ConstantTypeToType( LSG_NUM_CONSTANTS _ncType );

		/**
		 * Gets the name of a basic type.
		 *
		 * \param _ui64Type The type whose name is to be obtained.
		 * \return Returns the type in string form.
		 */
		static CString LSE_CALL						BasicTypeToString( uint64_t _ui64Type );

		/**
		 * Converts from one scalar type to another.
		 *
		 * \param _sScalar The value to cast.
		 * \param _ui64OriginalType The type of value in _sScalar.
		 * \param _ui64NewType The type to which to cast the given scalar value.
		 * \param _bRes If set to true, the function succeeded in converting the values.
		 * \return Returns the scalar cast to the given type.
		 */
		static LSG_SCALAR LSE_CALL					CastScalar( const LSG_SCALAR &_sScalar, uint64_t _ui64OriginalType, uint64_t _ui64NewType, LSBOOL &_bRes );

		/**
		 * Gets the best type between the two given types.  In order: bool, int, uint, float.
		 *
		 * \param _ncLeft The left type.
		 * \param _ncRight The right type.
		 * \return Returns the highest precision type between the two given types.
		 */
		static LSG_NUM_CONSTANTS LSE_CALL			GetHighType( LSG_NUM_CONSTANTS _ncLeft, LSG_NUM_CONSTANTS _ncRight );

		/**
		 * Is one type or the other a matrix?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if either type is a matrix type.
		 */
		static LSBOOL LSE_CALL						IsEitherMatrix( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Are both types matrices?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if both types are matrices.
		 */
		static LSBOOL LSE_CALL						BothAreMatrices( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Is one type or the other a vector?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if either type is a vector type.
		 */
		static LSBOOL LSE_CALL						IsEitherVector( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Are both types vectors?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if both types are vectors.
		 */
		static LSBOOL LSE_CALL						BothAreVectors( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Is one type or the other a scalar?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if either type is a scalar type.
		 */
		static LSBOOL LSE_CALL						IsEitherScalar( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Are both types scalars?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if both types are scalars.
		 */
		static LSBOOL LSE_CALL						BothAreScalars( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Is one type or the other a structure?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if either type is a structure type.
		 */
		static LSBOOL LSE_CALL						IsEitherStruct( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Are both types structures?
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \return Returns true if both types are structures.
		 */
		static LSBOOL LSE_CALL						BothAreStructs( uint64_t _ui64Left, uint64_t _ui64Right );

		/**
		 * Is the given type a sampler?
		 *
		 * \param _ui64Type Type to check for being a sampler.
		 * \return Returns true if the given type is a sampler.
		 */
		static LSBOOL LSE_FCALL						IsSampler( uint64_t _ui64Type );

		/**
		 * Is the given type a texture?
		 *
		 * \param _ui64Type Type to check for being a texture.
		 * \return Returns true if the given type is a texture.
		 */
		static LSBOOL LSE_FCALL						IsTexture( uint64_t _ui64Type );

		/**
		 * Gets the matrix dimensions from the type that is a matrix.  Exactly one of the types must be a matrix.
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \param _ui32Columns Number of columns.
		 * \param _ui32Rows Number of rows.
		 */
		static void LSE_CALL						GetMatrixSize( uint64_t _ui64Left, uint64_t _ui64Right,
			uint32_t &_ui32Columns, uint32_t &_ui32Rows );

		/**
		 * Gets the vector dimension from the type that is a vector.  Exactly one of the types must be a vector.
		 *
		 * \param _ui64Left Left type.
		 * \param _ui64Right Right type.
		 * \param _ui32Ret Number of scalars in the vector type.
		 */
		static void LSE_CALL						GetVectorSize( uint64_t _ui64Left, uint64_t _ui64Right,
			uint32_t &_ui32Ret );


	protected :
		// == Members.
		/**
		 * A pool of strings.  Strings are binary and not necessarily terminated with a NULL.
		 */
		CVector<CString, uint32_t, 512>				m_vStringPool;

		/**
		 * The stack of nodes.
		 */
		CVectorPoD<LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA, uint32_t, 512 * 10>
													m_vNodes;

		/**
		 * Stack of identifiers.
		 */
		CVectorPoD<LSG_IDENTIFIER, uint32_t, 512>	m_vIdentifiers;

		/**
		 * Sorted list of tokens.  Sorted for fast searching.
		 */
		CVectorPoD<uint32_t, uint32_t, 512>			m_vSortedTokens;

		/**
		 * Stack of scopes.  We maintain at least one scope always (the global scope) during parsing
		 *	of the data.  After the data has been parsed, the scope stack is cleared.
		 */
		CShaderScopeStack							m_sssScopes;

		/**
		 * The string index of the current function.
		 */
		uint32_t									m_ui32CurFunc;

		/**
		 * Multi-map of referenced functions.
		 */
		CMultiMap<uint32_t, uint32_t>				m_mmFuncRefs;

		/**
		 * The type of shader being compiled.
		 */
		LSG_SHADER_TYPES							m_stShaderType;

		/**
		 * Mapping between user types and their ID's.  Because a user type may only be declared once,
		 *	there is no need to support multiple ID's for a type name.
		 */
		CLinearMap<CString, uint32_t>				m_mUserTypes;

		/**
		 * Unique-type counter.
		 */
		uint32_t									m_ui32TypeId;

		/**
		 * Map of structures sorted by name.
		 */
		LSG_STRUCT_ARRAY							m_saStructures;

		/**
		 * The lexer, needed for the line number.
		 */
		CShaderLexer *								m_plLexer;


		// == Functions.
		/**
		 * Adds a node to the stack.  Set the node index too.
		 *
		 * \param _ndNode The node to add to the stack.
		 * \return Returns true if the node was added.
		 */
		LSBOOL LSE_CALL								AddNode( LSG_SHADER_SYNTAX_NODES::LSG_NODE_DATA &_ndNode );

		/**
		 * Adds an identifier by name.  Returns the index of the created identifier in the identifier stack.
		 *	The type of the created identifier is undefined.
		 *
		 * \param _pcText The string name of the identifier.
		 * \return Returns the index of the created identifier.
		 */
		uint32_t LSE_CALL							AddIdentifier( const char * _pcText );

		/**
		 * Gets the index of a token or the index where the token should be added if it is not in the list of tokens.
		 *
		 * \param _pcName Name of the token to find.
		 * \param _ui32Index Holds the returned index.
		 * \return Returns true if the token is in the list, false otherwise.
		 */
		LSBOOL LSE_CALL								FindToken( const char * _pcName, uint32_t &_ui32Index ) const;

	private :
	};

}	// namespace lsg

#endif	// __LSG_SHADERLANGUAGEPARSER_H__
