/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/
#ifndef NETWORK_HTTP_IHTTPREQUEST_H_
#define NETWORK_HTTP_IHTTPREQUEST_H_

#include <string>

#include "network/http/HttpRequestError.h"
#include "network/http/IHttpResponse.h"

namespace huesdk_lib {

    enum HttpRequestReturnCode {
        HTTP_REQUEST_STATUS_OK,
        HTTP_REQUEST_STATUS_FAILED   = -1,
        HTTP_REQUEST_STATUS_CANCELED = -2,
    };

    class IHttpRequest {
    public:
        /**
         Cancel this request
         */
        virtual void cancel() = 0;
        
        /**
         Destructor
         */
        virtual ~IHttpRequest() = default;

        /**
         get a header field
         @param field_name name of the field
         @return value of the field
         */
        virtual std::string get_header_field_value(const std::string &field_name) const = 0;

        /**
         get the url
         @return server url
         */
        virtual std::string get_url() const = 0;

        /**
         set the url
         @param url url
         */
        virtual void set_url(const std::string& url) = 0;

        /**
         get the method to be executed on the url
         @return method (GET, POST, PUT, DELETE)
         */
        virtual std::string get_method() const = 0;

        /**
         get the body to be sent
         @return body content
         */
        virtual std::string get_body() const = 0;
    };
}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_IHTTPREQUEST_H_
