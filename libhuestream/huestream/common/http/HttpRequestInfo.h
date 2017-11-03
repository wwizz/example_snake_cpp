/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef HUESTREAM_COMMON_HTTP_HTTPREQUESTINFO_H_
#define HUESTREAM_COMMON_HTTP_HTTPREQUESTINFO_H_

#include <huestream/common/serialize/SerializerHelper.h>

#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace huestream {

#define HTTP_REQUEST_PUT "PUT"
#define HTTP_REQUEST_POST "POST"
#define HTTP_REQUEST_GET "GET"

    class HttpRequestInfo {
    public:
        HttpRequestInfo();

        HttpRequestInfo(std::string method, std::string url, std::string body = "");

        virtual ~HttpRequestInfo() = default;

        virtual void FinishRequest();

        virtual void WaitUntilReady();

        virtual bool IsReady();

    PROP_DEFINE(HttpRequestInfo, std::string, url, Url);
    PROP_DEFINE(HttpRequestInfo, std::string, method, Method);
    PROP_DEFINE(HttpRequestInfo, std::string, body, Body);
    PROP_DEFINE(HttpRequestInfo, bool, success, Success);
    PROP_DEFINE(HttpRequestInfo, std::string, response, Response);

    protected:
        std::mutex _mutex;
        std::condition_variable _condition;
        bool _isFinished;
    };

    typedef std::shared_ptr<HttpRequestInfo> HttpRequestPtr;

}  // namespace huestream

#endif  // HUESTREAM_COMMON_HTTP_HTTPREQUESTINFO_H_
