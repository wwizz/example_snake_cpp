/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "bridgediscovery/method/nupnp/BridgeDiscoveryNupnp.h"

#include "tasks/BridgeDiscoveryNupnpTask.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/BridgeDiscoveryConfiguration.h"
#include "logging/Log.h"

using huesdk_lib::JobState;

namespace huesdk {
    JobState BridgeDiscoveryNupnp::method_search(const MethodResultCallback &callback) {
        _job = huesdk_lib::create_job<TaskType>(
                BridgeDiscoveryConfiguration::get_bridge_discovery_url() + bridge_discovery_const::NUPNP_HTTP_URL_PATH);
        return _job->run([callback](TaskType *task) {
            callback(task->get_result());
        });
    }

    BridgeDiscoveryClassType BridgeDiscoveryNupnp::get_type() const {
        return BridgeDiscoveryClassType::BRIDGE_DISCOVERY_CLASS_TYPE_NUPNP;
    }
}  // namespace huesdk