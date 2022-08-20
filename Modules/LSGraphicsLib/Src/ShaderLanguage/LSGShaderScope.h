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


#ifndef __LSG_SHADERSCOPE_H__
#define __LSG_SHADERSCOPE_H__

#include "../LSGGraphicsLib.h"
#include "Map/LSTLLinearMap.h"
#include "MultiMap/LSTLMultiMap.h"
#include "String/LSTLString.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsg {

	/**
	 * Class CShaderScope
	 * \brief A single scope within a stack of scopes.
	 *
	 * Description: A single scope within a stack of scopes.  When a new scope is created, it is pushed to the top
	 *	of a stack of scopes.  The top of the stack is always the current scope.
	 */
	class CShaderScope {
	public :
		// == Various constructors.
		LSE_CALLCTOR								CShaderScope();
		LSE_CALLCTOR								~CShaderScope();


		// == Enumerations.
		/**
		 * Types of things.
		 */
		enum LSG_TYPE {
			LSG_T_VARIABLE,							// A LSG_VARIABLE object.
			LSG_T_FUNCTION,							// A LSG_FUNCTION object.
			LSG_T_STRUCT,							// A LSG_STRUCT object.
			LSG_T_UNDEFINED,						// Unknown type.  May not end up using this, but keep it in case for future expansion.
		};


		// == Types.
		/**
		 * A variable.  Includes type information and name.
		 */
		typedef struct LSG_VARIABLE {
			/**
			 * Name.  This is just an index into an array of strings.
			 *	To get the actual name you must have the original array of strings.
			 */
			uint32_t								ui32Name;

			/**
			 * Qualifier information.
			 */
			uint64_t								ui64Qualifiers;

			/**
			 * Type information, which includes array data (related to the type).
			 */
			uint64_t								ui64Type;

			/**
			 * Array size or LSE_MAXU32 if it is not an array.
			 */
			uint32_t								ui32Array;

			/**
			 * Usage type (semantic) or LSE_MAXU32.
			 */
			uint32_t								ui32Semantic;

			/**
			 * Original declaration node within a stack of nodes representing the
			 *	syntax tree of a parsed shader file.
			 */
			uint32_t								ui32DeclNode;
		} * LPLSG_VARIABLE, * const LPCLSG_VARIABLE;

		/**
		 * A function.  Includes its return type and number of parameters.
		 */
		typedef struct LSG_FUNCTION {
			/**
			 * Name.  This is just an index into an array of strings.
			 *	To get the actual name you must have the original array of strings.
			 */
			uint32_t								ui32Name;

			/**
			 * Qualifier information.
			 */
			uint64_t								ui64Qualifiers;

			/**
			 * Type information, which includes array data (related to the type).
			 */
			uint64_t								ui64Type;

			/**
			 * Number of parameters.
			 */
			uint32_t								ui32ParmCount;

			/**
			 * Original declaration node within a stack of nodes representing the
			 *	syntax tree of a parsed shader file.
			 */
			uint32_t								ui32DeclNode;
		} * LPLSG_FUNCTION, * const LPCLSG_FUNCTION;

		/**
		 * A structure.  Inclues its name and its declaration node index.
		 */
		typedef struct LSG_STRUCT {
			/**
			 * Name.  This is just an index into an array of strings.
			 *	To get the actual name you must have the original array of strings.
			 */
			uint32_t								ui32Name;

			/**
			 * Original declaration node within a stack of nodes representing the
			 *	syntax tree of a parsed shader file.
			 */
			uint32_t								ui32DeclNode;
		} * LPLSG_STRUCT, * const LPCLSG_STRUCT;

		/**
		 * A structure for use with maps that associates a name with a type and index combination.
		 *	This is the type/index combination.
		 */
		typedef struct LSG_TYPE_AND_INDEX {
			/**
			 * Type.
			 */
			LSG_TYPE								tType;

			/**
			 * Index.
			 */
			uint32_t								ui32Index;
		} * LPLSG_TYPE_AND_INDEX, * const LPCLSG_TYPE_AND_INDEX;


		// == Operators.
		/**
		 * Copy operator.
		 *
		 * \param _ssOther The object to copy.
		 * \return Returns this object after the copy.
		 */
		CShaderScope & LSE_CALL						operator = ( const CShaderScope &_ssOther );


		// == Functions.
		/**
		 * Adds a variable to this scope.
		 *
		 * \param _sName The name of the variable.
		 * \param _vVar The variable data to add.
		 * \return Returns true if the variable was added.  A return of false indicates a memory failure.
		 */
		LSBOOL LSE_CALL								AddVariable( const CString &_sName, const LSG_VARIABLE &_vVar );

		/**
		 * Adds a function to this scope.
		 *
		 * \param _sName The name of the function.
		 * \param _fFunc The function data to add.
		 * \return Returns true if the function was added.  A return of false indicates a memory failure.
		 */
		LSBOOL LSE_CALL								AddFunction( const CString &_sName, const LSG_FUNCTION &_fFunc );

		/**
		 * Adds a structure to this scope.
		 *
		 * \param _sName The name of the structure.
		 * \param _sStruct The structure data to add.
		 * \return Returns true if the structure was added.  A return of false indicates a memory failure.
		 */
		LSBOOL LSE_CALL								AddStruct( const CString &_sName, const LSG_STRUCT &_sStruct );

		/**
		 * Gets a variable by index.
		 *
		 * \param _ui32Index Index of the variable to get.
		 * \return Returns a constant reference to the variable at the given index.
		 */
		const LSG_VARIABLE & LSE_CALL				GetVariable( uint32_t _ui32Index ) const;

		/**
		 * Gets a function by index.
		 *
		 * \param _ui32Index Index of the function to get.
		 * \return Returns a constant reference to the function at the given index.
		 */
		const LSG_FUNCTION & LSE_CALL				GetFunction( uint32_t _ui32Index ) const;

		/**
		 * Gets a structure by index.
		 *
		 * \param _ui32Index Index of the structure to get.
		 * \return Returns a constant reference to the structure at the given index.
		 */
		const LSG_STRUCT & LSE_CALL					GetStruct( uint32_t _ui32Index ) const;

		/**
		 * Does the given type-identifier name exist within this scope?
		 *
		 * \param _tType Type of the identifier.
		 * \param _sName The identifier to locate.
		 * \return Returns true if the given identifier exists in this scope with the given form.
		 */
		LSBOOL LSE_CALL								IdentifierExistsAs( LSG_TYPE _tType, const CString &_sName ) const;

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
		LSBOOL LSE_CALL								GetIdentifiersAs( LSG_TYPE _tType, const CString &_sName, CVectorPoD<LSG_TYPE_AND_INDEX> &_vRet ) const;

		/**
		 * Adds a name association.
		 *
		 * \param _sOldName The name of the original identifier.
		 * \param _pcNewName The new name of the identifier.
		 * \return Returns false if there is not enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL								CreateNameAssociation( const CString &_sOldName, const char * _pcNewName );

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
		 * Variables in this scope.
		 */
		CVectorPoD<LSG_VARIABLE>					m_vVariables;

		/**
		 * Functions in this scope.
		 */
		CVectorPoD<LSG_FUNCTION>					m_vFuncs;

		/**
		 * Structures in this scope.
		 */
		CVectorPoD<LSG_STRUCT>						m_vStructs;

		/**
		 * Mapping between names and types.  Since functions can be overloaded, a CMultiMap
		 *	has to be used instead of a CLinearMap.
		 */
		CMultiMap<CString, LSG_TYPE_AND_INDEX>		m_mmTypeMap;

		/**
		 * Name associations.
		 */
		CLinearMap<CString, CString>				m_mNameAssociations;
	};

}	// namespace lsg

#endif	// __LSG_SHADERSCOPE_H__
