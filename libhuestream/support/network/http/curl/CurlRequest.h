/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_CURL_CURLREQUEST_H_
#define NETWORK_HTTP_CURL_CURLREQUEST_H_

#include <curl/curl.h>

#include <future>
#include <memory>
#include <string>

#include "network/http/HttpResponse.h"
#include "network/http/HttpRequestError.h"
#include "network/http/HttpRequestParams.h"
#include "threading/ThreadPool.h"

namespace huesdk_lib {

    class CurlRequest {
    public:
        CurlRequest(const HttpRequestParams &data, HttpRequestCallback callback);

        CURLM *get_handle() const;

        void send_response(CURLcode curl_code);

        void wait_for_completion();

        void set_as_complete();

    private:
        CURL *_curl;
        struct curl_slist *_header_list;
        struct curl_httppost *_form_post;

        char const* _caarray[2];
        std::shared_ptr<std::string> _certificate;

        /* response buffers */
        std::string _header_buffer;
        std::string _body_buffer;

        HttpRequestCallback _callback;

        HttpRequestError _error;
        HttpResponse     _response;

        std::promise<void> _is_complete;
        std::promise<void> _callback_called;

        void setup_options(const HttpRequestParams &data);

        void setup_tls(const HttpRequestParams &data);

        void setup_proxy(const HttpRequestParams &data);

        void setup_request_headers(const HttpRequestParams &data);

        void setup_post_body(const HttpRequestParams &data);

        HttpRequestError::ErrorCode parse_error_code(CURLcode curl_code);

        void process_response_headers();

        void cleanup();
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_CURL_CURLREQUEST_H_
