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


#ifndef __LSP_PHYSUTILS_H__
#define __LSP_PHYSUTILS_H__

#include "../LSPPhysicsLib.h"
#include "../BoundingBoxes/LSPObb.h"
#include "../Intersections/LSPClassify.h"
#include "../Intersections/LSPIntersect.h"
#include "Frustum/LSMFrustum.h"
#include "Matrix/LSMMatrix3x3.h"
#include "Plane/LSMPlane3.h"
#include "Sphere/LSMSphere.h"
#include "Vector/LSMVector3.h"
#include "Vector/LSTLVector.h"

namespace lsp {

	/**
	 * Class CPhysUtils
	 * \brief Miscellaneous helper functions for the physics library.
	 *
	 * Description: Miscellaneous helper functions for the physics library.
	 */
	class CPhysUtils {
		// All is public.  This class has no secrets.
	public :
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
		static void LSE_CALL				ExtremePointsAlongDirection( const CVector3 &_vDir, const CVector3 * _pvPoints,
			uint32_t _ui32TotalPoints, uint32_t &_ui32Min, uint32_t &_ui32Max );

		/**
		 * Gets the most extreme points in both directions of a directional vector.  Returns the indices of
		 *	the found points.
		 *
		 * \param _vDir Direction along which to get the most extreme points.
		 * \param _pu8Points Array of points to scan.
		 * \param _ui32Total Number of points to which _pvPoints points.
		 * \param _ui32Stride Space between the starts of vertices in bytes.
		 * \param _ui32Min Holds the index of the point representing the most minimum point along the given direction.
		 * \param _ui32Max Holds the index of the point representing the most maximum point along the given direction.
		 */
		template <typename _tVectorType>
		static void LSE_CALL				ExtremePointsAlongDirection( const CVector3 &_vDir,
			const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride,
			uint32_t &_ui32Min, uint32_t &_ui32Max );

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
		static void LSE_CALL				ExtremeValuesAlongDirection( const CVector3 &_vDir, const CVector3 * _pvPoints,
			uint32_t _ui32TotalPoints, LSREAL &_fMin, LSREAL &_fMax );

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
		static CSphere & LSE_CALL			SphereOfSphereAndPoint( CSphere &_sSphere, const CVector3 &_vPoint );

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
		static void LSE_CALL				SymSchur2( const CMatrix3x3 &_mMat, int32_t _i32P, int32_t _i32Q,
			LSREALHP &_fCos, LSREALHP &_fSin );

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
		static void LSE_CALL				Jacobi( CMatrix3x3 &_mA, CMatrix3x3 &_mV );

		/**
		 * Computes the eigensphere for a given set of points.  The eigensphere is a close approximation
		 *	of the smallest bounding sphere that can encapsulate all of the given points.
		 *
		 * \param _pvPoints Points around which to create an enclosing spehere.
		 * \param _ui32TotalPoints Total number of points to which _pvPoints points.
		 * \return Returns a sphere that encloses all of the given points.
		 */
		static CSphere LSE_CALL				EigenSphere( const CVector3 * _pvPoints, uint32_t _ui32TotalPoints );

		/**
		 * Computes the eigensphere for a given set of non-contiguous points like in a vertex buffer.
		 *	The eigensphere is a close approximation of the smallest bounding sphere that can encapsulate
		 *	all of the given points.
		 *
		 * \param _pu8Points Points around which to create an enclosing spehere.
		 * \param _ui32Total Total number of points to which _pvPoints points.
		 * \param _ui32Stride Space between the starts of vertices in bytes.
		 * \return Returns a sphere that encloses all of the given points.
		 */
		template <typename _tVectorType>
		static CSphere LSE_CALL				EigenSphere( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride );

		/**
		 * Computes an approximate bounding sphere for a set of points.  Uses the axis of maximum spread for
		 *	the initial guess and then refines the sphere using the Ritter method.
		 *
		 * \param _pvPoints Points around which to create an enclosing spehere.
		 * \param _ui32TotalPoints Total number of points to which _pvPoints points.
		 * \return Returns a sphere that encloses all of the given points.
		 */
		static CSphere LSE_CALL				RitterEigenSphere( const CVector3 * _pvPoints, uint32_t _ui32TotalPoints );

		/**
		 * Computes an approximate bounding sphere for a set of non-contiguous points like in a vertex buffer.
		 *	Uses the axis of maximum spread for the initial guess and then refines the sphere using the Ritter method.
		 *
		 * \param _pu8Points Points around which to create an enclosing spehere.
		 * \param _ui32Total Total number of points to which _pvPoints points.
		 * \param _ui32Stride Space between the starts of vertices in bytes.
		 * \return Returns a sphere that encloses all of the given points.
		 */
		template <typename _tVectorType>
		static CSphere LSE_CALL				RitterEigenSphere( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride );

