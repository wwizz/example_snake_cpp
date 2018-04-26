/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "support/threading/Job.h"
#include "support/threading/QueueExecutor.h"

#include "tasks/BridgeDiscoveryCheckIpArrayTask.h"
#include "method/BridgeDiscoveryMethodUtil.h"

using Task = support::JobTask;

namespace huesdk {

    class BridgeDiscoveryIpscanTask : public Task {
    public:
        BridgeDiscoveryIpscanTask();

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
        std::vector<std::shared_ptr<BridgeDiscoveryResult>> _results;
        support::QueueExecutor _executor;
        std::atomic<bool> _stopped_by_user;
    };

}  // namespace huesdk


