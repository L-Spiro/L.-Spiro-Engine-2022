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
 * Description: A min-max axis-aligned bounding box.
 */


#ifndef __LSP_LSP_AABB_H__
#define __LSP_LSP_AABB_H__

#include "../LSPPhysicsLib.h"
#include "Line/LSMRay3.h"
#include "Line/LSMLineSeg3.h"
#include "Matrix/LSMMatrix4x4.h"
#include "Plane/LSMPlane3.h"
#include "Vector/LSMVector3.h"

namespace lsp {

	// == Enumerations.
	/**
	 * The axis of each side of the AABB.
	 */
	enum LSP_AABB_CORNERS {
		LSP_AC_FORWARD_LEFT_BOTTOM,
		LSP_AC_FORWARD_LEFT_TOP,
		LSP_AC_FORWARD_RIGHT_BOTTOM,
		LSP_AC_FORWARD_RIGHT_TOP,

		LSP_AC_BACK_LEFT_BOTTOM,
		LSP_AC_BACK_LEFT_TOP,
		LSP_AC_BACK_RIGHT_BOTTOM,
		LSP_AC_BACK_RIGHT_TOP,
	};

	/**
	 * Class CAabb
	 * \brief A min-max axis-aligned bounding box.
	 *
	 * Description: A min-max axis-aligned bounding box.
	 */
	class CAabb {
		// All is public.  This class has no secrets.
	public :
		// == Functions.
		/**
		 * Make the box infinitely small.
		 */
		LSE_INLINE void LSE_CALL				MakeInfinitelySmall();

		/**
		 * Computes an AABB from a set of points given in the form of an array of CVector3's.
		 *
		 * \param _vPoints The array of points to enclose.
		 * \param _ui32Total The number of points in the array to which _vPoints points.
		 */
		void LSE_CALL							ComputeAabbFromPointArray( const CVector3 * _pvPoints, uint32_t _ui32Total );

		/**
		 * Computes an AABB from a set of points given in the form of a graphics-like vertex buffer with extra
		 *	data interleaved between the vertices.  The vertex type is also templated.
		 *
		 * \param _pu8Points The array of points to enclose.
		 * \param _ui32Total The number of points in the array to which _vPoints points.
		 * \param _ui32Stride Space between the starts of vertices in bytes.
		 */
		template <typename _tVectorType>
		void LSE_CALL							ComputeAabbFromPointArray( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride );


		/**
		 * Computes an AABB from an existing AABB with a matrix-stored translation.
		 *	The matrix must be row-major.
		 *
		 * \param _aabbOther The source bounding box to transform by the given matrix and store in this object.
		 * \param _mTrans The matrix used to transform _aabbOther into this bounding box.
		 */
		void LSE_CALL							ComputeAabbFromAabbAndMatrix( const CAabb &_aabbOther, const CMatrix4x4 &_mTrans );

		/**
		 * Change the size of the AABB to encompass another AABB.
		 *
		 * \param _aabbOther The bounding box to fully encompass by this bounding box.
		 */
		void LSE_CALL							Encompass( const CAabb &_aabbOther );

		/**
		 * Change the size of the AABB to encompass a point.
		 *
		 * \param _vOther The point to fully encompass by this bounding box.
		 */
		void LSE_CALL							Encompass( const CVector3 &_vOther );

		/**
		 * Expands the box in all 3 directions by the given amount.
		 *
		 * \param _fAmount The amount by which to expand the box.
		 */
		void LSE_CALL							Expand( LSREAL _fAmount );

		/**
		 * Fill an array of 8 vectors with the vertices of the bounding box represented by this object.
		 *
		 * \param _vPoints An array of at least 8 vectors to be filled with the points at the corners of
		 *	this bounding box.
		 */
		void LSE_CALL							GetBox( CVector3 _vPoints[8] ) const;

		/**
		 * Gets the center of the bounding box.
		 *
		 * \return Returns a point marking the center of this bounding box.
		 */
		CVector3 LSE_CALL						GetCenter() const;

		/**
		 * Gets its X size.
		 *
		 * \return Returns the length of this bounding box along the X axis.
		 */
		LSREAL LSE_CALL							GetSizeX() const;

		/**
		 * Gets its Y size.
		 *
		 * \return Returns the length of this bounding box along the Y axis.
		 */
		LSREAL LSE_CALL							GetSizeY() const;

		/**
		 * Gets its Z size.
		 *
		 * \return Returns the length of this bounding box along the Z axis.
		 */
		LSREAL LSE_CALL							GetSizeZ() const;

