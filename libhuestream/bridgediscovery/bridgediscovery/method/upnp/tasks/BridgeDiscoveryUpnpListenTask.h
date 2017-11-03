/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPLISTENTASK_H_
#define BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPLISTENTASK_H_

#include <string>
#include "threading/Job.h"

using Task = huesdk_lib::JobTask;

#ifndef SDK_TEST
#include "network/sockets/SocketUdp.h"
#define SOCKET_UDP huesdk_lib::SocketUdp
#else
// Test interface
#include "network/sockets/_test/SocketUdpDelegator.h"
#define SOCKET_UDP huesdk_lib::SocketUdpDelegator
#endif

namespace huesdk {
    class BridgeDiscoveryUpnpListenTask : public Task {
    public:
        /**
         Constructor
         @param socket pointer to socket owned by caller
        */
        explicit BridgeDiscoveryUpnpListenTask(SOCKET_UDP* socket, const string &result = "");

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         Gets task execution result
         @return const reference to result string
         */
        const string &get_result() const;

    private:
        SOCKET_UDP* _socket;
        std::string _result;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPLISTENTASK_H_
