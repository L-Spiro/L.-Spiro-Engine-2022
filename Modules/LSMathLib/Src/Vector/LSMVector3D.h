/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A standard 3D vector of type double.
 */


#ifndef __LSM_VECTOR3D_H__
#define __LSM_VECTOR3D_H__

#include "../LSMMathLib.h"
#include "LSMVector3Base.h"

namespace lsm {

	/**
	 * Class CVector3D
	 * \brief A standard 3D vector of type double.
	 *
	 * Description: A standard 3D vector of type double.
	 */
	class CVector3D : public CVector3Base<double, CVector3D> {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CVector3D();
		LSE_INLINE LSE_CALLCTOR							CVector3D( double _fX, double _fY, double _fZ );
		LSE_INLINE LSE_CALLCTOR							CVector3D( const CVector3D &_vOther );
		LSE_INLINE LSE_CALLCTOR							CVector3D( double * _pfValues );


		// == Functions.
		/**
		 * Gets the global forward vector.
		 *
		 * \return Returns CVector3D( LSM_ZERO, LSM_ZERO, LSM_ONE ).
		 */
		static LSE_INLINE const CVector3D & LSE_CALL		ForwardVector();

		/**
		 * Gets the global up vector.
		 *
		 * \return Returns CVector3D( LSM_ZERO, LSM_ONE, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3D & LSE_CALL		UpVector();

		/**
		 * Gets the global right vector.
		 *
		 * \return Returns CVector3D( LSM_ONE, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3D & LSE_CALL		RightVector();

		/**
		 * Gets the global zero vector.
		 *
		 * \return Returns CVector3D( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3D & LSE_CALL		ZeroVector();


	protected :
		// == Members.
		/** Global up vector. */
		static CVector3D									m_vUp;

		/** Global right vector. */
		static CVector3D									m_vRight;

		/** Global forward vector. */
		static CVector3D									m_vForward;

		/** Zero vector. */
		static CVector3D									m_vZero;


	private :
		typedef CVector3Base<double, CVector3D>			Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CVector3D::CVector3D() {
	}
	LSE_INLINE LSE_CALLCTOR CVector3D::CVector3D( double _fX, double _fY, double _fZ ) :
		Parent( _fX, _fY, _fZ ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3D::CVector3D( const CVector3D &_vOther ) :
		Parent( _vOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3D::CVector3D( double * _pfValues ) :
		Parent( _pfValues ) {
	}

	// == Functions.
	/**
	 * Gets the global forward vector.
	 *
	 * \return Returns CVector3D( LSM_ZERO, LSM_ZERO, LSM_ONE ).
	 */
	LSE_INLINE const CVector3D & LSE_CALL CVector3D::ForwardVector() {
		return m_vForward;
	}

	/**
	 * Gets the global up vector.
	 *
	 * \return Returns CVector3D( LSM_ZERO, LSM_ONE, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3D & LSE_CALL CVector3D::UpVector() {
		return m_vUp;
	}

	/**
	 * Gets the global right vector.
	 *
	 * \return Returns CVector3D( LSM_ONE, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3D & LSE_CALL CVector3D::RightVector() {
		return m_vRight;
	}

	/**
	 * Gets the global zero vector.
	 *
	 * \return Returns CVector3D( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3D & LSE_CALL CVector3D::ZeroVector() {
		return m_vZero;
	}

}	// namespace lsm

#endif	// __LSM_VECTOR3D_H__

