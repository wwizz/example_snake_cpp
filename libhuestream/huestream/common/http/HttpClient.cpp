/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <memory>

#include "huestream/common/http/HttpClient.h"

#include "support/network/http/HttpRequest.h"
#include "support/threading/QueueExecutor.h"
#include "support/util/DeleteLater.h"

#include <utility>

namespace huestream {

void HttpClient::Execute(HttpRequestPtr request) {
    ExecuteAsync(request);
    request->WaitUntilReady();
}

void HttpClient::ExecuteAsync(HttpRequestPtr request) {
    request->StartRequest();
    auto req = std::make_shared<support::HttpRequest>(request->GetUrl());

    if (!request->GetToken().empty()) {
        req->set_bearer_auth_header(request->GetToken());
    }

    req->set_verify_ssl(request->GetEnableSslVerification());

    req->do_request(request->GetMethod(), request->GetBody(), nullptr,
                    [req, request](const support::HttpRequestError &error, const support::IHttpResponse &response) mutable {
                        request->SetSuccess(false);

                        if (error.get_code() == support::HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS) {
                            request->SetResponse(response.get_body());
                            request->SetStatusCode(response.get_status_code());
                            request->SetSuccess(true);
                        }

                        request->FinishRequest();
                        support::delete_later(std::move(req));
                    });
}

shared_ptr<support::HttpRequest> HttpClient::CreateHttpRequest(const std::string& url,
                                                               int connect_timeout,
                                                               int receive_timeout,
                                                               int request_timeout,
                                                               bool enable_logging,
                                                               support::HttpRequestSecurityLevel security_level) {
    return std::make_shared<support::HttpRequest>(url, connect_timeout, receive_timeout, request_timeout, enable_logging, security_level);
}

}  // namespace huestream
