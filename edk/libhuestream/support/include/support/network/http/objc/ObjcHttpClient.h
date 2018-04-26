/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef INCLUDE_SUPPORT_NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_
#define INCLUDE_SUPPORT_NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_

#include "support/network/http/HttpRequestParams.h"
#include "support/network/http/IHttpClient.h"

namespace support {

    class ObjcHttpClient : public IHttpClient {
    public:
        ObjcHttpClient();

        ~ObjcHttpClient();

        Handle start_request(const HttpRequestParams &data, HttpRequestCallback callback) override;

        void stop_request(Handle handle) override;
    };

}  // namespace support

#endif  // INCLUDE_SUPPORT_NETWORK_HTTP_OBJC_OBJCHTTPREQUEST_H_
