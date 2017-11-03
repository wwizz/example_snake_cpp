/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_HTTPMONITORSUBSCRIPTION_H_
#define NETWORK_HTTP_HTTPMONITORSUBSCRIPTION_H_

#include "network/http/HttpMonitorObserver.h"

namespace huesdk_lib {

    class HttpMonitorSubscription {
    public:
        /**
         Class constructor. Registers observer as HttpMonitor listener
         @param observer observer that has to be registered.
         */
        explicit HttpMonitorSubscription(const HttpMonitorObserver& observer);

        /**
         Class destructor. Un-registers observer
         */
        ~HttpMonitorSubscription();

    private:
        /** Reference to the registered observer */
        const HttpMonitorObserver& _observer;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_HTTPMONITORSUBSCRIPTION_H_
