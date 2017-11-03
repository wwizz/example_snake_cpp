/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_IHTTPRESPONSE_H_
#define NETWORK_HTTP_IHTTPRESPONSE_H_

#include <cstdlib>

namespace huesdk_lib {
    
    class IHttpResponse {
    public:
        /**
         destructor
         */
        virtual ~IHttpResponse() {}

        /**
         Get status code
         @return The http status code
         */
        virtual unsigned int get_status_code() const = 0;

        /**
         Get body
         @return The http body
         */
        virtual const char* get_body() const = 0;

        /**
         Get body size
         @return The http body size
         */
        virtual size_t get_body_size() const = 0;

        /**
         Get the http header field with the given field name
         @param field_name name of the requested field
         @return the http header field value of the field with the corresponding name, or an empty string if it was not found
         */
        virtual const char* get_header_field_value(const char* field_name) const = 0;
    
        /**
         Clone the http response
         @return The cloned http response
         */
        virtual IHttpResponse* clone() const = 0;
    };
}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_IHTTPRESPONSE_H_