		/**
		 * Gets its volume.
		 *
		 * \return Returns the volume of this bounding box.
		 */
		LSREAL LSE_CALL							GetVolume() const;

		/**
		 * Gets the distance from one corner to the opposite corner.
		 *
		 * \return Returns the distance from one corner to the opposite corner.
		 */
		LSE_INLINE LSREAL LSE_CALL				GetMaxDist() const;



		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// DISTANCE TESTS
		// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
		// == Points.
		/**
		 * Gets the closest point on this AABB to a given point.
		 *
		 * \param _vPoint The point from which the closest point on this bounding box is to be calculated.
		 * \return Returns the closest point on this box from the given point.
		 */
		CVector3 LSE_CALL						ClosestPointToPoint( const CVector3 &_vPoint ) const;
		
		/**
		 * Gets the distance from the given point to this AABB.
		 *
		 * \param _vPoint The point from which the distance to this bounding box is to be calculated.
		 * \return Returns the squared distance from the closest point on this box to the given point to the
		 *	given point.
		 */
		LSREAL LSE_CALL							GetSqDistFromPoint( const CVector3 &_vPoint ) const;

		/**
		 * Returns a point of an AABB by a special bit mask.
		 *
		 * \param _ui32Mask The bit mask.
		 * \return Returns the point determined by the special bit mask.
		 */
		CVector3 LSE_CALL						GetCorner( uint32_t _ui32Mask ) const;


		// == Members.
		/** The minimum X-, Y-, and Z- axis values. */
		CVector3								m_vMin;

		/** The maximum X-, Y-, and Z- axis values. */
		CVector3								m_vMax;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Make the box infinitely small.
	 */
	LSE_INLINE void LSE_CALL CAabb::MakeInfinitelySmall() {
		m_vMin.x = m_vMin.y = m_vMin.z = LSM_INFINITY;
		m_vMax.x = m_vMax.y = m_vMax.z = -LSM_INFINITY;
	}

	/**
	 * Computes an AABB from a set of points given in the form of a graphics-like vertex buffer with extra
	 *	data interleaved between the vertices.  The vertex type is also templated.
	 *
	 * \param _pu8Points The array of points to enclose.
	 * \param _ui32Total The number of points in the array to which _vPoints points.
	 * \param _ui32Stride Space between the starts of vertices in bytes.
	 */
	template <typename _tVectorType>
	LSE_INLINE void LSE_CALL CAabb::ComputeAabbFromPointArray( const uint8_t * _pu8Points, uint32_t _ui32Total, uint32_t _ui32Stride ) {
		MakeInfinitelySmall();
		for ( uint32_t I = _ui32Total; I--; ) {
			const _tVectorType * pvThis = reinterpret_cast<const _tVectorType *>(_pu8Points);
			m_vMin.x = CStd::Min( m_vMin.x, static_cast<LSREAL>((*pvThis)[0]) );
			m_vMin.y = CStd::Min( m_vMin.y, static_cast<LSREAL>((*pvThis)[1]) );
			m_vMin.z = CStd::Min( m_vMin.z, static_cast<LSREAL>((*pvThis)[2]) );

			m_vMax.x = CStd::Max( m_vMax.x, static_cast<LSREAL>((*pvThis)[0]) );
			m_vMax.y = CStd::Max( m_vMax.y, static_cast<LSREAL>((*pvThis)[1]) );
			m_vMax.z = CStd::Max( m_vMax.z, static_cast<LSREAL>((*pvThis)[2]) );
			_pu8Points += _ui32Stride;
		}
	}

	/**
	 * Gets its X size.
	 *
	 * \return Returns the length of this bounding box along the X axis.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb::GetSizeX() const {
		return m_vMax.x - m_vMin.x;
	}

	/**
	 * Gets its Y size.
	 *
	 * \return Returns the length of this bounding box along the Y axis.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb::GetSizeY() const {
		return m_vMax.y - m_vMin.y;
	}

	/**
	 * Gets its Z size.
	 *
	 * \return Returns the length of this bounding box along the Z axis.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb::GetSizeZ() const {
		return m_vMax.z - m_vMin.z;
	}

	/**
	 * Gets its volume.
	 *
	 * \return Returns the volume of this bounding box.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb::GetVolume() const {
		return GetSizeX() * GetSizeY() * GetSizeZ();
	}

	/**
	 * Gets the distance from one corner to the opposite corner.
	 *
	 * \return Returns the distance from one corner to the opposite corner.
	 */
	LSE_INLINE LSREAL LSE_CALL CAabb::GetMaxDist() const {
		return (m_vMax - m_vMin).Len();
	}

}	// namespace lsp

#endif	// __LSP_LSP_AABB_H__
