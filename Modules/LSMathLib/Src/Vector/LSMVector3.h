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
 * Description: A standard 3D vector of type LSREAL.
 */


#ifndef __LSM_VECTOR3_H__
#define __LSM_VECTOR3_H__

#include "../LSMMathLib.h"
#include "LSMVector3Base.h"

namespace lsm {

	/**
	 * Class CVector3
	 * \brief A standard 3D vector of type LSREAL.
	 *
	 * Description: A standard 3D vector of type LSREAL.
	 */
	class CVector3 : public CVector3Base<LSREAL, CVector3> {
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR							CVector3();
		LSE_INLINE LSE_CALLCTOR							CVector3( LSREAL _fX, LSREAL _fY, LSREAL _fZ );
		LSE_INLINE LSE_CALLCTOR							CVector3( const CVector3 &_vOther );
		LSE_INLINE LSE_CALLCTOR							CVector3( LSREAL * _pfValues );


		// == Functions.
		/**
		 * Calculates the length of the vector.
		 *
		 * \return Returns the length of the vector.
		 */
		LSE_INLINE LSREAL LSE_FCALL						Len() const;

		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		LSE_INLINE void LSE_FCALL						Normalize();

		/**
		 * Gets the global forward vector.
		 *
		 * \return Returns CVector3( LSM_ZERO, LSM_ZERO, LSM_ONE ).
		 */
		static LSE_INLINE const CVector3 & LSE_CALL		ForwardVector();

		/**
		 * Gets the global up vector.
		 *
		 * \return Returns CVector3( LSM_ZERO, LSM_ONE, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3 & LSE_CALL		UpVector();

		/**
		 * Gets the global right vector.
		 *
		 * \return Returns CVector3( LSM_ONE, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3 & LSE_CALL		RightVector();

		/**
		 * Gets the global zero vector.
		 *
		 * \return Returns CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
		 */
		static LSE_INLINE const CVector3 & LSE_CALL		ZeroVector();


	protected :
		// == Members.
#if LSM_PERFORMANCE <= 0 && (LSM_BASETYPE == LSM_FLOAT || LSM_BASETYPE == LSM_FIXED)
#pragma pack( push, 1 )
		// If ultimate optimization is enabled, add a value that forces an alignment.
	private :
		LSREAL											m_fPad;
	protected :
#pragma pack( pop )
#endif	// LSM_PERFORMANCE <= 0

		/** Global up vector. */
		static CVector3									m_vUp;

		/** Global right vector. */
		static CVector3									m_vRight;

		/** Global forward vector. */
		static CVector3									m_vForward;

		/** Zero vector. */
		static CVector3									m_vZero;


	private :
		typedef CVector3Base<LSREAL, CVector3>			Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CVector3::CVector3() {
	}
	LSE_INLINE LSE_CALLCTOR CVector3::CVector3( LSREAL _fX, LSREAL _fY, LSREAL _fZ ) :
		Parent( _fX, _fY, _fZ ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3::CVector3( const CVector3 &_vOther ) :
		Parent( _vOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CVector3::CVector3( LSREAL * _pfValues ) :
		Parent( _pfValues ) {
	}

	// == Functions.
	/**
	 * Calculates the length of the vector.
	 *
	 * \return Returns the length of the vector.
	 */
	LSE_INLINE LSREAL LSE_FCALL CVector3::Len() const {
		register LSREAL fSquared = x * x + y * y + z * z;
		return fSquared * CMathLib::InvSqrt( fSquared );
	}

	/**
	 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
	 *	unit length (1 unit in length) while maintaining its direction.
	 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
	 */
	LSE_INLINE void LSE_FCALL CVector3::Normalize() {
		LSREAL fInvLen = CMathLib::InvSqrt( x * x + y * y + z * z );
		x *= fInvLen;
		y *= fInvLen;
		z *= fInvLen;
	}

	/**
	 * Gets the global forward vector.
	 *
	 * \return Returns CVector3( LSM_ZERO, LSM_ZERO, LSM_ONE ).
	 */
	LSE_INLINE const CVector3 & LSE_CALL CVector3::ForwardVector() {
		return m_vForward;
	}

	/**
	 * Gets the global up vector.
	 *
	 * \return Returns CVector3( LSM_ZERO, LSM_ONE, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3 & LSE_CALL CVector3::UpVector() {
		return m_vUp;
	}

	/**
	 * Gets the global right vector.
	 *
	 * \return Returns CVector3( LSM_ONE, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3 & LSE_CALL CVector3::RightVector() {
		return m_vRight;
	}

	/**
	 * Gets the global zero vector.
	 *
	 * \return Returns CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ).
	 */
	LSE_INLINE const CVector3 & LSE_CALL CVector3::ZeroVector() {
		return m_vZero;
	}

}	// namespace lsm

#endif	// __LSM_VECTOR3_H__

