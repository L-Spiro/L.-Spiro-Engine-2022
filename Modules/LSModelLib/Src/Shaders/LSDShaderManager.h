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
 * Description: A shader manager holds all of the key translators, receives shader keys and translator names,
 *	selects the appropriate translator, and generates shared shaders.  Shaders that have already been created
 *	are returned immediately.  Shader keys are modified to contain meta-data which improves performance when
 *	checking to see if the shader has already been created.  This meta-data should not be modified anywhere
 *	else.
 */


#ifndef __LSD_SHADERMANAGER_H__
#define __LSD_SHADERMANAGER_H__

#include "../LSDModelLib.h"
#include "LSDShaderKey.h"
#include "LSDShaderKeyTranslator.h"
#include "LSDShaderNameKeyPair.h"
#include "Map/LSTLLinearMap.h"

namespace lsd {

	/**
	 * Class CShaderManager
	 * \brief A shader manager holds all of the key translators, receives shader keys and translator names,
	 *	selects the appropriate translator, and generates shared shaders.
	 *
	 * Description: A shader manager holds all of the key translators, receives shader keys and translator names,
	 *	selects the appropriate translator, and generates shared shaders.  Shaders that have already been created
	 *	are returned immediately.  Shader keys are modified to contain meta-data which improves performance when
	 *	checking to see if the shader has already been created.  This meta-data should not be modified anywhere
	 *	else.
	 */
	class CShaderManager {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CShaderManager();
		LSE_CALLCTOR							~CShaderManager();


		// == Functions.
		/**
		 * Translates a shader name and key pair and produces a shared pointer.  If the shader key has already been translated
		 *	into a shader, the existing shader is returned.
		 *
		 * \param _skpPair Shader name and key to translate into a shared shader.
		 * \return Returns a shared shader representing the shader key.
		 */
		CSharedShaderPtr LSE_CALL				Translate( const LSD_SHADERNAMEKEYPAIR &_skpPair );

		/**
		 * Creates a shader-key translator object based off a shader-key translator structure.  The created key must either be
		 *	passed successfully to AddTranslator() or manually deleted via LSEDELETE.
		 *
		 * \param _stTrans The shader-key translator structure off of which a shader-key translator object is to be
		 *	created.
		 * \return Returns the created shader-key translator or NULL if there was not enough memory to create one.
		 */
		CShaderKeyTranslator * LSE_CALL			CreateTranslator( const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR &_stTrans );

		/**
		 * Adds a translator to the manager.  If a translator with the same name already exists, nothing happens.
		 *	The translator is destroyed by the manager via LSEDELETE when Reset() is called.  The reference is set to NULL
		 *	if the deletion of the object will be handled by the manager.  If, upon return, _psktTranslator is not set to
		 *	NULL, the user is resposible for deleting the object.  The safest policy is therefor to always call LSEDELETE
		 *	on _psktTranslator after calling this method.
		 *
		 * \param _psktTranslator The translator, which must have been created via LSENEW, to add to the manager.
		 * \return Returns true if there was enough memory to store the translator.  Upon failure, _psktTranslator is not
		 *	added to the manager and therefor must be destroyed manually via LSEDELETE.
		 */
		LSBOOL LSE_CALL							AddTranslator( CShaderKeyTranslator * &_psktTranslator );

		/**
		 * Adds a translator to the manager based off a descriptive structure.
		 *
		 * \param _stTrans The translator descriptor.
		 * \return Returns true if there was enough memory to create and store the translator.
		 */
		LSBOOL LSE_CALL							AddTranslator( const CShaderKeyTranslator::LSD_SHADER_TRANSLATOR &_stTrans );

		/**
		 * Resets the shader manager back to scratch, releasing all allocated memory.
		 */
		void LSE_CALL							Reset();


	protected :
		// == Types.
		/** Each translator is associated with the shader keys it is translated.  This stores a pointer to
		 *	the translator and to a map of shader-keys/shaders. */
		typedef struct LSD_SHADER_TRANSLATOR_AND_KEYS {
			CLinearMap<LSD_SHADERKEY, CSharedShaderPtr> *
												plmTranslated;							/**< Keys that have been translated into shaders. */
			CShaderKeyTranslator *				psktTranslator;							/**< Translator. */
		} * LPLSD_SHADER_TRANSLATOR_AND_KEYS, * const LPCLSD_SHADER_TRANSLATOR_AND_KEYS;


		// == Members.
		/** Association between translator names and the translators themselves. */
		CLinearMap<CNameKey, LSD_SHADER_TRANSLATOR_AND_KEYS *>
												m_lmTranslators;

		/** Paths to search for shaders. */
		CVector<CString>						m_vShaderPaths;

		/** Loaded files. */
		mutable CLinearMap<CString, CVectorPoD<uint8_t, uint32_t> >
												m_lmLoadedShaderFiles;


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
		 * \return Returns the loaded-to-memory file specified by the given path.
		 */
		const CVectorPoD<uint8_t, uint32_t> * LSE_CALL
												LoadFile( const CString &_sPath ) const;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

}	// namespace lsd

#endif	// __LSD_SHADERMANAGER_H__
