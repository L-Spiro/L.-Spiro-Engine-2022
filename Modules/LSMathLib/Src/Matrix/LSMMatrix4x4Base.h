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


#ifndef __LSM_MATRIX4X4BASE_H__
#define __LSM_MATRIX4X4BASE_H__

#include "../LSMMathLib.h"
#include "../Vector/LSMVector3Base.h"
#include "../Vector/LSMVector4Base.h"

namespace lsm {

	/**
	 * Class CMatrix4x4Base
	 * \brief The base matrix template allowing any type of number to be used.  Vectors are stored row-major.
	 *
	 * Description: The base matrix template allowing any type of number to be used.  Vectors are stored row-major.
	 */
	template <typename _tType, typename _tVector3Type, typename _tVector4Type>
	class CMatrix4x4Base {
		// All is public.  This class has no secrets.
	public :
		// == Various constructors.
		LSE_CALLCTOR									CMatrix4x4Base() {
		}
		LSE_CALLCTOR									CMatrix4x4Base( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bOther ) :
			_11( _m44bOther._11 ), _12( _m44bOther._12 ), _13( _m44bOther._13 ), _14( _m44bOther._14 ),
			_21( _m44bOther._21 ), _22( _m44bOther._22 ), _23( _m44bOther._23 ), _24( _m44bOther._24 ),
			_31( _m44bOther._31 ), _32( _m44bOther._32 ), _33( _m44bOther._33 ), _34( _m44bOther._34 ),
			_41( _m44bOther._41 ), _42( _m44bOther._42 ), _43( _m44bOther._43 ), _44( _m44bOther._44 ) {
		}
		LSE_CALLCTOR									CMatrix4x4Base( _tType _t11, _tType _t12, _tType _t13, _tType _t14,
			_tType _t21, _tType _t22, _tType _t23, _tType _t24,
			_tType _t31, _tType _t32, _tType _t33, _tType _t34,
			_tType _t41, _tType _t42, _tType _t43, _tType _t44 ) :
			_11( _t11 ), _12( _t12 ), _13( _t13 ), _14( _t14 ),
			_21( _t21 ), _22( _t22 ), _23( _t23 ), _24( _t24 ),
			_31( _t31 ), _32( _t32 ), _33( _t33 ), _34( _t34 ),
			_41( _t41 ), _42( _t42 ), _43( _t43 ), _44( _t44 ) {
		}
		LSE_CALLCTOR									CMatrix4x4Base( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bOther, LSBOOL _bTranspose ) {
			if ( !_bTranspose ) {
				_11 =_m44bOther._11; _12 =_m44bOther._12; _13 =_m44bOther._13; _14 =_m44bOther._14;
				_21 =_m44bOther._21; _22 =_m44bOther._22; _23 =_m44bOther._23; _24 =_m44bOther._24;
				_31 =_m44bOther._31; _32 =_m44bOther._32; _33 =_m44bOther._33; _34 =_m44bOther._34;
				_41 =_m44bOther._41; _42 =_m44bOther._42; _43 =_m44bOther._43; _44 =_m44bOther._44;
			}
			else {
				_11 =_m44bOther._11; _12 =_m44bOther._21; _13 =_m44bOther._31; _14 =_m44bOther._41;
				_21 =_m44bOther._12; _22 =_m44bOther._22; _23 =_m44bOther._32; _24 =_m44bOther._42;
				_31 =_m44bOther._13; _32 =_m44bOther._23; _33 =_m44bOther._33; _34 =_m44bOther._43;
				_41 =_m44bOther._14; _32 =_m44bOther._24; _43 =_m44bOther._34; _44 =_m44bOther._44;
			}
		}
		explicit LSE_CALLCTOR							CMatrix4x4Base( const _tType * _ptArray ) :
			_11( _ptArray[0] ), _12( _ptArray[1] ), _13( _ptArray[2] ), _14( _ptArray[3] ),
			_21( _ptArray[4] ), _22( _ptArray[5] ), _23( _ptArray[6] ), _24( _ptArray[7] ),
			_31( _ptArray[8] ), _32( _ptArray[9] ), _33( _ptArray[10] ), _34( _ptArray[11] ),
			_41( _ptArray[12] ), _42( _ptArray[13] ), _43( _ptArray[14] ), _44( _ptArray[15] ) {
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
			return reinterpret_cast<_tType *>(this)[(_ui32Row<<2)+_ui32Col];
		}

		/**
		 * Provides row/column access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \param _ui32Col The column of the value to obtain.
		 * \return Returns the value at the given row and column.
		 */
		_tType LSE_FCALL								operator () ( uint32_t _ui32Row, uint32_t _ui32Col ) const {
			return reinterpret_cast<const _tType *>(this)[(_ui32Row<<2)+_ui32Col];
		}

		/**
		 * Provides array access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \return Returns a reference to the value at the given row.  Can be used for reading
		 *	or writing.
		 */
		_tType * LSE_FCALL								operator [] ( uint32_t _ui32Row ) {
			return &reinterpret_cast<_tType *>(this)[_ui32Row<<2];
		}

		/**
		 * Provides array access to the values in the matrix.
		 *
		 * \param _ui32Row The row of the value to obtain.
		 * \return Returns a constant reference to the value at the given row.
		 */
		const _tType * LSE_FCALL						operator [] ( uint32_t _ui32Row ) const {
			return &reinterpret_cast<_tType *>(this)[_ui32Row<<2];
		}

		/**
		 * Multiply this matrix with another, returning the result as M = this * _mOther.
		 *
		 * \param _mOther The matrix by which to multiply this matrix.
		 * \return Returns the result of the matrix combination.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> LSE_FCALL
														operator * ( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_mOther ) const {
			CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> _mOut;
			LSE_UNALIGNED const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> * /*LSE_RESTRICT*/ pmSrc1 = this;
			// WTF.  Using LSE_RESTRICT on this screws it up.  Damn compiler bugs.
			LSE_UNALIGNED const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> * /*LSE_RESTRICT*/ pmSrc2 = &_mOther;

