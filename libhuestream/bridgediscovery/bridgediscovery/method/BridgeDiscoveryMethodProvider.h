/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODPROVIDER_H_
#define BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODPROVIDER_H_

#include <memory>
#include "bridgediscovery/method/IBridgeDiscoveryMethod.h"

namespace huesdk {

    enum BridgeDiscoveryOption;

    class BridgeDiscoveryMethodProvider {
    public:
        /**
         Get instance of a discovery method based on a enumeration value
         @note   The caller becomes the owner of the discovery method pointer and is therefore
                 responsible for cleaning up the memory
         @param  option The discovery method instance which should be created
         @return Discovery method instance when valid option provided, nullptr otherwise
         */
        static std::unique_ptr<IBridgeDiscoveryMethod> get_discovery_method(BridgeDiscoveryOption option);

    private:
        /**
         Constructor declared private, so no instance of this class can be created   
         */
        BridgeDiscoveryMethodProvider();
    };

}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD_BRIDGEDISCOVERYMETHODPROVIDER_H_
