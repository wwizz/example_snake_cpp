/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKSTREAM_H
#define LIBHUESTREAM_MOCKSTREAM_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "huestream/stream/IStream.h"

MATCHER_P(MatchBridgeId, bridge, "Request did not match") { return arg->GetId() == bridge->GetId(); };

namespace huestream {
    class MockStream : public IStream {
        StreamRenderCallback _callback;

    public:
        void ExecuteRenderCallback(){_callback();};
        void SetRenderCallback(StreamRenderCallback callback) { _callback = callback;};

        MOCK_METHOD1(Start, bool(BridgePtr bridge));

        MOCK_METHOD1(StartWithRenderThread, bool(BridgePtr bridge));

        MOCK_METHOD0(Stop, void());

        MOCK_METHOD1(Stop, void(BridgePtr bridge));

        MOCK_CONST_METHOD0(IsStreaming, bool());

        MOCK_METHOD0(RenderSingleFrame, void());

        MOCK_CONST_METHOD0(GetStreamCounter, int32_t());

        MOCK_METHOD1(UpdateBridgeGroup, void(BridgePtr bridge));
    };
}

#endif  // LIBHUESTREAM_MOCKSTREAM_H
