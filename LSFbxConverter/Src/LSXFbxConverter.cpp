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

#include "LSXFbxConverter.h"
#include "FileStream/LSFFileStream.h"
#include "Scene/LSXScene.h"
#include "Time/LSSTDTime.h"
#include "Vector/LSMVector3Base.h"
#include <iostream>


/**
 * The main entrypoint for this program.
 *
 * \param _i32Args Number of strings to which _pwcArgv points.
 * \param _pwcArgv Pointer to an array of strings composing the command line passed to this application.
 * \return Returns 0 in the case of success, non-zero otherwise.  The error returned will be one of the
 *	LSSTD_E_* enumerated values and can be printed via lsstd::PrintError().
 */
LSINT32 LSE_CCALL wmain( LSINT32 _i32Args, LSUTFX * _pwcArgv[] ) {
	--_i32Args;
	++_pwcArgv;

	if ( _i32Args == 0 ) {
		return LSSTD_E_INVALIDCALL;
	}

	CMemLib::Init( 0x10000, true );
	CFilesEx::InitFilesEx();
	CImageLib::InitImageLibrary();
	{
#define return									CImageLib::ShutDownImageLibrary(); CFilesEx::ShutDownFilesEx(); CMemLib::Destroy(); return
#define LSX_ERROR( CODE )						oOptions.slInputs.Reset();						\
												oOptions.slOutputs.Reset();						\
												CStd::PrintError( CODE );						\
												CStd::DebugPrintA( "\r\n" );					\
												if ( oOptions.bPause ) { ::system( "pause" ); }	\
												return CODE
		// Get the command-line options and fill out the options structure.
		lsx::CFbxConverter::LSX_OPTIONS oOptions = {
			CStringList(),						// slInputs
			CStringList(),						// slOutputs
			false,								// bPause
		};

		for ( LSINT32 I = 0; I < _i32Args; ++I ) {
#define LSX_VERIFY_INPUT( STRING, ARGS )		CStd::WStrICmp( &_pwcArgv[I][1], L ## #STRING ) == 0 && (I + (ARGS)) < _i32Args
			if ( _pwcArgv[I][0] == L'-' ) {
				// Input files.
				if ( LSX_VERIFY_INPUT( file, 1 ) ) {
					if ( !oOptions.slInputs.Push( CString::CStringFromUtfX( _pwcArgv[++I] ) ) ) {
						LSX_ERROR( LSSTD_E_OUTOFMEMORY );
					}
				}
				// Output files.
				else if ( LSX_VERIFY_INPUT( outfile, 1 ) ) {
					// Make sure the output list has 1 fewer entries than the input list.
					if ( oOptions.slOutputs.Length() >= oOptions.slInputs.Length() ) {
						// Too many outputs have already been submitted.
						CStd::DebugPrintA( "Too many outputs for the given number of inputs.\r\n" );
						LSX_ERROR( LSSTD_E_INVALIDCALL );
					}
					for ( LSUINT32 J = oOptions.slOutputs.Length(); oOptions.slOutputs.Length() < oOptions.slInputs.Length() - 1UL; ++J ) {
						// Make a string by copying the input string and changing the extension.
						LSCHAR szPath[LSF_MAX_PATH];
						CStd::StrNCpy( szPath, oOptions.slInputs[I].CStr(), LSF_MAX_PATH );
						CStd::StrNCat( CFileLib::NoExtension( szPath ), ".lsm", LSF_MAX_PATH - CStd::StrLen( szPath ) );
						if ( !oOptions.slOutputs.Push( szPath ) ) {
							LSX_ERROR( LSSTD_E_OUTOFMEMORY );
						}
					}
					if ( !oOptions.slOutputs.Push( CString::CStringFromUtfX( _pwcArgv[++I] ) ) ) {
						LSX_ERROR( LSSTD_E_OUTOFMEMORY );
					}
				}
				// Pausing.
				else if ( LSX_VERIFY_INPUT( pause, 0 ) ) {
					oOptions.bPause = true;
				}
			}
#undef LSX_VERIFY_INPUT
		}



		// Ensure the same number of outputs as there are inputs.
		for ( LSUINT32 J = oOptions.slOutputs.Length(); oOptions.slOutputs.Length() < oOptions.slInputs.Length(); ++J ) {
			// Make a string by copying the input string and changing the extension.
			LSCHAR szPath[LSF_MAX_PATH];
			CStd::StrNCpy( szPath, oOptions.slInputs[J].CStr(), LSF_MAX_PATH );
			CStd::StrNCat( CFileLib::NoExtension( szPath ), ".lsm", LSF_MAX_PATH - CStd::StrLen( szPath ) );
			if ( !oOptions.slOutputs.Push( szPath ) ) {
				LSX_ERROR( LSSTD_E_OUTOFMEMORY );
			}
		}


		CTime tTime;
		LSSTD_ERRORS eError = lsx::CFbxConverter::Process( oOptions );
		if ( eError != LSSTD_E_SUCCESS ) {
			LSX_ERROR( eError );
		}
		tTime.Update( false );
		LSCHAR szPrintfMe[512];
		::sprintf( szPrintfMe, "Total time: %.13f seconds.\r\n", tTime.GetCurMicros() / 1000000.0 );
		CStd::DebugPrintA( szPrintfMe );
		if ( oOptions.bPause ) {
			::system( "pause" );
		}
	}

	return LSSTD_E_SUCCESS;
#undef LSX_ERROR
#undef return	// Hacky so sue me.
}


namespace lsx {

	// == Various constructors.
	LSE_CALLCTOR LSX_VECTOR::LSX_VECTOR() :
		Parent( 0.0f, 0.0f, 0.0f ) {
	}
	LSE_CALLCTOR LSX_VECTOR::LSX_VECTOR( LSFLOAT _fX, LSFLOAT _fY, LSFLOAT _fZ ) :
		Parent( _fX, _fY, _fZ ) {

	}

	// == Functions.
	/**
	 * Executes the program.  Takes an LSX_OPTIONS structure on input and performs
	 *	all requested operations, ultimately generating the resulting file(s).
	 *
	 * \param _oOptions The input options for processing.
	 * \return Returns an error code determining the result of the operation.
	 */
	LSSTD_ERRORS LSE_CALL CFbxConverter::Process( const LSX_OPTIONS &_oOptions ) {
		for ( LSUINT32 I = 0; I < _oOptions.slInputs.Length(); ++I ) {
			CScene sScene;
			if ( !sScene.LoadFile( reinterpret_cast<const lsstd::LSUTF8 *>(_oOptions.slInputs[I].CStr()), "", 0 ) ) {
				return LSSTD_E_FILENOTFOUND;
			}
			if ( !sScene.LoadMeshes() ) {
				return LSSTD_E_OUTOFMEMORY;
			}
			LSSTD_ERRORS eError = sScene.SaveTo( reinterpret_cast<const lsstd::LSUTF8 *>(_oOptions.slOutputs[I].CStr()) );
			if ( eError != LSSTD_E_SUCCESS ) { return eError; }
		}

		return LSSTD_E_SUCCESS;
	}


}	// namespace lsx
