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
 * Description: An SSE3-optimized matrix class.
 */

#ifndef __LSM_MATRIX4X4_SSE3_H__
#define __LSM_MATRIX4X4_SSE3_H__

#include "../LSMMathLib.h"

#ifdef LSE_X64

#include "../Vector/LSMVector3.h"
#include "../Vector/LSMVector4.h"
#include "LSMMatrix3x3.h"
#include "LSMMatrix4x4Base.h"

namespace lsm {

	/**
	 * Class CMatrix4x4_SSE3
	 * \brief Specialization of the template base class for use with standard LSREAL matrices.
	 *
	 * Description: Specialization of the template base class for use with standard LSREAL matrices.
	 */
	LSE_ALIGN(  16 )
	class CMatrix4x4_SSE3 : public CMatrix4x4Base<LSREAL, CVector3, CVector4> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR									CMatrix4x4_SSE3();
		LSE_CALLCTOR									CMatrix4x4_SSE3( const CMatrix4x4_SSE3 &_m44bOther );
		LSE_CALLCTOR									CMatrix4x4_SSE3( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_m44bOther );
		LSE_CALLCTOR									CMatrix4x4_SSE3( float _t11, float _t12, float _t13, float _t14,
			float _t21, float _t22, float _t23, float _t24,
			float _t31, float _t32, float _t33, float _t34,
			float _t41, float _t42, float _t43, float _t44 );
		LSE_CALLCTOR									CMatrix4x4_SSE3( CMatrix4x4_SSE3 _m44bOther, LSBOOL _bTranspose );
		explicit LSE_CALLCTOR							CMatrix4x4_SSE3( const float * _ptArray );


		// == Operators.
		/**
		 * Multiply this matrix with another, returning the result as M = this * _mOther.
		 *
		 * \param _mOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		CMatrix4x4_SSE3 LSE_FCALL						operator * ( const CMatrix4x4_SSE3 &_mOther ) const;
		
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
		CMatrix4x4_SSE3 & LSE_FCALL						Transpose();
		
		/**
		 * Copies the transpose of a matrix into another matrix.
		 *
		 * \return Returns the copied object.
		 */
		CMatrix4x4_SSE3 LSE_FCALL						GetTranspose() const;
		
		/**
		 * Transforms vector (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v4bIn The vector to transform.
		 */
		static CVector4 LSE_FCALL						MultiplyVec4( const CMatrix4x4_SSE3 &_m44bMat, const CVector4 &_v4bIn );
		
		/**
		 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _pv4bIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pv4bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL							MultiplyVec4Batch( const CMatrix4x4_SSE3 &_m44bMat,
			const CVector4 * _pv4bIn, uint32_t _ui32Total,
			CVector4 * _pv4bOut );
		
		
		// == Members.
		/** An identity matrix. */
		static const CMatrix4x4_SSE3					m_mIdentity;
		
