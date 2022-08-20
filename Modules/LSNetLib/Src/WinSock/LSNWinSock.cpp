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

#include "LSNWinSock.h"

#ifdef LSE_WINDOWS

#pragma warning( push )

// warning C4127: conditional expression is constant
#pragma warning( disable : 4127 )

namespace lsn {

	// == Members.
	/**
	 * The socket information required by WinSock.
	 */
	WSADATA CWinSock::m_dData;

	// == Functions.
	/**
	 * Initialize the network library for use with WinSock 2.2.
	 *
	 * \return Returns false if the required version of WinSock is unavailable.
	 */
	LSBOOL LSE_CALL CWinSock::InitNetwork() {
		if ( ::WSAStartup( MAKEWORD( 2, 2 ), &m_dData ) != 0 ) {
			return false;
		}
		return true;
	}

	/**
	 * Shut down the network library.
	 */
	void LSE_CALL CWinSock::ShutDownNetwork() {
		::WSACleanup();
	}

	/**
	 * Creates a non-blocking UDP socket.
	 */
	LSN_SOCKET LSE_CALL CWinSock::CreateUpdSocket() {
		LSN_SOCKET sRet = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		// All sockets are non-blocking.
		u_long ulArg = 1;
		::ioctlsocket( sRet, FIONBIO, &ulArg );
		return sRet;
	}

	/**
	 * Close a created socket.
	 *
	 * \param _sSock The socket to close.
	 */
	void LSE_CALL CWinSock::CloseSocket( LSN_SOCKET _sSock ) {
		::closesocket( _sSock );
	}

	/**
	 * Check a socket to see if it is valid.
	 *
	 * \param _sSock The socket to check.
	 * \return Returns true if the given socket is not invalid.
	 */
	LSBOOL LSE_CALL CWinSock::CheckSocket( LSN_SOCKET _sSock ) {
		return _sSock != INVALID_SOCKET;
	}

	/**
	 * Resolve an address (including domain names) to an IP address.
	 *
	 * \param _pcName Host name to resolve.
	 * \return Returns the IP of the given host name or LSN_NO_ADDRESS if none could be determined.
	 */
	LSN_ADDRESS LSE_CALL CWinSock::LookUpAddress( const char * _pcName ) {
		CString sHost;
		if ( !_pcName ) {
			if ( !GetHostName( sHost ) ) {
				return static_cast<LSN_ADDRESS>(LSN_NO_ADDRESS);
			}
			_pcName = sHost.CStr();
		}
		uint32_t ui32Ret = ::inet_addr( _pcName );
		if ( ui32Ret == INADDR_NONE ) {
			// Not a dotted address.  Look up its domain name.
			::hostent * pheHost = ::gethostbyname( _pcName );
			if ( !pheHost ) {
				return static_cast<LSN_ADDRESS>(LSN_NO_ADDRESS);
			}

			// It resolved.
			ui32Ret = (*reinterpret_cast<uint32_t *>(pheHost->h_addr_list[0]));
		}
		return ui32Ret;
	}

