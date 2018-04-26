/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <thread>
#include <string>

#include "support/threading/Job.h"

#include "bridgediscovery/BridgeDiscoveryIpCheckResult.h"

namespace support {
    class HttpRequestError;
    class IHttpResponse;
}

namespace huesdk {

    class BridgeDiscoveryCheckIpTask : public support::JobTask {
    public:
        /**
         Constructor
         * @param _ip ip to check
         */
        explicit BridgeDiscoveryCheckIpTask(const std::string &_ip);

        /**
         @see Job.h
         */
        void execute(CompletionHandler completion_handler) override;

        /**
         Gets task execution result
         @return const reference to result struct
        */
        const BridgeDiscoveryIpCheckResult &get_result() const;

    private:
        void parse_config_response(support::HttpRequestError* error, support::IHttpResponse* response);

        std::string _ip;
        BridgeDiscoveryIpCheckResult _result;
    };

}  // namespace huesdk


