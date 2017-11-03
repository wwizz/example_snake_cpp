/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_SOCKETS_BSD_SOCKETBSD_H_
#define NETWORK_SOCKETS_BSD_SOCKETBSD_H_

#ifndef SDK_TEST
    #include "network/sockets/bsd/SocketBsdCompat.h"

#if defined(_NX)

#define ERRNO       ::nn::socket::GetLastErrno()

#define CLOSE       ::nn::socket::Close
#define _SOCKET     ::nn::socket::Socket
#define BIND        ::nn::socket::Bind
#define CONNECT     ::nn::socket::Connect
#define SELECT      ::nn::socket::Select
#define SEND        ::nn::socket::Send
#define SENDTO      ::nn::socket::SendTo
#define RECV        ::nn::socket::Recv
#define GETSOCKOPT  ::nn::socket::GetSockOpt
#define SETSOCKOPT  ::nn::socket::SetSockOpt
#define INET_PTON   ::nn::socket::InetPton
#define htonl       ::nn::socket::InetHtonl
#define htons       ::nn::socket::InetHtons

#define SETNONBLOCKING(x) socket_set_nonblocking(x)

#else
    #define ERRNO       SOCKET_ERRNO

    #define CLOSE       ::closesocket
    #define _SOCKET     ::socket
    #define BIND        ::bind
    #define CONNECT     ::connect
    #define SELECT      ::select
    #define SEND        ::send
    #define SENDTO      ::sendto
    #define RECV        ::recv
    #define GETSOCKOPT  ::getsockopt_
    #define SETSOCKOPT  ::setsockopt_
    #define INET_PTON   ::inet_pton

    #define SETNONBLOCKING socket_set_nonblocking
#endif

#else
    // Test interface
    #include "network/sockets/bsd/_test/CMethodDelegator.h"

    #define ERRNO       (huesdk_lib::CMethodDelegator::get_errno())

    #define CLOSE       huesdk_lib::CMethodDelegator::close
    #define _SOCKET     huesdk_lib::CMethodDelegator::socket
    #define BIND        huesdk_lib::CMethodDelegator::bind
    #define CONNECT     huesdk_lib::CMethodDelegator::connect
    #define SELECT      huesdk_lib::CMethodDelegator::select
    #define SENDTO      huesdk_lib::CMethodDelegator::sendto
    #define SEND        huesdk_lib::CMethodDelegator::send
    #define RECV        huesdk_lib::CMethodDelegator::recv
    #define GETSOCKOPT  huesdk_lib::CMethodDelegator::getsockopt
    #define SETSOCKOPT  huesdk_lib::CMethodDelegator::setsockopt
    #define INET_PTON   huesdk_lib::CMethodDelegator::inet_pton

    #define SETNONBLOCKING huesdk_lib::CMethodDelegator::set_nonblocking
#endif

#endif  // NETWORK_SOCKETS_BSD_SOCKETBSD_H_
