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
 * Description: The base matrix template allowing any type of number to be used.  Vectors are stored row-major.
 */


#ifndef __LSM_MATRIX3X3BASE_H__
#define __LSM_MATRIX3X3BASE_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3Base.h"

namespace lsm {

	/**
	 * Class CMatrix3x3Base
	 * \brief The base matrix template allowing any type of number to be used.
	 *
	 * Description: The base matrix template allowing any type of number to be used.  Vectors are stored row-major.
	 */
	template <typename _tType, typename _tVector3Type>
	class CMatrix3x3Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR									CMatrix3x3Base() {
		}
		LSE_CALLCTOR									CMatrix3x3Base( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bOther ) :
			_11( _m33bOther._11 ), _12( _m33bOther._12 ), _13( _m33bOther._13 ),
			_21( _m33bOther._21 ), _22( _m33bOther._22 ), _23( _m33bOther._23 ),
			_31( _m33bOther._31 ), _32( _m33bOther._32 ), _33( _m33bOther._33 ) {
		}
		LSE_CALLCTOR									CMatrix3x3Base( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bOther, LSBOOL _bTranspose ) {
			if ( !_bTranspose ) {
				_11 =_m33bOther._11; _12 =_m33bOther._12; _13 =_m33bOther._13;
				_21 =_m33bOther._21; _22 =_m33bOther._22; _23 =_m33bOther._23;
				_31 =_m33bOther._31; _32 =_m33bOther._32; _33 =_m33bOther._33;
			}
			else {
				_11 =_m33bOther._11; _12 =_m33bOther._21; _13 =_m33bOther._31;
				_21 =_m33bOther._12; _22 =_m33bOther._22; _23 =_m33bOther._32;
				_31 =_m33bOther._31; _32 =_m33bOther._32; _33 =_m33bOther._33;
			}
		}
		LSE_CALLCTOR									CMatrix3x3Base( const _tType * _ptArray ) {
			_11 = _ptArray[0];
			_12 = _ptArray[1];
			_13 = _ptArray[2];

			_21 = _ptArray[3];
			_22 = _ptArray[4];
			_23 = _ptArray[5];

			_31 = _ptArray[6];
			_32 = _ptArray[7];
			_33 = _ptArray[8];
		}


		// == Operators.
		/**
		 * Provides row/column access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \param _ui32Col The column of the value to obtain.
		 * \return Returns a reference to the value at the given row and column.  Can be used for reading
		 *	or writing.
		 */
		_tType & LSE_FCALL								operator () ( uint32_t _ui32Row, uint32_t _ui32Col ) {
			return reinterpret_cast<_tType *>(this)[(_ui32Row*3)+_ui32Col];
		}

		/**
		 * Provides row/column access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \param _ui32Col The column of the value to obtain.
		 * \return Returns the value at the given row and column.
		 */
		_tType LSE_FCALL								operator () ( uint32_t _ui32Row, uint32_t _ui32Col ) const {
			return reinterpret_cast<const _tType *>(this)[(_ui32Row*3)+_ui32Col];
		}

		/**
		 * Multiply this matrix with another, returning the result as M = this * _m33bOther.
		 *
		 * \param _m33bOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> LSE_FCALL	operator * ( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bOther ) const {
			CMatrix3x3Base<_tType, _tVector3Type> m33bOut;
			for ( uint32_t I = 3; I--; ) {
				const _tType * pfBufferIn = &reinterpret_cast<const _tType *>(this)[I*3];
				_tType * pfBufferOut = &reinterpret_cast<_tType *>(&m33bOut)[I*3];
				for ( uint32_t J = 3; J--; ) {
					_tType fSum = LSM_ZERO;
					for ( uint32_t K = 3; K--; ) {
						fSum += pfBufferIn[K] * _m33bOther( K, J );
					}
					pfBufferOut[J] = fSum;
				}
			}

			return m33bOut;
		}

		/**
		 * Multiply this matrix with a vector, returning the result as V = this * _v3bOther.
		 *
		 * \param _v3bOther The vector to transform by this matrix.
		 * \return Returns the vector transformed by this matrix.
		 */
		_tVector3Type LSE_FCALL							operator * ( const _tVector3Type &_v3bOther ) const {
			_tVector3Type v3bRet;
			v3bRet.x = (_11 * _v3bOther.x + _21 * _v3bOther.y + 
				_31 * _v3bOther.z);
			v3bRet.y = (_12 * _v3bOther.x + _22 * _v3bOther.y +
				_32 * _v3bOther.z);
			v3bRet.z = (_13 * _v3bOther.x + _23 * _v3bOther.y +
				_33 * _v3bOther.z);
			return v3bRet;
		}

