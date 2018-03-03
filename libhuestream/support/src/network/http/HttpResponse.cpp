/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "support/network/http/HttpResponse.h"

using std::string;
using std::map;

namespace support {

    const char* HttpResponse::EMPTY_STRING = "";

    HttpResponse::HttpResponse() :
    _status_code(0) {
    }

    HttpResponse::HttpResponse(unsigned int status_code, const char* body) :
    _status_code(status_code),
    _body(string(body)) {
    }
    
    HttpResponse::HttpResponse(unsigned int status_code, const char* body, size_t body_size) :
    _status_code(status_code),
    _body(string(body, body_size)) {
    }

    unsigned int HttpResponse::get_status_code() const {
        return _status_code;
    }

    void HttpResponse::set_status_code(unsigned int status_code) {
        _status_code = status_code;
    }

    std::string HttpResponse::get_body() const {
        return _body;
    }
    
    void HttpResponse::set_body(const char* body) {
        _body = body;
    }
    
    void HttpResponse::set_body(const string& body) {
        _body = body;
    }

    size_t HttpResponse::get_body_size() const {
        return _body.size();
    }

    const char* HttpResponse::get_header_field_value(const char* field_name)  const {
        if (field_name == nullptr) {
            return EMPTY_STRING;
        }
        std::string field_name_string(field_name);
        std::transform(field_name_string.begin(), field_name_string.end(), field_name_string.begin(), tolower);
        
        HttpFieldMap::const_iterator field_value = _header_fields.find(field_name_string);
        
        if (field_value ==  _header_fields.end()) {
            return EMPTY_STRING;
        }
        
        return field_value->second.c_str();
    }
    
    void HttpResponse::add_header_field(const char* name, const char* value) {
        std::string name_string(name);
        std::transform(name_string.begin(), name_string.end(), name_string.begin(), tolower);

        _header_fields[name_string] = value;
    }
    
    void HttpResponse::set_certificate_chain(const std::vector<std::string>& certificate_chain) {
        _certificate_chain = certificate_chain;
    }

    std::vector<std::string> HttpResponse::get_certificate_chain() const {
        return _certificate_chain;
    }
    
    std::unique_ptr<IHttpResponse> HttpResponse::clone() const {
        return std::unique_ptr<IHttpResponse>(new HttpResponse(_status_code, _body.c_str(), _body.length()));
    }
    
    
}  // namespace support
