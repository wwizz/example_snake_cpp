#ifndef LIBHUESTREAM_MOCKBRIDGESTORAGEACCESSER_H
#define LIBHUESTREAM_MOCKBRIDGESTORAGEACCESSER_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <functional>
#include <huestream/connect/IBridgeStorageAccessor.h>

using namespace testing;
using namespace huestream;


class MockBridgeStorageAccesser : public IBridgeStorageAccessor {
public:
    BridgesLoadCallbackHandler load_callback;
    BridgesSaveCallbackHandler save_callback;

    MOCK_METHOD1(Load, void (BridgesLoadCallbackHandler(cb)));
    MOCK_METHOD2(Save, void(HueStreamDataPtr
            b, BridgesSaveCallbackHandler(cb)));
};

#endif //LIBHUESTREAM_MOCKBRIDGESTORAGEACCESSER_H
