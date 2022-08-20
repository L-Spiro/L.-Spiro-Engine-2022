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
 * Description: A 3D line segment stored as 2 points.
 */



#ifndef __LSM_LINESEG3BASE_H__
#define __LSM_LINESEG3BASE_H__

#include "../LSMMathLib.h"

namespace lsm {

	/**
	 * Class CLineSeg3Base
	 * \brief A 3D line segment stored as 2 points.
	 *
	 * Description: A 3D line segment stored as 2 points.
	 */
	template <typename _tType, typename _tVec3Type>
	class CLineSeg3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_INLINE LSE_CALLCTOR			CLineSeg3Base() {
		}
		LSE_INLINE LSE_CALLCTOR			CLineSeg3Base( const CLineSeg3Base<_tType, _tVec3Type> &_lsbLine ) {
			(*this) = _lsbLine;
		}
		LSE_INLINE LSE_CALLCTOR			CLineSeg3Base( const _tVec3Type &_vStart, const _tVec3Type &_vEnd ) {
			p = _vStart;
			q = _vEnd;
		}


		// == Functions.
		/**
		 * Gets the closest point on the line segment to the given point.
		 *	Also returns t for the position of that point along this line.
		 *
		 * \param _vPoint Point to test against this line segment.
		 * \param _tT Holds the returned time value of the returned point.
		 * \return Returns the closest point on the line segment to the given point and the time (t)
		 *	for the position of the point along the line segment.
		 */
		_tVec3Type LSE_FCALL			ClosestPointOnLineSegToPoint( const _tVec3Type &_vPoint, _tType &_tT ) const {
			_tVec3Type vAb = q - p;
			// Project C onto AB but deferring divide.
			_tT = vAb.Dot( _vPoint - p );
			if ( _tT <= static_cast<_tType>(0.0) ) {
				// Beyond point A.  Return the starting point of the segment.
				_tT = static_cast<_tType>(0.0);
				return p;
			}
			else {
				_tType fDenom = vAb.LenSq();
				if ( _tT >= fDenom ) {
					// Beyond point B.  Return the end of the segment.
					_tT = static_cast<_tType>(1.0);
					return q;
				}
				// Somewhere in the middle.  Return it the normal way (with the deferred divide here).
				_tT /= fDenom;
				return p + (vAb * _tT);
			}
		}

		/**
		 * Returns the squared distance between this segment and the given point.
		 *
		 * \param _vPoint The point to which the squared distance from this segment is to be calculated.
		 * \return Returns the shortest squared distance from this line segment to the given point.
		 */
		_tType LSE_FCALL				SqDistToPoint( const _tVec3Type &_vPoint ) const {
			_tVec3Type vAb = q - p;
			_tVec3Type vAc = _vPoint - p;
			

			_tType fE = vAb.Dot( vAc );
			if ( fE <= static_cast<_tType>(0.0) ) { return vAc.LenSq(); }
			_tType fF = vAb.LenSq();
			_tVec3Type vBc = _vPoint - q;
			if ( fE >= fF ) { return vBc.LenSq(); }

			return vAc.LenSq() - fE * fE / fF;
		}

