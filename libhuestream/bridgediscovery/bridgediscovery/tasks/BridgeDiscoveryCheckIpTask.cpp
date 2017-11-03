/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "bridgediscovery/tasks/BridgeDiscoveryCheckIpTask.h"

#include <thread>
#include "network/http/HttpRequestTask.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/BridgeDiscoveryConfiguration.h"

using Task = huesdk_lib::JobTask;

using huesdk_lib::HttpRequestTask;
using std::thread;
using std::string;

namespace {
    using huesdk::BridgeDiscoveryConfiguration;

    const HttpRequestTask::Options *http_options_ipcheck() {
        static HttpRequestTask::Options options;

        options.connect_timeout = huesdk::bridge_discovery_const::IPCHECK_HTTP_CONNECT_TIMEOUT;
        options.receive_timeout = huesdk::bridge_discovery_const::IPCHECK_HTTP_RECEIVE_TIMEOUT;
        options.request_timeout = huesdk::bridge_discovery_const::IPCHECK_HTTP_REQUEST_TIMEOUT;
        options.use_proxy = BridgeDiscoveryConfiguration::has_proxy_settings();
        options.proxy_address = BridgeDiscoveryConfiguration::get_proxy_address();
        options.proxy_port = static_cast<int>(BridgeDiscoveryConfiguration::get_proxy_port());

        return &options;
    }
}  // namespace

namespace huesdk {
    BridgeDiscoveryCheckIpTask::BridgeDiscoveryCheckIpTask(const string &ip) : _ip(ip) {}

    void BridgeDiscoveryCheckIpTask::execute(Task::CompletionHandler done) {
        const auto bridge_config_url = "http://" + _ip + bridge_discovery_const::IPCHECK_BRIDGE_CONFIG_HTTP_URL_PATH;

        create_job<HttpRequestTask>(bridge_config_url, http_options_ipcheck())->run([this, done](HttpRequestTask *task) {
            parse_config_response(task->get_error(), task->get_response());
            done();
        });
    }

    const BridgeDiscoveryIpCheckResult &BridgeDiscoveryCheckIpTask::get_result() const {
        return _result;
    }

    void BridgeDiscoveryCheckIpTask::parse_config_response(HttpRequestError *error, IHttpResponse *response) {
        if (error != nullptr && response != nullptr && error->get_code() != HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS) {
            _result.ip = _ip;
            _result.reachable = false;
            _result.is_bridge = false;
        } else if (response != nullptr) {
            _result = BridgeDiscoveryMethodUtil::parse_bridge_config_result(_ip, response->get_body());
        }
    }
}  // namespace huesdk
