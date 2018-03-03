/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <mutex>
#include <vector>

#include "support/network/_test/NetworkDelegator.h"

using std::mutex;
using std::unique_lock;

namespace support {

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
    
}  // namespace support
