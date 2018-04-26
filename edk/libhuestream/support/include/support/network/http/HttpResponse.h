/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "support/util/StringUtil.h"
#include "support/network/http/IHttpResponse.h"

namespace support {
    
    class HttpResponse : public IHttpResponse {
    public:
        /**
         Construct without data
         */
        HttpResponse();
        
        /**
         Destructor
         */
        ~HttpResponse() = default;

        /**
         Construct with data
         @param status_code The http status code
         @param body        The http body
         */
        HttpResponse(unsigned int status_code, const char* body);
        
        /**
         Construct with data
         @param status_code The http status code
         @param body        The http body
         @param body_size   By default -1, which means the size of the body will be counteds
                            until the first null terminating character
         */
        HttpResponse(unsigned int status_code, const char* body, size_t body_size);
        
        /**
         @see support/network/http/IHttpResponse.h
         */
        unsigned int get_status_code() const;
        
        /**
         Set status code
         @param status_code The http status code
         */
        void set_status_code(unsigned int status_code);
        
        /**
         @see support/network/http/IHttpResponse.h
         */
        std::string get_body() const;
        
        /**
         Set body
         @param body The http body
         */
        void set_body(const char* body);
        
        /**
         Set body
         @param body The http body
         */
        void set_body(const std::string& body);
        
        /**
         @see support/network/http/IHttpResponse.h
         */
        size_t get_body_size() const;
        
        /**
         @see support/network/http/IHttpResponse.h
         */
        const char* get_header_field_value(const char* field_name) const;
            
        /**
         add a header field
         @param name name of the header field
         @param value value of the header field
         */
        void add_header_field(const char* name, const char* value);
        
        /**
         Set the certificate chain
         */
        void set_certificate_chain(const std::vector<std::string>& certificate_chain);

        /**
         @see support/network/http/IHttpResponse.h
         */
        std::vector<std::string> get_certificate_chain() const;

        /**
         @see support/network/http/IHttpResponse.h
         */
        virtual std::unique_ptr<IHttpResponse> clone() const;
        
    private:
        typedef std::map<std::string, std::string> HttpFieldMap;

        static const char* EMPTY_STRING;
        
        /** the http status code */
        unsigned int _status_code;
        /** the http body */
        std::string  _body;

        /** the http header */
        HttpFieldMap _header_fields;

        /** certificate chain */
        std::vector<std::string> _certificate_chain;
    };

}  // namespace support

