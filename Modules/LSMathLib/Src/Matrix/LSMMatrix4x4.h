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
 * Description: Specialization of the template base class for use with standard LSREAL matrices.
 */


#ifndef __LSM_MATRIX4X4_H__
#define __LSM_MATRIX4X4_H__

#include "../LSMMathLib.h"

#if defined( __ARM_NEON__ ) && LSM_BASETYPE == LSM_FLOAT
#include "LSMMatrix4x4_ARM.h"

namespace lsm {

	typedef CMatrix4x4_ARM									CMatrix4x4;
	
}	// namespace lsm
#elif LSE_X64 && LSM_BASETYPE == LSM_FLOAT
#include "LSMMatrix4x4_SSE3.h"

namespace lsm {

	typedef CMatrix4x4_SSE3									CMatrix4x4;
	
}	// namespace lsm
#else
#include "../Vector/LSMVector3.h"
#include "../Vector/LSMVector4.h"
#include "LSMMatrix3x3.h"
#include "LSMMatrix4x4Base.h"

namespace lsm {

	/**
	 * Class CMatrix4x4
	 * \brief Specialization of the template base class for use with standard LSREAL matrices.
	 *
	 * Description: Specialization of the template base class for use with standard LSREAL matrices.
	 */
	LSE_ALIGN( 16 )
	class CMatrix4x4 : public CMatrix4x4Base<LSREAL, CVector3, CVector4> {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR										CMatrix4x4();
		LSE_CALLCTOR										CMatrix4x4( const CMatrix4x4 &_mOther );
		LSE_CALLCTOR										CMatrix4x4( LSREAL _t11, LSREAL _t12, LSREAL _13, LSREAL _14,
			LSREAL _t21, LSREAL _t22, LSREAL _23, LSREAL _24,
			LSREAL _t31, LSREAL _t32, LSREAL _33, LSREAL _34,
			LSREAL _t41, LSREAL _t42, LSREAL _43, LSREAL _44 );
		LSE_CALLCTOR										CMatrix4x4( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_mOther );
		LSE_CALLCTOR										CMatrix4x4( const CMatrix4x4 &_mOther, LSBOOL _bTranspose );
		explicit LSE_CALLCTOR								CMatrix4x4( const LSREAL * _pfArray );


		// == Operators.
#if (LSM_BASETYPE == LSM_FLOAT)
		/**
		 * Copy/assignment operator.
		 *
		 * \param _mOther The matrix to copy into this one.
		 * \return Returns this matrix after the copy operation.
		 */
		LSE_INLINE CMatrix4x4 & LSE_FCALL					operator = ( const CMatrix4x4 &_mOther );

#if defined( LSE_X86 ) || defined( LSE_X64 ) || defined( LSE_IPHONE )
		/**
		 * Multiply this matrix with another, returning the result as M = this * _mOther.
		 *
		 * \param _mOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		LSE_INLINE CMatrix4x4 LSE_FCALL						operator * ( const CMatrix4x4 &_mOther ) const;
#endif	// #if defined( LSE_X86 ) || defined( LSE_X64 ) || defined( LSE_IPHONE )
#else
		/**
		 * Multiply this matrix with another, returning the result as M = this * _mOther.
		 *
		 * \param _mOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		LSE_INLINE CMatrix4x4 LSE_FCALL						operator * ( const CMatrix4x4 &_mOther ) const;
#endif	// #if (LSM_BASETYPE == LSM_FLOAT)

		/**
		 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
		 *
		 * \param _vOther The vector to transform by this matrix.
		 * \return Returns the vector transformed by this matrix.
		 */
		LSE_INLINE CVector3 LSE_FCALL						operator * ( const CVector3 &_vOther ) const;


		// == Functions.
		/**
		 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
		 *
		 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
		 */
		LSE_INLINE CMatrix3x3 LSE_CALL						Get3x3Matrix() const;

		/**
		 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
		 *
		 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix transposed.
		 */
		LSE_INLINE CMatrix3x3 LSE_CALL						Get3x3MatrixTranspose() const;

#if (LSM_BASETYPE == LSM_FLOAT)
#if defined( LSE_X86 )
#if LSM_PERFORMANCE <= 0
		/**
		 * Transforms a batch of vectors (x, y, z, 1) by a given matrix.
		 *
		 * \param _mMat The matrix by which to transform the given vector.
		 * \param _pvIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pvOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL								MultiplyVec3Batch( const CMatrix4x4 &_mMat,
			const CVector3 * _pvIn, uint32_t _ui32Total,
			CVector3 * _pvOut );
#endif	// #if LSM_PERFORMANCE <= 0
#if 0
		/**
		 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
		 *
		 * \param _mMat The matrix by which to transform the given vector.
		 * \param _pvIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _v4bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL								MultiplyVec4Batch( const CMatrix4x4 &_mMat,
			const CVector4 * _pvIn, uint32_t _ui32Total,
			CVector4 * _pvOut );
#endif	// #if 0
#endif	// #if defined( LSE_X86 )

#if defined( LSE_X64 )

		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this object.
		 */
		LSE_INLINE CMatrix4x4 & LSE_FCALL					Transpose();

