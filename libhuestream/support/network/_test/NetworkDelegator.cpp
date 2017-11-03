/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#include <mutex>
#include <vector>

#include "network/_test/NetworkDelegator.h"

using std::mutex;
using std::unique_lock;

namespace huesdk_lib {

    /* delegate provider */
    
    static shared_ptr<NetworkDelegate> _delegate = shared_ptr<NetworkDelegate>(new NetworkDelegateImpl());
    static mutex _delegate_mutex;

    void NetworkDelegator::set_delegate(shared_ptr<NetworkDelegate> delegate) {
        unique_lock<mutex> delegate_lock(_delegate_mutex);
        _delegate = delegate;
    }
    
    
    /* delegator */

    const std::vector<NetworkInterface> NetworkDelegator::get_network_interfaces() {
        unique_lock<mutex> delegate_lock(_delegate_mutex);
        return _delegate->get_network_interfaces();
    }
    
}  // namespace huesdk_lib

#endif
