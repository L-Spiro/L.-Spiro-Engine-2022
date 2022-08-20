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
 * Description: A standard 32-bit cyclic redundancy check calculator.
 */


#ifndef __LSSTD_CRC_H__
#define __LSSTD_CRC_H__

#include "../LSSTDStandardLib.h"

namespace lsstd {

	/**
	 * Class CCrc
	 * \brief A standard 32-bit cyclic redundancy check calculator.
	 *
	 * Description: A standard 32-bit cyclic redundancy check calculator.
	 */
	class CCrc {
	public :
		// == Functions.
		/**
		 * Calculate the standard 32-bit CRC value for a given set of bytes.
		 *
		 * \param _pui8Data The data for which to calculate a CRC value.
		 * \param _uiptrLen Length of the data.
		 * \return Returns the standard 32-bit CRC value for the given data
		 */
		static uint32_t LSE_CALL		GetCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen );

		/**
		 * Calculate the alternative 32-bit CRC value for a given set of bytes.
		 *
		 * \param _pui8Data The data for which to calculate a CRC value.
		 * \param _uiptrLen Length of the data.
		 * \return Returns the alternative 32-bit CRC value for the given data
		 */
		static uint32_t LSE_CALL		GetAltCrc( const uint8_t * _pui8Data, uintptr_t _uiptrLen );


	protected :
		// == Members.
		/**
		 * Standard table of codes.
		 */
		static uint32_t					m_ui32StdTable[256];

		/**
		 * Alternative table of codes.
		 */
		static uint32_t					m_ui32AltTable[256];

		/**
		 * Has the table been initialized?
		 */
		static LSBOOL					m_bInit;


		// == Functions.
		/**
		 * Initialize the tables.
		 */
		static void LSE_CALL			Init();

		/**
		 * Initialize a table with a key.
		 *
		 * \param _pui32Table The table to initialize
		 * \param _ui32Key The key to use for initialization.
		 */
		static void LSE_CALL			Init( uint32_t * _pui32Table, uint32_t _ui32Key );

		/**
		 * Calculates the CRC of a given byte, modifying the previous CRC value in-place.
		 *
		 * \param _pui32Table The table to use.
		 * \param _ui8Val The byte to add to the CRC calculation.
		 * \param _ui32Crc The CRC to update.
		 */
		static void LSE_CALL			GetCrc( uint32_t * _pui32Table, uint8_t _ui8Val, uint32_t &_ui32Crc );
	};

}	// namespace lsstd

#endif	// __LSSTD_CRC_H__
