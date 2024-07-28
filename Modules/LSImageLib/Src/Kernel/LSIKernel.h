/**
 * Copyright L. Spiro 2016
 * All rights reserved.
 *
 * Written by:	Shawn (L. Spiro) Wilcoxen
 *
 * This code may not be sold or traded for any personal gain without express written consent.  You may use
 *	this code in your own projects and modify it to suit your needs as long as this disclaimer remains intact.
 *	You may not take credit for having written this code.
 *
 *
 * Description: A kernel for images.
 */


#ifndef __LSI_KERNEL_H__
#define __LSI_KERNEL_H__

#include "../LSIImageLib.h"

namespace lsi {

	/**
	 * Class CKernel
	 * \brief A kernel for images.
	 *
	 * Description: A kernel for images.
	 */
	class CKernel {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CKernel();
		LSE_CALLCTOR							CKernel( const CKernel &_kOther );
		LSE_CALLCTOR							~CKernel();


		// == Operators.
		/**
		 * Gets a pointer to the start of the given line in the kernel.  Meant to be used for double array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the start of the given row.
		 */
		LSE_INLINE float * LSE_FCALL			operator [] ( uint32_t _ui32Index );

		/**
		 * Gets a read-only pointer to the start of the given line in the kernel.  Meant to be used for double array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the start of the given row.
		 */
		LSE_INLINE const float * LSE_FCALL		operator [] ( uint32_t _ui32Index ) const;

		/**
		 * Copy operator.
		 *
		 * \param _kOther The object to copy.
		 * \return Returns a reference to this object after the copy.
		 */
		LSE_INLINE CKernel & LSE_FCALL			operator = ( const CKernel &_kOther );


		// == Functions.
		/**
		 * Sets the size of the kernel.  The kernel is initialized to all 0's.
		 *
		 * \param _ui32Size The new size of the kernel.
		 * \return Returns true if the new kernel was made.
		 */
		LSE_INLINE LSBOOL LSE_CALL				SetSize( uint32_t _ui32Size );

		/**
		 * Gets the size of the kernel.
		 *
		 * \return Returns the size of the kernel.
		 */
		LSE_INLINE uint32_t LSE_CALL			Size() const;

		/**
		 * Transposes the kernel in-place.
		 */
		LSE_INLINE void LSE_CALL				Transpose();

		/**
		 * Normalizes the kernel.
		 */
		LSE_INLINE void LSE_CALL				Normalize();

		/**
		 * Creates a 3-by-3 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		LSBOOL LSE_CALL							CreateSobel3x3();

		/**
		 * Creates a 5-by-5 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		LSBOOL LSE_CALL							CreateSobel5x5();

		/**
		 * Creates a 7-by-7 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		LSBOOL LSE_CALL							CreateSobel7x7();

		/**
		 * Creates a 9-by-9 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		LSBOOL LSE_CALL							CreateSobel9x9();


	protected :
		// == Members.
		/** The kernel array. */
		float *									m_pfKernel;

		/** Size of the array. */
		uint32_t								m_ui32Size;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Gets a pointer to the start of the given line in the kernel.  Meant to be used for double array access.
	 *
	 * \param _ui32Index Index of the row to get.
	 * \return Returns a pointer to the start of the given row.
	 */
	LSE_INLINE float * LSE_FCALL CKernel::operator [] ( uint32_t _ui32Index ) {
		assert( m_pfKernel );
		return &m_pfKernel[_ui32Index*m_ui32Size];
	}

	/**
	 * Gets a read-only pointer to the start of the given line in the kernel.  Meant to be used for double array access.
	 *
	 * \param _ui32Index Index of the row to get.
	 * \return Returns a pointer to the start of the given row.
	 */
	LSE_INLINE const float * LSE_FCALL CKernel::operator [] ( uint32_t _ui32Index ) const {
		assert( m_pfKernel );
		return &m_pfKernel[_ui32Index*m_ui32Size];
	}

	/**
	 * Copy operator.
	 *
	 * \param _kOther The object to copy.
	 * \return Returns a reference to this object after the copy.
	 */
	LSE_INLINE CKernel & LSE_FCALL CKernel::operator = ( const CKernel &_kOther ) {
		if ( SetSize( _kOther.Size() ) ) {
			CStd::MemCpy( m_pfKernel, _kOther.m_pfKernel, Size() * Size() * sizeof( float ) );
		}
		return (*this);
	}

	// == Functions.
	/**
	 * Sets the size of the kernel.  The kernel is initialized to all 0's.
	 *
	 * \param _ui32Size The new size of the kernel.
	 * \return Returns true if the new kernel was made.
	 */
	LSE_INLINE LSBOOL LSE_CALL CKernel::SetSize( uint32_t _ui32Size ) {
		LSEDELETE [] m_pfKernel;
		m_pfKernel = LSENEW float[_ui32Size*_ui32Size];
		if ( m_pfKernel ) {
			CStd::MemSet( m_pfKernel, 0, _ui32Size * _ui32Size * sizeof( m_pfKernel[0] ) );
			m_ui32Size = _ui32Size;
			return true;
		}
		m_ui32Size = 0;
		return false;
	}

	/**
	 * Gets the size of the kernel.
	 *
	 * \return Returns the size of the kernel.
	 */
	LSE_INLINE uint32_t LSE_CALL CKernel::Size() const {
		return m_ui32Size;
	}

	/**
	 * Transposes the kernel in-place.
	 */
	LSE_INLINE void LSE_CALL CKernel::Transpose() {
		for ( uint32_t Y = 0; Y < Size(); ++Y ) {
			for ( uint32_t X = 0; X < Size(); ++X ) {
				if ( Y == X ) { continue; }
				CStd::Swap( (*this)[Y][X], (*this)[X][Y] );
			}
		}
	}

}	// namespace lsi

#endif	// __LSI_KERNEL_H__
