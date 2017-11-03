/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_TIME_TIMEPROVIDERPROVIDER_H_
#define HUESTREAM_COMMON_TIME_TIMEPROVIDERPROVIDER_H_

#include <util/Provider.h>
#include <huestream/common/time/ITimeProvider.h>

namespace huestream {
    typedef huesdk_lib::Provider<TimeProviderPtr> TimeProviderProvider;
}

#endif  // HUESTREAM_COMMON_TIME_TIMEPROVIDERPROVIDER_H_
