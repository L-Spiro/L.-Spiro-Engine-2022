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
 * Description: Handles the loading of 8-bit PNG files.
 */


#ifndef __LSI_PNG_H__
#define __LSI_PNG_H__

#include "../LSIImageLib.h"
#include "Streams/LSSTDExternalByteStream.h"

namespace lsi {

	/**
	 * Class CPng
	 * \brief Handles the loading of 8-bit PNG files.
	 *
	 * Description: Handles the loading of 8-bit PNG files.
	 */
	class CPng {
	public :
		// == Functions.
		/**
		 * Load a PNG file from memory.
		 *
		 * \param _pui8FileData The in-memory image of the file.
		 * \param _ui32DataLen The length of the in-memory image of the file.
		 * \param _ui32Width The returned width of the image.
		 * \param _ui32Height The returned height of the image.
		 * \param _pfFormat Indicates the format of the converted data held in _tbReturn.
		 * \param _tbReturn The returned data.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		static LSBOOL LSE_CALL			LoadPng( const uint8_t * _pui8FileData, uint32_t _ui32DataLen,
			uint32_t &_ui32Width, uint32_t &_ui32Height, LSI_PIXEL_FORMAT &_pfFormat,
			CImageLib::CTexelBuffer &_tbReturn );



	protected :
		// == Enumerations.
		enum LSI_PNG_FILTER {
			LSI_PF_NONE,
			LSI_PF_SUB,
			LSI_PF_UP,
			LSI_PF_AVG,
			LSI_PF_PAETH,
			LSI_PF_AVG_FIRST,
			LSI_PF_PAETH_FIRST,
		};


		// == Types.
		/**
		 * A PNG chunk.
		 */
		typedef struct LSI_PNG_CHUNK {
			/**
			 * The chunk type.
			 */
			uint32_t 					ui32Type;

			/**
			 * The length of the chunk.
			 */
			uint32_t					ui32Length;
		} * LPLSI_PNG_CHUNK, * const LPCLSI_PNG_CHUNK;

		/**
		 * For composing the final PNG structure.
		 */
		typedef struct LSI_PNG_FINAL {
			/**
			 * Width of the image.
			 */
			uint32_t					ui32Width;

			/**
			 * Height of the image.
			 */
			uint32_t					ui32Height;

			/**
			 * X value.
			 */
			uint32_t					ui32X;

			/**
			 * Y value.
			 */
			uint32_t					ui32Y;

			/**
			 * Pointer to the raw data.
			 */
			uint8_t *					puiRawData;

			/**
			 * Length of the raw data.
			 */
			uint32_t					ui32RawLen;

			/**
			 * Components.
			 */
			uint32_t					ui32Components;
		} * LPLSI_PNG_FINAL, * const LPCLSI_PNG_FINAL;

		// == Members.
		/**
		 * Translates from a standard filter to a first-row filter.
		 */
		static LSI_PNG_FILTER			m_pfFirstRowFilter[];


		// == Functions.
		/**
		 * Check the header of a PNG file loaded to memory.  Validates that the header data matches a PNG
		 *	header.
		 *
		 * \param _ebsStream The stream containing the loaded PNG file.  The position must be at the start of
		 *	the stream.
		 * \return Returns true if the bytes in the stream form a valid PNG header.
		 */
		static LSBOOL LSE_CALL			CheckHeader( const CExternalByteStream &_ebsStream );

		/**
		 * Read the next chunk.
		 *
		 * \param _ebsStream The stream from which to read the next chunk.
		 * \param _pcChunk Holds the returned chunk data, which does not need to be freed.
		 * \return Returns true if the next chunk was read.
		 */
		static LSBOOL LSE_CALL			NextChunk( const CExternalByteStream &_ebsStream, LSI_PNG_CHUNK &_pcChunk );

		/**
		 * Create the final image data from decompressed PNG data.  Image data must not be interlaced.
		 *
		 * \param _pfFinal Parameters.
		 * \param _tbReturn The output of the processing.
		 * \return Returns true if there is enough memory to allocate the final buffer.
		 */
		static LSBOOL LSE_CALL			CreatePng( LSI_PNG_FINAL &_pfFinal, CImageLib::CTexelBuffer &_tbReturn );

		/**
		 * Create the final image data from decompressed PNG data.
		 *
		 * \param _pfFinal Parameters.
		 * \param _tbReturn The output of the processing.
		 * \param _bInterlaced If true, the interlace decoder is used.
		 * \return Returns true if there is enough memory to allocate the final buffer.
		 */
		static LSBOOL LSE_CALL			CreatePng( LSI_PNG_FINAL &_pfFinal, CImageLib::CTexelBuffer &_tbReturn, LSBOOL _bInterlaced );

		/**
		 * Expand the color data with the data in the given palette.
		 *
		 * \param _tbReturn The buffer to expand in-place.
		 * \param _pui8Palette The palette.
		 * \param _ui32Len Length of the palette.
		 * \param _ui32Width Width of the image.
		 * \param _ui32Height Height of the image.
		 * \param _ui32Components Components in the image.
		 * \param _bSwapRB If true, the R and B components of the palette are swapped as they are placed into the image buffer.
		 * \return Returns true if there is enough memory to expand the buffer and if none of the indices refence above
		 *	or equal to _ui32Len.
		 */
		static LSBOOL LSE_CALL			ExpandPalette( CImageLib::CTexelBuffer &_tbReturn, const uint8_t * _pui8Palette, uint32_t _ui32Len,
			uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Components, LSBOOL _bSwapRB );

		/**
		 * Swap the RGB colors in the given buffer.
		 *
		 * \param _tbBuffer Buffer of colors to fix.
		 * \param _ui32Components Number of components.
		 */
		static void LSE_CALL			FixColors( CImageLib::CTexelBuffer &_tbBuffer, uint32_t _ui32Components );

		/**
		 * Reads a single 8-bit value from the given stream.
		 *
		 * \param _ebsStream The stream from which to read an 8-bit value.
		 * \param _ui8Dst Destination for the read value.
		 * \return Returns true if the value was read from the stream.
		 */
		static LSBOOL LSE_CALL			ReadUi8( const CExternalByteStream &_ebsStream, uint8_t &_ui8Dst );

		/**
		 * Reads a single 16-bit value from the given stream.
		 *
		 * \param _ebsStream The stream from which to read an 16-bit value.
		 * \param _ui16Dst Destination for the read value.
		 * \return Returns true if the value was read from the stream.
		 */
		static LSBOOL LSE_CALL			ReadUi16( const CExternalByteStream &_ebsStream, uint16_t &_ui16Dst );

		/**
		 * Reads a single 32-bit value from the given stream.
		 *
		 * \param _ebsStream The stream from which to read an 32-bit value.
		 * \param _ui32Dst Destination for the read value.
		 * \return Returns true if the value was read from the stream.
		 */
		static LSBOOL LSE_CALL			ReadUi32( const CExternalByteStream &_ebsStream, uint32_t &_ui32Dst );

		/**
		 * Skip a section of the stream.
		 *
		 * \param _ebsStream The stream.
		 * \param _ui32Length Number of bytes to skip.
		 */
		static void LSE_CALL			Skip( const CExternalByteStream &_ebsStream, uint32_t _ui32Length );

		/**
		 * Perform a PAETH filter.
		 *
		 * \param _i32A A.
		 * \param _i32B B.
		 * \param _i32C C.
		 * \return Returns happiness.
		 */
		static int32_t LSE_CALL			Paeth( int32_t _i32A, int32_t _i32B, int32_t _i32C );
	};

}	// namespace lsi

#endif	// __LSI_PNG_H__
