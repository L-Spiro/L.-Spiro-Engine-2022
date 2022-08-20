/* -----------------------------------------------------------------------------

	Copyright (c) 2006 Simon Brown                          si@sjbrown.co.uk

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the 
	"Software"), to	deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to 
	permit persons to whom the Software is furnished to do so, subject to 
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
   -------------------------------------------------------------------------- */
   
#include <squish.h>
#include "colourset.h"
#include "maths.h"
#include "rangefit.h"
#include "clusterfit.h"
#include "colourblock.h"
#include "alpha.h"
#include "singlecolourfit.h"

namespace squish {

static int FixFlags( int flags )
{
	// grab the flag bits
	int method = flags & (kDxt1 | kDxt3 | kDxt5 | kBc4 | kBc5);
	int fit = flags & (kColorIterativeClusterFit | kColorClusterFit | kColorRangeFit);
	int metric = flags & (kColorMetricPerceptual | kColorMetricUniform);
	int extra = flags & (kWeightColorByAlpha | kBc5IsLatc2 | kSigned);
	
	// set defaults
	if( method != kDxt3 && method != kDxt5 && method != kBc4 && method != kBc5 ) {
		method = kDxt1;
	}
	if( fit != kColorRangeFit ) {
		fit = kColorClusterFit;
	}
	if( metric != kColorMetricUniform ) {
		metric = kColorMetricPerceptual;
	}
		
	// done
	return method | fit | metric | extra;
}
#ifdef SQUISH_USE_FLOATS
void Compress( float const* rgba, void* block, int flags, const SquishConfig &config )
#else
void Compress( u8 const* rgba, void* block, int flags, const SquishConfig &config )
#endif	// #ifdef SQUISH_USE_FLOATS
{
	// compress with full mask
	CompressMasked( rgba, 0xffff, block, flags, config );
}
#ifdef SQUISH_USE_FLOATS
void CompressMasked( float const* rgba, int mask, void* block, int flags, const SquishConfig &config )
#else
void CompressMasked( u8 const* rgba, int mask, void* block, int flags, const SquishConfig &config )
#endif	// #ifdef SQUISH_USE_FLOATS
{
	// fix any bad flags
	flags = FixFlags( flags );

	// get the block locations
	void* colorBlock = block;
	void* alphaBock = block;
	if( (flags & (kDxt3 | kDxt5)) != 0 ) {
		colorBlock = reinterpret_cast<u8 *>(block) + 8;
	}
	else if ( (flags & kBc4) != 0 ) {
		if ( (flags & kSigned) != 0 ) {
			CompressAlphaDxt5Signed( rgba, mask, alphaBock, 0 );
		}
		else {
			CompressAlphaDxt5( rgba, mask, alphaBock, 0 );
		}
		return;
	}
	else if ( (flags & kBc5) != 0 ) {
		if ( (flags & kSigned) != 0 ) {
			CompressAlphaDxt5Signed( rgba, mask, alphaBock, 0 );
		}
		else {
			CompressAlphaDxt5( rgba, mask, alphaBock, 0 );
		}
		alphaBock = reinterpret_cast<u8 *>(alphaBock) + 8;
		if ( (flags & kBc5IsLatc2) != 0 ) {
			if ( (flags & kSigned) != 0 ) {
				CompressAlphaDxt5Signed( rgba, mask, alphaBock, 3 );
			}
			else {
				CompressAlphaDxt5( rgba, mask, alphaBock, 3 );
			}
		}
		else {
			if ( (flags & kSigned) != 0 ) {
				CompressAlphaDxt5Signed( rgba, mask, alphaBock, 1 );
			}
			else {
				CompressAlphaDxt5( rgba, mask, alphaBock, 1 );
			}
		}
		return;
	}

	// create the minimal point set
	ColorSet colors( rgba, mask, flags, config );
	
	// check the compression type and compress color
	if( colors.GetCount() == 1 ) {
		// always do a single color fit
		SingleColorFit fit( &colors, flags );
		fit.Compress( colorBlock );
	}
	else if( ( flags & kColorRangeFit ) != 0 || colors.GetCount() == 0 ) {
		// do a range fit
		RangeFit fit( &colors, flags, config );
		fit.Compress( colorBlock );
	}
	else
	{
		// default to a cluster fit (could be iterative or not)
		ClusterFit fit( &colors, flags, config );
		fit.Compress( colorBlock );
	}
	
	// compress alpha separately if necessary
	if( ( flags & kDxt3 ) != 0 ) {
		CompressAlphaDxt3( rgba, mask, alphaBock );
	}
	else if( ( flags & kDxt5 ) != 0 ) {
		if ( (flags & kSigned) != 0 ) {
			CompressAlphaDxt5Signed( rgba, mask, alphaBock );
		}
		else {
			CompressAlphaDxt5( rgba, mask, alphaBock );
		}
	}
}

void Decompress( u8* rgba, void const* block, int flags )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// get the block locations
	void const* colorBlock = block;
	void const* alphaBock = block;
	if( ( flags & ( kDxt3 | kDxt5 ) ) != 0 )
		colorBlock = reinterpret_cast< u8 const* >( block ) + 8;

