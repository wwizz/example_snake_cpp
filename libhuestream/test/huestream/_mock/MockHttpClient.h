/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKHTTPCLIENT_H
#define LIBHUESTREAM_MOCKHTTPCLIENT_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "huestream/common/http/IHttpClient.h"

MATCHER_P3(MatchHttpRequest, method, url, body, "Request did not match") {
    return arg->GetMethod() == method && arg->GetUrl() == url && arg->GetBody() == body;
};

namespace huestream {
    class MockHttpClient : public IHttpClient {
    public:
        MOCK_METHOD1(Execute, void(HttpRequestPtr request));
        MOCK_METHOD6(CreateHttpRequest, shared_ptr<huesdk_lib::HttpRequest>(const std::string& url,
            int connect_timeout,
            int receive_timeout,
            int request_timeout,
            bool enable_logging,
            huesdk_lib::HttpRequestSecurityLevel security_level));
    };
}

#endif //LIBHUESTREAM_MOCKHTTPCLIENT_H
