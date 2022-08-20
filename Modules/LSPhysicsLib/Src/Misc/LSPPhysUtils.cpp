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
 * Description: Miscellaneous helper functions for the physics library.
 */

#include "LSPPhysUtils.h"
#include "../Intersections/LSPIntersect.h"


namespace lsp {

	// == Functions.
	/**
	 * Gets the most extreme points in both directions of a directional vector.  Returns the indices of
	 *	the found points.
	 *
	 * \param _vDir Direction along which to get the most extreme points.
	 * \param _pvPoints Array of points to scan.
	 * \param _ui32TotalPoints Number of points to which _pvPoints points.
	 * \param _ui32Min Holds the index of the point representing the most minimum point along the given direction.
	 * \param _ui32Max Holds the index of the point representing the most maximum point along the given direction.
	 */
	void LSE_CALL CPhysUtils::ExtremePointsAlongDirection( const CVector3 &_vDir, const CVector3 * _pvPoints,
		uint32_t _ui32TotalPoints, uint32_t &_ui32Min, uint32_t &_ui32Max ) {
		LSREAL fMin = LSM_INFINITY;
		LSREAL fMax = -LSM_INFINITY;
		_ui32Min = _ui32Max = _ui32TotalPoints;

		for ( uint32_t I = 0; I < _ui32TotalPoints; ++I ) {
			LSREAL fProj = _vDir.Dot( _pvPoints[I] );

			// Double-check here; it is safer.  Be aware of fixed-point madness.
			if ( fProj < fMin || I == 0 ) {
				fMin = fProj;
				_ui32Min = I;
			}
			if ( fProj > fMax || I == 0 ) {
				fMax = fProj;
				_ui32Max = I;
			}
		}
	}

	/**
	 * Gets the extreme distances in both directions of a directional vector.  Returns the min and max values
	 *	in the direction of the given vector.
	 *
	 * \param _vDir Direction along which to get the most extreme points.
	 * \param _pvPoints Array of points to scan.
	 * \param _ui32TotalPoints Number of points to which _pvPoints points.
	 * \param _fMin Holds the most minimum value along the given direction.
	 * \param _fMax Holds the most maximum value along the given direction.
	 */
	void LSE_CALL CPhysUtils::ExtremeValuesAlongDirection( const CVector3 &_vDir, const CVector3 * _pvPoints,
		uint32_t _ui32TotalPoints, LSREAL &_fMin, LSREAL &_fMax ) {
		_fMin = LSM_INFINITY;
		_fMax = -LSM_INFINITY;

		for ( uint32_t I = 0; I < _ui32TotalPoints; ++I ) {
			LSREAL fProj = _vDir.Dot( _pvPoints[I] );

			// Double-check here; it is safer.  Be aware of fixed-point madness.
			if ( fProj < _fMin || I == 0 ) {
				_fMin = fProj;
			}
			if ( fProj > _fMax || I == 0 ) {
				_fMax = fProj;
			}
		}
	}

	/**
	 * Creates a sphere given an existing sphere that represents that sphere expanded to encapsulate the
	 *	given point.  If the given point is already inside the given sphere, the given sphere is returned
	 *	unmodified.  Modifies the input sphere and returns it so that this function can be used as a
	 *	parameter to another function.
	 *
	 * \param _sSphere The sphere to modify to encapsulate the given point.
	 * \param _vPoint The point to encapsulate by the given sphere.
	 * \return Returns the input sphere.
	 */
	CSphere & LSE_CALL CPhysUtils::SphereOfSphereAndPoint( CSphere &_sSphere, const CVector3 &_vPoint ) {
		// Get the squared distance between the point and sphere.
		CVector3 vD = _vPoint - _sSphere.c;
		LSREAL fDist2 = vD.LenSq();


		// Only update if the point is outside of the sphere.
		if ( fDist2 > _sSphere.r * _sSphere.r ) {
			LSREAL fDist = CMathLib::Sqrt( fDist2 );
			LSREAL fNewRadius = (_sSphere.r + fDist) * LSM_HALF;
			LSREAL fK = (fNewRadius - _sSphere.r) / fDist;
			_sSphere.r = fNewRadius;
			_sSphere.c += vD * fK;
		}

		return _sSphere;
	}

