/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <type_traits>

#include "bridgediscovery/IBridgeDiscoveryCallback.h"
#include "bridgediscovery/BridgeDiscoveryReturnCode.h"
#include "support/threading/Job.h"
#include "support/threading/QueueDispatcher.h"
#include "hueutilities/EnumSet.h"

namespace huesdk {

    class IBridgeDiscoveryMethod;
    class BridgeDiscoveryTask;

    class BridgeDiscovery {
    public:
        enum class Option {
            UPNP   = 1,        ///< search for bridges via UPnP on the local network
            IPSCAN = 1 << 1,   ///< brute force scanning for bridges on the local network.
                               ///< Scans only the last subnet of the ip (IPV4 only).
                               ///< If multiple network interfaces are present it picks the first one in the list */
            NUPNP  = 1 << 2,   ///< search for bridges via the portal
        };

        enum class ReturnCode {
            SUCCESS                       =  0,    ///< search was successful
            BUSY                          = -5,    ///< a search is already in progress, it's not allowed to start multiple searches simultaneously
            NULL_PARAMETER                = -101,
            STOPPED                       = -303,  ///< search has been stopped
            MISSING_DISCOVERY_METHODS     = -401,  ///< this indicates no discovery methods could be found
        };

        using Callback = std::function<void(std::vector<std::shared_ptr<BridgeDiscoveryResult>>, ReturnCode)>;

        BridgeDiscovery();

        ~BridgeDiscovery();

        /** 
         Start searching for bridges. Since no options can be provided, only the UPnP and NUPnP discovery methods will
         be executed. The search will be performed in a different thread and also the callback will be called from that
         specific thread. This method is asynchronous and returns immediately
         @param  callback The callback which will be called when the search is finished and provides
                          all the found results. The callback will only be called once
         */
        void search(IBridgeDiscoveryCallback *callback);

        void search(Callback);

        /** 
         Start searching for bridges. Which of the available discovery methods should be executed, can be set
         with the options parameter. The search will be performed in a different thread and also the callback will
         be called from that specific thread. This method is asynchronous and returns immediately
         @param  options Which of the discovery methods should be executed.
         @param  callback The callback which will be called when the search is finished and provides
                          all the found results. The callback will only be called once
         */
        void search(EnumSet<Option> options, IBridgeDiscoveryCallback *callback);

        void search(EnumSet<Option> options, Callback);

        /**
         Whether a search is in progress
         */
        bool is_searching();

        /**
         Stop searching if still in progress. This method will block until the search is completely stopped.
         It's also guaranteed, if a search was still in progress, that the callback will be called with the
         found results.
         */
        void stop();

    private:
        std::vector<std::unique_ptr<IBridgeDiscoveryMethod>> get_discovery_methods(EnumSet<Option> options);

        std::recursive_mutex _mutex;
        std::shared_ptr<support::Job<BridgeDiscoveryTask>> _discovery_job;
        Callback _callback;
        support::QueueDispatcher _dispatcher;
    };

    inline EnumSet<BridgeDiscovery::Option> operator|(BridgeDiscovery::Option lhs, BridgeDiscovery::Option rhs) {
        return {lhs, rhs};
    }

}  // namespace huesdk

