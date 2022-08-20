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
 * Description: The network manager main header.  All files in the network module include this first.
 */

#include "LSNNetLib.h"

#ifdef LSE_WINDOWS
#include "WinSock/LSNWinSock.h"
#else
#include "PosixSocks/LSNPosixSock.h"
#endif	// #ifdef LSE_WINDOWS


namespace lsn {

#ifdef LSE_WINDOWS
	typedef CWinSock						CBase;
#else
	typedef CPosixSock						CBase;
#endif	// #ifdef LSE_WINDOWS

	// == Members.
	/**
	 * Count of successful initializations.
	 */
	uint32_t CNetLib::m_ui32InitCount = 0;


	// == Functions.
	/**
	 * Initialize the network library.
	 *
	 * \return Returns false if the required network resources are unavailable.
	 */
	LSBOOL LSE_CALL CNetLib::InitNetwork() {
		if ( m_ui32InitCount == 0 ) {
			if ( !CBase::InitNetwork() ) { return false; }
		}
		++m_ui32InitCount;
		return true;
	}

	/**
	 * Shut down the network library.
	 */
	void LSE_CALL CNetLib::ShutDownNetwork() {
		if ( m_ui32InitCount > 0 ) {
			if ( --m_ui32InitCount == 0 ) {
				CBase::ShutDownNetwork();
			}
		}
	}

	/**
	 * Creates a non-blocking UDP socket.
	 *
	 * \return Returns the created UDP socket.
	 */
	LSN_SOCKET LSE_CALL CNetLib::CreateUpdSocket() {
		return CBase::CreateUpdSocket();
	}

	/**
	 * Close a created socket.
	 *
	 * \param _sSock The socket to close.
	 */
	void LSE_CALL CNetLib::CloseSocket( LSN_SOCKET _sSock ) {
		CBase::CloseSocket( _sSock );
	}

	/**
	 * Check a socket to see if it is valid.
	 *
	 * \param _sSock The socket to check.
	 * \return Returns true if the given socket is not invalid.
	 */
	LSBOOL LSE_CALL CNetLib::CheckSocket( LSN_SOCKET _sSock ) {
		return CBase::CheckSocket( _sSock );
	}

	/**
	 * Resolve an address (including domain names) to an IP address.
	 *
	 * \param _pcName Host name to resolve.
	 * \return Returns the IP of the given host name or LSN_NO_ADDRESS if none could be determined.
	 */
	LSN_ADDRESS LSE_CALL CNetLib::LookUpAddress( const char * _pcName ) {
		return CBase::LookUpAddress( _pcName );
	}

	/**
	 * Gets the host name.  Attempts to wait through blocking operations.
	 *
	 * \param _sReturn Holds the returned host name.
	 * \return Returns true if the host name was resolved.
	 */
	LSBOOL LSE_CALL CNetLib::GetHostName( CString &_sReturn ) {
		return CBase::GetHostName( _sReturn );
	}

	/**
	 * Bind a socket to the given IP address and port.  If no IP address is supplied,
	 *	the host name is used.
	 *
	 * \param _sSock The socket to bind.
	 * \param _ui16Port The port to which to bind the socket.
	 * \param _aAddress The address to which to bind the socket.
	 * \return Returns an error code.
	 */
	uint32_t LSE_CALL CNetLib::Bind( LSN_SOCKET _sSock, uint16_t _ui16Port,
		LSN_ADDRESS _aAddress ) {
		return CBase::Bind( _sSock, _ui16Port, _aAddress == 0 ? LSN_NO_ADDRESS : _aAddress );
	}

	/**
	 * Send a packet to a given address.
	 *
	 * \param _aAddress The address.
	 * \param _ui16Port The port.
	 * \param _pvData The data to send.
	 * \param _ui32Len The length in bytes of the data to send.
	 * \param _sSocket The socket through which to send the data.
	 * \return Returns an error code.
	 */
	uint32_t LSE_CALL CNetLib::SendTo( LSN_ADDRESS _aAddress,
		uint16_t _ui16Port,
		const void * _pvData,
		uint32_t _ui32Len,
		LSN_SOCKET _sSocket ) {
		return CBase::SendTo( _aAddress,
			_ui16Port,
			_pvData,
			_ui32Len,
			_sSocket );
	}

	/**
	 * Receive a packet from any connection.  The address of the sender is returned.
	 *
	 * \param _sSocket The socket through which to receive the data.
	 * \param _pvData Buffer to hold the received data, if any.
	 * \param _ui32Len The length in bytes of the buffer to which _pvData points.
	 * \param _aAddress The sender's address.
	 * \param _ui16Port The sender's port.
	 * \return Returns the number of bytes received or an error code.
	 */
	uint32_t LSE_CALL CNetLib::ReceiveFrom( LSN_SOCKET _sSocket,
		void * _pvData,
		uint32_t _ui32Len,
		LSN_ADDRESS &_aAddress,
		uint16_t &_ui16Port ) {
		return CBase::ReceiveFrom( _sSocket,
			_pvData,
			_ui32Len,
			_aAddress,
			_ui16Port );
	}

}	// namespace lsn
