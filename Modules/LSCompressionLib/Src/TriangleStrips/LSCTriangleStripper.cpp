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
 * Description: Creates a single one-sided triangle strip.  Code based on Pierre Terdiman's original work.
 */

#include "LSCTriangleStripper.h"

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

#define		MAKE_ADJ_TRI( X )			((X) & 0x3FFFFFFF)
#define		GET_EDGE_NB( X )			((X) >> 30)
#define		IS_BOUNDARY( X )			((X) == 0xFFFFFFFF)

namespace lsc {

	// == Various constructors.
	LSE_CALLCTOR CTriangleStripper::CTriangleStripper() :
		m_bWords( false ) {
	}
	LSE_CALLCTOR CTriangleStripper::~CTriangleStripper() {
	}

	// == Functions.
	/**
	 * Initialize the triangle stripper.
	 *
	 * \param _ui32Faces Number of triangles.
	 * \param _pui32Faces Pointer to the triangles with 32-bit indices or NULL.
	 * \param _pui16Faces Pointer to the triangles with 16-bit indices or NULL.
	 * \return Returns false if there are not enough resources to complete the setup.
	 */
	LSBOOL LSE_CALL CTriangleStripper::InitTriangleStripper( uint32_t _ui32Faces, const uint32_t * _pui32Faces, const uint16_t * _pui16Faces ) {
		m_saAdj.Reset();
		m_vVerts.Reset();
		m_bWords = _pui32Faces == NULL;

		if ( !m_saAdj.InitStripAdjacency( _ui32Faces, _pui32Faces, _pui16Faces ) ) { return false; }
		if ( !m_saAdj.CreateDatabase() ) { return false; }
		return true;
	}

	/**
	 * Create the triangle strip.  After a successful call to this function, the created triangle strip can
	 *	be read directly.
	 *
	 * \return Returns true if there were no memory failures during the operation.
	 */
	LSBOOL LSE_CALL CTriangleStripper::Compute() {
		if ( !m_saAdj.TotalFaces() ) { return true; }


		CVectorPoD<uint8_t, uint32_t> vTags;
		if ( !vTags.Resize( m_saAdj.TotalFaces() ) ) { return false; }
		CStd::MemSet( &vTags[0], 0, vTags.Length() * sizeof( vTags[0] ) );

		CVectorPoD<uint32_t, uint32_t> vConnectivity;
		if ( !vConnectivity.Resize( m_saAdj.TotalFaces() ) ) { return false; }
		CStd::MemSet( &vConnectivity[0], 0, vConnectivity.Length() * sizeof( vConnectivity[0] ) );

		
		for ( uint32_t I = 0UL; I < m_saAdj.TotalFaces(); ++I ) {
			const CStripAdjacency::LSC_ADJ_TRI * patTri = &m_saAdj.Faces()[I];
			if ( !IS_BOUNDARY( patTri->ui32Tris[0] ) ) { ++vConnectivity[I]; }
			if ( !IS_BOUNDARY( patTri->ui32Tris[1] ) ) { ++vConnectivity[I]; }
			if ( !IS_BOUNDARY( patTri->ui32Tris[2] ) ) { ++vConnectivity[I]; }
		}
		// Sort by number of neighbors.
		CIndexSorter<uint32_t> isSorter;
		const uint32_t * pui32Sorted = isSorter.Sort( &vConnectivity[0], vConnectivity.Length() ).GetIndices();
		vConnectivity.Reset();	// Working on pui32Sorted from here out.

		CVectorPoD<uint32_t, uint32_t, 1024UL> vFinalStrip;
		uint32_t ui32Counter = 0UL;
		{
			CVectorPoD<uint8_t, uint32_t> vTempTags;	// Our tags are copied into this each time a run is made over
														//	the faces to determine which direction yields the longest
														//	strip.  We create the buffer once and pass it down to avoid
														//	reallocation.
			if ( !vTempTags.Resize( m_saAdj.TotalFaces() ) ) { return false; }

			// More buffers the helper functions are going to need.
			CVectorPoD<uint32_t, uint32_t> vTempFaces;
			CVectorPoD<uint32_t, uint32_t> vTempBestFaces;

			CVectorPoD<uint32_t, uint32_t> vTempStrips;
			CVectorPoD<uint32_t, uint32_t> vTempBestStrips;


			if ( !vTempStrips.Resize( m_saAdj.TotalFaces() + 2UL + 1UL + 2UL ) ) { return false; }
			if ( !vTempBestStrips.Resize( m_saAdj.TotalFaces() + 2UL + 1UL + 2UL ) ) { return false; }

			if ( !vTempFaces.Resize( m_saAdj.TotalFaces() + 2UL ) ) { return false; }
			if ( !vTempBestFaces.Resize( m_saAdj.TotalFaces() + 2UL ) ) { return false; }


			// Create strips.
			
			uint32_t ui32TotalFacesHandled = 0UL;
			uint32_t ui32Index = 0UL;
			while ( ui32TotalFacesHandled < m_saAdj.TotalFaces() ) {
				// Find the first non-handled face.
				while ( vTags[pui32Sorted[ui32Index]] ) { ++ui32Index; }

				uint32_t ui32Len;
				uint32_t ui32Faces = GetBestStrip( pui32Sorted[ui32Index], &vTempFaces[0], &vTempBestFaces[0], &vTempStrips[0], &vTempBestStrips[0],
					&vTempTags[0], &vTags[0], ui32Len );
				ui32TotalFacesHandled += ui32Faces;

				// Add it to the previous strip.
				if ( !ConnectStrip( vFinalStrip, &vTempBestStrips[0], ui32Len ) ) { return false; }
				++ui32Counter;
			}
		}

		if ( !m_vVerts.Resize( vFinalStrip.Length() * (m_bWords ? sizeof( uint16_t ) : sizeof( uint32_t )) ) ) {
			return false;
		}
		if ( m_bWords ) {
			for ( uint32_t I = vFinalStrip.Length(); I--; ) {
				reinterpret_cast<uint16_t *>(&m_vVerts[0])[I] = static_cast<uint16_t>(vFinalStrip[I]);
			}
		}
		else {
			for ( uint32_t I = vFinalStrip.Length(); I--; ) {
				reinterpret_cast<uint32_t *>(&m_vVerts[0])[I] = vFinalStrip[I];
			}
		}

		return true;
	}

