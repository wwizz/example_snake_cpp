/*******************************************************************************
Copyright (C) 2017 Philips Lighting Holding B.V.
All Rights Reserved.
********************************************************************************/
#ifndef NETWORK_HTTP_HTTPREQUESTEXECUTOR_H_
#define NETWORK_HTTP_HTTPREQUESTEXECUTOR_H_

#include <future>
#include <memory>
#include <map>

#include "network/http/IHttpResponse.h"
#include "network/http/HttpRequestError.h"
#include "std/types/File.h"
#include "threading/ThreadPool.h"

namespace huesdk_lib {
    class HttpRequest;

    class HttpRequestExecutor{
    public:
        class IRequestInfo {
        public:
            virtual ~IRequestInfo() {}
            virtual HttpRequest* get_request() = 0;
            virtual int retry_countdown() = 0;
        };

        class RequestInfo;

        /**
         Callback
         @param error the http request error that occurred (or HTTP_REQUEST_ERROR_CODE_SUCCESS)
         @param response the http response from the server
         @param request_info can be used to execute the same request again, by calling add(request_info)
         */
        typedef function<void (const huesdk_lib::HttpRequestError& error, const IHttpResponse& response, std::shared_ptr<huesdk_lib::HttpRequestExecutor::IRequestInfo> request_info)> Callback;

        typedef enum RequestType {
            REQUEST_TYPE_GET = 0,
            REQUEST_TYPE_PUT,
            REQUEST_TYPE_POST,
            REQUEST_TYPE_DELETE
        } RequestType;

        /**
         constructor
         @param max_retries number of times to retry when a request fails
        */
        explicit HttpRequestExecutor(int max_retries);

        /**
        destructor
        */
        ~HttpRequestExecutor();

        /**
         constructor
         @param max_retries number of times to retry when a request fails
         */
        void set_max_retries(int max_retries);

        /**
         * cancel requests and stop executor
         */
        void stop();

        /**
         * start the executor (Note: by default executor is already in "started" state)
         */
        void start();

        /**
         add a request to be executed
         @param request the HttpRequest to preform
         @param request_type the type of request: get, put, post or delete
         @param callback the callback to be called after the request has finished
         @param resource_path the URL of the request
         @param body the HTTP body to send with the request (optional)
         @param file the file to send with the request (optional)
         */
        void add(HttpRequest* request, RequestType request_type, Callback callback, const char* resource_path, const char* body = nullptr, File* file = nullptr);

        /**
         add a request to be executed by giving the request_info object that was recieved on callback of a previous execution of the request
         @param request_info request info object
         */
        void add(std::shared_ptr<IRequestInfo> request_info);

        /**
         notify that a request has been canceled (only to be called by HttpRequest)
         @param request the request that was canceled
         */
        void request_canceled(HttpRequest* request);

    private:
        typedef std::future<RequestInfo> RequestFuture;

        static const char* REQUEST_METHOD[];

        ThreadPool _request_pool;

        std::atomic<int>            _max_retries;
        std::atomic<bool>           _stopped;
        std::mutex                  _request_map_mutex;
        std::condition_variable     _request_map_cond;
        std::map<HttpRequest*, std::shared_ptr<IRequestInfo>> _request_map;

        /**
         execute an HTTP request
         @request_info all required information to perform an HTTP request
         */
        void execute(std::shared_ptr<IRequestInfo> request_info);

        /**
         handle the response of an HTTP request
         @param error an error that occurred during the request or HTTP_REQUEST_ERROR_CODE_SUCCESS
         @param response the HTTP response of the server
         @param request_info all information to (re-)execute the request
         */
        void handle_response(const HttpRequestError& error, const IHttpResponse& response, std::shared_ptr<IRequestInfo> request_info);

        /**
         add a request to the map of requests
         */
        void add_request(HttpRequest* request, std::shared_ptr<IRequestInfo> request_info);
        /**
         remove a request from the map of requests
         */
        void remove_request(HttpRequest* request);
        /**
         get request from the map of requests
         */
        HttpRequest* get_request(std::shared_ptr<IRequestInfo> request_info);
    };


}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_HTTPREQUESTEXECUTOR_H_
