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
 * Description: The main class for the FBX Converter.
 */


#ifndef __LSX_FBXCONVERTERT_H__
#define __LSX_FBXCONVERTERT_H__

#define FBXSDK_NEW_API

// Primary headers.
#include "LSAMemLib.h"
using namespace ::lsa;

#include "LSCCompressionLib.h"
using namespace ::lsc;

#include "LSFFileLib.h"
using namespace ::lsf;

#include "LSIImageLib.h"
using namespace ::lsi;

#include "LSMMathLib.h"
using namespace ::lsm;

#include "LSDModelLib.h"
using namespace ::lsd;

#include "LSPPhysicsLib.h"
using namespace ::lsp;

#include "LSSTDStandardLib.h"
using namespace ::lsstd;

#include "LSTLib.h"
using namespace ::lstl;

// Secondary headers.
#include "FileStream/LSFFileStream.h"
#include "HighLevel/LSFFilesEx.h"
#include "Image/LSIImage.h"
#include "String/LSTLStringList.h"
#include "String/LSTLWString.h"
#include <fbxsdk.h>

namespace lsx {

	// == Types.
	/** A floating-point vector. */
	typedef struct LSX_VECTOR : public CVector3Base<LSFLOAT, LSX_VECTOR> {
		// == Various constructors.
		LSE_CALLCTOR						LSX_VECTOR();
		LSE_CALLCTOR						LSX_VECTOR( LSFLOAT _fX, LSFLOAT _fY, LSFLOAT _fZ );


	private :
		typedef CVector3Base<LSFLOAT, LSX_VECTOR>
											Parent;
	} * LPLSX_VECTOR, * const LPCLSX_VECTOR;

	/**
	 * Class CFbxConverter
	 * \brief The main class for the DXT Compressor.
	 *
	 * Description: The main class for the DXT Compressor.
	 */
	class CFbxConverter {
	public :
		// == Enumerations.
		/** Quality settings. */
		enum LSX_QUALITY {
			LSX_Q_QUICK,						/**< Fastest compression. */
			LSX_Q_NORMAL,						/**< Normal quality. */
			LSX_Q_PRODUCTION,					/**< Production quality. */
			LSX_Q_BEST,							/**< Best quality. */
		};


		// == Types.
		/** Options for compression. */
		typedef struct LSX_OPTIONS {
			/** List of input files in UTF-8 format. */
			CStringList							slInputs;

			/** List of output files in UTF-8 format. */
			CStringList							slOutputs;

			/** If true, the tool pauses after the conversion. */
			LSBOOL								bPause;
		};


		// == Functions.
		/**
		 * Executes the program.  Takes an LSX_OPTIONS structure on input and performs
		 *	all requested operations, ultimately generating the resulting file(s).
		 *
		 * \param _oOptions The input options for processing.
		 * \return Returns an error code determining the result of the operation.
		 */
		static LSSTD_ERRORS LSE_CALL			Process( const LSX_OPTIONS &_oOptions );

	protected :
		// == Members.
	};

}	// namespace lsx

#endif	// __LSX_FBXCONVERTERT_H__