		/**
		 * Multiply each component of the matrix by the given real.
		 *
		 * \param _tValue The real by which to multiply each component of this matrix.
		 * \return The result of the multiplication is returned.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> LSE_FCALL	operator * ( _tType _tValue ) const {
			CMatrix3x3Base<_tType, _tVector3Type> m33bRet;
			m33bRet._11 = _11 * _tValue;
			m33bRet._12 = _12 * _tValue;
			m33bRet._13 = _13 * _tValue;
			m33bRet._21 = _21 * _tValue;
			m33bRet._22 = _22 * _tValue;
			m33bRet._23 = _23 * _tValue;
			m33bRet._31 = _31 * _tValue;
			m33bRet._32 = _32 * _tValue;
			m33bRet._33 = _33 * _tValue;
			return m33bRet;
		}

		/**
		 * Multiply each component of the matrix by the given real in-place.
		 *
		 * \param _tValue The real by which to multiply each component of this matrix.
		 * \return The result of the multiplication is returned.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														operator *= ( _tType _tValue ) {
			_11 = _11 * _tValue;
			_12 = _12 * _tValue;
			_13 = _13 * _tValue;
			_21 = _21 * _tValue;
			_22 = _22 * _tValue;
			_23 = _23 * _tValue;
			_31 = _31 * _tValue;
			_32 = _32 * _tValue;
			_33 = _33 * _tValue;
			return (*this);
		}

		/**
		 * Performs a componentwise addition between this matrix and the given matrix.
		 *
		 * \param _m33bOther The matrix by which each component is added.
		 * \return Returns this matrix.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														operator += ( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bOther ) {
			_11 += _m33bOther._11;
			_12 += _m33bOther._12;
			_13 += _m33bOther._13;
			_21 += _m33bOther._21;
			_22 += _m33bOther._22;
			_23 += _m33bOther._23;
			_31 += _m33bOther._31;
			_32 += _m33bOther._32;
			_33 += _m33bOther._33;
			return (*this);
		}


		// == Functions.
		/**
		 * Sets the identity matrix in place.
		 *
		 * \return Returns this matrix.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														Identity() {
			_11 = _22 = _33 = _tType( 1.0 );
			_12 = _13 = _21 = _23 = _31 = _32 = _tType( 0.0 );
			return (*this);
		}

		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this matrix.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														Transpose() {
			CMathLib::Swap( _12, _21 );
			CMathLib::Swap( _13, _31 );
			CMathLib::Swap( _23, _32 );
			return (*this);
		}

		/**
		 * Gets the inverse of this matrix.  Upon failure, the identity matrix is returned.
		 *
		 * \return Returns the inverse of this matrix, or identity on failure.
		 */
		CMatrix3x3Base<_tType, _tVector3Type>			Inverse() const {
			// Invert a 3-by-3 matrix using cofactors.
			CMatrix3x3Base<_tType, _tVector3Type> m33bInv;

			m33bInv._11 =
				_22 * _33 - _23 * _32;
			m33bInv._12 =
				_13 * _32 - _12 * _33;
			m33bInv._13 =
				_12 * _23 - _13 * _22;
			m33bInv._21 =
				_23 * _31 - _21 * _33;
			m33bInv._22 =
				_11 * _33 - _13 * _31;
			m33bInv._23 =
				_13 * _21 - _11 * _23;
			m33bInv._31 =
				_21 * _32 - _22 * _31;
			m33bInv._32 =
				_12 * _31 - _11 * _32;
			m33bInv._33 =
				_11 * _22 - _12 * _21;

			_tType fDet =
				_11 * m33bInv._11 +
				_12 * m33bInv._21 +
				_13 * m33bInv._31;

			if ( CMathLib::Abs( static_cast<LSREAL>(fDet) ) > LSM_MAT_EPSILON ) {
				_tType fInvDet = _tType( 1.0 ) / fDet;
				m33bInv._11 *= fInvDet;
				m33bInv._12 *= fInvDet;
				m33bInv._13 *= fInvDet;
				m33bInv._21 *= fInvDet;
				m33bInv._22 *= fInvDet;
				m33bInv._23 *= fInvDet;
				m33bInv._31 *= fInvDet;
				m33bInv._32 *= fInvDet;
				m33bInv._33 *= fInvDet;
			}
			else {
				m33bInv.Identity();
			}

			return m33bInv;
		}

