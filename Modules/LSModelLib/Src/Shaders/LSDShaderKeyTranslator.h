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
 * Description: Translates a shader key into macros to be passed to a shader.  A translator is named and is
 *	selected by the system for translation based on its name, which will be one of the parameters in a high-level
 *	effects-type language.
 */


#ifndef __LSD_SHADERKEYTRANSLATOR_H__
#define __LSD_SHADERKEYTRANSLATOR_H__

#include "../LSDModelLib.h"
#include "LSDNameKey.h"
#include "LSDShaderKey.h"
#include "ShaderLanguage/LSGShader.h"
#include "Vector/LSTLVectorPoD.h"
#include "Vector/LSTLVector.h"

namespace lsd {

	/**
	 * Class CShaderKeyTranslator
	 * \brief Translates a shader key into macros to be passed to a shader.
	 *
	 * Description: Translates a shader key into macros to be passed to a shader.  A translator is named and is
	 *	selected by the system for translation based on its name, which will be one of the parameters in a high-level
	 *	effects-type language.
	 */
	class CShaderKeyTranslator {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CShaderKeyTranslator();
		LSE_CALLCTOR							~CShaderKeyTranslator();


		// == Types.
		/** A shader pre-defined macro. */
		typedef struct LSD_SHADER_DEFINE {
			CString								sMacro;							/**< The macro. */
			CString								sDefinition;					/**< The definition. */
		} * LPLSD_SHADER_DEFINE, * const LPCLSD_SHADER_DEFINE;


		/** Translation callback.  Takes a shader key on input and adds as many macros to the given
		 *	dictionary as needed.  Any number of these can be added to a shader-key translator,
		 *	allowing each callback to translate any number of bits/members of the shader key.  Typically,
		 *	1 shader-translator callback translates 1 bit or member of the shader key, but a single
		 *	callback could also translate multiple bits/members, particularly those that occur frequently. */
		typedef LSBOOL (LSE_CALL *				PfTransFunc)( const LSD_SHADERKEY &_skKey, CVector<LSD_SHADER_DEFINE> &_vDefines );

		/** Information for creating a translator. */
		typedef struct LSD_SHADER_TRANSLATOR {
			const char *						pcName;						/**< Name of the translator. */
			const char *						pcShaderPaths[LSG_ST_TOTAL];/**< Shader paths. */
			const CShaderKeyTranslator::PfTransFunc *
												ptfCallbacks;				/**< Array of callbacks to set. */
			uint32_t							ui32TotalCallbacks;			/**< Total callbacks to which ptfCallbacks points. */
		} * LPLSD_SHADER_TRANSLATOR, * const LPCLSD_SHADER_TRANSLATOR;


		// == Operators.
		/**
		 * Less-than operator.
		 *
		 * \param _sktOther Object against which to compare.
		 * \return Returns true if this object compares less than the given object.
		 */
		LSE_INLINE bool LSE_FCALL				operator < ( const CShaderKeyTranslator &_sktOther ) const;

		/**
		 * Equality operator.
		 *
		 * \param _sktOther Object against which to compare.
		 * \return Returns true if this object equals the given object.
		 */
		LSE_INLINE bool LSE_FCALL				operator == ( const CShaderKeyTranslator &_sktOther ) const;


		// == Functions.
		/**
		 * Registers the given callback.  The callback will be used for shader-key translation purposes.
		 *
		 * \param _ptfFunc The callback to register.
		 * \return Returns true if there was enough memory to add the callback.
		 */
		LSE_INLINE LSBOOL LSE_FCALL				RegisterCallback( PfTransFunc _ptfFunc );

		/**
		 * Sets the name of the translator.
		 *
		 * \param _pcName Name of the translator.
		 */
		void LSE_CALL							SetName( const char * _pcName );

		/**
		 * Gets the name of the translator.
		 *
		 * \return Returns the name of the translator.
		 */
		LSE_INLINE const CNameKey & LSE_CALL	Name() const;

		/**
		 * Sets the input translator.  This is useful for adding custom macros or changing the names
		 *	of the inputs.
		 *
		 * \param _ptfFunc The callback for handling vertex-shader inputs or NULL to use the default handler.
		 */
		LSE_INLINE void LSE_FCALL				SetVsInputHandler( PfTransFunc _ptfFunc );

		/**
		 * Sets a path to a shader.
		 *
		 * \param _stType The shader type.
		 * \param _pcPath Path to the shader.
		 */
		LSE_INLINE void LSE_CALL				SetShaderPath( LSG_SHADER_TYPES _stType, const char * _pcPath );