			const _tType * ptBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[3<<2];
			_tType * pfBufferOut = &reinterpret_cast<_tType *>(&_mOut)[3<<2];
			pfBufferOut[3] = ptBufferIn[3] * (*pmSrc2)( 3, 3 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 3 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 3 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 3 );
			pfBufferOut[2] = ptBufferIn[3] * (*pmSrc2)( 3, 2 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 2 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 2 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 2 );
			pfBufferOut[1] = ptBufferIn[3] * (*pmSrc2)( 3, 1 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 1 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 1 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 1 );
			pfBufferOut[0] = ptBufferIn[3] * (*pmSrc2)( 3, 0 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 0 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 0 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 0 );
			
			ptBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[2<<2];
			pfBufferOut = &reinterpret_cast<_tType *>(&_mOut)[2<<2];
			pfBufferOut[3] = ptBufferIn[3] * (*pmSrc2)( 3, 3 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 3 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 3 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 3 );
			pfBufferOut[2] = ptBufferIn[3] * (*pmSrc2)( 3, 2 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 2 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 2 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 2 );
			pfBufferOut[1] = ptBufferIn[3] * (*pmSrc2)( 3, 1 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 1 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 1 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 1 );
			pfBufferOut[0] = ptBufferIn[3] * (*pmSrc2)( 3, 0 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 0 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 0 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 0 );
			
			ptBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[1<<2];
			pfBufferOut = &reinterpret_cast<_tType *>(&_mOut)[1<<2];
			pfBufferOut[3] = ptBufferIn[3] * (*pmSrc2)( 3, 3 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 3 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 3 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 3 );
			pfBufferOut[2] = ptBufferIn[3] * (*pmSrc2)( 3, 2 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 2 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 2 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 2 );
			pfBufferOut[1] = ptBufferIn[3] * (*pmSrc2)( 3, 1 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 1 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 1 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 1 );
			pfBufferOut[0] = ptBufferIn[3] * (*pmSrc2)( 3, 0 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 0 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 0 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 0 );
			
