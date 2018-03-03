/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "support/network/http/HttpMonitorSubscription.h"
#include "support/network/http/HttpMonitor.h"

using support::HttpMonitorSubscription;
using support::HttpMonitorObserver;
using support::HttpMonitor;

HttpMonitorSubscription::HttpMonitorSubscription(const HttpMonitorObserver& observer)
        : _observer(observer) {
    HttpMonitor::add_observer(const_cast<HttpMonitorObserver*>(&_observer));
}

HttpMonitorSubscription::~HttpMonitorSubscription() {
    HttpMonitor::remove_observer(const_cast<HttpMonitorObserver*>(&_observer));
}
