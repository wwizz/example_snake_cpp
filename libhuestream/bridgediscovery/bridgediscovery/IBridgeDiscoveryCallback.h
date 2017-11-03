/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_IBRIDGEDISCOVERYCALLBACK_H_
#define BRIDGEDISCOVERY_IBRIDGEDISCOVERYCALLBACK_H_

#include <functional>
#include <utility>
#include <vector>

#include "bridgediscovery/BridgeDiscoveryResult.h"
#include "bridgediscovery/BridgeDiscoveryReturnCode.h"

namespace huesdk {

    class IBridgeDiscoveryCallback {
    public:
        /**
         virtual destructor by default
         */
        virtual ~IBridgeDiscoveryCallback() = default;

        /**
         Functor which handles the callback. Since a functor allows you to invoke an object
         as if it were a function, this class provides a generic and flexible way to handle
         callbacks
         @param results     The results which were found during the discovery
         @param return_code Will return BRIDGE_DISCOVERY_RETURN_CODE_STOPPED when the search is stopped by the user, and BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS otherwise
         */
        virtual void operator()(const std::vector<BridgeDiscoveryResult> &results, BridgeDiscoveryReturnCode return_code) = 0;
    };

    /**
     Class for using lambdas to handle the callback
     */
    class BridgeDiscoveryCallback : public IBridgeDiscoveryCallback {
    public:
        // Definition of the lambda function
        typedef std::function<void (const std::vector<BridgeDiscoveryResult> &results, BridgeDiscoveryReturnCode return_code)> CallbackFunction;
    
        /**
         Constructor
         @param function The lambda function which should be called to handle the callback
         */
        explicit BridgeDiscoveryCallback(const CallbackFunction& function) : _function(function) { }
    
        /**
         @see IBridgeDiscoveryCallback::operator()(...)
         */
        void operator()(const std::vector<BridgeDiscoveryResult> &results, BridgeDiscoveryReturnCode return_code) {
            // Call the lambda method
            _function(results, return_code);
        }

    private:
        /** the lambda function */
        CallbackFunction _function;
    };
    
}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_IBRIDGEDISCOVERYCALLBACK_H_
