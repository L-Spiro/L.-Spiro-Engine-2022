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
 * Description: A 3D rectangle.  It is axis-aligned and technically works well as an AABB,
 *	but is not specialized in that direction.
 */


#ifndef __LSM_RECT3_H__
#define __LSM_RECT3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMRect2.h"

namespace lsm {

	/**
	 * Class CRect3
	 *
	 * Description: A simple axis-aligned 3D rectanagle.
	 */
	class CRect3 {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR				CRect3();
		LSE_INLINE LSE_CALLCTOR				CRect3( LSREAL _fX0, LSREAL _fX1,
			LSREAL _fY0, LSREAL _fY1,
			LSREAL _fZ0, LSREAL _fZ1 );


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
		 * Returns the size of the box along the Z world axis.
		 *
		 * \return Returns the size of the box along the Z world axis.
		 */
		LSE_INLINE LSREAL LSE_FCALL			SizeZ() const;

		/**
		 * Returns the size of the box along all world axes.
		 *
		 * \return Returns the size of the box along all world axes.
		 */
		LSE_INLINE CVector3 LSE_FCALL		Size() const;

		/**
		 * Sets all axes to 0.
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
		LSE_INLINE void LSE_FCALL			EnclosePoint( const CVector3 &_vPoint );

		/**
		 * Resize to fit around another box, assuming *0 are mins and *1 are maxes.
		 *
		 * \param _rRect The rectangle to enclose.
		 */
		LSE_INLINE void LSE_FCALL			EncloseBox( const CRect3 &_rRect );

		/**
		 * Sets the x and y values to those given by the 2D rectangle.
		 *
		 * \param _rRect The 2D rectangle whose X and Y components are to be copied to this rectangle.
		 */
		LSE_INLINE void LSE_FCALL			Set2D( const CRect2 &_rRect );


		// == Members.
		/**
		 * The X-axis extents.
		 */
		LSREAL								x0, x1;

		/**
		 * The Y-axis extents.
		 */
		LSREAL								y0, y1;

		/**
		 * The Z-axis extents.
		 */
		LSREAL								z0, z1;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CRect3::CRect3() {
	}
	LSE_INLINE LSE_CALLCTOR CRect3::CRect3( LSREAL _fX0, LSREAL _fX1,
		LSREAL _fY0, LSREAL _fY1,
		LSREAL _fZ0, LSREAL _fZ1 ) :
		x0( _fX0 ), x1( _fX1 ),
		y0( _fY0 ), y1( _fY1 ),
		z0( _fZ0 ), z1( _fZ1 ) {
	}


	// == Functions.
	/**
	 * Returns the size of the box along the X world axis.
	 *
	 * \return Returns the size of the box along the X world axis.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRect3::SizeX() const		{ return x1 - x0; }

	/**
	 * Returns the size of the box along the Y world axis.
	 *
	 * \return Returns the size of the box along the Y world axis.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRect3::SizeY() const		{ return y1 - y0; }

	/**
	 * Returns the size of the box along the Z world axis.
	 *
	 * \return Returns the size of the box along the Z world axis.
	 */
	LSE_INLINE LSREAL LSE_FCALL CRect3::SizeZ() const		{ return z1 - z0; }

	/**
	 * Returns the size of the box along all world axes.
	 *
	 * \return Returns the size of the box along all world axes.
	 */
	LSE_INLINE CVector3 LSE_FCALL CRect3::Size() const		{ return CVector3( SizeX(), SizeY(), SizeZ() ); }

	/**
	 * Sets all axes to 0.
	 */
	LSE_INLINE void LSE_FCALL CRect3::Clear() {
		x0 = x1 = y0 = y1 = z0 = z1 = LSM_ZERO;
	}

	/**
	 * Sets to infitely small negative size.
	 */
	LSE_INLINE void LSE_FCALL CRect3::NegativeBox() {
		x0 = y0 = z0 = LSM_INFINITY;
		x1 = y1 = z1 = -LSM_INFINITY;
	}

	/**
	 * Resize to enclose the given point.
	 *
	 * \param _vPoint The point to enclose.
	 */
	LSE_INLINE void LSE_FCALL CRect3::EnclosePoint( const CVector3 &_vPoint ) {
		x0 = CMathLib::Min( x0, _vPoint.x );
		x1 = CMathLib::Max( x1, _vPoint.x );
		y0 = CMathLib::Min( y0, _vPoint.y );
		y1 = CMathLib::Max( y1, _vPoint.y );
		z0 = CMathLib::Min( z0, _vPoint.z );
		z1 = CMathLib::Max( z1, _vPoint.z );
	}

	/**
	 * Resize to fit around another box, assuming *0 are mins and *1 are maxes.
	 *
	 * \param _rRect The rectangle to enclose.
	 */
	LSE_INLINE void LSE_FCALL CRect3::EncloseBox( const CRect3 &_rRect ) {
		x0 = CMathLib::Min( x0, _rRect.x0 );
		x1 = CMathLib::Max( x1, _rRect.x1 );
		y0 = CMathLib::Min( y0, _rRect.y0 );
		y1 = CMathLib::Max( y1, _rRect.y1 );
		z0 = CMathLib::Min( z0, _rRect.z0 );
		z1 = CMathLib::Max( z1, _rRect.z1 );
	}

	/**
	 * Sets the x and y values to those given by the 2D rectangle.
	 *
	 * \param _rRect The 2D rectangle whose X and Y components are to be copied to this rectangle.
	 */
	LSE_INLINE void LSE_FCALL CRect3::Set2D( const CRect2 &_rRect ) {
		x0 = _rRect.x0;
		x1 = _rRect.x1;
		y0 = _rRect.y0;
		y1 = _rRect.y1;
	}

}	// namespace lsm

#endif	// __LSM_RECT3_H__