		/**
		 * Gets the determinant.
		 *
		 * \return Returns the determinant of this matrix.
		 */
		_tType LSE_FCALL								Determ() const {
			CMatrix3x3Base<_tType, _tVector3Type> m33bInv;

			m33bInv._11 =
				_22 * _33 - _23 * _32;
			m33bInv._12 =
				_13 * _32 - _12 * _33;
			m33bInv._13 =
				_12 * _23 - _13 * _22;
			m33bInv._21 =
				_23 * _31 - _21 * _33;
			m33bInv._22 =
				_11 * _33 - _13 * _31;
			m33bInv._23 =
				_13 * _21 - _11 * _23;
			m33bInv._31 =
				_21 * _32 - _22 * _31;
			m33bInv._32 =
				_12 * _31 - _11 * _32;
			m33bInv._33 =
				_11 * _22 - _12 * _21;

			return _11 * m33bInv._11 +
				_12 * m33bInv._21 +
				_13 * m33bInv._31;
		}

		/**
		 * Gets a reference to a row.
		 *
		 * \param _ui32Row The row to get.
		 * \return Returns a reference to the requested row.
		 */
		_tVector3Type & LSE_FCALL						GetRow( uint32_t _ui32Row ) {
			LSE_UNALIGNED _tType * pfData = &_11;
			pfData += (_ui32Row * 3);
			return (*reinterpret_cast<_tVector3Type *>(pfData));
		}

		/**
		 * Sets a row.
		 *
		 * \param _ui32Row The row to set.
		 * \param _vRet The value to apply to the row.
		 */
		void LSE_FCALL								SetRow( uint32_t _ui32Row, const _tVector3Type &_v3bVec ) {
			LSE_UNALIGNED _tType * pfData = &_11;
			pfData += (_ui32Row * 3);
			
			pfData[0] = _v3bVec.x;
			pfData[1] = _v3bVec.y;
			pfData[2] = _v3bVec.z;
		}

		/**
		 * Apply a skew symmetric matrix to this object.
		 *
		 * \param _v3bVec The vector from which the skew symmetric matrix is to be obtained.
		 * \return Returns this matrix.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														SetSkewSymmetric( const _tVector3Type &_v3bVec ) {
			_11 = _22 = _33 = 0;
			_21 = -_v3bVec.z;
			_31 = _v3bVec.y;
			_12 = _v3bVec.z;
			_32 = -_v3bVec.x;
			_13 = -_v3bVec.y;
			_23 = _v3bVec.x;
			return (*this);
		}

		/**
		 * Gets the yaw, pitch, and roll from this matrix.
		 *
		 * \param _tYaw Holds the returned yaw.
		 * \param _tPitch Holds the returned pitch.
		 * \param _tRoll Holds the returned roll.
		 */
		void LSE_FCALL								GetYawPitchRoll( _tType &_tYaw, _tType &_tPitch, _tType &_tRoll ) const {
			_tYaw = CMathLib::ATan( static_cast<LSREAL>(_21), static_cast<LSREAL>(_11) );
			_tPitch = CMathLib::ATan( static_cast<LSREAL>(-_31), CMathLib::Sqrt( static_cast<LSREAL>(_32 * _32 + _33 * _33) ) );
			_tRoll = CMathLib::ATan( static_cast<LSREAL>(_32), static_cast<LSREAL>(_33) );
		}

