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
 * Description: An extended 3D triangle with precomputed barycentric weights and a normal.
 */


#ifndef __LSM_TRIANGLE3EX_H__
#define __LSM_TRIANGLE3EX_H__

#include "../LSMMathLib.h"
#include "LSMTriangle3.h"

namespace lsm {

	/**
	 * Class CTriangle3Ex
	 *
	 * Description: Extending a basic triangle to give us a triangle with precomputed
	 *	values of various kinds.  These should be used to represent static non-changing
	 *	triangles since updating the values is costly.
	 */
	class CTriangle3Ex : protected CTriangle3 {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						CTriangle3Ex();
		LSE_INLINE LSE_CALLCTOR						CTriangle3Ex( const CVector3 _vArray[3] );
		LSE_INLINE LSE_CALLCTOR						CTriangle3Ex( const CTriangle3 &_tTri );
		LSE_INLINE LSE_CALLCTOR						CTriangle3Ex( const CTriangle3Ex &_tTri );


		// == Operators.
		/**
		 * Give array access.
		 *
		 * \param _ui32Index Index of the point to get.
		 * \return Returns the point at the given index.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		operator [] ( uint32_t _ui32Index ) const;

		/**
		 * Equality operator.
		 *
		 * \param _tTri Triangle against which to check for equality.
		 * \return Returns true if both triangles are equal.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator == ( const CTriangle3 &_tTri ) const;

		/**
		 * Inequality operator.
		 *
		 * \param _tTri Triangle against which to check for inequality.
		 * \return Returns true if both triangles are not equal.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator != ( const CTriangle3 &_tTri ) const;

		/**
		 * Equality operator.
		 *
		 * \param _teTri Triangle against which to check for equality.
		 * \return Returns true if both triangles are equal.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator == ( const CTriangle3Ex &_teTri ) const;

		/**
		 * Inequality operator.
		 *
		 * \param _teTri Triangle against which to check for inequality.
		 * \return Returns true if both triangles are not equal.
		 */
		LSE_INLINE LSBOOL LSE_FCALL					operator != ( const CTriangle3Ex &_tTri ) const;


		// == Functions.
		/**
		 * Gets point A.
		 *
		 * \return Returns a read-only reference to point A.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		A() const;

		/**
		 * Gets point B.
		 *
		 * \return Returns a read-only reference to point B.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		B() const;

		/**
		 * Gets point C.
		 *
		 * \return Returns a read-only reference to point C.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		C() const;

		/**
		 * Gets the normal.
		 *
		 * \return Returns a read-only reference to the normal.
		 */
		LSE_INLINE const CVector3 & LSE_FCALL		N() const;

		/**
		 * Gets the barycentric term D00.
		 *
		 * \return Returns a read-only reference to the barycentric term D00.
		 */
		LSE_INLINE const LSREAL LSE_FCALL			D00() const;

		/**
		 * Gets the barycentric term D01.
		 *
		 * \return Returns a read-only reference to the barycentric term D01.
		 */
		LSE_INLINE const LSREAL LSE_FCALL			D01() const;

		/**
		 * Gets the barycentric term D11.
		 *
		 * \return Returns a read-only reference to the barycentric term D11.
		 */
		LSE_INLINE const LSREAL LSE_FCALL			D11() const;

		/**
		 * Gets the denominator.
		 *
		 * \return Returns a read-only reference to the denominator.
		 */
		LSE_INLINE const LSREAL LSE_FCALL			Denom() const;

		/**
		 * Reverse the order of the triangle.
		 */
		LSE_INLINE void LSE_FCALL					ReverseOrder();

		/**
		 * Gets the barycentric coordinates of a given point.
		 *
		 * \param _vPoint The point whose barycentric weights are to be obtained.
		 * \param _fU Barycentric U coordinate.
		 * \param _fV Barycentric V coordinate.
		 * \param _fW Barycentric W coordinate.
		 */
		void LSE_FCALL							Barycentric( const CVector3 &_vPoint,
			LSREAL &_fU, LSREAL &_fV, LSREAL &_fW ) const;


	protected :
		// == Members.
		/** The normal. */
		CVector3									m_vNormal;
		
		/** Barycentric term. */
		LSREAL										m_fD00;

		/** Barycentric term. */
		LSREAL										m_fD01;

		/** Barycentric term. */
		LSREAL										m_fD11;

		/** Barycentric term. */
		LSREAL										m_fDenom;