		/**
		 * Copies the transpose of a matrix into another matrix.
		 *
		 * \return Returns the copied object.
		 */
		LSE_INLINE CMatrix4x4 LSE_FCALL						GetTranspose() const;

		/**
		 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
		 *
		 * \param _mMat The matrix by which to transform the given vector.
		 * \param _pvIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _v4bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static LSE_INLINE void LSE_FCALL					MultiplyVec4Batch( const CMatrix4x4 &_mMat,
			const CVector4 * _pvIn, uint32_t _ui32Total,
			CVector4 * _pvOut );
#endif	// #if defined( LSE_X64 )
#endif	// #if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT)


		// == Members.
		/** An identity matrix. */
		static const CMatrix4x4								m_mIdentity;

	private :
		typedef CMatrix4x4Base<LSREAL, CVector3, CVector4>	Parent;
	} LSE_POSTALIGN( 16 );


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Various constructors.
	LSE_INLINE LSE_CALLCTOR CMatrix4x4::CMatrix4x4( const CMatrix4x4Base<LSREAL, CVector3, CVector4> &_mOther ) :
		Parent( _mOther ) {
	}
	LSE_INLINE LSE_CALLCTOR CMatrix4x4::CMatrix4x4( LSREAL _t11, LSREAL _t12, LSREAL _t13, LSREAL _t14,
		LSREAL _t21, LSREAL _t22, LSREAL _t23, LSREAL _t24,
		LSREAL _t31, LSREAL _t32, LSREAL _t33, LSREAL _t34,
		LSREAL _t41, LSREAL _t42, LSREAL _t43, LSREAL _t44 ) :
		Parent( _t11, _t12, _t13, _t14,
			_t21, _t22, _t23, _t24,
			_t31, _t32, _t33, _t34,
			_t41, _t42, _t43, _t44 ) {
	}

	// == Operators.
#if (LSM_BASETYPE == LSM_FLOAT)
	/**
	 * Copy/assignment operator.
	 *
	 * \param _mOther The matrix to copy into this one.
	 * \return Returns this matrix after the copy operation.
	 */
	LSE_INLINE CMatrix4x4 & LSE_FCALL CMatrix4x4::operator = ( const CMatrix4x4 &_mOther ) {
#if defined( LSE_X86 ) || defined( LSE_X64 )
		const float * pfThis = reinterpret_cast<const float *>(&_mOther);
		float * pfOut = reinterpret_cast<float *>(this);

		__m128 m128Row0 = _mm_load_ps( &pfThis[0] );
		__m128 m128Row1 = _mm_load_ps( &pfThis[4] );
		__m128 m128Row2 = _mm_load_ps( &pfThis[8] );
		__m128 m128Row3 = _mm_load_ps( &pfThis[12] );

		_mm_store_ps( &pfOut[0], m128Row0 );
		_mm_store_ps( &pfOut[4], m128Row1 );
		_mm_store_ps( &pfOut[8], m128Row2 );
		_mm_store_ps( &pfOut[12], m128Row3 );
#else
		// Copy in pairs.
		uint64_t * pui64This = reinterpret_cast<uint64_t *>(this);
		const uint64_t * pui64That = reinterpret_cast<const uint64_t *>(&_mOther);
		(*pui64This++) = (*pui64That++);
		(*pui64This++) = (*pui64That++);

		(*pui64This++) = (*pui64That++);
		(*pui64This++) = (*pui64That++);

		(*pui64This++) = (*pui64That++);
		(*pui64This++) = (*pui64That++);

		(*pui64This++) = (*pui64That++);
		(*pui64This) = (*pui64That);
#endif	// #if defined( LSE_X86 ) || defined( LSE_X64 )
		return (*this);
	}

#if defined( LSE_X86 ) || defined( LSE_X64 )

