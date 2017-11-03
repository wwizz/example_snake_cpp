/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/http/HttpClient.h>

#include <memory>

namespace huestream {

    void HttpClient::Execute(HttpRequestPtr request) {
        auto req = std::make_shared<huesdk_lib::HttpRequest>(request->GetUrl());
        req->do_request(request->GetMethod(), request->GetBody(), nullptr,
                        [&](const huesdk_lib::HttpRequestError &error, const huesdk_lib::IHttpResponse &response) {
                            request->SetSuccess(false);
                            if (error.get_code() == huesdk_lib::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS) {
                                request->SetSuccess(true);
                                request->SetResponse(response.get_body());
                            }
                            request->FinishRequest();
                        });
        request->WaitUntilReady();
    }

    shared_ptr<huesdk_lib::HttpRequest> HttpClient::CreateHttpRequest(const std::string& url,
        int connect_timeout,
        int receive_timeout,
        int request_timeout,
        bool enable_logging,
        huesdk_lib::HttpRequestSecurityLevel security_level) {

        return std::make_shared<huesdk_lib::HttpRequest>(url, connect_timeout, receive_timeout, request_timeout, enable_logging, security_level);
    }

}  // namespace huestream
