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

#include "LSMMatrix4x4.h"

#if defined( __ARM_NEON__ ) && LSM_BASETYPE == LSM_FLOAT
#elif LSE_X64 && LSM_BASETYPE == LSM_FLOAT
#else

namespace lsm {

	// == Members.
	/** An identity matrix. */
	const CMatrix4x4 CMatrix4x4::m_mIdentity = CMatrix4x4( LSM_ONE, LSM_ZERO, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ONE, LSM_ZERO, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ONE, LSM_ZERO,
		LSM_ZERO, LSM_ZERO, LSM_ZERO, LSM_ONE );

	// == Various constructors.
	LSE_CALLCTOR CMatrix4x4::CMatrix4x4() {
	}
	LSE_CALLCTOR CMatrix4x4::CMatrix4x4( const CMatrix4x4 &_mOther ) :
		Parent( _mOther ) {
	}
	LSE_CALLCTOR CMatrix4x4::CMatrix4x4( const CMatrix4x4 &_mOther, LSBOOL _bTranspose ) :
		Parent( _mOther, _bTranspose ) {
	}
	LSE_CALLCTOR CMatrix4x4::CMatrix4x4( const LSREAL * _pfArray ) :
		Parent( _pfArray ) {
	}

	// == Functions.
#if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT)
#if 0
	/**
	 * Transforms a batch of vectors (x, y, z, w) by a given matrix.
	 *
	 * \param _mMat The matrix by which to transform the given vector.
	 * \param _pvIn The vectors to transform.
	 * \param _ui32Total Number of vectors to transform.
	 * \param _v4bOut Holds the transformed vectors, which must not be the same as the input vector list.
	 */
	void LSE_FCALL CMatrix4x4::MultiplyVec4Batch( const CMatrix4x4 &_mMat,
		const CVector4 * _pvIn, uint32_t _ui32Total,
		CVector4 * _pvOut ) {
		const LSREAL * pfMat = reinterpret_cast<const LSREAL *>(&_mMat);
		static const uint32_t ui32VecSize = (sizeof( CVector4 ) << 1);
	
		// If there is an odd number, do the first vectors manually and adjust.
		if ( _ui32Total & 0x01 ) {
			--_ui32Total;
			_pvOut[0] = MultiplyVec4( _mMat, _pvIn[0] );
			++_pvIn;
			++_pvOut;
		}

		// We do 2 at a time, so divide the total in half.
		_ui32Total >>= 1;


		LSE_ASM_BEGIN
			mov			esi, _pvIn
			mov			edi, _pvOut
			mov			ecx, _ui32Total
			
			// Load columns of matrix into xmm4-7.
			mov			edx, pfMat
			movaps		xmm4, [edx]
			movaps		xmm5, [edx+0x10]
			movaps		xmm6, [edx+0x20]
			movaps		xmm7, [edx+0x30]
			
		BT2_START :
			// Process X (hiding the prefetches in the delays).
			movss		xmm1, [esi+0x00]
			movss		xmm3, [esi+0x10]
			shufps		xmm1, xmm1, 0x00
			prefetchnta	[edi+0x30]
			shufps		xmm3, xmm3, 0x00
			mulps		xmm1, xmm4
			prefetchnta	[esi+0x30]
			mulps		xmm3, xmm4
			
			// Process Y.
			movss		xmm0, [esi+0x04]
			movss		xmm2, [esi+0x14]
			shufps		xmm0, xmm0, 0x00
			shufps		xmm2, xmm2, 0x00
			mulps		xmm0, xmm5
			mulps		xmm2, xmm5
			addps		xmm1, xmm0
			addps		xmm3, xmm2
			
			// Process Z (hiding some pointer arithmetic between
			//	the multiplies).
			movss		xmm0, [esi+0x08]
			movss		xmm2, [esi+0x18]
			shufps		xmm0, xmm0, 0x00
			shufps		xmm2, xmm2, 0x00
			mulps		xmm0, xmm6
			mulps		xmm2, xmm6
			addps		xmm1, xmm0
			addps		xmm3, xmm2
			
			// Process W.
			movss		xmm0, [esi+0x0C]
			movss		xmm2, [esi+0x1C]
			shufps		xmm0, xmm0, 0x00
			shufps		xmm2, xmm2, 0x00
			mulps		xmm0, xmm7
			add			esi, ui32VecSize
			mulps		xmm2, xmm7
			add			edi, ui32VecSize
			addps		xmm1, xmm0
			addps		xmm3, xmm2
			
			// Write output vectors to memory and loop.
			movaps		[edi-0x20], xmm1
			movaps		[edi-0x10], xmm3
			dec			ecx
			jnz			BT2_START
		LSE_ASM_END
	}
#endif	// #if 0
#endif	// #if defined( LSE_X86 ) && (LSM_BASETYPE == LSM_FLOAT)

}	// namespace lsm

#endif	// #if defined( __ARM_NEON__ ) && LSM_BASETYPE == LSM_FLOAT
