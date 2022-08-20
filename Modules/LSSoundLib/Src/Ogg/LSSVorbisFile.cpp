/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2007             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: stdio-based convenience library for opening/seeking/decoding
 last mod: $Id: vorbisfile.h 17182 2010-04-29 03:48:32Z xiphmont $

 ********************************************************************/

#include "LSSVorbisFile.h"
#include "LSSVorbisConstants.h"
#include "LSSVorbisOs.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

// warning C4706: assignment within conditional expression
#pragma warning( disable : 4706 )


namespace lss {

	// == Functions.
	/**
	 * Load a given Ogg file to RAM, decompressing it and returning raw PCM data.
	 *
	 * \param _pcFile The file to load.
	 * \param _pui8Data Pointer filled with the file data allocated with _paAllocator.
	 * \param _ui32Length Length of the wave data in bytes.
	 * \param _ui32Freq Frequency of the wave data.
	 * \param _ui32Bits Bits per sample of the wave data.
	 * \param _ui32Channels Channels in the wave data.
	 * \param _paAllocator Allocator used to allocate _pui8Data, or NULL to use LSENEW.
	 * \return Returns false if the load fails or the file is not recognized as
	 *	a valid Ogg file.
	 */
	LSBOOL LSE_CALL CVorbisFile::LoadOggFile( const char * _pcFile,
		uint8_t * &_pui8Data, uint32_t &_ui32Length,
		uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator ) {
		LSS_OV_FILE ovfFile;
		if ( CVorbisFile::OvFOpen( _pcFile, &ovfFile ) ) { return false; }
		int64_t i64Pcms = OvPcmTotal( &ovfFile, -1 );
		CVorbisCodec::vorbis_info * pviInfo = OvInfo( &ovfFile, -1 );

		uint64_t ui64Size = i64Pcms * sizeof( uint16_t ) * pviInfo->channels;
		if ( ui64Size > ~0x0UL ) {
			OvClear( &ovfFile );
			return false;
		}
		_ui32Freq = pviInfo->rate;
		_ui32Bits = 16;
		_ui32Channels = pviInfo->channels;
		_ui32Length = static_cast<uint32_t>(ui64Size);
		_pui8Data = _paAllocator ?
			reinterpret_cast<uint8_t *>(_paAllocator->Alloc( _ui32Length )) :
			LSENEW uint8_t[_ui32Length];
		if ( !_pui8Data ) {
			OvClear( &ovfFile );
			return false;
		}
		int32_t i32BitStream = -1;
		int32_t i32Ret = 0, i32TotalRead = 0;

		for ( uint32_t I = 0; I < _ui32Length; I += i32Ret ) {
			i32Ret = OvRead( &ovfFile, reinterpret_cast<char *>(&_pui8Data[I]), _ui32Length - I, false, sizeof( uint16_t ),
				true, &i32BitStream );
			if ( !i32Ret ) { break; }
			i32TotalRead += i32Ret;
		}

		
		OvClear( &ovfFile );
		_ui32Length = i32TotalRead;
		
		switch ( i32Ret ) {
			case OV_HOLE : {
				CStd::DebugPrintA( "OV_HOLE returned by CVorbisFile::LoadOggFile().\r\n" );
				if ( _paAllocator ) { _paAllocator->Free( _pui8Data ); }
				else { CMemLib::Free( _pui8Data ); }
				_pui8Data = NULL;
				return false;
			}
			case OV_EBADLINK : {
				CStd::DebugPrintA( "OV_EBADLINK returned by CVorbisFile::LoadOggFile().\r\n" );
				if ( _paAllocator ) { _paAllocator->Free( _pui8Data ); }
				else { CMemLib::Free( _pui8Data ); }
				_pui8Data = NULL;
				return false;
			}
			case OV_EINVAL : {
				CStd::DebugPrintA( "OV_EINVAL returned by CVorbisFile::LoadOggFile().\r\n" );
				if ( _paAllocator ) { _paAllocator->Free( _pui8Data ); }
				else { CMemLib::Free( _pui8Data ); }
				_pui8Data = NULL;
				return false;
			}
		}


		return true;
	}

	/**
	 * Returns the actual number of PCM's in the given file.
	 *
	 * \param _ovfFile The file whose PCM count is to be obtained.
	 * \return Returns the number of PCM's in the given file.
	 */
	uint64_t LSE_CALL CVorbisFile::GetPcmCount( LSS_OV_FILE &_ovfFile ) {
		// Unfortunately an accurate number of PCM's cannot be obtained by using CVorbisFile::OvPcmTotal().
		//	The only way to get the real number of PCM's is to use CVorbisFile::OvRead().
		// Reading a large file could take time.  Start by seeking near the end of the file, using CVorbisFile::OvPcmTotal() as a guess.
		int64_t i64Pcms = OvPcmTotal( &_ovfFile, -1 );
		CVorbisCodec::vorbis_info * pviInfo = OvInfo( &_ovfFile, -1 );

		// Since the Vorbis library OvPcmTotal() function returns incorrect information, I
		//	wonder about the accuracy of OvPcmSeek()
		OvPcmSeek( &_ovfFile, i64Pcms );

		// Seeking is allowed to go past the end of the file, so we have 3 cases.
		// #1: Seeked exactly to the end of the file.
		// #2: Seeked beyond the end of the file.  Usually is the case.
		// #3: Seeked almost to the end of the file.
		int32_t i32Total = 0;
		// We can use a static buffer for reading because it doesn't matter if multiple threads try to
		//	call this function, updating the same buffer.  We don't care what happens in the buffer, we
		//	just care about the return of the function.
		static int8_t i8Buffer[4096];
		int32_t i32BitStream = -1;
		int32_t i32Ret = OvRead( &_ovfFile, reinterpret_cast<char *>(i8Buffer), sizeof( i8Buffer ), false, sizeof( uint16_t ),
			true, &i32BitStream );
		if ( i32Ret == 0 ) {
			// Either case #2 or #1.  Both are handled the same way, by creating case #3.
			i64Pcms = i64Pcms * 3LL / 4LL;
			while ( i64Pcms ) {
				OvPcmSeek( &_ovfFile, i64Pcms );
				i32Ret = OvRead( &_ovfFile, reinterpret_cast<char *>(i8Buffer), sizeof( i8Buffer ), false, sizeof( uint16_t ),
					true, &i32BitStream );
				if ( i32Ret ) { break; }

				i64Pcms = i64Pcms * 3LL / 4LL;
			}
		}

		i32Total += i32Ret;
		// Case #3.
		while ( i32Ret ) {
			i32Ret = OvRead( &_ovfFile, reinterpret_cast<char *>(i8Buffer), sizeof( i8Buffer ), false, sizeof( uint16_t ),
				true, &i32BitStream );
			i32Total += i32Ret;
		}

		// Raw PCM's to the seek point + bytes read after that (converted to PCM's) = return.
		return i64Pcms + ((i32Total >> 1) / pviInfo->channels);
	}

