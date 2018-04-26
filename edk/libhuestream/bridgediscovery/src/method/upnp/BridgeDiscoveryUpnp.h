/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <memory>
#include <string>

#include "support/threading/Job.h"

#include "method/upnp/tasks/BridgeDiscoveryUpnpTask.h"
#include "method/BridgeDiscoveryMethodBase.h"

namespace huesdk {

    class BridgeDiscoveryUpnp : public BridgeDiscoveryMethodBase<BridgeDiscoveryUpnpTask> {
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