		/**
		 * Fills an array of 8 points with the corner points of a frustum.
		 *
		 * \param _fFrustum The frustum whose corners are to be extracted.
		 * \param _vRet The array of exactly 8 points into which the frustum corners are extracted.
		 */
		static void LSE_CALL				FrustumCorners( const CFrustum &_fFrustum, CVector3 _vRet[8] );

		/**
		 * Gets the most major axis of a vector.
		 *
		 * \param _vVector The vector whose most major component is to be found.
		 * \return Returns a vector with one component set to 1 (or -1) and the rest to 0.  This vector points along the
		 *	most major axis in the given vector and is signed.
		 */
		static CVector3 LSE_CALL			MajorAxis( const CVector3 &_vVector );

		/**
		 * Splits a polygon into 2 new polygons along a given plane.
		 *
		 * \param _pPoly The polygon to split.
		 * \param _pPlane The plane along which to split the polygon.
		 * \param _pui32LeftSides If not NULL, holds the returned number of sides in the left returned polygon.
		 * \param _ppLeftRet If not NULL, holds the returned left polygon.
		 * \param _pui32RightSides If not NULL, holds the returned number of sides in the right returned polygon.
		 * \param _ppRightRet If not NULL, holds the returned right polygon.
		 * \param _fThickness Thickness of planes.
		 */
		template <typename _tType, typename _tVecType, typename _tPolyType, typename _tPlaneType>
		static void LSE_CALL				Split( const _tPolyType &_pPoly, const _tPlaneType &_pPlane,
			uint32_t * _pui32LeftSides, _tPolyType * _ppLeftRet,
			uint32_t * _pui32RightSides, _tPolyType * _ppRightRet,
			_tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
			// No way to split a polygon with fewer than 3 vertices.
			assert( _pPoly.TotalSegments() >= 3 );

			uint32_t ui32FrontCount = 0;
			uint32_t ui32BackCount = 0;

			_tVecType vA = _pPoly[_pPoly.TotalSegments()-1]->p;
			LSM_PLANE_INTERSECT piSide = CClassify::Point<_tType, _tVecType, _tPlaneType>( vA, _pPlane, _fThickness );
			uint32_t ui32Total = _pPoly.TotalSegments();

			_tVecType vSplitPoint;
#define LSP_PUSHFRONT( VEC )	if ( _ppLeftRet ) { (*_ppLeftRet).Segments()[ui32FrontCount].p = VEC; } ++ui32FrontCount
#define LSP_PUSHBACK( VEC )		if ( _ppRightRet ) { (*_ppRightRet).Segments()[ui32BackCount].p = VEC; } ++ui32BackCount

			// Run over all edges of the polygon.
			for ( uint32_t N = 0; N < ui32Total; ++N ) {
				_tVecType vB = _pPoly[N]->p;
				LSM_PLANE_INTERSECT piSide2 = CClassify::Point<_tType>( vB, _pPlane, _fThickness );
				switch ( piSide2 ) {
					case LSM_PI_FRONT : {
						if ( piSide == LSM_PI_BACK ) {
							// Edge (A, B) crosses plane.
							CIntersect::LineSegPlaneFast<_tType, _tVecType, _tPlaneType>( vB, vA, _pPlane, vSplitPoint );
							LSP_PUSHFRONT( vSplitPoint );
							//if ( !vFrontVerts.Push( vSplitPoint ) ) { return false; }

							LSP_PUSHBACK( vSplitPoint );
							//if ( !vBackVerts.Push( vSplitPoint ) ) { return false; }
						}
						// Always put B on the front side.
						LSP_PUSHFRONT( vB );
						//if ( !vFrontVerts.Push( vB ) ) { return false; }
						break;
					}
					case LSM_PI_BACK : {
						if ( piSide == LSM_PI_FRONT ) {
							// Edge (A, B) crosses plane.
							CIntersect::LineSegPlaneFast<_tType, _tVecType, _tPlaneType>( vA, vB, _pPlane, vSplitPoint );
							LSP_PUSHFRONT( vSplitPoint );
							//if ( !vFrontVerts.Push( vSplitPoint ) ) { return false; }

							LSP_PUSHBACK( vSplitPoint );
							//if ( !vBackVerts.Push( vSplitPoint ) ) { return false; }
						}
						else if ( piSide == LSM_PI_COPLANAR ) {
							// Output A when edge goes from on to behind the plane.
							LSP_PUSHBACK( vA );
							//if ( !vBackVerts.Push( vA ) ) { return false; }
						}
						// Always put B on the back side.
						LSP_PUSHBACK( vB );
						//if ( !vBackVerts.Push( vB ) ) { return false; }
						break;
					}
					default : {
						// B is on the plane.  Always pass it to the front side.
						LSP_PUSHFRONT( vB );
						//if ( !vFrontVerts.Push( vB ) ) { return false; }
						// If A is behind the plane, also put B on the back side.
						if ( piSide == LSM_PI_BACK ) {
							LSP_PUSHBACK( vB );
							//if ( !vBackVerts.Push( vB ) ) { return false; }
						}
					}
				}

				// Next edge will be from B to the next.
				vA = vB;
				piSide = piSide2;
			}
#undef LSP_PUSHBACK
#undef LSP_PUSHFRONT

			if ( _pui32LeftSides ) {
				(*_pui32LeftSides) = ui32FrontCount;
			}
			if ( _pui32RightSides ) {
				(*_pui32RightSides) = ui32BackCount;
			}
			if ( _ppLeftRet ) {
				for ( uint32_t I = ui32FrontCount; I--; ) {
					(*_ppLeftRet)[I]->q = (*_ppLeftRet)[(I+1)%ui32FrontCount]->p;
				}
				(*_ppLeftRet).Plane() = _pPoly.Plane();
				(*_ppLeftRet).SetUser( _pPoly.GetUser() );
			}
			if ( _ppRightRet ) {
				for ( uint32_t I = ui32BackCount; I--; ) {
					(*_ppRightRet)[I]->q = (*_ppRightRet)[(I+1)%ui32BackCount]->p;
				}
				(*_ppRightRet).Plane() = _pPoly.Plane();
				(*_ppRightRet).SetUser( _pPoly.GetUser() );
			}
		}

