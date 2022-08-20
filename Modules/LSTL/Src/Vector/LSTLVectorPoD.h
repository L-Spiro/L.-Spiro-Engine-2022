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
 * Description: Inherits from a simple vector and adds a member that allows defining an allocator.  In order to
 *	use the allocator the Allocate() and Reset() functions are replaced.  This vector should be used only for
 *	plain data types.
 */


#ifndef __LSTL_VECTORPOD_H__
#define __LSTL_VECTORPOD_H__

#include "../LSTLib.h"
#include "LSTLSVectorCrtp.h"

namespace lstl {

	/**
	 * Class CVectorPoD
	 * \brief Adds the ability to supply a specific allocator to a simple vector.
	 *
	 * Description: Inherits from a simple vector and adds a member that allows defining an allocator.  In order to
	 *	use the allocator the Allocate() and Reset() functions are replaced.  This vector should be used only for
	 *	plain data types.
	 */
	template <typename _tType, typename _tDataType = uint16_t, unsigned _uAllocSize = 512>
	class CVectorPoD : public CSVectorCrtp<CVectorPoD<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize> {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CVectorPoD() :
			m_paOurAllocator( Parent::m_paDefaultAllocator ) {
		}
		explicit LSE_CALLCTOR					CVectorPoD( uint32_t _tTotal ) :
			m_paOurAllocator( Parent::m_paDefaultAllocator ) {
			Allocate( _tTotal );
		}
		explicit LSE_CALLCTOR					CVectorPoD( CAllocator * _paAllocator ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
		}
		LSE_CALLCTOR							CVectorPoD( uint32_t _tTotal, CAllocator * _paAllocator ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
			Allocate( _tTotal );
		}
		LSE_CALLCTOR							CVectorPoD( const CVectorPoD<_tType, _tDataType, _uAllocSize> &_vOther, CAllocator * _paAllocator = NULL ) :
			m_paOurAllocator( _paAllocator ) {
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
			(*this) = _vOther;
		}
		LSE_CALLCTOR							~CVectorPoD() {
			Reset();
		}


		/**
		 * Allocates a given number of elements.
		 *	If the allocation is less than what there already is, items are removed.
		 *
		 * \param _ui32Total The new total number of values to allocate.
		 * \return Returns false if there is not enough memory to allocate the requested amount.
		 */
		LSBOOL LSE_CALL							Allocate( uint32_t _ui32Total ) {
			// If allocating 0 bytes, just reset the list.
			if ( !_ui32Total ) {
				Reset();
				return true;
			}

			// Destroy items that are going to be removed.
			if ( Parent::m_tLen ) {
				for ( uint32_t I = Parent::m_tLen; --I >= _ui32Total; ) {
					Parent::Destroy( static_cast<_tDataType>(I) );
				}
			}
			// Adjust the length.
			if ( Parent::m_tLen > static_cast<_tDataType>(_ui32Total) ) { Parent::m_tLen = static_cast<_tDataType>(_ui32Total); }

			// Attempt to allocate.
			_tType * ptNew = reinterpret_cast<_tType *>(m_paOurAllocator->ReAlloc( Parent::m_ptData, _ui32Total * sizeof( _tType ) ));
			if ( !ptNew ) { return false; }

			// Success.
			Parent::m_ptData = ptNew;
			Parent::m_tAllocated = static_cast<_tDataType>(_ui32Total);
			return true;
		}

		/**
		 * Reset the list completely.
		 */
		void LSE_CALL							Reset() {
			for ( uint32_t I = Parent::m_tLen; I--; ) {
				Parent::Destroy( static_cast<_tDataType>(I) );
			}
			if ( Parent::m_ptData ) {
				m_paOurAllocator->Free( Parent::m_ptData );
				Parent::m_ptData = NULL;
			}
			Parent::m_tLen = Parent::m_tAllocated = 0;
		}

		/**
		 * Sets the allocator.  Causes the object to be fully reset.  Should not be used on
		 *	lists that have contents already.
		 *
		 * \param _paAllocator The new allocator to be used by this object or NULL to use the
		 *	default allocator for vector objects.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator ) {
			Reset();
			m_paOurAllocator = _paAllocator;
			if ( !m_paOurAllocator ) {
				m_paOurAllocator = Parent::m_paDefaultAllocator;
			}
		}

		/**
		 * Gets our allocator.
		 *
		 * \return Returns a pointer to the allocator used by this object.
		 */
		CAllocator * LSE_CALL					GetAllocator() {
			return m_paOurAllocator;
		}

	protected :
		// == Members.
		/**
		 * The allocator we should use.
		 */
		CAllocator *							m_paOurAllocator;

	private :
		typedef CSVectorCrtp<CVectorPoD<_tType, _tDataType, _uAllocSize>, _tType, _tDataType, _uAllocSize>
			Parent;
	};


}	// namespace lstl

#endif	// __LSTL_VECTORPOD_H__
