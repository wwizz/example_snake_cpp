/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>
#include <vector>
#include <algorithm>

#include "network/http/HttpResponse.h"

using std::string;
using std::map;

namespace huesdk_lib {

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

    const char* HttpResponse::get_body() const {
        return _body.c_str();
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
    
    IHttpResponse* HttpResponse::clone() const {
        return new HttpResponse(_status_code, _body.c_str(), _body.length());
    }
    
    
}  // namespace huesdk_lib
