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
 * Description: A 3D triangle with no additional data stored as indices into a vertex list.  The triangle must
 *	always be used in conjunction with a vertex list.  No pointer to a vertex list is stored here.
 */


#ifndef __LSM_TRIANGLE3I_H__
#define __LSM_TRIANGLE3I_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CTriangle3I
	 *
	 * Description: 3 indices into a (not-included) vertex list.
	 */
	class CTriangle3I {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						CTriangle3I();
		LSE_INLINE LSE_CALLCTOR						CTriangle3I( const CTriangle3I &_t3iTri );
		LSE_INLINE LSE_CALLCTOR						CTriangle3I( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32C );


		// == Operators.
		/**
		 * Give array access.
		 *
		 * \param _ui32Index Index of the point to access.
		 * \return Returns the point at the given index.
		 */
		LSE_INLINE uint32_t LSE_FCALL				operator [] ( uint32_t _ui32Index ) const;

		/**
		 * Give array access.
		 *
		 * \param _ui32Index Index of the point to access.
		 * \return Returns the point at the given index.
		 */
		LSE_INLINE uint32_t & LSE_FCALL				operator [] ( uint32_t _ui32Index );

		/**
		 * Equality check.
		 *
		 * \param _t3iTri Object against which to check for equality.
		 * \return Returns true if both triangles are the same in terms of indices.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator == ( const CTriangle3I &_t3iTri ) const;

		/**
		 * Inequality check.
		 *
		 * \param _t3iTri Object against which to check for inequality.
		 * \return Returns true if both triangles are not the same in terms of indices.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator != ( const CTriangle3I &_t3iTri ) const;


		// == Functions.
		/**
		 * Reverse the order of the triangle.
		 */
		LSE_INLINE void LSE_FCALL					ReverseOrder();

		// == Members.
#pragma pack( push, 1 )
		/**
		 * The points, in indices, that compose this triangle.
		 */
		uint32_t									a, b, c;
#pragma pack( pop )
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CTriangle3I::CTriangle3I() {
	}
	LSE_INLINE LSE_CALLCTOR CTriangle3I::CTriangle3I( const CTriangle3I &_t3iTri ) {
		a = _t3iTri.a;
		b = _t3iTri.b;
		c = _t3iTri.c;
	}
	LSE_INLINE LSE_CALLCTOR CTriangle3I::CTriangle3I( uint32_t _ui32A, uint32_t _ui32B, uint32_t _ui32C ) {
		a = _ui32A;
		b = _ui32B;
		c = _ui32C;
	}

	// == Operators.
	/**
	 * Give array access.
	 *
	 * \param _ui32Index Index of the point to access.
	 * \return Returns the point at the given index.
	 */
	LSE_INLINE uint32_t LSE_FCALL CTriangle3I::operator [] ( uint32_t _ui32Index ) const { return reinterpret_cast<const uint32_t *>(this)[_ui32Index]; }

	/**
	 * Give array access.
	 *
	 * \param _ui32Index Index of the point to access.
	 * \return Returns the point at the given index.
	 */
	LSE_INLINE uint32_t & LSE_FCALL CTriangle3I::operator [] ( uint32_t _ui32Index ) { return reinterpret_cast<uint32_t *>(this)[_ui32Index]; }

	/**
	 * Equality check.
	 *
	 * \param _t3iTri Object against which to check for equality.
	 * \return Returns true if both triangles are the same in terms of indices.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3I::operator == ( const CTriangle3I &_t3iTri ) const {
		return a == _t3iTri.a && b == _t3iTri.b && c == _t3iTri.c;
	}

	/**
	 * Inequality check.
	 *
	 * \param _t3iTri Object against which to check for inequality.
	 * \return Returns true if both triangles are not the same in terms of indices.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3I::operator != ( const CTriangle3I &_t3iTri ) const {
		return a != _t3iTri.a || b != _t3iTri.b || c != _t3iTri.c;
	}

	// == Functions.
	/**
	 * Reverse the order of the triangle.
	 */
	LSE_INLINE void LSE_FCALL CTriangle3I::ReverseOrder() {
		CMathLib::Swap( a, c );
	}

}	// namespace lsm

#endif	// __LSM_TRIANGLE3I_H__