	/**
	 * Gets the length of a triangle if extended from the given face in the given direction.
	 *
	 * \param _ui32Face Index of the face where to start checking.
	 * \param _ui32Oldest First index of the strip.
	 * \param _ui32Middle Second index of the strip.
	 * \param _pui32Strip Buffer in which to store the strip.
	 * \param _pui32Faces Buffer in which to store the faces.
	 * \param _pui8Tags Buffer to use to keep track of tagged faces.
	 * \return Returns the length of the strip.
	 */
	uint32_t LSE_CALL CTriangleStripper::GetStripLength( uint32_t _ui32Face, uint32_t _ui32Oldest, uint32_t _ui32Middle, uint32_t * _pui32Strip, uint32_t * _pui32Faces, uint8_t * _pui8Tags ) {
		uint32_t ui32Len = 2UL;			// Strips start with 2.
		_pui32Strip[0] = _ui32Oldest;	// Buffer assumed to be large enough for the largest possible strip.
		_pui32Strip[1] = _ui32Middle;	// Buffer assumed to be large enough for the largest possible strip.

		while ( true ) {
			uint32_t ui32Next = CStripAdjacency::OppositeVertex( m_saAdj.Faces()[_ui32Face], _ui32Oldest, _ui32Middle );
	
			// Add it to the strip.
			_pui32Strip[ui32Len++] = ui32Next;

			// Track the face.
			(*_pui32Faces++) = _ui32Face;
			_pui8Tags[_ui32Face] = true;

			uint32_t ui32CurEdge = CStripAdjacency::FindEdge( m_saAdj.Faces()[_ui32Face], _ui32Middle, ui32Next );
			uint32_t ui32Link = m_saAdj.Faces()[_ui32Face].ui32Tris[ui32CurEdge];
			if ( IS_BOUNDARY( ui32Link ) ) { break; }
			_ui32Face = MAKE_ADJ_TRI( ui32Link );
			// Did we already do this face?
			if ( _pui8Tags[_ui32Face] ) { break; }

			_ui32Oldest = _ui32Middle;
			_ui32Middle = ui32Next;
		}

		return ui32Len;
	}

