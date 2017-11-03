/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_
#define NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_

#include "network/http/HttpRequestParams.h"
#include "network/http/IHttpClient.h"

namespace huesdk_lib {

    class ObjcHttpClient : public IHttpClient {
    public:
        ObjcHttpClient();

        ~ObjcHttpClient();

        Handle start_request(const HttpRequestParams &data, HttpRequestCallback callback) override;

        void stop_request(Handle handle) override;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_
