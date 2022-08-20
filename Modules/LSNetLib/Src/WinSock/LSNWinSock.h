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
 * Description: The interface for the WinSock 2.2 sockets implementation.  Our interface is the same as for any
 *	other sockets implementation.
 */


#ifndef __LSN_WINSOCK_H__
#define __LSN_WINSOCK_H__

#include "../LSNNetLib.h"
#include "String/LSTLString.h"

#ifdef LSE_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>

namespace lsn {

	// == Enumerations.
	/**
	 * Errors codes.
	 */
	enum LSN_ERRORS {
		LSN_FAILURE									= 0,
		LSN_SUCCESS									= 1,
		LSN_NO_ADDRESS								= INADDR_NONE,
		LSN_CONN_ABORTED							= WSAECONNABORTED,
		LSN_CONN_RESET								= WSAECONNRESET,
		LSN_NET_UNREACHABLE							= WSAENETUNREACH,
		LSN_HOST_UNREACHABLE						= WSAEHOSTUNREACH,
		LSN_TIMED_OUT								= WSAETIMEDOUT,
		LSN_IN_USE									= WSAEADDRINUSE,
		
		LSN_FORCE_DWORD								= 0x7FFFFFFF
	};

	/**
	 * Class CWinSock
	 * \brief The interface for the WinSock 2.2 sockets implementation.
	 *
	 * Description: The interface for the WinSock 2.2 sockets implementation.  Our interface is the same as for any
	 *	other sockets implementation.
	 */
	class CWinSock {
	public :
		// == Functions.
		/**
		 * Initialize the network library for use with WinSock 2.2.
		 *
		 * \return Returns false if the required version of WinSock is unavailable.
		 */
		static LSBOOL LSE_CALL						InitNetwork();

		/**
		 * Shut down the network library.
		 */
		static void LSE_CALL						ShutDownNetwork();

		/**
		 * Creates a non-blocking UDP socket.
		 *
		 * \return Returns the created UDP socket.
		 */
		static LSN_SOCKET LSE_CALL					CreateUpdSocket();

		/**
		 * Close a created socket.
		 *
		 * \param _sSock The socket to close.
		 */
		static void LSE_CALL						CloseSocket( LSN_SOCKET _sSock );

		/**
		 * Check a socket to see if it is valid.
		 *
		 * \param _sSock The socket to check.
		 * \return Returns true if the given socket is not invalid.
		 */
		static LSBOOL LSE_CALL						CheckSocket( LSN_SOCKET _sSock );

		/**
		 * Resolve an address (including domain names) to an IP address.
		 *
		 * \param _pcName Host name to resolve.
		 * \return Returns the IP of the given host name or LSN_NO_ADDRESS if none could be determined.
		 */
		static LSN_ADDRESS LSE_CALL					LookUpAddress( const char * _pcName );

		/**
		 * Gets the host name.  Attempts to wait through blocking operations.
		 *
		 * \param _sReturn Holds the returned host name.
		 * \return Returns true if the host name was resolved.
		 */
		static LSBOOL LSE_CALL						GetHostName( CString &_sReturn );

		/**
		 * Bind a socket to the given IP address and port.  If no IP address is supplied,
		 *	the host name is used.
		 *
		 * \param _sSock The socket to bind.
		 * \param _ui16Port The port to which to bind the socket.
		 * \param _aAddress The address to which to bind the socket.
		 * \return Returns an error code.
		 */
		static uint32_t LSE_CALL					Bind( LSN_SOCKET _sSock, uint16_t _ui16Port,
			LSN_ADDRESS _aAddress = LSN_NO_ADDRESS );

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
		static uint32_t LSE_CALL					SendTo( LSN_ADDRESS _aAddress,
			uint16_t _ui16Port,
			const void * _pvData,
			uint32_t _ui32Len,
			LSN_SOCKET _sSocket );

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
		static uint32_t LSE_CALL					ReceiveFrom( LSN_SOCKET _sSocket,
			void * _pvData,
			uint32_t _ui32Len,
			LSN_ADDRESS &_aAddress,
			uint16_t &_ui16Port );


	protected :
		// == Members.
		/**
		 * The socket information required by WinSock.
		 */
		static WSADATA								m_dData;
	};

}	// namespace lsn

#endif	// #ifdef LSE_WINDOWS

#endif	// __LSN_WINSOCK_H__
