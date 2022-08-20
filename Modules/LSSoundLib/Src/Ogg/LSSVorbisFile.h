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


#ifndef __LSE_VORBISFILE_H__
#define __LSE_VORBISFILE_H__

#include "../LSSSoundLib.h"
#include "Allocator/LSTLAllocator.h"
#include "LSSOgg.h"
#include "LSSVorbisCodec.h"

namespace lss {

	/**
	 * Class CVorbisFile
	 * \brief Convenience library for opening/seeking/decoding.
	 *
	 * Description: Convenience library for opening/seeking/decoding.
	 */
	class CVorbisFile {
	public :
		// == Types.
		/**
		 * The LSS_OV_CALLBACKS structure contains file manipulation function prototypes necessary for opening, closing, seeking, and location.
		 * The LSS_OV_CALLBACKS structure does not need to be user-defined if you are working with stdio-based file manipulation; the OvFOpen() and OvOpen() calls internally provide default callbacks for stdio. LSS_OV_CALLBACKS are defined and passed to OvOpenCallbacks() when implementing non-stdio based stream manipulation (such as playback from a memory buffer) or when OvOpen()-style initialization from a FILE * is required under Windows [a].
		 */
		typedef struct LSS_OV_CALLBACKS {
			/**
			 * Pointer to custom data reading function.
			 */
			uintptr_t							(* PfReadFunc)( void * _pvPtr, uintptr_t _uiptrSize, uintptr_t _uiptrNMemb, void * _pvDataSource );

			/**
			 * Pointer to custom data seeking function. If the data source is not seekable (or the application wants the data source to be treated as unseekable at all times), the provided seek callback should always return -1 (failure) or the PfSeekFunc and PfTellFunc fields should be set to NULL.
			 */
			int32_t								(* PfSeekFunc)( void * _pvDataSource, int64_t _i64Offset, int32_t _i32Whence );

			/**
			 * Pointer to custom data source closure function. Set to NULL if libvorbisfile should not attempt to automatically close the file/data handle.
			 */
			int32_t								(* PfCloseFunc)( void * _pvDataSource );

			/**
			 * Pointer to custom data location function. If the data source is not seekable (or the application wants the data source to be treated as unseekable at all times), the provided tell callback should always return -1 (failure) or the PfSeekFunc and PfTellFunc fields should be set to NULL.
			 */
			int64_t								(* PfTellFunc)( void * _pvDataSource );
		} * LPLSS_OV_CALLBACKS, * const LPCLSS_OV_CALLBACKS;

		/**
		 * Ogg file data.
		 */
		typedef struct LSS_OV_FILE {
			void *							datasource; // Pointer to a FILE *, etc.
			int32_t								seekable;
			int64_t								offset;
			int64_t								end;
			COgg::ogg_sync_state				oy;

			// If the FILE handle isn't seekable (eg. a pipe), only the current stream appears.
			int32_t								links;
			int64_t *							offsets;
			int64_t *							dataoffsets;
			int32_t *							serialnos;
			int64_t *							pcmlengths; // Overloaded to maintain binary
															//	compatibility; x2 size, stores both
															//	beginning and end values.
			CVorbisCodec::vorbis_info *			vi;
			CVorbisCodec::vorbis_comment *		vc;

			// Decoding working state local storage.
			int64_t								pcm_offset;
			int32_t								ready_state;
			int32_t								current_serialno;
			int32_t								current_link;

			double							bittrack;
			double							samptrack;

			COgg::ogg_stream_state				os;			// Take physical pages, weld into a logical stream of packets.
			CVorbisCodec::vorbis_dsp_state		vd;			// Central working state for the packet->PCM decoder.
			CVorbisCodec::vorbis_block			vb;			// Local working space for packet->PCM decode.

			LSS_OV_CALLBACKS					callbacks;

		} * LPLSS_OV_FILE, * const LPCLSS_OV_FILE;


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
		static LSBOOL LSE_CALL					LoadOggFile( const char * _pcFile,
			uint8_t * &_pui8Data, uint32_t &_ui32Length,
			uint32_t &_ui32Freq, uint32_t &_ui32Bits, uint32_t &_ui32Channels, CAllocator * _paAllocator );

