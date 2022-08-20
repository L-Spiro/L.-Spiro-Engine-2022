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
 * Description: Nodes for the syntax tree of the parser.
 */


#ifndef __LSG_SHADERSYNTAXNODES_H__
#define __LSG_SHADERSYNTAXNODES_H__

#include "../LSGGraphicsLib.h"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef yy
#define yy																lsg
#endif	// #ifndef yy
#ifdef YYSTYPE
#undef YYSTYPE
#endif	// #ifdef YYSTYPE
#define YYSTYPE															LSG_SHADER_SYNTAX_NODES

namespace lsg {

	/**
	 * Enumerate node types.
	 */
	enum LSG_NODE {
		LSG_N_STRINGCONSTANT,
		LSG_N_NUMERICCONSTANT,
		LSG_N_IDENTIFIER,
		LSG_N_BOOLEAN,
		LSG_N_ARRAYACCESS,
		LSG_N_POSTFIXINC,
		LSG_N_POSTFIXDEC,
		LSG_N_PREFIXINC,
		LSG_N_PREFIXDEC,
		LSG_N_UNARY,
		LSG_N_CAST,
		LSG_N_MATH,
		LSG_N_TERTIARY,
		LSG_N_ASSIGNMENT,
		LSG_N_COMMA,
		LSG_N_INTRINSIC,
		LSG_N_ARGLIST,
		LSG_N_FUNCTIONCALL,
		LSG_N_FIELDSELECT,
		LSG_N_CONSTRUCTOR,
		LSG_N_EXPRESSIONSTATEMENT,
		LSG_N_DECLARATIONSTATEMENT,
		LSG_N_SEMANTIC,
		LSG_N_TYPESPECIFIERNOPREC,
		LSG_N_FULLYSPECIFIEDTYPE,
		LSG_N_SINGLEDECL,
		LSG_N_INITLIST,
		LSG_N_INITLISTWRAP,
		LSG_N_STATEMENTLIST,
		LSG_N_NEWSCOPESTATEMENT,
		LSG_N_BLANKSTATEMENT,
		LSG_N_STRUCTDECLARATOR,
		LSG_N_STRUCTDECLARATORLIST,
		LSG_N_STRUCTDECLARATION,
		LSG_N_STRUCTDECLARATIONLIST,
		LSG_N_STRUCT,
		LSG_N_PARMDECLARATOR,
		LSG_N_PARMDECLARATION,
		LSG_N_PARMDECLARATIONLIST,
		LSG_N_FUNCDECL,
		LSG_N_FUNCDEFINITION,
		LSG_N_SELECTION,
		LSG_N_FORINIT,
		LSG_N_WHILE,
		LSG_N_DOWHILE,
		LSG_N_FOR,
		LSG_N_CONTINUE,
		LSG_N_BREAK,
		LSG_N_RETURN,
		LSG_N_DISCARD,
		LSG_N_EXTERNALDECLARATIONLIST,
	};

	/**
	 * Numeric constant types.
	 */
	enum LSG_NUM_CONSTANTS {
		LSG_NC_SIGNED,
		LSG_NC_UNSIGNED,
		LSG_NC_FLOATING,
		LSG_NC_INVALID,
	};

	/**
	 * Storage qualifiers.  Bit masks are used.
	 */
	enum LSG_STORAGE_QUALIFIER {
		LSG_SQ_BASEBIT													= 16,
		LSG_SQ_CONST													= (1 << (0 + LSG_SQ_BASEBIT)),
		LSG_SQ_IN														= (1 << (1 + LSG_SQ_BASEBIT)),
		LSG_SQ_OUT														= (1 << (2 + LSG_SQ_BASEBIT)),
		LSG_SQ_INOUT													= (LSG_SQ_IN | LSG_SQ_OUT),
		LSG_SQ_UNIFORM													= (1 << (3 + LSG_SQ_BASEBIT)),

		LSG_SQ_BITSUSED													= 4 + LSG_SQ_BASEBIT,
		LSG_SQ_FORCEDWORD												= 0x7FFFFFFF,
	};

	/**
	 * Interpolation qualifiers.  Bits follow LSG_STORAGE_QUALIFIER.
	 */
	enum LSG_INTERPOLATION_QUALIFIER {
		LSG_IQ_SMOOTH													= (1 << (0 + LSG_SQ_BITSUSED)),
		LSG_IQ_FLAT														= (1 << (1 + LSG_SQ_BITSUSED)),

		LSG_IQ_BITSUSED													= 2 + LSG_SQ_BITSUSED,
		LSG_IQ_FORCEDWORD												= 0x7FFFFFFF,
	};

	/**
	 * Nodes in the parser.
	 */
	union YYSTYPE {
		uint32_t														ui32StringIndex;	// Strings are stored as indices into a master list of shared
																							//	strings.
		uint64_t														ui64Op;				// Operators (mainly unary).
		struct LSG_NODE_DATA {
			LSG_NODE													nNodeType;			// One of the LSG_N_* constants.
			uint32_t													ui32NodeIndex;		// Index of the node within the stack of nodes.
			uint32_t													ui32NodeLine;		// Line of the node in the original file.
			uint32_t													ui32NodeFile;		// Name of the file.
			union LSG_NODE_UNION {
				uint32_t												ui32StringIndex;	// Index of a string in the master list of strings.
				uint32_t												ui32IdentifierIndex;// Index of an identifier in the master list of identifiers.
				uint64_t												ui64Const;			// Value of unsigned constant numeric.
				int64_t													i64Const;			// Value of signed constant numeric.
				double												dConst;				// Value of floating-point numeric.
				uint32_t												ui32NodeIndex;		// Index of a node.  Multi-purpose.
				uint32_t												ui32UnaryOp;		// Unary operator.
				uint64_t												ui64CastType;		// Cast type.
			}															nuNodeData;			// Node data.
			union LSG_NODE_UNION_EX {
				LSG_NUM_CONSTANTS										ncConstantType;		// Whether the given numeric constant is signed, unsigned, or floating.
				uint32_t												ui32NodeIndexEx;	// Index of a node.  Multi-purpose.
			}															nueNodeDataEx;
			union LSG_NODE_UNION_OP {
				uint32_t												ui32Op;				// Operator.
				uint32_t												ui32NodeIndex3;		// Tertiary expression and initializer index in single declarations.
			}															nuoOp;
			uint32_t													ui32SingleDeclName;	// Index of the identifier in a single declaration or the body of a for ().
			uint32_t													ui32Semantic;		// Semantic on declarations.
		}																ndData;

	};


}	// namespace lsg

#endif	// __LSG_SHADERSYNTAXNODES_H__
