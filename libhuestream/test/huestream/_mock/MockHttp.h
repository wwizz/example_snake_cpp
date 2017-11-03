/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKHTTP_H
#define LIBHUESTREAM_MOCKHTTP_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "huestream/common/http/IHttpClient.h"

MATCHER_P3(MatchHttpRequest, method, url, body, "Request did not match") {
    return arg->GetMethod() == method && arg->GetUrl() == url && arg->GetBody() == body;
};

namespace huestream {
    class MockHttp : public IHttpClient {
    public:
        MOCK_METHOD1(Execute, void(HttpRequestPtr));
    };
}

#endif  // LIBHUESTREAM_MOCKHTTP_H
