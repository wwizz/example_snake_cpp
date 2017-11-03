/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>

#include "logging/Log.h"

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodUtil.h"
#include "bridgediscovery/method/upnp/BridgeDiscoveryUpnp.h"

using huesdk_lib::JobState;

namespace huesdk {
    JobState BridgeDiscoveryUpnp::method_search(const BridgeDiscoveryMethodBase::MethodResultCallback &callback) {
        _job = huesdk_lib::create_job<TaskType>();

        return _job->run([callback](TaskType *task) {
            callback(task->get_result());
        });
    }

    BridgeDiscoveryClassType BridgeDiscoveryUpnp::get_type() const {
        return BRIDGE_DISCOVERY_CLASS_TYPE_UPNP;
    }
}  // namespace huesdk
