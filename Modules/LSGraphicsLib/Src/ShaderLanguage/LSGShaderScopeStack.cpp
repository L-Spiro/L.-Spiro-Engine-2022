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
 * Description: A stack of scopes.  The bottom scope is global.  The top scope is the current scope.
 */

#include "LSGShaderScopeStack.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShaderScopeStack::CShaderScopeStack() {
	}
	LSE_CALLCTOR CShaderScopeStack::~CShaderScopeStack() {
	}

	// == Functions.
	/**
	 * Adds a scope.
	 *
	 * \return Returns true if there is enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderScopeStack::Push() {
		return m_vStack.Push( CShaderScope() );
	}

	/**
	 * Leave a scope.
	 */
	void LSE_CALL CShaderScopeStack::Pop() {
		m_vStack.PopNoDealloc();
	}

	/**
	 * Reset the stack.
	 */
	void LSE_CALL CShaderScopeStack::Reset() {
		m_vStack.ResetNoDealloc();
	}

	/**
	 * Gets the current scope, if any.  If used properly, there will always be at
	 *	least a global scope.  Since the engine is the only thing that should be
	 *	using this function, and the engine uses it properly, the engine does
	 *	not perform a NULL check on the return.  If you did not write this engine,
	 *	you should be checking for NULL on this return.
	 *
	 * \return Returns the current scope or NULL if there is none.
	 */
	CShaderScope * LSE_CALL CShaderScopeStack::GetCurrentScope() {
		if ( !m_vStack.Length() ) { return NULL; }
		return &m_vStack[m_vStack.Length()-1];
	}

	/**
	 * Gets the current scope, if any.  If used properly, there will always be at
	 *	least a global scope.  Since the engine is the only thing that should be
	 *	using this function, and the engine uses it properly, the engine does
	 *	not perform a NULL check on the return.  If you did not write this engine,
	 *	you should be checking for NULL on this return.
	 *
	 * \return Returns the current scope or NULL if there is none.
	 */
	const CShaderScope * LSE_CALL CShaderScopeStack::GetCurrentScope() const {
		if ( !m_vStack.Length() ) { return NULL; }
		return &m_vStack[m_vStack.Length()-1];
	}

	/**
	 * Gets a scope by index.
	 *
	 * \param _ui32Index Index of the scope to get.
	 * \return Returns the scope at the given index.
	 */
	CShaderScope * LSE_CALL CShaderScopeStack::GetScope( uint32_t _ui32Index ) {
		return &m_vStack[_ui32Index];
	}

	/**
	 * Gets a scope by index.
	 *
	 * \param _ui32Index Index of the scope to get.
	 * \return Returns the scope at the given index.
	 */
	const CShaderScope * LSE_CALL CShaderScopeStack::GetScope( uint32_t _ui32Index ) const {
		return &m_vStack[_ui32Index];
	}

	/**
	 * Gets the total number of scopes.
	 *
	 * \return Returns the total number of scopes.
	 */
	uint32_t LSE_CALL CShaderScopeStack::TotalScopes() const {
		return m_vStack.Length();
	}

	/**
	 * Does the given type-identifier name exist within any scopes?
	 *
	 * \param _tType Type of the identifier.
	 * \param _sName The identifier to locate.
	 * \return Returns true if the given identifier exists in any scope with the given form.
	 */
	LSBOOL LSE_CALL CShaderScopeStack::IdentifierExistsAs( CShaderScope::LSG_TYPE _tType, const CString &_sName ) const {
		for ( uint32_t I = m_vStack.Length(); I--; ) {
			if ( m_vStack[I].IdentifierExistsAs( _tType, _sName ) ) { return true; }
		}
		return false;
	}

	/**
	 * Gets all forms of the given identifier as the given type.  For example, if an identifier exists as
	 *	both a structure name and a function, this allows to get only the function or only the structure
	 *	form.
	 *
	 * \param _tType Type of the identifier.
	 * \param _sName The identifier to locate.
	 * \param _vRet Holds the returned list of variables, structures, or functions, all of which will match
	 *	the type given in _tType.
	 * \return Returns true if the given identifier exists in any scope with the given form.
	 */
	LSBOOL LSE_CALL CShaderScopeStack::GetIdentifiersAs( CShaderScope::LSG_TYPE _tType, const CString &_sName, CVectorPoD<LSG_TYPE_INDEX_AND_INDEX> &_vRet ) const {
		LSBOOL bExists = false;
		LSG_TYPE_INDEX_AND_INDEX tiaiIndex = {
			m_vStack.Length()
		};
		CVectorPoD<CShaderScope::LSG_TYPE_AND_INDEX> vType;
		for ( ; tiaiIndex.ui32StackIndex--; ) {
			vType.ResetNoDealloc();
			if ( m_vStack[tiaiIndex.ui32StackIndex].GetIdentifiersAs( _tType, _sName, vType ) ) {
				for ( uint32_t I = vType.Length(); I--; ) {
					tiaiIndex.taiScopeIndex = vType[I];
					_vRet.Push( tiaiIndex );
				}
				bExists = true;
			}
		}
		return bExists;
	}

	/**
	 * Gets a name association.
	 *
	 * \param _sName The name for which to find an association.
	 * \param _pcNew The replacement name, if any.  Filled on successful return.
	 * \return Returns true if there is a replacement name for the given name.
	 */
	LSBOOL LSE_CALL CShaderScopeStack::GetNameAssociation( const CString &_sName, const char * &_pcNew ) const {
		for ( uint32_t I = m_vStack.Length(); I--; ) {
			if ( m_vStack[I].GetNameAssociation( _sName, _pcNew ) ) { return true; }
		}
		return false;
	}

}	// namespace lsg