	/**
	 * After a bitstream has been opened using OvFOpen()/OvOpen()/OvOpenCallbacks() and decoding is complete, the application must call OvClear() to clear the decoder's buffers. OvClear() will also close the file unless it was opened using OvOpenCallbacks() with the PfCloseFunc callback set to NULL.
	 * OvClear() must also be called after a successful call to OvTest() or OvTestCallbacks().
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. After OvClear has been called, the contents of this structure are deallocated, and it can no longer be used without being reinitialized by a call to OvFOpen(), OvOpen() or OvOpenCallbacks().
	 * \return Returns 0 for success.
	 */
	int32_t CVorbisFile::OvClear( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile ) {
			CVorbisCodec::vorbis_block_clear( &_povfFile->vb );
			CVorbisCodec::vorbis_dsp_clear( &_povfFile->vd );
			COgg::ogg_stream_clear( &_povfFile->os );

			if ( _povfFile->vi && _povfFile->links ) {
				int32_t I;
				for ( I = 0;I<_povfFile->links;I++ ) {
					CVorbisCodec::vorbis_info_clear( _povfFile->vi + I );
					CVorbisCodec::vorbis_comment_clear( _povfFile->vc + I );
				}
				CMemLib::Free( _povfFile->vi );
				CMemLib::Free( _povfFile->vc );
			}
			if ( _povfFile->dataoffsets ) { CMemLib::Free( _povfFile->dataoffsets ); }
			if ( _povfFile->pcmlengths ) { CMemLib::Free( _povfFile->pcmlengths ); }
			if ( _povfFile->serialnos ) { CMemLib::Free( _povfFile->serialnos ); }
			if ( _povfFile->offsets ) { CMemLib::Free( _povfFile->offsets ); }
			COgg::ogg_sync_clear( &_povfFile->oy );
			if ( _povfFile->datasource && _povfFile->callbacks.PfCloseFunc ) {
				(_povfFile->callbacks.PfCloseFunc)( _povfFile->datasource );
			}
			CStd::MemSet( _povfFile, 0, sizeof( (*_povfFile) ) );
		}
		return 0;
	}

	/**
	 * This is the simplest function used to open and initialize an LSS_OV_FILE structure. It sets up all the related decoding structure.
	 * The first argument is a file _pcPath suitable for passing to fopen(). _povfFile should be a pointer to an empty LSS_OV_FILE structure -- this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * The _povfFile structure initialized using OvFOpen() must eventually be cleaned using OvClear().
	 * It is often useful to call OvFOpen() simply to determine whether a given file is a Vorbis bitstream. If the OvFOpen() call fails, then the file is either inaccessable (errno is set) or not recognizable as Vorbis (errno unchanged). If the call succeeds but the initialized _povfFile structure will not be used, the application is responsible for calling OvClear() to clear the decoder's buffers and close the file.
	 *
	 * \param _pcPath Null terminated string containing a file _pcPath suitable for passing to fopen().
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvFOpen( const char * _pcPath, LSS_OV_FILE * _povfFile ) {
		int32_t i32Ret;
#ifdef LSS_ORIGINAL_ROUTINE
		FILE * pfFile = fopen( _pcPath, "rb" );
		if ( !pfFile ) { return -1; }

		i32Ret = OvOpen( pfFile, _povfFile, NULL, 0 );
		if ( i32Ret ) { fclose( pfFile ); }
		return i32Ret;
#else
		LSF_FILE fFile = CFileLib::OpenFile( _pcPath, LSF_OF_READ, false );
		if ( !fFile ) { return -1; }

		static const LSS_OV_CALLBACKS ocCallbacks = {
			ReadFunc, 
			SeekFunc,
			CloseFunc,
			TellFunc
		};

		i32Ret = OvOpenCallbacks( reinterpret_cast<void *>(fFile), _povfFile, NULL, 0, ocCallbacks );
		if ( i32Ret ) { CloseFunc( fFile ); }
		return i32Ret;
#endif	// #ifdef LSS_ORIGINAL_ROUTINE
	}

	/**
	 * OvOpen is one of three initialization functions used to initialize an LSS_OV_FILE structure and prepare a bitstream for playback.
	 * WARNING for Windows developers: Do not use OvOpen() in Windows applications; Windows linking places restrictions on passing FILE * handles successfully, and OvOpen() runs afoul of these restrictions [a]. See the OvOpenCallbacks() page for details on using OvOpenCallbacks() instead.
	 * The first argument must be a file pointer to an already opened file or pipe (it need not be seekable--though this obviously restricts what can be done with the bitstream). _povfFile should be a pointer to the LSS_OV_FILE structure -- this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * The _povfFile structure initialized using OvFOpen() must eventually be cleaned using OvClear(). Once a FILE * handle is passed to OvOpen() successfully, the application MUST NOT fclose() or in any other way manipulate that file handle. Vorbisfile will close the file in OvClear(). If the application must be able to close the FILE * handle itself, see OvOpenCallbacks() with the use of OV_CALLBACKS_NOCLOSE.
	 * It is often useful to call OvOpen() simply to determine whether a given file is a Vorbis bitstream. If the OvOpen() call fails, then the file is not recognizable as Vorbis. If the call succeeds but the initialized _povfFile structure will not be used, the application is responsible for calling OvClear() to clear the decoder's buffers and close the file.
	 * If [and only if] an OvOpen() call fails, the application must explicitly fclose() the FILE * pointer itself.
	 *
	 * \param _pfFile File pointer to an already opened file or pipe (it need not be seekable--though this obviously restricts what can be done with the bitstream).
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \param _pcInitial Typically set to NULL. This parameter is useful if some data has already been read from the file and the stream is not seekable. It is used in conjunction with _i32BytesI. In this case, _pcInitial should be a pointer to a buffer containing the data read.
	 * \param _i32BytesI Typically set to 0. This parameter is useful if some data has already been read from the file and the stream is not seekable. In this case, _i32BytesI should contain the length (in bytes) of the buffer. Used together with _pcInitial.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	/*int32_t CVorbisFile::OvOpen( FILE * _pfFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, int32_t _i32BytesI ) {
		LSS_OV_CALLBACKS ocCallbacks = {
			reinterpret_cast<uintptr_t (*)( void *, uintptr_t, uintptr_t, void * )>(fread), 
			reinterpret_cast<int32_t (*)( void *, int64_t, int32_t )>(OvHeaderFSeekWrap),
			reinterpret_cast<int32_t (*)( void * )>(fclose),
			reinterpret_cast<int64_t (*)( void * )>(ftell)
		};

		return OvOpenCallbacks( static_cast<void *>(_pfFile), _povfFile, _pcInitial, _i32BytesI, ocCallbacks );
	}*/

	/**
	 * This is an alternative function used to open and initialize an LSS_OV_FILE structure when using a data source other than a file, when its necessary to modify default file access behavior, or to initialize a Vorbis decode from a FILE * pointer under Windows where OvOpen() cannot be used. It allows the application to specify custom file manipulation routines and sets up all the related decoding structures.
	 * Once OvOpenCallbacks() has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions. Unlike OvFOpen() and OvOpen(), OvOpenCallbacks() may be used to instruct vorbisfile to either automatically close or not to close the file/data access handle in OvClear(). Automatic closure is disabled by passing NULL as the close callback, or using one of the predefined callback sets that specify a NULL close callback. The application is responsible for closing a file when a call to OvOpenCallbacks() is unsuccessful.
	 * See also Callbacks and Non-stdio I/O for information on designing and specifying custom callback functions.
	 *
	 * \param _pvDataSource Pointer to a data structure allocated by the calling application, containing any state needed by the _ocCallbacks provided.
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \param _pcInitial Typically set to NULL. This parameter is useful if some data has already been read from the stream and the stream is not seekable. It is used in conjunction with _i32BytesI. In this case, _pcInitial should be a pointer to a buffer containing the data read.
	 * \param _i32BytesI Typically set to 0. This parameter is useful if some data has already been read from the stream and the stream is not seekable. In this case, _i32BytesI should contain the length (in bytes) of the buffer. Used together with _pcInitial.
	 * \param _ocCallbacks A completed LSS_OV_CALLBACKS struct which indicates desired custom file manipulation routines. vorbisfile.h defines several preprovided callback sets; see LSS_OV_CALLBACKS for details.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvOpenCallbacks( void * _pvDataSource, LSS_OV_FILE * _povfFile, 
		const char * _pcInitial, int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks ) {
		int32_t i32Ret = OvOpen1( _pvDataSource, _povfFile, _pcInitial, _i32BytesI, _ocCallbacks );
		if ( i32Ret ) { return i32Ret; }
		return OvOpen2( _povfFile );
	}

	/**
	 * This partially opens a vorbis file to test for Vorbis-ness. It loads the headers for the first chain and tests for seekability (but does not seek). Use OvTestOpen() to finish opening the file or OvClear to close/free it.
	 * WARNING for Windows developers: Do not use OvTest() in Windows applications; Windows linking places restrictions on passing FILE * handles successfully, and OvTest() runs afoul of these restrictions [a] in exactly the same way as OvOpen(). See the OvTestCallbacks() page for details on using OvTestCallbacks() instead.
	 *
	 * \param _pfFile File pointer to an already opened file or pipe (it need not be seekable--though this obviously restricts what can be done with the bitstream).
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \param _pcInitial Typically set to NULL. This parameter is useful if some data has already been read from the file and the stream is not seekable. It is used in conjunction with _i32BytesI. In this case, _pcInitial should be a pointer to a buffer containing the data read.
	 * \param _i32BytesI Typically set to 0. This parameter is useful if some data has already been read from the file and the stream is not seekable. In this case, _i32BytesI should contain the length (in bytes) of the buffer. Used together with _pcInitial.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	/*int32_t CVorbisFile::OvTest( FILE * _pfFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, int32_t _i32BytesI ) {
		LSS_OV_CALLBACKS ocCallbacks = {
			reinterpret_cast<uintptr_t (*)( void *, uintptr_t, uintptr_t, void * )>(fread), 
			reinterpret_cast<int32_t (*)( void *, int64_t, int32_t )>(OvHeaderFSeekWrap),
			reinterpret_cast<int32_t (*)( void * )>(fclose),
			reinterpret_cast<int64_t (*)( void * )>(ftell)
		};

		return OvTestCallbacks( static_cast<void *>(_pfFile), _povfFile, _pcInitial, _i32BytesI, ocCallbacks );
	}*/

	/**
	 * This is an alternative function used to open and test an LSS_OV_FILE structure when using a data source other than a file, when its necessary to modify default file access behavior, or to test for Vorbis content from a FILE * pointer under Windows where OvTest() cannot be used. It allows the application to specify custom file manipulation routines and sets up all the related decoding structures.
	 * Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 *
	 * \param _pvDataSource File pointer to an already opened file or pipe (it need not be seekable--though this obviously restricts what can be done with the bitstream).
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \param _pcInitial Typically set to NULL. This parameter is useful if some data has already been read from the file and the stream is not seekable. It is used in conjunction with _i32BytesI. In this case, _pcInitial should be a pointer to a buffer containing the data read.
	 * \param _i32BytesI Typically set to 0. This parameter is useful if some data has already been read from the file and the stream is not seekable. In this case, _i32BytesI should contain the length (in bytes) of the buffer. Used together with _pcInitial.
	 * \param _ocCallbacks A completed LSS_OV_CALLBACKS struct which indicates desired custom file manipulation routines. vorbisfile.h defines several preprovided callback sets; see LSS_OV_CALLBACKS for details.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTestCallbacks( void * _pvDataSource, LSS_OV_FILE * _povfFile, 
		const char * _pcInitial, int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks ) {
		return OvOpen1( _pvDataSource, _povfFile, _pcInitial, _i32BytesI, _ocCallbacks );
	}

	/**
	 * Finish opening a file partially opened with OvTest() or OvTestCallbacks().
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. Once this has been called, the same LSS_OV_FILE struct should be passed to all the libvorbisfile functions.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTestOpen( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile->ready_state != PARTOPEN ) { return OV_EINVAL; }
		return OvOpen2( _povfFile );
	}

	/**
	 * This function returns the average bitrate for the specified logical bitstream. This may be different from the OvInfo->nominal_bitrate value, as it is based on the actual average for this bitstream if the file is seekable.
	 * Nonseekable files will return the nominal bitrate setting or the average of the upper and lower bounds, if any of these values are set.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the bitrate for the entire bitstream, this parameter should be set to -1.
	 * \return OV_EINVAL indicates that an invalid argument value was submitted or that the stream represented by _povfFile is not open.
	 *	OV_FALSE means the call returned a 'false' status, which in this case most likely indicates that the file is nonseekable and the upper, lower, and nominal bitrates were unset.
	 *	n indicates the bitrate for the given logical bitstream or the entire physical bitstream. If the file is open for random (seekable) access, it will find the *actual* average bitrate. If the file is streaming (nonseekable), it returns the nominal bitrate (if set) or else the average of the upper/lower bounds (if set).
	 */
	int32_t CVorbisFile::OvBitrate( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( _i32I >= _povfFile->links ) { return OV_EINVAL; }
		if ( !_povfFile->seekable && _i32I != 0 ) { return OvBitrate( _povfFile, 0 ); }
		if ( _i32I < 0 ) {
			int64_t i64Bits = 0;
			int32_t I;
			float fBr;
			for ( I = 0; I < _povfFile->links; I++ ) {
				i64Bits += (_povfFile->offsets[I+1] - _povfFile->dataoffsets[I]) * 8;
			}
			fBr = static_cast<float>(i64Bits / OvTimeTotal( _povfFile, -1 ));
			return static_cast<int32_t>(rint( fBr ));
		}
		else {
			if ( _povfFile->seekable ) {
				// Return the actual bitrate.
				return static_cast<int32_t>(rint( (_povfFile->offsets[_i32I+1] - _povfFile->dataoffsets[_i32I]) * 8 / OvTimeTotal( _povfFile, _i32I ) ));
			}
			else {
				// Return nominal if set.
				if ( _povfFile->vi[_i32I].bitrate_nominal>0 ) {
					return _povfFile->vi[_i32I].bitrate_nominal;
				}
				else {
					if ( _povfFile->vi[_i32I].bitrate_upper > 0 ) {
						if ( _povfFile->vi[_i32I].bitrate_lower > 0 ) {
							return (_povfFile->vi[_i32I].bitrate_upper + _povfFile->vi[_i32I].bitrate_lower) / 2;
						}
						else {
							return _povfFile->vi[_i32I].bitrate_upper;
						}
					}
					return OV_FALSE;
				}
			}
		}
	}

	/**
	 * Used to find the most recent bitrate played back within the file. Will return 0 if the bitrate has not changed or it is the beginning of the file.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return 0 indicates the beginning of the file or unchanged bitrate info.
	 *	n indicates the actual bitrate since the last call.
	 *	OV_FALSE indicates that playback is not in progress, and thus there is no instantaneous bitrate information to report.
	 *	OV_EINVAL indicates that the stream represented by _povfFile is not open.
	 */
	int32_t CVorbisFile::OvBitrateInstant( LSS_OV_FILE * _povfFile ) {
		int32_t i32Link = _povfFile->seekable ? _povfFile->current_link : 0;
		int32_t i32Ret;
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( _povfFile->samptrack == 0 ) { return OV_FALSE; }
		i32Ret = static_cast<int32_t>(_povfFile->bittrack / _povfFile->samptrack * _povfFile->vi[i32Link].rate + 0.5);
		_povfFile->bittrack = 0.0;
		_povfFile->samptrack = 0.0;
		return i32Ret;
	}

	/**
	 * Returns the number of logical bitstreams within our physical bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return 1 indicates a single logical bitstream or an unseekable file.
	 *	n indicates the number of logical bitstreams.
	 */
	int32_t CVorbisFile::OvStreams( LSS_OV_FILE * _povfFile ) {
		return _povfFile->links;
	}

	/**
	 * This indicates whether or not the bitstream is seekable.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return 0 indicates that the file is not seekable.
	 *	nonzero indicates that the file is seekable.
	 */
	int32_t CVorbisFile::OvSeekable( LSS_OV_FILE * _povfFile ) {
		return _povfFile->seekable;
	}

	/**
	 * Returns the serialnumber of the specified logical bitstream link number within the overall physical bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the serial number of the current bitstream, this parameter should be set to -1.
	 * \return -1 if the specified logical bitstream _i32I does not exist.
	 *	Returns the serial number of the logical bitstream _i32I or the serial number of the current bitstream if the file is nonseekable.
	 */
	int32_t CVorbisFile::OvSerialNumber( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _i32I >= _povfFile->links ) { return OvSerialNumber( _povfFile, _povfFile->links - 1 ); }
		if ( !_povfFile->seekable && _i32I >= 0 ) { return OvSerialNumber( _povfFile, -1 ); }
		if ( _i32I < 0 ) {
			return _povfFile->current_serialno;
		}
		else {
			return _povfFile->serialnos[_i32I];
		}
	}

	/**
	 * Returns the total (compressed) bytes of the physical bitstream or a specified logical bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. To retrieve the total bytes for the entire physical bitstream, this parameter should be set to -1.
	 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is nonseekable
	 *	n total length in compressed bytes of content if _i32I=-1.
	 *	n length in compressed bytes of logical bitstream if _i32I=0 to n.
	 */
	int64_t CVorbisFile::OvRawTotal( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable || _i32I >= _povfFile->links ) { return OV_EINVAL; }
		if ( _i32I < 0 ) {
			int64_t i64Acc = 0;
			int32_t I;
			for ( I = 0; I < _povfFile->links; I++ ) {
				i64Acc += OvRawTotal( _povfFile, I );
			}
			return i64Acc;
		}
		else {
			return _povfFile->offsets[_i32I+1] - _povfFile->offsets[_i32I];
		}
	}

	/**
	 * Returns the total pcm samples of the physical bitstream or a specified logical bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. To retrieve the total pcm samples for the entire physical bitstream, this parameter should be set to -1.
	 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is unseekable.
	 *	total length in pcm samples of content if _i32I=-1.
	 *	length in pcm samples of logical bitstream if _i32I=0 to n.
	 */
	int64_t CVorbisFile::OvPcmTotal( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable || _i32I >= _povfFile->links ) { return OV_EINVAL; }
		if ( _i32I < 0 ) {
			int64_t i64Acc = 0;
			int32_t I;
			for ( I = 0; I < _povfFile->links; I++ ) {
				i64Acc += OvPcmTotal( _povfFile, I );
			}
			return i64Acc;
		}
		else {
			return _povfFile->pcmlengths[_i32I*2+1];
		}
	}

	/**
	 * Returns the total time in _dPos of the physical bitstream or a specified logical bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. To retrieve the time total for the entire physical bitstream, this parameter should be set to -1.
	 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is nonseekable.
	 *	n total length in _dPos of content if _i32I=-1.
	 *	n length in _dPos of logical bitstream if _i32I=0 to n.
	 */
	double CVorbisFile::OvTimeTotal( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable || _i32I >= _povfFile->links ) { return OV_EINVAL; }
		if ( _i32I < 0 ) {
			double dAcc = 0;
			int32_t I;
			for ( I = 0; I < _povfFile->links; I++ ) {
				dAcc += OvTimeTotal( _povfFile, I );
			}
			return dAcc;
		}
		else {
			return static_cast<double>(_povfFile->pcmlengths[_i32I*2+1]) / static_cast<double>(_povfFile->vi[_i32I].rate);
		}
	}

	/**
	 * Seeks to the offset specified (in compressed raw bytes) within the physical bitstream. This function only works for seekable streams.
	 * This also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * When seek speed is a priority, this is the best seek funtion to use.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i64Pos Position in compressed bytes to seek to in the bitstream.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvRawSeek( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		COgg::ogg_stream_state ossWorkdOs;
		int32_t i32Ret;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable ) {
			return OV_ENOSEEK;	// Don't dump machine if we can't seek.
		}

		if ( _i64Pos < 0 || _i64Pos > _povfFile->end ) { return OV_EINVAL; }

		// Is the seek position outside our current link [if any]?
		if ( _povfFile->ready_state >= STREAMSET ) {
			if ( _i64Pos <_povfFile->offsets[_povfFile->current_link] || _i64Pos >= _povfFile->offsets[_povfFile->current_link+1] ) {
				OvDecodeClear( _povfFile );		// Clear out stream state.
			}
		}

		// Don't yet clear out decoding machine (if it's initialized), in the case we're in the same link.  Restart the decode lapping, and
		//	let OvFetchAndProcessPacket deal with a potential bitstream boundary.
		_povfFile->pcm_offset = -1;
		COgg::ogg_stream_reset_serialno( &_povfFile->os, _povfFile->current_serialno ); // Must set serialno.
		CVorbisCodec::vorbis_synthesis_restart( &_povfFile->vd );

		i32Ret = OvSeekHelper( _povfFile, _i64Pos );
		if ( i32Ret ) { goto SeekError; }

		{
			COgg::ogg_page opOg;
			COgg::ogg_packet opOp;
			int32_t i32LastBlock = 0;
			int32_t i32AccBlock = 0;
			int32_t i32ThisBlock = 0;
			int32_t i32LastFlag = 0;
			int32_t i32FirstFlag = 0;
			int64_t i64PagePos = -1;

			COgg::ogg_stream_init( &ossWorkdOs, _povfFile->current_serialno );	// Get the memory ready.
			COgg::ogg_stream_reset( &ossWorkdOs );								// Eliminate the spurious OV_HOLE return from not necessarily starting from the beginning.

			while ( true ) {
				if ( _povfFile->ready_state >= STREAMSET ) {
					// Snarf/scan a packet if we can.
					int32_t i32Result = COgg::ogg_stream_packetout( &ossWorkdOs, &opOp );
					if ( i32Result > 0 ) {

						if ( _povfFile->vi[_povfFile->current_link].codec_setup ) {
							i32ThisBlock = CVorbisCodec::vorbis_packet_blocksize( _povfFile->vi + _povfFile->current_link, &opOp );
							if ( i32ThisBlock < 0 ) {
								COgg::ogg_stream_packetout( &_povfFile->os, NULL );
								i32ThisBlock = 0;
							}
							else {
								if ( i32LastFlag && !i32FirstFlag ) {
									COgg::ogg_stream_packetout( &_povfFile->os, NULL );
								}
								else {
									if ( i32LastBlock ) { i32AccBlock += (i32LastBlock + i32ThisBlock) >> 2; }
								}
							}

							if ( opOp.granulepos != -1 ) {
								int32_t I, i32Link = _povfFile->current_link;
								int64_t i64GranulePos = opOp.granulepos - _povfFile->pcmlengths[i32Link*2];
								if ( i64GranulePos < 0 ) { i64GranulePos = 0; }

								for ( I = 0; I < i32Link; I++ ) {
									i64GranulePos += _povfFile->pcmlengths[I*2+1];
								}
								_povfFile->pcm_offset = i64GranulePos - i32AccBlock;
								if ( _povfFile->pcm_offset < 0 ) { _povfFile->pcm_offset = 0; }
								break;
							}
							i32LastBlock = i32ThisBlock;
							continue;
						}
						else {
							COgg::ogg_stream_packetout( &_povfFile->os, NULL );
						}
					}
				}

				if ( !i32LastBlock ) {
					i64PagePos = OvGetNextPage( _povfFile, &opOg, -1 );
					if ( i64PagePos < 0 ) {
						_povfFile->pcm_offset = OvPcmTotal( _povfFile, -1 );
						break;
					}
				}
				else {
					// Huh?	 Bogus stream with packets but no granulepos.
					_povfFile->pcm_offset = -1;
					break;
				}

				// Has our decoding just traversed a bitstream boundary?
				if ( _povfFile->ready_state >= STREAMSET ) {
					if ( _povfFile->current_serialno != COgg::ogg_page_serialno( &opOg ) ) {

						// Two possibilities:
						//	1: Our decoding just traversed a bitstream boundary.
						//	2: Another stream is multiplexed into this logical section?

						if ( COgg::ogg_page_bos( &opOg ) ) {
							// We traversed.
							OvDecodeClear( _povfFile );		// Clear out stream state.
							COgg::ogg_stream_clear( &ossWorkdOs );
						} // Else, do nothing; next loop will scoop another page.
					}
				}

				if ( _povfFile->ready_state < STREAMSET ) {
					int32_t i32Link;
					int32_t i32SerialNo = COgg::ogg_page_serialno( &opOg );

					for ( i32Link = 0;i32Link<_povfFile->links;i32Link++ ) {
						if ( _povfFile->serialnos[i32Link] == i32SerialNo ) { break; }
					}

					if ( i32Link == _povfFile->links ) { continue; }	// Not the desired Vorbis bitstream section; keep trying.
					_povfFile->current_link = i32Link;
					_povfFile->current_serialno = i32SerialNo;
					COgg::ogg_stream_reset_serialno( &_povfFile->os, i32SerialNo );
					COgg::ogg_stream_reset_serialno( &ossWorkdOs, i32SerialNo );
					_povfFile->ready_state = STREAMSET;
					i32FirstFlag = (i64PagePos <= _povfFile->dataoffsets[i32Link]);
				}

				COgg::ogg_stream_pagein( &_povfFile->os, &opOg );
				COgg::ogg_stream_pagein( &ossWorkdOs, &opOg );
				i32LastFlag = COgg::ogg_page_eos( &opOg );

			}
		}

		COgg::ogg_stream_clear( &ossWorkdOs );
		_povfFile->bittrack = 0.0;
		_povfFile->samptrack = 0.0;
		return 0;

	SeekError :
		// Dump the machine so we're in a known state.
		_povfFile->pcm_offset = -1;
		COgg::ogg_stream_clear( &ossWorkdOs );
		OvDecodeClear( _povfFile );
		return OV_EBADLINK;
	}

	/**
	 * Seeks to the offset specified (in pcm samples) within the physical bitstream. This function only works for seekable streams.
	 * This also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i64Pos Position in pcm samples to seek to in the bitstream.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvPcmSeek( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		int32_t i32ThisBlock, i32LastBlock = 0;
		int32_t i32Ret = OcPvmSeekPage( _povfFile, _i64Pos );
		if ( i32Ret < 0 ) { return i32Ret; }
		if ( (i32Ret = OvMakeDecodeReady( _povfFile )) ) { return i32Ret; }

		// Discard leading packets we don't need for the lapping of the position we want; don't decode them.

		while ( true ) {
			COgg::ogg_packet opPacket;
			COgg::ogg_page opPage;

			i32Ret = COgg::ogg_stream_packetpeek( &_povfFile->os, &opPacket );
			if ( i32Ret > 0 ) {
				i32ThisBlock = CVorbisCodec::vorbis_packet_blocksize( _povfFile->vi + _povfFile->current_link, &opPacket );
				if ( i32ThisBlock < 0 ) {
					COgg::ogg_stream_packetout( &_povfFile->os, NULL );
					continue;	// Non-audio packet.
				}
				if ( i32LastBlock ) { _povfFile->pcm_offset += (i32LastBlock + i32ThisBlock) >> 2; }

				if ( _povfFile->pcm_offset + ((i32ThisBlock + CVorbisCodec::vorbis_info_blocksize( _povfFile->vi, 1 )) >> 2) >= _i64Pos ) { break; }

				// Remove the packet from packet queue and track its granulepos.
				COgg::ogg_stream_packetout( &_povfFile->os, NULL );
				CVorbisCodec::vorbis_synthesis_trackonly( &_povfFile->vb, &opPacket );	// Set up a vb with only tracking, no pcm_decode.
				CVorbisCodec::vorbis_synthesis_blockin( &_povfFile->vd, &_povfFile->vb );

				// End of logical stream case is hard, especially with exact length positioning.
				if ( opPacket.granulepos > -1 ) {
					int32_t I;
					// Always believe the stream markers.
					_povfFile->pcm_offset = opPacket.granulepos - _povfFile->pcmlengths[_povfFile->current_link*2];
					if ( _povfFile->pcm_offset < 0 ) { _povfFile->pcm_offset = 0; }
					for ( I = 0; I < _povfFile->current_link; I++ ) {
						_povfFile->pcm_offset += _povfFile->pcmlengths[I*2+1];
					}
				}

				i32LastBlock = i32ThisBlock;

			}
			else {
				if ( i32Ret < 0 && i32Ret != OV_HOLE ) { break; }

				// Suck in a new page.
				if ( OvGetNextPage( _povfFile, &opPage, -1 ) < 0 ) { break; }
				if ( COgg::ogg_page_bos( &opPage ) ) { OvDecodeClear( _povfFile ); }

				if ( _povfFile->ready_state < STREAMSET ) {
					int32_t i32SerialNo = COgg::ogg_page_serialno( &opPage );
					int32_t i32Link;

					for ( i32Link = 0; i32Link < _povfFile->links; i32Link++ ) {
						if ( _povfFile->serialnos[i32Link] == i32SerialNo ) { break; }
					}
					if ( i32Link == _povfFile->links ) { continue; }
					_povfFile->current_link = i32Link;

					_povfFile->ready_state = STREAMSET;
					_povfFile->current_serialno = COgg::ogg_page_serialno( &opPage );
					COgg::ogg_stream_reset_serialno( &_povfFile->os, i32SerialNo );
					i32Ret = OvMakeDecodeReady( _povfFile );
					if ( i32Ret ) { return i32Ret; }
					i32LastBlock = 0;
				}

				COgg::ogg_stream_pagein( &_povfFile->os, &opPage );
			}
		}

		_povfFile->bittrack = 0.0;
		_povfFile->samptrack = 0.0;
		// Discard samples until we reach the desired position.  Crossing a logical bitstream boundary with abandon is okay.
		{
			// Note that halfrate could be set differently in each link, but vorbisfile encoforces all links are set or unset.
			int32_t i32Hs = CVorbisCodec::vorbis_synthesis_halfrate_p( _povfFile->vi );
			while ( _povfFile->pcm_offset < ((_i64Pos >> i32Hs) << i32Hs) ) {
				int64_t i64Target = (_i64Pos - _povfFile->pcm_offset) >> i32Hs;
				int32_t i32Samples = CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, NULL );

				if ( i32Samples > i64Target ) { i32Samples = static_cast<int32_t>(i64Target); }
				CVorbisCodec::vorbis_synthesis_read( &_povfFile->vd, i32Samples );
				_povfFile->pcm_offset += i32Samples<<i32Hs;

				if ( i32Samples < i64Target ) {
					if ( OvFetchAndProcessPacket( _povfFile, NULL, 1, 1 ) <= 0 ) {
						_povfFile->pcm_offset = OvPcmTotal( _povfFile, -1 );
					}
				}
			}
		}
		return 0;
	}

	/**
	 * Seeks to the closest page preceding the specified location (in pcm samples) within the physical bitstream. This function only works for seekable streams.
	 * This function is faster than OvPcmSeek() because the function can begin decoding at a page boundary rather than seeking through any remaining samples before the specified location. However, it is less accurate.
	 * This also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i64Pos Position in pcm samples to seek to in the bitstream.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OcPvmSeekPage( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		int32_t i32Link = -1;
		int64_t i64Result = 0;
		int64_t i64Total = OvPcmTotal( _povfFile, -1 );

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable ) { return OV_ENOSEEK; }

		if ( _i64Pos < 0 || _i64Pos>i64Total ) { return OV_EINVAL; }

		// In which bitstream section does this pcm offset occur?
		for ( i32Link = _povfFile->links - 1; i32Link >= 0; i32Link-- ) {
			i64Total -= _povfFile->pcmlengths[i32Link*2+1];
			if ( _i64Pos >= i64Total ) { break; }
		}

		{
			int64_t i64End = _povfFile->offsets[i32Link+1];
			int64_t i64Begin = _povfFile->offsets[i32Link];
			int64_t i64BeginTime = _povfFile->pcmlengths[i32Link*2];
			int64_t i64EndTime = _povfFile->pcmlengths[i32Link*2+1] + i64BeginTime;
			int64_t i64Target = _i64Pos - i64Total + i64BeginTime;
			int64_t i64Best = i64Begin;

			COgg::ogg_page opPage;
			while ( i64Begin < i64End ) {
				int64_t i64BiSect;

				if ( i64End - i64Begin < CHUNKSIZE ) {
					i64BiSect = i64Begin;
				}
				else {
					// Take a (pretty decent) guess.
					i64BiSect = i64Begin +
						static_cast<int64_t>(static_cast<double>(i64Target - i64BeginTime) * (i64End - i64Begin) / (i64EndTime - i64BeginTime))
						- CHUNKSIZE;
					if ( i64BiSect < i64Begin + CHUNKSIZE ) {
						i64BiSect = i64Begin;
					}
				}

				if ( i64BiSect != _povfFile->offset ) {
					i64Result = OvSeekHelper( _povfFile, i64BiSect );
					if ( i64Result ) { goto SeekError; }
				}

				while ( i64Begin < i64End ) {
					i64Result = OvGetNextPage( _povfFile, &opPage, i64End - _povfFile->offset );
					if ( i64Result == OV_EREAD ) { goto SeekError; }
					if ( i64Result < 0 ) {
						if ( i64BiSect <= i64Begin + 1 ) {
							i64End = i64Begin;	// Found it.
						}
						else {
							if ( i64BiSect == 0 ) { goto SeekError; }
							i64BiSect -= CHUNKSIZE;
							if ( i64BiSect <= i64Begin ) { i64BiSect = i64Begin + 1; }
							i64Result = OvSeekHelper( _povfFile, i64BiSect );
							if ( i64Result ) { goto SeekError; }
						}
					}
					else {
						int64_t i64GranulePos;

						if ( COgg::ogg_page_serialno( &opPage ) != _povfFile->serialnos[i32Link] ) {
							continue;
						}

						i64GranulePos = COgg::ogg_page_granulepos( &opPage );
						if ( i64GranulePos == -1 ) { continue; }

						if ( i64GranulePos<i64Target ) {
							i64Best = i64Result;			// Raw offset of packet with i64GranulePos.
							i64Begin = _povfFile->offset;	// Raw offset of next page.
							i64BeginTime = i64GranulePos;

							if ( i64Target - i64BeginTime > 44100 ) { break; }
							i64BiSect = i64Begin;			// NOT i64Begin + 1.
						}
						else {
							if ( i64BiSect <= i64Begin + 1 ) {
								i64End = i64Begin;			// Found it.
							}
							else {
								if ( i64End == _povfFile->offset ) {
									i64End = i64Result;
									i64BiSect -= CHUNKSIZE; // An endless loop otherwise.
									if ( i64BiSect <= i64Begin ) { i64BiSect = i64Begin + 1; }
									i64Result = OvSeekHelper( _povfFile, i64BiSect );
									if ( i64Result ) { goto SeekError; }
								}
								else {
									i64End = i64BiSect;
									i64EndTime = i64GranulePos;
									break;
								}
							}
						}
					}
				}
			}

			// Found our page.  Seek to it, update pcm offset.  Easier case than raw_seek, don't keep packets preceding granulepos.
			{
				COgg::ogg_page opPage2;
				COgg::ogg_packet opPacket;

				// Seek.
				i64Result = OvSeekHelper( _povfFile, i64Best );
				_povfFile->pcm_offset = -1;
				if ( i64Result ) { goto SeekError; }
				i64Result = OvGetNextPage( _povfFile, &opPage2, -1 );
				if ( i64Result < 0 ) { goto SeekError; }

				if ( i32Link != _povfFile->current_link ) {
					// Different link; dump entire decode machine.
					OvDecodeClear( _povfFile );

					_povfFile->current_link = i32Link;
					_povfFile->current_serialno = _povfFile->serialnos[i32Link];
					_povfFile->ready_state = STREAMSET;

				}
				else {
					CVorbisCodec::vorbis_synthesis_restart( &_povfFile->vd );
				}

				COgg::ogg_stream_reset_serialno( &_povfFile->os, _povfFile->current_serialno );
				COgg::ogg_stream_pagein( &_povfFile->os, &opPage2 );

				// Pull out all but last packet; the one with granulepos.
				while ( true ) {
					i64Result = COgg::ogg_stream_packetpeek( &_povfFile->os, &opPacket );
					if ( i64Result == 0 ) {

						i64Result = OvSeekHelper( _povfFile, i64Best );
						if ( i64Result < 0 ) { goto SeekError; }

						while ( true ) {
							i64Result = OvGetPrevPage( _povfFile, &opPage2 );
							if ( i64Result < 0 ) { goto SeekError; }
							if ( COgg::ogg_page_serialno( &opPage2 ) == _povfFile->current_serialno &&
								(COgg::ogg_page_granulepos( &opPage2 ) > -1 || !COgg::ogg_page_continued( &opPage2 )) ) {
								return OvRawSeek( _povfFile, i64Result );
							}
							_povfFile->offset = i64Result;
						}
					}
					if ( i64Result < 0 ) {
						i64Result = OV_EBADPACKET;
						goto SeekError;
					}
					if ( opPacket.granulepos != -1 ) {
						_povfFile->pcm_offset = opPacket.granulepos-_povfFile->pcmlengths[_povfFile->current_link*2];
						if ( _povfFile->pcm_offset < 0 ) { _povfFile->pcm_offset = 0; }
						_povfFile->pcm_offset += i64Total;
						break;
					}
					else {
						i64Result = COgg::ogg_stream_packetout( &_povfFile->os, NULL );
					}
				}
			}
		}

		// Verify result.
		if ( _povfFile->pcm_offset>_i64Pos || _i64Pos>OvPcmTotal( _povfFile, -1 ) ) {
			i64Result = OV_EFAULT;
			goto SeekError;
		}
		_povfFile->bittrack = 0.0;
		_povfFile->samptrack = 0.0;
		return 0;

	SeekError :
		// Dump machine so we're in a known state.
		_povfFile->pcm_offset = -1;
		OvDecodeClear( _povfFile );
		return static_cast<int32_t>(i64Result);
	}

	/**
	 * For seekable streams, this seeks to the given time. For implementing seeking in a player, this is the only function generally needed. This also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position. This function does not work for unseekable streams.
	 *
	 * \param _povfFile Pointer to our already opened and initialized LSS_OV_FILE structure.
	 * \param _dPos Location to seek to within the file, specified in _dPos.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTimeSeek( LSS_OV_FILE * _povfFile, double _dPos ) {
		// Translate time to PCM position and call OvPcmSeek.
		int32_t i32Link = -1;
		int64_t i64PcmTotal = 0;
		double dTimeTotal = 0.0;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable ) { return OV_ENOSEEK; }
		if ( _dPos < 0 ) { return OV_EINVAL; }

		// In which bitstream section does this time offset occur?
		for ( i32Link = 0; i32Link < _povfFile->links; i32Link++ ) {
			double dAddSec = OvTimeTotal( _povfFile, i32Link );
			if ( _dPos < dTimeTotal + dAddSec ) { break; }
			dTimeTotal += dAddSec;
			i64PcmTotal += _povfFile->pcmlengths[i32Link*2+1];
		}

		if ( i32Link == _povfFile->links ) { return OV_EINVAL; }

		// Enough information to convert time offset to pcm offset.
		{
			int64_t i64Target = static_cast<int64_t>(i64PcmTotal + (_dPos - dTimeTotal) * _povfFile->vi[i32Link].rate);
			return OvPcmSeek( _povfFile, i64Target );
		}
	}

	/**
	 * For seekable streams, this seeks to closest full page preceding the given time. This function is faster than OvTimeSeek() because it doesn't seek through the last few samples to reach an exact time, but it is also less accurate. This should be used when speed is important.
	 * This function also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * This function does not work for unseekable streams.
	 *
	 * \param _povfFile Pointer to our already opened and initialized LSS_OV_FILE structure.
	 * \param _dPos Location to seek to within the file, specified in seconds.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTimeSeekPage( LSS_OV_FILE * _povfFile, double _dPos ) {
		// Translate time to PCM position and call OvPcmSeek.
		int32_t i32Link = -1;
		int64_t i64PcmTotal = 0;
		double dTimeTotal = 0.0;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		if ( !_povfFile->seekable ) { return OV_ENOSEEK; }
		if ( _dPos < 0 ) { return OV_EINVAL; }

		// Which bitstream section does this time offset occur in?
		for ( i32Link = 0; i32Link < _povfFile->links; i32Link++ ) {
			double dAddSec  = OvTimeTotal( _povfFile, i32Link );
			if ( _dPos < dTimeTotal + dAddSec ) { break; }
			dTimeTotal += dAddSec;
			i64PcmTotal += _povfFile->pcmlengths[i32Link*2+1];
		}

		if ( i32Link == _povfFile->links ) { return OV_EINVAL; }

		// Enough information to convert time offset to pcm offset.
		{
			int64_t i64Target = static_cast<int64_t>(i64PcmTotal + (_dPos - dTimeTotal) * _povfFile->vi[i32Link].rate);
			return OcPvmSeekPage( _povfFile, i64Target );
		}
	}

	/**
	 * Seeks to the offset specified (in compressed raw bytes) within the physical bitstream. This variant of OvRawSeek() also automatically crosslaps the transition from the previous playback position into the new playback position in order to eliminate clicking and boundary discontinuities. Otherwise, usage and behavior is identical to OvRawSeek().
	 * When seek speed is a priority, but crosslapping is still desired, this is the best seek funtion to use.
	 * OvRawSeekLap also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * OvRawSeekLap will lap between logical stream links of differing numbers of channels. Any extra channels from the origin of the seek are ignored; playback of these channels simply ends. Extra channels at the destination are lapped from silence. OvRawSeekLap will also lap between logical stream links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 * This function only works for seekable streams.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i64Pos Position in compressed bytes to seek to in the bitstream.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvRawSeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		return Ov64SeekLap( _povfFile, _i64Pos, OvRawSeek );
	}

	/**
	 * Seeks to the offset specified (in pcm samples) within the physical bitstream. This variant of OvPcmSeek also automatically crosslaps the transition from the previous playback position into the new playback position in order to eliminate clicking and boundary discontinuities. Otherwise, usage and behavior is identical to OvPcmSeek.
	 * OvPcmSeekLap() also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * OvPcmSeekLap() will lap between logical stream links of differing numbers of channels. Any extra channels from the origin of the seek are ignored; playback of these channels simply ends. Extra channels at the destination are lapped from silence. OvPcmSeekLap() will also lap between logical stream links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 * This function only works for seekable streams.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i64Pos Position in pcm samples to seek to in the bitstream.
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvPcmSeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		return Ov64SeekLap( _povfFile, _i64Pos, OvPcmSeek );
	}

	/**
	 * Seeks to the closest page preceding the specified location (in pcm samples) within the physical bitstream. This variant of OcPvmSeekPage() also automatically crosslaps the transition from the previous playback position into the new playback position in order to eliminate clicking and boundary discontinuities. Otherwise, usage and behavior is identical to OcPvmSeekPage().
	 * This function is faster than OvPcmSeekLap() because the function can begin decoding at a page boundary rather than seeking through any remaining samples before the specified location. However, it is less accurate.
	 * OvPcmSeekPageLap also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * OvPcmSeekPageLap will lap between logical stream links of differing numbers of channels. Any extra channels from the origin of the seek are ignored; playback of these channels simply ends. Extra channels at the destination are lapped from silence. OvPcmSeekPageLap will also lap between logical stream links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 * This function only works for seekable streams.
	 *
	 * \param _povfFile DESC
	 * \param _i64Pos DESC
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvPcmSeekPageLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos ) {
		return Ov64SeekLap( _povfFile, _i64Pos, OcPvmSeekPage );
	}

	/**
	 * For seekable streams, OvTimeSeekLap() seeks to the given time. This variant of OvTimeSeek() also automatically crosslaps the transition from the previous playback position into the new playback position in order to eliminate clicking and boundary discontinuities. Otherwise, usage and behavior is identical to OvTimeSeek().
	 * OvTimeSeekLap() also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * OvTimeSeekLap() will lap between logical stream links of differing numbers of channels. Any extra channels from the origin of the seek are ignored; playback of these channels simply ends. Extra channels at the destination are lapped from silence. OvTimeSeekLap() will also lap between logical stream links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 * This function does not work for unseekable streams.
	 *
	 * \param _povfFile DESC
	 * \param _dPos DESC
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTimeSeekLap( LSS_OV_FILE * _povfFile, double _dPos ) {
		return OvDSeekLap( _povfFile, _dPos, OvTimeSeek );
	}

	/**
	 * For seekable streams, OvTimeSeekPageLap() seeks to the closest full page preceeding the given time. This variant of OvTimeSeekPage also automatically crosslaps the transition from the previous playback position into the new playback position in order to eliminate clicking and boundary discontinuities. Otherwise, usage and behavior is identical to OvTimeSeekPage.
	 * OvTimeSeekPageLap() is faster than OvTimeSeekLap() because it doesn't seek through the last few samples to reach an exact time, but it is also less accurate. This should be used when speed is important, but crosslapping is still desired.
	 * OvTimeSeekPageLap() also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position.
	 * OvTimeSeekPageLap() will lap between logical stream links of differing numbers of channels. Any extra channels from the origin of the seek are ignored; playback of these channels simply ends. Extra channels at the destination are lapped from silence. OvTimeSeekPageLap() will also lap between logical stream links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 * This function does not work for unseekable streams.
	 *
	 * \param _povfFile DESC
	 * \param _dPos DESC
	 * \return Returns 0 for success.
	 *	OV_EREAD - A read from media returned an error.
	 *	OV_ENOTVORBIS - Bitstream does not contain any Vorbis data.
	 *	OV_EVERSION - Vorbis version mismatch.
	 *	OV_EBADHEADER - Invalid Vorbis bitstream header.
	 *	OV_EFAULT - Internal logic fault; indicates a bug or heap/stack corruption.
	 */
	int32_t CVorbisFile::OvTimeSeekPageLap( LSS_OV_FILE * _povfFile, double _dPos ) {
		return OvDSeekLap( _povfFile, _dPos, OvTimeSeekPage );
	}

	/**
	 * Returns the current offset in raw compressed bytes.
	 * Note that if you later use OvRawSeek() to return to this point, you won't generally get back to exactly the same place, due to internal buffering. Also note that a read operation may not cause a change to the current raw offset - only a read that requires reading more data from the underlying data source will do that.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return n indicates the current offset in bytes.
	 *	OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist.
	 */
	int64_t CVorbisFile::OvRawTell( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		return _povfFile->offset;
	}

	/**
	 * Returns the current offset in samples.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return n indicates the current offset in samples.
	 *	OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist.
	 */
	int64_t CVorbisFile::OvPcmTell( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		return _povfFile->pcm_offset;
	}

	/**
	 * Returns the current decoding offset in seconds.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \return return n indicates the current decoding time offset in seconds.
	 *	OV_EINVAL means that the argument was invalid.  In this case, the requested bitstream did not exist.
	 */
	double CVorbisFile::OvTimeTell( LSS_OV_FILE * _povfFile ) {
		int32_t i32Link = 0;
		int64_t i64PcmTotal = 0;
		double dTimeTotal = 0.0;

		if ( _povfFile->ready_state<OPENED ) { return OV_EINVAL; }
		if ( _povfFile->seekable ) {
			i64PcmTotal = OvPcmTotal( _povfFile, -1 );
			dTimeTotal = OvTimeTotal( _povfFile, -1 );

			// In which bitstream section does this time offset occur in?
			for ( i32Link = _povfFile->links - 1; i32Link >= 0; i32Link-- ) {
				i64PcmTotal -= _povfFile->pcmlengths[i32Link*2+1];
				dTimeTotal -= OvTimeTotal( _povfFile, i32Link );
				if ( _povfFile->pcm_offset >= i64PcmTotal ) { break; }
			}
		}

		return static_cast<double>(dTimeTotal) + static_cast<double>(_povfFile->pcm_offset - i64PcmTotal) / _povfFile->vi[i32Link].rate;
	}

	/**
	 * Returns the vorbis_info struct for the specified bitstream. For nonseekable files, always returns the current vorbis_info struct.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the vorbis_info struct for the current bitstream, this parameter should be set to -1.
	 * \return Returns the vorbis_info struct for the specified bitstream. Returns vorbis_info for current bitstream if the file is nonseekable or _i32I=-1.
	 *	NULL if the specified bitstream does not exist or the file has been initialized improperly.
	 */
	CVorbisCodec::vorbis_info * CVorbisFile::OvInfo( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->seekable ) {
			if ( _i32I < 0 ) {
				if ( _povfFile->ready_state >= STREAMSET ) {
					return _povfFile->vi + _povfFile->current_link;
				}
				else {
					return _povfFile->vi;
				}
			}
			else {
				if ( _i32I >= _povfFile->links ) {
					return NULL;
				}
				else {
					return _povfFile->vi + _i32I;
				}
			}
		}
		else {
			return _povfFile->vi;
		}
	}

	/**
	 * Returns a pointer to the vorbis_comment struct for the specified bitstream. For nonseekable streams, returns the struct for the current bitstream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the vorbis_comment struct for the current bitstream, this parameter should be set to -1.
	 * \return Returns the vorbis_comment struct for the specified bitstream.
	 *	NULL if the specified bitstream does not exist or the file has been initialized improperly.
	 */
	CVorbisCodec::vorbis_comment * CVorbisFile::OvComment( LSS_OV_FILE * _povfFile, int32_t _i32I ) {
		if ( _povfFile->seekable ) {
			if ( _i32I < 0 ) {
				if ( _povfFile->ready_state >= STREAMSET ) {
					return _povfFile->vc + _povfFile->current_link;
				}
				else {
					return _povfFile->vc;
				}
			}
			else {
				if ( _i32I >= _povfFile->links ) {
					return NULL;
				}
				else {
					return _povfFile->vc + _i32I;
				}
			}
		}
		else {
			return _povfFile->vc;
		}
	}

	/**
	 * This is the function used to decode a Vorbis file within a loop, but returns samples in native float format instead of in integer formats.
	 * For information on channel ordering and how OvReadFloat() deals with the complex issues of chaining, etc, refer to the documentation for OvRead().
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible vorbisfile functions.
	 * \param _pppfPcmChannels A pointer to an output buffer. The pointer will be set to the decoded output buffer.
	 * \param _i32Samples Maximum number of decoded _i32Samples to produce.
	 * \param _pi32BitStream A pointer to the number of the current logical bitstream.
	 * \return OV_HOLE: indicates there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page).
	 *	OV_EBADLINK: indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
	 *	OV_EINVAL: indicates the initial file headers couldn't be read or are corrupt, or that the initial open call for _povfFile failed.
	 *	0: indicates EOF.
	 *	n: indicates actual number of samples read. OvReadFloat() will decode at most one vorbis packet per invocation, so the value returned will generally be less than length.
	 */
	int32_t CVorbisFile::OvReadFloat( LSS_OV_FILE * _povfFile, float *** _pppfPcmChannels, int32_t _i32Samples,
		int32_t * _pi32BitStream ) {
		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }

		while ( true ) {
			if ( _povfFile->ready_state == INITSET ) {
				float ** ppfPcm;
				int32_t i32Samples = CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, &ppfPcm );
				if ( i32Samples ) {
					int32_t i32Hs = CVorbisCodec::vorbis_synthesis_halfrate_p( _povfFile->vi );
					if ( _pppfPcmChannels ) { (*_pppfPcmChannels) = ppfPcm; }
					if ( i32Samples > _i32Samples ) { i32Samples = _i32Samples; }
					CVorbisCodec::vorbis_synthesis_read( &_povfFile->vd, i32Samples );
					_povfFile->pcm_offset += i32Samples << i32Hs;
					if ( _pi32BitStream ) { (*_pi32BitStream) = _povfFile->current_link; }
					return i32Samples;

				}
			}

			// Suck in another packet.
			{
				int32_t i32Ret = OvFetchAndProcessPacket( _povfFile, NULL, 1, 1 );
				if ( i32Ret == OV_EOF ) { return 0; }
				if ( i32Ret <= 0 ) { return i32Ret; }
			}
		}
	}

	/**
	 * OvReadFilter() is a variant of OvRead(), the main function used to decode a Vorbis file within a loop. It passes the decoded floating point PCM data to the filter specified in the function arguments before converting the data to integer output samples. All other aspects of its behavior are as with OvRead().
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _pcBuffer A pointer to an output buffer. The decoded output is inserted into this buffer.
	 * \param _i32Length Number of bytes to be read into the _pcBuffer. Should be the same size as the _pcBuffer. A typical value is 4096.
	 * \param _i32BigEndianP Specifies big or little endian byte packing. 0 for little endian, 1 for big endian. Typical value is 0.
	 * \param _i32Word Specifies word size. Possible arguments are 1 for 8-bit samples, or 2 or 16-bit samples. Typical value is 2.
	 * \param _i32Signed Signed or unsigned data. 0 for unsigned, 1 for signed. Typically 1.
	 * \param _pi32BitStream A pointer to the number of the current logical bitstream.
	 * \param _pfFilter Filter function to process float PCM data prior to conversion to interleaved integer output.
	 * \param _pvFilterParam Data to pass through to the filter function.
	 * \return OV_HOLE: indicates there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page).
	 *	OV_EBADLINK: indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
	 *	OV_EINVAL: indicates the initial file headers couldn't be read or are corrupt, or that the initial open call for _povfFile failed.
	 *	0: indicates EOF.
	 *	n: indicates actual number of samples read. OvReadFloat() will decode at most one vorbis packet per invocation, so the value returned will generally be less than _i32Length.
	 */
	int32_t CVorbisFile::OvReadFilter( LSS_OV_FILE * _povfFile, char * _pcBuffer, int32_t _i32Length,
		int32_t _i32BigEndianP, int32_t _i32Word, int32_t _i32Signed, int32_t * _pi32BitStream,
		void (* _pfFilter)( float ** _ppPcm, int32_t _i32Channels, int32_t _i32Samples, void * _pvFilterParam ), void * _pvFilterParam ) {

		int32_t I, J;
		int32_t i32HostEndian = OvHostIsBigEndian();
		int32_t i32Hs;

		float **ppfPcm;
		int32_t i32Samples;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }

		while ( true ) {
			if ( _povfFile->ready_state == INITSET ) {
				i32Samples = CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, &ppfPcm );
				if ( i32Samples ) { break; }
			}

			// Suck in another packet.
			{
				int32_t i32Ret = OvFetchAndProcessPacket( _povfFile, NULL, 1, 1 );
				if ( i32Ret == OV_EOF ) {
					return 0;
				}
				if ( i32Ret <= 0 ) {
					return i32Ret;
				}
			}

		}

		if ( i32Samples > 0 ) {

			// Yay!  Proceed to pack data into the byte buffer.
			int32_t i32Channels = OvInfo( _povfFile, -1 )->channels;
			int32_t i32BytesPerSample = _i32Word * i32Channels;
			CVorbisOs::vorbis_fpu_control vfcFpu;
			if ( i32Samples > _i32Length / i32BytesPerSample ) { i32Samples = _i32Length/i32BytesPerSample; }

			if ( i32Samples <= 0 ) {
				return OV_EINVAL;
			}

			// Here.
			if ( _pfFilter ) {
				_pfFilter( ppfPcm, i32Channels, i32Samples, _pvFilterParam );
			}

			// A tight loop to pack each size.
			{
				int32_t i32Val;
				if ( _i32Word == 1 ) {
					int32_t i32Off = (_i32Signed ? 0 : 128);
					CVorbisOs::vorbis_fpu_setround( &vfcFpu );
					for ( J = 0; J < i32Samples; J++ ) {
						for ( I = 0; I < i32Channels; I++ ) {
							i32Val = CVorbisOs::vorbis_ftoi( ppfPcm[I][J] * 128.0f );
							if ( i32Val > 127 ) { i32Val = 127; }
							else if ( i32Val < -128 ) { i32Val = -128; }
							*_pcBuffer++ = static_cast<char>(i32Val + i32Off);
						}
					}
					CVorbisOs::vorbis_fpu_restore( vfcFpu );
				}
				else {
					int32_t i32Off = (_i32Signed ? 0 : 32768);

					if ( i32HostEndian == _i32BigEndianP ) {
						if ( _i32Signed ) {

							CVorbisOs::vorbis_fpu_setround( &vfcFpu );
							for ( I = 0; I < i32Channels; I++ ) {	// It's faster in this order.
								float *pfSrc = ppfPcm[I];
								int16_t * pi16Dest = reinterpret_cast<int16_t *>(_pcBuffer) + I;
								for ( J = 0; J < i32Samples; J++ ) {
									i32Val = CVorbisOs::vorbis_ftoi( pfSrc[J] * 32768.0f );
									if ( i32Val > 32767 ) { i32Val = 32767; }
									else if ( i32Val < -32768 ) { i32Val = -32768; }
									(*pi16Dest) = static_cast<int16_t>(i32Val);
									pi16Dest += i32Channels;
								}
							}
							CVorbisOs::vorbis_fpu_restore( vfcFpu );

						}
						else {

							CVorbisOs::vorbis_fpu_setround( &vfcFpu );
							for ( I = 0; I < i32Channels; I++ ) {
								float * pfSrc = ppfPcm[I];
								int16_t * pi16Dest = reinterpret_cast<int16_t *>(_pcBuffer) + I;
								for ( J = 0; J < i32Samples; J++ ) {
									i32Val = CVorbisOs::vorbis_ftoi( pfSrc[J] * 32768.0f );
									if ( i32Val > 32767 ) { i32Val = 32767; }
									else if ( i32Val < -32768 ) { i32Val = -32768; }
									(*pi16Dest) = static_cast<int16_t>(i32Val + i32Off);
									pi16Dest += i32Channels;
								}
							}
							CVorbisOs::vorbis_fpu_restore( vfcFpu );

						}
					}
					else if ( _i32BigEndianP ) {
						CVorbisOs::vorbis_fpu_setround( &vfcFpu );
						for ( J = 0; J < i32Samples; J++ ) {
							for ( I = 0; I < i32Channels; I++ ) {
								i32Val = CVorbisOs::vorbis_ftoi( ppfPcm[I][J] * 32768.0f );
								if ( i32Val > 32767 ) { i32Val = 32767; }
								else if ( i32Val < -32768 ) { i32Val = -32768; }
								i32Val += i32Off;
								*_pcBuffer++ = static_cast<char>((i32Val >> 8));
								*_pcBuffer++ = static_cast<char>((i32Val & 0xFF));
							}
						}
						CVorbisOs::vorbis_fpu_restore( vfcFpu );

					}
					else {
						CVorbisOs::vorbis_fpu_setround( &vfcFpu );
						for ( J = 0; J < i32Samples; J++ ) {
							for ( I = 0; I < i32Channels; I++ ) {
								i32Val = CVorbisOs::vorbis_ftoi( ppfPcm[I][J] * 32768.0f );
								if ( i32Val > 32767 ) { i32Val = 32767; }
								else if ( i32Val < -32768 ) { i32Val = -32768; }
								i32Val += i32Off;
								*_pcBuffer++ = static_cast<char>((i32Val & 0xFF));
								*_pcBuffer++ = static_cast<char>((i32Val >> 8));
							}
						}
						CVorbisOs::vorbis_fpu_restore( vfcFpu );

					}
				}
			}

			CVorbisCodec::vorbis_synthesis_read( &_povfFile->vd, i32Samples );
			i32Hs = CVorbisCodec::vorbis_synthesis_halfrate_p( _povfFile->vi );
			_povfFile->pcm_offset += ( i32Samples<<i32Hs );
			if ( _pi32BitStream ) { (*_pi32BitStream) = _povfFile->current_link; }
			return i32Samples * i32BytesPerSample;
		}
		else {
			return i32Samples;
		}
	}

	/**
	 * This is the main function used to decode a Vorbis file within a loop. It returns up to the specified number of bytes of decoded PCM audio in the requested endianness, signedness, and word size. If the audio is multichannel, the channels are interleaved in the output buffer. If the passed in buffer is large, OvRead() will not fill it; the passed in buffer size is treated as a limit and not a request.
	 * The output channels are in stream order and not remapped. Vorbis I defines channel order as follows:
	 *	one channel - the stream is monophonic
	 *	two channels - the stream is stereo. channel order: left, right
	 *	three channels - the stream is a 1d-surround encoding. channel order: left, center, right
	 *	four channels - the stream is quadraphonic surround. channel order: front left, front right, rear left, rear right
	 *	five channels - the stream is five-channel surround. channel order: front left, center, front right, rear left, rear right
	 *	six channels - the stream is 5.1 surround. channel order: front left, center, front right, rear left, rear right, LFE
	 *	seven channels - the stream is 6.1 surround. channel order: front left, center, front right, side left, side right, rear center, LFE
	 *	eight channels - the stream is 7.1 surround. channel order: front left, center, front right, side left, side right, rear left, rear right, LFE
	 *	greater than eight channels - channel use and order is undefined
	 * Note that up to this point, the Vorbisfile API could more or less hide the multiple logical bitstream nature of chaining from the toplevel application if the toplevel application didn't particularly care. However, when reading audio back, the application must be aware that multiple _pi32BitStream sections do not necessarily use the same number of channels or sampling rate.
	 * OvRead() passes back the index of the sequential logical bitstream currently being decoded (in *_pi32BitStream) along with the PCM data in order that the toplevel application can handle channel and/or sample rate changes. This number will be incremented at chaining boundaries even for non-seekable streams. For seekable streams, it represents the actual chaining index within the physical _pi32BitStream.
	 *
	 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
	 * \param _pcBuffer A pointer to an output buffer. The decoded output is inserted into this buffer.
	 * \param _i32Length Number of bytes to be read into the _pcBuffer. Should be the same size as the _pcBuffer. A typical value is 4096.
	 * \param _i32BigEndianP Specifies big or little endian byte packing. 0 for little endian, 1 for big endian. Typical value is 0.
	 * \param _i32Word Specifies word size. Possible arguments are 1 for 8-bit samples, or 2 or 16-bit samples. Typical value is 2.
	 * \param _i32Signed Signed or unsigned data. 0 for unsigned, 1 for signed. Typically 1.
	 * \param _pi32BitStream A pointer to the number of the current logical bitstream.
	 * \return OV_HOLE: indicates there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page).
	 *	OV_EBADLINK: indicates that an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.
	 *	OV_EINVAL: indicates the initial file headers couldn't be read or are corrupt, or that the initial open call for _povfFile failed.
	 *	0: indicates EOF.
	 *	n: indicates actual number of samples read. OvReadFloat() will decode at most one vorbis packet per invocation, so the value returned will generally be less than _i32Length.
	 */
	int32_t CVorbisFile::OvRead( LSS_OV_FILE * _povfFile, char * _pcBuffer, int32_t _i32Length,
		int32_t _i32BigEndianP, int32_t _i32Word, int32_t _i32Signed, int32_t * _pi32BitStream ) {
		return OvReadFilter( _povfFile, _pcBuffer, _i32Length, _i32BigEndianP, _i32Word, _i32Signed, _pi32BitStream, NULL, NULL );
	}

	/**
	 * OvCrosslap overlaps and blends the boundary at a transition between two separate streams represented by separate LSS_OV_FILE structures. For lapping transitions due to seeking within a single stream represented by a single LSS_OV_FILE structure, consider using the lapping versions of the vorbisfile seeking functions instead.
	 * OvCrosslap is used between the last (usually OvRead()) call on the old stream and the first OvRead() from the new stream. Any desired positioning of the new stream must occur before the call to OvCrosslap() as a seek dumps all prior lapping information from a stream's decode state. Crosslapping does not introduce or remove any extraneous samples; positioning works exactly as if OvCrosslap was not called.
	 * OvCrosslap will lap between streams of differing numbers of channels. Any extra channels from the old stream are ignored; playback of these channels simply ends. Extra channels in the new stream are lapped from silence. OvCrosslap will also lap between streams links of differing sample rates. In this case, the sample rates are ignored (no implicit resampling is done to match playback). It is up to the application developer to decide if this behavior makes any sense in a given context; in practical use, these default behaviors perform sensibly.
	 *
	 * \param _povfFile1 A pointer to the LSS_OV_FILE structure representing the origin stream from which to transition playback.
	 * \param _povfFile2 A pointer to the LSS_OV_FILE structure representing the stream with which playback continues.
	 * \return OV_EINVAL: crosslap called with an LSS_OV_FILE structure that isn't open.
	 *	OV_EFAULT: internal error; implies a library bug or external heap corruption.
	 *	OV_EREAD: A read from media returned an error.
	 *	OV_EOF indicates stream _povfFile2 is at end of file, or that _povfFile1 is at end of file immediately after a seek (making crosslap impossible as there's no preceding decode state to crosslap).
	 *	0: success.
	 */
	int32_t CVorbisFile::OvCrosslap( LSS_OV_FILE * _povfFile1, LSS_OV_FILE * _povfFile2 ) {
		CVorbisCodec::vorbis_info * pviInfo1, * pviInfo2;
		float ** pffLapPcm;
		float ** ppfPcm;
		float * pfW1, * pfW2;
		int32_t i32N1, i32N2, I, i32Ret, i32Hs1, i32Hs2;

		if ( _povfFile1 == _povfFile2 ) { return 0; }	// Degenerate case.
		if ( _povfFile1->ready_state < OPENED ) { return OV_EINVAL; }
		if ( _povfFile2->ready_state < OPENED ) { return OV_EINVAL; }

		// The relevant overlap buffers must be pre-checked and pre-primed
		//	before looking at settings in the event that priming would cross
		//	a bitstream boundary.  So, do it now.

		i32Ret = OvInitSet( _povfFile1 );
		if ( i32Ret ) { return i32Ret; }
		i32Ret = OvInitPrime( _povfFile2 );
		if ( i32Ret ) { return i32Ret; }

		pviInfo1 = OvInfo( _povfFile1, -1 );
		pviInfo2 = OvInfo( _povfFile2, -1 );
		i32Hs1 = OvHalfRateP( _povfFile1 );
		i32Hs2 = OvHalfRateP( _povfFile2 );

		pffLapPcm = static_cast<float **>(::alloca( sizeof( *pffLapPcm ) * pviInfo1->channels ));
		i32N1 = CVorbisCodec::vorbis_info_blocksize( pviInfo1, 0 ) >> (1 + i32Hs1);
		i32N2 = CVorbisCodec::vorbis_info_blocksize( pviInfo2, 0 ) >> (1 + i32Hs2);
		pfW1 = CVorbisCodec::vorbis_window( &_povfFile1->vd, 0 );
		pfW2 = CVorbisCodec::vorbis_window( &_povfFile2->vd, 0 );

		for ( I = 0; I < pviInfo1->channels; I++ ) {
			pffLapPcm[I] = static_cast<float *>(::alloca( sizeof( (*(*pffLapPcm)) ) * i32N1 ));
		}

		OvGetLap( _povfFile1, pviInfo1, &_povfFile1->vd, pffLapPcm, i32N1 );

		// Have a lapping buffer from _povfFile1; now to splice it into the lapping buffer of _povfFile2.
		// Consolidate and expose the buffer.
		CVorbisCodec::vorbis_synthesis_lapout( &_povfFile2->vd, &ppfPcm );

		// Splice.
		OvSplice( ppfPcm, pffLapPcm, i32N1, i32N2, pviInfo1->channels, pviInfo2->channels, pfW1, pfW2 );

		// Done.
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _i32Flag Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvHalfRate( LSS_OV_FILE * _povfFile, int32_t _i32Flag ) {
		int32_t I;
		if ( _povfFile->vi == NULL ) { return OV_EINVAL; }
		if ( _povfFile->ready_state > STREAMSET ) {
			// Clear out stream state; dumping the decode machine is needed to reinit the MDCT lookups.
			CVorbisCodec::vorbis_dsp_clear( &_povfFile->vd );
			CVorbisCodec::vorbis_block_clear( &_povfFile->vb );
			_povfFile->ready_state = STREAMSET;
			if ( _povfFile->pcm_offset >= 0 ) {
				int64_t i64Pos = _povfFile->pcm_offset;
				_povfFile->pcm_offset = -1;		// Make sure the position is dumped if unseekable.
				OvPcmSeek( _povfFile, i64Pos );
			}
		}

		for ( I = 0; I < _povfFile->links; I++ ) {
			if ( CVorbisCodec::vorbis_synthesis_halfrate( _povfFile->vi + I, _i32Flag ) ) {
				if ( _i32Flag ) { OvHalfRate( _povfFile, 0 ); }
				return OV_EINVAL;
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvHalfRateP( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile->vi == NULL ) { return OV_EINVAL; }
		return CVorbisCodec::vorbis_synthesis_halfrate_p( _povfFile->vi );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pfFile Undocumented parameter from the Vorbis library.
	 * \param _i64Off Undocumented parameter from the Vorbis library.
	 * \param _i32Whence Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	/*int32_t CVorbisFile::OvHeaderFSeekWrap( FILE * _pfFile, int64_t _i64Off, int32_t _i32Whence ) {
		if ( _pfFile == NULL ) { return -1; }

#ifdef __MINGW32__
		return fseeko64( _pfFile, _i64Off, _i32Whence );
#elif defined( _WIN32 )
		return _fseeki64( _pfFile, _i64Off, _i32Whence );
#else
		return fseek( _pfFile, _i64Off, _i32Whence );
#endif
	}*/

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _pvFile Undocumented parameter from the Vorbis library.
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _pcInitial Undocumented parameter from the Vorbis library.
	 * \param _i32BytesI Undocumented parameter from the Vorbis library.
	 * \param _ocCallbacks Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvOpen1( void * _pvFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, 
		int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks ) {
		int32_t _i32OffsetTest = ((_pvFile && _ocCallbacks.PfSeekFunc) ? _ocCallbacks.PfSeekFunc( _pvFile, 0, LSF_S_CURRENT ) : -1);
		int32_t * pi32SerialNoList = NULL;
		int32_t i32SerialNoListSize = 0;
		int32_t i32Ret;

		CStd::MemSet( _povfFile, 0, sizeof( (*_povfFile) ) );
		_povfFile->datasource = _pvFile;
		_povfFile->callbacks = _ocCallbacks;

		// Init the framing state.
		COgg::ogg_sync_init( &_povfFile->oy );


		if ( _pcInitial ) {
			int8_t * pi8Buffer = COgg::ogg_sync_buffer( &_povfFile->oy, _i32BytesI );
			CStd::MemCpy( pi8Buffer, _pcInitial, _i32BytesI );
			COgg::ogg_sync_wrote( &_povfFile->oy, _i32BytesI );
		}

		// Can we seek?  Stevens suggests the seek test was portable.
		if ( _i32OffsetTest != -1 ) { _povfFile->seekable = 1; }

		// No seeking yet; set up a "single" (current) logical bitstream entry for partial open.
		_povfFile->links = 1;
		_povfFile->vi = static_cast<CVorbisCodec::vorbis_info *>(CMemLib::CMAlloc( _povfFile->links * sizeof( (*_povfFile->vi) ) ));
		_povfFile->vc = static_cast<CVorbisCodec::vorbis_comment *>(CMemLib::CMAlloc( _povfFile->links * sizeof( (*_povfFile->vc) ) ));
		COgg::ogg_stream_init( &_povfFile->os, -1 );	// Fill in the serialno later.

		// Fetch all BOS pages, store the vorbis header and all seen serial numbers, load subsequent vorbis setup headers.
		if ( (i32Ret = OvFetchHeaders( _povfFile, _povfFile->vi, _povfFile->vc, &pi32SerialNoList, &i32SerialNoListSize, NULL )) < 0 ) {
			_povfFile->datasource = NULL;
			OvClear( _povfFile );
		}
		else {
			// Serial number list for first link needs to be held somewhere for second stage of seekable stream open; this saves having to
			//	seek/reread first link's serialnumber data then.
			_povfFile->serialnos = static_cast<int32_t *>(CMemLib::CMAlloc( (i32SerialNoListSize + 2) * sizeof( (*_povfFile->serialnos) ) ));
			_povfFile->serialnos[0] = _povfFile->current_serialno = _povfFile->os.serialno;
			_povfFile->serialnos[1] = i32SerialNoListSize;
			CStd::MemCpy( _povfFile->serialnos + 2, pi32SerialNoList, i32SerialNoListSize * sizeof( (*_povfFile->serialnos) ) );

			_povfFile->offsets = static_cast<int64_t *>(CMemLib::CMAlloc( sizeof( (*_povfFile->offsets) ) ));
			_povfFile->dataoffsets = static_cast<int64_t *>(CMemLib::CMAlloc( sizeof( (*_povfFile->dataoffsets) ) ));
			_povfFile->offsets[0] = 0;
			_povfFile->dataoffsets[0] = _povfFile->offset;

			_povfFile->ready_state = PARTOPEN;
		}
		if ( pi32SerialNoList ) { CMemLib::Free( pi32SerialNoList ); }
		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvOpen2( LSS_OV_FILE *_povfFile ) {
		if ( _povfFile->ready_state != PARTOPEN ) { return OV_EINVAL; }
		_povfFile->ready_state = OPENED;
		if ( _povfFile->seekable ) {
			int32_t i32Ret = OvOpenSeekable2( _povfFile );
			if ( i32Ret ) {
				_povfFile->datasource = NULL;
				OvClear( _povfFile );
			}
			return i32Ret;
		}
		else {
			_povfFile->ready_state = STREAMSET;
		}

		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _pviVi Undocumented parameter from the Vorbis library.
	 * \param _pvcVc Undocumented parameter from the Vorbis library.
	 * \param _ppi32SerialNoList Undocumented parameter from the Vorbis library.
	 * \param _pi32SerialNoN Undocumented parameter from the Vorbis library.
	 * \param _popOgPtr Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvFetchHeaders( LSS_OV_FILE * _povfFile, CVorbisCodec::vorbis_info * _pviVi, CVorbisCodec::vorbis_comment * _pvcVc, 
		int32_t ** _ppi32SerialNoList, int32_t * _pi32SerialNoN, 
		COgg::ogg_page * _popOgPtr ) {
		COgg::ogg_page opOg;
		COgg::ogg_packet opOp;
		int32_t I, i32Ret;
		int32_t i32AllBos = 0;

		if ( !_popOgPtr ) {
			int64_t i64LlRet = OvGetNextPage( _povfFile, &opOg, CHUNKSIZE );
			if ( i64LlRet == OV_EREAD ) { return OV_EREAD; }
			if ( i64LlRet < 0 ) { return OV_ENOTVORBIS; }
			_popOgPtr = &opOg;
		}

		CVorbisCodec::vorbis_info_init( _pviVi );
		CVorbisCodec::vorbis_comment_init( _pvcVc );
		_povfFile->ready_state = OPENED;

		// Extract the serialnos of all BOS pages + the first set of vorbis
		//	headers we see in the link.

		while ( COgg::ogg_page_bos( _popOgPtr ) ) {
			if ( _ppi32SerialNoList ) {
				if ( OvLookUpPageSerialNo( _popOgPtr, (*_ppi32SerialNoList), (*_pi32SerialNoN) ) ) {
					// A dupe serialnumber in an initial header packet set == invalid stream.
					if ( (*_ppi32SerialNoList) ) { CMemLib::Free( *_ppi32SerialNoList ); }
					(*_ppi32SerialNoList) = 0;
					(*_pi32SerialNoN) = 0;
					i32Ret = OV_EBADHEADER;
					goto BailHeader;
				}

				OvAddSerialNo( _popOgPtr, _ppi32SerialNoList, _pi32SerialNoN );
			}

			if ( _povfFile->ready_state < STREAMSET ) {
				// We don't have a vorbis stream in this link yet, so begin prospective stream setup.  We need a stream to get packets.
				COgg::ogg_stream_reset_serialno( &_povfFile->os, COgg::ogg_page_serialno( _popOgPtr ) );
				COgg::ogg_stream_pagein( &_povfFile->os, _popOgPtr );

				if ( COgg::ogg_stream_packetout( &_povfFile->os, &opOp ) > 0 &&
					 CVorbisCodec::vorbis_synthesis_idheader( &opOp ) ) {
					// Vorbis header; continue setup.
					_povfFile->ready_state = STREAMSET;
					if ( (i32Ret = CVorbisCodec::vorbis_synthesis_headerin( _pviVi, _pvcVc, &opOp )) ) {
						i32Ret = OV_EBADHEADER;
						goto BailHeader;
					}
				}
			}

			// Get next page.
			{
				int64_t i64LlRet = OvGetNextPage( _povfFile, _popOgPtr, CHUNKSIZE );
				if ( i64LlRet == OV_EREAD ) {
					i32Ret = OV_EREAD;
					goto BailHeader;
				}
				if ( i64LlRet < 0 ) {
					i32Ret = OV_ENOTVORBIS;
					goto BailHeader;
				}

				// If this page also belongs to our vorbis stream, submit it and break.
				if ( _povfFile->ready_state == STREAMSET &&
					 _povfFile->os.serialno == COgg::ogg_page_serialno( _popOgPtr ) ) {
					COgg::ogg_stream_pagein( &_povfFile->os, _popOgPtr );
					break;
				}
			}
		}

		if ( _povfFile->ready_state != STREAMSET ) {
			i32Ret = OV_ENOTVORBIS;
			goto BailHeader;
		}

		while ( true ) {

			I = 0;
			while ( I < 2 ) { // Get a page loop.

				while ( I < 2 ) { // Get a packet loop.
					int32_t i32Result = COgg::ogg_stream_packetout( &_povfFile->os, &opOp );
					if ( i32Result == 0 ) { break; }
					if ( i32Result == -1 ) {
						i32Ret = OV_EBADHEADER;
						goto BailHeader;
					}

					if ( (i32Ret = CVorbisCodec::vorbis_synthesis_headerin( _pviVi, _pvcVc, &opOp )) ) {
						goto BailHeader;
					}

					I++;
				}

				while ( I < 2 ) {
					if ( OvGetNextPage( _povfFile, _popOgPtr, CHUNKSIZE ) < 0 ) {
						i32Ret = OV_EBADHEADER;
						goto BailHeader;
					}

					// If this page belongs to the correct stream, go parse it.
					if ( _povfFile->os.serialno == COgg::ogg_page_serialno( _popOgPtr ) ) {
						COgg::ogg_stream_pagein( &_povfFile->os, _popOgPtr );
						break;
					}

					// If we never see the final vorbis headers before the link ends, abort.
					if ( COgg::ogg_page_bos( _popOgPtr ) ) {
						if ( i32AllBos ) {
							i32Ret = OV_EBADHEADER;
							goto BailHeader;
						}
						else {
							i32AllBos = 1;
						}
					}

					// Otherwise, keep looking.
				}
			}

			return 0;
		}

	BailHeader:
		CVorbisCodec::vorbis_info_clear( _pviVi );
		CVorbisCodec::vorbis_comment_clear( _pvcVc );
		_povfFile->ready_state = OPENED;

		return i32Ret;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvOpenSeekable2( LSS_OV_FILE *_povfFile ) {
		int64_t i64DataOffset = _povfFile->dataoffsets[0], i64End, i64EndGran = -1;
		int32_t i32EndSerial = _povfFile->os.serialno;
		int32_t i32SerialNo = _povfFile->os.serialno;

		// We're partially open and have a first link header state in storage in _povfFile.

		// Fetch the initial PCM offset.
		int64_t i64PcmOffset = OvInitialPcmOffset( _povfFile, _povfFile->vi );

		// We can seek, so set out learning all about this file.
		if ( _povfFile->callbacks.PfSeekFunc && _povfFile->callbacks.PfTellFunc ) {
			(_povfFile->callbacks.PfSeekFunc)( _povfFile->datasource, 0, LSF_S_END );
			_povfFile->offset = _povfFile->end = (_povfFile->callbacks.PfTellFunc)( _povfFile->datasource );
		}
		else {
			_povfFile->offset = _povfFile->end = -1;
		}

		// If PfSeekFunc is implemented, PfTellFunc must also be implemented.
		if ( _povfFile->end == -1 ) { return OV_EINVAL; }

		// Get the offset of the last page of the physical bitstream, or, if we're lucky the last vorbis page of this link as most OggVorbis
		//	files will contain a single logical bitstream.
		i64End = OvGetPrevPageSerial( _povfFile, _povfFile->serialnos + 2, _povfFile->serialnos[1], &i32EndSerial, &i64EndGran );
		if ( i64End < 0 ) { return static_cast<int32_t>(i64End); }

		// Now determine bitstream structure recursively.
		if ( OvBisectForwardSerialNo( _povfFile, 0, i64DataOffset, _povfFile->offset, i64EndGran, i32EndSerial, 
			_povfFile->serialnos + 2, _povfFile->serialnos[1], 0 ) < 0 ) { return OV_EREAD; }

		_povfFile->offsets[0] = 0;
		_povfFile->serialnos[0] = i32SerialNo;
		_povfFile->dataoffsets[0] = i64DataOffset;
		_povfFile->pcmlengths[0] = i64PcmOffset;
		_povfFile->pcmlengths[1] -= i64PcmOffset;
		if ( _povfFile->pcmlengths[1] < 0 ) { _povfFile->pcmlengths[1] = 0; }

		return OvRawSeek( _povfFile, i64DataOffset );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _popOg Undocumented parameter from the Vorbis library.
	 * \param _i64Boundary Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int64_t CVorbisFile::OvGetNextPage( LSS_OV_FILE *_povfFile, COgg::ogg_page * _popOg, 
		int64_t _i64Boundary ) {
		if ( _i64Boundary > 0 ) { _i64Boundary += _povfFile->offset; }
		while ( true ) {
			int32_t i32More;

			if ( _i64Boundary > 0 && _povfFile->offset >= _i64Boundary ) { return OV_FALSE; }
			i32More = COgg::ogg_sync_pageseek( &_povfFile->oy, _popOg );

			if ( i32More < 0 ) {
				// Skipped n bytes.
				_povfFile->offset -= i32More;
			}
			else {
				if ( i32More == 0 ) {
					// Send more paramedics.
					if ( !_i64Boundary ) { return OV_FALSE; }
					{
						int32_t i32Ret = OvGetData( _povfFile );
						if ( i32Ret == 0 ) { return OV_EOF; }
						if ( i32Ret < 0 ) { return OV_EREAD; }
					}
				}
				else {
					// Got a page.  Return the offset at the page beginning, advance the internal offset past the page end.
					int64_t i64Ret = _povfFile->offset;
					_povfFile->offset += i32More;
					return i64Ret;
				}
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _popOg Undocumented parameter from the Vorbis library.
	 * \param _pi32SerialNoList Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvLookUpPageSerialNo( COgg::ogg_page * _popOg, int32_t * _pi32SerialNoList, int32_t _i32N ) {
		int32_t i32S = COgg::ogg_page_serialno( _popOg );
		return OvLookUpSerialNo( i32S, _pi32SerialNoList, _i32N );
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _popOg Undocumented parameter from the Vorbis library.
	 * \param _ppi32SerialNoList Undocumented parameter from the Vorbis library.
	 * \param _pi32N Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFile::OvAddSerialNo( COgg::ogg_page * _popOg, int32_t ** _ppi32SerialNoList, int32_t * _pi32N ) {
		int32_t i32S = COgg::ogg_page_serialno( _popOg );
		(*_pi32N)++;

		if ( (*_ppi32SerialNoList) ) {
			(*_ppi32SerialNoList) = static_cast<int32_t *>(CMemLib::MReAlloc( (*_ppi32SerialNoList), sizeof( (*(*_ppi32SerialNoList)) ) * (*_pi32N) ));
		}
		else {
			(*_ppi32SerialNoList) = static_cast<int32_t *>(CMemLib::MAlloc( sizeof( (*(*_ppi32SerialNoList)) ) ));
		}

		(*_ppi32SerialNoList)[(*_pi32N)-1] = i32S;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _pviInfo Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int64_t CVorbisFile::OvInitialPcmOffset( LSS_OV_FILE * _povfFile, CVorbisCodec::vorbis_info * _pviInfo ) {
		COgg::ogg_page og;
		int64_t i64Accumulated = 0;
		int32_t i32LastBlock = -1;
		int32_t i32Result;
		int32_t i32SerialNo = _povfFile->os.serialno;

		while ( true ) {
			COgg::ogg_packet op;
			if ( OvGetNextPage( _povfFile, &og, -1 ) < 0 ) {
				break; 
			}

			if ( COgg::ogg_page_bos( &og ) ) { break; }
			if ( COgg::ogg_page_serialno( &og ) != i32SerialNo ) { continue; }

			// Count blocksizes of all frames in the page.
			COgg::ogg_stream_pagein( &_povfFile->os, &og );
			while ( (i32Result = COgg::ogg_stream_packetout( &_povfFile->os, &op )) ) {
				if ( i32Result > 0 ) {	// Ignore holes.
					int32_t i32ThisBlock = CVorbisCodec::vorbis_packet_blocksize( _pviInfo, &op );
					if ( i32LastBlock != -1 ) {
						i64Accumulated += (i32LastBlock + i32ThisBlock) >> 2;
					}
					i32LastBlock = i32ThisBlock;
				}
			}

			if ( COgg::ogg_page_granulepos( &og ) != -1 ) {
				// PCM offset of last packet on the first audio page.
				i64Accumulated = COgg::ogg_page_granulepos( &og ) - i64Accumulated;
				break;
			}
		}

		// Less than zero?  Either a corrupt file or a stream with samples
		//	trimmed off the beginning, a normal occurrence; in both cases set
		//	the offset to zero.
		if ( i64Accumulated < 0 ) { i64Accumulated = 0; }

		return i64Accumulated;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _pi32SerialList Undocumented parameter from the Vorbis library.
	 * \param _i32SerialCount Undocumented parameter from the Vorbis library.
	 * \param _pi32SerialNo Undocumented parameter from the Vorbis library.
	 * \param _pi64GranPos Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int64_t CVorbisFile::OvGetPrevPageSerial( LSS_OV_FILE * _povfFile, 
		int32_t * _pi32SerialList, int32_t _i32SerialCount, 
		int32_t * _pi32SerialNo, int64_t * _pi64GranPos ) {
		COgg::ogg_page opOg;
		int64_t i64Begin = _povfFile->offset;
		int64_t i64End = i64Begin;
		int64_t i64Ret;

		int64_t i64PrefOffset = -1;
		int64_t i64Offset = -1;
		int64_t i64RetSerialNo = -1;
		int64_t i64RetGran = -1;

		while ( i64Offset == -1 ) {
			i64Begin -= CHUNKSIZE;
			if ( i64Begin < 0 ) {
				i64Begin = 0;
			}

			i64Ret = OvSeekHelper( _povfFile, i64Begin );
			if ( i64Ret ) { return i64Ret; }

			while ( _povfFile->offset < i64End ) {
				i64Ret = OvGetNextPage( _povfFile, &opOg, i64End-_povfFile->offset );
				if ( i64Ret == OV_EREAD ) { return OV_EREAD; }
				if ( i64Ret < 0 ) {
					break;
				}
				else {
					i64RetSerialNo = COgg::ogg_page_serialno( &opOg );
					i64RetGran = COgg::ogg_page_granulepos( &opOg );
					i64Offset = i64Ret;

					if ( i64RetSerialNo == (*_pi32SerialNo) ) {
						i64PrefOffset = i64Ret;
						(*_pi64GranPos) = i64RetGran;
					}

					if ( !OvLookUpSerialNo( static_cast<int32_t>(i64RetSerialNo), _pi32SerialList, _i32SerialCount ) ) {
						// We fell off the end of the link, which means we seeked
						//	back too far and shouldn't have been looking in that link
						//	to begin with.  If we found the preferred serial number, 
						//	forget that we saw it.
						i64PrefOffset = -1;
					}
				}
			}
		}

		// We're not interested in the page, just the _pi32SerialNo and _pi64GranPos.
		if ( i64PrefOffset >= 0 ) { return i64PrefOffset; }

		(*_pi32SerialNo) = static_cast<int32_t>(i64RetSerialNo);
		(*_pi64GranPos) = i64RetGran;
		return i64Offset;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _i64Begin Undocumented parameter from the Vorbis library.
	 * \param _i64Searched Undocumented parameter from the Vorbis library.
	 * \param _i64End Undocumented parameter from the Vorbis library.
	 * \param _i64EndGran Undocumented parameter from the Vorbis library.
	 * \param _i32EndSerial Undocumented parameter from the Vorbis library.
	 * \param _pi32CurrentNoList Undocumented parameter from the Vorbis library.
	 * \param _i32CurrentNoS Undocumented parameter from the Vorbis library.
	 * \param _i32M Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvBisectForwardSerialNo( LSS_OV_FILE * _povfFile, 
		int64_t _i64Begin, 
		int64_t _i64Searched, 
		int64_t _i64End, 
		int64_t _i64EndGran, 
		int32_t _i32EndSerial, 
		int32_t * _pi32CurrentNoList, 
		int32_t _i32CurrentNoS, 
		int32_t _i32M ) {
		int64_t i64PcmOffset;
		int64_t i64DataOffset = _i64Searched;
		int64_t i64EndSearched = _i64End;
		int64_t i64Next = _i64End;
		int64_t i64SearchGran = -1;
		COgg::ogg_page opOg;
		int64_t i64Ret, i64Last;
		int32_t i32SerialNo = _povfFile->os.serialno;

		// Invariants:
		//	We have the headers and serialnos for the link beginning at "_i64Begin"
		//	We have the offset and granpos of the last page in the file (potentially not a page we care about).

		// Is the last page in our list of current serialnumbers?
		if ( OvLookUpSerialNo( _i32EndSerial, _pi32CurrentNoList, _i32CurrentNoS ) ) {

			// Last page is in the starting i32SerialNo list, so we've bisected
			//	down to (or just started with) a single link.  Now we need to
			//	find the i64Last vorbis page belonging to the first vorbis stream
			//	for this link.
			while ( _i32EndSerial != i32SerialNo ) {
				_i32EndSerial = i32SerialNo;
				_povfFile->offset = OvGetPrevPageSerial( _povfFile, _pi32CurrentNoList, _i32CurrentNoS, &_i32EndSerial, &_i64EndGran );
			}

			_povfFile->links = _i32M + 1;
			if ( _povfFile->offsets ) { CMemLib::Free( _povfFile->offsets ); }
			if ( _povfFile->serialnos ) { CMemLib::Free( _povfFile->serialnos ); }
			if ( _povfFile->dataoffsets ) { CMemLib::Free( _povfFile->dataoffsets ); }

			_povfFile->offsets = static_cast<int64_t *>(CMemLib::MAlloc( ( _povfFile->links + 1 ) * sizeof( (*_povfFile->offsets) ) ));
			_povfFile->vi = static_cast<CVorbisCodec::vorbis_info *>(CMemLib::MReAlloc( _povfFile->vi, _povfFile->links * sizeof( (*_povfFile->vi) ) ));
			_povfFile->vc = static_cast<CVorbisCodec::vorbis_comment *>(CMemLib::MReAlloc( _povfFile->vc, _povfFile->links * sizeof( (*_povfFile->vc) ) ));
			_povfFile->serialnos = static_cast<int32_t *>(CMemLib::MAlloc( _povfFile->links * sizeof( (*_povfFile->serialnos) ) ));
			_povfFile->dataoffsets = static_cast<int64_t *>(CMemLib::MAlloc( _povfFile->links * sizeof( (*_povfFile->dataoffsets) ) ));
			_povfFile->pcmlengths = static_cast<int64_t *>(CMemLib::MAlloc( _povfFile->links * 2 * sizeof( (*_povfFile->pcmlengths) ) ));

			_povfFile->offsets[_i32M+1] = _i64End;
			_povfFile->offsets[_i32M] = _i64Begin;
			_povfFile->pcmlengths[_i32M*2+1] = (_i64EndGran < 0 ? 0 : _i64EndGran);

		}
		else {
			int32_t * pi32NextSerialNoList = NULL;
			int32_t i32NextSerialNoS = 0;
			CVorbisCodec::vorbis_info viInfo;
			CVorbisCodec::vorbis_comment vcComment;

			// The below guards against garbage seperating the i64Last and first pages of two links.
			while ( _i64Searched < i64EndSearched ) {
				int64_t bisect;

				if ( i64EndSearched - _i64Searched < CHUNKSIZE ) {
					bisect = _i64Searched;
				}
				else {
					bisect = (_i64Searched + i64EndSearched) / 2;
				}

				if ( bisect != _povfFile->offset ) {
					i64Ret = OvSeekHelper( _povfFile, bisect );
					if ( i64Ret ) { return static_cast<int32_t>(i64Ret); }
				}

				i64Last = OvGetNextPage( _povfFile, &opOg, -1 );
				if ( i64Last == OV_EREAD ) { return OV_EREAD; }
				if ( i64Last < 0 || !OvLookUpPageSerialNo( &opOg, _pi32CurrentNoList, _i32CurrentNoS ) ) {
					i64EndSearched = bisect;
					if ( i64Last >= 0 ) { i64Next = i64Last; }
				}
				else {
					_i64Searched = _povfFile->offset;
				}
			}

			// Bisection point found.

			// For the time being, fetch end PCM offset the simple way.
			{
				int32_t i32TestSerial = i32SerialNo + 1;
				_povfFile->offset = i64Next;
				while ( i32TestSerial != i32SerialNo ) {
					i32TestSerial = i32SerialNo;
					_povfFile->offset = OvGetPrevPageSerial( _povfFile, _pi32CurrentNoList, _i32CurrentNoS, &i32TestSerial, &i64SearchGran );
				}
			}

			if ( _povfFile->offset != i64Next ) {
				i64Ret = OvSeekHelper( _povfFile, i64Next );
				if ( i64Ret ) { return static_cast<int32_t>(i64Ret); }
			}

			i64Ret = OvFetchHeaders( _povfFile, &viInfo, &vcComment, &pi32NextSerialNoList, &i32NextSerialNoS, NULL );
			if ( i64Ret ) { return static_cast<int32_t>(i64Ret); }
			i32SerialNo = _povfFile->os.serialno;
			i64DataOffset = _povfFile->offset;

			// This will consume a page, however the next bistection always starts with a raw seek.
			i64PcmOffset = OvInitialPcmOffset( _povfFile, &viInfo );

			i64Ret = OvBisectForwardSerialNo( _povfFile, i64Next, _povfFile->offset, _i64End, _i64EndGran, _i32EndSerial,
				pi32NextSerialNoList, i32NextSerialNoS, _i32M + 1 );
			if ( i64Ret ) { return static_cast<int32_t>(i64Ret); }

			if ( pi32NextSerialNoList ) { CMemLib::Free( pi32NextSerialNoList ); }

			_povfFile->offsets[_i32M+1] = i64Next;
			_povfFile->serialnos[_i32M+1] = i32SerialNo;
			_povfFile->dataoffsets[_i32M+1] = i64DataOffset;

			_povfFile->vi[_i32M+1] = viInfo;
			_povfFile->vc[_i32M+1] = vcComment;

			_povfFile->pcmlengths[_i32M*2+1] = i64SearchGran;
			_povfFile->pcmlengths[_i32M*2+2] = i64PcmOffset;
			_povfFile->pcmlengths[_i32M*2+3] -= i64PcmOffset;
			if ( _povfFile->pcmlengths[_i32M*2+3] < 0 ) { _povfFile->pcmlengths[_i32M*2+3] = 0; }
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvGetData( LSS_OV_FILE * _povfFile ) {
		errno = 0;
		if ( !_povfFile->callbacks.PfReadFunc ) { return -1; }
		if ( _povfFile->datasource ) {
			int8_t * pi8Buffer = COgg::ogg_sync_buffer( &_povfFile->oy, READSIZE );
			int32_t i32Bytes = static_cast<int32_t>(_povfFile->callbacks.PfReadFunc( pi8Buffer, 1, READSIZE, _povfFile->datasource ));
			if ( i32Bytes > 0 ) { COgg::ogg_sync_wrote( &_povfFile->oy, i32Bytes ); }
			if ( i32Bytes == 0 && errno ) { return -1; }
			return i32Bytes;
		}
		else {
			return 0;
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _i32S Undocumented parameter from the Vorbis library.
	 * \param _pi32SerialNoList Undocumented parameter from the Vorbis library.
	 * \param _i32N Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvLookUpSerialNo( int32_t _i32S, int32_t * _pi32SerialNoList, int32_t _i32N ) {
		if ( _pi32SerialNoList ) {
			while ( _i32N-- ) {
				if ( (*_pi32SerialNoList) == _i32S ) { return 1; }
				_pi32SerialNoList++;
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _i64Offset Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvSeekHelper( LSS_OV_FILE *_povfFile, int64_t _i64Offset ) {
		if ( _povfFile->datasource ) {
			if ( !_povfFile->callbacks.PfSeekFunc ||
				_povfFile->callbacks.PfSeekFunc( _povfFile->datasource, _i64Offset, LSF_S_BEGIN ) == -1 ) {
				return OV_EREAD;
			}
			_povfFile->offset = _i64Offset;
			COgg::ogg_sync_reset( &_povfFile->oy );
		}
		else {
			// Shouldn't happen unless someone writes a broken callback.
			return OV_EFAULT;
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFile::OvDecodeClear( LSS_OV_FILE * _povfFile ) {
		CVorbisCodec::vorbis_dsp_clear( &_povfFile->vd );
		CVorbisCodec::vorbis_block_clear( &_povfFile->vb );
		_povfFile->ready_state = OPENED;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvMakeDecodeReady( LSS_OV_FILE * _povfFile ) {
		if ( _povfFile->ready_state > STREAMSET ) { return 0; }
		if ( _povfFile->ready_state < STREAMSET ) { return OV_EFAULT; }
		if ( _povfFile->seekable ) {
			if ( CVorbisCodec::vorbis_synthesis_init( &_povfFile->vd, _povfFile->vi + _povfFile->current_link ) )
				return OV_EBADLINK;
		}
		else {
			if ( CVorbisCodec::vorbis_synthesis_init( &_povfFile->vd, _povfFile->vi ) ) {
				return OV_EBADLINK;
			}
		}
		CVorbisCodec::vorbis_block_init( &_povfFile->vd, &_povfFile->vb );
		_povfFile->ready_state = INITSET;
		_povfFile->bittrack = 0.0;
		_povfFile->samptrack = 0.0;
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _popOpIn Undocumented parameter from the Vorbis library.
	 * \param _i32ReadP Undocumented parameter from the Vorbis library.
	 * \param _i32SpanP Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvFetchAndProcessPacket( LSS_OV_FILE *_povfFile, COgg::ogg_packet * _popOpIn, int32_t _i32ReadP, int32_t _i32SpanP ) {
		COgg::ogg_page opPage;

		// Handle one packet.  Try to fetch it from current stream state.  Extract packets from page.
		while ( true ) {

			if ( _povfFile->ready_state == STREAMSET ) {
				int32_t i32Ret = OvMakeDecodeReady( _povfFile );
				if ( i32Ret < 0 ) { return i32Ret; }
			}

			// Process a packet if we can.
			if ( _povfFile->ready_state == INITSET ) {
				int32_t i32Hs = CVorbisCodec::vorbis_synthesis_halfrate_p( _povfFile->vi );

				while ( true ) {
					COgg::ogg_packet opPacket;
					COgg::ogg_packet * popPacketPtr = (_popOpIn ? _popOpIn : &opPacket);
					int32_t i32Result = COgg::ogg_stream_packetout( &_povfFile->os, popPacketPtr );
					int64_t i64GranulePos;

					_popOpIn = NULL;
					if ( i32Result == -1 ) { return OV_HOLE; }	// Hole in the data.
					if ( i32Result > 0 ) {
						// Got a packet.  Process it.
						i64GranulePos = popPacketPtr->granulepos;
						if ( !CVorbisCodec::vorbis_synthesis( &_povfFile->vb, popPacketPtr ) ) {
							// Suck in the synthesis data and track bitrate.
							{
								int32_t i32OldSamples = CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, NULL );
								// For proper use of libvorbis within libvorbisfile, i32OldSamples will always be zero.
								if ( i32OldSamples ) { return OV_EFAULT; }

								CVorbisCodec::vorbis_synthesis_blockin( &_povfFile->vd, &_povfFile->vb );
								_povfFile->samptrack += (CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, NULL ) << i32Hs);
								_povfFile->bittrack += popPacketPtr->bytes * 8;
							}

							// Update the pcm offset.
							if ( i64GranulePos != -1 && !popPacketPtr->e_o_s ) {
								int32_t i32Link = (_povfFile->seekable ? _povfFile->current_link : 0);
								int32_t I, i32Samples;

								// This packet has a pcm_offset on it (the last packet completed on a page carries the offset).  After processing
								//	(above), we know the pcm position of the last sample ready to be returned.  Find the offset of the first.
								//
								// As an aside, this trick is inaccurate if we begin reading anew right at the last page; the end-of-stream
								//	i64GranulePos declares the last frame in the stream, and the last packet of the last page may be a partial frame.
								//	So, we need a previous i64GranulePos from an in-sequence page to have a reference point.  Thus the !popPacketPtr->e_o_s clause
								//	above.

								if ( _povfFile->seekable && i32Link > 0 ) {
									i64GranulePos -= _povfFile->pcmlengths[i32Link*2];
								}
								if ( i64GranulePos < 0 ) { i64GranulePos = 0; }	// Actually, this shouldn't be possible here unless the stream is very broken.

								i32Samples = (CVorbisCodec::vorbis_synthesis_pcmout( &_povfFile->vd, NULL ) << i32Hs);

								i64GranulePos -= i32Samples;
								for ( I = 0; I < i32Link; I++ ) {
									i64GranulePos += _povfFile->pcmlengths[I*2+1];
								}
								_povfFile->pcm_offset = i64GranulePos;
							}
							return 1;
						}
					}
					else
						break;
				}
			}

			if ( _povfFile->ready_state >= OPENED ) {
				int64_t i64Ret;

				while ( true ) {
					// The loop is not strictly necessary, but there's no sense in doing the extra checks of the larger loop for the common
					//	case in a multiplexed bistream where the page is simply part of a different logical bitstream; keep reading until
					//	we get one with the correct serial number.

					if ( !_i32ReadP ) { return 0; }
					if ( (i64Ret = OvGetNextPage( _povfFile, &opPage, -1 )) < 0 ) {
						return OV_EOF; // Eof.  Leave unitialized.
					}

					// Bitrate tracking; add the header's bytes here, the body bytes are done by packet above.
					_povfFile->bittrack += opPage.header_len *8;

					if ( _povfFile->ready_state == INITSET ) {
						if ( _povfFile->current_serialno != COgg::ogg_page_serialno( &opPage ) ) {

							// Two possibilities:
							//	1: our decoding just traversed a bitstream boundary.
							//	2: another stream is multiplexed into this logical section.
							if ( COgg::ogg_page_bos( &opPage ) ) {
								// Boundary case.
								if ( !_i32SpanP ) {
									return OV_EOF;
								}

								OvDecodeClear( _povfFile );

								if ( !_povfFile->seekable ) {
									CVorbisCodec::vorbis_info_clear( _povfFile->vi );
									CVorbisCodec::vorbis_comment_clear( _povfFile->vc );
								}
								break;

							}
							else {
								continue; // Possibility #2.
							}
						}
					}

					break;
				}
			}

			// Do we need to load a new machine before submitting the page?
			// This is different in the seekable and non-seekable cases.
			//
			// In the seekable case, we already have all the header
			//	information loaded and cached; we just initialize the machine
			//	with it and continue on our merry way.
			//
			// In the non-seekable (streaming) case, we'll only be at a
			//	boundary if we just left the previous logical bitstream and
			//	we're now nominally at the header of the next bitstream.

			if ( _povfFile->ready_state != INITSET ) {
				int32_t i32Link;

				if ( _povfFile->ready_state<STREAMSET ) {
					if ( _povfFile->seekable ) {
						int32_t i32SerialNo = COgg::ogg_page_serialno( &opPage );

						// Match the i32SerialNo to bitstream section.  We use this rather than offset positions to avoid problems near logical bitstream boundaries.

						for ( i32Link = 0; i32Link < _povfFile->links; i32Link++ ) {
							if ( _povfFile->serialnos[i32Link] == i32SerialNo ) { break; }
						}

						if ( i32Link == _povfFile->links ) { continue; }	// Not the desired Vorbis bitstream section; keep trying.

						_povfFile->current_serialno = i32SerialNo;
						_povfFile->current_link = i32Link;

						COgg::ogg_stream_reset_serialno( &_povfFile->os, _povfFile->current_serialno );
						_povfFile->ready_state = STREAMSET;

					}
					else {
						// We're streaming.
						// Fetch the three header packets, build the info struct.

						int32_t i32Ret = OvFetchHeaders( _povfFile, _povfFile->vi, _povfFile->vc, NULL, NULL, &opPage );
						if ( i32Ret ) { return i32Ret; }
						_povfFile->current_serialno = _povfFile->os.serialno;
						_povfFile->current_link++;
						i32Link = 0;
					}
				}
			}

			// The buffered page is the data we want, and we're ready for it; add it to the stream state.
			COgg::ogg_stream_pagein( &_povfFile->os, &opPage );

		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _popPage Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int64_t CVorbisFile::OvGetPrevPage( LSS_OV_FILE * _povfFile, COgg::ogg_page * _popPage ) {
		int64_t i64Begin = _povfFile->offset;
		int64_t i64End = i64Begin;
		int64_t i64Ret;
		int64_t i64Offset = -1;

		while ( i64Offset == -1 ) {
			i64Begin -= CHUNKSIZE;
			if ( i64Begin < 0 ) {
				i64Begin = 0;
			}

			i64Ret = OvSeekHelper( _povfFile, i64Begin );
			if ( i64Ret ) { return i64Ret; }

			while ( _povfFile->offset < i64End ) {
				CStd::MemSet( _popPage, 0, sizeof( (*_popPage) ) );
				i64Ret = OvGetNextPage( _povfFile, _popPage, i64End - _povfFile->offset );
				if ( i64Ret == OV_EREAD ) { return OV_EREAD; }
				if ( i64Ret < 0 ) {
					break;
				}
				else {
					i64Offset = i64Ret;
				}
			}
		}

		// In a fully compliant, non-multiplexed stream, we'll still be holding the last page.  In multiplexed (or noncompliant streams), 
		//	we will probably have to re-read the last page we saw.
		if ( _popPage->header_len == 0 ) {
			i64Ret = OvSeekHelper( _povfFile, i64Offset );
			if ( i64Ret ) { return i64Ret; }

			i64Ret = OvGetNextPage( _povfFile, _popPage, CHUNKSIZE );
			if ( i64Ret < 0 ) {
				return OV_EFAULT;
			}
		}

		return i64Offset;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _i64Pos Undocumented parameter from the Vorbis library.
	 * \param _pfLocalSeek Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::Ov64SeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos, int32_t (* _pfLocalSeek)( LSS_OV_FILE *, int64_t ) ) {
		CVorbisCodec::vorbis_info * pviInfo;
		float ** ppfLapPcm;
		float ** ppfPcm;
		float * pfW1, * pfW2;
		int32_t i32N1, i32N2, i32Ch1, i32Ch2, i32Hs;
		int32_t I, i32Ret;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		i32Ret = OvInitSet( _povfFile );
		if ( i32Ret ) { return i32Ret; }
		pviInfo = OvInfo( _povfFile, -1 );
		i32Hs = OvHalfRateP( _povfFile );

		i32Ch1 = pviInfo->channels;
		i32N1 = CVorbisCodec::vorbis_info_blocksize( pviInfo, 0 ) >> (1 + i32Hs);
		pfW1 = CVorbisCodec::vorbis_window( &_povfFile->vd, 0 );

		ppfLapPcm = static_cast<float **>(::alloca( sizeof( (*ppfLapPcm) ) * i32Ch1 ));
		for ( I = 0; I < i32Ch1; I++ ) {
			ppfLapPcm[I] = static_cast<float *>(::alloca( sizeof( (*(*ppfLapPcm)) ) * i32N1 ));
		}
		OvGetLap( _povfFile, pviInfo, &_povfFile->vd, ppfLapPcm, i32N1 );

		// Have lapping data; seek and prime the buffer.
		i32Ret = _pfLocalSeek( _povfFile, _i64Pos );
		if ( i32Ret ) { return i32Ret; }
		i32Ret = OvInitPrime( _povfFile );
		if ( i32Ret ) { return i32Ret; }

		// Guard against cross-link changes; they're perfectly legal.
		pviInfo = OvInfo( _povfFile, -1 );
		i32Ch2 = pviInfo->channels;
		i32N2 = CVorbisCodec::vorbis_info_blocksize( pviInfo, 0 ) >> (1 + i32Hs);
		pfW2 = CVorbisCodec::vorbis_window( &_povfFile->vd, 0 );

		// Consolidate and expose the buffer.
		CVorbisCodec::vorbis_synthesis_lapout( &_povfFile->vd, &ppfPcm );

		// Splice.
		OvSplice( ppfPcm, ppfLapPcm, i32N1, i32N2, i32Ch1, i32Ch2, pfW1, pfW2 );

		// Done.
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvInitSet( LSS_OV_FILE * _povfFile ) {
		while ( true ) {
			if ( _povfFile->ready_state == INITSET ) { break; }
			// Suck in another packet.
			{
				int32_t i32Ret = OvFetchAndProcessPacket( _povfFile, NULL, 1, 0 );
				if ( i32Ret < 0 && i32Ret != OV_HOLE ) { return i32Ret; }
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _pviInfo Undocumented parameter from the Vorbis library.
	 * \param _pvdsVd Undocumented parameter from the Vorbis library.
	 * \param _ppfLapPcm Undocumented parameter from the Vorbis library.
	 * \param _i32LapSize Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFile::OvGetLap( LSS_OV_FILE *_povfFile, CVorbisCodec::vorbis_info * _pviInfo, CVorbisCodec::vorbis_dsp_state * _pvdsVd, float ** _ppfLapPcm, int32_t _i32LapSize ) {
		int32_t i32LapCount = 0, I;
		float ** ppfPcm;

		// Try first to decode the lapping data.
		while ( i32LapCount < _i32LapSize ) {
			int32_t i32Samples = CVorbisCodec::vorbis_synthesis_pcmout( _pvdsVd, &ppfPcm );
			if ( i32Samples ) {
				if ( i32Samples > _i32LapSize - i32LapCount ) { i32Samples = _i32LapSize - i32LapCount; }
				for ( I = 0; I < _pviInfo->channels; I++ ) {
					CStd::MemCpy( _ppfLapPcm[I] + i32LapCount, ppfPcm[I], sizeof( (*(*ppfPcm)) ) * i32Samples );
				}
				i32LapCount += i32Samples;
				CVorbisCodec::vorbis_synthesis_read( _pvdsVd, i32Samples );
			}
			else {
				// Suck in another packet.
				int32_t i32Ret = OvFetchAndProcessPacket( _povfFile, NULL, 1, 0 );
				if ( i32Ret == OV_EOF ) { break; }
			}
		}
		if ( i32LapCount < _i32LapSize ) {
			// Failed to get lapping data from normal decode; pry it from the postextrapolation buffering, or the second half of the MDCT from the last packet.
			int32_t i32Samples = CVorbisCodec::vorbis_synthesis_lapout( &_povfFile->vd, &ppfPcm );
			if ( i32Samples == 0 ) {
				for ( I = 0; I < _pviInfo->channels; I++ ) {
					CStd::MemSet( _ppfLapPcm[I] + i32LapCount, 0, sizeof( (*(*ppfPcm)) ) * _i32LapSize - i32LapCount );
				}
				i32LapCount = _i32LapSize;
			}
			else {
				if ( i32Samples > _i32LapSize - i32LapCount ) { i32Samples = _i32LapSize - i32LapCount; }
				for ( I = 0; I < _pviInfo->channels; I++ ) {
					CStd::MemCpy( _ppfLapPcm[I] + i32LapCount, ppfPcm[I], sizeof( (*(*ppfPcm)) ) * i32Samples );
				}
				i32LapCount += i32Samples;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvInitPrime( LSS_OV_FILE * _povfFile ) {
		CVorbisCodec::vorbis_dsp_state * pvdsState = &_povfFile->vd;
		while ( true ) {
			if ( _povfFile->ready_state == INITSET ) {
				if ( CVorbisCodec::vorbis_synthesis_pcmout( pvdsState, NULL ) ) { break; }
			}
			// Suck in another packet.
			{
				int32_t i32Ret = OvFetchAndProcessPacket( _povfFile, NULL, 1, 0 );
				if ( i32Ret < 0 && i32Ret != OV_HOLE ) { return i32Ret; }
			}
		}
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _ppfPcm Undocumented parameter from the Vorbis library.
	 * \param _ppfLapPcm Undocumented parameter from the Vorbis library.
	 * \param _i32N1 Undocumented parameter from the Vorbis library.
	 * \param _i32N2 Undocumented parameter from the Vorbis library.
	 * \param _i32Ch1 Undocumented parameter from the Vorbis library.
	 * \param _i32Ch2 Undocumented parameter from the Vorbis library.
	 * \param _pfW1 Undocumented parameter from the Vorbis library.
	 * \param _pfW2 Undocumented parameter from the Vorbis library.
	 */
	void CVorbisFile::OvSplice( float ** _ppfPcm, float ** _ppfLapPcm, 
		int32_t _i32N1, int32_t _i32N2, 
		int32_t _i32Ch1, int32_t _i32Ch2, 
		float * _pfW1, float * _pfW2 ) {
		int32_t I, J;
		float * pfW = _pfW1;
		int32_t i32N = _i32N1;

		if ( _i32N1 > _i32N2 ) {
			i32N = _i32N2;
			pfW = _pfW2;
		}

		// Splice.
		for ( J = 0; J < _i32Ch1 && J < _i32Ch2; J++ ) {
			float * pfS = _ppfLapPcm[J];
			float * pfD = _ppfPcm[J];

			for ( I = 0; I < i32N; I++ ) {
				float fWd = pfW[I] * pfW[I];
				float fWs = 1.0f - fWd;
				pfD[I] = pfD[I] * fWd + pfS[I] * fWs;
			}
		}
		// Window from zero.
		for ( ; J < _i32Ch2; J++ ) {
			float * pfD = _ppfPcm[J];
			for ( I = 0; I < i32N; I++ ) {
				float fWd = pfW[I] * pfW[I];
				pfD[I] = pfD[I] * fWd;
			}
		}
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \param _povfFile Undocumented parameter from the Vorbis library.
	 * \param _dPos Undocumented parameter from the Vorbis library.
	 * \param _pfLocalSeek Undocumented parameter from the Vorbis library.
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvDSeekLap( LSS_OV_FILE *_povfFile, double _dPos, int32_t (* _pfLocalSeek)( LSS_OV_FILE *, double ) ) {
		CVorbisCodec::vorbis_info * pviInfo;
		float ** ppfLapPcm;
		float ** ppfPcm;
		float * pfW1, * pfW2;
		int32_t i32N1, i32N2, i32Ch1, i32Ch2, i32Hs;
		int32_t I, i32Ret;

		if ( _povfFile->ready_state < OPENED ) { return OV_EINVAL; }
		i32Ret = OvInitSet( _povfFile );
		if ( i32Ret ) { return i32Ret; }
		pviInfo = OvInfo( _povfFile, -1 );
		i32Hs = OvHalfRateP( _povfFile );

		i32Ch1 = pviInfo->channels;
		i32N1 = CVorbisCodec::vorbis_info_blocksize( pviInfo, 0 ) >> (1 + i32Hs);
		pfW1 = CVorbisCodec::vorbis_window( &_povfFile->vd, 0 );

		ppfLapPcm = static_cast<float **>(::alloca( sizeof( (*ppfLapPcm) ) * i32Ch1 ));
		for ( I = 0; I < i32Ch1; I++ ) {
			ppfLapPcm[I] = static_cast<float *>(::alloca( sizeof( (*(*ppfLapPcm)) ) * i32N1 ));
		}
		OvGetLap( _povfFile, pviInfo, &_povfFile->vd, ppfLapPcm, i32N1 );

		// Have lapping data; seek and prime the buffer.
		i32Ret = _pfLocalSeek( _povfFile, _dPos );
		if ( i32Ret ) { return i32Ret; }
		i32Ret = OvInitPrime( _povfFile );
		if ( i32Ret ) { return i32Ret; }

		// Guard against cross-link changes; they're perfectly legal.
		pviInfo = OvInfo( _povfFile, -1 );
		i32Ch2 = pviInfo->channels;
		i32N2 = CVorbisCodec::vorbis_info_blocksize( pviInfo, 0 ) >> (1 + i32Hs);
		pfW2 = CVorbisCodec::vorbis_window( &_povfFile->vd, 0 );

		// Consolidate and expose the buffer.
		CVorbisCodec::vorbis_synthesis_lapout( &_povfFile->vd, &ppfPcm );

		// Splice.
		OvSplice( ppfPcm, ppfLapPcm, i32N1, i32N2, i32Ch1, i32Ch2, pfW1, pfW2 );

		// Done.
		return 0;
	}

	/**
	 * Undocumented function from the Vorbis library.
	 *
	 * \return Undocumented return from the Vorbis library.
	 */
	int32_t CVorbisFile::OvHostIsBigEndian() {
		uint32_t i32Pattern = 0xFEEDFACE;
		uint8_t * pui8ByteWise = reinterpret_cast<uint8_t *>(&i32Pattern);
		if ( pui8ByteWise[0] == 0xFE ) { return 1; }
		return 0;
	}

	/**
	 * Our custom read function.
	 *
	 * \param _pvPtr Location to store the read data.
	 * \param _uiptrSize Size of the elements to read.
	 * \param _uiptrNMemb Number of elements to read.
	 * \param _pvDataSource File handle.
	 * \return Returns the number of bytes read.
	 */
	uintptr_t CVorbisFile::ReadFunc( void * _pvPtr, uintptr_t _uiptrSize, uintptr_t _uiptrNMemb, void * _pvDataSource ) {
		LSF_FILE fFile = reinterpret_cast<LSF_FILE>(_pvDataSource);
		if ( CFileLib::ReadFile( fFile, _pvPtr, _uiptrSize * _uiptrNMemb ) ) {
			return _uiptrSize * _uiptrNMemb;
		}
		return 0;
	}

	/**
	 * Our custom seek function.
	 *
	 * \param _pvDataSource File handle.
	 * \param _i64Offset Amount to seek.
	 * \param _i32Whence Starting point.
	 * \return Returns 0 on success, -1 on failure.
	 */
	int32_t CVorbisFile::SeekFunc( void * _pvDataSource, int64_t _i64Offset, int32_t _i32Whence ) {
		LSF_FILE fFile = reinterpret_cast<LSF_FILE>(_pvDataSource);
		if ( !CFileLib::SetFilePointer( fFile, _i64Offset, static_cast<LSF_SEEK>(_i32Whence) ) ) { return -1; }
		return 0;
	}

	/**
	 * Our custom close function.
	 *
	 * \param _pvDataSource File handle to close.
	 * \return Returns 0 if the handle was closed, EOF otherwise.
	 */
	int32_t CVorbisFile::CloseFunc( void * _pvDataSource ) {
		LSF_FILE fFile = reinterpret_cast<LSF_FILE>(_pvDataSource);
		if ( CFileLib::CloseFile( fFile ) ) { return 0; }
		return CFileLib::Eof();
	}

	/**
	 * Our custom tell function.
	 *
	 * \param _pvDataSource File handle.
	 * \return Returns the position of the file pointer or -1.
	 */
	int64_t CVorbisFile::TellFunc( void * _pvDataSource ) {
		LSF_FILE fFile = reinterpret_cast<LSF_FILE>(_pvDataSource);
		return CFileLib::GetFilePointer( fFile );
	}

}	// namespace lss

#pragma warning( pop )
