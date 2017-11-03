/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_NUPNP_BRIDGEDISCOVERYNUPNP_H_
#define BRIDGEDISCOVERY_METHOD_NUPNP_BRIDGEDISCOVERYNUPNP_H_

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "threading/Job.h"
#include "tasks/BridgeDiscoveryNupnpTask.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodBase.h"

using huesdk_lib::JobState;

namespace huesdk {
    class BridgeDiscoveryNupnp : public BridgeDiscoveryMethodBase<BridgeDiscoveryNupnpTask> {
    public:
        using BridgeDiscoveryMethodBase::MethodResultCallback;

        /**
        @see IBridgeDiscoveryMethod.h
        */
        BridgeDiscoveryClassType get_type() const override;

    protected:
        /**
        @see BridgeDiscoveryMethodBase.h
        */
        JobState method_search(const MethodResultCallback &callback) override;
    };

}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD_NUPNP_BRIDGEDISCOVERYNUPNP_H_
