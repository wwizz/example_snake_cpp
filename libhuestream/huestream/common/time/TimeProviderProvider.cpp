/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/time/TimeProviderProvider.h>
#include <huestream/common/time/TimeManager.h>
#include <memory>

using huestream::TimeProviderPtr;

template<>
TimeProviderPtr huesdk_lib_default_provider<TimeProviderPtr>() {
    static TimeProviderPtr _default_time_manager = std::make_shared<huestream::TimeManager>();
    return _default_time_manager;
}