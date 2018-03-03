/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <curl/curl.h>
#include <mbedtls/ssl.h>

#include <future>
#include <memory>
#include <string>
#include <vector>

#include "support/network/http/HttpResponse.h"
#include "support/network/http/HttpRequestError.h"
#include "support/network/http/HttpRequestParams.h"
#include "support/threading/ThreadPool.h"
#include "support/threading/QueueDispatcher.h"

namespace support {

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

        /* response buffers */
        std::string _header_buffer;
        std::string _body_buffer;

        HttpRequestCallback _callback;

        HttpRequestError _error;
        HttpResponse     _response;

        std::promise<void> _is_complete;
        std::future<void> _is_complete_future;

        std::promise<void> _callback_posted;
        std::future<void> _callback_posted_future;

        QueueDispatcher _dispatcher;

        /* mbedtls certificate setup */
        mbedtls_x509_crt _crt;
        mbedtls_x509_crl _crl;

        /* common name resolution */
        struct curl_slist *_resolve_list;

        /* retrieved server certificate chain */
        std::vector<std::string> _certificate_chain;

        bool _verify_ssl;

        void setup_options(const HttpRequestParams &data);

        void setup_tls(const HttpRequestParams &data);

        void setup_tls_common_name(const HttpRequestParams &data);

        void setup_proxy(const HttpRequestParams &data);

        void setup_request_headers(const HttpRequestParams &data);

        void setup_post_body(const HttpRequestParams &data);

        HttpRequestError::ErrorCode parse_error_code(CURLcode curl_code);

        void process_response_headers();

        void cleanup();

        static CURLcode curl_sslctx_function(CURL* curl, void* sslctx, void* data);
        static int mbedtls_x509parse_verify(void* data, mbedtls_x509_crt* cert, int path_cnt, uint32_t* flags);
    };

}  // namespace support

