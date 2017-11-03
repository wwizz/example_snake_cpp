/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_
#define HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_

#include <huestream/common/http/IHttpClient.h>

namespace huestream {

    class HttpClient : public IHttpClient {
    public:
        void Execute(HttpRequestPtr request) override;

        shared_ptr<huesdk_lib::HttpRequest> CreateHttpRequest(const std::string& url,
            int connect_timeout = huesdk_lib::HTTP_CONNECT_TIMEOUT,
            int receive_timeout = huesdk_lib::HTTP_RECEIVE_TIMEOUT,
            int request_timeout = huesdk_lib::HTTP_REQUEST_TIMEOUT,
            bool enable_logging = true,
            huesdk_lib::HttpRequestSecurityLevel security_level = huesdk_lib::HTTP_REQUEST_SECURITY_LEVEL_LOW) override;

    };

}  // namespace huestream

#endif  // HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_
