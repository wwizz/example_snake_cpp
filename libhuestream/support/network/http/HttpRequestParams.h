/*******************************************************************************
 Copyright (C) 2017 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#ifndef NETWORK_HTTP_HTTPREQUESTPARAMS_H_
#define NETWORK_HTTP_HTTPREQUESTPARAMS_H_

#include <string>
#include <unordered_map>

#include "network/http/HttpRequestBase.h"
#include "std/types/File.h"
#include "logging/Log.h"

namespace huesdk_lib {

    typedef std::unordered_map<std::string, std::string> HeaderFieldMap;

    struct HttpRequestParams {
        std::string              url;
        std::string              proxy_address;
        int                      proxy_port;
        int                      connect_timeout;
        int                      receive_timeout;
        int                      request_timeout;
        bool                     enable_logging;
        log::LogComponentType    log_component;
        HttpRequestSecurityLevel security_level;
        HeaderFieldMap           headers;
        std::string              method;
        std::string              body;
        File*                    file;
    };

}  // namespace huesdk_lib

#endif  // NETWORK_HTTP_HTTPREQUESTPARAMS_H_
