/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "bridgediscovery/BridgeDiscovery.h"

#include "method/IBridgeDiscoveryMethod.h"
#include "method/ipscan/BridgeDiscoveryIpscan.h"
#include "method/nupnp/BridgeDiscoveryNupnp.h"
#include "method/upnp/BridgeDiscoveryUpnp.h"
#include "BridgeDiscoveryMethodFactory.h"

using std::unique_ptr;

template<>
std::unique_ptr<huesdk::IBridgeDiscoveryMethod>
huesdk_lib_default_factory<huesdk::IBridgeDiscoveryMethod, huesdk::BridgeDiscovery::Option>(
        huesdk::BridgeDiscovery::Option option) {
    switch (option) {
        case huesdk::BridgeDiscovery::Option::UPNP:
            return std::unique_ptr<huesdk::BridgeDiscoveryUpnp>(new huesdk::BridgeDiscoveryUpnp());
        case huesdk::BridgeDiscovery::Option::NUPNP:
            return std::unique_ptr<huesdk::BridgeDiscoveryNupnp>(new huesdk::BridgeDiscoveryNupnp());
        case huesdk::BridgeDiscovery::Option::IPSCAN:
            return std::unique_ptr<huesdk::BridgeDiscoveryIpscan>(new huesdk::BridgeDiscoveryIpscan());
    }
    return nullptr;
}