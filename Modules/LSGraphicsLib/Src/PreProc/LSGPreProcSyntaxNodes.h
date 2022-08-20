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
 * Description: Nodes for the syntax tree of the preprocessor parser.
 */


#ifndef __LSG_PREPROCSYNTAXNODES_H__
#define __LSG_PREPROCSYNTAXNODES_H__

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
#define YYSTYPE															LSG_PREPROC_SYNTAX_NODES

namespace lsg {

	/**
	 * Enumerate node types.
	 */
	enum LSG_PREPROC_NODE {
		LSG_PPN_IDENTIFIER,
		LSG_PPN_NUMERICCONSTANT,
		LSG_PPN_UNARY,
		LSG_PPN_MATH,
		LSG_PPN_TERTIARY,
		LSG_PPN_DEFINED,
		LSG_PPN_TU
	};

	/**
	 * Numeric constant types.
	 */
	enum LSG_PREPROC_NUM_CONSTANTS {
		LSG_PPNC_SIGNED,
		LSG_PPNC_UNSIGNED,
		LSG_PPNC_FLOATING,
		LSG_PPNC_INVALID,
	};

	/**
	 * Nodes in the parser.
	 */
	union YYSTYPE {
		uint64_t														ui64Op;				// Operators (mainly unary).

		struct LSG_NODE_DATA {
			LSG_PREPROC_NODE											ppnNodeType;		// One of the LSG_PPN_* constants.
			uint32_t													ui32NodeIndex;		// Index of the node within the stack of nodes.
			union LSG_NODE_UNION {
				uint32_t												ui32IdentifierIndex;// Index of an identifier in the master list of identifiers.
				uint64_t												ui64Const;			// Value of unsigned constant numeric.
				int64_t													i64Const;			// Value of signed constant numeric.
				double												dConst;				// Value of floating-point numeric.
				uint32_t												ui32UnaryOp;		// Unary operator.
				uint32_t												ui32NodeIndex;		// Index of a node.  Multi-purpose.
			}															nuNodeData;			// Node data.
			union LSG_NODE_UNION_EX {
				LSG_PREPROC_NUM_CONSTANTS								ncConstantType;		// Whether the given numeric constant is signed, unsigned, or floating.
				uint32_t												ui32NodeIndexEx;	// Index of a node.  Multi-purpose.
			}															nueNodeDataEx;
			union LSG_NODE_UNION_OP {
				uint32_t												ui32Op;				// Operator.
				uint32_t												ui32NodeIndex3;		// Tertiary expression and initializer index in single declarations.
			}															nuoOp;
		}																ndData;
	};


}	// namespace lsg

#endif	// __LSG_PREPROCSYNTAXNODES_H__
