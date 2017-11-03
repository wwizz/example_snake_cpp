/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#ifndef BRIDGEDISCOVERY_METHOD__TEST_BRIDGEDISCOVERYMETHODPROVIDERDELEGATOR_H_
#define BRIDGEDISCOVERY_METHOD__TEST_BRIDGEDISCOVERYMETHODPROVIDERDELEGATOR_H_

#include <memory>

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodProvider.h"
#include "bridgediscovery/method/IBridgeDiscoveryMethod.h"

using std::shared_ptr;

namespace huesdk {

    class BridgeDiscoveryMethodProviderDelegate {
    public:
        /** 
         @see bridgediscovery/method/IBridgeDiscoveryMethod.h
         */
        virtual std::unique_ptr<IBridgeDiscoveryMethod> get_discovery_method(BridgeDiscoveryOption option) = 0;
    };
    
    /**
     The default delegate uses the normal implementation for getting the bridge
     discovery methods
     */
    class BridgeDiscoveryMethodProviderDelegateImpl : public BridgeDiscoveryMethodProviderDelegate {
    public:
        /**
         @see bridgediscovery/method/IBridgeDiscoveryMethod.h
         */
        std::unique_ptr<IBridgeDiscoveryMethod> get_discovery_method(BridgeDiscoveryOption option) override {
            return BridgeDiscoveryMethodProvider::get_discovery_method(option);
        }
    };

    class BridgeDiscoveryMethodProviderDelegator : public BridgeDiscoveryMethodProvider {
    public:
        /**
         @see bridgediscovery/method/IBridgeDiscoveryMethod.h
         */
        static std::unique_ptr<IBridgeDiscoveryMethod> get_discovery_method(BridgeDiscoveryOption option);
        
        
        /* delegate */
        
        /**
         Set the delegate
         @note   Initially BridgeDiscoveryMethodProviderDelegateImpl is set as delegate
         @return The delegate, nullptr if no delegate has been set
         */
        static void set_delegate(shared_ptr<BridgeDiscoveryMethodProviderDelegate> delegate);
        
    private:
        /**
         Constructor declared private, so no instance of this class can be created
         */
        BridgeDiscoveryMethodProviderDelegator();
    };

}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_METHOD__TEST_BRIDGEDISCOVERYMETHODPROVIDERDELEGATOR_H_

#endif  // SDK_TEST  // NOLINT
