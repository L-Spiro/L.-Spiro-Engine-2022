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


#ifndef __LSM_MATRIX3X3_H__
#define __LSM_MATRIX3X3_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "LSMMatrix3x3Base.h"

namespace lsm {

	/**
	 * Class CMatrix3x3
	 * \brief A specialization of the 3-by-3 matrix template using the standard LSREAL type.
	 *
	 * Description: A specialization of the 3-by-3 matrix template using the standard LSREAL type.
	 */
	class CMatrix3x3 : public CMatrix3x3Base<LSREAL, CVector3> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						CMatrix3x3();
		LSE_INLINE LSE_CALLCTOR						CMatrix3x3( const CMatrix3x3 &_m33bOther );
		LSE_INLINE LSE_CALLCTOR						CMatrix3x3( const CMatrix3x3Base<LSREAL, CVector3> &_m33bOther );
		LSE_INLINE LSE_CALLCTOR						CMatrix3x3( const CMatrix3x3 &_m33bOther, LSBOOL _bTranspose );
		LSE_INLINE LSE_CALLCTOR						CMatrix3x3( const LSREAL * _ptArray );


		// == Functions.
		/**
		 * Creates a covariance matrix in place.  Uses high-precision math.
		 *
		 * \param _pvPoints Array of points from which the covariance matrix is to be created.
		 * \param _ui32Total Number of points in the array to which _pvPoints points.
		 */
		void LSE_FCALL							Covariance( const CVector3 * _pvPoints, uint32_t _ui32Total );

		/**
		 * Creates a covariance matrix in place using non-contiguous vertices.  Uses high-precision math.
		 *
		 * \param _pu8Points Array of points from which the covariance matrix is to be created.
		 * \param _ui32Total Number of points in the array to which _pvPoints points.
		 * \param _ui32Stride Space between the starts of vertices in bytes.
		 */
		void LSE_FCALL							Covariance( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride );

	private :
		typedef CMatrix3x3Base<LSREAL, CVector3>	Parent;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CMatrix3x3::CMatrix3x3() {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix3x3::CMatrix3x3( const CMatrix3x3 &_m33bOther ) :
		Parent( _m33bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix3x3::CMatrix3x3( const CMatrix3x3Base<LSREAL, CVector3> &_m33bOther ) :
		Parent( _m33bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix3x3::CMatrix3x3( const CMatrix3x3 &_m33bOther, LSBOOL _bTranspose ) :
		Parent( _m33bOther, _bTranspose ) {
		
	}
	LSE_INLINE LSE_CALLCTOR CMatrix3x3::CMatrix3x3( const LSREAL * _ptArray ) :
		Parent( _ptArray ) {
	}

}	// namespace lsm

#endif	// __LSM_MATRIX3X3_H__

