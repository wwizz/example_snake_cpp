/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "network/sockets/bsd/SocketBsdCompat.h"

SocketInitializer::SocketInitializer() {
#ifdef _WIN32
    WSADATA WinsockData;
    WSAStartup(MAKEWORD(2, 2), &WinsockData);
#endif
}

SocketInitializer::~SocketInitializer() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int socket_set_nonblocking(SOCKET_ID fd) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode);
#else
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#endif
}