		// == Functions.
		/**
		 * Update the data.
		 */
		void LSE_FCALL							Update();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CTriangle3Ex::CTriangle3Ex() {
	}
	LSE_INLINE LSE_CALLCTOR CTriangle3Ex::CTriangle3Ex( const CVector3 _vArray[3] ) {
		a = _vArray[0];
		b = _vArray[1];
		c = _vArray[2];
		Update();
	}
	LSE_INLINE LSE_CALLCTOR CTriangle3Ex::CTriangle3Ex( const CTriangle3 &_tTri ) {
		a = _tTri.a;
		b = _tTri.b;
		c = _tTri.c;
		Update();
	}
	LSE_INLINE LSE_CALLCTOR CTriangle3Ex::CTriangle3Ex( const CTriangle3Ex &_tTri ) {
		a = _tTri.a;
		b = _tTri.b;
		c = _tTri.c;
		m_vNormal = _tTri.m_vNormal;
		m_fD00 = _tTri.m_fD00;
		m_fD01 = _tTri.m_fD01;
		m_fD11 = _tTri.m_fD11;
		m_fDenom = _tTri.m_fDenom;
	}


	// == Operators.
	/**
	 * Give array access.
	 *
	 * \param _ui32Index Index of the point to get.
	 * \return Returns the point at the given index.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL CTriangle3Ex::operator [] ( uint32_t _ui32Index ) const { return reinterpret_cast<const CVector3 *>(this)[_ui32Index]; }

	/**
	 * Equality operator.
	 *
	 * \param _tTri Triangle against which to check for equality.
	 * \return Returns true if both triangles are equal.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3Ex::operator == ( const CTriangle3 &_tTri ) const {
		return a == _tTri.a && b == _tTri.b && c == _tTri.c;
	}

	/**
	 * Inequality operator.
	 *
	 * \param _tTri Triangle against which to check for inequality.
	 * \return Returns true if both triangles are not equal.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3Ex::operator != ( const CTriangle3 &_tTri ) const {
		return a != _tTri.a || b != _tTri.b || c != _tTri.c;
	}

	/**
	 * Equality operator.
	 *
	 * \param _teTri Triangle against which to check for equality.
	 * \return Returns true if both triangles are equal.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3Ex::operator == ( const CTriangle3Ex &_teTri ) const {
		return a == _teTri.a && b == _teTri.b && c == _teTri.c;
	}

	/**
	 * Inequality operator.
	 *
	 * \param _teTri Triangle against which to check for inequality.
	 * \return Returns true if both triangles are not equal.
	 */
	LSE_INLINE LSBOOL LSE_FCALL CTriangle3Ex::operator != ( const CTriangle3Ex &_teTri ) const {
		return a != _teTri.a || b != _teTri.b || c != _teTri.c;
	}

	// == Functions.
	/**
	 * Gets point A.
	 *
	 * \return Returns a read-only reference to point A.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL CTriangle3Ex::A() const		{ return a; }

	/**
	 * Gets point B.
	 *
	 * \return Returns a read-only reference to point B.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL CTriangle3Ex::B() const		{ return b; }

	/**
	 * Gets point C.
	 *
	 * \return Returns a read-only reference to point C.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL CTriangle3Ex::C() const		{ return c; }

	/**
	 * Gets the normal.
	 *
	 * \return Returns a read-only reference to the normal.
	 */
	LSE_INLINE const CVector3 & LSE_FCALL CTriangle3Ex::N() const		{ return m_vNormal; }

	/**
	 * Gets the barycentric term D00.
	 *
	 * \return Returns a read-only reference to the barycentric term D00.
	 */
	LSE_INLINE const LSREAL LSE_FCALL CTriangle3Ex::D00() const			{ return m_fD00; }

	/**
	 * Gets the barycentric term D01.
	 *
	 * \return Returns a read-only reference to the barycentric term D01.
	 */
	LSE_INLINE const LSREAL LSE_FCALL CTriangle3Ex::D01() const			{ return m_fD01; }

	/**
	 * Gets the barycentric term D11.
	 *
	 * \return Returns a read-only reference to the barycentric term D1.
	 */
	LSE_INLINE const LSREAL LSE_FCALL CTriangle3Ex::D11() const			{ return m_fD11; }

	/**
	 * Gets the denominator.
	 *
	 * \return Returns a read-only reference to the denominator.
	 */
	LSE_INLINE const LSREAL LSE_FCALL CTriangle3Ex::Denom() const		{ return m_fDenom; }

	/**
	 * Reverse the order of the triangle.
	 */
	LSE_INLINE void LSE_FCALL CTriangle3Ex::ReverseOrder()			{ CMathLib::Swap( a, c ); }



}	// namespace lsm

#endif	// __LSM_TRIANGLE3EX_H__
