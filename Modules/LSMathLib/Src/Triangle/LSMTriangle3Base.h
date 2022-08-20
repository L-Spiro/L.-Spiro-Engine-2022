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
 * Description: A 3D triangle with no additional data.  It is just 3 points assumed to be in counter-clockwise
 *	order.
 */


#ifndef __LSM_TRIANGLE3BASE_H__
#define __LSM_TRIANGLE3BASE_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CTriangle3Base
	 *
	 * Description: A 3D triangle.
	 */
	template <typename _tType, typename _tVec3Type>
	class CTriangle3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR						CTriangle3Base() {}
		LSE_INLINE LSE_CALLCTOR						CTriangle3Base( const CTriangle3Base<_tType, _tVec3Type> &_tTri ) :
			a( _tTri.a ),
			b( _tTri.b ),
			c( _tTri.c ) {
		}
		LSE_INLINE LSE_CALLCTOR						CTriangle3Base( const _tVec3Type &_vA, const _tVec3Type &_vB, const _tVec3Type &_vC ) :
			a( _vA ),
			b( _vB ),
			c( _vC ) {
		}


		// == Operators.
		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the vertex to retrieve.
		 * \return Returns the vertex at the given index, which must be valid.
		 */
		LSE_INLINE _tVec3Type & LSE_FCALL			operator [] ( uint32_t _ui32I ) { return reinterpret_cast<_tVec3Type *>(this)[_ui32I]; }

		/**
		 * Give array access.
		 *
		 * \param _ui32I The index of the vertex to retrieve.
		 * \return Returns the vertex at the given index, which must be valid.
		 */
		LSE_INLINE const _tVec3Type & LSE_FCALL		operator [] ( uint32_t _ui32I ) const { return reinterpret_cast<const _tVec3Type *>(this)[_ui32I]; }

		/**
		 * Equality.
		 *
		 * \param _tTri Object to test for equality.
		 * \return Returns true if the objects are equal.
		 */
		LSE_INLINE bool LSE_FCALL					operator == ( const CTriangle3Base &_tTri ) const {
			return a == _tTri.a && b == _tTri.b && c == _tTri.c;
		}

		/**
		 * Inequality.
		 *
		 * \param _tTri Object to test for inequality.
		 * \return Returns true if the objects are not equal.
		 */
		LSE_INLINE bool LSE_FCALL					operator != ( const CTriangle3Base &_tTri ) const {
			return a != _tTri.a || b != _tTri.b || c != _tTri.c;
		}


		// == Functions.
		/**
		 * Reverse the order of the triangle.
		 */
		LSE_INLINE void LSE_FCALL					ReverseOrder() {
			CStd::Swap( a, c );
		}

		/**
		 * Gets the first vertex of the triangle.
		 *
		 * \return Returns the first vertex in the triangle.
		 */
		LSE_INLINE const _tVec3Type & LSE_FCALL		A() const { return a; }

		/**
		 * Gets the second vertex of the triangle.
		 *
		 * \return Returns the second vertex in the triangle.
		 */
		LSE_INLINE const _tVec3Type & LSE_FCALL		B() const { return b; }

		/**
		 * Gets the third vertex of the triangle.
		 *
		 * \return Returns the third vertex in the triangle.
		 */
		LSE_INLINE const _tVec3Type & LSE_FCALL		C() const { return c; }

		/**
		 * Gets the barycentric coordinates of a given point.
		 *
		 * \param _vPoint The point on the plan of the triangle at which to get the barycentric
		 *	coordinate.
		 * \param _fU The returned U weight.
		 * \param _fV The returned V weight.
		 * \param _fW The returned W weight.
		 */
		void LSE_FCALL							Barycentric( const _tVec3Type &_vPoint,
			_tType &_fU, _tType &_fV, _tType &_fW ) const {
			// Non-normalized triangle normal.
			_tVec3Type vM = (b - a).Cross( c - a );
			// Nominators and reciprical denominator for U and V ratios.
			_tType fNu, fNv, fOod;
			// Absolute components for determining projection plane.
			_tType fX = CMathLib::Abs( vM.x ),
				fY = CMathLib::Abs( vM.y ),
				fZ = CMathLib::Abs( vM.z );

			// Compute areas in plane of largest projection.
			if ( fX >= fY && fX >= fZ ) {
				// X is the largest so project onto YZ.
				fNu = TriArea2D( _vPoint.y, _vPoint.z, b.y, b.z, c.y, c.z );
				fNv = TriArea2D( _vPoint.y, _vPoint.z, c.y, c.z, a.y, a.z );
				fOod = _tType( 1.0 ) / vM.x;
			}
			else if ( fY >= fX && fY >= fZ ) {
				// Y is the largest so project onto XZ.
				fNu = TriArea2D( _vPoint.x, _vPoint.z, b.x, b.z, c.x, c.z );
				fNv = TriArea2D( _vPoint.x, _vPoint.z, c.x, c.z, a.x, a.z );
				fOod = _tType( 1.0 ) / -vM.y;
			}
			else {
				// Z is the largest so project onto XY.
				fNu = TriArea2D( _vPoint.x, _vPoint.y, b.x, b.y, c.x, c.y );
				fNv = TriArea2D( _vPoint.x, _vPoint.y, c.x, c.y, a.x, a.y );
				fOod = _tType( 1.0 ) / vM.z;
			}
			_fU = fNu * fOod;
			_fV = fNv * fOod;
			_fW = _tType( 1.0 ) - _fU - _fV;
		}



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DISTANCE TESTS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// == Points.
		/**
		 * Gets the closest point on this triangle to a given point.
		 *
		 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
		 * \return Returns the closest point on this box from the given point.
		 */
		_tVec3Type LSE_FCALL						ClosestPointToPoint( const _tVec3Type &_vPoint ) const {
			_tVec3Type vAb = b - a;
			_tVec3Type vAc = c - a;
			_tVec3Type vBc = c - b;

			// Computes the parametric position s for projection P' of P on AB,
			//	P' = A + s*AB, s = snom/(snom+sdenom).
			_tVec3Type vAp = _vPoint - a;
			_tVec3Type vBp = _vPoint - b;
			_tVec3Type vCp = _vPoint - c;
			_tType fSNom = vAp.Dot( vAb ), fSDenom = -vBp.Dot( vAb );

			// Compute parametric position t for projection P' of P on AC,
			//	P' = A + t*AC, s = tnom/(tnom+tdenom).
			_tType fTNom = vAp.Dot( vAc ), fTDenom = -vCp.Dot( vAc );

			// Early out on vertex region.
			if ( fSNom <= _tType( 0.0 ) && fTNom <= _tType( 0.0 ) ) { return a; }


			// Computes the parametric position u for projection P' of P on BC,
			//	P' = B + u*BC, u = unom/(unom+udenom).
			_tType fUNom = vBp.Dot( vBc ), fUDenom = -vCp.Dot( vBc );

			// Now we can make 2 vertex-region early-outs.
			if ( fSDenom <= _tType( 0.0 ) && fUNom <= _tType( 0.0 ) ) { return b; }
			if ( fTDenom <= _tType( 0.0 ) && fUDenom <= _tType( 0.0 ) ) { return c; }


			// From here out, tests using vAp, vBp, and vCp will be inverted.
			vAp = -vAp;
			vBp = -vBp;
			// Save switching vCp for after the potential early-out.

			// P is outside (or on) AB if the triple scalar product [N PA PB] <= 0.
			_tVec3Type vN = vAb % vAc;
			_tType fVc = vN.Dot( vAp % vBp );


			// If P outside AB and within feature region of AB,
			//	return projection of P onto AB.
			if ( fVc <= _tType( 0.0 ) && fSNom >= _tType( 0.0 ) && fSDenom >= _tType( 0.0 ) ) {
				return a + vAb * (fSNom / (fSNom + fSDenom));
			}

			// Flip the postponed vCp.
			vCp = -vCp;

			// P is outside (or on) BC if the triple scalar product [N PB PC] <= 0.
			_tType fVa = vN.Dot( vBp % vCp );

			// If P outside AB and within feature region of AB,
			//	return projection of P onto AB.
			if ( fVa <= _tType( 0.0 ) && fUNom >= _tType( 0.0 ) && fUDenom >= _tType( 0.0 ) ) {
				return b + vBc * (fUNom / (fUNom + fUDenom));
			}


			// P is outside (or on) CA if the triple scalar product [N PC PA] <= 0.
			_tType fVb = vN.Dot( vCp % vAp );

			// If P outside CA and within feature region of CA,
			//	return projection of P onto CA.
			if ( fVb <= _tType( 0.0 ) && fTNom >= _tType( 0.0 ) && fTDenom >= _tType( 0.0 ) ) {
				return c + vAc * (fTNom / (fTNom + fTDenom));
			}

			// P must project inside a face region.  Compute Q using barycentric coordinates.
			_tType fDenom = _tType( 1.0 ) / (fVa + fVb + fVc);
			_tType fU = fVa * fDenom;
			_tType fV = fVb * fDenom;
			_tType fW = _tType( 1.0 ) - fU - fV;
			return a * fU + b * fV + c * fW;
		}
		
		/**
		 * Gets the distance from the given point to this triangle.
		 *
		 * \param _vPoint The point from which the distance to this triangle is to be calculated.
		 * \return Returns the squared distance from the closest point on this triangle to the
		 *	given point.
		 */
		_tType LSE_FCALL							GetSqDistFromPoint( const _tVec3Type &_vPoint ) const {
			// Get the closest point and return the squared length.
			return ClosestPointToPoint( _vPoint ).LenSq();
		}

		/**
		 * Determines if a point is inside the triangle or not.  The point must actually be inside the triangle and
		 *	not on any of its edges.
		 *
		 * \param _vPoint Point to test for being contained by this triangle.
		 * \return Returns true if the point is inside this triangle.
		 */
		LSBOOL LSE_FCALL							PointIsInTriangle( const _tVec3Type &_vPoint ) const {
			_tVec3Type vA = A() - _vPoint;
			_tVec3Type vB = B() - _vPoint;
			_tVec3Type vC = C() - _vPoint;
			_tType fAb = vA.Dot( vB );
			_tType fAc = vA.Dot( vC );
			_tType fBc = vB.Dot( vC );
			_tType fCc = vC.Dot( vC );
			_tType fTest = fBc * fAc - fCc * fAb;
			if ( fTest <= _tType( 0.0 ) ) { return false; }

			_tType fBb = vB.Dot( vB );
			fTest = fAb * fBc - fBb * fAc;
			if ( fTest <= _tType( 0.0 ) ) { return false; }
			return true;
		}

		/**
		 * Determines if a point is inside the triangle or not.
		 *
		 * \param _vPoint Point to test for being contained by this triangle.
		 * \return Returns true if the point is inside this triangle.
		 */
		LSBOOL LSE_FCALL							PointIsInTriangleOrEdges( const _tVec3Type &_vPoint ) const {
			_tVec3Type vA = A() - _vPoint;
			_tVec3Type vB = B() - _vPoint;
			_tVec3Type vC = C() - _vPoint;
			_tType fAb = vA.Dot( vB );
			_tType fAc = vA.Dot( vC );
			_tType fBc = vB.Dot( vC );
			_tType fCc = vC.Dot( vC );
			if ( fBc * fAc - fCc * fAb < _tType( 0.0 ) ) { return false; }

			_tType fBb = vB.Dot( vB );
			if ( fAb * fBc - fBb * fAc < _tType( 0.0 ) ) { return false; }
			return true;
		}


		// == Members.
#pragma pack( push, 1 )
		/** Point A. */
		_tVec3Type									a;

		/** Point B. */
		_tVec3Type									b;

		/** Point C. */
		_tVec3Type									c;
#pragma pack( pop )


	protected :
		// == Functions.
		/**
		 * Gets the area of a given 2D triangle.  Used for the Barycentric() equation.
		 *
		 * \param _fX1 X of the first triangle.
		 * \param _fY1 Y of the first triangle.
		 * \param _fX2 X of the second triangle.
		 * \param _fY2 Y of the second triangle.
		 * \param _fX3 X of the third triangle.
		 * \param _fY3 Y of the third triangle.
		 * \return Returns the area of the given 2D triangle.
		 */
		LSE_INLINE static _tType LSE_FCALL			TriArea2D( _tType _fX1, _tType _fY1,
			_tType _fX2, _tType _fY2,
			_tType _fX3, _tType _fY3 ) {
			return (_fX1 - _fX2) * (_fY2 - _fY3) - (_fX2 - _fX3) * (_fY1 - _fY2);
		}
	};

}	// namespace lsm

#endif	// __LSM_TRIANGLE3BASE_H__