	/**
	 * Gets the symmetric Schur decomposition of a 3-by-3 covariance matrix.
	 * _i32P and _i32Q are given such that 0 <= _i32P < _i32Q <= 2.  Returns the sin/cos pair that
	 *	serves to form a Jacobi rotation matrix.
	 *
	 * \param _mMat The input covariance matrix.
	 * \param _i32P The P parameter.
	 * \param _i32Q The Q parameter.
	 * \param _fCos Holds the returned cosine.
	 * \param _fSin Holds the returned sine.
	 */
	void LSE_CALL CPhysUtils::SymSchur2( const CMatrix3x3 &_mMat, int32_t _i32P, int32_t _i32Q,
		LSREALHP &_fCos, LSREALHP &_fSin ) {
		// We work in high-precision.
		LSREALHP fValPq = LSREAL_TO_HP( _mMat( static_cast<uint32_t>(_i32P), static_cast<uint32_t>(_i32Q) ) );
		if ( LSMABSHP( fValPq ) > static_cast<LSREALHP>(0.0001) ) {
			LSREALHP fValPp = LSREAL_TO_HP( _mMat( static_cast<uint32_t>(_i32P), static_cast<uint32_t>(_i32P) ) );
			LSREALHP fValQq = LSREAL_TO_HP( _mMat( static_cast<uint32_t>(_i32Q), static_cast<uint32_t>(_i32Q) ) );
			LSREALHP fR = (fValQq - fValPp) / (static_cast<LSREALHP>(2.0) * fValPq);
			LSREALHP fT;
			if ( fR >= static_cast<LSREALHP>(0.0) ) {
				fT = static_cast<LSREALHP>(1.0) / (fR + LSMSQRTHP( static_cast<LSREALHP>(1.0) + fR * fR ));
			}
			else {
				fT = static_cast<LSREALHP>(-1.0) / (-fR + LSMSQRTHP( static_cast<LSREALHP>(1.0) + fR * fR ));
			}
			_fCos = static_cast<LSREALHP>(1.0) / LSMSQRTHP( static_cast<LSREALHP>(1.0) + fT * fT );
			_fSin = fT * _fCos;
		}
		else {
			_fCos = static_cast<LSREALHP>(1.0);
			_fSin = static_cast<LSREALHP>(0.0);
		}
	}

