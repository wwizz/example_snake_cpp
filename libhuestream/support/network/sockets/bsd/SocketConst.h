/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_SOCKETS_BSD_SOCKETCONST_H_
#define NETWORK_SOCKETS_BSD_SOCKETCONST_H_

namespace huesdk_lib {

    /** the select timeout of the socket, which determines the socket status */
    const unsigned int SOCKET_SELECT_TIMEOUT = 10000;
    /** buffer size for receiving data */
    const unsigned int SOCKET_BUFFER_SIZE    = 4096;

}  // namespace huesdk_lib


#endif  // NETWORK_SOCKETS_BSD_SOCKETCONST_H_