		/**
		 * Gets a shader path.
		 *
		 * \param _stType The shader type.
		 * \return Returns a constant reference to a string containing the requested shader's path.
		 */
		LSE_INLINE const CString & LSE_FCALL	GetShaderPath( LSG_SHADER_TYPES _stType ) const;

		/**
		 * Translates a shader key, supplying macros based off the flags and values in the key.
		 *
		 * \param _skKey The shader key to translate.
		 * \param _vDefines The resulting array of macros from the translation process.
		 * \return Returns true if there was enough memory to supply all macros for the translation process.
		 */
		LSBOOL LSE_CALL							Translate( const LSD_SHADERKEY &_skKey, CVector<LSD_SHADER_DEFINE> &_vDefines ) const;


	protected :
		// == Members.
		/** Name of the translator. */
		CNameKey								m_nkName;

		/** Path to the shaders. */
		CString									m_sShaderPaths[LSG_ST_TOTAL];

		/** Array of translation callbacks. */
		CVectorPoD<PfTransFunc, uint16_t, 8>	m_vCallbacks;

		/** Callback for translating vertex-buffer inputs. */
		PfTransFunc								m_ptfInputCallback;


		// == Functions.
		/**
		 * The default function for translating vertex-buffer inputs.
		 *
		 * \param _skKey The key to parse.
		 * \param _vDefines Holds generated macros.
		 * \return Returns true if there was enough memory to add all necessary macros to the dictionary.
		 */
		static LSBOOL LSE_CALL					DefaultInputTranslator( const LSD_SHADERKEY &_skKey, CVector<LSD_SHADER_DEFINE> &_vDefines );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Less-than operator.
	 *
	 * \param _sktOther Object against which to compare.
	 * \return Returns true if this object compares less than the given object.
	 */
	LSE_INLINE bool LSE_FCALL CShaderKeyTranslator::operator < ( const CShaderKeyTranslator &_sktOther ) const {
		return m_nkName < _sktOther.m_nkName;
	}

	/**
	 * Equality operator.
	 *
	 * \param _sktOther Object against which to compare.
	 * \return Returns true if this object equals the given object.
	 */
	LSE_INLINE bool LSE_FCALL CShaderKeyTranslator::operator == ( const CShaderKeyTranslator &_sktOther ) const {
		return m_nkName == _sktOther.m_nkName;
	}

	// == Functions.
	/**
	 * Registers the given callback.  The callback will be used for shader-key translation purposes.
	 *
	 * \param _ptfFunc The callback to register.
	 * \return Returns true if there was enough memory to add the callback.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CShaderKeyTranslator::RegisterCallback( PfTransFunc _ptfFunc ) {
		return m_vCallbacks.Push( _ptfFunc );
	}

	/**
	 * Gets the name of the translator.
	 *
	 * \return Returns the name of the translator.
	 */
	LSE_INLINE const CNameKey & LSE_CALL CShaderKeyTranslator::Name() const {
		return m_nkName;
	}

	/**
	 * Sets the input translator.  This is useful for adding custom macros or changing the names
	 *	of the inputs.
	 *
	 * \param _ptfFunc The callback for handling vertex-shader inputs or NULL to use the default handler.
	 */
	LSE_INLINE void LSE_FCALL CShaderKeyTranslator::SetVsInputHandler( PfTransFunc _ptfFunc ) {
		m_ptfInputCallback = _ptfFunc ? _ptfFunc : DefaultInputTranslator;
	}

	/**
	 * Sets a path to a shader.
	 *
	 * \param _stType The shader type.
	 * \param _pcPath Path to the shader.
	 */
	LSE_INLINE void LSE_CALL CShaderKeyTranslator::SetShaderPath( LSG_SHADER_TYPES _stType, const char * _pcPath ) {
		assert( _stType < LSG_ST_TOTAL );
		m_sShaderPaths[_stType] = _pcPath;
	}

	/**
	 * Gets a shader path.
	 *
	 * \param _stType The shader type.
	 * \return Returns a constant reference to a string containing the requested shader's path.
	 */
	LSE_INLINE const CString & LSE_FCALL CShaderKeyTranslator::GetShaderPath( LSG_SHADER_TYPES _stType ) const {
		assert( _stType < LSG_ST_TOTAL );
		return m_sShaderPaths[_stType];
	}

}	// namespace lsd

#endif	// __LSD_SHADERKEYTRANSLATOR_H__
