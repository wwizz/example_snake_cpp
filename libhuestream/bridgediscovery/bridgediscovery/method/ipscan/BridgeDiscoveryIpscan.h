/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_IPSCAN_BRIDGEDISCOVERYIPSCAN_H_
#define BRIDGEDISCOVERY_METHOD_IPSCAN_BRIDGEDISCOVERYIPSCAN_H_

#include <string>

#include "tasks/BridgeDiscoveryIpscanTask.h"
#include "bridgediscovery/BridgeDiscoveryClassType.h"
#include "bridgediscovery/IBridgeDiscoveryCallback.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodBase.h"

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
        huesdk_lib::JobState method_search(const MethodResultCallback &callback) override;
    };

}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD_IPSCAN_BRIDGEDISCOVERYIPSCAN_H_
