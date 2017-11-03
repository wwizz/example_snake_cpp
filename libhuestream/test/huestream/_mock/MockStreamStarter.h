/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKSTREAMSTARTER_H
#define LIBHUESTREAM_MOCKSTREAMSTARTER_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "huestream/stream/IStreamStarter.h"

namespace huestream {
    class MockStreamStarter : public IStreamStarter {
    public:
        BridgePtr bridge;

        MOCK_METHOD1(Start, bool(bool force));
        MOCK_METHOD0(Stop, void());

        bool ActivateSuccess(bool force) {
            bridge->GetGroup()->SetActive(true);
            bridge->GetGroup()->SetOwner(bridge->GetUser());
            return true;
        }
        void DeactivateSuccess() {
            bridge->GetGroup()->SetActive(false);
        }
    };
}

#endif  // LIBHUESTREAM_MOCKSTREAMSTARTER_H
