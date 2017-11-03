/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include "network/http/HttpMonitorSubscription.h"
#include "network/http/HttpMonitor.h"

using huesdk_lib::HttpMonitorSubscription;
using huesdk_lib::HttpMonitorObserver;
using huesdk_lib::HttpMonitor;

HttpMonitorSubscription::HttpMonitorSubscription(const HttpMonitorObserver& observer)
        : _observer(observer) {
    HttpMonitor::add_observer(const_cast<HttpMonitorObserver*>(&_observer));
}

HttpMonitorSubscription::~HttpMonitorSubscription() {
    HttpMonitor::remove_observer(const_cast<HttpMonitorObserver*>(&_observer));
}
