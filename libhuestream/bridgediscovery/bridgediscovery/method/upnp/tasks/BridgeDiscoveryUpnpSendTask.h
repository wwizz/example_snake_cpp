/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPSENDTASK_H_
#define BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPSENDTASK_H_

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
    class BridgeDiscoveryUpnpSendTask : public Task {
    public:
        /**
         Constructor
         @param socket pointer to socket owned by caller
         */
        explicit BridgeDiscoveryUpnpSendTask(SOCKET_UDP* socket);

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         Returns task execution result
         @return true if task was executed successfully
         */
        bool is_successful() const;

    private:
        bool _successful;

        SOCKET_UDP* _socket;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPSENDTASK_H_
