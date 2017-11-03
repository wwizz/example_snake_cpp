/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPTASK_H_
#define BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPTASK_H_

#include <vector>
#include <memory>
#include <string>
#include "threading/Job.h"
#include "chrono/Timer.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "BridgeDiscoveryUpnpListenTask.h"
#include "BridgeDiscoveryUpnpSendTask.h"

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

    class BridgeDiscoveryUpnpTask : public Task {
    public:
        BridgeDiscoveryUpnpTask();

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         @see Job.h
         */
        void stop() override;

        /**
         Gets task execution result
         @return const reference to result array
         */
        const std::vector<BridgeDiscoveryResult> &get_result() const;

    private:
        /**
         Runs the job with BridgeDiscoveryUpnpListenTask. Will be continuously invoked until socket is closed.
         After socket is closed, parses the listen results and signals of the task completion
         */
        void start_listen_task();

        std::string _listen_result;
        std::unique_ptr<huesdk_lib::Timer> _timeout_timer;
        std::unique_ptr<SOCKET_UDP> _socket;
        std::shared_ptr<huesdk_lib::Job<BridgeDiscoveryUpnpSendTask>> _send_job;
        std::vector<BridgeDiscoveryResult> _results;
        CompletionHandler _done;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_METHOD_UPNP_TASKS_BRIDGEDISCOVERYUPNPTASK_H_
