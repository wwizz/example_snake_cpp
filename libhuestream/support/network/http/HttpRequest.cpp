/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <string>

#ifdef DEFAULT_HTTP_CLIENT_INCLUDE
    #define Q(x) #x
    #define QUOTE(x) Q(x)
    #include QUOTE(DEFAULT_HTTP_CLIENT_INCLUDE)
#endif

#include "network/http/HttpRequest.h"
#include "network/http/HttpRequestBase.h"
#include "network/http/HttpRequestParams.h"
#include "network/http/IHttpClient.h"
#include "network/http/IHttpResponse.h"

namespace huesdk_lib {

    /* http client management */

    static std::mutex   _instance_mutex;
    static IHttpClient* _client_instance = nullptr;

    void HttpRequest::set_http_client(IHttpClient* client) {
        std::lock_guard<std::mutex> lk(_instance_mutex);
        if (_client_instance != nullptr) {
            HUE_LOG << HUE_ERROR << HUE_NETWORK << "An http client was already defined." << HUE_ENDL;
            std::terminate();
        }
        _client_instance = client;
    }

    static IHttpClient* get_http_client() {
        std::lock_guard<std::mutex> lk(_instance_mutex);
#ifdef DEFAULT_HTTP_CLIENT
        if (_client_instance == nullptr) {
            _client_instance = new DEFAULT_HTTP_CLIENT();
        }
#endif
        if (_client_instance == nullptr) {
            HUE_LOG << HUE_ERROR << HUE_NETWORK << "No http client specified. Please call HttpRequest::set_http_client before any request is made." << HUE_ENDL;
            std::terminate();
        }
        return _client_instance;
    }

    /* httprequest implementation */

    HttpRequest::HttpRequest(const std::string& url, int connect_timeout, int receive_timeout, int request_timeout, bool enable_logging, HttpRequestSecurityLevel security_level) :
            HttpRequestBase(url, connect_timeout, receive_timeout, request_timeout, enable_logging, security_level), _handle() {
    }

    HttpRequest::~HttpRequest() {
        cancel();
    }

    int HttpRequest::do_request_internal(const std::string& method, const std::string& body, File* file, HttpRequestCallback original_callback) {
        HUE_LOG << _log_component <<  HUE_DEBUG << "HttpRequest: url: " << _url << ", method: " << method << ", body: " << body << HUE_ENDL;

        if (file != nullptr) {
            set_content_type(HTTP_CONTENT_TYPE_MULTIPART_FORM_DATA);
        }

        HttpRequestParams data;
        data.url = _url;
        data.proxy_address = _proxy_address;
        data.proxy_port = _proxy_port;
        data.connect_timeout = _connect_timeout;
        data.receive_timeout = _receive_timeout;
        data.request_timeout = _request_timeout;
        data.enable_logging = _enable_logging;
        data.log_component = _log_component;
        data.security_level = _security_level;
        data.headers = _headers;
        data.method = method;
        data.body = body;
        data.file = file;

        auto callback = [this, original_callback](const huesdk_lib::HttpRequestError& error, const IHttpResponse& response) {
            HUE_LOG << _log_component <<  HUE_DEBUG << "HttpRequest: " << error.get_message() << ", status: " << response.get_status_code() << ", body: " << response.get_body() << HUE_ENDL;

            notify_monitor_response_received(error, response);
            original_callback(error, response);
        };

        {
            std::lock_guard<std::mutex> lk(_mutex);

            if (_handle) {
                get_http_client()->stop_request(_handle);
            }

            _handle = get_http_client()->start_request(data, callback);
        }

        return HTTP_REQUEST_STATUS_OK;
    }

    void HttpRequest::cancel() {
        {
            std::lock_guard<std::mutex> lk(_mutex);

            if (_handle) {
                get_http_client()->stop_request(_handle);
                _handle = {};
            }
        }

        if (_executor != nullptr) {
            _executor->request_canceled(this);
        }
    }
}  // namespace huesdk_lib
