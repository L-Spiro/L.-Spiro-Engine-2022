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
 * Description: The interface for the POSIX sockets implementation.
 */


#ifndef __LSNPOSIXSOCK_H__
#define __LSNPOSIXSOCK_H__

#include "../LSNNetLib.h"

#ifndef LSE_WINDOWS
#include <errno.h>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#define LSN_INVALID_SOCKET							-1	/**< An invalid socket identifier. */



namespace lsn {

	// == Enumerations.
	/**
	 * Errors codes.
	 */
	enum LSN_ERRORS {
		LSN_FAILURE									= 0,
		LSN_SUCCESS									= 1,
		LSN_NO_ADDRESS,
		LSN_CONN_ABORTED							= ECONNABORTED,
		LSN_CONN_RESET								= ECONNRESET,
		LSN_NET_UNREACHABLE							= ENETUNREACH,
		LSN_HOST_UNREACHABLE						= EHOSTUNREACH,
		LSN_TIMED_OUT								= ETIMEDOUT,
		LSN_IN_USE									= EADDRINUSE,		
		LSN_FORCE_DWORD								= 0x7FFFFFFF
	};

	/**
	 * Class CPosixSock
	 * \brief The interface for the Posix Socks sockets implementation.
	 *
	 * Description: The interface for the Posix Socks sockets implementation.  Our interface is the same as for any
	 *	other sockets implementation.
	 */
	class CPosixSock {
	public :
		// == Functions.
		/**
		 * Initializes the network library for use with Posix Socks.
		 *
		 * \return Returns true.
		 */
		static LSBOOL LSE_CALL						InitNetwork();

		/**
		 * Shuts down the network library.
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
		 * Get the host name.  Attempts to wait through blocking operations.
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
	};

}	// namespace lsn

#endif	// #ifndef LSE_WINDOWS

#endif	// #ifndef __LSNPOSIXSOCK_H__
