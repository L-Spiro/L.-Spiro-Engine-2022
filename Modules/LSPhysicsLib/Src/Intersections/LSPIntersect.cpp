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
 * Description: Performs intersection tests between various primitive types.
 */

#include "LSPIntersect.h"
#include "Line/LSMRay3.h"
#include "Triangle/LSMTriangle3.h"
#include "Triangle/LSMTriangle3Ex.h"


namespace lsp {

	// == Functions.
	/**
	 * Returns true or false indicating intersection.  Upon intersection, the barycentric coordinates
	 *	of the intersection are also returned.
	 *
	 * \param _tTri The triangle to test for intersection.
	 * \param _rRay The ray to test against the given triangle for intersection.
	 * \param _fU The returned U barycentric coordinate.
	 * \param _fV The returned V barycentric coordinate.
	 * \param _fW The returned W barycentric coordinate.
	 * \param _fT Time of intersection along the ray.
	 * \return Returns true if the ray interesects the given triangle.  In the case of a true return, the barycentric coordinates are
	 *	also returned.
	 */
	LSBOOL LSE_CALL CIntersect::RayTriangle( const CTriangle3Ex &_tTri, const CRay3 &_rRay,
		LSREAL &_fU, LSREAL &_fV, LSREAL &_fW, LSREAL &_fT ) {
		CVector3 vAb = _tTri.B() - _tTri.A();
		CVector3 vAc = _tTri.C() - _tTri.A();

		//// Do a cool test to find out which triangles SHOULD pass but are not.
		//CVector3 vPVec;
		//vPVec.Cross( _rRay.dir, vAc );


		//LSREAL fD2 = vAb.Dot( vPVec );
		//if ( fD2 > static_cast<LSREAL>(-0.00001) ) { return false; }


		//LSREAL fInvDet = LSM_ONE / fD2;

		//CVector3 vTVec = _rRay.p - _tTri.A();
		//

		//_fU = vTVec.Dot( vPVec ) * fInvDet;
		//if ( _fU < static_cast<LSREAL>(-0.001) || _fU > static_cast<LSREAL>(1.001) ) { return false; }

		//CVector3 vQVec;
		//vQVec.Cross( vTVec, vAb );

		//_fV = _rRay.dir.Dot( vQVec ) * fInvDet;
		//if ( _fV < static_cast<LSREAL>(-0.001) || _fU + _fV > static_cast<LSREAL>(1.001) ) { return false; }

		//_fT = vAc.Dot( vQVec ) * fInvDet;

		//if ( _fT <= 0 ) { return false; }

		//_fW = LSM_ONE - _fU - _fV;



		

		// Computes the demoninator.  If it is less than or equal to 0, the segment is parallel to or
		//	pointing away from the triangle.
		//LSREAL fD = _rRay.dir.Dot( _tTri.N() );
		//if ( fD <= LSM_ZERO ) { return false; }

		//// Compute intersection T value of vPq with the triangle plane.  A ray intersects if
		////	0 <= T.  Avoid division until an intersection has been found.
		////CVector3 vAp = _rRay.p - _tTri.A();
		//CVector3 vAp = _tTri.A() - _rRay.p;

		//_fT = vAp.Dot( _tTri.N() );
		//if ( _fT < LSM_ZERO ) { return false; }

		//// Computer barycentric coordinate components and test within bounds.
		//CVector3 vE;
		////vE.CVector3::Cross( _rRay.dir, vAp );
		//vE.CVector3::Cross( vAp, _rRay.dir );
		//_fV = vAc.Dot( vE );
		//if ( _fV < LSM_ZERO ) { return false; }
		//_fW = -(vAb.Dot( vE ));
		////if ( _fW < LSM_ZERO ) { return false; }

		//// Segment intersects the triangle.  Perform the delayed division and compute the last
		////	barycentric coordinate.
		//LSREAL fOod = LSM_ONE / fD;
		//_fT *= fOod;
		//_fV *= fOod;
		//_fW *= fOod;
		//_fU = LSM_ONE - _fV - _fW;
		//return true;


		CVector3 vPVec = _rRay.dir % vAc;


		LSREAL fD = vAb.Dot( vPVec );
		if ( fD < LSM_FLT_EPSILON ) { return false; }


		LSREAL fInvDet = LSM_ONE / fD;

		CVector3 vTVec = _rRay.p - _tTri.A();


		_fU = vTVec.Dot( vPVec ) * fInvDet;
		if ( _fU < -LSM_FLT_EPSILON || _fU > static_cast<LSREAL>(1.0) + LSM_FLT_EPSILON ) { return false; }

		CVector3 vQVec = vTVec % vAb;

		_fV = _rRay.dir.Dot( vQVec ) * fInvDet;
		if ( _fV < -LSM_FLT_EPSILON || _fU + _fV > static_cast<LSREAL>(1.0) + LSM_FLT_EPSILON ) { return false; }

		_fT = vAc.Dot( vQVec ) * fInvDet;

		if ( _fT <= LSM_ZERO ) { return false; }

		_fW = LSM_ONE - _fU - _fV;

		return true;
	}

	/**
	 * Returns true or false indicating intersection.  Upon intersection, the barycentric coordinates
	 *	of the intersection are also returned.
	 *
	 * \param _tTri The triangle to test for intersection.
	 * \param _rRay The ray to test against the given triangle for intersection.
	 * \param _fU The returned U barycentric coordinate.
	 * \param _fV The returned V barycentric coordinate.
	 * \param _fW The returned W barycentric coordinate.
	 * \param _fT Time of intersection along the ray.
	 * \return Returns true if the ray interesects the given triangle.  In the case of a true return, the barycentric coordinates are
	 *	also returned.
	 */
	LSBOOL LSE_CALL CIntersect::RayTriangle( const CTriangle3 &_tTri, const CRay3 &_rRay,
		LSREAL &_fU, LSREAL &_fV, LSREAL &_fW, LSREAL &_fT ) {
		CVector3 vAb = _tTri.B() - _tTri.A();
		CVector3 vAc = _tTri.C() - _tTri.A();

		CVector3 vPVec = _rRay.dir % vAc;


		LSREAL fD = vAb.Dot( vPVec );
		if ( fD <= LSM_ZERO ) { return false; }


		LSREAL fInvDet = LSM_ONE / fD;

		CVector3 vTVec = _rRay.p - _tTri.A();


		_fU = vTVec.Dot( vPVec ) * fInvDet;
		if ( _fU < LSM_ZERO || _fU > LSM_ONE ) { return false; }

		CVector3 vQVec = vTVec % vAb;

		_fV = _rRay.dir.Dot( vQVec ) * fInvDet;
		if ( _fV < LSM_ZERO || _fU + _fV > LSM_ONE ) { return false; }

		_fT = vAc.Dot( vQVec ) * fInvDet;

		if ( _fT <= LSM_ZERO ) { return false; }

		_fW = LSM_ONE - _fU - _fV;

		return true;
	}

}	// namespace lsp
