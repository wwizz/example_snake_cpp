/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKTIMEMANAGER_H_
#define LIBHUESTREAM_TEST_MOCKTIMEMANAGER_H_

#include <huestream/common/time/ITimeManager.h>
#include "gmock/gmock.h"

namespace huestream {

    class MockTimeManager : public ITimeManager {
    public:
        MOCK_METHOD1(Sleep, void(int64_t duration));
        MOCK_METHOD0(Now, int64_t());
        MOCK_METHOD0(UpdateTime, void());
    };
}

#endif   // LIBHUESTREAM_TEST_MOCKTIMEMANAGER_H_