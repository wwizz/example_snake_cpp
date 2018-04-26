/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <vector>
#include <string>

#include "support/threading/Job.h"

#include "method/BridgeDiscoveryMethodUtil.h"

using Task = support::JobTask;

namespace huesdk {

    class BridgeDiscoveryCheckIpArrayTask : public Task {
    public:
        using ResultCallback = std::function<void(const BridgeDiscoveryIpCheckResult &)>;

        /**
         Constructor
         @param ips array of ips to check
         @param results_callback optional callback. If provided, will be called every time the result is finished being checked.
         */
        explicit BridgeDiscoveryCheckIpArrayTask(const std::vector<std::string> &ips, const ResultCallback &results_callback = nullptr);

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         Gets task execution result
         @return const reference to result array
        */
        std::vector<BridgeDiscoveryIpCheckResult> &get_result();

    private:
        std::vector<std::string> _ips;
        size_t _jobs_count;
        std::vector<BridgeDiscoveryIpCheckResult> _result;
        ResultCallback _callback;
    };

}  // namespace huesdk


