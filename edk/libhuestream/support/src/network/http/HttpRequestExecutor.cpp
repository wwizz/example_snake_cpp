/*******************************************************************************
Copyright (C) 2018 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#include <math.h>

#include <string>
#include <utility>
#include <map>
#include <mutex>
#include <algorithm>

#include "support/logging/Log.h"

#include "support/network/http/HttpRequestExecutor.h"
#include "support/network/http/IHttpRequest.h"
#include "support/network/http/HttpRequestError.h"

#define HUESDK_LIB_NETWORK_HTTP_RESPONSE_STATUS_CODE_OK (200)

#include "support/network/http/_test/HttpRequestDelegator.h"
#define HTTP_REQUEST support::HttpRequestDelegator

using std::shared_ptr;
using std::mutex;
using std::lock_guard;
using std::unique_lock;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

namespace support {

    class HttpRequestExecutor::RequestInfo : public HttpRequestExecutor::IRequestInfo {
    public:
        RequestInfo(HttpRequest* request,
                    RequestType request_type,
                    int retry_countdown,
                    int server_reset_retry_countdown,
                    Callback callback,
                    string resource_path,
                    string body,
                    File* file) :
        _request(request),
        _request_type(request_type),
        _retry_countdown(retry_countdown),
        _server_reset_retry_countdown(server_reset_retry_countdown),
        _callback(callback),
        _resource_path(resource_path),
        _body(body),
        _file(file),
        _num_active_instances(1) {
        }

        virtual ~RequestInfo() {
        }
        
        HttpRequest* get_request() override {
            return _request;
        }

        int retry_countdown() override {
            return _retry_countdown;
        }
        
        HttpRequest* _request;
        RequestType _request_type;
        int _retry_countdown;
        int _server_reset_retry_countdown;
        Callback _callback;
        string _resource_path;
        string _body;
        File* _file;
        int _num_active_instances;
    };

    const char* HttpRequestExecutor::REQUEST_METHOD[] = {"GET", "PUT", "POST", "DELETE" };
    
    /** */
    static const int SERVER_RESET_OCCURRENCE_MS = 900;

    HttpRequestExecutor::HttpRequestExecutor(int max_retries) :
    _request_pool(1),
    _max_retries(max_retries),
    _stopped(false),
    _http_error_delegate(std::bind(&HttpRequestExecutor::handle_http_error, _1, _2, _3)) {
    }
 
    HttpRequestExecutor::~HttpRequestExecutor() {
    }

    void HttpRequestExecutor::set_max_retries(int max_retries) {
        _max_retries = max_retries;
    }

    void HttpRequestExecutor::set_http_error_delegate(HttpErrorDelegate http_error_delegate) {
        _http_error_delegate = http_error_delegate ? http_error_delegate : std::bind(&HttpRequestExecutor::handle_http_error, _1, _2, _3);
    }

    void HttpRequestExecutor::stop() {
        unique_lock<mutex> lock(_request_map_mutex);
        _stopped = true;
        _request_map_cond.wait(lock, [this] { return _request_map.empty(); } );
    }

    void HttpRequestExecutor::start() {
        _stopped = false;
    }

    void HttpRequestExecutor::add_request(HttpRequest* request, shared_ptr<IRequestInfo> request_info) {
        auto item = std::pair<HttpRequest*, shared_ptr<IRequestInfo>>(request, request_info);
        auto result = _request_map.insert(item);
        
        if (result.second == false) {
            HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "HttpRequestExecutor: add_request called more than once on the same request" << HUE_ENDL;

            auto request_iterator = result.first;
            auto request_info_second = request_iterator->second;
            auto request_info_cast = static_cast<RequestInfo*>(request_info_second.get());
            request_info_cast->_num_active_instances++;
        }
    }

    void HttpRequestExecutor::remove_request(HttpRequest* request) {
        unique_lock<mutex> lock(_request_map_mutex);

        auto request_iterator = _request_map.find(request);
        if (request_iterator != _request_map.end()) {
            auto request_info = request_iterator->second;
            auto request_info_cast = static_cast<RequestInfo*>(request_info.get());

            if (request_info_cast->_num_active_instances == 1) {
                _request_map.erase(request_iterator);
                _request_map_cond.notify_all();
            } else {
                request_info_cast->_num_active_instances--;
            }
        } else {
            HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "HttpRequestExecutor: remove_request called on unknown request" << HUE_ENDL;
        }
    }
   
    void HttpRequestExecutor::add(HttpRequest* request, RequestType request_type, Callback callback, const char* resource_path, const char* body, File* file) {
        unique_lock<mutex> lock(_request_map_mutex);
        string body_string;
        if (body != nullptr) {
            body_string = body;
        }
        /**
         Calculate the max number of retries for a server reset based on the request timeout
         and the average times it takes for the server reset to occur.
         
         Example: if you have a request timeout set of 10 seconds and the average reset occurrence of the server
         reset is 1 second, you can do up to 9 retries (you still have 9 seconds left after the first occurrence).
         */
        int server_reset_max_retries = static_cast<int>(floor((request->get_request_timeout() * 1000) / SERVER_RESET_OCCURRENCE_MS) - 1);

        auto request_info = std::make_shared<RequestInfo>(request, request_type, _max_retries, server_reset_max_retries, callback, resource_path, body_string, file);

        add_request(request, request_info);
        
        request->set_executor(this);
        _request_pool.add_task([this, request_info] () -> void { return execute(request_info); });
    }

    void HttpRequestExecutor::add(shared_ptr<IRequestInfo> request_info) {
        unique_lock<mutex> lock(_request_map_mutex);
        RequestInfo* request_info_cast = static_cast<RequestInfo*>(request_info.get());
        if (request_info_cast) {
            request_info_cast->_retry_countdown = std::max(0, request_info_cast->_retry_countdown - 1);
        }
        add_request(request_info->get_request(), request_info);
        _request_pool.add_task([this, request_info] () -> void { return execute(request_info); });
    }

    HttpRequest* HttpRequestExecutor::get_request(shared_ptr<IRequestInfo> request_info) {
        HttpRequest* request = nullptr;
        const RequestInfo* request_info_cast = static_cast<const RequestInfo*>(request_info.get());

        unique_lock<mutex> lock(_request_map_mutex);
        if (_request_map.find(request_info_cast->_request) != _request_map.end()) {
            // sanity check
            if (request_info_cast->_request_type > RequestType::REQUEST_TYPE_DELETE) {
                HUE_LOG << HUE_NETWORK <<  HUE_ERROR << "HttpRequestExecutor::execute: invalid request type" << static_cast<std::uint8_t>(request_info_cast->_request_type) << HUE_ENDL;
            } else {
                request = request_info_cast->_request;
            }
        }

        return request;
    }

    void HttpRequestExecutor::execute(shared_ptr<IRequestInfo> request_info) {
        const RequestInfo* request_info_cast = static_cast<const RequestInfo*>(request_info.get());
        HttpRequest* request = get_request(request_info);

        if (request) {
            request->do_request(REQUEST_METHOD[static_cast<std::uint8_t>(request_info_cast->_request_type)], request_info_cast->_body, request_info_cast->_file, [this, request_info] (const HttpRequestError& error, const IHttpResponse& response) {
                handle_response(error, response, request_info);
            });
        }
    }

    void HttpRequestExecutor::request_canceled(HttpRequest* request) {
        unique_lock<mutex> lock(_request_map_mutex);
        // wait until this request is not in the map anymore
        _request_map_cond.wait(lock, [this, request] { return _request_map.find(request) == _request_map.end(); } );
    }
    
    void HttpRequestExecutor::handle_response(const HttpRequestError& error, const IHttpResponse& response, shared_ptr<IRequestInfo> request_info) {
        RequestInfo* request_info_cast = static_cast<RequestInfo*>(request_info.get());

        HUE_LOG << HUE_NETWORK <<  HUE_DEBUG << "HttpRequestExecutor: handle_response " << request_info_cast->_resource_path.c_str() << HUE_ENDL;

        HttpRequestError mutable_error(error);


        // Discard request when request executor is stopped
        HttpErrorPostAction action;
        action.type = HttpErrorPostActionType::DISCARD;

        if (!_stopped) {
            action = _http_error_delegate(mutable_error, response, request_info);
        }

        if (action.type == HttpErrorPostActionType::RETRY && request_info_cast->_retry_countdown > 0) {
            request_info_cast->_retry_countdown--;

            _request_pool.add_task([this, action, request_info] () -> void {
                if (action.delay.count() > 0) {
                    std::this_thread::sleep_for(action.delay);
                }

                return execute(request_info);
            });
        } else {
            if (request_info_cast->_callback != nullptr) {
                request_info_cast->_callback(mutable_error, response, request_info);
            } else {
                HUE_LOG << HUE_NETWORK << HUE_WARN << "HttpRequestExecutor: no callback defined " << HUE_ENDL;
            }

            do {
                remove_request(request_info_cast->_request);
            } while (action.type == HttpErrorPostActionType::DISCARD &&
                     _request_map.find(request_info_cast->_request) != _request_map.end());
        }
    }

    HttpRequestExecutor::HttpErrorPostAction HttpRequestExecutor::handle_http_error(support::HttpRequestError& error, const support::IHttpResponse& /*response*/, const std::shared_ptr<support::HttpRequestExecutor::IRequestInfo>& /*request_info*/) {
        HttpRequestExecutor::HttpErrorPostAction action;
        action.type = HttpErrorPostActionType::NONE;

        const HttpRequestError::ErrorCode error_code = error.get_code();

        if (error_code != HttpRequestError::HTTP_REQUEST_ERROR_CODE_SUCCESS &&
            error_code != HttpRequestError::HTTP_REQUEST_ERROR_CODE_CANCELED) {
            action.type = HttpErrorPostActionType::RETRY;
        }

        return action;
    }

}  // namespace support
