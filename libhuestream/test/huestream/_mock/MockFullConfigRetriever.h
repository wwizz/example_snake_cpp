/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKFULLCONFIGRETRIEVER_H
#define LIBHUESTREAM_MOCKFULLCONFIGRETRIEVER_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <functional>
#include <huestream/connect/ConnectionFlow.h>

using namespace testing;
using namespace huestream;



class MockFullConfigRetriever : public IFullConfigRetriever {

public:
    BridgePtr Bridge;
    RetrieveCallbackHandler RetrieveCallback;
    void ExecuteRetrieveCallback(OperationResult result, BridgePtr bridge) {
        if (RetrieveCallback == nullptr)
            FAIL();
        RetrieveCallback(result, bridge);
    }
    MOCK_METHOD2(Execute, bool(BridgePtr bridge, RetrieveCallbackHandler cb));

};

#endif //LIBHUESTREAM_MOCKFULLCONFIGRETRIEVER_H
