/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKENGINE_H_
#define LIBHUESTREAM_TEST_MOCKENGINE_H_

#include <huestream/effect/IMixer.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace huestream {

    class MockMixer : public IMixer {
    public:

        MOCK_METHOD1(AddEffect, void(EffectPtr effect));
        MOCK_METHOD1(AddEffectList, void(EffectListPtr effects));
        MOCK_METHOD0(GetGroup, GroupPtr());
        MOCK_METHOD0(Render, void());
        MOCK_METHOD1(SetGroup, void(const GroupPtr group));
        MOCK_METHOD1(GetEffectByName, EffectPtr(std::string name));
        MOCK_METHOD0(Lock, void());
        MOCK_METHOD0(Unlock, void());
    };

}// namespace huestream
#endif     // LIBHUESTREAM_TEST_MOCKENGINE_H_
