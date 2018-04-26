/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKHTTPCLIENT_H
#define LIBHUESTREAM_MOCKHTTPCLIENT_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>

#include "huestream/common/http/IHttpClient.h"

MATCHER_P3(MatchHttpRequest, method, url, body, "Request did not match") {
    return arg->GetMethod() == method && arg->GetUrl() == url && arg->GetBody() == body;
};

MATCHER_P4(MatchHttpRequestWithAccessToken, method, url, body, token, "Request did not match") {
	return arg->GetMethod() == method && arg->GetUrl() == url && arg->GetBody() == body && arg->GetToken() == token;
};

namespace huestream {
    class MockHttpClient : public IHttpClient {
    public:
        MOCK_METHOD1(Execute, void(HttpRequestPtr request));
        MOCK_METHOD1(ExecuteAsync, void(HttpRequestPtr request));
        MOCK_METHOD6(CreateHttpRequest, shared_ptr<support::HttpRequest>(const std::string& url,
            int connect_timeout,
            int receive_timeout,
            int request_timeout,
            bool enable_logging,
            support::HttpRequestSecurityLevel security_level));
    };

    class MockWrapperHttpClient : public IHttpClient {
    public:
        explicit MockWrapperHttpClient(const std::shared_ptr<MockHttpClient>& mock)
          : _mock(mock) {}

        void Execute(HttpRequestPtr request) {
            _mock->Execute(request);
        }

        void ExecuteAsync(HttpRequestPtr request) {
            _mock->ExecuteAsync(request);
        }

        std::shared_ptr<support::HttpRequest> CreateHttpRequest(
           const std::string& url,
           int connect_timeout,
           int receive_timeout,
           int request_timeout,
           bool enable_logging,
           support::HttpRequestSecurityLevel security_level) {
            return _mock->CreateHttpRequest(
                    url, connect_timeout, receive_timeout,
                    request_timeout, enable_logging, security_level);
        }

    private:
        std::shared_ptr<MockHttpClient> _mock;
    };
}

#endif //LIBHUESTREAM_MOCKHTTPCLIENT_H