		/**
		 * Multiply two matrices together.  The result represents the transformation M1
		 *	followed by the transformation M2.
		 *
		 * \param _m33bOut The output matrix holding the result of the operation.
		 * \param _m33bM1 The left operand.
		 * \param _m33bM2 The right operand.
		 * \return Returns a pointer to _m33bOut.
		 */
		static CMatrix3x3Base<_tType, _tVector3Type> * LSE_FCALL
														MatrixMultiply( CMatrix3x3Base<_tType, _tVector3Type> &_m33bOut, const CMatrix3x3Base<_tType, _tVector3Type> &_m33bM1, const CMatrix3x3Base<_tType, _tVector3Type> &_m33bM2 ) {
			const CMatrix3x3Base<_tType, _tVector3Type> * LSE_RESTRICT pmSrc1 = &_m33bM1;
			const CMatrix3x3Base<_tType, _tVector3Type> * LSE_RESTRICT pmSrc2 = &_m33bM2;

			for ( int32_t I = 3; --I >= 0; ) {
				const _tType * pfBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[I*3];
				_tType * pfBufferOut = &reinterpret_cast<_tType *>(&_m33bOut)[I*3];
				for ( int32_t J = 3; --J >= 0; ) {
					_tType tSum = LSM_ZERO;
					for ( int32_t K = 3; --K >= 0; ) {
						tSum += pfBufferIn[K] * (*pmSrc2)( K, J );
					}
					pfBufferOut[J] = tSum;
				}
			}

			return &_m33bOut;
		}

		/**
		 * Transforms vector (x, y, z) by a given matrix.  _v3bIn and _v3bOut must not be the same vectors.
		 *
		 * \param _m33bMat The matrix by which to transform the given vector.
		 * \param _v3bIn The vector to transform.
		 * \param _v3bOut The resulting vector after the transformation has been applied.
		 */
		static void LSE_FCALL							MultiplyVec3ByMat3x3( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bMat, const _tVector3Type &_v3bIn, _tVector3Type &_v3bOut ) {
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v3bIn);
			_v3bOut[0] = _m33bMat._11 * pfIn[0] + _m33bMat._21 * pfIn[1] + 
				_m33bMat._31 * pfIn[2];
			_v3bOut[1] = _m33bMat._12 * pfIn[0] + _m33bMat._22 * pfIn[1] +
				_m33bMat._32 * pfIn[2];
			_v3bOut[2] = _m33bMat._13 * pfIn[0] + _m33bMat._23 * pfIn[1] +
				_m33bMat._33 * pfIn[2];
		}

		/**
		 * Transforms vector (x, y, z) by the transpose of a given matrix.  _v3bIn and _v3bOut must not be the same vectors.
		 *
		 * \param _m33bMat The matrix by which to transform the given vector.
		 * \param _v3bIn The vector to transform.
		 * \param _v3bOut The resulting vector after the transformation has been applied.
		 */
		static void LSE_FCALL							MultiplyVec3ByMat3x3Transpose( const CMatrix3x3Base<_tType, _tVector3Type> &_m33bMat, const _tVector3Type &_v3bIn, _tVector3Type &_v3bOut ) {
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v3bIn);
			_v3bOut[0] = _m33bMat._11 * pfIn[0] + _m33bMat._12 * pfIn[1] + 
				_m33bMat._13 * pfIn[2];
			_v3bOut[1] = _m33bMat._21 * pfIn[0] + _m33bMat._22 * pfIn[1] +
				_m33bMat._23 * pfIn[2];
			_v3bOut[2] = _m33bMat._31 * pfIn[0] + _m33bMat._32 * pfIn[1] +
				_m33bMat._33 * pfIn[2];
		}

		/**
		 * Builds a matrix that represents a Euler XYZ rotation around a left-handed coordinate system.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix3x3Base<_tType, _tVector3Type> & LSE_FCALL
														MatrixRotationXYZLH( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );

			_11 = fC2 * fC3;
			_12 = fC2 * fS3;
			_13 = -fS2;

			_21 = fC3 * fS1 * fS2 - fC1 * fS3;
			_22 = fC1 * fC3 + fS1 * fS2 * fS3;
			_23 = fC2 * fS1;

			_31 = fC1 * fC3 * fS2 + fS1 * fS3;
			_32 = fC1 * fS2 * fS3 - fC3 * fS1;
			_33 = fC1 * fC2;

			return (*this);
		}

		// == Members.
#pragma pack( push, 1 )
		_tType											_11, _12, _13;		// Vector 0.
		_tType											_21, _22, _23;		// Vector 1.
		_tType											_31, _32, _33;		// Vector 2.
#pragma pack( pop )
	};

}	// namespace lsm

#endif	// __LSM_MATRIX3X3BASE_H__