		/**
		 * Returns the actual number of PCM's in the given file.
		 *
		 * \param _ovfFile The file whose PCM count is to be obtained.
		 * \return Returns the number of PCM's in the given file.
		 */
		static uint64_t LSE_CALL				GetPcmCount( LSS_OV_FILE &_ovfFile );

		/**
		 * After a bitstream has been opened using OvFOpen()/OvOpen()/OvOpenCallbacks() and decoding is complete, the application must call OvClear() to clear the decoder's buffers. OvClear() will also close the file unless it was opened using OvOpenCallbacks() with the PfCloseFunc callback set to NULL.
		 * OvClear() must also be called after a successful call to OvTest() or OvTestCallbacks().
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions. After OvClear has been called, the contents of this structure are deallocated, and it can no longer be used without being reinitialized by a call to OvFOpen(), OvOpen() or OvOpenCallbacks().
		 * \return Returns 0 for success.
		 */
		static int32_t							OvClear( LSS_OV_FILE * _povfFile );

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
		static int32_t							OvFOpen( const char * _pcPath, LSS_OV_FILE * _povfFile );

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
		//static int32_t							OvOpen( FILE * _pfFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, int32_t _i32BytesI );

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
		static int32_t							OvOpenCallbacks( void * _pvDataSource, LSS_OV_FILE * _povfFile, 
			const char * _pcInitial, int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks );

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
		//static int32_t							OvTest( FILE * _pfFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, int32_t _i32BytesI );

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
		static int32_t							OvTestCallbacks( void * _pvDataSource, LSS_OV_FILE * _povfFile, 
			const char * _pcInitial, int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks );

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
		static int32_t							OvTestOpen( LSS_OV_FILE * _povfFile );

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
		static int32_t							OvBitrate( LSS_OV_FILE * _povfFile, int32_t _i32I );

		/**
		 * Used to find the most recent bitrate played back within the file. Will return 0 if the bitrate has not changed or it is the beginning of the file.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return 0 indicates the beginning of the file or unchanged bitrate info.
		 *	n indicates the actual bitrate since the last call.
		 *	OV_FALSE indicates that playback is not in progress, and thus there is no instantaneous bitrate information to report.
		 *	OV_EINVAL indicates that the stream represented by _povfFile is not open.
		 */
		static int32_t							OvBitrateInstant( LSS_OV_FILE * _povfFile );

		/**
		 * Returns the number of logical bitstreams within our physical bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return 1 indicates a single logical bitstream or an unseekable file.
		 *	n indicates the number of logical bitstreams.
		 */
		static int32_t							OvStreams( LSS_OV_FILE * _povfFile );

		/**
		 * This indicates whether or not the bitstream is seekable.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return 0 indicates that the file is not seekable.
		 *	nonzero indicates that the file is seekable.
		 */
		static int32_t							OvSeekable( LSS_OV_FILE * _povfFile );

		/**
		 * Returns the serialnumber of the specified logical bitstream link number within the overall physical bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the serial number of the current bitstream, this parameter should be set to -1.
		 * \return -1 if the specified logical bitstream _i32I does not exist.
		 *	Returns the serial number of the logical bitstream _i32I or the serial number of the current bitstream if the file is nonseekable.
		 */
		static int32_t							OvSerialNumber( LSS_OV_FILE * _povfFile, int32_t _i32I );

		/**
		 * Returns the total (compressed) bytes of the physical bitstream or a specified logical bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. To retrieve the total bytes for the entire physical bitstream, this parameter should be set to -1.
		 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is nonseekable
		 *	n total length in compressed bytes of content if _i32I=-1.
		 *	n length in compressed bytes of logical bitstream if _i32I=0 to n.
		 */
		static int64_t							OvRawTotal( LSS_OV_FILE * _povfFile, int32_t _i32I );

