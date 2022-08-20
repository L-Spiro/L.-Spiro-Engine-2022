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

#include "LSXMisc.h"


namespace lsx {

	// == Functions.
	/**
	 * Gather objects of a certain kind.
	 *
	 * \param _pNode The node to search recursively for objects.
	 * \param _eType The type of node to gather.
	 * \param _naArray The returned array of gathered nodes.
	 * \param _bVisibleOnly If true, only visible nodes are gathered.
	 */
	LSVOID LSE_CALL CMisc::GatherNodes( FbxNode * _pNode, FbxNodeAttribute::EType _eType, LSX_NODE_ARRAY &_naArray, LSBOOL _bVisibleOnly ) {
		if ( _bVisibleOnly && !_pNode->GetVisibility() ) { return; }
		for ( int I = 0; I < _pNode->GetNodeAttributeCount(); ++I ) {
			// Get the node type.
			if ( _pNode->GetNodeAttributeByIndex( I )->GetAttributeType() == _eType ) {
				_naArray.Push( _pNode );
			}
		}

		for ( int I = 0; I < _pNode->GetChildCount(); ++I ) {
			GatherNodes( _pNode->GetChild( I ), _eType, _naArray, _bVisibleOnly );
		}
	}

	/**
	 * Counts the number of visible child nodes on a node.
	 *
	 * \param _pNode The node whose visible children are to be counted.
	 * \return Returns the number of child nodes that are visible.
	 */
	LSUINT32 LSE_CALL CMisc::CountVisibleChildNodes( FbxNode * _pNode ) {
		LSUINT32 ui32Total = 0;
		for ( int I = 0; I < _pNode->GetChildCount(); ++I ) {
			if ( _pNode->GetChild( I )->GetVisibility() ) { ++ui32Total; }
		}
		return ui32Total;
	}

	/**
	 * Is an object of a given type?
	 *
	 * \param _pNode The node to check for being of the given type.
	 * \param _eType The type of node to check.
	 * \return Returns true if the node is of the given type.
	 */
	LSBOOL LSE_CALL CMisc::NodeIsType( FbxNode * _pNode, FbxNodeAttribute::EType _eType ) {
		for ( int I = 0; I < _pNode->GetNodeAttributeCount(); ++I ) {
			// Get the node type.
			if ( _pNode->GetNodeAttributeByIndex( I )->GetAttributeType() == _eType ) {
				return true;
			}
		}
		return false;
	}

	/**
	 * Returns the model-library node type given an FBX node type.
	 *
	 * \param _eType The FBX node type to convert.
	 * \return Returns the matching model-library node type.
	 */
	LSD_MODEL_ATTRIBUTES LSE_CALL CMisc::FbxNodeToModelLibraryNode( FbxNodeAttribute::EType _eType ) {
		switch ( _eType ) {
			case FbxNodeAttribute::eSkeleton : { return LSD_MA_OBJECT_JOINT; }
			case FbxNodeAttribute::eMesh : { return LSD_MA_OBJECT_MESH; }
			case FbxNodeAttribute::eCamera : { return LSD_MA_OBJECT_CAMERA; }
			case FbxNodeAttribute::eCameraStereo : { return LSD_MA_OBJECT_STEREOCAMERA; }
			case FbxNodeAttribute::eLight : { return LSD_MA_OBJECT_LIGHT; }
			default: { return LSD_MA_OBJECT_NODE; }
		}
	}

