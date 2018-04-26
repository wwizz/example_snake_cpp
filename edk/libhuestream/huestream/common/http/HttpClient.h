/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_
#define HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_

#include <huestream/common/http/IHttpClient.h>

#include <memory>
#include <string>

namespace huestream {

    class HttpClient : public IHttpClient {
    public:
        void Execute(HttpRequestPtr request) override;
        void ExecuteAsync(HttpRequestPtr request) override;
        shared_ptr<support::HttpRequest> CreateHttpRequest(const std::string& url,
            int connect_timeout = support::HTTP_CONNECT_TIMEOUT,
            int receive_timeout = support::HTTP_RECEIVE_TIMEOUT,
            int request_timeout = support::HTTP_REQUEST_TIMEOUT,
            bool enable_logging = true,
            support::HttpRequestSecurityLevel security_level = support::HTTP_REQUEST_SECURITY_LEVEL_LOW) override;
    };

}  // namespace huestream

#endif  // HUESTREAM_COMMON_HTTP_HTTPCLIENT_H_