	/**
	 * Computes the eigenvectors and eigenvalues of the symmetric matrix provided using the classic Jacobi
	 *	method of iteratively updating A (the matrix) as A = J^T * A * J, where J = J(p, q, theta) as the
	 *	Jacobi rotation matrix.  Uses high-precision types.
	 * _mV is filled with the eigenvectors and the diagonal elements of _mA will be set to the
	 *	corresponding eigenvalues.
	 *
	 * \param _mA On input, contains a covariance matrix.  On output, holds the returned eigenvalues.
	 * \param _mV Filled with the eigenvectors.
	 */
	void LSE_CALL CPhysUtils::Jacobi( CMatrix3x3 &_mA, CMatrix3x3 &_mV ) {
		LSREALHP fCos, fSin;
		CMatrix3x3 mJ, mB, mT;
		LSREALHP fPrevOff = static_cast<LSREALHP>(0.0);


		for ( uint32_t I = 3; I--; ) {
			_mV( I, 0 ) = _mV( I, 1 ) = _mV( I, 2 ) = LSM_ZERO;
			_mV( I, I ) = LSM_ONE;
		}

		// Repeat for some maximum number of iterations.
		const int32_t i32MaxIterations = 50;
		for ( int32_t N = 0; N < i32MaxIterations; ++N ) {
			int32_t i32P = 0, i32Q = 1;
			// Find the largest off-diagonal element of _mA( i32P, i32Q ).
			for ( uint32_t I = 0; I < 3; ++I ) {
				for ( uint32_t J = 0; J < 3; ++J ) {
					if ( I == J ) { continue; }
					if ( LSMABSHP( _mA( I, J ) ) > LSMABSHP( _mA( static_cast<uint32_t>(i32P), static_cast<uint32_t>(i32Q) ) ) ) {
						i32P = static_cast<int32_t>(I);
						i32Q = static_cast<int32_t>(J);
					}
				}
			}

			// Computes the Jacobi rotation matrix.
			SymSchur2( _mA, i32P, i32Q, fCos, fSin );
			for ( uint32_t I = 3; I--; ) {
				mJ( I, 0 ) = mJ( I, 1 ) = mJ( I, 2 ) = LSM_ZERO;
				mJ( I, I ) = LSM_ONE;
			}
			mJ( static_cast<uint32_t>(i32P), static_cast<uint32_t>(i32P) ) = HP_TO_LSREAL( fCos );
			mJ( static_cast<uint32_t>(i32P), static_cast<uint32_t>(i32Q) ) = HP_TO_LSREAL( fSin );
			mJ( static_cast<uint32_t>(i32Q), static_cast<uint32_t>(i32P) ) = HP_TO_LSREAL( -fSin );
			mJ( static_cast<uint32_t>(i32Q), static_cast<uint32_t>(i32Q) ) = HP_TO_LSREAL( fCos );

			// Cumulate rotations into what will contain the eigenvectors.
			_mV = _mV * mJ;

			// Make A more diagonal, until just eigenvalues remain on the diagonal.
			_mA = (mJ.Transpose() * _mA) * mJ;

			// Compute "norm" of off-diagonal elements.
			LSREALHP fOff = static_cast<LSREALHP>(0.0);

			for ( uint32_t I = 0; I < 3; ++I ) {
				for ( uint32_t J = 0; J < 3; ++J ) {
					if ( I == J ) { continue; }
					fOff += LSREAL_TO_HP( _mA( I, J ) ) * LSREAL_TO_HP( _mA( I, J ) );
				}
			}

			// Stop when "norm" is no longer decreasing.
			// Minimum of 2 iterations.
			if ( N > 2 && fOff >= fPrevOff ) { return; }

			fPrevOff = fOff;
			// Go again.
		}
	}

	/**
	 * Computes the eigensphere for a given set of points.  The eigensphere is a close approximation
	 *	of the smallest bounding sphere that can encapsulate all of the given points.
	 *
	 * \param _pvPoints Points around which to create an enclosing spehere.
	 * \param _ui32TotalPoints Total number of points to which _pvPoints points.
	 * \return Returns a sphere that encloses all of the given points.
	 */
	CSphere LSE_CALL CPhysUtils::EigenSphere( const CVector3 * _pvPoints, uint32_t _ui32TotalPoints ) {
		// Sanity checking.
		if ( _ui32TotalPoints == 0 ) {
			return CSphere( CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ), LSM_ZERO );
		}
		if ( _ui32TotalPoints == 1 ) {
			return CSphere( _pvPoints[0], LSM_ZERO );
		}
		if ( _ui32TotalPoints == 2 ) {
			return CSphere( (_pvPoints[0] + _pvPoints[1]) * LSM_HALF,
				(_pvPoints[0] - _pvPoints[1]).Len() * LSM_HALF );
		}

		CSphere sReturn;
		CMatrix3x3 mM, mV;

		// Computes the covariance matrix M.
		mM.Covariance( _pvPoints, _ui32TotalPoints );

		// Decompose it into eigenvectors (in mV) and eigenvalues (in mM).
		Jacobi( mM, mV );

		CVector3 vE;
		uint32_t ui32MaxC = 0;
		LSREAL fMaxF, fMaxE = CMathLib::Abs( mM( 0, 0 ) );

		if ( (fMaxF = CMathLib::Abs( mM( 1, 1 ) )) > fMaxE ) {
			ui32MaxC = 1;
			fMaxE = fMaxF;
		}
		if ( (fMaxF = CMathLib::Abs( mM( 2, 2 ) )) > fMaxE ) {
			ui32MaxC = 2;
		}

		vE[0] = mV( 0, ui32MaxC );
		vE[1] = mV( 1, ui32MaxC );
		vE[2] = mV( 2, ui32MaxC );

		// Find the most extreme points in direction vE.
		uint32_t ui32Min, ui32Max;
		ExtremePointsAlongDirection( vE, _pvPoints, _ui32TotalPoints, ui32Min, ui32Max );

