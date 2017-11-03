/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_HTTP_IHTTPCLIENT_H_
#define HUESTREAM_COMMON_HTTP_IHTTPCLIENT_H_

#include <huestream/common/http/HttpRequestInfo.h>
#include <network/http/HttpRequest.h>

#include <string>
#include <memory>

namespace huestream {

    class IHttpClient {
    public:
        virtual ~IHttpClient() {}

        virtual void Execute(HttpRequestPtr request) = 0;

        virtual shared_ptr<huesdk_lib::HttpRequest> CreateHttpRequest(const std::string& url,
            int connect_timeout = huesdk_lib::HTTP_CONNECT_TIMEOUT,
            int receive_timeout = huesdk_lib::HTTP_RECEIVE_TIMEOUT,
            int request_timeout = huesdk_lib::HTTP_REQUEST_TIMEOUT,
            bool enable_logging = true,
            huesdk_lib::HttpRequestSecurityLevel security_level = huesdk_lib::HTTP_REQUEST_SECURITY_LEVEL_LOW) = 0;
    };

    typedef std::shared_ptr<IHttpClient> HttpClientPtr;
}  // namespace huestream

#endif  // HUESTREAM_COMMON_HTTP_IHTTPCLIENT_H_