	/**
	 * Gets the host name.  Attempts to wait through blocking operations.
	 *
	 * \param _sReturn Holds the returned host name.
	 * \return Returns true if the host name was resolved.
	 */
	LSBOOL LSE_CALL CWinSock::GetHostName( CString &_sReturn ) {
		static const char szBuffer[256] = "\0";
		if ( !_sReturn.Set( szBuffer, LSE_ELEMENTS( szBuffer ) ) ) { return false; }
		LSBOOL bSuccess;
		do {
			bSuccess = false;
			if ( ::gethostname( const_cast<char *>(_sReturn.CStr()), _sReturn.Length() ) == SOCKET_ERROR ) {
				switch ( ::WSAGetLastError() ) {
					case WSAEINPROGRESS : {
						// Try again.
						break;
					}
					default : {
						// Any other condition is a failure.
						_sReturn.Reset();
						return false;
					}

				}
			}
			else {
				bSuccess = true;
			}
		} while ( !bSuccess );
		_sReturn.Snap();
		return true;
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
	uint32_t LSE_CALL CWinSock::Bind( LSN_SOCKET _sSock, uint16_t _ui16Port,
		LSN_ADDRESS _aAddress ) {
		sockaddr_in saAddr = {
			0,
		};
		saAddr.sin_family = AF_INET;
		saAddr.sin_port = ::htons( _ui16Port );
		if ( _aAddress == LSN_NO_ADDRESS ) {
			_aAddress = LookUpAddress( NULL );
			if ( _aAddress == LSN_NO_ADDRESS ) {
				return LSN_FAILURE;
			}
		}
		saAddr.sin_addr.s_addr = _aAddress;

		do {
			if ( ::bind( _sSock, reinterpret_cast<const sockaddr *>(&saAddr), sizeof( saAddr ) ) == SOCKET_ERROR ) {
				switch ( ::WSAGetLastError() ) {
					case WSAEADDRINUSE : { return LSN_IN_USE; }
					case WSAEINPROGRESS : { continue; }
					default : { return LSN_FAILURE; }
				}
			}
			return LSN_SUCCESS;
		} while ( true );
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
	uint32_t LSE_CALL CWinSock::SendTo( LSN_ADDRESS _aAddress,
		uint16_t _ui16Port,
		const void * _pvData,
		uint32_t _ui32Len,
		LSN_SOCKET _sSocket ) {
		sockaddr_in saAddr = {
			0,
		};
		saAddr.sin_family = AF_INET;
		saAddr.sin_port = ::htons( _ui16Port );
		saAddr.sin_addr.s_addr = _aAddress;

		uint32_t ui32Retries = 4;
		do {
			int iSent = ::sendto( _sSocket, reinterpret_cast<const char *>(_pvData), _ui32Len, 0, reinterpret_cast<const sockaddr *>(&saAddr), sizeof( saAddr ) );
			if ( iSent == SOCKET_ERROR ) {
				switch ( ::WSAGetLastError() ) {
					case WSAEWOULDBLOCK : {}
					case WSAEINPROGRESS : {
						// Try again.
						continue;
					}
					case WSAECONNABORTED : { return LSN_CONN_ABORTED; }
					case WSAECONNRESET : { return LSN_CONN_RESET; }
					case WSAENETUNREACH : { return LSN_NET_UNREACHABLE; }
					case WSAEHOSTUNREACH : { return LSN_HOST_UNREACHABLE; }
					case WSAETIMEDOUT : { return LSN_TIMED_OUT; }
					default : { return LSN_FAILURE; }
				}
			}
			else if ( static_cast<uint32_t>(iSent) == _ui32Len ) { return LSN_SUCCESS; }
		} while ( ui32Retries-- );
		return LSN_FAILURE;
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
	uint32_t LSE_CALL CWinSock::ReceiveFrom( LSN_SOCKET _sSocket,
		void * _pvData,
		uint32_t _ui32Len,
		LSN_ADDRESS &_aAddress,
		uint16_t &_ui16Port ) {
		sockaddr_in saAddr = {
			0,
		};
		int iFromLen = sizeof( saAddr );
		int iRet = ::recvfrom( _sSocket, reinterpret_cast<char *>(_pvData), _ui32Len,
			0,
			reinterpret_cast<sockaddr *>(&saAddr), &iFromLen );
		if ( iRet == SOCKET_ERROR ) {
			switch ( ::WSAGetLastError() ) {
				case WSAEWOULDBLOCK : { break; }
				case WSAECONNRESET : { return LSN_CONN_RESET; }
				default : { return LSE_MAXU32; }
			}
		}
		_aAddress = saAddr.sin_addr.s_addr;
		_ui16Port = ::ntohs( saAddr.sin_port );
		
		return static_cast<uint32_t>(iRet);
	}

}	// namespace lsn

#pragma warning( pop )

#endif	// #ifdef LSE_WINDOWS
