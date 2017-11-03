/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
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
            _isFinished(false) {
    }

    PROP_IMPL(HttpRequestInfo, std::string, url, Url);
    PROP_IMPL(HttpRequestInfo, std::string, method, Method);
    PROP_IMPL(HttpRequestInfo, std::string, body, Body);
    PROP_IMPL(HttpRequestInfo, bool, success, Success);
    PROP_IMPL(HttpRequestInfo, std::string, response, Response)

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
    }

}  // namespace huestream
