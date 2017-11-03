/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef BRIDGEDISCOVERY_BRIDGEDISCOVERY_H_
#define BRIDGEDISCOVERY_BRIDGEDISCOVERY_H_

#include <vector>
#include <memory>

#include "bridgediscovery/IBridgeDiscoveryCallback.h"
#include "bridgediscovery/BridgeDiscoveryReturnCode.h"
#include "tasks/BridgeDiscoveryTask.h"

namespace huesdk {

    enum BridgeDiscoveryOption {
        DISCOVERY_OPTION_UPNP   = 1,
        DISCOVERY_OPTION_IPSCAN = 1 << 1,
        DISCOVERY_OPTION_NUPNP  = 1 << 2,
    };

    class IBridgeDiscoveryMethod;

    class BridgeDiscovery {
    public:
        /**
         Destructor
         */
        ~BridgeDiscovery();

        /** 
         Start searching for bridges. Since no options can be provided, only the UPnP and NUPnP discovery methods will
         be executed. The search will be performed in a different thread and also the callback will be called from that
         specific thread. This method is asynchronous and returns immediately with a result code
         @note   The caller becomes the owner of the results provided by the callback and is therefore
                 responsible for cleaning up the memory
         @param  callback The callback which will be called when the search is finished and provides
                          all the found results. The callback will only be called once
         The result code dispatched through the callback represents one of the following values:
                 - BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS:          search has been started
                 - BRIDGE_DISCOVERY_RETURN_CODE_ALREADY_IN_PROGRESS: another search is already in progress, it's not allowed to
                                                 start multiple searches simultaneously
                 - BRIDGE_DISCOVERY_RETURN_CODE_MISSING_DISCOVERY_METHODS:  this indicates no discovery methods could be found, however
                                                 since no methods are provided to this method this should
                                                 not never happen
                 - BRIDGE_DISCOVERY_RETURN_CODE_MISSING_CALLBACK: no callback is provided, nullptr or NULL is not allowed
         */
        void search(IBridgeDiscoveryCallback *callback);

        /** 
         Start searching for bridges. Which of the available discovery methods should be executed, can be set
         with the options parameter. The search will be performed in a different thread and also the callback will
         be called from that specific thread. This method is asynchronous and returns immediately with a result code
         @note   The caller becomes the owner of the results provided by the callback and is therefore
                 responsible for cleaning up the memory
         @param  options Which of the discovery methods should be executed. Available options:
                        - DISCOVERY_OPTION_UPNP:   search for bridges via UPnP on the local network
                        - DISCOVERY_OPTION_IPSCAN: brute force scanning for bridges on the local network.
                                                   Scans only the last subnet of the ip (IPV4 only). If multiple network interfaces
                                                   are present it picks the first one in the list
                        - DISCOVERY_OPTION_NUPNP:  search for bridges via the portal
                        
                        e.g.: int options = DISCOVERY_OPTION_UPNP | DISCOVERY_OPTION_IPSCAN | DISCOVERY_OPTION_NUPNP;
         @param  callback The callback which will be called when the search is finished and provides
                          all the found results. The callback will only be called once
         The result code dispatched through the callback represents one of the following values:
                 - BRIDGE_DISCOVERY_RETURN_CODE_SUCCESS:          search has been started
                 - BRIDGE_DISCOVERY_RETURN_CODE_ALREADY_IN_PROGRESS: another search is already in progress, it's not allowed to
                                                 start multiple searches simultaneously
                 - BRIDGE_DISCOVERY_RETURN_CODE_MISSING_DISCOVERY_METHODS:  this indicates no discovery methods could be found, however
                                                 since no methods are provided to this method this should
                                                 not never happen
                 - BRIDGE_DISCOVERY_RETURN_CODE_MISSING_CALLBACK: no callback is provided, nullptr or NULL is not allowed
         */
        void search(int options, IBridgeDiscoveryCallback *callback);

        /**
         Whether a search is still in progress
         @return true when searching, false otherwise
         */
        bool is_searching();

        /**
         Stop searching if still in progress. This method will block until the search is completely stopped.
         It's also guaranteed, if a search was still in progress, that the callback will be called with the
         found results.
         */
        void stop();

    private:
        /**
         Get discovery methods
         @param  options Mask of the discovery methods which should be retrieved
                         e.g.: int options = DISCOVERY_OPTION_UPNP | DISCOVERY_OPTION_IPSCAN
         @return The requested discovery method instances
         */
        std::vector<std::unique_ptr<IBridgeDiscoveryMethod>> get_discovery_methods(int options);

        std::shared_ptr<huesdk_lib::Job<BridgeDiscoveryTask>> _discovery_job;
        IBridgeDiscoveryCallback *_callback;
    };
}  // namespace huesdk

#endif  // BRIDGEDISCOVERY_BRIDGEDISCOVERY_H_
