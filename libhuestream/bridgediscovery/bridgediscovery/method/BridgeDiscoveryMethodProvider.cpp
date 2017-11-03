/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "bridgediscovery/BridgeDiscovery.h"
#include "bridgediscovery/method/BridgeDiscoveryMethodProvider.h"
#include "bridgediscovery/method/ipscan/BridgeDiscoveryIpscan.h"
#include "bridgediscovery/method/nupnp/BridgeDiscoveryNupnp.h"
#include "bridgediscovery/method/upnp/BridgeDiscoveryUpnp.h"

using std::unique_ptr;

namespace huesdk {

    unique_ptr<IBridgeDiscoveryMethod> BridgeDiscoveryMethodProvider::get_discovery_method(BridgeDiscoveryOption option) {
        switch (option) {
            case DISCOVERY_OPTION_UPNP:
                return unique_ptr<BridgeDiscoveryUpnp>(new BridgeDiscoveryUpnp());
            case DISCOVERY_OPTION_NUPNP:
                return unique_ptr<BridgeDiscoveryNupnp>(new BridgeDiscoveryNupnp());
            case DISCOVERY_OPTION_IPSCAN:
                return unique_ptr<BridgeDiscoveryIpscan>(new BridgeDiscoveryIpscan());
        }
        
        return nullptr;
    }

}  // namespace huesdk