	private :
		typedef CMatrix4x4Base<LSREAL, CVector3, CVector4>
														Parent;
	} LSE_POSTALIGN( 16 );


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3() {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3( const CMatrix4x4_SSE3 &_m44bOther ) :
		Parent( _m44bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_m44bOther ) :
		Parent( _m44bOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3( float _t11, float _t12, float _t13, float _t14,
		float _t21, float _t22, float _t23, float _t24,
		float _t31, float _t32, float _t33, float _t34,
		float _t41, float _t42, float _t43, float _t44 ) :
		Parent( _t11, _t12, _t13, _t14,
			_t21, _t22, _t23, _t24,
			_t31, _t32, _t33, _t34,
			_t41, _t42, _t43, _t44 ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3( CMatrix4x4_SSE3 _m44bOther, LSBOOL _bTranspose ) :
		Parent( _m44bOther, _bTranspose ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4_SSE3::CMatrix4x4_SSE3( const float * _ptArray ) :
		Parent( _ptArray ) {
	}

	// == Operators.
	/**
	 * Multiply this matrix with another, returning the result as M = this * _mOther.
	 *
	 * \param _mOther The matrix by which to multiply this matrix.
	 * \return Returns the result of the matrix combination.
	 */
	LSE_INLINE CMatrix4x4_SSE3 LSE_FCALL CMatrix4x4_SSE3::operator * ( const CMatrix4x4_SSE3 &_mOther ) const {
		const float * pfA = reinterpret_cast<const float *>(&_mOther);
		const float * pfB = reinterpret_cast<const float *>(this);
		
		const __m128 mL0 = _mm_load_ps( &pfA[0] );
		const __m128 mL1 = _mm_load_ps( &pfA[4] );
		const __m128 mL2 = _mm_load_ps( &pfA[8] );
		const __m128 mL3 = _mm_load_ps( &pfA[12] );

		const __m128 mR0 = _mm_load_ps( &pfB[0] );
		const __m128 mR1 = _mm_load_ps( &pfB[4] );
		const __m128 mR2 = _mm_load_ps( &pfB[8] );
		const __m128 mR3 = _mm_load_ps( &pfB[12] );

		// ==
		const __m128 mM0 =	mL0 * _mm_shuffle_ps( mR0, mR0, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mR0, mR0, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mR0, mR0, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mR0, mR0, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		// ==
		const __m128 mM1 =	mL0 * _mm_shuffle_ps( mR1, mR1, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mR1, mR1, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mR1, mR1, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mR1, mR1, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		// ==
		const __m128 mM2 =	mL0 * _mm_shuffle_ps( mR2, mR2, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mR2, mR2, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mR2, mR2, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mR2, mR2, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		// ==
		const __m128 mM3 =	mL0 * _mm_shuffle_ps( mR3, mR3, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mR3, mR3, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mR3, mR3, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mR3, mR3, _MM_SHUFFLE( 3, 3, 3, 3 ) );


		CMatrix4x4_SSE3 mOut;
		float * pfOut = reinterpret_cast<float *>(&mOut);
		_mm_store_ps( &pfOut[0], mM0 );
		_mm_store_ps( &pfOut[4], mM1 );
		_mm_store_ps( &pfOut[8], mM2 );
		_mm_store_ps( &pfOut[12], mM3 );

		return mOut;
	}
	
	/**
	 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
	 *
	 * \param _vOther The vector to transform by this matrix.
	 * \return Returns the vector transformed by this matrix.
	 */
	LSE_INLINE CVector4 LSE_FCALL CMatrix4x4_SSE3::operator * ( const CVector4 &_vOther ) const {
		const float * pfB = reinterpret_cast<const float *>(this);

		const __m128 mV = _mm_load_ps( reinterpret_cast<const float *>(&_vOther) );

		const __m128 mL0 = _mm_load_ps( &pfB[0] );
		const __m128 mL1 = _mm_load_ps( &pfB[4] );
		const __m128 mL2 = _mm_load_ps( &pfB[8] );
		const __m128 mL3 = _mm_load_ps( &pfB[12] );

		// ==
		const __m128 mM0 =	mL0 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		CVector4 vRet;
		_mm_store_ps( reinterpret_cast<float *>(&vRet), mM0 );
		return vRet;
	}

	/**
	 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
	 *
	 * \param _vOther The vector to transform by this matrix.
	 * \return Returns the vector transformed by this matrix.
	 */
	LSE_INLINE CVector3 LSE_FCALL CMatrix4x4_SSE3::operator * ( const CVector3 &_vOther ) const {
		CVector4 vTemp = (*this) * CVector4( _vOther.x, _vOther.y, _vOther.z, 1.0f );
		return vTemp.Vec3();
	}

	// == Functions.
	/**
	 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 *
	 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 */
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4_SSE3::Get3x3Matrix() const {
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
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4_SSE3::Get3x3MatrixTranspose() const {
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
	LSE_INLINE CMatrix4x4_SSE3 & LSE_FCALL CMatrix4x4_SSE3::Transpose() {
		(*this) = GetTranspose();
		return (*this);
	}

	/**
	 * Copies the transpose of a matrix into another matrix.
	 *
	 * \return Returns the copied object.
	 */
	LSE_INLINE CMatrix4x4_SSE3 LSE_FCALL CMatrix4x4_SSE3::GetTranspose() const {
		return CMatrix4x4_SSE3( _11, _21, _31, _41,
			_12, _22, _32, _42,
			_13, _23, _33, _43,
			_14, _24, _34, _44 );
	}

	/**
	 * Transforms vector (x, y, z, w) by a given matrix.
	 *
	 * \param _m44bMat The matrix by which to transform the given vector.
	 * \param _v4bIn The vector to transform.
	 */
	LSE_INLINE CVector4 LSE_FCALL CMatrix4x4_SSE3::MultiplyVec4( const CMatrix4x4_SSE3 &_m44bMat, const CVector4 &_v4bIn ) {
		const float * pfB = reinterpret_cast<const float *>(&_m44bMat);
		const __m128 mV = _mm_load_ps( reinterpret_cast<const float *>(&_v4bIn) );

		const __m128 mL0 = _mm_load_ps( &pfB[0] );
		const __m128 mL1 = _mm_load_ps( &pfB[4] );
		const __m128 mL2 = _mm_load_ps( &pfB[8] );
		const __m128 mL3 = _mm_load_ps( &pfB[12] );

		// ==
		const __m128 mM0 =	mL0 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 0, 0, 0, 0 ) ) +
							mL1 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 1, 1, 1, 1 ) ) +
							mL2 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 2, 2, 2, 2 ) ) +
							mL3 * _mm_shuffle_ps( mV, mV, _MM_SHUFFLE( 3, 3, 3, 3 ) );

		CVector4 vRet;
		_mm_store_ps( reinterpret_cast<float *>(&vRet), mM0 );
		return vRet;
	}

	/**
	 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
	 *
	 * \param _m44bMat The matrix by which to transform the given vector.
	 * \param _pv4bIn The vectors to transform.
	 * \param _ui32Total Number of vectors to transform.
	 * \param _pv4bOut Holds the transformed vectors, which must not be the same as the input vector list.
	 */
	LSE_INLINE void LSE_FCALL CMatrix4x4_SSE3::MultiplyVec4Batch( const CMatrix4x4_SSE3 &_m44bMat,
		const CVector4 * _pv4bIn, uint32_t _ui32Total,
		CVector4 * _pv4bOut ) {
		for ( uint32_t I = 0; I < _ui32Total; ++I ) {
			LSE_PREFETCH_LINE_WRITE( _pv4bOut );
			(*_pv4bOut++) = MultiplyVec4( _m44bMat, _pv4bIn[I] );
		}
	}

}	// namespace lsm

#endif	// #ifdef LSE_X64

#endif	// __LSM_MATRIX4X4_SSE3_H__