	/**
	 * Multiply this matrix with another, returning the result as M = this * _mOther.
	 *
	 * \param _mOther The matrix by which to multiply this matrix.
	 * \return Returns the result of the matrix combination.
	 */
	LSE_INLINE CMatrix4x4 LSE_FCALL CMatrix4x4::operator * ( const CMatrix4x4 &_mOther ) const {
		CMatrix4x4 mTemp;
		const LSREAL * pfA = reinterpret_cast<const LSREAL *>(this);
		const LSREAL * pfB = reinterpret_cast<const LSREAL *>(&_mOther);
		LSREAL * pfC = reinterpret_cast<LSREAL *>(&mTemp);
		__m128 m128Row0 = _mm_load_ps( &pfB[0] );
		__m128 m128Row1 = _mm_load_ps( &pfB[4] );
		__m128 m128Row2 = _mm_load_ps( &pfB[8] );
		__m128 m128Row3 = _mm_load_ps( &pfB[12] );
		for( uint32_t I = 4; I--; ) {
			uint32_t ui32Index = I << 2;
			__m128 m128Broad0 = _mm_set_ps1( pfA[ui32Index] );
			__m128 m128Broad1 = _mm_set_ps1( pfA[ui32Index+1] );
			__m128 m128Broad2 = _mm_set_ps1( pfA[ui32Index+2] );
			__m128 m128Broad3 = _mm_set_ps1( pfA[ui32Index+3] );
			__m128 m128Row = _mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps( m128Broad0, m128Row0 ),
					_mm_mul_ps( m128Broad1, m128Row1 ) ),
				_mm_add_ps(
					_mm_mul_ps( m128Broad2, m128Row2 ),
					_mm_mul_ps( m128Broad3, m128Row3 ) ) );
			_mm_store_ps( &pfC[ui32Index], m128Row );
		}
		return mTemp;
	}
#elif defined( LSE_IPHONE )
	/**
	 * Multiply this matrix with another, returning the result as M = this * _mOther.
	 *
	 * \param _mOther The matrix by which to multiply this matrix.
	 * \return Returns the result of the matrix combination.
	 */
	LSE_INLINE CMatrix4x4 LSE_FCALL CMatrix4x4::operator * ( const CMatrix4x4 &_mOther ) const {
		return (*static_cast<const Parent *>(this)) * static_cast<const Parent &>(_mOther);
	}
#endif	// #if defined( LSE_X86 ) || defined( LSE_X64 )
#else
	/**
	 * Multiply this matrix with another, returning the result as M = this * _mOther.
	 *
	 * \param _mOther The matrix by which to multiply this matrix.
	 * \return Returns the result of the matrix combination.
	 */
	LSE_INLINE CMatrix4x4 LSE_FCALL CMatrix4x4::operator * ( const CMatrix4x4 &_mOther ) const {
		return (*static_cast<const Parent *>(this)) * static_cast<const Parent &>(_mOther);
	}