		/**
		 * Splits a polygon into 2 new polygons along a given plane.
		 *
		 * \param _pPoly The polygon to split.
		 * \param _pPlane The plane along which to split the polygon.
		 * \param _pui32LeftSides If not NULL, holds the returned number of sides in the left returned polygon.
		 * \param _ppLeftRet If not NULL, holds the returned left polygon.
		 * \param _pui32RightSides If not NULL, holds the returned number of sides in the right returned polygon.
		 * \param _ppRightRet If not NULL, holds the returned right polygon.
		 * \param _fThickness Thickness of planes.
		 */
		/*template <typename _tType, typename _tVecType, typename _tPolyType, typename _tPlaneType>
		static void LSE_CALL				Carve( const _tPolyType &_pPoly, const _tPolyType &_pSplitterPoly,
			uint32_t * _pui32LeftSides, _tPolyType * _ppLeftRet,
			_tType _fThickness = static_cast<_tType>(LSM_PLANE_THICKNESS) ) {
		}*/


	protected :
		// == Members.
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the most extreme points in both directions of a directional vector.  Returns the indices of
	 *	the found points.
	 *
	 * \param _vDir Direction along which to get the most extreme points.
	 * \param _pu8Points Array of points to scan.
	 * \param _ui32Total Number of points to which _pvPoints points.
	 * \param _ui32Stride Space between the starts of vertices in bytes.
	 * \param _ui32Min Holds the index of the point representing the most minimum point along the given direction.
	 * \param _ui32Max Holds the index of the point representing the most maximum point along the given direction.
	 */
	template <typename _tVectorType>
	void LSE_CALL CPhysUtils::ExtremePointsAlongDirection( const CVector3 &_vDir,
		const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride,
		uint32_t &_ui32Min, uint32_t &_ui32Max ) {
		LSREAL fMin = LSM_INFINITY;
		LSREAL fMax = -LSM_INFINITY;
		_ui32Min = _ui32Max = _ui32Total;

		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			const _tVectorType & vThis = (*reinterpret_cast<const _tVectorType *>(_pu8Points + I * _ui32Stride));
			CVector3 vThisVec = CVector3( static_cast<LSREAL>(vThis[0]), static_cast<LSREAL>(vThis[1]), static_cast<LSREAL>(vThis[2]) );
			LSREAL fProj = _vDir.Dot( vThisVec );

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
	 * Computes the eigensphere for a given set of non-contiguous points like in a vertex buffer.
	 *	The eigensphere is a close approximation of the smallest bounding sphere that can encapsulate
	 *	all of the given points.
	 *
	 * \param _pu8Points Points around which to create an enclosing spehere.
	 * \param _ui32Total Total number of points to which _pvPoints points.
	 * \param _ui32Stride Space between the starts of vertices in bytes.
	 * \return Returns a sphere that encloses all of the given points.
	 */
	template <typename _tVectorType>
	CSphere LSE_CALL CPhysUtils::EigenSphere( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride ) {
		// Sanity checking.
		if ( _ui32Total == 0 ) {
			return CSphere( CVector3( LSM_ZERO, LSM_ZERO, LSM_ZERO ), LSM_ZERO );
		}
		if ( _ui32Total == 1 ) {
			const _tVectorType & vPoint = (*reinterpret_cast<const _tVectorType *>(_pu8Points));
			return CSphere( CVector3( static_cast<LSREAL>(vPoint[0]), static_cast<LSREAL>(vPoint[1]), static_cast<LSREAL>(vPoint[2]) ), LSM_ZERO );
		}
		if ( _ui32Total == 2 ) {
			_tVectorType v1 = (*reinterpret_cast<const _tVectorType *>(_pu8Points));
			_tVectorType v2 = (*reinterpret_cast<const _tVectorType *>(_pu8Points + _ui32Stride));
			_tVectorType vAdded = _tVectorType( v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2] );
			_tVectorType vMuled = _tVectorType( vAdded[0] * CStd::ImpCast( vAdded[0], 0.5 ),
				vAdded[1] * CStd::ImpCast( vAdded[1], 0.5 ),
				vAdded[2] * CStd::ImpCast( vAdded[2], 0.5 ) );
			_tVectorType vSubbed = _tVectorType( v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2] );
			return CSphere( CVector3( static_cast<LSREAL>(vMuled[0]), static_cast<LSREAL>(vMuled[1]), static_cast<LSREAL>(vMuled[2]) ),
				CVector3( static_cast<LSREAL>(vSubbed[0]), static_cast<LSREAL>(vSubbed[1]), static_cast<LSREAL>(vSubbed[2]) ).Len() * LSM_HALF );
		}