		/**
		 * Returns the total pcm samples of the physical bitstream or a specified logical bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. To retrieve the total pcm samples for the entire physical bitstream, this parameter should be set to -1.
		 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is unseekable.
		 *	total length in pcm samples of content if _i32I=-1.
		 *	length in pcm samples of logical bitstream if _i32I=0 to n.
		 */
		static int64_t							OvPcmTotal( LSS_OV_FILE * _povfFile, int32_t _i32I );

		/**
		 * Returns the total time in seconds of the physical bitstream or a specified logical bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. To retrieve the time total for the entire physical bitstream, this parameter should be set to -1.
		 * \return OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist or the bitstream is nonseekable.
		 *	n total length in seconds of content if _i32I=-1.
		 *	n length in seconds of logical bitstream if _i32I=0 to n.
		 */
		static double							OvTimeTotal( LSS_OV_FILE * _povfFile, int32_t _i32I );

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
		static int32_t							OvRawSeek( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

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
		static int32_t							OvPcmSeek( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

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
		static int32_t							OcPvmSeekPage( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

		/**
		 * For seekable streams, this seeks to the given time. For implementing seeking in a player, this is the only function generally needed. This also updates everything needed within the decoder, so you can immediately call OvRead() and get data from the newly seeked to position. This function does not work for unseekable streams.
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
		static int32_t							OvTimeSeek( LSS_OV_FILE * _povfFile, double _dPos );

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
		static int32_t							OvTimeSeekPage( LSS_OV_FILE * _povfFile, double _dPos );

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
		static int32_t							OvRawSeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

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
		static int32_t							OvPcmSeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

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
		static int32_t							OvPcmSeekPageLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos );

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
		static int32_t							OvTimeSeekLap( LSS_OV_FILE * _povfFile, double _dPos );

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
		static int32_t							OvTimeSeekPageLap( LSS_OV_FILE * _povfFile, double _dPos );

		/**
		 * Returns the current offset in raw compressed bytes.
		 * Note that if you later use OvRawSeek() to return to this point, you won't generally get back to exactly the same place, due to internal buffering. Also note that a read operation may not cause a change to the current raw offset - only a read that requires reading more data from the underlying data source will do that.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return n indicates the current offset in bytes.
		 *	OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist.
		 */
		static int64_t							OvRawTell( LSS_OV_FILE * _povfFile );

		/**
		 * Returns the current offset in samples.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return n indicates the current offset in samples.
		 *	OV_EINVAL means that the argument was invalid. In this case, the requested bitstream did not exist.
		 */
		static int64_t							OvPcmTell( LSS_OV_FILE * _povfFile );

		/**
		 * Returns the current decoding offset in seconds.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \return n indicates the current decoding time offset in seconds.
		 *	OV_EINVAL means that the argument was invalid.  In this case, the requested bitstream did not exist.
		 */
		static double							OvTimeTell( LSS_OV_FILE * _povfFile );

		/**
		 * Returns the vorbis_info struct for the specified bitstream. For nonseekable files, always returns the current vorbis_info struct.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the vorbis_info struct for the current bitstream, this parameter should be set to -1.
		 * \return Returns the vorbis_info struct for the specified bitstream. Returns vorbis_info for current bitstream if the file is nonseekable or _i32I=-1.
		 *	NULL if the specified bitstream does not exist or the file has been initialized improperly.
		 */
		static CVorbisCodec::vorbis_info *		OvInfo( LSS_OV_FILE * _povfFile, int32_t _i32I );

		/**
		 * Returns a pointer to the vorbis_comment struct for the specified bitstream. For nonseekable streams, returns the struct for the current bitstream.
		 *
		 * \param _povfFile A pointer to the LSS_OV_FILE structure--this is used for ALL the externally visible libvorbisfile functions.
		 * \param _i32I Link to the desired logical bitstream. For nonseekable files, this argument is ignored. To retrieve the vorbis_comment struct for the current bitstream, this parameter should be set to -1.
		 * \return Returns the vorbis_comment struct for the specified bitstream.
		 *	NULL if the specified bitstream does not exist or the file has been initialized improperly.
		 */
		static CVorbisCodec::vorbis_comment *	OvComment( LSS_OV_FILE * _povfFile, int32_t _i32I );

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
		static int32_t							OvReadFloat( LSS_OV_FILE * _povfFile, float *** _pppfPcmChannels, int32_t _i32Samples,
			int32_t * _pi32BitStream );

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
		static int32_t							OvReadFilter( LSS_OV_FILE * _povfFile, char * _pcBuffer, int32_t _i32Length,
			int32_t _i32BigEndianP, int32_t _i32Word, int32_t _i32Signed, int32_t * _pi32BitStream,
			void (* _pfFilter)( float ** _ppPcm, int32_t _i32Channels, int32_t _i32Samples, void * _pvFilterParam ), void * _pvFilterParam );

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
		static int32_t							OvRead( LSS_OV_FILE * _povfFile, char * _pcBuffer, int32_t _i32Length,
			int32_t _i32BigEndianP, int32_t _i32Word, int32_t _i32Signed, int32_t * _pi32BitStream );

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
		static int32_t							OvCrosslap( LSS_OV_FILE * _povfFile1, LSS_OV_FILE * _povfFile2 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _i32Flag Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvHalfRate( LSS_OV_FILE * _povfFile, int32_t _i32Flag );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvHalfRateP( LSS_OV_FILE * _povfFile );


	protected :
		// == Functions.
		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _pfFile Undocumented parameter from the Vorbis library.
		 * \param _i64Off Undocumented parameter from the Vorbis library.
		 * \param _i32Whence Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		//static int32_t							OvHeaderFSeekWrap( FILE * _pfFile, int64_t _i64Off, int32_t _i32Whence );

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
		static int32_t							OvOpen1( void * _pvFile, LSS_OV_FILE * _povfFile, const char * _pcInitial, 
			int32_t _i32BytesI, LSS_OV_CALLBACKS _ocCallbacks );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvOpen2( LSS_OV_FILE *_povfFile );

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
		static int32_t							OvFetchHeaders( LSS_OV_FILE * _povfFile, CVorbisCodec::vorbis_info * _pviVi, CVorbisCodec::vorbis_comment * _pvcVc, 
			int32_t ** _ppi32SerialNoList, int32_t * _pi32SerialNoN, 
			COgg::ogg_page * _popOgPtr );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvOpenSeekable2( LSS_OV_FILE *_povfFile );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _popOg Undocumented parameter from the Vorbis library.
		 * \param _i64Boundary Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int64_t							OvGetNextPage( LSS_OV_FILE *_povfFile, COgg::ogg_page * _popOg, 
			int64_t _i64Boundary );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _popOg Undocumented parameter from the Vorbis library.
		 * \param _pi32SerialNoList Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvLookUpPageSerialNo( COgg::ogg_page * _popOg, int32_t * _pi32SerialNoList, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _popOg Undocumented parameter from the Vorbis library.
		 * \param _ppi32SerialNoList Undocumented parameter from the Vorbis library.
		 * \param _pi32N Undocumented parameter from the Vorbis library.
		 */
		static void							OvAddSerialNo( COgg::ogg_page * _popOg, int32_t ** _ppi32SerialNoList, int32_t * _pi32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _pviInfo Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int64_t							OvInitialPcmOffset( LSS_OV_FILE * _povfFile, CVorbisCodec::vorbis_info * _pviInfo );

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
		static int64_t							OvGetPrevPageSerial( LSS_OV_FILE * _povfFile, 
			int32_t * _pi32SerialList, int32_t _i32SerialCount, 
			int32_t * _pi32SerialNo, int64_t * _pi64GranPos );

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
		static int32_t							OvBisectForwardSerialNo( LSS_OV_FILE * _povfFile, 
			int64_t _i64Begin, 
			int64_t _i64Searched, 
			int64_t _i64End, 
			int64_t _i64EndGran, 
			int32_t _i32EndSerial, 
			int32_t * _pi32CurrentNoList, 
			int32_t _i32CurrentNoS, 
			int32_t _i32M );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvGetData( LSS_OV_FILE * _povfFile );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _i32S Undocumented parameter from the Vorbis library.
		 * \param _pi32SerialNoList Undocumented parameter from the Vorbis library.
		 * \param _i32N Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvLookUpSerialNo( int32_t _i32S, int32_t * _pi32SerialNoList, int32_t _i32N );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _i64Offset Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvSeekHelper( LSS_OV_FILE *_povfFile, int64_t _i64Offset );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 */
		static void							OvDecodeClear( LSS_OV_FILE * _povfFile );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvMakeDecodeReady( LSS_OV_FILE * _povfFile );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _popOpIn Undocumented parameter from the Vorbis library.
		 * \param _i32ReadP Undocumented parameter from the Vorbis library.
		 * \param _i32SpanP Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvFetchAndProcessPacket( LSS_OV_FILE *_povfFile, COgg::ogg_packet * _popOpIn, int32_t _i32ReadP, int32_t _i32SpanP );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _popPage Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int64_t							OvGetPrevPage( LSS_OV_FILE * _povfFile, COgg::ogg_page * _popPage );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _i64Pos Undocumented parameter from the Vorbis library.
		 * \param _pfLocalSeek Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							Ov64SeekLap( LSS_OV_FILE * _povfFile, int64_t _i64Pos, int32_t (* _pfLocalSeek)( LSS_OV_FILE *, int64_t ) );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvInitSet( LSS_OV_FILE * _povfFile );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _pviInfo Undocumented parameter from the Vorbis library.
		 * \param _pvdsVd Undocumented parameter from the Vorbis library.
		 * \param _ppfLapPcm Undocumented parameter from the Vorbis library.
		 * \param _i32LapSize Undocumented parameter from the Vorbis library.
		 */
		static void							OvGetLap( LSS_OV_FILE *_povfFile, CVorbisCodec::vorbis_info * _pviInfo, CVorbisCodec::vorbis_dsp_state * _pvdsVd, float ** _ppfLapPcm, int32_t _i32LapSize );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvInitPrime( LSS_OV_FILE * _povfFile );

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
		static void							OvSplice( float ** _ppfPcm, float ** _ppfLapPcm, 
			int32_t _i32N1, int32_t _i32N2, 
			int32_t _i32Ch1, int32_t _i32Ch2, 
			float * _pfW1, float * _pfW2 );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \param _povfFile Undocumented parameter from the Vorbis library.
		 * \param _dPos Undocumented parameter from the Vorbis library.
		 * \param _pfLocalSeek Undocumented parameter from the Vorbis library.
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvDSeekLap( LSS_OV_FILE *_povfFile, double _dPos, int32_t (* _pfLocalSeek)( LSS_OV_FILE *, double ) );

		/**
		 * Undocumented function from the Vorbis library.
		 *
		 * \return Undocumented return from the Vorbis library.
		 */
		static int32_t							OvHostIsBigEndian();

		/**
		 * Our custom read function.
		 *
		 * \param _pvPtr Location to store the read data.
		 * \param _uiptrSize Size of the elements to read.
		 * \param _uiptrNMemb Number of elements to read.
		 * \param _pvDataSource File handle.
		 * \return Returns the number of bytes read.
		 */
		static uintptr_t						ReadFunc( void * _pvPtr, uintptr_t _uiptrSize, uintptr_t _uiptrNMemb, void * _pvDataSource );

		/**
		 * Our custom seek function.
		 *
		 * \param _pvDataSource File handle.
		 * \param _i64Offset Amount to seek.
		 * \param _i32Whence Starting point.
		 * \return Returns 0 on success, -1 on failure.
		 */
		static int32_t							SeekFunc( void * _pvDataSource, int64_t _i64Offset, int32_t _i32Whence );

		/**
		 * Our custom close function.
		 *
		 * \param _pvDataSource File handle to close.
		 * \return Returns 0 if the handle was closed, EOF otherwise.
		 */
		static int32_t							CloseFunc( void * _pvDataSource );

		/**
		 * Our custom tell function.
		 *
		 * \param _pvDataSource File handle.
		 * \return Returns the position of the file pointer or -1.
		 */
		static int64_t							TellFunc( void * _pvDataSource );
	};

}	// namespace lss

#endif	// __LSE_VORBISFILE_H__

