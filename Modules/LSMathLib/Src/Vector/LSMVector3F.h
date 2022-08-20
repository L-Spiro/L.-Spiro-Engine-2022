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
 * Description: A standard 3D vector of type float.
 */


#ifndef __LSM_VECTOR3F_H__
#define __LSM_VECTOR3F_H__

#include "../LSMMathLib.h"
#include "LSMVector3Base.h"

namespace lsm {

	/**
	 * Class CVector3F
	 * \brief A standard 3D vector of type float.
	 *
	 * Description: A standard 3D vector of type float.
	 */
	class CVector3F : public CVector3Base<float, CVector3F> {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CVector3F();
		LSE_INLINE LSE_CALLCTOR							CVector3F( float _fX, float _fY, float _fZ );
		LSE_INLINE LSE_CALLCTOR							CVector3F( const CVector3F &_vOther );
		LSE_INLINE LSE_CALLCTOR							CVector3F( float * _pfValues );


		// == Functions.
		/**
		 * Gets the global forward vector.
		 *
		 * \return Returns CVector3F( LSM_ZERO, LSM_ZERO, LSM_ONE ).
		 */
		static LSE_INLINE const CVector3F & LSE_CALL		ForwardVector();

		/**
		 * Gets the global up vector.
		 *
		 * \return Returns CVector3F( LSM_ZERO, LSM_ONE, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3F & LSE_CALL		UpVector();

		/**
		 * Gets the global right vector.
		 *
		 * \return Returns CVector3F( LSM_ONE, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3F & LSE_CALL		RightVector();

		/**
		 * Gets the global zero vector.
		 *
		 * \return Returns CVector3F( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3F & LSE_CALL		ZeroVector();


	protected :
		// == Members.
#if LSM_PERFORMANCE <= 0
#pragma pack( push, 1 )
		// If ultimate optimization is enabled, add a value that forces an alignment.
	private :
		float												m_fPad;
	protected :
#pragma pack( pop )
#endif	// LSM_PERFORMANCE <= 0

		/** Global up vector. */
		static CVector3F									m_vUp;

		/** Global right vector. */
		static CVector3F									m_vRight;

		/** Global forward vector. */
		static CVector3F									m_vForward;

		/** Zero vector. */
		static CVector3F									m_vZero;


	private :
		typedef CVector3Base<float, CVector3F>			Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CVector3F::CVector3F() {
	}
	LSE_INLINE LSE_CALLCTOR CVector3F::CVector3F( float _fX, float _fY, float _fZ ) :
		Parent( _fX, _fY, _fZ ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3F::CVector3F( const CVector3F &_vOther ) :
		Parent( _vOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3F::CVector3F( float * _pfValues ) :
		Parent( _pfValues ) {
	}

	// == Functions.
	/**
	 * Gets the global forward vector.
	 *
	 * \return Returns CVector3F( LSM_ZERO, LSM_ZERO, LSM_ONE ).
	 */
	LSE_INLINE const CVector3F & LSE_CALL CVector3F::ForwardVector() {
		return m_vForward;
	}

	/**
	 * Gets the global up vector.
	 *
	 * \return Returns CVector3F( LSM_ZERO, LSM_ONE, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3F & LSE_CALL CVector3F::UpVector() {
		return m_vUp;
	}

	/**
	 * Gets the global right vector.
	 *
	 * \return Returns CVector3F( LSM_ONE, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3F & LSE_CALL CVector3F::RightVector() {
		return m_vRight;
	}

	/**
	 * Gets the global zero vector.
	 *
	 * \return Returns CVector3F( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3F & LSE_CALL CVector3F::ZeroVector() {
		return m_vZero;
	}

}	// namespace lsm

#endif	// __LSM_VECTOR3F_H__

