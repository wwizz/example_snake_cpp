/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_IPSCAN_TASKS_BRIDGEDISCOVERYIPSCANTASK_H_
#define BRIDGEDISCOVERY_METHOD_IPSCAN_TASKS_BRIDGEDISCOVERYIPSCANTASK_H_

#include <string>
#include <vector>

#include "threading/Job.h"
#include "threading/QueueExecutor.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "bridgediscovery/tasks/BridgeDiscoveryCheckIpArrayTask.h"

using Task = huesdk_lib::JobTask;

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
        const std::vector<BridgeDiscoveryResult> &get_result() const;

    private:
        std::vector<BridgeDiscoveryResult> _results;
        huesdk_lib::QueueExecutor _executor;
        std::atomic<bool> _stopped_by_user;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_METHOD_IPSCAN_TASKS_BRIDGEDISCOVERYIPSCANTASK_H_
