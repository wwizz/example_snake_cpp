/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "support/threading/Job.h"
#include "support/network/http/HttpRequestError.h"
#include "support/network/http/IHttpResponse.h"

#include "method/BridgeDiscoveryMethodUtil.h"

using Task = support::JobTask;

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
        const std::vector<std::shared_ptr<BridgeDiscoveryResult>> &get_result() const;

    private:
        std::string _url;
        std::vector<std::shared_ptr<BridgeDiscoveryResult>> _results;
    };

}  // namespace huesdk


