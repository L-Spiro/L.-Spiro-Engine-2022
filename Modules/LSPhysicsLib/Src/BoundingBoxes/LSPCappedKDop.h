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
 * Description: A capped k-DOP is a k-DOP with a maximum number of planes.  This stores enough planes to hold the
 *	maximum, so allocations are avoided, making this the choice for a limited number of planes in high-performance
 *	situations.
 */


#ifndef __LSP_CAPPEDKDOP_H__
#define __LSP_CAPPEDKDOP_H__

#include "../LSPPhysicsLib.h"
#include "Plane/LSMPlane3.h"
#include <cassert>

namespace lsp {

	/**
	 * Class CCappedKDop
	 * \brief A k-DOP with a maximum number of planes when performance is needed.
	 *
	 * Description: A capped k-DOP is a k-DOP with a maximum number of planes.  This stores enough planes to hold the
	 *	maximum, so allocations are avoided, making this the choice for a limited number of planes in high-performance
	 *	situations.
	 */
	template <unsigned _uMax>
	class CCappedKDop {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR					CCappedKDop() :
			m_ui32Total( 0 ) {
		}


		// == Operators.
		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the plane to retrieve.
		 * \return Returns the plane at the given index, which must be valid.
		 */
		LSE_INLINE CPlane3 & LSE_CALL			operator [] ( uint32_t _ui32I ) {
			assert( _ui32I < m_ui32Total );
			return m_pPlanes[_ui32I];
		}

		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the plane to retrieve.
		 * \return Returns the plane at the given index, which must be valid.
		 */
		LSE_INLINE const CPlane3 & LSE_CALL		operator [] ( uint32_t _ui32I ) const {
			assert( _ui32I < m_ui32Total );
			return m_pPlanes[_ui32I];
		}


		// == Functions.
		/**
		 * Gets the total number of planes in this k-DOP.
		 *
		 * \return Returns the number of planes in the k_DOP.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalPlanes() const {
			return m_ui32Total;
		}

		/**
		 * Adds a plane.
		 *
		 * \param _pPlane The plane to add.
		 * \return Returns false if a memory failure prevents the adding of the plane.
		 */
		LSE_INLINE LSBOOL LSE_CALL				AddPlane( const CPlane3 &_pPlane ) {
			if ( m_ui32Total < _uMax ) {
				m_pPlanes[m_ui32Total++] = _pPlane;
				return true;
			}
			return false;
		}

		/**
		 * Reset the list of planes.  Very fast.
		 */
		LSE_INLINE void LSE_CALL				Reset() {
			m_ui32Total = 0;
		}


	protected :
		// == Members.
		/**
		 * The array of planes.
		 */
		CPlane3									m_pPlanes[_uMax];

		/**
		 * The actual total of planes.
		 */
		uint32_t								m_ui32Total;
	};

}	// namespace lsp

#endif	// __LSP_CAPPEDKDOP_H__
