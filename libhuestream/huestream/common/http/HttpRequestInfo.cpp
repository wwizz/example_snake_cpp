/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#include <huestream/common/http/HttpRequestInfo.h>

#include <string>

namespace huestream {

HttpRequestInfo::HttpRequestInfo() : HttpRequestInfo("", "", "") {
}

HttpRequestInfo::HttpRequestInfo(std::string method, std::string url, std::string body) :
    _url(url),
    _method(method),
    _body(body),
    _success(false),
    _response(""),
    _statusCode(0),
    _token(""),
    _roundTripTime(0),
    _isFinished(false),
    _enableSslVerification(true) {
}

PROP_IMPL(HttpRequestInfo, std::string, url, Url);
PROP_IMPL(HttpRequestInfo, std::string, method, Method);
PROP_IMPL(HttpRequestInfo, std::string, body, Body);
PROP_IMPL(HttpRequestInfo, bool, success, Success);
PROP_IMPL(HttpRequestInfo, std::string, response, Response)
PROP_IMPL(HttpRequestInfo, uint32_t, statusCode, StatusCode)
PROP_IMPL(HttpRequestInfo, std::string, token, Token);
PROP_IMPL(HttpRequestInfo, HttpRequestInfoCallback, callback, Callback);
PROP_IMPL(HttpRequestInfo, uint32_t, roundTripTime, RoundTripTime)
PROP_IMPL(HttpRequestInfo, bool, enableSslVerification, EnableSslVerification);

    void HttpRequestInfo::WaitUntilReady() {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition.wait(lock, [this]() -> bool { return _isFinished; });
        _isFinished = false;
    }

    bool HttpRequestInfo::IsReady() {
        bool result = false;
        std::unique_lock<std::mutex> lock(_mutex);
        result = _isFinished;
        return result;
    }

void HttpRequestInfo::FinishRequest() {
    std::unique_lock<std::mutex> lock(_mutex);
    _isFinished = true;
    _condition.notify_all();
    lock.unlock();
    if (_callback != nullptr) {
        _callback();
    }
}
void HttpRequestInfo::StartRequest() {
    std::unique_lock<std::mutex> lock(_mutex);
    _isFinished = false;
    lock.unlock();
}

}  // namespace huestream
