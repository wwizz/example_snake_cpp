/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <memory>
#include <vector>

#include "support/logging/Log.h"

#include "method/ipscan/tasks/BridgeDiscoveryIpscanTask.h"
#include "method/ipscan/BridgeDiscoveryIpscanPreCheck.h"

using Task = support::JobTask;
using support::OperationalQueue;
using support::NetworkInterface;
using support::QueueExecutor;
using std::shared_ptr;
using std::vector;
using std::string;

namespace {
    using huesdk::BridgeDiscoveryMethodUtil;

    vector<string> get_ips_to_check() {
        // Get the network interface for the ip address
        unique_ptr<NetworkInterface> network_interface = BridgeDiscoveryMethodUtil::get_first_private_network_interface();

        if (network_interface == nullptr) {
            HUE_LOG << HUE_CORE << HUE_ERROR << "BridgeDiscoveryIpscan: no valid network interface available"
                    << HUE_ENDL;
            return {};
        } else {
            HUE_LOG << HUE_CORE << HUE_DEBUG << "BridgeDiscoveryIpscan: network interface found -> name: "
                    << network_interface->get_name() + ", ip: " << network_interface->get_ip() << HUE_ENDL;
            return BridgeDiscoveryMethodUtil::list_ips_from_subnets({network_interface->get_ip()});
        }
    }
}  // namespace

namespace huesdk {
    BridgeDiscoveryIpscanTask::BridgeDiscoveryIpscanTask() : _executor(std::make_shared<OperationalQueue>()), _stopped_by_user(false) {}

    void BridgeDiscoveryIpscanTask::execute(Task::CompletionHandler done) {
        _executor.execute([this, done]() {
            auto filtered_ips = BridgeDiscoveryIpscanPreCheck::get_instance()->filter_reachable_ips(get_ips_to_check(), _stopped_by_user);
            auto check_ip_array_job = create_job<BridgeDiscoveryCheckIpArrayTask>(filtered_ips, [this](const BridgeDiscoveryIpCheckResult &result) {
                _results.emplace_back(std::make_shared<BridgeDiscoveryResult>(result.unique_id, result.ip, result.api_version, result.model_id));
            });
            check_ip_array_job->run([done](BridgeDiscoveryCheckIpArrayTask * /*task*/) {
                done();
            });
        });
    }

    const vector<std::shared_ptr<BridgeDiscoveryResult>> &BridgeDiscoveryIpscanTask::get_result() const {
        return _results;
    }

    void BridgeDiscoveryIpscanTask::stop() {
        _stopped_by_user = true;
    }

}  // namespace huesdk