	/**
	 * Determine the best strip starting at the given face.
	 *
	 * \param _ui32Face The face where to start searching.
	 * \param _pui32TempFaceBuffer Buffer to hold faces for checking strip lengths.
	 * \param _pui32BestFaceBuffer Best face buffer found.  Output.
	 * \param _pui32TempStripBuffer Buffer to hold strips for checking strip lengths.
	 * \param _pui32BestStripBuffer Best strip buffer found.  Output.
	 * \param _pui8TempTagsBuffer Buffer to hold tags for checking strip lengths.
	 * \param _pui8Tags The master copy of the tags buffer.  Updated with the longest strip's details.
	 * \param _ui32StripLen The length of the longest strip.  Output.
	 * \return Returns the number of faces in the strip.
	 */
	uint32_t LSE_CALL CTriangleStripper::GetBestStrip( uint32_t _ui32Face,
		uint32_t * _pui32TempFaceBuffer, uint32_t * _pui32BestFaceBuffer,
		uint32_t * _pui32TempStripBuffer, uint32_t * _pui32BestStripBuffer,
		uint8_t * _pui8TempTagsBuffer, uint8_t * _pui8Tags,
		uint32_t &_ui32StripLen ) {
		// Only the best strip is kept.
		uint32_t ui32BestLength = 0UL;
		uint32_t ui32FirstLength = 0UL;

		const uint32_t ui32Refs0[] = {
			m_saAdj.Faces()[_ui32Face].ui32Verts[0],
			m_saAdj.Faces()[_ui32Face].ui32Verts[2],
			m_saAdj.Faces()[_ui32Face].ui32Verts[1],
		};
		const uint32_t ui32Refs1[] = {
			m_saAdj.Faces()[_ui32Face].ui32Verts[1],
			m_saAdj.Faces()[_ui32Face].ui32Verts[0],
			m_saAdj.Faces()[_ui32Face].ui32Verts[2],
		};

		for ( uint32_t I = 0UL; I < 3UL; ++I ) {
			CStd::MemSet( _pui32TempFaceBuffer, 0xFF, (m_saAdj.TotalFaces() + 2UL) * sizeof( (*_pui32TempFaceBuffer) ) );
			CStd::MemSet( _pui32TempStripBuffer, 0xFF, (m_saAdj.TotalFaces() + 2UL + 1UL + 2UL) * sizeof( (*_pui32TempStripBuffer) ) );
			CStd::MemCpy( _pui8TempTagsBuffer, _pui8Tags, m_saAdj.TotalFaces() * sizeof( (*_pui8Tags) ) );

			uint32_t ui32ThisLen = GetStripLength( _ui32Face, ui32Refs0[I], ui32Refs1[I], _pui32TempStripBuffer, _pui32TempFaceBuffer, _pui8TempTagsBuffer );
			uint32_t ui32ThisFirstLen = ui32ThisLen;

			// Reverse the first part of the strip.
			for ( uint32_t J = 0UL; J < (ui32ThisLen >> 1); ++J ) {
				uint32_t ui32Temp = _pui32TempStripBuffer[ui32ThisLen-J-1];
				_pui32TempStripBuffer[ui32ThisLen-J-1] = _pui32TempStripBuffer[J];
				_pui32TempStripBuffer[J] = ui32Temp;
			}
			for ( uint32_t J = 0UL; J < ((ui32ThisLen - 2UL) >> 1); ++J ) {
				uint32_t ui32Temp = _pui32TempFaceBuffer[ui32ThisLen-J-3];
				_pui32TempFaceBuffer[ui32ThisLen-J-3] = _pui32TempFaceBuffer[J];
				_pui32TempFaceBuffer[J] = ui32Temp;
			}

			// Append another strip.
			uint32_t ui32NewRef0 = _pui32TempStripBuffer[ui32ThisLen-3];
			uint32_t ui32NewRef1 = _pui32TempStripBuffer[ui32ThisLen-2];
			uint32_t ui32ExtraLength = GetStripLength( _ui32Face, ui32NewRef0, ui32NewRef1, &_pui32TempStripBuffer[ui32ThisLen-3], &_pui32TempFaceBuffer[ui32ThisLen-3], _pui8TempTagsBuffer );
			ui32ThisLen += ui32ExtraLength - 3;

			// Keep it if it is the best yet.
			if ( ui32ThisLen > ui32BestLength ) {
				ui32BestLength = ui32ThisLen;
				ui32FirstLength = ui32ThisFirstLen;
				CStd::MemCpy( _pui32BestFaceBuffer, _pui32TempFaceBuffer, (ui32ThisLen - 2UL) * sizeof( (*_pui32TempFaceBuffer) ) );
				CStd::MemCpy( _pui32BestStripBuffer, _pui32TempStripBuffer, (ui32ThisLen) * sizeof( (*_pui32TempStripBuffer) ) );
			}
		}


		uint32_t ui32Faces = ui32BestLength - 2UL;
		for ( uint32_t I = ui32Faces; I--; ) {
			_pui8Tags[_pui32BestFaceBuffer[I]] = true;
		}

		

		// If the length of the first part of the strip is odd, it must be reversed.
		if ( ui32FirstLength & 1 ) {
			// Special case for triangles and quads.
			if ( ui32BestLength <= 4UL ) {
				uint32_t ui32Temp = _pui32BestStripBuffer[1];
				_pui32BestStripBuffer[1] = _pui32BestStripBuffer[2];
				_pui32BestStripBuffer[2] = ui32Temp;

			}
			else {
				// Reverse it by writing it in reverse order.
				for ( uint32_t I = 0UL; I < (ui32BestLength >> 1UL); ++I ) {
					uint32_t ui32Temp = _pui32BestStripBuffer[I];
					_pui32BestStripBuffer[I] = _pui32BestStripBuffer[ui32BestLength-I-1];
					_pui32BestStripBuffer[ui32BestLength-I-1] = ui32Temp;
				}


				// If the position of the original face in this new list is odd, we are done.
				if ( (ui32BestLength - ui32FirstLength) & 1 ) {
					// Otherwise, replicate the first vertex.
					for ( uint32_t I = ui32BestLength; I--; ) {
						_pui32BestStripBuffer[I+1UL] = _pui32BestStripBuffer[I];
					}
					++ui32BestLength;
				}
			}
		}

		_ui32StripLen = ui32BestLength;
		return ui32Faces;
	}

