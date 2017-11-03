/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef LIBHUESTREAM_MOCKHTTPREQUEST_H
#define LIBHUESTREAM_MOCKHTTPREQUEST_H

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "network/http/HttpRequest.h"

namespace huestream {
    class MockHttpRequest : public huesdk_lib::HttpRequest {
    public:
        explicit MockHttpRequest(const string& url, int connect_timeout, int receive_timeout = 8, int request_timeout = 8)
        : HttpRequest(url, connect_timeout, receive_timeout, request_timeout) {}

        MOCK_METHOD4(do_request, int(const string& method, string& body, huesdk_lib::File* file, huesdk_lib::HttpRequestCallback callback));

        MOCK_METHOD1(do_get, int(huesdk_lib::HttpRequestCallback callback));

        MOCK_METHOD3(do_post, int(const string& body, huesdk_lib::File* file, huesdk_lib::HttpRequestCallback callback));

        MOCK_METHOD2(do_put, int(const string& body, huesdk_lib::HttpRequestCallback callback));

        MOCK_METHOD1(do_delete, int(huesdk_lib::HttpRequestCallback callback));

        MOCK_METHOD0(cancel, void());

        MOCK_METHOD1(set_executor, void(huesdk_lib::HttpRequestExecutor* executor));

        MOCK_METHOD2(add_header_field, void(const string& name, const string& value));
    };
}

#endif //LIBHUESTREAM_MOCKHTTPCLIENT_H
