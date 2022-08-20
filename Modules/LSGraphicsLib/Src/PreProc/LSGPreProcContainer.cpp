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
 * Description: Stores all of the data parsed during the parsing of an expression for use with the preprocessor.
 */

#include "LSGPreProcContainer.h"
#include "Algorithm/LSTLAlgorithm.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CPreProcContainer::CPreProcContainer() {
	}

	// == Functions.
	/**
	 * Creates a node pointing to an identifier (by index into the identifier stack).
	 *
	 * \param _pcText The string name of the identifier.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::CreateIdentifier( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_IDENTIFIER;

		_ndNode.nuNodeData.ui32IdentifierIndex = AddIdentifier( _pcText );
		if ( _ndNode.nuNodeData.ui32IdentifierIndex == LSE_MAXU32 ) { return false; }

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a string representation of a decimal constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::CreateDecimal( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_NUMERICCONSTANT;

		CString sString = _pcText;
		if ( sString.Length() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_INVALID;
			return false;
		}

		// If the last character is L, eat it.
		while ( CStd::ToLower( sString.GetLastChar() ) == 'l' ) {
			sString.RemLastChar();
		}

		// If the last character is a U, set to unsigned.
		if ( CStd::ToLower( sString.GetLastChar() ) == 'u' ) {
			sString.RemLastChar();
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_UNSIGNED;
		}
		else {
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_SIGNED;
		}

		_ndNode.nuNodeData.i64Const = CStd::AtoI64( sString.CStr() );

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a string representation of a hexadecimal constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::CreateHexadecimal( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_NUMERICCONSTANT;

		CString sString = _pcText;
		if ( sString.Length() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_INVALID;
			return false;
		}

		// If the last character is L, eat it.
		while ( CStd::ToLower( sString.GetLastChar() ) == 'l' ) {
			sString.RemLastChar();
		}

		// If the last character is a U, set to unsigned.
		if ( CStd::ToLower( sString.GetLastChar() ) == 'u' ) {
			sString.RemLastChar();
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_UNSIGNED;
		}
		else {
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_SIGNED;
		}

		_ndNode.nuNodeData.i64Const = CStd::AHtoI64( sString.CStr() );

		return AddNode( _ndNode );
	}

	/**
	 * Accepts a representation of a floating-point constant and stores the result in the
	 *	given node.
	 *
	 * \param _pcText String constant containing the numeric.
	 * \param _ndNode Node to create based off the string constant.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::CreateFloatingPoint( const char * _pcText, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_NUMERICCONSTANT;

		CString sString = _pcText;
		if ( sString.Length() == 0 ) {
			// Error.
			_ndNode.nuNodeData.ui64Const = 0ULL;
			_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_INVALID;
			return false;
		}

		_ndNode.nuNodeData.dConst = CStd::AtoF( _pcText );
		_ndNode.nueNodeDataEx.ncConstantType = LSG_PPNC_FLOATING;

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
	LSBOOL LSE_CALL CPreProcContainer::CreateUnaryOp( uint32_t _ui32Op, const YYSTYPE::LSG_NODE_DATA &_ndBase,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_UNARY;

		_ndNode.nuNodeData.ui32UnaryOp = _ui32Op;
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
	LSBOOL LSE_CALL CPreProcContainer::CreateMath( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		uint32_t _ui32Op, YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_MATH;
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
	LSBOOL LSE_CALL CPreProcContainer::CreateTertiary( const YYSTYPE::LSG_NODE_DATA &_ndCond,
		const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_TERTIARY;
		_ndNode.nuNodeData.ui32NodeIndex = _ndCond.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndLeft.ui32NodeIndex;
		_ndNode.nuoOp.ui32NodeIndex3 = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Creates a defined() expression.
	 *
	 * \param _ndIdent The identifier.
	 * \param _ndNode Node to create.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::CreateDefined( const YYSTYPE::LSG_NODE_DATA &_ndIdent,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_DEFINED;
		_ndNode.nuNodeData.ui32NodeIndex = _ndIdent.ui32NodeIndex;

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
	LSBOOL LSE_CALL CPreProcContainer::CreateTranslationUnit( const YYSTYPE::LSG_NODE_DATA &_ndLeft,
		const YYSTYPE::LSG_NODE_DATA &_ndRight,
		YYSTYPE::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ppnNodeType = LSG_PPN_TU;
		_ndNode.nuNodeData.ui32NodeIndex = _ndLeft.ui32NodeIndex;
		_ndNode.nueNodeDataEx.ui32NodeIndexEx = _ndRight.ui32NodeIndex;

		return AddNode( _ndNode );
	}

	/**
	 * Gets a node for read-only.
	 *
	 * \param _ui32Index Index of the node to get.
	 * \return Returns a constant reference to the requested node.
	 */
	const LSG_PREPROC_SYNTAX_NODES::LSG_NODE_DATA & LSE_CALL CPreProcContainer::GetNode( uint32_t _ui32Index ) const {
		return m_vNodes[_ui32Index];
	}

	/**
	 * Gets the total number of nodes.
	 *
	 * \return Returns the total number of nodes in the syntax tree.
	 */
	uint32_t LSE_CALL CPreProcContainer::TotalNodes() const {
		return m_vNodes.Length();
	}

	/**
	 * Gets a string from the string pool.
	 *
	 * \param _ui32Index Index of the string to get.
	 * \return Returns the string at the given index.
	 */
	const CString & LSE_CALL CPreProcContainer::GetString( uint32_t _ui32Index ) const {
		return m_vStringPool[_ui32Index];
	}

	/**
	 * Adds a node to the stack.  Set the node index too.
	 *
	 * \param _ndNode The node to add to the stack.
	 * \return Returns true if the node was added.
	 */
	LSBOOL LSE_CALL CPreProcContainer::AddNode( LSG_PREPROC_SYNTAX_NODES::LSG_NODE_DATA &_ndNode ) {
		_ndNode.ui32NodeIndex = m_vNodes.Length();
		return m_vNodes.Push( _ndNode );
	}

	/**
	 * Adds an identifier by name.  Returns the index of the created identifier in the identifier stack.
	 *	The type of the created identifier is undefined.
	 *
	 * \param _pcText The string name of the identifier.
	 * \return Returns the index of the created identifier.
	 */
	uint32_t LSE_CALL CPreProcContainer::AddIdentifier( const char * _pcText ) {
		// Find the name in the existing stack of strings if it is there.
		uint32_t ui32Ret = CAlgorithm::Find( m_vStringPool, CString( _pcText ) );
		if ( ui32Ret == m_vStringPool.Length() ) {
			// Not there so add it.
			if ( !m_vStringPool.Push( _pcText ) ) { return LSE_MAXU32; }
		}
		return ui32Ret;
	}

}	// namespace lsg
