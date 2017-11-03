/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef LIBHUESTREAM_TEST_MOCKTMESSAGETRANSLATOR_H_
#define LIBHUESTREAM_TEST_MOCKTMESSAGETRANSLATOR_H_

#include <huestream/common/language/IMessageTranslator.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace huestream {

    class MockMessageTranslator : public IMessageTranslator{
    public:
        MOCK_CONST_METHOD1(Translate, std::string(std::string tag));
    };

}// namespace huestream
#endif     // LIBHUESTREAM_TEST_MOCKTMESSAGETRANSLATOR_H_
