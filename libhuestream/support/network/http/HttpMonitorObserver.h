/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_HTTPMONITOROBSERVER_H_
#define NETWORK_HTTP_HTTPMONITOROBSERVER_H_

#include "network/http/IHttpResponse.h"
#include "network/http/IHttpRequest.h"
#include "network/http/HttpRequestError.h"

namespace huesdk_lib {

    class HttpMonitorObserver {
    public:
        /**
         Method called when a request has been performed.
         @param request request that has been performed.
         @param status  status of performed action. Check HttpRequestReturnCode for expected values.
         */
        virtual void on_request_performed(const IHttpRequest& request, int status) = 0;

        /**
         Method called when a response has been received.
         @param resopnse response that has been received.
         @param error    error information about received response.
         */
        virtual void on_response_received(const IHttpResponse& response, const HttpRequestError& error) = 0;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_HTTPMONITOROBSERVER_H_
