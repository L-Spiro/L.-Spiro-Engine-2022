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


#ifndef __LSM_MATRIX2X2BASE_H__
#define __LSM_MATRIX2X2BASE_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector2Base.h"

namespace lsm {

	/**
	 * Class CMatrix2x2Base
	 * \brief The base matrix template allowing any type of number to be used.
	 *
	 * Description: The base matrix template allowing any type of number to be used.  Vectors are stored row-major.
	 */
	template <typename _tType, typename _tVector2Type>
	class CMatrix2x2Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR									CMatrix2x2Base() {
		}
		LSE_CALLCTOR									CMatrix2x2Base( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bOther ) :
			_11( _m22bOther._11 ), _12( _m22bOther._12 ),
			_21( _m22bOther._21 ), _22( _m22bOther._22 ) {
		}
		LSE_CALLCTOR									CMatrix2x2Base( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bOther, LSBOOL _bTranspose ) {
			if ( !_bTranspose ) {
				_11 =_m22bOther._11; _12 =_m22bOther._12;
				_21 =_m22bOther._21; _22 =_m22bOther._22;
			}
			else {
				_11 =_m22bOther._11; _12 =_m22bOther._21;
				_21 =_m22bOther._12; _22 =_m22bOther._22;
			}
		}
		LSE_CALLCTOR									CMatrix2x2Base( const _tType * _ptArray ) {
			_11 = _ptArray[0];
			_12 = _ptArray[1];

			_21 = _ptArray[2];
			_22 = _ptArray[3];
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
			return reinterpret_cast<_tType *>(this)[(_ui32Row<<1)+_ui32Col];
		}

		/**
		 * Provides row/column access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \param _ui32Col The column of the value to obtain.
		 * \return Returns the value at the given row and column.
		 */
		_tType LSE_FCALL								operator () ( uint32_t _ui32Row, uint32_t _ui32Col ) const {
			return reinterpret_cast<const _tType *>(this)[(_ui32Row<<1)+_ui32Col];
		}

		/**
		 * Multiply this matrix with another, returning the result as M = this * _m22bOther.
		 *
		 * \param _m22bOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> LSE_FCALL	operator * ( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bOther ) const {
			CMatrix2x2Base<_tType, _tVector2Type> m22bOut;
			for ( int32_t I = 2; --I >= 0; ) {
				const _tType * pfBufferIn = &reinterpret_cast<const _tType *>(this)[I<<1];
				_tType * pfBufferOut = &reinterpret_cast<_tType *>(&m22bOut)[I<<1];
				for ( int32_t J = 2; --J >= 0; ) {
					_tType fSum = LSM_ZERO;
					for ( int32_t K = 2; --K >= 0; ) {
						fSum += pfBufferIn[K] * _m22bOther( K, J );
					}
					pfBufferOut[J] = fSum;
				}
			}

			return m22bOut;
		}

		/**
		 * Multiply this matrix with a vector, returning the result as V = this * _v2bOther.
		 *
		 * \param _v2bOther The vector to transform by this matrix.
		 * \return Returns the vector transformed by this matrix.
		 */
		_tVector2Type LSE_FCALL							operator * ( const _tVector2Type &_v2bOther ) const {
			_tVector2Type v3bRet;
			v3bRet.x = (_11 * _v2bOther.x + _21 * _v2bOther.y);
			v3bRet.y = (_12 * _v2bOther.x + _22 * _v2bOther.y);
			return v3bRet;
		}

		/**
		 * Multiply each component of the matrix by the given real.
		 *
		 * \param _tValue The real by which to multiply each component of this matrix.
		 * \return The result of the multiplication is returned.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> LSE_FCALL	operator * ( _tType _tValue ) const {
			CMatrix2x2Base<_tType, _tVector2Type> m22bRet;
			m22bRet._11 = _11 * _tValue;
			m22bRet._12 = _12 * _tValue;
			m22bRet._21 = _21 * _tValue;
			m22bRet._22 = _22 * _tValue;
			return m22bRet;
		}

		/**
		 * Multiply each component of the matrix by the given real in-place.
		 *
		 * \param _tValue The real by which to multiply each component of this matrix.
		 * \return The result of the multiplication is returned.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> & LSE_FCALL
														operator *= ( _tType _tValue ) {
			_11 = _11 * _tValue;
			_12 = _12 * _tValue;
			_21 = _21 * _tValue;
			_22 = _22 * _tValue;
			return (*this);
		}

		/**
		 * Performs a componentwise addition between this matrix and the given matrix.
		 *
		 * \param _m22bOther The matrix by which each component is added.
		 * \return Returns this matrix.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> & LSE_FCALL
														operator += ( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bOther ) {
			_11 += _m22bOther._11;
			_12 += _m22bOther._12;
			_21 += _m22bOther._21;
			_22 += _m22bOther._22;
			return (*this);
		}


		// == Functions.
		/**
		 * Sets the identity matrix in place.
		 *
		 * \return Returns this matrix.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> & LSE_FCALL
														Identity() {
			_11 = _22 = _tType( 1.0 );
			_12 = _21 = _tType( 0.0 );
			return (*this);
		}

		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this matrix.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> & LSE_FCALL
														Transpose() {
			CMathLib::Swap( _12, _21 );
			return (*this);
		}

		/**
		 * Sets a row.
		 *
		 * \param _ui32Row The row to set.
		 * \param _vRet The value to apply to the row.
		 */
		void LSE_FCALL								SetRow( uint32_t _ui32Row, const _tVector2Type &_v3bVec ) {
			LSE_UNALIGNED _tType * pfData = &_11;
			pfData += (_ui32Row << 1);
			
			pfData[0] = _v3bVec.x;
			pfData[1] = _v3bVec.y;
		}