	/**
	 * Connect one strip to another.
	 *
	 * \param _vLeft The first strip to connect to the second.
	 * \param _pui32Right Second strip to connect.
	 * \param _ui32RightLen Length of the second strip to connect.
	 * \return Returns false if there was not enough memory to perform the operation.
	 */
	LSBOOL LSE_CALL CTriangleStripper::ConnectStrip( CVectorPoD<uint32_t, uint32_t, 1024UL> &_vLeft, uint32_t * _pui32Right, uint32_t _ui32RightLen ) {
		// If the left is empty, just copy into it.
		if ( _vLeft.Length() == 0UL ) {
			return _vLeft.Append( _pui32Right, _ui32RightLen );
		}


		// Going to append.  The list is always at least 2 points, so no check is necessary.
		uint32_t ui32Last = _vLeft[_vLeft.Length()-1UL];
		uint32_t ui32First = _pui32Right[0UL];
		if ( !_vLeft.Push( ui32Last ) ) { return false; }
		if ( !_vLeft.Push( ui32First ) ) { return false; }

		// If the base list's length is odd, culling will be inverted.
		if ( _vLeft.Length() & 1UL ) {
			// Two ways to fix it.  Either add the vertices in reverse order or, if _pui32Right begins with a duplicated vertex already, we
			//	can just remove that vertex.
			if ( _pui32Right[0] == _pui32Right[1] ) {
				// Duplicate vertex.  Just add the vertices after it.
				if ( !_vLeft.Append( &_pui32Right[1], _ui32RightLen - 1UL ) ) { return false; }
				return true;
			}
			if ( !_vLeft.Append( &ui32First, 1UL ) ) { return false; }
			
			// Add them in reverse.
			/*for ( uint32_t I = 0UL; I < (_ui32RightLen >> 1UL); ++I ) {
				uint32_t ui32Temp = _pui32Right[I];
				_pui32Right[I] = _pui32Right[_ui32RightLen-I-1];
				_pui32Right[_ui32RightLen-I-1] = ui32Temp;
			}*/
		}
		// Just add all of the vertices as they are.
		return _vLeft.Append( _pui32Right, _ui32RightLen );
	}

}	// namespace lsc

#pragma warning( pop )
