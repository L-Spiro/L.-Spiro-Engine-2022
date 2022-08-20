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
 * Description: A single scope within a stack of scopes.  When a new scope is created, it is pushed to the top
 *	of a stack of scopes.  The top of the stack is always the current scope.
 */

#include "LSGShaderScope.h"


namespace lsg {

	// == Various constructors.
	LSE_CALLCTOR CShaderScope::CShaderScope() {
	}
	LSE_CALLCTOR CShaderScope::~CShaderScope() {
	}

	// == Operators.
	/**
	 * Copy operator.
	 *
	 * \param _ssOther The object to copy.
	 * \return Returns this object after the copy.
	 */
	CShaderScope & LSE_CALL CShaderScope::operator = ( const CShaderScope &_ssOther ) {
		m_vVariables = _ssOther.m_vVariables;
		m_vFuncs = _ssOther.m_vFuncs;
		m_vStructs = _ssOther.m_vStructs;
		m_mmTypeMap = _ssOther.m_mmTypeMap;
		return (*this);
	}

	// == Functions.
	/**
	 * Adds a variable to this scope.
	 *
	 * \param _sName The name of the variable.
	 * \param _vVar The variable data to add.
	 * \return Returns true if the variable was added.  A return of false indicates a memory failure.
	 */
	LSBOOL LSE_CALL CShaderScope::AddVariable( const CString &_sName, const LSG_VARIABLE &_vVar ) {
		LSG_TYPE_AND_INDEX taiIndex = {
			LSG_T_VARIABLE,
			m_vVariables.Length()
		};
		if ( !m_vVariables.Push( _vVar ) ) { return false; }
		if ( !m_mmTypeMap.Insert( _sName, taiIndex ) ) {
			m_vVariables.PopNoDealloc();
			return false;
		}
		return true;
	}

	/**
	 * Adds a function to this scope.
	 *
	 * \param _sName The name of the function.
	 * \param _fFunc The function data to add.
	 * \return Returns true if the function was added.  A return of false indicates a memory failure.
	 */
	LSBOOL LSE_CALL CShaderScope::AddFunction( const CString &_sName, const LSG_FUNCTION &_fFunc ) {
		LSG_TYPE_AND_INDEX taiIndex = {
			LSG_T_FUNCTION,
			m_vFuncs.Length()
		};
		if ( !m_vFuncs.Push( _fFunc ) ) { return false; }
		if ( !m_mmTypeMap.Insert( _sName, taiIndex ) ) {
			m_vFuncs.PopNoDealloc();
			return false;
		}
		return true;
	}

	/**
	 * Adds a structure to this scope.
	 *
	 * \param _sName The name of the structure.
	 * \param _sStruct The structure data to add.
	 * \return Returns true if the structure was added.  A return of false indicates a memory failure.
	 */
	LSBOOL LSE_CALL CShaderScope::AddStruct( const CString &_sName, const LSG_STRUCT &_sStruct ) {
		LSG_TYPE_AND_INDEX taiIndex = {
			LSG_T_STRUCT,
			m_vStructs.Length()
		};
		if ( !m_vStructs.Push( _sStruct ) ) { return false; }
		if ( !m_mmTypeMap.Insert( _sName, taiIndex ) ) {
			m_vStructs.PopNoDealloc();
			return false;
		}
		return true;
	}

	/**
	 * Gets a variable by index.
	 *
	 * \param _ui32Index Index of the variable to get.
	 * \return Returns a constant reference to the variable at the given index.
	 */
	const CShaderScope::LSG_VARIABLE & LSE_CALL CShaderScope::GetVariable( uint32_t _ui32Index ) const {
		return m_vVariables[_ui32Index];
	}

	/**
	 * Gets a function by index.
	 *
	 * \param _ui32Index Index of the function to get.
	 * \return Returns a constant reference to the function at the given index.
	 */
	const CShaderScope::LSG_FUNCTION & LSE_CALL CShaderScope::GetFunction( uint32_t _ui32Index ) const {
		return m_vFuncs[_ui32Index];
	}

	/**
	 * Gets a structure by index.
	 *
	 * \param _ui32Index Index of the structure to get.
	 * \return Returns a constant reference to the structure at the given index.
	 */
	const CShaderScope::LSG_STRUCT & LSE_CALL CShaderScope::GetStruct( uint32_t _ui32Index ) const {
		return m_vStructs[_ui32Index];
	}

	/**
	 * Does the given type-identifier name exist within this scope?
	 *
	 * \param _tType Type of the identifier.
	 * \param _sName The identifier to locate.
	 * \return Returns true if the given identifier exists in this scope with the given form.
	 */
	LSBOOL LSE_CALL CShaderScope::IdentifierExistsAs( LSG_TYPE _tType, const CString &_sName ) const {
		uint32_t ui32Index;
		if ( !m_mmTypeMap.GetItemIndex( _sName, ui32Index ) ) { return false; }	// Identifier not found at all.

		// Identifier found.  Check each entry and determine if there is an entry that matches the given type.
		for ( uint32_t I = m_mmTypeMap.ItemsOnKey( ui32Index ); I--; ) {
			LSG_TYPE tType = m_mmTypeMap.GetByIndex( ui32Index, I ).tType;
			if ( tType == _tType ) { return true; }
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
	 * \return Returns true if the given identifier exists in this scope with the given form.
	 */
	LSBOOL LSE_CALL CShaderScope::GetIdentifiersAs( LSG_TYPE _tType, const CString &_sName, CVectorPoD<LSG_TYPE_AND_INDEX> &_vRet ) const {
		_vRet.ResetNoDealloc();
		uint32_t ui32Index;
		if ( !m_mmTypeMap.GetItemIndex( _sName, ui32Index ) ) { return false; }	// Identifier not found at all.

		// Identifier found.  Check each entry and determine if there is an entry that matches the given type.
		for ( uint32_t I = m_mmTypeMap.ItemsOnKey( ui32Index ); I--; ) {
			LSG_TYPE tType = m_mmTypeMap.GetByIndex( ui32Index, I ).tType;
			if ( tType == _tType ) {
				LSG_TYPE_AND_INDEX taiIndex;
				taiIndex.tType = _tType;
				taiIndex.ui32Index = m_mmTypeMap.GetByIndex( ui32Index, I ).ui32Index;
				if ( !_vRet.Push( taiIndex ) ) { return false; }
			}
		}

		return _vRet.Length() != 0;
	}

	/**
	 * Adds a name association.
	 *
	 * \param _sOldName The name of the original identifier.
	 * \param _pcNewName The new name of the identifier.
	 * \return Returns false if there is not enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CShaderScope::CreateNameAssociation( const CString &_sOldName, const char * _pcNewName ) {
		return m_mNameAssociations.Insert( _sOldName, CString( _pcNewName ) );
	}

	/**
	 * Gets a name association.
	 *
	 * \param _sName The name for which to find an association.
	 * \param _pcNew The replacement name, if any.  Filled on successful return.
	 * \return Returns true if there is a replacement name for the given name.
	 */
	LSBOOL LSE_CALL CShaderScope::GetNameAssociation( const CString &_sName, const char * &_pcNew ) const {
		if ( !m_mNameAssociations.ItemExists( _sName ) ) { return false; }

		_pcNew = m_mNameAssociations[_sName].CStr();
		return true;
	}

}	// namespace lsg
