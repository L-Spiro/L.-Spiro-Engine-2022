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
 * Description: A specialization of the 3-by-3 matrix template using the standard LSREAL type.
 */

#include "LSMMatrix3x3.h"


namespace lsm {

	// == Functions.
	/**
	 * Creates a covariance matrix in place.  Uses high-precision math.
	 *
	 * \param _pvPoints Array of points from which the covariance matrix is to be created.
	 * \param _ui32Total Number of points in the array to which _pvPoints points.
	 */
	void LSE_FCALL CMatrix3x3::Covariance( const CVector3 * _pvPoints, uint32_t _ui32Total ) {
		LSREALHP fOon = static_cast<LSREALHP>(1.0) / static_cast<LSREALHP>(_ui32Total);
		// Implicit high-precision vector.
		LSREALHP vC[3];
		vC[0] = vC[1] = vC[2] = static_cast<LSREALHP>(0.0);
		LSREALHP fE00, fE11, fE22, fE01, fE02, fE12;

		// Find the centroid of the points.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			vC[0] += LSREAL_TO_HP( _pvPoints[I].x );
			vC[1] += LSREAL_TO_HP( _pvPoints[I].y );
			vC[2] += LSREAL_TO_HP( _pvPoints[I].z );
		}
		vC[0] *= fOon;
		vC[1] *= fOon;
		vC[2] *= fOon;

		// Computes the covariance elements.
		fE00 = fE11 = fE22 = fE01 = fE02 = fE12 = static_cast<LSREALHP>(0.0);
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			// Translate so that center of mass is at origin.
			LSREALHP vThis[3] = {
				LSREAL_TO_HP( _pvPoints[I].x ) - vC[0],
				LSREAL_TO_HP( _pvPoints[I].y ) - vC[1],
				LSREAL_TO_HP( _pvPoints[I].z ) - vC[2],
			};

			// Computes the covariance of the translated points.
			fE00 += vThis[0] * vThis[0];
			fE11 += vThis[1] * vThis[1];
			fE22 += vThis[2] * vThis[2];
			fE01 += vThis[0] * vThis[1];
			fE02 += vThis[0] * vThis[2];
			fE12 += vThis[1] * vThis[2];
		}
		// Fill in the covariance matrix.
		(*this)( 0, 0 ) = HP_TO_LSREAL( fE00 * fOon );
		(*this)( 1, 1 ) = HP_TO_LSREAL( fE11 * fOon );
		(*this)( 2, 2 ) = HP_TO_LSREAL( fE22 * fOon );
		(*this)( 0, 1 ) = (*this)( 1, 0 ) = HP_TO_LSREAL( fE01 * fOon );
		(*this)( 0, 2 ) = (*this)( 2, 0 ) = HP_TO_LSREAL( fE02 * fOon );
		(*this)( 1, 2 ) = (*this)( 2, 1 ) = HP_TO_LSREAL( fE12 * fOon );
	}

	/**
	 * Creates a covariance matrix in place using non-contiguous vertices.  Uses high-precision math.
	 *
	 * \param _pu8Points Array of points from which the covariance matrix is to be created.
	 * \param _ui32Total Number of points in the array to which _pvPoints points.
	 * \param _ui32Stride Space between the starts of vertices in bytes.
	 */
	void LSE_FCALL CMatrix3x3::Covariance( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride ) {
		LSREALHP fOon = static_cast<LSREALHP>(1.0) / static_cast<LSREALHP>(_ui32Total);
		// Implicit high-precision vector.
		LSREALHP vC[3];
		vC[0] = vC[1] = vC[2] = static_cast<LSREALHP>(0.0);
		LSREALHP fE00, fE11, fE22, fE01, fE02, fE12;

		// Find the centroid of the points.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			const CVector3 & vThisVector = (*reinterpret_cast<const CVector3 *>(_pu8Points + _ui32Stride * I));
			vC[0] += LSREAL_TO_HP( vThisVector.x );
			vC[1] += LSREAL_TO_HP( vThisVector.y );
			vC[2] += LSREAL_TO_HP( vThisVector.z );
		}
		vC[0] *= fOon;
		vC[1] *= fOon;
		vC[2] *= fOon;

		// Computes the covariance elements.
		fE00 = fE11 = fE22 = fE01 = fE02 = fE12 = static_cast<LSREALHP>(0.0);
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			// Translate so that center of mass is at origin.
			const CVector3 & vThisVector = (*reinterpret_cast<const CVector3 *>(_pu8Points + _ui32Stride * I));
			LSREALHP vThis[3] = {
				LSREAL_TO_HP( vThisVector.x ) - vC[0],
				LSREAL_TO_HP( vThisVector.y ) - vC[1],
				LSREAL_TO_HP( vThisVector.z ) - vC[2],
			};

			// Computes the covariance of the translated points.
			fE00 += vThis[0] * vThis[0];
			fE11 += vThis[1] * vThis[1];
			fE22 += vThis[2] * vThis[2];
			fE01 += vThis[0] * vThis[1];
			fE02 += vThis[0] * vThis[2];
			fE12 += vThis[1] * vThis[2];
		}
		// Fill in the covariance matrix.
		(*this)( 0, 0 ) = HP_TO_LSREAL( fE00 * fOon );
		(*this)( 1, 1 ) = HP_TO_LSREAL( fE11 * fOon );
		(*this)( 2, 2 ) = HP_TO_LSREAL( fE22 * fOon );
		(*this)( 0, 1 ) = (*this)( 1, 0 ) = HP_TO_LSREAL( fE01 * fOon );
		(*this)( 0, 2 ) = (*this)( 2, 0 ) = HP_TO_LSREAL( fE02 * fOon );
		(*this)( 1, 2 ) = (*this)( 2, 1 ) = HP_TO_LSREAL( fE12 * fOon );
	}

}	// namespace lsm