	/**
	 * Writes a string to a file.
	 *
	 * \param _sString The string to write to the file.
	 * \param _fFile The file to which to write the string.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteString( const CString &_sString, LSF_FILE &_fFile ) {
		LSUINT32 ui32Len = _sString.Length();
		if ( !CFileLib::WriteFile( _fFile, &ui32Len, sizeof( ui32Len ) ) ) { return false; }
		if ( !CFileLib::WriteFile( _fFile, _sString.CStr(), ui32Len ) ) { return false; }
		return true;
	}

	/**
	 * Writes a string to a buffer.
	 *
	 * \param _sString The string to write to the buffer.
	 * \param _fFile The buffer to which to write the string.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteString( const CString &_sString, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		if ( !CMisc::WriteUInt( _sString.Length(), _bBuffer ) ) { return false; }
		if ( !CMisc::WriteBytes( reinterpret_cast<const LSUINT8 *>(_sString.CStr()), _sString.Length(), _bBuffer ) ) { return false; }
		return true;
	}

	/**
	 * Writes an unsigned integer to a file.
	 *
	 * \param _ui32Val The value to write to the file.
	 * \param _fFile The file to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt( LSUINT32 _ui32Val, LSF_FILE &_fFile ) {
		if ( !CFileLib::WriteFile( _fFile, &_ui32Val, sizeof( _ui32Val ) ) ) { return false; }
		return true;
	}

	/**
	 * Writes an unsigned integer to a buffer.
	 *
	 * \param _ui32Val The value to write to the file.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt( LSUINT32 _ui32Val, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_ui32Val), sizeof( _ui32Val ), _bBuffer );
	}

	/**
	 * Writes an unsigned 64-bit integer to a file.
	 *
	 * \param _ui64Val The value to write to the file.
	 * \param _fFile The file to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt64( LSUINT64 _ui64Val, LSF_FILE &_fFile ) {
		if ( !CFileLib::WriteFile( _fFile, &_ui64Val, sizeof( _ui64Val ) ) ) { return false; }
		return true;
	}

	/**
	 * Writes an unsigned 64-bit integer to a buffer.
	 *
	 * \param _ui64Val The value to write to the buffer.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt64( LSUINT64 _ui64Val, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_ui64Val), sizeof( _ui64Val ), _bBuffer );
	}

	/**
	 * Writes a byte to a file.
	 *
	 * \param _ui8Val The value to write to the file.
	 * \param _fFile The file to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteUChar( LSUINT8 _ui8Val, LSF_FILE &_fFile ) {
		if ( !CFileLib::WriteFile( _fFile, &_ui8Val, sizeof( _ui8Val ) ) ) { return false; }
		return true;
	}

	/**
	 * Writes a byte to a buffer.
	 *
	 * \param _ui8Val The value to write to the buffer.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUChar( LSUINT8 _ui8Val, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_ui8Val), sizeof( _ui8Val ), _bBuffer );
	}

	/**
	 * Writes an unsigned 16-bit integer to a buffer.
	 *
	 * \param _ui16Val The value to write to the buffer.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteUInt16( LSUINT16 _ui16Val, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_ui16Val), sizeof( _ui16Val ), _bBuffer );
	}

	/**
	 * Writes a floating-point value to a file.
	 *
	 * \param _fVal The value to write to the file.
	 * \param _fFile The file to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteFloat( LSFLOAT _fVal, LSF_FILE &_fFile ) {
		if ( !CFileLib::WriteFile( _fFile, &_fVal, sizeof( _fVal ) ) ) { return false; }
		return true;
	}

	/**
	 * Writes a floating-point value to a buffer.
	 *
	 * \param _fVal The value to write to the buffer.
	 * \param _bBuffer The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteFloat( LSFLOAT _fVal, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_fVal), sizeof( _fVal ), _bBuffer );
	}

	/**
	 * Writes a matrix to a file.
	 *
	 * \param _mMat The matrix to write to the file.
	 * \param _fFile The file to which to write the value.
	 * \param Returns true only if all bytes are written to the given file.
	 */
	LSBOOL LSE_CALL CMisc::WriteMatrix( const CMatrix4x4 &_mMat, LSF_FILE &_fFile ) {
		for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
			for ( LSUINT32 X = 0UL; X < 4UL; ++X ) {
				if ( !CMisc::WriteFloat( _mMat( Y, X ), _fFile ) ) { return false; }
			}
		}
		return true;
	}

	/**
	 * Writes a matrix to a buffer.
	 *
	 * \param _mMat The matrix to write to the buffer.
	 * \param _fFile The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteMatrix( const CMatrix4x4 &_mMat, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return WriteBytes( reinterpret_cast<const LSUINT8 *>(&_mMat), sizeof( _mMat ), _bBuffer );
	}

	/**
	 * Writes only the 3-by-4 part of a matrix to a buffer.
	 *
	 * \param _mMat The matrix to write to the buffer.
	 * \param _fFile The buffer to which to write the value.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteMatrixSimple( const CMatrix4x4 &_mMat, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
			for ( LSUINT32 X = 0UL; X < 3UL; ++X ) {
				if ( !CMisc::WriteFloat( _mMat( Y, X ), _bBuffer ) ) { return false; }
			}
		}
		return true;
	}

	/**
	 * Writes bytes to a buffer.
	 *
	 * \param _pui8Bytes The values to write to the buffer.
	 * \param _ui32Length Number of bytes to write to the buffer.
	 * \param _bBuffer The buffer to which to write the bytes.
	 * \param Returns true only if all bytes are written to the given buffer.
	 */
	LSBOOL LSE_CALL CMisc::WriteBytes( const LSUINT8 * _pui8Bytes, LSUINT32 _ui32Length, lsc::CMisc::LSC_BUFFER &_bBuffer ) {
		return _bBuffer.Append( _pui8Bytes, _ui32Length );
	}

	/**
	 * Pads the given buffer to the given multiple.
	 *
	 * \param _bBuffer Buffer to pad.
	 * \param _ui32Multiple Multilple up to which to pad the given buffer.
	 * \return Returns true if the padding was successful.
	 */
	LSBOOL LSE_CALL CMisc::Pad( lsc::CMisc::LSC_BUFFER &_bBuffer, LSUINT32 _ui32Multiple ) {
		while ( _bBuffer.Length() % _ui32Multiple ) {
			if ( !_bBuffer.Push( 0 ) ) { return false; }
		}
		return true;
	}

	/**
	 * Converts an FBX matrix to an L. Spiro Engine matrix.
	 *
	 * \param _mMat The FBX matrix to convert.
	 * \return Returns the converted matrix.
	 */
	CMatrix4x4 LSE_CALL CMisc::ConvertMatrix( const FbxMatrix &_mMat ) {
		CMatrix4x4 mRet;
		for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
			for ( LSUINT32 X = 0UL; X < 4UL; ++X ) {
				mRet( X, Y ) = static_cast<LSREAL>(_mMat.Get( X, Y ));
			}
		}
		return mRet;
	}

	/**
	 * Converts an FBX matrix to an L. Spiro Engine matrix.
	 *
	 * \param _amMat The FBX matrix to convert.
	 * \return Returns the converted matrix.
	 */
	CMatrix4x4 LSE_CALL CMisc::ConvertMatrix( const FbxAMatrix &_amMat ) {
		CMatrix4x4 mRet;
		for ( LSUINT32 Y = 0UL; Y < 4UL; ++Y ) {
			for ( LSUINT32 X = 0UL; X < 4UL; ++X ) {
				mRet( X, Y ) = static_cast<LSREAL>(_amMat.Get( X, Y ));
			}
		}
		return mRet;
	}

}	// namespace lsx