#endif	// #if (LSM_BASETYPE == LSM_FLOAT)

	/**
	 * Multiply this matrix with a vector, returning the result as V = this * _vOther.
	 *
	 * \param _vOther The vector to transform by this matrix.
	 * \return Returns the vector transformed by this matrix.
	 */
	LSE_INLINE CVector3 LSE_FCALL CMatrix4x4::operator * ( const CVector3 &_vOther ) const {
		return (*static_cast<const Parent *>(this)) * _vOther;
	}

	// == Functions.
	/**
	 * Creates a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 *
	 * \return Returns a 3-by-3 matrix representing the first 3 rows and columns of this matrix.
	 */
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4::Get3x3Matrix() const {
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
	LSE_INLINE CMatrix3x3 LSE_CALL CMatrix4x4::Get3x3MatrixTranspose() const {
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

#if (LSM_BASETYPE == LSM_FLOAT)
#if defined( LSE_X86 )
#if LSM_PERFORMANCE <= 0
	/**
	 * Transforms a batch of vectors (x, y, z, 1) by a given matrix.
	 *
	 * \param _mMat The matrix by which to transform the given vector.
	 * \param _pvIn The vectors to transform.
	 * \param _ui32Total Number of vectors to transform.
	 * \param _pvOut Holds the transformed vectors, which must not be the same as the input vector list.
	 */
	LSE_INLINE void LSE_FCALL CMatrix4x4::MultiplyVec3Batch( const CMatrix4x4 &_mMat,
		const CVector3 * _pvIn, uint32_t _ui32Total,
		CVector3 * _pvOut ) {
		// When using LSM_PERFORMANCE at 0, the CVector3 class actually has 4 members, making it a CVector4 for alignment
		//	purposes.
		MultiplyVec4Batch( _mMat, reinterpret_cast<const CVector4 *>(_pvIn), _ui32Total, reinterpret_cast<CVector4 *>(_pvOut) );
	}
#endif	// #if defined( LSE_X86 )
#endif	// #if LSM_PERFORMANCE <= 0

#if defined( LSE_X64 )
	
	/**
	 * Transpose a matrix in place.
	 *
	 * \return Returns this object.
	 */
	LSE_INLINE CMatrix4x4 & LSE_FCALL CMatrix4x4::Transpose() {
		float * pfThis = reinterpret_cast<float *>(this);
		float * pfOut = reinterpret_cast<float *>(this);

		__m128 m128Row0 = _mm_load_ps( &pfThis[0] );
		__m128 m128Row1 = _mm_load_ps( &pfThis[4] );
		__m128 m128Row2 = _mm_load_ps( &pfThis[8] );
		__m128 m128Row3 = _mm_load_ps( &pfThis[12] );

		__m128 m128Temp0, m128Temp1, m128Temp2, m128Temp3;
		m128Temp0 = _mm_unpacklo_ps( m128Row0, m128Row1 );
		m128Temp2 = _mm_unpacklo_ps( m128Row2, m128Row3 );
		m128Temp1 = _mm_unpackhi_ps( m128Row0, m128Row1 );
		m128Temp3 = _mm_unpackhi_ps( m128Row2, m128Row3 );

		_mm_store_ps( &pfOut[0], _mm_movelh_ps( m128Temp0, m128Temp2 ) );
		_mm_store_ps( &pfOut[4], _mm_movehl_ps( m128Temp2, m128Temp0 ) );
		_mm_store_ps( &pfOut[8], _mm_movelh_ps( m128Temp1, m128Temp3 ) );
		_mm_store_ps( &pfOut[12], _mm_movehl_ps( m128Temp3, m128Temp1 ) );
		return (*this);
	}

	/**
	 * Copies the transpose of a matrix into another matrix.
	 *
	 * \return Returns the copied object.
	 */
	LSE_INLINE CMatrix4x4 LSE_FCALL CMatrix4x4::GetTranspose() const {
		CMatrix4x4 m44bOut;
		const float * pfThis = reinterpret_cast<const float *>(this);
		float * pfOut = reinterpret_cast<float *>(&m44bOut);

		__m128 m128Row0 = _mm_load_ps( &pfThis[0] );
		__m128 m128Row1 = _mm_load_ps( &pfThis[4] );
		__m128 m128Row2 = _mm_load_ps( &pfThis[8] );
		__m128 m128Row3 = _mm_load_ps( &pfThis[12] );

		__m128 m128Temp0, m128Temp1, m128Temp2, m128Temp3;
		m128Temp0 = _mm_unpacklo_ps( m128Row0, m128Row1 );
		m128Temp2 = _mm_unpacklo_ps( m128Row2, m128Row3 );
		m128Temp1 = _mm_unpackhi_ps( m128Row0, m128Row1 );
		m128Temp3 = _mm_unpackhi_ps( m128Row2, m128Row3 );

		_mm_store_ps( &pfOut[0], _mm_movelh_ps( m128Temp0, m128Temp2 ) );
		_mm_store_ps( &pfOut[4], _mm_movehl_ps( m128Temp2, m128Temp0 ) );
		_mm_store_ps( &pfOut[8], _mm_movelh_ps( m128Temp1, m128Temp3 ) );
		_mm_store_ps( &pfOut[12], _mm_movehl_ps( m128Temp3, m128Temp1 ) );

		return m44bOut;
	}

	/**
	 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
	 *
	 * \param _mMat The matrix by which to transform the given vector.
	 * \param _pvIn The vectors to transform.
	 * \param _ui32Total Number of vectors to transform.
	 * \param _v4bOut Holds the transformed vectors, which must not be the same as the input vector list.
	 */
	LSE_INLINE void LSE_FCALL CMatrix4x4::MultiplyVec4Batch( const CMatrix4x4 &_mMat,
		const CVector4 * _pvIn, uint32_t _ui32Total,
		CVector4 * _pvOut ) {
		const CMatrix4x4 mTrans = _mMat.GetTranspose();
		const __m128 * pm128 = reinterpret_cast<const __m128 *>(&mTrans);
		for ( uint32_t M = _ui32Total; M--; ++_pvIn, ++_pvOut ) {
			const __m128 * pm128B = reinterpret_cast<const __m128 *>(_pvIn);
			__m128 m128X = _mm_dp_ps( pm128[0], (*pm128B), 0xF0 | 0x0F );
			__m128 m128Y = _mm_dp_ps( pm128[1], (*pm128B), 0xF0 | 0x0F );
			__m128 m128Z = _mm_dp_ps( pm128[2], (*pm128B), 0xF0 | 0x0F );
			__m128 m128W = _mm_dp_ps( pm128[3], (*pm128B), 0xF0 | 0x0F );

			__m128 m128Xy = _mm_movelh_ps( m128X, m128Y );
			__m128 m128Zw = _mm_movelh_ps( m128Z, m128W );
			_mm_store_ps( reinterpret_cast<float *>(_pvOut), _mm_shuffle_ps( m128Xy, m128Zw, _MM_SHUFFLE( 2, 0, 2, 0 ) ) );
		}
	}
#endif	// #if defined( LSE_X64 )
#endif	// #if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT)

}	// namespace lsm

#endif	// #if defined( __ARM_NEON__ ) && LSM_BASETYPE == LSM_FLOAT

#endif	// __LSM_MATRIX4X4_H__

