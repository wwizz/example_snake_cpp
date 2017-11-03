/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYCHECKIPTASK_H_
#define BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYCHECKIPTASK_H_

#include <memory>
#include <thread>
#include <string>

#include "threading/Job.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "network/http/HttpRequestError.h"
#include "network/http/HttpResponse.h"

using Task = huesdk_lib::JobTask;
using huesdk_lib::HttpRequestError;
using huesdk_lib::IHttpResponse;

namespace huesdk {

    class BridgeDiscoveryCheckIpTask : public Task {
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
        /**
         Parses the configuration response from bridge. Modifies _result
         @param error pointer to http error object
         @param response pointer to http response object
         */
        void parse_config_response(HttpRequestError* error, IHttpResponse* response);

        std::string _ip;
        BridgeDiscoveryIpCheckResult _result;
    };

}  // namespace huesdk


#endif  // BRIDGEDISCOVERY_TASKS_BRIDGEDISCOVERYCHECKIPTASK_H_
