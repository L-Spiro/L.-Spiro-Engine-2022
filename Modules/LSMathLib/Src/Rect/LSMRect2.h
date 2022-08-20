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
 * Description: A 2D rectangle.  It is axis-aligned and technically works well as an AABB,
 *	but is not specialized in that direction.
 */


#ifndef __LSM_RECT2_H__
#define __LSM_RECT2_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector2.h"

namespace lsm {

	/**
	 * Class CRect2
	 *
	 * Description: A simple axis-aligned 2D rectanagle.
	 */
	class CRect2 {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CRect2();
		LSE_INLINE LSE_CALLCTOR				CRect2( LSREAL _fX0, LSREAL _fX1,
			LSREAL _fY0, LSREAL _fY1 );


		// == Functions.
		/**
		 * Returns the size of the box along the X world axis.
		 *
		 * \return Returns the size of the box along the X world axis.
		 */
		LSE_INLINE LSREAL LSE_FCALL			SizeX() const;

		/**
		 * Returns the size of the box along the Y world axis.
		 *
		 * \return Returns the size of the box along the Y world axis.
		 */
		LSE_INLINE LSREAL LSE_FCALL			SizeY() const;

		/**
		 * Returns the size of the box along both the X and Y world axes.
		 *
		 * \return Returns the size of the box along both the X and Y world axes.
		 */
		LSE_INLINE CVector2 LSE_FCALL		Size() const;

		/**
		 * Sets the rectangle to all 0's.
		 */
		LSE_INLINE void LSE_FCALL			Clear();

		/**
		 * Sets to infitely small negative size.
		 */
		LSE_INLINE void LSE_FCALL			NegativeBox();
		
		/**
		 * Resize to enclose the given point.
		 *
		 * \param _vPoint The point to enclose.
		 */
		LSE_INLINE void LSE_FCALL			EnclosePoint( const CVector2 &_vPoint );
		
		/**
		 * Resize to fit around another box, assuming *0 are mins and *1 are maxes.
		 *
		 * \param _rRect The box to enclose.
		 */
		LSE_INLINE void LSE_FCALL			EncloseBox( const CRect2 &_rRect );


		// == Members.
		/**
		 * The X extents.
		 */
		LSREAL								x0, x1;

		/**
		 * The Y extents.
		 */
		LSREAL								y0, y1;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CRect2::CRect2() {
	}
	LSE_INLINE LSE_CALLCTOR CRect2::CRect2( LSREAL _fX0, LSREAL _fX1,
		LSREAL _fY0, LSREAL _fY1 ) {
		x0 = _fX0, x1 = _fX1;
		y0 = _fY0, y1 = _fY1;
	}


	// == Functions.
	/**
	 * Returns the size of the box along the X world axis.
	 *
	 * \return Returns the size of the box along the X world axis.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRect2::SizeX() const		{ return x1 - x0; }

	/**
	 * Returns the size of the box along the Y world axis.
	 *
	 * \return Returns the size of the box along the Y world axis.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRect2::SizeY() const		{ return y1 - y0; }

	/**
	 * Returns the size of the box along both the X and Y world axes.
	 *
	 * \return Returns the size of the box along both the X and Y world axes.
	 */
	LSE_INLINE CVector2 LSE_FCALL CRect2::Size() const		{ return CVector2( SizeX(), SizeY() ); }

	/**
	 * Sets the rectangle to all 0's.
	 */
	LSE_INLINE void LSE_FCALL CRect2::Clear() {
		x0 = x1 = y0 = y1 = LSM_ZERO;
	}

	/**
	 * Sets to infitely small negative size.
	 */
	LSE_INLINE void LSE_FCALL CRect2::NegativeBox() {
		x0 = y0 = LSM_INFINITY;
		x1 = y1 = -LSM_INFINITY;
	}

	/**
	 * Resize to enclose the given point.
	 *
	 * \param _vPoint The point to enclose.
	 */
	LSE_INLINE void LSE_FCALL CRect2::EnclosePoint( const CVector2 &_vPoint ) {
		x0 = CMathLib::Min( x0, _vPoint.x );
		x1 = CMathLib::Max( x1, _vPoint.x );
		y0 = CMathLib::Min( y0, _vPoint.y );
		y1 = CMathLib::Max( y1, _vPoint.y );
	}

	/**
	 * Resize to fit around another box, assuming *0 are mins and *1 are maxes.
	 *
	 * \param _rRect The box to enclose.
	 */
	LSE_INLINE void LSE_FCALL CRect2::EncloseBox( const CRect2 &_rRect ) {
		x0 = CMathLib::Min( x0, _rRect.x0 );
		x1 = CMathLib::Max( x1, _rRect.x1 );
		y0 = CMathLib::Min( y0, _rRect.y0 );
		y1 = CMathLib::Max( y1, _rRect.y1 );
	}

}	// namespace lsm

#endif	// __LSM_RECT2_H__
