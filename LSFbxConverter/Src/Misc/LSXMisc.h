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
 * Description: Miscellaneous functions for messing with FBX files.
 */


#ifndef __LSX_MISC_H__
#define __LSX_MISC_H__

#include "../LSXFbxConverter.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Models/LSDModelAttributes.h"
#include "Vector/LSTLVector.h"

namespace lsx {

	/**
	 * Class CMisc
	 * \brief Miscellaneous functions for messing with FBX files.
	 *
	 * Description: Miscellaneous functions for messing with FBX files.
	 */
	class CMisc {
	public :
		// == Enumerations.


		// == Types.
		/** An array of nodes. */
		typedef CVector<FbxNode *, LSUINT32, 1024 * 16>	LSX_NODE_ARRAY;


		// == Functions.
		/**
		 * Gather objects of a certain kind.
		 *
		 * \param _pNode The node to search recursively for objects.
		 * \param _eType The type of node to gather.
		 * \param _naArray The returned array of gathered nodes.
		 * \param _bVisibleOnly If true, only visible nodes are gathered.
		 */
		static LSVOID LSE_CALL							GatherNodes( FbxNode * _pNode, FbxNodeAttribute::EType _eType, LSX_NODE_ARRAY &_naArray, LSBOOL _bVisibleOnly );

		/**
		 * Counts the number of visible child nodes on a node.
		 *
		 * \param _pNode The node whose visible children are to be counted.
		 * \return Returns the number of child nodes that are visible.
		 */
		static LSUINT32 LSE_CALL						CountVisibleChildNodes( FbxNode * _pNode );

		/**
		 * Is an object of a given type?
		 *
		 * \param _pNode The node to check for being of the given type.
		 * \param _eType The type of node to check.
		 * \return Returns true if the node is of the given type.
		 */
		static LSBOOL LSE_CALL							NodeIsType( FbxNode * _pNode, FbxNodeAttribute::EType _eType );

		/**
		 * Returns the model-library node type given an FBX node type.
		 *
		 * \param _eType The FBX node type to convert.
		 * \return Returns the matching model-library node type.
		 */
		static LSD_MODEL_ATTRIBUTES LSE_CALL			FbxNodeToModelLibraryNode( FbxNodeAttribute::EType _eType );

		/**
		 * Writes a string to a file.
		 *
		 * \param _sString The string to write to the file.
		 * \param _fFile The file to which to write the string.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteString( const CString &_sString, LSF_FILE &_fFile );

		/**
		 * Writes a string to a buffer.
		 *
		 * \param _sString The string to write to the buffer.
		 * \param _fFile The buffer to which to write the string.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteString( const CString &_sString, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes an unsigned integer to a file.
		 *
		 * \param _ui32Val The value to write to the file.
		 * \param _fFile The file to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteUInt( LSUINT32 _ui32Val, LSF_FILE &_fFile );

		/**
		 * Writes an unsigned integer to a buffer.
		 *
		 * \param _ui32Val The value to write to the file.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUInt( LSUINT32 _ui32Val, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes an unsigned 64-bit integer to a file.
		 *
		 * \param _ui64Val The value to write to the file.
		 * \param _fFile The file to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteUInt64( LSUINT64 _ui64Val, LSF_FILE &_fFile );

		/**
		 * Writes an unsigned 64-bit integer to a buffer.
		 *
		 * \param _ui64Val The value to write to the buffer.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUInt64( LSUINT64 _ui64Val, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes a byte to a file.
		 *
		 * \param _ui8Val The value to write to the file.
		 * \param _fFile The file to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteUChar( LSUINT8 _ui8Val, LSF_FILE &_fFile );

		/**
		 * Writes a byte to a buffer.
		 *
		 * \param _ui8Val The value to write to the buffer.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUChar( LSUINT8 _ui8Val, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes an unsigned 16-bit integer to a buffer.
		 *
		 * \param _ui16Val The value to write to the buffer.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteUInt16( LSUINT16 _ui16Val, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes a floating-point value to a file.
		 *
		 * \param _fVal The value to write to the file.
		 * \param _fFile The file to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteFloat( LSFLOAT _fVal, LSF_FILE &_fFile );

		/**
		 * Writes a floating-point value to a buffer.
		 *
		 * \param _fVal The value to write to the buffer.
		 * \param _bBuffer The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteFloat( LSFLOAT _fVal, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes a matrix to a file.
		 *
		 * \param _mMat The matrix to write to the file.
		 * \param _fFile The file to which to write the value.
		 * \param Returns true only if all bytes are written to the given file.
		 */
		static LSBOOL LSE_CALL							WriteMatrix( const CMatrix4x4 &_mMat, LSF_FILE &_fFile );

		/**
		 * Writes a matrix to a buffer.
		 *
		 * \param _mMat The matrix to write to the buffer.
		 * \param _fFile The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteMatrix( const CMatrix4x4 &_mMat, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes only the 3-by-4 part of a matrix to a buffer.
		 *
		 * \param _mMat The matrix to write to the buffer.
		 * \param _fFile The buffer to which to write the value.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteMatrixSimple( const CMatrix4x4 &_mMat, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Writes bytes to a buffer.
		 *
		 * \param _pui8Bytes The values to write to the buffer.
		 * \param _ui32Length Number of bytes to write to the buffer.
		 * \param _bBuffer The buffer to which to write the bytes.
		 * \param Returns true only if all bytes are written to the given buffer.
		 */
		static LSBOOL LSE_CALL							WriteBytes( const LSUINT8 * _pui8Bytes, LSUINT32 _ui32Length, lsc::CMisc::LSC_BUFFER &_bBuffer );

		/**
		 * Pads the given buffer to the given multiple.
		 *
		 * \param _bBuffer Buffer to pad.
		 * \param _ui32Multiple Multilple up to which to pad the given buffer.
		 * \return Returns true if the padding was successful.
		 */
		static LSBOOL LSE_CALL							Pad( lsc::CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32Multiple );

		/**
		 * Converts an FBX matrix to an L. Spiro Engine matrix.
		 *
		 * \param _mMat The FBX matrix to convert.
		 * \return Returns the converted matrix.
		 */
		static CMatrix4x4 LSE_CALL						ConvertMatrix( const FbxMatrix &_mMat );

		/**
		 * Converts an FBX matrix to an L. Spiro Engine matrix.
		 *
		 * \param _amMat The FBX matrix to convert.
		 * \return Returns the converted matrix.
		 */
		static CMatrix4x4 LSE_CALL						ConvertMatrix( const FbxAMatrix &_amMat );


	protected :
		// == Members.
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace lsx

#endif	// __LSX_MISC_H__
