/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "support/threading/Job.h"

#include "method/nupnp/tasks/BridgeDiscoveryNupnpTask.h"
#include "method/BridgeDiscoveryMethodBase.h"

using support::JobState;

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
        bool method_search(const MethodResultCallback &callback) override;
    };

}  // namespace huesdk

