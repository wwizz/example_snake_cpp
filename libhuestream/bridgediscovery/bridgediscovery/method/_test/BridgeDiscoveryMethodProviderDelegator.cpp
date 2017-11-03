/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#include "bridgediscovery/method/_test/BridgeDiscoveryMethodProviderDelegator.h"

#include <memory>
#include <mutex>

using std::mutex;
using std::unique_lock;
using std::unique_ptr;

namespace huesdk {

    /* provider */
    
    static shared_ptr<BridgeDiscoveryMethodProviderDelegate> _delegate = shared_ptr<BridgeDiscoveryMethodProviderDelegate>(new BridgeDiscoveryMethodProviderDelegateImpl());
    static mutex _delegate_mutex;

    void BridgeDiscoveryMethodProviderDelegator::set_delegate(shared_ptr<BridgeDiscoveryMethodProviderDelegate> delegate) {
        unique_lock<mutex> _delegate_lock(_delegate_mutex);
        _delegate = delegate;
    }
    
    
    /* delegate */

    unique_ptr<IBridgeDiscoveryMethod> BridgeDiscoveryMethodProviderDelegator::get_discovery_method(BridgeDiscoveryOption option) {
        unique_lock<mutex> _delegate_lock(_delegate_mutex);
        return _delegate->get_discovery_method(option);
    }

}  // namespace huesdk

#endif
