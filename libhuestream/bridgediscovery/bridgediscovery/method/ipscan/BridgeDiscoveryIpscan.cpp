/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "logging/Log.h"
#include "network/NetworkInterface.h"

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/BridgeDiscoveryClassType.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "bridgediscovery/method/ipscan/BridgeDiscoveryIpscanPreCheck.h"
#include "bridgediscovery/method/ipscan/BridgeDiscoveryIpscan.h"

using std::future;
using huesdk_lib::Vector;
using huesdk_lib::JobState;

namespace huesdk {
    JobState BridgeDiscoveryIpscan::method_search(const MethodResultCallback &callback) {
        _job = huesdk_lib::create_job<TaskType>();
        return _job->run([callback](TaskType *task) {
            callback(task->get_result());
        });
    }

    BridgeDiscoveryClassType BridgeDiscoveryIpscan::get_type() const {
        return BRIDGE_DISCOVERY_CLASS_TYPE_IPSCAN;
    }
}  // namespace huesdk
