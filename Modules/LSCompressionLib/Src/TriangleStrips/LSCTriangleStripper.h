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


#ifndef __LSC_TRIANGLESTRIPPER_H__
#define __LSC_TRIANGLESTRIPPER_H__

#include "../LSCCompressionLib.h"
#include "LSCStripAdjacency.h"
#include "Vector/LSTLVectorPoD.h"

namespace lsc {

	/**
	 * Class CTriangleStripper
	 * \brief Creates a single one-sided triangle strip.
	 *
	 * Description: Creates a single one-sided triangle strip.  Code based on Pierre Terdiman's original work.
	 */
	class CTriangleStripper {
	public :
		// == Various constructors.
		LSE_CALLCTOR					CTriangleStripper();
		LSE_CALLCTOR					~CTriangleStripper();


		// == Functions.
		/**
		 * Initialize the triangle stripper.
		 *
		 * \param _ui32Faces Number of triangles.
		 * \param _pui32Faces Pointer to the triangles with 32-bit indices or NULL.
		 * \param _pui16Faces Pointer to the triangles with 16-bit indices or NULL.
		 * \return Returns false if there are not enough resources to complete the setup.
		 */
		LSBOOL LSE_CALL					InitTriangleStripper( uint32_t _ui32Faces = 0UL, const uint32_t * _pui32Faces = NULL, const uint16_t * _pui16Faces = NULL );

		/**
		 * Create the triangle strip.  After a successful call to this function, the created triangle strip can
		 *	be read directly.
		 *
		 * \return Returns true if there were no memory failures during the operation.
		 */
		LSBOOL LSE_CALL					Compute();

		/**
		 * Gets the total indices.
		 *
		 * \return Returns the total indices.
		 */
		LSE_INLINE uint32_t LSE_CALL	TotalIndices() const;

		/**
		 * Gets the indices as a word size.
		 *
		 * \return Returns the indices as a word size.
		 */
		LSE_INLINE const uint16_t * LSE_CALL
										IndicesAsWords() const;

		/**
		 * Gets the indices as a double-word size.
		 *
		 * \return Returns the indices as a double-word size.
		 */
		LSE_INLINE const uint32_t * LSE_CALL
										IndicesAsDoubleWords() const;

	protected :
		// == Members.
		/** The adjacency information. */
		CStripAdjacency					m_saAdj;

		/** The final vertices. */
		CVectorPoD<uint8_t, uint32_t>	m_vVerts;

		/** Use words? */
		LSBOOL							m_bWords;


		// == Functions.
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
		uint32_t LSE_CALL				GetStripLength( uint32_t _ui32Face, uint32_t _ui32Oldest, uint32_t _ui32Middle, uint32_t * _pui32Strip, uint32_t * _pui32Faces, uint8_t * _pui8Tags );

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
		uint32_t LSE_CALL				GetBestStrip( uint32_t _ui32Face,
			uint32_t * _pui32TempFaceBuffer, uint32_t * _pui32BestFaceBuffer,
			uint32_t * _pui32TempStripBuffer, uint32_t * _pui32BestStripBuffer,
			uint8_t * _pui8TempTagsBuffer, uint8_t * _pui8Tags,
			uint32_t &_ui32StripLen );

		/**
		 * Connect one strip to another.
		 *
		 * \param _vLeft The first strip to connect to the second.
		 * \param _pui32Right Second strip to connect.
		 * \param _ui32RightLen Length of the second strip to connect.
		 * \return Returns false if there was not enough memory to perform the operation.
		 */
		LSBOOL LSE_CALL					ConnectStrip( CVectorPoD<uint32_t, uint32_t, 1024UL> &_vLeft, uint32_t * _pui32Right, uint32_t _ui32RightLen );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the total indices.
	 *
	 * \return Returns the total indices.
	 */
	LSE_INLINE uint32_t LSE_CALL CTriangleStripper::TotalIndices() const {
		if ( m_bWords ) { return m_vVerts.Length() / sizeof( uint16_t ); }
		return m_vVerts.Length() / sizeof( uint32_t );
	}

	/**
	 * Gets the indices as a word size.
	 *
	 * \return Returns the indices as a word size.
	 */
	LSE_INLINE const uint16_t * LSE_CALL CTriangleStripper::IndicesAsWords() const {
		return reinterpret_cast<const uint16_t *>(&m_vVerts[0]);
	}

	/**
	 * Gets the indices as a double-word size.
	 *
	 * \return Returns the indices as a double-word size.
	 */
	LSE_INLINE const uint32_t * LSE_CALL CTriangleStripper::IndicesAsDoubleWords() const {
		return reinterpret_cast<const uint32_t *>(&m_vVerts[0]);
	}

}	// namespace lsc

#endif	// __LSC_TRIANGLESTRIPPER_H__
