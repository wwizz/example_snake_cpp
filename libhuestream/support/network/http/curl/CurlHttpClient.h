/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_CURL_CURLHTTPCLIENT_H_
#define NETWORK_HTTP_CURL_CURLHTTPCLIENT_H_

#include <curl/curl.h>

#include <thread>

#include "network/http/curl/CurlRequest.h"
#include "network/http/HttpRequestParams.h"
#include "network/http/IHttpClient.h"
#include "network/sockets/bsd/SocketBsdCompat.h"
#include "threading/BlockingQueue.h"
#include "threading/ThreadPool.h"

namespace huesdk_lib {

    class CurlHttpClient : public IHttpClient {
    public:
        CurlHttpClient();

        ~CurlHttpClient();

        Handle start_request(const HttpRequestParams &data, HttpRequestCallback callback) override;

        void stop_request(Handle handle) override;

    private:
        /* share handle that shares the tls session information for all http requests */
        CURLSH* _curlsh;

        /* multi handle that shares the connection pool for all http requests */
        CURLM* _curlm;

        /* long living thread that performs all http requests */
        std::thread _thread;

        struct Message {
            enum MessageType {
                NONE,
                START_REQUEST,
                STOP_REQUEST,
                STOP_EXECUTOR
            };

            MessageType type;
            CurlRequest *request;

            explicit Message(MessageType _type = NONE, CurlRequest* _request = nullptr);
        };

        /* communication pipe between requesters and the background thread */
        BlockingQueue<Message> _queue;

        /* fds used to force curl_multi_wait to return immediately */
        SOCKET_ID _signal_fds[2];

        /* wake up the executor by sending data through the socket pair */
        void wake_up_executor();

        /* thread method that performs all http requests */
        void thread_method();
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_CURL_CURLHTTPCLIENT_H_
