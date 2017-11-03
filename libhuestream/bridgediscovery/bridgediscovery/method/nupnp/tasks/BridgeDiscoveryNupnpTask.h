/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_NUPNP_TASKS_BRIDGEDISCOVERYNUPNPTASK_H_
#define BRIDGEDISCOVERY_METHOD_NUPNP_TASKS_BRIDGEDISCOVERYNUPNPTASK_H_

#include <string>
#include <vector>

#include "threading/Job.h"
#include "network/http/HttpRequestError.h"
#include "network/http/IHttpResponse.h"

#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"

using Task = huesdk_lib::JobTask;

namespace huesdk {

    class BridgeDiscoveryNupnpTask : public Task {
    public:
        /**
         Constructor
         @param url Hue portal URL
         */
        explicit BridgeDiscoveryNupnpTask(const std::string &url);

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         Gets task execution result
         @return const reference to result array
         */
        const std::vector<BridgeDiscoveryResult> &get_result() const;

    private:
        std::string _url;
        std::vector<BridgeDiscoveryResult> _results;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_METHOD_NUPNP_TASKS_BRIDGEDISCOVERYNUPNPTASK_H_
