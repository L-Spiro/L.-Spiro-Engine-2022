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
 * Description: Extends the lsstd::CBitStream class with memory-manager functionality.
 */


#ifndef __LSC_BITSTREAM_H__
#define __LSC_BITSTREAM_H__

#include "../LSCCompressionLib.h"
#include "Allocator/LSTLAllocator.h"
#include "Streams/LSSTDBitStream.h"

namespace lsc {

	/**
	 * Class CBitStream
	 * \brief Extends the lsstd::CBitStream class with memory-manager functionality.
	 *
	 * Description: Extends the lsstd::CBitStream class with memory-manager functionality.
	 */
	class CBitStream : public lsstd::CBitStream {
	public :
		// == Various constructors.
		LSE_CALLCTOR							CBitStream();
		LSE_CALLCTOR							~CBitStream();


		// == Functions.
		/**
		 * Sets the allocator.  Causes all data in the bit stream to be lost if the allocator is not the
		 *	same as the only already set.
		 *
		 * \param _paAllocator The allocator to use by this class.
		 */
		void LSE_CALL							SetAllocator( CAllocator * _paAllocator );

		/**
		 * Gets our allocator.
		 *
		 * \return Returns the allocator assigned to this object.
		 */
		CAllocator * LSE_CALL					GetAllocator();


	protected :
		// == Members.
		/**
		 * Our allocator.
		 */
		CAllocator *							m_paAllocator;


		// == Functions.
		/**
		 * Re-allocate bytes.  Overrides the base definition to allow usage of our allocator, if not NULL.
		 *
		 * \param _pvOriginal Address to re-alloate.  Pass NULL to allocate new data.
		 * \param _ui32Total Total number of bytes to allocate.
		 * \return Returns the re-allocated bytes or NULL upon failure.
		 */
		virtual void * LSE_CALL				ReAlloc( void * _pvOriginal, uint32_t _ui32Total ) const;

		/**
		 * Free bytes.  Overrides the base definition to allow usage of our allocator, if not NULL.
		 *
		 * \param _pvOriginal Pointer to the data, allocated with ReAlloc() to free.
		 */
		virtual void LSE_CALL					Free( void * _pvOriginal ) const;


	private :
		typedef lsstd::CBitStream				Parent;
	};

}	// namespace lsc

#endif	// __LSC_BITSTREAM_H__
