/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>

#include "bridgediscovery/BridgeDiscoveryClassType.h"
#include "bridgediscovery/IBridgeDiscoveryCallback.h"

#include "method/ipscan/tasks/BridgeDiscoveryIpscanTask.h"
#include "method/BridgeDiscoveryMethodBase.h"

namespace huesdk {

    class BridgeDiscoveryIpscan : public BridgeDiscoveryMethodBase<BridgeDiscoveryIpscanTask> {
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

