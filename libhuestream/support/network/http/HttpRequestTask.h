/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_HTTPREQUESTTASK_H_
#define NETWORK_HTTP_HTTPREQUESTTASK_H_

#include <string>
#include <memory>

#include "threading/Job.h"
#include "network/http/IHttpResponse.h"
#include "network/http/HttpRequestError.h"
#include "network/http/HttpRequestConst.h"

using Task = huesdk_lib::JobTask;
using huesdk_lib::IHttpResponse;
using huesdk_lib::HttpRequestError;

namespace huesdk_lib {
    class HttpRequestTask : public Task {
    public:
        struct Options {
            int connect_timeout = huesdk_lib::HTTP_CONNECT_TIMEOUT;
            int receive_timeout = huesdk_lib::HTTP_RECEIVE_TIMEOUT;
            int request_timeout = huesdk_lib::HTTP_REQUEST_TIMEOUT;
            bool use_proxy = false;
            std::string proxy_address = "";
            int proxy_port = 0;
        };

        struct HttpRequest;

        explicit HttpRequestTask(const std::string& url, const Options* options = nullptr);

        void set_connect_timeout(int connect_timeout);
        void set_receive_timeout(int receive_timeout);
        void set_request_timeout(int request_timeout);
        void set_proxy(const std::string& proxy_address, int proxy_port);

        IHttpResponse* get_response() const;
        HttpRequestError* get_error() const;

        ~HttpRequestTask() override;

    protected:
        void execute(CompletionHandler completion_handler) override;
        void stop() override;

    private:
        std::unique_ptr<HttpRequestError> _error;
        std::unique_ptr<HttpRequest> _request;
        std::unique_ptr<IHttpResponse> _response;
    };
}  // namespace huesdk_lib


#endif  // NETWORK_HTTP_HTTPREQUESTTASK_H_
