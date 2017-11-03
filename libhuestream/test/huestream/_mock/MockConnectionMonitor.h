/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKCONNECTIONMONITOR_H_
#define LIBHUESTREAM_TEST_MOCKCONNECTIONMONITOR_H_

#include "gmock/gmock.h"
#include "huestream/connect/IConnectionMonitor.h"

namespace huestream {

    class MockConnectionMonitor : public IConnectionMonitor {
    public:
        FeedbackMessageCallback callback;

        MockConnectionMonitor() {}
        MOCK_METHOD2(Start, void(BridgePtr bridge, int interval_msec));
        MOCK_METHOD0(Stop, void());
        MOCK_METHOD1(SetFeedbackMessageCallback, void(std::function<void(const huestream::FeedbackMessage &)> callback));
    };

}// namespace huestream
#endif     // LIBHUESTREAM_TEST_MOCKCONNECTIONMONITOR_H_
