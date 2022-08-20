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
 * Description: A k-DOP is a bounding box of any number of planes.
 */


#ifndef __LSP_KDOP_H__
#define __LSP_KDOP_H__

#include "../LSPPhysicsLib.h"
#include "Plane/LSMPlane3.h"
#include "Vector/LSTLSVectorPoD.h"

namespace lsp {

	/**
	 * Class CKDop
	 * \brief A k-DOP is a bounding box of any number of planes.
	 *
	 * Description: A k-DOP is a bounding box of any number of planes.
	 */
	class CKDop {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR					CKDop();
		LSE_INLINE LSE_CALLCTOR					CKDop( uint32_t _ui32AllocPlanes );


		// == Operators.
		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the plane to retrieve.
		 * \return Returns the plane at the given index, which must be valid.
		 */
		LSE_INLINE CPlane3 & LSE_CALL			operator [] ( uint32_t _ui32I );

		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the plane to retrieve.
		 * \return Returns the plane at the given index, which must be valid.
		 */
		LSE_INLINE const CPlane3 & LSE_CALL		operator [] ( uint32_t _ui32I ) const;

		
		// == Functions.
		/**
		 * Gets the total number of planes in this k-DOP.
		 *
		 * \return Returns the number of planes in the k-DOP.
		 */
		LSE_INLINE uint32_t LSE_CALL			TotalPlanes() const;

		/**
		 * Adds a plane.
		 *
		 * \param _pPlane The plane to add.
		 * \return Returns false if a memory failure prevents the adding of the plane.
		 */
		LSE_INLINE LSBOOL LSE_CALL				AddPlane( const CPlane3 &_pPlane );

		/**
		 * Reset the list of planes.  Deallocates all memory used by this class.
		 */
		LSE_INLINE void LSE_CALL				Reset();

		/**
		 * Reset the list of planes.  Does not perform deallocation.
		 */
		LSE_INLINE void LSE_CALL				ResetNoDealloc();


	protected :
		// == Members.
		/**
		 * The dynamically growable array of planes.
		 */
		CSVectorPoD<CPlane3>					m_pPlanes;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_CALLCTOR CKDop::CKDop() {
	}
	LSE_INLINE LSE_CALLCTOR CKDop::CKDop( uint32_t _ui32AllocPlanes ) :
		m_pPlanes( _ui32AllocPlanes ) {
	}

	// == Operators.
	/**
	 * Give array access.
	 *
	 * \param _ui32I The index of the plane to retrieve.
	 * \return Returns the plane at the given index, which must be valid.
	 */
	LSE_INLINE CPlane3 & LSE_CALL CKDop::operator [] ( uint32_t _ui32I ) {
		return m_pPlanes[_ui32I];
	}
	
	/**
	 * Give array access.
	 *
	 * \param _ui32I The index of the plane to retrieve.
	 * \return Returns the plane at the given index, which must be valid.
	 */
	LSE_INLINE const CPlane3 & LSE_CALL CKDop::operator [] ( uint32_t _ui32I ) const {
		return m_pPlanes[_ui32I];
	}

	// == Functions.
	/**
	 * Gets the total number of planes in this k-DOP.
	 *
	 * \return Returns the number of planes in the k-DOP.
	 */
	LSE_INLINE uint32_t LSE_CALL CKDop::TotalPlanes() const {
		return m_pPlanes.Length();
	}

	/**
	 * Adds a plane.
	 *
	 * \param _pPlane The plane to add.
	 * \return Returns false if a memory failure prevents the adding of the plane.
	 */
	LSE_INLINE LSBOOL LSE_CALL CKDop::AddPlane( const CPlane3 &_pPlane ) {
		return m_pPlanes.Push( _pPlane );
	}

	/**
	 * Reset the list of planes.  Deallocates all memory used by this class.
	 */
	LSE_INLINE void LSE_CALL CKDop::Reset() {
		m_pPlanes.Reset();
	}

	/**
	 * Reset the list of planes.  Does not perform deallocation.
	 */
	LSE_INLINE void LSE_CALL CKDop::ResetNoDealloc() {
		m_pPlanes.ResetNoDealloc();
	}

}	// namespace lsp

#endif	// __LSP_KDOP_H__
