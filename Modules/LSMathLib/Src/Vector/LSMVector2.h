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
 * Description: A standard 2D vector of type LSREAL.
 */


#ifndef __LSM_VECTOR2_H__
#define __LSM_VECTOR2_H__

#include "../LSMMathLib.h"
#include "LSMVector2Base.h"

namespace lsm {

	/**
	 * Class CVector2
	 * \brief A standard 2D vector of type LSREAL.
	 *
	 * Description: A standard 2D vector of type LSREAL.
	 */
	class CVector2 : public CVector2Base<LSREAL, CVector2> {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CVector2();
		LSE_INLINE LSE_CALLCTOR							CVector2( LSREAL _fX, LSREAL _fY );
		LSE_INLINE LSE_CALLCTOR							CVector2( const CVector2 &_vOther );
		LSE_INLINE LSE_CALLCTOR							CVector2( LSREAL * _pfValues );


		// == Functions.
		/**
		 * Gets the read-only pointer to the 25-element array of Poisson points.
		 *
		 * \return Returns the read-only pointer to the 25-element array of Poisson points.
		 */
		static LSE_INLINE const CVector2 *				Poisson25();

		/**
		 * Gets the read-only pointer to the 32-element array of Poisson points.
		 *
		 * \return Returns the read-only pointer to the 32-element array of Poisson points.
		 */
		static LSE_INLINE const CVector2 *				Poisson32();

		/**
		 * Gets the read-only pointer to the 64-element array of Poisson points.
		 *
		 * \return Returns the read-only pointer to the 64-element array of Poisson points.
		 */
		static LSE_INLINE const CVector2 *				Poisson64();


	protected :
		// == Members.
		/**
		 * 25-point Poisson filter from -1 to 1 on both axes.
		 */
		static const CVector2							m_vPoisson25[25];

		/**
		 * 32-point Poisson filter from -1 to 1 on both axes.
		 */
		static const CVector2							m_vPoisson32[32];

		/**
		 * 64-point Poisson filter from -1 to 1 on both axes.
		 */
		static const CVector2							m_vPoisson64[64];


	private :
		typedef CVector2Base<LSREAL, CVector2>			Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CVector2::CVector2() {
	}
	LSE_INLINE LSE_CALLCTOR CVector2::CVector2( LSREAL _fX, LSREAL _fY ) :
		Parent( _fX, _fY ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector2::CVector2( const CVector2 &_vOther ) :
		Parent( _vOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector2::CVector2( LSREAL * _pfValues ) :
		Parent( _pfValues ) {
	}

	// == Functions.
	/**
	 * Gets the read-only pointer to the 25-element array of Poisson points.
	 *
	 * \return Returns the read-only pointer to the 25-element array of Poisson points.
	 */
	LSE_INLINE const CVector2 * CVector2::Poisson25() { return m_vPoisson25; }

	/**
	 * Gets the read-only pointer to the 32-element array of Poisson points.
	 *
	 * \return Returns the read-only pointer to the 32-element array of Poisson points.
	 */
	LSE_INLINE const CVector2 * CVector2::Poisson32() { return m_vPoisson32; }

	/**
	 * Gets the read-only pointer to the 64-element array of Poisson points.
	 *
	 * \return Returns the read-only pointer to the 64-element array of Poisson points.
	 */
	LSE_INLINE const CVector2 * CVector2::Poisson64() { return m_vPoisson64; }

}	// namespace lsm

#endif	// __LSM_VECTOR2_H__

