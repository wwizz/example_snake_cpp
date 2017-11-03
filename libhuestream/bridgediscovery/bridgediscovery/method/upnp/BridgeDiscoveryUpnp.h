/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_UPNP_BRIDGEDISCOVERYUPNP_H_
#define BRIDGEDISCOVERY_METHOD_UPNP_BRIDGEDISCOVERYUPNP_H_

#include <memory>
#include <string>

#include "threading/Job.h"
#include "tasks/BridgeDiscoveryUpnpTask.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodBase.h"

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
        huesdk_lib::JobState method_search(const MethodResultCallback &callback) override;
    };

}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD_UPNP_BRIDGEDISCOVERYUPNP_H_
