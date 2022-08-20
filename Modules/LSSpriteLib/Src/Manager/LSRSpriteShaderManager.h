/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: Manages the sharing of shaders between sprites.
 */


#ifndef __LSR_SPRITESHADERMANAGER_H__
#define __LSR_SPRITESHADERMANAGER_H__

#include "../LSRSpriteLib.h"
#include "LSRSpriteProperties.h"
#include "Map/LSTLLinearMap.h"
#include "ShaderLanguage/LSGShader.h"

namespace lsr {

	/**
	 * Class CSpriteShaderManager
	 * \brief Manages the sharing of shaders between sprites.
	 *
	 * Description: Manages the sharing of shaders between sprites.
	 */
	class CSpriteShaderManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CSpriteShaderManager();
		LSE_CALLCTOR							~CSpriteShaderManager();


		// == Functions.
		/**
		 * Gets a shared shader pointer given a sprite property (LSR_SPRITE_PROPERTIES).
		 *
		 * \param _spProps Sprite properties.
		 * \param _pcVertexShader Vertex shader file name, or NULL to use the default.
		 * \param _pcPixelShader Pixel shader file name, or NULL to use the default.
		 * \return Returns a shared shader pointer, which will be invalid in the event of an error.
		 */
		CSharedShaderPtr LSE_CALL				GetShader( const LSR_SPRITE_PROPERTIES &_spProps, const char * _pcVertexShader = NULL, const char * _pcPixelShader = NULL );

		/**
		 * Adds a path to search for shaders.  If no paths are added manually only the default path is searched.
		 *
		 * \param _pcPath The path to add to the search list when looking for shaders.
		 */
		void LSE_CALL							AddSearchPath( const char * _pcPath );

		/**
		 * Resets the object to scratch.
		 */
		void LSE_CALL							Reset();

	protected :
		// == Types.
		/** A sprite property type with comparison operators for sorting. */
		typedef struct LSR_SORTABLE_SPRITE_PROPERTIES : public LSR_SPRITE_PROPERTIES {
			// == Operators.
			/**
			 * Equality operator.
			 *
			 * \param _sspOther The object against which to compare.
			 * \return Returns true if the objects are the same.
			 */
			LSE_INLINE LSBOOL LSE_CALL			operator == ( const LSR_SORTABLE_SPRITE_PROPERTIES &_sspOther ) const;

			/**
			 * Less-than operator.
			 *
			 * \param _sspOther The object against which to compare.
			 * \return Returns true if this object is less than the given object.
			 */
			LSE_INLINE LSBOOL LSE_CALL			operator < ( const LSR_SORTABLE_SPRITE_PROPERTIES &_sspOther ) const;
		} * LPLSR_SORTABLE_SPRITE_PROPERTIES, * const LPCLSR_SORTABLE_SPRITE_PROPERTIES;

		/** A shader pre-defined macro. */
		typedef struct LDR_SHADER_DEFINE {
			CString								sMacro;							/**< The macro. */
			CString								sDefinition;					/**< The definition. */
		} * LPLDR_SHADER_DEFINE, * const LPCLDR_SHADER_DEFINE;


		// == Members.
		/** Map of sprite properties and the shaders created from them. */
		CLinearMap<LSR_SORTABLE_SPRITE_PROPERTIES, CSharedShaderPtr>
												m_lmShaders;

		/** Paths to search for shaders. */
		CVector<CString>						m_vShaderPaths;


		// == Functions.
		/**
		 * Finds a shader file by searching shader directories.
		 *
		 * \param _pcName Name of the shader file to find.
		 * \return Returns a string containing the full path to a valid shader file or an empty string if the file could be found.
		 */
		CString LSE_CALL						FindFile( const char * _pcPath ) const;

		/**
		 * Loads a file to memory and stores it in the map containing loaded files.  If the file is already in the
		 *	map it is not loaded again.
		 *
		 * \param _sPath Path to find/load.
		 * \param _vpdResult Loaded file.
		 * \return Returns the loaded-to-memory file specified by the given path.
		 */
		LSBOOL LSE_CALL							LoadFile( const CString &_sPath, CVectorPoD<uint8_t, uint32_t> &_vpdResult ) const;

		/**
		 * Generates macros for a given shader type based on the sprite properties.
		 *
		 * \param _spProps Sprite properties.
		 * \param _stType Shader type.
		 * \param _vReturn Holds the returned array of defines.
		 * \return Returns true if there was enough memory to complete the operation.
		 */
		LSBOOL LSE_CALL							GenerateMacros( const LSR_SPRITE_PROPERTIES &_spProps, LSG_SHADER_TYPES _stType, CVector<LDR_SHADER_DEFINE> &_vReturn );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Equality operator.
	 *
	 * \param _sspOther The object against which to compare.
	 * \return Returns true if the objects are the same.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSpriteShaderManager::LSR_SORTABLE_SPRITE_PROPERTIES::operator == ( const LSR_SORTABLE_SPRITE_PROPERTIES &_sspOther ) const {
		return CStd::MemCmpF( this, &_sspOther, sizeof( LSR_SORTABLE_SPRITE_PROPERTIES ) );
	}

	/**
	 * Less-than.
	 *
	 * \param _sspOther The object against which to compare.
	 * \return Returns true if this object is less than the given object.
	 */
	LSE_INLINE LSBOOL LSE_CALL CSpriteShaderManager::LSR_SORTABLE_SPRITE_PROPERTIES::operator < ( const LSR_SORTABLE_SPRITE_PROPERTIES &_sspOther ) const {
		return CStd::MemCmp( this, &_sspOther, sizeof( LSR_SORTABLE_SPRITE_PROPERTIES ) ) < 0;
	}

}	// namespace lsr

#endif	// __LSR_SPRITESHADERMANAGER_H__
