/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifdef SDK_TEST

#include "network/http/_test/HttpRequestDelegator.h"

#include <string>
#include <mutex>
#include <cassert>

using std::string;
using std::mutex;
using std::unique_lock;

namespace huesdk_lib {

    /* provider */
    
    static shared_ptr<HttpRequestDelegateProvider> _delegate_provider = shared_ptr<HttpRequestDelegateProvider>(new HttpRequestDelegateProviderImpl());
    static mutex _delegate_provider_mutex;

    void HttpRequestDelegator::set_delegate_provider(shared_ptr<HttpRequestDelegateProvider> delegate_provider) {
        unique_lock<mutex> _delegate_provider_lock(_delegate_provider_mutex);
        _delegate_provider = delegate_provider;
    }


    /* delegator */

    HttpRequestDelegator::HttpRequestDelegator(const string& url, int connect_timeout, int receive_timeout, int request_timeout, bool enable_logging, HttpRequestSecurityLevel security_level) 
        : HttpRequest(url, connect_timeout, receive_timeout, request_timeout, enable_logging, security_level) {
        unique_lock<mutex> _delegate_provider_lock(_delegate_provider_mutex);
        _delegate = _delegate_provider->get_delegate(url, connect_timeout, receive_timeout, request_timeout, enable_logging, security_level);
        HUE_LOG << HUE_TEST << HUE_INFO << url << " delegate has been set" << HUE_ENDL;
        assert(_delegate != nullptr);
    }
                                                                    
    int HttpRequestDelegator::get_connect_timeout() {
        return _delegate->get_connect_timeout();
    }

    void HttpRequestDelegator::set_connect_timeout(int connect_timeout) {
        _delegate->set_connect_timeout(connect_timeout);
    }
    
    int HttpRequestDelegator::get_receive_timeout() {
        return _delegate->get_receive_timeout();
    }
    
    void HttpRequestDelegator::set_receive_timeout(int receive_timeout) {
        _delegate->set_receive_timeout(receive_timeout);
    }
    
    int HttpRequestDelegator::get_request_timeout() {
        return _delegate->get_request_timeout();
    }
    
    void HttpRequestDelegator::set_request_timeout(int request_timeout) {
        _delegate->set_request_timeout(request_timeout);
    }
    
    const string& HttpRequestDelegator::get_proxy_address() {
        return _delegate->get_proxy_address();
    }

    int HttpRequestDelegator::get_proxy_port() {
        return _delegate->get_proxy_port();
    }
    
    void HttpRequestDelegator::set_proxy(const string& proxy_address, int proxy_port) {
        _delegate->set_proxy(proxy_address, proxy_port);
    }
    
    string HttpRequestDelegator::get_content_type() {
        return _delegate->get_content_type();
    }

    void HttpRequestDelegator::set_content_type(const string& content_type) {
        _delegate->set_content_type(content_type);
    }

    void HttpRequestDelegator::add_header_field(const string& name, const string& value) {
        _delegate->add_header_field(name, value);
    }

    string HttpRequestDelegator::get_header_field_value(const string &field_name) const {
        return _delegate->get_header_field_value(field_name);
    }
    
    int HttpRequestDelegator::do_get(HttpRequestCallback callback) {
        HUE_LOG << HUE_TEST << HUE_INFO << "do_get: " << _delegate->get_url() << HUE_ENDL;
        return _delegate->do_get(callback);
    }
    
    int HttpRequestDelegator::do_post(const string& body, huesdk_lib::File* file, HttpRequestCallback callback) {
        HUE_LOG << HUE_TEST << HUE_INFO << "do_post: " << _delegate->get_url() << ", body \"" << body << "\"" << HUE_ENDL;
        return _delegate->do_post(body, file, callback);
    }

    int HttpRequestDelegator::do_put(const string& body, HttpRequestCallback callback) {
        HUE_LOG << HUE_TEST << HUE_INFO << "do_put: " << _delegate->get_url() << ", body \"" << body << "\"" << HUE_ENDL;
        return _delegate->do_put(body, callback);
    }

    int HttpRequestDelegator::do_delete(HttpRequestCallback callback) {
        HUE_LOG << HUE_TEST << HUE_INFO << "do_delete: " << _delegate->get_url() << HUE_ENDL;
        return _delegate->do_delete(callback);
    }

    int HttpRequestDelegator::do_request(const string& method, const string& body, File* file, HttpRequestCallback callback) {
        HUE_LOG << HUE_TEST << HUE_INFO << "do_request: " << _delegate->get_url() << ", method: " << method << ", body \"" << body << "\"" << HUE_ENDL;
        return _delegate->do_request(method, body, file, callback);
    }

    void HttpRequestDelegator::cancel() {
        HUE_LOG << HUE_TEST << HUE_INFO << "HttpRequestDelegator::cancel" << HUE_ENDL;
        _delegate->cancel();

        if (_executor != nullptr) {
            _executor->request_canceled(this);
        }
    }
    
    void HttpRequestDelegator::set_executor(huesdk_lib::HttpRequestExecutor *executor) {
        _delegate->set_executor(executor);
        _executor = executor;
    }

}  // namespace huesdk_lib

#endif