		CSphere sReturn;
		CMatrix3x3 mM, mV;

		// Computes the covariance matrix M.
		mM.Covariance( _pu8Points, _ui32Total, _ui32Stride );

		// Decompose it into eigenvectors (in mV) and eigenvalues (in mM).
		Jacobi( mM, mV );

		_tVectorType vE;
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
		ExtremePointsAlongDirection<_tVectorType>( CVector3( static_cast<LSREAL>(vE[0]), static_cast<LSREAL>(vE[1]), static_cast<LSREAL>(vE[2]) ),
			_pu8Points, _ui32Total, _ui32Stride, ui32Min, ui32Max );

		const _tVectorType & vMinPoint = (*reinterpret_cast<const _tVectorType *>(_pu8Points + ui32Min * _ui32Stride));
		CVector3 vThisMin = CVector3( static_cast<LSREAL>(vMinPoint[0]), static_cast<LSREAL>(vMinPoint[1]), static_cast<LSREAL>(vMinPoint[2]) );
		const _tVectorType & vMaxPoint = (*reinterpret_cast<const _tVectorType *>(_pu8Points + ui32Max * _ui32Stride));
		CVector3 vThisMax = CVector3( static_cast<LSREAL>(vMaxPoint[0]), static_cast<LSREAL>(vMaxPoint[1]), static_cast<LSREAL>(vMaxPoint[2]) );
		
		sReturn.r = (vThisMax - vThisMin).Len() * LSM_HALF;
		sReturn.c = (vThisMax + vThisMin) * LSM_HALF;

		return sReturn;
	}

	/**
	 * Computes an approximate bounding sphere for a set of non-contiguous points like in a vertex buffer.
	 *	Uses the axis of maximum spread for the initial guess and then refines the sphere using the Ritter method.
	 *
	 * \param _pu8Points Points around which to create an enclosing spehere.
	 * \param _ui32Total Total number of points to which _pvPoints points.
	 * \param _ui32Stride Space between the starts of vertices in bytes.
	 * \return Returns a sphere that encloses all of the given points.
	 */
	template <typename _tVectorType>
	CSphere LSE_CALL CPhysUtils::RitterEigenSphere( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride ) {
		CSphere sRet = EigenSphere<_tVectorType>( _pu8Points, _ui32Total, _ui32Stride );

		// Refine it until it includes all points.
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			const _tVectorType & vPoint = (*reinterpret_cast<const _tVectorType *>(_pu8Points + I * _ui32Stride));
			SphereOfSphereAndPoint( sRet, CVector3( static_cast<LSREAL>(vPoint[0]), static_cast<LSREAL>(vPoint[1]), static_cast<LSREAL>(vPoint[2]) ) );
		}

		return sRet;
	}

}	// namespace lsp

#endif	// __LSP_PHYSUTILS_H__
