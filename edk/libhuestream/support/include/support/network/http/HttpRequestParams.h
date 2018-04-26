/*******************************************************************************
 Copyright (C) 2018 Philips Lighting Holding B.V.
 All Rights Reserved.
 ********************************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "support/network/http/HttpRequestBase.h"
#include "support/std/types/File.h"
#include "support/logging/Log.h"

namespace support {

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
        std::string              common_name;
        std::vector<std::string> trusted_certs;
        bool                     verify_ssl;
    };

}  // namespace support

