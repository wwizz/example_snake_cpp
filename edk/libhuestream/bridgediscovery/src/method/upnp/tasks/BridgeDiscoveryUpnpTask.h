/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <string>

#include "support/threading/Job.h"
#include "support/chrono/Timer.h"

#include "BridgeDiscoveryUpnpListenTask.h"
#include "BridgeDiscoveryUpnpSendTask.h"
#include "method/BridgeDiscoveryMethodUtil.h"

using Task = support::JobTask;

#include "support/network/sockets/_test/SocketUdpDelegator.h"
#define SOCKET_UDP support::SocketUdpDelegator

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
        const std::vector<std::shared_ptr<BridgeDiscoveryResult>> &get_result() const;

    private:
        /**
         Runs the job with BridgeDiscoveryUpnpListenTask. Will be continuously invoked until socket is closed.
         After socket is closed, parses the listen results and signals of the task completion
         */
        void start_listen_task();

        std::string _listen_result;
        // Timer has to be destroyed before socket to prevent data race between timer event lambda and socket destruction
        std::unique_ptr<SOCKET_UDP> _socket;
        std::unique_ptr<support::Timer> _timeout_timer;
        std::shared_ptr<support::Job<BridgeDiscoveryUpnpSendTask>> _send_job;
        std::vector<std::shared_ptr<BridgeDiscoveryResult>> _results;
        CompletionHandler _done;
    };

}  // namespace huesdk


