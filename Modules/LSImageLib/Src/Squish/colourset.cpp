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
   
#include "colourset.h"

namespace squish {

#ifdef SQUISH_USE_FLOATS
ColorSet::ColorSet( float const* rgba, int mask, int flags, const SquishConfig &config )
  : m_count( 0 ), 
	m_transparent( false )
{
	// check the compression mode for dxt1
	bool isDxt1 = ( ( flags & kDxt1 ) != 0 );
	bool weightByAlpha = ( ( flags & kWeightColorByAlpha ) != 0 );

	// create the minimal set
	for( int i = 0; i < 16; ++i )
	{
		int iIndex = i << 2;
		// check this pixel is enabled
		int bit = 1 << i;
		if( ( mask & bit ) == 0 )
		{
			m_remap[i] = -1;
			continue;
		}
	
		// check for transparent pixels when using dxt1
		if( isDxt1 && rgba[iIndex + 3] < config.fAlphaCutoff )
		{
			m_remap[i] = -1;
			m_transparent = true;
			continue;
		}

		// loop over previous points for a match
		for( int j = 0;; ++j )
		{
			// allocate a new point
			if( j == i )
			{
				float x = rgba[iIndex];
				float y = rgba[iIndex + 1];
				float z = rgba[iIndex + 2];
				
				// ensure there is always non-zero weight even for zero alpha
				float w = ((rgba[iIndex + 3] * 255.0f) + 1.0f) * (1.0f / 256.0f);

				// add the point
				m_points[m_count] = Vec3( x, y, z );
				m_weights[m_count] = (weightByAlpha ? w : 1.0f);
				m_remap[i] = m_count;
				
				// advance
				++m_count;
				break;
			}
		
			// check for a match
			int oldbit = 1 << j;
			int iIndexJ = j << 2;
			bool match = ( ( mask & oldbit ) != 0 )
				&& ( rgba[iIndex] == rgba[iIndexJ] )
				&& ( rgba[iIndex + 1] == rgba[iIndexJ + 1] )
				&& ( rgba[iIndex + 2] == rgba[iIndexJ + 2] )
				&& ( rgba[iIndexJ + 3] >= config.fAlphaCutoff || !isDxt1 );
			if( match )
			{
				// get the index of the match
				int index = m_remap[j];
				
				// ensure there is always non-zero weight even for zero alpha
				float w = (float)( (rgba[iIndex + 3] * 255.0f) + 1.0f ) / 256.0f;

				// map to this point and increase the weight
				m_weights[index] += ( weightByAlpha ? w : 1.0f );
				m_remap[i] = index;
				break;
			}
		}
	}

	// square root the weights
	for( int i = 0; i < m_count; ++i ) {
		m_weights[i] = std::sqrt( m_weights[i] );
	}
}
#else
ColorSet::ColorSet( u8 const* rgba, int mask, int flags, const SquishConfig &config )
  : m_count( 0 ), 
	m_transparent( false )
{
	// check the compression mode for dxt1
	bool isDxt1 = ( ( flags & kDxt1 ) != 0 );
	bool weightByAlpha = ( ( flags & kWeightColorByAlpha ) != 0 );
	u8 u8Alpha = static_cast<u8>(config.fAlphaCutoff * 255.0f);

	// create the minimal set
	for( int i = 0; i < 16; ++i )
	{
		// check this pixel is enabled
		int bit = 1 << i;
		if( ( mask & bit ) == 0 )
		{
			m_remap[i] = -1;
			continue;
		}
	
		// check for transparent pixels when using dxt1
		if( isDxt1 && rgba[4*i + 3] < u8Alpha )
		{
			m_remap[i] = -1;
			m_transparent = true;
			continue;
		}

		// loop over previous points for a match
		for( int j = 0;; ++j )
		{
			// allocate a new point
			if( j == i )
			{
				// normalise coordinates to [0,1]
				float x = (float)rgba[4*i] / 255.0f;
				float y = (float)rgba[4*i + 1] / 255.0f;
				float z = (float)rgba[4*i + 2] / 255.0f;
				
				// ensure there is always non-zero weight even for zero alpha
				float w = (float)( rgba[4*i + 3] + 1 ) / 256.0f;

				// add the point
				m_points[m_count] = Vec3( x, y, z );
				m_weights[m_count] = ( weightByAlpha ? w : 1.0f );
				m_remap[i] = m_count;
				
				// advance
				++m_count;
				break;
			}
		
			// check for a match
			int oldbit = 1 << j;
			bool match = ( ( mask & oldbit ) != 0 )
				&& ( rgba[4*i] == rgba[4*j] )
				&& ( rgba[4*i + 1] == rgba[4*j + 1] )
				&& ( rgba[4*i + 2] == rgba[4*j + 2] )
				&& ( rgba[4*j + 3] >= u8Alpha || !isDxt1 );
			if( match )
			{
				// get the index of the match
				int index = m_remap[j];
				
				// ensure there is always non-zero weight even for zero alpha
				float w = (float)( rgba[4*i + 3] + 1 ) / 256.0f;

				// map to this point and increase the weight
				m_weights[index] += ( weightByAlpha ? w : 1.0f );
				m_remap[i] = index;
				break;
			}
		}
	}

	// square root the weights
	for( int i = 0; i < m_count; ++i )
		m_weights[i] = std::sqrt( m_weights[i] );
}
#endif	// #ifdef SQUISH_USE_FLOATS

void ColorSet::RemapIndices( u8 const* source, u8* target ) const
{
	for( int i = 0; i < 16; ++i )
	{
		int j = m_remap[i];
		if( j == -1 )
			target[i] = 3;
		else
			target[i] = source[j];
	}
}

} // namespace squish
