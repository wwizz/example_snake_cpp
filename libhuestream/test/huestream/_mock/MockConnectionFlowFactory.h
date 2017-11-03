//
// Created by 310236882 on 6/28/2016.
//

#ifndef LIBHUESTREAM_MOCKCONNECTIONFLOWFACTORY_H
#define LIBHUESTREAM_MOCKCONNECTIONFLOWFACTORY_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <functional>
#include <huestream/connect/ConnectionFlow.h>

using namespace testing;
using namespace huestream;


class MockConnectionFlowFactory : public IConnectionFlowFactory {
public:
    MOCK_METHOD0(CreateSearcher, BridgeSearcherPtr());

    MOCK_METHOD0(CreateAuthenticator, BridgeAuthenticatorPtr());

    MOCK_METHOD1(CreateFullConfigRetriever, FullConfigRetrieverPtr(bool useForcedActivation));

    MOCK_METHOD0(GetMessageDispatcher, MessageDispatcherPtr());

    MOCK_METHOD0(GetStorageAccesser, BridgeStorageAccessorPtr());
};

#endif //LIBHUESTREAM_MOCKCONNECTIONFLOWFACTORY_H