		const CVector3 & vMinPoint = _pvPoints[ui32Min];
		const CVector3 & vMaxPoint = _pvPoints[ui32Max];
		
		sReturn.r = (vMaxPoint - vMinPoint).Len() * LSM_HALF;
		sReturn.c = (vMaxPoint + vMinPoint) * LSM_HALF;

		return sReturn;
	}

	/**
	 * Computes an approximate bounding sphere for a set of points.  Uses the axis of maximum spread for
	 *	the initial guess and then refines the sphere using the Ritter method.
	 *
	 * \param _pvPoints Points around which to create an enclosing spehere.
	 * \param _ui32TotalPoints Total number of points to which _pvPoints points.
	 * \return Returns a sphere that encloses all of the given points.
	 */
	CSphere LSE_CALL CPhysUtils::RitterEigenSphere( const CVector3 * _pvPoints, uint32_t _ui32TotalPoints ) {
		// Get the sphere from maximum spread.
		CSphere sRet = EigenSphere( _pvPoints, _ui32TotalPoints );

		// Refine it until it includes all points.
		for ( uint32_t I = 0; I < _ui32TotalPoints; ++I ) {
			SphereOfSphereAndPoint( sRet, _pvPoints[I] );
		}

		return sRet;
	}

	/**
	 * Fills an array of 8 points with the corner points of a frustum.
	 *
	 * \param _fFrustum The frustum whose corners are to be extracted.
	 * \param _vRet The array of exactly 8 points into which the frustum corners are extracted.
	 */
	void LSE_CALL CPhysUtils::FrustumCorners( const CFrustum &_fFrustum, CVector3 _vRet[8] ) {
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pFarPlane, _fFrustum.m_pBottomPlane, _fFrustum.m_pLeftPlane, _vRet[LSM_FP_FAR_BOTTOM_LEFT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pFarPlane, _fFrustum.m_pTopPlane, _fFrustum.m_pLeftPlane, _vRet[LSM_FP_FAR_TOP_LEFT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pFarPlane, _fFrustum.m_pTopPlane, _fFrustum.m_pRightPlane, _vRet[LSM_FP_FAR_TOP_RIGHT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pFarPlane, _fFrustum.m_pBottomPlane, _fFrustum.m_pRightPlane, _vRet[LSM_FP_FAR_BOTTOM_RIGHT] );

		// Again with the near plane.
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pNearPlane, _fFrustum.m_pBottomPlane, _fFrustum.m_pLeftPlane, _vRet[LSM_FP_NEAR_BOTTOM_LEFT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pNearPlane, _fFrustum.m_pTopPlane, _fFrustum.m_pLeftPlane, _vRet[LSM_FP_NEAR_TOP_LEFT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pNearPlane, _fFrustum.m_pTopPlane, _fFrustum.m_pRightPlane, _vRet[LSM_FP_NEAR_TOP_RIGHT] );
		CIntersect::ThreePlanes<LSREAL>( _fFrustum.m_pNearPlane, _fFrustum.m_pBottomPlane, _fFrustum.m_pRightPlane, _vRet[LSM_FP_NEAR_BOTTOM_RIGHT] );
	}

	/**
	 * Gets the most major axis of a vector.
	 *
	 * \param _vVector The vector whose most major component is to be found.
	 * \return Returns a vector with one component set to 1 (or -1) and the rest to 0.  This vector points along the
	 *	most major axis in the given vector and is signed.
	 */
	CVector3 LSE_CALL CPhysUtils::MajorAxis( const CVector3 &_vVector ) {
		uint32_t ui32Best = 0;
		LSREAL fBest = _vVector[0];
		for ( uint32_t I = 1; I < 3; ++I ) {
			if ( CMathLib::Abs( _vVector[I] ) < CMathLib::Abs( fBest ) ) {
				fBest = _vVector[I];
				ui32Best = I;
			}
		}
		CVector3 vRet( LSM_ZERO, LSM_ZERO, LSM_ZERO );
		vRet[ui32Best] = fBest > LSM_ZERO ? LSM_ONE : -LSM_ONE;
		return vRet;
	}

}	// namespace lsp
