/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_BRIDGEDISCOVERYRETURNCODE_H_
#define BRIDGEDISCOVERY_BRIDGEDISCOVERYRETURNCODE_H_

namespace huesdk {
    
    typedef enum BridgeDiscoveryReturnCode {
        BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS                       =  0,
        BRIDGE_DISCOVERY_RETURN_CODE_BUSY                          = -5,
        BRIDGE_DISCOVERY_RETURN_CODE_NULL_PARAMETER                = -101,
        BRIDGE_DISCOVERY_RETURN_CODE_STOPPED                       = -303,
        BRIDGE_DISCOVERY_RETURN_CODE_MISSING_DISCOVERY_METHODS     = -401,
        BRIDGE_DISCOVERY_RETURN_CODE_MISSING_CALLBACK              = -402,
    } BridgeDiscoveryReturnCode;
    
}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_BRIDGEDISCOVERYRETURNCODE_H_