	// decompress color
	DecompressColor( rgba, colorBlock, ( flags & kDxt1 ) != 0 );

	// decompress alpha separately if necessary
	if( ( flags & kDxt3 ) != 0 )
		DecompressAlphaDxt3( rgba, alphaBock );
	else if( ( flags & kDxt5 ) != 0 )
		DecompressAlphaDxt5( rgba, alphaBock );
}

int GetStorageRequirements( int width, int height, int flags )
{
	// fix any bad flags
	flags = FixFlags( flags );
	
	// compute the storage requirements
	int blockcount = ( ( width + 3 )/4 ) * ( ( height + 3 )/4 );
	int blocksize = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;
	return blockcount*blocksize;	
}

void CompressImage( u8 const* rgba, int width, int height, void* blocks, int flags, const SquishConfig &config )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// initialise the block output
	u8* targetBlock = reinterpret_cast< u8* >( blocks );
	int bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;

	// loop over blocks
	for( int y = 0; y < height; y += 4 )
	{
		for( int x = 0; x < width; x += 4 )
		{
			// build the 4x4 block of pixels
#ifdef SQUISH_USE_FLOATS
			float sourceRgba[16*4];
			float* targetPixel = sourceRgba;
#else
			u8 sourceRgba[16*4];
			u8* targetPixel = sourceRgba;
#endif	// #ifdef SQUISH_USE_FLOATS
			
			int mask = 0;
			for( int py = 0; py < 4; ++py )
			{
				for( int px = 0; px < 4; ++px )
				{
					// get the source pixel in the image
					int sx = x + px;
					int sy = y + py;
					
					// enable if we're in the image
					if( sx < width && sy < height )
					{
						// copy the rgba value
						u8 const* sourcePixel = rgba + 4*( width*sy + sx );
						for( int i = 0; i < 4; ++i ) {
#ifdef SQUISH_USE_FLOATS
							*targetPixel++ = (*sourcePixel++) * (1.0f / 255.0f);
#else
							*targetPixel++ = *sourcePixel++;
#endif	// #ifdef SQUISH_USE_FLOATS
						}
							
						// enable this pixel
						mask |= ( 1 << ( 4*py + px ) );
					}
					else
					{
						// skip this pixel as its outside the image
						targetPixel += 4;
					}
				}
			}
			
			// compress it into the output
			CompressMasked( sourceRgba, mask, targetBlock, flags, config );
			
			// advance
			targetBlock += bytesPerBlock;
		}
	}
}

void DecompressImage( u8* rgba, int width, int height, void const* blocks, int flags )
{
	// fix any bad flags
	flags = FixFlags( flags );

	// initialise the block input
	u8 const* sourceBlock = reinterpret_cast< u8 const* >( blocks );
	int bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;

	// loop over blocks
	for( int y = 0; y < height; y += 4 )
	{
		for( int x = 0; x < width; x += 4 )
		{
			// decompress the block
			u8 targetRgba[4*16];
			Decompress( targetRgba, sourceBlock, flags );

			// Byteswap the result.
			uint32_t * pui32ByteSwap = reinterpret_cast<uint32_t *>(targetRgba);
			for ( int32_t I = 0; I < sizeof( targetRgba ) / sizeof( targetRgba[0] ); I += 4, ++pui32ByteSwap ) {
				(*pui32ByteSwap) = _byteswap_ulong( (*pui32ByteSwap) );
				/*u8 u8Temp = targetRgba[I];
				targetRgba[I] = targetRgba[I+3];
				targetRgba[I+3] = u8Temp;
				u8Temp = targetRgba[I+1];
				targetRgba[I+1] = targetRgba[I+2];
				targetRgba[I+2] = u8Temp;*/
			}
			
			// write the decompressed pixels to the correct image locations
			u8 const* sourcePixel = targetRgba;
			for( int py = 0; py < 4; ++py )
			{
				for( int px = 0; px < 4; ++px )
				{
					// get the target location
					int sx = x + px;
					int sy = y + py;
					if( sx < width && sy < height )
					{
						u8* targetPixel = rgba + 4*( width*sy + sx );
						
						// copy the rgba value
						for( int i = 0; i < 4; ++i )
							*targetPixel++ = *sourcePixel++;
					}
					else
					{
						// skip this pixel as its outside the image
						sourcePixel += 4;
					}
				}
			}
			
			// advance
			sourceBlock += bytesPerBlock;
		}
	}
}

} // namespace squish
