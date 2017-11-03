/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKHUECONNECT_H
#define LIBHUESTREAM_MOCKHUECONNECT_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "huestream/connect/IConnectionFlow.h"

namespace huestream {


class MockConnect : public IConnect {

 public:
    FeedbackMessageCallback callback;
    MOCK_METHOD0(Load, void());
    MOCK_METHOD0(ConnectToBridge, void());
    MOCK_METHOD0(ConnectToBridgeBackground, void());
    MOCK_METHOD1(ConnectToBridgeWithIp, void(const std::string &ipAddress));
    MOCK_METHOD0(ConnectToNewBridge, void());
    MOCK_METHOD1(SetManual, void(BridgePtr bridge));
    MOCK_METHOD0(ResetBridge, void());
    MOCK_METHOD0(ResetAllData, void());
    MOCK_METHOD1(SelectGroup, void(std::string id));
    MOCK_METHOD1(StartStream, void(StreamPtr stream));
    MOCK_METHOD1(StopStream, void(StreamPtr stream));
    MOCK_METHOD0(Abort, void());
    MOCK_METHOD1(OnBridgeMonitorEvent, void(const huestream::FeedbackMessage &message));
    MOCK_METHOD1(NewMessage, void(const huestream::FeedbackMessage &message));

    MOCK_METHOD0(WaitUntilReady, void());
    MOCK_METHOD0(IsReady, bool());
    MOCK_METHOD0(GetResult, ConnectResult());
    MOCK_METHOD1(SetFeedbackMessageCallback, void(FeedbackMessageCallback callback));
};


}  // namespace huestream
#endif  // LIBHUESTREAM_MOCKHUECONNECT_H
