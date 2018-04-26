/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <curl/curl.h>

#include <mutex>
#include <thread>

#include "support/network/http/curl/CurlHttpClient.h"
#include "support/network/http/curl/CurlRequest.h"
#include "support/network/http/HttpRequestParams.h"

#define SHORT_WAIT_TIMEOUT_MS 100
#define LONG_WAIT_TIMEOUT_MS 10000

namespace support {
    /* curl doesnt know about threads, so we have to provide our own locking mechanism */

    static auto curl_mutex = std::make_shared<std::mutex>();

    static void share_lock(CURL*, curl_lock_data, curl_lock_access, void*) {
        curl_mutex->lock();
    }

    static void share_unlock(CURL*, curl_lock_data, curl_lock_access, void*) {
        curl_mutex->unlock();
    }

    CurlHttpClient::Message::Message(MessageType _type, CurlRequest* _request) :
            type(_type), request(_request) { }

    CurlHttpClient::CurlHttpClient() : _mutex{curl_mutex} {
        curl_global_init(CURL_GLOBAL_ALL);

        _curlsh = curl_share_init();
        curl_share_setopt(_curlsh, CURLSHOPT_LOCKFUNC, share_lock);
        curl_share_setopt(_curlsh, CURLSHOPT_UNLOCKFUNC, share_unlock);
        curl_share_setopt(_curlsh, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION);

        _curlm = curl_multi_init();
        curl_multi_setopt(_curlm, CURLMOPT_PIPELINING, CURLPIPE_HTTP1);

        _thread = std::thread(&CurlHttpClient::thread_method, this);

        _signal_fds[0] = INVALID_SOCKET;
        _signal_fds[1] = INVALID_SOCKET;

#ifndef _WIN32
        socketpair(PF_LOCAL, SOCK_STREAM, 0, _signal_fds);
#endif
    }

    CurlHttpClient::~CurlHttpClient() {
        _queue.push_back(Message(Message::STOP_EXECUTOR));
        wake_up_executor();
        _thread.join();

        curl_multi_cleanup(_curlm);
        curl_share_cleanup(_curlsh);

        for (auto fd : _signal_fds) {
            if (fd != INVALID_SOCKET) {
                closesocket(fd);
            }
        }
    }

    IHttpClient::Handle CurlHttpClient::start_request(const HttpRequestParams &data, HttpRequestCallback callback) {
        CurlRequest *request = new CurlRequest(data, callback);

        // make this request use the shared tls data
        curl_easy_setopt(request->get_handle(), CURLOPT_SHARE, _curlsh);

        // back reference from the curl handle to the request object
        curl_easy_setopt(request->get_handle(), CURLOPT_PRIVATE, request);

        _queue.push_back(Message(Message::START_REQUEST, request));
        wake_up_executor();

        return request;
    }

    void CurlHttpClient::stop_request(IHttpClient::Handle handle) {
        CurlRequest* request = static_cast<CurlRequest*>(handle);
        _queue.push_back(Message(Message::STOP_REQUEST, request));
        wake_up_executor();
        request->wait_for_completion();
        delete request;
    }

    void CurlHttpClient::wake_up_executor() {
        if (_signal_fds[0] != INVALID_SOCKET) {
            char tmp_buf[1] = {};
            send(_signal_fds[0], tmp_buf, 1, 0);
        }
    }

    void CurlHttpClient::thread_method() {
        int num_active = 0;

        while (true) {
            if (num_active == 0) {
                // block until a new request arrives
                _queue.wait_for_data();
            }

            while (true) {
                Message message = _queue.pop_front();

                if (message.type == Message::NONE) {
                    break;
                }

                if (message.type == Message::START_REQUEST) {
                    curl_multi_add_handle(_curlm, message.request->get_handle());
                }

                if (message.type == Message::STOP_REQUEST) {
                    // we can tell if this request is still active by checking if the curl handle still exists
                    CURL *handle = message.request->get_handle();
                    if (handle != nullptr) {
                        curl_multi_remove_handle(_curlm, handle);
                        message.request->send_response(CURLE_ABORTED_BY_CALLBACK);
                    }
                    message.request->set_as_complete();
                }

                if (message.type == Message::STOP_EXECUTOR) {
                    return;
                }
            }

            if (_signal_fds[1] == INVALID_SOCKET) {
                curl_multi_wait(_curlm, nullptr, 0, SHORT_WAIT_TIMEOUT_MS, nullptr);
            } else {
                struct curl_waitfd extra_fds[1];

                extra_fds[0].fd = _signal_fds[1];
                extra_fds[0].events = CURL_WAIT_POLLIN;
                extra_fds[0].revents = 0;

                curl_multi_wait(_curlm, extra_fds, 1, LONG_WAIT_TIMEOUT_MS, nullptr);
                if (extra_fds[0].revents) {
                    // receive any data sent to the signaling socket
                    char tmp_buf[8] = {};
                    recv(_signal_fds[1], tmp_buf, 8, 0);
                }
            }

            curl_multi_perform(_curlm, &num_active);

            int msgs_in_queue = 0;
            do {
                struct CURLMsg *msg = curl_multi_info_read(_curlm, &msgs_in_queue);

                if (msg != nullptr && msg->msg == CURLMSG_DONE) {
                    curl_multi_remove_handle(_curlm, msg->easy_handle);

                    CurlRequest *request = nullptr;
                    curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &request);
                    request->send_response(msg->data.result);
                }
            } while (msgs_in_queue > 0);
        }
    }

}  // namespace support