			ptBufferIn = &reinterpret_cast<const _tType *>(pmSrc1)[0<<2];
			pfBufferOut = &reinterpret_cast<_tType *>(&_mOut)[0<<2];
			pfBufferOut[3] = ptBufferIn[3] * (*pmSrc2)( 3, 3 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 3 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 3 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 3 );
			pfBufferOut[2] = ptBufferIn[3] * (*pmSrc2)( 3, 2 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 2 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 2 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 2 );
			pfBufferOut[1] = ptBufferIn[3] * (*pmSrc2)( 3, 1 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 1 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 1 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 1 );
			pfBufferOut[0] = ptBufferIn[3] * (*pmSrc2)( 3, 0 ) +
				ptBufferIn[2] * (*pmSrc2)( 2, 0 ) +
				ptBufferIn[1] * (*pmSrc2)( 1, 0 ) +
				ptBufferIn[0] * (*pmSrc2)( 0, 0 );
			return _mOut;
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
				_31 * _v3bOther.z + _41);
			v3bRet.y = (_12 * _v3bOther.x + _22 * _v3bOther.y +
				_32 * _v3bOther.z + _42);
			v3bRet.z = (_13 * _v3bOther.x + _23 * _v3bOther.y +
				_33 * _v3bOther.z + _43);
			return v3bRet;
		}

		/**
		 * Assignment operator.
		 *
		 * \param _mOther The matrix to copy into this matrix object.
		 * \return Returns this matrix object after copying the given matrix into it.
		 */
		/*CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														operator = ( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_mOther ) {
			_11 = _mOther._11; _12 = _mOther._12; _13 = _mOther._13; _14 = _mOther._14;
			_21 = _mOther._21; _22 = _mOther._22; _23 = _mOther._23; _24 = _mOther._24;
			_31 = _mOther._31; _32 = _mOther._32; _33 = _mOther._33; _34 = _mOther._34;
			_41 = _mOther._41; _42 = _mOther._42; _43 = _mOther._43; _44 = _mOther._44;
			return (*this);
		}*/



		// == Functions.
		/**
		 * Sets the matrix.
		 *
		 * \param _t11 Component to set.
		 * \param _t12 Component to set.
		 * \param _t13 Component to set.
		 * \param _t14 Component to set.
		 * \param _t21 Component to set.
		 * \param _t22 Component to set.
		 * \param _t23 Component to set.
		 * \param _t24 Component to set.
		 * \param _t31 Component to set.
		 * \param _t32 Component to set.
		 * \param _t33 Component to set.
		 * \param _t34 Component to set.
		 * \param _t41 Component to set.
		 * \param _t42 Component to set.
		 * \param _t43 Component to set.
		 * \param _t44 Component to set.
		 */
		void LSE_FCALL								Set( _tType _t11, _tType _t12, _tType _t13, _tType _t14,
			_tType _t21, _tType _t22, _tType _t23, _tType _t24,
			_tType _t31, _tType _t32, _tType _t33, _tType _t34,
			_tType _t41, _tType _t42, _tType _t43, _tType _t44 ) {
#define LSM_MAT_COPY( VAL )  _ ## VAL = _t ## VAL
			LSM_MAT_COPY( 11 ), LSM_MAT_COPY( 12 ), LSM_MAT_COPY( 13 ), LSM_MAT_COPY( 14 );
			LSM_MAT_COPY( 21 ), LSM_MAT_COPY( 22 ), LSM_MAT_COPY( 23 ), LSM_MAT_COPY( 24 );
			LSM_MAT_COPY( 31 ), LSM_MAT_COPY( 32 ), LSM_MAT_COPY( 33 ), LSM_MAT_COPY( 34 );
			LSM_MAT_COPY( 41 ), LSM_MAT_COPY( 42 ), LSM_MAT_COPY( 43 ), LSM_MAT_COPY( 44 );
#undef LSM_MAT_COPY
		}

		/**
		 * Sets the identity matrix in place.
		 *
		 * \return Returns this object.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														Identity() {
			// Set the ones that need to be zero first.
			_12 = _13 = _14 =
			_21 = _23 = _24 =
			_31 = _32 = _34 =
			_41 = _42 = _43 = _tType( 0.0 );

			// Now the ones that need to be 1.
			_11 = _22 = _33 = _44 = _tType( 1.0 );
			return (*this);
		}
		
		/**
		 * Transpose a matrix in place.
		 *
		 * \return Returns this object.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														Transpose() {
			CMathLib::Swap( _12, _21 );
			CMathLib::Swap( _13, _31 );
			CMathLib::Swap( _14, _41 );
			CMathLib::Swap( _23, _32 );
			CMathLib::Swap( _24, _42 );
			CMathLib::Swap( _34, _43 );
			return (*this);
		}

		/**
		 * Copies the transpose of a matrix into another matrix.
		 *
		 * \return Returns the copied object.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> LSE_FCALL
														GetTranspose() const {
			CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> m44bOut;
			m44bOut._11 = _11;
			m44bOut._12 = _21;
			m44bOut._13 = _31;
			m44bOut._14 = _41;

			m44bOut._21 = _12;
			m44bOut._22 = _22;
			m44bOut._23 = _32;
			m44bOut._24 = _42;

			m44bOut._31 = _13;
			m44bOut._32 = _23;
			m44bOut._33 = _33;
			m44bOut._34 = _43;

			m44bOut._41 = _14;
			m44bOut._42 = _24;
			m44bOut._43 = _34;
			m44bOut._44 = _44;

			return m44bOut;
		}
		
		/**
		 * Gets the inverse of this matrix.  Upon failure, the identity matrix is returned.
		 *
		 * \return Returns the inverse of this matrix, or identity on failure.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> LSE_FCALL
														InverseAccurate() const {
			CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> m44bOut;
			_tType fA0 = (*this)( 0, 0 ) * (*this)( 1, 1 ) - (*this)( 0, 1 ) * (*this)( 1, 0 );
			_tType fA1 = (*this)( 0, 0 ) * (*this)( 1, 2 ) - (*this)( 0, 2 ) * (*this)( 1, 0 );
			_tType fA2 = (*this)( 0, 0 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 0 );
			_tType fA3 = (*this)( 0, 1 ) * (*this)( 1, 2 ) - (*this)( 0, 2 ) * (*this)( 1, 1 );
			_tType fA4 = (*this)( 0, 1 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 1 );
			_tType fA5 = (*this)( 0, 2 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 2 );
			_tType fB0 = (*this)( 2, 0 ) * (*this)( 3, 1 ) - (*this)( 2, 1 ) * (*this)( 3, 0 );
			_tType fB1 = (*this)( 2, 0 ) * (*this)( 3, 2 ) - (*this)( 2, 2 ) * (*this)( 3, 0 );
			_tType fB2 = (*this)( 2, 0 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 0 );
			_tType fB3 = (*this)( 2, 1 ) * (*this)( 3, 2 ) - (*this)( 2, 2 ) * (*this)( 3, 1 );
			_tType fB4 = (*this)( 2, 1 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 1 );
			_tType fB5 = (*this)( 2, 2 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 2 );
			_tType fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
			
			LSE_UNALIGNED const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> * LSE_RESTRICT pmSrc = &(*this);
			
			if ( CMathLib::AbsT( fDet ) > LSM_MAT_EPSILON ) {
				register _tType fTemp = (*pmSrc)( 1, 3 );
				m44bOut( 0, 0 ) = +(*pmSrc)( 1, 1 ) * fB5 - (*pmSrc)( 1, 2 ) * fB4 + fTemp * fB3;
					m44bOut( 1, 0 ) = -(*pmSrc)( 1, 0 ) * fB5 + (*pmSrc)( 1, 2 ) * fB2 - fTemp * fB1;
					m44bOut( 2, 0 ) = +(*pmSrc)( 1, 0 ) * fB4 - (*pmSrc)( 1, 1 ) * fB2 + fTemp * fB0;
					m44bOut( 3, 0 ) = -(*pmSrc)( 1, 0 ) * fB3 + (*pmSrc)( 1, 1 ) * fB1 - (*pmSrc)( 1, 2 ) * fB0;
				fTemp = (*pmSrc)( 0, 3 );
				m44bOut( 0, 1 ) = -(*pmSrc)( 0, 1 ) * fB5 + (*pmSrc)( 0, 2 ) * fB4 - fTemp * fB3;
					m44bOut( 1, 1 ) = +(*pmSrc)( 0, 0 ) * fB5 - (*pmSrc)( 0, 2 ) * fB2 + fTemp * fB1;
					m44bOut( 2, 1 ) = -(*pmSrc)( 0, 0 ) * fB4 + (*pmSrc)( 0, 1 ) * fB2 - fTemp * fB0;
					m44bOut( 3, 1 ) = +(*pmSrc)( 0, 0 ) * fB3 - (*pmSrc)( 0, 1 ) * fB1 + (*pmSrc)( 0, 2 ) * fB0;
				fTemp = (*pmSrc)( 3, 3 );
				m44bOut( 0, 2 ) = +(*pmSrc)( 3, 1 ) * fA5 - (*pmSrc)( 3, 2 ) * fA4 + fTemp * fA3;
					m44bOut( 1, 2 ) = -(*pmSrc)( 3, 0 ) * fA5 + (*pmSrc)( 3, 2 ) * fA2 - fTemp * fA1;
					m44bOut( 2, 2 ) = +(*pmSrc)( 3, 0 ) * fA4 - (*pmSrc)( 3, 1 ) * fA2 + fTemp * fA0;
					m44bOut( 3, 2 ) = -(*pmSrc)( 3, 0 ) * fA3 + (*pmSrc)( 3, 1 ) * fA1 - (*pmSrc)( 3, 2 ) * fA0;
				fTemp = (*pmSrc)( 2, 3 );
				m44bOut( 0, 3 ) = -(*pmSrc)( 2, 1 ) * fA5 + (*pmSrc)( 2, 2 ) * fA4 - fTemp * fA3;
					m44bOut( 1, 3 ) = +(*pmSrc)( 2, 0 ) * fA5 - (*pmSrc)( 2, 2 ) * fA2 + fTemp * fA1;
					m44bOut( 2, 3 ) = -(*pmSrc)( 2, 0 ) * fA4 + (*pmSrc)( 2, 1 ) * fA2 - fTemp * fA0;
					m44bOut( 3, 3 ) = +(*pmSrc)( 2, 0 ) * fA3 - (*pmSrc)( 2, 1 ) * fA1 + (*pmSrc)( 2, 2 ) * fA0;
				
				_tType fInvDet = _tType( 1.0 ) / fDet;
				if ( fInvDet != _tType( 1.0 ) ) {
					_tType * pfThis = reinterpret_cast<_tType *>(&m44bOut);
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis) *= fInvDet;
				}
			}
			else {
				m44bOut.Identity();
			}
			
			return m44bOut;
		}

		/**
		 * Gets the inverse of this matrix.  Upon failure, the identity matrix is returned.
		 *
		 * \return Returns the inverse of this matrix, or identity on failure.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> LSE_FCALL
														Inverse() const {
			CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> mResult( *this );
			for( int32_t K = 0; K < 4; K++ ) {
				_tType fT = mResult[K][K];
#if LSM_BASETYPE == LSM_FIXED
				if ( LSE_UNLIKELY( (fT >= _tType( 0.0 )) && (fT < LSM_REAL_EPSILON) ) ) {
					fT = LSM_REAL_EPSILON;
				}
				else if ( LSE_UNLIKELY( (fT < _tType( 0.0 )) && (fT > -LSM_REAL_EPSILON) ) ) {
					fT = -LSM_REAL_EPSILON;
				}
#else
				if ( LSE_UNLIKELY( (fT >= _tType( 0.0 )) && (fT < _tType( 0.000001 )) ) ) {
					fT = _tType( 0.000001 );
				}
				else if ( LSE_UNLIKELY( (fT < _tType( 0.0 )) && (fT > -_tType( 0.000001 )) ) ) {
					fT = -_tType( 0.000001 );
				}
#endif	// #if LSM_BASETYPE == LSM_FIXED
				fT = 1.0f / fT;
				for( int32_t I = 4; --I >= 0; ) {
					mResult[K][I] *= fT;
				}
				mResult[K][K] = fT;
				for( int32_t J = 4; --J >= 0; ) {
					if ( J != K ) {
						_tType tU = mResult[J][K];
						for( int32_t I = 4; --I >= 0; ) {
							// 18.09% branch mispredict.
							/*if ( I != K ) {
								mResult[J][I] -= mResult[K][I] * tU;
							}
							else {
								mResult[J][I] = -tU * fT;
							}*/

							// 15.38% branch mispredict.
							/*if ( I == K ) {
								mResult[J][I] = -tU * fT;
							}
							else {
								mResult[J][I] -= mResult[K][I] * tU;
							}*/

							// 7.12% branch mispredict.
							mResult[J][I] = (I == K) ? -tU * fT :
								mResult[J][I] - mResult[K][I] * tU;

							// 8.42% branch mispredict.
							/*mResult[J][I] = (I != K) ? mResult[J][I] - mResult[K][I] * tU :
								-tU * fT;*/
						}
					}
				}
			}
			return mResult;
		}
		
		/**
		 * Gets the determinant.
		 *
		 * \return Returns the determinant of this matrix.
		 */
		_tType LSE_FCALL								Determ() const {
			_tType fA0 = (*this)( 0, 0 ) * (*this)( 1, 1 ) - (*this)( 0, 1 ) * (*this)( 1, 0 );
			_tType fA1 = (*this)( 0, 0 ) * (*this)( 1, 2 ) - (*this)( 0, 2 ) * (*this)( 1, 0 );
			_tType fA2 = (*this)( 0, 0 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 0 );
			_tType fA3 = (*this)( 0, 1 ) * (*this)( 1, 2 ) - (*this)( 0, 2 ) * (*this)( 1, 1 );
			_tType fA4 = (*this)( 0, 1 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 1 );
			_tType fA5 = (*this)( 0, 2 ) * (*this)( 1, 3 ) - (*this)( 0, 3 ) * (*this)( 1, 2 );
			_tType fB0 = (*this)( 2, 0 ) * (*this)( 3, 1 ) - (*this)( 2, 1 ) * (*this)( 3, 0 );
			_tType fB1 = (*this)( 2, 0 ) * (*this)( 3, 2 ) - (*this)( 2, 2 ) * (*this)( 3, 0 );
			_tType fB2 = (*this)( 2, 0 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 0 );
			_tType fB3 = (*this)( 2, 1 ) * (*this)( 3, 2 ) - (*this)( 2, 2 ) * (*this)( 3, 1 );
			_tType fB4 = (*this)( 2, 1 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 1 );
			_tType fB5 = (*this)( 2, 2 ) * (*this)( 3, 3 ) - (*this)( 2, 3 ) * (*this)( 3, 2 );
			return fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
		}

		/**
		 * Compares, with epsilon, the 2 matrices.
		 *
		 * \param _mOther The matrix against which to compare this matrix.
		 * \param _tEpsilon The epsilon to apply during the compare.
		 * \return Returns true if the 2 matrices are the same compared with _tEpsilon tolerance.
		 */
		LSBOOL LSE_FCALL								CompEpsilon( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_mOther, _tType _tEpsilon ) {
#define LSM_COMP( MEMBER )		CMathLib::EqualsT<_tType>( MEMBER, _mOther.MEMBER, _tEpsilon )
			return LSM_COMP( _11 ) && LSM_COMP( _12 ) && LSM_COMP( _13 ) && LSM_COMP( _14 ) &&
				LSM_COMP( _21 ) && LSM_COMP( _22 ) && LSM_COMP( _23 ) && LSM_COMP( _24 ) &&
				LSM_COMP( _31 ) && LSM_COMP( _32 ) && LSM_COMP( _33 ) && LSM_COMP( _34 ) &&
				LSM_COMP( _41 ) && LSM_COMP( _42 ) && LSM_COMP( _43 ) && LSM_COMP( _44 );
#undef LSM_COMP
		}

		/**
		 * Gets a row.
		 *
		 * \param _ui32Row The row to get.
		 * \param _v3bRet Holds the returned row.
		 */
		_tVector3Type & LSE_FCALL						GetRow( uint32_t _ui32Row, _tVector3Type &_v3bRet ) const {
			LSE_UNALIGNED const _tType * pfData = &_11;
			pfData += (_ui32Row << 2);
			
			_v3bRet.x = pfData[0];
			_v3bRet.y = pfData[1];
			_v3bRet.z = pfData[2];
			return _v3bRet;
		}

		/**
		 * Gets a row.
		 *
		 * \param _ui32Row The row to get.
		 * \param _v4bRet Holds the returned row.
		 */
		_tVector4Type & LSE_FCALL						GetRow( uint32_t _ui32Row, _tVector4Type &_v4bRet ) const {
			LSE_UNALIGNED const _tType * pfData = &_11;
			pfData += (_ui32Row << 2);
			
			_v4bRet.x = pfData[0];
			_v4bRet.y = pfData[1];
			_v4bRet.z = pfData[2];
			_v4bRet.w = pfData[3];
			return _v4bRet;
		}

		/**
		 * Gets a reference to a row.
		 *
		 * \param _ui32Row The row to get.
		 */
		const _tVector3Type & LSE_FCALL					GetRow( const uint32_t _ui32Row ) const {
			LSE_UNALIGNED const _tType * pfData = &_11;
			pfData += (_ui32Row << 2);
			
			return (*reinterpret_cast<const _tVector3Type *>(pfData));
		}

		/**
		 * Sets a row.
		 *
		 * \param _ui32Row The row to set.
		 * \param _v3bVec The value to apply to the row.  The 4th component of the row is not modified.
		 */
		void LSE_FCALL								SetRow( uint32_t _ui32Row, const _tVector3Type &_v3bVec ) {
			LSE_UNALIGNED _tType * pfData = &_11;
			pfData += (_ui32Row << 2);
			
			pfData[0] = _v3bVec.x;
			pfData[1] = _v3bVec.y;
			pfData[2] = _v3bVec.z;
		}

		/**
		 * Sets a row.
		 *
		 * \param _ui32Row The row to set.
		 * \param _v4bVec The value to apply to the row.  The 4th component of the row is not modified.
		 */
		void LSE_FCALL								SetRow( uint32_t _ui32Row, const _tVector4Type &_v4bVec ) {
			LSE_UNALIGNED _tType * pfData = &_11;
			pfData += (_ui32Row << 2);
			
			pfData[0] = _v4bVec.x;
			pfData[1] = _v4bVec.y;
			pfData[2] = _v4bVec.z;
			pfData[3] = _v4bVec.w;
		}

		/**
		 * Creates a right-handed perspective projection matrix based on a field of view.
		 *
		 * \param _tFov Field of view in the y direction, in radians.
		 * \param _tAspect Aspect ratio, defined as view space width divided by height.
		 * \param _tNear Z-value of the near view-plane.
		 * \param _tFar Z-value of the far view-plane.
		 */
		void LSE_FCALL								PerspectiveFovRH( _tType _tFov, _tType _tAspect, _tType _tNear, _tType _tFar ) {
			// Zero out the ones that need to be zero.
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _tType( 0.0 );
			_41 = _42 = _44 = _tType( 0.0 );
			
			_tType fInvDist = _tType( 1.0 ) / (_tNear - _tFar);
			
			// Set the remaining ones.
			_22 = _tType( 1.0 ) / CMathLib::Tan( static_cast<LSREAL>(_tType( 0.5 ) * _tFov) );
			_11 = _22 / _tAspect;

			_33 = _tFar * fInvDist;
			_34 = -_tType( 1.0 );

			_43 = (_tFar * _tNear) * fInvDist;
		}

		/**
		 * Creates a right-handed perspective projection matrix based on a field of view.  The far plane is at infinity.
		 *
		 * \param _tFov Field of view in the y direction, in radians.
		 * \param _tAspect Aspect ratio, defined as view space width divided by height.
		 * \param _tNear Z-value of the near view-plane.
		 */
		void LSE_FCALL								PerspectiveFovRH( _tType _tFov, _tType _tAspect, _tType _tNear ) {
			// Zero out the ones that need to be zero.
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _tType( 0.0 );
			_41 = _42 = _44 = _tType( 0.0 );
			
			// Set the remaining ones.
			_22 = _tType( 1.0 ) / CMathLib::Tan( static_cast<LSREAL>(_tType( 0.5 ) * _tFov) );
			_11 = _22 / _tAspect;

			_33 = LSM_REAL_EPSILON - _tType( 1.0 );
			_34 = -_tType( 1.0 );

			_43 = _tNear * (LSM_REAL_EPSILON - _tType( 2.0 ));
		}

		/**
		 * Creates a customized, right-handed orthographic projection matrix.
		 *
		 * \param _tLeft Minimum x-value of the view volume.
		 * \param _tRight Maximum x-value of the view volume.
		 * \param _tBottom Minimum y-value of the view volume.
		 * \param _tTop Maximum y-value of the view volume.
		 * \param _tNear Minimum z-value of the view volume.
		 * \param _tFar Maximum z-value of the view volume.
		 */
		void LSE_FCALL								OrthoOffCenterRH( _tType _tLeft, _tType _tRight, _tType _tBottom, _tType _tTop, _tType _tNear, _tType _tFar ) {
			// Zero out the ones that need to be zero.
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _34 = _tType( 0.0 );
			
			register _tType fWidth = _tRight - _tLeft;
			register _tType fHeight = _tTop - _tBottom;
			
			_11 = _tType( 2.0 ) / fWidth;
			_22 = _tType( 2.0 ) / fHeight;
			_33 = _tType( -2.0 ) / (_tNear - _tFar);
			
			_41 = (_tRight + _tLeft) / (_tLeft - _tRight);
			_42 = (_tTop + _tBottom) / (_tBottom - _tTop);
			_43 = (_tFar + _tNear) / (_tNear - _tFar);
			_44 = _tType( 1.0 );
		}

		/**
		 * Creates a customized, left-handed orthographic projection matrix.
		 *
		 * \param _tLeft Minimum x-value of the view volume.
		 * \param _tRight Maximum x-value of the view volume.
		 * \param _tBottom Minimum y-value of the view volume.
		 * \param _tTop Maximum y-value of the view volume.
		 * \param _tNear Minimum z-value of the view volume.
		 * \param _tFar Maximum z-value of the view volume.
		 */
		void LSE_FCALL								OrthoOffCenterLH( _tType _tLeft, _tType _tRight, _tType _tBottom, _tType _tTop, _tType _tNear, _tType _tFar ) {
			// Zero out the ones that need to be zero.
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _34 = _tType( 0.0 );
			
			register _tType fWidth = _tRight - _tLeft;
			register _tType fHeight = _tTop - _tBottom;
			
			_11 = _tType( 2.0 ) / fWidth;
			_22 = _tType( 2.0 ) / fHeight;
			_33 = _tType( 1.0 ) / (_tFar - _tNear);
			
			_41 = (_tRight + _tLeft) / (_tLeft - _tRight);
			_42 = (_tTop + _tBottom) / (_tBottom - _tTop);
			_43 = _tNear / (_tNear - _tFar);
			_44 = _tType( 1.0 );
		}

		/**
		 * Creates a matrix that rotates around an arbitrary axis.
		 *
		 * \param _tX The X component of the arbitrary axis.
		 * \param _tY The Y component of the arbitrary axis.
		 * \param _tZ The Z component of the arbitrary axis.
		 * \param _tAngle Angle of rotation in radians.
		 * \return Returns this object.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationAxis( _tType _tX, _tType _tY, _tType _tZ, _tType _tAngle ) {
			LSREAL fS;
			LSREAL fC;
			CMathLib::SinCos( static_cast<LSREAL>(_tAngle), fS, fC );
			
			_tType fT = _tType( 1.0 ) - fC;
			register _tType fTX = fT * _tX;
			register _tType fTY = fT * _tY;
			register _tType fTZ = fT * _tZ;
			_tType fSX = fS * _tX;
			_tType fSY = fS * _tY;
			_tType fSZ = fS * _tZ;
			
			_11 = fTX * _tX + fC;
			_12 = fTX * _tY + fSZ;
			_13 = fTX * _tZ - fSY;
			_14 = _tType( 0.0 );
			
			_21 = fTY * _tX - fSZ;
			_22 = fTY * _tY + fC;
			_23 = fTY * _tZ + fSX;
			_24 = _tType( 0.0 );
			
			_31 = fTZ * _tX + fSY;
			_32 = fTZ * _tY - fSX;
			_33 = fTZ * _tZ + fC;
			_34 = _tType( 0.0 );
			
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );

			return (*this);
		}

		/**
		 * Creates a matrix that rotates around an arbitrary axis.
		 *
		 * \param _v3bAxis The arbitrary axis.
		 * \param _tAngle Angle of rotation in radians.
		 * \return Returns this object.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationAxis( const _tVector3Type &_v3bAxis, _tType _tAngle ) {
			return MatrixRotationAxis( _v3bAxis.x, _v3bAxis.y, _v3bAxis.z, _tAngle );
		}

		/**
		 * Builds a matrix that rotates around the X axis using a left-handed coordinate system.
		 *
		 * \param _tAngle Angle of rotation in radians.  Angles are measured clockwise when looking along the rotation axis toward the origin.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationXLH( _tType _tAngle ) {
			LSREAL fS;
			LSREAL fC;
			CMathLib::SinCos( static_cast<LSREAL>(_tAngle), fS, fC );
			_11 = _tType( 1.0 );
			_12 = _tType( 0.0 );
			_13 = _tType( 0.0 );
			_14 = _tType( 0.0 );
			
			_21 = _tType( 0.0 );
			_22 = fC;
			_23 = fS;
			_24 = _tType( 0.0 );
			
			_31 = _tType( 0.0 );
			_32 = -fS;
			_33 = fC;
			_34 = _tType( 0.0 );
			
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that rotates around the Y axis using a left-handed coordinate system.
		 *
		 * \param _tAngle Angle of rotation in radians.  Angles are measured clockwise when looking along the rotation axis toward the origin.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationYLH( _tType _tAngle ) {
			LSREAL fS;
			LSREAL fC;
			CMathLib::SinCos( static_cast<LSREAL>(_tAngle), fS, fC );
			_11 = fC;
			_12 = _tType( 0.0 );
			_13 = -fS;
			_14 = _tType( 0.0 );
			
			_21 = _tType( 0.0 );
			_22 = _tType( 1.0 );
			_23 = _tType( 0.0 );
			_24 = _tType( 0.0 );
			
			_31 = fS;
			_32 = _tType( 0.0 );
			_33 = fC;
			_34 = _tType( 0.0 );
			
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that rotates around the Z axis using a left-handed coordinate system.
		 *
		 * \param _tAngle Angle of rotation in radians.  Angles are measured clockwise when looking along the rotation axis toward the origin.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationZLH( _tType _tAngle ) {
			LSREAL fS;
			LSREAL fC;
			CMathLib::SinCos( static_cast<LSREAL>(_tAngle), fS, fC );
			_11 = fC;
			_12 = fS;
			_13 = _tType( 0.0 );
			_14 = _tType( 0.0 );
			
			_21 = -fS;
			_22 = fC;
			_23 = _tType( 0.0 );
			_24 = _tType( 0.0 );
			
			_31 = _tType( 0.0 );
			_32 = _tType( 0.0 );
			_33 = _tType( 1.0 );
			_34 = _tType( 0.0 );
			
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that expresses a Euler rotation.
		 *
		 * \param _tYaw Yaw around the Y axis, in radians.
		 * \param _tPitch Pitch around the X axis, in radians.
		 * \param _tRoll Roll around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationYawPitchRoll( _tType _tYaw, _tType _tPitch, _tType _tRoll ) {
			LSREAL fSa, fCa;
			CMathLib::SinCos( static_cast<LSREAL>(_tYaw), fSa, fCa );

			LSREAL fSb, fCb;
			CMathLib::SinCos( static_cast<LSREAL>(_tPitch), fSb, fCb );

			LSREAL fSh, fCh;
			CMathLib::SinCos( static_cast<LSREAL>(_tRoll), fSh, fCh );


			_11 = fCa * fCb;
			_12 = fCa * fSb * fSh - fSa * fCh;
			_13 = fCa * fSb * fCh + fSa * fSh;

			_21 = fSa * fCb;
			_22 = fSa * fSb * fSh + fCa * fCh;
			_23 = fSa * fSb * fCh - fCa * fSh;

			_31 = -fSb;
			_32 = fCb * fSh;
			_33 = fCb * fCh;

			_14 = _tType( 0.0 );
			_24 = _tType( 0.0 );
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler XYZ rotation around a right-handed coordinate system.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationXYZRH( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC2 * fC3;
			_12 = fC1 * fS3 + fC3 * fS1 * fS2;
			_13 = fS1 * fS3 - fC1 * fC3 * fS2;
			_14 = _tType( 0.0 );

			_21 = -fC2 * fS3;
			_22 = fC1 * fC3 - fS1 * fS2 * fS3;
			_23 = fC3 * fS1 + fC1 * fS2 * fS3;
			_24 = _tType( 0.0 );

			_31 = fS2;
			_32 = -fC2 * fS1;
			_33 = fC1 * fC2;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler XYZ rotation around a left-handed coordinate system.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
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
			_14 = _tType( 0.0 );

			_21 = fC3 * fS1 * fS2 - fC1 * fS3;
			_22 = fC1 * fC3 + fS1 * fS2 * fS3;
			_23 = fC2 * fS1;
			_24 = _tType( 0.0 );

			_31 = fC1 * fC3 * fS2 + fS1 * fS3;
			_32 = fC1 * fS2 * fS3 - fC3 * fS1;
			_33 = fC1 * fC2;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler YZX rotation.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationYZX( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC1 * fC2;
			_12 = fS2;
			_13 = -fC2 * fS1;
			_14 = _tType( 0.0 );

			_21 = fS1 * fS3 - fC1 * fC3 * fS2;
			_22 = fC2 * fC3;
			_23 = fC1 * fS3 + fC3 * fS1 * fS2;
			_24 = _tType( 0.0 );

			_31 = fC3 * fS1 + fC1 * fS2 * fS3;
			_32 = -fC2 * fS3;
			_33 = fC1 * fC3 - fS1 * fS2 * fS3;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler ZXY rotation.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationZXY( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC1 * fC3 - fS1 * fS2 * fS3;
			_12 = fC3 * fS1 + fC1 * fS2 * fS3;
			_13 = -fC2 * fS3;
			_14 = _tType( 0.0 );

			_21 = fC2 * fS1;
			_22 = fC1 * fC2;
			_23 = fS2;
			_24 = _tType( 0.0 );

			_31 = fC1 * fS3 + fC3 * fS1 * fS2;
			_32 = fS1 * fS3 - fC1 * fC3 * fS2;
			_33 = fC2 * fC3;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler XZY rotation.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationXZY( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC1 * fC3;
			_12 = fS1 * fS3 + fC1 * fC3 * fS2;
			_13 = fC3 * fS1 * fS2 - fC1 * fS3;
			_14 = _tType( 0.0 );

			_21 = -fS2;
			_22 = fC1 * fC2;
			_23 = fC2 * fS1;
			_24 = _tType( 0.0 );

			_31 = fC2 * fS3;
			_32 = fC1 * fS2 * fS3 - fC3 * fS1;
			_33 = fC1 * fC3 + fS1 * fS2 * fS3;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler YXZ rotation.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationYXZ( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC1 * fC3 + fS1 * fS2 * fS3;
			_12 = fS2 * fS3;
			_13 = fC1 * fS2 * fS3 - fC3 * fS1;
			_14 = _tType( 0.0 );

			_21 = fC3 * fS1 * fS2 - fC1 * fS3;
			_22 = fC2 * fC3;
			_23 = fS1 * fS3 + fC1 * fC3 * fS2;
			_24 = _tType( 0.0 );

			_31 = fC2 * fS1;
			_32 = -fS2;
			_33 = fC1 * fC2;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}

		/**
		 * Builds a matrix that represents a Euler ZYX rotation.
		 *
		 * \param _tX Angle around the X axis, in radians.
		 * \param _tY Angle around the Y axis, in radians.
		 * \param _tZ Angle around the Z axis, in radians.
		 * \return Returns the built matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixRotationZYX( _tType _tX, _tType _tY, _tType _tZ ) {
			LSREAL fS1, fC1;
			CMathLib::SinCos( static_cast<LSREAL>(_tX), fS1, fC1 );

			LSREAL fS2, fC2;
			CMathLib::SinCos( static_cast<LSREAL>(_tY), fS2, fC2 );

			LSREAL fS3, fC3;
			CMathLib::SinCos( static_cast<LSREAL>(_tZ), fS3, fC3 );


			_11 = fC1 * fC2;
			_12 = fC2 * fS1;
			_13 = -fS2;
			_14 = _tType( 0.0 );

			_21 = fC1 * fS2 * fS3 - fC3 * fS1;
			_22 = fC1 * fC3 + fS1 * fS2 * fS3;
			_23 = fC2 * fS3;
			_24 = _tType( 0.0 );

			_31 = fS1 * fS3 + fC1 * fC3 * fS2;
			_32 = fC3 * fS1 * fS2 - fC1 * fS3;
			_33 = fC2 * fC3;
			_34 = _tType( 0.0 );

			// Zero the position.
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			return (*this);
		}
		
		/**
		 * Creates a matrix that represents the given scales.
		 *
		 * \param _tX Scaling factor that is applied along the X axis.
		 * \param _tY Scaling factor that is applied along the Y axis.
		 * \param _tZ Scaling factor that is applied along the Z axis.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixScaling( _tType _tX, _tType _tY, _tType _tZ ) {
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _34 = _tType( 0.0 );
			
			_11 = _tX;
			_22 = _tY;
			_33 = _tZ;
			
			_41 = _tType( 0.0 );
			_42 = _tType( 0.0 );
			_43 = _tType( 0.0 );
			_44 = _tType( 1.0 );
			
			return (*this);
		}

		/**
		 * Creates a matrix that represents the given position.
		 *
		 * \param _tX X-coordinate offset.
		 * \param _tY Y-coordinate offset.
		 * \param _tZ Z-coordinate offset.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixTranslation( _tType _tX, _tType _tY, _tType _tZ ) {
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _34 = _tType( 0.0 );
			_11 = _22 = _33 = _tType( 1.0 );

			_41 = _tX;
			_42 = _tY;
			_43 = _tZ;
			_44 = _tType( 1.0 );

			return (*this);
		}

		/**
		 * Creates a matrix that represents the given position.
		 *
		 * \param _v3tVec3 The position.
		 * \return Returns this matrix.
		 */
		CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> & LSE_FCALL
														MatrixTranslation( const _tVector3Type &_v3tVec3 ) {
			_12 = _13 = _14 = _tType( 0.0 );
			_21 = _23 = _24 = _tType( 0.0 );
			_31 = _32 = _34 = _tType( 0.0 );
			_11 = _22 = _33 = _tType( 1.0 );

			_41 = _v3tVec3.x;
			_42 = _v3tVec3.y;
			_43 = _v3tVec3.z;
			_44 = _tType( 1.0 );

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
		 * Transforms vector (x, y, z, 1) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v3bIn The vector to transform.
		 */
		static _tVector3Type LSE_FCALL					MultiplyVec3( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat, const _tVector3Type &_v3bIn ) {
			_tVector3Type _v3bOut;
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v3bIn);
			_v3bOut.x = _m44bMat._11 * pfIn[0] + _m44bMat._21 * pfIn[1] + 
				_m44bMat._31 * pfIn[2] + _m44bMat._41;
			_v3bOut.y = _m44bMat._12 * pfIn[0] + _m44bMat._22 * pfIn[1] +
				_m44bMat._32 * pfIn[2] + _m44bMat._42;
			_v3bOut.z = _m44bMat._13 * pfIn[0] + _m44bMat._23 * pfIn[1] +
				_m44bMat._33 * pfIn[2] + _m44bMat._43;
			return _v3bOut;
		}

		/**
		 * Transforms the 3D vector normal by the given matrix.  If you want to transform a normal, the matrix you pass to
		 *	this function should be the transpose of the inverse of the matrix you would use to transform a point.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v3bIn The vector to transform.
		 */
		static _tVector3Type LSE_FCALL					MultiplyVec3Normal( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat, const _tVector3Type &_v3bIn ) {
			_tVector3Type _v3bOut;
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v3bIn);
			_v3bOut.x = _m44bMat._11 * pfIn[0] + _m44bMat._21 * pfIn[1] + 
				_m44bMat._31 * pfIn[2];
			_v3bOut.y = _m44bMat._12 * pfIn[0] + _m44bMat._22 * pfIn[1] +
				_m44bMat._32 * pfIn[2];
			_v3bOut.z = _m44bMat._13 * pfIn[0] + _m44bMat._23 * pfIn[1] +
				_m44bMat._33 * pfIn[2];
			return _v3bOut;
		}

		/**
		 * Transforms vector (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v4bIn The vector to transform.
		 */
		static _tVector4Type LSE_FCALL					MultiplyVec4( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat, const _tVector4Type &_v4bIn ) {
			_tVector4Type _v4bOut;
			const _tType * /*LSE_RESTRICT*/ pfIn = reinterpret_cast<const _tType *>(&_v4bIn);
			_v4bOut.x = _m44bMat._11 * pfIn[0] + _m44bMat._21 * pfIn[1] + 
				_m44bMat._31 * pfIn[2] + _m44bMat._41 * pfIn[3];
			_v4bOut.y = _m44bMat._12 * pfIn[0] + _m44bMat._22 * pfIn[1] +
				_m44bMat._32 * pfIn[2] + _m44bMat._42 * pfIn[3];
			_v4bOut.z = _m44bMat._13 * pfIn[0] + _m44bMat._23 * pfIn[1] +
				_m44bMat._33 * pfIn[2] + _m44bMat._43 * pfIn[3];
			_v4bOut.w = _m44bMat._14 * pfIn[0] + _m44bMat._24 * pfIn[1] +
				_m44bMat._34 * pfIn[2] + _m44bMat._44 * pfIn[3];
			return _v4bOut;
		}

		/**
		 * Transforms a batch of vectors (x, y, z, 1) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _pv3bIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pv3bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL							MultiplyVec3Batch( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat,
			const _tVector3Type * _pv3bIn, uint32_t _ui32Total,
			_tVector3Type * _pv3bOut ) {
			while ( _ui32Total-- != 0 ) {
				(*_pv3bOut++) = MultiplyVec3( _m44bMat, (*_pv3bIn++) );
			}
		}

		/**
		 * Transforms a batch of vectors (x, y, z) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _pv3bIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pv3bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL							MultiplyVec3BatchNormal( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat,
			const _tVector3Type * _pv3bIn, uint32_t _ui32Total,
			_tVector3Type * _pv3bOut ) {
			while ( _ui32Total-- != 0 ) {
				(*_pv3bOut++) = MultiplyVec3Normal( _m44bMat, (*_pv3bIn++) );
			}
		}

		/**
		 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _pv4bIn The vectors to transform.
		 * \param _ui32Total Number of vectors to transform.
		 * \param _pv4bOut Holds the transformed vectors, which must not be the same as the input vector list.
		 */
		static void LSE_FCALL							MultiplyVec4Batch( const CMatrix4x4Base<_tType, _tVector3Type, _tVector4Type> &_m44bMat,
			const _tVector4Type * _pv4bIn, uint32_t _ui32Total,
			_tVector4Type * _pv4bOut ) {
			while ( _ui32Total-- != 0 ) {
				(*_pv4bOut++) = MultiplyVec4( _m44bMat, (*_pv4bIn++) );
			}
		}


		// == Members.
#pragma pack( push, 1 )	// Enforce alignment for array access (() operator).
		_tType											_11, _12, _13, _14;
		_tType											_21, _22, _23, _24;
		_tType											_31, _32, _33, _34;
		_tType											_41, _42, _43, _44;
#pragma pack( pop )
	};

}	// namespace lsm

#endif	// __LSM_MATRIX4X4BASE_H__

