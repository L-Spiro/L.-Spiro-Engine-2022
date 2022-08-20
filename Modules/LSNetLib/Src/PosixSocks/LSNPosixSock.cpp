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
 
#include "LSNPosixSock.h"

#ifndef LSE_WINDOWS
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace lsn {
	
	// == Functions.
	/**
	 * Initializes the network library for use with Posix Socks.
	 *
	 * \return Returns true.
	 */
	LSBOOL LSE_CALL CPosixSock::InitNetwork() {
		return true;
	}

	/**
	 * Shuts down the network library.
	 */
	void LSE_CALL CPosixSock::ShutDownNetwork() {
	}
	
	/**
	 * Creates a non-blocking UDP socket.
	 */
	LSN_SOCKET LSE_CALL CPosixSock::CreateUpdSocket() {
		LSN_SOCKET sRet = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
		// All sockets are non-blocking.
		::fcntl( sRet, F_SETFL, O_NONBLOCK );
		return sRet;
	}

	/**
	 * Close a created socket.
	 *
	 * \param _sSock The socket to close.
	 */
	void LSE_CALL CPosixSock::CloseSocket( LSN_SOCKET _sSock ) {
		::close( _sSock );
	}

	/**
	 * Check a socket to see if it is valid.
	 *
	 * \param _sSock The socket to check.
	 * \return Returns true if the given socket is not invalid.
	 */
	LSBOOL LSE_CALL CPosixSock::CheckSocket( LSN_SOCKET _sSock ) {
		return _sSock != LSN_INVALID_SOCKET;
	}
	
	/**
	 * Resolve an address (including domain names) to an IP address.
	 *
	 * \param _pcName Host name to resolve.
	 * \return Returns the IP of the given host name or LSN_NO_ADDRESS if none could be determined.
	 */
	LSN_ADDRESS LSE_CALL CPosixSock::LookUpAddress( const char * _pcName ) {
		CString sHost;
		if ( !_pcName ) {
			if ( !GetHostName( sHost ) ) {
				return static_cast<LSN_ADDRESS>(LSN_NO_ADDRESS);
			}
			_pcName = sHost.CStr();
		}
		LSN_ADDRESS aRet = 0;
		if ( ::inet_pton( AF_INET, _pcName, &aRet ) == INADDR_NONE ) {
			// Not a dotted address.  Look up its domain name.
			::hostent * pheHost = ::gethostbyname( _pcName );
			if ( !pheHost ) {
				return static_cast<LSN_ADDRESS>(LSN_NO_ADDRESS);
			}

			// It resolved.
			aRet = (*reinterpret_cast<uint32_t *>(pheHost->h_addr_list[0]));
		}
		return aRet;
	}
	
	/**
	 * Get the host name.  Attempts to wait through blocking operations.
	 *
	 * \param _sReturn Holds the returned host name.
	 * \return Returns true if the host name was resolved.
	 */
	LSBOOL LSE_CALL CPosixSock::GetHostName( CString &_sReturn ) {
		static const char szBuffer[256] = "\0";
		if ( !_sReturn.Set( szBuffer, LSE_ELEMENTS( szBuffer ) ) ) { return false; }
		LSBOOL bSuccess;
		do {
			bSuccess = false;
			if ( ::gethostname( const_cast<char *>(_sReturn.CStr()), _sReturn.Length() ) == -1 ) {
				switch ( errno ) {
					case EINPROGRESS : {
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
	uint32_t LSE_CALL CPosixSock::Bind( LSN_SOCKET _sSock, uint16_t _ui16Port, LSN_ADDRESS _aAddress ) {
		sockaddr_in saThisAddr = {
			sizeof( sockaddr_in ),
		};
		saThisAddr.sin_port = htons( _ui16Port );
		saThisAddr.sin_family = AF_INET;
		if ( _aAddress == LSN_NO_ADDRESS ) {
			_aAddress = LookUpAddress( NULL );
			if ( _aAddress == LSN_NO_ADDRESS ) {
				return LSN_FAILURE;
			}
		}
		saThisAddr.sin_addr.s_addr = _aAddress;

		do {
			if ( ::bind( _sSock, reinterpret_cast<const sockaddr *>(&saThisAddr), sizeof( saThisAddr ) ) == -1 ) {
				switch ( errno ) {
					case EADDRINUSE : { return LSN_IN_USE; }
					case EINPROGRESS : { continue; }
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
	uint32_t LSE_CALL CPosixSock::SendTo( LSN_ADDRESS _aAddress,
		uint16_t _ui16Port,
		const void * _pvData,
		uint32_t _ui32Len,
		LSN_SOCKET _sSocket ) {
		sockaddr_in saThisAddr = {
			sizeof( sockaddr_in ),
		};
		saThisAddr.sin_family = AF_INET;
		saThisAddr.sin_port = htons( _ui16Port );
		saThisAddr.sin_addr.s_addr = _aAddress;

		uint32_t ui32Retries = 4;
		do {
			ssize_t sSent = ::sendto( _sSocket, reinterpret_cast<const char *>(_pvData), _ui32Len, 0, reinterpret_cast<const sockaddr *>(&saThisAddr), sizeof( saThisAddr ) );
			if ( sSent == -1 ) {
				switch ( errno ) {
					case EWOULDBLOCK : {}
					case EINPROGRESS : {
						// Try again.
						continue;
					}
					case ECONNABORTED : { return LSN_CONN_ABORTED; }
					case ECONNRESET : { return LSN_CONN_RESET; }
					case ENETUNREACH : { return LSN_NET_UNREACHABLE; }
					case EHOSTUNREACH : { return LSN_HOST_UNREACHABLE; }
					case ETIMEDOUT : { return LSN_TIMED_OUT; }
					default : { return LSN_FAILURE; }
				}
			}
			else if ( static_cast<uint32_t>(sSent) == _ui32Len ) { return LSN_SUCCESS; }
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
	uint32_t LSE_CALL CPosixSock::ReceiveFrom( LSN_SOCKET _sSocket,
		void * _pvData,
		uint32_t _ui32Len,
		LSN_ADDRESS &_aAddress,
		uint16_t &_ui16Port ) {
		sockaddr_in saAddr = {
			0,
		};
		socklen_t slFromLen = sizeof( saAddr );
		ssize_t sRet = ::recvfrom( _sSocket, reinterpret_cast<char *>(_pvData), _ui32Len,
			0,
			reinterpret_cast<sockaddr *>(&saAddr), &slFromLen );
		if ( sRet == -1 ) {
			switch ( errno ) {
				case EWOULDBLOCK : { break; }
				case ECONNRESET : { return LSN_CONN_RESET; }
				default : { return LSE_MAXU32; }
			}
		}
		_aAddress = saAddr.sin_addr.s_addr;
		_ui16Port = ntohs( saAddr.sin_port );
		
		return static_cast<uint32_t>(sRet);
	}

}	// namespace lsn

#endif	// #ifndef LSE_WINDOWS