		/**
		 * Builds a rotation matrix.
		 *
		 * \param _tAngle The angle, in radians, of the rotation to apply to this matrix.
		 * \return Returns this matrix after the rotation matrix has been applied to it.
		 */
		CMatrix2x2Base<_tType, _tVector2Type> & LSE_FCALL
														MatrixRotation( _tType _tAngle ) {
			LSREAL fS;
			LSREAL fC;
			CMathLib::SinCos( static_cast<LSREAL>(_tAngle), fS, fC );
			_11 = fC;
			_12 = fS;
			_21 = -fS;
			_22 = fC;
			return (*this);
		}

		/**
		 * Multiply two matrices together.  The result represents the transformation M1
		 *	followed by the transformation M2.
		 *
		 * \param _m22bOut The output matrix holding the result of the operation.
		 * \param _m22bM1 The left operand.
		 * \param _m22bM2 The right operand.
		 * \return Returns a pointer to _m22bOut.
		 */
		static CMatrix2x2Base<_tType, _tVector2Type> * LSE_FCALL
														MatrixMultiply( CMatrix2x2Base<_tType, _tVector2Type> &_m22bOut, const CMatrix2x2Base<_tType, _tVector2Type> &_m22bM1, const CMatrix2x2Base<_tType, _tVector2Type> &_m22bM2 ) {
			const CMatrix2x2Base<_tType, _tVector2Type> * LSE_RESTRICT pmSrc1 = &_m22bM1;
			const CMatrix2x2Base<_tType, _tVector2Type> * LSE_RESTRICT pmSrc2 = &_m22bM2;

			for ( int32_t I = 2; --I >= 0; ) {
				const _tType * pfBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[I<<1];
				_tType * pfBufferOut = &reinterpret_cast<_tType *>(&_m22bOut)[I<<1];
				for ( int32_t J = 2; --J >= 0; ) {
					_tType tSum = LSM_ZERO;
					for ( int32_t K = 2; --K >= 0; ) {
						tSum += pfBufferIn[K] * (*pmSrc2)( K, J );
					}
					pfBufferOut[J] = tSum;
				}
			}

			return &_m22bOut;
		}

		/**
		 * Transforms vector (x, y) by a given matrix.  _v2bIn and _v2bOut must not be the same vectors.
		 *
		 * \param _m22bMat The matrix by which to transform the given vector.
		 * \param _v2bIn The vector to transform.
		 * \param _v2bOut The resulting vector after the transformation has been applied.
		 */
		static void LSE_FCALL							MultiplyVec2ByMat2x2( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bMat, const _tVector2Type &_v2bIn, _tVector2Type &_v2bOut ) {
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v2bIn);
			_v2bOut[0] = _m22bMat._11 * pfIn[0] + _m22bMat._21 * pfIn[1];
			_v2bOut[1] = _m22bMat._12 * pfIn[0] + _m22bMat._22 * pfIn[1];
		}

		/**
		 * Transforms vector (x, y) by the transpose of a given matrix.  _v2bIn and _v2bOut must not be the same vectors.
		 *
		 * \param _m22bMat The matrix by which to transform the given vector.
		 * \param _v2bIn The vector to transform.
		 * \param _v2bOut The resulting vector after the transformation has been applied.
		 */
		void LSE_FCALL								MultiplyVec2ByMat2x2Transpose( const CMatrix2x2Base<_tType, _tVector2Type> &_m22bMat, const _tVector2Type &_v2bIn, _tVector2Type &_v2bOut ) {
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v2bIn);
			_v2bOut[0] = _m22bMat._11 * pfIn[0] + _m22bMat._12 * pfIn[1];
			_v2bOut[1] = _m22bMat._21 * pfIn[0] + _m22bMat._22 * pfIn[1];
		}

		// == Members.
#pragma pack( push, 1 )
		_tType											_11, _12;		// Vector 0.
		_tType											_21, _22;		// Vector 1.
#pragma pack( pop )
	};

}	// namespace lsm

#endif	// __LSM_MATRIX2X2BASE_H__
