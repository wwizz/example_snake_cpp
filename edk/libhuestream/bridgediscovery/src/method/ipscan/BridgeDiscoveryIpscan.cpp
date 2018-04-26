/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "support/logging/Log.h"
#include "support/network/NetworkInterface.h"

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/BridgeDiscoveryClassType.h"

#include "method/BridgeDiscoveryMethodUtil.h"
#include "method/ipscan/BridgeDiscoveryIpscanPreCheck.h"
#include "method/ipscan/BridgeDiscoveryIpscan.h"

using std::future;
using support::JobState;

namespace huesdk {

    bool BridgeDiscoveryIpscan::method_search(const MethodResultCallback &callback) {
        _job = support::create_job<TaskType>();

        return _job->run([callback](TaskType *task) {
            callback(task->get_result());
        });
    }

    BridgeDiscoveryClassType BridgeDiscoveryIpscan::get_type() const {
        return BRIDGE_DISCOVERY_CLASS_TYPE_IPSCAN;
    }

}  // namespace huesdk
