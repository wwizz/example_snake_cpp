/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>

#include "support/logging/Log.h"

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/BridgeDiscoveryConst.h"

#include "method/BridgeDiscoveryMethodUtil.h"
#include "method/upnp/BridgeDiscoveryUpnp.h"

using support::JobState;

namespace huesdk {
    bool BridgeDiscoveryUpnp::method_search(const BridgeDiscoveryMethodBase::MethodResultCallback &callback) {
        _job = support::create_job<TaskType>();

        return _job->run([callback](TaskType *task) {
            callback(task->get_result());
        });
    }

    BridgeDiscoveryClassType BridgeDiscoveryUpnp::get_type() const {
        return BRIDGE_DISCOVERY_CLASS_TYPE_UPNP;
    }
}  // namespace huesdk