		/**
		 * Gets the squared distance between this segment and another segment.  Also returns t for
		 *	the position of that point along both segments.
		 *
		 * \param _lsbLine Line segment to test against this line segment.
		 * \param _tS Holds the returned time value of the point along this segment.
		 * \param _tT Holds the returned time value of the point along the given segment.
		 * \param _tRealS Holds the unclamped time of intersection on this line segment.
		 * \param _tRealT Holds the unclamped time of intersection on the second line segment.
		 * \param _v0 Holds the closest point on this line segment to the given line segment.
		 * \param _v1 Holds the closest point on the given line segment to this line segment.
		 * \return Returns the squared distance between the line segments and the times t and s along each segment that represents
		 *	the closest points between them.
		 */
		_tType LSE_FCALL				ClosestPointOnLineSegToLineSeg( const CLineSeg3Base<_tType, _tVec3Type> &_lsbLine, _tType &_tS, _tType &_tT,
			_tType &_tRealS, _tType &_tRealT, _tVec3Type &_v0, _tVec3Type &_v1 ) const {
			_tVec3Type vD1 = q - p;
			_tVec3Type vD2 = _lsbLine.q - _lsbLine.p;
			_tVec3Type vR = p - _lsbLine.p;
			_tType fA = vD1.LenSq();
			_tType fE = vD2.LenSq();
			_tType fF = vD2.Dot( vR );

			// If both segments degenerate to a point.
			if ( fA <= static_cast<_tType>(LSM_REAL_EPSILON) && fE <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
				_tT = _tS = static_cast<_tType>(0.0);
				// No valid value for _tRealS and _tRealT.
				_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
				_v0 = p;
				_v1 = _lsbLine.p;
				return vR.LenSq();
			}
			// If this segment degenerates into a point.
			if ( fA <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
				_tS = static_cast<_tType>(0.0);
				// No valid value for _tRealS and _tRealT.
				_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
				_tT = CMathLib::Clamp( fF / fE, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
			}
			else {
				_tType fC = vD1.Dot( vR );
				// If the other segment degenerates into a point.
				if ( fE <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
					_tT = static_cast<_tType>(0.0);
					// No valid value for _tRealS and _tRealT.
					_tRealS = _tRealT = static_cast<_tType>(LSM_INFINITY);
					_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
				}
				else {
					// Neither are degenerate.  This is most cases.
					_tType fB = vD1.Dot( vD2 );
					_tType fDenom = fA * fE - fB * fB;

					// If not parallel.
					if ( fDenom != static_cast<_tType>(0.0) ) {
						_tRealS = (fB * fF - fC * fE) / fDenom;
						_tS = CMathLib::Clamp( _tRealS, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					// Otherwise pick an arbitrary point (0 here).
					else {
						_tRealS = _tS = static_cast<_tType>(0.0);
					}

					_tT = _tRealT = (fB * _tS + fF) / fE;

					// If _tT is in the range [0,1] then we are done.  Otherwise
					//	adjusting T requires updating S also.
					if ( _tT < static_cast<_tType>(0.0) ) {
						_tT = static_cast<_tType>(0.0);
						_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					else if ( _tT > static_cast<_tType>(1.0) ) {
						_tT = static_cast<_tType>(1.0);
						_tS = CMathLib::Clamp( (fB - fC) / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}

				}
			}

			_v0 = p + vD1 * _tS;
			_v1 = _lsbLine.p + vD2 * _tT;
			_tType fDist = (_v1 - _v0).LenSq();
			// If there was a degenerate situation, _tRealS/_tRealT will only be valid in the case where the line segments actually
			//	intersect.  In a degenerate situation, one or both of the line segments is treated as a point, so they cannot
			//	be calculated except if the degenerate point is on the valid line segment, or both degenerate points are the same.
			if ( _tRealS == static_cast<_tType>(LSM_INFINITY) && fDist <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
				_tRealS = _tS;
				_tRealT = _tT;
			}
			return fDist;
		}

		/**
		 * Gets the squared distance between this segment and another segment.  Also returns t for
		 *	the position of that point along both segments.
		 *
		 * \param _lsbLine Line segment to test against this line segment.
		 * \param _tS Holds the returned time value of the point along this segment.
		 * \param _tT Holds the returned time value of the point along the given segment.
		 * \return Returns the squared distance between the line segments and the times t and s along each segment that represents
		 *	the closest points between them.
		 */
		_tType LSE_FCALL				ClosestPointOnLineSegToLineSeg( const CLineSeg3Base<_tType, _tVec3Type> &_lsbLine, _tType &_tS, _tType &_tT ) const {
			_tVec3Type vD1 = q - p;
			_tVec3Type vD2 = _lsbLine.q - _lsbLine.p;
			_tVec3Type vR = p - _lsbLine.p;
			_tType fA = vD1.LenSq();
			_tType fE = vD2.LenSq();
			_tType fF = vD2.Dot( vR );

			_tType tRealS, tRealT;

			// If both segments degenerate to a point.
			if ( fA <= static_cast<_tType>(LSM_REAL_EPSILON) && fE <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
				_tT = _tS = static_cast<_tType>(0.0);
				return vR.LenSq();
			}
			// If this segment degenerates into a point.
			if ( fA <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
				_tS = static_cast<_tType>(0.0);
				_tT = CMathLib::Clamp( fF / fE, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
			}
			else {
				_tType fC = vD1.Dot( vR );
				// If the other segment degenerates into a point.
				if ( fE <= static_cast<_tType>(LSM_REAL_EPSILON) ) {
					_tT = static_cast<_tType>(0.0);
					_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
				}
				else {
					// Neither are degenerate.  This is most cases.
					_tType fB = vD1.Dot( vD2 );
					_tType fDenom = fA * fE - fB * fB;

					// If not parallel.
					if ( fDenom != static_cast<_tType>(0.0) ) {
						tRealS = (fB * fF - fC * fE) / fDenom;
						_tS = CMathLib::Clamp( tRealS, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					// Otherwise pick an arbitrary point (0 here).
					else {
						tRealS = _tS = static_cast<_tType>(0.0);
					}

					_tT = tRealT = (fB * _tS + fF) / fE;

					// If _tT is in the range [0,1] then we are done.  Otherwise
					//	adjusting T requires updating S also.
					if ( _tT < static_cast<_tType>(0.0) ) {
						_tT = static_cast<_tType>(0.0);
						_tS = CMathLib::Clamp( -fC / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}
					else if ( _tT > static_cast<_tType>(1.0) ) {
						_tT = static_cast<_tType>(1.0);
						_tS = CMathLib::Clamp( (fB - fC) / fA, static_cast<_tType>(0.0), static_cast<_tType>(1.0) );
					}

				}
			}

			_tVec3Type v0 = p + vD1 * _tS;
			_tVec3Type v1 = _lsbLine.p + vD2 * _tT;
			_tType fDist = (v1 - v0).LenSq();
			return fDist;
		}
		
		/**
		 * Gets the length of the line segment.
		 *
		 * \return Returns the length of this line segment.
		 */
		_tType LSE_FCALL				Len() const {
			return (p - q).Len();
		}

		/**
		 * Gets the squared length of the line segment.
		 *
		 * \return Returns the squared length of this line segment.
		 */
		_tType LSE_FCALL				LenSq() const {
			return (p - q).LenSq();
		}


		// == Members.
		/**
		 * Starting point.
		 */
		_tVec3Type						p;

		/**
		 * Ending point.
		 */
		_tVec3Type						q;
	};

}	// namespace lsm

#endif	// __LSM_LINESEG3BASE_H__
