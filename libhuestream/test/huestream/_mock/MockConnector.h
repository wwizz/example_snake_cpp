/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKCONNECTOR_H_
#define LIBHUESTREAM_TEST_MOCKCONNECTOR_H_

#include "gmock/gmock.h"
#include "huestream/stream/IConnector.h"

namespace huestream {

    class MockConnector : public IConnector {
    public:
        MockConnector() {}

        MOCK_METHOD2(Connect, bool(BridgePtr bridge, unsigned short port));
        MOCK_METHOD0(Disconnect, void());
        MOCK_METHOD2(Send, bool(const char *buffer, unsigned int bufferSize));
    };

}// namespace huestream
#endif     // LIBHUESTREAM_TEST_MOCKCONNECTOR_H_
