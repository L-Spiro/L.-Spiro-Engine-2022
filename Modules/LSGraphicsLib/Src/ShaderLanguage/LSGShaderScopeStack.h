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


#ifndef __LSG_SHADERSCOPESTACK_H__
#define __LSG_SHADERSCOPESTACK_H__

#include "../LSGGraphicsLib.h"
#include "LSGShaderScope.h"
#include "Vector/LSTLVector.h"

namespace lsg {

	/**
	 * Class CShaderScopeStack
	 * \brief A stack of scopes containing variables, functions, and structures declared in shaders.
	 *
	 * Description: A stack of scopes.  The bottom scope is global.  The top scope is the current scope.
	 */
	class CShaderScopeStack {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CShaderScopeStack();
		LSE_CALLCTOR								~CShaderScopeStack();


		// == Types.
		/**
		 * The location of an identifier.
		 */
		typedef struct LSG_TYPE_INDEX_AND_INDEX {
			/**
			 * Index within our stack.
			 */
			uint32_t								ui32StackIndex;

			/**
			 * Type/index values for the scope at index ui32StackIndex.
			 */
			CShaderScope::LSG_TYPE_AND_INDEX		taiScopeIndex;
		} * LPLSG_TYPE_INDEX_AND_INDEX, * const LPCLSG_TYPE_INDEX_AND_INDEX;


		// == Functions.
		/**
		 * Adds a scope.
		 *
		 * \return Returns true if there is enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								Push();

		/**
		 * Leave a scope.
		 */
		void LSE_CALL								Pop();

		/**
		 * Reset the stack.
		 */
		void LSE_CALL								Reset();

		/**
		 * Gets the current scope, if any.  If used properly, there will always be at
		 *	least a global scope.  Since the engine is the only thing that should be
		 *	using this function, and the engine uses it properly, the engine does
		 *	not perform a NULL check on the return.  If you did not write this engine,
		 *	you should be checking for NULL on this return.
		 *
		 * \return Returns the current scope or NULL if there is none.
		 */
		CShaderScope * LSE_CALL						GetCurrentScope();

		/**
		 * Gets the current scope, if any.  If used properly, there will always be at
		 *	least a global scope.  Since the engine is the only thing that should be
		 *	using this function, and the engine uses it properly, the engine does
		 *	not perform a NULL check on the return.  If you did not write this engine,
		 *	you should be checking for NULL on this return.
		 *
		 * \return Returns the current scope or NULL if there is none.
		 */
		const CShaderScope * LSE_CALL				GetCurrentScope() const;

		/**
		 * Gets a scope by index.
		 *
		 * \param _ui32Index Index of the scope to get.
		 * \return Returns the scope at the given index.
		 */
		CShaderScope * LSE_CALL						GetScope( uint32_t _ui32Index );

		/**
		 * Gets a scope by index.
		 *
		 * \param _ui32Index Index of the scope to get.
		 * \return Returns the scope at the given index.
		 */
		const CShaderScope * LSE_CALL				GetScope( uint32_t _ui32Index ) const;

		/**
		 * Gets the total number of scopes.
		 *
		 * \return Returns the total number of scopes.
		 */
		uint32_t LSE_CALL							TotalScopes() const;

		/**
		 * Does the given type-identifier name exist within any scopes?
		 *
		 * \param _tType Type of the identifier.
		 * \param _sName The identifier to locate.
		 * \return Returns true if the given identifier exists in any scope with the given form.
		 */
		LSBOOL LSE_CALL								IdentifierExistsAs( CShaderScope::LSG_TYPE _tType, const CString &_sName ) const;

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
		LSBOOL LSE_CALL								GetIdentifiersAs( CShaderScope::LSG_TYPE _tType, const CString &_sName, CVectorPoD<LSG_TYPE_INDEX_AND_INDEX> &_vRet ) const;

		/**
		 * Gets a name association.
		 *
		 * \param _sName The name for which to find an association.
		 * \param _pcNew The replacement name, if any.  Filled on successful return.
		 * \return Returns true if there is a replacement name for the given name.
		 */
		LSBOOL LSE_CALL								GetNameAssociation( const CString &_sName, const char * &_pcNew ) const;


	protected :
		// == Members.
		/**
		 * Our stack of scopes.
		 */
		CVector<CShaderScope, uint32_t, 512>		m_vStack;
	};

}	// namespace lsg

#endif	// __LSG_SHADERSCOPESTACK_H__
