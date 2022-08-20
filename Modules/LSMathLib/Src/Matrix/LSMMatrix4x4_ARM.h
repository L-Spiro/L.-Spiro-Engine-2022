/**
 * Copyright L. Spiro 2015
 * All rights reserved.
 *
 * Written by:	Shawn ( L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: An ARM-optimized matrix class.
 */


#ifndef __LSM_MATRIX4X4_ARM_H__
#define __LSM_MATRIX4X4_ARM_H__

#ifdef __ARM_NEON__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3.h"
#include "../Vector/LSMVector4.h"
#include "LSMMatrix3x3.h"
#include "LSMMatrix4x4Base.h"

#include <arm_neon.h>

namespace lsm {

	/**
	 * Class CMatrix4x4_ARM
	 * \brief Specialization of the template base class for use with standard LSREAL matrices.
	 *
	 * Description: Specialization of the template base class for use with standard LSREAL matrices.
	 */
	LSE_ALIGN(  16 )
	class CMatrix4x4_ARM : public CMatrix4x4Base<LSREAL, CVector3, CVector4> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR									CMatrix4x4_ARM();
		LSE_CALLCTOR									CMatrix4x4_ARM( const CMatrix4x4_ARM &_m44bOther );
		LSE_CALLCTOR									CMatrix4x4_ARM( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_m44bOther );
		LSE_CALLCTOR									CMatrix4x4_ARM( float _t11, float _t12, float _t13, float _t14,
			float _t21, float _t22, float _t23, float _t24,
			float _t31, float _t32, float _t33, float _t34,
			float _t41, float _t42, float _t43, float _t44 );
		LSE_CALLCTOR									CMatrix4x4_ARM( CMatrix4x4_ARM _m44bOther, LSBOOL _bTranspose );
		explicit LSE_CALLCTOR							CMatrix4x4_ARM( const float * _ptArray );


		// == Operators.
		/**
		 * Multiply this matrix with another, returning the result as M = this * _mOther.
		 *
		 * \param _mOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		CMatrix4x4_ARM LSE_FCALL						operator * ( const CMatrix4x4_ARM &_mOther ) const;
		
		/**
		 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
		 *
		 * \param _vOther The vector to transform by this matrix.
		 * \return Returns the vector transformed by this matrix.
		 */
		CVector4 LSE_FCALL								operator * ( const CVector4 &_vOther ) const;
		
		/**
		 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
		 *
		 * \param _vOther The vector to transform by this matrix.
		 * \return Returns the vector transformed by this matrix.
		 */
		CVector3 LSE_FCALL								operator * ( const CVector3 &_vOther ) const;

		/**
		 * Assignment operator.
		 *
		 * \param _mOther The matrix to copy into this matrix object.
		 * \return Returns this matrix object after copying the given matrix into it.
		 */
		CMatrix4x4_ARM & LSE_FCALL						operator = ( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_mOther );
		
		
		// == Functions.
		/**
		 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
		 *
		 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
		 */
		CMatrix3x3 LSE_CALL								Get3x3Matrix() const;

		/**
		 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
		 *
		 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
		 */
		CMatrix3x3 LSE_CALL								Get3x3MatrixTranspose() const;
		
		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this object.
		 */
		CMatrix4x4_ARM & LSE_FCALL						Transpose();
		
		/**
		 * Copies the transpose of a matrix into another matrix.
		 *
		 * \return Returns the copied object.
		 */
		CMatrix4x4_ARM LSE_FCALL						GetTranspose() const;
		
		/**
		 * Transforms vector (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v4bIn The vector to transform.
		 */
		static CVector4 LSE_FCALL						MultiplyVec4( const CMatrix4x4_ARM &_m44bMat, const CVector4 &_v4bIn );
		
		/**
		 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _pv4bIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pv4bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL							MultiplyVec4Batch( const CMatrix4x4_ARM &_m44bMat,
			const CVector4 * _pv4bIn, uint32_t _ui32Total,
			CVector4 * _pv4bOut );
		
		
		// == Members.
		/** An identity matrix. */
		static const CMatrix4x4_ARM						m_mIdentity;
		
	private :
		typedef CMatrix4x4Base<LSREAL, CVector3, CVector4>
														Parent;
	} LSE_POSTALIGN( 16 );


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM() {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM( const CMatrix4x4_ARM &_m44bOther ) :
		Parent( _m44bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_m44bOther ) :
		Parent( _m44bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM( float _t11, float _t12, float _t13, float _t14,
		float _t21, float _t22, float _t23, float _t24,
		float _t31, float _t32, float _t33, float _t34,
		float _t41, float _t42, float _t43, float _t44 ) :
		Parent( _t11, _t12, _t13, _t14,
			_t21, _t22, _t23, _t24,
			_t31, _t32, _t33, _t34,
			_t41, _t42, _t43, _t44 ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM( CMatrix4x4_ARM _m44bOther, LSBOOL _bTranspose ) :
		Parent( _m44bOther, _bTranspose ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_ARM::CMatrix4x4_ARM( const float * _ptArray ) :
		Parent( _ptArray ) {
	}

	// == Operators.
	/**
	 * Multiply this matrix with another, returning the result as M = this * _mOther.
	 *
	 * \param _mOther The matrix by which to multiply this matrix.
	 * \return Returns the result of the matrix combination.
	 */
	LSE_INLINE CMatrix4x4_ARM LSE_FCALL CMatrix4x4_ARM::operator * ( const CMatrix4x4_ARM &_mOther ) const {
		float32x4x4_t iMatrixLeft = (*reinterpret_cast<const float32x4x4_t *>(&_mOther));
		float32x4x4_t iMatrixRight = (*reinterpret_cast<const float32x4x4_t *>(this));
		float32x4x4_t mOut;

		mOut.val[0] = vmulq_n_f32( iMatrixLeft.val[0], vgetq_lane_f32( iMatrixRight.val[0], 0 ) );
		mOut.val[1] = vmulq_n_f32( iMatrixLeft.val[0], vgetq_lane_f32( iMatrixRight.val[1], 0 ) );
		mOut.val[2] = vmulq_n_f32( iMatrixLeft.val[0], vgetq_lane_f32( iMatrixRight.val[2], 0 ) );
		mOut.val[3] = vmulq_n_f32( iMatrixLeft.val[0], vgetq_lane_f32( iMatrixRight.val[3], 0 ) );

		mOut.val[0] = vmlaq_n_f32( mOut.val[0], iMatrixLeft.val[1], vgetq_lane_f32( iMatrixRight.val[0], 1 ) );
		mOut.val[1] = vmlaq_n_f32( mOut.val[1], iMatrixLeft.val[1], vgetq_lane_f32( iMatrixRight.val[1], 1 ) );
		mOut.val[2] = vmlaq_n_f32( mOut.val[2], iMatrixLeft.val[1], vgetq_lane_f32( iMatrixRight.val[2], 1 ) );
		mOut.val[3] = vmlaq_n_f32( mOut.val[3], iMatrixLeft.val[1], vgetq_lane_f32( iMatrixRight.val[3], 1 ) );

		mOut.val[0] = vmlaq_n_f32( mOut.val[0], iMatrixLeft.val[2], vgetq_lane_f32( iMatrixRight.val[0], 2 ) );
		mOut.val[1] = vmlaq_n_f32( mOut.val[1], iMatrixLeft.val[2], vgetq_lane_f32( iMatrixRight.val[1], 2 ) );
		mOut.val[2] = vmlaq_n_f32( mOut.val[2], iMatrixLeft.val[2], vgetq_lane_f32( iMatrixRight.val[2], 2 ) );
		mOut.val[3] = vmlaq_n_f32( mOut.val[3], iMatrixLeft.val[2], vgetq_lane_f32( iMatrixRight.val[3], 2 ) );

		mOut.val[0] = vmlaq_n_f32( mOut.val[0], iMatrixLeft.val[3], vgetq_lane_f32( iMatrixRight.val[0], 3 ) );
		mOut.val[1] = vmlaq_n_f32( mOut.val[1], iMatrixLeft.val[3], vgetq_lane_f32( iMatrixRight.val[1], 3 ) );
		mOut.val[2] = vmlaq_n_f32( mOut.val[2], iMatrixLeft.val[3], vgetq_lane_f32( iMatrixRight.val[2], 3 ) );
		mOut.val[3] = vmlaq_n_f32( mOut.val[3], iMatrixLeft.val[3], vgetq_lane_f32( iMatrixRight.val[3], 3 ) );

		return (*reinterpret_cast<CMatrix4x4_ARM *>(&mOut));
	}
	
	/**
	 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
	 *
	 * \param _vOther The vector to transform by this matrix.
	 * \return Returns the vector transformed by this matrix.
	 */
	LSE_INLINE CVector4 LSE_FCALL CMatrix4x4_ARM::operator * ( const CVector4 &_vOther ) const {
		float32x4x4_t iMatrix = (*reinterpret_cast<const float32x4x4_t *>(this));
		

		iMatrix.val[0] = vmulq_n_f32( iMatrix.val[0], static_cast<float32_t>(_vOther.x) );
		iMatrix.val[1] = vmulq_n_f32( iMatrix.val[1], static_cast<float32_t>(_vOther.y) );
		iMatrix.val[2] = vmulq_n_f32( iMatrix.val[2], static_cast<float32_t>(_vOther.z) );
		iMatrix.val[3] = vmulq_n_f32( iMatrix.val[3], static_cast<float32_t>(_vOther.w) );

		iMatrix.val[0] = vaddq_f32( iMatrix.val[0], iMatrix.val[1] );
		iMatrix.val[2] = vaddq_f32( iMatrix.val[2], iMatrix.val[3] );

		float32x4_t vRet = vaddq_f32( iMatrix.val[0], iMatrix.val[2] );

		return (*reinterpret_cast<CVector4 *>(&vRet));
	}
	
	/**
	 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
	 *
	 * \param _vOther The vector to transform by this matrix.
	 * \return Returns the vector transformed by this matrix.
	 */
	LSE_INLINE CVector3 LSE_FCALL CMatrix4x4_ARM::operator * ( const CVector3 &_vOther ) const {
		CVector4 vTemp = (*this) * CVector4( _vOther.x, _vOther.y, _vOther.z, 1.0f );
		return vTemp.Vec3();
	}

	/**
	 * Assignment operator.
	 *
	 * \param _mOther The matrix to copy into this matrix object.
	 * \return Returns this matrix object after copying the given matrix into it.
	 */
	LSE_INLINE CMatrix4x4_ARM & LSE_FCALL CMatrix4x4_ARM::operator = ( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_mOther ) {
		float32x4x4_t * pfThis = reinterpret_cast<float32x4x4_t *>(this);
		const float32x4x4_t * pfSrc = reinterpret_cast<const float32x4x4_t *>(&_mOther);
		(*pfThis) = (*pfSrc);
		return (*this);
	}
	
	// == Functions.
	/**
	 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 *
	 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 */
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4_ARM::Get3x3Matrix() const {
		CMatrix3x3 mRet;
		mRet._11 = _11;
		mRet._12 = _12;
		mRet._13 = _13;

		mRet._21 = _21;
		mRet._22 = _22;
		mRet._23 = _23;

		mRet._31 = _31;
		mRet._32 = _32;
		mRet._33 = _33;

		return mRet;
	}

	/**
	 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
	 *
	 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
	 */
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4_ARM::Get3x3MatrixTranspose() const {
		CMatrix3x3 mRet;
		mRet._11 = _11;
		mRet._12 = _21;
		mRet._13 = _31;

		mRet._21 = _12;
		mRet._22 = _22;
		mRet._23 = _32;

		mRet._31 = _13;
		mRet._32 = _23;
		mRet._33 = _33;

		return mRet;
	}
	
	/**
	 * Transpose a matrix in place.
	 *
	 * \return Returns this object.
	 */
	LSE_INLINE CMatrix4x4_ARM & LSE_FCALL CMatrix4x4_ARM::Transpose() {
		(*this) = GetTranspose();
		return (*this);
	}

	/**
	 * Copies the transpose of a matrix into another matrix.
	 *
	 * \return Returns the copied object.
	 */
	LSE_INLINE CMatrix4x4_ARM LSE_FCALL CMatrix4x4_ARM::GetTranspose() const {
		float32x4x4_t mTemp = vld4q_f32( reinterpret_cast<const float *>(this) );
		return (*reinterpret_cast<CMatrix4x4_ARM *>(&mTemp));
	}
	
	/**
	 * Transforms vector (x, y, z, w) by a given matrix.
	 *
	 * \param _m44bMat The matrix by which to transform the given vector.
	 * \param _v4bIn The vector to transform.
	 */
	LSE_INLINE CVector4 LSE_FCALL CMatrix4x4_ARM::MultiplyVec4( const CMatrix4x4_ARM &_m44bMat, const CVector4 &_v4bIn ) {
		float32x4x4_t iMatrix = (*reinterpret_cast<const float32x4x4_t *>(&_m44bMat));
		

		iMatrix.val[0] = vmulq_n_f32( iMatrix.val[0], static_cast<float32_t>(_v4bIn.x) );
		iMatrix.val[1] = vmulq_n_f32( iMatrix.val[1], static_cast<float32_t>(_v4bIn.y) );
		iMatrix.val[2] = vmulq_n_f32( iMatrix.val[2], static_cast<float32_t>(_v4bIn.z) );
		iMatrix.val[3] = vmulq_n_f32( iMatrix.val[3], static_cast<float32_t>(_v4bIn.w) );

		iMatrix.val[0] = vaddq_f32( iMatrix.val[0], iMatrix.val[1] );
		iMatrix.val[2] = vaddq_f32( iMatrix.val[2], iMatrix.val[3] );

		float32x4_t vRet = vaddq_f32( iMatrix.val[0], iMatrix.val[2] );

		return (*reinterpret_cast<CVector4 *>(&vRet));
	}
	
	/**
	 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
	 *
	 * \param _m44bMat The matrix by which to transform the given vector.
	 * \param _pv4bIn The vectors to transform.
	 * \param _ui32Total Number of vectors to transform.
	 * \param _pv4bOut Holds the transformed vectors, which must not be the same as the input vector list.
	 */
	LSE_INLINE void LSE_FCALL CMatrix4x4_ARM::MultiplyVec4Batch( const CMatrix4x4_ARM &_m44bMat,
		const CVector4 * _pv4bIn, uint32_t _ui32Total,
		CVector4 * _pv4bOut ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			LSE_PREFETCH_LINE_WRITE( _pv4bOut );
			(*_pv4bOut++) = MultiplyVec4( _m44bMat, _pv4bIn[I] );
		}
	}

}	// namespace lsm

#endif	// #ifdef __ARM_NEON__

#endif	// __LSM_MATRIX4X4_ARM_H__
